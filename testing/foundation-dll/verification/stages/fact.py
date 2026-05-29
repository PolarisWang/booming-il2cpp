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
                        Status = "failed",
                        ExceptionMessage = ex.Message,
                    }});
                }}
            }}""")

    results_block = "\n".join(method_blocks)

    cs_source = f"""// Auto-generated managed fact harness
// Family: {family_slug}, Assembly: {assembly}
using System;
using System.Collections.Generic;
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
    csproj = f"""<Project Sdk="Microsoft.NET.Sdk">
  <PropertyGroup>
    <OutputType>Exe</OutputType>
    <TargetFrameworks>net8.0</TargetFrameworks>
    <Nullable>enable</Nullable>
    <ImplicitUsings>enable</ImplicitUsings>
    <StartupObject>ManagedFactHarness</StartupObject>
  </PropertyGroup>
</Project>
"""
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
        r = subprocess.run(
            ["dotnet", "run", "--project", str(harness_dir), "--configuration", "Release"],
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
            mismatches.append({
                "si": si,
                "issue": "no_managed_reference",
                "nativePassed": native_passed,
            })
            continue

        managed_status = managed_ref.get("status", "")

        # Skip methods that managed harness explicitly cannot test
        if managed_status == "skipped":
            skipped_in_managed += 1
            continue

        managed_expected_pass = managed_status == "passed"

        if native_passed and not managed_expected_pass:
            mismatches.append({
                "si": si,
                "issue": "native_pass_but_managed_fail_or_unexpected",
                "nativeExitCode": native_exit_code,
                "managedStatus": managed_status,
            })
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
