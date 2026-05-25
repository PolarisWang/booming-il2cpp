"""AsmCompare stage runner — JIT vs AOT instruction-level analysis."""

from __future__ import annotations

import sys
import time
from pathlib import Path
from typing import Any

from orchestration.context import FamilyContext, StageResult


def run_asm_compare(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Stage 6: JIT vs AOT instruction-level comparison (delegates to old pipeline)."""
    start = time.perf_counter()

    # Ensure old pipeline path is available for asm_compare_verifier
    _old_pipeline = (
        Path(__file__).resolve().parents[4] / "build" / "toolchains" / "run" / "testing" / "foundation_dll"
    )
    if str(_old_pipeline) not in sys.path:
        sys.path.insert(0, str(_old_pipeline))

    try:
        from asm_compare_verifier import verify_family_asm_compare

        v_base = ctx.family_dir.parent.parent
        result = verify_family_asm_compare(ctx.slug, assembly=ctx.assembly, verification=v_base)

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
