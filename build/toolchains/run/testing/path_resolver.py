from __future__ import annotations

import json
from pathlib import Path


SUBJECT_MANIFEST_NAME = "subject.manifest.json"
_LEGACY_SUBJECT_ALIAS_FIELDS = ("legacySubjectIds", "subjectAliases", "aliases")


def _profile_relative_path(profile_id: str) -> Path:
    parts = [part for part in str(profile_id).split("/") if part]
    if not parts:
        raise ValueError("profile_id must be non-empty")
    return Path(*parts).with_suffix(".json")


def repo_root_from_subject_manifest(manifest_path: Path) -> Path:
    for parent in manifest_path.parents:
        if parent.name == "subjects":
            return parent.parent
    raise ValueError(f"subject manifest path must be rooted under subjects/: {manifest_path}")


def _iter_top_level_subject_manifest_paths(repo_root: Path) -> list[Path]:
    subjects_root = repo_root / "subjects"
    if not subjects_root.is_dir():
        return []

    manifest_paths: list[Path] = []
    for candidate in subjects_root.iterdir():
        if not candidate.is_dir():
            continue
        manifest_path = candidate / SUBJECT_MANIFEST_NAME
        if manifest_path.is_file():
            manifest_paths.append(manifest_path)
    return sorted(manifest_paths)


def _legacy_subject_aliases(manifest_path: Path) -> set[str]:
    try:
        payload = json.loads(manifest_path.read_text(encoding="utf-8"))
    except (OSError, UnicodeDecodeError, json.JSONDecodeError):
        return set()

    compatibility = payload.get("compatibility")
    if not isinstance(compatibility, dict):
        return set()

    aliases: set[str] = set()
    for field_name in _LEGACY_SUBJECT_ALIAS_FIELDS:
        value = compatibility.get(field_name, [])
        if not isinstance(value, list):
            continue
        for item in value:
            alias = str(item or "").strip()
            if alias:
                aliases.add(alias)
    return aliases


def _resolve_subject_id(repo_root: Path, subject_id: str) -> str:
    normalized_subject_id = str(subject_id).strip()
    if not normalized_subject_id:
        return normalized_subject_id

    direct_manifest_path = repo_root / "subjects" / normalized_subject_id / SUBJECT_MANIFEST_NAME
    if direct_manifest_path.is_file():
        return normalized_subject_id

    for manifest_path in _iter_top_level_subject_manifest_paths(repo_root):
        if normalized_subject_id in _legacy_subject_aliases(manifest_path):
            return manifest_path.parent.name
    return normalized_subject_id


def subject_root(repo_root: Path, subject_id: str) -> Path:
    return repo_root / "subjects" / _resolve_subject_id(repo_root, subject_id)


def subject_manifest_path(repo_root: Path, subject_id: str) -> Path:
    return subject_root(repo_root, subject_id) / SUBJECT_MANIFEST_NAME


def orchestration_root(repo_root: Path) -> Path:
    return repo_root / "testing" / "orchestration"


def pipeline_profile_path(repo_root: Path, profile_id: str) -> Path:
    return orchestration_root(repo_root) / "pipelines" / _profile_relative_path(profile_id)


def matrix_profile_path(repo_root: Path, profile_id: str) -> Path:
    return orchestration_root(repo_root) / "matrices" / _profile_relative_path(profile_id)


def budget_profile_path(repo_root: Path, profile_id: str) -> Path:
    return orchestration_root(repo_root) / "budgets" / _profile_relative_path(profile_id)


def baseline_profile_path(repo_root: Path, profile_id: str) -> Path:
    return orchestration_root(repo_root) / "baselines" / _profile_relative_path(profile_id)


def subject_artifacts_root(repo_root: Path, subject_id: str) -> Path:
    return repo_root / "artifacts" / "subjects" / subject_id


def subject_runs_root(repo_root: Path, subject_id: str) -> Path:
    return subject_artifacts_root(repo_root, subject_id) / "runs"


def subject_run_root(repo_root: Path, subject_id: str, run_id: str) -> Path:
    return subject_runs_root(repo_root, subject_id) / run_id


def subject_content_roots(repo_root: Path, subject_id: str) -> dict[str, Path]:
    root = subject_root(repo_root, subject_id)
    return {
        "validationRoot": root / "validation",
        "expectedRoot": root / "expected",
        "baselinesRoot": root / "baselines",
    }


def subject_perf_baseline_path(
    repo_root: Path,
    subject_id: str,
    matrix_id: str,
    host_platform: str,
) -> Path:
    return (
        subject_content_roots(repo_root, subject_id)["baselinesRoot"]
        / "perf"
        / matrix_id
        / f"{host_platform}.json"
    )


def contract_roots(repo_root: Path, *, version: str = "v0") -> dict[str, Path]:
    artifact_schema_root = repo_root / "contracts" / "artifacts" / version / "schemas"
    analysis_version_root = repo_root / "tests" / "contracts" / "analysis" / version
    native_version_root = repo_root / "tests" / "contracts" / "native" / version
    return {
        "artifactSchemaRoot": artifact_schema_root,
        "artifactSampleRoot": analysis_version_root / "samples",
        "artifactSnapshotRoot": analysis_version_root / "snapshots",
        "nativeSampleRoot": native_version_root / "samples",
    }
