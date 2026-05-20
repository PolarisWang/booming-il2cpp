"""Microbench stage runner — interpreter internal metrics."""

from __future__ import annotations

import time
from typing import Any

from _core.python.models import FamilyContext, StageResult


def run_microbench(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 7: Interpreter internal metrics (FramePool, FastExecute, CallVirt dispatch)."""
    start = time.perf_counter()

    # Not yet migrated — placeholder
    return StageResult(
        stage="microbench", status="skipped",
        summary="Microbench not yet migrated to new framework",
        duration_ms=int((time.perf_counter() - start) * 1000),
    )
