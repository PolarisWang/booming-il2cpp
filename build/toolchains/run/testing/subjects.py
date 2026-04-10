from __future__ import annotations

from pathlib import Path
from typing import Any
import sys

try:
    from ..core.common import read_json
    from . import path_resolver as path_resolver_module
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from core.common import read_json
    from testing import path_resolver as path_resolver_module


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
    manifest_path = path_resolver_module.subject_manifest_path(repo_root, subject_id)
    if not manifest_path.is_file():
        raise FileNotFoundError(f"subject manifest missing: {manifest_path}")
    return load_subject_manifest_file(manifest_path)


def load_subject_manifest_file(manifest_path: Path) -> dict[str, Any]:
    manifest = read_json(manifest_path)
    if not isinstance(manifest, dict):
        raise RuntimeError(f"subject manifest must be an object: {manifest_path}")
    return manifest


def _sorted_unique(values: list[str]) -> list[str]:
    return sorted({value for value in values if value})


def pipeline_stage_kinds(pipeline: dict[str, Any]) -> list[str]:
    return _sorted_unique(
        [
            str(stage.get("kind") or "")
            for stage in list(pipeline.get("stages") or [])
        ]
    )


def manifest_capabilities(manifest: dict[str, Any]) -> dict[str, Any]:
    validation = dict(manifest.get("validation") or {})
    validation_profiles = dict(manifest.get("validationProfiles") or {})
    frameworks = [
        str(item.get("framework") or "")
        for item in validation.values()
        if isinstance(item, dict)
    ]
    drivers = [
        str(item.get("driver") or "")
        for item in validation.values()
        if isinstance(item, dict)
    ]

    return {
        "category": str(manifest.get("category") or ""),
        "sourceType": str(dict(manifest.get("source") or {}).get("type") or ""),
        "defaultValidationProfile": str(manifest.get("defaultValidationProfile") or ""),
        "pipelineIds": _sorted_unique(
            [str(pipeline.get("pipelineId") or "") for pipeline in list(manifest.get("executionPipelines") or [])]
        ),
        "stageKinds": _sorted_unique(
            [
                stage_kind
                for pipeline in list(manifest.get("executionPipelines") or [])
                for stage_kind in pipeline_stage_kinds(dict(pipeline))
            ]
        ),
        "matrixIds": _sorted_unique(
            [str(matrix.get("matrixId") or "") for matrix in list(manifest.get("environmentMatrices") or [])]
        ),
        "goalIds": _sorted_unique(
            [
                str(goal_id)
                for matrix in list(manifest.get("environmentMatrices") or [])
                for goal_id in list(dict(matrix).get("supportedGoals") or [])
            ]
        ),
        "hostPlatforms": _sorted_unique(
            [
                str(dict(matrix.get("executionContext") or {}).get("hostPlatform") or "")
                for matrix in list(manifest.get("environmentMatrices") or [])
            ]
        ),
        "validationKinds": _sorted_unique(list(validation.keys())),
        "validationProfileIds": _sorted_unique(list(validation_profiles.keys())),
        "validationFrameworks": _sorted_unique(frameworks),
        "validationDrivers": _sorted_unique(drivers),
    }


def load_subject_records(repo_root: Path) -> list[dict[str, Any]]:
    records: list[dict[str, Any]] = []
    for manifest_path in discover_subject_manifests(repo_root):
        manifest = load_subject_manifest_file(manifest_path)
        subject_id = str(manifest.get("subjectId") or "")
        if not subject_id:
            raise RuntimeError(f"subjectId is required: {manifest_path}")
        records.append(
            {
                "subjectId": subject_id,
                "manifestPath": manifest_path,
                "manifest": manifest,
                "capabilities": manifest_capabilities(manifest),
            }
        )
    return records


def query_subject_records(
    records: list[dict[str, Any]],
    *,
    category: str | None = None,
    source_type: str | None = None,
    required_stage_kinds: list[str] | None = None,
    required_goal_ids: list[str] | None = None,
    required_host_platforms: list[str] | None = None,
    required_validation_kinds: list[str] | None = None,
    required_validation_profile_ids: list[str] | None = None,
    required_validation_frameworks: list[str] | None = None,
    required_validation_drivers: list[str] | None = None,
) -> list[dict[str, Any]]:
    matches: list[dict[str, Any]] = []
    for record in records:
        capabilities = dict(record["capabilities"])
        if category is not None and capabilities.get("category") != category:
            continue
        if source_type is not None and capabilities.get("sourceType") != source_type:
            continue
        if required_stage_kinds and not set(required_stage_kinds).issubset(set(capabilities.get("stageKinds") or [])):
            continue
        if required_goal_ids and not set(required_goal_ids).issubset(set(capabilities.get("goalIds") or [])):
            continue
        if required_host_platforms and not set(required_host_platforms).issubset(set(capabilities.get("hostPlatforms") or [])):
            continue
        if required_validation_kinds and not set(required_validation_kinds).issubset(set(capabilities.get("validationKinds") or [])):
            continue
        if required_validation_profile_ids and not set(required_validation_profile_ids).issubset(set(capabilities.get("validationProfileIds") or [])):
            continue
        if required_validation_frameworks and not set(required_validation_frameworks).issubset(set(capabilities.get("validationFrameworks") or [])):
            continue
        if required_validation_drivers and not set(required_validation_drivers).issubset(set(capabilities.get("validationDrivers") or [])):
            continue
        matches.append(record)
    return matches


def require_single_subject_record(records: list[dict[str, Any]], **filters: Any) -> dict[str, Any]:
    matches = query_subject_records(records, **filters)
    if len(matches) != 1:
        raise ValueError(f"expected exactly one subject record, found {len(matches)}")
    return matches[0]


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


def find_matrix_with_stage_kind(manifest: dict[str, Any], stage_kind: str) -> dict[str, Any]:
    for matrix in list(manifest.get("environmentMatrices") or []):
        pipeline = find_pipeline(manifest, str(matrix.get("pipelineId") or ""))
        if stage_kind in pipeline_stage_kinds(pipeline):
            return matrix
    raise ValueError(f"matrix not found for stage kind: {stage_kind}")


def find_validation(manifest: dict[str, Any], validation_kind: str) -> dict[str, Any]:
    validation = dict(manifest.get("validation") or {})
    if validation_kind not in validation:
        raise ValueError(f"validation not found: {validation_kind}")
    return dict(validation[validation_kind])


def resolve_validation_selection(
    manifest: dict[str, Any],
    *,
    validation_profile_id: str | None = None,
    validation_kind: str | None = None,
    variant: str | None = None,
) -> dict[str, Any]:
    validation_profiles = dict(manifest.get("validationProfiles") or {})
    validation = dict(manifest.get("validation") or {})

    selected_profile_id = validation_profile_id or str(manifest.get("defaultValidationProfile") or "")
    if not selected_profile_id:
        raise ValueError("subject manifest missing defaultValidationProfile")
    if selected_profile_id not in validation_profiles:
        raise ValueError(f"validation profile not found: {selected_profile_id}")

    selected_validation_kinds = [str(item) for item in list(validation_profiles.get(selected_profile_id) or []) if str(item)]
    if not selected_validation_kinds:
        raise ValueError(f"validation profile '{selected_profile_id}' does not declare validations")
    for current_validation_kind in selected_validation_kinds:
        if current_validation_kind not in validation:
            raise ValueError(
                f"validation profile '{selected_profile_id}' references unknown validation '{current_validation_kind}'"
            )

    selected_validation_kind = validation_kind or None
    if selected_validation_kind is not None and selected_validation_kind not in selected_validation_kinds:
        raise ValueError(
            f"validation '{selected_validation_kind}' is not part of validation profile '{selected_profile_id}'"
        )

    variant_source_kinds = [selected_validation_kind] if selected_validation_kind else list(selected_validation_kinds)
    default_variants = [
        str(dict(validation[current_validation_kind]).get("defaultVariant") or "")
        for current_validation_kind in variant_source_kinds
    ]
    if any(not item for item in default_variants):
        raise ValueError(f"validation profile '{selected_profile_id}' is missing defaultVariant")

    unique_default_variants: list[str] = []
    for current_variant in default_variants:
        if current_variant not in unique_default_variants:
            unique_default_variants.append(current_variant)
    if len(unique_default_variants) != 1:
        if not variant:
            raise ValueError(
                f"validation profile '{selected_profile_id}' resolves to multiple default variants: {unique_default_variants}"
            )

        default_variant = str(variant)
    else:
        default_variant = unique_default_variants[0]

    selected_variant = variant or default_variant
    return {
        "validationProfileId": selected_profile_id,
        "validationKinds": selected_validation_kinds,
        "validationKind": selected_validation_kind,
        "defaultVariant": default_variant,
        "variant": selected_variant,
    }


def subject_artifact_roots(subject_id: str, matrix_id: str, *, run_id: str | None = None) -> dict[str, str]:
    subject_root = path_resolver_module.subject_artifacts_root(Path("."), subject_id).as_posix()
    runs_root = path_resolver_module.subject_runs_root(Path("."), subject_id).as_posix()
    run_root = f"{runs_root}/{run_id or 'subject-exec'}"
    matrix_root = f"{run_root}/matrices/{matrix_id}"
    return {
        "subjectRoot": subject_root,
        "runsRoot": runs_root,
        "runRoot": run_root,
        "analysisRoot": f"{run_root}/analysis",
        "matrixRoot": matrix_root,
        "pipelineReportRoot": f"{matrix_root}/pipeline-report",
        "validationRoot": f"{matrix_root}/validations",
        "runReportRoot": f"{run_root}/run-report",
        "subjectReportRoot": f"{run_root}/subject-report",
    }


def bucket_manifest_name(bucket: str) -> str:
    if bucket not in BUCKET_MANIFEST_NAMES:
        raise ValueError(f"unsupported bucket: {bucket}")
    return BUCKET_MANIFEST_NAMES[bucket]


def stage_paths(
    subject_id: str,
    matrix_id: str,
    *,
    run_id: str | None = None,
    bucket: str,
    scope: str,
    kind: str,
) -> dict[str, Any]:
    roots = subject_artifact_roots(subject_id, matrix_id, run_id=run_id)

    if scope == "shared":
        bucket_root = f"{roots['analysisRoot']}/{bucket}"
    elif scope == "matrix":
        bucket_root = roots["pipelineReportRoot"] if bucket == "report" else f"{roots['matrixRoot']}/{bucket}"
    else:
        raise ValueError(f"unsupported scope: {scope}")

    manifest_name = bucket_manifest_name(bucket)
    manifest_path = f"{bucket_root}/{manifest_name}" if bucket != "report" else f"{bucket_root}/report.json"

    report_paths: list[str] = []
    if kind == "analysis-frontend":
        report_paths.append(f"{bucket_root}/contract-validate.report.json")
    elif kind == "runtime-engine-observe":
        report_paths.append(f"{bucket_root}/engine-observe.report.json")
    elif kind == "runtime-engine-trace-compare":
        report_paths.append(f"{bucket_root}/engine-trace-compare.report.json")
    elif kind == "runtime-trace-compare":
        report_paths.append(f"{bucket_root}/trace-compare.report.json")

    return {
        "bucketRoot": bucket_root,
        "manifestPath": manifest_path,
        "reportPaths": report_paths,
    }
