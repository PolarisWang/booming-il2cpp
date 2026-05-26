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

    The managed fact harness is at {family_dir}/managed_test/fact/ManagedFactHarness.cs.
    If the harness doesn't exist, this stage is skipped.
    """
    start = time.perf_counter()

    harness_dir = ctx.family_dir / "managed_test" / "fact"
    csproj = harness_dir / "ManagedFactHarness.csproj"

    if not csproj.exists():
        return StageResult(
            stage="managed_fact", status="skipped",
            summary="ManagedFactHarness.csproj not found",
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
