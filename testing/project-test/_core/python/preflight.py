"""Stage 0: Preflight — verify project.json and directory structure."""

from __future__ import annotations

import json
import time

from _pipeline.stage_result import StageResult


REQUIRED_PROJECT_FIELDS = {
    "schemaVersion": str,
    "projectId": str,
    "entryPoint": str,
}

RECOMMENDED_FIELDS = {
    "displayName": str,
    "description": str,
    "assemblies": list,
}


def run_project_preflight(ctx, stages) -> StageResult:
    """Verify project.json exists and contains valid metadata."""
    start = time.perf_counter()

    contract_path = ctx.contract_path
    if not contract_path.exists():
        return StageResult(
            stage="preflight", status="failed",
            summary=f"project.json not found: {contract_path}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    try:
        with open(contract_path, encoding="utf-8") as f:
            meta = json.load(f)
    except (json.JSONDecodeError, OSError) as e:
        return StageResult(
            stage="preflight", status="failed",
            summary=f"Failed to read project.json: {e}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Check required fields
    missing = [f for f, t in REQUIRED_PROJECT_FIELDS.items()
               if f not in meta or not isinstance(meta.get(f), t)]
    if missing:
        return StageResult(
            stage="preflight", status="failed",
            summary=f"Missing/invalid fields: {', '.join(missing)}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Verify managed source directory exists
    managed_dir = ctx.managed_dir
    cs_files = list(managed_dir.glob("*.cs")) if managed_dir.exists() else []
    csproj_files = list(managed_dir.glob("*.csproj")) if managed_dir.exists() else []

    warnings = []
    if not csproj_files:
        warnings.append("no .csproj found in managed/src/")
    if not cs_files:
        warnings.append("no .cs files found in managed/src/")

    return StageResult(
        stage="preflight", status="passed",
        summary=f"{meta.get('projectId', '?')}: {len(cs_files)} .cs files",
        details={
            "projectId": meta.get("projectId", ""),
            "displayName": meta.get("displayName", ""),
            "entryPoint": meta.get("entryPoint", ""),
            "csFileCount": len(cs_files),
            "csprojCount": len(csproj_files),
            "warnings": warnings,
            "subjects": meta.get("subjects", []),
        },
        duration_ms=int((time.perf_counter() - start) * 1000),
    )
