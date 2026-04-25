from __future__ import annotations

from dataclasses import dataclass, field
from datetime import datetime
from pathlib import Path
from typing import Any
import sys

try:
    from ..core.common import read_json, write_json
    from . import capability_coverage as capability_coverage_module
    from . import compiled_catalog as compiled_catalog_module
    from . import declared_metadata_labels as declared_metadata_labels_module
    from . import subjects as subjects_module
    from . import verification_layout as verification_layout_module
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from core.common import read_json, write_json
    from testing import capability_coverage as capability_coverage_module
    from testing import compiled_catalog as compiled_catalog_module
    from testing import declared_metadata_labels as declared_metadata_labels_module
    from testing import subjects as subjects_module
    from testing import verification_layout as verification_layout_module


SUBJECT_EXECUTION_OBJECT_TYPES = {
    "subject",
    "engineering-validation",
    "engineering-workload",
    "declared-unit-test",
    "declared-benchmark",
}

ENGINEERING_MATRIX_STAGE_KINDS: dict[str, tuple[str, ...]] = {
    "project-graph": ("host-input-build", "analysis-frontend"),
    "managed-build": ("host-input-build",),
    "managed-runtime-output": ("runtime-managed-output",),
    "native-executable-smoke": (
        "runtime-observe",
        "runtime-trace-compare",
        "runtime-engine-observe",
        "runtime-engine-trace-compare",
        "native-runtime-perf",
        "mobile-native-perf",
        "build-target",
    ),
    "package-integrity": ("build-target",),
    "device-deploy-smoke": ("build-target",),
    "host-build": ("host-input-build",),
    "patch-load": (),
    "patch-roundtrip": (),
    "patch-rollback": (),
    "convert": ("analysis-frontend",),
    "codegen": ("generated-native-aot", "generated-native-proof"),
    "native-link": ("build-target",),
    "device-package": ("build-target",),
    "patch-generation": (),
}

ENGINEERING_MATRIX_STAGE_PRIORITY: dict[str, tuple[tuple[str, ...], ...]] = {
    "managed-build": (("runtime-managed-output",), ("host-input-build",)),
}

DECLARED_BENCHMARK_MATRIX_STAGE_KINDS: dict[str, tuple[str, ...]] = {
    "managed": ("runtime-perf-collect",),
    "native": ("native-runtime-perf", "mobile-native-perf"),
    "interpreter": ("interpreter-runtime-perf",),
}


@dataclass(frozen=True)
class RegistryIndex:
    host_platform: str
    suites: list[dict[str, Any]] = field(default_factory=list)
    subjects: list[dict[str, Any]] = field(default_factory=list)
    engineering_validations: list[dict[str, Any]] = field(default_factory=list)
    engineering_workloads: list[dict[str, Any]] = field(default_factory=list)
    declared_unit_tests: list[dict[str, Any]] = field(default_factory=list)
    declared_benchmarks: list[dict[str, Any]] = field(default_factory=list)
    module_verifications: list[dict[str, Any]] = field(default_factory=list)
    system_scenarios: list[dict[str, Any]] = field(default_factory=list)
    errors: list[str] = field(default_factory=list)
    warnings: list[str] = field(default_factory=list)

    @property
    def flat_items(self) -> list[dict[str, Any]]:
        return sorted(
            [
                *self.suites,
                *self.subjects,
                *self.engineering_validations,
                *self.engineering_workloads,
                *self.declared_unit_tests,
                *self.declared_benchmarks,
                *self.module_verifications,
                *self.system_scenarios,
            ],
            key=lambda item: item["id"],
        )

    @property
    def public_flat_items(self) -> list[dict[str, Any]]:
        return self.flat_items

    def to_dict(self) -> dict[str, Any]:
        return {
            "hostPlatform": self.host_platform,
            "suites": self.suites,
            "subjects": self.subjects,
            "engineeringValidations": self.engineering_validations,
            "engineeringWorkloads": self.engineering_workloads,
            "declaredUnitTests": self.declared_unit_tests,
            "declaredBenchmarks": self.declared_benchmarks,
            "moduleVerifications": self.module_verifications,
            "systemScenarios": self.system_scenarios,
            "flatItems": self.flat_items,
            "errors": list(self.errors),
            "warnings": list(self.warnings),
        }

    def to_public_dict(self) -> dict[str, Any]:
        return {
            "hostPlatform": self.host_platform,
            "suites": self.suites,
            "subjects": self.subjects,
            "engineeringValidations": self.engineering_validations,
            "engineeringWorkloads": self.engineering_workloads,
            "declaredUnitTests": self.declared_unit_tests,
            "declaredBenchmarks": self.declared_benchmarks,
            "moduleVerifications": self.module_verifications,
            "systemScenarios": self.system_scenarios,
            "flatItems": self.public_flat_items,
            "errors": list(self.errors),
            "warnings": list(self.warnings),
        }


CANONICAL_MODULE_MANIFEST_LAYOUT = "tests/fixtures/registry/modules/<module>/<profile>/verification.manifest.json"
CANONICAL_SYSTEM_MANIFEST_LAYOUT = "tests/fixtures/registry/systems/<scenario>/scenario.manifest.json"


def _require_string(payload: dict[str, Any], field_name: str) -> str:
    value = payload.get(field_name)
    if not isinstance(value, str) or not value.strip():
        raise ValueError(f"{field_name} is required")
    return value


def _string_list(payload: dict[str, Any], field_name: str) -> list[str]:
    value = payload.get(field_name, [])
    if value is None:
        return []
    if not isinstance(value, list) or any(not isinstance(item, str) or not item.strip() for item in value):
        raise ValueError(f"{field_name} must be a string list")
    return list(value)


def _member_list(payload: dict[str, Any], field_name: str) -> list[dict[str, str]]:
    members = payload.get(field_name, [])
    if not isinstance(members, list):
        raise ValueError(f"{field_name} must be a list")
    normalized: list[dict[str, str]] = []
    for index, member in enumerate(members):
        if not isinstance(member, dict):
            raise ValueError(f"{field_name}[{index}] must be an object")
        normalized.append(
            {
                "type": _require_string(member, "type"),
                "id": _require_string(member, "id"),
            }
        )
    return normalized


def _deprecated_flag(payload: dict[str, Any]) -> bool:
    value = payload.get("deprecated", False)
    if not isinstance(value, bool):
        raise ValueError("deprecated must be a boolean")
    return value


def _base_registry_object(
    *,
    object_id: str,
    object_type: str,
    display_name: str,
    level: str,
    manifest_path: Path | None,
    primary_module_id: str | None,
    module_ids: list[str],
    subsystem_ids: list[str],
    supported_hosts: list[str],
    doc_refs: list[str],
) -> dict[str, Any]:
    return {
        "id": object_id,
        "type": object_type,
        "displayName": display_name,
        "level": level,
        "manifestPath": str(manifest_path) if manifest_path is not None else None,
        "primaryModuleId": primary_module_id,
        "moduleIds": list(module_ids),
        "subsystemIds": list(subsystem_ids),
        "supportedHosts": list(supported_hosts),
        "docRefs": list(doc_refs),
    }


def _resolved_member_ids(item: dict[str, Any]) -> list[str]:
    if item["type"] == "subject":
        return [str(item["id"])]
    return [member["id"] for member in item.get("members", [])]


def _normalize_host_platform(host_platform: str) -> str:
    if host_platform.startswith("windows"):
        return "windows"
    if host_platform.startswith("macos"):
        return "macos"
    if host_platform.startswith("linux"):
        return "linux"
    return host_platform


def _matches_registry_collection_root(
    collection_root: Path,
    *,
    canonical_name: str,
) -> bool:
    registry_root = collection_root.parent
    return (
        collection_root.name == canonical_name
        and registry_root.name == "registry"
        and registry_root.parent.name == "fixtures"
        and registry_root.parent.parent.name == "tests"
    )


def _canonical_registry_collection_root(
    repo_root: Path,
    *,
    canonical_parts: tuple[str, ...],
) -> Path:
    return repo_root.joinpath(*canonical_parts)


def _load_module_manifest(path: Path) -> dict[str, Any]:
    payload = read_json(path)
    collection_root = path.parent.parent.parent
    if not _matches_registry_collection_root(collection_root, canonical_name="modules"):
        raise ValueError(f"module manifest path must be {CANONICAL_MODULE_MANIFEST_LAYOUT}")
    module_name = path.parent.parent.name
    profile = path.parent.name
    item = _base_registry_object(
        object_id=f"module/{module_name}/{profile}",
        object_type="module",
        display_name=_require_string(payload, "displayName"),
        level="module",
        manifest_path=path,
        primary_module_id=str(payload.get("primaryModuleId") or module_name),
        module_ids=_string_list(payload, "moduleIds") or [module_name],
        subsystem_ids=_string_list(payload, "subsystemIds"),
        supported_hosts=_string_list(payload, "supportedHosts"),
        doc_refs=_string_list(payload, "docRefs"),
    )
    item["profileId"] = profile
    item["members"] = _member_list(payload, "members")
    item["resolvedMembers"] = _resolved_member_ids(item)
    item["deprecated"] = _deprecated_flag(payload)
    return item


def _load_system_manifest(path: Path) -> dict[str, Any]:
    payload = read_json(path)
    collection_root = path.parent.parent
    if not _matches_registry_collection_root(collection_root, canonical_name="systems"):
        raise ValueError(f"system manifest path must be {CANONICAL_SYSTEM_MANIFEST_LAYOUT}")
    scenario = str(payload.get("scenarioId") or path.parent.name)
    if not scenario.strip():
        raise ValueError("scenarioId must be a non-empty string")
    primary_module_id = payload.get("primaryModuleId")
    if primary_module_id is not None and not isinstance(primary_module_id, str):
        raise ValueError("primaryModuleId must be a string")
    item = _base_registry_object(
        object_id=f"system/{scenario}",
        object_type="system",
        display_name=_require_string(payload, "displayName"),
        level="system",
        manifest_path=path,
        primary_module_id=primary_module_id,
        module_ids=_string_list(payload, "moduleIds"),
        subsystem_ids=_string_list(payload, "subsystemIds"),
        supported_hosts=_string_list(payload, "supportedHosts"),
        doc_refs=_string_list(payload, "docRefs"),
    )
    item["scenarioId"] = scenario
    item["members"] = _member_list(payload, "members")
    item["resolvedMembers"] = _resolved_member_ids(item)
    item["deprecated"] = _deprecated_flag(payload)
    return item


def _load_subject_manifest(path: Path) -> dict[str, Any]:
    payload = subjects_module.load_subject_manifest_file(path)
    if not (
        path.parent.parent.name == "subjects"
        or (
            path.parent.parent.name == "owners"
            and path.parent.parent.parent.name == "catalog"
            and path.parent.parent.parent.parent.name == verification_layout_module.VERIFICATION_ROOT_NAME
        )
    ):
        raise ValueError(
            "subject manifest path must be verification/catalog/owners/<subject>/owner.manifest.json"
        )

    subject_id = _require_string(payload, "subjectId")
    if path.parent.name != subject_id:
        raise ValueError("subjectId must match the subject directory name")

    matrices = payload.get("environmentMatrices", [])
    if not isinstance(matrices, list) or not matrices:
        raise ValueError("environmentMatrices must be a non-empty list")

    supported_hosts = sorted(
        {
            _normalize_host_platform(str(dict(matrix.get("executionContext") or {}).get("hostPlatform") or ""))
            for matrix in matrices
            if str(dict(matrix.get("executionContext") or {}).get("hostPlatform") or "").strip()
        }
    )
    if not supported_hosts:
        raise ValueError("subject must declare at least one executionContext.hostPlatform")

    default_matrix = subjects_module.find_matrix(payload, _require_string(payload, "defaultMatrix"))
    resolved_source = dict(payload.get("source") or {})
    resolved_source.update(dict(default_matrix.get("source") or {}))

    item = _base_registry_object(
        object_id=f"subject/{subject_id}",
        object_type="subject",
        display_name=_require_string(payload, "displayName"),
        level="subject",
        manifest_path=path,
        primary_module_id=None,
        module_ids=[],
        subsystem_ids=[],
        supported_hosts=supported_hosts,
        doc_refs=_string_list(payload, "docRefs"),
    )
    item["subjectId"] = subject_id
    item["category"] = str(payload.get("category") or "")
    item["subjectCategory"] = str(payload.get("category") or "")
    item["sourceModel"] = str(payload.get("sourceModel") or "")
    item["dependencyModel"] = str(payload.get("dependencyModel") or "")
    item["executablePlan"] = str(payload.get("executablePlan") or "")
    item["engineeringProfile"] = str(payload.get("engineeringProfile") or "")
    item["orchestration"] = dict(payload.get("orchestration") or {})
    item["availability"] = dict(payload.get("availability") or {})
    item["compatibility"] = dict(payload.get("compatibility") or {})
    item["defaultGoalId"] = _require_string(payload, "defaultGoal")
    item["defaultMatrixId"] = str(default_matrix.get("matrixId") or "")
    item["matrixIds"] = [str(matrix.get("matrixId") or "") for matrix in matrices]
    item["goalIds"] = sorted(
        {
            str(goal_id)
            for matrix in matrices
            for goal_id in list(dict(matrix).get("supportedGoals") or [])
            if str(goal_id).strip()
        }
    )
    item["tags"] = _string_list(payload, "tags")
    item["defaultPrimaryProjectPath"] = subjects_module.resolve_source_primary_project_path(resolved_source)
    item["displaySourceEntry"] = str(resolved_source.get("entry") or "")
    item["resolvedMembers"] = _resolved_member_ids(item)
    item["deprecated"] = _deprecated_flag(payload)
    return item


def _exclude_deprecated(items: list[dict[str, Any]]) -> list[dict[str, Any]]:
    return [item for item in items if not bool(item.get("deprecated", False))]


def _filter_host_supported(items: list[dict[str, Any]], host_platform: str) -> list[dict[str, Any]]:
    return [
        item
        for item in items
        if host_platform in list(item.get("supportedHosts", []))
    ]


def _scan_directory(root: Path, pattern: str, loader) -> tuple[list[dict[str, Any]], list[str]]:
    if not root.is_dir():
        return [], []
    items: list[dict[str, Any]] = []
    errors: list[str] = []
    for path in sorted(root.rglob(pattern)):
        try:
            items.append(loader(path))
        except ValueError as error:
            errors.append(f"{path}: {error}")
    return items, errors


def _suite_items(host_platform: str, public_suite_specs: list[dict[str, Any]]) -> list[dict[str, Any]]:
    items: list[dict[str, Any]] = []
    for spec in public_suite_specs:
        if host_platform not in spec["supported_hosts"]:
            continue
        item = _base_registry_object(
            object_id=spec["id"],
            object_type="suite",
            display_name=spec["suite"],
            level=str(spec.get("level", "code")),
            manifest_path=None,
            primary_module_id=spec.get("primaryModuleId"),
            module_ids=list(spec.get("moduleIds", [])),
            subsystem_ids=list(spec.get("subsystemIds", [])),
            supported_hosts=list(spec["supported_hosts"]),
            doc_refs=list(spec.get("docRefs", [])),
        )
        item["family"] = spec["family"]
        item["suite"] = spec["suite"]
        item["stages"] = list(spec["stages"])
        item["resolvedMembers"] = [spec["id"]]
        items.append(item)
    return items


def _read_doc_metadata(path: Path) -> dict[str, str]:
    metadata: dict[str, str] = {}
    if not path.is_file():
        return metadata
    for line in path.read_text(encoding="utf-8").splitlines():
        if ":" not in line:
            if metadata:
                break
            continue
        key, value = line.split(":", 1)
        key = key.strip()
        value = value.strip()
        if key in {"objectId", "objectType", "primaryModuleId"} and value:
            metadata[key] = value
    return metadata


def _subject_manifest_path(subject_item: dict[str, Any]) -> Path | None:
    manifest_path = subject_item.get("manifestPath")
    if not manifest_path:
        return None
    return Path(str(manifest_path))


def _subject_execution_base(
    subject_item: dict[str, Any],
    *,
    object_id: str,
    object_type: str,
    display_name: str,
) -> dict[str, Any]:
    item = _base_registry_object(
        object_id=object_id,
        object_type=object_type,
        display_name=display_name,
        level="subject",
        manifest_path=_subject_manifest_path(subject_item),
        primary_module_id=subject_item.get("primaryModuleId"),
        module_ids=list(subject_item.get("moduleIds") or []),
        subsystem_ids=list(subject_item.get("subsystemIds") or []),
        supported_hosts=list(subject_item.get("supportedHosts") or []),
        doc_refs=list(subject_item.get("docRefs") or []),
    )
    item["subjectId"] = str(subject_item.get("subjectId") or "")
    item["category"] = str(subject_item.get("category") or "")
    item["subjectCategory"] = str(subject_item.get("category") or "")
    item["sourceModel"] = str(subject_item.get("sourceModel") or "")
    item["dependencyModel"] = str(subject_item.get("dependencyModel") or "")
    item["executablePlan"] = str(subject_item.get("executablePlan") or "")
    item["engineeringProfile"] = str(subject_item.get("engineeringProfile") or "")
    item["orchestration"] = dict(subject_item.get("orchestration") or {})
    item["availability"] = dict(subject_item.get("availability") or {})
    item["compatibility"] = dict(subject_item.get("compatibility") or {})
    item["defaultGoalId"] = str(subject_item.get("defaultGoalId") or "")
    item["defaultMatrixId"] = str(subject_item.get("defaultMatrixId") or "")
    item["goalIds"] = list(subject_item.get("goalIds") or [])
    item["matrixIds"] = list(subject_item.get("matrixIds") or [])
    item["tags"] = list(subject_item.get("tags") or [])
    item["resolvedMembers"] = [object_id]
    item["deprecated"] = False
    return item


def _preferred_subject_matrices(manifest: dict[str, Any]) -> list[dict[str, Any]]:
    default_matrix_id = str(manifest.get("defaultMatrix") or "")
    matrices = [dict(matrix) for matrix in list(manifest.get("environmentMatrices") or [])]
    if not default_matrix_id:
        return matrices

    ordered: list[dict[str, Any]] = []
    for matrix in matrices:
        if str(matrix.get("matrixId") or "") == default_matrix_id:
            ordered.append(matrix)
    for matrix in matrices:
        if str(matrix.get("matrixId") or "") != default_matrix_id:
            ordered.append(matrix)
    return ordered


def _matrix_goal_id(manifest: dict[str, Any], matrix: dict[str, Any]) -> str:
    supported_goals = [str(goal_id) for goal_id in list(matrix.get("supportedGoals") or []) if str(goal_id)]
    if supported_goals:
        return supported_goals[0]
    return str(manifest.get("defaultGoal") or "")


def _matrix_stage_kinds(manifest: dict[str, Any], matrix: dict[str, Any]) -> set[str]:
    pipeline = subjects_module.find_pipeline(manifest, str(matrix.get("pipelineId") or ""))
    return set(subjects_module.pipeline_stage_kinds(pipeline))


def _matrix_uses_hotupdate_host(manifest: dict[str, Any], matrix: dict[str, Any]) -> bool:
    engineering_profile = str(manifest.get("engineeringProfile") or "").strip().lower()
    if engineering_profile == "hot-update-host":
        return True

    execution_context = dict(matrix.get("executionContext") or {})
    for field_name in ("runtimeProfile", "toolchainProfile"):
        normalized = str(execution_context.get(field_name) or "").strip().lower()
        if "hot-update" in normalized or "hotupdate" in normalized:
            return True
    return False


def _matrix_is_managed_proof(matrix: dict[str, Any]) -> bool:
    execution_context = dict(matrix.get("executionContext") or {})
    toolchain_profile = str(execution_context.get("toolchainProfile") or "").strip().lower()
    runtime_profile = str(execution_context.get("runtimeProfile") or "").strip().lower()
    return toolchain_profile == "dotnet-managed" and runtime_profile == "managed-proof-output"


def _select_engineering_matrix(manifest: dict[str, Any], kind: str) -> tuple[str, str]:
    candidate_stage_kinds = ENGINEERING_MATRIX_STAGE_KINDS.get(kind, ())
    matrices = _preferred_subject_matrices(manifest)
    matrix_stage_kinds = {
        str(matrix.get("matrixId") or ""): _matrix_stage_kinds(manifest, matrix)
        for matrix in matrices
    }
    for matrix in matrices:
        execution_context = dict(matrix.get("executionContext") or {})
        host_platform = str(execution_context.get("hostPlatform") or "")
        target_platform = str(execution_context.get("targetPlatform") or "")
        if kind in {"package-integrity", "device-deploy-smoke", "device-package"}:
            if host_platform and target_platform and host_platform != target_platform:
                return str(matrix.get("matrixId") or ""), _matrix_goal_id(manifest, matrix)
            continue
    for preferred_stage_kinds in ENGINEERING_MATRIX_STAGE_PRIORITY.get(kind, ()):
        preferred_stage_kind_set = set(preferred_stage_kinds)
        for matrix in matrices:
            if matrix_stage_kinds.get(str(matrix.get("matrixId") or ""), set()) & preferred_stage_kind_set:
                return str(matrix.get("matrixId") or ""), _matrix_goal_id(manifest, matrix)
    candidate_stage_kind_set = set(candidate_stage_kinds)
    for matrix in matrices:
        if candidate_stage_kind_set and matrix_stage_kinds.get(str(matrix.get("matrixId") or ""), set()) & candidate_stage_kind_set:
            return str(matrix.get("matrixId") or ""), _matrix_goal_id(manifest, matrix)

    default_matrix = subjects_module.find_matrix(manifest, str(manifest.get("defaultMatrix") or ""))
    return str(default_matrix.get("matrixId") or ""), _matrix_goal_id(manifest, default_matrix)


def _declared_source_entry(entry: dict[str, Any]) -> str:
    assembly_name = str(entry.get("assemblyName") or "")
    declaring_type = str(entry.get("declaringType") or "")
    method_signature = str(entry.get("methodSignature") or "")
    if not assembly_name or not declaring_type or not method_signature:
        return ""
    type_name = declaring_type.rsplit(".", 1)[-1]
    return f"{assembly_name}/{type_name}::{method_signature}"


def _select_declared_matrix(
    manifest: dict[str, Any],
    *,
    family: str,
    payload: dict[str, Any],
    source_entry: str,
) -> tuple[str, str]:
    if family == "declared-unit-test" and int(payload.get("hotUpdateCapability") or 0) > 0:
        for matrix in _preferred_subject_matrices(manifest):
            if _matrix_uses_hotupdate_host(manifest, matrix):
                return str(matrix.get("matrixId") or ""), _matrix_goal_id(manifest, matrix)

    if family == "declared-unit-test":
        for matrix in _preferred_subject_matrices(manifest):
            if _matrix_is_managed_proof(matrix):
                return str(matrix.get("matrixId") or ""), _matrix_goal_id(manifest, matrix)

    if family == "declared-benchmark":
        supported_modes = declared_metadata_labels_module.supported_modes_from_mask(payload.get("modes"))
        candidate_stage_kinds = {
            stage_kind
            for mode in supported_modes
            for stage_kind in DECLARED_BENCHMARK_MATRIX_STAGE_KINDS.get(mode, ())
        }
        perf_fallback: tuple[str, str] | None = None
        for matrix in _preferred_subject_matrices(manifest):
            goal_id = _matrix_goal_id(manifest, matrix)
            if not goal_id.startswith("perf."):
                continue
            if perf_fallback is None:
                perf_fallback = (str(matrix.get("matrixId") or ""), goal_id)
            if candidate_stage_kinds and _matrix_stage_kinds(manifest, matrix) & candidate_stage_kinds:
                return str(matrix.get("matrixId") or ""), goal_id
        if perf_fallback is not None:
            return perf_fallback

    if source_entry:
        for matrix in _preferred_subject_matrices(manifest):
            matrix_source = dict(matrix.get("source") or {})
            matrix_entry = str(matrix.get("workloadEntry") or matrix_source.get("entry") or "")
            if matrix_entry == source_entry:
                return str(matrix.get("matrixId") or ""), _matrix_goal_id(manifest, matrix)

    default_matrix = subjects_module.find_matrix(manifest, str(manifest.get("defaultMatrix") or ""))
    return str(default_matrix.get("matrixId") or ""), _matrix_goal_id(manifest, default_matrix)


def _engineering_registry_item(
    *,
    subject_item: dict[str, Any],
    manifest: dict[str, Any],
    family: str,
    payload: dict[str, Any],
) -> dict[str, Any]:
    kind = str(payload.get("kind") or "")
    default_matrix_id, default_goal_id = _select_engineering_matrix(manifest, kind)
    item = _subject_execution_base(
        subject_item,
        object_id=str(payload.get("stableId") or ""),
        object_type=family,
        display_name=kind or str(payload.get("stableId") or ""),
    )
    item["kind"] = kind
    item["defaultGoalId"] = default_goal_id or str(payload.get("defaultGoalId") or item.get("defaultGoalId") or "")
    item["defaultMatrixId"] = default_matrix_id or str(payload.get("defaultMatrixId") or item.get("defaultMatrixId") or "")
    item["goalIds"] = list(payload.get("goalIds") or item.get("goalIds") or [])
    item["matrixIds"] = list(payload.get("matrixIds") or item.get("matrixIds") or [])
    item["pipelineIds"] = list(payload.get("pipelineIds") or [])
    return item


def _declared_registry_item(
    *,
    subject_item: dict[str, Any],
    manifest: dict[str, Any],
    family: str,
    payload: dict[str, Any],
) -> dict[str, Any]:
    stable_id = str(payload.get("stableId") or "")
    source_entry = _declared_source_entry(payload)
    default_matrix_id, default_goal_id = _select_declared_matrix(
        manifest,
        family=family,
        payload=payload,
        source_entry=source_entry,
    )
    item = _subject_execution_base(
        subject_item,
        object_id=f"{family}/{stable_id}",
        object_type=family,
        display_name=str(payload.get("alias") or stable_id),
    )
    item["stableId"] = stable_id
    item["alias"] = str(payload.get("alias") or "")
    item["assemblyName"] = str(payload.get("assemblyName") or "")
    item["declaringType"] = str(payload.get("declaringType") or "")
    item["methodName"] = str(payload.get("methodName") or "")
    item["methodSignature"] = str(payload.get("methodSignature") or "")
    item["sourceEntry"] = source_entry
    item["workloadEntry"] = source_entry if family == "declared-benchmark" else ""
    entry_index = payload.get("entryIndex")
    if isinstance(entry_index, int) and not isinstance(entry_index, bool) and entry_index >= 0:
        item["entryIndex"] = int(entry_index)
    item["category"] = int(payload.get("category") or 0)
    item["categoryLabel"] = (
        declared_metadata_labels_module.unit_category_label(payload.get("category"))
        if family == "declared-unit-test"
        else declared_metadata_labels_module.benchmark_category_label(payload.get("category"))
    )
    capability_contract = capability_coverage_module.resolve_capability_contract(
        capability_family=payload.get("capabilityFamily"),
        capability_item=payload.get("capabilityItem"),
    )
    item["capabilityFamily"] = int(capability_contract["capabilityFamily"])
    item["capabilityFamilyLabel"] = str(capability_contract["capabilityFamilyLabel"])
    item["capabilityItem"] = int(capability_contract["capabilityItem"])
    item["capabilityItemLabel"] = str(capability_contract["capabilityItemLabel"])
    item["ownerSubjectId"] = str(capability_contract["ownerSubjectId"])
    item["supportStates"] = [int(value) for value in list(capability_contract["supportStates"])]
    item["supportStateLabels"] = [str(value) for value in list(capability_contract["supportStateLabels"])]
    item["proofRequired"] = bool(capability_contract["proofRequired"])
    item["benchmarkRequired"] = bool(capability_contract["benchmarkRequired"])
    item["archetype"] = int(payload.get("archetype") or 0)
    item["archetypeLabel"] = declared_metadata_labels_module.archetype_label(payload.get("archetype"))
    item["hotUpdateCapability"] = int(payload.get("hotUpdateCapability") or 0)
    item["hotUpdateCapabilityLabels"] = declared_metadata_labels_module.labels_from_mask(
        payload.get("hotUpdateCapability"),
        declared_metadata_labels_module.HOT_UPDATE_CAPABILITY_LABELS,
    )
    item["requires"] = int(payload.get("requires") or 0)
    item["requirementLabels"] = declared_metadata_labels_module.labels_from_mask(
        payload.get("requires"),
        declared_metadata_labels_module.RUNTIME_FEATURE_LABELS,
    )
    if family == "declared-unit-test":
        item["evidence"] = int(payload.get("evidence") or 0)
        item["evidenceLabels"] = declared_metadata_labels_module.labels_from_mask(
            payload.get("evidence"),
            declared_metadata_labels_module.EVIDENCE_LABELS,
        )
        item["priority"] = int(payload.get("priority") or 0)
    else:
        item["metrics"] = int(payload.get("metrics") or 0)
        item["metricLabels"] = declared_metadata_labels_module.labels_from_mask(
            payload.get("metrics"),
            declared_metadata_labels_module.METRIC_LABELS,
        )
        item["modes"] = int(payload.get("modes") or 0)
        item["supportedModes"] = declared_metadata_labels_module.supported_modes_from_mask(payload.get("modes"))
        item["warmupCount"] = int(payload.get("warmupCount") or 0)
        item["iterationCount"] = int(payload.get("iterationCount") or 0)
        item["invocationCount"] = int(payload.get("invocationCount") or 0)
    item["defaultGoalId"] = default_goal_id or item["defaultGoalId"]
    item["defaultMatrixId"] = default_matrix_id or item["defaultMatrixId"]
    return item


def _compiled_registry_items_for_subject(
    repo_root: Path,
    subject_item: dict[str, Any],
) -> tuple[dict[str, list[dict[str, Any]]], list[str]]:
    subject_id = str(subject_item.get("subjectId") or "")
    manifest = subjects_module.load_subject_manifest(repo_root, subject_id)
    errors: list[str] = []

    try:
        declared_catalog = compiled_catalog_module.build_subject_declared_test_catalog(
            repo_root=repo_root,
            subject_id=subject_id,
        )
    except (FileNotFoundError, RuntimeError, ValueError) as error:
        errors.append(f"subject/{subject_id}: {error}")
        declared_catalog = {
            "subjectId": subject_id,
            "declaredUnitTests": [],
            "declaredBenchmarks": [],
        }

    try:
        catalog = compiled_catalog_module.build_compiled_subject_catalog(
            repo_root=repo_root,
            subject_id=subject_id,
            declared_catalog=declared_catalog,
        )
    except (FileNotFoundError, RuntimeError, ValueError) as error:
        return {
            "engineeringValidations": [],
            "engineeringWorkloads": [],
            "declaredUnitTests": [],
            "declaredBenchmarks": [],
        }, [*errors, f"subject/{subject_id}: {error}"]

    engineering_validations = [
        _engineering_registry_item(
            subject_item=subject_item,
            manifest=manifest,
            family="engineering-validation",
            payload=dict(payload),
        )
        for payload in list(catalog.get("engineeringValidations") or [])
    ]
    engineering_workloads = [
        _engineering_registry_item(
            subject_item=subject_item,
            manifest=manifest,
            family="engineering-workload",
            payload=dict(payload),
        )
        for payload in list(catalog.get("engineeringWorkloads") or [])
    ]
    declared_unit_tests = [
        _declared_registry_item(
            subject_item=subject_item,
            manifest=manifest,
            family="declared-unit-test",
            payload=dict(payload),
        )
        for payload in list(catalog.get("declaredUnitTests") or [])
    ]
    declared_benchmarks = [
        _declared_registry_item(
            subject_item=subject_item,
            manifest=manifest,
            family="declared-benchmark",
            payload=dict(payload),
        )
        for payload in list(catalog.get("declaredBenchmarks") or [])
    ]
    return {
        "engineeringValidations": engineering_validations,
        "engineeringWorkloads": engineering_workloads,
        "declaredUnitTests": declared_unit_tests,
        "declaredBenchmarks": declared_benchmarks,
    }, errors


def _canonical_command(item: dict[str, Any]) -> str:
    object_type = str(item["type"])
    if object_type == "suite":
        return f"run test suite --id {item['id']}"
    if object_type == "subject":
        return f"run test subject --id {item['id']}"
    if object_type in {
        "engineering-validation",
        "engineering-workload",
        "declared-unit-test",
        "declared-benchmark",
    }:
        return f"run test {object_type} --id {item['id']}"
    if object_type == "module":
        return f"run test module --id {item['id']}"
    if object_type == "system":
        return f"run test system --id {item['id']}"
    return f"run test --id {item['id']}"


def _scope_tokens(item: dict[str, Any]) -> set[str]:
    tokens: set[str] = set()
    primary_module_id = item.get("primaryModuleId")
    if isinstance(primary_module_id, str) and primary_module_id:
        tokens.add(primary_module_id)
    for field_name in ("moduleIds", "subsystemIds"):
        for value in item.get(field_name, []):
            if isinstance(value, str) and value:
                tokens.add(value)
    return tokens


def _module_scope_tokens(item: dict[str, Any]) -> set[str]:
    tokens: set[str] = set()
    primary_module_id = item.get("primaryModuleId")
    if isinstance(primary_module_id, str) and primary_module_id:
        tokens.add(primary_module_id)
    for value in item.get("moduleIds", []):
        if isinstance(value, str) and value:
            tokens.add(value)
    return tokens


def _subsystem_scope_tokens(item: dict[str, Any]) -> set[str]:
    tokens: set[str] = set()
    for value in item.get("subsystemIds", []):
        if isinstance(value, str) and value:
            tokens.add(value)
    return tokens


def _recommendation_entry(item: dict[str, Any]) -> dict[str, str]:
    entry = {
        "objectId": str(item["id"]),
        "objectType": str(item["type"]),
        "displayName": str(item.get("displayName") or item["id"]),
    }
    command = str(item.get("canonicalCommand") or _canonical_command(item))
    if command:
        entry["command"] = command
    return entry


def _related_items(index: RegistryIndex, item: dict[str, Any]) -> list[dict[str, Any]]:
    item_module_tokens = _module_scope_tokens(item)
    item_subsystem_tokens = _subsystem_scope_tokens(item)
    if not item_module_tokens and not item_subsystem_tokens:
        return []
    module_related: list[dict[str, Any]] = []
    subsystem_related: list[dict[str, Any]] = []
    for candidate in index.flat_items:
        if candidate["id"] == item["id"]:
            continue
        candidate_module_tokens = _module_scope_tokens(candidate)
        if item_module_tokens and candidate_module_tokens & item_module_tokens:
            module_related.append(candidate)
            continue
        candidate_subsystem_tokens = _subsystem_scope_tokens(candidate)
        if (
            not item_module_tokens
            and item_subsystem_tokens
            and candidate_subsystem_tokens & item_subsystem_tokens
        ):
            subsystem_related.append(candidate)
    return sorted([*module_related, *subsystem_related], key=lambda candidate: (candidate["type"], candidate["id"]))


def _dedupe_entries(items: list[dict[str, Any]]) -> list[dict[str, str]]:
    seen: set[str] = set()
    entries: list[dict[str, str]] = []
    for item in items:
        object_id = str(item["id"])
        if object_id in seen:
            continue
        seen.add(object_id)
        entries.append(_recommendation_entry(item))
    return entries


def _decorate_registry_items(index: RegistryIndex) -> None:
    items_by_id = {item["id"]: item for item in index.flat_items}
    for item in items_by_id.values():
        item["canonicalCommand"] = _canonical_command(item)

    for item in items_by_id.values():
        related = _related_items(index, item)
        primary_module_id = item.get("primaryModuleId")
        related_modules = [
            candidate
            for candidate in related
            if candidate["type"] == "module" and candidate.get("primaryModuleId") == primary_module_id
        ]

        if item["type"] == "suite":
            required_before_completion = related_modules or [item]
        elif item["type"] == "module":
            required_before_completion = [item]
        elif item["type"] == "system":
            required_before_completion = related_modules
        else:
            required_before_completion = []

        excluded_ids = {candidate["id"] for candidate in required_before_completion}
        recommended = [candidate for candidate in related if candidate["id"] not in excluded_ids]

        item["skillRecommendations"] = {
            "recommended": _dedupe_entries(recommended),
            "requiredBeforeCompletion": _dedupe_entries(required_before_completion),
        }


def check_registry_consistency(repo_root: Path, index: RegistryIndex) -> tuple[list[str], list[str]]:
    errors = list(index.errors)
    warnings = list(index.warnings)
    items_by_id = {item["id"]: item for item in index.flat_items}
    for item in index.flat_items:
        for member_id in item.get("resolvedMembers", []):
            if member_id not in items_by_id:
                errors.append(f"{item['id']}: referenced object not found: {member_id}")
        for doc_ref in item.get("docRefs", []):
            doc_path = repo_root / doc_ref
            if not doc_path.is_file():
                warnings.append(f"{item['id']}: docRef missing: {doc_ref}")
                continue
            metadata = _read_doc_metadata(doc_path)
            if metadata.get("objectType") == "guide":
                continue
            if metadata.get("objectId") and metadata["objectId"] != item["id"]:
                errors.append(f"{item['id']}: doc objectId mismatch: {doc_ref}")
            if metadata.get("objectType") and metadata["objectType"] != item["type"]:
                errors.append(f"{item['id']}: doc objectType mismatch: {doc_ref}")
            primary = item.get("primaryModuleId")
            if primary and metadata.get("primaryModuleId") and metadata["primaryModuleId"] != primary:
                errors.append(f"{item['id']}: doc primaryModuleId mismatch: {doc_ref}")
    return errors, warnings


def scan_registry(
    repo_root: Path,
    *,
    host_platform: str,
    public_suite_specs: list[dict[str, Any]],
) -> RegistryIndex:
    modules_root = _canonical_registry_collection_root(
        repo_root,
        canonical_parts=("tests", "fixtures", "registry", "modules"),
    )
    systems_root = _canonical_registry_collection_root(
        repo_root,
        canonical_parts=("tests", "fixtures", "registry", "systems"),
    )
    suites = _suite_items(host_platform, public_suite_specs)
    subjects_root = verification_layout_module.owners_root(repo_root)
    subject_pattern = subjects_module.SUBJECT_MANIFEST_NAME
    subjects, subject_errors = _scan_directory(subjects_root, subject_pattern, _load_subject_manifest)
    modules, module_errors = _scan_directory(modules_root, "verification.manifest.json", _load_module_manifest)
    systems, system_errors = _scan_directory(systems_root, "scenario.manifest.json", _load_system_manifest)
    subjects = _exclude_deprecated(subjects)
    modules = _exclude_deprecated(modules)
    systems = _exclude_deprecated(systems)
    subjects = _filter_host_supported(subjects, host_platform)
    modules = _filter_host_supported(modules, host_platform)
    systems = _filter_host_supported(systems, host_platform)
    engineering_validations: list[dict[str, Any]] = []
    engineering_workloads: list[dict[str, Any]] = []
    declared_unit_tests: list[dict[str, Any]] = []
    declared_benchmarks: list[dict[str, Any]] = []
    compiled_errors: list[str] = []
    for subject_item in subjects:
        compiled_items, item_errors = _compiled_registry_items_for_subject(repo_root, subject_item)
        engineering_validations.extend(compiled_items["engineeringValidations"])
        engineering_workloads.extend(compiled_items["engineeringWorkloads"])
        declared_unit_tests.extend(compiled_items["declaredUnitTests"])
        declared_benchmarks.extend(compiled_items["declaredBenchmarks"])
        compiled_errors.extend(item_errors)
    provisional = RegistryIndex(
        host_platform=host_platform,
        suites=suites,
        subjects=subjects,
        engineering_validations=engineering_validations,
        engineering_workloads=engineering_workloads,
        declared_unit_tests=declared_unit_tests,
        declared_benchmarks=declared_benchmarks,
        module_verifications=modules,
        system_scenarios=systems,
        errors=[*subject_errors, *compiled_errors, *module_errors, *system_errors],
        warnings=[],
    )
    _decorate_registry_items(provisional)
    errors, warnings = check_registry_consistency(repo_root, provisional)
    return RegistryIndex(
        host_platform=provisional.host_platform,
        suites=provisional.suites,
        subjects=provisional.subjects,
        engineering_validations=provisional.engineering_validations,
        engineering_workloads=provisional.engineering_workloads,
        declared_unit_tests=provisional.declared_unit_tests,
        declared_benchmarks=provisional.declared_benchmarks,
        module_verifications=provisional.module_verifications,
        system_scenarios=provisional.system_scenarios,
        errors=errors,
        warnings=warnings,
    )


def write_registry_snapshot(
    repo_root: Path,
    index: RegistryIndex,
    *,
    stamp: str | None = None,
) -> dict[str, Path]:
    snapshot_stamp = stamp or datetime.now().strftime("%Y%m%d-%H%M%S")
    current_path = repo_root / "artifacts" / "tests" / "registry" / "current" / "index.json"
    history_path = repo_root / "artifacts" / "tests" / "registry" / "history" / snapshot_stamp / "index.json"
    payload = index.to_dict()
    payload["snapshotStamp"] = snapshot_stamp
    write_json(current_path, payload)
    write_json(history_path, payload)
    return {"currentPath": current_path, "historyPath": history_path}
def find_registry_object(index: RegistryIndex, object_id: str) -> dict[str, Any] | None:
    for item in index.flat_items:
        if item["id"] == object_id:
            return item
    return None


def expand_execution_plan(index: RegistryIndex, object_id: str) -> list[dict[str, Any]]:
    plan: list[dict[str, Any]] = []
    active_stack: set[str] = set()
    added_leaf_ids: set[str] = set()

    def visit(current_id: str) -> None:
        if current_id in active_stack:
            raise ValueError(f"cyclic registry reference: {current_id}")
        item = find_registry_object(index, current_id)
        if item is None:
            raise ValueError(f"registry object not found: {current_id}")
        if item["type"] in SUBJECT_EXECUTION_OBJECT_TYPES or item["type"] == "suite":
            if current_id not in added_leaf_ids:
                added_leaf_ids.add(current_id)
                plan.append(item)
            return
        active_stack.add(current_id)
        for member in item.get("members", []):
            visit(member["id"])
        active_stack.remove(current_id)

    visit(object_id)
    return plan
