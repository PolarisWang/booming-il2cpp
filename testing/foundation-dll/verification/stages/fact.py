"""Fact verification stage runners — run native entry EXE and verify assertions."""

from __future__ import annotations

import re
import subprocess
import time
from typing import Any

from orchestration.context import FamilyContext, StageResult


def run_fact(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 3: Fact AOT — run il2cpp-translated native entry EXE."""
    start = time.perf_counter()

    exe_path = ctx.entry_exe_path
    if not exe_path.exists():
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


def run_fact_cross_verify(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 4.5: Compare fact (AOT) vs fact_jit (JIT) results.

    Reads the fact and fact_jit stage results from the pipeline, compares
    their passed/total counts. If they match, the AOT codegen is semantically
    equivalent to the interpreter.
    """
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
