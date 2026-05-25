"""Audit stage runner — Mechanism audit + Principle alignment checks."""

from __future__ import annotations

import sys
import time
from pathlib import Path
from typing import Any

from orchestration.context import FamilyContext, StageResult


def run_audit(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 5: Mechanism audit (stub detection, skip audit) + Principle alignment."""
    start = time.perf_counter()

    # Ensure old pipeline path is available for mechanism_audit
    _old_pipeline = (
        Path(__file__).resolve().parents[4] / "build" / "toolchains" / "run" / "testing" / "foundation_dll"
    )
    if str(_old_pipeline) not in sys.path:
        sys.path.insert(0, str(_old_pipeline))

    try:
        from mechanism_audit import run_full_audit

        audit_result = run_full_audit(ctx.assembly, ctx.slug)

        mechanism = audit_result.get("mechanism_audit", {})
        principle = audit_result.get("principle_alignment", {})
        overall = audit_result.get("overall", {})

        _apply_audit_overrides(principle, overall, mechanism, ctx)

        mechanism_passed = overall.get("mechanism_passed", False)
        principle_status = overall.get("principle_status", "NOT_APPLICABLE")
        overall["passed"] = mechanism_passed and principle_status != "VIOLATION"

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
    except ImportError:
        return StageResult(
            stage="audit", status="skipped",
            summary="mechanism_audit not available",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )
    except Exception as e:
        return StageResult(
            stage="audit", status="error",
            summary=f"Audit failed: {e}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )


def _apply_audit_overrides(
    principle: dict[str, Any],
    overall: dict[str, Any],
    mechanism: dict[str, Any],
    ctx: FamilyContext,
) -> None:
    """Apply overrides matching old family_verification_orchestrator._stage_audit logic."""
    checks = principle.get("checks", {})
    modified = False

    if "benchmark" in ctx.skip_stages:
        bm_check = checks.get("p1_benchmark", {})
        bm_status = bm_check.get("status")
        if bm_status in ("VIOLATION", "CONCERN"):
            bm_check["status"] = "NOT_APPLICABLE"
            bm_check["summary"] = bm_check.get("summary", "") + " [overridden — benchmark stage was skipped]"
            modified = True

    if (ctx.codegen_mode or "").lower() == "jit":
        bm_check = checks.get("p1_benchmark", {})
        bm_status = bm_check.get("status")
        if bm_status in ("VIOLATION", "CONCERN"):
            bm_check["status"] = "NOT_APPLICABLE"
            bm_check["summary"] = bm_check.get("summary", "") + " [overridden — codegen-mode=jit]"
            modified = True

    if modified:
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
