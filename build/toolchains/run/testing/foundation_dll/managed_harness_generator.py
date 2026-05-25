"""Shared managed harness generator — methodSubjectId to C# call expression translator.

Extracted from family_verification_orchestrator.py's auto_generate_managed_benchmark()
to be reused by both benchmark and fact harness generators.

Usage:
    from managed_harness_generator import generate_call_expr, extract_param_types
    expr, always_throws = generate_call_expr(method_subject_id, idx)
"""

from __future__ import annotations

import re
from typing import Callable

__all__ = [
    "generate_call_expr",
    "extract_param_types",
    "return_type_from_mid",
    "CallExprResult",
    "CallExprMode",
]

CallExprResult = tuple[str, bool]  # (call_expression, always_throws)

# ── Regex: extract param list from methodSubjectId ──────────────────────────
_PARAM_RE = re.compile(r'\(([^)]+)\)')

# ── ToXxx(string) literal table ────────────────────────────────────────────
_TOXXX_STRING_LITERALS: dict[str, Callable[[str], str]] = {
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

# ── Always-throwing param types for Convert.ToChar ─────────────────────────
_TOCHAR_ALWAYS_THROWS = {"System.Boolean", "System.DateTime", "System.Decimal",
                         "System.Double", "System.Single"}

# ── Trivial types (cast-only, no range check, risk of JIT elision) ─────────
TRIVIAL_TYPES = {"System.Byte", "System.Char", "System.Int16", "System.Int32",
                 "System.Int64", "System.SByte", "System.UInt16", "System.UInt32",
                 "System.UInt64", "System.Object"}


def extract_param_types(mid: str) -> list[str]:
    """Extract parameter type list from a methodSubjectId string.

    Format: "Assembly/Type::Method:ReturnType(Type1,Type2,...)"
    Returns empty list if no params found.
    """
    m = _PARAM_RE.search(mid)
    if not m:
        return []
    raw = m.group(1)
    parts: list[str] = []
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


def return_type_from_mid(mid: str) -> str:
    """Extract return type from a methodSubjectId.

    Format: Assembly/Type::Method:ReturnType(Params)
    """
    m = re.match(r'[^/]+/[^:]+::[^:]+:([^(]+)\(', mid)
    if m:
        return m.group(1).strip()
    return ''


class CallExprMode:
    """Mode flags for generate_call_expr."""
    BENCHMARK = "benchmark"  # Uses loop variable 'i' for input variation
    FACT = "fact"            # Uses deterministic literal values


def generate_call_expr(mid: str, idx: int = 0, mode: str = CallExprMode.BENCHMARK) -> CallExprResult:
    """Generate a C# call expression from a methodSubjectId.

    Returns (call_expression, always_throws_bool).

    Supports patterns:
      - Convert.ToXxx(string)     -> Convert.ToXxx("literal")
      - Convert.ToXxx(nonstring)  -> Convert.ToXxx(variable)
      - Convert.ToString(xxx)     -> Convert.ToString(variable)
      - Xxx.Parse(string)         -> Xxx.Parse("literal")
      - System.Array.* methods
      - System.Guid.* methods
      - System.Random.* methods
      - System.Enum.* methods
      - System.Threading.* methods
      - System.Collections.Generic.* methods
    """
    ipart = f'(i + {idx})' if idx > 0 and mode == CallExprMode.BENCHMARK else 'i'
    if mode == CallExprMode.FACT:
        ipart = str(idx)  # Use constant inputs for fact mode

    m = re.match(r'[^/]+/([^:]+)::([^:]+):[^(]+\(([^)]*)\)', mid)
    if not m:
        return '', False
    declaring_type = m.group(1)
    method_name = m.group(2)
    param_str = m.group(3)
    param_types = [p.strip() for p in param_str.split(',') if p.strip()]

    # ── Xxx.Parse(string) pattern ────────────────────────────────────────
    if method_name == 'Parse' and param_types == ['System.String']:
        parse_tbl = {
            'System.Double': f'Double.Parse("3.14159")',
            'System.Int32': f'Int32.Parse((({ipart}) % 100000 + 1).ToString())',
            'System.Int64': f'Int64.Parse((({ipart}) % 100000 + 1).ToString())',
        }
        if declaring_type in parse_tbl:
            return parse_tbl[declaring_type], False
        return '', False

    # ── Convert.ToString(xxx) pattern ────────────────────────────────────
    if declaring_type == 'System.Convert' and method_name == 'ToString' and len(param_types) == 1:
        t = param_types[0]
        if 'Int32' in t:
            return f'Convert.ToString((int)({ipart} & 0xFF))', False
        elif 'Int64' in t:
            return f'Convert.ToString((long)({ipart} & 0xFF))', False
        elif 'Double' in t:
            return f'Convert.ToString((double)({ipart} & 0xFF))', False
        elif 'Single' in t:
            return f'Convert.ToString((float)({ipart} & 0xFF))', False
        return '', False

    # ── Convert.ToXxx(string) — use string literal ───────────────────────
    if declaring_type == 'System.Convert' and method_name.startswith('To') and param_types == ['System.String']:
        rt = method_name[2:]
        if rt in _TOXXX_STRING_LITERALS:
            return _TOXXX_STRING_LITERALS[rt](ipart), False
        return '', False

    # ── Convert.ToXxx(non-string, 1 param) — use variable ────────────────
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
        return '', False

    # ── Collection generic type patterns ─────────────────────────────────
    coll_match = re.match(r'System\.Collections\.Generic\.(\w+)`\d+', declaring_type)
    if coll_match:
        coll_type = coll_match.group(1)
        if coll_type == 'List':
            if method_name == 'Add':
                return f'new System.Collections.Generic.List<int>().Add({ipart})', False
            elif method_name == 'Clear':
                return f'new System.Collections.Generic.List<int>{{{ipart}}}.Clear()', False
            elif method_name == 'Contains':
                return f'new System.Collections.Generic.List<int>{{{ipart}}}.Contains({ipart})', False
            elif method_name == 'IndexOf':
                return f'new System.Collections.Generic.List<int>{{{ipart}}}.IndexOf({ipart})', False
            elif method_name == 'Remove':
                return f'new System.Collections.Generic.List<int>{{{ipart}}}.Remove({ipart})', False
            elif method_name == 'RemoveAt':
                return f'new System.Collections.Generic.List<int>{{{ipart}}}.RemoveAt(0)', False
            elif method_name == 'Sort':
                return f'new System.Collections.Generic.List<int>{{3, 1, 2}}.Sort()', False
            elif method_name == 'ToArray':
                return f'new System.Collections.Generic.List<int>{{{ipart}}}.ToArray()', False
        elif coll_type == 'Dictionary':
            if method_name == 'Add':
                return f'new System.Collections.Generic.Dictionary<int, int>().Add({ipart}, {ipart})', False
            elif method_name == 'get_Count':
                return f'_ = new System.Collections.Generic.Dictionary<int, int>{{{{{ipart}, {ipart}}}}}.Count', False
            elif method_name == 'TryGetValue':
                return f'new System.Collections.Generic.Dictionary<int, int>{{{{{ipart}, {ipart}}}}}.TryGetValue({ipart}, out _)', False
            elif method_name == 'ContainsKey':
                return f'new System.Collections.Generic.Dictionary<int, int>{{{{{ipart}, {ipart}}}}}.ContainsKey({ipart})', False
            elif method_name == 'Remove':
                return f'new System.Collections.Generic.Dictionary<int, int>{{{{{ipart}, {ipart}}}}}.Remove({ipart})', False
        elif coll_type == 'HashSet':
            if method_name == 'Add':
                return f'new System.Collections.Generic.HashSet<int>().Add({ipart})', False
            elif method_name == 'Contains':
                return f'new System.Collections.Generic.HashSet<int>{{{ipart}}}.Contains({ipart})', False
            elif method_name == 'Remove':
                return f'new System.Collections.Generic.HashSet<int>{{{ipart}}}.Remove({ipart})', False
        return '', False

    # ── System.Array methods ─────────────────────────────────────────────
    if declaring_type == 'System.Array':
        if method_name == 'Copy' and len(param_types) == 3:
            return 'System.Array.Copy(new byte[]{1,2,3,4,5}, new byte[5], 3)', False
        elif method_name == 'Copy' and len(param_types) == 5:
            return 'System.Array.Copy(new byte[]{1,2,3,4,5}, 1, new byte[3], 0, 3)', False
        elif method_name == 'Clear':
            return 'System.Array.Clear(new byte[]{1,2,3,4,5}, 0, 3)', False
        elif method_name == 'Sort' and len(param_types) == 1:
            return 'System.Array.Sort(new byte[]{3,1,4,1,5})', False
        elif method_name == 'Sort' and len(param_types) == 2:
            return 'System.Array.Sort(new byte[]{3,1,2}, (System.Collections.IComparer)null)', False
        elif method_name == 'BinarySearch' and len(param_types) == 2:
            return 'System.Array.BinarySearch(new byte[]{10,20,30,40}, (object)(byte)30)', False
        elif method_name == 'BinarySearch' and len(param_types) == 4:
            return 'System.Array.BinarySearch(new byte[]{10,20,30,40}, 0, 3, (object)(byte)20)', False
        elif method_name == 'IndexOf':
            return 'System.Array.IndexOf(new byte[]{5,3,5,3}, (object)(byte)3)', False
        elif method_name == 'LastIndexOf':
            return 'System.Array.LastIndexOf(new byte[]{5,3,5,3}, (object)(byte)3)', False
        elif method_name == 'Reverse':
            return 'System.Array.Reverse(new byte[]{1,2,3,4,5})', False
        elif method_name == 'GetLength':
            return 'System.Array.CreateInstance(typeof(byte), 3).GetLength(0)', False
        elif method_name == 'GetValue':
            return 'new byte[]{10,20,30}.GetValue(0)', False
        return '', False

    # ── System.Guid methods ──────────────────────────────────────────────
    if declaring_type == 'System.Guid':
        if method_name == 'NewGuid' and len(param_types) == 0:
            return 'Guid.NewGuid()', False
        elif method_name == 'Parse' and param_types == ['System.String']:
            return 'Guid.Parse("00000000-0000-0000-0000-000000000000")', False
        elif method_name == 'GetHashCode' and len(param_types) == 0:
            return 'Guid.NewGuid().GetHashCode()', False
        elif method_name == 'ToString' and len(param_types) == 0:
            return 'Guid.NewGuid().ToString()', False
        elif method_name == '.ctor' and param_types == ['System.String']:
            return 'new Guid("00000000-0000-0000-0000-000000000000")', False
        return '', False

    # ── System.Random methods ────────────────────────────────────────────
    if declaring_type == 'System.Random':
        if method_name == '.ctor' and len(param_types) == 0:
            return 'new Random()', False
        elif method_name == 'Next' and len(param_types) == 0:
            return 'new Random().Next()', False
        elif method_name == 'Next' and param_types == ['System.Int32']:
            return f'new Random().Next({ipart})', False
        elif method_name == 'NextBytes' and param_types == ['System.Byte[]']:
            return 'new Random().NextBytes(new byte[16])', False
        elif method_name == 'NextDouble' and len(param_types) == 0:
            return 'new Random().NextDouble()', False
        return '', False

    # ── System.HashCode methods ──────────────────────────────────────────
    if declaring_type == 'System.HashCode':
        if method_name == 'ToHashCode' and len(param_types) == 0:
            return 'default(HashCode).ToHashCode()', False
        elif method_name.startswith('Combine') and len(param_types) == 2:
            return f'HashCode.Combine({ipart}, {ipart})', False
        return '', False

    # ── System.Threading.Thread methods ───────────────────────────────────
    if declaring_type == 'System.Threading.Thread':
        if method_name == 'get_CurrentThread' and len(param_types) == 0:
            return '_ = System.Threading.Thread.CurrentThread.GetHashCode()', False
        elif method_name == 'get_ManagedThreadId' and len(param_types) == 0:
            return '_ = System.Threading.Thread.CurrentThread.ManagedThreadId', False
        elif method_name == 'Sleep' and param_types == ['System.Int32']:
            return 'System.Threading.Thread.Sleep(0)', False
        elif method_name == 'Start' and len(param_types) == 0:
            return 'new System.Threading.Thread(() => {}).Start()', False
        return '', False

    # ── System.Threading.Tasks.Task methods ──────────────────────────────
    if declaring_type == 'System.Threading.Tasks.Task':
        if method_name == 'get_IsCompleted' and len(param_types) == 0:
            return '_ = System.Threading.Tasks.Task.CompletedTask.IsCompleted', False
        elif method_name == 'get_Status' and len(param_types) == 0:
            return '_ = (int)System.Threading.Tasks.Task.CompletedTask.Status', False
        elif method_name == 'Run' and param_types == ['System.Action']:
            return 'System.Threading.Tasks.Task.Run(() => { _g++; })', False
        elif method_name == 'Run' and param_types == ['System.Func`1']:
            return 'System.Threading.Tasks.Task.FromResult(42)', False
        elif method_name == 'Delay' and param_types == ['System.Int32']:
            return 'System.Threading.Tasks.Task.Delay(0).Wait()', False
        elif method_name == 'Wait' and len(param_types) == 0:
            return 'System.Threading.Tasks.Task.FromResult(42).Wait()', False
        elif method_name == 'Wait' and param_types == ['System.Boolean', 'System.Int32']:
            return 'System.Threading.Tasks.Task.FromResult(42).Wait(true, System.Threading.Timeout.Infinite)', False
        elif method_name == 'ContinueWith' and param_types == ['System.Action`1']:
            return 'System.Threading.Tasks.Task.CompletedTask.ContinueWith(_ => { _g++; })', False
        elif method_name == 'WhenAll' and param_types == ['System.Threading.Tasks.Task[]']:
            return 'System.Threading.Tasks.Task.WhenAll(System.Threading.Tasks.Task.CompletedTask)', False
        elif method_name == 'WhenAny' and param_types == ['System.Threading.Tasks.Task[]']:
            return '_ = System.Threading.Tasks.Task.WhenAny(System.Threading.Tasks.Task.CompletedTask)', False
        elif method_name == 'FromResult':
            return '_ = System.Threading.Tasks.Task.FromResult(42)', False
        return '', False

    # ── System.Enum methods ──────────────────────────────────────────────
    if declaring_type == 'System.Enum':
        if method_name == 'Format' and param_types == ['System.Type', 'System.Object', 'System.String']:
            return 'System.Enum.Format(typeof(System.StringComparison), System.StringComparison.Ordinal, "G")', False
        elif method_name == 'GetName' and param_types == ['System.RuntimeType', 'System.UInt64']:
            return 'System.Enum.GetName(typeof(System.StringComparison), System.StringComparison.Ordinal)', False
        elif method_name == 'GetName' and param_types == ['System.Type', 'System.Object']:
            return 'System.Enum.GetName(typeof(System.StringComparison), System.StringComparison.Ordinal)', False
        elif method_name == 'GetNames' and param_types == ['System.Type']:
            return '_ = System.Enum.GetNames(typeof(System.StringComparison))', False
        elif method_name == 'GetValues' and param_types == ['System.Type']:
            return '_ = System.Enum.GetValues(typeof(System.StringComparison))', False
        elif method_name == 'IsDefined' and param_types == ['System.Type', 'System.Object']:
            return 'System.Enum.IsDefined(typeof(System.StringComparison), System.StringComparison.Ordinal)', False
        elif method_name == 'Parse' and param_types == ['System.Type', 'System.String']:
            return 'System.Enum.Parse(typeof(System.StringComparison), "Ordinal")', False
        elif method_name == 'Parse' and param_types == ['System.Type', 'System.String', 'System.Boolean']:
            return 'System.Enum.Parse(typeof(System.StringComparison), "Ordinal", true)', False
        elif method_name == 'ToString' and len(param_types) == 0:
            return 'System.StringComparison.Ordinal.ToString()', False
        elif method_name == 'ToString' and param_types == ['System.String']:
            return 'System.StringComparison.Ordinal.ToString("G")', False
        elif method_name == 'TryParse' and param_types == ['System.Type', 'System.String', 'System.Boolean', 'System.Object&']:
            return 'System.Enum.TryParse(typeof(System.StringComparison), "Ordinal", true, out _)', False
        elif method_name == 'TryParse' and param_types == ['System.Type', 'System.String', 'System.Object&']:
            return 'System.Enum.TryParse(typeof(System.StringComparison), "Ordinal", out _)', False
        return '', False

    return '', False
