"""Audit stage runner — Mechanism audit + Principle alignment checks."""

from __future__ import annotations

import time
from typing import Any

from _core.python.models import FamilyContext, StageResult
from foundation_dll.pipeline_adapter import run_audit as _run_adapter_audit


def run_audit(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 5: Mechanism audit (stub detection, skip audit) + Principle alignment."""
    start = time.perf_counter()

    # Delegate to old mechanism_audit module via adapter
    audit_result = _run_adapter_audit(ctx.slug, ctx.assembly)

    mechanism = audit_result.get("mechanism_audit", {})
    principle = audit_result.get("principle_alignment", {})
    overall = audit_result.get("overall", {})

    # Apply skip-stage overrides matching old orchestrator behavior
    _apply_audit_overrides(principle, overall, mechanism, ctx)

    status = "passed" if overall.get("passed", False) else "failed"

    total = mechanism.get("total_methods", 0)
    audited = mechanism.get("audited", 0)
    false_passing = mechanism.get("false_passing", 0)
    stubs = mechanism.get("stubs_found", 0)

    summary_parts = [f"methods={total}", f"audited={audited}"]
    if false_passing:
        summary_parts.append(f"false_passing={false_passing}")
    if stubs:
        summary_parts.append(f"stubs={stubs}")

    return StageResult(
        stage="audit", status=status,
        summary=", ".join(summary_parts),
        details={
            "total_methods": total,
            "audited": audited,
            "false_passing": false_passing,
            "stubs_found": stubs,
            "mechanismReport": mechanism,
            "principleReport": principle,
        },
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def _apply_audit_overrides(
    principle: dict[str, Any],
    overall: dict[str, Any],
    mechanism: dict[str, Any],
    ctx: FamilyContext,
) -> None:
    """Apply overrides matching old family_verification_orchestrator._stage_audit logic.

    When benchmark is skipped, p1_benchmark is demoted to NOT_APPLICABLE
    since no benchmark data was collected.
    """
    checks = principle.get("checks", {})
    modified = False

    # If benchmark is skipped, demote p1_benchmark
    if "benchmark" in ctx.skip_stages:
        bm_check = checks.get("p1_benchmark", {})
        bm_status = bm_check.get("status")
        if bm_status in ("VIOLATION", "CONCERN"):
            bm_check["status"] = "NOT_APPLICABLE"
            bm_check["summary"] = bm_check.get("summary", "") + " [overridden — benchmark stage was skipped]"
            modified = True

    # In JIT mode, demote p1_benchmark (interpreter is intentionally slower)
    if (ctx.codegen_mode or "").lower() == "jit":
        bm_check = checks.get("p1_benchmark", {})
        bm_status = bm_check.get("status")
        if bm_status in ("VIOLATION", "CONCERN"):
            bm_check["status"] = "NOT_APPLICABLE"
            bm_check["summary"] = bm_check.get("summary", "") + " [overridden — codegen-mode=jit]"
            modified = True

    if modified:
        # Recompute overall principle status
        status_counts: dict[str, int] = {}
        for c in checks.values():
            s = c.get("status", "NOT_APPLICABLE")
            status_counts[s] = status_counts.get(s, 0) + 1

        if status_counts.get("VIOLATION", 0) > 0:
            principle_overall = "VIOLATION"
        elif status_counts.get("CONCERN", 0) > 0:
            principle_overall = "CONCERN"
        elif status_counts.get("ALIGNED", 0) > 0:
            principle_overall = "ALIGNED"
        else:
            principle_overall = "NOT_APPLICABLE"

        principle.setdefault("summary", {})["overall"] = principle_overall
        overall["principle_status"] = principle_overall
        overall["passed"] = mechanism.get("passed", False) and principle_overall != "VIOLATION"
