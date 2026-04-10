from __future__ import annotations

import shutil
import sys
import uuid
from pathlib import Path
import re
from typing import Any

try:
    from ..core import tooling as tooling_module
    from ..core.common import combine_process_output, read_json, run_process, write_json
    from ..testing import subject_executor as subject_executor_module
    from ..testing import subject_planner as subject_planner_module
    from ..testing import subjects as subjects_module
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from core import tooling as tooling_module
    from core.common import combine_process_output, read_json, run_process, write_json
    from testing import subject_executor as subject_executor_module
    from testing import subject_planner as subject_planner_module
    from testing import subjects as subjects_module


WINDOWS_VISUAL_STUDIO_GENERATOR = "Visual Studio 17 2022"
CSHARP_PROJECT_TYPE_GUID = "9A19103F-16F7-4668-BE54-9A1E7A4F7556"
VCX_PROJECT_TYPE_GUID = "8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942"
SUBJECT_GENERATED_NATIVE_TARGET = "chaos_subject_generated_native"
SUBJECT_PROOF_NATIVE_TARGET = "chaos_subject_reference_proof"
SUBJECT_VISUAL_STUDIO_STATE_VERSION = 8
CORE_MANAGED_PROJECTS = [
    "Chaos.IL2CPP.Contracts",
    "Chaos.IL2CPP.Loader",
    "Chaos.IL2CPP.SemanticWorld",
    "Chaos.IL2CPP.MetadataWriter",
    "Chaos.IL2CPP.CodeGen",
    "Chaos.IL2CPP.Linker",
    "Chaos.IL2CPP.Pipeline",
    "Chaos.IL2CPP.Driver",
]
CORE_TARGET_ORDER = [
    "windows-x64-reference",
    "macos-reference",
    "android-arm64",
    "ios-arm64",
    "linux-x64",
]
CORE_TARGET_SPECS: dict[str, dict[str, Any]] = {
    "windows-x64-reference": {
        "hosts": {"windows"},
        "targetPlatform": "windows-x64",
        "presetTarget": "windows-x64-reference",
        "toolchainFile": "build/toolchains/windows-x64-reference.cmake",
        "deliveryKind": "direct-run-host",
        "buildArgs": ["--config", "Release"],
    },
    "macos-reference": {
        "hosts": {"macos"},
        "targetPlatform": "macos-arm64",
        "presetTarget": "macos-reference",
        "toolchainFile": "build/toolchains/macos-reference.cmake",
        "deliveryKind": "direct-run-host",
        "buildArgs": [],
    },
    "android-arm64": {
        "hosts": {"windows"},
        "targetPlatform": "android-arm64",
        "presetTarget": "android-arm64-smoke",
        "toolchainFile": "build/toolchains/android-arm64.cmake",
        "deliveryKind": "project-package-root",
        "buildArgs": [],
    },
    "ios-arm64": {
        "hosts": {"macos"},
        "targetPlatform": "ios-arm64",
        "presetTarget": "ios-arm64-packaging",
        "toolchainFile": "build/toolchains/ios-arm64.cmake",
        "deliveryKind": "project-package-root",
        "buildArgs": [],
    },
    "linux-x64": {
        "hosts": {"windows", "macos", "linux"},
        "targetPlatform": "linux-x64",
        "presetTarget": "linux-x64-packaging",
        "toolchainFile": "build/toolchains/linux-x64.cmake",
        "deliveryKind": "project-package-root",
        "buildArgs": [],
    },
}
DEFAULT_CORE_TARGET_BY_HOST = {
    "windows": "windows-x64-reference",
    "macos": "macos-reference",
    "linux": "linux-x64",
}
GENERIC_SUBJECT_REFERENCE_RUN_SCRIPT = """if(NOT DEFINED CHAOS_SUBJECT_PROOF_EXE OR CHAOS_SUBJECT_PROOF_EXE STREQUAL "")
    message(FATAL_ERROR "CHAOS_SUBJECT_PROOF_EXE is required")
endif()

if(NOT DEFINED CHAOS_SUBJECT_PROOF_RUN_OUTPUT_ROOT OR CHAOS_SUBJECT_PROOF_RUN_OUTPUT_ROOT STREQUAL "")
    message(FATAL_ERROR "CHAOS_SUBJECT_PROOF_RUN_OUTPUT_ROOT is required")
endif()

if(NOT DEFINED CHAOS_SUBJECT_PROOF_STDOUT_PATH OR CHAOS_SUBJECT_PROOF_STDOUT_PATH STREQUAL "")
    message(FATAL_ERROR "CHAOS_SUBJECT_PROOF_STDOUT_PATH is required")
endif()

if(NOT DEFINED CHAOS_SUBJECT_PROOF_STDERR_PATH OR CHAOS_SUBJECT_PROOF_STDERR_PATH STREQUAL "")
    message(FATAL_ERROR "CHAOS_SUBJECT_PROOF_STDERR_PATH is required")
endif()

if(NOT DEFINED CHAOS_SUBJECT_PROOF_EXIT_CODE_PATH OR CHAOS_SUBJECT_PROOF_EXIT_CODE_PATH STREQUAL "")
    message(FATAL_ERROR "CHAOS_SUBJECT_PROOF_EXIT_CODE_PATH is required")
endif()

file(MAKE_DIRECTORY "${CHAOS_SUBJECT_PROOF_RUN_OUTPUT_ROOT}")

execute_process(
    COMMAND "${CHAOS_SUBJECT_PROOF_EXE}"
    WORKING_DIRECTORY "${CHAOS_SUBJECT_PROOF_RUN_OUTPUT_ROOT}"
    RESULT_VARIABLE proof_exit_code
    OUTPUT_FILE "${CHAOS_SUBJECT_PROOF_STDOUT_PATH}"
    ERROR_FILE "${CHAOS_SUBJECT_PROOF_STDERR_PATH}")

file(WRITE "${CHAOS_SUBJECT_PROOF_EXIT_CODE_PATH}" "${proof_exit_code}\n")

if(NOT proof_exit_code EQUAL 0)
    message(FATAL_ERROR
        "Subject native reference proof failed with exit code ${proof_exit_code}; "
        "see ${CHAOS_SUBJECT_PROOF_STDOUT_PATH}, ${CHAOS_SUBJECT_PROOF_STDERR_PATH}, and ${CHAOS_SUBJECT_PROOF_EXIT_CODE_PATH}.")
endif()
"""
SUBJECT_NATIVE_WORKSPACE_CMAKELISTS_TEMPLATE = """cmake_minimum_required(VERSION 3.20)

project(chaos_subject_native_workspace LANGUAGES CXX)

if(NOT DEFINED CHAOS_SUBJECT_REPO_ROOT OR CHAOS_SUBJECT_REPO_ROOT STREQUAL "")
    message(FATAL_ERROR "CHAOS_SUBJECT_REPO_ROOT is required")
endif()

set(REPO_ROOT "${CHAOS_SUBJECT_REPO_ROOT}")
set(CHAOS_SUBJECT_VARIANT "CHECK" CACHE STRING "Variant selector for the current subject reference fixture")
set_property(CACHE CHAOS_SUBJECT_VARIANT PROPERTY STRINGS CHECK PROFILE SHIP)

set(CHAOS_SUBJECT_BUILD_OUT_ROOT
    "${REPO_ROOT}/artifacts/subjects/FixtureSubject/runs/subject-exec/matrices/windows-dev-output/build/out"
    CACHE PATH "Build output root for the current subject reference fixture")
set(CHAOS_SUBJECT_RUNTIME_ROOT
    "${REPO_ROOT}/artifacts/subjects/FixtureSubject/runs/subject-exec/matrices/windows-dev-output/runtime"
    CACHE PATH "Runtime evidence root for the current subject reference fixture")

function(chaos_apply_subject_variant target_name)
    if(CHAOS_SUBJECT_VARIANT STREQUAL "CHECK")
        target_compile_definitions(
            "${target_name}"
            PRIVATE
                CHAOS_VARIANT_CHECK
                "CHAOS_VARIANT_NAME=\\"CHECK\\"")
        target_compile_options(
            "${target_name}"
            PRIVATE
                $<$<CXX_COMPILER_ID:MSVC>:/Od /Zi>
                $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-O0 -g>)
        return()
    endif()

    if(CHAOS_SUBJECT_VARIANT STREQUAL "PROFILE")
        target_compile_definitions(
            "${target_name}"
            PRIVATE
                CHAOS_VARIANT_PROFILE
                "CHAOS_VARIANT_NAME=\\"PROFILE\\"")
        target_compile_options(
            "${target_name}"
            PRIVATE
                $<$<CXX_COMPILER_ID:MSVC>:/O2 /DNDEBUG>
                $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-O3 -DNDEBUG>)
        return()
    endif()

    if(CHAOS_SUBJECT_VARIANT STREQUAL "SHIP")
        target_compile_definitions(
            "${target_name}"
            PRIVATE
                CHAOS_VARIANT_SHIP
                "CHAOS_VARIANT_NAME=\\"SHIP\\"")
        target_compile_options(
            "${target_name}"
            PRIVATE
                $<$<CXX_COMPILER_ID:MSVC>:/O2 /GL /DNDEBUG>
                $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-O3 -DNDEBUG>)
        target_link_options(
            "${target_name}"
            PRIVATE
                $<$<CXX_COMPILER_ID:MSVC>:/LTCG>)
        return()
    endif()

    message(FATAL_ERROR "Unsupported CHAOS_SUBJECT_VARIANT='${CHAOS_SUBJECT_VARIANT}'")
endfunction()

function(chaos_configure_subject_target target_name)
    target_compile_features("${target_name}" PRIVATE cxx_std_17)
    target_include_directories(
        "${target_name}"
        PRIVATE
            "${REPO_ROOT}/contracts/native/v0"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/src/native/bootstrap"
            "${REPO_ROOT}/src/native/support")
    chaos_apply_subject_variant("${target_name}")
endfunction()

add_subdirectory("${REPO_ROOT}/src/native/runtime-core" "runtime-core")
add_subdirectory("${REPO_ROOT}/src/native/support" "support")
add_subdirectory("${REPO_ROOT}/src/native/bootstrap" "bootstrap")
add_subdirectory(generated)
add_subdirectory(proof)
"""

GENERIC_SUBJECT_GENERATED_CMAKELISTS_TEMPLATE = """set(CHAOS_SUBJECT_GENERATED_INPUT_SOURCE "@@GENERATED_INPUT_SOURCE@@")

if(NOT EXISTS "${CHAOS_SUBJECT_GENERATED_INPUT_SOURCE}")
    message(FATAL_ERROR "Missing generated subject source: ${CHAOS_SUBJECT_GENERATED_INPUT_SOURCE}")
endif()

add_library(chaos_subject_generated_native STATIC EXCLUDE_FROM_ALL
    "${CHAOS_SUBJECT_GENERATED_INPUT_SOURCE}")
chaos_configure_subject_target(chaos_subject_generated_native)

set_target_properties(
    chaos_subject_generated_native
    PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CHAOS_SUBJECT_BUILD_OUT_ROOT}"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CHAOS_SUBJECT_BUILD_OUT_ROOT}")
"""

GENERIC_SUBJECT_PROOF_CMAKELISTS_TEMPLATE = """set(CHAOS_SUBJECT_PROOF_HOST_MAIN "@@PROOF_MAIN@@")
set(CHAOS_SUBJECT_PROOF_RUN_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/RunSubjectProof.cmake")
set(CHAOS_SUBJECT_PROOF_STDOUT_LOG "${CHAOS_SUBJECT_RUNTIME_ROOT}/stdout.log")
set(CHAOS_SUBJECT_PROOF_STDERR_LOG "${CHAOS_SUBJECT_RUNTIME_ROOT}/stderr.log")
set(CHAOS_SUBJECT_PROOF_EXIT_CODE_PATH "${CHAOS_SUBJECT_RUNTIME_ROOT}/exit-code.txt")

if(NOT EXISTS "${CHAOS_SUBJECT_PROOF_HOST_MAIN}")
    message(FATAL_ERROR "Missing subject proof host source: ${CHAOS_SUBJECT_PROOF_HOST_MAIN}")
endif()

add_executable(chaos_subject_reference_proof EXCLUDE_FROM_ALL
    "${CHAOS_SUBJECT_PROOF_HOST_MAIN}")
chaos_configure_subject_target(chaos_subject_reference_proof)
target_link_libraries(
    chaos_subject_reference_proof
    PRIVATE
        chaos_subject_generated_native
        chaos_runtime_core
        chaos_bootstrap
        chaos_support)

add_custom_target(chaos_subject_reference_proof_run
    BYPRODUCTS
        "${CHAOS_SUBJECT_PROOF_STDOUT_LOG}"
        "${CHAOS_SUBJECT_PROOF_STDERR_LOG}"
        "${CHAOS_SUBJECT_PROOF_EXIT_CODE_PATH}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${CHAOS_SUBJECT_RUNTIME_ROOT}"
    COMMAND "${CMAKE_COMMAND}"
        "-DCHAOS_SUBJECT_PROOF_EXE=$<TARGET_FILE:chaos_subject_reference_proof>"
        "-DCHAOS_SUBJECT_PROOF_RUN_OUTPUT_ROOT=${CHAOS_SUBJECT_RUNTIME_ROOT}"
        "-DCHAOS_SUBJECT_PROOF_STDOUT_PATH=${CHAOS_SUBJECT_PROOF_STDOUT_LOG}"
        "-DCHAOS_SUBJECT_PROOF_STDERR_PATH=${CHAOS_SUBJECT_PROOF_STDERR_LOG}"
        "-DCHAOS_SUBJECT_PROOF_EXIT_CODE_PATH=${CHAOS_SUBJECT_PROOF_EXIT_CODE_PATH}"
        -P "${CHAOS_SUBJECT_PROOF_RUN_SCRIPT}"
    DEPENDS
        chaos_subject_reference_proof
    VERBATIM)

set_target_properties(
    chaos_subject_reference_proof
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CHAOS_SUBJECT_BUILD_OUT_ROOT}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CHAOS_SUBJECT_BUILD_OUT_ROOT}")
"""


def _path_text(repo_root: Path, path: Path) -> str:
    try:
        return path.relative_to(repo_root).as_posix()
    except ValueError:
        return path.as_posix()


def _normalize_host_platform(host_platform: str) -> str:
    if host_platform.startswith("windows"):
        return "windows"
    if host_platform.startswith("macos"):
        return "macos"
    if host_platform.startswith("linux"):
        return "linux"
    return host_platform


def _flag(options: dict[str, object], name: str) -> bool:
    return bool(options.get(name, False))


def _text_option(options: dict[str, object], name: str) -> str:
    return str(options.get(name) or "").strip()


def _native_target_alias(target_ref: str) -> str:
    target_id = target_ref.strip().lower()
    if target_id in {"generated", "generated-native"}:
        return SUBJECT_GENERATED_NATIVE_TARGET
    if target_id in {"proof", "proof-native"}:
        return SUBJECT_PROOF_NATIVE_TARGET
    return target_ref.strip()


def _build_target_from_args(build_args: list[str], fallback_target: str) -> str:
    for index, argument in enumerate(build_args):
        if argument == "--target" and index + 1 < len(build_args):
            return str(build_args[index + 1])
    return fallback_target


def _clear_dir(path: Path) -> None:
    if path.exists():
        shutil.rmtree(path, ignore_errors=True)


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
        message = failure_message
        if output:
            message += f"\n{output}"
        raise RuntimeError(message)
    return output


def _solution_project_guid(relative_project_path: str) -> str:
    return str(uuid.uuid5(uuid.NAMESPACE_URL, relative_project_path)).upper()


def _vcxproj_project_guid(project_path: Path) -> str | None:
    if not project_path.is_file():
        return None

    try:
        project_text = project_path.read_text(encoding="utf-8")
    except (OSError, UnicodeDecodeError):
        return None

    match = re.search(r"<ProjectGuid>\s*\{?([0-9A-Fa-f-]{36})\}?\s*</ProjectGuid>", project_text)
    if match is None:
        return None
    return match.group(1).upper()


def _solution_project_entry_guid(repo_root: Path, relative_project_path: str, project_type_guid: str) -> str:
    if project_type_guid == VCX_PROJECT_TYPE_GUID:
        project_guid = _vcxproj_project_guid(repo_root / relative_project_path)
        if project_guid:
            return project_guid
    return _solution_project_guid(relative_project_path)


def _solution_project_platform(project_type_guid: str) -> str:
    if project_type_guid == VCX_PROJECT_TYPE_GUID:
        return "x64"
    return "Any CPU"


def _normalize_project_name_fragment(value: str) -> str:
    normalized_chars: list[str] = []
    last_was_separator = False
    for char in value:
        if char.isalnum():
            normalized_chars.append(char)
            last_was_separator = False
            continue
        if not last_was_separator:
            normalized_chars.append("_")
            last_was_separator = True
    normalized = "".join(normalized_chars).strip("_")
    if not normalized:
        normalized = "project"
    if normalized[0].isdigit():
        normalized = f"p_{normalized}"
    return normalized


def _vcxproj_project_name(project_stem: str, matrix_id: str) -> str:
    return f"{_normalize_project_name_fragment(project_stem)}_{_normalize_project_name_fragment(matrix_id)}"


def _rewrite_vcxproj_project_name(project_path: Path, project_name: str) -> None:
    if not project_path.is_file():
        return

    project_text = project_path.read_text(encoding="utf-8")
    updated_text, project_name_count = re.subn(
        r"<ProjectName>[^<]*</ProjectName>",
        f"<ProjectName>{project_name}</ProjectName>",
        project_text,
        count=1,
    )
    if project_name_count == 0:
        return

    updated_text = re.sub(
        r"<RootNamespace>[^<]*</RootNamespace>",
        f"<RootNamespace>{project_name}</RootNamespace>",
        updated_text,
        count=1,
    )
    project_path.write_text(updated_text, encoding="utf-8")


def _rewrite_vcxproj_project_reference_names(project_path: Path, project_names_by_path: dict[Path, str]) -> None:
    if not project_path.is_file():
        return

    project_text = project_path.read_text(encoding="utf-8")

    def resolve_reference_path(include_path_text: str) -> Path:
        reference_path = Path(include_path_text.replace("\\", "/"))
        if not reference_path.is_absolute():
            return (project_path.parent / reference_path).resolve()
        return reference_path.resolve()

    def replace_reference_name(match: re.Match[str]) -> str:
        include_path_text = match.group("include")
        reference_path = resolve_reference_path(include_path_text)
        reference_name = project_names_by_path.get(reference_path)
        if not reference_name:
            return match.group(0)

        updated_block, updated_count = re.subn(
            r"<Name>[^<]*</Name>",
            f"<Name>{reference_name}</Name>",
            match.group(0),
            count=1,
        )
        if updated_count == 0:
            return match.group(0)
        return updated_block

    updated_text = re.sub(
        r'<ProjectReference Include="(?P<include>[^"]+)">.*?</ProjectReference>',
        replace_reference_name,
        project_text,
        flags=re.DOTALL,
    )
    if updated_text == project_text:
        return
    project_path.write_text(updated_text, encoding="utf-8")


def _filter_vcxproj_project_references(project_path: Path, allowed_reference_paths: set[Path]) -> None:
    if not project_path.is_file():
        return

    project_text = project_path.read_text(encoding="utf-8")

    def replace_reference(match: re.Match[str]) -> str:
        include_path_text = match.group("include")
        reference_path = Path(include_path_text.replace("\\", "/"))
        if not reference_path.is_absolute():
            reference_path = (project_path.parent / reference_path).resolve()
        else:
            reference_path = reference_path.resolve()

        if reference_path in allowed_reference_paths:
            return match.group(0)
        return ""

    updated_text = re.sub(
        r"\s*<ProjectReference Include=\"(?P<include>[^\"]+)\">.*?</ProjectReference>",
        replace_reference,
        project_text,
        flags=re.DOTALL,
    )
    updated_text = re.sub(r"\s*<ItemGroup>\s*</ItemGroup>", "", updated_text, flags=re.DOTALL)
    if updated_text == project_text:
        return
    project_path.write_text(updated_text, encoding="utf-8")


def _rewrite_subject_facing_vcxproj_project_references(
    configure_root: Path,
    *,
    target_platform: str,
    host_platform: str,
) -> None:
    if host_platform != "windows" or target_platform != "windows-x64":
        return

    generated_project_path = _subject_native_project_file_path(
        configure_root,
        target_id=SUBJECT_GENERATED_NATIVE_TARGET,
        target_platform=target_platform,
        host_platform=host_platform,
    )
    proof_project_path = _subject_native_project_file_path(
        configure_root,
        target_id=SUBJECT_PROOF_NATIVE_TARGET,
        target_platform=target_platform,
        host_platform=host_platform,
    )

    _filter_vcxproj_project_references(generated_project_path, set())
    _filter_vcxproj_project_references(proof_project_path, set())


def _subject_native_project_file_path(
    configure_root: Path,
    *,
    target_id: str,
    target_platform: str,
    host_platform: str,
) -> Path:
    if host_platform == "windows" and target_platform == "windows-x64":
        if target_id == SUBJECT_GENERATED_NATIVE_TARGET:
            return configure_root / "generated" / f"{target_id}.vcxproj"
        if target_id == SUBJECT_PROOF_NATIVE_TARGET:
            return configure_root / "proof" / f"{target_id}.vcxproj"

    return configure_root / f"{target_id}.vcxproj"


def _configure_root_vcxproj_paths_for_project_name_rewrite(
    configure_root: Path,
    *,
    primary_open_target: str,
    target_platform: str,
    host_platform: str,
) -> list[Path]:
    if host_platform == "windows" and target_platform == "windows-x64":
        project_paths: list[Path] = []
        for project_path in sorted(configure_root.rglob("*.vcxproj")):
            try:
                relative_parts = project_path.relative_to(configure_root).parts
            except ValueError:
                continue
            if "CMakeFiles" in relative_parts:
                continue
            project_paths.append(project_path)
        return project_paths

    return [
        _subject_native_project_file_path(
            configure_root,
            target_id=primary_open_target,
            target_platform=target_platform,
            host_platform=host_platform,
        )
    ]


def _rewrite_configure_root_vcxproj_project_names(
    configure_root: Path,
    *,
    matrix_id: str,
    primary_open_target: str,
    target_platform: str,
    host_platform: str,
) -> None:
    project_paths = _configure_root_vcxproj_paths_for_project_name_rewrite(
        configure_root,
        primary_open_target=primary_open_target,
        target_platform=target_platform,
        host_platform=host_platform,
    )
    project_names_by_path = {
        project_path.resolve(): _vcxproj_project_name(project_path.stem, matrix_id)
        for project_path in project_paths
    }

    for project_path in project_paths:
        _rewrite_vcxproj_project_name(
            project_path,
            project_names_by_path[project_path.resolve()],
        )
    for project_path in project_paths:
        _rewrite_vcxproj_project_reference_names(project_path, project_names_by_path)


def _solution_project_names(project_paths: list[str]) -> dict[str, str]:
    def qualifier_parts(relative_project_path: str) -> tuple[str, ...]:
        parent_parts = list(Path(relative_project_path).parent.parts)
        if parent_parts and parent_parts[-1] in {"generated", "proof"}:
            return tuple(parent_parts[:-1])
        return tuple(parent_parts)

    grouped_paths: dict[str, list[str]] = {}
    for relative_project_path in project_paths:
        grouped_paths.setdefault(Path(relative_project_path).stem, []).append(relative_project_path)

    project_names: dict[str, str] = {}
    for base_name, grouped_project_paths in grouped_paths.items():
        if len(grouped_project_paths) == 1:
            project_names[grouped_project_paths[0]] = base_name
            continue

        qualifier_map: dict[str, str] | None = None
        qualifier_part_map = {
            relative_project_path: qualifier_parts(relative_project_path)
            for relative_project_path in grouped_project_paths
        }
        max_depth = max(len(parts) for parts in qualifier_part_map.values())
        for depth in range(1, max_depth + 1):
            candidate_qualifiers = {
                relative_project_path: "/".join(qualifier_part_map[relative_project_path][-depth:])
                for relative_project_path in grouped_project_paths
            }
            if len(set(candidate_qualifiers.values())) == len(grouped_project_paths):
                qualifier_map = candidate_qualifiers
                break

        if qualifier_map is None:
            qualifier_map = {
                relative_project_path: relative_project_path
                for relative_project_path in grouped_project_paths
            }

        for relative_project_path, qualifier in qualifier_map.items():
            project_names[relative_project_path] = f"{base_name} ({qualifier})"

    duplicate_names: dict[str, list[str]] = {}
    for relative_project_path, project_name in project_names.items():
        duplicate_names.setdefault(project_name, []).append(relative_project_path)
    for project_name, grouped_project_paths in duplicate_names.items():
        if len(grouped_project_paths) == 1:
            continue
        for relative_project_path in grouped_project_paths:
            project_names[relative_project_path] = f"{project_name} [{_solution_project_guid(relative_project_path)[:8]}]"

    return project_names


def _write_solution_file(
    solution_path: Path,
    repo_root: Path,
    managed_project_paths: list[str],
    native_project_paths: list[str] | None = None,
) -> None:
    solution_path.parent.mkdir(parents=True, exist_ok=True)
    lines = [
        "Microsoft Visual Studio Solution File, Format Version 12.00",
        "# Visual Studio Version 17",
        "VisualStudioVersion = 17.0.31903.59",
        "MinimumVisualStudioVersion = 10.0.40219.1",
    ]

    project_records = [
        (relative_project_path, CSHARP_PROJECT_TYPE_GUID)
        for relative_project_path in managed_project_paths
    ]
    project_records.extend(
        (relative_project_path, VCX_PROJECT_TYPE_GUID)
        for relative_project_path in list(native_project_paths or [])
    )
    project_names = _solution_project_names(
        [relative_project_path for relative_project_path, _ in project_records]
    )

    project_entries: list[tuple[str, str, str]] = []
    for relative_project_path, project_type_guid in project_records:
        project_name = project_names[relative_project_path]
        project_guid = _solution_project_entry_guid(repo_root, relative_project_path, project_type_guid)
        project_relative_path = _path_text(solution_path.parent, repo_root / relative_project_path)
        lines.append(
            f'Project("{{{project_type_guid}}}") = "{project_name}", "{project_relative_path}", "{{{project_guid}}}"'
        )
        lines.append("EndProject")
        project_entries.append((project_guid, project_type_guid, relative_project_path))

    lines.extend(
        [
            "Global",
            "\tGlobalSection(SolutionConfigurationPlatforms) = preSolution",
            "\t\tDebug|Any CPU = Debug|Any CPU",
            "\t\tRelease|Any CPU = Release|Any CPU",
            "\tEndGlobalSection",
            "\tGlobalSection(ProjectConfigurationPlatforms) = postSolution",
        ]
    )
    for project_guid, project_type_guid, _ in project_entries:
        project_platform = _solution_project_platform(project_type_guid)
        lines.extend(
            [
                f"\t\t{{{project_guid}}}.Debug|Any CPU.ActiveCfg = Debug|{project_platform}",
                f"\t\t{{{project_guid}}}.Debug|Any CPU.Build.0 = Debug|{project_platform}",
                f"\t\t{{{project_guid}}}.Release|Any CPU.ActiveCfg = Release|{project_platform}",
                f"\t\t{{{project_guid}}}.Release|Any CPU.Build.0 = Release|{project_platform}",
            ]
        )
    lines.extend(
        [
            "\tEndGlobalSection",
            "\tGlobalSection(SolutionProperties) = preSolution",
            "\t\tHideSolutionNode = FALSE",
            "\tEndGlobalSection",
            "EndGlobal",
        ]
    )
    solution_path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def _subject_workspace_root(repo_root: Path, subject_id: str) -> Path:
    return repo_root / "solutions" / "subjects" / subject_id


def _subject_workspace_manifest_path(repo_root: Path, subject_id: str) -> Path:
    return _subject_workspace_root(repo_root, subject_id) / "workspace.manifest.json"


def _workspace_visual_studio_state_root(workspace_root: Path) -> Path:
    return workspace_root / ".vs"


def _subject_materialized_source_root(workspace_root: Path, matrix_id: str) -> Path:
    return workspace_root / "native-source" / matrix_id


def _subject_generated_run_root(repo_root: Path, subject_id: str, *, run_id: str = "subject-exec") -> Path:
    return repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id


def _subject_generated_run_id(matrix_id: str) -> str:
    return f"subject-exec-{_normalize_project_name_fragment(matrix_id)}"


def _subject_generated_root(repo_root: Path, subject_id: str, *, run_id: str = "subject-exec") -> Path:
    return _subject_generated_run_root(repo_root, subject_id, run_id=run_id) / "analysis" / "generated"


def _subject_generated_source_path(repo_root: Path, subject_id: str, *, run_id: str = "subject-exec") -> Path:
    return _subject_generated_root(repo_root, subject_id, run_id=run_id) / "generated" / "native-reference.generated.cpp"


def _subject_generated_source_is_stale(generated_source_path: Path) -> bool:
    if not generated_source_path.is_file():
        return False

    try:
        generated_source = generated_source_path.read_text(encoding="utf-8")
    except (OSError, UnicodeDecodeError):
        return True

    stale_markers = (
        "NativeReferenceProofContext",
        "g_proof_context",
        "GetProofImage",
        "ResetProofContext",
        "InitializeProofContext",
        "EnsureProofContextReady",
        "RunNativeReferenceProof",
    )
    return any(marker in generated_source for marker in stale_markers)


def _subject_generated_solution_root(workspace_root: Path, *, matrix_id: str, multi_matrix: bool) -> Path:
    if multi_matrix:
        return workspace_root / "generated" / matrix_id / "subject-exec"
    return workspace_root / "generated" / "subject-exec"


def _subject_generated_solution_source_path(generated_solution_root: Path) -> Path:
    return generated_solution_root / "analysis" / "generated" / "generated" / "native-reference.generated.cpp"


def _ensure_subject_generated_source(
    repo_root: Path,
    *,
    subject_id: str,
    matrix_id: str,
    variant: str,
    run_id: str = "subject-exec",
    refresh_generated: bool,
    refresh_if_missing: bool = False,
) -> None:
    generated_source_path = _subject_generated_source_path(repo_root, subject_id, run_id=run_id)
    if (
        refresh_generated
        or (refresh_if_missing and not generated_source_path.is_file())
        or _subject_generated_source_is_stale(generated_source_path)
    ):
        refresh_subject_generated_root(repo_root, subject_id, matrix_id, variant, run_id=run_id)

    if not generated_source_path.is_file():
        raise RuntimeError(f"subject-exec generated root is missing for subject '{subject_id}'")


def _mirror_subject_generated_run(
    repo_root: Path,
    *,
    workspace_root: Path,
    subject_id: str,
    matrix_id: str,
    run_id: str = "subject-exec",
    multi_matrix: bool,
) -> Path:
    generated_run_root = _subject_generated_run_root(repo_root, subject_id, run_id=run_id)
    if not generated_run_root.is_dir():
        raise RuntimeError(f"subject-exec generated run is missing for subject '{subject_id}'")

    mirrored_root = _subject_generated_solution_root(workspace_root, matrix_id=matrix_id, multi_matrix=multi_matrix)
    _clear_dir(mirrored_root)
    mirrored_root.parent.mkdir(parents=True, exist_ok=True)
    shutil.copytree(generated_run_root, mirrored_root)

    mirrored_source_path = _subject_generated_solution_source_path(mirrored_root)
    if not mirrored_source_path.is_file():
        raise RuntimeError(f"subject-exec generated source mirror is missing for subject '{subject_id}'")
    return mirrored_root


def _stored_visual_studio_state_version(manifest_path: Path) -> int:
    if not manifest_path.is_file():
        return 0

    try:
        payload = read_json(manifest_path)
    except Exception:
        return 0
    if not isinstance(payload, dict):
        return 0

    try:
        return int(payload.get("visualStudioStateVersion") or 0)
    except (TypeError, ValueError):
        return 0


def _reset_stale_visual_studio_state(workspace_root: Path, manifest_path: Path) -> None:
    visual_studio_state_root = _workspace_visual_studio_state_root(workspace_root)
    if not visual_studio_state_root.exists():
        return

    if _stored_visual_studio_state_version(manifest_path) == SUBJECT_VISUAL_STUDIO_STATE_VERSION:
        return

    _clear_dir(visual_studio_state_root)


def _core_workspace_root(repo_root: Path, host_platform: str) -> Path:
    return repo_root / "solutions" / "core" / host_platform


def _core_workspace_manifest_path(repo_root: Path, host_platform: str) -> Path:
    return _core_workspace_root(repo_root, host_platform) / "workspace.manifest.json"


def _all_solutions_manifest_path(repo_root: Path) -> Path:
    return repo_root / "solutions" / "manifest.json"


def _all_solutions_report_path(repo_root: Path) -> Path:
    return repo_root / "solutions" / "all" / "generation.report.json"


def _write_all_solutions_outputs(
    repo_root: Path,
    *,
    host_platform: str,
    status: str,
    subject_manifest_paths: list[str],
    core_manifest_path: str,
    generated_subject_ids: list[str],
    generated_core_targets: list[str],
) -> tuple[Path, Path]:
    manifest_path = _all_solutions_manifest_path(repo_root)
    report_path = _all_solutions_report_path(repo_root)

    write_json(
        manifest_path,
        {
            "kind": "all-workspaces",
            "hostPlatform": host_platform,
            "status": status,
            "subjectWorkspaceManifests": subject_manifest_paths,
            "coreWorkspaceManifest": core_manifest_path,
        },
    )
    write_json(
        report_path,
        {
            "kind": "all-workspaces-report",
            "hostPlatform": host_platform,
            "status": status,
            "generatedSubjectIds": generated_subject_ids,
            "generatedCoreTargets": generated_core_targets,
        },
    )
    return manifest_path, report_path


def _subject_id_from_options(options: dict[str, object]) -> str:
    subject_ref = _text_option(options, "id")
    if not subject_ref.startswith("subject/"):
        raise RuntimeError("subject workspace requires --id subject/<subject-id>")
    return subject_ref.split("/", 1)[1]


def _subject_managed_projects(manifest: dict[str, Any]) -> list[str]:
    projects = [str(dict(manifest.get("source") or {}).get("path") or "")]
    validation = dict(manifest.get("validation") or {})
    for item in validation.values():
        project_path = str(dict(item).get("project") or "")
        if project_path:
            projects.append(project_path)
    deduped: list[str] = []
    for project_path in projects:
        if project_path and project_path not in deduped:
            deduped.append(project_path)
    return deduped


def _subject_variant(manifest: dict[str, Any], options: dict[str, object]) -> str:
    variant = _text_option(options, "variant")
    selection = subjects_module.resolve_validation_selection(manifest, variant=variant or None)
    return str(selection["variant"])


def _subject_selected_matrices(manifest: dict[str, Any], host_platform: str, options: dict[str, object]) -> list[dict[str, Any]]:
    requested_matrix_id = _text_option(options, "matrix")
    all_targets = _flag(options, "all-targets")
    matrices = list(manifest.get("environmentMatrices") or [])

    if requested_matrix_id:
        selected = subjects_module.find_matrix(manifest, requested_matrix_id)
        execution_context = dict(selected.get("executionContext") or {})
        matrix_host = _normalize_host_platform(str(execution_context.get("hostPlatform") or ""))
        if matrix_host != host_platform:
            raise RuntimeError(f"matrix '{requested_matrix_id}' is not supported on host '{host_platform}'")
        return [selected]

    host_matrices = [
        dict(matrix)
        for matrix in matrices
        if _normalize_host_platform(str(dict(matrix.get("executionContext") or {}).get("hostPlatform") or "")) == host_platform
    ]
    if not host_matrices:
        raise RuntimeError(f"no subject matrices available for host '{host_platform}'")

    if all_targets:
        return host_matrices

    default_matrix_id = str(manifest.get("defaultMatrix") or "")
    if default_matrix_id:
        return [subjects_module.find_matrix(manifest, default_matrix_id)]
    return [host_matrices[0]]


def _pipeline_has_generated_stage(manifest: dict[str, Any], pipeline_id: str) -> bool:
    pipeline = subjects_module.find_pipeline(manifest, pipeline_id)
    for stage in list(pipeline.get("stages") or []):
        bucket = str(stage.get("bucket") or "")
        kind = str(stage.get("kind") or "")
        if bucket == "generated" or kind.startswith("generated-"):
            return True
    return False


def _subject_supports_workspace_generation(repo_root: Path, subject_id: str, host_platform: str) -> bool:
    manifest = subjects_module.load_subject_manifest(repo_root, subject_id)
    try:
        selected_matrices = _subject_selected_matrices(manifest, host_platform, {"all-targets": True})
    except Exception:
        return False

    if not selected_matrices:
        return False

    proof_root = repo_root / "subjects" / subject_id / "validation" / "proof" / "native-reference"
    proof_main_path = proof_root / "main.cpp"
    for matrix in selected_matrices:
        pipeline_id = str(matrix.get("pipelineId") or "")
        if not pipeline_id or not _pipeline_has_generated_stage(manifest, pipeline_id):
            return False

        execution_context = dict(matrix.get("executionContext") or {})
        target_platform = str(execution_context.get("targetPlatform") or "")
        if target_platform == "windows-x64" and not proof_main_path.is_file():
            return False

    return True


def _subject_matrix_goal(manifest: dict[str, Any], matrix: dict[str, Any]) -> str:
    supported_goals = [str(item) for item in list(matrix.get("supportedGoals") or []) if str(item)]
    default_goal = str(manifest.get("defaultGoal") or "")
    if default_goal and default_goal in supported_goals:
        return default_goal
    if supported_goals:
        return supported_goals[0]
    raise RuntimeError(f"matrix '{matrix.get('matrixId')}' does not declare supportedGoals")


def _materialize_subject_native_reference_source(
    repo_root: Path,
    *,
    workspace_root: Path,
    subject_id: str,
    matrix_id: str,
    generated_source_path: Path,
) -> Path:
    proof_root = repo_root / "subjects" / subject_id / "validation" / "proof" / "native-reference"
    proof_main_path = proof_root / "main.cpp"
    if not proof_main_path.is_file():
        raise RuntimeError(f"subject proof source is missing: {proof_main_path}")

    materialized_root = _subject_materialized_source_root(workspace_root, matrix_id)
    _clear_dir(materialized_root)
    materialized_root.mkdir(parents=True, exist_ok=True)
    (materialized_root / "generated").mkdir(parents=True, exist_ok=True)
    (materialized_root / "proof").mkdir(parents=True, exist_ok=True)
    (materialized_root / "CMakeLists.txt").write_text(
        SUBJECT_NATIVE_WORKSPACE_CMAKELISTS_TEMPLATE,
        encoding="utf-8",
    )
    (materialized_root / "generated" / "CMakeLists.txt").write_text(
        GENERIC_SUBJECT_GENERATED_CMAKELISTS_TEMPLATE.replace("@@GENERATED_INPUT_SOURCE@@", generated_source_path.as_posix()),
        encoding="utf-8",
    )
    (materialized_root / "proof" / "CMakeLists.txt").write_text(
        GENERIC_SUBJECT_PROOF_CMAKELISTS_TEMPLATE.replace("@@PROOF_MAIN@@", proof_main_path.as_posix()),
        encoding="utf-8",
    )
    (materialized_root / "proof" / "RunSubjectProof.cmake").write_text(
        GENERIC_SUBJECT_REFERENCE_RUN_SCRIPT,
        encoding="utf-8",
    )
    return materialized_root


def _generated_stage_cutoff(stages: list[dict[str, Any]]) -> int:
    for index in range(len(stages) - 1, -1, -1):
        stage = stages[index]
        kind = str(stage.get("kind") or "")
        bucket = str(stage.get("bucket") or "")
        if bucket == "generated" or kind.startswith("generated-"):
            return index
    return -1


def refresh_subject_generated_root(
    repo_root: Path,
    subject_id: str,
    matrix_id: str,
    variant: str,
    *,
    run_id: str | None = None,
) -> None:
    selected_run_id = run_id or "subject-exec"
    _clear_dir(_subject_generated_run_root(repo_root, subject_id, run_id=selected_run_id))
    manifest = subjects_module.load_subject_manifest(repo_root, subject_id)
    matrix = subjects_module.find_matrix(manifest, matrix_id)
    goal_id = _subject_matrix_goal(manifest, matrix)
    plan = subject_planner_module.build_plan(
        repo_root,
        subject_id,
        goal_id=goal_id,
        matrix_id=matrix_id,
        variant=variant,
        run_id=selected_run_id,
    )
    stages = [dict(stage) for stage in list(plan.get("stagePlan") or [])]
    cutoff = _generated_stage_cutoff(stages)
    if cutoff < 0:
        raise RuntimeError(f"subject '{subject_id}' does not define a generated stage")

    plan["stagePlan"] = stages[: cutoff + 1]
    result = subject_executor_module.execute_plan(repo_root, plan, run_id=selected_run_id)
    if str(result.get("status") or "fail") != "ok":
        errors = [str(item) for item in list(result.get("errors") or []) if str(item)]
        raise RuntimeError("\n".join(errors) or "subject-exec generated refresh failed")


def _platform_gate_generator(host_platform: str) -> str:
    if host_platform == "windows":
        return WINDOWS_VISUAL_STUDIO_GENERATOR
    if host_platform == "macos":
        return "Xcode"
    return "Ninja"


def _subject_configure_arguments(
    repo_root: Path,
    *,
    subject_id: str,
    matrix: dict[str, Any],
    variant: str,
    workspace_root: Path,
    mirrored_generated_root: Path | None,
    configure_root: Path,
    cmake_path: str,
    host_platform: str,
) -> tuple[list[str], list[str], str]:
    matrix_id = str(matrix.get("matrixId") or "")
    execution_context = dict(matrix.get("executionContext") or {})
    target_platform = str(execution_context.get("targetPlatform") or "")
    out_root = configure_root / "out"
    runtime_root = configure_root / "runtime"

    if target_platform == "windows-x64":
        if mirrored_generated_root is None:
            raise RuntimeError(f"matrix '{matrix_id}' is missing mirrored generated source")
        source_root = _materialize_subject_native_reference_source(
            repo_root,
            workspace_root=workspace_root,
            subject_id=subject_id,
            matrix_id=matrix_id,
            generated_source_path=_subject_generated_solution_source_path(mirrored_generated_root),
        )
        configure_args = [
            cmake_path,
            "-S",
            str(source_root),
            "-B",
            str(configure_root),
            "-G",
            WINDOWS_VISUAL_STUDIO_GENERATOR,
            f"-DCHAOS_SUBJECT_REPO_ROOT={repo_root.as_posix()}",
            f"-DCHAOS_SUBJECT_VARIANT={variant}",
            f"-DCHAOS_SUBJECT_BUILD_OUT_ROOT={out_root}",
            f"-DCHAOS_SUBJECT_RUNTIME_ROOT={runtime_root}",
        ]
        return configure_args, ["--config", "Release", "--target", SUBJECT_PROOF_NATIVE_TARGET], SUBJECT_PROOF_NATIVE_TARGET

    preset_target = {
        "android-arm64": "android-arm64-smoke",
        "linux-x64": "linux-x64-packaging",
    }.get(target_platform)
    toolchain_file = {
        "android-arm64": "build/toolchains/android-arm64.cmake",
        "linux-x64": "build/toolchains/linux-x64.cmake",
    }.get(target_platform)
    if not preset_target or not toolchain_file:
        raise RuntimeError(f"unsupported subject target platform: {target_platform}")

    configure_args = [
        cmake_path,
        "-S",
        str(repo_root),
        "-B",
        str(configure_root),
        "-G",
        _platform_gate_generator(host_platform),
        f"-DROADMAP0_PRESET_TARGET={preset_target}",
        "-DROADMAP0_TOOLCHAIN_VALIDATE_ONLY=ON",
        f"-DCMAKE_TOOLCHAIN_FILE={repo_root / toolchain_file}",
    ]
    return configure_args, [], preset_target


def _subject_native_projects(
    repo_root: Path,
    *,
    configure_root: Path,
    primary_open_target: str,
    target_platform: str,
    host_platform: str,
) -> list[dict[str, Any]]:
    if host_platform != "windows":
        return []

    if target_platform == "windows-x64":
        return [
            {
                "targetId": SUBJECT_GENERATED_NATIVE_TARGET,
                "kind": "generated-native",
                "projectPath": _path_text(
                    repo_root,
                    _subject_native_project_file_path(
                        configure_root,
                        target_id=SUBJECT_GENERATED_NATIVE_TARGET,
                        target_platform=target_platform,
                        host_platform=host_platform,
                    ),
                ),
                "buildArgs": ["--config", "Release", "--target", SUBJECT_GENERATED_NATIVE_TARGET],
            },
            {
                "targetId": SUBJECT_PROOF_NATIVE_TARGET,
                "kind": "proof-native",
                "projectPath": _path_text(
                    repo_root,
                    _subject_native_project_file_path(
                        configure_root,
                        target_id=SUBJECT_PROOF_NATIVE_TARGET,
                        target_platform=target_platform,
                        host_platform=host_platform,
                    ),
                ),
                "buildArgs": ["--config", "Release", "--target", SUBJECT_PROOF_NATIVE_TARGET],
            },
        ]

    return [
        {
            "targetId": primary_open_target,
            "kind": "workspace-primary",
            "projectPath": _path_text(
                repo_root,
                _subject_native_project_file_path(
                    configure_root,
                    target_id=primary_open_target,
                    target_platform=target_platform,
                    host_platform=host_platform,
                ),
            ),
            "buildArgs": [],
        }
    ]


def _find_native_project(native_projects: list[dict[str, Any]], target_id: str) -> dict[str, Any] | None:
    for item in native_projects:
        if str(item.get("targetId") or "") == target_id:
            return item
    return None


def _select_subject_native_target(
    requested_target: str,
    native_projects: list[dict[str, Any]],
    *,
    default_target: str,
) -> str:
    if not native_projects:
        return default_target

    if not requested_target or requested_target.lower() == "default":
        return default_target

    normalized_target = _native_target_alias(requested_target)
    if _find_native_project(native_projects, normalized_target) is not None:
        return normalized_target

    if len(native_projects) == 1:
        return default_target

    raise RuntimeError(f"subject workspace native target was not found: {requested_target}")


def _subject_native_project_path(native_projects: list[dict[str, Any]], target_id: str) -> str | None:
    selected = _find_native_project(native_projects, target_id)
    if selected is None:
        return None
    project_path = str(selected.get("projectPath") or "")
    return project_path or None


def _subject_solution_native_project_paths(native_projects: list[dict[str, Any]]) -> list[str]:
    top_level_kinds = {
        "generated-native",
        "proof-native",
    }
    return [
        str(item.get("projectPath") or "")
        for item in native_projects
        if str(item.get("kind") or "") in top_level_kinds and str(item.get("projectPath") or "")
    ]


def _discover_subject_ids(repo_root: Path, host_platform: str) -> list[str]:
    subjects_root = repo_root / "subjects"
    if not subjects_root.is_dir():
        return []

    discovered: list[str] = []
    for manifest_path in sorted(subjects_root.glob("*/subject.manifest.json")):
        subject_id = manifest_path.parent.name
        try:
            if not _subject_supports_workspace_generation(repo_root, subject_id, host_platform):
                continue
        except Exception:
            continue
        discovered.append(subject_id)
    return discovered


def generate_subject_workspace(repo_root: Path, host_platform: str, options: dict[str, object]) -> dict[str, Any]:
    subject_id = _subject_id_from_options(options)
    manifest = subjects_module.load_subject_manifest(repo_root, subject_id)
    variant = _subject_variant(manifest, options)
    requested_open_native_target = _text_option(options, "open-native-target")
    selected_matrices = _subject_selected_matrices(manifest, host_platform, options)
    refresh_generated = _flag(options, "refresh-generated")
    auto_refresh_missing_generated = _flag(options, "auto-refresh-missing-generated")
    generated_matrix_ids = [
        str(matrix.get("matrixId") or "")
        for matrix in selected_matrices
        if str(dict(matrix.get("executionContext") or {}).get("targetPlatform") or "") == "windows-x64"
    ]
    multi_matrix_generated = len(generated_matrix_ids) > 1
    cmake_path, cmake_env = tooling_module.cmake_environment(repo_root)
    if cmake_path is None:
        raise RuntimeError("cmake not found")

    managed_projects = _subject_managed_projects(manifest)
    workspace_root = _subject_workspace_root(repo_root, subject_id)
    solution_path = workspace_root / f"{subject_id}.sln"
    manifest_path = _subject_workspace_manifest_path(repo_root, subject_id)
    if host_platform == "windows":
        _reset_stale_visual_studio_state(workspace_root, manifest_path)
    _clear_dir(workspace_root / "generated")

    matrix_payloads: list[dict[str, Any]] = []
    solution_native_project_paths: list[str] = []
    artifacts = [_path_text(repo_root, manifest_path)]
    important_outputs = [
        {"label": "Workspace manifest", "path": _path_text(repo_root, manifest_path)},
        {"label": "Managed solution", "path": _path_text(repo_root, solution_path)},
    ]
    console_parts: list[str] = []
    mirrored_generated_roots: dict[str, Path] = {}
    for matrix in selected_matrices:
        matrix_id = str(matrix.get("matrixId") or "")
        execution_context = dict(matrix.get("executionContext") or {})
        target_platform = str(execution_context.get("targetPlatform") or "")
        mirrored_generated_root: Path | None = None
        if target_platform == "windows-x64":
            generated_run_id = _subject_generated_run_id(matrix_id) if multi_matrix_generated else "subject-exec"
            _ensure_subject_generated_source(
                repo_root,
                subject_id=subject_id,
                matrix_id=matrix_id,
                variant=variant,
                run_id=generated_run_id,
                refresh_generated=refresh_generated or multi_matrix_generated,
                refresh_if_missing=auto_refresh_missing_generated,
            )
            mirrored_generated_root = _mirror_subject_generated_run(
                repo_root,
                workspace_root=workspace_root,
                subject_id=subject_id,
                matrix_id=matrix_id,
                run_id=generated_run_id,
                multi_matrix=multi_matrix_generated,
            )
            mirrored_generated_roots[matrix_id] = mirrored_generated_root
        configure_root = workspace_root / "native" / matrix_id
        _clear_dir(configure_root)
        configure_root.parent.mkdir(parents=True, exist_ok=True)
        configure_args, build_args, primary_open_target = _subject_configure_arguments(
            repo_root,
            subject_id=subject_id,
            matrix=matrix,
            variant=variant,
            workspace_root=workspace_root,
            mirrored_generated_root=mirrored_generated_root,
            configure_root=configure_root,
            cmake_path=cmake_path,
            host_platform=host_platform,
        )
        output = _run_checked(configure_args, repo_root=repo_root, failure_message=f"subject workspace configure failed: {matrix_id}", env=cmake_env)
        if output:
            console_parts.append(output)
        if host_platform == "windows":
            _rewrite_configure_root_vcxproj_project_names(
                configure_root,
                matrix_id=matrix_id,
                primary_open_target=primary_open_target,
                target_platform=target_platform,
                host_platform=host_platform,
            )
            _rewrite_subject_facing_vcxproj_project_references(
                configure_root,
                target_platform=target_platform,
                host_platform=host_platform,
            )

        native_projects = _subject_native_projects(
            repo_root,
            configure_root=configure_root,
            primary_open_target=primary_open_target,
            target_platform=target_platform,
            host_platform=host_platform,
        )
        default_build_native_target = _build_target_from_args(list(build_args), primary_open_target)
        default_open_native_target = _select_subject_native_target(
            requested_open_native_target,
            native_projects,
            default_target=primary_open_target,
        )
        native_project_path = _subject_native_project_path(native_projects, default_open_native_target)
        generated_native_project_path = _subject_native_project_path(native_projects, SUBJECT_GENERATED_NATIVE_TARGET)
        proof_native_project_path = _subject_native_project_path(native_projects, SUBJECT_PROOF_NATIVE_TARGET)
        matrix_payloads.append(
            {
                "matrixId": matrix_id,
                "targetPlatform": target_platform,
                "toolchainProfile": str(execution_context.get("toolchainProfile") or ""),
                "configureRoot": _path_text(repo_root, configure_root),
                "generatedRoot": _path_text(repo_root, mirrored_generated_root) if mirrored_generated_root is not None else None,
                "nativeProjects": native_projects,
                "defaultOpenNativeProject": default_open_native_target,
                "defaultBuildNativeProject": default_build_native_target,
                "generatedNativeProjectPath": generated_native_project_path,
                "proofNativeProjectPath": proof_native_project_path,
                "nativeProjectPath": native_project_path,
                "buildArgs": list(build_args),
                "primaryOpenTarget": default_open_native_target,
            }
        )
        if mirrored_generated_root is not None:
            artifacts.append(_path_text(repo_root, mirrored_generated_root))
        artifacts.append(_path_text(repo_root, configure_root))
        solution_native_project_paths.extend(_subject_solution_native_project_paths(native_projects))
        label_suffix = f" ({matrix_id})" if len(selected_matrices) > 1 else ""
        if native_project_path:
            important_outputs.append({"label": f"Default native project{label_suffix}", "path": native_project_path})
        if generated_native_project_path:
            important_outputs.append({"label": f"Generated native project{label_suffix}", "path": generated_native_project_path})
        if proof_native_project_path:
            important_outputs.append({"label": f"Proof native project{label_suffix}", "path": proof_native_project_path})

    _write_solution_file(solution_path, repo_root, managed_projects, solution_native_project_paths)
    artifacts.insert(1, _path_text(repo_root, solution_path))

    payload = {
        "kind": "subject-workspace",
        "subjectId": subject_id,
        "variant": variant,
        "visualStudioStateVersion": SUBJECT_VISUAL_STUDIO_STATE_VERSION,
        "defaultMatrix": str(selected_matrices[0].get("matrixId") or ""),
        "generatedRoot": _path_text(repo_root, mirrored_generated_roots[str(selected_matrices[0].get("matrixId") or "")])
        if str(selected_matrices[0].get("matrixId") or "") in mirrored_generated_roots
        else None,
        "managedSolutionPath": _path_text(repo_root, solution_path),
        "managedProjects": managed_projects,
        "matrices": matrix_payloads,
    }
    write_json(manifest_path, payload)
    return {
        "manifestPath": _path_text(repo_root, manifest_path),
        "artifacts": artifacts,
        "importantOutputs": important_outputs,
        "consoleText": "\n".join(part for part in console_parts if part),
    }


def _core_requested_host(current_host_platform: str, options: dict[str, object]) -> str:
    requested_host = _text_option(options, "host") or current_host_platform
    if requested_host != current_host_platform:
        raise RuntimeError(f"host override '{requested_host}' is not supported from '{current_host_platform}'")
    return requested_host


def _core_selected_target_specs(host_platform: str, options: dict[str, object]) -> list[dict[str, Any]]:
    requested_target = _text_option(options, "target")
    all_targets = _flag(options, "all-targets")
    available_ids = [
        target_id
        for target_id in CORE_TARGET_ORDER
        if host_platform in set(CORE_TARGET_SPECS[target_id]["hosts"])
    ]
    if not available_ids:
        raise RuntimeError(f"no core targets available for host '{host_platform}'")

    if requested_target:
        if requested_target not in CORE_TARGET_SPECS:
            raise RuntimeError(f"unknown core target: {requested_target}")
        if host_platform not in set(CORE_TARGET_SPECS[requested_target]["hosts"]):
            raise RuntimeError(f"core target '{requested_target}' is not available on host '{host_platform}'")
        return [dict(CORE_TARGET_SPECS[requested_target], targetId=requested_target)]

    if all_targets:
        return [dict(CORE_TARGET_SPECS[target_id], targetId=target_id) for target_id in available_ids]

    default_target_id = DEFAULT_CORE_TARGET_BY_HOST[host_platform]
    return [dict(CORE_TARGET_SPECS[default_target_id], targetId=default_target_id)]


def _core_managed_projects(repo_root: Path) -> list[str]:
    project_paths: list[str] = []
    for project_name in CORE_MANAGED_PROJECTS:
        project_path = repo_root / "src" / "managed" / project_name / f"{project_name}.csproj"
        if not project_path.is_file():
            raise RuntimeError(f"managed core project is missing: {project_path}")
        project_paths.append(_path_text(repo_root, project_path))
    return project_paths


def _core_configure_arguments(
    repo_root: Path,
    *,
    host_platform: str,
    target_spec: dict[str, Any],
    configure_root: Path,
    cmake_path: str,
) -> list[str]:
    target_id = str(target_spec["targetId"])
    preset_target = str(target_spec["presetTarget"])
    if target_id in {"windows-x64-reference", "macos-reference"}:
        return [cmake_path, "--preset", preset_target, "-B", str(configure_root)]

    return [
        cmake_path,
        "-S",
        str(repo_root),
        "-B",
        str(configure_root),
        "-G",
        _platform_gate_generator(host_platform),
        f"-DROADMAP0_PRESET_TARGET={preset_target}",
        "-DROADMAP0_TOOLCHAIN_VALIDATE_ONLY=ON",
        f"-DCMAKE_TOOLCHAIN_FILE={repo_root / str(target_spec['toolchainFile'])}",
    ]


def generate_core_workspace(repo_root: Path, host_platform: str, options: dict[str, object]) -> dict[str, Any]:
    requested_host = _core_requested_host(host_platform, options)
    selected_specs = _core_selected_target_specs(requested_host, options)
    managed_projects = _core_managed_projects(repo_root)
    workspace_root = _core_workspace_root(repo_root, requested_host)
    solution_path = workspace_root / "chaos-il2cpp-core.sln"
    manifest_path = _core_workspace_manifest_path(repo_root, requested_host)
    _write_solution_file(solution_path, repo_root, managed_projects)

    cmake_path, cmake_env = tooling_module.cmake_environment(repo_root)
    if cmake_path is None:
        raise RuntimeError("cmake not found")

    artifacts = [
        _path_text(repo_root, manifest_path),
        _path_text(repo_root, solution_path),
    ]
    console_parts: list[str] = []
    native_targets: list[dict[str, Any]] = []
    for target_spec in selected_specs:
        target_id = str(target_spec["targetId"])
        configure_root = workspace_root / "native" / target_id
        _clear_dir(configure_root)
        configure_root.parent.mkdir(parents=True, exist_ok=True)
        configure_args = _core_configure_arguments(
            repo_root,
            host_platform=requested_host,
            target_spec=target_spec,
            configure_root=configure_root,
            cmake_path=cmake_path,
        )
        output = _run_checked(
            configure_args,
            repo_root=repo_root,
            failure_message=f"core workspace configure failed: {target_id}",
            env=cmake_env,
        )
        if output:
            console_parts.append(output)
        native_targets.append(
            {
                "targetId": target_id,
                "presetTarget": str(target_spec["presetTarget"]),
                "targetPlatform": str(target_spec["targetPlatform"]),
                "deliveryKind": str(target_spec["deliveryKind"]),
                "configureRoot": _path_text(repo_root, configure_root),
                "buildArgs": list(target_spec["buildArgs"]),
                "deployCommand": f"deploy core --host {requested_host} --target {target_id}",
            }
        )
        artifacts.append(_path_text(repo_root, configure_root))

    payload = {
        "kind": "core-workspace",
        "hostPlatform": requested_host,
        "defaultTarget": str(selected_specs[0]["targetId"]),
        "managedSolutionPath": _path_text(repo_root, solution_path),
        "managedProjects": managed_projects,
        "nativeTargets": native_targets,
    }
    write_json(manifest_path, payload)
    return {
        "manifestPath": _path_text(repo_root, manifest_path),
        "artifacts": artifacts,
        "importantOutputs": [
            {"label": "Workspace manifest", "path": _path_text(repo_root, manifest_path)},
            {"label": "Managed solution", "path": _path_text(repo_root, solution_path)},
        ],
        "consoleText": "\n".join(part for part in console_parts if part),
    }


def generate_all_workspaces(repo_root: Path, host_platform: str, options: dict[str, object]) -> dict[str, Any]:
    requested_host = _core_requested_host(host_platform, options)
    refresh_generated = _flag(options, "refresh-generated")
    subject_ids = _discover_subject_ids(repo_root, requested_host)

    artifacts: list[str] = []
    important_outputs: list[dict[str, str]] = []
    console_parts: list[str] = []
    subject_manifest_paths: list[str] = []
    generated_subject_ids: list[str] = []

    manifest_path, report_path = _write_all_solutions_outputs(
        repo_root,
        host_platform=requested_host,
        status="running",
        subject_manifest_paths=[],
        core_manifest_path="",
        generated_subject_ids=[],
        generated_core_targets=[],
    )

    for subject_id in subject_ids:
        outcome = generate_subject_workspace(
            repo_root,
            requested_host,
            {
                "id": f"subject/{subject_id}",
                "all-targets": True,
                "auto-refresh-missing-generated": True,
                "refresh-generated": refresh_generated,
            },
        )
        subject_manifest_paths.append(str(outcome["manifestPath"]))
        generated_subject_ids.append(subject_id)
        artifacts.extend(str(item) for item in list(outcome.get("artifacts") or []))
        important_outputs.extend(dict(item) for item in list(outcome.get("importantOutputs") or []) if isinstance(item, dict))
        console_text = str(outcome.get("consoleText") or "")
        if console_text:
            console_parts.append(console_text)

    core_outcome = generate_core_workspace(
        repo_root,
        requested_host,
        {
            "host": requested_host,
            "all-targets": True,
        },
    )
    core_manifest_path = str(core_outcome["manifestPath"])
    core_manifest = read_json(repo_root / core_manifest_path)
    if not isinstance(core_manifest, dict):
        raise RuntimeError("core workspace manifest must be an object")

    generated_core_targets = [
        str(item.get("targetId") or "")
        for item in list(core_manifest.get("nativeTargets") or [])
        if str(item.get("targetId") or "")
    ]
    _write_all_solutions_outputs(
        repo_root,
        host_platform=requested_host,
        status="completed",
        subject_manifest_paths=subject_manifest_paths,
        core_manifest_path=core_manifest_path,
        generated_subject_ids=generated_subject_ids,
        generated_core_targets=generated_core_targets,
    )

    artifacts.extend(str(item) for item in list(core_outcome.get("artifacts") or []))
    artifacts.extend(
        [
            _path_text(repo_root, manifest_path),
            _path_text(repo_root, report_path),
        ]
    )
    important_outputs.extend(
        [
            {"label": "Solutions manifest", "path": _path_text(repo_root, manifest_path)},
            {"label": "Generation report", "path": _path_text(repo_root, report_path)},
        ]
    )
    console_text = str(core_outcome.get("consoleText") or "")
    if console_text:
        console_parts.append(console_text)

    return {
        "manifestPath": _path_text(repo_root, manifest_path),
        "artifacts": artifacts,
        "importantOutputs": important_outputs,
        "consoleText": "\n".join(part for part in console_parts if part),
    }


def _build_managed_projects(repo_root: Path, managed_projects: list[str]) -> list[str]:
    console_parts: list[str] = []
    for project_path in managed_projects:
        output = _run_checked(
            ["dotnet", "build", str(repo_root / project_path), "-c", "Release"],
            repo_root=repo_root,
            failure_message=f"dotnet build failed: {project_path}",
        )
        if output:
            console_parts.append(output)
    return console_parts


def _selected_entries(
    entries: list[dict[str, Any]],
    *,
    explicit_name: str,
    all_name: str,
    default_key: str | None,
    default_fallback_key: str,
    options: dict[str, object],
) -> list[dict[str, Any]]:
    requested = _text_option(options, explicit_name)
    if requested:
        for entry in entries:
            if str(entry.get(default_fallback_key) or "") == requested:
                return [entry]
        raise RuntimeError(f"requested workspace entry was not found: {requested}")
    if _flag(options, all_name):
        return list(entries)
    if default_key:
        for entry in entries:
            if str(entry.get(default_fallback_key) or "") == default_key:
                return [entry]
    if entries:
        return [entries[0]]
    raise RuntimeError("workspace manifest contains no build entries")


def _subject_matrix_build_args(matrix: dict[str, Any], options: dict[str, object]) -> list[str]:
    default_build_args = [str(item) for item in list(matrix.get("buildArgs") or []) if str(item)]
    native_projects = [dict(item) for item in list(matrix.get("nativeProjects") or []) if isinstance(item, dict)]
    if not native_projects:
        return default_build_args

    default_target = str(matrix.get("defaultBuildNativeProject") or "") or _build_target_from_args(
        default_build_args,
        str(matrix.get("primaryOpenTarget") or ""),
    )
    selected_target = _select_subject_native_target(
        _text_option(options, "native-target"),
        native_projects,
        default_target=default_target,
    )
    selected_project = _find_native_project(native_projects, selected_target)
    selected_build_args = [str(item) for item in list((selected_project or {}).get("buildArgs") or []) if str(item)]
    return selected_build_args or default_build_args


def build_subject_workspace(repo_root: Path, host_platform: str, options: dict[str, object]) -> dict[str, Any]:
    del host_platform
    subject_id = _subject_id_from_options(options)
    manifest_path = _subject_workspace_manifest_path(repo_root, subject_id)
    if not manifest_path.is_file():
        raise RuntimeError(f"subject workspace is missing; run generate project subject first: {manifest_path}")
    manifest = read_json(manifest_path)
    if not isinstance(manifest, dict):
        raise RuntimeError("subject workspace manifest must be an object")

    bootstrap = tooling_module.ensure_dotnet_available("build project subject", "windows")
    if not bootstrap.ready:
        raise RuntimeError("\n".join(bootstrap.errors) or bootstrap.output or "dotnet not available")

    managed_projects = [str(item) for item in list(manifest.get("managedProjects") or []) if str(item)]
    matrices = [dict(item) for item in list(manifest.get("matrices") or []) if isinstance(item, dict)]
    selected = _selected_entries(
        matrices,
        explicit_name="matrix",
        all_name="all-targets",
        default_key=str(manifest.get("defaultMatrix") or ""),
        default_fallback_key="matrixId",
        options=options,
    )

    console_parts = _build_managed_projects(repo_root, managed_projects)
    for matrix in selected:
        build_args = [
            "cmake",
            "--build",
            str(repo_root / str(matrix["configureRoot"])),
            *_subject_matrix_build_args(matrix, options),
        ]
        output = _run_checked(build_args, repo_root=repo_root, failure_message=f"subject workspace build failed: {matrix['matrixId']}")
        if output:
            console_parts.append(output)

    build_report_path = _subject_workspace_root(repo_root, subject_id) / "build.report.json"
    write_json(
        build_report_path,
        {
            "kind": "subject-workspace-build",
            "subjectId": subject_id,
            "workspaceManifestPath": _path_text(repo_root, manifest_path),
            "builtMatrices": [str(item["matrixId"]) for item in selected],
            "managedProjects": managed_projects,
        },
    )
    return {
        "buildReportPath": _path_text(repo_root, build_report_path),
        "artifacts": [_path_text(repo_root, build_report_path)],
        "importantOutputs": [{"label": "Build report", "path": _path_text(repo_root, build_report_path)}],
        "consoleText": "\n".join(part for part in console_parts if part),
    }


def build_core_workspace(repo_root: Path, host_platform: str, options: dict[str, object]) -> dict[str, Any]:
    requested_host = _core_requested_host(host_platform, options)
    manifest_path = _core_workspace_manifest_path(repo_root, requested_host)
    if not manifest_path.is_file():
        raise RuntimeError(f"core workspace is missing; run generate project core first: {manifest_path}")
    manifest = read_json(manifest_path)
    if not isinstance(manifest, dict):
        raise RuntimeError("core workspace manifest must be an object")

    bootstrap = tooling_module.ensure_dotnet_available("build project core", requested_host)
    if not bootstrap.ready:
        raise RuntimeError("\n".join(bootstrap.errors) or bootstrap.output or "dotnet not available")

    managed_projects = [str(item) for item in list(manifest.get("managedProjects") or []) if str(item)]
    native_targets = [dict(item) for item in list(manifest.get("nativeTargets") or []) if isinstance(item, dict)]
    selected = _selected_entries(
        native_targets,
        explicit_name="target",
        all_name="all-targets",
        default_key=str(manifest.get("defaultTarget") or ""),
        default_fallback_key="targetId",
        options=options,
    )

    console_parts = _build_managed_projects(repo_root, managed_projects)
    for target in selected:
        build_args = ["cmake", "--build", str(repo_root / str(target["configureRoot"])), *list(target.get("buildArgs") or [])]
        output = _run_checked(build_args, repo_root=repo_root, failure_message=f"core workspace build failed: {target['targetId']}")
        if output:
            console_parts.append(output)

    build_report_path = _core_workspace_root(repo_root, requested_host) / "build.report.json"
    write_json(
        build_report_path,
        {
            "kind": "core-workspace-build",
            "hostPlatform": requested_host,
            "workspaceManifestPath": _path_text(repo_root, manifest_path),
            "builtTargets": [str(item["targetId"]) for item in selected],
            "managedProjects": managed_projects,
        },
    )
    return {
        "buildReportPath": _path_text(repo_root, build_report_path),
        "artifacts": [_path_text(repo_root, build_report_path)],
        "importantOutputs": [{"label": "Build report", "path": _path_text(repo_root, build_report_path)}],
        "consoleText": "\n".join(part for part in console_parts if part),
    }
