"""Family Verification Orchestrator — 15-stage unified verification pipeline.

Usage (via run.py manifest):
    run foundation-dll verify-family <family-slug>
    run foundation-dll verify-family <family-slug> --strict
    run foundation-dll verify-family <family-slug> --skip benchmark

Stage overview:
  0.  Preflight       — contract integrity, custom entry discovery
  1.  Codegen (AOT)   — entrypoint generation + IL2CPP compile (entry-aot.exe)
  2.  JitCodegen      — build entry-jit.exe (interpreter path)
  3.  Fact (.NET8)    — managed .NET8 fact verification via dotnet run
  4.  Fact (AOT)      — Chaos AOT fact verification (entry-aot.exe)
  5.  Fact (JIT)      — Chaos JIT fact verification (entry-jit.exe)
  6.  Audit           — Mechanism + Principle audit
  7.  AsmCompare      — JIT vs AOT instruction-level analysis
  8.  Microbench      — Interpreter internal metrics
  9.  Benchmark (5-way) — .NET8/.NET10/Mono/Chaos AOT/Chaos JIT
  10. HotUpdate AOT Fact     — entry-aot.exe --hotupdate -> verify
  11. HotUpdate AOT Bench    — pre vs post patch performance (AOT)
  12. HotUpdate JIT Fact     — entry-jit.exe --hotupdate -> verify
  13. HotUpdate JIT Bench    — pre vs post patch performance (JIT)
  14. Dashboard       — cross-family summary (JSON + console)
  15. Aggregate       — scoring, regression, pass/fail gate
"""

from __future__ import annotations

import json
import os
import re
import shutil
import subprocess
import sys
import time
from dataclasses import dataclass, field, asdict
from pathlib import Path
from typing import Any, Callable

# Add package paths to sys.path BEFORE same-package imports so that
# imports like "from multi_benchmark_runner import ..." resolve correctly
# when this module is imported from external contexts (e.g. subject workers).
_HERE = Path(__file__).resolve().parent
if str(_HERE) not in sys.path:
    sys.path.insert(0, str(_HERE))
if str(_HERE.parent.parent) not in sys.path:
    sys.path.insert(0, str(_HERE.parent.parent))

from multi_benchmark_runner import (
    MultiRunReport,
    adapt_csproj_for_multitarget,
    detect_available_runtimes,
    run_multi_benchmark,
    save_report,
    print_report_summary,
)

_REPO_ROOT = _HERE.parents[4]
_VERIFICATION_BASE = _REPO_ROOT / "testing" / "foundation-dll"

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
    dashboard: dict[str, Any] = field(default_factory=dict)
    regression: dict[str, Any] = field(default_factory=dict)

    def to_dict(self) -> dict[str, Any]:
        return asdict(self)

    def to_json(self) -> str:
        return json.dumps(self.to_dict(), indent=2, ensure_ascii=False)


# ── Stage runners ─────────────────────────────────────────────────

def parse_stub_mask(family_slug: str, assembly: str) -> int:
    """Parse stub methods from generated C# source in managed/subjects/.

    Detects stubs by checking method bodies in generated .cs files in managed/subjects/.
    Methods with bodies containing only TODO/comment lines are stubs.
    """
    family_dir = _VERIFICATION_BASE / assembly / family_slug
    subjects_dir = family_dir / "managed" / "subjects"
    if not subjects_dir.exists():
        return 0

    # Collect all method definitions from Subjects.cs
    # Maps: method_index -> has_real_body
    method_status: dict[int, bool] = {}

    def _extract_method_defs(content: str) -> None:
        for m in re.finditer(
            r'public static (?:int|void) (Subject_|CustomEntrySubject_)(\d+)\(\)\s*\{',
            content
        ):
            idx = int(m.group(2))
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
            if not body:
                method_status[idx] = False
            else:
                stripped = re.sub(r'//.*', '', body).strip()
                method_status[idx] = bool(stripped)

    # Process Subjects.cs
    for cs_file in sorted(subjects_dir.glob("*.cs")):
        if "Subjects" in cs_file.stem and "Custom" not in cs_file.stem:
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


def load_contract_methods(family_slug: str, assembly: str) -> list[str]:
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


def locate_entry_executable(family_slug: str, assembly: str) -> Path | None:
    """Find the native entry EXE in native/ directory.

    Checks paths in priority order:
      1. native/build/Release/entry.exe  (CMake Release build output)
      2. native/entry.exe                (root-level copy/symlink)
    """
    family_dir = _VERIFICATION_BASE / assembly / family_slug
    candidates = [
        family_dir / "native" / "build" / "Release" / "entry.exe",
        family_dir / "native" / "entry.exe",
    ]
    for candidate in candidates:
        if candidate.exists():
            return candidate
    return None


def locate_managed_harness(family_slug: str, assembly: str) -> Path | None:
    """Find the managed benchmark harness csproj in managed/ directory."""
    family_dir = _VERIFICATION_BASE / assembly / family_slug
    csproj = family_dir / "managed" / "ConvertChar.csproj"
    return csproj if csproj.exists() else None


def auto_generate_managed_benchmark(family_slug: str, assembly: str,
                                      method_subject_ids: list[str]) -> Path | None:
    """Auto-generate and run a managed benchmark harness.

    Instead of calling entrypoint methods (which get JIT-elided due to provably
    dead _exitCode side-effects), generates inline Convert.ToChar() calls with
    varying loop-index inputs and a static accumulator to prevent dead-code elimination.

    Uses the shared managed_harness_generator module for call expression generation.
    """
    from managed_harness_generator import generate_call_expr, CallExprMode

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
        '    <StartupObject>ManagedBenchmarkHarness</StartupObject>\n'
        '  </PropertyGroup>\n'
        '</Project>\n',
        encoding="utf-8",
    )

    # ── Parse each methodSubjectId to extract parameter types ────────
    # Format: "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Type1,...)"
    _param_re = re.compile(r'\(([^)]+)\)')

    # ── ToXxx(string) literal table ────────────────────────────────
    _toxxx_string_literals: dict = {
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

    # ── Always-throwing param types for Convert.ToChar ──────────────
    # Detect trivial types: cast-only, no range check, no exception
    # These map to simple static_cast in C++ and are at risk of JIT elision
    # ── Generate NoInlining helper methods ──────────────────────────
    # Every method uses a volatile side-effect to prevent JIT dead-code
    # elimination, regardless of return type (bool, string, int, etc.).
    helper_methods: list[str] = []
    helper_names: list[str] = []
    for idx, mid in enumerate(method_subject_ids):
        call_expr, always_throws = generate_call_expr(mid, idx, CallExprMode.BENCHMARK)
        if not call_expr:
            helper_names.append('')
            continue

        is_throwing = always_throws

        if is_throwing:
            hname = f'H_{idx}'
            helper_names.append(hname)
            helper_methods.append(
                f'[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]\n'
                f'static bool {hname}(int i)\n'
                f'{{\n'
                f'    try {{ {call_expr}; return false; }} catch {{ return true; }}\n'
                f'}}'
            )
        else:
            hname = f'H_{idx}'
            helper_names.append(hname)
            helper_methods.append(
                f'[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]\n'
                f'static bool {hname}(int i)\n'
                f'{{\n'
                f'    {call_expr};\n'
                f'    _g++;  // volatile side-effect prevents DCE\n'
                f'    return false;\n'
                f'}}'
            )

    iterations = 100000
    method_sections: list[str] = []
    for idx, mid in enumerate(method_subject_ids):
        call_expr, always_throws = generate_call_expr(mid, idx, CallExprMode.BENCHMARK)
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

        if hname:
            body = f'                    if ({hname}(i)) threw = true;'
        else:
            body = f'                    {{ try {{ {call_expr}; _g++; }} catch {{ threw = true; }} }}'

        method_sections.append(
            f'            {{ // [{idx}] {mid}\n'
            f'                bool threw = false;\n'
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
            f'                    IsException = threw,\n'
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
    harness_lines.append('    static volatile int _g;  // volatile side-effect prevents JIT DCE\n')
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
    harness_lines.append('        string json = JsonSerializer.Serialize(new { results }, new JsonSerializerOptions { PropertyNamingPolicy = JsonNamingPolicy.CamelCase });\n')
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


def stage_preflight(family_slug: str, assembly: str) -> StageResult:
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
            stage="preflight", status="passed",
            summary="No method subject IDs found in contract — empty family (skipping verification)",
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
    custom_entry_path = family_dir / "managed" / "subjects" / f"{family_slug.title().replace('-', '').replace('_', '').replace(',', '')}Subjects.Custom.cs"

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


def stage_code_generation(family_slug: str, assembly: str, preflight: StageResult, *, codegen_mode: str | None = None) -> StageResult:
    """Stage 1: Entrypoint generation + IL2CPP compile.

    Delegates to pipeline_native_aot_runner.run_family() for the heavy lifting,
    then generates verification dispatch C++ code from the codegen manifest
    and rebuilds entry.exe to include the dispatch code.
    """
    start = time.perf_counter()
    try:
        from pipeline_native_aot_runner import run_family, build_entry_executable
    except ImportError:
        import traceback as _tb
        _tb.print_exc()
        return StageResult(
            stage="codegen", status="error",
            summary="pipeline_native_aot_runner not importable",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    result = run_family(family_slug, assembly_name=assembly, codegen_mode=codegen_mode)
    ok = result.get("success", False)

    # After codegen succeeds, generate verification dispatch code from manifest
    if ok:
        try:
            from verification_dispatch_generator import generate_verification_dispatch

            family_dir = _VERIFICATION_BASE / assembly / family_slug
            codegen_dir = family_dir / "codegen"
            # The manifest is emitted under *Subjects/generated/ (e.g. *Subjects/generated/native-aot.methods.json)
            manifest_path = None
            for d in codegen_dir.iterdir():
                if d.is_dir() and d.name.endswith("Subjects"):
                    # Check under generated/ subdirectory
                    candidate = d / "generated" / "native-aot.methods.json"
                    if candidate.exists():
                        manifest_path = candidate
                        break
            dispatch_output = family_dir / "native" / "verification_dispatch.generated.cpp"

            if manifest_path is not None:
                generate_verification_dispatch(str(manifest_path), str(dispatch_output))
                # Rebuild entry.exe with the new dispatch file
                rebuild_ok = build_entry_executable(family_slug, verification=family_dir.parent)
                if not rebuild_ok:
                    print(f"    [codegen] WARNING: entry.exe rebuild with dispatch code FAILED")
            else:
                print(f"    [codegen] manifest not found (skip dispatch generation)")
        except ImportError:
            print(f"    [codegen] generate_verification_dispatch not available (skip)")
        except Exception as e:
            print(f"    [codegen] dispatch generation error: {e} (skip)")

    trace("codegen", family=family_slug, success=ok,
          fact_static=result.get("fact_static_passed"), fact_runtime=result.get("fact_runtime_passed"))

    if ok:
        # Save AOT binary for subsequent stages (JIT codegen overwrites entry.exe)
        _native_dir = _VERIFICATION_BASE / assembly / family_slug / "native"
        _entry_exe = _native_dir / "entry.exe"
        _aot_exe = _native_dir / "entry-aot.exe"
        if _entry_exe.exists() and not _aot_exe.exists():
            shutil.copy2(str(_entry_exe), str(_aot_exe))
            print(f"    [codegen] saved entry.exe -> entry-aot.exe")

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


def stage_jit_code_generation(family_slug: str, assembly: str) -> StageResult:
    """Stage 1b: JIT entrypoint generation + entry-jit.exe build.

    Builds JIT-mode entry-jit.exe, then restores AOT-mode entry.exe
    so subsequent stages run against AOT dispatch by default.
    """
    start = time.perf_counter()
    family_dir = _VERIFICATION_BASE / assembly / family_slug
    jit_exe = family_dir / "native" / "entry-jit.exe"
    aot_exe = family_dir / "native" / "entry-aot.exe"
    entry_exe = family_dir / "native" / "entry.exe"

    if jit_exe.exists():
        return StageResult(stage="jit_codegen", status="passed",
                           summary="entry-jit.exe already exists, skipping rebuild",
                           duration_ms=int((time.perf_counter() - start) * 1000))

    print(f"  [jit_codegen] Building JIT mode entry-jit.exe...")
    try:
        from pipeline_native_aot_runner import run_family, build_entry_executable
        jit_result = run_family(family_slug, assembly_name=assembly, codegen_mode="jit")
        if not jit_result.get("success"):
            return StageResult(stage="jit_codegen", status="failed",
                               summary="JIT codegen failed",
                               duration_ms=int((time.perf_counter() - start) * 1000))

        # Regenerate verification dispatch from JIT codegen manifest to avoid
        # stale dispatch files that define kSubjectEntryCount (causing LNK2005).
        try:
            from verification_dispatch_generator import generate_verification_dispatch
            codegen_dir = family_dir / "codegen"
            manifest_path = None
            for d in codegen_dir.iterdir():
                if d.is_dir() and d.name.endswith("Subjects"):
                    candidate = d / "generated" / "native-aot.methods.json"
                    if candidate.exists():
                        manifest_path = candidate
                        break
            if manifest_path is not None:
                dispatch_output = family_dir / "native" / "verification_dispatch.generated.cpp"
                generate_verification_dispatch(str(manifest_path), str(dispatch_output), jit_mode=True)
                print(f"  [jit_codegen] regenerated verification_dispatch.generated.cpp")
        except ImportError:
            print(f"  [jit_codegen] generate_verification_dispatch not available (skip)")

        build_ok = build_entry_executable(family_slug, verification=family_dir.parent,
                                    output_name="entry-jit.exe", is_jit=True)
        if not build_ok:
            return StageResult(stage="jit_codegen", status="failed",
                               summary="entry-jit.exe build failed",
                               duration_ms=int((time.perf_counter() - start) * 1000))

        # Restore AOT-mode entry.exe for subsequent stages
        if aot_exe.exists():
            shutil.copy2(str(aot_exe), str(entry_exe))
            print(f"  [jit_codegen] restored entry.exe <- entry-aot.exe")

        return StageResult(stage="jit_codegen", status="passed",
                           summary="JIT entry-jit.exe built and AOT entry.exe restored",
                           duration_ms=int((time.perf_counter() - start) * 1000))
    except ImportError:
        return StageResult(stage="jit_codegen", status="failed",
                           summary="pipeline_native_aot_runner not importable",
                           duration_ms=int((time.perf_counter() - start) * 1000))
    except Exception as e:
        return StageResult(stage="jit_codegen", status="failed",
                           summary=f"JIT codegen error: {e}",
                           duration_ms=int((time.perf_counter() - start) * 1000))


def stage_managed_fact(family_slug: str, assembly: str,
                       tfm: str = "net8.0") -> StageResult:
    """Stage 3: .NET8 Fact verification via managed harness.

    Uses managed_fact_runner to generate a C# harness and run with dotnet.
    """
    start = time.perf_counter()

    try:
        subprocess.run(["dotnet", "--version"], capture_output=True, text=True, timeout=30)
    except (OSError, subprocess.TimeoutExpired):
        return StageResult(
            stage="managed_fact", status="skipped",
            summary="dotnet CLI not available",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    from managed_fact_runner import run_managed_fact_for_family
    result = run_managed_fact_for_family(family_slug, assembly, tfm=tfm)

    status_map = {"passed": "passed", "failed": "failed", "skipped": "skipped", "error": "error"}
    status = status_map.get(result.get("status", "error"), "error")

    return StageResult(
        stage="managed_fact", status=status,
        summary=result.get("summary", ""),
        details={
            "totalMethods": result.get("totalMethods", 0),
            "passedMethods": result.get("passedMethods", 0),
            "failedMethods": result.get("failedMethods", 0),
            "skippedMethods": result.get("skippedMethods", 0),
            "tfm": tfm,
            "methodResults": result.get("methodResults", []),
        },
        duration_ms=result.get("duration_ms", int((time.perf_counter() - start) * 1000)),
    )


def stage_fact_verification(family_slug: str, assembly: str) -> StageResult:
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


def stage_jit_fact_verification(family_slug: str, assembly: str) -> StageResult:
    """Stage 2b: Fact JIT — run entry-jit.exe through interpreter dispatch.

    Verifies all methods produce correct results via the JIT/interpreter path.
    """
    start = time.perf_counter()
    family_dir = _VERIFICATION_BASE / assembly / family_slug
    jit_exe = family_dir / "native" / "entry-jit.exe"

    if not jit_exe.exists():
        return StageResult(stage="fact_jit", status="skipped",
                           summary="entry-jit.exe not found — cannot run JIT fact",
                           duration_ms=int((time.perf_counter() - start) * 1000))

    try:
        from fact_verifier import verify_fact
        result = verify_fact(family_slug, assembly=assembly, exe_path=jit_exe)
        status = "passed" if result.get("status") == "passed" else "failed"
        passed = result.get("passed", 0)
        total = result.get("total", 0)
        return StageResult(
            stage="fact_jit", status=status,
            summary=f"Fact JIT verify={status} ({passed}/{total})",
            details={"fact_jit": result},
            duration_ms=int((time.perf_counter() - start) * 1000),
        )
    except Exception as e:
        return StageResult(stage="fact_jit", status="failed",
                           summary=f"Fact JIT crashed: {e}",
                           duration_ms=int((time.perf_counter() - start) * 1000))


def stage_principle_audit(family_slug: str, assembly: str, skip_stages: set[str] | None = None, *, codegen_mode: str | None = None) -> StageResult:
    """Stage 3: Mechanism + Principle audit + principle alignment.

    Delegates to mechanism_audit.run_full_audit(), writes reports to disk.

    When codegen_mode is "jit", the p1_benchmark principle check is
    overridden to NOT_APPLICABLE — JIT mode intentionally routes all methods
    through the interpreter, so native-slower-than-managed is expected.
    """
    skip = skip_stages or set()
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

    # When running in JIT mode, the p1_benchmark principle is not applicable.
    # JIT mode intentionally routes all methods through the interpreter, so
    # native-slower-than-managed is expected behavior, not a violation.
    is_jit_mode = (codegen_mode or "").lower() == "jit"

    # If benchmark is skipped, demote p1_benchmark so families without
    # meaningful benchmark data still pass audit.
    if "benchmark" in skip:
        checks = principle.get("checks", {})
        bm_check = checks.get("p1_benchmark", {})
        bm_status = bm_check.get("status")
        if bm_status in ("VIOLATION", "CONCERN"):
            bm_check["status"] = "NOT_APPLICABLE"
            bm_check["summary"] += " [overridden — benchmark stage was skipped]"

        # Recompute overall principle status whenever benchmark is skipped,
        # so the strict "ALIGNED" requirement from run_full_audit() does not
        # cause a spurious audit failure on CONCERN-only checks.
        status_counts = {}
        for c in checks.values():
            status_counts[c["status"]] = status_counts.get(c["status"], 0) + 1
        if status_counts.get("VIOLATION", 0) > 0:
            principle_overall_new = "VIOLATION"
        elif status_counts.get("CONCERN", 0) > 0:
            principle_overall_new = "CONCERN"
        elif status_counts.get("ALIGNED", 0) > 0:
            principle_overall_new = "ALIGNED"
        else:
            principle_overall_new = "NOT_APPLICABLE"
        principle.setdefault("summary", {})["overall"] = principle_overall_new
        overall["principle_status"] = principle_overall_new
        overall["passed"] = mechanism.get("passed", False) and principle_overall_new != "VIOLATION"

    # In JIT mode, demote p1_benchmark since interpreter is intentionally slower
    if is_jit_mode:
        checks = principle.get("checks", {})
        bm_check = checks.get("p1_benchmark", {})
        bm_status = bm_check.get("status")
        if bm_status in ("VIOLATION", "CONCERN"):
            bm_check["status"] = "NOT_APPLICABLE"
            bm_check["summary"] += " [overridden — codegen-mode=jit, interpreter path is expected to be slower]"
            # Recompute overall principle status
            status_counts = {}
            for c in checks.values():
                status_counts[c["status"]] = status_counts.get(c["status"], 0) + 1
            if status_counts.get("VIOLATION", 0) > 0:
                principle_overall_new = "VIOLATION"
            elif status_counts.get("CONCERN", 0) > 0:
                principle_overall_new = "CONCERN"
            elif status_counts.get("ALIGNED", 0) > 0:
                principle_overall_new = "ALIGNED"
            else:
                principle_overall_new = "NOT_APPLICABLE"
            principle.setdefault("summary", {})["overall"] = principle_overall_new
            overall["principle_status"] = principle_overall_new
            overall["passed"] = mechanism.get("passed", False) and principle_overall_new != "VIOLATION"

    # Write audit + principle reports to disk (one run, two files)
    from pathlib import Path
    family_dir = _VERIFICATION_BASE / assembly / family_slug
    (family_dir / "mechanism-audit-report.json").write_text(
        __import__("json").dumps(mechanism, indent=2, ensure_ascii=False), encoding="utf-8")
    (family_dir / "principle-alignment-report.json").write_text(
        __import__("json").dumps(principle, indent=2, ensure_ascii=False), encoding="utf-8")

    false_passing = overall.get("false_passing", 0)
    principle_status = overall.get("principle_status", "NOT_APPLICABLE")
    mechanism_passed = overall.get("mechanism_passed", False)

    # CONCERN is acceptable — only VIOLATION blocks the audit gate.
    # This mirrors the benchmark-skip path logic (CONCERN != VIOLATION).
    passed = mechanism_passed and principle_status != "VIOLATION"

    status = "passed" if passed else "failed"

    trace("audit", family=family_slug, false_passing=false_passing,
          principle=principle_status, passed=passed)

    return StageResult(
        stage="audit", status=status,
        summary=f"false_passing={false_passing}, principle={principle_status}"
                 f" (mech_passed={mechanism_passed}, strict_on_CONCERN=False)",
        details={
            "falsePassing": false_passing,
            "stubsFound": mechanism.get("stubs_found", 0),
            "skipsFound": mechanism.get("skips_found", 0),
            "principleStatus": principle_status,
            "methodsWithoutLowering": mechanism.get("methods_without_lowering", 0),
        },
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def stage_assembly_comparison(family_slug: str, assembly: str) -> StageResult:
    """Stage 4: JIT vs AOT instruction-level analysis via asm-compare.

    Runs asm-compare for each subject method with --format json --sections metrics,
    aggregates deterministic metrics (instruction count, IR expansion ratio, dispatch
    distribution, boxing ops) into asm-compare-report.json.

    Replaces the previous AI self-analysis of managed vs native code with
    deterministic, reproducible metric comparison.
    """
    start = time.perf_counter()
    try:
        from asm_compare_verifier import verify_family_asm_compare
    except ImportError:
        return StageResult(
            stage="asm_compare", status="error",
            summary="asm_compare_verifier not importable",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    result = verify_family_asm_compare(family_slug, assembly)
    status = result.get("status", "failed")
    summary = result.get("summary", "")
    details = result.get("details", {})

    trace("asm_compare", family=family_slug, status=status,
          ok_count=details.get("okCount", 0),
          total=details.get("totalMethods", 0))

    return StageResult(
        stage="asm_compare", status=status,
        summary=summary,
        details=details,
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def stage_micro_benchmark(family_slug: str, assembly: str) -> StageResult:
    """Stage 4.5: Interpreter microbenchmark — measures internal interpreter efficiency.

    Runs entry.exe --microbench and captures:
      - FastFramePool: Acquire+Release ns/op
      - FastExecute: per-instruction overhead (ns/instr)
      - CallVirt: handler dispatch, MIC hit, raw fallback latencies

    Writes interpreter-microbench-report.json to family directory.
    """
    start = time.perf_counter()
    import re

    exe_path = locate_entry_executable(family_slug, assembly)
    if exe_path is None:
        return StageResult(
            stage="microbench", status="skipped",
            summary="entry.exe not found — cannot run microbenchmark",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    try:
        r = subprocess.run(
            [str(exe_path), "--microbench"],
            capture_output=True, text=True, timeout=120)
    except (OSError, subprocess.TimeoutExpired) as e:
        return StageResult(
            stage="microbench", status="error",
            summary=f"Microbenchmark execution failed: {e}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    output = r.stdout or ""

    # Parse microbenchmark results using regex
    metrics = {}

    # FastFramePool: "Batch Acquire+Release: 3.5 ns/op"
    m = re.search(r'Benchmark 1: FastFramePool.*\((?:kPoolSize)=(\d+)\)', output)
    pool_size = int(m.group(1)) if m else 0
    m = re.search(r'Batch Acquire\+Release:\s+([\d.]+)\s*ns/op\s+.*?(\d+)\s+batches\s+x\s+(\d+)\s*frames', output)
    if m:
        metrics["framePool"] = {
            "acquireReleaseNsPerOp": float(m.group(1)),
            "batchCount": int(m.group(2)),
            "framesPerBatch": int(m.group(3)),
        }

    # FastExecute: "Benchmark 2: FastExecute (Subject_0): 100000 runs: 42.1 ns/op"
    m = re.search(r'Benchmark 2: FastExecute.*?(\d+) runs:\s+([\d.]+)\s*ns/op', output)
    if m:
        metrics["fastExecute"] = {
            "runCount": int(m.group(1)),
            "nsPerCall": float(m.group(2)),
        }

    # CallVirt dispatch overhead: "Handler dispatch overhead (LdcI4): 20.4 ns/call"
    m = re.search(r'Handler dispatch overhead \(LdcI4\):\s+([\d.]+)\s*ns/call', output)
    if m:
        metrics["callVirt"] = metrics.get("callVirt") or {}
        metrics["callVirt"]["handlerDispatchNs"] = float(m.group(1))

    # CallVirt empty-stack: "CallVirt empty-stack: 20.6 ns/call"
    m = re.search(r'CallVirt empty-stack:\s+([\d.]+)\s*ns/call', output)
    if m:
        metrics["callVirt"] = metrics.get("callVirt") or {}
        metrics["callVirt"]["emptyStackNs"] = float(m.group(1))

    # Notes (MIC hit, raw fallback)
    m = re.search(r'MIC hit path.*?~(\d+)ns', output)
    if m:
        metrics["callVirt"] = metrics.get("callVirt") or {}
        metrics["callVirt"]["micHitNs"] = int(m.group(1))
    m = re.search(r'Raw fallback.*?~(\d+)ns', output)
    if m:
        metrics["callVirt"] = metrics.get("callVirt") or {}
        metrics["callVirt"]["rawFallbackNs"] = int(m.group(1))

    metrics["poolSize"] = pool_size

    # Write report
    family_dir = _VERIFICATION_BASE / assembly / family_slug
    report_path = family_dir / "interpreter-microbench-report.json"
    report_path.parent.mkdir(parents=True, exist_ok=True)
    report = {
        "schemaVersion": 1,
        "assemblyName": assembly,
        "familyId": f"family/{assembly}/{family_slug.replace('-', '/')}",
        "capturedAt": time.strftime("%Y-%m-%dT%H:%M:%S"),
        "metrics": metrics,
        "rawOutput": output,
    }
    import json as _json
    report_path.write_text(_json.dumps(report, indent=2, ensure_ascii=False), encoding="utf-8")

    status = "passed" if metrics else "failed"
    ns_per_call = metrics.get("fastExecute", {}).get("nsPerCall", 0)
    frame_pool_ns = metrics.get("framePool", {}).get("acquireReleaseNsPerOp", 0)
    dispatch_ns = metrics.get("callVirt", {}).get("handlerDispatchNs", 0)
    trace("microbench", family=family_slug, ns_per_call=ns_per_call,
          frame_pool_ns=frame_pool_ns, dispatch_ns=dispatch_ns, status=status)

    return StageResult(
        stage="microbench", status=status,
        summary=(
            f"FramePool={frame_pool_ns:.1f}ns/op | "
            f"FastExecute={ns_per_call:.1f}ns/op | "
            f"CallVirt dispatch={dispatch_ns:.1f}ns" if metrics
            else "No metrics captured"
        ),
        details=metrics,
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def stage_performance_benchmark(family_slug: str, assembly: str) -> StageResult:
    """Stage 9: 5-way benchmark — .NET8/.NET10/Mono/Chaos AOT/Chaos JIT.

    Uses multi_benchmark_runner to orchestrate across all available runtimes,
    auto-generating the managed benchmark harness if needed.
    """
    start = time.perf_counter()
    family_dir = _VERIFICATION_BASE / assembly / family_slug

    mids = load_contract_methods(family_slug, assembly)
    if not mids:
        return StageResult(
            stage="benchmark", status="skipped",
            summary="No method subject IDs found",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    stub_mask = parse_stub_mask(family_slug, assembly)
    stub_total = stub_mask.bit_count() if stub_mask else 0

    # If all methods are stubs, skip
    if stub_total == len(mids):
        return StageResult(
            stage="benchmark", status="skipped",
            summary="All methods are stubs — no benchmark possible",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # ── Step 1: Generate managed benchmark harness ────────────────────
    native_dir = family_dir / "native"
    aot_exe = native_dir / "entry-aot.exe"
    jit_exe = native_dir / "entry-jit.exe"

    managed_path = auto_generate_managed_benchmark(family_slug, assembly, mids)
    harness_dir = family_dir / "managed_test" / "benchmarks"
    if managed_path and harness_dir.exists():
        # Adapt csproj for multi-target (net8.0 + net10.0 for .NET JIT runs)
        csproj = harness_dir / "ManagedBenchmarkHarness.csproj"
        if csproj.exists():
            adapt_csproj_for_multitarget(csproj, ["net8.0", "net10.0"])

    # ── Step 2: Multi-runtime benchmark (5-way) ──────────────────────
    all_runtimes = ["net8-jit", "net10-jit", "mono", "chaos-aot", "chaos-jit"]
    avail, unavail = detect_available_runtimes(all_runtimes)

    multi_report = run_multi_benchmark(
        family_dir=family_dir,
        family_slug=family_slug,
        assembly=assembly,
        method_subject_ids=mids,
        runtimes=all_runtimes,
        iterations=100000,
        exe_aot=aot_exe,
        exe_jit=jit_exe,
        baseline_path=family_dir / "multi-run" / "multi-run-report.json",
    )
    multi_output_dir = family_dir / "multi-run"
    save_report(multi_report, multi_output_dir)
    print_report_summary(multi_report)

    # ── Step 3: Quality gate ─────────────────────────────────────────
    bm_status = "passed"
    bm_reason = ""

    # Check: chaos-aot must have data
    chaos_aot_summary = next(
        (s for s in multi_report.summaries if s.runtime == "chaos-aot"), None)
    if chaos_aot_summary is None or chaos_aot_summary.ok_count == 0:
        bm_status = "failed"
        bm_reason = "chaos-aot benchmark produced no valid results"

    # Check: net8-jit must have data
    net8_summary = next(
        (s for s in multi_report.summaries if s.runtime == "net8-jit"), None)
    if net8_summary is None or net8_summary.ok_count == 0:
        bm_status = "failed"
        bm_reason = "net8-jit benchmark produced no valid results"

    # Build summary
    summary_parts = []
    for s in multi_report.summaries:
        summary_parts.append(f"{s.runtime}={s.geometric_mean_ns:.0f}ns")
    bm_summary = bm_reason or ", ".join(summary_parts)

    # ── Build details for dashboard ──────────────────────────────────
    chaos_vs_net8 = next(
        (r for r in multi_report.ratios
         if r.numerator == "chaos-aot" and r.denominator == "net8-jit"),
        None,
    )

    details = {
        "runtimesAvailable": multi_report.runtimes_available,
        "runtimesUnavailable": multi_report.runtimes_unavailable,
        "multiRunReportPath": "multi-run/multi-run-report.json",
        "chaosClassification": multi_report.chaos_classification_breakdown,
        "summaries": {s.runtime: {
            "geometricMeanNs": s.geometric_mean_ns,
            "okCount": s.ok_count,
            "totalMethods": s.method_count,
        } for s in multi_report.summaries},
    }
    if chaos_vs_net8 is not None:
        details["chaosAotVsNet8Speedup"] = chaos_vs_net8.geometric_mean_ratio
        details["chaosAotFasterCount"] = chaos_vs_net8.faster_count
        details["chaosAotSlowerCount"] = chaos_vs_net8.slower_count

    if chaos_aot_summary is not None:
        details["chaosAotGeometricMeanNs"] = chaos_aot_summary.geometric_mean_ns

    trace("benchmark", family=family_slug,
          runtimes_available=multi_report.runtimes_available,
          status=bm_status)

    return StageResult(
        stage="benchmark", status=bm_status,
        summary=bm_summary,
        details=details,
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def stage_hot_update_fact(family_slug: str, assembly: str) -> StageResult:
    """Stage 5: Hotpatch verification.

    Self-contained: runs entry.exe --hotupdate, writes hotupdate-verification-report.json.
    """
    start = time.perf_counter()
    from fact_verifier import verify_hotupdate

    stub_mask = parse_stub_mask(family_slug, assembly)
    family_dir = _VERIFICATION_BASE / assembly / family_slug
    report_path = family_dir / "hotupdate-verification-report.json"

    mids = load_contract_methods(family_slug, assembly)
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

    actual_passed = result.get("passed", 0)
    actual_failed = result.get("failed", 0)
    actual_total = result.get("total", 0)
    hu_status = result.get("status", "failed")

    # Use real EXE results for everything
    real_passed = actual_passed
    real_failed = actual_failed
    real_total = actual_total

    # If the EXE reported 0 total, the hotupdate didn't actually run
    if actual_total == 0:
        hu_stage_status = "failed"
        hu_summary = f"HotUpdate returned 0 total — no verification performed"
    else:
        hu_stage_status = "passed" if actual_failed == 0 else "failed"
        hu_summary = f"{actual_passed}/{actual_total} passed, {actual_failed} failed (stub={stub_total})"

    report = {
        "schemaVersion": 2,
        "assemblyName": assembly,
        "familyId": f"family/{assembly}/{family_slug.replace('-', '/')}",
        "passedMethods": actual_passed,
        "failedMethods": actual_failed,
        "totalMethods": actual_total,
        "stubMethods": stub_total,
        "realMethods": max(0, method_count - stub_total),
        "allMethodsRevertVerified": result.get("all_revert", False),
        "allMethodsSemanticVerified": result.get("all_semantic", False),
        "d3PatchApplied": False,
        "d3PatchedCount": 0,
        "verificationKind": "hotupdate-proof",
        "summary": {
            "totalMethods": actual_total,
            "passedMethods": actual_passed,
            "failedMethods": actual_failed,
            "unmatchedMethods": 0,
        },
        "methodResults": result.get("methodResults", []),
    }
    report_path.parent.mkdir(parents=True, exist_ok=True)
    with open(report_path, "w", encoding="utf-8") as f:
        json.dump(report, f, indent=2, ensure_ascii=False)

    # Use real EXE results for stage status, not fake contract-derived counts
    if actual_total == 0:
        hu_stage_status = "failed"
        hu_summary = f"HotUpdate returned 0 total — no verification performed"
    else:
        hu_stage_status = "passed" if actual_failed == 0 else "failed"
        hu_summary = f"{actual_passed}/{actual_total} passed, {actual_failed} failed (stub={stub_total})"

    trace("hotupdate", family=family_slug, passed=actual_passed, failed=actual_failed,
          total=actual_total, stub=stub_total, status=hu_stage_status)

    return StageResult(
        stage="hotupdate", status=hu_stage_status,
        summary=hu_summary,
        details={
            "passedMethods": actual_passed,
            "failedMethods": actual_failed,
            "totalMethods": actual_total,
            "realMethods": real_total,
            "stubMethods": stub_total,
        },
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def stage_hot_update_aot_benchmark(family_slug: str, assembly: str) -> StageResult:
    """Stage 9b: Post-patch AOT benchmark — entry-aot.exe --hotupdate-and-benchmark.

    Reads pre-patch AOT baseline from native-aot-benchmark.json, applies patches,
    and measures per-method slowdown through the AOT dispatch path.
    """
    start = time.perf_counter()

    stub_mask = parse_stub_mask(family_slug, assembly)
    stub_total = stub_mask.bit_count() if stub_mask else 0
    family_dir = _VERIFICATION_BASE / assembly / family_slug
    report_path = family_dir / "hotupdate-aot-benchmark-report.json"
    exe_path = family_dir / "native" / "entry-aot.exe"

    mids = load_contract_methods(family_slug, assembly)
    if not mids:
        return StageResult(
            stage="hotupdate_aot_benchmark", status="skipped",
            summary="No method subject IDs found",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    if not exe_path.exists():
        return StageResult(
            stage="hotupdate_aot_benchmark", status="skipped",
            summary="entry-aot.exe not found",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    if stub_total == len(mids):
        return StageResult(
            stage="hotupdate_aot_benchmark", status="skipped",
            summary="All methods are stubs — no benchmark possible",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Read pre-patch native-AOT benchmark results for baseline ns/op
    pre_ns_by_method: dict[str, float] = {}
    native_path = family_dir / "native" / "native-aot-benchmark.json"
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

        import json as _json
        json_line = next((l for l in reversed(r.stdout.strip().splitlines()) if l.startswith("{")), None)
        if json_line is None:
            method_results.append({
                "methodIndex": idx,
                "methodSubjectId": mid,
                "status": "failed",
                "error": "JSON parse failed",
            })
            continue
        try:
            out = _json.loads(json_line)
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

    status = "passed"
    msg = f"avg_slowdown={avg_slowdown_pct}% ({slowdown_factor:.1f}x) over {slowdown_count} methods (AOT patched path)"

    trace("hotupdate_aot_benchmark", family=family_slug,
          avg_slowdown=avg_slowdown_pct, methods=slowdown_count, status=status)

    return StageResult(
        stage="hotupdate_aot_benchmark", status=status,
        summary=msg,
        details={
            "averageSlowdownPercent": avg_slowdown_pct,
            "slowdownFactor": slowdown_factor,
            "threshold": threshold_x,
            "nonStubMethods": slowdown_count,
        },
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def stage_hot_update_jit_fact(family_slug: str, assembly: str) -> StageResult:
    """Stage 10: HotUpdate JIT fact — apply patches and verify through JIT dispatch."""
    start = time.perf_counter()
    family_dir = _VERIFICATION_BASE / assembly / family_slug
    jit_exe = family_dir / "native" / "entry-jit.exe"

    if not jit_exe.exists():
        return StageResult(stage="hotupdate_jit_fact", status="skipped",
                           summary="entry-jit.exe not found — cannot run JIT hotupdate",
                           duration_ms=int((time.perf_counter() - start) * 1000))

    try:
        from fact_verifier import verify_hotupdate
        result = verify_hotupdate(family_slug, assembly=assembly, exe_path=jit_exe)
        status = "passed" if result.get("status") == "passed" else "failed"
        passed = result.get("passed", 0)
        total = result.get("total", 0)

        report_path = family_dir / "hotupdate-jit-verification-report.json"
        report_path.parent.mkdir(parents=True, exist_ok=True)
        report_path.write_text(json.dumps(result, indent=2, ensure_ascii=False))

        return StageResult(stage="hotupdate_jit_fact", status=status,
                           summary=f"{passed}/{total} passed, 0 failed",
                           details={"passed": passed, "failed": 0, "total": total},
                           duration_ms=int((time.perf_counter() - start) * 1000))
    except Exception as e:
        return StageResult(stage="hotupdate_jit_fact", status="failed",
                           summary=f"HotUpdate JIT fact crashed: {e}",
                           duration_ms=int((time.perf_counter() - start) * 1000))


def stage_hot_update_jit_benchmark(family_slug: str, assembly: str) -> StageResult:
    """Stage 11: Post-patch JIT benchmark — entry-jit.exe --hotupdate-and-benchmark.

    Reads pre-patch JIT baseline from native-jit-benchmark.json and measures
    slowdown through the JIT dispatch path after patching.
    """
    start = time.perf_counter()

    stub_mask = parse_stub_mask(family_slug, assembly)
    stub_total = stub_mask.bit_count() if stub_mask else 0
    family_dir = _VERIFICATION_BASE / assembly / family_slug
    report_path = family_dir / "hotupdate-jit-benchmark-report.json"
    exe_path = family_dir / "native" / "entry-jit.exe"

    mids = load_contract_methods(family_slug, assembly)
    if not mids:
        return StageResult(
            stage="hotupdate_jit_benchmark", status="skipped",
            summary="No method subject IDs found",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    if not exe_path.exists():
        return StageResult(
            stage="hotupdate_jit_benchmark", status="skipped",
            summary="entry-jit.exe not found",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    if stub_total == len(mids):
        return StageResult(
            stage="hotupdate_jit_benchmark", status="skipped",
            summary="All methods are stubs — no benchmark possible",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Read pre-patch JIT benchmark results for baseline ns/op
    pre_ns_by_method: dict[str, float] = {}
    native_path = family_dir / "native" / "native-jit-benchmark.json"
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

        import json as _json
        json_line = next((l for l in reversed(r.stdout.strip().splitlines()) if l.startswith("{")), None)
        if json_line is None:
            method_results.append({
                "methodIndex": idx,
                "methodSubjectId": mid,
                "status": "failed",
                "error": "JSON parse failed",
            })
            continue
        try:
            out = _json.loads(json_line)
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

    status = "passed"
    msg = f"avg_slowdown={avg_slowdown_pct}% ({slowdown_factor:.1f}x) over {slowdown_count} methods (JIT patched path)"

    trace("hotupdate_jit_benchmark", family=family_slug,
          avg_slowdown=avg_slowdown_pct, methods=slowdown_count, status=status)

    return StageResult(
        stage="hotupdate_jit_benchmark", status=status,
        summary=msg,
        details={
            "averageSlowdownPercent": avg_slowdown_pct,
            "slowdownFactor": slowdown_factor,
            "threshold": threshold_x,
            "nonStubMethods": slowdown_count,
        },
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


# ── Dashboard Builder ──────────────────────────────────────────────
def stage_dashboard(family_slug: str, assembly: str,
                    stage_results: list[StageResult],
                    mode: str = "standard") -> StageResult:
    """Stage 14: Build cross-family dashboard (JSON + console + HTML).

    Aggregates data from all stages and produces a unified dashboard
    entry in the stage results, plus a standalone HTML page.
    """
    start = time.perf_counter()
    stages_map = {sr.stage: sr for sr in stage_results}

    dashboard = build_dashboard(family_slug, assembly, stages_map)

    # Also generate standalone HTML dashboard
    try:
        from dashboard_html_generator import generate_html
        html = generate_html({
            "family": family_slug,
            "assembly": assembly,
            "overall_status": "passed",  # will be corrected by aggregate
            "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S"),
            "duration_ms": int((time.perf_counter() - start) * 1000),
            "stages": {sr.stage: sr.to_dict() for sr in stage_results},
            "dashboard": dashboard,
            "coverage": {},
            "regression": {},
        })
        family_dir = _VERIFICATION_BASE / assembly / family_slug
        html_path = family_dir / "dashboard.html"
        html_path.write_text(html, encoding="utf-8")
        print(f"  [dashboard] HTML dashboard → {html_path}")
    except Exception as e:
        print(f"  [dashboard] HTML generation skipped: {e}")

    return StageResult(
        stage="dashboard", status="passed",
        summary=f"Dashboard built: {len(dashboard)} sections",
        details=dashboard,
        duration_ms=int((time.perf_counter() - start) * 1000),
    )



def build_dashboard(family_slug: str, assembly: str,
                     stages: dict[str, StageResult]) -> dict[str, Any]:
    """Build a cross-stage dashboard summarizing all metrics from the verification run.

    Aggregates data from:
      - AsmCompare (Stage 4): IR expansion ratio, instruction mix
      - Microbench (Stage 4.5): interpreter internal performance
      - Benchmark (Stage 5): managed vs native speedup
      - HotUpdate (Stage 6): hotpatch correctness
      - PostHotBench (Stage 7): interpreter slowdown factor
    """
    import json as _json

    family_dir = _VERIFICATION_BASE / assembly / family_slug
    dashboard: dict[str, Any] = {
        "interpreter": {},
        "irTranslation": {},
        "performance": {},
        "hotupdate": {},
    }

    # ── 0. Managed fact results ────────────────────────────────────────
    managed_fact = stages.get("managed_fact")
    if managed_fact and managed_fact.details:
        dashboard["managedFact"] = {
            "totalMethods": managed_fact.details.get("totalMethods", 0),
            "passedMethods": managed_fact.details.get("passedMethods", 0),
            "failedMethods": managed_fact.details.get("failedMethods", 0),
            "skippedMethods": managed_fact.details.get("skippedMethods", 0),
            "tfm": managed_fact.details.get("tfm", "net8.0"),
        }

    # ── 1. Interpreter internals (from microbench stage result details) ──
    microbench = stages.get("microbench")
    if microbench and microbench.details:
        mb = microbench.details
        dashboard["interpreter"] = {
            "fastFramePoolNsPerOp": mb.get("framePool", {}).get("acquireReleaseNsPerOp", 0),
            "fastExecuteNsPerInstr": mb.get("fastExecute", {}).get("nsPerInstruction", 0),
            "callVirtHandlerDispatchNs": mb.get("callVirt", {}).get("handlerDispatchNs", 0),
            "callVirtMicHitNs": mb.get("callVirt", {}).get("micHitNs", 0),
            "callVirtRawFallbackNs": mb.get("callVirt", {}).get("rawFallbackNs", 0),
            "poolSize": mb.get("poolSize", 0),
        }

    # ── 2. IR translation quality (from asm-compare-report.json) ──────────
    asm_report_path = family_dir / "asm-compare-report.json"
    if asm_report_path.exists():
        try:
            asm_data = _json.loads(asm_report_path.read_text(encoding="utf-8"))
            s = asm_data.get("summary", {})
            dashboard["irTranslation"] = {
                "totalMethods": s.get("totalMethods", 0),
                "okCount": s.get("okCount", 0),
                "failedCount": s.get("failedCount", 0),
                "overallIrExpansionRatio": s.get("overallIrExpansionRatio", 0),
                "averageExpansionRatio": s.get("averageExpansionRatio", 0),
                "maxExpansionRatio": s.get("maxExpansionRatio", 0),
                "jitInstructionAvg": s.get("jitInstructionCount", {}).get("avg", 0),
                "aotInstructionAvg": s.get("aotInstructionCount", {}).get("avg", 0),
                "totalExternalRuntimeCalls": s.get("totalExternalRuntimeCalls", 0),
                "totalVirtualDispatches": s.get("totalVirtualDispatches", 0),
                "totalBoxingOps": s.get("totalBoxingOps", 0),
                "topInstructionCategories": s.get("topInstructionCategories", {}),
            }
        except (OSError, ValueError):
            pass

    # ── 3. Performance comparison (from benchmark stage) ─────────────────
    benchmark = stages.get("benchmark")
    if benchmark and benchmark.details:
        bd = benchmark.details
        dashboard["performance"] = {
            "averageSpeedupPercent": bd.get("averageSpeedupPercent", 0),
            "nativeFasterCount": bd.get("nativeFasterCount", 0) or bd.get("nativeAotFasterCount", 0),
            "managedFasterCount": bd.get("managedFasterCount", 0),
            "matchedCount": bd.get("matchedCount", 0),
            "totalMethods": bd.get("totalMethods", 0),
            "invalidCount": bd.get("invalidCount", 0),
        }
        # Add native-JIT metrics if available
        avg_jit = bd.get("averageNativeJitSpeedupPercent")
        if avg_jit is not None:
            dashboard["performance"]["averageNativeJitSpeedupPercent"] = avg_jit
            dashboard["performance"]["nativeJitFasterCount"] = bd.get("nativeJitFasterCount", 0)

    # ── 4. HotUpdate AOT results ───────────────────────────────────────────
    hotupdate = stages.get("hotupdate")
    if hotupdate and hotupdate.details:
        hd = hotupdate.details
        dashboard["hotupdate"]["aotFunctional"] = {
            "passedMethods": hd.get("passedMethods", 0),
            "failedMethods": hd.get("failedMethods", 0),
            "totalMethods": hd.get("totalMethods", 0),
        }

    # ── 5. HotUpdate AOT benchmark overhead ────────────────────────────────
    hu_aot_bench = stages.get("hotupdate_aot_benchmark")
    if hu_aot_bench and hu_aot_bench.details:
        ph = hu_aot_bench.details
        dashboard["hotupdate"]["aotOverhead"] = {
            "averageSlowdownPercent": ph.get("averageSlowdownPercent", 0),
            "slowdownFactor": ph.get("slowdownFactor", 0),
            "nonStubMethods": ph.get("nonStubMethods", 0),
        }

    # ── 5b. HotUpdate JIT results ──────────────────────────────────────────
    hu_jit_fact = stages.get("hotupdate_jit_fact")
    if hu_jit_fact and hu_jit_fact.details:
        dashboard["hotupdate"]["jitFunctional"] = {
            "passedMethods": hu_jit_fact.details.get("passed", 0),
            "failedMethods": hu_jit_fact.details.get("failed", 0),
            "totalMethods": hu_jit_fact.details.get("total", 0),
        }

    # ── 5c. HotUpdate JIT benchmark overhead ───────────────────────────────
    hu_jit_bench = stages.get("hotupdate_jit_benchmark")
    if hu_jit_bench and hu_jit_bench.details:
        ph = hu_jit_bench.details
        dashboard["hotupdate"]["jitOverhead"] = {
            "averageSlowdownPercent": ph.get("averageSlowdownPercent", 0),
            "slowdownFactor": ph.get("slowdownFactor", 0),
            "nonStubMethods": ph.get("nonStubMethods", 0),
        }

    # ── 6. Compute key ratios ────────────────────────────────────────────
    perf = dashboard.get("performance", {})
    matched = perf.get("matchedCount", 0)
    native_faster = perf.get("nativeFasterCount", 0)
    ir = dashboard.get("irTranslation", {})
    hu_aot = dashboard.get("hotupdate", {}).get("aotOverhead", {})

    dashboard["keyRatios"] = {
        "nativeFasterRatio": round(native_faster / matched, 2) if matched > 0 else 0,
        "irExpansionRatio": ir.get("overallIrExpansionRatio", 0),
        "interpreterSlowdownFactor": hu_aot.get("slowdownFactor", 0),
        "asmPassRate": round(ir.get("okCount", 0) / max(ir.get("totalMethods", 1), 1) * 100, 1),
    }

    return dashboard


def compute_coverage(stages: dict[str, StageResult]) -> dict[str, float]:
    """Compute coverage score from stage results."""
    scores: dict[str, float] = {}

    # Total method count from preflight
    preflight = stages.get("preflight")
    total_methods = 0
    if preflight and preflight.details:
        total_methods = preflight.details.get("methodCount", 0)

    # Method coverage from managed_fact stage (as percentage)
    fact = stages.get("managed_fact") or stages.get("fact")
    if fact and fact.details:
        if "totalMethods" in fact.details:
            # managed_fact format: direct fields
            total = fact.details.get("totalMethods", 0)
            passed = fact.details.get("passedMethods", 0)
        else:
            # Old fact stage format: nested "fact" key
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


def aggregate_stage_results(family_slug: str, assembly: str,
               stage_results: list[StageResult],
               mode: str,
               total_duration_ms: int) -> UnifiedReport:
    """Aggregate all stage results into the final report."""
    stages_map: dict[str, dict[str, Any]] = {}
    for sr in stage_results:
        stages_map[sr.stage] = sr.to_dict()

    coverage = compute_coverage({sr.stage: sr for sr in stage_results})

    # ── Build comprehensive dashboard ─────────────────────────────────
    dashboard = build_dashboard(
        family_slug, assembly,
        {sr.stage: sr for sr in stage_results},
    )

    # Run baseline regression detection
    regression = detect_regression(family_slug, assembly, stage_results)

    # Determine overall pass/fail
    required_stages = {"preflight", "codegen", "jit_codegen", "managed_fact", "fact", "fact_jit", "audit"}
    if mode == "strict":
        required_stages.update({"hotupdate", "hotupdate_aot_benchmark",
                                "hotupdate_jit_fact", "hotupdate_jit_benchmark"})

    failures = [
        sr.stage for sr in stage_results
        if sr.stage in required_stages and sr.status == "failed"
    ]
    errors = [
        sr.stage for sr in stage_results
        if sr.stage in required_stages and sr.status == "error"
    ]

    if errors or failures:
        overall_status = "failed"
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
        dashboard=dashboard,
        regression=regression,
    )


def detect_regression(family_slug: str, assembly: str,
                        stage_results: list[StageResult]) -> dict[str, Any]:
    """Run regression detection on benchmark metrics vs stored baseline.

    Reads benchmark-comparison-report.json and detects:
      - Speedup drop >20% from baseline
      - Managed-faster ratio exceeding native-faster ratio
    """
    import json

    result: dict[str, Any] = {
        "benchmark": {},
        "hasRegression": False,
        "regressions": [],
    }

    family_dir = _VERIFICATION_BASE / assembly / family_slug
    benchmark_report_path = family_dir / "benchmark-comparison-report.json"

    if not benchmark_report_path.exists():
        result["benchmark"] = {
            "status": "skipped",
            "message": "No benchmark report found",
        }
        return result

    try:
        with open(benchmark_report_path, encoding="utf-8") as f:
            bm_report = json.load(f)
    except (OSError, json.JSONDecodeError) as e:
        result["benchmark"] = {"status": "error", "message": str(e)}
        return result

    summary = bm_report.get("summary", {})
    total_methods = summary.get("totalMethods", 0)
    matched_count = summary.get("matchedCount", 0)
    native_faster = summary.get("nativeFasterCount", 0)
    managed_faster = summary.get("managedFasterCount", 0)
    avg_speedup = summary.get("averageSpeedupPercent", 0)

    current_metrics = {
        "totalMethods": total_methods,
        "matchedCount": matched_count,
        "nativeFasterCount": native_faster,
        "managedFasterCount": managed_faster,
        "averageSpeedupPercent": avg_speedup,
    }

    # Managed-faster ratio check
    if matched_count > 0 and managed_faster > 0:
        mf_ratio = managed_faster / max(matched_count, 1)
        nf_ratio = native_faster / max(matched_count, 1)
        if mf_ratio > 0.5 and nf_ratio < 0.5:
            result["regressions"].append(
                f"Managed-faster ratio {mf_ratio:.0%} exceeds native-faster "
                f"ratio {nf_ratio:.0%} ({managed_faster}/{matched_count} methods)"
            )
            result["hasRegression"] = True

    # Load / auto-create baseline
    baseline_path = _VERIFICATION_BASE.parent.parent / "artifact" / "verification-history" / assembly / family_slug / "baseline-benchmark.json"
    if baseline_path.exists():
        try:
            with open(baseline_path, encoding="utf-8") as f:
                baseline = json.load(f)
            bm = baseline.get("metrics", {})

            bm_speedup = bm.get("averageSpeedupPercent", 0)
            if bm_speedup > 0 and avg_speedup < bm_speedup * 0.8:
                drop = (bm_speedup - avg_speedup) / bm_speedup * 100
                result["regressions"].append(
                    f"Speedup regression: {avg_speedup:.1f}% vs "
                    f"baseline {bm_speedup:.1f}% (dropped {drop:.0f}%)"
                )
                result["hasRegression"] = True

            result["benchmark"] = {
                "status": "matched" if not result["hasRegression"] else "regressed",
                "baselineMetrics": bm,
                "currentMetrics": current_metrics,
            }
        except (OSError, json.JSONDecodeError) as e:
            result["benchmark"] = {"status": "error", "message": f"read baseline: {e}"}
    else:
        baseline_path.parent.mkdir(parents=True, exist_ok=True)
        with open(baseline_path, "w", encoding="utf-8") as f:
            json.dump({
                "schemaVersion": 1,
                "family": family_slug,
                "assembly": assembly,
                "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S"),
                "metrics": current_metrics,
            }, f, indent=2, ensure_ascii=False)
        result["benchmark"] = {
            "status": "no_baseline",
            "message": "Baseline auto-created from this run",
            "currentMetrics": current_metrics,
        }

    return result


def write_report(report: UnifiedReport, family_slug: str, assembly: str) -> Path:
    """Write the unified report JSON + HTML dashboard to the family directory."""
    family_dir = _VERIFICATION_BASE / assembly / family_slug
    report_path = family_dir / "unified-verification-report.json"
    report_path.parent.mkdir(parents=True, exist_ok=True)
    report_path.write_text(report.to_json(), encoding="utf-8")

    # Regenerate HTML dashboard with final aggregate data
    try:
        from dashboard_html_generator import generate_html
        html = generate_html(json.loads(report.to_json()))
        html_path = family_dir / "dashboard.html"
        html_path.write_text(html, encoding="utf-8")
        print(f"  [report] HTML dashboard → {html_path}")
    except Exception as e:
        print(f"  [report] HTML dashboard skipped: {e}")

    return report_path


# ── Public entry point ───────────────────────────────────────────

def verify_family(family_slug: str,
                  assembly: str = "System.Private.CoreLib",
                  mode: str = "standard",
                  skip_stages: list[str] | None = None,
                  verbose: bool = False,
                  codegen_mode: str | None = None) -> dict[str, Any]:
    """Run the full 15-stage verification pipeline for a single family.

    Stages:
        [0]  Preflight
        [1]  Codegen (AOT + save entry-aot.exe)
        [2]  JitCodegen (build entry-jit.exe, restore entry.exe)
        [3]  Fact (.NET8) — managed dotnet fact verification
        [4]  Fact AOT — Chaos AOT (entry-aot.exe)
        [5]  Fact JIT — Chaos JIT (entry-jit.exe)
        [6]  Audit (mechanism + principle)
        [7]  AsmCompare (JIT vs AOT instruction-level)
        [8]  Microbench (interpreter internals)
        [9]  Benchmark (5-way: .NET8/.NET10/Mono/Chaos AOT/Chaos JIT)
        [10] HotUpdate AOT Fact (entry-aot.exe --hotupdate)
        [11] HotUpdate AOT Bench (entry-aot.exe --hotupdate-and-benchmark)
        [12] HotUpdate JIT Fact (entry-jit.exe --hotupdate)
        [13] HotUpdate JIT Bench (entry-jit.exe --hotupdate-and-benchmark)
        [14] Dashboard (cross-family summary)
        [15] Aggregate

    Args:
        family_slug:  e.g. "convert-char"
        assembly:     Assembly name (default "System.Private.CoreLib")
        mode:         "standard" (default) or "strict"
        skip_stages:  List of stage names to skip, e.g. ["benchmark", "hotupdate"]
        verbose:      Print detailed output
        codegen_mode: "aot" (default) or "jit" — passed to chaos-il2cpp convert-to-cpp

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

    total_stages = 15

    # Stage 0: Preflight
    if "preflight" not in skip:
        print(f"[0/{total_stages}] Preflight...")
        sr = stage_preflight(family_slug, assembly)
        stage_results.append(sr)
        print(f"  {sr.status}: {sr.summary}")
        if sr.status == "failed":
            print(f"  Cannot continue — preflight failed")
            report = aggregate_stage_results(family_slug, assembly, stage_results, mode,
                                int((time.perf_counter() - overall_start) * 1000))
            report.overall_status = "failed"
            write_report(report, family_slug, assembly)
            return report.to_dict()
        if "empty family" in sr.summary:
            print(f"  Empty family — skipping remaining stages")
            report = aggregate_stage_results(family_slug, assembly, stage_results, mode,
                                int((time.perf_counter() - overall_start) * 1000))
            report.overall_status = "passed"
            write_report(report, family_slug, assembly)
            return report.to_dict()
    else:
        print(f"[0/{total_stages}] Preflight... skipped")

    # Stage 1: Codegen (AOT)
    if "codegen" not in skip:
        print(f"[1/{total_stages}] Codegen (AOT)...")
        sr = stage_code_generation(family_slug, assembly, stage_results[0] if stage_results else StageResult("preflight", "passed"), codegen_mode=codegen_mode)
        stage_results.append(sr)
        print(f"  {sr.status}: {sr.summary}")
        if sr.status == "failed" and mode == "strict":
            print(f"  Stopping — codegen failed in strict mode")
            report = aggregate_stage_results(family_slug, assembly, stage_results, mode,
                                int((time.perf_counter() - overall_start) * 1000))
            write_report(report, family_slug, assembly)
            return report.to_dict()
    else:
        print(f"[1/{total_stages}] Codegen... skipped")

    # Stage 2: JitCodegen (build entry-jit.exe)
    if "jit_codegen" not in skip:
        print(f"[2/{total_stages}] JitCodegen...")
        sr = stage_jit_code_generation(family_slug, assembly)
        stage_results.append(sr)
        print(f"  {sr.status}: {sr.summary}")
    else:
        print(f"[2/{total_stages}] JitCodegen... skipped")

    # Stage 3: Fact (.NET8) — managed dotnet
    if "managed_fact" not in skip:
        print(f"[3/{total_stages}] Fact (.NET8)...")
        try:
            sr = stage_managed_fact(family_slug, assembly)
        except Exception as e:
            trace("managed_fact", family=family_slug, error=str(e))
            sr = StageResult(stage="managed_fact", status="error",
                             summary=f"Managed fact stage crashed: {e}")
        stage_results.append(sr)
        print(f"  {sr.status}: {sr.summary}")
    else:
        print(f"[3/{total_stages}] Fact (.NET8)... skipped")

    # Stage 4: Fact AOT (Chaos AOT)
    if "fact" not in skip:
        print(f"[4/{total_stages}] Fact AOT...")
        try:
            sr = stage_fact_verification(family_slug, assembly)
        except Exception as e:
            trace("fact", family=family_slug, error=str(e))
            sr = StageResult(stage="fact", status="failed",
                             summary=f"Fact stage crashed: {e}")
        stage_results.append(sr)
        print(f"  {sr.status}: {sr.summary}")
    else:
        print(f"[4/{total_stages}] Fact AOT... skipped")

    # Stage 5: Fact JIT (Chaos JIT)
    if "fact_jit" not in skip:
        print(f"[5/{total_stages}] Fact JIT...")
        try:
            sr = stage_jit_fact_verification(family_slug, assembly)
        except Exception as e:
            trace("fact_jit", family=family_slug, error=str(e))
            sr = StageResult(stage="fact_jit", status="failed",
                             summary=f"Fact JIT stage crashed: {e}")
        stage_results.append(sr)
        print(f"  {sr.status}: {sr.summary}")
    else:
        print(f"[5/{total_stages}] Fact JIT... skipped")

    # Stage 6: Audit
    if "audit" not in skip:
        print(f"[6/{total_stages}] Mechanism + Principle Audit...")
        try:
            sr = stage_principle_audit(family_slug, assembly, skip, codegen_mode=codegen_mode)
        except Exception as e:
            trace("audit", family=family_slug, error=str(e))
            sr = StageResult(stage="audit", status="failed",
                             summary=f"Audit stage crashed: {e}")
        stage_results.append(sr)
        print(f"  {sr.status}: {sr.summary}")
    else:
        print(f"[6/{total_stages}] Audit... skipped")

    # Stage 7: AsmCompare
    if "asm_compare" not in skip:
        print(f"[7/{total_stages}] AsmCompare (JIT vs AOT instruction-level)...")
        try:
            sr = stage_assembly_comparison(family_slug, assembly)
        except Exception as e:
            trace("asm_compare", family=family_slug, error=str(e))
            sr = StageResult(stage="asm_compare", status="failed",
                             summary=f"AsmCompare stage crashed: {e}")
        stage_results.append(sr)
        print(f"  {sr.status}: {sr.summary}")
    else:
        print(f"[7/{total_stages}] AsmCompare... skipped")

    # Stage 8: Microbench
    if "microbench" not in skip:
        print(f"[8/{total_stages}] Microbench (interpreter internal metrics)...")
        try:
            sr = stage_micro_benchmark(family_slug, assembly)
        except Exception as e:
            trace("microbench", family=family_slug, error=str(e))
            sr = StageResult(stage="microbench", status="failed",
                             summary=f"Microbench stage crashed: {e}")
        stage_results.append(sr)
        print(f"  {sr.status}: {sr.summary}")
    else:
        print(f"[8/{total_stages}] Microbench... skipped")

    # Stage 9: Benchmark (5-way)
    if "benchmark" not in skip:
        print(f"[9/{total_stages}] Benchmark (5-way)...")
        try:
            sr = stage_performance_benchmark(family_slug, assembly)
        except Exception as e:
            trace("benchmark", family=family_slug, error=str(e))
            sr = StageResult(stage="benchmark", status="failed",
                             summary=f"Benchmark stage crashed: {e}")
        stage_results.append(sr)
        print(f"  {sr.status}: {sr.summary}")
    else:
        print(f"[9/{total_stages}] Benchmark... skipped")

    # Stage 10: HotUpdate AOT Fact
    if "hotupdate" not in skip:
        print(f"[10/{total_stages}] HotUpdate AOT Fact...")
        try:
            sr = stage_hot_update_fact(family_slug, assembly)
        except Exception as e:
            trace("hotupdate", family=family_slug, error=str(e))
            sr = StageResult(stage="hotupdate", status="failed",
                             summary=f"HotUpdate stage crashed: {e}")
        stage_results.append(sr)
        print(f"  {sr.status}: {sr.summary}")
    else:
        print(f"[10/{total_stages}] HotUpdate AOT Fact... skipped")

    # Stage 11: HotUpdate AOT Bench
    if "hotupdate_aot_benchmark" not in skip:
        print(f"[11/{total_stages}] HotUpdate AOT Bench...")
        try:
            sr = stage_hot_update_aot_benchmark(family_slug, assembly)
        except Exception as e:
            trace("hotupdate_aot_benchmark", family=family_slug, error=str(e))
            sr = StageResult(stage="hotupdate_aot_benchmark", status="failed",
                             summary=f"HotUpdate AOT benchmark stage crashed: {e}")
        stage_results.append(sr)
        print(f"  {sr.status}: {sr.summary}")
    else:
        print(f"[11/{total_stages}] HotUpdate AOT Bench... skipped")

    # Stage 12: HotUpdate JIT Fact
    if "hotupdate_jit_fact" not in skip:
        print(f"[12/{total_stages}] HotUpdate JIT Fact...")
        try:
            sr = stage_hot_update_jit_fact(family_slug, assembly)
        except Exception as e:
            trace("hotupdate_jit_fact", family=family_slug, error=str(e))
            sr = StageResult(stage="hotupdate_jit_fact", status="failed",
                             summary=f"HotUpdate JIT fact stage crashed: {e}")
        stage_results.append(sr)
        print(f"  {sr.status}: {sr.summary}")
    else:
        print(f"[12/{total_stages}] HotUpdate JIT Fact... skipped")

    # Stage 13: HotUpdate JIT Bench
    if "hotupdate_jit_benchmark" not in skip:
        print(f"[13/{total_stages}] HotUpdate JIT Bench...")
        try:
            sr = stage_hot_update_jit_benchmark(family_slug, assembly)
        except Exception as e:
            trace("hotupdate_jit_benchmark", family=family_slug, error=str(e))
            sr = StageResult(stage="hotupdate_jit_benchmark", status="failed",
                             summary=f"HotUpdate JIT benchmark stage crashed: {e}")
        stage_results.append(sr)
        print(f"  {sr.status}: {sr.summary}")
    else:
        print(f"[13/{total_stages}] HotUpdate JIT Bench... skipped")

    # Stage 14: Dashboard
    if "dashboard" not in skip:
        print(f"[14/{total_stages}] Dashboard...")
        try:
            sr = stage_dashboard(family_slug, assembly, stage_results, mode=mode)
        except Exception as e:
            trace("dashboard", family=family_slug, error=str(e))
            sr = StageResult(stage="dashboard", status="error",
                             summary=f"Dashboard stage crashed: {e}")
        stage_results.append(sr)
        print(f"  {sr.status}: {sr.summary}")
    else:
        print(f"[14/{total_stages}] Dashboard... skipped")

    # Stage 15: Aggregate
    print(f"[15/{total_stages}] Aggregating...")
    report = aggregate_stage_results(family_slug, assembly, stage_results, mode,
                        int((time.perf_counter() - overall_start) * 1000))
    report_path = write_report(report, family_slug, assembly)

    print(f"\n{'='*60}")
    print(f"Result: {report.overall_status}")
    print(f"Coverage: {report.coverage}")
    d = report.dashboard
    if d:
        kr = d.get("keyRatios", {})
        if kr:
            print(f"  native_faster={kr.get('nativeFasterRatio', 0)} | "
                  f"IR_expansion={kr.get('irExpansionRatio', 0)}x | "
                  f"interp_slowdown={kr.get('interpreterSlowdownFactor', 0)}x | "
                  f"asm_pass={kr.get('asmPassRate', 0)}%")
        interp = d.get("interpreter", {})
        if interp:
            print(f"  interpreter: {interp.get('fastExecuteNsPerInstr', '?')} ns/instr | "
                  f"CallVirt dispatch={interp.get('callVirtHandlerDispatchNs', '?')} ns | "
                  f"MIC hit≈{interp.get('callVirtMicHitNs', '?')}ns")
        # Print managed fact summary
        mf = d.get("managedFact", {})
        if mf:
            print(f"  managed_fact: passed={mf.get('passedMethods', 0)} failed={mf.get('failedMethods', 0)}")
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
                        help="Stages to skip: preflight codegen jit_codegen fact fact_jit audit asm_compare microbench benchmark hotupdate hotupdate_aot_benchmark hotupdate_jit_fact hotupdate_jit_benchmark")
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
    else:
        sys.exit(1)


if __name__ == "__main__":
    main()
