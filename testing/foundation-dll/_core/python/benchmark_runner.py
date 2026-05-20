"""Benchmark stage runner — 3-way managed vs AOT vs JIT performance comparison."""

from __future__ import annotations

import time
from typing import Any

from _core.python.models import FamilyContext, StageResult


def run_benchmark(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 8: 3-way benchmark (managed vs native-aot vs native-jit)."""
    start = time.perf_counter()

    # Delegate to old multi_benchmark_runner via adapter
    try:
        from multi_benchmark_runner import detect_available_runtimes, run_multi_benchmark

        runtimes = detect_available_runtimes()
        if not runtimes:
            return StageResult(
                stage="benchmark", status="skipped",
                summary="No runtimes available for benchmark",
                duration_ms=int((time.perf_counter() - start) * 1000),
            )

        # Placeholder — full migration requires the native benchmark harness
        return StageResult(
            stage="benchmark", status="skipped",
            summary="Benchmark not fully migrated — delegates to old pipeline",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )
    except ImportError:
        return StageResult(
            stage="benchmark", status="skipped",
            summary="multi_benchmark_runner not available",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )
    except Exception as e:
        return StageResult(
            stage="benchmark", status="error",
            summary=f"Benchmark failed: {e}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )
