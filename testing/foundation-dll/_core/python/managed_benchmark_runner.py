"""Managed benchmark runner — .NET 8 JIT, .NET 10 JIT, and Mono.

Generates harness, runs via dotnet/mono, and persists to perf_store.

CLI:
    python managed_benchmark_runner.py convert-char --technology net8-jit
    python managed_benchmark_runner.py convert-char --technology mono
"""

from __future__ import annotations

import json
import re
import shutil
import subprocess
import sys
import time
from pathlib import Path
from typing import Any

# Ensure _core package is importable when run as script
_HERE = Path(__file__).resolve().parent
_CORE_PACKAGE = _HERE.parents[1]
if str(_CORE_PACKAGE) not in sys.path:
    sys.path.insert(0, str(_CORE_PACKAGE))

# Ensure testing/ is importable for _pipeline shared components
_TESTING_ROOT = _CORE_PACKAGE.parent
if str(_TESTING_ROOT) not in sys.path:
    sys.path.insert(0, str(_TESTING_ROOT))

# Ensure old pipeline is importable for DotNetSdkDetector
_OLD_PIPELINE = _HERE.parents[4] / "build" / "toolchains" / "run" / "testing" / "foundation_dll"
if str(_OLD_PIPELINE) not in sys.path:
    sys.path.insert(0, str(_OLD_PIPELINE))

from _core.python.models import FamilyContext, StageResult
from _core.python.perf_store import save_managed_benchmark_records


# ── Technology definitions ──────────────────────────────────────────────

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


# ═══════════════════════════════════════════════════════════════════════════
# Managed Benchmark Harness Generation
# ═══════════════════════════════════════════════════════════════════════════

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

# Types for which Convert.ToChar always throws InvalidCastException
_TOCHAR_ALWAYS_THROWS = {"System.Boolean", "System.DateTime", "System.Decimal",
                          "System.Double", "System.Single", "System.Object"}

# Trivial cast-only types (JIT may elide these)
_TRIVIAL_TYPES = {"System.Byte", "System.Char", "System.Int16", "System.Int32",
                   "System.Int64", "System.SByte", "System.UInt16", "System.UInt32",
                   "System.UInt64", "System.Object"}


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
        # List
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
        # Dictionary
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
        # HashSet
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


def _generate_managed_harness(
    harness_dir: Path,
    family_slug: str,
    assembly: str,
    method_subject_ids: list[str],
    iterations: int = 100000,
) -> bool:
    """Generate ManagedBenchmarkHarness.cs and .csproj.

    Returns True if generation succeeded.
    """
    harness_dir.mkdir(parents=True, exist_ok=True)
    cs_path = harness_dir / "ManagedBenchmarkHarness.cs"
    csproj_path = harness_dir / "ManagedBenchmarkHarness.csproj"

    # ── Write .csproj (multi-target for net8.0 + net10.0) ────────────
    csproj_path.write_text(
        '<Project Sdk="Microsoft.NET.Sdk">\n'
        '  <PropertyGroup>\n'
        '    <OutputType>Exe</OutputType>\n'
        '    <StartupObject>ManagedBenchmarkHarness</StartupObject>\n'
        '    <TargetFrameworks>net8.0;net10.0</TargetFrameworks>\n'
        '    <Nullable>enable</Nullable>\n'
        '    <ImplicitUsings>enable</ImplicitUsings>\n'
        '  </PropertyGroup>\n'
        '</Project>\n',
        encoding="utf-8",
    )

    # ── Generate NoInlining helper methods ───────────────────────────
    helper_methods: list[str] = []
    helper_names: list[str] = []
    for idx, mid in enumerate(method_subject_ids):
        call_expr, always_throws = _generate_call_expr(mid, idx)
        if not call_expr:
            helper_names.append('')
            continue

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
                '    _g++;  // volatile side-effect prevents DCE\n'
                '    return false;\n'
                '}'
            )

    # ── Generate method sections ─────────────────────────────────────
    method_sections: list[str] = []
    for idx, mid in enumerate(method_subject_ids):
        call_expr, always_throws = _generate_call_expr(mid, idx)
        hname = helper_names[idx] if idx < len(helper_names) else ''

        if not call_expr:
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
            '                // Warmup: JIT compile before measurement\n'
            f'                for (int i = 0; i < {iterations}; i++) {{\n'
            f'{body}\n'
            '                }\n'
            '                // 3 rounds, take minimum to reduce GC/scheduling noise\n'
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
    if helper_methods:
        helpers_code = "\n\n".join(helper_methods)
    else:
        helpers_code = ""

    # ── Write harness .cs ────────────────────────────────────────────
    lines = [
        '// Auto-generated managed benchmark harness\n',
        f'// Family: {family_slug}, Assembly: {assembly}\n',
        'using System;\n',
        'using System.Collections.Generic;\n',
        'using System.Diagnostics;\n',
        'using System.Text.Json;\n',
        '\n',
        'class ManagedBenchmarkHarness\n',
        '{\n',
        '    static volatile int _g;  // volatile side-effect prevents JIT DCE\n',
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


# ═══════════════════════════════════════════════════════════════════════════
# Per-Runtime Runners
# ═══════════════════════════════════════════════════════════════════════════

def _run_dotnet_benchmark(
    harness_dir: Path,
    tfm: str,
    iterations: int = 100000,
) -> dict[str, Any]:
    """Run the managed harness under a specific .NET TFM via dotnet run.

    Returns: {method_results: [{methodIndex, elapsedMilliseconds, iterations, status}],
              runtime_info: {sdk_version}}
    """
    csproj = harness_dir / "ManagedBenchmarkHarness.csproj"
    if not csproj.exists():
        return {"method_results": [], "error": "harness not found"}

    print(f"  [managed-runner] Running .NET {tfm} benchmark...")
    start = time.perf_counter()
    try:
        # Try --no-build first (faster if already built)
        r = subprocess.run(
            ["dotnet", "run", "--project", str(csproj), "-f", tfm,
             "--configuration", "Release", "--no-build"],
            capture_output=True, text=True, timeout=300,
        )
        if r.returncode != 0:
            # Build first, then run
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

        # Get SDK version
        sdk_r = subprocess.run(
            ["dotnet", "--version"], capture_output=True, text=True, timeout=10
        )

        # Compute opsPerSecond for each result
        for mr in method_results:
            ms = mr.get("elapsedMilliseconds", 0)
            it = mr.get("iterations", iterations)
            mr["opsPerSecond"] = (it / (ms / 1000.0)) if ms > 0 else 0.0
            mr["status"] = "completed" if mr.get("isBodyReal", False) else "error"
            # Map C# PascalCase -> snake_case keys for perf_store
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
        # Build for the target TFM
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


# ═══════════════════════════════════════════════════════════════════════════
# Main Entry Point
# ═══════════════════════════════════════════════════════════════════════════

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
    """Run a single managed runtime benchmark and persist results.

    Args:
        ctx: Family context
        technology: "net8-jit", "net10-jit", or "mono"
        iterations: iterations per method

    Returns: StageResult with per-method details.
    """
    start = time.perf_counter()

    if technology not in TECHNOLOGY_DEFS:
        return StageResult(
            stage=f"managed_{technology}", status="error",
            summary=f"Unknown technology: {technology}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Check runtime availability
    available, unavailable = detect_managed_runtimes([technology])
    if technology in unavailable:
        return StageResult(
            stage=f"managed_{technology}", status="skipped",
            summary=f"{technology} not available (SDK not found)",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Load method subject IDs
    mids = _load_method_subject_ids(ctx)
    if not mids:
        return StageResult(
            stage=f"managed_{technology}", status="skipped",
            summary="no methods in contract",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Generate or locate harness
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
        # Verify method count matches contract
        cs_text = cs_path.read_text(encoding="utf-8")
        existing_count = len(re.findall(r'\{\s*//\s*\[\d+\]', cs_text))
        if existing_count != len(mids):
            print(f"  [managed-runner] Method count changed ({existing_count} -> {len(mids)}), regenerating harness...")
            if not _generate_managed_harness(harness_dir, ctx.slug, ctx.assembly, mids, iterations):
                return StageResult(
                    stage=f"managed_{technology}", status="error",
                    summary="harness regeneration failed",
                    duration_ms=int((time.perf_counter() - start) * 1000),
                )

    spec = TECHNOLOGY_DEFS[technology]

    # Run benchmark
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

    # Persist to perf_store
    if method_results:
        save_managed_benchmark_records(ctx, technology, method_results)
        print(f"  [managed-runner] Saved {len(method_results)} records to perf store")

    # Compute aggregate ops/s
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


def run_all_managed(
    ctx: FamilyContext,
    technologies: list[str] | None = None,
    iterations: int = 100000,
) -> list[StageResult]:
    """Run all requested managed benchmarks.

    Returns list of StageResults, one per technology.
    """
    if technologies is None:
        technologies = list(TECHNOLOGY_DEFS.keys())

    results = []
    for tech in technologies:
        sr = run_managed_benchmark(ctx, tech, iterations)
        results.append(sr)
        print(f"  [managed-runner] {tech}: {sr.status} — {sr.summary}")
    return results


# ── CLI entry point ─────────────────────────────────────────────────────

def main() -> None:
    import argparse

    parser = argparse.ArgumentParser(description="Managed benchmark runner")
    parser.add_argument("family", help="Family slug (e.g., convert-char)")
    parser.add_argument("--assembly", default="System.Private.CoreLib")
    parser.add_argument("--technology", nargs="*",
                        default=list(TECHNOLOGY_DEFS.keys()),
                        help="Runtimes to run: net8-jit net10-jit mono")
    parser.add_argument("--iterations", type=int, default=100000)
    parser.add_argument("--list", action="store_true",
                        help="List available runtimes and exit")

    args = parser.parse_args()

    if args.list:
        available, unavailable = detect_managed_runtimes()
        print("Managed runtimes:")
        for alias in list(TECHNOLOGY_DEFS.keys()):
            status = "[OK]" if alias in available else "[NO]"
            spec = TECHNOLOGY_DEFS[alias]
            print(f"  {status} {alias:12s} ({spec['runner']}, {spec['tfm']})")
        return

    ctx = FamilyContext(
        slug=args.family,
        assembly=args.assembly,
        root_dir=_CORE_PACKAGE / args.assembly / args.family,
    )

    results = run_all_managed(ctx, args.technology, args.iterations)

    # Summary
    passed = sum(1 for r in results if r.status == "passed")
    failed = sum(1 for r in results if r.status == "failed")
    skipped = sum(1 for r in results if r.status == "skipped")
    print(f"\n=== Managed benchmark summary ===")
    print(f"  Passed: {passed}, Failed: {failed}, Skipped: {skipped}")
    for r in results:
        print(f"  {r.stage:20s} {r.status:8s} {r.summary}")


if __name__ == "__main__":
    main()
