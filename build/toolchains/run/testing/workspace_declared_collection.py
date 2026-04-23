from __future__ import annotations

import json
from pathlib import Path
from typing import Any

try:
    from . import workspace_manifests as workspace_manifests_module
    from . import verification_layout as verification_layout_module
except ImportError:
    root = Path(__file__).resolve().parents[1]
    import sys

    sys.path.insert(0, str(root))
    from testing import workspace_manifests as workspace_manifests_module
    from testing import verification_layout as verification_layout_module


def _load_subject_manifest(repo_root: Path, subject_id: str) -> dict[str, Any] | None:
    manifest_path = verification_layout_module.owner_manifest_path(repo_root, subject_id)
    if not manifest_path.is_file():
        return None

    try:
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    except json.JSONDecodeError:
        return None

    return manifest if isinstance(manifest, dict) else None


def _is_relevant_source_file(path: Path) -> bool:
    if not path.is_file():
        return False
    if path.suffix.lower() in {".cs", ".csproj", ".sln", ".props", ".targets"}:
        return True
    return path.name in {"Directory.Build.props", "Directory.Build.targets"}


def _workspace_collection_is_stale(repo_root: Path, subject_id: str, collection_path: Path) -> bool:
    try:
        collection_mtime = collection_path.stat().st_mtime
    except OSError:
        return True

    subject_manifest_path = verification_layout_module.owner_manifest_path(repo_root, subject_id)
    try:
        if subject_manifest_path.is_file() and subject_manifest_path.stat().st_mtime > collection_mtime:
            return True
    except OSError:
        return True

    subject_manifest = _load_subject_manifest(repo_root, subject_id)
    if subject_manifest is None:
        return False

    source = dict(subject_manifest.get("source") or {})
    source_path_text = str(source.get("path") or "").strip()
    if not source_path_text:
        return False

    source_path = repo_root / source_path_text
    source_root = source_path if source_path.is_dir() else source_path.parent
    if not source_root.is_dir():
        return False

    for candidate in source_root.rglob("*"):
        if not _is_relevant_source_file(candidate):
            continue
        try:
            if candidate.stat().st_mtime > collection_mtime:
                return True
        except OSError:
            return True
    return False


def _managed_test_projects_with_collections(manifest: dict[str, Any], *, host_kind: str) -> list[dict[str, Any]]:
    preferred: list[dict[str, Any]] = []
    fallback: list[dict[str, Any]] = []
    for item in list(manifest.get("managedTestProjects") or []):
        if not isinstance(item, dict):
            continue
        collection_path = str(item.get("collectionPath") or "").strip()
        if not collection_path:
            continue
        normalized_item = dict(item)
        if str(item.get("hostKind") or "").strip() == host_kind:
            preferred.append(normalized_item)
            continue
        fallback.append(normalized_item)
    return [*preferred, *fallback]


def load_workspace_declared_collection(
    repo_root: Path,
    subject_id: str,
    *,
    host_kind: str,
) -> dict[str, Any] | None:
    loaded_manifest = workspace_manifests_module.load_subject_workspace_manifest(repo_root, subject_id)
    if loaded_manifest is None:
        return None
    _, manifest = loaded_manifest

    for project in _managed_test_projects_with_collections(manifest, host_kind=host_kind):
        collection_path = str(project.get("collectionPath") or "").strip()
        if not collection_path:
            continue
        collection_abspath = repo_root / collection_path
        if not collection_abspath.is_file():
            continue
        if _workspace_collection_is_stale(repo_root, subject_id, collection_abspath):
            continue
        try:
            collection = json.loads(collection_abspath.read_text(encoding="utf-8"))
        except json.JSONDecodeError:
            continue
        if isinstance(collection, dict):
            return collection
    return None
