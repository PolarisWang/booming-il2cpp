"""Stage runners package — chunk pipeline stages only."""
from __future__ import annotations

from verification.orchestration.context import StageResult

from .build import run_build
from .fact_chunk import run_fact_chunk
from .benchmark_chunk import run_benchmark_chunk
from .hotupdate_chunk import run_hotupdate_chunk
from .coverage_audit import run_coverage_audit
from .aggregate import run_aggregate
from .allure_report import run_allure_report

__all__ = [
    "run_build", "run_fact_chunk", "run_benchmark_chunk",
    "run_hotupdate_chunk", "run_coverage_audit", "run_aggregate",
    "run_allure_report",
]
