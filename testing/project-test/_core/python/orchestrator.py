"""Project pipeline — complete application verification orchestrator.

Stages mirror the foundation-dll pipeline but with project-level verification:
  - fact: validate full stdout output against expected baseline
  - hotupdate: patch injection + behavior change verification
  - benchmark: whole-application throughput measurement
"""

from __future__ import annotations

from _pipeline.base_pipeline import PipelineBase
from _pipeline.stage_result import StageResult
from _core.python.models import ProjectContext, ProjectReport

from _core.python.preflight import run_project_preflight
from _core.python.codegen_runner import run_project_codegen, run_project_jit_codegen
from _core.python.fact_verifier import run_project_fact, run_project_fact_jit
from _core.python.hotupdate_runner import run_project_hotupdate, run_project_hotupdate_jit
from _core.python.benchmark_runner import run_project_benchmark
from _core.python.aggregate import project_aggregate


class ProjectPipeline(PipelineBase):
    """Verification pipeline for a complete .NET project."""

    STAGES = [
        ("preflight", run_project_preflight, "Preflight"),
        ("codegen", run_project_codegen, "Codegen (AOT)"),
        ("jit_codegen", run_project_jit_codegen, "JitCodegen"),
        ("fact", run_project_fact, "Fact AOT"),
        ("fact_jit", run_project_fact_jit, "Fact JIT"),
        ("benchmark", run_project_benchmark, "Benchmark"),
        ("hotupdate", run_project_hotupdate, "HotUpdate AOT"),
        ("hotupdate_jit", run_project_hotupdate_jit, "HotUpdate JIT"),
    ]

    FATAL_STAGES = {"preflight"}

    def aggregate(self, stages: dict[str, StageResult], total_ms: int) -> ProjectReport:
        """Combine all stage results into the final project report."""
        return project_aggregate(self.ctx, stages, total_ms)
