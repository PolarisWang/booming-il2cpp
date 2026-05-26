from __future__ import annotations

from enum import Enum
import json
import os
from pathlib import Path
import re
import tempfile
from typing import Any, Iterable
import sys

try:
    from ..core.common import combine_process_output, run_process
    from ..core import tooling as tooling_module
    from . import declarations as declarations_module
    from . import subjects as subjects_module
    from . import verification_layout as verification_layout_module
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from core.common import combine_process_output, run_process
    from core import tooling as tooling_module
    from testing import declarations as declarations_module
    from testing import subjects as subjects_module
    from testing import verification_layout as verification_layout_module


DISCOVERY_PROJECT_PATH = Path("src/tools/Chaos.IL2CPP.Tools.TestFramework.CollectionGen/Chaos.IL2CPP.Tools.TestFramework.CollectionGen.csproj")
DISCOVERY_DLL_PATH = Path("src/tools/Chaos.IL2CPP.Tools.TestFramework.CollectionGen/bin/Release/net8.0/Chaos.IL2CPP.Tools.TestFramework.CollectionGen.dll")
_DISCOVERY_TOOL_CACHE: Path | None = None


class EngineeringValidationKind(str, Enum):
    PROJECT_GRAPH = "project-graph"
    MANAGED_BUILD = "managed-build"
    MANAGED_RUNTIME_OUTPUT = "managed-runtime-output"
    NATIVE_EXECUTABLE_SMOKE = "native-executable-smoke"
    PACKAGE_INTEGRITY = "package-integrity"
    DEVICE_DEPLOY_SMOKE = "device-deploy-smoke"
    HOST_BUILD = "host-build"
    PATCH_LOAD = "patch-load"
    PATCH_ROUNDTRIP = "patch-roundtrip"
    PATCH_ROLLBACK = "patch-rollback"


class EngineeringWorkloadKind(str, Enum):
    CONVERT = "convert"
    CODEGEN = "codegen"
    NATIVE_LINK = "native-link"
    DEVICE_PACKAGE = "device-package"
    PATCH_GENERATION = "patch-generation"


_ENGINEERING_VALIDATIONS_BY_PROFILE: dict[str, list[EngineeringValidationKind]] = {
    subjects_module.EngineeringProfile.MANAGED_OUTPUT.value: [
        EngineeringValidationKind.PROJECT_GRAPH,
        EngineeringValidationKind.MANAGED_BUILD,
        EngineeringValidationKind.MANAGED_RUNTIME_OUTPUT,
    ],
    subjects_module.EngineeringProfile.NATIVE_EXECUTABLE.value: [
        EngineeringValidationKind.PROJECT_GRAPH,
        EngineeringValidationKind.NATIVE_EXECUTABLE_SMOKE,
    ],
    subjects_module.EngineeringProfile.DEVICE_PACKAGE.value: [
        EngineeringValidationKind.PROJECT_GRAPH,
        EngineeringValidationKind.NATIVE_EXECUTABLE_SMOKE,
        EngineeringValidationKind.PACKAGE_INTEGRITY,
        EngineeringValidationKind.DEVICE_DEPLOY_SMOKE,
    ],
    subjects_module.EngineeringProfile.HOT_UPDATE_HOST.value: [
        EngineeringValidationKind.HOST_BUILD,
        EngineeringValidationKind.PATCH_LOAD,
        EngineeringValidationKind.PATCH_ROUNDTRIP,
        EngineeringValidationKind.PATCH_ROLLBACK,
    ],
}


_ENGINEERING_WORKLOADS_BY_PROFILE: dict[str, list[EngineeringWorkloadKind]] = {
    subjects_module.EngineeringProfile.MANAGED_OUTPUT.value: [],
    subjects_module.EngineeringProfile.NATIVE_EXECUTABLE.value: [
        EngineeringWorkloadKind.CONVERT,
        EngineeringWorkloadKind.CODEGEN,
        EngineeringWorkloadKind.NATIVE_LINK,
    ],
    subjects_module.EngineeringProfile.DEVICE_PACKAGE.value: [
        EngineeringWorkloadKind.CONVERT,
        EngineeringWorkloadKind.CODEGEN,
        EngineeringWorkloadKind.NATIVE_LINK,
        EngineeringWorkloadKind.DEVICE_PACKAGE,
    ],
    subjects_module.EngineeringProfile.HOT_UPDATE_HOST.value: [
        EngineeringWorkloadKind.PATCH_GENERATION,
    ],
}

_NATIVE_GENERATION_STAGE_KINDS = {
    "generated-native-aot",
    "generated-native-proof",
}

_NATIVE_EXECUTION_STAGE_KINDS = {
    "runtime-observe",
    "runtime-trace-compare",
    "runtime-engine-observe",
    "runtime-engine-trace-compare",
    "native-runtime-perf",
    "mobile-native-perf",
}

_RELEVANT_SOURCE_SUFFIXES = {".cs", ".csproj", ".sln", ".props", ".targets"}
_COLLECTION_SCHEMA_VERSION = 1
_ASSEMBLY_NAME_PATTERN = re.compile(r"<AssemblyName>\s*([^<]+)\s*</AssemblyName>", re.IGNORECASE)


def _host_platform_name() -> str:
    if os.name == "nt":
        return "windows"
    if sys.platform == "darwin":
        return "macos"
    return "linux"


def _dotnet_intermediate_args(project_name: str) -> list[str]:
    intermediate_root = tooling_module.allocate_dotnet_intermediate_dir(project_name, host_platform=_host_platform_name())
    if intermediate_root is None:
        return []
    return [f"-p:ChaosTempIntermediateRoot={Path(intermediate_root).as_posix()}/"]


def _normalize_assembly_paths(repo_root: Path, assembly_paths: Iterable[str | Path]) -> list[Path]:
    normalized: list[Path] = []
    seen: set[str] = set()
    for value in assembly_paths:
        path = Path(value)
        if not path.is_absolute():
            path = repo_root / path
        resolved = path.resolve()
        key = str(resolved).lower() if os.name == "nt" else str(resolved)
        if key in seen:
            continue
        seen.add(key)
        if not resolved.is_file():
            raise FileNotFoundError(f"assembly missing: {resolved}")
        normalized.append(resolved)
    if not normalized:
        raise ValueError("assembly_paths must not be empty")
    return normalized


def _is_relevant_source_file(path: Path) -> bool:
    if not path.is_file():
        return False
    if path.suffix.lower() in _RELEVANT_SOURCE_SUFFIXES:
        return True
    return path.name in {"Directory.Build.props", "Directory.Build.targets"}


def _assemblies_are_stale(
    repo_root: Path,
    manifest: dict[str, Any],
    assembly_paths: list[Path],
) -> bool:
    try:
        assembly_mtime = min(path.stat().st_mtime for path in assembly_paths)
    except (OSError, ValueError):
        return True

    subject_id = str(manifest.get("subjectId") or "").strip()
    if subject_id:
        subject_manifest_path = verification_layout_module.owner_manifest_path(repo_root, subject_id)
        try:
            if subject_manifest_path.is_file() and subject_manifest_path.stat().st_mtime > assembly_mtime:
                return True
        except OSError:
            return True

    source = dict(manifest.get("source") or {})
    source_path_text = str(source.get("path") or "").strip()
    if not source_path_text:
        return False

    source_path = Path(source_path_text)
    if not source_path.is_absolute():
        source_path = repo_root / source_path
    source_root = verification_layout_module.owner_scan_root(repo_root, subject_id, source_path_text)
    if not source_root.is_dir():
        return False

    for candidate in source_root.rglob("*"):
        if not _is_relevant_source_file(candidate):
            continue
        try:
            if candidate.stat().st_mtime > assembly_mtime:
                return True
        except OSError:
            return True
    return False


def ensure_declaration_discovery_tool(repo_root: Path) -> Path:
    global _DISCOVERY_TOOL_CACHE
    project_path = repo_root / DISCOVERY_PROJECT_PATH
    dll_path = repo_root / DISCOVERY_DLL_PATH
    if _DISCOVERY_TOOL_CACHE is not None and _DISCOVERY_TOOL_CACHE.is_file():
        return _DISCOVERY_TOOL_CACHE
    if not project_path.is_file():
        raise FileNotFoundError(f"declaration discovery project missing: {project_path}")

    # If the DLL already exists from a previous build, return it directly.
    # Rebuilding with ChaosTempIntermediateRoot forces a full dependency graph
    # rebuild which can OOM for large projects like CollectionGen.
    if dll_path.is_file():
        _DISCOVERY_TOOL_CACHE = dll_path
        return dll_path

    arguments = [
        "dotnet",
        "build",
        str(project_path),
        "-c",
        "Release",
        "-m:1",
        *_dotnet_intermediate_args(project_path.stem),
    ]
    completed = run_process(arguments, cwd=repo_root)
    if completed.returncode != 0:
        output = combine_process_output(completed)
        raise RuntimeError(f"dotnet build failed: {DISCOVERY_PROJECT_PATH.as_posix()}\n{output}".strip())

    if not dll_path.is_file():
        raise FileNotFoundError(f"declaration discovery dll missing after build: {dll_path}")
    _DISCOVERY_TOOL_CACHE = dll_path
    return dll_path


def discover_declared_test_metadata(
    *,
    repo_root: Path,
    assembly_paths: Iterable[str | Path],
) -> dict[str, Any]:
    normalized_assembly_paths = _normalize_assembly_paths(repo_root, assembly_paths)
    tool_path = ensure_declaration_discovery_tool(repo_root)
    list_file_path: Path | None = None
    try:
        with tempfile.NamedTemporaryFile("w", encoding="utf-8", suffix=".txt", delete=False) as handle:
            handle.write("\n".join(str(path) for path in normalized_assembly_paths))
            handle.write("\n")
            list_file_path = Path(handle.name)
        completed = run_process(
            [
                "dotnet",
                "run",
                "--project",
                str(repo_root / DISCOVERY_PROJECT_PATH),
                "-c",
                "Release",
                "--no-build",
                "--",
                str(list_file_path),
            ],
            cwd=repo_root,
        )
    finally:
        if list_file_path is not None:
            list_file_path.unlink(missing_ok=True)
    if completed.returncode != 0:
        output = combine_process_output(completed)
        raise RuntimeError(f"declaration metadata discovery failed\n{output}".strip())

    output_text = (completed.stdout or "").strip()
    if not output_text:
        raise RuntimeError("declaration metadata discovery produced no output")

    payload = json.loads(output_text)
    if not isinstance(payload, dict):
        raise RuntimeError("declaration metadata discovery output must be an object")
    return payload


def _declared_entry_from_raw(
    *,
    subject_id: str,
    kind: declarations_module.DeclaredTestKind,
    payload: dict[str, Any],
) -> declarations_module.DeclaredTestEntry:
    assembly_name = str(payload.get("assemblyName") or "")
    declaring_type = str(payload.get("declaringType") or "")
    method_name = str(payload.get("methodName") or "")
    method_signature = str(payload.get("methodSignature") or "")
    if not method_signature:
        method_signature = f"{method_name}()"

    return declarations_module.DeclaredTestEntry(
        kind=kind,
        stable_id=declarations_module.stable_declared_test_id(
            subject_id=subject_id,
            assembly_name=assembly_name,
            declaring_type=declaring_type,
            method_signature=method_signature,
        ),
        alias=str(payload.get("alias") or "") or None,
        assembly_name=assembly_name,
        declaring_type=declaring_type,
        method_name=method_name,
        method_signature=method_signature,
        category=int(payload.get("category") or 0),
        capability_family=int(payload.get("capabilityFamily") or 0),
        capability_item=int(payload.get("capabilityItem") or 0),
        archetype=int(payload.get("archetype") or 0),
        hot_update_capability=int(payload.get("hotUpdateCapability") or 0),
        metrics=int(payload.get("metrics") or 0),
        modes=int(payload.get("modes") or 0),
        requires=int(payload.get("requires") or 0),
        evidence=int(payload.get("evidence") or 0),
        priority=int(payload.get("priority") or 0),
        warmup_count=int(payload.get("warmupCount") or 0),
        iteration_count=int(payload.get("iterationCount") or 0),
        invocation_count=int(payload.get("invocationCount") or 0),
    )


def _entry_to_dict(entry: declarations_module.DeclaredTestEntry) -> dict[str, Any]:
    payload = {
        "stableId": entry.stable_id,
        "entryIndex": entry.entry_index,
        "alias": entry.alias,
        "assemblyName": entry.assembly_name,
        "declaringType": entry.declaring_type,
        "methodName": entry.method_name,
        "methodSignature": entry.method_signature,
        "category": entry.category,
        "capabilityFamily": entry.capability_family,
        "capabilityItem": entry.capability_item,
        "archetype": entry.archetype,
        "hotUpdateCapability": entry.hot_update_capability,
        "requires": entry.requires,
    }
    if entry.kind is declarations_module.DeclaredTestKind.UNIT:
        payload["evidence"] = entry.evidence
        payload["priority"] = entry.priority
        return payload

    payload["metrics"] = entry.metrics
    payload["modes"] = entry.modes
    payload["warmupCount"] = entry.warmup_count
    payload["iterationCount"] = entry.iteration_count
    payload["invocationCount"] = entry.invocation_count
    return payload


def _manifest_matrix_ids(manifest: dict[str, Any]) -> list[str]:
    return [
        matrix_id
        for matrix_id in [str(matrix.get("matrixId") or "").strip() for matrix in list(manifest.get("environmentMatrices") or [])]
        if matrix_id
    ]


def _manifest_pipeline_ids(manifest: dict[str, Any]) -> list[str]:
    return [
        pipeline_id
        for pipeline_id in [str(pipeline.get("pipelineId") or "").strip() for pipeline in list(manifest.get("executionPipelines") or [])]
        if pipeline_id
    ]


def _manifest_goal_ids(manifest: dict[str, Any]) -> list[str]:
    goal_ids: list[str] = []
    for matrix in list(manifest.get("environmentMatrices") or []):
        for goal_id in list(dict(matrix).get("supportedGoals") or []):
            normalized = str(goal_id or "").strip()
            if normalized and normalized not in goal_ids:
                goal_ids.append(normalized)
    return goal_ids


def _engineering_entry(
    *,
    family: str,
    subject_id: str,
    kind: str,
    manifest: dict[str, Any],
) -> dict[str, Any]:
    return {
        "stableId": f"{family}/{subject_id}/{kind}",
        "kind": kind,
        "subjectId": subject_id,
        "engineeringProfile": str(manifest.get("engineeringProfile") or ""),
        "defaultGoalId": str(manifest.get("defaultGoal") or ""),
        "defaultMatrixId": str(manifest.get("defaultMatrix") or ""),
        "goalIds": _manifest_goal_ids(manifest),
        "matrixIds": _manifest_matrix_ids(manifest),
        "pipelineIds": _manifest_pipeline_ids(manifest),
        "orchestration": dict(manifest.get("orchestration") or {}),
        "availability": dict(manifest.get("availability") or {}),
    }


def _derived_engineering_projection(
    manifest: dict[str, Any],
) -> tuple[set[EngineeringValidationKind], set[EngineeringWorkloadKind]]:
    validations: set[EngineeringValidationKind] = set()
    workloads: set[EngineeringWorkloadKind] = set()

    for matrix in list(manifest.get("environmentMatrices") or []):
        pipeline = subjects_module.find_pipeline(manifest, str(matrix.get("pipelineId") or ""))
        stage_kinds = set(subjects_module.pipeline_stage_kinds(pipeline))
        execution_context = dict(matrix.get("executionContext") or {})
        host_platform = str(execution_context.get("hostPlatform") or "")
        target_platform = str(execution_context.get("targetPlatform") or "")

        if stage_kinds & {
            "analysis-frontend",
            "runtime-managed-output",
            *list(_NATIVE_GENERATION_STAGE_KINDS),
            *list(_NATIVE_EXECUTION_STAGE_KINDS),
        }:
            validations.add(EngineeringValidationKind.PROJECT_GRAPH)

        if "runtime-managed-output" in stage_kinds:
            validations.add(EngineeringValidationKind.MANAGED_BUILD)
            validations.add(EngineeringValidationKind.MANAGED_RUNTIME_OUTPUT)

        if stage_kinds & _NATIVE_EXECUTION_STAGE_KINDS or (
            "build-target" in stage_kinds and stage_kinds & _NATIVE_GENERATION_STAGE_KINDS
        ):
            validations.add(EngineeringValidationKind.NATIVE_EXECUTABLE_SMOKE)

        if host_platform and target_platform and host_platform != target_platform:
            validations.add(EngineeringValidationKind.PACKAGE_INTEGRITY)
            validations.add(EngineeringValidationKind.DEVICE_DEPLOY_SMOKE)
            workloads.add(EngineeringWorkloadKind.DEVICE_PACKAGE)

        if "analysis-frontend" in stage_kinds:
            workloads.add(EngineeringWorkloadKind.CONVERT)
        if stage_kinds & _NATIVE_GENERATION_STAGE_KINDS:
            workloads.add(EngineeringWorkloadKind.CODEGEN)
        if "build-target" in stage_kinds and stage_kinds & _NATIVE_GENERATION_STAGE_KINDS:
            workloads.add(EngineeringWorkloadKind.NATIVE_LINK)
        if any("patch" in stage_kind or "hot-update" in stage_kind for stage_kind in stage_kinds):
            workloads.add(EngineeringWorkloadKind.PATCH_GENERATION)

    return validations, workloads


def build_engineering_projection(
    *,
    subject_id: str,
    manifest: dict[str, Any],
) -> dict[str, list[dict[str, Any]]]:
    engineering_profile = str(manifest.get("engineeringProfile") or "")
    if engineering_profile not in _ENGINEERING_VALIDATIONS_BY_PROFILE:
        raise ValueError(f"unsupported engineeringProfile: {engineering_profile}")

    derived_validations, derived_workloads = _derived_engineering_projection(manifest)
    selected_validations = {
        *_ENGINEERING_VALIDATIONS_BY_PROFILE[engineering_profile],
        *derived_validations,
    }
    selected_workloads = {
        *_ENGINEERING_WORKLOADS_BY_PROFILE[engineering_profile],
        *derived_workloads,
    }

    engineering_validations = [
        _engineering_entry(
            family="engineering-validation",
            subject_id=subject_id,
            kind=kind.value,
            manifest=manifest,
        )
        for kind in sorted(selected_validations, key=lambda item: item.value)
    ]
    engineering_workloads = [
        _engineering_entry(
            family="engineering-workload",
            subject_id=subject_id,
            kind=kind.value,
            manifest=manifest,
        )
        for kind in sorted(selected_workloads, key=lambda item: item.value)
    ]
    return {
        "engineeringValidations": engineering_validations,
        "engineeringWorkloads": engineering_workloads,
    }


def build_declared_test_catalog(
    *,
    repo_root: Path,
    subject_id: str,
    declaration_mode: str | declarations_module.TestDeclarationMode,
    assembly_paths: Iterable[str | Path],
) -> dict[str, Any]:
    mode = (
        declaration_mode
        if isinstance(declaration_mode, declarations_module.TestDeclarationMode)
        else declarations_module.TestDeclarationMode(str(declaration_mode))
    )
    if mode is declarations_module.TestDeclarationMode.NONE:
        summary = declarations_module.summarize_declaration_scan(
            mode=mode,
            framework_referenced=False,
            declared_unit_tests=[],
            declared_benchmarks=[],
        )
        return {
            "schemaVersion": _COLLECTION_SCHEMA_VERSION,
            "subjectId": subject_id,
            "frameworkReferenced": summary.framework_referenced,
            "subjectKind": summary.subject_kind.value,
            "warningCodes": [code.value for code in summary.warning_codes],
            "declaredUnitTests": [],
            "declaredBenchmarks": [],
        }

    raw_payload = discover_declared_test_metadata(repo_root=repo_root, assembly_paths=assembly_paths)

    declared_unit_tests = [
        _declared_entry_from_raw(
            subject_id=subject_id,
            kind=declarations_module.DeclaredTestKind.UNIT,
            payload=dict(item),
        )
        for item in list(raw_payload.get("declaredUnitTests") or [])
    ]
    declared_benchmarks = [
        _declared_entry_from_raw(
            subject_id=subject_id,
            kind=declarations_module.DeclaredTestKind.BENCHMARK,
            payload=dict(item),
        )
        for item in list(raw_payload.get("declaredBenchmarks") or [])
    ]
    summary = declarations_module.summarize_declaration_scan(
        mode=mode,
        framework_referenced=bool(raw_payload.get("frameworkReferenced")),
        declared_unit_tests=declared_unit_tests,
        declared_benchmarks=declared_benchmarks,
    )
    return {
        "schemaVersion": _COLLECTION_SCHEMA_VERSION,
        "subjectId": subject_id,
        "frameworkReferenced": summary.framework_referenced,
        "subjectKind": summary.subject_kind.value,
        "warningCodes": [code.value for code in summary.warning_codes],
        "declaredUnitTests": [_entry_to_dict(entry) for entry in summary.declared_unit_tests],
        "declaredBenchmarks": [_entry_to_dict(entry) for entry in summary.declared_benchmarks],
    }


def _find_primary_project_assembly(project_path: Path) -> Path | None:
    candidates = sorted(
        project_path.parent.glob(f"bin/Debug/**/{project_path.stem}.dll"),
        key=lambda candidate: (len(candidate.parts), str(candidate)),
    )
    for candidate in candidates:
        if any(part in {"ref", "refint"} for part in candidate.parts):
            continue
        if candidate.is_file():
            return candidate.resolve()
    return None


def _solution_assembly_paths(
    repo_root: Path,
    source: dict[str, Any],
    *,
    primary_project_path: Path,
) -> list[Path]:
    solution_project_paths = subjects_module.resolve_source_solution_project_paths(repo_root, source)
    assembly_names = subjects_module.resolve_source_solution_assembly_names(repo_root, source)
    if not solution_project_paths or not assembly_names:
        return []

    primary_assembly = _find_primary_project_assembly(primary_project_path)
    assembly_paths: list[Path] = []
    seen: set[str] = set()

    candidate_roots: list[Path] = []
    if primary_assembly is not None:
        candidate_roots.append(primary_assembly.parent)
    candidate_roots.append(primary_project_path.parent)
    source_path_text = str(source.get("path") or "").strip()
    if source_path_text.endswith(".sln"):
        solution_path = Path(source_path_text)
        if not solution_path.is_absolute():
            solution_path = repo_root / solution_path
        if solution_path.is_file():
            candidate_roots.append(solution_path.parent)

    project_path_by_assembly_name: dict[str, Path] = {}
    for project_path_text in solution_project_paths:
        project_path = Path(project_path_text)
        if not project_path.is_absolute():
            project_path = repo_root / project_path
        if not project_path.is_file():
            continue
        assembly_name = _ASSEMBLY_NAME_PATTERN.search(project_path.read_text(encoding="utf-8"))
        normalized_assembly_name = (
            assembly_name.group(1).strip()
            if assembly_name is not None
            else project_path.stem
        )
        if normalized_assembly_name and normalized_assembly_name not in project_path_by_assembly_name:
            project_path_by_assembly_name[normalized_assembly_name] = project_path

    for assembly_name in assembly_names:
        candidates: list[Path] = []
        for candidate_root in candidate_roots:
            direct_candidate = candidate_root / f"{assembly_name}.dll"
            if direct_candidate.is_file():
                candidates.append(direct_candidate)
            candidates.extend(
                sorted(
                    candidate_root.glob(f"bin/Debug/**/{assembly_name}.dll"),
                    key=lambda candidate: (len(candidate.parts), str(candidate)),
                )
            )
            candidates.extend(
                sorted(
                    candidate_root.glob(f"**/bin/Debug/**/{assembly_name}.dll"),
                    key=lambda candidate: (len(candidate.parts), str(candidate)),
                )
            )
        project_path = project_path_by_assembly_name.get(assembly_name)
        if project_path is not None:
            candidates.extend(
                sorted(
                    project_path.parent.glob(f"bin/Debug/**/{assembly_name}.dll"),
                    key=lambda candidate: (len(candidate.parts), str(candidate)),
                )
            )
        for candidate in candidates:
            if any(part in {"ref", "refint"} for part in candidate.parts):
                continue
            resolved = candidate.resolve()
            key = str(resolved).lower() if os.name == "nt" else str(resolved)
            if key in seen:
                continue
            seen.add(key)
            assembly_paths.append(resolved)
            break

    return assembly_paths


def _resolve_subject_assembly_paths(
    repo_root: Path,
    manifest: dict[str, Any],
    *,
    build_if_missing: bool = True,
    force_build: bool = False,
) -> list[Path]:
    source = dict(manifest.get("source") or {})
    source_type = str(source.get("type") or "")
    if source_type != "dotnet-project":
        raise ValueError("declared test discovery currently requires source.type=dotnet-project")

    project_path_text = subjects_module.resolve_source_primary_project_path(source)
    project_path = repo_root / project_path_text
    if not project_path.is_file():
        raise FileNotFoundError(f"subject source project missing: {project_path}")

    source_path_text = str(source.get("path") or "").strip()
    stale_existing_output = False
    if not force_build:
        existing_solution_assemblies = _solution_assembly_paths(
            repo_root,
            source,
            primary_project_path=project_path,
        )
        if existing_solution_assemblies:
            if not _assemblies_are_stale(repo_root, manifest, existing_solution_assemblies):
                return existing_solution_assemblies
            stale_existing_output = True

        existing_primary_assembly = _find_primary_project_assembly(project_path)
        if existing_primary_assembly is not None:
            if not _assemblies_are_stale(repo_root, manifest, [existing_primary_assembly]):
                return [existing_primary_assembly]
            stale_existing_output = True

    if not build_if_missing and not force_build and not stale_existing_output:
        return []

    build_target = project_path
    if source_path_text.endswith(".sln"):
        build_target = repo_root / source_path_text
        if not build_target.is_file():
            raise FileNotFoundError(f"subject source solution missing: {build_target}")

    completed = run_process(
        [
            "dotnet",
            "build",
            str(build_target),
            "-c",
            "Debug",
            "-m:1",
            *_dotnet_intermediate_args(build_target.stem),
        ],
        cwd=repo_root,
    )
    if completed.returncode != 0:
        output = combine_process_output(completed)
        raise RuntimeError(f"dotnet build failed: {build_target.relative_to(repo_root).as_posix()}\n{output}".strip())

    built_solution_assemblies = _solution_assembly_paths(
        repo_root,
        source,
        primary_project_path=project_path,
    )
    if built_solution_assemblies:
        return built_solution_assemblies

    built_primary_assembly = _find_primary_project_assembly(project_path)
    if built_primary_assembly is None:
        raise FileNotFoundError(f"built assembly missing for subject source project: {project_path}")
    return [built_primary_assembly]


def build_subject_declared_test_catalog(
    *,
    repo_root: Path,
    subject_id: str,
    force_build: bool = False,
) -> dict[str, Any]:
    manifest = subjects_module.load_subject_manifest(repo_root, subject_id)
    declaration_mode = declarations_module.test_declaration_mode(manifest)
    if declaration_mode is declarations_module.TestDeclarationMode.NONE:
        return build_declared_test_catalog(
            repo_root=repo_root,
            subject_id=subject_id,
            declaration_mode=declaration_mode,
            assembly_paths=[],
        )

    assembly_paths = _resolve_subject_assembly_paths(
        repo_root,
        manifest,
        build_if_missing=declaration_mode is declarations_module.TestDeclarationMode.REQUIRE,
        force_build=force_build,
    )
    if not assembly_paths:
        summary = declarations_module.summarize_declaration_scan(
            mode=declaration_mode,
            framework_referenced=False,
            declared_unit_tests=[],
            declared_benchmarks=[],
        )
        return {
            "schemaVersion": _COLLECTION_SCHEMA_VERSION,
            "subjectId": subject_id,
            "frameworkReferenced": summary.framework_referenced,
            "subjectKind": summary.subject_kind.value,
            "warningCodes": [code.value for code in summary.warning_codes],
            "declaredUnitTests": [],
            "declaredBenchmarks": [],
        }
    return build_declared_test_catalog(
        repo_root=repo_root,
        subject_id=subject_id,
        declaration_mode=declaration_mode,
        assembly_paths=assembly_paths,
    )


def build_compiled_subject_catalog(
    *,
    repo_root: Path,
    subject_id: str,
    declared_catalog: dict[str, Any] | None = None,
) -> dict[str, Any]:
    manifest = subjects_module.load_subject_manifest(repo_root, subject_id)
    if declared_catalog is not None:
        declared_subject_id = str(declared_catalog.get("subjectId") or "")
        if declared_subject_id and declared_subject_id != subject_id:
            raise ValueError(
                f"declared collection subjectId mismatch: expected {subject_id}, got {declared_subject_id}"
            )
        declared_unit_tests = list(declared_catalog.get("declaredUnitTests") or [])
        declared_benchmarks = list(declared_catalog.get("declaredBenchmarks") or [])
    else:
        declared_unit_tests = []
        declared_benchmarks = []

    projection = build_engineering_projection(subject_id=subject_id, manifest=manifest)
    return {
        "subjectId": subject_id,
        "displayName": str(manifest.get("displayName") or subject_id),
        "sourceModel": str(manifest.get("sourceModel") or ""),
        "dependencyModel": str(manifest.get("dependencyModel") or ""),
        "executablePlan": str(manifest.get("executablePlan") or ""),
        "engineeringProfile": str(manifest.get("engineeringProfile") or ""),
        "testDeclarationMode": declarations_module.test_declaration_mode(manifest).value,
        "orchestration": dict(manifest.get("orchestration") or {}),
        "availability": dict(manifest.get("availability") or {}),
        "compatibility": dict(manifest.get("compatibility") or {}),
        "engineeringValidations": list(projection["engineeringValidations"]),
        "engineeringWorkloads": list(projection["engineeringWorkloads"]),
        "declaredUnitTests": declared_unit_tests,
        "declaredBenchmarks": declared_benchmarks,
    }
