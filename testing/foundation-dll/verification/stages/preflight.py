"""Stage 0: Preflight — verify contract integrity and family structure."""

from __future__ import annotations

import json
import time
from pathlib import Path
from typing import Any

from verification.orchestration.context import FamilyContext, StageResult
from verification.stages.pre_verification_audit import audit_family


def run_preflight(ctx: FamilyContext, stages: dict[str, StageResult]) -> StageResult:
    """Verify contract exists, then run test-code audit.

    Returns FAILED if the test code is not meaningful (missing handwritten
    implementations for non-auto-callable methods). Returns PASSED with a
    warning if metadata is stale but tests are otherwise intact.
    """
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
            stage="preflight", status="passed",
            summary="0 methods — no verification needed",
            details={
                "methodCount": 0,
                "familyId": contract.get("familyId", ""),
                "displayName": contract.get("displayName", ""),
            },
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # ── Run test-code audit ──
    audit = audit_family(ctx.slug, ctx.assembly)
    audit_elapsed = int((time.perf_counter() - start) * 1000)

    base_details = {
        "methodCount": len(mids),
        "familyId": contract.get("familyId", ""),
        "displayName": contract.get("displayName", ""),
        "auditVerdict": audit["verdict"],
        "auditSummary": audit["summary"],
        "auditIssues": len(audit["issues"]),
        "nonCallableCount": audit["non_callable_count"],
        "coveredNonCallableCount": audit["covered_non_callable_count"],
        "uncoveredCount": audit.get("uncovered_count", 0),
        "declaredMissingCount": audit.get("declared_missing_count", 0),
        "auditDetails": {
            "checks": audit["checks"],
            "issues": audit["issues"],
        },
    }

    if audit["verdict"] == "PASS":
        return StageResult(
            stage="preflight", status="passed",
            summary=f"{len(mids)} methods, audit: {audit['summary']}",
            details=base_details,
            duration_ms=audit_elapsed,
        )

    if audit["verdict"] == "STALE_METADATA":
        return StageResult(
            stage="preflight", status="passed",
            summary=f"{len(mids)} methods, audit: {audit['summary']}",
            details=base_details,
            duration_ms=audit_elapsed,
        )

    # MISSING_HANDWRITTEN → block the pipeline
    errors = [
        i["message"] for i in audit["issues"]
        if i["severity"] in ("FAIL", "ERROR")
    ]
    return StageResult(
        stage="preflight", status="failed",
        summary=f"Test code audit FAILED: {audit['summary']}",
        details=base_details,
        errors=errors,
        duration_ms=audit_elapsed,
    )
