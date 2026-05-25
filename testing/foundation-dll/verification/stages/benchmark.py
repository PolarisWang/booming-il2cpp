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
    "mono":      {"runner": "mono",       "tfm": "net8.0"},
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


def _mono_available() -> bool:
    return shutil.which("mono") is not None


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


def _generate_call_expr(mid: str, idx: int) -> tuple[str, bool]:
    """Generate a C# call expression for benchmarking by parsing methodSubjectId.

    Returns (call_expression, always_throws_bool).
    """
    ipart = f'(i + {idx})' if idx > 0 else 'i'

    m = re.match(r'[^/]+/([^:]+)::([^:]+):[^(]+\(([^)]*)\)', mid)
    if not m:
        return '', False
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
        return '', False

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
        return '', False

    # Convert.ToXxx(string)
    if declaring_type == 'System.Convert' and method_name.startswith('To') and param_types == ['System.String']:
        rt = method_name[2:]
        if rt in _TOXXX_STRING_LITERALS:
            return _TOXXX_STRING_LITERALS[rt](ipart), False
        return '', False

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
        return '', False

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
                'RemoveAt': 'new System.Collections.Generic.List<int>{{{ipart}}}.RemoveAt(0)',
                'Sort': 'new System.Collections.Generic.List<int>{{3, 1, 2}}.Sort()',
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
        return '', False

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
        }
        if method_name in tbl:
            entry = tbl[method_name]
            if isinstance(entry, dict):
                if len(param_types) in entry:
                    return entry[len(param_types)], False
            else:
                return entry, False
        return '', False

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
        return '', False

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
        return '', False

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
        return '', False

    # System.HashCode
    if declaring_type == 'System.HashCode':
        if method_name == 'ToHashCode':
            return 'default(HashCode).ToHashCode()', False
        if method_name.startswith('Combine') and len(param_types) == 2:
            return f'HashCode.Combine({ipart}, {ipart})', False
        return '', False

    # System.Threading.Thread
    if declaring_type == 'System.Threading.Thread':
        tbl = {
            'get_CurrentThread': '_ = System.Threading.Thread.CurrentThread.GetHashCode()',
            'get_ManagedThreadId': '_ = System.Threading.Thread.CurrentThread.ManagedThreadId',
            'Sleep': 'System.Threading.Thread.Sleep(0)',
            'Start': 'new System.Threading.Thread(() => {}).Start()',
        }
        if method_name in tbl:
            return tbl[method_name], False
        return '', False

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
        return '', False

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
        return '', False

    return '', False


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
        '    <TargetFrameworks>net8.0;net10.0</TargetFrameworks>\n'
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

    lines = [
        '// Auto-generated managed benchmark harness\n',
        f'// Family: {family_slug}, Assembly: {assembly}\n',
        'using System;\n',
        'using System.Collections.Generic;\n',
        'using System.Diagnostics;\n',
        'using System.Text.Json;\n',
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
        '        string json = JsonSerializer.Serialize('
        'new { results }, '
        'new JsonSerializerOptions { PropertyNamingPolicy = JsonNamingPolicy.CamelCase });\n',
        '        Console.WriteLine(json);\n',
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
            mr["status"] = "completed" if mr.get("isBodyReal", False) else "error"
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
) -> dict[str, Any]:
    """Build the harness for net8.0, then run with Mono."""
    csproj = harness_dir / "ManagedBenchmarkHarness.csproj"
    build_dir = harness_dir / "bin" / "Release" / tfm
    exe_path = build_dir / "ManagedBenchmarkHarness.exe"

    print("  [managed-runner] Running Mono benchmark...")
    try:
        r = subprocess.run(
            ["dotnet", "build", str(csproj), "-f", tfm, "--configuration", "Release"],
            capture_output=True, text=True, timeout=120,
        )
        if r.returncode != 0:
            return {"method_results": [], "error": f"build failed: {r.stderr[:300]}"}

        if not exe_path.exists():
            return {"method_results": [], "error": f"exe not found at {exe_path}"}

        start = time.perf_counter()
        r = subprocess.run(
            ["mono", str(exe_path)],
            capture_output=True, text=True, timeout=300,
        )
        elapsed = time.perf_counter() - start
        if r.returncode != 0:
            return {"method_results": [], "error": f"mono exit_code={r.returncode}: {r.stderr[:300]}"}

        data = json.loads(r.stdout)
        method_results = data.get("results", [])

        for mr in method_results:
            ms = mr.get("elapsedMilliseconds", 0)
            it = mr.get("iterations", iterations)
            mr["opsPerSecond"] = (it / (ms / 1000.0)) if ms > 0 else 0.0
            mr["status"] = "completed" if mr.get("isBodyReal", False) else "error"
            mr["methodIndex"] = mr.get("methodIndex", 0)

        return {
            "method_results": method_results,
            "runtime_info": {"tfm": tfm, "runner": "mono"},
            "duration_s": round(elapsed, 2),
        }
    except subprocess.TimeoutExpired:
        return {"method_results": [], "error": "timeout (300s)"}
    except json.JSONDecodeError as e:
        return {"method_results": [], "error": f"json parse failed: {e}"}
    except Exception as e:
        return {"method_results": [], "error": str(e)}


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
        result = _run_mono_benchmark(harness_dir, spec["tfm"], iterations)
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
