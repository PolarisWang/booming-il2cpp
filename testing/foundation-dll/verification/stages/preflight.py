"""Stage 0: Preflight — verify contract integrity and family structure."""

from __future__ import annotations

import json
import time
from pathlib import Path
from typing import Any

from verification.orchestration.context import FamilyContext, StageResult


def run_preflight(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Verify contract.json exists and contains valid methodSubjectIds."""
    start = time.perf_counter()
    contract_path = ctx.contract_path

    if not contract_path.exists():
        return StageResult(
            stage="preflight", status="failed",
            summary=f"Contract not found: {contract_path}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    try:
        with open(contract_path, encoding="utf-8") as f:
            contract = json.load(f)
    except (json.JSONDecodeError, OSError) as e:
        return StageResult(
            stage="preflight", status="failed",
            summary=f"Failed to read contract: {e}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    mids = contract.get("methodSubjectIds", [])
    if not mids:
        return StageResult(
            stage="preflight", status="failed",
            summary="No methodSubjectIds found in contract",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    return StageResult(
        stage="preflight", status="passed",
        summary=f"{len(mids)} methods",
        details={
            "methodCount": len(mids),
            "familyId": contract.get("familyId", ""),
            "displayName": contract.get("displayName", ""),
        },
        duration_ms=int((time.perf_counter() - start) * 1000),
    )
