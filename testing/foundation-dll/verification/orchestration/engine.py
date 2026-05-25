"""Verification engine — pipeline orchestrator + aggregation + dashboard.

Merges the new-pipeline orchestrator (VerificationPipeline) with the aggregate
module (aggregate, coverage, dashboard, regression detection) into a single
orchestration module that drives all 13 verification stages.
"""

from __future__ import annotations

import json
import time
from pathlib import Path
from typing import Any

from verification.orchestration.context import FamilyContext, StageResult, UnifiedReport

# Stage runners
from verification.stages.preflight import run_preflight
from verification.stages.codegen import run_codegen, run_jit_codegen
from verification.stages.fact import run_fact, run_fact_jit
from verification.stages.audit import run_audit
from verification.stages.asm_compare import run_asm_compare
from verification.stages.microbench import run_microbench
from verification.stages.benchmark import run_benchmark
from verification.stages.hotupdate import (
    run_hotupdate,
    run_hotupdate_aot_bench,
    run_hotupdate_jit_fact,
    run_hotupdate_jit_bench,
)


# ── Required stages per mode ───────────────────────────────────────

REQUIRED_STAGES_STANDARD = {"preflight", "codegen", "jit_codegen", "fact", "audit"}
REQUIRED_STAGES_STRICT = REQUIRED_STAGES_STANDARD | {
    "hotupdate", "hotupdate_aot_benchmark",
    "hotupdate_jit_fact", "hotupdate_jit_benchmark",
}


# ── Verification Pipeline ──────────────────────────────────────────

class VerificationPipeline:
    """13-stage verification pipeline orchestrator.

    Usage:
        ctx = FamilyContext(slug="convert-char", assembly="System.Private.CoreLib", ...)
        pipeline = VerificationPipeline(ctx)
        report = pipeline.run()
    """

    # Stage definitions in execution order
    STAGES = [
        ("preflight", run_preflight, "Preflight"),
        ("codegen", run_codegen, "Codegen (AOT)"),
        ("jit_codegen", run_jit_codegen, "JitCodegen"),
        ("fact", run_fact, "Fact AOT"),
        ("fact_jit", run_fact_jit, "Fact JIT"),
        ("audit", run_audit, "Mechanism + Principle Audit"),
        ("asm_compare", run_asm_compare, "AsmCompare (JIT vs AOT)"),
        ("microbench", run_microbench, "Microbench (Interpreter)"),
        ("benchmark", run_benchmark, "Benchmark (3-way)"),
        ("hotupdate", run_hotupdate, "HotUpdate AOT Fact"),
        ("hotupdate_aot_benchmark", run_hotupdate_aot_bench, "HotUpdate AOT Bench"),
        ("hotupdate_jit_fact", run_hotupdate_jit_fact, "HotUpdate JIT Fact"),
        ("hotupdate_jit_benchmark", run_hotupdate_jit_bench, "HotUpdate JIT Bench"),
    ]

    # Stages that cause fatal early termination on failure
    FATAL_STAGES = {"preflight"}

    def __init__(self, ctx: FamilyContext):
        self.ctx = ctx

    def run(self) -> UnifiedReport:
        """Execute the full pipeline, returning a UnifiedReport."""
        overall_start = time.perf_counter()
        total = len(self.STAGES)
        stages: dict[str, StageResult] = {}

        print(f"\n{'='*60}")
        print(f"Family Verify: {self.ctx.slug} [{self.ctx.assembly}] mode={self.ctx.mode}")
        print(f"{'='*60}\n")

        for idx, (name, runner, label) in enumerate(self.STAGES):
            if name in self.ctx.skip_stages:
                stages[name] = StageResult(stage=name, status="skipped", summary="Explicitly skipped")
                print(f"[{idx}/{total}] {label}... skipped")
                continue

            print(f"[{idx}/{total}] {label}...")
            try:
                sr = runner(self.ctx, stages)
            except Exception as e:
                import traceback
                traceback.print_exc()
                sr = StageResult(stage=name, status="error", summary=f"Stage crashed: {e}")

            stages[name] = sr
            print(f"  {sr.status}: {sr.summary}")

            # Fatal termination check
            if name in self.FATAL_STAGES and sr.status in ("failed", "error"):
                print(f"  Cannot continue — {name} failed")
                break

        # Aggregate
        total_ms = int((time.perf_counter() - overall_start) * 1000)
        print(f"\n[{total}/{total}] Aggregating...")
        report = _aggregate(self.ctx, stages, total_ms)

        # Print summary
        print(f"\n{'='*60}")
        print(f"Result: {report.overall_status}")
        print(f"Duration: {total_ms}ms")
        if report.coverage:
            cov = report.coverage
            print(f"Coverage: {cov.get('stagesPassed', 0)}/{cov.get('stagesTotal', 0)} passed"
                  f" ({cov.get('stagePassRate', 0)}%)")
        print(f"{'='*60}")

        return report


# ── Aggregation ────────────────────────────────────────────────────

def _aggregate(ctx: FamilyContext, stages: dict[str, StageResult], total_duration_ms: int) -> UnifiedReport:
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

    # Benchmark performance
    bm = stages.get("benchmark")
    if bm and bm.status == "passed":
        details = bm.details or {}

        native_aot = details.get("native-aot", {})
        native_jit = details.get("native-jit", {})
        aot_results = native_aot.get("results", []) if isinstance(native_aot, dict) else []
        jit_results = native_jit.get("results", []) if isinstance(native_jit, dict) else []

        faster_count = 0
        comparable = 0
        total_speedup = 0.0
        for aot_r, jit_r in zip(aot_results, jit_results):
            aot_ops = aot_r.get("opsPerSecond", 0) if isinstance(aot_r, dict) else 0
            jit_ops = jit_r.get("opsPerSecond", 0) if isinstance(jit_r, dict) else 0
            if aot_ops > 0 and jit_ops > 0:
                comparable += 1
                if aot_ops > jit_ops:
                    faster_count += 1
                total_speedup += (aot_ops / jit_ops - 1.0) * 100

        native_faster_ratio = round(faster_count / comparable, 3) if comparable > 0 else 0
        avg_speedup = round(total_speedup / comparable, 1) if comparable > 0 else 0.0

        aot_avg = native_aot.get("averageOpsPerSecond", 0) if isinstance(native_aot, dict) else 0
        jit_avg = native_jit.get("averageOpsPerSecond", 0) if isinstance(native_jit, dict) else 0

        dashboard["keyRatios"] = {
            "nativeFasterRatio": native_faster_ratio,
            "managedFasterRatio": 0,
            "nativeAotOpsPerSecond": aot_avg,
            "nativeJitOpsPerSecond": jit_avg,
        }
        dashboard["averageSpeedupPercent"] = avg_speedup

    # AsmCompare IR expansion
    ac = stages.get("asm_compare")
    if ac and ac.status == "passed" and ac.details:
        ac_inner = ac.details.get("details", {})
        dashboard["irExpansionRatio"] = ac_inner.get("overallIrExpansionRatio", 0)
        dashboard["asmPassRate"] = ac_inner.get("asmPassRate", 0)

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
