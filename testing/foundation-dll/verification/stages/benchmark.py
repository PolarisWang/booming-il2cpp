"""Benchmark stage runner — native AOT/JIT + managed (.NET/Mono) performance.

Runs entry.exe --benchmark and entry-jit.exe --benchmark for all native methods.
Also runs managed benchmarks (net8-jit, net10-jit, mono) via managed harness generation.
Managed results are soft-fail: they contribute data to perf store but don't
gate the pipeline stage status.
"""

from __future__ import annotations

import json
import re
import shutil
import subprocess
import time
from pathlib import Path
from typing import Any

from verification.orchestration.context import FamilyContext, StageResult
from verification.analysis.perf_store import save_managed_benchmark_records

_MANAGED_TECHNOLOGIES = ["net8-jit", "net10-jit", "mono"]


# ── Managed benchmark technology definitions ─────────────────────────────

TECHNOLOGY_DEFS: dict[str, dict[str, Any]] = {
    "net8-jit":  {"runner": "dotnet-run", "tfm": "net8.0"},
    "net10-jit": {"runner": "dotnet-run", "tfm": "net10.0"},
    "mono":      {"runner": "mono",       "tfm": "net48"},
}


# ── SDK detection ───────────────────────────────────────────────────────

def _detect_frameworks() -> list[str]:
    """Return list of available TFMs: ['net8.0', 'net9.0', 'net10.0']."""
    try:
        r = subprocess.run(
            ["dotnet", "--list-sdks"],
            capture_output=True, text=True, timeout=15,
        )
        versions = []
        for line in r.stdout.strip().splitlines():
            m = re.match(r'(\d+\.\d+)\.', line.strip())
            if m:
                v = m.group(1)
                versions.append(f"net{v}")
        return sorted(set(versions))
    except Exception:
        return []


def _mono_path() -> str | None:
    """Find the mono executable path.

    Checks PATH first, then falls back to standard Windows install locations.
    """
    path = shutil.which("mono")
    if path:
        return path
    for candidate in (
        r"C:\Program Files\Mono\bin\mono.exe",
        r"C:\Program Files (x86)\Mono\bin\mono.exe",
    ):
        if Path(candidate).exists():
            return candidate
    return None


def _mono_available() -> bool:
    return _mono_path() is not None


def detect_managed_runtimes(
    requested: list[str] | None = None,
) -> tuple[list[str], list[str]]:
    """Return (available, unavailable) runtime aliases."""
    if requested is None:
        requested = list(TECHNOLOGY_DEFS.keys())

    frameworks = _detect_frameworks()
    mono_ok = _mono_available()

    available: list[str] = []
    unavailable: list[str] = []
    for alias in requested:
        if alias not in TECHNOLOGY_DEFS:
            unavailable.append(alias)
            continue
        spec = TECHNOLOGY_DEFS[alias]
        if spec["runner"] == "dotnet-run":
            if spec["tfm"] in frameworks:
                available.append(alias)
            else:
                unavailable.append(alias)
        elif spec["runner"] == "mono":
            if mono_ok:
                available.append(alias)
            else:
                unavailable.append(alias)
        else:
            unavailable.append(alias)

    return available, unavailable


# ── ToXxx(string) literal table ────────────────────────────────────────

_TOXXX_STRING_LITERALS: dict[str, Any] = {
    'Boolean': lambda ipart: f'Convert.ToBoolean(({ipart} % 2 == 0) ? "true" : "false")',
    'Byte':    lambda _:     'Convert.ToByte("123")',
    'Char':    lambda _:     'Convert.ToChar("A")',
    'Int16':   lambda _:     'Convert.ToInt16("12345")',
    'Int32':   lambda _:     'Convert.ToInt32("1234567")',
    'Int64':   lambda _:     'Convert.ToInt64("12345678901")',
    'Single':  lambda _:     'Convert.ToSingle("3.14")',
    'Double':  lambda _:     'Convert.ToDouble("3.14159")',
    'Decimal': lambda _:     'Convert.ToDecimal("123.45")',
}

_TOCHAR_ALWAYS_THROWS = {"System.Boolean", "System.DateTime", "System.Decimal",
                          "System.Double", "System.Single", "System.Object"}


# ── Managed harness generation ──────────────────────────────────────────

def _extract_param_types(mid: str) -> list[str]:
    """Extract parameter type list from a methodSubjectId."""
    m = re.search(r'\(([^)]+)\)', mid)
    if not m:
        return []
    raw = m.group(1)
    parts = []
    depth = 0
    cur: list[str] = []
    for ch in raw:
        if ch == ',' and depth == 0:
            parts.append(''.join(cur).strip())
            cur = []
        else:
            if ch == '<':
                depth += 1
            elif ch == '>':
                depth -= 1
            cur.append(ch)
    if cur:
        parts.append(''.join(cur).strip())
    return parts


def _return_type_from_mid(mid: str) -> str:
    """Extract return type from a methodSubjectId."""
    m = re.match(r'[^/]+/[^:]+::[^:]+:([^(]+)\(', mid)
    return m.group(1).strip() if m else ''


def _param_type_to_expr(pt: str, i_val: str, idx: int) -> str | None:
    """Map a parameter type string to a C# expression for benchmark invocation.

    Returns the expression string, or None if the type cannot be handled
    (falls through to the reflection runner).

    This is the core of the generic benchmark harness — it eliminates the
    need for per-method pattern matching by deriving call expressions from
    type signatures alone.
    """
    # Primitives and simple value types
    tbl = {
        'System.Int32':    i_val,
        'System.Int64':    f'({i_val} & 0xFF)',
        'System.UInt32':   f'(uint)({i_val} & 0xFF)',
        'System.UInt64':   f'(ulong)({i_val} & 0xFF)',
        'System.Int16':    f'(short)({i_val} & 0xFF)',
        'System.UInt16':   f'(ushort)({i_val} & 0xFF)',
        'System.Byte':     f'(byte)({i_val} & 0xFF)',
        'System.SByte':    f'(sbyte)({i_val} & 0xFF)',
        'System.Char':     f'(char)({i_val} & 0xFF)',
        'System.Boolean':  f'({i_val} % 2 == 0)',
        'System.Double':   f'(double)({i_val} & 0xFF)',
        'System.Single':   f'(float)({i_val} & 0xFF)',
        'System.Decimal':  f'(decimal)({i_val} & 0xFF)',
        'System.IntPtr':   f'(System.IntPtr)({i_val} & 0xFF)',
        'System.UIntPtr':  f'(System.UIntPtr)({i_val} & 0xFF)',
    }
    if pt in tbl:
        return tbl[pt]

    # String
    if pt == 'System.String':
        return f'(({i_val} & 1) == 0 ? "A" : "B")'

    # Type
    if pt == 'System.Type':
        return 'typeof(int)'

    # Object
    if pt == 'System.Object':
        return f'(object)({i_val} & 0xFF)'

    # Array types — create small non-null arrays
    if pt.endswith('[]'):
        elem_type = pt[:-2]
        if elem_type == 'System.Byte':
            return 'new byte[]{1,2,3,4,5}'
        if elem_type == 'System.Int32':
            return 'new int[]{1,2,3}'
        if elem_type == 'System.String':
            return 'new string[]{"A","B"}'
        if elem_type == 'System.Object':
            return 'new object[]{1,"B"}'
        if elem_type == 'System.Type':
            return 'new Type[]{typeof(int)}'
        # Generic array fallback
        return 'new byte[]{1,2,3,4,5}'

    # Enum types — use default value cast
    if pt.startswith('System.') and pt.count('.') <= 2:
        # Common enums
        enum_defaults = {
            'System.Reflection.BindingFlags': 'System.Reflection.BindingFlags.Default',
            'System.StringComparison': 'System.StringComparison.Ordinal',
            'System.StringSplitOptions': 'System.StringSplitOptions.None',
            'System.Threading.Timeout': 'System.Threading.Timeout.Infinite',
            'System.Threading.Tasks.TaskStatus': 'System.Threading.Tasks.TaskStatus.RanToCompletion',
            'System.Threading.Tasks.TaskCreationOptions': 'System.Threading.Tasks.TaskCreationOptions.None',
            'System.Threading.Tasks.TaskContinuationOptions': 'System.Threading.Tasks.TaskContinuationOptions.None',
            'System.Threading.ThreadPriority': 'System.Threading.ThreadPriority.Normal',
            'System.IO.FileMode': 'System.IO.FileMode.Open',
            'System.IO.FileAccess': 'System.IO.FileAccess.Read',
            'System.IO.FileShare': 'System.IO.FileShare.Read',
            'System.IO.SeekOrigin': 'System.IO.SeekOrigin.Begin',
            'System.IO.FileOptions': 'System.IO.FileOptions.None',
            'System.IO.PathFormat': 'System.IO.PathFormat.FullPath',
        }
        if pt in enum_defaults:
            return enum_defaults[pt]

    # Generic types like IComparer<T>, IEnumerable<T>
    # For benchmark purposes, use null (safe for reference types)
    if pt.startswith('System.Collections'):
        return 'null'

    # Special well-known types
    if pt == 'System.Array':
        return 'new byte[]{1,2,3,4,5}'
    if pt == 'System.Delegate':
        return 'null'
    if pt == 'System.IFormatProvider':
        return 'null'
    if pt == 'System.IAsyncResult':
        return 'null'
    if pt == 'System.Range':
        return 'new System.Range(0, 3)'
    if pt == 'System.TimeSpan':
        return 'System.TimeSpan.Zero'
    if pt == 'System.DateTime':
        return 'System.DateTime.UtcNow'
    if pt == 'System.Guid':
        return 'System.Guid.Empty'
    if pt == 'System.Version':
        return 'new System.Version(1, 0)'
    if pt == 'System.RuntimeTypeHandle':
        return 'typeof(int).TypeHandle'
    if pt == 'System.RuntimeMethodHandle':
        return 'typeof(int).GetMethods()[0].MethodHandle'
    if pt == 'System.RuntimeFieldHandle':
        return 'typeof(int).GetFields()[0].FieldHandle'
    if pt == 'System.Reflection.MethodInfo':
        return 'typeof(int).GetMethods()[0]'
    if pt == 'System.Reflection.FieldInfo':
        return 'typeof(int).GetFields()[0]'
    if pt == 'System.Reflection.PropertyInfo':
        return 'typeof(int).GetProperties()[0]'
    if pt == 'System.Reflection.EventInfo':
        return 'typeof(int).GetEvents()[0]'
    if pt == 'System.Reflection.ConstructorInfo':
        return 'typeof(int).GetConstructors()[0]'
    if pt == 'System.Reflection.MemberInfo':
        return 'typeof(int).GetMethods()[0]'
    if pt == 'System.Reflection.ParameterInfo':
        return 'typeof(int).GetMethods()[0].GetParameters()[0]'
    if pt == 'System.Reflection.Module':
        return 'typeof(int).Module'
    if pt == 'System.Reflection.Assembly':
        return 'typeof(int).Assembly'
    if pt == 'System.Reflection.IReflect':
        return 'null'
    if pt == 'System.Reflection.Binder':
        return 'null'
    if pt == 'System.Globalization.CultureInfo':
        return 'System.Globalization.CultureInfo.InvariantCulture'
    if pt == 'System.Globalization.NumberStyles':
        return 'System.Globalization.NumberStyles.Any'
    if pt == 'System.Globalization.DateTimeStyles':
        return 'System.Globalization.DateTimeStyles.None'
    if pt == 'System.Runtime.Serialization.StreamingContext':
        return 'new System.Runtime.Serialization.StreamingContext()'
    if pt == 'System.Runtime.Serialization.SerializationInfo':
        return 'null'

    # For reference types, null is usually safe for benchmark purposes
    # (the method may throw, but that's caught by the harness)
    return 'null'


def _is_static_declaring_type(dt: str) -> bool:
    """Heuristic: types with only static utility methods (no instance state)."""
    static_types = {
        'System.Convert', 'System.Buffer', 'System.Array',
        'System.Guid', 'System.Runtime.InteropServices.Marshal',
        'System.Runtime.CompilerServices.RuntimeHelpers',
        'System.Threading.Interlocked', 'System.Threading.Monitor',
        'System.Activator',
    }
    return dt in static_types or dt.startswith('System.Reflection.CustomAttribute')


# Static methods on types that also have instance methods.
# The code generator normally uses an instance receiver (e.g. typeof(int) for Type),
# but these specific methods must be called with the type name directly.
_KNOWN_STATIC_METHODS: dict[str, set[str]] = {
    'System.Type': {
        'GetType',
        'GetTypeFromHandle',
        'GetTypeCode',
    },
    'System.Reflection.Assembly': {
        'GetExecutingAssembly',
        'GetCallingAssembly',
        'GetEntryAssembly',
    },
    'System.Threading.Thread': {
        'ResetAbort',
        'Yield',
    },
}


def _generate_generic_call(mid: str, idx: int) -> tuple[str, bool]:
    """Generate a C# call expression from methodSubjectId without pattern matching.

    Uses _param_type_to_expr() to build argument expressions from parameter types.
    This eliminates the need for per-method pattern matching in the benchmark harness.

    Returns (call_expression_or_empty, always_throws_bool).
    Returns ('', False) when the method signature is too complex for expression generation,
    in which case the harness falls back to the reflection runner.
    """
    m = re.match(r'[^/]+/([^:]+)::([^:]+):([^(]+)\(([^)]*)\)', mid)
    if not m:
        return '', False
    declaring_type = m.group(1)
    method_name = m.group(2)
    return_type_str = m.group(3)
    param_str = m.group(4)
    param_types = [p.strip() for p in param_str.split(',') if p.strip()]

    i_expr = f'(i + {idx})' if idx > 0 else 'i'

    # Check for pointer/ref parameters — requires unsafe code, falls through to reflection
    if any('*' in p or '&' in p for p in param_types):
        return '', False

    # Detect instance methods: if declaring_type is not a known static-only type
    # and method is not a constructor, we may need an instance receiver.
    if declaring_type == 'System.Object' and method_name in ('MemberwiseClone', 'GetObjectValue'):
        # Special cases that take/return object
        if method_name == 'MemberwiseClone':
            # MemberwiseClone is protected — cannot call via qualifier; fall through to reflection
            return '', False
        if method_name == 'GetObjectValue':
            return f'System.Runtime.CompilerServices.RuntimeHelpers.GetObjectValue({i_expr})', False

    is_string_ctor = declaring_type == 'System.String' and method_name == '.ctor'
    is_instance = not _is_static_declaring_type(declaring_type) and not is_string_ctor

    # Build receiver expression
    if is_string_ctor:
        # string.ctor — use string.Create or direct constructor via typeof
        # These are tricky; fall through to reflection
        return '', False
    elif is_instance:
        # For instance methods on types like System.RuntimeType, System.Reflection.*Info
        # we need an instance. Use typeof(T) for Type, or Activator for others.
        if declaring_type == 'System.RuntimeType':
            receiver = 'typeof(int)'
        elif declaring_type == 'System.Reflection.MethodInfo':
            receiver = 'typeof(int).GetMethods()[0]'
        elif declaring_type == 'System.Reflection.FieldInfo':
            receiver = 'typeof(int).GetFields()[0]'
        elif declaring_type == 'System.Reflection.PropertyInfo':
            # typeof(int) has no properties — use typeof(string) which has Length
            receiver = 'typeof(string).GetProperties()[0]'
        elif declaring_type == 'System.Reflection.EventInfo':
            # typeof(int) has no events — use typeof(System.AppDomain) which has events
            receiver = 'typeof(System.AppDomain).GetEvents()[0]'
        elif declaring_type == 'System.Reflection.ConstructorInfo':
            # typeof(int) has no explicit constructors — use typeof(Exception)
            receiver = 'typeof(System.Exception).GetConstructors()[0]'
        elif declaring_type == 'System.Reflection.ParameterInfo':
            receiver = 'typeof(int).GetMethods()[0].GetParameters()[0]'
        elif declaring_type == 'System.Reflection.Module':
            receiver = 'typeof(int).Module'
        elif declaring_type == 'System.Reflection.Assembly':
            receiver = 'typeof(int).Assembly'
        elif declaring_type == 'System.Reflection.MemberInfo':
            receiver = 'typeof(int).GetMethods()[0]'
        elif declaring_type == 'System.Threading.CancellationToken':
            receiver = 'default(System.Threading.CancellationToken)'
        elif declaring_type == 'System.Array':
            receiver = 'new byte[]{1,2,3}'
        elif declaring_type == 'System.Exception':
            receiver = 'new System.Exception()'
        elif declaring_type == 'System.ArgumentException':
            receiver = 'new System.ArgumentException()'
        elif declaring_type == 'System.Type':
            receiver = 'typeof(int)'
        elif declaring_type == 'System.Threading.Thread':
            receiver = 'System.Threading.Thread.CurrentThread'
        else:
            # Reference type with default constructor? Try Activator.
            # For complex types, fall through to reflection
            return '', False
    else:
        receiver = declaring_type

    # Build argument expressions
    args = []
    for pt in param_types:
        arg = _param_type_to_expr(pt, i_expr, idx)
        if arg is None:
            return '', False
        args.append(arg)

    # Override receiver for static methods on types that also have instance methods.
    # e.g. Type.GetType(string) must be called as Type.GetType(...) not typeof(int).GetType(...)
    static_methods = _KNOWN_STATIC_METHODS.get(declaring_type, set())
    if method_name in static_methods:
        receiver = declaring_type

    # Known property names that appear WITHOUT the get_ prefix in subject IDs.
    # When detected, produce property access (no parens) instead of method call.
    _PROPERTY_ALIASES: set[tuple[str, str]] = {
        ('System.Type', 'ContainsGenericParameters'),
    }

    # Handle return type
    if method_name.startswith('get_') and len(param_types) == 0:
        # Property getter — discard with _ =, no () since properties are not methods.
        call = f'_ = {receiver}.{method_name[4:]}'
    elif method_name.startswith('set_') and len(param_types) == 1:
        # Property setter — assignment syntax, not method call. No _ = since it's void.
        call = f'{receiver}.{method_name[4:]} = {args[0]}'
    elif (declaring_type, method_name) in _PROPERTY_ALIASES:
        # Property without get_ prefix in subject ID — access without ()
        call = f'_ = {receiver}.{method_name}'
    elif method_name == '.ctor':
        # Constructor
        call = f'new {declaring_type}({", ".join(args)})'
    elif method_name.startswith('op_'):
        # Operator overload — these are complex, fall through
        return '', False
    elif re.match(r'^[A-Z]$', return_type_str):
        # Generic return type (T, U, etc.) — C# cannot infer type arguments
        # from usage alone for methods like Activator.CreateInstance<T>().
        # Fall through to reflection runner which resolves at runtime.
        return '', False
    elif return_type_str == 'System.Void':
        call = f'{receiver}.{method_name}({", ".join(args)})'
    else:
        call = f'{receiver}.{method_name}({", ".join(args)})'

    # Methods known to always throw with auto-generated arguments should be
    # wrapped in try/catch so the exception is captured as a benchmark event
    # rather than crashing the harness.
    throwing_by_sig = (
        # Activator.CreateInstance(typeof(int), args...) — int has no
        # parameterized constructor, so these calls always throw.
        (declaring_type == 'System.Activator'
         and method_name == 'CreateInstance'
         and len(param_types) >= 1
         and param_types[0] == 'System.Type'
         and len(param_types) > 1)
        or
        # ConstructorInfo.Invoke(args) — auto-generated args rarely match
        # the actual constructor signature (e.g. int expects 1 arg but
        # auto-generated args may pass 2+).
        (declaring_type == 'System.Reflection.ConstructorInfo'
         and method_name == 'Invoke')
        or
        # Array.CreateInstance(System.Type, args...) with auto-generated
        # dimension args: lengths may be 0 causing zero-length array which
        # is valid, but mismatched dimension counts always throw.
        (declaring_type == 'System.Array'
         and method_name == 'CreateInstance'
         and param_types[0] == 'System.Type'
         and len(param_types) > 1
         and any('[]' not in p for p in param_types[1:]))
        or
        # Assembly.GetType(string, ...) — auto-generated type names like "A"/"B"
        # are never valid, causing TypeLoadException at runtime.
        (declaring_type == 'System.Reflection.Assembly'
         and method_name == 'GetType')
        or
        # Type.GetType(string, ...) — same issue: "A"/"B" never resolve to a
        # real type, and .NET 10 throws TypeLoadException (not null) on failure.
        (declaring_type == 'System.Type'
         and method_name in ('GetType', 'GetTypeFromHandle', 'get_GenericParameterPosition'))
        or
        # Type.GetGenericTypeDefinition() — typeof(int) is not a generic type
        (declaring_type == 'System.Type'
         and method_name == 'GetGenericTypeDefinition')
        or
        # RuntimeHelpers.InitializeArray — FieldHandle via GetFields()[0] may
        # throw NotSupportedException (MdFieldInfo doesn't support FieldHandle).
        (declaring_type == 'System.Runtime.CompilerServices.RuntimeHelpers'
         and method_name == 'InitializeArray')
        or
        # Thread.Abort() — throws PlatformNotSupportedException on this platform
        (declaring_type == 'System.Threading.Thread'
         and method_name == 'Abort')
        or
        # FieldInfo.SetValue / SetValueDirect — auto-generated args on literal
        # fields (e.g. Int32.MaxValue) throw FieldAccessException.
        (declaring_type in ('System.Reflection.FieldInfo', 'System.Reflection.MemberInfo')
         and method_name in ('SetValue', 'SetValueDirect'))
        or
        # MemberInfo.IsDefined/GetCustomAttribute — auto-generated Type arg
        # (typeof(int)) is not derived from System.Attribute.
        (declaring_type in ('System.Reflection.MemberInfo', 'System.Reflection.Module')
         and method_name in ('IsDefined', 'GetCustomAttribute', 'GetCustomAttributes'))
        or
        # Type.MakeArrayType(Int32) — auto-generated rank may exceed CLR limit
        # (e.g. i=36), throwing TypeLoadException.
        (declaring_type == 'System.Type'
         and method_name in ('MakeArrayType', 'MakeByRefType', 'MakePointerType'))
        or
        # Type.MakeGenericType(Type[]) — typeof(int) is not a generic type definition
        (declaring_type == 'System.Type'
         and method_name == 'MakeGenericType')
        or
        # Type.GetEnumUnderlyingType() — typeof(int) is not an enum type
        (declaring_type == 'System.Type'
         and method_name == 'GetEnumUnderlyingType')
        or
        # Thread.ResetAbort() — throws PlatformNotSupportedException on this platform
        (declaring_type == 'System.Threading.Thread'
         and method_name == 'ResetAbort')
        or
        # CancellationToken.Register(Action) — auto-generated null callback throws
        (declaring_type == 'System.Threading.CancellationToken'
         and method_name == 'Register')
        or
        # Monitor.Exit/Monitor.TryEnter — auto-generated null obj causes
        # SynchronizationLockException or ArgumentNullException
        (declaring_type == 'System.Threading.Monitor'
         and method_name in ('Exit', 'TryEnter', 'Enter', 'Pulse', 'Wait', 'PulseAll'))
        or
        # PropertyInfo.GetValue/SetValue — auto-generated instance arg type
        # (typeof(int)) doesn't match the property's declaring type (typeof(string))
        (declaring_type == 'System.Reflection.PropertyInfo'
         and method_name in ('GetValue', 'SetValue', 'GetConstantValue', 'GetRawConstantValue'))
        or
        # FieldInfo.GetValue/SetValue — same type mismatch issue
        (declaring_type == 'System.Reflection.FieldInfo'
         and method_name in ('GetValue'))
        or
        # MethodBase.Invoke — auto-generated instance/args rarely match
        (declaring_type in ('System.Reflection.MethodBase', 'System.Reflection.MethodInfo')
         and method_name == 'Invoke')
        or
        # Type.GetGenericParameterConstraints() — typeof(int) is not generic
        (declaring_type == 'System.Type'
         and method_name == 'GetGenericParameterConstraints')
        or
        # Thread.Sleep(Int32) — may get ThreadInterruptedException from
        # concurrent Thread.Interrupt() within the same benchmark family
        (declaring_type == 'System.Threading.Thread'
         and method_name == 'Sleep')
        or
        # FieldInfo.get_FieldHandle — throws NotSupportedException on
        # some field implementations (MdFieldInfo)
        (declaring_type == 'System.Reflection.FieldInfo'
         and method_name == 'get_FieldHandle')
        or
        # Array.Sort(Array, int, int, ...) — auto-generated index/length params
        # exceed the bounds of the fixed-size array argument.
        (declaring_type == 'System.Array'
         and method_name == 'Sort'
         and len(param_types) >= 3)
    )

    return call, throwing_by_sig


def _generate_reflection_call(mid: str) -> tuple[str, bool]:
    """Generate a reflection-based call expression for complex method signatures.

    Falls back to C# _ReflectionRunner which resolves types/methods at runtime
    using System.Reflection. This handles ANY method signature without needing
    per-method code generation patterns.

    Returns (call_expression, always_throws_bool). The call expression is a void
    invocation of _ReflectionRunner.Invoke which handles caching and parameter
    creation internally.
    """
    # Reflection call: the _ReflectionRunner catches invocation errors internally,
    # but ParseEntry (method resolution on first call) CAN throw for methods that
    # are not publicly accessible (e.g., protected members like MemberwiseClone).
    # Mark always_throws=True so the generated harness wraps in try/catch,
    # preventing a single unresolvable method from crashing the entire benchmark.
    return f'_ReflectionRunner.Invoke("{mid}", i)', True


def _fallback_call_expr(mid: str, idx: int) -> tuple[str, bool]:
    """Three-tier fallback when pattern matching cannot generate a call expression."""
    call_expr, always_throws = _generate_generic_call(mid, idx)
    if call_expr:
        return call_expr, always_throws
    # _generate_reflection_call already returns (call_expr, always_throws) tuple.
    return _generate_reflection_call(mid)


def _generate_call_expr(mid: str, idx: int) -> tuple[str, bool]:
    """Generate a C# call expression for benchmarking by parsing methodSubjectId.

    Three-tier strategy:
    1. Try explicit pattern matching (for known methods with specific arguments)
    2. Try generic type-based expression generation (for any well-typed method)
    3. Fall back to reflection-based invocation (for complex edge cases)

    Returns (call_expression, always_throws_bool).
    """
    ipart = f'(i + {idx})' if idx > 0 else 'i'

    m = re.match(r'[^/]+/([^:]+)::([^:]+):[^(]+\(([^)]*)\)', mid)
    if not m:
        return _generate_reflection_call(mid)
    declaring_type = m.group(1)
    method_name = m.group(2)
    param_str = m.group(3)
    param_types = [p.strip() for p in param_str.split(',') if p.strip()]

    # Xxx.Parse(string)
    if method_name == 'Parse' and param_types == ['System.String']:
        parse_tbl = {
            'System.Double': 'Double.Parse("3.14159")',
            'System.Int32': f'Int32.Parse((({ipart}) % 100000 + 1).ToString())',
            'System.Int64': f'Int64.Parse((({ipart}) % 100000 + 1).ToString())',
        }
        if declaring_type in parse_tbl:
            return parse_tbl[declaring_type], False
        # Not a handled Parse type — fall through to later sections

    # Convert.ToString(xxx)
    if declaring_type == 'System.Convert' and method_name == 'ToString' and len(param_types) == 1:
        t = param_types[0]
        if 'Int32' in t:
            return f'Convert.ToString({ipart})', False
        elif 'Int64' in t:
            return f'Convert.ToString((long)({ipart} & 0xFF))', False
        elif 'Double' in t:
            return f'Convert.ToString((double)({ipart} & 0xFF))', False
        elif 'Single' in t:
            return f'Convert.ToString((float)({ipart} & 0xFF))', False
        return _fallback_call_expr(mid, idx)

    # Convert.ToXxx(string)
    if declaring_type == 'System.Convert' and method_name.startswith('To') and param_types == ['System.String']:
        rt = method_name[2:]
        if rt in _TOXXX_STRING_LITERALS:
            return _TOXXX_STRING_LITERALS[rt](ipart), False
        return _fallback_call_expr(mid, idx)

    # Convert.ToXxx(non-string, 1 param)
    if declaring_type == 'System.Convert' and method_name.startswith('To') and len(param_types) == 1:
        t = param_types[0]
        type_to_cast = {
            'System.Byte':    f'(byte)({ipart} & 0xFF)',
            'System.SByte':   f'(sbyte)({ipart} & 0x7F)',
            'System.Char':    f'(char)({ipart} & 0xFF)',
            'System.Int16':   f'(short)({ipart} & 0xFF)',
            'System.Int32':   f'({ipart} & 0xFF)',
            'System.Int64':   f'({ipart} & 0xFF)',
            'System.UInt16':  f'(ushort)({ipart} & 0xFF)',
            'System.UInt32':  f'(uint)({ipart} & 0xFF)',
            'System.UInt64':  f'(ulong)({ipart} & 0xFF)',
            'System.Double':  f'(double)({ipart} & 0xFF)',
            'System.Single':  f'(float)({ipart} & 0xFF)',
            'System.Decimal': f'(decimal)({ipart} & 0xFF)',
            'System.Boolean': f'({ipart} % 2 == 0)',
            'System.Object':  f'(object)({ipart} & 0xFF)',
            'System.DateTime': 'System.DateTime.Now',
        }
        if t in type_to_cast:
            always_throws = (method_name == 'ToChar' and t in _TOCHAR_ALWAYS_THROWS)
            return f'Convert.{method_name}({type_to_cast[t]})', always_throws
        if t == 'System.String':
            return f'Convert.{method_name}((({ipart} & 1) == 0) ? "A" : "B")', False
        return _fallback_call_expr(mid, idx)

    # Generic collection types (List<T>, Dictionary<K,V>, HashSet<T>)
    coll_match = re.match(r'System\.Collections\.Generic\.(\w+)`\d+', declaring_type)
    if coll_match:
        coll_type = coll_match.group(1)
        if coll_type == 'List':
            tbl = {
                'Add': f'new System.Collections.Generic.List<int>().Add({ipart})',
                'Clear': f'new System.Collections.Generic.List<int>{{{ipart}}}.Clear()',
                'Contains': f'new System.Collections.Generic.List<int>{{{ipart}}}.Contains({ipart})',
                'IndexOf': f'new System.Collections.Generic.List<int>{{{ipart}}}.IndexOf({ipart})',
                'Remove': f'new System.Collections.Generic.List<int>{{{ipart}}}.Remove({ipart})',
                'RemoveAt': f'new System.Collections.Generic.List<int>{{{ipart}}}.RemoveAt(0)',
                'Sort': 'new System.Collections.Generic.List<int>{3, 1, 2}.Sort()',
                'ToArray': f'new System.Collections.Generic.List<int>{{{ipart}}}.ToArray()',
            }
            if method_name in tbl:
                return tbl[method_name], False
        elif coll_type == 'Dictionary':
            tbl = {
                'Add': f'new System.Collections.Generic.Dictionary<int, int>().Add({ipart}, {ipart})',
                'get_Count': f'_ = new System.Collections.Generic.Dictionary<int, int>{{{{{ipart}, {ipart}}}}}.Count',
                'TryGetValue': f'new System.Collections.Generic.Dictionary<int, int>{{{{{ipart}, {ipart}}}}}.TryGetValue({ipart}, out _)',
                'ContainsKey': f'new System.Collections.Generic.Dictionary<int, int>{{{{{ipart}, {ipart}}}}}.ContainsKey({ipart})',
                'Remove': f'new System.Collections.Generic.Dictionary<int, int>{{{{{ipart}, {ipart}}}}}.Remove({ipart})',
            }
            if method_name in tbl:
                return tbl[method_name], False
        elif coll_type == 'HashSet':
            tbl = {
                'Add': f'new System.Collections.Generic.HashSet<int>().Add({ipart})',
                'Contains': f'new System.Collections.Generic.HashSet<int>{{{ipart}}}.Contains({ipart})',
                'Remove': f'new System.Collections.Generic.HashSet<int>{{{ipart}}}.Remove({ipart})',
            }
            if method_name in tbl:
                return tbl[method_name], False
        return _fallback_call_expr(mid, idx)

    # System.Array
    if declaring_type == 'System.Array':
        tbl = {
            'Copy': {
                3: 'System.Array.Copy(new byte[]{1,2,3,4,5}, new byte[5], 3)',
                5: 'System.Array.Copy(new byte[]{1,2,3,4,5}, 1, new byte[3], 0, 3)',
            },
            'Clear': 'System.Array.Clear(new byte[]{1,2,3,4,5}, 0, 3)',
            'Sort': {
                1: 'System.Array.Sort(new byte[]{3,1,4,1,5})',
                2: 'System.Array.Sort(new byte[]{3,1,2}, (System.Collections.IComparer)null)',
            },
            'BinarySearch': {
                2: 'System.Array.BinarySearch(new byte[]{10,20,30,40}, (object)(byte)30)',
                4: 'System.Array.BinarySearch(new byte[]{10,20,30,40}, 0, 3, (object)(byte)20)',
            },
            'IndexOf': 'System.Array.IndexOf(new byte[]{5,3,5,3}, (object)(byte)3)',
            'LastIndexOf': 'System.Array.LastIndexOf(new byte[]{5,3,5,3}, (object)(byte)3)',
            'Reverse': 'System.Array.Reverse(new byte[]{1,2,3,4,5})',
            'GetLength': 'System.Array.CreateInstance(typeof(byte), 3).GetLength(0)',
            'GetValue': 'new byte[]{10,20,30}.GetValue(0)',
            'CreateInstance': {
                # len(param_types) includes System.Type arg + dimension args.
                # CreateInstance(Type, Int32) → 2 params → fixed single dim
                2: 'System.Array.CreateInstance(typeof(int), 3)',
                # CreateInstance(Type, Int32, Int32) → 3 params → fixed 2D dims
                3: 'System.Array.CreateInstance(typeof(int), 3, 2)',
            },
        }
        if method_name in tbl:
            entry = tbl[method_name]
            if isinstance(entry, dict):
                if len(param_types) in entry:
                    return entry[len(param_types)], False
            else:
                return entry, False
        return _fallback_call_expr(mid, idx)

    # System.Buffer
    if declaring_type == 'System.Buffer':
        safe_tbl = {
            'BlockCopy': 'Buffer.BlockCopy(new byte[]{1,2,3,4,5}, 0, new byte[5], 0, 5)',
            'ByteLength': 'Buffer.ByteLength(new byte[10])',
            'GetByte': 'Buffer.GetByte(new byte[]{10,20,30,40,50}, 2)',
            'SetByte': 'Buffer.SetByte(new byte[1], 0, (byte)42)',
        }
        if method_name in safe_tbl:
            return safe_tbl[method_name], False
        return _fallback_call_expr(mid, idx)

    # System.Guid
    if declaring_type == 'System.Guid':
        tbl = {
            'NewGuid': 'Guid.NewGuid()',
            'Parse': 'Guid.Parse("00000000-0000-0000-0000-000000000000")',
            'GetHashCode': 'Guid.NewGuid().GetHashCode()',
            'ToString': 'Guid.NewGuid().ToString()',
            '.ctor': 'new Guid("00000000-0000-0000-0000-000000000000")',
        }
        if method_name in tbl:
            return tbl[method_name], False
        return _fallback_call_expr(mid, idx)

    # System.Random
    if declaring_type == 'System.Random':
        tbl = {
            'Next': {
                0: 'new Random().Next()',
                1: f'new Random().Next({ipart})',
            },
            'NextBytes': 'new Random().NextBytes(new byte[16])',
            'NextDouble': 'new Random().NextDouble()',
        }
        if method_name == '.ctor':
            return 'new Random()', False
        if method_name in tbl:
            entry = tbl[method_name]
            if isinstance(entry, dict):
                if len(param_types) in entry:
                    return entry[len(param_types)], False
            else:
                return entry, False
        return _fallback_call_expr(mid, idx)

    # System.HashCode
    if declaring_type == 'System.HashCode':
        if method_name == 'ToHashCode':
            return 'default(HashCode).ToHashCode()', False
        if method_name.startswith('Combine') and len(param_types) == 2:
            return f'HashCode.Combine({ipart}, {ipart})', False
        return _fallback_call_expr(mid, idx)

    # System.Threading.Thread
    if declaring_type == 'System.Threading.Thread':
        tbl = {
            'get_CurrentThread': '_ = System.Threading.Thread.CurrentThread.GetHashCode()',
            'get_ManagedThreadId': '_ = System.Threading.Thread.CurrentThread.ManagedThreadId',
            'Sleep': ('System.Threading.Thread.Sleep(0)', True),
            'Start': 'new System.Threading.Thread(() => {}).Start()',
        }
        if method_name in tbl:
            entry = tbl[method_name]
            if isinstance(entry, tuple):
                return entry
            return entry, False
        return _fallback_call_expr(mid, idx)

    # System.Threading sync primitives — blocking methods (Wait, WaitOne)
    # must use timeout overloads to prevent hangs. Non-blocking methods
    # use direct C# patterns to avoid _ReflectionRunner overhead.
    if declaring_type in (
        'System.Threading.SemaphoreSlim',
        'System.Threading.ManualResetEvent',
        'System.Threading.AutoResetEvent',
    ):
        if method_name == '.ctor':
            if declaring_type == 'System.Threading.SemaphoreSlim':
                # Fixed initial count avoids 0-count hangs
                if len(param_types) == 1:
                    return 'new System.Threading.SemaphoreSlim(1)', False
                elif len(param_types) == 2:
                    return 'new System.Threading.SemaphoreSlim(1, 5)', False
        elif method_name in ('Wait', 'WaitOne'):
            if declaring_type == 'System.Threading.SemaphoreSlim':
                return 'new System.Threading.SemaphoreSlim(1).Wait(0)', False
            elif declaring_type == 'System.Threading.ManualResetEvent':
                return 'new System.Threading.ManualResetEvent(true).WaitOne(0)', False
            elif declaring_type == 'System.Threading.AutoResetEvent':
                return 'new System.Threading.AutoResetEvent(true).WaitOne(0)', False
        elif method_name == 'Release':
            return 'new System.Threading.SemaphoreSlim(1).Release()', False
        elif method_name == 'get_CurrentCount':
            return '_ = new System.Threading.SemaphoreSlim(1).CurrentCount', False
        elif method_name == 'Set':
            return 'new System.Threading.ManualResetEvent(true).Set()', False
        elif method_name == 'Reset':
            return 'new System.Threading.ManualResetEvent(true).Reset()', False

    # SpinLock and SpinWait are structs that _ReflectionRunner handles
    # poorly (boxing, ref params). Use explicit patterns instead.
    if declaring_type == 'System.Threading.SpinLock':
        if method_name == '.ctor':
            return 'new System.Threading.SpinLock(false)', False
        elif method_name == 'Exit':
            # SpinLock requires an instance; create one inline
            return 'new System.Threading.SpinLock(false).Exit()', False
    if declaring_type == 'System.Threading.SpinWait':
        if method_name == 'SpinOnce':
            return 'new System.Threading.SpinWait().SpinOnce()', False

    # System.Threading.Tasks.Task
    if declaring_type == 'System.Threading.Tasks.Task':
        tbl = {
            'get_IsCompleted': '_ = System.Threading.Tasks.Task.CompletedTask.IsCompleted',
            'get_Status': '_ = (int)System.Threading.Tasks.Task.CompletedTask.Status',
            'Run': {
                0: 'System.Threading.Tasks.Task.Run(() => { _g++; })',
                1: 'System.Threading.Tasks.Task.FromResult(42)',
            },
            'Delay': 'System.Threading.Tasks.Task.Delay(0).Wait()',
            'Wait': {
                0: 'System.Threading.Tasks.Task.FromResult(42).Wait()',
                2: 'System.Threading.Tasks.Task.FromResult(42).Wait(true, System.Threading.Timeout.Infinite)',
            },
            'ContinueWith': 'System.Threading.Tasks.Task.CompletedTask.ContinueWith(_ => { _g++; })',
            'WhenAll': 'System.Threading.Tasks.Task.WhenAll(System.Threading.Tasks.Task.CompletedTask)',
            'WhenAny': '_ = System.Threading.Tasks.Task.WhenAny(System.Threading.Tasks.Task.CompletedTask)',
            'FromResult': '_ = System.Threading.Tasks.Task.FromResult(42)',
        }
        if method_name in tbl:
            entry = tbl[method_name]
            if isinstance(entry, dict):
                if len(param_types) in entry:
                    return entry[len(param_types)], False
            else:
                return entry, False
        return _fallback_call_expr(mid, idx)

    # System.Enum
    if declaring_type == 'System.Enum':
        tbl = {
            'Format': 'System.Enum.Format(typeof(System.StringComparison), System.StringComparison.Ordinal, "G")',
            'GetName': 'System.Enum.GetName(typeof(System.StringComparison), System.StringComparison.Ordinal)',
            'GetNames': '_ = System.Enum.GetNames(typeof(System.StringComparison))',
            'GetValues': '_ = System.Enum.GetValues(typeof(System.StringComparison))',
            'IsDefined': 'System.Enum.IsDefined(typeof(System.StringComparison), System.StringComparison.Ordinal)',
            'ToString': {
                0: 'System.StringComparison.Ordinal.ToString()',
                1: 'System.StringComparison.Ordinal.ToString("G")',
            },
            'Parse': {
                2: 'System.Enum.Parse(typeof(System.StringComparison), "Ordinal")',
                3: 'System.Enum.Parse(typeof(System.StringComparison), "Ordinal", true)',
            },
            'TryParse': {
                3: 'System.Enum.TryParse(typeof(System.StringComparison), "Ordinal", true, out _)',
            },
        }
        if method_name in tbl:
            entry = tbl[method_name]
            if isinstance(entry, dict):
                if len(param_types) in entry:
                    return entry[len(param_types)], False
            else:
                return entry, False
        return _fallback_call_expr(mid, idx)

    # System.Runtime.CompilerServices.RuntimeHelpers
    if declaring_type == 'System.Runtime.CompilerServices.RuntimeHelpers':
        # BoxEnum doesn't exist in .NET 10 — force reflection fallback
        if method_name == 'BoxEnum':
            return _generate_reflection_call(mid)
        return _fallback_call_expr(mid, idx)

    # Final catch-all: three-tier fallback (generic call → reflection)
    return _fallback_call_expr(mid, idx)


def _has_pointer_params(mid: str) -> bool:
    """Check if a methodSubjectId uses pointer (*) or ref (&) parameters."""
    m = re.search(r'\(([^)]+)\)', mid)
    if not m:
        return False
    params = m.group(1)
    return '*' in params or '&' in params


def _generate_unsafe_helper(idx: int) -> tuple[str, str]:
    """Generate an unsafe helper method body and its name for pointer-based methods."""
    hname = f'H_{idx}'
    source = (
        '[System.Runtime.CompilerServices.MethodImpl('
        'System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]\n'
        f'static unsafe bool {hname}(int i)\n'
        '{\n'
        '    try\n'
        '    {\n'
        '        byte[] _bufSrc = new byte[64];\n'
        '        byte[] _bufDst = new byte[64];\n'
        '        fixed (byte* _pSrc = _bufSrc, _pDst = _bufDst)\n'
        '        {\n'
        '            Buffer.MemoryCopy(_pSrc, _pDst, 64, 64);\n'
        '        }\n'
        '        _g++;\n'
        '        return false;\n'
        '    }\n'
        '    catch { return true; }\n'
        '}'
    )
    return hname, source


def _generate_managed_harness(
    harness_dir: Path,
    family_slug: str,
    assembly: str,
    method_subject_ids: list[str],
    iterations: int = 100000,
) -> bool:
    """Generate ManagedBenchmarkHarness.cs and .csproj."""
    harness_dir.mkdir(parents=True, exist_ok=True)
    cs_path = harness_dir / "ManagedBenchmarkHarness.cs"
    csproj_path = harness_dir / "ManagedBenchmarkHarness.csproj"

    needs_unsafe = any(_has_pointer_params(mid) for mid in method_subject_ids)

    csproj_parts = [
        '<Project Sdk="Microsoft.NET.Sdk">\n'
        '  <PropertyGroup>\n'
        '    <OutputType>Exe</OutputType>\n'
        '    <StartupObject>ManagedBenchmarkHarness</StartupObject>\n'
        '    <TargetFrameworks>net8.0;net10.0;net48</TargetFrameworks>\n'
        '    <Nullable>enable</Nullable>\n'
        '    <ImplicitUsings>enable</ImplicitUsings>\n',
    ]
    if needs_unsafe:
        csproj_parts.append('    <AllowUnsafeBlocks>true</AllowUnsafeBlocks>\n')
    csproj_parts.append(
        '  </PropertyGroup>\n'
        '</Project>\n',
    )
    csproj_path.write_text(''.join(csproj_parts), encoding="utf-8")

    helper_methods: list[str] = []
    helper_names: list[str] = []
    for idx, mid in enumerate(method_subject_ids):
        call_expr, always_throws = _generate_call_expr(mid, idx)
        if call_expr:
            if always_throws:
                hname = f'H_{idx}'
                helper_names.append(hname)
                helper_methods.append(
                    '[System.Runtime.CompilerServices.MethodImpl('
                    'System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]\n'
                    f'static bool {hname}(int i)\n'
                    '{\n'
                    f'    try {{ {call_expr}; return false; }} catch {{ return true; }}\n'
                    '}'
                )
            else:
                hname = f'H_{idx}'
                helper_names.append(hname)
                helper_methods.append(
                    '[System.Runtime.CompilerServices.MethodImpl('
                    'System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]\n'
                    f'static bool {hname}(int i)\n'
                    '{\n'
                    f'    {call_expr};\n'
                    '    _g++;\n'
                    '    return false;\n'
                    '}'
                )
        elif _has_pointer_params(mid):
            hname, source = _generate_unsafe_helper(idx)
            helper_names.append(hname)
            helper_methods.append(source)
        else:
            helper_names.append('')
            continue

    method_sections: list[str] = []
    for idx, mid in enumerate(method_subject_ids):
        call_expr, always_throws = _generate_call_expr(mid, idx)
        hname = helper_names[idx] if idx < len(helper_names) else ''

        if not call_expr and not (hname and _has_pointer_params(mid)):
            method_sections.append(
                f'            {{ // [{idx}] {mid} — unsupported type\n'
                '                results.Add(new MethodResult {\n'
                f'                    MethodIndex = {idx},\n'
                f'                    MethodSubjectId = "{mid}",\n'
                '                    ElapsedMilliseconds = 0.0,\n'
                f'                    Iterations = {iterations},\n'
                '                    IsBodyReal = false,\n'
                '                    IsException = false,\n'
                '                });\n'
                '            }'
            )
            continue

        body = f'                    if ({hname}(i)) threw = true;' if hname else (
            f'                    {{ try {{ {call_expr}; _g++; }} catch {{ threw = true; }} }}'
        )

        method_sections.append(
            f'            {{ // [{idx}] {mid}\n'
            '                bool threw = false;\n'
            f'                for (int i = 0; i < {iterations}; i++) {{\n'
            f'{body}\n'
            '                }\n'
            '                double bestMs = double.MaxValue;\n'
            '                for (int r = 0; r < 3; r++) {\n'
            '                    var sw = System.Diagnostics.Stopwatch.StartNew();\n'
            f'                    for (int i = 0; i < {iterations}; i++) {{\n'
            f'{body}\n'
            '                    }\n'
            '                    sw.Stop();\n'
            '                    double ms = sw.Elapsed.TotalMilliseconds;\n'
            '                    if (ms < bestMs) bestMs = ms;\n'
            '                }\n'
            '                results.Add(new MethodResult {\n'
            f'                    MethodIndex = {idx},\n'
            f'                    MethodSubjectId = "{mid}",\n'
            '                    ElapsedMilliseconds = bestMs,\n'
            f'                    Iterations = {iterations},\n'
            '                    IsBodyReal = true,\n'
            '                    IsException = threw,\n'
            '                });\n'
            '            }'
        )

    sections_code = "\n".join(method_sections)
    helpers_code = "\n\n".join(helper_methods) if helper_methods else ""

    # Check if the reflection runner is needed
    needs_reflection = any('_ReflectionRunner' in h for h in helper_methods)

    reflection_runner_code = (
        '\n'
        '// _ReflectionRunner — generic reflection-based benchmark invocation.\n'
        '// Handles method signatures that cannot be expressed as direct C# calls.\n'
        '// Uses runtime type resolution and Delegate.CreateDelegate for performance.\n'
        'static class _ReflectionRunner\n'
        '{\n'
        '    static readonly Dictionary<string, InvokeEntry> _cache = new();\n'
        '\n'
        '    struct InvokeEntry\n'
        '    {\n'
        '        public System.Reflection.MethodBase Method;\n'
        '        public object? Instance;\n'
        '        public System.Reflection.ParameterInfo[] Parameters;\n'
        '    }\n'
        '\n'
        '    public static void Invoke(string mid, int i)\n'
        '    {\n'
        '        if (!_cache.TryGetValue(mid, out var entry))\n'
        '        {\n'
        '            entry = ParseEntry(mid);\n'
        '            _cache[mid] = entry;\n'
        '        }\n'
        '\n'
        '        var args = new object[entry.Parameters.Length];\n'
        '        for (int p = 0; p < args.Length; p++)\n'
        '            args[p] = CreateDefault(entry.Parameters[p].ParameterType, i);\n'
        '\n'
        '        try { entry.Method.Invoke(entry.Instance, args); }\n'
        '        catch { /* benign — benchmark may throw on edge inputs */ }\n'
        '    }\n'
        '\n'
        '    static InvokeEntry ParseEntry(string mid)\n'
        '    {\n'
        '        // mid format: "Assembly/Type::Method:RetType(Param1,Param2,...)"\n'
        '        var assemblySep = mid.IndexOf(\'/\');\n'
        '        if (assemblySep < 0) throw new ArgumentException($"Invalid mid: {mid}");\n'
        '        var rest = mid.Substring(assemblySep + 1);\n'
        '\n'
        '        var methodSep = rest.LastIndexOf("::", StringComparison.Ordinal);\n'
        '        if (methodSep < 0) throw new ArgumentException($"Invalid mid (no ::): {mid}");\n'
        '        var typeName = rest.Substring(0, methodSep);\n'
        '\n'
        '        var sigPart = rest.Substring(methodSep + 2);\n'
        '        var colonSep = sigPart.IndexOf(\':\');\n'
        '        if (colonSep < 0) throw new ArgumentException($"Invalid mid (no :): {mid}");\n'
        '        var methodName = sigPart.Substring(0, colonSep);\n'
        '\n'
        '        var retAndParams = sigPart.Substring(colonSep + 1);\n'
        '        var parenOpen = retAndParams.IndexOf(\'(\');\n'
        '        var parenClose = retAndParams.LastIndexOf(\')\');\n'
        '        var paramStr = parenOpen >= 0 && parenClose > parenOpen\n'
        '            ? retAndParams.Substring(parenOpen + 1, parenClose - parenOpen - 1)\n'
        '            : "";\n'
        '\n'
        '        var type = Type.GetType(typeName, throwOnError: false);\n'
        '        if (type == null)\n'
        '        {\n'
        '            // Fallback: try with "System.Private.CoreLib" assembly\n'
        '            type = Type.GetType($"{typeName}, System.Private.CoreLib", throwOnError: false);\n'
        '        }\n'
        '        if (type == null)\n'
        '        {\n'
        '            // Fallback: try with "System.Runtime" assembly\n'
        '            type = Type.GetType($"{typeName}, System.Runtime", throwOnError: false);\n'
        '        }\n'
        '        if (type == null)\n'
        '        {\n'
        '            // Fallback: try with "System.Collections" assembly\n'
        '            type = Type.GetType($"{typeName}, System.Collections", throwOnError: false);\n'
        '        }\n'
        '        if (type == null)\n'
        '            throw new ArgumentException($"Cannot resolve type: {typeName} from mid: {mid}");\n'
        '\n'
        '        var paramTypeNames = string.IsNullOrEmpty(paramStr)\n'
        '            ? Array.Empty<string>()\n'
        '            : paramStr.Split(\',\').Select(p => p.Trim()).ToArray();\n'
        '        var paramTypes = paramTypeNames\n'
        '            .Select(n => Type.GetType(n, throwOnError: false)\n'
        '                       ?? Type.GetType($"{n}, System.Private.CoreLib", throwOnError: false)\n'
        '                       ?? typeof(object))\n'
        '            .ToArray();\n'
        '\n'
        '        // Handle get_ / set_ / .ctor prefixes\n'
        '        System.Reflection.MethodBase? method = type.GetMethod(methodName, paramTypes);\n'
        '        if (method == null && methodName.StartsWith("get_"))\n'
        '        {\n'
        '            var prop = type.GetProperty(methodName.Substring(4));\n'
        '            method = prop?.GetGetMethod();\n'
        '        }\n'
        '        if (method == null && methodName.StartsWith("set_"))\n'
        '        {\n'
        '            var prop = type.GetProperty(methodName.Substring(4));\n'
        '            method = prop?.GetSetMethod();\n'
        '        }\n'
        '        if (method == null && methodName == ".ctor")\n'
        '        {\n'
        '            method = type.GetConstructors().FirstOrDefault(c =>\n'
        '                c.GetParameters().Length == paramTypes.Length);\n'
        '        }\n'
        '        if (method == null)\n'
        '            throw new ArgumentException($"Cannot resolve method {methodName}({paramStr}) on {typeName}");\n'
        '\n'
        '        var isStatic = method.IsStatic;\n'
        '        object? instance = null;\n'
        '        if (!isStatic)\n'
        '        {\n'
        '            try { instance = Activator.CreateInstance(type); }\n'
        '            catch { instance = null; }\n'
        '        }\n'
        '\n'
        '        return new InvokeEntry { Method = method, Instance = instance, Parameters = method.GetParameters() };\n'
        '    }\n'
        '\n'
        '    static object CreateDefault(Type t, int i)\n'
        '    {\n'
        '        if (t == typeof(int)) return i & 0xFF;\n'
        '        if (t == typeof(long)) return (long)(i & 0xFF);\n'
        '        if (t == typeof(short)) return (short)(i & 0xFF);\n'
        '        if (t == typeof(byte)) return (byte)(i & 0xFF);\n'
        '        if (t == typeof(char)) return (char)(i & 0xFF);\n'
        '        if (t == typeof(bool)) return (i & 1) == 0;\n'
        '        if (t == typeof(float)) return (float)(i & 0xFF);\n'
        '        if (t == typeof(double)) return (double)(i & 0xFF);\n'
        '        if (t == typeof(string)) return (i & 1) == 0 ? "A" : "B";\n'
        '        if (t == typeof(Type)) return typeof(int);\n'
        '        if (t == typeof(object)) return (object)(i & 0xFF);\n'
        '        if (t.IsEnum) return Enum.ToObject(t, i & 7);\n'
        '        if (t.IsArray) return Array.CreateInstance(t.GetElementType()!, 3);\n'
        '        if (t.IsValueType) try { return Activator.CreateInstance(t)!; } catch { }\n'
        '        return null;\n'
        '    }\n'
        '}\n'
    ) if needs_reflection else ''

    lines = [
        '// Auto-generated managed benchmark harness\n',
        f'// Family: {family_slug}, Assembly: {assembly}\n',
        'using System;\n',
        'using System.Collections.Generic;\n',
        'using System.Diagnostics;\n',
        'using System.Linq;\n',
        'using System.Reflection;\n',
        '\n',
        reflection_runner_code,
        '\n',
        'class ManagedBenchmarkHarness\n' if not needs_unsafe else 'unsafe class ManagedBenchmarkHarness\n',
        '{\n',
        '    static volatile int _g;\n',
        '\n',
        '    struct MethodResult\n',
        '    {\n',
        '        public int MethodIndex { get; set; }\n',
        '        public string MethodSubjectId { get; set; }\n',
        '        public double ElapsedMilliseconds { get; set; }\n',
        '        public int Iterations { get; set; }\n',
        '        public bool IsBodyReal { get; set; }\n',
        '        public bool IsException { get; set; }\n',
        '    }\n',
        '\n',
        helpers_code,
        '\n',
        '    static void Main()\n',
        '    {\n',
        '        var results = new List<MethodResult>();\n',
        sections_code,
        '\n',
        '        string json = _BuildJson(results);\n',
        '        Console.WriteLine(json);\n',
        '    }\n',
        '\n',
        '    /// Build minimal JSON without System.Text.Json dependency.\n',
        '    static string _BuildJson(List<MethodResult> results)\n',
        '    {\n',
        '        var sb = new System.Text.StringBuilder();\n',
        '        sb.Append("{\\"results\\":[");\n',
        '        for (int i = 0; i < results.Count; i++)\n',
        '        {\n',
        '            if (i > 0) sb.Append(",");\n',
        '            var r = results[i];\n',
        '            sb.Append("{");\n',
        '            sb.Append("\\"methodIndex\\":").Append(r.MethodIndex).Append(",");\n',
        '            sb.Append("\\"methodSubjectId\\":\\"").Append(',
        'r.MethodSubjectId.Replace("\\\\", "\\\\\\\\").Replace("\\"", "\\\\\\"")).Append("\\",");\n',
        '            sb.Append("\\"elapsedMilliseconds\\":").Append(',
        'r.ElapsedMilliseconds.ToString("0.000", System.Globalization.CultureInfo.InvariantCulture)).Append(",");\n',
        '            sb.Append("\\"iterations\\":").Append(r.Iterations).Append(",");\n',
        '            sb.Append("\\"isBodyReal\\":").Append(r.IsBodyReal ? "true" : "false").Append(",");\n',
        '            sb.Append("\\"isException\\":").Append(r.IsException ? "true" : "false");\n',
        '            sb.Append("}");\n',
        '        }\n',
        '        sb.Append("]}");\n',
        '        return sb.ToString();\n',
        '    }\n',
        '}\n',
    ]
    cs_path.write_text(''.join(lines), encoding="utf-8")
    return True


# ── Per-Runtime Runners ─────────────────────────────────────────────────

def _run_dotnet_benchmark(
    harness_dir: Path,
    tfm: str,
    iterations: int = 100000,
) -> dict[str, Any]:
    """Run the managed harness under a specific .NET TFM via dotnet run."""
    csproj = harness_dir / "ManagedBenchmarkHarness.csproj"
    if not csproj.exists():
        return {"method_results": [], "error": "harness not found"}

    print(f"  [managed-runner] Running .NET {tfm} benchmark...")
    start = time.perf_counter()
    try:
        r = subprocess.run(
            ["dotnet", "run", "--project", str(csproj), "-f", tfm,
             "--configuration", "Release", "--no-build"],
            capture_output=True, text=True, timeout=300,
        )
        if r.returncode != 0:
            r = subprocess.run(
                ["dotnet", "run", "--project", str(csproj), "-f", tfm,
                 "--configuration", "Release"],
                capture_output=True, text=True, timeout=300,
            )
            if r.returncode != 0:
                return {
                    "method_results": [],
                    "error": f"exit_code={r.returncode}: {r.stderr[:300]}",
                }

        elapsed = time.perf_counter() - start
        data = json.loads(r.stdout)
        method_results = data.get("results", [])

        sdk_r = subprocess.run(
            ["dotnet", "--version"], capture_output=True, text=True, timeout=10
        )

        for mr in method_results:
            ms = mr.get("elapsedMilliseconds", 0)
            it = mr.get("iterations", iterations)
            mr["opsPerSecond"] = (it / (ms / 1000.0)) if ms > 0 else 0.0
            mr["status"] = "completed" if (mr.get("isBodyReal", False) or mr.get("opsPerSecond", 0) > 0) else "error"
            mr["methodIndex"] = mr.get("methodIndex", 0)

        return {
            "method_results": method_results,
            "runtime_info": {"tfm": tfm, "sdk_version": sdk_r.stdout.strip()},
            "duration_s": round(elapsed, 2),
        }
    except subprocess.TimeoutExpired:
        return {"method_results": [], "error": "timeout (300s)"}
    except json.JSONDecodeError as e:
        return {"method_results": [], "error": f"json parse failed: {e}"}
    except Exception as e:
        return {"method_results": [], "error": str(e)}


def _run_mono_benchmark(
    harness_dir: Path,
    tfm: str,
    iterations: int = 100000,
    slug: str = "",
    assembly: str = "",
    method_subject_ids: list[str] | None = None,
) -> dict[str, Any]:
    """Build the harness for net48, then run with Mono.

    If the build fails and method_subject_ids are provided, attempts to filter
    out net48-incompatible methods and retry.
    """
    csproj = harness_dir / "ManagedBenchmarkHarness.csproj"
    # With -r win-x86, the output goes to bin/Release/net48/win-x86/
    build_dir = harness_dir / "bin" / "Release" / tfm / "win-x86"
    exe_path = build_dir / "ManagedBenchmarkHarness.exe"
    cs_path = harness_dir / "ManagedBenchmarkHarness.cs"

    print("  [managed-runner] Running Mono benchmark...")
    try:
        r = subprocess.run(
            ["dotnet", "build", str(csproj), "-f", tfm, "--configuration", "Release", "-r", "win-x86"],
            capture_output=True, text=True, timeout=120,
        )
        if r.returncode != 0:
            log_path = harness_dir / "mono_build_stderr.log"
            log_path.write_text(r.stderr or "(empty)", encoding="utf-8")
            stdout_path = harness_dir / "mono_build_stdout.log"
            stdout_path.write_text(r.stdout or "(empty)", encoding="utf-8")
            print(f"  [managed-runner] Mono BUILD FAILED -> stderr={log_path}, stdout={stdout_path}")
            print(f"  [managed-runner] stderr: {r.stderr[:200]}")
            print(f"  [managed-runner] stdout (first 500): {r.stdout[:500]}")

            # Retry: filter out net48-incompatible methods
            if method_subject_ids:
                excluded = _parse_build_error_method_indices(r.stderr + "\n" + r.stdout, cs_path)
                if excluded:
                    compatible = [m for i, m in enumerate(method_subject_ids) if i not in excluded]
                    print(f"  [managed-runner] Retrying with {len(compatible)}/{len(method_subject_ids)} methods (excluded indices: {sorted(excluded)})")
                    _generate_managed_harness(harness_dir, slug, assembly, compatible, iterations)
                    r2 = subprocess.run(
                        ["dotnet", "build", str(csproj), "-f", tfm, "--configuration", "Release", "-r", "win-x86"],
                        capture_output=True, text=True, timeout=120,
                    )
                    if r2.returncode == 0:
                        # Rebuild succeeded — run with compatible methods
                        return _run_mono_after_build(harness_dir, exe_path, tfm, iterations, excluded, method_subject_ids)

            return {"method_results": [], "error": f"build failed: {r.stderr[:300]}"}

        if not exe_path.exists():
            return {"method_results": [], "error": f"exe not found at {exe_path}"}

        mono_exe = _mono_path()
        if not mono_exe:
            return {"method_results": [], "error": "mono not found"}

        return _run_mono_exec(mono_exe, exe_path, harness_dir, tfm, iterations)

    except subprocess.TimeoutExpired:
        return {"method_results": [], "error": "timeout (300s)"}
    except json.JSONDecodeError as e:
        log_path = harness_dir / "mono_stdout.json_debug.log"
        log_path.write_text(r.stdout[:5000] if hasattr(r, 'stdout') and r.stdout else "(empty)", encoding="utf-8")
        print(f"  [managed-runner] Mono JSON parse failed: {e}")
        print(f"  [managed-runner] Raw output -> {log_path}")
        return {"method_results": [], "error": f"json parse failed: {e}"}
    except Exception as e:
        return {"method_results": [], "error": str(e)}


def _run_mono_exec(
    mono_exe: str, exe_path: Path, harness_dir: Path, tfm: str, iterations: int,
) -> dict[str, Any]:
    """Execute mono and parse results."""
    start = time.perf_counter()
    r = subprocess.run(
        [mono_exe, str(exe_path)],
        capture_output=True, text=True, timeout=300,
    )
    elapsed = time.perf_counter() - start
    if r.returncode != 0:
        log_path = harness_dir / "mono_stderr.log"
        log_path.write_text(r.stderr or "(empty)", encoding="utf-8")
        stdout_path = harness_dir / "mono_stdout.log"
        stdout_path.write_text(r.stdout or "(empty)", encoding="utf-8")
        print(f"  [managed-runner] Mono FAILED (rc={r.returncode}) -> {log_path}")
        print(f"  [managed-runner] stderr preview: {r.stderr[:500]}")
        if r.stderr and "System.Text.Json" in r.stderr:
            print(f"  [managed-runner] DETECTED: System.Text.Json runtime issue")
        return {"method_results": [], "error": f"mono exit_code={r.returncode}: {r.stderr[:300]}"}

    data = json.loads(r.stdout)
    method_results = data.get("results", [])

    for mr in method_results:
        ms = mr.get("elapsedMilliseconds", 0)
        it = mr.get("iterations", iterations)
        mr["opsPerSecond"] = (it / (ms / 1000.0)) if ms > 0 else 0.0
        mr["status"] = "completed" if (mr.get("isBodyReal", False) or mr.get("opsPerSecond", 0) > 0) else "error"
        mr["methodIndex"] = mr.get("methodIndex", 0)

    return {
        "method_results": method_results,
        "runtime_info": {"tfm": tfm, "runner": "mono"},
        "duration_s": round(elapsed, 2),
    }


def _run_mono_after_build(
    harness_dir: Path, exe_path: Path, tfm: str, iterations: int,
    excluded: set[int], all_method_subject_ids: list[str],
) -> dict[str, Any]:
    """Run mono after successful filtered build, adding back skipped methods."""
    mono_exe = _mono_path()
    if not mono_exe:
        return {"method_results": [], "error": "mono not found"}

    result = _run_mono_exec(mono_exe, exe_path, harness_dir, tfm, iterations)

    if "error" in result:
        return result

    # Remap filtered method indices back to original indices
    filtered_results: list[dict] = result.get("method_results", [])
    original_indices = [i for i in range(len(all_method_subject_ids)) if i not in excluded]
    remapped: list[dict] = []
    for mr in filtered_results:
        fi = mr.get("methodIndex", 0)
        if fi < len(original_indices):
            mr["methodIndex"] = original_indices[fi]
            mr["methodSubjectId"] = all_method_subject_ids[original_indices[fi]]
        remapped.append(mr)

    # Add back excluded methods as unsupported
    for idx in sorted(excluded):
        remapped.append({
            "methodIndex": idx,
            "methodSubjectId": all_method_subject_ids[idx],
            "elapsedMilliseconds": 0.0,
            "iterations": iterations,
            "isBodyReal": False,
            "isException": False,
            "opsPerSecond": 0.0,
            "status": "skipped_unsupported_api",
        })

    result["method_results"] = remapped
    return result


def _parse_build_error_method_indices(stderr: str, cs_path: Path) -> set[int]:
    """Parse build stderr/stdout to find which method indices have compilation errors.

    Maps error line numbers back to method indices using H_N helper definitions
    in the generated .cs file (e.g., ``static bool H_5(int i)`` at line 197).
    """
    error_lines: set[int] = set()
    for m in re.finditer(r'\.cs\((\d+),', stderr):
        error_lines.add(int(m.group(1)))
    if not error_lines:
        return set()

    if not cs_path.exists():
        return set()

    src_lines = cs_path.read_text(encoding="utf-8", errors="replace").splitlines()
    # Map: line number -> method index for ``static bool H_N(`` definitions
    def_lines: dict[int, int] = {}
    for i, line in enumerate(src_lines, 1):
        mm = re.match(r'\s*static bool (H_\d+)\(', line)
        if mm:
            idx = int(mm.group(1)[2:])  # H_5 -> 5
            def_lines[i] = idx

    if not def_lines:
        return set()

    sorted_defs = sorted(def_lines.keys())
    excluded: set[int] = set()
    for err_line in error_lines:
        for def_line in reversed(sorted_defs):
            if def_line <= err_line:
                excluded.add(def_lines[def_line])
                break

    return excluded


# ── Managed benchmark entry point ───────────────────────────────────────

def _load_method_subject_ids(ctx: FamilyContext) -> list[str]:
    """Load methodSubjectIds from contract.json."""
    contract_path = ctx.contract_path
    if not contract_path.exists():
        return []
    try:
        c = json.loads(contract_path.read_text(encoding="utf-8"))
        mids = c.get("methodSubjectIds", [])
        if not mids:
            mids = [m["methodSubjectId"] for m in c.get("methodContracts", []) if m.get("methodSubjectId")]
        return mids
    except Exception:
        return []


def run_managed_benchmark(
    ctx: FamilyContext,
    technology: str,
    iterations: int = 100000,
) -> StageResult:
    """Run a single managed runtime benchmark and persist results."""
    start = time.perf_counter()

    if technology not in TECHNOLOGY_DEFS:
        return StageResult(
            stage=f"managed_{technology}", status="error",
            summary=f"Unknown technology: {technology}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    available, unavailable = detect_managed_runtimes([technology])
    if technology in unavailable:
        return StageResult(
            stage=f"managed_{technology}", status="skipped",
            summary=f"{technology} not available (SDK not found)",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    mids = _load_method_subject_ids(ctx)
    if not mids:
        return StageResult(
            stage=f"managed_{technology}", status="skipped",
            summary="no methods in contract",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    harness_dir = ctx.family_dir / "managed_test" / "benchmarks"
    cs_path = harness_dir / "ManagedBenchmarkHarness.cs"
    csproj = harness_dir / "ManagedBenchmarkHarness.csproj"
    if not csproj.exists() or not cs_path.exists():
        print(f"  [managed-runner] Generating managed harness at {harness_dir}...")
        if not _generate_managed_harness(harness_dir, ctx.slug, ctx.assembly, mids, iterations):
            return StageResult(
                stage=f"managed_{technology}", status="error",
                summary="harness generation failed",
                duration_ms=int((time.perf_counter() - start) * 1000),
            )
    else:
        cs_text = cs_path.read_text(encoding="utf-8")
        existing_count = len(re.findall(r'\{\s*//\s*\[\d+\]', cs_text))
        if existing_count != len(mids):
            print(f"  [managed-runner] Method count changed ({existing_count} -> {len(mids)}), regenerating...")
            if not _generate_managed_harness(harness_dir, ctx.slug, ctx.assembly, mids, iterations):
                return StageResult(
                    stage=f"managed_{technology}", status="error",
                    summary="harness regeneration failed",
                    duration_ms=int((time.perf_counter() - start) * 1000),
                )

    spec = TECHNOLOGY_DEFS[technology]

    if spec["runner"] == "dotnet-run":
        result = _run_dotnet_benchmark(harness_dir, spec["tfm"], iterations)
    elif spec["runner"] == "mono":
        result = _run_mono_benchmark(
            harness_dir, spec["tfm"], iterations,
            slug=ctx.slug, assembly=ctx.assembly,
            method_subject_ids=mids,
        )
    else:
        return StageResult(
            stage=f"managed_{technology}", status="error",
            summary=f"Unknown runner: {spec['runner']}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    method_results = result.get("method_results", [])
    error = result.get("error")

    status = "passed" if (method_results and not error) else "failed"
    summary = f"{status}: {len(method_results)} methods"
    if error:
        summary += f", error: {error[:100]}"

    if method_results:
        save_managed_benchmark_records(ctx, technology, method_results)
        print(f"  [managed-runner] Saved {len(method_results)} records to perf store")

    ops_list = [
        mr.get("opsPerSecond", 0) for mr in method_results
        if mr.get("opsPerSecond", 0) > 0
    ]
    avg_ops = sum(ops_list) / len(ops_list) if ops_list else 0.0

    return StageResult(
        stage=f"managed_{technology}",
        status=status,
        summary=summary,
        details={
            "technology": technology,
            "results": method_results,
            "runtime_info": result.get("runtime_info", {}),
            "duration_s": result.get("duration_s", 0),
            "averageOpsPerSecond": round(avg_ops, 0),
            "okCount": len(ops_list),
            "totalMethods": len(method_results),
        },
        errors=[error] if error else [],
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


# ── Native benchmark runners ────────────────────────────────────────────

def _load_method_count(ctx: FamilyContext) -> int:
    """Load method count from contract.json."""
    contract_path = ctx.contract_path
    if not contract_path.exists():
        return 0
    try:
        import json as _json
        c = _json.loads(contract_path.read_text(encoding="utf-8"))
        mids = c.get("methodSubjectIds", [])
        if mids:
            return len(mids)
        return c.get("methodSubjectCount", 0)
    except Exception:
        return 0


def _run_single_benchmark(
    exe_path: Path, method_index: int, iterations: int = 100000,
) -> dict[str, Any] | None:
    """Run entry.exe --benchmark N and parse JSON timing output."""
    try:
        r = subprocess.run(
            [str(exe_path), "--benchmark", str(method_index), str(iterations)],
            capture_output=True, text=True, timeout=120,
        )
    except subprocess.TimeoutExpired:
        return {"methodIndex": method_index, "error": "timed out"}
    except OSError as e:
        return {"methodIndex": method_index, "error": str(e)}

    output = (r.stdout or "").strip()
    for line in output.splitlines():
        line = line.strip()
        if line.startswith("{"):
            try:
                return json.loads(line)
            except json.JSONDecodeError:
                pass
    return {"methodIndex": method_index, "error": f"no JSON output: {output[:200]}", "exitCode": r.returncode}


def _run_all_benchmarks(
    ctx: FamilyContext, exe_path: Path, label: str,
) -> dict[str, Any]:
    """Run --benchmark for all methods under a given EXE.

    R5: Before timing, runs --fact-json to verify all methods pass (value gate).
    Methods that fail the value gate are excluded from benchmark timing.
    """
    method_count = _load_method_count(ctx)
    if method_count == 0:
        return {"status": "skipped", "summary": "no methods in contract"}

    # R5: Value gate — pre-verify all methods before benchmarking
    value_gate_failures: set[int] = set()
    try:
        r = subprocess.run(
            [str(exe_path), "--fact-json"],
            capture_output=True, text=True, timeout=120,
        )
        # Check exit code zero AND valid JSON output
        fact_data = None
        if r.returncode == 0:
            # Robust JSON extraction — find first complete JSON object in output
            output = (r.stdout or "").strip()
            json_start = output.find("{")
            if json_start >= 0:
                depth = 0
                json_end = -1
                for i in range(json_start, len(output)):
                    if output[i] == '{':
                        depth += 1
                    elif output[i] == '}':
                        depth -= 1
                        if depth == 0:
                            json_end = i + 1
                            break
                if json_end > json_start:
                    try:
                        fact_data = json.loads(output[json_start:json_end])
                    except json.JSONDecodeError:
                        pass

        if fact_data:
            for nr in fact_data.get("factResults", []):
                si = nr.get("si", -1)
                if si >= 0 and not nr.get("passed", True):
                    value_gate_failures.add(si)
                    print(f"    [benchmark/{label}] value gate FAIL: si={si}")
        elif r.returncode != 0:
            print(f"    [benchmark/{label}] WARNING: --fact-json returned exit={r.returncode}, skipping value gate")
        else:
            print(f"    [benchmark/{label}] WARNING: could not parse --fact-json output, skipping value gate")
    except Exception as e:
        print(f"    [benchmark/{label}] WARNING: value gate error: {e}, skipping")

    if value_gate_failures:
        print(f"    [benchmark/{label}] value gate: {len(value_gate_failures)} methods excluded from benchmark")

    results: list[dict[str, Any]] = []
    total_ops = 0.0
    ok_count = 0
    fail_count = 0

    for i in range(method_count):
        if i in value_gate_failures:
            results.append({
                "methodIndex": i,
                "error": "value_gate_failed",
                "opsPerSecond": 0,
            })
            fail_count += 1
            continue

        result = _run_single_benchmark(exe_path, i)
        results.append(result)
        if result and "error" not in result:
            total_ops += result.get("opsPerSecond", 0)
            ok_count += 1
        else:
            fail_count += 1

    avg_ops = total_ops / ok_count if ok_count > 0 else 0.0
    print(f"  [benchmark/{label}] {ok_count}/{method_count} OK, avg {avg_ops:.0f} ops/s"
          f" ({len(value_gate_failures)} value-gate skipped)")

    return {
        "status": "passed" if ok_count > 0 else "failed",
        "label": label,
        "results": results,
        "okCount": ok_count,
        "failCount": fail_count,
        "totalMethods": method_count,
        "averageOpsPerSecond": avg_ops,
        "valueGateFailures": list(value_gate_failures),
    }


def run_benchmark(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 8: 3-way benchmark (native-aot + native-jit + managed)."""
    start = time.perf_counter()

    method_count = _load_method_count(ctx)
    if method_count == 0:
        return StageResult(
            stage="benchmark", status="skipped",
            summary="no methods in contract",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    aot_exe = ctx.entry_exe_path
    jit_exe = ctx.entry_jit_exe_path

    benchmarks: dict[str, Any] = {}

    if aot_exe.exists():
        aot_result = _run_all_benchmarks(ctx, aot_exe, "native-aot")
        benchmarks["native-aot"] = aot_result
    else:
        benchmarks["native-aot"] = {"status": "skipped", "summary": "entry.exe not found"}

    if jit_exe.exists():
        jit_result = _run_all_benchmarks(ctx, jit_exe, "native-jit")
        benchmarks["native-jit"] = jit_result
    else:
        benchmarks["native-jit"] = {"status": "skipped", "summary": "entry-jit.exe not found"}

    managed_results: dict[str, Any] = {}
    for tech in _MANAGED_TECHNOLOGIES:
        sr = run_managed_benchmark(ctx, tech)
        managed_results[tech] = {"status": sr.status, "summary": sr.summary}
        if sr.status == "passed":
            print(f"  [benchmark/managed] {tech}: {sr.summary}")
        elif sr.status == "skipped":
            print(f"  [benchmark/managed] {tech}: skipped ({sr.summary})")
        else:
            print(f"  [benchmark/managed] {tech}: {sr.status} ({sr.summary})")
    benchmarks["managed"] = managed_results

    total_ok = sum(
        1 for b in benchmarks.values()
        if isinstance(b, dict) and b.get("status") == "passed"
        and b.get("label") in ("native-aot", "native-jit")
    )

    status = "passed" if total_ok > 0 else "failed"

    return StageResult(
        stage="benchmark", status=status,
        summary=f"{status}: native-aot="
                f"{benchmarks.get('native-aot', {}).get('okCount', 0)}/"
                f"{benchmarks.get('native-aot', {}).get('totalMethods', 0)}, "
                f"native-jit="
                f"{benchmarks.get('native-jit', {}).get('okCount', 0)}/"
                f"{benchmarks.get('native-jit', {}).get('totalMethods', 0)}, "
                f"managed="
                f"{sum(1 for m in managed_results.values() if m['status'] == 'passed')}/"
                f"{len(_MANAGED_TECHNOLOGIES)}",
        details=benchmarks,
        duration_ms=int((time.perf_counter() - start) * 1000),
    )
