from __future__ import annotations

from pathlib import Path


SUBJECT_MANIFEST_NAME = "subject.manifest.json"


def _profile_relative_path(profile_id: str) -> Path:
    parts = [part for part in str(profile_id).split("/") if part]
    if not parts:
        raise ValueError("profile_id must be non-empty")
    return Path(*parts).with_suffix(".json")


def repo_root_from_subject_manifest(manifest_path: Path) -> Path:
    for parent in manifest_path.parents:
        if parent.name == "subjects":
            fixtures_parent = parent.parent
            if fixtures_parent.name == "fixtures" and fixtures_parent.parent.name == "tests":
                return fixtures_parent.parent.parent
            return fixtures_parent
    raise ValueError(f"subject manifest path must be rooted under subjects/: {manifest_path}")


def _normalize_subject_id(subject_id: str) -> str:
    return str(subject_id).strip()


def subject_root(repo_root: Path, subject_id: str) -> Path:
    return repo_root / "subjects" / _normalize_subject_id(subject_id)


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
