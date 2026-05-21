"""Aggregation stage — combine project results into final report."""

from __future__ import annotations

import time
from typing import Any

from _pipeline.stage_result import StageResult
from _core.python.models import ProjectContext, ProjectReport


def project_aggregate(
    ctx: ProjectContext,
    stages: dict[str, StageResult],
    total_duration_ms: int,
) -> ProjectReport:
    """Aggregate all project verification stage results."""
    stages_map = {name: sr.to_dict() for name, sr in stages.items()}
    coverage = _compute_coverage(stages)

    required_stages = {"preflight", "codegen", "fact"}
    failures = [name for name, sr in stages.items()
                if name in required_stages and sr.status == "failed"]
    errors = [name for name, sr in stages.items()
              if name in required_stages and sr.status == "error"]

    overall_status = "failed" if (errors or failures) else "passed"

    # Extract fact output match status
    fact_sr = stages.get("fact")
    fact_match = fact_sr.status == "passed" if fact_sr else False

    # Extract hotupdate summary
    hu_summary: dict[str, Any] = {}
    for name in ("hotupdate", "hotupdate_jit"):
        sr = stages.get(name)
        if sr:
            hu_summary[name] = {"status": sr.status, "summary": sr.summary}

    return ProjectReport(
        slug=ctx.slug,
        mode=ctx.mode,
        timestamp=time.strftime("%Y-%m-%dT%H:%M:%S"),
        duration_ms=total_duration_ms,
        overall_status=overall_status,
        stages=stages_map,
        coverage=coverage,
        fact_output_match=fact_match,
        hotupdate_summary=hu_summary,
    )


def _compute_coverage(stages: dict[str, StageResult]) -> dict[str, float]:
    """Compute verification coverage metrics."""
    passed = sum(1 for sr in stages.values() if sr.status == "passed")
    failed = sum(1 for sr in stages.values() if sr.status == "failed")
    skipped = sum(1 for sr in stages.values() if sr.status == "skipped")
    total = len(stages)

    return {
        "stagePassRate": round(passed / total * 100, 1) if total else 0.0,
        "stagesPassed": passed,
        "stagesFailed": failed,
        "stagesSkipped": skipped,
        "stagesTotal": total,
    }
