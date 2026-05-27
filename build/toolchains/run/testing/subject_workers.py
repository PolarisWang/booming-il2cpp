from __future__ import annotations

import importlib.util
import json
import os
from pathlib import Path
import shutil
import shlex
import subprocess
from typing import Any
import statistics
import sys
import time

try:
    from ..core.common import combine_process_output, read_json, run_process, write_json
    from ..core import tooling as tooling_module
    from . import compiled_catalog as compiled_catalog_module
    from . import contracts as contracts_module
    from . import mobile_perf_collector
    from . import perf as perf_module
    from . import subjects as subjects_module
    from . import template_assets as template_assets_module
    from . import verification_layout as verification_layout_module
    from . import workspace_manifests as workspace_manifests_module
    from . import workspace_declared_collection as workspace_declared_collection_module
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from core.common import combine_process_output, read_json, run_process, write_json
    from core import tooling as tooling_module
    from testing import compiled_catalog as compiled_catalog_module
    from testing import contracts as contracts_module
    from testing import mobile_perf_collector
    from testing import perf as perf_module
    from testing import subjects as subjects_module
    from testing import template_assets as template_assets_module
    from testing import verification_layout as verification_layout_module
    from testing import workspace_manifests as workspace_manifests_module
    from testing import workspace_declared_collection as workspace_declared_collection_module


DRIVER_PROJECT_PATH = Path("src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj")
DRIVER_DLL_PATH = Path("src/managed/Chaos.IL2CPP.Driver/bin/Release/net8.0/Chaos.IL2CPP.Driver.dll")
WINDOWS_TRACE_SNAPSHOT_PATH = Path("tests/contracts/trace/snapshots/windows-warmup-trace.snapshot.json")
MACOS_TRACE_SNAPSHOT_PATH = Path("tests/contracts/trace/snapshots/macos-warmup-trace.snapshot.json")
TRACE_SCHEMA_PATH = Path("tests/contracts/trace/schema/warmup-trace.schema.json")
WINDOWS_REFERENCE_BUILD_TARGET = "chaos_subject_reference_proof"
WINDOWS_REFERENCE_RUN_TARGET = "chaos_subject_reference_proof_run"
WINDOWS_REFERENCE_CMAKE_BUILD_STRATEGY = "windows-reference-cmake"
WINDOWS_BENCHMARK_CMAKE_BUILD_STRATEGY = "windows-benchmark-cmake"
WINDOWS_NATIVE_AOT_CMAKE_BUILD_STRATEGY = "windows-native-aot-cmake"
WINDOWS_DIRECT_BUILD_STRATEGY = "direct-msvc"
WINDOWS_NATIVE_AOT_BUILD_TARGET = "chaos_subject_native_aot"
WINDOWS_NATIVE_AOT_BUILD_STRATEGY = "direct-msvc-native-aot"
ANDROID_NATIVE_BUILD_STRATEGY = "android-native-cmake"
ANDROID_RUNTIME_BUILD_TARGET = "mobile_hello_world_android_host_runtime"
ANDROID_RUNTIME_REMOTE_ROOT = "/data/local/tmp/chaos-subjects"
ANDROID_EXIT_CODE_PREFIX = "__CHAOS_EXIT_CODE__="
ANDROID_RUNTIME_ARGUMENT_ENVIRONMENTS = {
    "--soak-duration-seconds=": "CHAOS_MOBILE_HOST_SOAK_DURATION_SECONDS",
    "--heartbeat-interval-seconds=": "CHAOS_MOBILE_HOST_HEARTBEAT_INTERVAL_SECONDS",
    "--subject-id=": "CHAOS_MOBILE_HOST_SUBJECT_ID",
}
CHAOS_ENTRY_KIND_ARGUMENT_PREFIX = "--chaos-entry-kind="
CHAOS_ENTRY_SLICE_ARGUMENT_PREFIX = "--chaos-entry-slice="
CHAOS_HOST_KIND_ARGUMENT_PREFIX = "--host-kind="
CHAOS_COLLECTION_PATH_ARGUMENT_PREFIX = "--collection-path="
CHAOS_ENTRY_INDEX_ARGUMENT_PREFIX = "--entry-index="
SHARED_RUNTIME_HOST_EXECUTION_MODEL = "shared-runtime-host"
VARIANT_MACROS = {
    "CHECK": {
        "codegen": ["CHAOS_VARIANT_CHECK", "CHAOS_VARIANT_NAME=CHECK"],
        "native": ["CHAOS_VARIANT_CHECK", "CHAOS_VARIANT_NAME=CHECK"],
    },
    "PROFILE": {
        "codegen": ["CHAOS_VARIANT_PROFILE", "CHAOS_VARIANT_NAME=PROFILE"],
        "native": ["CHAOS_VARIANT_PROFILE", "CHAOS_VARIANT_NAME=PROFILE"],
    },
    "SHIP": {
        "codegen": ["CHAOS_VARIANT_SHIP", "CHAOS_VARIANT_NAME=SHIP"],
        "native": ["CHAOS_VARIANT_SHIP", "CHAOS_VARIANT_NAME=SHIP"],
    },
}
ENGINE_OBSERVE_PREFIX = "CHAOS_ENGINE_OBSERVE "
_SUBJECT_TEMPLATE_OWNER_FILE = Path(__file__).resolve().parents[1] / "subject" / "project_workspace.py"
_PROJECT_WORKSPACE_MODULE_PATH = Path(__file__).resolve().parents[1] / "subject" / "project_workspace.py"
_NATIVE_REFERENCE_RUN_SCRIPT_TEMPLATE = "templates/native-proof-run.cmake.tmpl"
_NATIVE_REFERENCE_WORKSPACE_TEMPLATE = "templates/native-reference-workspace.cmake.tmpl"
_NATIVE_GENERATED_TEMPLATE = "templates/native-generated.cmake.tmpl"
_NATIVE_PROOF_TEMPLATE = "templates/native-proof.cmake.tmpl"
_NATIVE_PROOF_MAIN_TEMPLATE = "templates/native-proof-main.cpp.tmpl"
_WORKSPACE_MANIFEST_RELEVANT_KEYS = (
    "projectPath",
    "collectionPath",
    "generatedSourcePath",
    "bindingManifestPath",
    "configureRoot",
)
_PROJECT_WORKSPACE_MODULE_CACHE: Any | None = None
_ACTIVE_WORKSPACE_MANIFEST_GENERATIONS: set[str] = set()


def _render_windows_native_aot_workspace_cmakelists(repo_root: Path, *, host_subdir: str) -> str:
    repo_root_text = repo_root.as_posix()
    return f"""cmake_minimum_required(VERSION 3.20)

project(chaos_subject_native_aot_workspace LANGUAGES CXX)

if((NOT DEFINED CHAOS_SUBJECT_HOST_MAIN OR CHAOS_SUBJECT_HOST_MAIN STREQUAL "") AND DEFINED CHAOS_SUBJECT_BENCHMARK_HOST_MAIN AND NOT CHAOS_SUBJECT_BENCHMARK_HOST_MAIN STREQUAL "")
    set(CHAOS_SUBJECT_HOST_MAIN "${{CHAOS_SUBJECT_BENCHMARK_HOST_MAIN}}")
endif()

if(NOT DEFINED CHAOS_SUBJECT_HOST_MAIN OR CHAOS_SUBJECT_HOST_MAIN STREQUAL "")
    message(FATAL_ERROR "CHAOS_SUBJECT_HOST_MAIN is required")
endif()

if(NOT DEFINED CHAOS_SUBJECT_GENERATED_INPUT_SOURCE OR CHAOS_SUBJECT_GENERATED_INPUT_SOURCE STREQUAL "")
    message(FATAL_ERROR "CHAOS_SUBJECT_GENERATED_INPUT_SOURCE is required")
endif()

if(NOT DEFINED CHAOS_SUBJECT_BUILD_OUT_ROOT OR CHAOS_SUBJECT_BUILD_OUT_ROOT STREQUAL "")
    message(FATAL_ERROR "CHAOS_SUBJECT_BUILD_OUT_ROOT is required")
endif()

set(REPO_ROOT "{repo_root_text}")
set(CHAOS_SUBJECT_VARIANT "CHECK" CACHE STRING "Variant selector for the current subject benchmark fixture")
set_property(CACHE CHAOS_SUBJECT_VARIANT PROPERTY STRINGS CHECK PROFILE SHIP)

function(chaos_apply_subject_variant target_name)
    if(CHAOS_SUBJECT_VARIANT STREQUAL "CHECK")
        target_compile_definitions(
            "${{target_name}}"
            PRIVATE
                CHAOS_VARIANT_CHECK
                CHAOS_IL2CPP_CONFIG_CHECK
                "CHAOS_VARIANT_NAME=\\"CHECK\\"")
        target_compile_options(
            "${{target_name}}"
            PRIVATE
                $<$<CXX_COMPILER_ID:MSVC>:/Od /Zi>
                $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-O0 -g>)
        return()
    endif()

    if(CHAOS_SUBJECT_VARIANT STREQUAL "PROFILE")
        target_compile_definitions(
            "${{target_name}}"
            PRIVATE
                CHAOS_VARIANT_PROFILE
                CHAOS_IL2CPP_CONFIG_PROFILE
                "CHAOS_VARIANT_NAME=\\"PROFILE\\"")
        target_compile_options(
            "${{target_name}}"
            PRIVATE
                $<$<CXX_COMPILER_ID:MSVC>:/O2 /DNDEBUG>
                $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-O3 -DNDEBUG>)
        return()
    endif()

    if(CHAOS_SUBJECT_VARIANT STREQUAL "SHIP")
        target_compile_definitions(
            "${{target_name}}"
            PRIVATE
                CHAOS_VARIANT_SHIP
                CHAOS_IL2CPP_CONFIG_SHIP
                "CHAOS_VARIANT_NAME=\\"SHIP\\"")
        target_compile_options(
            "${{target_name}}"
            PRIVATE
                $<$<CXX_COMPILER_ID:MSVC>:/O2 /GL /DNDEBUG>
                $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-O3 -DNDEBUG>)
        target_link_options(
            "${{target_name}}"
            PRIVATE
                $<$<CXX_COMPILER_ID:MSVC>:/LTCG>)
        return()
    endif()

    message(FATAL_ERROR "Unsupported CHAOS_SUBJECT_VARIANT='${{CHAOS_SUBJECT_VARIANT}}'")
endfunction()

function(chaos_configure_subject_target target_name)
    target_compile_features("${{target_name}}" PRIVATE cxx_std_20)
    target_compile_options("${{target_name}}" PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        "${{target_name}}"
        PRIVATE
            "${{REPO_ROOT}}/contracts/native/v0"
            "${{REPO_ROOT}}/src/native/common"
            "${{REPO_ROOT}}/src/native/runtime-core"
            "${{REPO_ROOT}}/src/native/runtime-core/gc"
            "${{REPO_ROOT}}/src/native/bootstrap"
            "${{REPO_ROOT}}/src/native/support"
            "${{REPO_ROOT}}/src/native/benchmark-host"
            "${{REPO_ROOT}}/src/native/proof-host"
            "${{REPO_ROOT}}/third_party/fmt/include"
            "${{REPO_ROOT}}/third_party/unordered_dense/include")
    chaos_apply_subject_variant("${{target_name}}")
endfunction()

add_subdirectory("{repo_root_text}/third_party/fmt" "fmt")
add_subdirectory("{repo_root_text}/third_party/unordered_dense" "unordered_dense")
add_subdirectory("{repo_root_text}/src/native/common" "common")
add_subdirectory("{repo_root_text}/src/native/diagnostics" "diagnostics")
add_subdirectory("{repo_root_text}/src/native/runtime-core" "runtime-core")
add_subdirectory("{repo_root_text}/src/native/hot-update" "hot-update")
add_subdirectory("{repo_root_text}/src/native/interpreter" "interpreter")
add_subdirectory("{repo_root_text}/src/native/jit" "jit")
add_subdirectory("{repo_root_text}/src/native/engine-bridge" "engine-bridge")
add_subdirectory("{repo_root_text}/src/native/support" "support")
add_subdirectory("{repo_root_text}/src/native/bootstrap" "bootstrap")
add_subdirectory(generated)
add_subdirectory({host_subdir})
"""


def _render_windows_native_aot_generated_cmakelists() -> str:
    return """if(NOT EXISTS "${CHAOS_SUBJECT_GENERATED_INPUT_SOURCE}")
    message(FATAL_ERROR "Missing generated subject source: ${CHAOS_SUBJECT_GENERATED_INPUT_SOURCE}")
endif()

add_library(chaos_subject_generated_native STATIC EXCLUDE_FROM_ALL
    "${CHAOS_SUBJECT_GENERATED_INPUT_SOURCE}")
chaos_configure_subject_target(chaos_subject_generated_native)
target_link_libraries(chaos_subject_generated_native PUBLIC chaos_runtime_core)

set_target_properties(
    chaos_subject_generated_native
    PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CHAOS_SUBJECT_BUILD_OUT_ROOT}"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CHAOS_SUBJECT_BUILD_OUT_ROOT}")
"""


def _render_cmake_source_list(paths: list[Path]) -> str:
    entries = [f'    "{path.as_posix()}"' for path in paths]
    if not entries:
        raise RuntimeError("generated source list cannot be empty")
    return "\n".join(entries)


def _generated_translation_unit_paths(
    output_root: Path,
    *,
    manifest_name: str,
    primary_generated_source_name: str,
) -> list[Path]:
    primary_generated_source_path = output_root / "generated" / primary_generated_source_name
    manifest_path = output_root / manifest_name
    generated_source_paths: list[Path] = []
    if primary_generated_source_path.is_file():
        generated_source_paths.append(primary_generated_source_path)
    if not manifest_path.is_file():
        return generated_source_paths or [primary_generated_source_path]

    manifest = read_json(manifest_path)
    if not isinstance(manifest, dict):
        return generated_source_paths or [primary_generated_source_path]

    discovered_paths: list[Path] = []
    for artifact in list(manifest.get("generatedArtifacts") or []):
        if not isinstance(artifact, dict):
            continue
        if str(artifact.get("kind") or "").strip() != "generatedTranslationUnit":
            continue
        artifact_path = str(artifact.get("path") or "").strip()
        if artifact_path:
            discovered_paths.append(output_root / artifact_path)

    if not discovered_paths:
        for page in list(manifest.get("translationUnitPages") or []):
            if not isinstance(page, dict):
                continue
            page_path = str(page.get("path") or "").strip()
            if page_path:
                discovered_paths.append(output_root / page_path)

    for path in discovered_paths:
        if path not in generated_source_paths:
            generated_source_paths.append(path)
    return generated_source_paths or [primary_generated_source_path]


def _generated_source_paths_from_manifest(generated_manifest: dict[str, Any]) -> list[str]:
    generated_source_paths = [
        str(value).strip()
        for value in list(generated_manifest.get("generatedSourcePaths") or [])
        if str(value).strip()
    ]
    if generated_source_paths:
        return generated_source_paths

    generated_source_path = str(generated_manifest.get("generatedSourcePath") or "").strip()
    return [generated_source_path] if generated_source_path else []


def _native_aot_entry_function_name(
    repo_root: Path,
    generated_manifest: dict[str, Any],
) -> str:
    native_entry_function_name = str(generated_manifest.get("nativeEntryFunctionName") or "").strip()
    if native_entry_function_name:
        return native_entry_function_name

    native_aot_plan_path_text = str(generated_manifest.get("nativeAotPlanPath") or "").strip()
    if native_aot_plan_path_text:
        native_aot_plan_path = _resolve(repo_root, native_aot_plan_path_text)
        if native_aot_plan_path.is_file():
            native_aot_plan = read_json(native_aot_plan_path)
            if isinstance(native_aot_plan, dict):
                native_entry_function_name = str(native_aot_plan.get("nativeEntryFunctionName") or "").strip()
                if native_entry_function_name:
                    return native_entry_function_name

    return "RunNativeAot"


def _generated_artifact_path(output_root: Path, manifest_name: str, artifact_kind: str) -> Path | None:
    manifest_path = output_root / manifest_name
    if not manifest_path.is_file():
        return None

    manifest = read_json(manifest_path)
    if not isinstance(manifest, dict):
        return None

    for artifact in list(manifest.get("generatedArtifacts") or []):
        if not isinstance(artifact, dict):
            continue
        if str(artifact.get("kind") or "").strip() != artifact_kind:
            continue
        artifact_path = str(artifact.get("path") or "").strip()
        if artifact_path:
            return output_root / artifact_path

    return None


def _slug_text(value: str) -> str:
    characters: list[str] = []
    previous_hyphen = False
    for character in str(value or "").strip().lower():
        if character.isalnum():
            characters.append(character)
            previous_hyphen = False
            continue
        if previous_hyphen:
            continue
        characters.append("-")
        previous_hyphen = True
    return "".join(characters).strip("-") or "supplemental"


def _normalized_supplemental_full_closure_requests(selection_source: dict[str, Any]) -> list[dict[str, str]]:
    normalized_requests: list[dict[str, str]] = []
    for item in list(selection_source.get("supplementalFullClosureAssemblies") or []):
        if not isinstance(item, dict):
            continue
        input_assembly_path = str(item.get("inputAssemblyPath") or "").strip()
        if not input_assembly_path:
            continue
        assembly_name = str(item.get("assemblyName") or Path(input_assembly_path).stem).strip()
        normalized_requests.append(
            {
                "assemblyName": assembly_name or Path(input_assembly_path).stem,
                "inputAssemblyPath": input_assembly_path,
                "slug": _slug_text(assembly_name or Path(input_assembly_path).stem),
            }
        )
    return normalized_requests


def _emit_native_generated_bundle(
    *,
    repo_root: Path,
    driver_dll_path: Path,
    driver_command: str,
    analysis_root: Path,
    output_root: Path,
    failure_label: str,
    generated_source_name: str,
    manifest_name: str,
    plan_name: str,
    manifest_key: str,
    plan_key: str,
) -> tuple[dict[str, Any], list[str]]:
    output_root.mkdir(parents=True, exist_ok=True)
    _run_checked(
        ["dotnet", str(driver_dll_path), driver_command, str(analysis_root), str(output_root)],
        repo_root=repo_root,
        failure_message=f"{failure_label} failed: {analysis_root}",
    )

    generated_source_paths = [
        _relative(repo_root, path)
        for path in _generated_translation_unit_paths(
            output_root,
            manifest_name=manifest_name,
            primary_generated_source_name=generated_source_name,
        )
    ]
    emitted_bundle: dict[str, Any] = {
        "generatedSourcePath": generated_source_paths[0],
        "generatedSourcePaths": generated_source_paths,
        manifest_key: _relative(repo_root, output_root / manifest_name),
        plan_key: _relative(repo_root, output_root / plan_name),
    }
    evidence_paths = [
        *list(emitted_bundle["generatedSourcePaths"]),
        str(emitted_bundle[manifest_key]),
    ]
    codegen_metrics_path = _generated_artifact_path(output_root, manifest_name, "codegenMetrics")
    if codegen_metrics_path is not None and codegen_metrics_path.is_file():
        emitted_bundle["codegenMetricsPath"] = _relative(repo_root, codegen_metrics_path)
        evidence_paths.append(emitted_bundle["codegenMetricsPath"])
    runtime_skeleton_coverage_path = _generated_artifact_path(output_root, manifest_name, "runtimeSkeletonCoverageReport")
    if runtime_skeleton_coverage_path is not None and runtime_skeleton_coverage_path.is_file():
        emitted_bundle["runtimeSkeletonCoverageReportPath"] = _relative(repo_root, runtime_skeleton_coverage_path)
        evidence_paths.append(emitted_bundle["runtimeSkeletonCoverageReportPath"])
    return emitted_bundle, evidence_paths


def _numeric_metric(payload: dict[str, Any], key: str) -> int | float | None:
    value = payload.get(key)
    if isinstance(value, bool) or not isinstance(value, (int, float)):
        return None
    return value


def _native_codegen_gate_metrics(codegen_metrics_payload: dict[str, Any]) -> dict[str, int | float]:
    metrics: dict[str, int | float] = {}
    for key in ("generatedCppTotalBytes", "generatedSymbolCount", "peakWorkingSetBytes"):
        numeric_value = _numeric_metric(codegen_metrics_payload, key)
        if numeric_value is not None:
            metrics[key] = numeric_value
    return metrics


def _render_windows_native_aot_host_cmakelists() -> str:
    return """if(NOT EXISTS "${CHAOS_SUBJECT_HOST_MAIN}")
    message(FATAL_ERROR "Missing native-aot host source: ${CHAOS_SUBJECT_HOST_MAIN}")
endif()

add_executable(chaos_subject_native_aot EXCLUDE_FROM_ALL
    "${CHAOS_SUBJECT_HOST_MAIN}")
chaos_configure_subject_target(chaos_subject_native_aot)
if(DEFINED CHAOS_SUBJECT_NATIVE_AOT_ENTRY_FUNCTION_NAME AND NOT CHAOS_SUBJECT_NATIVE_AOT_ENTRY_FUNCTION_NAME STREQUAL "")
    target_compile_definitions(
        chaos_subject_native_aot
        PRIVATE
            "CHAOS_NATIVE_AOT_ENTRY=${CHAOS_SUBJECT_NATIVE_AOT_ENTRY_FUNCTION_NAME}")
endif()
target_link_libraries(
    chaos_subject_native_aot
    PRIVATE
        chaos_subject_generated_native
        chaos_runtime_core
        chaos_hot_update
        chaos_bootstrap
        chaos_support)

set_target_properties(
    chaos_subject_native_aot
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CHAOS_SUBJECT_BUILD_OUT_ROOT}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CHAOS_SUBJECT_BUILD_OUT_ROOT}")
"""


def _resolve(repo_root: Path, relative_path: str) -> Path:
    return repo_root / Path(relative_path)


def _relative(repo_root: Path, path: Path) -> str:
    return path.relative_to(repo_root).as_posix()


def _selection_variant(selection: dict[str, Any]) -> str:
    return str(selection.get("variant") or "CHECK")


def _selection_runtime_arguments(selection: dict[str, Any]) -> list[str]:
    execution_context = dict(selection.get("executionContext") or {})
    return [str(value) for value in list(execution_context.get("runtimeArguments") or []) if str(value)]


def _selection_workload_entry(selection: dict[str, Any]) -> str:
    return str(selection.get("workloadEntry") or "")


def _declared_benchmark_has_reflection_contract(declared_benchmark: dict[str, Any] | None) -> bool:
    benchmark_meta = dict(declared_benchmark or {})
    return bool(
        str(benchmark_meta.get("assemblyName") or "").strip()
        and str(benchmark_meta.get("declaringType") or "").strip()
        and str(benchmark_meta.get("methodName") or "").strip()
    )


def _benchmark_resolution_kind(
    *,
    workload_entry: str,
    declared_benchmark: dict[str, Any] | None,
    host_kind: str = "",
    collection_path: str = "",
    entry_index: int | None = None,
) -> str:
    if host_kind == "benchmark-host" and collection_path and isinstance(entry_index, int) and entry_index >= 0:
        return "collection-entry"
    if _declared_benchmark_has_reflection_contract(declared_benchmark):
        return "declared-reflection"
    if workload_entry:
        return "legacy-workload-entry"
    return "unresolved"


def _selection_workspace_host_kind(selection: dict[str, Any]) -> str:
    entry_selection = _selection_declared_entry_selection(selection)
    family = str(entry_selection.get("family") or "")
    host_kind = {
        "declared-unit-test": "proof-host",
        "declared-benchmark": "benchmark-host",
    }.get(family, "")
    if host_kind:
        return host_kind

    execution_context = dict(selection.get("executionContext") or {})
    goal_id = str(selection.get("goalId") or "").strip().lower()
    matrix_id = str(selection.get("matrixId") or "").strip().lower()
    runtime_profile = str(execution_context.get("runtimeProfile") or "").strip().lower()
    toolchain_profile = str(execution_context.get("toolchainProfile") or "").strip().lower()
    selector_haystack = " ".join(
        value
        for value in (
            goal_id,
            matrix_id,
            runtime_profile,
            toolchain_profile,
        )
        if value
    )
    if "perf" in selector_haystack or "benchmark" in selector_haystack:
        return "benchmark-host"
    if _selection_uses_hotupdate_host(selection):
        return "proof-host"
    return ""


def _selection_native_aot_workspace_host_kind(selection: dict[str, Any]) -> str:
    host_kind = _selection_workspace_host_kind(selection)
    if host_kind:
        return host_kind
    if _selection_subject_entry_selection(selection):
        return "benchmark-host"
    return ""


def _declared_source_entry(entry: dict[str, Any]) -> str:
    assembly_name = str(entry.get("assemblyName") or "")
    declaring_type = str(entry.get("declaringType") or "")
    method_signature = str(entry.get("methodSignature") or "")
    if not assembly_name or not declaring_type or not method_signature:
        return ""

    type_name = declaring_type.rsplit(".", 1)[-1]
    return f"{assembly_name}/{type_name}::{method_signature}"


def _workload_assembly_name(workload_entry: str, *, assembly_name: str = "") -> str:
    explicit_assembly_name = str(assembly_name or "").strip()
    if explicit_assembly_name:
        return explicit_assembly_name
    return str(workload_entry or "").split("/", 1)[0].strip()


def _resolve_workload_assembly_path(
    repo_root: Path,
    host_input_manifest: dict[str, Any],
    workload_entry: str,
    *,
    assembly_name: str = "",
) -> tuple[str, Path | None]:
    candidate_paths = []
    primary_assembly_path_text = str(host_input_manifest.get("primaryAssemblyPath") or "")
    if primary_assembly_path_text:
        candidate_paths.append(primary_assembly_path_text)
    candidate_paths.extend(
        str(path)
        for path in list(host_input_manifest.get("additionalAssemblyPaths") or [])
        if str(path)
    )

    workload_assembly_name = _workload_assembly_name(workload_entry, assembly_name=assembly_name)
    for candidate_path_text in candidate_paths:
        candidate_path = _resolve(repo_root, candidate_path_text)
        if workload_assembly_name and candidate_path.stem.lower() == workload_assembly_name.lower():
            return candidate_path_text, candidate_path

    if primary_assembly_path_text:
        return primary_assembly_path_text, _resolve(repo_root, primary_assembly_path_text)

    return "", None


def _selection_subject_entry_selection(selection: dict[str, Any]) -> dict[str, int]:
    source = dict(selection.get("source") or {})
    payload = source.get("entrySelection")
    if payload is None:
        return {}
    if not isinstance(payload, dict):
        raise RuntimeError("selection.source.entrySelection must be an object")

    entry_kind = payload.get("entryKind")
    entry_slice = payload.get("entrySlice")
    if entry_kind is None and entry_slice is None:
        return {}
    if entry_kind is None or entry_slice is None:
        raise RuntimeError("selection.source.entrySelection requires both entryKind and entrySlice")
    if isinstance(entry_kind, bool) or not isinstance(entry_kind, int) or entry_kind < 0:
        raise RuntimeError("selection.source.entrySelection.entryKind must be a non-negative integer")
    if isinstance(entry_slice, bool) or not isinstance(entry_slice, int) or entry_slice < 0:
        raise RuntimeError("selection.source.entrySelection.entrySlice must be a non-negative integer")

    return {
        "entryKind": int(entry_kind),
        "entrySlice": int(entry_slice),
    }


def _selection_declared_entry_selection(selection: dict[str, Any]) -> dict[str, Any]:
    payload = selection.get("entrySelection")
    if payload is None:
        return {}
    if not isinstance(payload, dict):
        raise RuntimeError("selection.entrySelection must be an object")

    family = str(payload.get("family") or "").strip()
    if family not in {"declared-unit-test", "declared-benchmark"}:
        return {}

    normalized = {"family": family}
    stable_id = str(payload.get("stableId") or "").strip()
    alias = str(payload.get("alias") or "").strip()
    if stable_id:
        normalized["stableId"] = stable_id
    if alias:
        normalized["alias"] = alias
    entry_index = payload.get("entryIndex")
    if isinstance(entry_index, int) and not isinstance(entry_index, bool) and entry_index >= 0:
        normalized["entryIndex"] = int(entry_index)
    return normalized


def _load_project_workspace_module() -> Any:
    global _PROJECT_WORKSPACE_MODULE_CACHE
    if _PROJECT_WORKSPACE_MODULE_CACHE is not None:
        return _PROJECT_WORKSPACE_MODULE_CACHE

    spec = importlib.util.spec_from_file_location(
        "chaos_subject_project_workspace_runtime",
        _PROJECT_WORKSPACE_MODULE_PATH,
    )
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load project workspace module: {_PROJECT_WORKSPACE_MODULE_PATH}")

    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    _PROJECT_WORKSPACE_MODULE_CACHE = module
    return module


def _workspace_manifest_referenced_paths(manifest: dict[str, Any]) -> list[str]:
    referenced_paths: list[str] = []
    for collection_name in (
        "managedProjects",
        "managedTestProjects",
        "nativeProjects",
        "nativeTestProjects",
        "hotupdatePatchProjects",
        "hotupdateTestProjects",
    ):
        for item in list(manifest.get(collection_name) or []):
            if not isinstance(item, dict):
                continue
            for field_name in _WORKSPACE_MANIFEST_RELEVANT_KEYS:
                field_value = str(item.get(field_name) or "").strip()
                if field_value and field_value not in referenced_paths:
                    referenced_paths.append(field_value)
    return referenced_paths


def _workspace_manifest_missing_outputs(repo_root: Path, manifest: dict[str, Any]) -> bool:
    for relative_path in _workspace_manifest_referenced_paths(manifest):
        candidate_path = _resolve(repo_root, relative_path)
        if candidate_path.exists():
            continue
        return True
    return False


def _is_relevant_workspace_source_file(path: Path) -> bool:
    if not path.is_file():
        return False
    if path.suffix.lower() in {".cs", ".csproj", ".sln", ".props", ".targets"}:
        return True
    return path.name in {"Directory.Build.props", "Directory.Build.targets"}


def _load_subject_manifest_payload(repo_root: Path, subject_id: str) -> dict[str, Any] | None:
    manifest_path = verification_layout_module.owner_manifest_path(repo_root, subject_id)
    if not manifest_path.is_file():
        return None
    try:
        payload = json.loads(manifest_path.read_text(encoding="utf-8"))
    except (OSError, ValueError):
        return None
    return payload if isinstance(payload, dict) else None


def _workspace_generation_dependency_paths() -> list[Path]:
    testing_root = Path(__file__).resolve().parent
    run_root = testing_root.parent
    dependency_paths = [
        Path(__file__).resolve(),
        _PROJECT_WORKSPACE_MODULE_PATH,
        testing_root / "compiled_catalog.py",
        testing_root / "generated_managed_hosts.py",
        testing_root / "generated_hotupdate_hosts.py",
        testing_root / "template_assets.py",
    ]
    for templates_root in (
        testing_root / "templates",
        run_root / "subject" / "templates",
    ):
        if not templates_root.is_dir():
            continue
        dependency_paths.extend(candidate for candidate in sorted(templates_root.rglob("*")) if candidate.is_file())

    unique_paths: list[Path] = []
    seen_paths: set[str] = set()
    for candidate in dependency_paths:
        normalized = str(candidate.resolve())
        if normalized in seen_paths:
            continue
        seen_paths.add(normalized)
        unique_paths.append(candidate)
    return unique_paths


def _workspace_manifest_is_stale(
    repo_root: Path,
    *,
    subject_id: str,
    manifest_path: Path,
    manifest: dict[str, Any],
) -> bool:
    if _workspace_manifest_missing_outputs(repo_root, manifest):
        return True

    try:
        manifest_mtime = manifest_path.stat().st_mtime
    except OSError:
        return True

    subject_manifest_path = verification_layout_module.owner_manifest_path(repo_root, subject_id)
    try:
        if subject_manifest_path.is_file() and subject_manifest_path.stat().st_mtime > manifest_mtime:
            return True
    except OSError:
        return True

    # Workspace-managed hosts are generated from Python emitters and Scriban-like
    # text templates under build/toolchains/run. When those change, the workspace
    # has to be regenerated even if the subject source tree itself is untouched.
    for dependency_path in _workspace_generation_dependency_paths():
        try:
            if dependency_path.stat().st_mtime > manifest_mtime:
                return True
        except OSError:
            return True

    subject_manifest = _load_subject_manifest_payload(repo_root, subject_id)
    if subject_manifest is None:
        return False

    source = dict(subject_manifest.get("source") or {})
    source_path_text = str(source.get("path") or "").strip()
    if not source_path_text:
        return False

    source_path = _resolve(repo_root, source_path_text)
    source_root = verification_layout_module.owner_scan_root(repo_root, subject_id, source_path_text)
    if not source_root.is_dir():
        return False

    for candidate in source_root.rglob("*"):
        if not _is_relevant_workspace_source_file(candidate):
            continue
        try:
            if candidate.stat().st_mtime > manifest_mtime:
                return True
        except OSError:
            return True
    return False


def _ensure_subject_workspace_manifest(
    repo_root: Path,
    selection: dict[str, Any],
) -> tuple[Path, dict[str, Any]] | None:
    subject_id = str(selection.get("subjectId") or "").strip()
    if not subject_id:
        return None

    loaded_manifest = workspace_manifests_module.load_subject_workspace_manifest(repo_root, subject_id)
    loaded_manifest_fresh = False
    if loaded_manifest is not None:
        manifest_path, manifest = loaded_manifest
        loaded_manifest_fresh = not _workspace_manifest_is_stale(
            repo_root,
            subject_id=subject_id,
            manifest_path=manifest_path,
            manifest=manifest,
        )
        if loaded_manifest_fresh:
            return manifest_path, manifest

    # Workspace generation can execute host-input stages to refresh generated
    # sources. Those stages must not recursively request the same workspace
    # manifest again, or subject-exec loops indefinitely.
    if subject_id in _ACTIVE_WORKSPACE_MANIFEST_GENERATIONS:
        return loaded_manifest if loaded_manifest_fresh else None

    execution_context = dict(selection.get("executionContext") or {})
    host_platform = _normalize_host_platform(str(execution_context.get("hostPlatform") or ""))
    if not host_platform:
        return loaded_manifest

    project_workspace_module = _load_project_workspace_module()
    options: dict[str, Any] = {
        "id": f"subject/{subject_id}",
        "auto-refresh-missing-generated": True,
        "refresh-generated": True,
    }
    run_id = str(selection.get("runId") or "").strip()
    if run_id:
        options["generated-run-id"] = run_id
    matrix_id = str(selection.get("matrixId") or "").strip()
    if matrix_id:
        options["matrix"] = matrix_id
    else:
        options["all-targets"] = True
    variant = str(selection.get("variant") or "").strip()
    if variant:
        options["variant"] = variant
    entry_selection = _selection_declared_entry_selection(selection)
    if entry_selection and "matrix" in options:
        options["entry-selection"] = dict(entry_selection)
    _ACTIVE_WORKSPACE_MANIFEST_GENERATIONS.add(subject_id)
    try:
        project_workspace_module.generate_subject_workspace(
            repo_root,
            host_platform,
            options,
        )
    finally:
        _ACTIVE_WORKSPACE_MANIFEST_GENERATIONS.discard(subject_id)
    return workspace_manifests_module.load_subject_workspace_manifest(repo_root, subject_id)


def _resolve_workspace_managed_host(
    repo_root: Path,
    selection: dict[str, Any],
    *,
    host_kind_override: str = "",
) -> dict[str, str] | None:
    host_kind = str(host_kind_override or "").strip() or _selection_workspace_host_kind(selection)
    if not host_kind:
        return None

    subject_id = str(selection.get("subjectId") or "").strip()
    if not subject_id:
        return None

    loaded_manifest = _ensure_subject_workspace_manifest(repo_root, selection)
    if loaded_manifest is None:
        return None
    _, manifest = loaded_manifest

    managed_test_project = _resolve_native_benchmark_workspace_managed_host(manifest, selection)
    if managed_test_project is None:
        managed_test_project = workspace_manifests_module.find_managed_test_project(manifest, host_kind=host_kind)
    if managed_test_project is None:
        return None

    project_path = str(managed_test_project.get("projectPath") or "").strip()
    collection_path = str(managed_test_project.get("collectionPath") or "").strip()
    if not project_path or not collection_path:
        return None

    return {
        "projectId": str(managed_test_project.get("projectId") or "").strip(),
        "hostKind": host_kind,
        "projectPath": project_path,
        "collectionPath": collection_path,
        "executionModel": str(managed_test_project.get("executionModel") or "").strip(),
    }


def _resolve_native_benchmark_workspace_managed_host(
    manifest: dict[str, Any],
    selection: dict[str, Any],
) -> dict[str, Any] | None:
    entry_selection = _selection_declared_entry_selection(selection)
    if str(entry_selection.get("family") or "") != "declared-benchmark":
        return None

    matrix_id = str(selection.get("matrixId") or "").strip()
    if not matrix_id:
        return None

    native_test_project = workspace_manifests_module.find_native_test_project(
        manifest,
        matrix_id=matrix_id,
        host_kind="benchmark-host",
    )
    if native_test_project is None:
        return None

    managed_test_project_id = str(native_test_project.get("managedTestProjectId") or "").strip()
    if not managed_test_project_id:
        return None

    if hasattr(workspace_manifests_module, "find_managed_test_project_by_id"):
        managed_test_project = workspace_manifests_module.find_managed_test_project_by_id(
            manifest,
            project_id=managed_test_project_id,
        )
        if managed_test_project is not None:
            return managed_test_project

    for item in list(manifest.get("managedTestProjects") or []):
        if not isinstance(item, dict):
            continue
        if str(item.get("projectId") or "").strip() == managed_test_project_id:
            return dict(item)
    return None


def _selection_uses_hotupdate_host(selection: dict[str, Any]) -> bool:
    engineering_profile = str(selection.get("engineeringProfile") or "").strip().lower()
    if engineering_profile == subjects_module.EngineeringProfile.HOT_UPDATE_HOST.value:
        return True

    execution_context = dict(selection.get("executionContext") or {})
    for field_name in ("runtimeProfile", "toolchainProfile"):
        normalized = str(execution_context.get(field_name) or "").strip().lower()
        if "hot-update" in normalized or "hotupdate" in normalized:
            return True
    return False


def _selection_uses_native_hotupdate_chain(selection: dict[str, Any]) -> bool:
    execution_context = dict(selection.get("executionContext") or {})
    source = dict(selection.get("source") or {})
    runtime_profile = str(execution_context.get("runtimeProfile") or "").strip().lower()
    toolchain_profile = str(execution_context.get("toolchainProfile") or "").strip().lower()
    return bool(
        _selection_uses_hotupdate_host(selection)
        and bool(source.get("fullAssemblyClosure"))
        and runtime_profile == "native-hotupdate-proof-output"
        and toolchain_profile != "dotnet-managed"
    )


def _resolve_workspace_hotupdate_test_host(
    repo_root: Path,
    selection: dict[str, Any],
) -> dict[str, str] | None:
    if not _selection_uses_hotupdate_host(selection):
        return None

    host_kind = _selection_workspace_host_kind(selection)
    if not host_kind:
        return None

    subject_id = str(selection.get("subjectId") or "").strip()
    if not subject_id:
        return None

    loaded_manifest = _ensure_subject_workspace_manifest(repo_root, selection)
    if loaded_manifest is None:
        return None
    _, manifest = loaded_manifest

    hotupdate_test_project = workspace_manifests_module.find_hotupdate_test_project(
        manifest,
        host_kind=host_kind,
    )
    if hotupdate_test_project is None:
        return None

    project_path = str(hotupdate_test_project.get("projectPath") or "").strip()
    collection_path = str(hotupdate_test_project.get("collectionPath") or "").strip()
    binding_manifest_path = str(hotupdate_test_project.get("bindingManifestPath") or "").strip()
    if not project_path or not collection_path or not binding_manifest_path:
        return None

    return {
        "projectId": str(hotupdate_test_project.get("projectId") or "").strip(),
        "hostKind": host_kind,
        "projectPath": project_path,
        "collectionPath": collection_path,
        "bindingManifestPath": binding_manifest_path,
    }


def _resolve_workspace_hotupdate_patch_projects(
    repo_root: Path,
    selection: dict[str, Any],
) -> list[dict[str, str]]:
    if not _selection_uses_hotupdate_host(selection):
        return []

    subject_id = str(selection.get("subjectId") or "").strip()
    if not subject_id:
        return []

    loaded_manifest = _ensure_subject_workspace_manifest(repo_root, selection)
    if loaded_manifest is None:
        return []
    _, manifest = loaded_manifest

    matrix_id = str(selection.get("matrixId") or "").strip()
    records = workspace_manifests_module.find_hotupdate_patch_projects(
        manifest,
        matrix_id=matrix_id,
    )
    return [
        {
            "projectId": str(record.get("projectId") or "").strip(),
            "projectPath": str(record.get("projectPath") or "").strip(),
            "assemblyName": str(record.get("assemblyName") or "").strip(),
        }
        for record in records
        if str(record.get("projectPath") or "").strip()
    ]


def _resolve_workspace_native_test_host(
    repo_root: Path,
    selection: dict[str, Any],
    *,
    host_kind_override: str = "",
) -> dict[str, str] | None:
    host_kind = str(host_kind_override or "").strip() or _selection_workspace_host_kind(selection)
    if not host_kind:
        return None

    subject_id = str(selection.get("subjectId") or "").strip()
    matrix_id = str(selection.get("matrixId") or "").strip()
    if not subject_id or not matrix_id:
        return None

    loaded_manifest = _ensure_subject_workspace_manifest(repo_root, selection)
    if loaded_manifest is None:
        return None
    _, manifest = loaded_manifest

    native_test_project = workspace_manifests_module.find_native_test_project(
        manifest,
        matrix_id=matrix_id,
        host_kind=host_kind,
    )
    if native_test_project is None:
        return None

    return {
        "projectId": str(native_test_project.get("projectId") or "").strip(),
        "projectPath": str(native_test_project.get("projectPath") or "").strip(),
        "configureRoot": str(native_test_project.get("configureRoot") or "").strip(),
        "hostKind": host_kind,
        "managedTestProjectId": str(native_test_project.get("managedTestProjectId") or "").strip(),
    }


def _load_subject_declared_collection(
    repo_root: Path,
    *,
    subject_id: str,
    host_kind: str,
) -> dict[str, Any] | None:
    workspace_collection = workspace_declared_collection_module.load_workspace_declared_collection(
        repo_root,
        subject_id,
        host_kind=host_kind,
    )
    if isinstance(workspace_collection, dict):
        return workspace_collection

    try:
        collection = compiled_catalog_module.build_subject_declared_test_catalog(
            repo_root=repo_root,
            subject_id=subject_id,
            force_build=False,
        )
    except Exception:
        return None

    return dict(collection) if isinstance(collection, dict) else None


def _find_declared_collection_entry(
    collection: dict[str, Any],
    *,
    family: str,
    entry_selection: dict[str, Any],
) -> dict[str, Any] | None:
    collection_key = "declaredUnitTests" if family == "declared-unit-test" else "declaredBenchmarks"
    entries = [dict(item) for item in list(collection.get(collection_key) or []) if isinstance(item, dict)]

    entry_index = entry_selection.get("entryIndex")
    if isinstance(entry_index, int) and not isinstance(entry_index, bool) and entry_index >= 0:
        for entry in entries:
            candidate_index = entry.get("entryIndex")
            if isinstance(candidate_index, int) and not isinstance(candidate_index, bool) and candidate_index == entry_index:
                return entry

    stable_id = str(entry_selection.get("stableId") or "").strip()
    if stable_id:
        for entry in entries:
            if str(entry.get("stableId") or "").strip() == stable_id:
                return entry

    alias = str(entry_selection.get("alias") or "").strip()
    if alias:
        for entry in entries:
            if str(entry.get("alias") or "").strip() == alias:
                return entry

    return None


def _resolve_declared_benchmark(selection: dict[str, Any], *, repo_root: Path, subject_id: str) -> dict[str, Any]:
    entry_selection = _selection_declared_entry_selection(selection)
    if str(entry_selection.get("family") or "") != "declared-benchmark":
        return {}

    collection = _load_subject_declared_collection(
        repo_root,
        subject_id=subject_id,
        host_kind="benchmark-host",
    )
    if not isinstance(collection, dict):
        return {}

    entry = _find_declared_collection_entry(
        collection,
        family="declared-benchmark",
        entry_selection=entry_selection,
    )
    if entry is None:
        return {}

    payload = {
        "stableId": str(entry.get("stableId") or entry_selection.get("stableId") or ""),
        "alias": str(entry.get("alias") or entry_selection.get("alias") or ""),
        "assemblyName": str(entry.get("assemblyName") or ""),
        "declaringType": str(entry.get("declaringType") or ""),
        "methodName": str(entry.get("methodName") or ""),
        "methodSignature": str(entry.get("methodSignature") or ""),
    }
    entry_index = entry.get("entryIndex")
    if isinstance(entry_index, int) and not isinstance(entry_index, bool) and entry_index >= 0:
        payload["entryIndex"] = int(entry_index)

    workload_entry = _declared_source_entry(entry)
    if workload_entry:
        payload["workloadEntry"] = workload_entry

    return payload


def _selection_managed_runtime_arguments(selection: dict[str, Any]) -> list[str]:
    runtime_arguments = _selection_runtime_arguments(selection)
    subject_entry_selection = _selection_subject_entry_selection(selection)
    if subject_entry_selection:
        runtime_arguments.extend(
            [
                f"{CHAOS_ENTRY_KIND_ARGUMENT_PREFIX}{subject_entry_selection['entryKind']}",
                f"{CHAOS_ENTRY_SLICE_ARGUMENT_PREFIX}{subject_entry_selection['entrySlice']}",
            ]
        )
    return runtime_arguments


def _android_runtime_environment_exports(runtime_arguments: list[str]) -> list[str]:
    assignments: list[str] = []
    for argument in runtime_arguments:
        for prefix, environment_name in ANDROID_RUNTIME_ARGUMENT_ENVIRONMENTS.items():
            if not argument.startswith(prefix):
                continue

            value = argument[len(prefix):]
            if value:
                assignments.append(f"export {environment_name}={shlex.quote(value)} >/dev/null")
            break
    return assignments


def _variant_macros(variant: str) -> dict[str, list[str]]:
    if variant not in VARIANT_MACROS:
        raise RuntimeError(f"unsupported subject variant: {variant}")
    return dict(VARIANT_MACROS[variant])


def _trace_platform(selection: dict[str, Any]) -> str:
    execution_context = dict(selection.get("executionContext") or {})
    target_platform = str(execution_context.get("targetPlatform") or execution_context.get("hostPlatform") or "")
    if target_platform.startswith("windows"):
        return "windows"
    if target_platform.startswith("macos"):
        return "macos"
    raise RuntimeError(f"unsupported trace platform: {target_platform}")


def _trace_snapshot_path(trace_platform: str) -> Path:
    if trace_platform == "windows":
        return WINDOWS_TRACE_SNAPSHOT_PATH
    if trace_platform == "macos":
        return MACOS_TRACE_SNAPSHOT_PATH
    raise RuntimeError(f"unsupported trace platform: {trace_platform}")


def _run_checked(
    arguments: list[str],
    *,
    repo_root: Path,
    failure_message: str,
    env: dict[str, str] | None = None,
) -> str:
    completed = run_process(arguments, cwd=repo_root, env=env)
    output = combine_process_output(completed)
    if completed.returncode != 0:
        raise RuntimeError(f"{failure_message}\n{output}".strip())
    return output


def _windows_visual_studio_generator(repo_root: Path) -> str:
    cmake_path = tooling_module.find_cmake_executable(repo_root) or "cmake"
    return tooling_module.detect_visual_studio_generator(cmake_path) or "Visual Studio 17 2022"


def _windows_native_cmake_context(repo_root: Path) -> tuple[str, dict[str, str] | None, str | None]:
    cmake_path, _cmake_env = tooling_module.cmake_environment(repo_root)
    developer_env = tooling_module.windows_developer_environment()
    ninja_path = tooling_module.find_ninja_executable()
    return cmake_path or "cmake", developer_env or None, ninja_path


def _materialize_windows_native_aot_cmake_source_at(
    repo_root: Path,
    *,
    source_root: Path,
    host_kind: str,
) -> Path:
    host_subdir = "proof" if host_kind == "proof-host" else "benchmark"
    if source_root.exists():
        shutil.rmtree(source_root)
    (source_root / "generated").mkdir(parents=True, exist_ok=True)
    (source_root / host_subdir).mkdir(parents=True, exist_ok=True)
    (source_root / "CMakeLists.txt").write_text(
        _render_windows_native_aot_workspace_cmakelists(repo_root, host_subdir=host_subdir),
        encoding="utf-8",
    )
    (source_root / "generated" / "CMakeLists.txt").write_text(
        _render_windows_native_aot_generated_cmakelists(),
        encoding="utf-8",
    )
    (source_root / host_subdir / "CMakeLists.txt").write_text(
        _render_windows_native_aot_host_cmakelists(),
        encoding="utf-8",
    )
    return source_root


def _materialize_windows_native_aot_cmake_source(
    repo_root: Path,
    *,
    build_root: Path,
    host_kind: str,
) -> Path:
    return _materialize_windows_native_aot_cmake_source_at(
        repo_root,
        source_root=build_root / "cmake-src",
        host_kind=host_kind,
    )


def _materialize_workspace_windows_native_aot_cmake_source(
    repo_root: Path,
    *,
    subject_id: str,
    matrix_id: str,
    host_kind: str,
) -> Path:
    workspace_source_root = verification_layout_module.subject_workspace_root(repo_root, subject_id) / "native-source" / matrix_id
    return _materialize_windows_native_aot_cmake_source_at(
        repo_root,
        source_root=workspace_source_root,
        host_kind=host_kind,
    )


def _resolve_windows_native_aot_cmake_layout(
    repo_root: Path,
    *,
    selection: dict[str, Any],
    build_root: Path,
    workspace_native_test_host: dict[str, str],
    generator: str,
    host_kind: str,
) -> tuple[Path, Path]:
    configure_root_text = str(workspace_native_test_host.get("configureRoot") or "").strip()
    subject_id = str(selection.get("subjectId") or "").strip()
    matrix_id = str(selection.get("matrixId") or "").strip()
    if configure_root_text and subject_id and matrix_id:
        cmake_source_root = _materialize_workspace_windows_native_aot_cmake_source(
            repo_root,
            subject_id=subject_id,
            matrix_id=matrix_id,
            host_kind=host_kind,
        )
        cmake_binary_dir = _resolve(repo_root, configure_root_text)
        cmake_binary_dir.parent.mkdir(parents=True, exist_ok=True)
        return cmake_source_root, cmake_binary_dir

    return (
        _materialize_windows_native_aot_cmake_source(repo_root, build_root=build_root, host_kind=host_kind),
        tooling_module.allocate_cmake_binary_dir(
            build_root / "cmake",
            host_platform="windows",
            generator=generator,
        ),
    )


def _materialize_windows_native_reference_cmake_source(
    repo_root: Path,
    *,
    build_root: Path,
    subject_id: str,
    generated_source_paths: list[Path],
) -> Path:
    del repo_root

    source_root = build_root / "cmake-src"
    return _materialize_windows_native_reference_cmake_source_at(
        source_root,
        subject_id=subject_id,
        generated_source_paths=generated_source_paths,
    )


def _materialize_windows_native_reference_cmake_source_at(
    source_root: Path,
    *,
    subject_id: str,
    generated_source_paths: list[Path],
) -> Path:
    if source_root.exists():
        shutil.rmtree(source_root)
    (source_root / "generated").mkdir(parents=True, exist_ok=True)
    (source_root / "proof").mkdir(parents=True, exist_ok=True)
    if not generated_source_paths:
        raise RuntimeError("generated source paths are required for native-reference CMake source")
    generated_source_path = generated_source_paths[0]

    proof_main_path = source_root / "proof" / "main.cpp"
    (source_root / "CMakeLists.txt").write_text(
        template_assets_module.read_template(
            owner_file=_SUBJECT_TEMPLATE_OWNER_FILE,
            relative_template_path=_NATIVE_REFERENCE_WORKSPACE_TEMPLATE,
        ),
        encoding="utf-8",
    )
    (source_root / "generated" / "CMakeLists.txt").write_text(
        template_assets_module.render_template(
            owner_file=_SUBJECT_TEMPLATE_OWNER_FILE,
            relative_template_path=_NATIVE_GENERATED_TEMPLATE,
            replacements={
                "GENERATED_INPUT_SOURCES": _render_cmake_source_list(generated_source_paths),
            },
        ),
        encoding="utf-8",
    )
    native_reference_manifest_path = generated_source_path.parent.parent / "native-reference.manifest.json"
    runtime_execution_kind = ""
    assembly_dispatch_subject_id = ""
    if native_reference_manifest_path.is_file():
        native_reference_manifest = read_json(native_reference_manifest_path)
        if isinstance(native_reference_manifest, dict):
            runtime_execution_kind = str(native_reference_manifest.get("runtimeExecutionKind") or "").strip()
            assembly_dispatch_subject_id = str(
                native_reference_manifest.get("preferredAssemblyDispatchSubjectId") or ""
            ).strip()
            translation_unit_pages = [
                dict(item)
                for item in list(native_reference_manifest.get("translationUnitPages") or [])
                if isinstance(item, dict)
            ]
            if not assembly_dispatch_subject_id and translation_unit_pages:
                assembly_dispatch_subject_id = str(translation_unit_pages[0].get("firstMethodSubjectId") or "").strip()

    use_assembly_bound_dispatch = (
        runtime_execution_kind == "assembly-bound-native-reference-skeleton"
        and bool(assembly_dispatch_subject_id)
    )
    proof_main_path.write_text(
        template_assets_module.render_template(
            owner_file=_SUBJECT_TEMPLATE_OWNER_FILE,
            relative_template_path=_NATIVE_PROOF_MAIN_TEMPLATE,
            replacements={
                "IMAGE_NAME": subject_id,
                "RUNTIME_TAG": "subject-reference-proof",
                "USE_ASSEMBLY_BOUND_DISPATCH": "true" if use_assembly_bound_dispatch else "false",
                "ASSEMBLY_DISPATCH_SUBJECT_ID": assembly_dispatch_subject_id,
            },
        ),
        encoding="utf-8",
    )
    (source_root / "proof" / "CMakeLists.txt").write_text(
        template_assets_module.render_template(
            owner_file=_SUBJECT_TEMPLATE_OWNER_FILE,
            relative_template_path=_NATIVE_PROOF_TEMPLATE,
            replacements={
                "PROOF_MAIN": proof_main_path.as_posix(),
            },
        ),
        encoding="utf-8",
    )
    (source_root / "proof" / "RunSubjectProof.cmake").write_text(
        template_assets_module.read_template(
            owner_file=_SUBJECT_TEMPLATE_OWNER_FILE,
            relative_template_path=_NATIVE_REFERENCE_RUN_SCRIPT_TEMPLATE,
        ),
        encoding="utf-8",
    )
    return source_root


def _materialize_workspace_windows_native_reference_cmake_source(
    repo_root: Path,
    *,
    subject_id: str,
    matrix_id: str,
    generated_source_paths: list[Path],
) -> Path:
    workspace_source_root = verification_layout_module.subject_workspace_root(repo_root, subject_id) / "native-source" / matrix_id
    return _materialize_windows_native_reference_cmake_source_at(
        workspace_source_root,
        subject_id=subject_id,
        generated_source_paths=generated_source_paths,
    )


def _resolve_windows_native_reference_cmake_layout(
    repo_root: Path,
    *,
    selection: dict[str, Any],
    build_root: Path,
    workspace_native_test_host: dict[str, str],
    generator: str,
    generated_source_paths: list[Path],
) -> tuple[Path, Path]:
    configure_root_text = str(workspace_native_test_host.get("configureRoot") or "").strip()
    subject_id = str(selection.get("subjectId") or "").strip()
    matrix_id = str(selection.get("matrixId") or "").strip()
    if configure_root_text and subject_id and matrix_id:
        cmake_source_root = _materialize_workspace_windows_native_reference_cmake_source(
            repo_root,
            subject_id=subject_id,
            matrix_id=matrix_id,
            generated_source_paths=generated_source_paths,
        )
        cmake_binary_dir = _resolve(repo_root, configure_root_text)
        cmake_binary_dir.parent.mkdir(parents=True, exist_ok=True)
        return cmake_source_root, cmake_binary_dir

    return (
        _materialize_windows_native_reference_cmake_source(
            repo_root,
            build_root=build_root,
            subject_id=subject_id,
            generated_source_paths=generated_source_paths,
        ),
        tooling_module.allocate_cmake_binary_dir(
            build_root / "cmake",
            host_platform="windows",
            generator=generator,
        ),
    )


def _native_benchmark_dispatch_manifest_path(build_root: Path) -> Path:
    return build_root / "benchmark.dispatch.manifest.json"


def _write_native_benchmark_dispatch_manifest(
    repo_root: Path,
    *,
    build_root: Path,
    selection: dict[str, Any],
    collection_path: str,
    entry_selection: dict[str, Any],
    native_entry_function_name: str = "RunNativeAot",
) -> str:
    dispatch_manifest_path = _native_benchmark_dispatch_manifest_path(build_root)
    write_json(
        dispatch_manifest_path,
        {
            "subjectId": str(selection.get("subjectId") or ""),
            "matrixId": str(selection.get("matrixId") or ""),
            "hostKind": "benchmark-host",
            "collectionPath": collection_path,
            "nativeEntryFunctionName": native_entry_function_name or "RunNativeAot",
            "entrySelection": dict(entry_selection),
        },
    )
    return _relative(repo_root, dispatch_manifest_path)


def _normalize_host_platform(host_platform: str) -> str:
    return "windows" if host_platform.startswith("windows") else host_platform


def _windows_variant_build_flags(variant: str) -> tuple[list[str], list[str]]:
    if variant == "CHECK":
        return ["/Od", "/Zi"], []
    if variant == "PROFILE":
        return ["/O2", "/DNDEBUG"], []
    if variant == "SHIP":
        return ["/O2", "/GL", "/DNDEBUG"], ["/LTCG"]
    raise RuntimeError(f"unsupported subject variant: {variant}")


def _subject_mobile_host_root(repo_root: Path, subject_id: str, target_platform: str) -> Path:
    if target_platform == "android-arm64":
        candidate = verification_layout_module.owner_evidence_root(repo_root, subject_id) / "validation" / "mobile" / "android-host"
        fallback = repo_root / "tests" / "gate" / "android-smoke"
    elif target_platform == "ios-arm64":
        candidate = verification_layout_module.owner_evidence_root(repo_root, subject_id) / "validation" / "mobile" / "ios-host"
        fallback = repo_root / "tests" / "gate" / "ios-smoke"
    else:
        raise RuntimeError(f"unsupported mobile target platform: {target_platform}")

    return candidate if candidate.is_dir() else fallback


def _dotnet_intermediate_args(project_name: str, host_platform: str) -> list[str]:
    normalized_host = _normalize_host_platform(host_platform)
    intermediate_root = tooling_module.allocate_dotnet_intermediate_dir(project_name, host_platform=normalized_host)
    if intermediate_root is None:
        return []

    return [f"-p:ChaosTempIntermediateRoot={intermediate_root.as_posix()}/"]


def _success_result(
    *,
    bucket_manifest_path: str,
    report_paths: list[str],
    primary_evidence_paths: list[str],
    stdout_path: str | None = None,
    stderr_path: str | None = None,
    duration_ms: int = 0,
    details: dict[str, Any] | None = None,
) -> dict[str, Any]:
    return {
        "status": "ok",
        "bucketManifestPath": bucket_manifest_path,
        "reportPaths": report_paths,
        "primaryEvidencePaths": primary_evidence_paths,
        "metrics": {"durationMs": duration_ms},
        "diagnostics": {"stdoutPath": stdout_path, "stderrPath": stderr_path},
        "details": dict(details or {}),
        "failure": None,
    }


def _engine_profile(repo_root: Path, subject_id: str) -> dict[str, Any]:
    manifest = subjects_module.load_subject_manifest(repo_root, subject_id)
    return dict(manifest.get("engineProofProfile") or {})


def _engine_contract_summary(profile: dict[str, Any]) -> dict[str, Any]:
    if not profile:
        return {}

    focus_area = str(profile.get("focusArea") or "")
    engine_binding_kinds = [str(value) for value in list(profile.get("engineBindingKinds") or []) if str(value)]
    if not engine_binding_kinds and focus_area:
        engine_binding_kinds = [focus_area]

    host_binding_kinds = [str(value) for value in list(profile.get("hostBindingKinds") or []) if str(value)]
    if not host_binding_kinds:
        host_binding_kinds = ["artifact-observe-contract"]

    return {
        "proofKind": str(profile.get("proofKind") or "engine-binding"),
        "focusArea": focus_area,
        "resolvedCapabilityIds": [
            str(value)
            for value in list(profile.get("expectedCapabilityIds") or [])
            if str(value)
        ],
        "engineBindingKinds": engine_binding_kinds,
        "hostBindingKinds": host_binding_kinds,
        "expectedEvidenceKinds": [
            str(value)
            for value in list(profile.get("expectedEvidenceKinds") or [])
            if str(value)
        ],
    }


def _engine_lowering_bindings(lowering_plan: dict[str, Any]) -> tuple[dict[str, Any], dict[str, Any]]:
    engine_bindings = dict(lowering_plan.get("engineBindings") or lowering_plan.get("EngineBindings") or {})
    host_bindings = dict(lowering_plan.get("hostBindings") or lowering_plan.get("HostBindings") or {})
    return engine_bindings, host_bindings


def _engine_emission_summary(
    lowering_plan: dict[str, Any],
    *,
    generated_source_path: str,
    native_reference_manifest_path: str,
) -> dict[str, Any]:
    engine_bindings, host_bindings = _engine_lowering_bindings(lowering_plan)
    if not engine_bindings:
        return {}

    return {
        "proofKind": str(engine_bindings.get("proofKind") or "engine-binding"),
        "focusArea": str(engine_bindings.get("focusArea") or ""),
        "emittedCapabilityIds": [
            str(value)
            for value in list(engine_bindings.get("capabilityIds") or [])
            if str(value)
        ],
        "engineBindingKinds": [
            str(value)
            for value in list(engine_bindings.get("bindingKinds") or [])
            if str(value)
        ],
        "hostBindingKinds": [
            str(value)
            for value in list(host_bindings.get("bindingKinds") or [])
            if str(value)
        ],
        "bridgeArtifactPaths": [generated_source_path],
        "registrationArtifactPaths": [native_reference_manifest_path],
    }


def _parse_engine_observations(stdout_text: str) -> list[dict[str, Any]]:
    observations: list[dict[str, Any]] = []
    for line in stdout_text.splitlines():
        if not line.startswith(ENGINE_OBSERVE_PREFIX):
            continue

        payload_text = line[len(ENGINE_OBSERVE_PREFIX) :].strip()
        if not payload_text:
            continue

        try:
            payload = json.loads(payload_text)
        except ValueError:
            continue

        if isinstance(payload, dict):
            observations.append(payload)
    return observations


def run_source_resolve(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    source = dict(request["selection"]["source"])
    selection = dict(request["selection"])
    source_path = _resolve(repo_root, str(source["path"]))
    source_root = source_path.parent
    inputs = sorted(
        _relative(repo_root, candidate)
        for candidate in source_root.rglob("*")
        if candidate.is_file()
    )
    manifest = {
        "subjectId": str(request["selection"]["subjectId"]),
        "bucket": "source",
        "sourceType": str(source["type"]),
        "sourcePath": str(source["path"]),
        "entry": str(source["entry"]),
        "validationProfileId": selection.get("validationProfileId"),
        "validationKind": selection.get("validationKind"),
        "variant": _selection_variant(selection),
        "inputs": inputs,
    }
    write_json(_resolve(repo_root, request["paths"]["manifestPath"]), manifest)
    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=[],
        primary_evidence_paths=[str(source["path"])],
    )


def run_dotnet_host_input_builder(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    source = dict(request["selection"]["source"])
    selection = dict(request["selection"])
    output_root = _resolve(repo_root, request["paths"]["bucketRoot"])
    output_root.mkdir(parents=True, exist_ok=True)
    host_platform = str(request["selection"]["executionContext"]["hostPlatform"])
    hotupdate_host = _resolve_workspace_hotupdate_test_host(repo_root, selection)
    hotupdate_patch_projects = _resolve_workspace_hotupdate_patch_projects(repo_root, selection)
    managed_host = _resolve_workspace_managed_host(repo_root, selection)
    source_primary_project_path_text = subjects_module.resolve_source_primary_project_path(source)
    native_primary_project_path_text = source_primary_project_path_text
    host_execution_model = str(managed_host.get("executionModel") or "").strip() if managed_host is not None else ""
    use_shared_runtime_host = host_execution_model == SHARED_RUNTIME_HOST_EXECUTION_MODEL
    use_native_hotupdate_chain = bool(
        hotupdate_host is not None
        and bool(source.get("fullAssemblyClosure"))
        and str(selection.get("matrixId") or "").strip()
        and str(dict(selection.get("executionContext") or {}).get("toolchainProfile") or "").strip().lower() != "dotnet-managed"
    )
    primary_project_path_text = (
        native_primary_project_path_text
        if use_native_hotupdate_chain
        else (
            str(hotupdate_host["projectPath"])
            if hotupdate_host is not None
            else (
                str(managed_host["projectPath"])
                if managed_host is not None
                else source_primary_project_path_text
            )
        )
    )
    primary_project_path = _resolve(repo_root, primary_project_path_text)
    build_project_path_texts = [primary_project_path_text]
    if use_shared_runtime_host:
        build_project_path_texts = [source_primary_project_path_text, primary_project_path_text]

    built_project_paths: set[str] = set()
    for build_project_path_text in build_project_path_texts:
        if not build_project_path_text or build_project_path_text in built_project_paths:
            continue
        built_project_paths.add(build_project_path_text)
        _run_checked(
            [
                "dotnet",
                "build",
                str(_resolve(repo_root, build_project_path_text)),
                "-c",
                "Release",
                "-m:1",
                "-o",
                str(output_root),
                *_dotnet_intermediate_args(str(request["selection"]["subjectId"]), host_platform),
            ],
            repo_root=repo_root,
            failure_message=f"dotnet build failed: {build_project_path_text}",
        )

    if hotupdate_host is not None:
        supporting_project_paths = [
            *(
                [str(hotupdate_host.get("projectPath") or "")]
                if use_native_hotupdate_chain and str(hotupdate_host.get("projectPath") or "").strip()
                else []
            ),
            *[
                str(item.get("projectPath") or "")
                for item in hotupdate_patch_projects
                if str(item.get("projectPath") or "")
            ],
        ]
        built_project_paths: set[str] = {primary_project_path_text}
        for supporting_project_path_text in supporting_project_paths:
            if not supporting_project_path_text or supporting_project_path_text in built_project_paths:
                continue
            built_project_paths.add(supporting_project_path_text)
            supporting_project_path = _resolve(repo_root, supporting_project_path_text)
            _run_checked(
                [
                    "dotnet",
                    "build",
                    str(supporting_project_path),
                    "-c",
                    "Release",
                    "-m:1",
                    "-o",
                    str(output_root),
                    *_dotnet_intermediate_args(str(request["selection"]["subjectId"]), host_platform),
                ],
                repo_root=repo_root,
                failure_message=f"dotnet build failed: {supporting_project_path_text}",
            )

    subject_id = str(request["selection"]["subjectId"])
    primary_assembly_path = output_root / f"{primary_project_path.stem}.dll"
    solution_assembly_names = set(subjects_module.resolve_source_solution_assembly_names(repo_root, source))
    if not solution_assembly_names:
        solution_assembly_names.add(Path(subjects_module.resolve_source_primary_project_path(source)).stem)
    additional_assembly_paths = [
        _relative(repo_root, candidate)
        for candidate in sorted(output_root.iterdir())
        if candidate.is_file()
        and candidate.suffix.lower() == ".dll"
        and not candidate.samefile(primary_assembly_path)
        and candidate.stem in solution_assembly_names
    ]
    files = [
        _relative(repo_root, candidate)
        for candidate in sorted(output_root.iterdir())
        if candidate.is_file()
        and (
            candidate.suffix.lower() in {".dll", ".pdb", ".exe"}
            or candidate.name.lower().endswith(".deps.json")
            or candidate.name.lower().endswith(".runtimeconfig.json")
        )
    ]
    manifest = {
        "subjectId": subject_id,
        "bucket": "host-input",
        "sourceManifestPath": str(request["upstream"]["source"]["manifestPath"]),
        "primaryProjectPath": primary_project_path_text,
        "primaryAssemblyPath": _relative(repo_root, primary_assembly_path),
        "additionalAssemblyPaths": additional_assembly_paths,
        "variant": _selection_variant(selection),
        "files": files,
    }
    selected_host = hotupdate_host or managed_host
    if selected_host is not None:
        manifest["hostKind"] = str(selected_host["hostKind"])
        manifest["collectionPath"] = str(selected_host["collectionPath"])
        host_execution_model = str(selected_host.get("executionModel") or "").strip()
        if host_execution_model:
            manifest["hostExecutionModel"] = host_execution_model
    if hotupdate_host is not None:
        manifest["bindingManifestPath"] = str(hotupdate_host["bindingManifestPath"])
    if use_native_hotupdate_chain and hotupdate_host is not None:
        managed_runtime_project_path_text = str(hotupdate_host.get("projectPath") or "").strip()
        if managed_runtime_project_path_text:
            manifest["managedRuntimeProjectPath"] = managed_runtime_project_path_text
            manifest["managedRuntimeAssemblyPath"] = _relative(
                repo_root,
                output_root / f"{Path(managed_runtime_project_path_text).stem}.dll",
            )
    write_json(_resolve(repo_root, request["paths"]["manifestPath"]), manifest)
    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=[],
        primary_evidence_paths=[_relative(repo_root, primary_assembly_path), *additional_assembly_paths],
    )


def _ensure_driver_built(repo_root: Path) -> Path:
    host_platform = "windows" if sys.platform.startswith("win") else ""
    arguments = ["dotnet", "build", str(repo_root / DRIVER_PROJECT_PATH), "-c", "Release", "-m:1"]
    intermediate_root = tooling_module.allocate_dotnet_intermediate_dir(DRIVER_PROJECT_PATH.stem, host_platform=host_platform)
    if intermediate_root is not None:
        arguments.append(f"-p:ChaosTempIntermediateRoot={intermediate_root.as_posix()}/")

    _run_checked(
        arguments,
        repo_root=repo_root,
        failure_message=f"dotnet build failed: {DRIVER_PROJECT_PATH.as_posix()}",
    )
    return repo_root / DRIVER_DLL_PATH


def run_frontend_pipeline_worker(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    driver_dll_path = _ensure_driver_built(repo_root)
    host_input_manifest = read_json(_resolve(repo_root, request["upstream"]["host-input"]["manifestPath"]))
    if not isinstance(host_input_manifest, dict):
        raise RuntimeError("host-input manifest must be an object")

    selection = dict(request["selection"])
    variant = _selection_variant(selection)
    variant_macros = _variant_macros(variant)
    assembly_path = _resolve(repo_root, str(host_input_manifest["primaryAssemblyPath"]))
    additional_assembly_paths = [
        _resolve(repo_root, str(path))
        for path in list(host_input_manifest.get("additionalAssemblyPaths") or [])
        if str(path)
    ]
    output_root = _resolve(repo_root, request["paths"]["bucketRoot"])
    output_root.mkdir(parents=True, exist_ok=True)
    driver_arguments = ["dotnet", str(driver_dll_path), str(assembly_path), str(output_root)]
    selection_source = dict(selection.get("source") or {})
    supplemental_full_closure_requests = _normalized_supplemental_full_closure_requests(selection_source)
    entry_point_subject_id = str(selection_source.get("entry") or "")
    if entry_point_subject_id:
        driver_arguments.extend(["--entry-point-subject-id", entry_point_subject_id])
    for additional_assembly_path in additional_assembly_paths:
        driver_arguments.extend(["--additional-assembly", str(additional_assembly_path)])
    if bool(selection_source.get("fullAssemblyClosure")):
        driver_arguments.append("--full-assembly-closure")

    _run_checked(
        driver_arguments,
        repo_root=repo_root,
        failure_message=f"managed closure materialization failed: {assembly_path}",
    )

    supplemental_full_closures: list[dict[str, Any]] = []
    for supplemental_request in supplemental_full_closure_requests:
        supplemental_output_root = output_root / "supplemental-full-closures" / str(supplemental_request["slug"])
        supplemental_input_assembly_path = _resolve(repo_root, str(supplemental_request["inputAssemblyPath"]))
        _run_checked(
            [
                "dotnet",
                str(driver_dll_path),
                str(supplemental_input_assembly_path),
                str(supplemental_output_root),
                "--full-assembly-closure",
            ],
            repo_root=repo_root,
            failure_message=f"managed supplemental closure materialization failed: {supplemental_input_assembly_path}",
        )
        supplemental_full_closures.append(
            {
                "assemblyName": str(supplemental_request["assemblyName"]),
                "inputAssemblyPath": str(supplemental_request["inputAssemblyPath"]),
                "analysisRootPath": _relative(repo_root, supplemental_output_root),
                "closureManifestPath": _relative(repo_root, supplemental_output_root / "closure.manifest.json"),
                "nativeReferencePlanPath": _relative(repo_root, supplemental_output_root / "native-reference.lowering-plan.json"),
                "nativeAotPlanPath": _relative(repo_root, supplemental_output_root / "native-aot.lowering-plan.json"),
            }
        )

    contracts_module.validate_analysis_contracts(repo_root)
    report_path = _resolve(repo_root, request["paths"]["reportPaths"][0])
    write_json(
        report_path,
        {
            "subjectId": str(request["selection"]["subjectId"]),
            "status": "ok",
            "variant": variant,
            "validationProfileId": selection.get("validationProfileId"),
            "validationKind": selection.get("validationKind"),
            "codegenMacros": list(variant_macros["codegen"]),
            "schemaPath": "contracts/artifacts/v0/schemas",
            "snapshotPath": "tests/contracts/analysis/v0/snapshots",
            "errors": [],
        },
    )

    manifest = {
        "subjectId": str(request["selection"]["subjectId"]),
        "bucket": "analysis",
        "hostInputManifestPath": str(request["upstream"]["host-input"]["manifestPath"]),
        "bundleKind": "proof-input-bundle",
        "validationProfileId": selection.get("validationProfileId"),
        "validationKind": selection.get("validationKind"),
        "variant": variant,
        "fullAssemblyClosure": bool(selection_source.get("fullAssemblyClosure")),
        "codegenMacros": list(variant_macros["codegen"]),
        "artifacts": {
            "typedIlIrPath": _relative(repo_root, output_root / "typed-il-ir.json"),
            "aotManifestPath": _relative(repo_root, output_root / "aot-manifest.json"),
            "metadataRegistrationPath": _relative(repo_root, output_root / "metadata-registration.json"),
            "codeRegistrationPath": _relative(repo_root, output_root / "code-registration.json"),
            "genericInstantiationDemandGraphPath": _relative(repo_root, output_root / "generic-instantiation-demand-graph.json"),
            "genericCapabilityMatrixPath": _relative(repo_root, output_root / "generic-capability-matrix.json"),
            "optimizationFactsPath": _relative(repo_root, output_root / "optimization-facts.json"),
            "preserveDescriptorPath": _relative(repo_root, output_root / "preserve-descriptor.json"),
            "closureManifestPath": _relative(repo_root, output_root / "closure.manifest.json"),
        },
    }
    if supplemental_full_closures:
        manifest["supplementalFullAssemblyClosures"] = supplemental_full_closures
    details: dict[str, Any] = {}
    engine_contract_summary = _engine_contract_summary(
        _engine_profile(repo_root, str(request["selection"]["subjectId"])),
    )
    if engine_contract_summary:
        manifest["engineContractSummary"] = engine_contract_summary
        details["engineContractSummary"] = engine_contract_summary
    write_json(_resolve(repo_root, request["paths"]["manifestPath"]), manifest)
    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=list(request["paths"]["reportPaths"]),
        primary_evidence_paths=[
            manifest["artifacts"]["typedIlIrPath"],
            manifest["artifacts"]["genericInstantiationDemandGraphPath"],
            manifest["artifacts"]["genericCapabilityMatrixPath"],
            manifest["artifacts"]["optimizationFactsPath"],
            manifest["artifacts"]["closureManifestPath"],
            *[
                path
                for supplemental_entry in supplemental_full_closures
                for path in [
                    str(supplemental_entry.get("closureManifestPath") or ""),
                    str(supplemental_entry.get("nativeReferencePlanPath") or ""),
                    str(supplemental_entry.get("nativeAotPlanPath") or ""),
                ]
                if path
            ],
        ],
        details=details,
    )


def _run_native_generated_emitter(
    *,
    repo_root: Path,
    request: dict[str, Any],
    driver_command: str,
    failure_label: str,
    generated_source_name: str,
    manifest_name: str,
    plan_name: str,
    manifest_key: str,
    plan_key: str,
) -> dict[str, Any]:
    driver_dll_path = _ensure_driver_built(repo_root)
    selection = dict(request["selection"])
    variant = _selection_variant(selection)
    variant_macros = _variant_macros(variant)
    analysis_manifest_path = _resolve(repo_root, request["upstream"]["analysis"]["manifestPath"])
    analysis_root = analysis_manifest_path.parent
    analysis_manifest = read_json(analysis_manifest_path) if analysis_manifest_path.is_file() else {}
    if not isinstance(analysis_manifest, dict):
        analysis_manifest = {}
    output_root = _resolve(repo_root, request["paths"]["bucketRoot"])
    emitted_bundle, primary_evidence_paths = _emit_native_generated_bundle(
        repo_root=repo_root,
        driver_dll_path=driver_dll_path,
        driver_command=driver_command,
        analysis_root=analysis_root,
        output_root=output_root,
        failure_label=failure_label,
        generated_source_name=generated_source_name,
        manifest_name=manifest_name,
        plan_name=plan_name,
        manifest_key=manifest_key,
        plan_key=plan_key,
    )
    generated_manifest = {
        "subjectId": str(request["selection"]["subjectId"]),
        "bucket": "generated",
        "analysisManifestPath": str(request["upstream"]["analysis"]["manifestPath"]),
        "validationProfileId": selection.get("validationProfileId"),
        "validationKind": selection.get("validationKind"),
        "variant": variant,
        "codegenMacros": list(variant_macros["codegen"]),
        **emitted_bundle,
    }
    details: dict[str, Any] = {}
    codegen_metrics_path = _generated_artifact_path(output_root, manifest_name, "codegenMetrics")
    if codegen_metrics_path is not None and codegen_metrics_path.is_file():
        codegen_metrics_payload = read_json(codegen_metrics_path)
        if isinstance(codegen_metrics_payload, dict):
            generated_manifest["codegenMetricsPath"] = _relative(repo_root, codegen_metrics_path)
            primary_evidence_paths.append(generated_manifest["codegenMetricsPath"])

            gate_metrics = _native_codegen_gate_metrics(codegen_metrics_payload)
            execution_context = dict(selection.get("executionContext") or {})
            host_platform = _normalize_host_platform(str(execution_context.get("hostPlatform") or ""))
            matrix_id = str(selection.get("matrixId") or "")
            subject_id = str(selection.get("subjectId") or "")
            if gate_metrics and host_platform and matrix_id and subject_id:
                codegen_perf = perf_module.evaluate_codegen_subject(
                    repo_root=repo_root,
                    subject_id=subject_id,
                    matrix_id=matrix_id,
                    host_platform=host_platform,
                    metrics=gate_metrics,
                    update_baseline=False,
                )
                baseline_compare_path = output_root / "codegen-baseline-compare.json"
                baseline_compare_payload = {
                    "reportVersion": "v1",
                    "subjectId": subject_id,
                    "matrixId": matrix_id,
                    "metrics": dict(codegen_perf["metrics"]),
                    "baseline": dict(codegen_perf["baseline"]),
                    "regressionStatus": str(codegen_perf["regressionStatus"]),
                    "metricsArtifactPath": generated_manifest["codegenMetricsPath"],
                }
                write_json(baseline_compare_path, baseline_compare_payload)
                generated_manifest["codegenBaselineComparePath"] = _relative(repo_root, baseline_compare_path)
                generated_manifest["codegenRegressionStatus"] = str(codegen_perf["regressionStatus"])
                primary_evidence_paths.append(generated_manifest["codegenBaselineComparePath"])
                details["codegenPerformance"] = {
                    "metrics": dict(codegen_perf["metrics"]),
                    "baselinePath": str(codegen_perf["baselinePath"]),
                    "baseline": dict(codegen_perf["baseline"]),
                    "baselineUpdated": bool(codegen_perf["baselineUpdated"]),
                    "regressionStatus": str(codegen_perf["regressionStatus"]),
                    "regressions": list(codegen_perf.get("regressions") or []),
                    "metricsArtifactPath": generated_manifest["codegenMetricsPath"],
                    "baselineComparePath": generated_manifest["codegenBaselineComparePath"],
                }
    lowering_plan = read_json(output_root / plan_name)
    if isinstance(lowering_plan, dict):
        engine_emission_summary = _engine_emission_summary(
            lowering_plan,
            generated_source_path=generated_manifest["generatedSourcePath"],
            native_reference_manifest_path=str(generated_manifest[manifest_key]),
        )
        if engine_emission_summary:
            generated_manifest["engineEmissionSummary"] = engine_emission_summary
            details["engineEmissionSummary"] = engine_emission_summary
    supplemental_full_closures: list[dict[str, Any]] = []
    for supplemental_entry in list(analysis_manifest.get("supplementalFullAssemblyClosures") or []):
        if not isinstance(supplemental_entry, dict):
            continue
        supplemental_analysis_root_path = str(supplemental_entry.get("analysisRootPath") or "").strip()
        if not supplemental_analysis_root_path:
            continue
        supplemental_slug = _slug_text(
            str(supplemental_entry.get("assemblyName") or Path(supplemental_analysis_root_path).name)
        )
        supplemental_base_output_root = output_root / "supplemental-full-closures" / supplemental_slug
        supplemental_reference_bundle, supplemental_reference_evidence = _emit_native_generated_bundle(
            repo_root=repo_root,
            driver_dll_path=driver_dll_path,
            driver_command="emit-native-reference",
            analysis_root=_resolve(repo_root, supplemental_analysis_root_path),
            output_root=supplemental_base_output_root / "native-reference",
            failure_label="supplemental native reference emission",
            generated_source_name="native-reference.generated.cpp",
            manifest_name="native-reference.manifest.json",
            plan_name="native-reference.plan.json",
            manifest_key="nativeReferenceManifestPath",
            plan_key="nativeReferencePlanPath",
        )
        supplemental_aot_bundle, supplemental_aot_evidence = _emit_native_generated_bundle(
            repo_root=repo_root,
            driver_dll_path=driver_dll_path,
            driver_command="emit-native-aot",
            analysis_root=_resolve(repo_root, supplemental_analysis_root_path),
            output_root=supplemental_base_output_root / "native-aot",
            failure_label="supplemental native aot emission",
            generated_source_name="native-aot.generated.cpp",
            manifest_name="native-aot.manifest.json",
            plan_name="native-aot.plan.json",
            manifest_key="nativeAotManifestPath",
            plan_key="nativeAotPlanPath",
        )
        supplemental_manifest_entry = {
            "assemblyName": str(supplemental_entry.get("assemblyName") or ""),
            "inputAssemblyPath": str(supplemental_entry.get("inputAssemblyPath") or ""),
            "analysisRootPath": supplemental_analysis_root_path,
            "nativeReferenceManifestPath": str(supplemental_reference_bundle["nativeReferenceManifestPath"]),
            "nativeReferencePlanPath": str(supplemental_reference_bundle["nativeReferencePlanPath"]),
            "nativeAotManifestPath": str(supplemental_aot_bundle["nativeAotManifestPath"]),
            "nativeAotPlanPath": str(supplemental_aot_bundle["nativeAotPlanPath"]),
        }
        runtime_skeleton_coverage_report_path = str(supplemental_reference_bundle.get("runtimeSkeletonCoverageReportPath") or "")
        if runtime_skeleton_coverage_report_path:
            supplemental_manifest_entry["runtimeSkeletonCoverageReportPath"] = runtime_skeleton_coverage_report_path
        supplemental_full_closures.append(supplemental_manifest_entry)
        primary_evidence_paths.extend(supplemental_reference_evidence)
        primary_evidence_paths.extend(supplemental_aot_evidence)
    if supplemental_full_closures:
        generated_manifest["supplementalFullAssemblyClosures"] = supplemental_full_closures
    write_json(_resolve(repo_root, request["paths"]["manifestPath"]), generated_manifest)
    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=[],
        primary_evidence_paths=primary_evidence_paths,
        details=details,
    )


def run_native_proof_emitter(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    selection = dict(request.get("selection") or {})
    entry_selection = _selection_declared_entry_selection(selection)
    subject_entry_selection = _selection_subject_entry_selection(selection)
    if not _selection_uses_native_hotupdate_chain(selection) and (
        str(entry_selection.get("family") or "") == "declared-unit-test" or subject_entry_selection
    ):
        return run_native_aot_emitter(repo_root=repo_root, request=request)
    return _run_native_generated_emitter(
        repo_root=repo_root,
        request=request,
        driver_command="emit-native-reference",
        failure_label="native proof emission",
        generated_source_name="native-reference.generated.cpp",
        manifest_name="native-reference.manifest.json",
        plan_name="native-reference.plan.json",
        manifest_key="nativeReferenceManifestPath",
        plan_key="nativeReferencePlanPath",
    )


def run_native_aot_emitter(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    return _run_native_generated_emitter(
        repo_root=repo_root,
        request=request,
        driver_command="emit-native-aot",
        failure_label="native aot emission",
        generated_source_name="native-aot.generated.cpp",
        manifest_name="native-aot.manifest.json",
        plan_name="native-aot.plan.json",
        manifest_key="nativeAotManifestPath",
        plan_key="nativeAotPlanPath",
    )


def _windows_subject_build(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    build_root = _resolve(repo_root, request["paths"]["bucketRoot"])
    selection = dict(request["selection"])
    variant = _selection_variant(selection)
    variant_macros = _variant_macros(variant)
    out_root = build_root / "out"
    generated_manifest_path = _resolve(repo_root, request["upstream"]["generated"]["manifestPath"])
    generated_manifest = read_json(generated_manifest_path) if generated_manifest_path.is_file() else {}
    if not isinstance(generated_manifest, dict):
        raise RuntimeError("generated manifest must be an object")
    generated_source_path_text = str(generated_manifest.get("generatedSourcePath") or "")
    generated_source_path_texts = _generated_source_paths_from_manifest(generated_manifest)
    native_aot_build = str(generated_manifest.get("nativeAotManifestPath") or "") != ""

    if native_aot_build:
        if not generated_source_path_text:
            raise RuntimeError("generated manifest missing generatedSourcePath")
        native_entry_function_name = _native_aot_entry_function_name(repo_root, generated_manifest)
        expected_host_kind = _selection_native_aot_workspace_host_kind(selection) or "benchmark-host"
        if expected_host_kind not in {"proof-host", "benchmark-host"}:
            raise RuntimeError(f"unsupported native-aot host kind: {expected_host_kind}")
        workspace_managed_host = _resolve_workspace_managed_host(
            repo_root,
            selection,
            host_kind_override=expected_host_kind,
        )
        if workspace_managed_host is None or str(workspace_managed_host.get("hostKind") or "") != expected_host_kind:
            raise RuntimeError(f"workspace {expected_host_kind} contract missing for native-aot build")
        workspace_native_test_host = _resolve_workspace_native_test_host(
            repo_root,
            selection,
            host_kind_override=expected_host_kind,
        ) or {}
        collection_path = str(workspace_managed_host.get("collectionPath") or "").strip()
        managed_test_project_id = str(
            workspace_native_test_host.get("managedTestProjectId")
            or workspace_managed_host.get("projectId")
            or ""
        ).strip()
        if not collection_path:
            raise RuntimeError(f"workspace {expected_host_kind} contract missing collectionPath")
        if not managed_test_project_id:
            raise RuntimeError(f"workspace {expected_host_kind} contract missing managedTestProjectId")

        host_source_path = (
            repo_root / "src" / "native" / "proof-host" / "native_aot_main.cpp"
            if expected_host_kind == "proof-host"
            else repo_root / "src" / "native" / "benchmark-host" / "native_aot_main.cpp"
        )
        generated_source_path = _resolve(repo_root, generated_source_path_text)
        output_executable_path = out_root / f"{WINDOWS_NATIVE_AOT_BUILD_TARGET}.exe"
        for source_file in [
            host_source_path,
            generated_source_path,
        ]:
            if not source_file.is_file():
                raise RuntimeError(f"subject proof source is missing: {source_file}")

        dispatch_manifest_path = ""
        if expected_host_kind == "benchmark-host":
            dispatch_manifest_path = _write_native_benchmark_dispatch_manifest(
                repo_root,
                build_root=build_root,
                selection=selection,
                collection_path=collection_path,
                entry_selection=_selection_declared_entry_selection(selection),
                native_entry_function_name=native_entry_function_name,
            )

        out_root.mkdir(parents=True, exist_ok=True)
        cmake_path, developer_env, _ninja_path = _windows_native_cmake_context(repo_root)
        generator = _windows_visual_studio_generator(repo_root)
        instance_spec = tooling_module.detect_visual_studio_instance_spec(generator)
        cmake_source_root, cmake_binary_dir = _resolve_windows_native_aot_cmake_layout(
            repo_root,
            selection=selection,
            build_root=build_root,
            workspace_native_test_host=workspace_native_test_host,
            generator=generator,
            host_kind=expected_host_kind,
        )

        _run_checked(
            [
                str(cmake_path),
                "-S",
                str(cmake_source_root),
                "-B",
                str(cmake_binary_dir),
                "-G",
                generator,
                *(
                    [f"-DCHAOS_SUBJECT_HOST_MAIN={host_source_path.as_posix()}"]
                    if expected_host_kind == "proof-host"
                    else [f"-DCHAOS_SUBJECT_BENCHMARK_HOST_MAIN={host_source_path.as_posix()}"]
                ),
                f"-DCHAOS_SUBJECT_GENERATED_INPUT_SOURCE={generated_source_path.as_posix()}",
                f"-DCHAOS_SUBJECT_VARIANT={variant}",
                f"-DCHAOS_SUBJECT_BUILD_OUT_ROOT={out_root.as_posix()}",
                *(
                    [f"-DCHAOS_SUBJECT_NATIVE_AOT_ENTRY_FUNCTION_NAME={native_entry_function_name}"]
                    if native_entry_function_name and native_entry_function_name != "RunNativeAot"
                    else []
                ),
                *([f"-DCMAKE_GENERATOR_INSTANCE={instance_spec}"] if instance_spec else []),
            ],
            repo_root=repo_root,
            failure_message="subject native build failed: windows-x64-native-aot-configure",
            env=developer_env,
        )
        _run_checked(
            [
                str(cmake_path),
                "--build",
                str(cmake_binary_dir),
                "--config",
                "Release",
                "--target",
                WINDOWS_NATIVE_AOT_BUILD_TARGET,
            ],
            repo_root=repo_root,
            failure_message="subject native build failed: windows-x64-native-aot",
            env=developer_env,
        )

        manifest = {
            "subjectId": str(request["selection"]["subjectId"]),
            "matrixId": str(request["selection"]["matrixId"]),
            "bucket": "build",
            "targetPlatform": str(request["selection"]["executionContext"]["targetPlatform"]),
            "toolchainProfile": str(request["selection"]["executionContext"]["toolchainProfile"]),
            "variant": variant,
            "buildStrategy": (
                WINDOWS_NATIVE_AOT_CMAKE_BUILD_STRATEGY
                if expected_host_kind == "proof-host"
                else WINDOWS_BENCHMARK_CMAKE_BUILD_STRATEGY
            ),
            "buildKind": "native-aot",
            "variantMacros": {
                "codegen": list(variant_macros["codegen"]),
                "native": list(variant_macros["native"]),
            },
            "generatedManifestPath": str(request["upstream"]["generated"]["manifestPath"]),
            "generatedSourcePath": _relative(repo_root, generated_source_path),
            "hostSourcePath": _relative(repo_root, host_source_path),
            "binaryRoot": _relative(repo_root, out_root),
            "outputs": [_relative(repo_root, output_executable_path)],
            "cmakeBinaryDir": tooling_module.path_text(repo_root, cmake_binary_dir),
            "hostKind": expected_host_kind,
            "collectionPath": collection_path,
            "managedTestProjectId": managed_test_project_id,
            "nativeEntryFunctionName": native_entry_function_name,
        }
        if dispatch_manifest_path:
            manifest["dispatchManifestPath"] = dispatch_manifest_path
        entry_selection = _selection_declared_entry_selection(selection)
        if expected_host_kind == "proof-host" and entry_selection:
            manifest["entrySelection"] = dict(entry_selection)
        write_json(_resolve(repo_root, request["paths"]["manifestPath"]), manifest)
        return _success_result(
            bucket_manifest_path=request["paths"]["manifestPath"],
            report_paths=[],
            primary_evidence_paths=[_relative(repo_root, output_executable_path)],
        )

    generated_root = generated_manifest_path.parent
    generated_source_paths = [
        _resolve(repo_root, value)
        for value in generated_source_path_texts
    ]
    if not generated_source_paths:
        generated_source_paths = [generated_root / "generated" / "native-reference.generated.cpp"]
    generated_source_path = generated_source_paths[0]
    runtime_root = build_root.parent / "runtime"
    output_executable_path = out_root / f"{WINDOWS_REFERENCE_BUILD_TARGET}.exe"
    for source_path in generated_source_paths:
        if not source_path.is_file():
            raise RuntimeError(f"subject proof source is missing: {source_path}")

    out_root.mkdir(parents=True, exist_ok=True)
    cmake_path, developer_env, _ninja_path = _windows_native_cmake_context(repo_root)
    generator = _windows_visual_studio_generator(repo_root)
    instance_spec = tooling_module.detect_visual_studio_instance_spec(generator)
    workspace_native_test_host = _resolve_workspace_native_test_host(
        repo_root,
        selection,
        host_kind_override="proof-host",
    ) or {}
    cmake_source_root, cmake_binary_dir = _resolve_windows_native_reference_cmake_layout(
        repo_root,
        selection=selection,
        build_root=build_root,
        workspace_native_test_host=workspace_native_test_host,
        generator=generator,
        generated_source_paths=generated_source_paths,
    )
    host_source_path = cmake_source_root / "proof" / "main.cpp"

    _run_checked(
        [
            cmake_path,
            "-S",
            str(cmake_source_root),
            "-B",
            str(cmake_binary_dir),
            "-G",
            generator,
            f"-DCHAOS_SUBJECT_REPO_ROOT={repo_root}",
            f"-DCHAOS_SUBJECT_VARIANT={variant}",
            f"-DCHAOS_SUBJECT_BUILD_OUT_ROOT={out_root}",
            f"-DCHAOS_SUBJECT_RUNTIME_ROOT={runtime_root}",
            *([f"-DCMAKE_GENERATOR_INSTANCE={instance_spec}"] if instance_spec else []),
        ],
        repo_root=repo_root,
        failure_message="subject native build failed: windows-x64-native-reference-configure",
        env=developer_env,
    )
    _run_checked(
        [
            cmake_path,
            "--build",
            str(cmake_binary_dir),
            "--config",
            "Release",
            "--target",
            WINDOWS_REFERENCE_BUILD_TARGET,
        ],
        repo_root=repo_root,
        failure_message="subject native build failed: windows-x64-native-reference",
        env=developer_env,
    )

    manifest = {
        "subjectId": str(request["selection"]["subjectId"]),
        "matrixId": str(request["selection"]["matrixId"]),
        "bucket": "build",
        "targetPlatform": str(request["selection"]["executionContext"]["targetPlatform"]),
        "toolchainProfile": str(request["selection"]["executionContext"]["toolchainProfile"]),
        "variant": variant,
        "buildStrategy": WINDOWS_REFERENCE_CMAKE_BUILD_STRATEGY,
        "buildKind": "native-reference",
        "variantMacros": {
            "codegen": list(variant_macros["codegen"]),
            "native": list(variant_macros["native"]),
        },
        "generatedManifestPath": str(request["upstream"]["generated"]["manifestPath"]),
        "generatedSourcePath": _relative(repo_root, generated_source_path),
        "generatedSourcePaths": [_relative(repo_root, path) for path in generated_source_paths],
        "hostSourcePath": _relative(repo_root, host_source_path),
        "binaryRoot": _relative(repo_root, out_root),
        "outputs": [_relative(repo_root, output_executable_path)],
        "cmakeBinaryDir": tooling_module.path_text(repo_root, cmake_binary_dir),
    }
    write_json(_resolve(repo_root, request["paths"]["manifestPath"]), manifest)
    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=[],
        primary_evidence_paths=[_relative(repo_root, output_executable_path)],
    )


def _android_runtime_build_environment(repo_root: Path) -> tuple[str, dict[str, str], str]:
    cmake_path, env = tooling_module.cmake_environment(repo_root)
    if not cmake_path or not dict(env or {}).get("ANDROID_NDK_ROOT"):
        bootstrap = tooling_module.ensure_android_host_tooling_available(
            "subject android runtime build",
            "windows",
            repo_root,
        )
        if not bootstrap.ready:
            raise RuntimeError((bootstrap.output or "android host tooling bootstrap failed").strip())
        cmake_path, env = tooling_module.cmake_environment(repo_root)

    ninja_path = tooling_module.find_ninja_executable()
    if not cmake_path:
        raise RuntimeError("cmake not found for Android runtime build")
    if not ninja_path:
        raise RuntimeError("ninja not found for Android runtime build")
    return cmake_path, dict(env or {}), ninja_path


def _android_subject_runtime_build(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    build_root = _resolve(repo_root, request["paths"]["bucketRoot"])
    selection = dict(request["selection"])
    execution_context = dict(selection.get("executionContext") or {})
    subject_id = str(selection["subjectId"])
    variant = _selection_variant(selection)
    variant_macros = _variant_macros(variant)
    host_platform = _normalize_host_platform(str(execution_context.get("hostPlatform") or ""))
    android_host_root = _subject_mobile_host_root(repo_root, subject_id, "android-arm64")
    out_root = build_root / "out"
    out_root.mkdir(parents=True, exist_ok=True)

    cmake_path, env, ninja_path = _android_runtime_build_environment(repo_root)
    if host_platform == "windows":
        cmake_binary_dir = tooling_module.allocate_cmake_binary_dir(
            build_root / "cmake",
            host_platform=host_platform,
            generator="Ninja",
        )
    else:
        cmake_binary_dir = build_root / "cmake"

    runtime_binary_path = out_root / ANDROID_RUNTIME_BUILD_TARGET
    _run_checked(
        [
            str(cmake_path),
            "-S",
            str(repo_root),
            "-B",
            str(cmake_binary_dir),
            "-G",
            "Ninja",
            "-DROADMAP0_PRESET_TARGET=android-arm64-smoke",
            f"-DCHAOS_SUBJECT_VARIANT={variant}",
            f"-DCHAOS_SUBJECT_ANDROID_HOST_ROOT={android_host_root}",
            f"-DCHAOS_SUBJECT_ANDROID_ARTIFACT_ROOT={out_root}",
            f"-DCMAKE_TOOLCHAIN_FILE={repo_root / 'build' / 'toolchains' / 'android-arm64.cmake'}",
            "-DCMAKE_BUILD_TYPE=Release",
            f"-DCMAKE_MAKE_PROGRAM={ninja_path}",
        ],
        repo_root=repo_root,
        failure_message="subject proof build failed: android-arm64-runtime",
        env=env,
    )
    _run_checked(
        [
            str(cmake_path),
            "--build",
            str(cmake_binary_dir),
            "--target",
            ANDROID_RUNTIME_BUILD_TARGET,
        ],
        repo_root=repo_root,
        failure_message="subject proof build failed: android-arm64-runtime",
        env=env,
    )

    manifest = {
        "subjectId": subject_id,
        "matrixId": str(selection["matrixId"]),
        "bucket": "build",
        "targetPlatform": str(execution_context["targetPlatform"]),
        "toolchainProfile": str(execution_context["toolchainProfile"]),
        "variant": variant,
        "variantMacros": {
            "codegen": list(variant_macros["codegen"]),
            "native": list(variant_macros["native"]),
        },
        "generatedManifestPath": str(request["upstream"]["generated"]["manifestPath"]),
        "buildStrategy": ANDROID_NATIVE_BUILD_STRATEGY,
        "binaryRoot": _relative(repo_root, out_root),
        "outputs": [_relative(repo_root, runtime_binary_path)],
        "cmakeBinaryDir": tooling_module.path_text(repo_root, cmake_binary_dir),
    }
    write_json(_resolve(repo_root, request["paths"]["manifestPath"]), manifest)
    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=[],
        primary_evidence_paths=[_relative(repo_root, runtime_binary_path)],
    )


def _validate_only_build(
    *,
    repo_root: Path,
    request: dict[str, Any],
    preset_target: str,
    toolchain_file: Path,
    extra_cache_entries: list[str] | None = None,
) -> dict[str, Any]:
    build_root = _resolve(repo_root, request["paths"]["bucketRoot"])
    selection = dict(request["selection"])
    variant = _selection_variant(selection)
    variant_macros = _variant_macros(variant)
    execution_context = dict(request["selection"].get("executionContext") or {})
    host_platform = _normalize_host_platform(str(execution_context.get("hostPlatform") or ""))
    generator = "Ninja"
    instance_spec: str | None = None
    if host_platform == "windows":
        generator = _windows_visual_studio_generator(repo_root)
        instance_spec = tooling_module.detect_visual_studio_instance_spec(generator)
        cmake_binary_dir = tooling_module.allocate_cmake_binary_dir(
            build_root / "cmake",
            host_platform=host_platform,
            generator=generator,
        )
    else:
        cmake_binary_dir = build_root / "cmake"
    success_marker = build_root / "validate-only.success.txt"

    _run_checked(
        [
            "cmake",
            "-S",
            str(repo_root),
            "-B",
            str(cmake_binary_dir),
            "-G",
            generator,
            f"-DROADMAP0_PRESET_TARGET={preset_target}",
            "-DROADMAP0_TOOLCHAIN_VALIDATE_ONLY=ON",
            f"-DCHAOS_SUBJECT_VARIANT={variant}",
            *(list(extra_cache_entries or [])),
            f"-DCMAKE_TOOLCHAIN_FILE={repo_root / toolchain_file}",
            *([f"-DCMAKE_GENERATOR_INSTANCE={instance_spec}"] if instance_spec else []),
        ],
        repo_root=repo_root,
        failure_message=f"cmake routing configure failed: {preset_target}",
    )
    _run_checked(
        ["cmake", "--build", str(cmake_binary_dir)],
        repo_root=repo_root,
        failure_message=f"cmake routing build failed: {preset_target}",
    )

    success_marker.parent.mkdir(parents=True, exist_ok=True)
    success_marker.write_text("ok\n", encoding="utf-8")
    manifest = {
        "subjectId": str(request["selection"]["subjectId"]),
        "matrixId": str(request["selection"]["matrixId"]),
        "bucket": "build",
        "targetPlatform": str(request["selection"]["executionContext"]["targetPlatform"]),
        "toolchainProfile": str(request["selection"]["executionContext"]["toolchainProfile"]),
        "variant": variant,
        "variantMacros": {
            "codegen": list(variant_macros["codegen"]),
            "native": list(variant_macros["native"]),
        },
        "generatedManifestPath": str(request["upstream"]["generated"]["manifestPath"]),
        "binaryRoot": _relative(repo_root, build_root),
        "outputs": [_relative(repo_root, success_marker)],
        "cmakeBinaryDir": tooling_module.path_text(repo_root, cmake_binary_dir),
    }
    write_json(_resolve(repo_root, request["paths"]["manifestPath"]), manifest)
    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=[],
        primary_evidence_paths=[_relative(repo_root, success_marker)],
    )


def run_build_target(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    subject_id = str(request["selection"]["subjectId"])
    execution_context = dict(request["selection"].get("executionContext") or {})
    target_platform = str(execution_context["targetPlatform"])
    if target_platform == "windows-x64":
        return _windows_subject_build(repo_root=repo_root, request=request)
    if target_platform == "android-arm64":
        runtime_profile = str(execution_context.get("runtimeProfile") or "")
        if runtime_profile in {"android-native-runtime", "android-native-perf-profile"}:
            return _android_subject_runtime_build(repo_root=repo_root, request=request)
        android_host_root = _subject_mobile_host_root(repo_root, subject_id, target_platform)
        return _validate_only_build(
            repo_root=repo_root,
            request=request,
            preset_target="android-arm64-smoke",
            toolchain_file=Path("build/toolchains/android-arm64.cmake"),
            extra_cache_entries=[f"-DCHAOS_SUBJECT_ANDROID_HOST_ROOT={android_host_root}"],
        )
    if target_platform == "ios-arm64":
        ios_host_root = _subject_mobile_host_root(repo_root, subject_id, target_platform)
        return _validate_only_build(
            repo_root=repo_root,
            request=request,
            preset_target="ios-arm64-packaging",
            toolchain_file=Path("build/toolchains/ios-arm64.cmake"),
            extra_cache_entries=[f"-DCHAOS_SUBJECT_IOS_HOST_ROOT={ios_host_root}"],
        )
    if target_platform == "linux-x64":
        return _validate_only_build(
            repo_root=repo_root,
            request=request,
            preset_target="linux-x64-packaging",
            toolchain_file=Path("build/toolchains/linux-x64.cmake"),
        )
    raise RuntimeError(f"unsupported build target platform: {target_platform}")


def _android_adb_executable(repo_root: Path) -> str:
    candidate = tooling_module.android_adb_path(repo_root)
    return str(candidate if candidate.is_file() else Path("adb"))


def _android_emulator_executable(repo_root: Path) -> str:
    candidate = tooling_module.android_emulator_path(repo_root)
    return str(candidate if candidate.is_file() else Path("emulator"))


def _run_android_host_command(
    arguments: list[str],
    *,
    repo_root: Path,
    env: dict[str, str],
    failure_message: str,
) -> subprocess.CompletedProcess[str]:
    completed = run_process(arguments, cwd=repo_root, env=env)
    if completed.returncode != 0:
        raise RuntimeError(f"{failure_message}\n{combine_process_output(completed)}".strip())
    return completed


def _android_device_serials(*, repo_root: Path, env: dict[str, str]) -> list[str]:
    completed = run_process([_android_adb_executable(repo_root), "devices"], cwd=repo_root, env=env)
    if completed.returncode != 0:
        return []

    serials: list[str] = []
    for line in (completed.stdout or "").splitlines():
        line = line.strip()
        if not line or line.startswith("List of devices attached") or "\t" not in line:
            continue
        serial, state = line.split("\t", 1)
        if serial and state:
            serials.append(serial)
    return serials


def _launch_android_emulator(
    *,
    repo_root: Path,
    env: dict[str, str],
) -> tuple[str, subprocess.Popen[Any], Any, Any]:
    existing_serials = set(_android_device_serials(repo_root=repo_root, env=env))
    merged_env = dict(os.environ)
    merged_env.update(env)
    process = subprocess.Popen(
        [
            _android_emulator_executable(repo_root),
            "-avd",
            tooling_module.ANDROID_AVD_NAME,
            "-no-window",
            "-no-audio",
            "-no-boot-anim",
            "-no-snapshot",
        ],
        cwd=str(repo_root),
        env=merged_env,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )

    deadline = time.time() + 90
    while time.time() < deadline:
        if process.poll() is not None:
            raise RuntimeError("android emulator exited before adb exposed a device serial")

        current_serials = {
            serial
            for serial in _android_device_serials(repo_root=repo_root, env=env)
            if serial.startswith("emulator-")
        }
        new_serials = sorted(current_serials - existing_serials)
        if new_serials:
            return new_serials[0], process, None, None
        time.sleep(2)

    raise RuntimeError("android emulator did not expose an adb device serial in time")


def _wait_for_android_boot_completed(
    *,
    repo_root: Path,
    serial: str,
    env: dict[str, str],
    timeout_seconds: int = 300,
) -> None:
    adb_executable = _android_adb_executable(repo_root)
    _run_android_host_command(
        [adb_executable, "-s", serial, "wait-for-device"],
        repo_root=repo_root,
        env=env,
        failure_message=f"android emulator did not become available: {serial}",
    )

    deadline = time.time() + timeout_seconds
    while time.time() < deadline:
        completed = run_process(
            [adb_executable, "-s", serial, "shell", "getprop", "sys.boot_completed"],
            cwd=repo_root,
            env=env,
        )
        if completed.returncode == 0 and (completed.stdout or "").strip() == "1":
            return
        time.sleep(2)

    raise RuntimeError(f"android emulator boot timed out: {serial}")


def _run_android_binary_via_adb(
    *,
    repo_root: Path,
    executable_path: Path,
    serial: str,
    env: dict[str, str],
    runtime_arguments: list[str] | None = None,
) -> subprocess.CompletedProcess[str]:
    adb_executable = _android_adb_executable(repo_root)
    remote_path = f"{ANDROID_RUNTIME_REMOTE_ROOT}/{executable_path.name}"
    selected_runtime_arguments = list(runtime_arguments or [])
    _run_android_host_command(
        [adb_executable, "-s", serial, "shell", "mkdir", "-p", ANDROID_RUNTIME_REMOTE_ROOT],
        repo_root=repo_root,
        env=env,
        failure_message=f"failed to prepare Android runtime directory: {ANDROID_RUNTIME_REMOTE_ROOT}",
    )
    _run_android_host_command(
        [adb_executable, "-s", serial, "push", str(executable_path), remote_path],
        repo_root=repo_root,
        env=env,
        failure_message=f"failed to push Android runtime binary: {executable_path}",
    )
    _run_android_host_command(
        [adb_executable, "-s", serial, "shell", "chmod", "755", remote_path],
        repo_root=repo_root,
        env=env,
        failure_message=f"failed to chmod Android runtime binary: {remote_path}",
    )
    binary_command_text = " ".join(
        [
            shlex.quote(remote_path),
            *[shlex.quote(argument) for argument in selected_runtime_arguments],
        ]
    ).strip()
    command_text = "; ".join(
        [
            *_android_runtime_environment_exports(selected_runtime_arguments),
            binary_command_text,
            f"status=$?; printf '\\n{ANDROID_EXIT_CODE_PREFIX}%s\\n' \"$status\"",
        ]
    ).strip()
    return run_process(
        [
            adb_executable,
            "-s",
            serial,
            "shell",
            "sh",
            "-c",
            command_text,
        ],
        cwd=repo_root,
        env=env,
    )


def _shutdown_android_emulator(
    *,
    repo_root: Path,
    serial: str | None,
    process: subprocess.Popen[Any] | None,
    stdout_handle: Any,
    stderr_handle: Any,
    env: dict[str, str],
) -> None:
    try:
        if serial:
            run_process([_android_adb_executable(repo_root), "-s", serial, "emu", "kill"], cwd=repo_root, env=env)
    finally:
        if process is not None:
            try:
                process.wait(timeout=20)
            except subprocess.TimeoutExpired:
                process.terminate()
                try:
                    process.wait(timeout=10)
                except subprocess.TimeoutExpired:
                    process.kill()
                    process.wait(timeout=5)
        for handle in [stdout_handle, stderr_handle]:
            if handle is not None and hasattr(handle, "close"):
                handle.close()


def _extract_android_runtime_output(completed: subprocess.CompletedProcess[str]) -> tuple[str, list[str], int]:
    output_lines: list[str] = []
    stdout_lines: list[str] = []
    exit_code: int | None = None
    for line in (completed.stdout or "").splitlines():
        stripped = line.strip()
        if not stripped:
            continue
        if stripped.startswith(ANDROID_EXIT_CODE_PREFIX):
            try:
                exit_code = int(stripped.split("=", 1)[1])
            except ValueError:
                exit_code = completed.returncode
            continue
        stdout_lines.append(line)
        output_lines.append(stripped)

    stdout_text = "\n".join(stdout_lines)
    if stdout_text:
        stdout_text += "\n"
    return stdout_text, output_lines, int(completed.returncode if exit_code is None else exit_code)


def run_runtime_observe(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    build_manifest = read_json(_resolve(repo_root, request["upstream"]["build"]["manifestPath"]))
    if not isinstance(build_manifest, dict):
        raise RuntimeError("build manifest must be an object")

    selection = dict(request["selection"])
    runtime_root = _resolve(repo_root, request["paths"]["bucketRoot"])
    runtime_root.mkdir(parents=True, exist_ok=True)

    stdout_path = runtime_root / "stdout.log"
    stderr_path = runtime_root / "stderr.log"
    exit_code_path = runtime_root / "exit-code.txt"
    build_strategy = str(build_manifest.get("buildStrategy") or "")
    host_kind = str(build_manifest.get("hostKind") or "").strip()

    if (
        build_strategy in {WINDOWS_NATIVE_AOT_CMAKE_BUILD_STRATEGY, WINDOWS_BENCHMARK_CMAKE_BUILD_STRATEGY}
        and host_kind in {"proof-host", "benchmark-host"}
    ):
        output_paths = [str(value) for value in list(build_manifest.get("outputs") or []) if str(value)]
        if not output_paths:
            raise RuntimeError(f"{build_strategy} build manifest missing outputs")

        native_executable_path = _resolve(repo_root, output_paths[0])
        runtime_arguments = _selection_runtime_arguments(selection)
        collection_path = str(build_manifest.get("collectionPath") or "").strip()
        if host_kind == "proof-host" and collection_path:
            runtime_arguments.append(f"{CHAOS_COLLECTION_PATH_ARGUMENT_PREFIX}{collection_path}")
        entry_selection = _selection_declared_entry_selection(selection)
        entry_index = entry_selection.get("entryIndex")
        if isinstance(entry_index, int) and not isinstance(entry_index, bool) and entry_index >= 0:
            runtime_arguments.append(f"{CHAOS_ENTRY_INDEX_ARGUMENT_PREFIX}{entry_index}")
        completed = run_process([str(native_executable_path), *runtime_arguments], cwd=runtime_root)
        stdout_path.write_text(completed.stdout or "", encoding="utf-8")
        stderr_path.write_text(completed.stderr or "", encoding="utf-8")
        exit_code_path.write_text(f"{completed.returncode}\n", encoding="utf-8")

        manifest = {
            "subjectId": str(selection["subjectId"]),
            "matrixId": str(selection["matrixId"]),
            "bucket": "runtime",
            "variant": _selection_variant(selection),
            "buildManifestPath": str(request["upstream"]["build"]["manifestPath"]),
            "stdoutPath": _relative(repo_root, stdout_path),
            "stderrPath": _relative(repo_root, stderr_path),
            "exitCodePath": _relative(repo_root, exit_code_path),
            "tracePaths": [],
            "hostKind": host_kind,
        }
        if collection_path:
            manifest["collectionPath"] = collection_path
        write_json(_resolve(repo_root, request["paths"]["manifestPath"]), manifest)

        if completed.returncode != 0:
            return {
                "status": "fail",
                "bucketManifestPath": request["paths"]["manifestPath"],
                "reportPaths": [],
                "primaryEvidencePaths": [manifest["stdoutPath"], manifest["exitCodePath"]],
                "metrics": {"durationMs": 0},
                "diagnostics": {"stdoutPath": manifest["stdoutPath"], "stderrPath": manifest["stderrPath"]},
                "details": {},
                "failure": f"subject proof run failed: {native_executable_path}",
            }

        return _success_result(
            bucket_manifest_path=request["paths"]["manifestPath"],
            report_paths=[],
            primary_evidence_paths=[manifest["stdoutPath"], manifest["exitCodePath"]],
            stdout_path=manifest["stdoutPath"],
            stderr_path=manifest["stderrPath"],
        )

    if build_strategy == WINDOWS_DIRECT_BUILD_STRATEGY:
        output_paths = [str(value) for value in list(build_manifest.get("outputs") or []) if str(value)]
        if not output_paths:
            raise RuntimeError("direct-msvc build manifest missing outputs")

        native_executable_path = _resolve(repo_root, output_paths[0])
        completed = run_process([str(native_executable_path)], cwd=runtime_root)
        stdout_path.write_text(completed.stdout or "", encoding="utf-8")
        stderr_path.write_text(completed.stderr or "", encoding="utf-8")
        exit_code_path.write_text(f"{completed.returncode}\n", encoding="utf-8")

        manifest = {
            "subjectId": str(selection["subjectId"]),
            "matrixId": str(selection["matrixId"]),
            "bucket": "runtime",
            "variant": _selection_variant(selection),
            "buildManifestPath": str(request["upstream"]["build"]["manifestPath"]),
            "stdoutPath": _relative(repo_root, stdout_path),
            "stderrPath": _relative(repo_root, stderr_path),
            "exitCodePath": _relative(repo_root, exit_code_path),
            "tracePaths": [],
        }
        write_json(_resolve(repo_root, request["paths"]["manifestPath"]), manifest)

        if completed.returncode != 0:
            return {
                "status": "fail",
                "bucketManifestPath": request["paths"]["manifestPath"],
                "reportPaths": [],
                "primaryEvidencePaths": [manifest["stdoutPath"], manifest["exitCodePath"]],
                "metrics": {"durationMs": 0},
                "diagnostics": {"stdoutPath": manifest["stdoutPath"], "stderrPath": manifest["stderrPath"]},
                "details": {},
                "failure": f"subject proof run failed: {native_executable_path}",
            }

        return _success_result(
            bucket_manifest_path=request["paths"]["manifestPath"],
            report_paths=[],
            primary_evidence_paths=[manifest["stdoutPath"], manifest["exitCodePath"]],
            stdout_path=manifest["stdoutPath"],
            stderr_path=manifest["stderrPath"],
        )

    if build_strategy == ANDROID_NATIVE_BUILD_STRATEGY:
        output_paths = [str(value) for value in list(build_manifest.get("outputs") or []) if str(value)]
        if not output_paths:
            raise RuntimeError("android-native-cmake build manifest missing outputs")

        execution_context = dict(selection.get("executionContext") or {})
        runtime_arguments = _selection_runtime_arguments(selection)
        host_platform = _normalize_host_platform(str(execution_context.get("hostPlatform") or ""))
        bootstrap = tooling_module.ensure_android_host_tooling_available(
            "subject runtime observe",
            host_platform,
            repo_root,
        )
        if not bootstrap.ready:
            raise RuntimeError((bootstrap.output or "android host tooling bootstrap failed").strip())

        env = tooling_module.android_environment_overrides(repo_root)
        native_executable_path = _resolve(repo_root, output_paths[0])
        android_serial: str | None = None
        emulator_process: subprocess.Popen[Any] | None = None
        emulator_stdout = None
        emulator_stderr = None
        try:
            android_serial, emulator_process, emulator_stdout, emulator_stderr = _launch_android_emulator(
                repo_root=repo_root,
                env=env,
            )
            _wait_for_android_boot_completed(
                repo_root=repo_root,
                serial=android_serial,
                env=env,
            )
            completed = _run_android_binary_via_adb(
                repo_root=repo_root,
                executable_path=native_executable_path,
                serial=android_serial,
                env=env,
                runtime_arguments=runtime_arguments,
            )
        finally:
            _shutdown_android_emulator(
                repo_root=repo_root,
                serial=android_serial,
                process=emulator_process,
                stdout_handle=emulator_stdout,
                stderr_handle=emulator_stderr,
                env=env,
            )

        cleaned_stdout, output_lines, runtime_exit_code = _extract_android_runtime_output(completed)
        stdout_path.write_text(cleaned_stdout, encoding="utf-8")
        stderr_path.write_text(completed.stderr or "", encoding="utf-8")
        exit_code_path.write_text(f"{runtime_exit_code}\n", encoding="utf-8")

        manifest = {
            "subjectId": str(request["selection"]["subjectId"]),
            "matrixId": str(request["selection"]["matrixId"]),
            "bucket": "runtime",
            "variant": _selection_variant(dict(request["selection"])),
            "buildManifestPath": str(request["upstream"]["build"]["manifestPath"]),
            "stdoutPath": _relative(repo_root, stdout_path),
            "stderrPath": _relative(repo_root, stderr_path),
            "exitCodePath": _relative(repo_root, exit_code_path),
            "tracePaths": [],
            "outputLines": output_lines,
            "androidSerial": android_serial,
            "androidAvdName": tooling_module.ANDROID_AVD_NAME,
            "runtimeArguments": runtime_arguments,
        }
        write_json(_resolve(repo_root, request["paths"]["manifestPath"]), manifest)

        failure_reason: str | None = None
        if completed.returncode != 0:
            failure_reason = f"android adb execution failed: {native_executable_path}"
        elif runtime_exit_code != 0:
            failure_reason = f"android runtime observe failed: {native_executable_path}"

        if failure_reason is not None:
            return {
                "status": "fail",
                "bucketManifestPath": request["paths"]["manifestPath"],
                "reportPaths": [],
                "primaryEvidencePaths": [manifest["stdoutPath"], manifest["exitCodePath"]],
                "metrics": {"durationMs": 0},
                "diagnostics": {"stdoutPath": manifest["stdoutPath"], "stderrPath": manifest["stderrPath"]},
                "details": {},
                "failure": failure_reason,
            }

        return _success_result(
            bucket_manifest_path=request["paths"]["manifestPath"],
            report_paths=[],
            primary_evidence_paths=[manifest["stdoutPath"], manifest["exitCodePath"]],
            stdout_path=manifest["stdoutPath"],
            stderr_path=manifest["stderrPath"],
        )

    cmake_path, developer_env, _ninja_path = _windows_native_cmake_context(repo_root)
    cmake_binary_dir = _resolve(repo_root, str(build_manifest["cmakeBinaryDir"]))

    manifest = {
        "subjectId": str(request["selection"]["subjectId"]),
        "matrixId": str(request["selection"]["matrixId"]),
        "bucket": "runtime",
        "variant": _selection_variant(dict(request["selection"])),
        "buildManifestPath": str(request["upstream"]["build"]["manifestPath"]),
        "stdoutPath": _relative(repo_root, stdout_path),
        "stderrPath": _relative(repo_root, stderr_path),
        "exitCodePath": _relative(repo_root, exit_code_path),
        "tracePaths": [],
    }
    try:
        _run_checked(
            [
                cmake_path,
                "--build",
                str(cmake_binary_dir),
                "--config",
                "Release",
                "--target",
                WINDOWS_REFERENCE_RUN_TARGET,
            ],
            repo_root=repo_root,
            failure_message="subject proof run failed: windows-x64-reference",
            env=developer_env,
        )
    except Exception as exception:
        write_json(_resolve(repo_root, request["paths"]["manifestPath"]), manifest)
        return {
            "status": "fail",
            "bucketManifestPath": request["paths"]["manifestPath"],
            "reportPaths": [],
            "primaryEvidencePaths": [manifest["stdoutPath"], manifest["stderrPath"], manifest["exitCodePath"]],
            "metrics": {"durationMs": 0},
            "diagnostics": {"stdoutPath": manifest["stdoutPath"], "stderrPath": manifest["stderrPath"]},
            "details": {},
            "failure": str(exception),
        }

    write_json(_resolve(repo_root, request["paths"]["manifestPath"]), manifest)
    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=[],
        primary_evidence_paths=[manifest["stdoutPath"], manifest["exitCodePath"]],
        stdout_path=manifest["stdoutPath"],
        stderr_path=manifest["stderrPath"],
    )


def run_runtime_engine_observe(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    build_manifest = read_json(_resolve(repo_root, request["upstream"]["build"]["manifestPath"]))
    if not isinstance(build_manifest, dict):
        raise RuntimeError("build manifest must be an object")
    if str(build_manifest.get("buildStrategy") or "") != WINDOWS_DIRECT_BUILD_STRATEGY:
        raise RuntimeError("engine runtime observe requires direct-msvc build output")

    output_paths = [str(value) for value in list(build_manifest.get("outputs") or []) if str(value)]
    if not output_paths:
        raise RuntimeError("direct-msvc build manifest missing outputs")

    runtime_root = _resolve(repo_root, request["paths"]["bucketRoot"])
    runtime_root.mkdir(parents=True, exist_ok=True)

    native_executable_path = _resolve(repo_root, output_paths[0])
    started = time.perf_counter()
    completed = run_process([str(native_executable_path)], cwd=runtime_root)
    duration_ms = int(round((time.perf_counter() - started) * 1000))

    stdout_path = runtime_root / "stdout.log"
    stderr_path = runtime_root / "stderr.log"
    exit_code_path = runtime_root / "exit-code.txt"
    stdout_path.write_text(completed.stdout or "", encoding="utf-8")
    stderr_path.write_text(completed.stderr or "", encoding="utf-8")
    exit_code_path.write_text(f"{completed.returncode}\n", encoding="utf-8")

    observations = _parse_engine_observations(completed.stdout or "")
    evidence_items: list[dict[str, Any]] = []
    evidence_paths: list[str] = []
    for observation in observations:
        kind = str(observation.get("kind") or "")
        if not kind:
            continue

        artifact_file_name = str(observation.get("artifactFileName") or f"{kind}.json")
        evidence_path = runtime_root / artifact_file_name
        write_json(evidence_path, observation)
        evidence_relative_path = _relative(repo_root, evidence_path)
        evidence_paths.append(evidence_relative_path)
        evidence_items.append(
            {
                "kind": kind,
                "path": evidence_relative_path,
                "format": "json",
                "status": str(observation.get("status") or "ok"),
                "relatedCapabilityIds": [
                    str(value)
                    for value in list(observation.get("capabilityIds") or [])
                    if str(value)
                ],
            }
        )

    report_payload = {
        "reportVersion": "v1",
        "subjectId": str(request["selection"]["subjectId"]),
        "matrixId": str(request["selection"]["matrixId"]),
        "status": "ok" if completed.returncode == 0 and evidence_items else "fail",
        "evidenceItems": evidence_items,
    }
    write_json(_resolve(repo_root, request["paths"]["reportPaths"][0]), report_payload)

    engine_observation_summary = {
        "evidenceItems": evidence_items,
        "localReportPaths": [request["paths"]["reportPaths"][0]],
    }
    manifest = {
        "subjectId": str(request["selection"]["subjectId"]),
        "matrixId": str(request["selection"]["matrixId"]),
        "bucket": "runtime",
        "variant": _selection_variant(dict(request["selection"])),
        "buildManifestPath": str(request["upstream"]["build"]["manifestPath"]),
        "stdoutPath": _relative(repo_root, stdout_path),
        "stderrPath": _relative(repo_root, stderr_path),
        "exitCodePath": _relative(repo_root, exit_code_path),
        "tracePaths": evidence_paths,
        "engineEvidencePaths": evidence_paths,
        "engineObservationSummary": engine_observation_summary,
    }
    write_json(_resolve(repo_root, request["paths"]["manifestPath"]), manifest)

    if completed.returncode != 0 or not evidence_items:
        failure_reason = (
            f"engine proof run failed: {native_executable_path}"
            if completed.returncode != 0
            else "engine proof run did not emit observation payload"
        )
        return {
            "status": "fail",
            "bucketManifestPath": request["paths"]["manifestPath"],
            "reportPaths": list(request["paths"]["reportPaths"]),
            "primaryEvidencePaths": evidence_paths or [manifest["stdoutPath"], manifest["exitCodePath"]],
            "metrics": {"durationMs": duration_ms},
            "diagnostics": {"stdoutPath": manifest["stdoutPath"], "stderrPath": manifest["stderrPath"]},
            "details": {"engineObservationSummary": engine_observation_summary},
            "failure": failure_reason,
        }

    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=list(request["paths"]["reportPaths"]),
        primary_evidence_paths=evidence_paths,
        stdout_path=manifest["stdoutPath"],
        stderr_path=manifest["stderrPath"],
        duration_ms=duration_ms,
        details={"engineObservationSummary": engine_observation_summary},
    )


def run_runtime_engine_trace_compare(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    subject_id = str(request["selection"]["subjectId"])
    matrix_id = str(request["selection"]["matrixId"])
    subject_manifest = subjects_module.load_subject_manifest(repo_root, subject_id)
    expected_root_text = str(dict(subject_manifest.get("expected") or {}).get("runtime") or "")
    if not expected_root_text:
        raise RuntimeError(f"subject manifest missing expected.runtime for engine trace compare: {subject_id}")

    runtime_manifest_path = _resolve(repo_root, request["paths"]["manifestPath"])
    runtime_manifest = read_json(runtime_manifest_path) if runtime_manifest_path.is_file() else {}
    if not isinstance(runtime_manifest, dict):
        raise RuntimeError("runtime manifest must be an object")

    observation_summary = dict(runtime_manifest.get("engineObservationSummary") or {})
    evidence_items = [dict(item) for item in list(observation_summary.get("evidenceItems") or [])]
    if not evidence_items:
        raise RuntimeError("engine runtime trace compare requires observed evidence items")

    expected_root = _resolve(repo_root, expected_root_text) / matrix_id
    comparisons: list[dict[str, Any]] = []
    primary_evidence_paths: list[str] = []
    failed = False
    for item in evidence_items:
        observed_relative_path = str(item.get("path") or "")
        if not observed_relative_path:
            continue

        primary_evidence_paths.append(observed_relative_path)
        observed_path = _resolve(repo_root, observed_relative_path)
        expected_path = expected_root / Path(observed_relative_path).name

        observed_payload = read_json(observed_path)
        if not expected_path.is_file():
            failed = True
            comparisons.append(
                {
                    "kind": str(item.get("kind") or ""),
                    "status": "missing-expected",
                    "observedPath": observed_relative_path,
                    "expectedPath": _relative(repo_root, expected_path),
                }
            )
            continue

        expected_payload = read_json(expected_path)
        matched = observed_payload == expected_payload
        if not matched:
            failed = True

        comparisons.append(
            {
                "kind": str(item.get("kind") or ""),
                "status": "ok" if matched else "mismatch",
                "observedPath": observed_relative_path,
                "expectedPath": _relative(repo_root, expected_path),
            }
        )

    report_payload = {
        "reportVersion": "v1",
        "subjectId": subject_id,
        "matrixId": matrix_id,
        "status": "fail" if failed else "ok",
        "expectedRootPath": _relative(repo_root, expected_root),
        "comparisons": comparisons,
    }
    write_json(_resolve(repo_root, request["paths"]["reportPaths"][0]), report_payload)

    runtime_manifest["engineTraceCompareReportPaths"] = [
        *[
            str(value)
            for value in list(runtime_manifest.get("engineTraceCompareReportPaths") or [])
            if str(value)
        ],
        request["paths"]["reportPaths"][0],
    ]
    write_json(runtime_manifest_path, runtime_manifest)

    if failed:
        return {
            "status": "fail",
            "bucketManifestPath": request["paths"]["manifestPath"],
            "reportPaths": list(request["paths"]["reportPaths"]),
            "primaryEvidencePaths": primary_evidence_paths,
            "metrics": {"durationMs": 0},
            "diagnostics": {
                "stdoutPath": runtime_manifest.get("stdoutPath"),
                "stderrPath": runtime_manifest.get("stderrPath"),
            },
            "failure": f"engine trace compare failed: {subject_id}/{matrix_id}",
        }

    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=list(request["paths"]["reportPaths"]),
        primary_evidence_paths=primary_evidence_paths,
        stdout_path=str(runtime_manifest.get("stdoutPath") or "") or None,
        stderr_path=str(runtime_manifest.get("stderrPath") or "") or None,
    )


def run_managed_runtime_output(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    host_input_manifest = read_json(_resolve(repo_root, request["upstream"]["host-input"]["manifestPath"]))
    if not isinstance(host_input_manifest, dict):
        raise RuntimeError("host-input manifest must be an object")

    selection = dict(request["selection"])
    native_primary_assembly_path_text = str(host_input_manifest["primaryAssemblyPath"])
    managed_runtime_assembly_path_text = str(host_input_manifest.get("managedRuntimeAssemblyPath") or "").strip()
    assembly_path = _resolve(
        repo_root,
        managed_runtime_assembly_path_text or native_primary_assembly_path_text,
    )
    runtime_root = _resolve(repo_root, request["paths"]["bucketRoot"])
    runtime_root.mkdir(parents=True, exist_ok=True)
    subject_entry_selection = _selection_subject_entry_selection(selection)
    declared_entry_selection = _selection_declared_entry_selection(selection)
    collection_path = str(host_input_manifest.get("collectionPath") or "").strip()
    binding_manifest_path = str(host_input_manifest.get("bindingManifestPath") or "").strip()
    host_kind = str(host_input_manifest.get("hostKind") or "").strip()
    host_execution_model = str(host_input_manifest.get("hostExecutionModel") or "").strip()
    if host_kind in {"proof-host", "benchmark-host"} and collection_path and host_execution_model == SHARED_RUNTIME_HOST_EXECUTION_MODEL:
        runtime_arguments = _selection_runtime_arguments(selection)
        entry_index = declared_entry_selection.get("entryIndex")
        if isinstance(entry_index, bool) or not isinstance(entry_index, int) or entry_index < 0:
            raise RuntimeError("managed host requires selection.entrySelection.entryIndex")
        runtime_arguments.extend(
            [
                f"{CHAOS_HOST_KIND_ARGUMENT_PREFIX}{'proof' if host_kind == 'proof-host' else 'benchmark'}",
                f"{CHAOS_COLLECTION_PATH_ARGUMENT_PREFIX}{collection_path}",
                *([f"--binding-manifest-path={binding_manifest_path}"] if binding_manifest_path else []),
                f"{CHAOS_ENTRY_INDEX_ARGUMENT_PREFIX}{int(entry_index)}",
            ]
        )
    elif host_kind == "proof-host" and collection_path:
        runtime_arguments = _selection_runtime_arguments(selection)
        entry_index = declared_entry_selection.get("entryIndex")
        if isinstance(entry_index, bool) or not isinstance(entry_index, int) or entry_index < 0:
            raise RuntimeError("managed proof host requires selection.entrySelection.entryIndex")
        runtime_arguments.extend(
            [
                f"{CHAOS_COLLECTION_PATH_ARGUMENT_PREFIX}{collection_path}",
                *([f"--binding-manifest-path={binding_manifest_path}"] if binding_manifest_path else []),
                f"{CHAOS_ENTRY_INDEX_ARGUMENT_PREFIX}{int(entry_index)}",
            ]
        )
    else:
        runtime_arguments = _selection_managed_runtime_arguments(selection)

    completed = run_process(["dotnet", str(assembly_path), *runtime_arguments], cwd=repo_root)
    stdout_path = runtime_root / "stdout.log"
    stderr_path = runtime_root / "stderr.log"
    exit_code_path = runtime_root / "exit-code.txt"
    stdout_path.write_text(completed.stdout or "", encoding="utf-8")
    stderr_path.write_text(completed.stderr or "", encoding="utf-8")
    exit_code_path.write_text(f"{completed.returncode}\n", encoding="utf-8")

    output_lines = [line for line in (completed.stdout or "").splitlines() if line.strip()]
    manifest = {
        "subjectId": str(request["selection"]["subjectId"]),
        "matrixId": str(request["selection"]["matrixId"]),
        "bucket": "runtime",
        "variant": _selection_variant(dict(request["selection"])),
        "hostInputManifestPath": str(request["upstream"]["host-input"]["manifestPath"]),
        "stdoutPath": _relative(repo_root, stdout_path),
        "stderrPath": _relative(repo_root, stderr_path),
        "exitCodePath": _relative(repo_root, exit_code_path),
        "arguments": list(runtime_arguments),
        "outputLines": output_lines,
    }
    if subject_entry_selection and not (host_kind == "proof-host" and collection_path):
        manifest["subjectEntrySelection"] = subject_entry_selection
    if declared_entry_selection:
        manifest["declaredEntrySelection"] = declared_entry_selection
    if binding_manifest_path:
        manifest["bindingManifestPath"] = binding_manifest_path
    if managed_runtime_assembly_path_text:
        manifest["managedRuntimeAssemblyPath"] = managed_runtime_assembly_path_text
        manifest["nativePrimaryAssemblyPath"] = native_primary_assembly_path_text
    generated_upstream = dict(request.get("upstream", {}).get("generated") or {})
    build_upstream = dict(request.get("upstream", {}).get("build") or {})
    generated_manifest_path = str(generated_upstream.get("manifestPath") or "").strip()
    build_manifest_path = str(build_upstream.get("manifestPath") or "").strip()
    if generated_manifest_path:
        manifest["nativeGeneratedManifestPath"] = generated_manifest_path
    if build_manifest_path:
        manifest["nativeBuildManifestPath"] = build_manifest_path
    write_json(_resolve(repo_root, request["paths"]["manifestPath"]), manifest)

    if completed.returncode != 0:
        return {
            "status": "fail",
            "bucketManifestPath": request["paths"]["manifestPath"],
            "reportPaths": list(request["paths"]["reportPaths"]),
            "primaryEvidencePaths": [manifest["stdoutPath"]],
            "metrics": {"durationMs": 0},
            "diagnostics": {"stdoutPath": manifest["stdoutPath"], "stderrPath": manifest["stderrPath"]},
            "failure": f"managed runtime execution failed: {assembly_path}",
        }

    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=list(request["paths"]["reportPaths"]),
        primary_evidence_paths=[manifest["stdoutPath"]],
        stdout_path=manifest["stdoutPath"],
        stderr_path=manifest["stderrPath"],
    )


def _perf_sample_count(runtime_profile: str) -> int:
    return 3 if "release" in runtime_profile else 1


def _perf_harness_iterations(runtime_profile: str) -> int:
    return 1000 if "release" in runtime_profile else 100


def _subject_perf_iterations(
    *,
    subject_id: str,
    validation_spec: dict[str, Any],
    default_iterations: int,
) -> int:
    override = validation_spec.get("harnessIterations")
    if override is None:
        return default_iterations
    if isinstance(override, bool) or not isinstance(override, int) or override <= 0:
        raise RuntimeError(
            f"validation.perf.harnessIterations must be a positive integer for subject: {subject_id}"
        )
    return override


def _native_perf_warmup_count(runtime_profile: str) -> int:
    return 1 if "native-perf" in runtime_profile else 0


def _is_numeric_perf_value(value: Any) -> bool:
    return isinstance(value, (int, float)) and not isinstance(value, bool)


def _parse_perf_payload(output_lines: list[str]) -> dict[str, Any]:
    if not output_lines:
        return {}

    # Scan from the end — the JSON payload should be emitted by the benchmark
    # binary, but runtime shutdown logging (GC stats, etc.) may appear after
    # it on stdout.  Linear scan backwards avoids brittle "last line" logic.
    for line in reversed(output_lines):
        try:
            payload = json.loads(line)
        except ValueError:
            continue
        if isinstance(payload, dict):
            return dict(payload)
    return {}


def _payload_custom_perf_metrics(payload: dict[str, Any]) -> dict[str, float]:
    metrics = dict(payload.get("metrics") or {})
    if not metrics:
        metrics = {
            str(metric_name): metric_value
            for metric_name, metric_value in payload.items()
            if str(metric_name)
            not in {"harness", "mode", "subjectId", "workloadEntry", "iterations"}
        }
    custom_metrics: dict[str, float] = {}
    for metric_name, metric_value in metrics.items():
        key = str(metric_name or "").strip()
        if not key or not _is_numeric_perf_value(metric_value):
            continue

        custom_metrics[key] = round(float(metric_value), 3)

    return custom_metrics


def _summary_metric_name(prefix: str, metric_name: str) -> str:
    if not metric_name:
        return prefix
    return f"{prefix}{metric_name[0].upper()}{metric_name[1:]}"


def _perf_summary_metrics(samples: list[dict[str, Any]]) -> dict[str, float | int]:
    counted_samples = [sample for sample in samples if bool(sample.get("countedInSummary", True))]
    durations = [
        float(sample["durationMs"])
        for sample in counted_samples
        if _is_numeric_perf_value(sample.get("durationMs"))
    ]
    summary_metrics: dict[str, float | int] = {
        "sampleCount": len(durations),
        "meanDurationMs": round(statistics.fmean(durations), 3) if durations else 0.0,
        "minDurationMs": round(min(durations), 3) if durations else 0.0,
        "maxDurationMs": round(max(durations), 3) if durations else 0.0,
    }

    reserved_keys = {"sampleIndex", "durationMs", "exitCode", "countedInSummary"}
    custom_metric_names = sorted(
        {
            str(metric_name)
            for sample in counted_samples
            for metric_name, metric_value in sample.items()
            if metric_name not in reserved_keys and _is_numeric_perf_value(metric_value)
        }
    )
    for metric_name in custom_metric_names:
        metric_values = [
            float(sample[metric_name])
            for sample in counted_samples
            if _is_numeric_perf_value(sample.get(metric_name))
        ]
        if not metric_values:
            continue

        summary_metrics[_summary_metric_name("mean", metric_name)] = round(statistics.fmean(metric_values), 3)
        summary_metrics[_summary_metric_name("min", metric_name)] = round(min(metric_values), 3)
        summary_metrics[_summary_metric_name("max", metric_name)] = round(max(metric_values), 3)

    return summary_metrics


def _perf_harness_command(
    *,
    harness_dll_path: Path,
    iterations: int,
    assembly_path: Path | None,
    workload_entry: str,
    mode: str,
    declared_benchmark: dict[str, Any] | None = None,
    host_assembly_path: Path | None = None,
    host_kind: str = "",
    host_execution_model: str = "",
    collection_path: str = "",
    binding_manifest_path: str = "",
    entry_index: int | None = None,
) -> list[str]:
    arguments = ["dotnet", str(harness_dll_path), str(iterations)]
    if host_assembly_path is not None and collection_path and isinstance(entry_index, int) and entry_index >= 0:
        arguments.extend(["--host-assembly", str(host_assembly_path)])
        if host_execution_model == SHARED_RUNTIME_HOST_EXECUTION_MODEL and host_kind in {"proof-host", "benchmark-host"}:
            arguments.extend(["--host-kind", "proof" if host_kind == "proof-host" else "benchmark"])
        arguments.extend(["--collection-path", collection_path])
        arguments.extend(["--entry-index", str(entry_index)])
        if binding_manifest_path:
            arguments.extend(["--binding-manifest-path", binding_manifest_path])
        if mode:
            arguments.extend(["--mode", mode])
        return arguments

    if assembly_path is not None:
        arguments.extend(["--assembly", str(assembly_path)])

    benchmark_meta = dict(declared_benchmark or {})
    assembly_name = str(benchmark_meta.get("assemblyName") or "").strip()
    declaring_type = str(benchmark_meta.get("declaringType") or "").strip()
    method_name = str(benchmark_meta.get("methodName") or "").strip()
    method_signature = str(benchmark_meta.get("methodSignature") or "").strip()
    if assembly_name and declaring_type and method_name:
        arguments.extend(["--assembly-name", assembly_name])
        arguments.extend(["--declaring-type", declaring_type])
        arguments.extend(["--method-name", method_name])
        if method_signature:
            arguments.extend(["--method-signature", method_signature])
    elif workload_entry:
        arguments.extend(["--workload-entry", workload_entry])
    if mode:
        arguments.extend(["--mode", mode])
    return arguments


def _native_perf_command(
    *,
    native_executable_path: Path,
    iterations: int,
    entry_index: int | None = None,
) -> list[str]:
    arguments = [str(native_executable_path)]
    if iterations > 1:
        arguments.extend(["--iterations", str(iterations)])
    if isinstance(entry_index, int) and not isinstance(entry_index, bool) and entry_index >= 0:
        arguments.extend(["--entry-index", str(entry_index)])
    return arguments


def run_runtime_perf_collect(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    host_input_manifest = read_json(_resolve(repo_root, request["upstream"]["host-input"]["manifestPath"]))
    if not isinstance(host_input_manifest, dict):
        raise RuntimeError("host-input manifest must be an object")

    selection = dict(request["selection"])
    execution_context = dict(selection.get("executionContext") or {})
    subject_id = str(selection["subjectId"])
    matrix_id = str(selection["matrixId"])
    variant = _selection_variant(selection)
    declared_benchmark = _resolve_declared_benchmark(selection, repo_root=repo_root, subject_id=subject_id)
    workload_entry = str(declared_benchmark.get("workloadEntry") or _selection_workload_entry(selection))
    if not workload_entry:
        source_entry = str(selection.get("source", {}).get("entry") or "")
        if source_entry:
            workload_entry = source_entry
    host_platform = _normalize_host_platform(str(execution_context.get("hostPlatform") or ""))
    runtime_profile = str(execution_context.get("runtimeProfile") or "")
    sample_count = _perf_sample_count(runtime_profile)
    manifest = subjects_module.load_subject_manifest(repo_root, subject_id)
    validation_spec = subjects_module.find_validation(manifest, "perf")
    iterations = _subject_perf_iterations(
        subject_id=subject_id,
        validation_spec=validation_spec,
        default_iterations=_perf_harness_iterations(runtime_profile),
    )
    perf_project_path = str(validation_spec.get("project") or "")
    if not perf_project_path:
        raise RuntimeError(f"perf validation project missing for subject: {subject_id}")

    project_path = _resolve(repo_root, perf_project_path)
    runtime_root = _resolve(repo_root, request["paths"]["bucketRoot"])
    runtime_root.mkdir(parents=True, exist_ok=True)
    harness_root = runtime_root / "harness"
    harness_dll_path = harness_root / f"{project_path.stem}.dll"
    workload_assembly_path_text, workload_assembly_path = _resolve_workload_assembly_path(
        repo_root,
        host_input_manifest,
        workload_entry,
        assembly_name=str(declared_benchmark.get("assemblyName") or ""),
    )
    host_kind = str(host_input_manifest.get("hostKind") or "").strip()
    host_execution_model = str(host_input_manifest.get("hostExecutionModel") or "").strip()
    collection_path = str(host_input_manifest.get("collectionPath") or "").strip()
    binding_manifest_path = str(host_input_manifest.get("bindingManifestPath") or "").strip()
    host_assembly_path = None
    if host_kind == "benchmark-host" and collection_path:
        host_assembly_path = _resolve(repo_root, str(host_input_manifest["primaryAssemblyPath"]))

    stdout_path = runtime_root / "stdout.log"
    stderr_path = runtime_root / "stderr.log"
    exit_code_path = runtime_root / "exit-code.txt"

    _run_checked(
        [
            "dotnet",
            "build",
            str(project_path),
            "-c",
            "Release",
            "-m:1",
            "-o",
            str(harness_root),
            *_dotnet_intermediate_args(project_path.stem, host_platform),
        ],
        repo_root=repo_root,
        failure_message=f"dotnet build failed: {perf_project_path}",
    )

    samples: list[dict[str, Any]] = []
    stdout_chunks: list[str] = []
    stderr_chunks: list[str] = []
    output_lines: list[str] = []
    last_exit_code = 0
    declared_entry_selection = _selection_declared_entry_selection(selection)
    collection_entry_index = declared_entry_selection.get("entryIndex")
    if not (isinstance(collection_entry_index, int) and not isinstance(collection_entry_index, bool) and collection_entry_index >= 0):
        candidate_entry_index = declared_benchmark.get("entryIndex")
        if isinstance(candidate_entry_index, int) and not isinstance(candidate_entry_index, bool) and candidate_entry_index >= 0:
            collection_entry_index = int(candidate_entry_index)
        else:
            collection_entry_index = None
    benchmark_resolution_kind = _benchmark_resolution_kind(
        workload_entry=workload_entry,
        declared_benchmark=declared_benchmark or None,
        host_kind=host_kind,
        collection_path=collection_path,
        entry_index=collection_entry_index,
    )

    for sample_index in range(sample_count):
        started = time.perf_counter()
        completed = run_process(
            _perf_harness_command(
                harness_dll_path=harness_dll_path,
                iterations=iterations,
                assembly_path=workload_assembly_path,
                workload_entry=workload_entry,
                mode="managed",
                declared_benchmark=declared_benchmark or None,
                host_assembly_path=host_assembly_path,
                host_kind=host_kind,
                host_execution_model=host_execution_model,
                collection_path=collection_path,
                binding_manifest_path=binding_manifest_path,
                entry_index=collection_entry_index,
            ),
            cwd=repo_root,
        )
        duration_ms = round((time.perf_counter() - started) * 1000, 3)
        stdout_text = completed.stdout or ""
        stderr_text = completed.stderr or ""
        output_lines = [line for line in stdout_text.splitlines() if line.strip()]
        last_exit_code = int(completed.returncode)
        payload = _parse_perf_payload(output_lines)
        if _is_numeric_perf_value(payload.get("elapsedMilliseconds")):
            duration_ms = round(float(payload["elapsedMilliseconds"]), 3)

        sample = {
            "sampleIndex": sample_index + 1,
            "durationMs": duration_ms,
            "exitCode": last_exit_code,
        }
        sample.update(_payload_custom_perf_metrics(payload))
        samples.append(sample)
        stdout_chunks.append(f"=== sample {sample_index + 1} ({duration_ms:.3f} ms) ===\n{stdout_text}".rstrip() + "\n")
        if stderr_text:
            stderr_chunks.append(f"=== sample {sample_index + 1} ({duration_ms:.3f} ms) ===\n{stderr_text}".rstrip() + "\n")
        if last_exit_code != 0:
            break

    stdout_path.write_text("".join(stdout_chunks), encoding="utf-8")
    stderr_path.write_text("".join(stderr_chunks), encoding="utf-8")
    exit_code_path.write_text(f"{last_exit_code}\n", encoding="utf-8")

    summary_metrics = _perf_summary_metrics(samples)
    perf_result = perf_module.evaluate_perf_subject(
        repo_root=repo_root,
        subject_id=subject_id,
        matrix_id=matrix_id,
        host_platform=host_platform,
        metrics=summary_metrics,
        update_baseline=False,
    )
    performance = {
        "samples": samples,
        "metrics": dict(perf_result["metrics"]),
        "baselinePath": str(perf_result["baselinePath"]),
        "baseline": dict(perf_result["baseline"]),
        "baselineUpdated": bool(perf_result["baselineUpdated"]),
        "regressionStatus": str(perf_result["regressionStatus"]),
        "regressions": list(perf_result.get("regressions") or []),
    }
    manifest = {
        "subjectId": subject_id,
        "matrixId": matrix_id,
        "bucket": "runtime",
        "variant": variant,
        "hostInputManifestPath": str(request["upstream"]["host-input"]["manifestPath"]),
        "workloadEntry": workload_entry,
        "benchmarkResolutionKind": benchmark_resolution_kind,
        "workloadAssemblyPath": workload_assembly_path_text,
        "perfHarnessProjectPath": perf_project_path,
        "perfHarnessDllPath": _relative(repo_root, harness_dll_path),
        "stdoutPath": _relative(repo_root, stdout_path),
        "stderrPath": _relative(repo_root, stderr_path),
        "exitCodePath": _relative(repo_root, exit_code_path),
        "outputLines": output_lines,
        "samples": samples,
        "summaryMetrics": dict(performance["metrics"]),
        "baselinePath": str(performance["baselinePath"]),
        "baseline": dict(performance["baseline"]),
        "baselineUpdated": bool(performance["baselineUpdated"]),
        "regressionStatus": str(performance["regressionStatus"]),
        "regressions": list(performance["regressions"]),
    }
    if declared_entry_selection:
        manifest["declaredEntrySelection"] = declared_entry_selection
    if declared_benchmark:
        manifest["declaredBenchmark"] = declared_benchmark
    if host_kind == "benchmark-host" and collection_path:
        manifest["hostKind"] = host_kind
        manifest["collectionPath"] = collection_path
    if binding_manifest_path:
        manifest["bindingManifestPath"] = binding_manifest_path
    write_json(_resolve(repo_root, request["paths"]["manifestPath"]), manifest)

    if last_exit_code != 0:
        return {
            "status": "fail",
            "bucketManifestPath": request["paths"]["manifestPath"],
            "reportPaths": list(request["paths"]["reportPaths"]),
            "primaryEvidencePaths": [manifest["stdoutPath"]],
            "metrics": {"durationMs": int(round(sum(float(sample["durationMs"]) for sample in samples)))},
            "diagnostics": {"stdoutPath": manifest["stdoutPath"], "stderrPath": manifest["stderrPath"]},
            "details": {"performance": performance},
            "failure": f"managed perf execution failed: {harness_dll_path}",
        }

    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=list(request["paths"]["reportPaths"]),
        primary_evidence_paths=[manifest["stdoutPath"]],
        stdout_path=manifest["stdoutPath"],
        stderr_path=manifest["stderrPath"],
        duration_ms=int(round(sum(float(sample["durationMs"]) for sample in samples))),
        details={"performance": performance},
    )


def run_native_runtime_perf(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    build_manifest = read_json(_resolve(repo_root, request["upstream"]["build"]["manifestPath"]))
    if not isinstance(build_manifest, dict):
        raise RuntimeError("build manifest must be an object")

    selection = dict(request["selection"])
    execution_context = dict(selection.get("executionContext") or {})
    subject_id = str(selection["subjectId"])
    matrix_id = str(selection["matrixId"])
    variant = _selection_variant(selection)
    declared_benchmark = _resolve_declared_benchmark(selection, repo_root=repo_root, subject_id=subject_id)
    workload_entry = str(declared_benchmark.get("workloadEntry") or _selection_workload_entry(selection))
    host_platform = _normalize_host_platform(str(execution_context.get("hostPlatform") or ""))
    runtime_profile = str(execution_context.get("runtimeProfile") or "")
    sample_count = _perf_sample_count(runtime_profile)
    warmup_sample_count = _native_perf_warmup_count(runtime_profile)
    manifest = subjects_module.load_subject_manifest(repo_root, subject_id)
    validation_spec = subjects_module.find_validation(manifest, "perf")
    iterations = _subject_perf_iterations(
        subject_id=subject_id,
        validation_spec=validation_spec,
        default_iterations=_perf_harness_iterations(runtime_profile),
    )
    declared_entry_selection = _selection_declared_entry_selection(selection)
    collection_entry_index = declared_entry_selection.get("entryIndex")
    if not (isinstance(collection_entry_index, int) and not isinstance(collection_entry_index, bool) and collection_entry_index >= 0):
        candidate_entry_index = declared_benchmark.get("entryIndex")
        if isinstance(candidate_entry_index, int) and not isinstance(candidate_entry_index, bool) and candidate_entry_index >= 0:
            collection_entry_index = int(candidate_entry_index)
        else:
            collection_entry_index = None
    output_paths = [str(value) for value in list(build_manifest.get("outputs") or []) if str(value)]
    if not output_paths:
        raise RuntimeError("native perf build manifest missing outputs")

    native_executable_path = _resolve(repo_root, output_paths[0])
    runtime_root = _resolve(repo_root, request["paths"]["bucketRoot"])
    runtime_root.mkdir(parents=True, exist_ok=True)

    stdout_path = runtime_root / "stdout.log"
    stderr_path = runtime_root / "stderr.log"
    exit_code_path = runtime_root / "exit-code.txt"
    perf_runtime_path = runtime_root / "perf.runtime.json"
    perf_samples_path = runtime_root / "perf.samples.json"

    samples: list[dict[str, Any]] = []
    stdout_chunks: list[str] = []
    stderr_chunks: list[str] = []
    output_lines: list[str] = []
    last_exit_code = 0

    for sample_index in range(sample_count + warmup_sample_count):
        counted_in_summary = sample_index >= warmup_sample_count
        measured_sample_index = sample_index - warmup_sample_count + 1
        sample_label = (
            f"warmup sample {sample_index + 1}"
            if not counted_in_summary
            else f"sample {measured_sample_index}"
        )
        started = time.perf_counter()
        completed = run_process(
            _native_perf_command(
                native_executable_path=native_executable_path,
                iterations=iterations,
                entry_index=collection_entry_index,
            ),
            cwd=repo_root,
        )
        duration_ms = round((time.perf_counter() - started) * 1000, 3)
        stdout_text = completed.stdout or ""
        stderr_text = completed.stderr or ""
        output_lines = [line for line in stdout_text.splitlines() if line.strip()]
        last_exit_code = int(completed.returncode)
        payload = _parse_perf_payload(output_lines)
        if _is_numeric_perf_value(payload.get("elapsedMilliseconds")):
            duration_ms = round(float(payload["elapsedMilliseconds"]), 3)

        sample = {
            "sampleIndex": sample_index + 1,
            "durationMs": duration_ms,
            "exitCode": last_exit_code,
            "countedInSummary": counted_in_summary,
        }
        sample.update(_payload_custom_perf_metrics(payload))
        samples.append(sample)
        stdout_chunks.append(f"=== {sample_label} ({duration_ms:.3f} ms) ===\n{stdout_text}".rstrip() + "\n")
        if stderr_text:
            stderr_chunks.append(f"=== {sample_label} ({duration_ms:.3f} ms) ===\n{stderr_text}".rstrip() + "\n")
        if last_exit_code != 0:
            break

    stdout_path.write_text("".join(stdout_chunks), encoding="utf-8")
    stderr_path.write_text("".join(stderr_chunks), encoding="utf-8")
    exit_code_path.write_text(f"{last_exit_code}\n", encoding="utf-8")

    summary_metrics = _perf_summary_metrics(samples)
    perf_result = perf_module.evaluate_perf_subject(
        repo_root=repo_root,
        subject_id=subject_id,
        matrix_id=matrix_id,
        host_platform=host_platform,
        metrics=summary_metrics,
        update_baseline=False,
    )
    runtime_evidence = {
        "runtimePath": _relative(repo_root, perf_runtime_path),
        "samplesPath": _relative(repo_root, perf_samples_path),
    }
    performance = {
        "samples": samples,
        "warmupSampleCount": warmup_sample_count,
        "metrics": dict(perf_result["metrics"]),
        "baselinePath": str(perf_result["baselinePath"]),
        "baseline": dict(perf_result["baseline"]),
        "baselineUpdated": bool(perf_result["baselineUpdated"]),
        "regressionStatus": str(perf_result["regressionStatus"]),
        "regressions": list(perf_result.get("regressions") or []),
        "runtimeEvidence": runtime_evidence,
    }
    write_json(
        perf_runtime_path,
        {
            "reportVersion": "v1",
            "subjectId": subject_id,
            "matrixId": matrix_id,
            "variant": variant,
            "nativeExecutablePath": _relative(repo_root, native_executable_path),
            "harnessIterations": iterations,
            "warmupSampleCount": warmup_sample_count,
            "metrics": dict(performance["metrics"]),
            "baselinePath": str(performance["baselinePath"]),
            "baseline": dict(performance["baseline"]),
            "baselineUpdated": bool(performance["baselineUpdated"]),
            "regressionStatus": str(performance["regressionStatus"]),
            "regressions": list(performance["regressions"]),
            "stdoutPath": _relative(repo_root, stdout_path),
            "stderrPath": _relative(repo_root, stderr_path),
        },
    )
    write_json(
        perf_samples_path,
        {
            "reportVersion": "v1",
            "subjectId": subject_id,
            "matrixId": matrix_id,
            "warmupSampleCount": warmup_sample_count,
            "samples": samples,
        },
    )

    manifest = {
        "subjectId": subject_id,
        "matrixId": matrix_id,
        "bucket": "runtime",
        "variant": variant,
        "buildManifestPath": str(request["upstream"]["build"]["manifestPath"]),
        "nativeExecutablePath": _relative(repo_root, native_executable_path),
        "harnessIterations": iterations,
        "stdoutPath": _relative(repo_root, stdout_path),
        "stderrPath": _relative(repo_root, stderr_path),
        "exitCodePath": _relative(repo_root, exit_code_path),
        "perfRuntimePath": runtime_evidence["runtimePath"],
        "perfSamplesPath": runtime_evidence["samplesPath"],
        "outputLines": output_lines,
        "warmupSampleCount": warmup_sample_count,
        "samples": samples,
        "summaryMetrics": dict(performance["metrics"]),
        "baselinePath": str(performance["baselinePath"]),
        "baseline": dict(performance["baseline"]),
        "baselineUpdated": bool(performance["baselineUpdated"]),
        "regressionStatus": str(performance["regressionStatus"]),
        "regressions": list(performance["regressions"]),
    }
    host_kind = str(build_manifest.get("hostKind") or "").strip()
    collection_path = str(build_manifest.get("collectionPath") or "").strip()
    dispatch_manifest_path = str(build_manifest.get("dispatchManifestPath") or "").strip()
    if host_kind:
        manifest["hostKind"] = host_kind
    if collection_path:
        manifest["collectionPath"] = collection_path
    if dispatch_manifest_path:
        manifest["dispatchManifestPath"] = dispatch_manifest_path
    if isinstance(collection_entry_index, int) and collection_entry_index >= 0:
        manifest["entryIndex"] = int(collection_entry_index)
    write_json(_resolve(repo_root, request["paths"]["manifestPath"]), manifest)

    if last_exit_code != 0:
        return {
            "status": "fail",
            "bucketManifestPath": request["paths"]["manifestPath"],
            "reportPaths": list(request["paths"]["reportPaths"]),
            "primaryEvidencePaths": [manifest["perfRuntimePath"], manifest["perfSamplesPath"]],
            "metrics": {"durationMs": int(round(sum(float(sample["durationMs"]) for sample in samples)))},
            "diagnostics": {"stdoutPath": manifest["stdoutPath"], "stderrPath": manifest["stderrPath"]},
            "details": {"performance": performance},
            "failure": f"native perf execution failed: {native_executable_path}",
        }

    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=list(request["paths"]["reportPaths"]),
        primary_evidence_paths=[manifest["perfRuntimePath"], manifest["perfSamplesPath"]],
        stdout_path=manifest["stdoutPath"],
        stderr_path=manifest["stderrPath"],
        duration_ms=int(round(sum(float(sample["durationMs"]) for sample in samples))),
        details={"performance": performance},
    )


def run_mobile_native_perf(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    build_manifest = read_json(_resolve(repo_root, request["upstream"]["build"]["manifestPath"]))
    if not isinstance(build_manifest, dict):
        raise RuntimeError("build manifest must be an object")

    selection = dict(request["selection"])
    execution_context = dict(selection.get("executionContext") or {})
    subject_id = str(selection["subjectId"])
    matrix_id = str(selection["matrixId"])
    target_platform = str(execution_context.get("targetPlatform") or "")
    host_platform = str(execution_context.get("hostPlatform") or "")
    runtime_profile = str(execution_context.get("runtimeProfile") or "")
    variant = _selection_variant(selection)
    sample_count = _perf_sample_count(runtime_profile)
    runtime_arguments = _selection_runtime_arguments(selection)

    runtime_root = _resolve(repo_root, request["paths"]["bucketRoot"])
    runtime_root.mkdir(parents=True, exist_ok=True)

    stdout_path = runtime_root / "stdout.log"
    stderr_path = runtime_root / "stderr.log"
    exit_code_path = runtime_root / "exit-code.txt"
    perf_runtime_path = runtime_root / "perf.runtime.json"
    perf_samples_path = runtime_root / "perf.samples.json"

    if target_platform == "android-arm64":
        output_paths = [str(value) for value in list(build_manifest.get("outputs") or []) if str(value)]
        if not output_paths:
            raise RuntimeError("mobile perf build manifest missing outputs")

        collector_result = mobile_perf_collector.collect_android_perf(
            repo_root=repo_root,
            runtime_root=runtime_root,
            native_executable_path=_resolve(repo_root, output_paths[0]),
            runtime_arguments=runtime_arguments,
            sample_count=sample_count,
            host_platform=host_platform,
        )
    elif target_platform == "ios-arm64":
        cmake_binary_dir_text = str(build_manifest.get("cmakeBinaryDir") or "")
        if not cmake_binary_dir_text:
            raise RuntimeError("mobile perf build manifest missing cmakeBinaryDir")

        collector_result = mobile_perf_collector.collect_ios_perf(
            repo_root=repo_root,
            runtime_root=runtime_root,
            cmake_binary_dir=_resolve(repo_root, cmake_binary_dir_text),
            sample_count=sample_count,
            host_platform=host_platform,
        )
    else:
        raise RuntimeError(f"unsupported mobile target platform: {target_platform}")

    stdout_text = str(collector_result.get("stdout") or "")
    stderr_text = str(collector_result.get("stderr") or "")
    output_lines = [line for line in stdout_text.splitlines() if line.strip()]
    exit_code = int(collector_result.get("exitCode") or 0)
    samples = [
        dict(sample)
        for sample in list(collector_result.get("samples") or [])
        if isinstance(sample, dict)
    ]
    collector_details = dict(collector_result.get("details") or {})
    collector_evidence_paths = [str(value) for value in list(collector_result.get("evidencePaths") or []) if str(value)]

    stdout_path.write_text(stdout_text, encoding="utf-8")
    stderr_path.write_text(stderr_text, encoding="utf-8")
    exit_code_path.write_text(f"{exit_code}\n", encoding="utf-8")

    summary_metrics = _perf_summary_metrics(samples)
    perf_result = perf_module.evaluate_perf_subject(
        repo_root=repo_root,
        subject_id=subject_id,
        matrix_id=matrix_id,
        host_platform=target_platform,
        metrics=summary_metrics,
        update_baseline=False,
    )
    runtime_evidence = {
        "runtimePath": _relative(repo_root, perf_runtime_path),
        "samplesPath": _relative(repo_root, perf_samples_path),
    }
    performance = {
        "samples": samples,
        "metrics": dict(perf_result["metrics"]),
        "baselinePath": str(perf_result["baselinePath"]),
        "baseline": dict(perf_result["baseline"]),
        "baselineUpdated": bool(perf_result["baselineUpdated"]),
        "regressionStatus": str(perf_result["regressionStatus"]),
        "regressions": list(perf_result.get("regressions") or []),
        "runtimeEvidence": runtime_evidence,
        "collectorDetails": collector_details,
        "collectorEvidencePaths": collector_evidence_paths,
    }
    write_json(
        perf_runtime_path,
        {
            "reportVersion": "v1",
            "subjectId": subject_id,
            "matrixId": matrix_id,
            "variant": variant,
            "targetPlatform": target_platform,
            "metrics": dict(performance["metrics"]),
            "baselinePath": str(performance["baselinePath"]),
            "baseline": dict(performance["baseline"]),
            "baselineUpdated": bool(performance["baselineUpdated"]),
            "regressionStatus": str(performance["regressionStatus"]),
            "regressions": list(performance["regressions"]),
            "collectorDetails": collector_details,
            "collectorEvidencePaths": collector_evidence_paths,
            "stdoutPath": _relative(repo_root, stdout_path),
            "stderrPath": _relative(repo_root, stderr_path),
        },
    )
    write_json(
        perf_samples_path,
        {
            "reportVersion": "v1",
            "subjectId": subject_id,
            "matrixId": matrix_id,
            "targetPlatform": target_platform,
            "samples": samples,
        },
    )

    manifest = {
        "subjectId": subject_id,
        "matrixId": matrix_id,
        "bucket": "runtime",
        "variant": variant,
        "targetPlatform": target_platform,
        "buildManifestPath": str(request["upstream"]["build"]["manifestPath"]),
        "stdoutPath": _relative(repo_root, stdout_path),
        "stderrPath": _relative(repo_root, stderr_path),
        "exitCodePath": _relative(repo_root, exit_code_path),
        "perfRuntimePath": runtime_evidence["runtimePath"],
        "perfSamplesPath": runtime_evidence["samplesPath"],
        "collectorDetails": collector_details,
        "collectorEvidencePaths": collector_evidence_paths,
        "outputLines": output_lines,
        "samples": samples,
        "summaryMetrics": dict(performance["metrics"]),
        "baselinePath": str(performance["baselinePath"]),
        "baseline": dict(performance["baseline"]),
        "baselineUpdated": bool(performance["baselineUpdated"]),
        "regressionStatus": str(performance["regressionStatus"]),
        "regressions": list(performance["regressions"]),
    }
    write_json(_resolve(repo_root, request["paths"]["manifestPath"]), manifest)

    failure_reason: str | None = None
    if exit_code != 0:
        failure_reason = f"mobile perf execution failed: {target_platform}"
    elif not samples:
        failure_reason = f"mobile perf collector returned no samples: {target_platform}"

    if failure_reason is not None:
        return {
            "status": "fail",
            "bucketManifestPath": request["paths"]["manifestPath"],
            "reportPaths": list(request["paths"]["reportPaths"]),
            "primaryEvidencePaths": [manifest["perfRuntimePath"], manifest["perfSamplesPath"], *collector_evidence_paths],
            "metrics": {"durationMs": int(round(sum(float(sample.get("durationMs") or 0.0) for sample in samples)))},
            "diagnostics": {"stdoutPath": manifest["stdoutPath"], "stderrPath": manifest["stderrPath"]},
            "details": {"performance": performance},
            "failure": failure_reason,
        }

    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=list(request["paths"]["reportPaths"]),
        primary_evidence_paths=[manifest["perfRuntimePath"], manifest["perfSamplesPath"], *collector_evidence_paths],
        stdout_path=manifest["stdoutPath"],
        stderr_path=manifest["stderrPath"],
        duration_ms=int(round(sum(float(sample.get("durationMs") or 0.0) for sample in samples))),
        details={"performance": performance},
    )


def run_runtime_trace_compare(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    selection = dict(request["selection"])
    host_input_manifest = read_json(_resolve(repo_root, request["upstream"]["host-input"]["manifestPath"]))
    if not isinstance(host_input_manifest, dict):
        raise RuntimeError("host-input manifest must be an object")

    runtime_manifest_path = _resolve(repo_root, request["paths"]["manifestPath"])
    if runtime_manifest_path.is_file():
        runtime_manifest = read_json(runtime_manifest_path)
        if not isinstance(runtime_manifest, dict):
            raise RuntimeError("runtime manifest must be an object")
    else:
        runtime_manifest = {}

    runtime_root = _resolve(repo_root, request["paths"]["bucketRoot"])
    runtime_root.mkdir(parents=True, exist_ok=True)
    trace_path = runtime_root / "trace.runtime.json"
    trace_platform = _trace_platform(selection)
    expected_snapshot_path = _trace_snapshot_path(trace_platform)
    assembly_path = _resolve(repo_root, str(host_input_manifest["primaryAssemblyPath"]))
    _run_checked(
        [
            "dotnet",
            str(assembly_path),
            "--trace-platform",
            trace_platform,
            "--trace-output",
            str(trace_path),
        ],
        repo_root=repo_root,
        failure_message=f"trace export failed: {assembly_path}",
    )
    _run_checked(
        [
            sys.executable,
            str(repo_root / "tests" / "contracts" / "trace" / "compare-warmup-trace.py"),
            str(repo_root / expected_snapshot_path),
            str(trace_path),
        ],
        repo_root=repo_root,
        failure_message=f"{trace_platform} trace compare failed",
    )

    runtime_manifest["subjectId"] = str(selection["subjectId"])
    runtime_manifest["matrixId"] = str(selection["matrixId"])
    runtime_manifest["bucket"] = "runtime"
    runtime_manifest["variant"] = _selection_variant(selection)
    runtime_manifest["hostInputManifestPath"] = str(request["upstream"]["host-input"]["manifestPath"])
    runtime_manifest["tracePaths"] = [_relative(repo_root, trace_path)]
    write_json(runtime_manifest_path, runtime_manifest)
    report_path = _resolve(repo_root, request["paths"]["reportPaths"][0])
    write_json(
        report_path,
        {
            "subjectId": str(selection["subjectId"]),
            "matrixId": str(selection["matrixId"]),
            "status": "ok",
            "variant": _selection_variant(selection),
            "schemaPath": TRACE_SCHEMA_PATH.as_posix(),
            "expectedSnapshotPath": expected_snapshot_path.as_posix(),
            "actualTracePath": _relative(repo_root, trace_path),
            "errors": [],
        },
    )
    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=list(request["paths"]["reportPaths"]),
        primary_evidence_paths=[_relative(repo_root, trace_path)],
    )


def run_interpreter_runtime_perf(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    """Run the managed interpreter performance harness.

    Reads the compiled subject assembly from the host-input stage and invokes the
    interpreter harness project (if one is declared in the subject's manifest perf
    validation spec).  Output format is identical to run_runtime_perf_collect so
    that downstream comparison workers can treat all three modes uniformly.
    """
    host_input_manifest = read_json(_resolve(repo_root, request["upstream"]["host-input"]["manifestPath"]))
    if not isinstance(host_input_manifest, dict):
        raise RuntimeError("host-input manifest must be an object")

    selection = dict(request["selection"])
    execution_context = dict(selection.get("executionContext") or {})
    subject_id = str(selection["subjectId"])
    matrix_id = str(selection["matrixId"])
    variant = _selection_variant(selection)
    declared_benchmark = _resolve_declared_benchmark(selection, repo_root=repo_root, subject_id=subject_id)
    workload_entry = str(declared_benchmark.get("workloadEntry") or _selection_workload_entry(selection))
    host_platform = _normalize_host_platform(str(execution_context.get("hostPlatform") or ""))
    runtime_profile = str(execution_context.get("runtimeProfile") or "")
    sample_count = _perf_sample_count(runtime_profile)

    manifest = subjects_module.load_subject_manifest(repo_root, subject_id)

    # Look for an interpreter-specific harness project.
    # Subject may declare:  "validation": { "perf": { "driver": "interpreter-runtime-perf",
    #                                                  "project": "subjects/.../Harness.csproj" } }
    validation_spec = subjects_module.find_validation(manifest, "perf") or {}
    iterations = _subject_perf_iterations(
        subject_id=subject_id,
        validation_spec=validation_spec,
        default_iterations=_perf_harness_iterations(runtime_profile),
    )
    harness_project_path_str = str(validation_spec.get("project") or "")

    runtime_root = _resolve(repo_root, request["paths"]["bucketRoot"])
    runtime_root.mkdir(parents=True, exist_ok=True)
    stdout_path = runtime_root / "stdout.log"
    stderr_path = runtime_root / "stderr.log"
    exit_code_path = runtime_root / "exit-code.txt"

    if not harness_project_path_str:
        # No interpreter harness configured: return a clearly-labelled skip result so the
        # comparison stage can treat the missing data as "interpreter: N/A" rather than
        # propagating a hard failure.
        msg = (
            f"interpreter-runtime-perf: no harness project configured for subject '{subject_id}'. "
            "Add 'validation.perf.project' to the subject manifest to enable interpreter benchmarking."
        )
        stdout_path.write_text(msg + "\n", encoding="utf-8")
        stderr_path.write_text("", encoding="utf-8")
        exit_code_path.write_text("0\n", encoding="utf-8")

        skip_manifest = {
            "subjectId": subject_id,
            "matrixId": matrix_id,
            "bucket": "runtime",
            "variant": variant,
            "mode": "interpreter",
            "stdoutPath": _relative(repo_root, stdout_path),
            "stderrPath": _relative(repo_root, stderr_path),
            "exitCodePath": _relative(repo_root, exit_code_path),
            "skipped": True,
            "skipReason": "no-harness-configured",
            "samples": [],
            "summaryMetrics": {},
            "regressionStatus": "no-baseline",
        }
        write_json(_resolve(repo_root, request["paths"]["manifestPath"]), skip_manifest)
        return _success_result(
            bucket_manifest_path=request["paths"]["manifestPath"],
            report_paths=list(request["paths"]["reportPaths"]),
            primary_evidence_paths=[_relative(repo_root, stdout_path)],
            stdout_path=_relative(repo_root, stdout_path),
            stderr_path=_relative(repo_root, stderr_path),
            duration_ms=0,
            details={"mode": "interpreter", "skipped": True},
        )

    # Harness project is configured — build and run it.
    project_path = _resolve(repo_root, harness_project_path_str)
    harness_root = runtime_root / "harness"
    harness_dll_path = harness_root / f"{project_path.stem}.dll"
    workload_assembly_path_text, workload_assembly_path = _resolve_workload_assembly_path(
        repo_root,
        host_input_manifest,
        workload_entry,
        assembly_name=str(declared_benchmark.get("assemblyName") or ""),
    )
    host_kind = str(host_input_manifest.get("hostKind") or "").strip()
    host_execution_model = str(host_input_manifest.get("hostExecutionModel") or "").strip()
    collection_path = str(host_input_manifest.get("collectionPath") or "").strip()
    binding_manifest_path = str(host_input_manifest.get("bindingManifestPath") or "").strip()
    host_assembly_path = None
    if host_kind == "benchmark-host" and collection_path:
        host_assembly_path = _resolve(repo_root, str(host_input_manifest["primaryAssemblyPath"]))

    _run_checked(
        [
            "dotnet", "build", str(project_path),
            "-c", "Release",
            "-m:1",
            "-o", str(harness_root),
            *_dotnet_intermediate_args(project_path.stem, host_platform),
        ],
        repo_root=repo_root,
        failure_message=f"interpreter harness build failed: {harness_project_path_str}",
    )

    samples: list[dict[str, Any]] = []
    stdout_chunks: list[str] = []
    stderr_chunks: list[str] = []
    output_lines: list[str] = []
    last_exit_code = 0
    declared_entry_selection = _selection_declared_entry_selection(selection)
    collection_entry_index = declared_entry_selection.get("entryIndex")
    if not (isinstance(collection_entry_index, int) and not isinstance(collection_entry_index, bool) and collection_entry_index >= 0):
        candidate_entry_index = declared_benchmark.get("entryIndex")
        if isinstance(candidate_entry_index, int) and not isinstance(candidate_entry_index, bool) and candidate_entry_index >= 0:
            collection_entry_index = int(candidate_entry_index)
        else:
            collection_entry_index = None
    benchmark_resolution_kind = _benchmark_resolution_kind(
        workload_entry=workload_entry,
        declared_benchmark=declared_benchmark or None,
        host_kind=host_kind,
        collection_path=collection_path,
        entry_index=collection_entry_index,
    )

    for sample_index in range(sample_count):
        started = time.perf_counter()
        completed = run_process(
            _perf_harness_command(
                harness_dll_path=harness_dll_path,
                iterations=iterations,
                assembly_path=workload_assembly_path,
                workload_entry=workload_entry,
                mode="interpreter",
                declared_benchmark=declared_benchmark or None,
                host_assembly_path=host_assembly_path,
                host_kind=host_kind,
                host_execution_model=host_execution_model,
                collection_path=collection_path,
                binding_manifest_path=binding_manifest_path,
                entry_index=collection_entry_index,
            ),
            cwd=repo_root,
        )
        duration_ms = round((time.perf_counter() - started) * 1000, 3)
        stdout_text = completed.stdout or ""
        stderr_text = completed.stderr or ""
        output_lines = [line for line in stdout_text.splitlines() if line.strip()]
        last_exit_code = int(completed.returncode)
        payload = _parse_perf_payload(output_lines)
        if _is_numeric_perf_value(payload.get("elapsedMilliseconds")):
            duration_ms = round(float(payload["elapsedMilliseconds"]), 3)

        sample: dict[str, Any] = {
            "sampleIndex": sample_index + 1,
            "durationMs": duration_ms,
            "exitCode": last_exit_code,
            "mode": "interpreter",
        }
        sample.update(_payload_custom_perf_metrics(payload))
        samples.append(sample)
        stdout_chunks.append(f"=== sample {sample_index + 1} ({duration_ms:.3f} ms) ===\n{stdout_text}".rstrip() + "\n")
        if stderr_text:
            stderr_chunks.append(f"=== sample {sample_index + 1} ===\n{stderr_text}".rstrip() + "\n")
        if last_exit_code != 0:
            break

    stdout_path.write_text("".join(stdout_chunks), encoding="utf-8")
    stderr_path.write_text("".join(stderr_chunks), encoding="utf-8")
    exit_code_path.write_text(f"{last_exit_code}\n", encoding="utf-8")

    summary_metrics = _perf_summary_metrics(samples)
    perf_result = perf_module.evaluate_perf_subject(
        repo_root=repo_root,
        subject_id=subject_id,
        matrix_id=matrix_id,
        host_platform=host_platform,
        metrics=summary_metrics,
        update_baseline=False,
    )
    performance = {
        "samples": samples,
        "metrics": dict(perf_result["metrics"]),
        "baselinePath": str(perf_result["baselinePath"]),
        "baseline": dict(perf_result["baseline"]),
        "baselineUpdated": bool(perf_result["baselineUpdated"]),
        "regressionStatus": str(perf_result["regressionStatus"]),
        "regressions": list(perf_result.get("regressions") or []),
    }
    result_manifest = {
        "subjectId": subject_id,
        "matrixId": matrix_id,
        "bucket": "runtime",
        "variant": variant,
        "mode": "interpreter",
        "workloadEntry": workload_entry,
        "benchmarkResolutionKind": benchmark_resolution_kind,
        "workloadAssemblyPath": workload_assembly_path_text,
        "harnessProjectPath": harness_project_path_str,
        "harnessDllPath": _relative(repo_root, harness_dll_path),
        "stdoutPath": _relative(repo_root, stdout_path),
        "stderrPath": _relative(repo_root, stderr_path),
        "exitCodePath": _relative(repo_root, exit_code_path),
        "outputLines": output_lines,
        "samples": samples,
        "summaryMetrics": dict(performance["metrics"]),
        "baselinePath": str(performance["baselinePath"]),
        "baseline": dict(performance["baseline"]),
        "baselineUpdated": bool(performance["baselineUpdated"]),
        "regressionStatus": str(performance["regressionStatus"]),
        "regressions": list(performance["regressions"]),
    }
    if declared_entry_selection:
        result_manifest["declaredEntrySelection"] = declared_entry_selection
    if declared_benchmark:
        result_manifest["declaredBenchmark"] = declared_benchmark
    if host_kind == "benchmark-host" and collection_path:
        result_manifest["hostKind"] = host_kind
        result_manifest["collectionPath"] = collection_path
    if binding_manifest_path:
        result_manifest["bindingManifestPath"] = binding_manifest_path
    write_json(_resolve(repo_root, request["paths"]["manifestPath"]), result_manifest)

    if last_exit_code != 0:
        return {
            "status": "fail",
            "bucketManifestPath": request["paths"]["manifestPath"],
            "reportPaths": list(request["paths"]["reportPaths"]),
            "primaryEvidencePaths": [result_manifest["stdoutPath"]],
            "metrics": {"durationMs": int(round(sum(float(s["durationMs"]) for s in samples)))},
            "diagnostics": {"stdoutPath": result_manifest["stdoutPath"], "stderrPath": result_manifest["stderrPath"]},
            "details": {"performance": performance},
            "failure": f"interpreter perf execution failed: {harness_dll_path}",
        }

    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=list(request["paths"]["reportPaths"]),
        primary_evidence_paths=[result_manifest["stdoutPath"]],
        stdout_path=result_manifest["stdoutPath"],
        stderr_path=result_manifest["stderrPath"],
        duration_ms=int(round(sum(float(s["durationMs"]) for s in samples))),
        details={"performance": performance},
    )


def run_benchmark_comparison_aggregate(*, repo_root: Path, request: dict[str, Any]) -> dict[str, Any]:
    """Aggregate perf results from managed, native and interpreter stages into a comparison report.

    Upstream buckets expected (any subset is acceptable — missing modes produce null entries):
      - "managed-runtime"   or  "runtime-perf"
      - "native-runtime"    or  "native-perf"
      - "interpreter-runtime"
    """
    import importlib
    benchmark_comparison = importlib.import_module("benchmark_comparison")

    selection = dict(request["selection"])
    subject_id = str(selection["subjectId"])
    matrix_id = str(selection["matrixId"])
    runtime_root = _resolve(repo_root, request["paths"]["bucketRoot"])
    runtime_root.mkdir(parents=True, exist_ok=True)

    def _load_mode_metrics(bucket_key: str) -> dict[str, Any] | None:
        upstream = dict(request.get("upstream") or {})
        entry = upstream.get(bucket_key)
        if not entry:
            return None
        try:
            m = read_json(_resolve(repo_root, entry["manifestPath"]))
            return dict(m.get("summaryMetrics") or {}) if isinstance(m, dict) else None
        except Exception:
            return None

    # Try common upstream bucket names for each mode.
    managed_metrics = _load_mode_metrics("managed-runtime") or _load_mode_metrics("runtime-perf")
    native_metrics = _load_mode_metrics("native-runtime") or _load_mode_metrics("native-perf")
    interpreter_metrics = _load_mode_metrics("interpreter-runtime")

    comparison = benchmark_comparison.compute_comparison(managed_metrics, native_metrics, interpreter_metrics)
    verdict = benchmark_comparison.evaluate_targets(comparison)

    report = {
        "reportVersion": "v1",
        "subjectId": subject_id,
        "matrixId": matrix_id,
        "modes": {
            "managed": managed_metrics,
            "native": native_metrics,
            "interpreter": interpreter_metrics,
        },
        "comparison": comparison,
        "verdict": verdict,
    }

    comparison_path = runtime_root / "comparison.json"
    write_json(comparison_path, report)

    write_json(_resolve(repo_root, request["paths"]["manifestPath"]), {
        "subjectId": subject_id,
        "matrixId": matrix_id,
        "bucket": "report",
        "comparisonPath": _relative(repo_root, comparison_path),
        "overallPass": bool(verdict.get("overallPass")),
    })

    return _success_result(
        bucket_manifest_path=request["paths"]["manifestPath"],
        report_paths=list(request["paths"]["reportPaths"]),
        primary_evidence_paths=[_relative(repo_root, comparison_path)],
        details={"comparison": comparison, "verdict": verdict},
    )


# ── Foundation-DLL Family Verification Worker ──────────────────────────
# Wraps the existing verify_family() 13-stage pipeline as a subject worker.
# Expects source.type == "foundation-dll-family" with source.familySlug
# and source.assembly.
def run_family_verify_foundation_dll(
    *, repo_root: Path, request: dict[str, Any]
) -> dict[str, Any]:
    source = dict(request["selection"]["source"])
    family_slug = str(source.get("familySlug") or "")
    assembly = str(source.get("assembly") or "System.Private.CoreLib")

    if not family_slug:
        return _failure_result(
            bucket_manifest_path=str(request["paths"]["manifestPath"]),
            report_paths=list(request["paths"]["reportPaths"]),
            failure="source.familySlug is required for foundation-dll-family verification",
        )

    # Import the verification orchestrator (delayed to avoid circular imports).
    # The orchestrator and its helpers use same-package imports
    # (e.g. "from multi_benchmark_runner import ...") that require the
    # foundation_dll package directory to be directly on sys.path.
    _run_toolchains_root = repo_root / "build" / "toolchains" / "run"
    _fdl_pkg_root = _run_toolchains_root / "testing" / "foundation_dll"
    for _p in [str(_run_toolchains_root), str(_fdl_pkg_root)]:
        if _p not in sys.path:
            sys.path.insert(0, _p)
    from family_verification_orchestrator import verify_family  # type: ignore[import-unverified]

    start = time.perf_counter()
    try:
        payload = verify_family(family_slug, assembly=assembly)
    except Exception as exc:
        duration_ms = int((time.perf_counter() - start) * 1000)
        return _failure_result(
            bucket_manifest_path=str(request["paths"]["manifestPath"]),
            report_paths=list(request["paths"]["reportPaths"]),
            failure=f"verify_family({family_slug}, {assembly}) raised: {exc}",
            duration_ms=duration_ms,
        )

    duration_ms = int((time.perf_counter() - start) * 1000)
    report = payload.get("unifiedReport", payload)
    overall = report.get("overall_status", "failed")
    passed = overall == "passed"

    # Write the unified report as primary evidence (to the first report path dir)
    evidence_paths: list[str] = []
    _report_paths = list(request["paths"].get("reportPaths") or [])
    if _report_paths:
        evidence_dir = _resolve(repo_root, str(_report_paths[0])).parent
        evidence_dir.mkdir(parents=True, exist_ok=True)
        evidence_path = evidence_dir / "unified-verification-report.json"
        import json as _json
        _json.dump(report, evidence_path.open("w", encoding="utf-8"), indent=2, ensure_ascii=False)
        evidence_paths = [_relative(repo_root, evidence_path)]

    if not passed:
        return _failure_result(
            bucket_manifest_path=str(request["paths"]["manifestPath"]),
            report_paths=_report_paths,
            primary_evidence_paths=evidence_paths,
            failure=f"verify_family {family_slug}: {overall}",
            duration_ms=duration_ms,
            details=report,
        )

    return _success_result(
        bucket_manifest_path=str(request["paths"]["manifestPath"]),
        report_paths=_report_paths,
        primary_evidence_paths=evidence_paths,
        duration_ms=duration_ms,
        details=report,
    )


def _failure_result(
    *,
    bucket_manifest_path: str,
    report_paths: list[str],
    primary_evidence_paths: list[str] | None = None,
    stdout_path: str | None = None,
    stderr_path: str | None = None,
    duration_ms: int = 0,
    details: dict[str, Any] | None = None,
    failure: str | None = None,
) -> dict[str, Any]:
    return {
        "status": "fail",
        "bucketManifestPath": bucket_manifest_path,
        "reportPaths": report_paths,
        "primaryEvidencePaths": primary_evidence_paths or [],
        "metrics": {"durationMs": duration_ms},
        "diagnostics": {"stdoutPath": stdout_path, "stderrPath": stderr_path},
        "details": dict(details or {}),
        "failure": failure,
    }


DEFAULT_STAGE_WORKERS = {
    "source-resolve": run_source_resolve,
    "host-input-build": run_dotnet_host_input_builder,
    "analysis-frontend": run_frontend_pipeline_worker,
    "generated-native-proof": run_native_proof_emitter,
    "generated-native-aot": run_native_aot_emitter,
    "generated-engine-proof": run_native_proof_emitter,
    "build-target": run_build_target,
    "runtime-observe": run_runtime_observe,
    "runtime-engine-observe": run_runtime_engine_observe,
    "runtime-engine-trace-compare": run_runtime_engine_trace_compare,
    "runtime-managed-output": run_managed_runtime_output,
    "runtime-perf-collect": run_runtime_perf_collect,
    "native-runtime-perf": run_native_runtime_perf,
    "mobile-native-perf": run_mobile_native_perf,
    "runtime-trace-compare": run_runtime_trace_compare,
    "interpreter-runtime-perf": run_interpreter_runtime_perf,
    "benchmark-comparison-aggregate": run_benchmark_comparison_aggregate,
    "family-verify-foundation-dll": run_family_verify_foundation_dll,
}
