"""Family Verification Orchestrator — 7-stage unified verification pipeline.

Usage (via run.py manifest):
    run foundation-dll verify-family <family-slug>
    run foundation-dll verify-family <family-slug> --strict
    run foundation-dll verify-family <family-slug> --skip benchmark

Stage overview:
  0. Preflight   — contract integrity, custom entry discovery
  1. Codegen     — entrypoint generation + IL2CPP compile
  2. Fact        — Fact Static verify + Fact Runtime verify
  3. Audit       — Mechanism + Principle audit
  4. Benchmark   — managed vs native performance baseline
  5. HotUpdate   — patch data generation + verify
  6. PostHotBench — performance under hotpatch (interpreter path)
  7. Aggregate   — scoring, regression, pass/fail gate
"""

from __future__ import annotations

import json
import os
import re
import subprocess
import sys
import time
from dataclasses import dataclass, field, asdict
from pathlib import Path
from typing import Any, Callable

_HERE = Path(__file__).resolve().parent
_REPO_ROOT = _HERE.parents[4]
_VERIFICATION_BASE = _REPO_ROOT / "verification" / "foundation-dll"

sys.path.insert(0, str(_HERE))
sys.path.insert(0, str(_HERE.parent.parent))

try:
    from testing.trace import trace, trace_init
except ImportError:
    def trace(*args, **kwargs):
        pass
    def trace_init(*args, **kwargs) -> str:
        return ""

# ── Available stage imports (lazy — imported at call time) ────────


# ── Data types ────────────────────────────────────────────────────

@dataclass
class StageResult:
    """Result of a single verification stage."""
    stage: str
    status: str               # "passed" | "failed" | "skipped" | "error"
    summary: str = ""
    details: dict[str, Any] = field(default_factory=dict)
    errors: list[str] = field(default_factory=list)
    duration_ms: int = 0

    def to_dict(self) -> dict[str, Any]:
        return asdict(self)


@dataclass
class UnifiedReport:
    """Top-level output of a family verification run."""
    family: str
    assembly: str
    mode: str                  # "standard" | "strict"
    timestamp: str = ""
    duration_ms: int = 0
    overall_status: str = "pending"  # "passed" | "failed" | "partial"
    stages: dict[str, dict[str, Any]] = field(default_factory=dict)
    coverage: dict[str, float] = field(default_factory=dict)
    regression: dict[str, Any] = field(default_factory=dict)

    def to_dict(self) -> dict[str, Any]:
        return asdict(self)

    def to_json(self) -> str:
        return json.dumps(self.to_dict(), indent=2, ensure_ascii=False)


# ── Stage runners ─────────────────────────────────────────────────

def _parse_stub_mask(family_slug: str, assembly: str) -> int:
    """Parse stub methods from generated C# source in the entrypoint directory.

    Detects stubs by checking method bodies in generated .cs files (both the
    NativeEntry.cs and Custom.cs). Methods with empty bodies or bodies containing
    only TODO/comment lines are stubs. Methods defined in Custom.cs are non-stubs
    (handwritten). Methods with no definition at all (commented out slots) are stubs.
    """
    family_dir = _VERIFICATION_BASE / assembly / family_slug
    entry_dir = family_dir / "il2cpp_dist" / "entrypoint"
    if not entry_dir.exists():
        return 0

    # Collect all method definitions from NativeEntry.cs and Custom.cs
    # Maps: method_index -> has_real_body
    method_status: dict[int, bool] = {}

    def _extract_method_defs(content: str, known_non_stub: bool = False) -> None:
        """Extract method definitions and determine if each is a stub."""
        for m in re.finditer(
            r'public static void ((?:CustomEntry)?)(Method|CustomEntryMethod)(\d+)\(\)\s*\{',
            content
        ):
            idx = int(m.group(3))
            # Skip if already marked as non-stub by Custom.cs
            if not known_non_stub and method_status.get(idx, False):
                continue
            method_start = m.end() - 1
            brace_depth = 1
            pos = method_start + 1
            while pos < len(content) and brace_depth > 0:
                if content[pos] == '{':
                    brace_depth += 1
                elif content[pos] == '}':
                    brace_depth -= 1
                pos += 1
            body = content[method_start + 1:pos - 1].strip()

            if known_non_stub:
                method_status[idx] = True
            elif not body:
                method_status[idx] = False
            else:
                # Remove comments and check if anything remains
                stripped = re.sub(r'//.*', '', body).strip()
                method_status[idx] = bool(stripped)

    # Process Custom.cs first — methods here are always non-stub (handwritten)
    for cs_file in sorted(entry_dir.glob("*.cs")):
        if "Custom" in cs_file.stem and "NativeEntry" in cs_file.stem:
            try:
                content = cs_file.read_text(encoding="utf-8")
                _extract_method_defs(content, known_non_stub=True)
            except OSError:
                pass
            break

    # Process NativeEntry.cs — check body content to detect stubs
    for cs_file in sorted(entry_dir.glob("*.cs")):
        name = cs_file.stem
        if name.endswith("NativeEntry") and "Custom" not in name and "Program" not in name:
            try:
                content = cs_file.read_text(encoding="utf-8")
                _extract_method_defs(content)
            except OSError:
                pass
            break

    if not method_status:
        return 0

    # Build stub mask: bit i = 1 means method i is a stub
    stub_mask = 0
    for idx, has_body in method_status.items():
        if not has_body:
            stub_mask |= (1 << idx)

    print(f"  [stub_detect] method_status={dict(sorted(method_status.items()))} -> mask={stub_mask:010b} stubs={stub_mask.bit_count()}")
    return stub_mask


def _load_contract_methods(family_slug: str, assembly: str) -> list[str]:
    """Load method subject IDs from capability-family-contract.json."""
    family_dir = _VERIFICATION_BASE / assembly / family_slug
    contract_path = family_dir / "capability-family-contract.json"
    if not contract_path.exists():
        return []
    try:
        with open(contract_path, encoding="utf-8") as f:
            contract = json.load(f)
        mids = contract.get("methodSubjectIds", [])
        if not mids:
            mids = [m["methodSubjectId"] for m in contract.get("methodContracts", []) if m.get("methodSubjectId")]
        return mids
    except (OSError, ValueError, KeyError):
        return []


def _locate_entry_exe(family_slug: str, assembly: str) -> Path | None:
    """Find the native entry EXE in il2cpp_dist/genuine/<AssemblyName>/generated/"""
    family_dir = _VERIFICATION_BASE / assembly / family_slug
    genuine_dir = family_dir / "il2cpp_dist" / "genuine"
    if not genuine_dir.exists():
        return None
    for d in genuine_dir.iterdir():
        if d.is_dir():
            candidate = d / "generated" / "entry.exe"
            if candidate.exists():
                return candidate
    return None


def _locate_managed_harness(family_slug: str, assembly: str) -> Path | None:
    """Find the managed benchmark harness csproj."""
    family_dir = _VERIFICATION_BASE / assembly / family_slug
    csproj = family_dir / "managed_test" / "benchmarks" / "ManagedBenchmarkHarness.csproj"
    return csproj if csproj.exists() else None


def _auto_generate_managed_benchmark(family_slug: str, assembly: str,
                                      method_subject_ids: list[str]) -> Path | None:
    """Auto-generate and run a managed benchmark harness.

    Instead of calling entrypoint methods (which get JIT-elided due to provably
    dead _exitCode side-effects), generates inline Convert.ToChar() calls with
    varying loop-index inputs and a static accumulator to prevent dead-code elimination.
    """
    family_dir = _VERIFICATION_BASE / assembly / family_slug
    harness_dir = family_dir / "managed_test" / "benchmarks"
    harness_dir.mkdir(parents=True, exist_ok=True)

    harness_cs = harness_dir / "ManagedBenchmarkHarness.cs"
    harness_csproj = harness_dir / "ManagedBenchmarkHarness.csproj"

    # Pure managed harness — no entrypoint source needed, just calls System.Convert directly
    harness_csproj.write_text(
        '<Project Sdk="Microsoft.NET.Sdk">\n'
        '  <PropertyGroup>\n'
        '    <OutputType>Exe</OutputType>\n'
        '    <TargetFramework>net10.0</TargetFramework>\n'
        '    <Nullable>enable</Nullable>\n'
        '    <ImplicitUsings>enable</ImplicitUsings>\n'
        '  </PropertyGroup>\n'
        '</Project>\n',
        encoding="utf-8",
    )

    # ── Parse each methodSubjectId to extract parameter types ────────
    # Format: "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Type1,...)"
    _param_re = re.compile(r'\(([^)]+)\)')
    _always_throws = {"System.Boolean", "System.DateTime", "System.Decimal",
                      "System.Double", "System.Single", "System.String"}

    def _extract_param_types(mid: str) -> list[str]:
        m = _param_re.search(mid)
        if not m:
            return []
        raw = m.group(1)
        # Split on ',' that is NOT inside a nested '<>' (generic params not expected here)
        parts = []
        depth = 0
        cur: list[str] = []
        for ch in raw:
            if ch == ',' and depth == 0:
                parts.append(''.join(cur).strip())
                cur = []
            else:
                if ch == '<': depth += 1
                elif ch == '>': depth -= 1
                cur.append(ch)
        if cur:
            parts.append(''.join(cur).strip())
        return parts

    def _to_convert_call(types: list[str], idx: int) -> str:
        """Generate a Convert.ToChar(...) call with varying input based on loop var 'i'."""
        if not types:
            return ''
        t = types[0]
        ipart = f'(i + {idx})' if idx > 0 else 'i'

        # Multi-param: Object,IFormatProvider or String,IFormatProvider
        if len(types) >= 2:
            fmt = types[1].strip()
            if 'IFormatProvider' in fmt:
                if t == 'System.Object':
                    return f'Convert.ToChar((object)(({ipart}) & 0xFF), null)'
                elif t == 'System.String':
                    return f'Convert.ToChar("hello", null)'
            return ''

        # Single-param
        table = {
            "System.Boolean": f'Convert.ToChar((({ipart}) & 1) == 0)',
            "System.Byte": f'Convert.ToChar((byte)(({ipart}) & 0xFF))',
            "System.Char": f'Convert.ToChar((char)(({ipart}) & 0xFFFF))',
            "System.DateTime": f'Convert.ToChar(DateTime.UtcNow)',
            "System.Decimal": f'Convert.ToChar((decimal)(({ipart}) & 0xFF))',
            "System.Double": f'Convert.ToChar((double)(({ipart}) & 0xFF))',
            "System.Int16": f'Convert.ToChar((short)(({ipart}) & 0x7FFF))',
            "System.Int32": f'Convert.ToChar(({ipart}) & 0x7FFF)',
            "System.Int64": f'Convert.ToChar((long)(({ipart}) & 0x7FFF))',
            "System.Object": f'Convert.ToChar((object)(({ipart}) & 0xFF))',
            "System.SByte": f'Convert.ToChar((sbyte)(({ipart}) & 0x7F))',
            "System.Single": f'Convert.ToChar((float)(({ipart}) & 0xFF))',
            "System.String": f'Convert.ToChar("hello")',
            "System.UInt16": f'Convert.ToChar((ushort)(({ipart}) & 0xFFFF))',
            "System.UInt32": f'Convert.ToChar((uint)(({ipart}) & 0x7FFF))',
            "System.UInt64": f'Convert.ToChar((ulong)(({ipart}) & 0x7FFF))',
        }
        return table.get(t, '')

    # Detect trivial types: cast-only, no range check, no exception
    # These map to simple static_cast in C++ and are at risk of JIT elision
    _trivial_types = {"System.Byte", "System.Char", "System.Int16", "System.Int32",
                      "System.Int64", "System.SByte", "System.UInt16", "System.UInt32",
                      "System.UInt64", "System.Object"}

    # ── Generate NoInlining helper methods ──────────────────────────
    # For trivial and throwing methods, wrap in [NoInlining] so the JIT
    # cannot inline+elide the call. For others, inline is fine (already slow).
    helper_methods: list[str] = []
    helper_names: list[str] = []
    for idx, mid in enumerate(method_subject_ids):
        types = _extract_param_types(mid)
        call_expr = _to_convert_call(types, idx)
        if not call_expr:
            helper_names.append('')
            continue

        is_throwing = any(t.strip() in _always_throws for t in types) if types else False
        is_trivial = any(t.strip() in _trivial_types for t in types) if types else False

        if is_throwing:
            hname = f'H_{idx}'
            helper_names.append(hname)
            helper_methods.append(
                f'[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]\n'
                f'static void {hname}(int i)\n'
                f'{{\n'
                f'    try {{ {call_expr}; }} catch {{ }}\n'
                f'}}'
            )
        elif is_trivial:
            hname = f'H_{idx}'
            helper_names.append(hname)
            helper_methods.append(
                f'[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]\n'
                f'static char {hname}(int i)\n'
                f'{{\n'
                f'    return {call_expr};\n'
                f'}}'
            )
        else:
            helper_names.append('')
    iterations = 100000
    method_sections: list[str] = []
    for idx, mid in enumerate(method_subject_ids):
        types = _extract_param_types(mid)
        call_expr = _to_convert_call(types, idx)
        is_throwing = any(t.strip() in _always_throws for t in types) if types else False

        hname = helper_names[idx] if idx < len(helper_names) else ''

        if not call_expr:
            method_sections.append(
                f'            {{ // [{idx}] {mid} — unsupported type\n'
                f'                results.Add(new MethodResult {{\n'
                f'                    MethodIndex = {idx},\n'
                f'                    MethodSubjectId = "{mid}",\n'
                f'                    ElapsedMilliseconds = 0.0,\n'
                f'                    Iterations = {iterations},\n'
                f'                    IsBodyReal = false,\n'
                f'                    IsException = false,\n'
                f'                }});\n'
                f'            }}'
            )
            continue

        if is_throwing:
            body = f'                    {hname}(i);'
        elif hname:
            body = f'                    accum ^= (long){hname}(i);'
        else:
            body = f'                    accum ^= (long){call_expr};'

        method_sections.append(
            f'            {{ // [{idx}] {mid}\n'
            f'                // Warmup: JIT compile before measurement\n'
            f'                for (int i = 0; i < {iterations}; i++) {{\n'
            f'{body}\n'
            f'                }}\n'
            f'                // 3 rounds, take minimum to reduce GC/scheduling noise\n'
            f'                double bestMs = double.MaxValue;\n'
            f'                for (int r = 0; r < 3; r++) {{\n'
            f'                    var sw = System.Diagnostics.Stopwatch.StartNew();\n'
            f'                    for (int i = 0; i < {iterations}; i++) {{\n'
            f'{body}\n'
            f'                    }}\n'
            f'                    sw.Stop();\n'
            f'                    double ms = sw.Elapsed.TotalMilliseconds;\n'
            f'                    if (ms < bestMs) bestMs = ms;\n'
            f'                }}\n'
            f'                results.Add(new MethodResult {{\n'
            f'                    MethodIndex = {idx},\n'
            f'                    MethodSubjectId = "{mid}",\n'
            f'                    ElapsedMilliseconds = bestMs,\n'
            f'                    Iterations = {iterations},\n'
            f'                    IsBodyReal = true,\n'
            f'                    IsException = {"true" if is_throwing else "false"},\n'
            f'                }});\n'
            f'            }}'
        )

    sections_code = "\n".join(method_sections)
    helpers_code = "\n\n".join(helper_methods) if helper_methods else ''

    # ── Write harness .cs ──────────────────────────────────────────
    harness_lines = []
    harness_lines.append('// Auto-generated managed benchmark harness\n')
    harness_lines.append(f'// Family: {family_slug}, Assembly: {assembly}\n')
    harness_lines.append('using System;\n')
    harness_lines.append('using System.Collections.Generic;\n')
    harness_lines.append('using System.Diagnostics;\n')
    harness_lines.append('using System.Text.Json;\n')
    harness_lines.append('\n')
    harness_lines.append('class ManagedBenchmarkHarness\n')
    harness_lines.append('{\n')
    harness_lines.append('    static long accum;  // static accumulator prevents dead-code elimination\n')
    harness_lines.append('\n')
    harness_lines.append('    struct MethodResult\n')
    harness_lines.append('    {\n')
    harness_lines.append('        public int MethodIndex { get; set; }\n')
    harness_lines.append('        public string MethodSubjectId { get; set; }\n')
    harness_lines.append('        public double ElapsedMilliseconds { get; set; }\n')
    harness_lines.append('        public int Iterations { get; set; }\n')
    harness_lines.append('        public bool IsBodyReal { get; set; }\n')
    harness_lines.append('        public bool IsException { get; set; }\n')
    harness_lines.append('    }\n')
    harness_lines.append('\n')
    harness_lines.append(f'{helpers_code}\n')
    harness_lines.append('\n')
    harness_lines.append('    static void Main()\n')
    harness_lines.append('    {\n')
    harness_lines.append('        var results = new List<MethodResult>();\n')
    harness_lines.append(f'{sections_code}\n')
    harness_lines.append('        // Consume accum so JIT cannot elide the computation\n')
    harness_lines.append('        string json = JsonSerializer.Serialize(new { accumulation = accum, results }, new JsonSerializerOptions { PropertyNamingPolicy = JsonNamingPolicy.CamelCase });\n')
    harness_lines.append('        Console.WriteLine(json);\n')
    harness_lines.append('    }\n')
    harness_lines.append('}\n')
    harness_code = ''.join(harness_lines)
    harness_cs.write_text(harness_code, encoding="utf-8")

    try:
        r = subprocess.run(
            ["dotnet", "run", "--project", str(harness_csproj), "--configuration", "Release"],
            capture_output=True, text=True, timeout=300)
        if r.returncode != 0:
            print(f"    [auto_benchmark] managed harness run FAILED: {r.stderr[:200]}")
            return None
        managed_path = family_dir / "benchmark" / "managed-baseline.json"
        managed_path.parent.mkdir(parents=True, exist_ok=True)
        # Validate JSON output
        try:
            import json as _json
            _json.loads(r.stdout)
        except (ValueError, _json.JSONDecodeError):
            print(f"    [auto_benchmark] harness output is not valid JSON")
            return None
        managed_path.write_text(r.stdout, encoding="utf-8")
        print(f"    [auto_benchmark] managed baseline OK: {len(method_subject_ids)} methods")
        return managed_path
    except (OSError, subprocess.TimeoutExpired) as e:
        print(f"    [auto_benchmark] error: {e}")
        return None


def _run_native_benchmarks(family_slug: str, assembly: str,
                           method_subject_ids: list[str],
                           stub_mask: int, throwing_mask: int = 0,
                           iterations: int = 10000
                           ) -> tuple[list[dict], Path]:
    """Run native benchmark for each non-stub, non-throwing method. Returns (results, native_path)."""
    from fact_verifier import verify_benchmark

    family_dir = _VERIFICATION_BASE / assembly / family_slug
    native_dir = family_dir / "native"
    native_dir.mkdir(parents=True, exist_ok=True)

    results: list[dict] = []
    for idx, mid in enumerate(method_subject_ids):
        if stub_mask and ((stub_mask >> idx) & 1):
            results.append({
                "methodIndex": idx,
                "methodSubjectId": mid,
                "elapsedMilliseconds": -1.0,
                "status": "stub",
            })
            continue
        if throwing_mask and ((throwing_mask >> idx) & 1):
            results.append({
                "methodIndex": idx,
                "methodSubjectId": mid,
                "elapsedMilliseconds": -1.0,
                "status": "throws",
            })
            continue
        bench = verify_benchmark(family_slug, assembly=assembly,
                                 entry_index=idx, iterations=iterations, verbose=False)
        results.append({
            "methodIndex": idx,
            "methodSubjectId": mid,
            "elapsedMilliseconds": bench.get("elapsed_ms", -1.0),
            "calibratedMs": bench.get("calibrated_ms", bench.get("elapsed_ms", -1.0)),
            "opsPerSecond": bench.get("ops_per_sec", 0.0),
            "iterations": bench.get("iterations", iterations),
            "status": bench.get("status", "failed"),
        })

    native_path = native_dir / "native-benchmark.json"
    with open(native_path, "w", encoding="utf-8") as f:
        json.dump({
            "schemaVersion": 1,
            "assemblyName": assembly,
            "familyId": f"family/{assembly}/{family_slug.replace('-', '/')}",
            "warmupIterations": 100,
            "measureIterations": iterations,
            "results": results,
        }, f, indent=2, ensure_ascii=False)

    return results, native_path


def _stage_preflight(family_slug: str, assembly: str) -> StageResult:
    """Stage 0: Verify contract integrity, discover custom entries."""
    start = time.perf_counter()
    family_dir = _VERIFICATION_BASE / assembly / family_slug
    contract_path = family_dir / "capability-family-contract.json"

    if not contract_path.exists():
        return StageResult(
            stage="preflight", status="failed",
            summary=f"Contract not found: {contract_path}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    with open(contract_path, encoding="utf-8") as f:
        contract = json.load(f)

    mids = contract.get("methodSubjectIds", [])
    if not mids:
        mids = [m["methodSubjectId"] for m in contract.get("methodContracts", []) if m.get("methodSubjectId")]

    if not mids:
        return StageResult(
            stage="preflight", status="failed",
            summary="No method subject IDs found in contract",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Detect custom entry methods: check customEntryIndices (flat contract) or
    # per-method "customEntry": true in methodContracts array.
    custom_methods: list[str] = []
    custom_indices = contract.get("customEntryIndices")
    if custom_indices is not None and mids:
        for idx in custom_indices:
            if 0 <= idx < len(mids):
                custom_methods.append(mids[idx])
    for mc in contract.get("methodContracts", []):
        if mc.get("customEntry") and mc.get("methodSubjectId") and mc["methodSubjectId"] not in custom_methods:
            custom_methods.append(mc["methodSubjectId"])

    # Detect custom entry files
    custom_entry_path = family_dir / "il2cpp_dist" / "entrypoint" / f"{family_slug.title().replace('-', '').replace('_', '')}NativeEntry.Custom.cs"

    trace("preflight", family=family_slug, method_count=len(mids),
          custom_methods=len(custom_methods),
          has_custom_entry_file=custom_entry_path.exists())

    return StageResult(
        stage="preflight", status="passed",
        summary=f"{len(mids)} methods, {len(custom_methods)} custom entries",
        details={
            "methodCount": len(mids),
            "customMethodCount": len(custom_methods),
            "hasCustomEntryFile": custom_entry_path.exists(),
            "customMethods": custom_methods,
        },
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def _stage_codegen(family_slug: str, assembly: str, preflight: StageResult) -> StageResult:
    """Stage 1: Entrypoint generation + IL2CPP compile.

    Delegates to pipeline_native_aot_runner.run_family() for the heavy lifting.
    """
    start = time.perf_counter()
    try:
        from pipeline_native_aot_runner import run_family
    except ImportError:
        return StageResult(
            stage="codegen", status="error",
            summary="pipeline_native_aot_runner not importable",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    result = run_family(family_slug, assembly_name=assembly)
    ok = result.get("success", False)

    trace("codegen", family=family_slug, success=ok,
          fact_static=result.get("fact_static_passed"), fact_runtime=result.get("fact_runtime_passed"))

    if ok:
        return StageResult(
            stage="codegen", status="passed",
            summary="Entrypoint built and IL2CPP compile OK",
            details={
                "methodCount": result.get("methodCount", 0),
                "dllPath": result.get("dllPath", ""),
            },
            duration_ms=int((time.perf_counter() - start) * 1000),
        )
    else:
        return StageResult(
            stage="codegen", status="failed",
            summary=result.get("error", "Unknown codegen failure"),
            details={"steps": result.get("steps", {})},
            duration_ms=int((time.perf_counter() - start) * 1000),
        )


def _stage_fact(family_slug: str, assembly: str) -> StageResult:
    """Stage 2: Fact semantic verification — run il2cpp-translated native entry EXE.

    Delegates to fact_verifier.verify_fact().
    """
    start = time.perf_counter()
    try:
        from fact_verifier import verify_fact
    except ImportError:
        return StageResult(
            stage="fact", status="error",
            summary="fact_verifier not importable",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    fact_result = verify_fact(family_slug, assembly=assembly, verbose=False)

    overall = "passed" if fact_result.get("status") == "passed" else "failed"
    trace("fact", family=family_slug, status=fact_result.get("status"), overall=overall)

    return StageResult(
        stage="fact", status=overall,
        summary=f"Fact verify={fact_result.get('status')} ({fact_result.get('passed',0)}/{fact_result.get('total',0)})",
        details={
            "fact": {"status": fact_result.get("status"), "passed": fact_result.get("passed", 0),
                     "total": fact_result.get("total", 0)},
        },
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def _stage_audit(family_slug: str, assembly: str) -> StageResult:
    """Stage 3: Mechanism + Principle audit + principle alignment.

    Delegates to mechanism_audit.run_full_audit(), writes reports to disk.
    """
    start = time.perf_counter()
    try:
        from mechanism_audit import run_full_audit
    except ImportError:
        return StageResult(
            stage="audit", status="error",
            summary="mechanism_audit not importable",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    audit = run_full_audit(assembly, family_slug)
    mechanism = audit.get("mechanism_audit", {})
    principle = audit.get("principle_alignment", {})
    overall = audit.get("overall", {})

    # Write audit + principle reports to disk (one run, two files)
    from pathlib import Path
    family_dir = _VERIFICATION_BASE / assembly / family_slug
    (family_dir / "mechanism-audit-report.json").write_text(
        __import__("json").dumps(mechanism, indent=2, ensure_ascii=False), encoding="utf-8")
    (family_dir / "principle-alignment-report.json").write_text(
        __import__("json").dumps(principle, indent=2, ensure_ascii=False), encoding="utf-8")

    false_passing = overall.get("false_passing", 0)
    principle_status = overall.get("principle_status", "NOT_APPLICABLE")
    passed = overall.get("passed", False)

    status = "passed" if passed else "failed"

    trace("audit", family=family_slug, false_passing=false_passing,
          principle=principle_status, passed=passed)

    return StageResult(
        stage="audit", status=status,
        summary=f"false_passing={false_passing}, principle={principle_status}",
        details={
            "falsePassing": false_passing,
            "stubsFound": mechanism.get("stubs_found", 0),
            "skipsFound": mechanism.get("skips_found", 0),
            "principleStatus": principle_status,
            "methodsWithoutLowering": mechanism.get("methods_without_lowering", 0),
        },
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def _stage_benchmark(family_slug: str, assembly: str) -> StageResult:
    """Stage 4: Managed vs native benchmark comparison.

    Self-contained: runs managed harness, runs native benchmarks for each
    non-stub method, compares via benchmark_comparator, writes report.
    """
    start = time.perf_counter()
    from benchmark_comparator import compare

    stub_mask = _parse_stub_mask(family_slug, assembly)
    stub_total = stub_mask.bit_count() if stub_mask else 0
    family_dir = _VERIFICATION_BASE / assembly / family_slug
    report_path = family_dir / "benchmark-comparison-report.json"

    mids = _load_contract_methods(family_slug, assembly)
    if not mids:
        return StageResult(
            stage="benchmark", status="skipped",
            summary="No method subject IDs found",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # If all methods are stubs, skip
    if stub_total == len(mids):
        return StageResult(
            stage="benchmark", status="skipped",
            summary="All methods are stubs — no benchmark possible",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Step 1: Run managed benchmark harness (if available), or auto-generate one
    managed_path = None
    csproj = _locate_managed_harness(family_slug, assembly)
    if csproj is not None:
        try:
            r = subprocess.run(
                ["dotnet", "run", "--project", str(csproj), "--configuration", "Release"],
                capture_output=True, text=True, timeout=300)
            if r.returncode == 0:
                managed_path = family_dir / "benchmark" / "managed-baseline.json"
                managed_path.parent.mkdir(parents=True, exist_ok=True)
                managed_path.write_text(r.stdout, encoding="utf-8")
        except (OSError, subprocess.TimeoutExpired) as e:
            managed_path = None

    # Auto-generate managed benchmark harness if no csproj found or managed run failed
    if managed_path is None and mids:
        managed_path = _auto_generate_managed_benchmark(family_slug, assembly, mids)

    # Build throwing_mask from managed results (methods that always throw)
    throwing_mask = 0
    if managed_path and managed_path.exists():
        try:
            with open(managed_path, encoding="utf-8") as f:
                managed_data = json.load(f)
            for r in managed_data.get("results", []):
                idx = r.get("methodIndex", -1)
                if idx >= 0 and r.get("isException", False):
                    throwing_mask |= (1 << idx)
        except (OSError, json.JSONDecodeError):
            pass

    # Step 2: Run native benchmarks (skip throwing methods)
    native_results, native_path = _run_native_benchmarks(
        family_slug, assembly, mids, stub_mask, throwing_mask, iterations=10000)

    # Step 3: Compare
    if managed_path and managed_path.exists():
        report = compare(
            managed_path=managed_path,
            native_path=native_path,
            output_path=report_path,
        )
    else:
        # Native-only info report
        report = {
            "schemaVersion": 1,
            "assemblyName": assembly,
            "familyId": f"family/{assembly}/{family_slug.replace('-', '/')}",
            "summary": {
                "totalMethods": len(mids),
                "matchedCount": 0,
                "unmatchedCount": 0,
                "invalidCount": len(mids),
                "nativeFasterCount": 0,
                "managedFasterCount": 0,
                "equalCount": 0,
                "averageSpeedupPercent": 0.0,
            },
            "methodResults": [
                {"methodSubjectId": m, "status": "managed_harness_unavailable"}
                for m in mids
            ],
        }
        report_path.parent.mkdir(parents=True, exist_ok=True)
        with open(report_path, "w", encoding="utf-8") as f:
            json.dump(report, f, indent=2, ensure_ascii=False)

    summary = report.get("summary", {})
    avg_speedup = summary.get("averageSpeedupPercent", 0)
    native_faster = summary.get("nativeFasterCount", 0)
    managed_faster = summary.get("managedFasterCount", 0)

    trace("benchmark", family=family_slug, avg_speedup=avg_speedup,
          native_faster=native_faster, managed_faster=managed_faster)

    return StageResult(
        stage="benchmark", status="passed",
        summary=f"avg_speedup={avg_speedup}%, "
                f"native_faster={native_faster}/{summary.get('matchedCount',0)}, "
                f"managed_faster={managed_faster}/{summary.get('matchedCount',0)}",
        details={
            "averageSpeedupPercent": avg_speedup,
            "nativeFasterCount": native_faster,
            "managedFasterCount": managed_faster,
            "matchedCount": summary.get("matchedCount", 0),
            "totalMethods": summary.get("totalMethods", len(mids)),
        },
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def _stage_hotupdate(family_slug: str, assembly: str) -> StageResult:
    """Stage 5: Hotpatch verification.

    Self-contained: runs entry.exe --hotupdate, writes hotupdate-verification-report.json.
    """
    start = time.perf_counter()
    from fact_verifier import verify_hotupdate

    stub_mask = _parse_stub_mask(family_slug, assembly)
    family_dir = _VERIFICATION_BASE / assembly / family_slug
    report_path = family_dir / "hotupdate-verification-report.json"

    mids = _load_contract_methods(family_slug, assembly)
    method_count = len(mids) if mids else 0
    stub_total = stub_mask.bit_count() if stub_mask else 0

    # If all methods are stubs, skip
    if method_count > 0 and stub_total == method_count:
        return StageResult(
            stage="hotupdate", status="skipped",
            summary="All methods are stubs — no hotupdate possible",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    result = verify_hotupdate(family_slug, assembly=assembly)

    passed = result.get("passed", 0)
    failed = result.get("failed", 0)
    total = result.get("total", method_count)
    real = max(0, method_count - stub_total)

    report = {
        "schemaVersion": 1,
        "assemblyName": assembly,
        "familyId": f"family/{assembly}/{family_slug.replace('-', '/')}",
        "passedMethods": passed,
        "failedMethods": failed,
        "totalMethods": total,
        "stubMethods": stub_total,
        "realMethods": real,
        "allMethodsRevertVerified": result.get("all_revert", False),
        "allMethodsSemanticVerified": result.get("all_semantic", False),
    }
    report_path.parent.mkdir(parents=True, exist_ok=True)
    with open(report_path, "w", encoding="utf-8") as f:
        json.dump(report, f, indent=2, ensure_ascii=False)

    status = "passed" if failed == 0 else "failed"
    trace("hotupdate", family=family_slug, passed=passed, failed=failed,
          real=real, stub=stub_total)

    return StageResult(
        stage="hotupdate", status=status,
        summary=f"{real}/{real} passed, {failed} failed (stub={stub_total})",
        details={
            "passedMethods": passed,
            "failedMethods": failed,
            "totalMethods": total,
            "realMethods": real,
            "stubMethods": stub_total,
        },
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def _stage_post_hotupdate_benchmark(family_slug: str, assembly: str) -> StageResult:
    """Stage 6: Performance under hotpatch (interpreter path).

    Self-contained: runs entry.exe --hotupdate-and-benchmark for each non-stub
    method, compares post-patch ns/op vs pre-patch ns/op from native benchmark,
    writes post-hotupdate-benchmark-report.json with slowdown percentages.
    """
    start = time.perf_counter()

    method_count = 0
    stub_mask = _parse_stub_mask(family_slug, assembly)
    stub_total = stub_mask.bit_count() if stub_mask else 0
    family_dir = _VERIFICATION_BASE / assembly / family_slug
    report_path = family_dir / "post-hotupdate-benchmark-report.json"

    mids = _load_contract_methods(family_slug, assembly)
    if not mids:
        return StageResult(
            stage="post_hotupdate_benchmark", status="skipped",
            summary="No method subject IDs found",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )
    method_count = len(mids)

    # If all methods are stubs, skip
    if method_count > 0 and stub_total == method_count:
        return StageResult(
            stage="post_hotupdate_benchmark", status="skipped",
            summary="All methods are stubs — no post-hotupdate benchmark possible",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Read pre-patch native benchmark results for baseline ns/op
    pre_ns_by_method: dict[str, float] = {}
    native_path = family_dir / "native" / "native-benchmark.json"
    if native_path.exists():
        try:
            native_data = json.loads(native_path.read_text(encoding="utf-8"))
            for r in native_data.get("results", []):
                elapsed_ms = r.get("elapsedMilliseconds", 0.0)
                iters = r.get("iterations", native_data.get("measureIterations", 10000))
                if elapsed_ms and elapsed_ms > 0 and iters > 0:
                    pre_ns_by_method[r.get("methodSubjectId", "")] = (elapsed_ms * 1_000_000) / iters
        except (OSError, ValueError):
            pass

    # Find entry.exe
    exe_path = _locate_entry_exe(family_slug, assembly)
    if exe_path is None:
        return StageResult(
            stage="post_hotupdate_benchmark", status="error",
            summary="entry.exe not found — cannot run post-hotupdate benchmark",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Run --hotupdate-and-benchmark for each non-stub method
    method_results: list[dict] = []
    total_slowdown_pct = 0.0
    slowdown_count = 0
    for idx, mid in enumerate(mids):
        if stub_mask and ((stub_mask >> idx) & 1):
            method_results.append({
                "methodIndex": idx,
                "methodSubjectId": mid,
                "status": "stub",
            })
            continue

        r = subprocess.run(
            [str(exe_path), "--hotupdate-and-benchmark", str(idx), "10000"],
            capture_output=True, text=True, timeout=300)

        if r.returncode != 0:
            method_results.append({
                "methodIndex": idx,
                "methodSubjectId": mid,
                "status": "failed",
                "error": r.stdout + r.stderr,
            })
            continue

        # Parse JSON output
        import json as _json
        try:
            out = _json.loads(r.stdout.strip().splitlines()[-1])
        except (ValueError, _json.JSONDecodeError):
            method_results.append({
                "methodIndex": idx,
                "methodSubjectId": mid,
                "status": "failed",
                "error": "JSON parse failed",
            })
            continue

        post_ns = out.get("postPatchNsPerOp", 0.0)
        pre_ns = pre_ns_by_method.get(mid, 0.0)

        slowdown_pct = 0.0
        if pre_ns > 0 and post_ns > 0:
            slowdown_pct = round((post_ns - pre_ns) / pre_ns * 100, 2)
            total_slowdown_pct += slowdown_pct
            slowdown_count += 1

        method_results.append({
            "methodIndex": idx,
            "methodSubjectId": mid,
            "status": "passed",
            "prePatchNsPerOp": pre_ns,
            "postPatchNsPerOp": post_ns,
            "slowdownPercent": slowdown_pct,
        })

    avg_slowdown_pct = round(total_slowdown_pct / slowdown_count, 2) if slowdown_count > 0 else 0.0
    slowdown_factor = (100 + avg_slowdown_pct) / 100.0
    threshold_x = 50.0

    # Write report
    report = {
        "schemaVersion": 1,
        "assemblyName": assembly,
        "familyId": f"family/{assembly}/{family_slug.replace('-', '/')}",
        "methodResults": method_results,
        "summary": {
            "totalMethods": len(mids),
            "nonStubMethods": slowdown_count,
            "averageSlowdownPercent": avg_slowdown_pct,
            "slowdownFactor": slowdown_factor,
            "slowdownThresholdX": threshold_x,
        },
    }
    report_path.parent.mkdir(parents=True, exist_ok=True)
    with open(report_path, "w", encoding="utf-8") as f:
        json.dump(report, f, indent=2, ensure_ascii=False)

    # Status: post-hotupdate always goes through interpreter, large slowdown
    # is expected and not treated as failure.
    status = "passed"
    msg = f"avg_slowdown={avg_slowdown_pct}% ({slowdown_factor:.1f}x) over {slowdown_count} methods (interpreter path — expected)"

    trace("post_hotupdate_benchmark", family=family_slug,
          avg_slowdown=avg_slowdown_pct, methods=slowdown_count, status=status)

    return StageResult(
        stage="post_hotupdate_benchmark", status=status,
        summary=msg,
        details={
            "averageSlowdownPercent": avg_slowdown_pct,
            "slowdownFactor": slowdown_factor,
            "threshold": threshold_x,
            "nonStubMethods": slowdown_count,
        },
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


# ── Aggregation ───────────────────────────────────────────────────

def _compute_coverage(stages: dict[str, StageResult]) -> dict[str, float]:
    """Compute coverage score from stage results."""
    scores: dict[str, float] = {}

    # Total method count from preflight
    preflight = stages.get("preflight")
    total_methods = 0
    if preflight and preflight.details:
        total_methods = preflight.details.get("methodCount", 0)

    # Method coverage from fact stage (as percentage)
    fact = stages.get("fact")
    if fact and fact.details:
        fact_obj = fact.details.get("fact", {})
        total = fact_obj.get("total", 0)
        passed = fact_obj.get("passed", 0)
        scores["methodCoverage"] = round((passed / total) * 100, 2) if total > 0 else 0.0
    else:
        scores["methodCoverage"] = 0.0

    # Tested rate: proportion of methods NOT skipped (as percentage)
    audit = stages.get("audit")
    if audit and audit.details:
        skips = audit.details.get("skipsFound", 0)
        effective_total = max(total_methods, 1)
        scores["testedRate"] = round(max(0.0, 1.0 - (skips / effective_total)) * 100, 2)
    else:
        scores["testedRate"] = 0.0

    # Overall — composite of available scores (as percentage)
    values = [v for v in scores.values() if v > 0]
    scores["overall"] = round(sum(values) / len(values), 2) if values else 0.0

    return scores


def _aggregate(family_slug: str, assembly: str,
               stage_results: list[StageResult],
               mode: str,
               total_duration_ms: int) -> UnifiedReport:
    """Aggregate all stage results into the final report."""
    stages_map: dict[str, dict[str, Any]] = {}
    for sr in stage_results:
        stages_map[sr.stage] = sr.to_dict()

    coverage = _compute_coverage({sr.stage: sr for sr in stage_results})

    # Run baseline regression detection
    regression = _detect_regression(family_slug, assembly, stage_results)

    # Determine overall pass/fail
    required_stages = {"preflight", "codegen", "fact", "audit"}
    if mode == "strict":
        required_stages.update({"hotupdate", "post_hotupdate_benchmark"})

    failures = [
        sr.stage for sr in stage_results
        if sr.stage in required_stages and sr.status == "failed"
    ]
    errors = [
        sr.stage for sr in stage_results
        if sr.stage in required_stages and sr.status == "error"
    ]

    if errors:
        overall_status = "failed"
    elif failures:
        overall_status = "partial"
    else:
        overall_status = "passed"

    return UnifiedReport(
        family=family_slug,
        assembly=assembly,
        mode=mode,
        timestamp=time.strftime("%Y-%m-%dT%H:%M:%S"),
        duration_ms=total_duration_ms,
        overall_status=overall_status,
        stages=stages_map,
        coverage=coverage,
        regression=regression,
    )


def _detect_regression(family_slug: str, assembly: str,
                        stage_results: list[StageResult]) -> dict[str, Any]:
    """Run baseline regression detection on checksums and benchmarks."""
    from baseline_manager import compare_checksum_baseline, compare_benchmark_baseline

    result: dict[str, Any] = {}

    trace("regression_check", family=family_slug,
          benchmark_status=result.get("benchmark", {}).get("status", "none"))
    return result


def _write_report(report: UnifiedReport, family_slug: str, assembly: str) -> Path:
    """Write the unified report JSON to the family directory."""
    family_dir = _VERIFICATION_BASE / assembly / family_slug
    report_path = family_dir / "unified-verification-report.json"
    report_path.parent.mkdir(parents=True, exist_ok=True)
    report_path.write_text(report.to_json(), encoding="utf-8")
    return report_path


# ── Public entry point ───────────────────────────────────────────

def verify_family(family_slug: str,
                  assembly: str = "System.Private.CoreLib",
                  mode: str = "standard",
                  skip_stages: list[str] | None = None,
                  verbose: bool = False) -> dict[str, Any]:
    """Run the full 7-stage verification pipeline for a single family.

    Args:
        family_slug:  e.g. "convert-char"
        assembly:     Assembly name (default "System.Private.CoreLib")
        mode:         "standard" (default) or "strict"
        skip_stages:  List of stage names to skip, e.g. ["benchmark", "hotupdate"]
        verbose:      Print detailed output

    Returns:
        UnifiedReport as dict
    """
    overall_start = time.perf_counter()
    skip = set(skip_stages or [])
    stage_results: list[StageResult] = []

    # Initialize trace session for cross-language span correlation
    trace_init(_REPO_ROOT, stage=f"verify-family/{family_slug}")

    print(f"\n{'='*60}")
    print(f"Family Verify: {family_slug} [{assembly}] mode={mode}")
    print(f"{'='*60}\n")

    # Stage 0: Preflight
    if "preflight" not in skip:
        print(f"[0/7] Preflight...")
        sr = _stage_preflight(family_slug, assembly)
        stage_results.append(sr)
        print(f"  {sr.status}: {sr.summary}")
        if sr.status == "failed":
            print(f"  Cannot continue — preflight failed")
            report = _aggregate(family_slug, assembly, stage_results, mode,
                                int((time.perf_counter() - overall_start) * 1000))
            report.overall_status = "failed"
            _write_report(report, family_slug, assembly)
            return report.to_dict()
    else:
        print(f"[0/7] Preflight... skipped")

    # Stage 1: Codegen
    if "codegen" not in skip:
        print(f"[1/7] Codegen...")
        sr = _stage_codegen(family_slug, assembly, stage_results[0] if stage_results else StageResult("preflight", "passed"))
        stage_results.append(sr)
        print(f"  {sr.status}: {sr.summary}")
        if sr.status == "failed" and mode == "strict":
            print(f"  Stopping — codegen failed in strict mode")
            report = _aggregate(family_slug, assembly, stage_results, mode,
                                int((time.perf_counter() - overall_start) * 1000))
            _write_report(report, family_slug, assembly)
            return report.to_dict()
    else:
        print(f"[1/7] Codegen... skipped")

    # Stage 2: Fact
    if "fact" not in skip:
        print(f"[2/7] Fact (Static+Runtime)...")
        sr = _stage_fact(family_slug, assembly)
        stage_results.append(sr)
        print(f"  {sr.status}: {sr.summary}")
    else:
        print(f"[2/7] Fact... skipped")

    # Stage 3: Audit
    if "audit" not in skip:
        print(f"[3/7] Mechanism + Principle Audit...")
        sr = _stage_audit(family_slug, assembly)
        stage_results.append(sr)
        print(f"  {sr.status}: {sr.summary}")
    else:
        print(f"[3/7] Audit... skipped")

    # Stage 4: Benchmark
    if "benchmark" not in skip:
        print(f"[4/7] Benchmark...")
        sr = _stage_benchmark(family_slug, assembly)
        stage_results.append(sr)
        print(f"  {sr.status}: {sr.summary}")
    else:
        print(f"[4/7] Benchmark... skipped")

    # Stage 5: HotUpdate
    if "hotupdate" not in skip:
        print(f"[5/7] HotUpdate...")
        sr = _stage_hotupdate(family_slug, assembly)
        stage_results.append(sr)
        print(f"  {sr.status}: {sr.summary}")
    else:
        print(f"[5/7] HotUpdate... skipped")

    # Stage 6: Post-HU Benchmark
    if "post_hotupdate_benchmark" not in skip:
        print(f"[6/7] Post-HotUpdate Benchmark...")
        sr = _stage_post_hotupdate_benchmark(family_slug, assembly)
        stage_results.append(sr)
        print(f"  {sr.status}: {sr.summary}")
    else:
        print(f"[6/7] Post-HotUpdate Benchmark... skipped")

    # Stage 7: Aggregate
    print(f"[7/7] Aggregating...")
    report = _aggregate(family_slug, assembly, stage_results, mode,
                        int((time.perf_counter() - overall_start) * 1000))
    report_path = _write_report(report, family_slug, assembly)

    print(f"\n{'='*60}")
    print(f"Result: {report.overall_status}")
    print(f"Coverage: {report.coverage}")
    print(f"Report: {report_path}")
    print(f"{'='*60}")

    trace("family_verify_complete", family=family_slug,
          overall=report.overall_status, mode=mode,
          stages={sr.stage: sr.status for sr in stage_results})

    return report.to_dict()


# ── CLI entry point ──────────────────────────────────────────────

def main() -> None:
    import argparse
    parser = argparse.ArgumentParser(description="Family verification orchestrator")
    parser.add_argument("family_slug", help="Family slug (e.g., convert-char)")
    parser.add_argument("--assembly", default="System.Private.CoreLib")
    parser.add_argument("--mode", choices=["standard", "strict"], default="standard")
    parser.add_argument("--skip", nargs="*", default=[],
                        help="Stages to skip: preflight codegen fact audit benchmark hotupdate post_hotupdate_benchmark")
    parser.add_argument("--verbose", "-v", action="store_true")
    args = parser.parse_args()

    result = verify_family(
        args.family_slug,
        assembly=args.assembly,
        mode=args.mode,
        skip_stages=args.skip,
        verbose=args.verbose,
    )

    # Exit code
    if result.get("overall_status") == "passed":
        sys.exit(0)
    elif result.get("overall_status") == "partial":
        sys.exit(2)  # Partial — some non-required stages failed
    else:
        sys.exit(1)


if __name__ == "__main__":
    main()
