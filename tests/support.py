from __future__ import annotations

import json
import shutil
from pathlib import Path
from typing import Any

from tests._support.fs import REPO_ROOT, TEST_TMP_ROOT, make_temp_repo_root, write_json
from tests._support.module_loading import load_module

PUBLIC_SPECS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "public_specs.py"
FIXTURE_SUBJECTS_ROOT = REPO_ROOT / "tests" / "fixtures" / "subjects"
REGISTRY_FIXTURES_ROOT = REPO_ROOT / "tests" / "fixtures" / "registry"
VERIFICATION_CATALOG_ROOT = REPO_ROOT / "verification" / "catalog"
VERIFICATION_OWNERS_ROOT = VERIFICATION_CATALOG_ROOT / "owners"
VERIFICATION_SCENARIOS_ROOT = VERIFICATION_CATALOG_ROOT / "scenarios"

SOLUTION_CORE_PACK_OWNER_ROOT = VERIFICATION_OWNERS_ROOT / "SolutionCorePack"
SOLUTION_CORE_PACK_SCENARIO_ROOT = VERIFICATION_SCENARIOS_ROOT / "SolutionCorePack"
SOLUTION_CORE_PACK_OWNER_MANIFEST_PATH = SOLUTION_CORE_PACK_OWNER_ROOT / "owner.manifest.json"
SOLUTION_CORE_PACK_HOST_SOLUTION_PATH = SOLUTION_CORE_PACK_OWNER_ROOT / "support" / "host" / "SolutionCorePack.sln"
SOLUTION_CORE_PACK_HOST_PROJECT_PATH = SOLUTION_CORE_PACK_OWNER_ROOT / "support" / "host" / "SolutionCorePack.csproj"
SOLUTION_CORE_PACK_PROOFS_ROOT = SOLUTION_CORE_PACK_OWNER_ROOT / "proofs" / "CoreRuntimeFeatures"
SOLUTION_CORE_PACK_PROOFS_PROJECT_PATH = SOLUTION_CORE_PACK_PROOFS_ROOT / "CoreRuntimeFeatures.csproj"
SOLUTION_CORE_PACK_BENCHMARKS_ROOT = SOLUTION_CORE_PACK_OWNER_ROOT / "benchmarks" / "CoreRuntimeBenchmarks"
SOLUTION_CORE_PACK_BENCHMARKS_PROJECT_PATH = SOLUTION_CORE_PACK_BENCHMARKS_ROOT / "CoreRuntimeBenchmarks.csproj"

HOT_UPDATE_HOST_PACK_OWNER_ROOT = VERIFICATION_OWNERS_ROOT / "HotUpdateHostPack"
HOT_UPDATE_HOST_PACK_OWNER_MANIFEST_PATH = HOT_UPDATE_HOST_PACK_OWNER_ROOT / "owner.manifest.json"
HOT_UPDATE_HOST_PACK_HOST_PROJECT_PATH = HOT_UPDATE_HOST_PACK_OWNER_ROOT / "support" / "host" / "HotUpdateHostPack.csproj"
HOT_UPDATE_HOST_PACK_BENCHMARKS_ROOT = HOT_UPDATE_HOST_PACK_OWNER_ROOT / "benchmarks"
HOT_UPDATE_HOST_PACK_PROOFS_ROOT = HOT_UPDATE_HOST_PACK_OWNER_ROOT / "proofs"
HOT_UPDATE_HOST_PACK_HOST_PROGRAM_PATH = HOT_UPDATE_HOST_PACK_OWNER_ROOT / "support" / "host" / "Program.cs"
HOT_UPDATE_SKELETON_PROOF_PATH = HOT_UPDATE_HOST_PACK_PROOFS_ROOT / "HotUpdateSkeletonProofEntry.cs"

MIXED_EXECUTION_FEATURE_PACK_OWNER_ROOT = VERIFICATION_OWNERS_ROOT / "MixedExecutionFeaturePack"
MIXED_EXECUTION_FEATURE_PACK_OWNER_MANIFEST_PATH = MIXED_EXECUTION_FEATURE_PACK_OWNER_ROOT / "owner.manifest.json"
MIXED_EXECUTION_FEATURE_PACK_HOST_PROJECT_PATH = (
    MIXED_EXECUTION_FEATURE_PACK_OWNER_ROOT / "support" / "host" / "MixedExecutionFeaturePack.csproj"
)
MIXED_EXECUTION_FEATURE_PACK_HOST_PROGRAM_PATH = (
    MIXED_EXECUTION_FEATURE_PACK_OWNER_ROOT / "support" / "host" / "Program.cs"
)
MIXED_EXECUTION_FEATURE_PACK_PROOFS_ROOT = MIXED_EXECUTION_FEATURE_PACK_OWNER_ROOT / "proofs"

MIXED_EXECUTION_FEATURE_PACK_SCENARIO_ROOT = VERIFICATION_SCENARIOS_ROOT / "MixedExecutionFeaturePack"
MIXED_BRIDGE_SOLUTION_SCENARIO_ROOT = MIXED_EXECUTION_FEATURE_PACK_SCENARIO_ROOT / "MixedBridgeSolution"
MIXED_BRIDGE_SOLUTION_PATH = MIXED_BRIDGE_SOLUTION_SCENARIO_ROOT / "MixedBridgeSolution.sln"
INTERPRETER_ARITHMETIC_PROJECT_PATH = (
    MIXED_BRIDGE_SOLUTION_SCENARIO_ROOT
    / "InterpreterArithmeticProof"
    / "InterpreterArithmeticProof.csproj"
)


def read_text_bundle(*paths: Path) -> str:
    return "\n".join(path.read_text(encoding="utf-8") for path in paths)


def read_contracts_source(repo_root: Path = REPO_ROOT) -> str:
    contracts_root = repo_root / "src" / "managed" / "Chaos.IL2CPP.Contracts"
    return read_text_bundle(
        contracts_root / "ManagedClosureContracts.cs",
        contracts_root / "ManagedNaming.cs",
        contracts_root / "ManagedClosureModels.cs",
        contracts_root / "ManagedMethodIdentityContracts.cs",
        contracts_root / "ManagedSemanticWorldContracts.cs",
        contracts_root / "TypedIlAndAotCoreIrContracts.cs",
        contracts_root / "ManagedClosureArtifactModels.cs",
    )


def read_loader_stage_source(repo_root: Path = REPO_ROOT) -> str:
    loader_root = repo_root / "src" / "managed" / "Chaos.IL2CPP.Loader"
    return read_text_bundle(
        loader_root / "LoaderStage.cs",
        loader_root / "LoaderStage.CrossAssemblyInstantiation.cs",
        loader_root / "LoaderStage.AssemblyLoading.cs",
        loader_root / "LoaderStage.InstructionDecoding.cs",
        loader_root / "LoaderStage.MetadataResolution.cs",
        loader_root / "LoaderStage.GenericInstantiationProjection.cs",
    )


def read_linker_stage_source(repo_root: Path = REPO_ROOT) -> str:
    linker_root = repo_root / "src" / "managed" / "Chaos.IL2CPP.Linker"
    return read_text_bundle(
        linker_root / "LinkerStage.cs",
        linker_root / "LinkerStage.Reachability.cs",
        linker_root / "LinkerStage.OptimizationFacts.cs",
        linker_root / "LinkerStage.DispatchResolution.cs",
        linker_root / "LinkerStage.OutputProjection.cs",
    )


def read_native_reference_planner_source(repo_root: Path = REPO_ROOT) -> str:
    codegen_root = repo_root / "src" / "managed" / "Chaos.IL2CPP.Generator"
    reference_root = codegen_root / "ReferenceProof"
    return read_text_bundle(
        codegen_root / "NativeReferenceLoweringPlanner.cs",
        reference_root / "NativeReferenceLoweringPlanner.EngineAndFamilySelection.cs",
        reference_root / "NativeReferenceLoweringPlanner.PlanBuilders.cs",
        reference_root / "NativeReferenceLoweringPlanner.ShapeValidation.cs",
    )


def read_native_aot_planner_source(repo_root: Path = REPO_ROOT) -> str:
    codegen_root = repo_root / "src" / "managed" / "Chaos.IL2CPP.Generator"
    planner_parts = [
        codegen_root / "NativeAotLoweringPlanner.cs",
        *sorted((codegen_root / "Planning").glob("NativeAotLoweringPlanner*.cs")),
        *sorted((codegen_root / "Emission").glob("NativeAotLoweringPlanner*.cs")),
        *sorted((codegen_root / "RuntimeSupport").glob("NativeAotLoweringPlanner*.cs")),
    ]
    return read_text_bundle(*planner_parts)


def get_method_subject_display_string(subject_id: str) -> str:
    separator_index = subject_id.find("::")
    parameter_list_index = subject_id.rfind("(")
    if separator_index <= 0 or parameter_list_index <= separator_index + 2 or not subject_id.endswith(")"):
        return subject_id

    return_type_separator_index = subject_id.rfind(":", separator_index + 2, parameter_list_index)
    method_name_end_index = (
        return_type_separator_index
        if return_type_separator_index > separator_index + 1
        else parameter_list_index
    )
    method_name = subject_id[(separator_index + 2) : method_name_end_index]
    parameter_signature = subject_id[(parameter_list_index + 1) : -1]
    return f"{subject_id[:separator_index]}::{method_name}({parameter_signature})"


def find_method_by_subject_id(
    methods: list[dict[str, Any]] | dict[str, dict[str, Any]],
    requested_subject_id: str,
) -> dict[str, Any]:
    if isinstance(methods, dict):
        exact_match = methods.get(requested_subject_id)
        if exact_match is not None:
            return exact_match
        candidates = list(methods.values())
    else:
        candidates = list(methods)

    requested_display = get_method_subject_display_string(requested_subject_id)
    matches = [
        method
        for method in candidates
        if get_method_subject_display_string(str(method.get("subjectId") or "")) == requested_display
    ]
    if len(matches) != 1:
        raise AssertionError(
            f"expected exactly one method for subject id '{requested_subject_id}', found {len(matches)}"
        )
    return matches[0]


def _rewrite_path_prefixes(payload: object, *, prefix: str, replacement: str) -> object:
    if isinstance(payload, dict):
        return {
            key: _rewrite_path_prefixes(value, prefix=prefix, replacement=replacement)
            for key, value in payload.items()
        }
    if isinstance(payload, list):
        return [_rewrite_path_prefixes(value, prefix=prefix, replacement=replacement) for value in payload]
    if isinstance(payload, str) and payload.startswith(prefix):
        return replacement + payload.removeprefix(prefix)
    return payload


def rewrite_fixture_manifest_paths(payload: object, fixture_subject_id: str) -> object:
    return _rewrite_path_prefixes(
        payload,
        prefix=f"tests/fixtures/subjects/{fixture_subject_id}/",
        replacement=f"subjects/{fixture_subject_id}/",
    )


def clone_fixture_subject_repo(
    fixture_subject_id: str,
    *,
    area: str = "fixture-subjects",
) -> tuple[Path, dict[str, Any]]:
    repo_root = make_temp_repo_root(area, fixture_subject_id.lower())
    subject_root = repo_root / "subjects" / fixture_subject_id
    shutil.copytree(FIXTURE_SUBJECTS_ROOT / fixture_subject_id, subject_root)
    legacy_manifest_path = subject_root / "subject.manifest.json"
    manifest = json.loads(legacy_manifest_path.read_text(encoding="utf-8"))
    manifest = rewrite_fixture_manifest_paths(manifest, fixture_subject_id)
    legacy_manifest_path.unlink()
    owner_manifest_path = repo_root / "verification" / "catalog" / "owners" / fixture_subject_id / "owner.manifest.json"
    write_json(owner_manifest_path, manifest)
    write_json(
        owner_manifest_path.parent / "owner.features.json",
        {
            "subjectId": fixture_subject_id,
            "features": [],
        },
    )
    return repo_root, manifest


def clone_registry_fixture_tree(repo_root: Path) -> Path:
    target_root = repo_root / "tests" / "fixtures" / "registry"
    target_root.parent.mkdir(parents=True, exist_ok=True)
    shutil.copytree(REGISTRY_FIXTURES_ROOT, target_root)
    return target_root


def materialize_subject_manifest(repo_root: Path, manifest: dict[str, Any]) -> Path:
    subject_id = str(manifest["subjectId"])
    source = dict(manifest.get("source") or {})
    relative_paths = [
        str(source.get("path") or ""),
        str(source.get("primaryProjectPath") or ""),
    ]
    validation = dict(manifest.get("validation") or {})
    for validation_spec in validation.values():
        relative_paths.append(str(dict(validation_spec).get("project") or ""))

    for relative_path in relative_paths:
        if not relative_path:
            continue
        materialized_path = repo_root / relative_path
        materialized_path.parent.mkdir(parents=True, exist_ok=True)
        if materialized_path.suffix:
            materialized_path.write_text("<Project />\n", encoding="utf-8")
        else:
            materialized_path.mkdir(parents=True, exist_ok=True)

    for expected_path in dict(manifest.get("expected") or {}).values():
        (repo_root / str(expected_path)).mkdir(parents=True, exist_ok=True)
    for baseline_path in dict(manifest.get("baselines") or {}).values():
        target_path = repo_root / str(baseline_path)
        if target_path.suffix:
            target_path.parent.mkdir(parents=True, exist_ok=True)
        else:
            target_path.mkdir(parents=True, exist_ok=True)

    manifest_path = repo_root / "verification" / "catalog" / "owners" / subject_id / "owner.manifest.json"
    write_json(manifest_path, manifest)
    return manifest_path

def load_public_specs_module(module_name: str):
    return load_module(PUBLIC_SPECS_MODULE_PATH, module_name)


def select_public_suite_spec(
    module_name: str,
    *,
    host_platform: str | None = None,
    family: str | None = None,
    required_stages: list[str] | None = None,
    required_module_ids: list[str] | None = None,
    required_subsystem_ids: list[str] | None = None,
) -> dict[str, Any]:
    public_specs_module = load_public_specs_module(module_name)
    matches: list[dict[str, Any]] = []
    for spec in list(public_specs_module.PUBLIC_TEST_SPECS):
        if host_platform is not None and host_platform not in list(spec.get("supported_hosts") or []):
            continue
        if family is not None and str(spec.get("family") or "") != family:
            continue
        if required_stages and not set(required_stages).issubset(set(spec.get("stages") or [])):
            continue
        if required_module_ids and not set(required_module_ids).issubset(set(spec.get("moduleIds") or [])):
            continue
        if required_subsystem_ids and not set(required_subsystem_ids).issubset(set(spec.get("subsystemIds") or [])):
            continue
        matches.append(dict(spec))
    if not matches:
        raise AssertionError(
            "no public suite spec matched filters: "
            f"host_platform={host_platform}, family={family}, required_stages={required_stages}, "
            f"required_module_ids={required_module_ids}, required_subsystem_ids={required_subsystem_ids}"
        )
    return sorted(matches, key=lambda item: str(item["id"]))[0]


def select_registry_item(
    items: list[dict[str, Any]],
    *,
    object_type: str | None = None,
    required_family: str | None = None,
    required_level: str | None = None,
    required_module_ids: list[str] | None = None,
    required_subsystem_ids: list[str] | None = None,
    required_supported_hosts: list[str] | None = None,
) -> dict[str, Any]:
    matches: list[dict[str, Any]] = []
    for item in items:
        if object_type is not None and str(item.get("type") or "") != object_type:
            continue
        if required_family is not None and str(item.get("family") or "") != required_family:
            continue
        if required_level is not None and str(item.get("level") or "") != required_level:
            continue
        if required_module_ids and not set(required_module_ids).issubset(set(item.get("moduleIds") or [])):
            continue
        if required_subsystem_ids and not set(required_subsystem_ids).issubset(set(item.get("subsystemIds") or [])):
            continue
        if required_supported_hosts and not set(required_supported_hosts).issubset(set(item.get("supportedHosts") or [])):
            continue
        matches.append(item)
    if not matches:
        raise AssertionError(
            "no registry item matched filters: "
            f"object_type={object_type}, required_family={required_family}, required_level={required_level}, "
            f"required_module_ids={required_module_ids}, required_subsystem_ids={required_subsystem_ids}, "
            f"required_supported_hosts={required_supported_hosts}"
        )
    return sorted(matches, key=lambda item: str(item["id"]))[0]
