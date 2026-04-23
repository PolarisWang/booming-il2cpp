from __future__ import annotations

from pathlib import Path

try:
    from . import verification_layout as verification_layout_module
except ImportError:
    import sys

    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from testing import verification_layout as verification_layout_module


SUBJECT_MANIFEST_NAME = verification_layout_module.OWNER_MANIFEST_NAME


def _profile_relative_path(profile_id: str) -> Path:
    parts = [part for part in str(profile_id).split("/") if part]
    if not parts:
        raise ValueError("profile_id must be non-empty")
    return Path(*parts).with_suffix(".json")


def repo_root_from_subject_manifest(manifest_path: Path) -> Path:
    return verification_layout_module.repo_root_from_owner_manifest(manifest_path)


def _normalize_subject_id(subject_id: str) -> str:
    return str(subject_id).strip()


def subject_root(repo_root: Path, subject_id: str) -> Path:
    return verification_layout_module.owner_root(repo_root, _normalize_subject_id(subject_id))


def subject_manifest_path(repo_root: Path, subject_id: str) -> Path:
    return verification_layout_module.owner_manifest_path(repo_root, _normalize_subject_id(subject_id))


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
    owner_id = _normalize_subject_id(subject_id)
    return {
        "validationRoot": verification_layout_module.owner_evidence_root(repo_root, owner_id) / "validation",
        "expectedRoot": verification_layout_module.owner_evidence_root(repo_root, owner_id) / "expected",
        "baselinesRoot": verification_layout_module.owner_benchmark_baselines_root(repo_root, owner_id),
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


def subject_codegen_baseline_path(
    repo_root: Path,
    subject_id: str,
    matrix_id: str,
    host_platform: str,
) -> Path:
    return verification_layout_module.owner_codegen_stubs_root(
        repo_root,
        _normalize_subject_id(subject_id),
    ) / matrix_id / f"{host_platform}.json"


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
