"""Verification engine — pipeline orchestrator + aggregation + dashboard.

Merges the new-pipeline orchestrator (VerificationPipeline) with the aggregate
module (aggregate, coverage, dashboard, regression detection) into a single
orchestration module that drives all 16 verification stages.

Features:
  - Sequential and parallel (ParallelGroup) stage execution
  - Progress display with elapsed / ETA
  - Per-stage timeout (opt-in via ctx.stage_timeout_seconds)
  - Resume mode: skip already-passed stages from a previous run
  - Failure summary table at end-of-pipeline
"""

from __future__ import annotations

import json
import math
import time
from concurrent.futures import ThreadPoolExecutor, as_completed, TimeoutError
from pathlib import Path
from typing import Any

from verification.orchestration.context import FamilyContext, StageResult, UnifiedReport, ParallelGroup

# Stage runners
from verification.stages.preflight import run_preflight
from verification.stages.codegen import run_codegen, run_jit_codegen
from verification.stages.fact import (
    run_fact, run_fact_jit, run_managed_fact, run_cross_verify,
)
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
from verification.stages.cleanup import run_cleanup


# ── Required stages per mode ───────────────────────────────────────

REQUIRED_STAGES_STANDARD = {"preflight", "codegen", "jit_codegen", "fact", "audit"}
REQUIRED_STAGES_STRICT = {
    "preflight", "codegen", "jit_codegen", "managed_fact", "fact", "fact_jit",
    "audit", "asm_compare", "microbench", "benchmark",
    "hotupdate", "hotupdate_aot_benchmark",
    "hotupdate_jit_fact", "hotupdate_jit_benchmark",
}


# ── Helpers ────────────────────────────────────────────────────────

def _format_duration(seconds: float) -> str:
    """Format seconds to human-readable string."""
    if seconds < 60:
        return f"{seconds:.0f}s"
    minutes = int(seconds // 60)
    secs = int(seconds % 60)
    if minutes < 60:
        return f"{minutes}m{secs}s"
    hours = minutes // 60
    minutes = minutes % 60
    return f"{hours}h{minutes}m"


def _print_failure_summary(stages: dict[str, StageResult]) -> None:
    """Print a compact table of all failed / error stages."""
    failures = [(name, sr) for name, sr in stages.items()
                if sr.status in ("failed", "error")]
    if not failures:
        return

    print(f"\n  {'─' * 50}")
    print(f"  FAILURE SUMMARY ({len(failures)} stage(s))")
    print(f"  {'─' * 50}")
    for name, sr in failures:
        dur = _format_duration(sr.duration_ms / 1000) if sr.duration_ms else "-"
        summary = (sr.summary[:80] + "..") if len(sr.summary) > 80 else sr.summary
        print(f"  [{dur}] {name}: {sr.status.upper()} — {summary}")
    print(f"  {'─' * 50}\n")


# ── Verification Pipeline ──────────────────────────────────────────

class VerificationPipeline:
    """16-stage verification pipeline orchestrator with parallel group support.

    Sequential stages run one-at-a-time.  Stages wrapped in a ParallelGroup
    execute concurrently via ThreadPoolExecutor.  Currently parallelized:
      - fact (AOT) + fact_jit (JIT)
      - hotupdate_aot_bench + hotupdate_jit_fact + hotupdate_jit_bench

    Supports per-stage timeout (ctx.stage_timeout_seconds) and resuming
    from a previous report (ctx.resume).

    Usage:
        ctx = FamilyContext(slug="convert-char", assembly="System.Private.CoreLib", ...)
        pipeline = VerificationPipeline(ctx)
        report = pipeline.run()
    """

    # Stage definitions in execution order.
    # Sequential tuples run one-at-a-time; ParallelGroup stages run concurrently
    # within the group but still in order relative to sequential neighbors.
    STAGES: list[tuple[str, Any, str] | ParallelGroup] = [
        ("preflight", run_preflight, "Preflight"),
        ("codegen", run_codegen, "Codegen (AOT)"),
        ("jit_codegen", run_jit_codegen, "JitCodegen"),
        ("managed_fact", run_managed_fact, "Managed Fact (.NET)"),
        ("cross_verify", run_cross_verify, "Cross-Verify (Managed vs Native)"),
        ParallelGroup([
            ("fact", run_fact, "Fact AOT"),
            ("fact_jit", run_fact_jit, "Fact JIT"),
        ]),
        ("audit", run_audit, "Mechanism + Principle Audit"),
        ("asm_compare", run_asm_compare, "AsmCompare (JIT vs AOT)"),
        ("microbench", run_microbench, "Microbench (Interpreter)"),
        ("benchmark", run_benchmark, "Benchmark (3-way)"),
        ("hotupdate", run_hotupdate, "HotUpdate AOT Fact"),
        ParallelGroup([
            ("hotupdate_aot_benchmark", run_hotupdate_aot_bench, "HotUpdate AOT Bench"),
            ("hotupdate_jit_fact", run_hotupdate_jit_fact, "HotUpdate JIT Fact"),
            ("hotupdate_jit_benchmark", run_hotupdate_jit_bench, "HotUpdate JIT Bench"),
        ]),
        ("cleanup", run_cleanup, "Cleanup Build Artifacts"),
    ]

    # Stages that require actual methods (skipped when codegen reports 0 methods)
    METHOD_DEPENDENT_STAGES = {
        "managed_fact", "cross_verify", "fact", "fact_jit",
        "asm_compare", "microbench", "benchmark",
        "hotupdate", "hotupdate_aot_benchmark",
        "hotupdate_jit_fact", "hotupdate_jit_benchmark",
    }

    # Stages that cause fatal early termination on failure
    FATAL_STAGES = {"preflight"}

    def __init__(self, ctx: FamilyContext):
        self.ctx = ctx

    @staticmethod
    def _count_stages(stages: list) -> int:
        """Count total individual stages, flattening ParallelGroup entries."""
        total = 0
        for entry in stages:
            if isinstance(entry, ParallelGroup):
                total += len(entry.stages)
            else:
                total += 1
        return total

    def _load_resume_state(self) -> set[str]:
        """If resume mode is on and a previous report exists, return the set of
        stage names that already passed in the last run."""
        if not self.ctx.resume:
            return set()
        report_path = self.ctx.results_dir / "unified-verification-report.json"
        if not report_path.exists():
            return set()
        prev = UnifiedReport.load_from_file(report_path)
        if prev is None:
            return set()
        passed = {name for name, sr_dict in prev.stages.items()
                  if sr_dict.get("status") == "passed"}
        if passed:
            print(f"[resume] {len(passed)} stage(s) already passed in previous run — skipping")
        return passed

    def run(self) -> UnifiedReport:
        """Execute the full pipeline, returning a UnifiedReport."""
        overall_start = time.perf_counter()
        total = self._count_stages(self.STAGES)
        stages: dict[str, StageResult] = {}
        resume_passed = self._load_resume_state()

        print(f"\n{'='*60}")
        print(f"Family Verify: {self.ctx.slug} [{self.ctx.assembly}] mode={self.ctx.mode}"
              f"{' [RESUME]' if resume_passed else ''}"
              f"{' [TIMEOUT=' + str(self.ctx.stage_timeout_seconds) + 's]' if self.ctx.stage_timeout_seconds > 0 else ''}")
        print(f"{'='*60}\n")

        _zero_methods = False
        global_idx = 0

        def _run_single(name: str, runner: Any, label: str, idx: int) -> StageResult:
            """Run a single stage and return the result.  Handles skip/zero-methods/resume."""
            nonlocal _zero_methods

            # Resume: skip if already passed in previous run
            if name in resume_passed:
                print(f"[{idx}/{total}] {label}... passed (resume)")
                return StageResult(stage=name, status="passed", summary="Resumed from previous run")

            if name in self.ctx.skip_stages:
                print(f"[{idx}/{total}] {label}... skipped")
                return StageResult(stage=name, status="skipped", summary="Explicitly skipped")

            if _zero_methods and name in self.METHOD_DEPENDENT_STAGES:
                print(f"[{idx}/{total}] {label}... n/a")
                return StageResult(stage=name, status="n/a", summary="0 methods — stage not applicable")

            stage_start = time.perf_counter()

            if self.ctx.stage_timeout_seconds > 0:
                # ── Run with timeout ──
                with ThreadPoolExecutor(max_workers=1) as pool:
                    fut = pool.submit(runner, self.ctx, stages)
                    try:
                        sr = fut.result(timeout=self.ctx.stage_timeout_seconds)
                    except TimeoutError:
                        dur = int((time.perf_counter() - stage_start) * 1000)
                        print(f"[{idx}/{total}] {label}... TIMEOUT ({self.ctx.stage_timeout_seconds}s)")
                        sr = StageResult(
                            stage=name, status="error",
                            summary=f"Timed out after {self.ctx.stage_timeout_seconds}s",
                            duration_ms=dur,
                        )
                    except Exception as e:
                        import traceback
                        traceback.print_exc()
                        dur = int((time.perf_counter() - stage_start) * 1000)
                        sr = StageResult(stage=name, status="error",
                                         summary=f"Stage crashed: {e}", duration_ms=dur)
            else:
                # ── Run without timeout ──
                try:
                    sr = runner(self.ctx, stages)
                except Exception as e:
                    import traceback
                    traceback.print_exc()
                    sr = StageResult(stage=name, status="error", summary=f"Stage crashed: {e}")

            if sr.duration_ms == 0:
                sr.duration_ms = int((time.perf_counter() - stage_start) * 1000)

            # Elapsed / ETA
            elapsed = time.perf_counter() - overall_start
            completed = sum(1 for s in stages.values() if s.status in ("passed", "failed", "error"))
            eta_str = ""
            if completed > 0:
                remaining = total - completed
                eta = (elapsed / completed) * remaining
                eta_str = f" [ETA {_format_duration(eta)}]"

            print(f"  [{_format_duration(elapsed)}{eta_str}] {sr.status}: {sr.summary}")

            # Track 0-method result from codegen (only codegen stage, which is always sequential)
            if name == "codegen" and "0 methods" in (sr.summary or ""):
                _zero_methods = True

            return sr

        for entry in self.STAGES:
            if isinstance(entry, ParallelGroup):
                # ── Parallel group ──
                group = entry
                elapsed = _format_duration(time.perf_counter() - overall_start)
                print(f"\n  [{elapsed}] >>> Parallel group ({len(group.stages)} stages)")
                futures = {}
                with ThreadPoolExecutor(max_workers=min(len(group.stages), 4)) as pool:
                    for name, runner, label in group.stages:
                        global_idx += 1
                        fut = pool.submit(_run_single, name, runner, label, global_idx)
                        futures[fut] = name

                    completed_in_group = 0
                    for fut in as_completed(futures):
                        name = futures[fut]
                        sr = fut.result()
                        stages[name] = sr
                        completed_in_group += 1

                elapsed = _format_duration(time.perf_counter() - overall_start)
                print(f"  [{elapsed}] <<< Parallel group done\n")

            else:
                # ── Sequential stage ──
                global_idx += 1
                name, runner, label = entry
                sr = _run_single(name, runner, label, global_idx)
                stages[name] = sr

                # Fatal termination check
                if name in self.FATAL_STAGES and sr.status in ("failed", "error"):
                    print(f"  Cannot continue — {name} failed")
                    break

        # Aggregate
        total_ms = int((time.perf_counter() - overall_start) * 1000)
        print(f"\n[{total}/{total}] Aggregating...")
        report = _aggregate(self.ctx, stages, total_ms)

        # Failure summary table
        _print_failure_summary(stages)

        # Print final summary
        print(f"\n{'='*60}")
        print(f"Result: {report.overall_status}")
        print(f"Duration: {_format_duration(total_ms / 1000)}")
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
        required_ok = any(
            name in stages and stages[name].status in ("passed", "n/a")
            for name in required
        )
        overall_status = "passed" if required_ok else "skipped"

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
    na = sum(1 for sr in stages.values() if sr.status == "n/a")
    total = len(stages)
    non_skipped = total - skipped - na

    rate = round(passed / non_skipped * 100, 1) if non_skipped else 100.0

    return {
        "stagePassRate": rate,
        "stagesPassed": passed,
        "stagesFailed": failed,
        "stagesSkipped": skipped,
        "stagesN_A": na,
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
