"""Fact cross-verification stage — compare AOT vs JIT fact results.

Verifies semantic equivalence between AOT (native codegen) and JIT (interpreter):
the fact result counts should be identical since both execute the same managed
test assertions. Differences indicate a semantic divergence in the AOT codegen.
"""

from __future__ import annotations

import time
from typing import Any

from _core.python.models import FamilyContext, StageResult


def run_fact_cross_verify(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 4.5: Compare fact (AOT) vs fact_jit (JIT) results.

    Reads the fact and fact_jit stage results from the pipeline, compares
    their passed/total counts. If they match, the AOT codegen is semantically
    equivalent to the interpreter. If not, a semantic divergence exists.

    Position: runs after fact_jit (stage 4), before audit (stage 5).
    """
    start = time.perf_counter()

    fact = stages.get("fact")
    fact_jit = stages.get("fact_jit")

    # Need fact to compare against
    if not fact or fact.status not in ("passed", "failed"):
        return StageResult(
            stage="fact_cross_verify", status="skipped",
            summary="fact stage not available or incomplete",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # If JIT was skipped, we can't cross-verify
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

    # Normalise: if one side reports 0/0 and the other has real counts,
    # the comparison is not meaningful.
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
