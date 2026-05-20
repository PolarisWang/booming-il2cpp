"""Verification Pipeline — 13-stage unified verification orchestrator.

Orchestrates all verification stages for a single capability family.
Each stage is a standalone runner function with signature:
    fn(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult
"""

from __future__ import annotations

import time
from typing import Any, Callable

from _core.python.models import FamilyContext, StageResult, UnifiedReport
from _core.python.preflight import run_preflight
from _core.python.codegen_runner import run_codegen, run_jit_codegen
from _core.python.fact_verifier import run_fact, run_fact_jit
from _core.python.audit_runner import run_audit
from _core.python.asm_compare_runner import run_asm_compare
from _core.python.microbench_runner import run_microbench
from _core.python.benchmark_runner import run_benchmark
from _core.python.hotupdate_runner import (
    run_hotupdate,
    run_hotupdate_aot_bench,
    run_hotupdate_jit_fact,
    run_hotupdate_jit_bench,
)
from _core.python.aggregate import aggregate


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
        report = aggregate(self.ctx, stages, total_ms)

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
