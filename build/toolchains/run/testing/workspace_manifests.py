from __future__ import annotations

import json
from pathlib import Path
from typing import Any


WORKSPACE_MANIFEST_VERSION = 2


def subject_workspace_manifest_path(repo_root: Path, subject_id: str) -> Path:
    return repo_root / "solutions" / "subjects" / subject_id / "workspace.manifest.json"


def load_subject_workspace_manifest(repo_root: Path, subject_id: str) -> tuple[Path, dict[str, Any]] | None:
    manifest_path = subject_workspace_manifest_path(repo_root, subject_id)
    if not manifest_path.is_file():
        return None

    try:
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    except json.JSONDecodeError:
        return None

    if not isinstance(manifest, dict):
        return None

    try:
        workspace_version = int(manifest.get("workspaceVersion") or 0)
    except (TypeError, ValueError):
        return None
    if workspace_version < WORKSPACE_MANIFEST_VERSION:
        return None

    return manifest_path, manifest


def find_managed_test_project(manifest: dict[str, Any], *, host_kind: str) -> dict[str, Any] | None:
    for item in list(manifest.get("managedTestProjects") or []):
        if not isinstance(item, dict):
            continue
        if str(item.get("hostKind") or "").strip() != host_kind:
            continue
        return dict(item)
    return None


def find_managed_test_project_by_id(manifest: dict[str, Any], *, project_id: str) -> dict[str, Any] | None:
    for item in list(manifest.get("managedTestProjects") or []):
        if not isinstance(item, dict):
            continue
        if str(item.get("projectId") or "").strip() != project_id:
            continue
        return dict(item)
    return None


def find_native_test_project(
    manifest: dict[str, Any],
    *,
    matrix_id: str,
    host_kind: str,
) -> dict[str, Any] | None:
    for item in list(manifest.get("nativeTestProjects") or []):
        if not isinstance(item, dict):
            continue
        if str(item.get("matrixId") or "").strip() != matrix_id:
            continue
        if str(item.get("hostKind") or "").strip() != host_kind:
            continue
        return dict(item)
    return None


def find_hotupdate_test_project(
    manifest: dict[str, Any],
    *,
    host_kind: str,
) -> dict[str, Any] | None:
    for item in list(manifest.get("hotupdateTestProjects") or []):
        if not isinstance(item, dict):
            continue
        if str(item.get("hostKind") or "").strip() != host_kind:
            continue
        return dict(item)
    return None


def find_hotupdate_patch_projects(
    manifest: dict[str, Any],
    *,
    matrix_id: str = "",
) -> list[dict[str, Any]]:
    project_ids: list[str] = []
    if matrix_id:
        for matrix in list(manifest.get("matrices") or []):
            if not isinstance(matrix, dict):
                continue
            if str(matrix.get("matrixId") or "").strip() != matrix_id:
                continue
            project_ids = [str(item) for item in list(matrix.get("hotupdatePatchProjectIds") or []) if str(item)]
            break

    records_by_id = {
        str(item.get("projectId") or "").strip(): dict(item)
        for item in list(manifest.get("hotupdatePatchProjects") or [])
        if isinstance(item, dict) and str(item.get("projectId") or "").strip()
    }
    if not project_ids:
        return list(records_by_id.values())

    projects: list[dict[str, Any]] = []
    for project_id in project_ids:
        record = dict(records_by_id.get(project_id) or {})
        if record:
            projects.append(record)
    return projects
