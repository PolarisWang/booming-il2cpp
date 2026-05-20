"""AsmCompare stage runner — JIT vs AOT instruction-level analysis."""

from __future__ import annotations

import time
from typing import Any

from _core.python.models import FamilyContext, StageResult


def run_asm_compare(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 6: JIT vs AOT instruction-level comparison (delegates to old pipeline)."""
    start = time.perf_counter()

    # Delegate to old asm_compare_verifier module
    try:
        from asm_compare_verifier import run_asm_compare as old_asm_compare
        result = old_asm_compare(ctx.slug, assembly=ctx.assembly)

        status = "passed" if result.get("status") == "passed" else "failed"
        return StageResult(
            stage="asm_compare", status=status,
            summary=result.get("summary", ""),
            details=result,
            duration_ms=int((time.perf_counter() - start) * 1000),
        )
    except ImportError:
        return StageResult(
            stage="asm_compare", status="skipped",
            summary="asm_compare_verifier not available",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )
    except Exception as e:
        return StageResult(
            stage="asm_compare", status="error",
            summary=f"AsmCompare failed: {e}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )
