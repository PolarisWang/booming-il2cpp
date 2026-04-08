from __future__ import annotations

from pathlib import Path


def subject_root(repo_root: Path, subject_id: str) -> Path:
    return repo_root / "subjects" / subject_id


def subject_manifest_path(repo_root: Path, subject_id: str) -> Path:
    return subject_root(repo_root, subject_id) / "subject.manifest.json"


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
    artifact_version_root = repo_root / "contracts" / "artifacts" / version
    native_version_root = repo_root / "contracts" / "native" / version
    return {
        "artifactSchemaRoot": artifact_version_root / "schemas",
        "artifactSampleRoot": artifact_version_root / "samples",
        "artifactSnapshotRoot": artifact_version_root / "snapshots",
        "nativeSampleRoot": native_version_root / "samples",
        "artifactSampleCompatibilityRoot": repo_root / "contracts" / "examples" / version / "artifacts",
        "nativeSampleCompatibilityRoot": repo_root / "contracts" / "native" / "examples" / version,
    }
