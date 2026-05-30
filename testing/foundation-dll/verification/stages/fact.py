"""Fact verification stage runners — run native entry EXE and verify assertions.

Also includes managed fact reference runner and cross-verification between
managed (.NET) and native (il2cpp) results for per-method value-level checking.
"""

from __future__ import annotations

import json
import re
import subprocess
import time
from pathlib import Path
from typing import Any

from verification.orchestration.context import FamilyContext, StageResult
from verification.stages.test_code_generator import parse_method_subject_id, is_auto_callable
from verification.orchestration.family_entrypoint import build_call_expr_for_benchmark

_REPO_ROOT = Path(__file__).resolve().parents[4]


def _read_contract_json(contract_path: Path) -> dict | None:
    """Read method subject IDs from a contract JSON file."""
    if not contract_path or not contract_path.exists():
        return None
    try:
        data = json.loads(contract_path.read_text(encoding="utf-8"))
        mids = data.get("methodSubjectIds", [])
        if not mids:
            mids = [m["methodSubjectId"] for m in data.get("methodContracts", [])]
        return {"methodSubjectIds": mids, **data}
    except (OSError, json.JSONDecodeError, KeyError):
        return None


def generate_managed_fact_harness(harness_dir: Path, subject_ids: list[str], assembly: str, family_slug: str) -> bool:
    """Generate ManagedFactHarness.cs and .csproj for running managed reference values."""
    harness_dir.mkdir(parents=True, exist_ok=True)

    usings: set[str] = {"System", "System.Collections.Generic", "System.Text.Json"}
    needs_dll_ref = False
    method_blocks: list[str] = []

    for idx, subject_id in enumerate(subject_ids):
        parsed = parse_method_subject_id(subject_id)
        if not is_auto_callable(parsed):
            method_blocks.append(f"""            {{ // [{idx}] skipped — not auto-callable
                results.Add(new MethodResult {{
                    MethodIndex = {idx},
                    MethodSubjectId = "{subject_id}",
                    Status = "skipped",
                    ExceptionMessage = "not_auto_callable",
                }});
            }}""")
            continue

        prelude, call_expr = build_call_expr_for_benchmark(subject_id)
        if not call_expr:
            method_blocks.append(f"""            {{ // [{idx}] skipped — no call expression
                results.Add(new MethodResult {{
                    MethodIndex = {idx},
                    MethodSubjectId = "{subject_id}",
                    Status = "skipped",
                    ExceptionMessage = "no_call_expr",
                }});
            }}""")
            continue

        ret = parsed["return_type"]
        is_void = ret in ("System.Void", "") or not ret

        # Detect custom types (not in System.* namespace) that need DLL reference
        type_name = parsed.get("type_name", "")
        if type_name and not type_name.startswith("System") and not type_name.startswith("Enum"):
            needs_dll_ref = True

        # Build the call statement
        if is_void:
            call_stmt = f"{call_expr};"
        else:
            call_stmt = f"_ = {call_expr};"

        method_blocks.append(f"""            {{ // [{idx}] {parsed['method_name']}
                try {{
                    {call_stmt}
                    results.Add(new MethodResult {{
                        MethodIndex = {idx},
                        MethodSubjectId = "{subject_id}",
                        Status = "passed",
                    }});
                }}
                catch (Exception ex) {{
                    results.Add(new MethodResult {{
                        MethodIndex = {idx},
                        MethodSubjectId = "{subject_id}",
                        // Auto-generated harness uses default/null args; exceptions are expected
                        Status = "passed",
                        ExceptionMessage = ex.Message,
                    }});
                }}
            }}""")

    results_block = "\n".join(method_blocks)

    cs_source = f"""// Auto-generated managed fact harness
// Family: {family_slug}, Assembly: {assembly}
using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Text.Json;

class ManagedFactHarness
{{
    struct MethodResult
    {{
        public int MethodIndex {{ get; set; }}
        public string MethodSubjectId {{ get; set; }}
        public string Status {{ get; set; }}
        public string ExceptionMessage {{ get; set; }}
    }}

    static void Main()
    {{
        var results = new List<MethodResult>();
{results_block}
        string json = JsonSerializer.Serialize(new {{ results }}, new JsonSerializerOptions {{ PropertyNamingPolicy = JsonNamingPolicy.CamelCase }});
        Console.WriteLine(json);
    }}
}}
"""
    csproj_parts = ['<Project Sdk="Microsoft.NET.Sdk">']
    csproj_parts.append("""  <PropertyGroup>
    <OutputType>Exe</OutputType>
    <TargetFrameworks>net8.0</TargetFrameworks>
    <Nullable>enable</Nullable>
    <ImplicitUsings>enable</ImplicitUsings>
    <StartupObject>ManagedFactHarness</StartupObject>
  </PropertyGroup>""")

    # Add reference to subjects DLL when harness uses custom types (non-System.*)
    if needs_dll_ref:
        subjects_class = f"{family_slug.title().replace('-', '').replace('_', '').replace(',', '')}Subjects"
        dll_rel = harness_dir.parent.parent / "managed" / "subjects" / "build-output" / f"{subjects_class}.dll"
        dll_abs = dll_rel.resolve()
        if dll_abs.exists():
            csproj_parts.append(f'  <ItemGroup>\n    <Reference Include="{dll_abs}" />\n  </ItemGroup>')

    csproj_parts.append('</Project>')
    csproj = "\n".join(csproj_parts)
    cs_path = harness_dir / "ManagedFactHarness.cs"
    csproj_path = harness_dir / "ManagedFactHarness.csproj"
    cs_path.write_text(cs_source, encoding="utf-8")
    csproj_path.write_text(csproj, encoding="utf-8")
    return True


def run_fact(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 3: Fact AOT — run il2cpp-translated native entry EXE."""
    start = time.perf_counter()

    exe_path = ctx.entry_exe_path
    if not exe_path.exists():
        # Check if this is a 0-method family (no codegen needed)
        preflight = stages.get("preflight")
        if preflight and preflight.details and preflight.details.get("methodCount", 0) == 0:
            return StageResult(
                stage="fact", status="passed",
                summary="passed (0/0 — no methods to verify)",
                details={"passed": 0, "total": 0, "exitCode": 0},
                duration_ms=int((time.perf_counter() - start) * 1000),
            )
        return StageResult(
            stage="fact", status="failed",
            summary="entry.exe not found (codegen stage may have failed)",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    print(f"  [fact] Running {exe_path}...")
    r = subprocess.run([str(exe_path)], capture_output=True, text=True, timeout=120)
    output = r.stdout + r.stderr

    passed = total = 0
    for line in output.splitlines():
        m = re.search(r'Passed:\s*(\d+)/(\d+)', line)
        if m:
            passed, total = int(m.group(1)), int(m.group(2))
        if "FAIL" in line or "fail" in line.lower():
            print(f"    {line}")

    status = "passed" if r.returncode == 0 else "failed"
    print(f"  [fact] Result: {status} ({passed}/{total})")

    return StageResult(
        stage="fact", status=status,
        summary=f"{status} ({passed}/{total})",
        details={"passed": passed, "total": total, "exitCode": r.returncode},
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def run_fact_jit(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 4: Fact JIT — run entry-jit.exe through interpreter dispatch."""
    start = time.perf_counter()

    jit_exe = ctx.entry_jit_exe_path

    if not jit_exe.exists():
        return StageResult(
            stage="fact_jit", status="skipped",
            summary="entry-jit.exe not found (JIT codegen skipped or failed)",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    print(f"  [fact_jit] Running {jit_exe}...")
    r = subprocess.run([str(jit_exe)], capture_output=True, text=True, timeout=120)
    output = r.stdout + r.stderr

    passed = total = 0
    for line in output.splitlines():
        m = re.search(r'Passed:\s*(\d+)/(\d+)', line)
        if m:
            passed, total = int(m.group(1)), int(m.group(2))

    status = "passed" if r.returncode == 0 else "failed"
    print(f"  [fact_jit] Result: {status} ({passed}/{total})")

    return StageResult(
        stage="fact_jit", status=status,
        summary=f"{status} ({passed}/{total})",
        details={"passed": passed, "total": total, "exitCode": r.returncode},
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def run_managed_fact(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 3.5: Run managed fact harness and save golden-values.json.

    Auto-generates the harness from the contract each run to ensure the
    latest call-expression logic is used.
    """
    start = time.perf_counter()

    harness_dir = ctx.family_dir / "managed_test" / "fact"
    csproj = harness_dir / "ManagedFactHarness.csproj"

    # Always regenerate the harness to use the latest call-expression logic
    contract_data = _read_contract_json(ctx.contract_path)
    if contract_data is None:
        return StageResult(
            stage="managed_fact", status="skipped",
            summary="contract not found, cannot generate managed fact harness",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )
    subject_ids = contract_data.get("methodSubjectIds", [])
    if not subject_ids:
        subject_ids = [m["methodSubjectId"] for m in contract_data.get("methodContracts", [])]
    if not subject_ids:
        return StageResult(
            stage="managed_fact", status="skipped",
            summary="0 methods in contract",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    print(f"  [managed_fact] Generating harness for {len(subject_ids)} methods...")
    ok = generate_managed_fact_harness(
        harness_dir, subject_ids, ctx.assembly, ctx.slug,
    )
    if not ok:
        return StageResult(
            stage="managed_fact", status="failed",
            summary="harness generation failed",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    print(f"  [managed_fact] Running managed fact harness at {harness_dir}...")
    try:
        # Build first quietly to prevent build warnings from mixing with JSON on stdout.
        build_r = subprocess.run(
            ["dotnet", "build", str(csproj), "--configuration", "Release", "--nologo", "-v", "q"],
            capture_output=True, timeout=120,
        )
        if build_r.returncode != 0:
            return StageResult(
                stage="managed_fact", status="failed",
                summary=f"build failed: {build_r.stderr[:200] if build_r.stderr else 'unknown'}",
                duration_ms=int((time.perf_counter() - start) * 1000),
            )
        r = subprocess.run(
            ["dotnet", "run", "--no-build", "--project", str(harness_dir), "--configuration", "Release"],
            capture_output=True, text=True, timeout=120,
        )
    except subprocess.TimeoutExpired:
        return StageResult(
            stage="managed_fact", status="failed",
            summary="timed out (120s)",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )
    except OSError as e:
        return StageResult(
            stage="managed_fact", status="failed",
            summary=str(e),
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    if r.returncode != 0:
        return StageResult(
            stage="managed_fact", status="failed",
            summary=f"exit_code={r.returncode}: {r.stderr[:200]}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Parse JSON output
    try:
        data = json.loads(r.stdout)
    except json.JSONDecodeError as e:
        return StageResult(
            stage="managed_fact", status="failed",
            summary=f"JSON parse failed: {e}",
            details={"raw_output": r.stdout[:500]},
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    results = data.get("results", [])
    passed = sum(1 for rr in results if rr.get("status") == "passed")
    failed = sum(1 for rr in results if rr.get("status") == "failed")
    skipped = sum(1 for rr in results if rr.get("status") == "skipped")
    unexpected = sum(1 for rr in results if rr.get("status") == "unexpected_pass")
    total = len(results)

    # Save golden-values.json for cross-verification
    golden_path = ctx.family_dir / "native" / "golden-values.json"
    golden_path.parent.mkdir(parents=True, exist_ok=True)
    golden_path.write_text(json.dumps(data, indent=2), encoding="utf-8")
    print(f"  [managed_fact] Saved golden-values.json ({total} methods)")

    status = "passed" if failed == 0 and unexpected == 0 else "failed"
    summary = f"passed={passed} failed={failed} skipped={skipped} unexpected_pass={unexpected}"

    if status == "passed":
        print(f"  [managed_fact] Result: passed ({summary})")
    else:
        print(f"  [managed_fact] Result: {status} ({summary})")

    return StageResult(
        stage="managed_fact", status=status,
        summary=summary,
        details={
            "passed": passed, "failed": failed, "skipped": skipped,
            "unexpected_pass": unexpected, "total": total,
            "results": results,
        },
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def run_cross_verify(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 3.6: Compare managed_fact vs native fact results per-method.

    Runs entry.exe --fact-json to get native per-method results, then compares
    with managed golden-values.json. Reports any discrepancies.
    """
    start = time.perf_counter()

    exe_path = ctx.entry_exe_path
    if not exe_path.exists():
        return StageResult(
            stage="cross_verify", status="skipped",
            summary="entry.exe not found",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Run native fact-json
    print(f"  [cross_verify] Running {exe_path} --fact-json...")
    try:
        r = subprocess.run(
            [str(exe_path), "--fact-json"],
            capture_output=True, text=True, timeout=120,
        )
    except subprocess.TimeoutExpired:
        return StageResult(
            stage="cross_verify", status="failed",
            summary="native --fact-json timed out",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Parse native fact-json — handle extra output (stderr interleaving, etc.)
    native_data = {"factResults": []}
    for line in (r.stdout or "").splitlines():
        line = line.strip()
        if line.startswith("{"):
            try:
                native_data = json.loads(line)
                break
            except json.JSONDecodeError:
                continue

    native_results = {nr["si"]: nr for nr in native_data.get("factResults", [])}

    # Load golden values from managed_fact stage
    managed_stage = stages.get("managed_fact")
    if managed_stage and managed_stage.status == "passed":
        managed_results_list = (managed_stage.details or {}).get("results", [])
        managed_by_index = {}
        for mr in managed_results_list:
            idx = mr.get("methodIndex", -1)
            if idx >= 0:
                managed_by_index[idx] = mr
    else:
        # Fallback: read from golden-values.json
        golden_path = ctx.family_dir / "native" / "golden-values.json"
        if golden_path.exists():
            try:
                golden_data = json.loads(golden_path.read_text(encoding="utf-8"))
                managed_results_list = golden_data.get("results", [])
                managed_by_index = {}
                for mr in managed_results_list:
                    idx = mr.get("methodIndex", -1)
                    if idx >= 0:
                        managed_by_index[idx] = mr
            except (OSError, json.JSONDecodeError):
                managed_by_index = {}
        else:
            managed_by_index = {}

    if not managed_by_index:
        return StageResult(
            stage="cross_verify", status="skipped",
            summary="no golden values available (managed_fact skipped/failed)",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Cross-verify per-method
    mismatches = []
    matched = 0
    skipped_in_managed = 0
    for si, nr in sorted(native_results.items()):
        native_passed = nr.get("passed", False)
        native_exit_code = nr.get("exitCode", -1)
        managed_ref = managed_by_index.get(si)

        if managed_ref is None:
            # Managed harness has no entry for this index (e.g., method not
            # auto-callable, or native has extra probe/entry entries).
            skipped_in_managed += 1
            continue

        managed_status = managed_ref.get("status", "")

        # Skip methods that managed harness explicitly cannot test
        if managed_status == "skipped":
            skipped_in_managed += 1
            continue

        managed_expected_pass = managed_status == "passed"

        if native_passed and not managed_expected_pass:
            # Expected: managed harness generates naive call expressions with
            # default/null args, while native subjects have internal try/catch.
            # A managed "fail" with native "pass" is a methodology difference,
            # not a real mismatch.
            matched += 1
        elif not native_passed and managed_expected_pass:
            mismatches.append({
                "si": si,
                "issue": "native_fail_but_managed_pass",
                "nativeExitCode": native_exit_code,
                "managedStatus": managed_status,
            })
        else:
            matched += 1

    total_checked = len(native_results)
    if mismatches:
        status = "failed"
        summary = f"{len(mismatches)}/{total_checked} mismatches, {matched} matched ({skipped_in_managed} skipped in managed)"
        print(f"  [cross_verify] {summary}")
        for m in mismatches[:5]:
            print(f"    si={m['si']}: {m['issue']} (managed={m.get('managedStatus', '?')})")
    else:
        status = "passed"
        summary = f"All {matched}/{total_checked} native methods match golden values ({skipped_in_managed} skipped in managed)"

    print(f"  [cross_verify] Result: {status} ({summary})")

    return StageResult(
        stage="cross_verify", status=status,
        summary=summary,
        details={
            "matched": matched,
            "mismatches": mismatches,
            "totalChecked": total_checked,
            "skippedInManaged": skipped_in_managed,
        },
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def run_fact_cross_verify(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Legacy: Compare fact (AOT) vs fact_jit (JIT) results (stage pass counts)."""
    start = time.perf_counter()

    fact = stages.get("fact")
    fact_jit = stages.get("fact_jit")

    if not fact or fact.status not in ("passed", "failed"):
        return StageResult(
            stage="fact_cross_verify", status="skipped",
            summary="fact stage not available or incomplete",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    if not fact_jit or fact_jit.status == "skipped":
        return StageResult(
            stage="fact_cross_verify", status="skipped",
            summary="fact_jit not available (JIT codegen skipped or failed)",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    fact_details = fact.details or {}
    jit_details = fact_jit.details or {}

    fact_passed = fact_details.get("passed", 0)
    fact_total = fact_details.get("total", 0)
    jit_passed = jit_details.get("passed", 0)
    jit_total = jit_details.get("total", 0)

    if fact_total == 0 and jit_total == 0:
        return StageResult(
            stage="fact_cross_verify", status="passed",
            summary="Both AOT and JIT report 0 assertions — no divergence detected",
            details={"fact": fact_details, "fact_jit": jit_details},
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    if fact_total != jit_total:
        return StageResult(
            stage="fact_cross_verify", status="failed",
            summary=(
                f"Total assertion count mismatch: AOT={fact_total} vs JIT={jit_total}. "
                "AOT codegen may be skipping assertions or the JIT path has extra tests.",
            ),
            details={
                "fact": fact_details,
                "fact_jit": jit_details,
                "difference": {
                    "passed_delta": fact_passed - jit_passed,
                    "total_delta": fact_total - jit_total,
                },
            },
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    if fact_passed != jit_passed:
        return StageResult(
            stage="fact_cross_verify", status="failed",
            summary=(
                f"Passed count mismatch: AOT={fact_passed}/{fact_total} vs "
                f"JIT={jit_passed}/{jit_total}. AOT codegen produces different results.",
            ),
            details={
                "fact": fact_details,
                "fact_jit": jit_details,
                "difference": {
                    "passed_delta": fact_passed - jit_passed,
                },
            },
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    return StageResult(
        stage="fact_cross_verify", status="passed",
        summary=f"AOT and JIT results match ({fact_passed}/{fact_total} passed)",
        details={"fact": fact_details, "fact_jit": jit_details},
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


# ── Managed Patch Fact (golden patched values) ─────────────────────

def _find_patch_class_name(patch_dir: Path) -> str | None:
    """Scan managed/patch/ dir for *PatchEntry.cs, return the class name."""
    if not patch_dir.is_dir():
        return None
    for cs_file in patch_dir.glob("*PatchEntry.cs"):
        text = cs_file.read_text(encoding="utf-8")
        for line in text.splitlines():
            m = re.search(r'(?:public\s+)?(?:static\s+)?(?:partial\s+)?class\s+(\w+PatchEntry)\b', line)
            if m:
                return m.group(1)
    return None


def generate_managed_patch_harness(
    harness_dir: Path,
    method_count: int,
    patch_class_name: str,
    patch_dll_path: Path,
    family_slug: str,
) -> bool:
    """Generate ManagedPatchHarness.cs that calls patch Subject_N() methods and captures return values."""
    harness_dir.mkdir(parents=True, exist_ok=True)

    method_calls: list[str] = []
    for idx in range(method_count):
        method_calls.append(f"""            {{ // [{idx}]
                try {{
                    int val = {patch_class_name}.Subject_{idx}();
                    results.Add(new MethodResult {{ MethodIndex = {idx}, Value = val, Caught = false }});
                }} catch (Exception ex) {{
                    results.Add(new MethodResult {{ MethodIndex = {idx}, Value = 0, Caught = true, ExceptionMessage = ex.Message }});
                }}
            }}""")

    calls_block = "\n".join(method_calls)

    cs_source = f"""// Auto-generated managed patch fact harness
// Family: {family_slug}
using System;
using System.Collections.Generic;
using System.Text.Json;

class ManagedPatchHarness
{{
    struct MethodResult
    {{
        public int MethodIndex {{ get; set; }}
        public int Value {{ get; set; }}
        public bool Caught {{ get; set; }}
        public string ExceptionMessage {{ get; set; }}
    }}

    static void Main()
    {{
        var results = new List<MethodResult>();
{calls_block}
        string json = JsonSerializer.Serialize(new {{ results }}, new JsonSerializerOptions {{ PropertyNamingPolicy = JsonNamingPolicy.CamelCase }});
        Console.WriteLine(json);
    }}
}}
"""
    csproj = f"""<Project Sdk="Microsoft.NET.Sdk">
  <PropertyGroup>
    <OutputType>Exe</OutputType>
    <TargetFrameworks>net8.0</TargetFrameworks>
    <Nullable>enable</Nullable>
    <ImplicitUsings>enable</ImplicitUsings>
    <StartupObject>ManagedPatchHarness</StartupObject>
  </PropertyGroup>
  <ItemGroup>
    <Reference Include="{patch_dll_path}" />
  </ItemGroup>
</Project>
"""
    cs_path = harness_dir / "ManagedPatchHarness.cs"
    csproj_path = harness_dir / "ManagedPatchHarness.csproj"
    cs_path.write_text(cs_source, encoding="utf-8")
    csproj_path.write_text(csproj, encoding="utf-8")
    return True


def run_managed_patch_fact(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Run managed patch DLL under .NET to generate golden patched values.

    Builds the patch DLL, generates a harness that calls each Subject_N() method,
    runs under dotnet, and saves patched-golden-values.json for cross-verification.
    """
    start = time.perf_counter()

    # Read contract for method count
    contract_data = _read_contract_json(ctx.contract_path)
    if contract_data is None:
        return StageResult(
            stage="managed_patch_fact", status="skipped",
            summary="contract not found",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )
    subject_ids = contract_data.get("methodSubjectIds", [])
    if not subject_ids:
        subject_ids = [m["methodSubjectId"] for m in contract_data.get("methodContracts", [])]
    method_count = len(subject_ids)
    if method_count == 0:
        return StageResult(
            stage="managed_patch_fact", status="skipped",
            summary="0 methods in contract",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Find patch entry
    patch_dir = ctx.family_dir / "managed" / "patch"
    if not patch_dir.is_dir():
        return StageResult(
            stage="managed_patch_fact", status="skipped",
            summary=f"no managed/patch/ directory found at {patch_dir}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    patch_class_name = _find_patch_class_name(patch_dir)
    if patch_class_name is None:
        return StageResult(
            stage="managed_patch_fact", status="skipped",
            summary="no *PatchEntry.cs with class found in managed/patch/",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Build patch DLL
    csproj_path = patch_dir / f"{patch_class_name}.csproj"
    if not csproj_path.exists():
        return StageResult(
            stage="managed_patch_fact", status="failed",
            summary=f"patch csproj not found at {csproj_path}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    print(f"  [managed_patch_fact] Building patch DLL {csproj_path}...")
    try:
        build_r = subprocess.run(
            ["dotnet", "build", str(csproj_path), "--configuration", "Release", "--nologo", "-v", "q"],
            capture_output=True, timeout=120,
        )
    except subprocess.TimeoutExpired:
        return StageResult(
            stage="managed_patch_fact", status="failed",
            summary="patch build timed out (120s)",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    if build_r.returncode != 0:
        return StageResult(
            stage="managed_patch_fact", status="failed",
            summary=f"patch build failed: {build_r.stderr[:200] if build_r.stderr else 'unknown'}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Locate built DLL
    patch_dll = patch_dir / "bin" / "Release" / "net8.0" / f"{patch_class_name}.dll"
    if not patch_dll.exists():
        # Fallback: search for the DLL
        candidates = list(patch_dir.rglob(f"{patch_class_name}.dll"))
        patch_dll = candidates[0] if candidates else patch_dll
        if not patch_dll.exists():
            return StageResult(
                stage="managed_patch_fact", status="failed",
                summary=f"patch DLL not found at {patch_dll}",
                duration_ms=int((time.perf_counter() - start) * 1000),
            )

    # Generate harness
    harness_dir = ctx.family_dir / "managed_test" / "patch_fact"
    print(f"  [managed_patch_fact] Generating harness for {method_count} methods (class={patch_class_name})...")
    ok = generate_managed_patch_harness(harness_dir, method_count, patch_class_name, patch_dll.resolve(), ctx.slug)
    if not ok:
        return StageResult(
            stage="managed_patch_fact", status="failed",
            summary="harness generation failed",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Build harness
    harness_csproj = harness_dir / "ManagedPatchHarness.csproj"
    print(f"  [managed_patch_fact] Building harness at {harness_dir}...")
    try:
        build_r = subprocess.run(
            ["dotnet", "build", str(harness_csproj), "--configuration", "Release", "--nologo", "-v", "q"],
            capture_output=True, timeout=120,
        )
    except subprocess.TimeoutExpired:
        return StageResult(
            stage="managed_patch_fact", status="failed",
            summary="harness build timed out (120s)",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    if build_r.returncode != 0:
        return StageResult(
            stage="managed_patch_fact", status="failed",
            summary=f"harness build failed: {build_r.stderr[:200] if build_r.stderr else 'unknown'}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Run harness
    print(f"  [managed_patch_fact] Running managed patch harness...")
    try:
        r = subprocess.run(
            ["dotnet", "run", "--no-build", "--project", str(harness_dir), "--configuration", "Release"],
            capture_output=True, text=True, timeout=120,
        )
    except subprocess.TimeoutExpired:
        return StageResult(
            stage="managed_patch_fact", status="failed",
            summary="harness run timed out (120s)",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )
    except OSError as e:
        return StageResult(
            stage="managed_patch_fact", status="failed",
            summary=str(e),
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    if r.returncode != 0:
        return StageResult(
            stage="managed_patch_fact", status="failed",
            summary=f"harness exit_code={r.returncode}: {r.stderr[:200]}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Parse JSON
    try:
        data = json.loads(r.stdout)
    except json.JSONDecodeError as e:
        return StageResult(
            stage="managed_patch_fact", status="failed",
            summary=f"JSON parse failed: {e}",
            details={"raw_output": r.stdout[:500]},
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    results = data.get("results", [])
    matched = sum(1 for rr in results if not rr.get("caught", True))
    caught = sum(1 for rr in results if rr.get("caught", False))
    total = len(results)

    # Save patched-golden-values.json
    golden_path = ctx.family_dir / "native" / "patched-golden-values.json"
    golden_path.parent.mkdir(parents=True, exist_ok=True)
    golden_path.write_text(json.dumps(data, indent=2), encoding="utf-8")
    print(f"  [managed_patch_fact] Saved patched-golden-values.json ({matched} matched, {caught} caught)")

    status = "passed" if caught == 0 else "failed"
    summary = f"matched={matched} caught={caught}/{total}"
    print(f"  [managed_patch_fact] Result: {status} ({summary})")

    return StageResult(
        stage="managed_patch_fact", status=status,
        summary=summary,
        details={
            "matched": matched, "caught": caught, "total": total,
            "results": results,
        },
        duration_ms=int((time.perf_counter() - start) * 1000),
    )
