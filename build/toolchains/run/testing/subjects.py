from __future__ import annotations

from enum import Enum
from pathlib import Path
import re
from typing import Any
import sys

try:
    from ..core.common import read_json
    from . import declarations as declarations_module
    from . import path_resolver as path_resolver_module
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from core.common import read_json
    from testing import declarations as declarations_module
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


class SourceModel(str, Enum):
    DOTNET_SOLUTION = "dotnet-solution"
    DOTNET_PROJECT_SET = "dotnet-project-set"
    HOST_PLUS_PATCH = "host-plus-patch"
    MIXED_SOLUTION = "mixed-solution"


class DependencyModel(str, Enum):
    PROJECT_REFERENCE = "project-reference"
    PACKAGE_REFERENCE = "package-reference"
    BINARY_REFERENCE = "binary-reference"
    MIXED = "mixed"


class ExecutablePlan(str, Enum):
    MANAGED_HOST = "managed-host"
    GENERATED_NATIVE = "generated-native"
    DEVICE_PACKAGE = "device-package"
    HOST_PLUS_HOT_UPDATE_PATCH = "host-plus-hot-update-patch"


class EngineeringProfile(str, Enum):
    MANAGED_OUTPUT = "managed-output"
    NATIVE_EXECUTABLE = "native-executable"
    DEVICE_PACKAGE = "device-package"
    HOT_UPDATE_HOST = "hot-update-host"


class AvailabilityStatus(str, Enum):
    READY = "ready"
    PLANNED = "planned"
    BLOCKED = "blocked"
    UNSUPPORTED = "unsupported"


_PROJECT_REFERENCE_PATTERN = re.compile(r"<ProjectReference(?:\s|>)", re.IGNORECASE)
_PACKAGE_REFERENCE_PATTERN = re.compile(r"<PackageReference(?:\s|>)", re.IGNORECASE)
_BINARY_REFERENCE_PATTERN = re.compile(r"<(?:Reference|HintPath)(?:\s|>)", re.IGNORECASE)
_SOLUTION_PROJECT_PATTERN = re.compile(
    r'^Project\("[^"]+"\)\s*=\s*"[^"]+",\s*"([^"]+\.csproj)"\s*,',
    re.IGNORECASE,
)
_ASSEMBLY_NAME_PATTERN = re.compile(r"<AssemblyName>\s*([^<]+)\s*</AssemblyName>", re.IGNORECASE)
_ORCHESTRATION_REF_FIELDS = (
    "matrixProfile",
    "pipelineProfile",
    "budgetProfile",
    "baselineProfile",
)


def _normalize_enum_value(
    value: Any,
    enum_type: type[Enum],
    *,
    field_name: str,
) -> str:
    try:
        return enum_type(str(value).strip()).value
    except ValueError as error:
        raise ValueError(f"unsupported {field_name}: {value}") from error


def _project_files(subject_root: Path) -> list[Path]:
    source_root = subject_root / "source"
    if not source_root.is_dir():
        return []
    return sorted(source_root.rglob("*.csproj"))


def _derive_source_model(manifest_path: Path) -> str:
    source_root = manifest_path.parent / "source"
    if source_root.is_dir() and any(source_root.rglob("*.sln")):
        return SourceModel.DOTNET_SOLUTION.value
    return SourceModel.DOTNET_PROJECT_SET.value


def _derive_dependency_model(manifest_path: Path) -> str:
    dependency_kinds: set[str] = set()
    for project_file in _project_files(manifest_path.parent):
        content = project_file.read_text(encoding="utf-8")
        if _PROJECT_REFERENCE_PATTERN.search(content):
            dependency_kinds.add(DependencyModel.PROJECT_REFERENCE.value)
        if _PACKAGE_REFERENCE_PATTERN.search(content):
            dependency_kinds.add(DependencyModel.PACKAGE_REFERENCE.value)
        if _BINARY_REFERENCE_PATTERN.search(content):
            dependency_kinds.add(DependencyModel.BINARY_REFERENCE.value)

    if not dependency_kinds:
        return DependencyModel.PROJECT_REFERENCE.value
    if len(dependency_kinds) == 1:
        return next(iter(dependency_kinds))
    return DependencyModel.MIXED.value


def _normalize_optional_string(value: Any, *, field_name: str) -> str:
    normalized = str(value or "").strip()
    if not normalized:
        raise ValueError(f"{field_name} must be a non-empty string")
    return normalized


def resolve_source_primary_project_path(source_or_manifest: dict[str, Any]) -> str:
    payload = dict(source_or_manifest.get("source") or source_or_manifest)
    source_path = str(payload.get("path") or "").strip()
    primary_project_path = str(
        payload.get("primaryProjectPath")
        or payload.get("primaryProject")
        or payload.get("projectPath")
        or ""
    ).strip()
    if primary_project_path:
        return primary_project_path
    if source_path.endswith(".csproj"):
        return source_path
    raise ValueError("solution source requires source.primaryProjectPath")


def resolve_source_solution_assembly_names(repo_root: Path, source_or_manifest: dict[str, Any]) -> list[str]:
    payload = dict(source_or_manifest.get("source") or source_or_manifest)
    source_path = str(payload.get("path") or "").strip()
    if not source_path.endswith(".sln"):
        return []

    solution_path = Path(source_path)
    if not solution_path.is_absolute():
        solution_path = repo_root / solution_path
    if not solution_path.is_file():
        return []

    assembly_names: list[str] = []
    for line in solution_path.read_text(encoding="utf-8").splitlines():
        match = _SOLUTION_PROJECT_PATTERN.match(line.strip())
        if match is None:
            continue

        project_path = Path(match.group(1).replace("\\", "/"))
        if not project_path.is_absolute():
            project_path = solution_path.parent / project_path
        if not project_path.is_file():
            continue

        project_text = project_path.read_text(encoding="utf-8")
        assembly_name_match = _ASSEMBLY_NAME_PATTERN.search(project_text)
        assembly_name = (
            assembly_name_match.group(1).strip()
            if assembly_name_match is not None
            else project_path.stem
        )
        if assembly_name and assembly_name not in assembly_names:
            assembly_names.append(assembly_name)

    return assembly_names


def _normalize_orchestration(payload: Any) -> dict[str, Any]:
    if payload is None:
        return {}
    if not isinstance(payload, dict):
        raise ValueError("orchestration must be an object")

    normalized: dict[str, Any] = {}
    for key, value in payload.items():
        if key in _ORCHESTRATION_REF_FIELDS:
            if value is None:
                continue
            normalized[key] = _normalize_optional_string(value, field_name=f"orchestration.{key}")
            continue
        normalized[key] = value
    return normalized


def _load_shared_profile(
    repo_root: Path,
    *,
    profile_path: Path,
    field_name: str,
    collection_name: str,
) -> list[dict[str, Any]]:
    if not profile_path.is_file():
        raise FileNotFoundError(f"{field_name} not found: {profile_path}")

    payload = read_json(profile_path)
    if not isinstance(payload, dict):
        raise RuntimeError(f"{field_name} must be an object: {profile_path}")

    collection = payload.get(collection_name, [])
    if collection is None:
        return []
    if not isinstance(collection, list):
        raise RuntimeError(f"{field_name}.{collection_name} must be a list: {profile_path}")
    return [dict(item) for item in collection if isinstance(item, dict)]


def _merge_identified_objects(
    shared_items: list[dict[str, Any]],
    inline_items: list[dict[str, Any]],
    *,
    id_field: str,
    field_name: str,
) -> list[dict[str, Any]]:
    merged: dict[str, dict[str, Any]] = {}
    ordered_ids: list[str] = []

    for collection_name, items in (("shared", shared_items), ("inline", inline_items)):
        for item in items:
            identifier = str(item.get(id_field) or "").strip()
            if not identifier:
                raise ValueError(f"{field_name} item from {collection_name} is missing {id_field}")
            merged[identifier] = dict(item)
            if identifier not in ordered_ids:
                ordered_ids.append(identifier)

    return [merged[identifier] for identifier in ordered_ids]


def _apply_orchestration_profiles(
    manifest: dict[str, Any],
    *,
    manifest_path: Path,
) -> None:
    orchestration = _normalize_orchestration(manifest.get("orchestration"))
    manifest["orchestration"] = orchestration

    repo_root = path_resolver_module.repo_root_from_subject_manifest(manifest_path)
    shared_pipelines: list[dict[str, Any]] = []
    shared_matrices: list[dict[str, Any]] = []

    pipeline_profile_id = str(orchestration.get("pipelineProfile") or "")
    if pipeline_profile_id:
        shared_pipelines = _load_shared_profile(
            repo_root,
            profile_path=path_resolver_module.pipeline_profile_path(repo_root, pipeline_profile_id),
            field_name="orchestration.pipelineProfile",
            collection_name="executionPipelines",
        )

    matrix_profile_id = str(orchestration.get("matrixProfile") or "")
    if matrix_profile_id:
        shared_matrices = _load_shared_profile(
            repo_root,
            profile_path=path_resolver_module.matrix_profile_path(repo_root, matrix_profile_id),
            field_name="orchestration.matrixProfile",
            collection_name="environmentMatrices",
        )

    manifest["executionPipelines"] = _merge_identified_objects(
        shared_pipelines,
        [dict(item) for item in list(manifest.get("executionPipelines") or []) if isinstance(item, dict)],
        id_field="pipelineId",
        field_name="executionPipelines",
    )
    manifest["environmentMatrices"] = _merge_identified_objects(
        shared_matrices,
        [dict(item) for item in list(manifest.get("environmentMatrices") or []) if isinstance(item, dict)],
        id_field="matrixId",
        field_name="environmentMatrices",
    )


def _default_matrix(manifest: dict[str, Any]) -> dict[str, Any]:
    matrices = list(manifest.get("environmentMatrices") or [])
    if not matrices:
        return {}
    default_matrix_id = str(manifest.get("defaultMatrix") or "")
    if default_matrix_id:
        return find_matrix(manifest, default_matrix_id)
    return dict(matrices[0])


def _default_stage_kinds(manifest: dict[str, Any]) -> tuple[list[str], dict[str, Any]]:
    matrix = _default_matrix(manifest)
    pipeline_id = str(matrix.get("pipelineId") or "")
    pipeline = find_pipeline(manifest, pipeline_id) if pipeline_id else {}
    return pipeline_stage_kinds(pipeline), matrix


def _derive_executable_plan(manifest: dict[str, Any]) -> str:
    stage_kinds, matrix = _default_stage_kinds(manifest)
    execution_context = dict(matrix.get("executionContext") or {})
    host_platform = str(execution_context.get("hostPlatform") or "")
    target_platform = str(execution_context.get("targetPlatform") or "")

    if any("interpreter" in stage_kind or "hot-update" in stage_kind or "patch" in stage_kind for stage_kind in stage_kinds):
        return ExecutablePlan.HOST_PLUS_HOT_UPDATE_PATCH.value
    if host_platform and target_platform and host_platform != target_platform:
        return ExecutablePlan.DEVICE_PACKAGE.value
    if any(
        stage_kind.startswith("generated-")
        or stage_kind in {"analysis-frontend", "build-target", "runtime-observe", "runtime-trace-compare", "runtime-engine-observe", "runtime-engine-trace-compare", "native-runtime-perf", "mobile-native-perf"}
        for stage_kind in stage_kinds
    ):
        return ExecutablePlan.GENERATED_NATIVE.value
    return ExecutablePlan.MANAGED_HOST.value


def _derive_engineering_profile(manifest: dict[str, Any]) -> str:
    executable_plan = str(manifest.get("executablePlan") or "")
    if executable_plan == ExecutablePlan.HOST_PLUS_HOT_UPDATE_PATCH.value:
        return EngineeringProfile.HOT_UPDATE_HOST.value
    if executable_plan == ExecutablePlan.DEVICE_PACKAGE.value:
        return EngineeringProfile.DEVICE_PACKAGE.value
    if executable_plan == ExecutablePlan.GENERATED_NATIVE.value:
        return EngineeringProfile.NATIVE_EXECUTABLE.value
    return EngineeringProfile.MANAGED_OUTPUT.value


def _normalize_availability(payload: Any) -> dict[str, str]:
    if payload is None:
        return {}
    if not isinstance(payload, dict):
        raise ValueError("availability must be an object")
    normalized: dict[str, str] = {}
    for platform, status in payload.items():
        platform_name = str(platform).strip()
        if not platform_name:
            raise ValueError("availability platform key must be non-empty")
        normalized[platform_name] = _normalize_enum_value(
            status,
            AvailabilityStatus,
            field_name="availability",
        )
    return dict(sorted(normalized.items()))


def _derive_availability(manifest: dict[str, Any]) -> dict[str, str]:
    availability: dict[str, str] = {}
    for matrix in list(manifest.get("environmentMatrices") or []):
        execution_context = dict(matrix.get("executionContext") or {})
        target_platform = str(execution_context.get("targetPlatform") or "").strip()
        if target_platform:
            availability[target_platform] = AvailabilityStatus.READY.value
    return dict(sorted(availability.items()))


def _normalize_compatibility(payload: Any) -> dict[str, Any]:
    if payload is None:
        return {}
    if not isinstance(payload, dict):
        raise ValueError("compatibility must be an object")
    return dict(payload)


def normalize_subject_manifest(
    manifest: dict[str, Any],
    *,
    manifest_path: Path,
) -> dict[str, Any]:
    normalized = dict(manifest)
    _apply_orchestration_profiles(normalized, manifest_path=manifest_path)

    source_model = normalized.get("sourceModel")
    if source_model is None:
        normalized["sourceModel"] = _derive_source_model(manifest_path)
    else:
        normalized["sourceModel"] = _normalize_enum_value(source_model, SourceModel, field_name="sourceModel")

    dependency_model = normalized.get("dependencyModel")
    if dependency_model is None:
        normalized["dependencyModel"] = _derive_dependency_model(manifest_path)
    else:
        normalized["dependencyModel"] = _normalize_enum_value(
            dependency_model,
            DependencyModel,
            field_name="dependencyModel",
        )

    executable_plan = normalized.get("executablePlan")
    if executable_plan is None:
        normalized["executablePlan"] = _derive_executable_plan(normalized)
    else:
        normalized["executablePlan"] = _normalize_enum_value(
            executable_plan,
            ExecutablePlan,
            field_name="executablePlan",
        )

    engineering_profile = normalized.get("engineeringProfile")
    if engineering_profile is None:
        normalized["engineeringProfile"] = _derive_engineering_profile(normalized)
    else:
        normalized["engineeringProfile"] = _normalize_enum_value(
            engineering_profile,
            EngineeringProfile,
            field_name="engineeringProfile",
        )

    availability = normalized.get("availability")
    if availability is None:
        normalized["availability"] = _derive_availability(normalized)
    else:
        normalized["availability"] = _normalize_availability(availability)

    normalized["compatibility"] = _normalize_compatibility(normalized.get("compatibility"))
    return normalized


def discover_subject_manifests(repo_root: Path) -> list[Path]:
    subject_root = repo_root / "subjects"
    if not subject_root.is_dir():
        return []

    manifest_paths: list[Path] = []
    for candidate in subject_root.iterdir():
        if not candidate.is_dir():
            continue

        manifest_path = candidate / SUBJECT_MANIFEST_NAME
        if manifest_path.is_file():
            manifest_paths.append(manifest_path)

    return sorted(manifest_paths)


def load_subject_manifest(repo_root: Path, subject_id: str) -> dict[str, Any]:
    manifest_path = path_resolver_module.subject_manifest_path(repo_root, subject_id)
    if not manifest_path.is_file():
        raise FileNotFoundError(f"subject manifest missing: {manifest_path}")
    return load_subject_manifest_file(manifest_path)


def load_subject_manifest_file(manifest_path: Path) -> dict[str, Any]:
    manifest = read_json(manifest_path)
    if not isinstance(manifest, dict):
        raise RuntimeError(f"subject manifest must be an object: {manifest_path}")
    return normalize_subject_manifest(manifest, manifest_path=manifest_path)


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
        "sourceModel": str(manifest.get("sourceModel") or ""),
        "dependencyModel": str(manifest.get("dependencyModel") or ""),
        "executablePlan": str(manifest.get("executablePlan") or ""),
        "engineeringProfile": str(manifest.get("engineeringProfile") or ""),
        "orchestration": dict(manifest.get("orchestration") or {}),
        "availability": dict(manifest.get("availability") or {}),
        "compatibility": dict(manifest.get("compatibility") or {}),
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
        "testDeclarationMode": declarations_module.test_declaration_mode(manifest).value,
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


def perf_goal_matrices(manifest: dict[str, Any]) -> list[dict[str, Any]]:
    return [
        dict(matrix)
        for matrix in list(manifest.get("environmentMatrices") or [])
        if any(str(goal_id).startswith("perf.") for goal_id in list(dict(matrix).get("supportedGoals") or []))
    ]


def supports_perf_goal(manifest: dict[str, Any]) -> bool:
    return bool(perf_goal_matrices(manifest))


def discover_perf_subject_records(repo_root: Path) -> list[dict[str, Any]]:
    return [
        record
        for record in load_subject_records(repo_root)
        if supports_perf_goal(dict(record.get("manifest") or {}))
    ]


def discover_perf_subject_ids(repo_root: Path) -> list[str]:
    return sorted(str(record["subjectId"]) for record in discover_perf_subject_records(repo_root))


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


def _goal_profile_id(goal_id: str | None) -> str:
    return str(goal_id or "").replace(".", "-")


def _preferred_validation_kind(validation: dict[str, Any], validation_mode: str | None) -> str | None:
    candidate = str(validation_mode or "")
    if candidate and candidate in validation:
        return candidate
    return None


def _select_validation_profile_id(
    manifest: dict[str, Any],
    *,
    validation_profile_id: str | None,
    goal_id: str | None,
    preferred_validation_kind: str | None,
) -> str:
    validation_profiles = dict(manifest.get("validationProfiles") or {})
    default_profile_id = str(manifest.get("defaultValidationProfile") or "")

    if validation_profile_id:
        return validation_profile_id

    goal_profile_id = _goal_profile_id(goal_id)
    if goal_profile_id and goal_profile_id in validation_profiles:
        goal_profile_kinds = [str(item) for item in list(validation_profiles.get(goal_profile_id) or []) if str(item)]
        if preferred_validation_kind is None or preferred_validation_kind in goal_profile_kinds:
            return goal_profile_id

    if preferred_validation_kind is not None:
        default_profile_kinds = [str(item) for item in list(validation_profiles.get(default_profile_id) or []) if str(item)]
        if preferred_validation_kind in default_profile_kinds:
            return default_profile_id

        matching_profiles = sorted(
            profile_id
            for profile_id, profile_kinds in validation_profiles.items()
            if preferred_validation_kind in [str(item) for item in list(profile_kinds or []) if str(item)]
        )
        if len(matching_profiles) == 1:
            return matching_profiles[0]

    return default_profile_id


def resolve_validation_selection(
    manifest: dict[str, Any],
    *,
    goal_id: str | None = None,
    validation_mode: str | None = None,
    validation_profile_id: str | None = None,
    validation_kind: str | None = None,
    variant: str | None = None,
) -> dict[str, Any]:
    validation_profiles = dict(manifest.get("validationProfiles") or {})
    validation = dict(manifest.get("validation") or {})

    preferred_validation_kind = _preferred_validation_kind(validation, validation_mode)
    selected_profile_id = _select_validation_profile_id(
        manifest,
        validation_profile_id=validation_profile_id,
        goal_id=goal_id,
        preferred_validation_kind=preferred_validation_kind,
    )
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
    if selected_validation_kind is None and preferred_validation_kind in selected_validation_kinds:
        selected_validation_kind = preferred_validation_kind
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
