from __future__ import annotations

from pathlib import Path
from typing import Any
import sys

try:
    from ..common import read_json
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from common import read_json


SUBJECT_MANIFEST_NAME = "subject.manifest.json"
BUCKET_MANIFEST_NAMES = {
    "source": "source.manifest.json",
    "host-input": "host-input.manifest.json",
    "analysis": "analysis.manifest.json",
    "generated": "generated.manifest.json",
    "build": "build.manifest.json",
    "runtime": "runtime.manifest.json",
    "report": "report.json",
}


def discover_subject_manifests(repo_root: Path) -> list[Path]:
    subject_root = repo_root / "subjects"
    if not subject_root.is_dir():
        return []
    return sorted(subject_root.rglob(SUBJECT_MANIFEST_NAME))


def load_subject_manifest(repo_root: Path, subject_id: str) -> dict[str, Any]:
    manifest_path = repo_root / "subjects" / subject_id / SUBJECT_MANIFEST_NAME
    if not manifest_path.is_file():
        raise FileNotFoundError(f"subject manifest missing: {manifest_path}")
    manifest = read_json(manifest_path)
    if not isinstance(manifest, dict):
        raise RuntimeError(f"subject manifest must be an object: {manifest_path}")
    return manifest


def find_pipeline(manifest: dict[str, Any], pipeline_id: str) -> dict[str, Any]:
    for pipeline in list(manifest.get("executionPipelines") or []):
        if pipeline.get("pipelineId") == pipeline_id:
            return pipeline
    raise ValueError(f"pipeline not found: {pipeline_id}")


def find_matrix(manifest: dict[str, Any], matrix_id: str) -> dict[str, Any]:
    for matrix in list(manifest.get("environmentMatrices") or []):
        if matrix.get("matrixId") == matrix_id:
            return matrix
    raise ValueError(f"matrix not found: {matrix_id}")


def subject_artifact_roots(subject_id: str, matrix_id: str) -> dict[str, str]:
    subject_root = f"artifacts/subjects/{subject_id}"
    return {
        "subjectRoot": subject_root,
        "sharedRoot": f"{subject_root}/shared",
        "matrixRoot": f"{subject_root}/matrices/{matrix_id}",
        "subjectReportRoot": f"{subject_root}/subject-report",
    }


def bucket_manifest_name(bucket: str) -> str:
    if bucket not in BUCKET_MANIFEST_NAMES:
        raise ValueError(f"unsupported bucket: {bucket}")
    return BUCKET_MANIFEST_NAMES[bucket]


def stage_paths(
    subject_id: str,
    matrix_id: str,
    *,
    bucket: str,
    scope: str,
    kind: str,
) -> dict[str, Any]:
    roots = subject_artifact_roots(subject_id, matrix_id)

    if scope == "shared":
        bucket_root = f"{roots['sharedRoot']}/{bucket}"
    elif scope == "matrix":
        bucket_root = roots["matrixRoot"] if bucket == "report" else f"{roots['matrixRoot']}/{bucket}"
    else:
        raise ValueError(f"unsupported scope: {scope}")

    manifest_name = bucket_manifest_name(bucket)
    manifest_path = f"{bucket_root}/{manifest_name}" if bucket != "report" else f"{roots['matrixRoot']}/report.json"

    report_paths: list[str] = []
    if kind == "analysis-frontend":
        report_paths.append(f"{bucket_root}/contract-validate.report.json")
    elif kind == "runtime-trace-compare":
        report_paths.append(f"{bucket_root}/trace-compare.report.json")

    return {
        "bucketRoot": bucket_root,
        "manifestPath": manifest_path,
        "reportPaths": report_paths,
    }
