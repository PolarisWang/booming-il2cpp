"""Abstract pipeline base — template method for stage orchestration.

Subclasses define STAGES and FATAL_STAGES, and implement aggregate().
The run() template method handles iteration, skip/fatal logic, and timing.
"""

from __future__ import annotations

import time
from abc import ABC, abstractmethod
from typing import Any, Callable

from _pipeline.base_context import ContextBase
from _pipeline.base_report import ReportBase
from _pipeline.stage_result import StageResult


class PipelineBase(ABC):
    """Template-method pipeline orchestrator.

    Usage:
        class MyPipeline(PipelineBase):
            STAGES = [("preflight", run_preflight, "Preflight"), ...]
            FATAL_STAGES = {"preflight"}
            def aggregate(self, stages, total_ms) -> MyReport: ...
    """

    # Subclasses override these
    STAGES: list[tuple[str, Callable, str]] = []
    FATAL_STAGES: set[str] = {"preflight"}

    def __init__(self, ctx: ContextBase):
        self.ctx = ctx

    def run(self) -> ReportBase:
        """Template method: iterate stages, aggregate, return report."""
        overall_start = time.perf_counter()
        total = len(self.STAGES)
        stages: dict[str, StageResult] = {}

        print(f"\n{'='*60}")
        print(f"Verify: {self.ctx.slug} mode={self.ctx.mode}")
        print(f"{'='*60}\n")

        for idx, (name, runner, label) in enumerate(self.STAGES):
            if name in self.ctx.skip_stages:
                stages[name] = StageResult(
                    stage=name, status="skipped", summary="Explicitly skipped"
                )
                print(f"[{idx}/{total}] {label}... skipped")
                continue

            print(f"[{idx}/{total}] {label}...")
            try:
                sr = runner(self.ctx, stages)
            except Exception as e:
                import traceback
                traceback.print_exc()
                sr = StageResult(
                    stage=name, status="error", summary=f"Stage crashed: {e}"
                )

            stages[name] = sr
            print(f"  {sr.status}: {sr.summary}")

            if name in self.FATAL_STAGES and sr.status in ("failed", "error"):
                print(f"  Cannot continue — {name} failed")
                break

        total_ms = int((time.perf_counter() - overall_start) * 1000)
        print(f"\n[aggregating] ...")
        report = self.aggregate(stages, total_ms)

        print(f"\n{'='*60}")
        print(f"Result: {report.overall_status}")
        print(f"Duration: {total_ms}ms")
        if report.coverage:
            cov = report.coverage
            print(f"Coverage: {cov.get('stagesPassed', 0)}/"
                  f"{cov.get('stagesTotal', 0)} passed "
                  f"({cov.get('stagePassRate', 0)}%)")
        print(f"{'='*60}")

        return report

    @abstractmethod
    def aggregate(
        self, stages: dict[str, StageResult], total_ms: int
    ) -> ReportBase:
        """Combine all stage results into the final report."""
        ...
