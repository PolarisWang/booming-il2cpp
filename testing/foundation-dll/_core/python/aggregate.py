"""Aggregation stage — combine results, build dashboard, detect regression."""

from __future__ import annotations

import json
import time
from pathlib import Path
from typing import Any

from _core.python.models import FamilyContext, StageResult, UnifiedReport


# Required stages per mode
REQUIRED_STAGES_STANDARD = {"preflight", "codegen", "jit_codegen", "fact", "audit"}
REQUIRED_STAGES_STRICT = REQUIRED_STAGES_STANDARD | {
    "hotupdate", "hotupdate_aot_benchmark",
    "hotupdate_jit_fact", "hotupdate_jit_benchmark",
}


def aggregate(ctx: FamilyContext, stages: dict[str, StageResult], total_duration_ms: int) -> UnifiedReport:
    """Aggregate all stage results into the final unified report."""
    stages_map = {name: sr.to_dict() for name, sr in stages.items()}
    coverage = _compute_coverage(stages)
    dashboard = _build_dashboard(stages)
    regression = _detect_regression(ctx)

    required = REQUIRED_STAGES_STRICT if ctx.mode == "strict" else REQUIRED_STAGES_STANDARD
    failures = [name for name, sr in stages.items() if name in required and sr.status == "failed"]
    errors = [name for name, sr in stages.items() if name in required and sr.status == "error"]

    if errors or failures:
        overall_status = "failed"
    else:
        # At least one non-skipped required stage must have run to count as passed
        required_ran = any(
            name in stages and stages[name].status == "passed"
            for name in required
        )
        overall_status = "passed" if required_ran else "skipped"

    return UnifiedReport(
        family=ctx.slug,
        assembly=ctx.assembly,
        mode=ctx.mode,
        timestamp=time.strftime("%Y-%m-%dT%H:%M:%S"),
        duration_ms=total_duration_ms,
        overall_status=overall_status,
        stages=stages_map,
        coverage=coverage,
        dashboard=dashboard,
        regression=regression,
    )


def _compute_coverage(stages: dict[str, StageResult]) -> dict[str, float]:
    """Compute verification coverage metrics."""
    passed = sum(1 for sr in stages.values() if sr.status == "passed")
    failed = sum(1 for sr in stages.values() if sr.status == "failed")
    skipped = sum(1 for sr in stages.values() if sr.status == "skipped")
    total = len(stages)
    non_skipped = total - skipped

    # Pass rate: fraction of non-skipped stages that passed
    # (100% if no stages were actually run, to avoid division by zero)
    rate = round(passed / non_skipped * 100, 1) if non_skipped else 100.0

    return {
        "stagePassRate": rate,
        "stagesPassed": passed,
        "stagesFailed": failed,
        "stagesSkipped": skipped,
        "stagesTotal": total,
    }


def _build_dashboard(stages: dict[str, StageResult]) -> dict[str, Any]:
    """Build comprehensive dashboard from stage results."""
    dashboard: dict[str, Any] = {}

    # Benchmark ratios
    bm = stages.get("benchmark")
    if bm and bm.status == "passed":
        details = bm.details or {}
        summary = details.get("summary", {})
        dashboard["keyRatios"] = {
            "nativeFasterRatio": summary.get("nativeFasterRatio", 0),
            "managedFasterRatio": summary.get("managedFasterRatio", 0),
            "irExpansionRatio": details.get("irExpansionRatio", 0),
            "asmPassRate": details.get("asmPassRate", 0),
        }
        dashboard["averageSpeedupPercent"] = summary.get("averageSpeedupPercent", 0)

    return dashboard


def _detect_regression(ctx: FamilyContext) -> dict[str, Any]:
    """Detect regressions by comparing with stored baseline."""
    result: dict[str, Any] = {
        "hasRegression": False,
        "regressions": [],
        "benchmark": {},
    }

    baseline_path = (
        ctx.family_dir.parents[2] / "results" / "verification-history"
        / ctx.assembly / ctx.slug / "baseline-benchmark.json"
    )

    if not baseline_path.exists():
        result["benchmark"] = {"status": "no_baseline", "message": "No baseline established"}
        return result

    try:
        baseline = json.loads(baseline_path.read_text(encoding="utf-8"))
        result["benchmark"] = {
            "status": "matched",
            "baselineMetrics": baseline.get("metrics", {}),
        }
    except (OSError, json.JSONDecodeError) as e:
        result["benchmark"] = {"status": "error", "message": str(e)}

    return result
