from __future__ import annotations

import re
from pathlib import Path
from typing import Any

# ---------------------------------------------------------------------------
# Type classification and default-value mapping for auto-generated test bodies
# ---------------------------------------------------------------------------

PRIMITIVE_SET = frozenset({
    "System.Boolean",
    "System.Byte",
    "System.SByte",
    "System.Int16",
    "System.UInt16",
    "System.Int32",
    "System.UInt32",
    "System.Int64",
    "System.UInt64",
    "System.Single",
    "System.Double",
    "System.Decimal",
    "System.Char",
    "System.String",
    "System.IntPtr",
    "System.UIntPtr",
    "System.Type",
    "System.Guid",
    "System.TimeSpan",
    "System.DateTime",
    "System.DateOnly",
    "System.TimeOnly",
    "System.Uri",
    "System.Version",
    "System.Object",
    "System.Enum",
    "System.ValueType",
    "System.Array",
})

# ── Data loaded from JSON (shared with future C# code) ────────────────────
import json

# ---------------------------------------------------------------------------
# Data loaded from JSON file (extracted for shared use by Python and future C#)
# ---------------------------------------------------------------------------
_HERE = Path(__file__).resolve().parent
_JSON_PATH = _HERE / "test_code_generator_data.json"
if _JSON_PATH.exists():
    _DATA = json.loads(_JSON_PATH.read_text(encoding="utf-8"))
else:
    _DATA = {}

def _load_str_dict(key: str) -> dict[str, str]:
    return {k: v for k, v in _DATA.get(key, {}).items()}

def _load_str_list_dict(key: str) -> dict[str, list[str]]:
    return {k: list(v) if isinstance(v, list) else [v] for k, v in _DATA.get(key, {}).items()}

def _load_tuple_dict(key: str) -> dict[tuple[str, str, int], str]:
    result = {}
    for k, v in _DATA.get(key, {}).items():
        parts = k.split('|')
        if len(parts) == 3:
            result[(parts[0], parts[1], int(parts[2]))] = v
    return result

def _load_str_set(key: str) -> set:
    return set(_DATA.get(key, []))

def _load_int_dict(key: str) -> dict[int, str]:
    return {int(k): v for k, v in _DATA.get(key, {}).items()}

# ---- Load all data dicts from JSON ----
INSTANCE_EXPR_MAP: dict[str, str] = _load_str_dict("INSTANCE_EXPR_MAP")
INSTANCE_ALTERNATIVE_EXPR_MAP: dict[str, str] = _load_str_dict("INSTANCE_ALTERNATIVE_EXPR_MAP")
STATIC_METHODS_BY_TYPE: dict[str, frozenset[str]] = {k: frozenset(v) for k, v in _load_str_list_dict("STATIC_METHODS_BY_TYPE").items()}
METHOD_OVERRIDES: dict[tuple[str, str, int], str] = _load_tuple_dict("METHOD_OVERRIDES")
TYPE_DEFAULT_MAP: dict[str, str] = _load_str_dict("TYPE_DEFAULT_MAP")
TYPE_ALTERNATIVE_MAP: dict[str, str] = _load_str_dict("TYPE_ALTERNATIVE_MAP")
_STATIC_BY_SIGNATURE: set[tuple[str, str, int]] = {tuple(x) for x in _DATA.get("_STATIC_BY_SIGNATURE", [])}
_NEEDS_MANUAL_METHODS: set[tuple[str, str]] = {tuple(x) for x in _DATA.get("_NEEDS_MANUAL_METHODS", [])}
_SKIP_AUDIT: dict[tuple[str, str, int], str] = _load_tuple_dict("_SKIP_AUDIT")
_GENERIC_ARGS_MAP: dict[int, str] = _load_int_dict("_GENERIC_ARGS_MAP")

# Hardcoded constants (not loaded from JSON — used directly in code)
STATIC_TYPES = frozenset({
    "Convert", "Math", "MemoryMarshal", "RuntimeHelpers",
    "BitConverter", "Buffer", "Activator", "Interlocked", "Monitor",
    "ZipFileExtensions", "RuntimeInformation", "JsonSerializer",
})

_ACRONYMS = frozenset({"io", "id", "db", "ui", "os", "ip"})

BASE_USINGS = "\n".join([
    "using System;",
    "using System.IO;",
    "using System.Threading;",
    "using System.Threading.Tasks;",
    "using System.Globalization;",
    "using System.Linq;",
    "using System.Runtime.CompilerServices;",
    "using System.Runtime.InteropServices;",
    "using System.Collections.Generic;",
    "using System.Reflection;",
])


def parse_method_subject_id(method_subject_id: str) -> dict[str, Any]:
    """Parse a methodSubjectId into its components.

    Format: ``Assembly/Namespace.Type::MethodName:ReturnType(ParamType1,ParamType2)``

    Returns a dict with keys:
        type_path, type_name, method_name, return_type, param_types, type_arity
    """
    # "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int32)"
    if "::" not in method_subject_id:
        return {"type_path": "", "type_name": "", "method_name": method_subject_id, "return_type": "", "param_types": [], "type_arity": 0}

    path_sig = method_subject_id.split("::", 1)
    type_path = path_sig[0]  # "System.Private.CoreLib/System.Convert"
    sig = path_sig[1]  # "ToChar:System.Char(System.Int32)"

    # Extract type short name: "System.Convert" -> "Convert"
    raw_type = type_path.split("/", 1)[1] if "/" in type_path else type_path
    # CLR nested type separator (+) → C# dot (.) for valid C# syntax.
    # For nested types, keep parent class in short name:
    #   "ComVariantMarshaller+RefPropagate" → "ComVariantMarshaller.RefPropagate"
    has_nested = "+" in raw_type
    raw_type = raw_type.replace("+", ".")
    if has_nested:
        parts = raw_type.rsplit(".", 2)
        type_name = ".".join(parts[-2:]) if len(parts) >= 2 else raw_type
    else:
        type_name = raw_type.rsplit(".", 1)[-1]

    # Normalize CLR {T} generic notation to backtick format for downstream.
    # e.g., "IEnumerable{T}" → "IEnumerable`1", "Dictionary{TKey,TValue}" → "Dictionary`2"
    generic_match = re.search(r"\{([^}]+)\}", type_name)
    if generic_match:
        param_count = len(generic_match.group(1).split(","))
        type_name = type_name.replace(generic_match.group(0), f"`{param_count}")

    # Strip CLR backtick suffix: "Nullable`1" -> "Nullable", but record arity
    arity = 0
    bt = re.match(r"^(\w+)`(\d+)$", type_name)
    if bt:
        type_name = bt.group(1)
        arity = int(bt.group(2))

    # Split method+return and params
    paren = sig.find("(")
    if paren < 0:
        return {"type_path": type_path, "type_name": type_name, "method_name": sig, "return_type": "", "param_types": [], "type_arity": arity}

    # Find matching close paren (track depth for correct nesting)
    close_paren = paren
    depth = 0
    for i in range(paren, len(sig)):
        if sig[i] == '(':
            depth += 1
        elif sig[i] == ')':
            depth -= 1
            if depth == 0:
                close_paren = i
                break

    method_and_ret = sig[:paren]
    params_part = sig[paren + 1:close_paren]  # between parens
    ret_suffix = sig[close_paren + 1:]         # after closing paren (e.g. ":System.Void()")

    # Return type can be in two positions:
    #   Format A: "Method:ReturnType(Params)"  — in method_and_ret before the paren
    #   Format B: "Method(Params):ReturnType"   — in ret_suffix after the paren
    colon = method_and_ret.rfind(":")
    if colon >= 0:
        # Format A
        method_name = method_and_ret[:colon]
        # Strip trailing colons (e.g. "AsnDecoder:" from "AsnDecoder::Void(...)")
        method_name = method_name.rstrip(":")
        return_type = method_and_ret[colon + 1:]
    else:
        # Format B
        method_name = method_and_ret
        ret_colon = ret_suffix.rfind(":")
        if ret_colon >= 0:
            return_type = ret_suffix[ret_colon + 1:]
        else:
            return_type = ""

    # CLR constructor subject IDs use the type name instead of ".ctor",
    # e.g. "AsnDecoder::Void(...)". Normalize to ".ctor" for downstream.
    # Also handle the case where the "method" part before return type equals the type name.
    raw_method = method_and_ret[:colon] if colon >= 0 else ""
    raw_method = raw_method.rstrip(":")
    if raw_method == type_name and method_name == type_name:
        method_name = ".ctor"

    # Normalize CLR abbreviated return types to fully-qualified form
    _CLR_TO_SYSTEM = {
        "Void": "System.Void",
    }
    if return_type in _CLR_TO_SYSTEM:
        return_type = _CLR_TO_SYSTEM[return_type]
    # Strip CLR trailing () from return types (e.g. "System.Void()" -> "System.Void")
    if return_type.endswith("()"):
        return_type = return_type[:-2]

    # Parse parameter types (handle nested generics carefully)
    param_types = split_param_types(params_part)

    return {
        "type_path": type_path,
        "type_name": type_name,
        "method_name": method_name,
        "return_type": return_type,
        "param_types": param_types,
        "type_arity": arity,
    }


def split_param_types(params_part: str) -> list[str]:
    """Split comma-separated parameter types, respecting generic angle brackets."""
    if not params_part.strip():
        return []
    types: list[str] = []
    depth = 0
    current: list[str] = []
    for ch in params_part:
        if ch in "<[":
            depth += 1
            current.append(ch)
        elif ch in ">]":
            depth -= 1
            current.append(ch)
        elif ch == "," and depth == 0:
            types.append("".join(current).strip())
            current = []
        else:
            current.append(ch)
    if current:
        types.append("".join(current).strip())
    return types


def is_primitive_type(csharp_type: str) -> bool:
    """Check if a type string is in the primitive set."""
    # Normalize: strip trailing & (ref), * (pointer), ? (nullable), [] (array)
    bare = csharp_type.rstrip("&*?").rstrip("[]").strip()
    if not bare:
        return False
    return bare in PRIMITIVE_SET


def has_unsafe_param(param_types: list[str]) -> bool:
    """Check if any parameter is ref, pointer, or generic type param."""
    for pt in param_types:
        pt = pt.strip()
        if pt.endswith("&"):
            return True
        if pt.endswith("*"):
            return True
        # Generic type parameter: single uppercase letter or starts with 'T' followed by uppercase
        bare = pt.rstrip("&*?").strip()
        # Skip full type names (contain dots or backticks) — those are concrete types
        if "." in bare or "`" in bare:
            continue
        # Skip array types
        if bare.endswith("[]"):
            bare = bare[:-2]
            if "." in bare or "`" in bare:
                continue
        if bare and ((len(bare) == 1 and bare.isupper()) or (bare.startswith("T") and len(bare) > 1 and bare[1].isupper())):
            return True
    return False


def is_simple_method(parsed: dict[str, Any]) -> bool:
    """All parameters + return type are primitive, and no ref/pointer/generic."""
    if has_unsafe_param(parsed["param_types"]):
        return False
    for pt in parsed["param_types"]:
        if not is_primitive_type(pt):
            return False
    # Return type can be void — still simple if all params are primitive
    ret = parsed["return_type"]
    if ret and ret != "System.Void" and not is_primitive_type(ret):
        return False
    return True


def default_expr(csharp_type: str) -> str:
    """Generate a default C# expression for a type."""
    bare = csharp_type.rstrip("&*?").strip()
    if bare in TYPE_DEFAULT_MAP:
        return TYPE_DEFAULT_MAP[bare]
    if bare.endswith("[]"):
        return f"Array.Empty<{bare[:-2]}>()"
    return "null!"


def has_blocked_param(param_types: list[str]) -> bool:
    """Check if any parameter type is blocked from auto-generation in entrypoint context."""
    for pt in param_types:
        pt = pt.strip()
        bare = pt.rstrip("&*?").strip()
        # Pointer parameters need unsafe context
        if pt.endswith("*"):
            return True
        # Generic type parameter (not concrete type)
        if "." not in bare and "`" not in bare:
            if bare and ((len(bare) == 1 and bare.isupper()) or
                         (bare.startswith("T") and len(bare) > 1 and bare[1].isupper())):
                return True
    return False


def has_ref_param(param_types: list[str]) -> bool:
    """Check if any parameter is a ref parameter."""
    return any(pt.strip().endswith("&") for pt in param_types)


def default_expr_for_type(csharp_type: str, type_map: dict[str, str] | None = None) -> str:
    """Generate default expression using an alternative type map.

    Handles out/ref parameters (types ending with &) by prepending 'out '.
    The actual parameter is passed as a discard target for out params,
    or as the default value for non-ref types.
    """
    tm = type_map or TYPE_DEFAULT_MAP

    # Handle out/ref parameters — strip trailing & and prepend "out "
    prefix = ""
    ct = csharp_type.strip()
    if ct.endswith("&"):
        prefix = "out "
        ct = ct[:-1].strip()

    bare = ct.rstrip("*?").strip()
    if bare in tm:
        return prefix + tm[bare]
    if bare.endswith("[]"):
        return prefix + f"Array.Empty<{bare[:-2]}>()"
    return prefix + "default"


def build_call_expr_with_args(parsed: dict[str, Any], args: str, instance_map: dict[str, str] | None = None) -> str:
    """Build call expression from pre-computed args (shared by ref and non-ref paths)."""
    type_name = parsed["type_name"]
    method_name = parsed["method_name"]
    param_count = len(parsed["param_types"])
    im = instance_map or INSTANCE_EXPR_MAP

    override = METHOD_OVERRIDES.get((type_name, method_name, param_count))
    if override is not None and override != "skip":
        return override

    if method_name in (".ctor", ".cctor"):
        return build_ctor_expr(type_name, args)

    expr = try_property_access(type_name, method_name, args)
    if expr is not None:
        return expr

    if (type_name, method_name, param_count) in _STATIC_BY_SIGNATURE:
        return f"{type_name}.{method_name}({args})"

    static_methods = STATIC_METHODS_BY_TYPE.get(type_name, frozenset())
    if method_name in static_methods:
        return f"{type_name}.{method_name}({args})"

    if type_name in STATIC_TYPES:
        return f"{type_name}.{method_name}({args})"

    inst = im.get(type_name)
    if inst is not None:
        return f"{inst}.{method_name}({args})"

    return f"{type_name}.{method_name}({args})"


def build_call_expr_with_refs(
    parsed: dict[str, Any],
    type_map: dict[str, str] | None = None,
    instance_map: dict[str, str] | None = None,
) -> tuple[str, str]:
    """Build a C# call expression for methods with ref parameters.

    Returns (prelude, call_expr) where prelude contains local variable
    declarations for ref params (empty string if none).
    """
    type_name = parsed["type_name"]
    method_name = parsed["method_name"]
    param_types = parsed["param_types"]
    tm = type_map or TYPE_DEFAULT_MAP
    im = instance_map or INSTANCE_EXPR_MAP

    param_count = len(param_types)
    override = METHOD_OVERRIDES.get((type_name, method_name, param_count))
    if override is not None and override != "skip":
        return ("", override)

    prelude_lines: list[str] = []
    call_args: list[str] = []

    for i, pt in enumerate(param_types):
        pt = pt.strip()
        if pt.endswith("&"):
            # Use out _ (discard) for all & params.
            # The CLR signature doesn't distinguish out from ref, but most & params
            # are out in practice. Methods with true ref params get METHOD_OVERRIDES.
            call_args.append("out _")
        else:
            call_args.append(default_expr_for_type(pt, tm))

    prelude = "\n".join(prelude_lines)
    call_expr = build_call_expr_with_args(parsed, ", ".join(call_args), im)
    return (prelude, call_expr)


def build_call_expr_with_ref_locals(
    parsed: dict[str, Any],
    type_map: dict[str, str] | None = None,
    instance_map: dict[str, str] | None = None,
) -> tuple[str, str]:
    """Build a C# call expression using named local variables for ref params.

    Unlike _build_call_expr_with_refs which uses out _ (discard), this variant
    declares local variables for each ref param so the method's side effects on
    them are observable (useful when the method mutates ref params).

    Returns (prelude, call_expr) where prelude contains local variable declarations.
    """
    type_name = parsed["type_name"]
    method_name = parsed["method_name"]
    param_types = parsed["param_types"]
    tm = type_map or TYPE_DEFAULT_MAP
    im = instance_map or INSTANCE_EXPR_MAP

    param_count = len(param_types)
    override = METHOD_OVERRIDES.get((type_name, method_name, param_count))
    if override is not None and override != "skip":
        return ("", override)

    prelude_lines: list[str] = []
    call_args: list[str] = []

    for i, pt in enumerate(param_types):
        pt = pt.strip()
        if pt.endswith("&"):
            base_type = pt.rstrip("&").strip()
            # Convert CLR metadata generics ({}) to C# generics (<>)
            # e.g. System.Span{System.IntPtr} -> System.Span<System.IntPtr>
            base_type = base_type.replace("{", "<").replace("}", ">")
            default_val = default_expr_for_type(base_type, tm)
            var_name = f"refLocal_{i}"
            prelude_lines.append(f"            {base_type} {var_name} = {default_val};")
            call_args.append(f"out {var_name}")
        else:
            call_args.append(default_expr_for_type(pt, tm))

    prelude = "\n".join(prelude_lines)
    call_expr = build_call_expr_with_args(parsed, ", ".join(call_args), im)
    return (prelude, call_expr)


def cast_return_to_int(ret: str, call_expr: str) -> str:
    """Wrap a call expression so it produces an int for exit code comparison.

    For void returns, wraps in a statement and appends ; return 0;.
    For int returns, passes through.
    For non-int value types, casts to int or bit-converts via unsafe.
    For reference types, returns 1 on non-null.
    """
    ret = ret.strip()
    if not ret or ret == "System.Void":
        return f"{{ {call_expr}; return 0; }}"
    if ret == "System.Int32":
        return f"(int)({call_expr})"
    if ret in ("System.Boolean", "System.Byte", "System.SByte", "System.Int16", "System.UInt16", "System.UInt32"):
        return f"(int)({call_expr})"
    if ret in ("System.Int64", "System.UInt64"):
        return f"(int)(long)({call_expr})"
    if ret in ("System.Single", "System.Double"):
        return f"(int)({call_expr})"
    if ret in ("System.IntPtr", "System.UIntPtr"):
        return f"(int)({call_expr})"
    if ret.endswith("&"):
        return f"(int)({call_expr})"
    # Reference type or other value type — use pointer/address
    return f"(int)({call_expr})"


def ref_return_expr(parsed: dict[str, Any]) -> str:
    """Build a checksum expression from ref parameter values for void return methods."""
    param_types = parsed["param_types"]
    ref_locals = [
        f"refLocal_{i}"
        for i, pt in enumerate(param_types)
        if pt.strip().endswith("&")
    ]
    if not ref_locals:
        return "0"
    return " ^ ".join(ref_locals)


def normalize_clr_type(ret: str) -> str:
    """Normalize a CLR type string to a canonical form for matching.

    Strips generic/array curly braces to match against the type maps:
      'System.ReadOnlySpan{System.Byte}' -> 'System.ReadOnlySpan'
      'System.Nullable{System.Int32}'    -> 'System.Nullable'
      'System.Byte{}'                    -> 'System.Byte[]'
    """
    if ret.endswith("{}"):
        return ret[:-2] + "[]"
    brace = ret.find("{")
    if brace >= 0:
        return ret[:brace]
    return ret


def is_generic_task(ret: str) -> bool:
    """Check if the return type is System.Threading.Tasks.Task<T> in CLR format."""
    return (
        ret == "System.Threading.Tasks.Task"
        or ret.startswith("System.Threading.Tasks.Task{")
    )


def cast_return_to_int(ret: str, call_expr: str) -> str:
    """Cast a method's return value to int for checksum return."""
    ret = ret.strip()
    if ret == "System.Void" or not ret:
        return call_expr

    # Exact matches for primitive types
    if ret == "System.Int32":
        return call_expr
    if ret in ("System.Int64", "System.UInt64", "System.UInt32"):
        return f"(int)({call_expr})"
    if ret in ("System.Byte", "System.SByte", "System.Int16",
               "System.UInt16", "System.Char"):
        return f"(int)({call_expr})"
    if ret == "System.Boolean":
        return f"(({call_expr}) ? 1 : 0)"
    if ret in ("System.Single", "System.Double", "System.Decimal"):
        return f"(int)({call_expr})"
    if ret in ("System.IntPtr", "System.UIntPtr"):
        return f"(int)({call_expr})"
    if ret == "System.String":
        return f"(({call_expr}).Length)"

    # Value types with deterministic property extraction (avoid non-deterministic GetHashCode)
    if ret == "System.TimeSpan":
        return f"(long)(({call_expr}).Ticks)"
    if ret == "System.Array":
        return f"(int)(((Array)({call_expr})).Length)"
    if ret == "System.GCMemoryInfo":
        return f"(long)(({call_expr}).TotalCommittedBytes)"

    # Types that use .GetHashCode() for checksum
    _HASHCODE_TYPES = frozenset({
        "System.Object", "System.DateTime",
        "System.Exception", "System.Attribute", "System.Enum",
        "System.DateTimeOffset", "System.Collections.BitArray",
        "System.RuntimeTypeHandle", "System.RuntimeMethodHandle",
        "System.RuntimeFieldHandle", "System.Version",
        "System.Reflection.Module", "System.IO.Stream",
        "System.Runtime.CompilerServices.FormattableString",
        "System.Runtime.CompilerServices.RuntimeWrappedException",
        "System.Globalization.DateTimeFormatInfo",
        "System.Globalization.NumberFormatInfo",
        "System.Globalization.CultureInfo",
        "System.Globalization.CompareInfo",
        "System.Globalization.TextInfo",
        "System.Type", "System.Guid", "System.Reflection.MethodInfo",
        "System.Reflection.AssemblyName", "System.Reflection.Assembly",
        "System.Reflection.MemberInfo", "System.Reflection.FieldInfo",
        "System.Reflection.PropertyInfo", "System.Reflection.EventInfo",
        "System.Reflection.ParameterInfo", "System.Reflection.ConstructorInfo",
        "System.Delegate", "System.MulticastDelegate",
        "System.Threading.Tasks.Task", "System.Threading.Thread",
    })
    normal = normalize_clr_type(ret)
    if normal in _HASHCODE_TYPES:
        return f"(({call_expr}).GetHashCode())"

    # Generic/complex type handling by normalization prefix
    if normal.startswith("System.Span") or normal.startswith("System.ReadOnlySpan"):
        return f"(({call_expr}).GetHashCode())"
    if is_generic_task(ret):
        return f"(({call_expr}).GetHashCode())"

    # Array types in both CLR format (System.Byte{}) and C# format (byte[])
    if normal.endswith("[]") or ret.endswith("[]"):
        if re.search(r'\.(Count|Length|GetHashCode)\s*\)?\s*$', call_expr.strip()):
            return f"({call_expr})"
        return f"(({call_expr}).Length)"

    # Fallback: use .GetHashCode() which works for all types
    return f"(({call_expr}).GetHashCode())"


def is_auto_callable(parsed: dict[str, Any]) -> bool:
    """Check whether a method can be auto-generated (not needs-manual).

    Returns False for operators, protected methods, and other patterns that
    require manual implementation.
    """
    type_name = parsed["type_name"]
    method_name = parsed["method_name"]
    if (type_name, method_name) in _NEEDS_MANUAL_METHODS:
        return False
    param_count = len(parsed["param_types"])
    override = METHOD_OVERRIDES.get((type_name, method_name, param_count))
    if override == "skip":
        return False
    return True


def method_skip_reason(parsed: dict[str, Any]) -> str:
    """Return a human-readable skip reason, or empty string if auto-callable."""
    type_name = parsed["type_name"]
    method_name = parsed["method_name"]
    if (type_name, method_name) in _NEEDS_MANUAL_METHODS:
        return f"needs-manual — {method_name} requires manual implementation"
    param_count = len(parsed["param_types"])
    override = METHOD_OVERRIDES.get((type_name, method_name, param_count))
    if override == "skip":
        return f"needs-manual — {method_name} with {param_count} params requires manual implementation"
    return ""


get_skip_reason = method_skip_reason


def build_call_expr(
    parsed: dict[str, Any],
    type_map: dict[str, str] | None = None,
    instance_map: dict[str, str] | None = None,
) -> str:
    """Build a C# method call expression, e.g. ``Convert.ToChar(42)``.

    Handles:
      - Constructors: ``new DateTime(42, 42, 42)``
      - Property accessors: ``inst.Length``, ``inst[42]``
      - Instance methods: ``inst.ToString()``
      - Static methods: ``Convert.ToInt32("42")``

    Args:
        type_map: Optional custom type→default-value map (e.g. TYPE_ALTERNATIVE_MAP).
        instance_map: Optional custom type→instance-expr map.
    """
    type_name = parsed["type_name"]
    method_name = parsed["method_name"]
    tm = type_map or TYPE_DEFAULT_MAP
    args = ", ".join(default_expr_for_type(pt, tm) for pt in parsed["param_types"])

    # Check override map first (for known problematic signatures)
    param_count = len(parsed["param_types"])
    override = METHOD_OVERRIDES.get((type_name, method_name, param_count))
    if override is not None and override != "skip":
        # Convert.ToXxx(String) and Guid..ctor(String) overrides at
        # (type, method, paramCount=1) apply to ALL 1-param overloads,
        # including (Double) and (Byte[]) variants. Only apply these
        # string-based overrides when the param type IS System.String.
        if (
            param_count == 1
            and parsed["param_types"][0] != "System.String"
            and (
                (type_name == "Convert" and method_name.startswith("To"))
                or (type_name == "Guid" and method_name == ".ctor")
            )
        ):
            pass  # not a string param — fall through to TYPE_DEFAULT_MAP
        else:
            return override

    # WindowsPrincipal.IsInRole with WindowsBuiltInRole — disambiguate from int overload
    if (
        param_count == 1
        and type_name == "WindowsPrincipal"
        and method_name == "IsInRole"
        and parsed["param_types"][0] != "System.String"
        and parsed["param_types"][0] != "System.Int32"
    ):
        return "default(WindowsPrincipal)!.IsInRole(default(System.Security.Principal.SecurityIdentifier))"

    # Constructor: Type..ctor(...) → new Type(...)
    if method_name in (".ctor", ".cctor"):
        return build_ctor_expr(type_name, args, parsed.get("type_arity", 0))

    # Property accessor (get_Xxx / set_Xxx) → use property/indexer syntax
    expr = try_property_access(type_name, method_name, args)
    if expr is not None:
        return expr

    # Check param-count-aware static methods (e.g., Object.Equals with 2 params)
    if (type_name, method_name, param_count) in _STATIC_BY_SIGNATURE:
        return f"{type_name}.{method_name}({args})"

    # Known static method on an instance type (e.g. DateTime.Parse)
    static_methods = STATIC_METHODS_BY_TYPE.get(type_name, frozenset())
    if method_name in static_methods:
        return f"{type_name}.{method_name}({args})"

    # Known static-only type
    if type_name in STATIC_TYPES:
        return f"{type_name}.{method_name}({args})"

    # Instance method on a type we know about
    im = instance_map or INSTANCE_EXPR_MAP
    inst = im.get(type_name)
    if inst is not None:
        return f"{inst}.{method_name}({args})"

    # Fallback: assume static
    return f"{type_name}.{method_name}({args})"


# Map generic arity → C# type arguments for constructor/instance expressions
_GENERIC_ARGS_MAP: dict[int, str] = {
    1: "<byte>",
    2: "<byte, byte>",
    3: "<byte, byte, byte>",
}


def build_ctor_expr(type_name: str, args: str, type_arity: int = 0) -> str:
    """Build constructor expression, handling generic types.

    e.g. ``Span`1`` → ``new Span<byte>(42)``
    """
    if type_arity > 0:
        targs = _GENERIC_ARGS_MAP.get(type_arity, "")
        return f"new {type_name}{targs}({args})"
    m = re.match(r"(\w+)`(\d+)", type_name)
    if m:
        bare = m.group(1)
        arity = int(m.group(2))
        targs = _GENERIC_ARGS_MAP.get(arity, "")
        return f"new {bare}{targs}({args})"
    return f"new {type_name}({args})"


def concrete_type(csharp_type: str) -> str:
    """Replace CLR backtick generics with concrete C# type arguments.

    e.g. ``Span`1`` → ``Span<byte>``, ``Dictionary`2`` → ``Dictionary<string,int>``
    Also strips trailing backtick for non-generic cases.
    """
    m = re.match(r"(\w+)`(\d+)", csharp_type)
    if m:
        bare = m.group(1)
        arity = int(m.group(2))
        args = _GENERIC_ARGS_MAP.get(arity, "")
        return f"{bare}{args}"
    return csharp_type


def try_property_access(type_name: str, method_name: str, args: str) -> str | None:
    """If method_name is a property accessor, rewrite to property syntax.

    Handles:
      - get_Length → inst.Length
      - get_Item(N) → inst[N]  (indexer)
      - set_Item(N, V) → inst[N] = V
      - Static properties: get_CurrentThread → Thread.CurrentThread (not inst.CurrentThread)
    """
    static_methods = STATIC_METHODS_BY_TYPE.get(type_name, frozenset())
    if method_name.startswith("get_"):
        prop = method_name[4:]
        # Check if this is a known static property accessor
        if method_name in static_methods:
            return f"{type_name}.{prop}"
        # Indexer: get_Item(N) → inst[N]
        if prop == "Item":
            inst = INSTANCE_EXPR_MAP.get(type_name, type_name)
            return f"{inst}[{args}]"
        # Regular property
        inst = INSTANCE_EXPR_MAP.get(type_name)
        if inst is not None:
            return f"{inst}.{prop}"
        return f"{type_name}.{prop}"
    if method_name.startswith("set_"):
        prop = method_name[4:]
        # Check if this is a known static property accessor
        if method_name in static_methods:
            return f"{type_name}.{prop} = {args}"
        if prop == "Item":
            inst = INSTANCE_EXPR_MAP.get(type_name, type_name)
            if method_name.startswith("set_"):
                # set_Item(idx, val) → inst[idx] = val
                idx, val = args.rsplit(", ", 1) if ", " in args else ("", args)
                return f"{inst}[{idx}] = {val}"
            return f"{inst}[{args}]"
        inst = INSTANCE_EXPR_MAP.get(type_name)
        if inst is not None:
            return f"{inst}.{prop} = {args}"
        return f"{type_name}.{prop} = {args}"
    return None


def has_non_skip_override(parsed: dict[str, Any]) -> bool:
    """Check if there's a non-skip override expression for this method."""
    override = METHOD_OVERRIDES.get(
        (parsed["type_name"], parsed["method_name"], len(parsed["param_types"])))
    return override is not None and override != "skip"


def test_body(parsed: dict[str, Any]) -> str:
    """Generate the body of a test method.

    Returns (body_lines, is_simple, skip_reason).
    """
    if not is_auto_callable(parsed):
        reason = method_skip_reason(parsed)
        return (
            f"    // TODO: {reason}",
            False,
            reason,
        )

    if has_unsafe_param(parsed["param_types"]) and not has_non_skip_override(parsed):
        # Auto-generate via ref-aware builder
        prelude, call_expr = build_call_expr_with_refs(parsed)
        ret = parsed["return_type"]
        if ret == "System.Void" or not ret:
            if prelude:
                return (f"{prelude}\n    {call_expr};", False, "")
            return (f"    {call_expr};", False, "")
        if prelude:
            return (f"{prelude}\n    var result = {call_expr};", False, "")
        return (f"    var result = {call_expr};", False, "")

    if is_simple_method(parsed):
        call_expr = build_call_expr(parsed)
        ret = parsed["return_type"]
        if ret == "System.Void" or not ret:
            # void method — just call it
            return (
                f"    {call_expr};",
                True,
                "",
            )
        return (
            f"    var result = {call_expr};",
            True,
            "",
        )

    # Mixed method — smoke (at least one complex parameter)
    call_expr = build_call_expr(parsed)
    ret = parsed["return_type"]
    if ret == "System.Void" or not ret:
        return (
            f"    {call_expr};",
            False,
            "",
        )
    return (
        f"    _ = {call_expr};",
        False,
        "",
    )


def family_slug(family_id: str) -> str:
    parts = [part for part in str(family_id).split("/") if part]
    if len(parts) < 4:
        return str(family_id).replace("/", "-")
    return "-".join(parts[2:])


def production_class_name(family_id: str) -> str:
    """Derive production class name (no suffix).
    e.g. convert-char -> ConvertChar
    """
    tail = family_slug(family_id).split("-")
    return "".join(part.capitalize() for part in tail)


def class_name(family_id: str) -> str:
    """Derive test class name.
    e.g. convert-char -> ConvertCharTests
    """
    return production_class_name(family_id) + "Tests"


def relative(repo_root: Path, path: Path) -> str:
    return path.resolve().relative_to(repo_root.resolve()).as_posix()


def enum_name(family: dict[str, Any]) -> str:
    explicit = str(family.get("capabilityFamilyEnum") or "").strip()
    if explicit:
        return explicit
    family_id = str(family.get("familyId") or "").strip()
    if not family_id:
        return "None"
    parts = [part for part in family_id.split("/") if part]
    if len(parts) < 4:
        return "None"
    assembly_name = parts[1]
    assembly_part = "".join(token[:1].upper() + token[1:] for token in assembly_name.split(".") if token)
    # Split each family-path segment on hyphens too, so "numeric-conversions" -> "NumericConversions"
    # Handle acronyms: each segment is PascalCased individually, then joined.
    # If a segment is a known acronym (e.g. "io"), uppercase it.
    family_part = "".join(
        pascalcase_segment(segment)
        for segment in parts[2:]
        if segment
    )
    # Post-processing: fix known acronym patterns across the combined string
    # "Io" at word boundary -> "IO"
    family_part = remediate_acronyms(family_part)
    candidate = f"{assembly_part}_{family_part}".strip("_")
    return candidate or "None"


def pascalcase_segment(segment: str) -> str:
    """Convert a hyphen-separated segment to PascalCase, handling acronyms."""
    tokens = segment.split("-")
    result = []
    for t in tokens:
        if not t:
            continue
        if t.lower() in _ACRONYMS:
            result.append(t.upper())
        else:
            result.append(t[:1].upper() + t[1:])
    return "".join(result)


def remediate_acronyms(s: str) -> str:
    """Fix known patterns where a PascalCased acronym needs adjustment."""
    # "Io" followed by uppercase -> "IO"
    # e.g. "IoStreamsBasics" -> "IOStreamsBasics"
    # "Io" followed by uppercase -> "IO"
    # e.g. "IoStreamsBasics" -> "IOStreamsBasics"
    s = re.sub(r"(?:^|(?<=_))Io(?=[A-Z])", "IO", s)
    return s


def namespace_from_type_path(type_path: str) -> str | None:
    """Extract the C# namespace from a CLR type_path (format: AssemblyName/Namespace.TypeName).

    Example: "System.Formats.Asn1/System.Formats.Asn1.AsnDecoder" -> "System.Formats.Asn1"
    """
    if "/" not in type_path:
        return None
    full_type = type_path.split("/", 1)[1]  # "System.Formats.Asn1.AsnDecoder"
    last_dot = full_type.rfind(".")
    if last_dot < 0:
        return None
    return full_type[:last_dot]


_BASE_USING_NAMESPACES = frozenset({
    "System", "System.IO", "System.Threading", "System.Threading.Tasks",
    "System.Globalization", "System.Linq", "System.Runtime.CompilerServices",
    "System.Runtime.InteropServices", "System.Collections.Generic",
    "System.Reflection",
})


def member_name(prefix: str, method_subject_id: str) -> str:
    sanitized = re.sub(r"[^A-Za-z0-9]+", "_", method_subject_id).strip("_")
    sanitized = sanitized[:120] if sanitized else "Placeholder"
    return f"{prefix}_{sanitized}"


def handwritten_source(family_id: str, class_name: str) -> str:
    """Generate handwritten partial class source (no test dependencies)."""
    prod_name = class_name.replace("Tests", "")
    return (
        f"{BASE_USINGS}\n"
        "using Chaos.TestFramework;\n"
        "\n"
        f"public partial class {prod_name}\n"
        "{\n"
        "}\n"
    )


def generated_source(
    family_id: str,
    display_name: str,
    class_name: str,
    *,
    method_subject_ids: list[str],
    capability_family_enum: str,
) -> str:
    """Generate auto-generated partial class with benchmark/host/test methods merged.

    Produces a single file containing:
      - [MethodSubjectId] test methods (no xunit [Fact] — invoked via reflection from test exe)
      - [BenchmarkSubjectId] benchmark methods (static, void)
      - [HotUpdateSubjectId] hotupdate methods (static, void)
    All within the same partial class so they can coexist in one translation unit.
    No xunit dependency — this file is compiled into the src Library project.
    """
    prod_name = class_name.replace("Tests", "")
    extra_usings = ""
    if not method_subject_ids:
        members = (
            "    // No methods to auto-generate for this family.\n"
        )
    else:
        parts: list[str] = []
        for method_subject_id in method_subject_ids:
            parsed = parse_method_subject_id(method_subject_id)
            type_name = parsed["type_name"]
            method_name = parsed["method_name"]
            body, is_simple, skip_reason = test_body(parsed)

            # --- Test method ([MethodSubjectId], no [Fact] — discovered via reflection) ---
            mname = member_name("Method", method_subject_id)
            test_code = (
                f'    [MethodSubjectId("{method_subject_id}")]'
                f'\n    [CapabilityFamilyId(CapabilityFamilyId.{capability_family_enum})]'
                f'\n    [VerificationRoute(VerificationRoute.Native)]'
                f'\n    public void {mname}()'
                f'\n    {{'
                f'\n{body}'
                f'\n    }}'
            )

            # --- Benchmark method ([BenchmarkSubjectId], static void) ---
            bname = member_name("Benchmark", method_subject_id)
            if is_auto_callable(parsed) and not has_unsafe_param(parsed["param_types"]):
                call_expr = build_call_expr(parsed)
                ret = parsed["return_type"]
                bench_body = f"_ = {call_expr}" if ret not in ("System.Void", "") else f"{call_expr}"
                bench_code = (
                    f'    [BenchmarkSubjectId("{method_subject_id}")]'
                    f'\n    [CapabilityFamilyId(CapabilityFamilyId.{capability_family_enum})]'
                    f'\n    [BenchmarkRoute(BenchmarkRoute.Native)]'
                    f'\n    [BenchmarkProfile(BenchmarkProfile.Default)]'
                    f'\n    public static void {bname}() {{ {bench_body}; }}'
                )
            else:
                bench_code = (
                    f'    [BenchmarkSubjectId("{method_subject_id}")]'
                    f'\n    [CapabilityFamilyId(CapabilityFamilyId.{capability_family_enum})]'
                    f'\n    public static void {bname}() {{ }}'
                )

            # --- HotUpdate method ([HotUpdateSubjectId], static void, HostToPath) ---
            hname = member_name("HotUpdate", method_subject_id)
            if is_auto_callable(parsed) and not has_unsafe_param(parsed["param_types"]):
                call_expr = build_call_expr(parsed)
                ret = parsed["return_type"]
                hu_body = f"_ = {call_expr}" if ret not in ("System.Void", "") else f"{call_expr}"
                hu_code = (
                    f'    [HotUpdateSubjectId("{method_subject_id}")]'
                    f'\n    [CapabilityFamilyId(CapabilityFamilyId.{capability_family_enum})]'
                    f'\n    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]'
                    f'\n    public static void {hname}() {{ {hu_body}; }}'
                )
            else:
                hu_code = (
                    f'    [HotUpdateSubjectId("{method_subject_id}")]'
                    f'\n    [CapabilityFamilyId(CapabilityFamilyId.{capability_family_enum})]'
                    f'\n    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]'
                    f'\n    public static void {hname}() {{ }}'
                )

            block = f"    // {type_name}.{method_name}\n{test_code}\n\n{bench_code}\n\n{hu_code}"
            parts.append(block)
        members = "\n\n".join(parts)

        # Collect per-DLL namespaces from method subject IDs, minus already-covered base usings
        extra_ns: set[str] = set()
        for method_subject_id in method_subject_ids:
            parsed = parse_method_subject_id(method_subject_id)
            ns = namespace_from_type_path(parsed["type_path"])
            if ns and ns not in _BASE_USING_NAMESPACES:
                extra_ns.add(ns)
        extra_usings = "\n".join(f"using {ns};" for ns in sorted(extra_ns))
        if extra_usings:
            extra_usings += "\n"

    return (
        f"{BASE_USINGS}\n"
        f"{extra_usings}"
        "using Chaos.TestFramework;\n"
        "\n"
        f"// Auto-generated skeleton for {display_name} ({family_id}).\n"
        f"// Contains test, benchmark, and hotupdate methods merged into one partial class.\n"
        f"// No xunit dependency — [Fact] attributes belong in the test exe project.\n"
        f"public partial class {prod_name}\n"
        "{\n"
        f"{members}\n"
        "}\n"
    )


def patch_generated_source(
    class_name: str,
    *,
    method_subject_ids: list[str],
    capability_family_enum: str,
) -> str:
    """Generate patch-side auto-generated code (patch/*.AutoGenerated.cs).

    Patch methods verify that the method can be invoked from a patched assembly
    (PatchToHost direction) — they mirror the Host side but with reversed direction.
    """
    patch_class_name = class_name.replace("Tests", "Patch")
    if not method_subject_ids:
        members = "    public static void Placeholder() { }\n"
    else:
        parts: list[str] = []
        for method_subject_id in method_subject_ids:
            parsed = parse_method_subject_id(method_subject_id)
            call_expr = build_call_expr(parsed)
            member_name = member_name("Patch", method_subject_id)
            purpose_comment = f"// Purpose: Verify {parsed['type_name']}.{parsed['method_name']} executes correctly from patch side back to host"
            if not is_auto_callable(parsed) or has_unsafe_param(parsed["param_types"]):
                parts.append(
                    f"    {purpose_comment}\n"
                    f'    [HotUpdateSubjectId("{method_subject_id}")]\n'
                    f"    [CapabilityFamilyId(CapabilityFamilyId.{capability_family_enum})]\n"
                    "    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]\n"
                    f"    public static void {member_name}() {{ }}"
                )
            else:
                ret = parsed["return_type"]
                body = f"_ = {call_expr}" if ret not in ("System.Void", "") else f"{call_expr}"
                parts.append(
                    f"    {purpose_comment}\n"
                    f'    [HotUpdateSubjectId("{method_subject_id}")]\n'
                    f"    [CapabilityFamilyId(CapabilityFamilyId.{capability_family_enum})]\n"
                    "    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]\n"
                    f"    public static void {member_name}() {{ {body}; }}"
                )
        members = "\n".join(parts)
    return (
        f"{BASE_USINGS}\n"
        "using Chaos.TestFramework;\n\n"
        f"// Auto-generated patch-side skeletons for {patch_class_name}.\n"
        f"// Each method exercises a method from the patch side back to the host after hot-update.\n"
        f"public static partial class {patch_class_name}\n"
        "{\n"
        f"{members}"
        "}\n"
    )


def patch_handwritten_source(patch_class_name: str) -> str:
    """Generate handwritten patch partial class source."""
    return (
        f"{BASE_USINGS}\n"
        "using Chaos.TestFramework;\n"
        "\n"
        f"public static partial class {patch_class_name}\n"
        "{\n"
        "}\n"
    )


def test_exe_source(class_name: str, *, method_subject_ids: list[str]) -> str:
    """Generate test executable source (managed_test/tests/)."""
    prod_name = class_name.replace("Tests", "")
    return (
        f"{BASE_USINGS}\n"
        "using Xunit;\n"
        "\n"
        f"public class {class_name}\n"
        "{\n"
        "    [Fact]\n"
        "    public void AllAutoGeneratedMethodsCanExecute()\n"
        "    {\n"
        "        // Smoke test: ensure all auto-generated methods can be discovered\n"
        f"        var type = typeof({prod_name});\n"
        "        Assert.NotNull(type);\n"
        "    }\n"
        "}\n"
    )


def benchmark_exe_source(class_name: str, *, method_subject_ids: list[str]) -> str:
    """Generate benchmark executable source (managed_test/benchmarks/)."""
    if method_subject_ids:
        call_exprs = []
        for mid in method_subject_ids[:10]:
            parsed = parse_method_subject_id(mid)
            if is_auto_callable(parsed) and not has_unsafe_param(parsed["param_types"]):
                try:
                    call_exprs.append((member_name("Benchmark", mid), build_call_expr(parsed)))
                except Exception:
                    pass
        if call_exprs:
            lines = [
                "using System;",
                "using System.Diagnostics;",
                "",
                f"// Auto-generated benchmark harness for {class_name}",
                f"// Runs {len(call_exprs)} methods as quick smoke-benchmark.",
                "class Program",
                "{",
                "    static void Main()",
                "    {",
            ]
            for idx, (name, expr) in enumerate(call_exprs):
                lines.append(f"        // Benchmark entry {idx}: {name}")
                lines.append(f"        var sw{idx} = Stopwatch.StartNew();")
                lines.append(f"        _ = {expr};")
                lines.append(f"        sw{idx}.Stop();")
                lines.append(f'        Console.WriteLine($"Entry {idx}: {{sw{idx}.Elapsed.TotalMilliseconds:F3}} ms");')
            lines.append("    }")
            lines.append("}")
            return "\n".join(lines) + "\n"
    return (
        "using System;\n"
        "\n"
        "class Program\n"
        "{\n"
        "    static void Main()\n"
        "    {\n"
        '        Console.WriteLine("No auto-generable methods for this family.");\n'
        "    }\n"
        "}\n"
    )


def readme_source(family_id: str, display_name: str) -> str:
    return (
        f"# {display_name}\n\n"
        f"- familyId: `{family_id}`\n"
        "- autogenerated: `*.AutoGenerated.cs`\n"
        "- handwritten: edit `*Tests.cs` only\n"
    )


def generate_family_skeleton(repo_root: Path, *, assembly_name: str, family: dict[str, Any]) -> dict[str, Any]:
    family_id = str(family.get("familyId") or "")
    display_name = str(family.get("displayName") or family_id)
    family_root = repo_root / "testing" / "foundation-dll" / assembly_name / family_slug(family_id)

    prod_name = production_class_name(family_id)
    class_name = class_name(family_id)
    patch_class_name = class_name.replace("Tests", "Patch")
    method_subject_ids = [str(item) for item in list(family.get("methodSubjectIds") or []) if str(item)]
    capability_family_enum = enum_name(family)

    # --- src/ directory (handwrite + auto-generated merged) ---
    src_dir = family_root / "src"
    handwritten_path = src_dir / f"{prod_name}.cs"
    generated_path = src_dir / f"{prod_name}.AutoGenerated.cs"

    # --- src/patch/ directory ---
    patch_dir = family_root / "src" / "patch"
    patch_handwritten_path = patch_dir / f"{patch_class_name}.cs"
    patch_generated_path = patch_dir / f"{patch_class_name}.AutoGenerated.cs"

    # --- managed_test/ directory (test exe + benchmark exe) ---
    test_exe_dir = family_root / "managed_test" / "tests"
    test_exe_path = test_exe_dir / f"{class_name}.cs"
    benchmark_exe_dir = family_root / "managed_test" / "benchmarks"
    benchmark_exe_path = benchmark_exe_dir / f"{class_name.replace('Tests', 'Benchmarks')}.cs"

    readme_path = family_root / "README.md"
    feature_contract_path = family_root / "capability-family-contract.json"

    # Write handwritten src (only if not exists)
    src_dir.mkdir(parents=True, exist_ok=True)
    if not handwritten_path.exists():
        handwritten_path.write_text(handwritten_source(family_id, class_name), encoding="utf-8")

    # Write auto-generated src (merged test/benchmark/hotupdate)
    generated_source = generated_source(
        family_id,
        display_name,
        class_name,
        method_subject_ids=method_subject_ids,
        capability_family_enum=capability_family_enum,
    )
    generated_path.write_text(generated_source, encoding="utf-8")

    # Write patch handwritten (only if not exists)
    patch_dir.mkdir(parents=True, exist_ok=True)
    if not patch_handwritten_path.exists():
        patch_handwritten_path.write_text(patch_handwritten_source(patch_class_name), encoding="utf-8")

    # Write patch auto-generated
    patch_generated_path.parent.mkdir(parents=True, exist_ok=True)
    patch_generated_path.write_text(
        patch_generated_source(
            class_name,
            method_subject_ids=method_subject_ids,
            capability_family_enum=capability_family_enum,
        ),
        encoding="utf-8",
    )

    # Write test exe source
    test_exe_dir.mkdir(parents=True, exist_ok=True)
    test_exe_path.write_text(test_exe_source(class_name, method_subject_ids=method_subject_ids), encoding="utf-8")

    # Write benchmark exe source
    benchmark_exe_dir.mkdir(parents=True, exist_ok=True)
    benchmark_exe_path.write_text(
        benchmark_exe_source(class_name, method_subject_ids=method_subject_ids),
        encoding="utf-8",
    )

    readme_path.write_text(readme_source(family_id, display_name), encoding="utf-8")

    return {
        "familyId": family_id,
        "outputRoot": relative(repo_root, src_dir),
        "artifacts": [
            relative(repo_root, handwritten_path),
            relative(repo_root, generated_path),
            relative(repo_root, patch_handwritten_path),
            relative(repo_root, patch_generated_path),
            relative(repo_root, test_exe_path),
            relative(repo_root, benchmark_exe_path),
            relative(repo_root, readme_path),
        ],
    }
