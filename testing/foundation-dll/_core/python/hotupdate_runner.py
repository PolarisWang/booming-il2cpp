"""HotUpdate stage runners — patch data generation and verification."""

from __future__ import annotations

import time
from typing import Any

from _core.python.models import FamilyContext, StageResult


def run_hotupdate(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 9: HotUpdate AOT Fact."""
    start = time.perf_counter()
    # Placeholder — delegates to old pipeline
    return StageResult(
        stage="hotupdate", status="skipped",
        summary="HotUpdate not yet migrated to new framework",
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def run_hotupdate_aot_bench(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 10: HotUpdate AOT Bench."""
    start = time.perf_counter()
    return StageResult(
        stage="hotupdate_aot_benchmark", status="skipped",
        summary="HotUpdate AOT Bench not yet migrated",
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def run_hotupdate_jit_fact(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 11: HotUpdate JIT Fact."""
    start = time.perf_counter()
    return StageResult(
        stage="hotupdate_jit_fact", status="skipped",
        summary="HotUpdate JIT Fact not yet migrated",
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def run_hotupdate_jit_bench(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 12: HotUpdate JIT Bench."""
    start = time.perf_counter()
    return StageResult(
        stage="hotupdate_jit_benchmark", status="skipped",
        summary="HotUpdate JIT Bench not yet migrated",
        duration_ms=int((time.perf_counter() - start) * 1000),
    )
