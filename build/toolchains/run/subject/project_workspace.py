from __future__ import annotations

import json
import os
import shutil
import sys
import uuid
from pathlib import Path
import re
from typing import Any

try:
    from ..core import tooling as tooling_module
    from ..core.common import combine_process_output, read_json, run_process, write_json
    from ..testing import compiled_catalog as compiled_catalog_module
    from ..testing import declared_metadata_labels as declared_metadata_labels_module
    from ..testing import subject_executor as subject_executor_module
    from ..testing import subject_planner as subject_planner_module
    from ..testing import subjects as subjects_module
    from ..testing import template_assets as template_assets_module
    from ..testing import verification_layout as verification_layout_module
    from ..testing.events import build_event
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from core import tooling as tooling_module
    from core.common import combine_process_output, read_json, run_process, write_json
    from testing import compiled_catalog as compiled_catalog_module
    from testing import declared_metadata_labels as declared_metadata_labels_module
    from testing import subject_executor as subject_executor_module
    from testing import subject_planner as subject_planner_module
    from testing import subjects as subjects_module
    from testing import template_assets as template_assets_module
    from testing import verification_layout as verification_layout_module
    from testing.events import build_event


def _emit_progress(progress_callback, event_type: str, active_unit: str, completed: int = -1, total: int = -1, **kwargs: Any) -> None:
    if progress_callback is None:
        return
    payload: dict[str, Any] = {"activeUnit": active_unit}
    if completed >= 0 and total > 0:
        payload["completedUnits"] = completed
        payload["totalUnits"] = total
    payload.update(kwargs)
    progress_callback(build_event(event_type, payload))


WINDOWS_VISUAL_STUDIO_GENERATOR = "Visual Studio 17 2022"
CSHARP_PROJECT_TYPE_GUID = "9A19103F-16F7-4668-BE54-9A1E7A4F7556"
VCX_PROJECT_TYPE_GUID = "8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942"
SUBJECT_GENERATED_NATIVE_TARGET = "chaos_subject_generated_native"
SUBJECT_PROOF_NATIVE_TARGET = "chaos_subject_reference_proof"
SUBJECT_BENCHMARK_NATIVE_TARGET = "chaos_subject_native_aot"
SUBJECT_VISUAL_STUDIO_STATE_VERSION = 8
SUBJECT_WORKSPACE_VERSION = 2
_ASSEMBLY_NAME_PATTERN = re.compile(r"<AssemblyName>\s*([^<]+)\s*</AssemblyName>", re.IGNORECASE)
CORE_MANAGED_PROJECTS = [
    "Chaos.IL2CPP.Contracts",
    "Chaos.IL2CPP.Loader",
    "Chaos.IL2CPP.SemanticWorld",
    "Chaos.IL2CPP.MetadataWriter",
    "Chaos.IL2CPP.Generator",
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
_NATIVE_REFERENCE_RUN_SCRIPT_TEMPLATE = "templates/native-proof-run.cmake.tmpl"
_NATIVE_REFERENCE_WORKSPACE_TEMPLATE = "templates/native-reference-workspace.cmake.tmpl"
_NATIVE_GENERATED_TEMPLATE = "templates/native-generated.cmake.tmpl"
_NATIVE_PROOF_TEMPLATE = "templates/native-proof.cmake.tmpl"
_NATIVE_PROOF_MAIN_TEMPLATE = "templates/native-proof-main.cpp.tmpl"
_NATIVE_AOT_WORKSPACE_TEMPLATE = "templates/native-aot-workspace.cmake.tmpl"
_NATIVE_BENCHMARK_TEMPLATE = "templates/native-benchmark.cmake.tmpl"


def _path_text(repo_root: Path, path: Path) -> str:
    try:
        return path.relative_to(repo_root).as_posix()
    except ValueError:
        return path.as_posix()


def _relative_path_text(base_path: Path, target_path: Path) -> str:
    return os.path.relpath(target_path, start=base_path).replace("\\", "/")


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


def _entry_selection_host_kind(entry_selection: dict[str, Any] | None) -> str:
    if not isinstance(entry_selection, dict):
        return ""
    family = str(entry_selection.get("family") or "").strip()
    return {
        "declared-unit-test": "proof-host",
        "declared-benchmark": "benchmark-host",
    }.get(family, "")


def _declared_entry_selection_option(options: dict[str, object]) -> dict[str, Any]:
    payload = options.get("entry-selection")
    if payload is None:
        return {}
    if not isinstance(payload, dict):
        raise RuntimeError("subject workspace option 'entry-selection' must be an object")

    family = str(payload.get("family") or "").strip()
    if family not in {"declared-unit-test", "declared-benchmark"}:
        return {}

    normalized: dict[str, Any] = {"family": family}
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


def _source_entry_selection(source: dict[str, Any] | None) -> dict[str, int]:
    if not isinstance(source, dict):
        return {}

    payload = source.get("entrySelection")
    if payload is None:
        return {}
    if not isinstance(payload, dict):
        raise RuntimeError("subject source entrySelection must be an object")

    entry_kind = payload.get("entryKind")
    entry_slice = payload.get("entrySlice")
    if entry_kind is None and entry_slice is None:
        return {}
    if entry_kind is None or entry_slice is None:
        raise RuntimeError("subject source entrySelection requires both entryKind and entrySlice")
    if isinstance(entry_kind, bool) or not isinstance(entry_kind, int) or entry_kind < 0:
        raise RuntimeError("subject source entrySelection.entryKind must be a non-negative integer")
    if isinstance(entry_slice, bool) or not isinstance(entry_slice, int) or entry_slice < 0:
        raise RuntimeError("subject source entrySelection.entrySlice must be a non-negative integer")

    return {
        "entryKind": int(entry_kind),
        "entrySlice": int(entry_slice),
    }


def _subject_matrix_source_entry_selection(manifest: dict[str, Any], matrix: dict[str, Any]) -> dict[str, int]:
    matrix_source = dict(matrix.get("source") or {})
    if "entry" in matrix_source and "entrySelection" not in matrix_source:
        return {}

    source = dict(manifest.get("source") or {})
    source.update(matrix_source)
    return _source_entry_selection(source)


def _matrix_uses_native_hotupdate_chain(matrix: dict[str, Any], manifest: dict[str, Any]) -> bool:
    execution_context = dict(matrix.get("executionContext") or {})
    matrix_source = dict(matrix.get("source") or {})
    source = dict(manifest.get("source") or {})
    source.update(matrix_source)
    runtime_profile = str(execution_context.get("runtimeProfile") or "").strip().lower()
    toolchain_profile = str(execution_context.get("toolchainProfile") or "").strip().lower()
    engineering_profile = str(matrix.get("engineeringProfile") or manifest.get("engineeringProfile") or "").strip().lower()
    uses_hotupdate = engineering_profile == subjects_module.EngineeringProfile.HOT_UPDATE_HOST.value or (
        "hot-update" in runtime_profile or "hotupdate" in runtime_profile or "hot-update" in toolchain_profile or "hotupdate" in toolchain_profile
    )
    return bool(
        uses_hotupdate
        and bool(source.get("fullAssemblyClosure"))
        and runtime_profile == "native-hotupdate-proof-output"
        and toolchain_profile != "dotnet-managed"
    )


def _effective_generated_stage_kind(
    generated_stage_kind: str,
    entry_selection: dict[str, Any] | None,
    subject_entry_selection: dict[str, int] | None = None,
    *,
    uses_native_hotupdate_chain: bool = False,
) -> str:
    if (
        generated_stage_kind == "generated-native-proof"
        and not uses_native_hotupdate_chain
        and (
            _entry_selection_host_kind(entry_selection) == "proof-host"
            or bool(subject_entry_selection)
        )
    ):
        return "generated-native-aot"
    return generated_stage_kind


def _native_target_alias(target_ref: str) -> str:
    target_id = target_ref.strip().lower()
    if target_id in {"generated", "generated-native"}:
        return SUBJECT_GENERATED_NATIVE_TARGET
    if target_id in {"proof", "proof-native"}:
        return SUBJECT_PROOF_NATIVE_TARGET
    if target_id in {"benchmark", "benchmark-host", "native-aot"}:
        return SUBJECT_BENCHMARK_NATIVE_TARGET
    return target_ref.strip()


def _build_target_from_args(build_args: list[str], fallback_target: str) -> str:
    for index, argument in enumerate(build_args):
        if argument == "--target" and index + 1 < len(build_args):
            return str(build_args[index + 1])
    return fallback_target


def _clear_dir(path: Path) -> None:
    if path.exists():
        shutil.rmtree(path, ignore_errors=True)


def _remove_file_if_exists(path: Path) -> None:
    if path.is_file():
        path.unlink()


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
    benchmark_project_path = _subject_native_project_file_path(
        configure_root,
        target_id=SUBJECT_BENCHMARK_NATIVE_TARGET,
        target_platform=target_platform,
        host_platform=host_platform,
    )

    _filter_vcxproj_project_references(generated_project_path, set())
    _filter_vcxproj_project_references(proof_project_path, set())
    if benchmark_project_path.is_file():
        _filter_vcxproj_project_references(benchmark_project_path, set())


def _subject_native_project_file_path(
    configure_root: Path,
    *,
    target_id: str,
    target_platform: str,
    host_platform: str,
    host_kind: str = "",
) -> Path:
    if host_platform == "windows" and target_platform == "windows-x64":
        if target_id == SUBJECT_GENERATED_NATIVE_TARGET:
            return configure_root / "generated" / f"{target_id}.vcxproj"
        if target_id == SUBJECT_PROOF_NATIVE_TARGET:
            return configure_root / "proof" / f"{target_id}.vcxproj"
        if target_id == SUBJECT_BENCHMARK_NATIVE_TARGET:
            if host_kind == "proof-host":
                return configure_root / "proof" / f"{target_id}.vcxproj"
            return configure_root / "benchmark" / f"{target_id}.vcxproj"

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

    project_records: list[tuple[str, str]] = []
    seen_project_paths: set[tuple[str, str]] = set()
    for relative_project_path in managed_project_paths:
        record = (relative_project_path, CSHARP_PROJECT_TYPE_GUID)
        if record in seen_project_paths:
            continue
        seen_project_paths.add(record)
        project_records.append(record)
    for relative_project_path in list(native_project_paths or []):
        record = (relative_project_path, VCX_PROJECT_TYPE_GUID)
        if record in seen_project_paths:
            continue
        seen_project_paths.add(record)
        project_records.append(record)
    project_names = _solution_project_names(
        [relative_project_path for relative_project_path, _ in project_records]
    )

    solution_folder_type_guid = "2150E333-8FDC-42A3-9474-1A3956D46DE8"
    native_folder_assignments: dict[str, str] = {}
    folder_guids: dict[str, str] = {}

    for relative_project_path in list(native_project_paths or []):
        parent_parts = list(Path(relative_project_path).parent.parts)
        native_idx = next((i for i, p in enumerate(parent_parts) if p == "native"), -1)
        if native_idx >= 0 and native_idx + 1 < len(parent_parts):
            matrix_id = parent_parts[native_idx + 1]
            if matrix_id not in folder_guids:
                folder_guids[matrix_id] = _solution_project_guid(f"folder/{matrix_id}")
            native_folder_assignments[relative_project_path] = matrix_id

    for folder_name, folder_guid in folder_guids.items():
        lines.append(
            f'Project("{{{solution_folder_type_guid}}}") = "{folder_name}", "{folder_name}", "{{{folder_guid}}}"'
        )
        lines.append("EndProject")

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
        ]
    )

    if native_folder_assignments:
        lines.append("\tGlobalSection(NestedProjects) = preSolution")
        for project_guid, _, relative_project_path in project_entries:
            matrix_id = native_folder_assignments.get(relative_project_path)
            if matrix_id is not None:
                lines.append(f"\t\t{{{project_guid}}} = {{{folder_guids[matrix_id]}}}")
        lines.append("\tEndGlobalSection")

    lines.extend(["EndGlobal"])
    solution_path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def _subject_workspace_root(repo_root: Path, subject_id: str) -> Path:
    return verification_layout_module.subject_workspace_root(repo_root, subject_id)


def _subject_workspace_manifest_path(repo_root: Path, subject_id: str) -> Path:
    return verification_layout_module.subject_workspace_manifest_path(repo_root, subject_id)


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


def _generated_stage_source_name(generated_stage_kind: str) -> str:
    if generated_stage_kind == "generated-native-aot":
        return "native-aot.generated.cpp"
    return "native-reference.generated.cpp"


def _generated_stage_manifest_name(generated_stage_kind: str) -> str:
    if generated_stage_kind == "generated-native-aot":
        return "native-aot.manifest.json"
    if generated_stage_kind in {"generated-native-proof", "generated-engine-proof"}:
        return "native-reference.manifest.json"
    return ""


def _generated_stage_translation_unit_paths(generated_root: Path, *, generated_stage_kind: str) -> list[Path]:
    manifest_name = _generated_stage_manifest_name(generated_stage_kind)
    if not manifest_name:
        return []

    manifest_path = generated_root / manifest_name
    if not manifest_path.is_file():
        return []

    try:
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError):
        return []

    if not isinstance(manifest, dict):
        return []

    discovered_paths: list[Path] = []
    for artifact in list(manifest.get("generatedArtifacts") or []):
        if not isinstance(artifact, dict):
            continue
        if str(artifact.get("kind") or "").strip() != "generatedTranslationUnit":
            continue
        artifact_path = str(artifact.get("path") or "").strip()
        if artifact_path:
            discovered_paths.append(generated_root / artifact_path)

    if not discovered_paths:
        for page in list(manifest.get("translationUnitPages") or []):
            if not isinstance(page, dict):
                continue
            page_path = str(page.get("path") or "").strip()
            if page_path:
                discovered_paths.append(generated_root / page_path)

    unique_paths: list[Path] = []
    for path in discovered_paths:
        if path not in unique_paths:
            unique_paths.append(path)
    return unique_paths


def _generated_stage_source_path(generated_root: Path, *, generated_stage_kind: str) -> Path:
    primary_generated_source_path = generated_root / "generated" / _generated_stage_source_name(generated_stage_kind)
    if primary_generated_source_path.is_file():
        return primary_generated_source_path

    discovered_paths = _generated_stage_translation_unit_paths(
        generated_root,
        generated_stage_kind=generated_stage_kind,
    )
    if discovered_paths:
        return discovered_paths[0]
    return primary_generated_source_path


def _subject_generated_source_path(
    repo_root: Path,
    subject_id: str,
    *,
    generated_stage_kind: str,
    run_id: str = "subject-exec",
) -> Path:
    return _generated_stage_source_path(
        _subject_generated_root(repo_root, subject_id, run_id=run_id),
        generated_stage_kind=generated_stage_kind,
    )


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


def _subject_generated_solution_source_path(generated_solution_root: Path, *, generated_stage_kind: str) -> Path:
    return _generated_stage_source_path(
        generated_solution_root / "analysis" / "generated",
        generated_stage_kind=generated_stage_kind,
    )


def _ensure_subject_generated_source(
    repo_root: Path,
    *,
    subject_id: str,
    matrix_id: str,
    generated_stage_kind: str,
    variant: str,
    run_id: str = "subject-exec",
    refresh_generated: bool,
    refresh_if_missing: bool = False,
    entry_selection: dict[str, Any] | None = None,
) -> None:
    generated_source_path = _subject_generated_source_path(
        repo_root,
        subject_id,
        generated_stage_kind=generated_stage_kind,
        run_id=run_id,
    )
    if (
        refresh_generated
        or (refresh_if_missing and not generated_source_path.is_file())
        or _subject_generated_source_is_stale(generated_source_path)
    ):
        refresh_kwargs: dict[str, Any] = {"run_id": run_id}
        if entry_selection:
            refresh_kwargs["entry_selection"] = dict(entry_selection)
        refresh_subject_generated_root(
            repo_root,
            subject_id,
            matrix_id,
            variant,
            **refresh_kwargs,
        )
        generated_source_path = _subject_generated_source_path(
            repo_root,
            subject_id,
            generated_stage_kind=generated_stage_kind,
            run_id=run_id,
        )

    if not generated_source_path.is_file():
        raise RuntimeError(f"subject-exec generated root is missing for subject '{subject_id}'")


def _mirror_subject_generated_run(
    repo_root: Path,
    *,
    workspace_root: Path,
    subject_id: str,
    matrix_id: str,
    generated_stage_kind: str,
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

    mirrored_source_path = _subject_generated_solution_source_path(
        mirrored_root,
        generated_stage_kind=generated_stage_kind,
    )
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
    return verification_layout_module.core_workspace_root(repo_root, host_platform)


def _core_workspace_manifest_path(repo_root: Path, host_platform: str) -> Path:
    return verification_layout_module.core_workspace_manifest_path(repo_root, host_platform)


def _all_solutions_manifest_path(repo_root: Path) -> Path:
    return verification_layout_module.verification_all_manifest_path(repo_root)


def _all_solutions_report_path(repo_root: Path) -> Path:
    return verification_layout_module.verification_all_report_path(repo_root)


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
    solution_path = verification_layout_module.verification_all_solution_path(repo_root)

    write_json(
        manifest_path,
        {
            "kind": "all-workspaces",
            "hostPlatform": host_platform,
            "status": status,
            "solutionPath": _path_text(repo_root, solution_path),
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


def _load_workspace_manifest_payload(repo_root: Path, manifest_path_text: str) -> dict[str, Any]:
    manifest_path = repo_root / manifest_path_text
    payload = read_json(manifest_path)
    if not isinstance(payload, dict):
        raise RuntimeError(f"workspace manifest must be an object: {manifest_path}")
    return payload


def _verification_all_solution_paths(
    repo_root: Path,
    *,
    subject_manifest_paths: list[str],
    core_manifest_path: str,
) -> tuple[list[str], list[str]]:
    managed_project_paths: list[str] = []
    native_project_paths: list[str] = []

    for manifest_path_text in subject_manifest_paths:
        manifest = _load_workspace_manifest_payload(repo_root, manifest_path_text)
        managed_project_paths.extend(_manifest_project_paths(list(manifest.get("managedProjects") or [])))
        managed_project_paths.extend(_manifest_project_paths(list(manifest.get("managedTestProjects") or [])))
        managed_project_paths.extend(_manifest_project_paths(list(manifest.get("hotupdateTestProjects") or [])))
        managed_project_paths.extend(_manifest_project_paths(list(manifest.get("hotupdatePatchProjects") or [])))
        native_project_paths.extend(_manifest_project_paths(list(manifest.get("nativeProjects") or [])))
        native_project_paths.extend(_manifest_project_paths(list(manifest.get("nativeTestProjects") or [])))

    if core_manifest_path:
        core_manifest = _load_workspace_manifest_payload(repo_root, core_manifest_path)
        managed_project_paths.extend(_manifest_project_paths(list(core_manifest.get("managedProjects") or [])))
        native_targets = [dict(item) for item in list(core_manifest.get("nativeTargets") or []) if isinstance(item, dict)]
        native_project_paths.extend(
            _manifest_project_paths(
                [target.get("projectPath") for target in native_targets if str(target.get("projectPath") or "").strip()]
            )
        )

    deduped_managed = _manifest_project_paths(managed_project_paths)
    deduped_native = _manifest_project_paths(native_project_paths)
    return deduped_managed, deduped_native


def _subject_id_from_options(options: dict[str, object]) -> str:
    subject_ref = _text_option(options, "id")
    if not subject_ref.startswith("subject/"):
        raise RuntimeError("subject workspace requires --id subject/<subject-id>")
    return subject_ref.split("/", 1)[1]


def _subject_source_project_paths(repo_root: Path, manifest: dict[str, Any]) -> list[str]:
    project_paths = subjects_module.resolve_source_solution_project_paths(repo_root, manifest)
    if project_paths:
        return project_paths

    primary_project_path = subjects_module.resolve_source_primary_project_path(manifest)
    return [primary_project_path] if primary_project_path else []


def _project_assembly_name(repo_root: Path, project_path_text: str) -> str:
    project_path = repo_root / project_path_text
    if project_path.is_file():
        try:
            project_text = project_path.read_text(encoding="utf-8")
        except OSError:
            project_text = ""
        match = _ASSEMBLY_NAME_PATTERN.search(project_text)
        if match is not None:
            assembly_name = match.group(1).strip()
            if assembly_name:
                return assembly_name
    return Path(project_path_text).stem


def _subject_managed_projects(repo_root: Path, manifest: dict[str, Any]) -> list[dict[str, Any]]:
    subject_id = str(manifest.get("subjectId") or "")
    primary_project_path = subjects_module.resolve_source_primary_project_path(manifest)
    project_records: list[dict[str, Any]] = []
    for project_path in _subject_source_project_paths(repo_root, manifest):
        project_records.append(
            {
                "projectId": f"managed/{subject_id}/{_normalize_project_name_fragment(Path(project_path).stem)}",
                "projectPath": project_path,
                "assemblyName": _project_assembly_name(repo_root, project_path),
                "isPrimary": project_path == primary_project_path,
            }
        )
    return project_records


def _manifest_project_paths(entries: list[Any]) -> list[str]:
    project_paths: list[str] = []
    for entry in entries:
        if isinstance(entry, dict):
            project_path = str(entry.get("projectPath") or "")
        else:
            project_path = str(entry or "")
        if project_path and project_path not in project_paths:
            project_paths.append(project_path)
    return project_paths


def _manifest_workspace_version(manifest: dict[str, Any]) -> int:
    try:
        return int(manifest.get("workspaceVersion") or 0)
    except (TypeError, ValueError):
        return 0


def _manifest_project_records_by_id(entries: list[Any]) -> dict[str, dict[str, Any]]:
    records: dict[str, dict[str, Any]] = {}
    for entry in entries:
        if not isinstance(entry, dict):
            continue
        project_id = str(entry.get("projectId") or "").strip()
        if not project_id:
            continue
        records[project_id] = dict(entry)
    return records


def _subject_matrix_native_entries(manifest: dict[str, Any], matrix: dict[str, Any]) -> list[dict[str, Any]]:
    if _manifest_workspace_version(manifest) < SUBJECT_WORKSPACE_VERSION:
        return [dict(item) for item in list(matrix.get("nativeProjects") or []) if isinstance(item, dict)]

    records_by_id = {
        **_manifest_project_records_by_id(list(manifest.get("nativeProjects") or [])),
        **_manifest_project_records_by_id(list(manifest.get("nativeTestProjects") or [])),
    }
    entries: list[dict[str, Any]] = []
    for project_id in [
        *list(matrix.get("nativeProjectIds") or []),
        *list(matrix.get("nativeTestProjectIds") or []),
    ]:
        record = dict(records_by_id.get(str(project_id) or "") or {})
        if not record:
            continue
        if not str(record.get("projectPath") or ""):
            continue
        if not str(record.get("targetId") or ""):
            record["targetId"] = Path(str(record["projectPath"])).stem
        entries.append(record)
    return entries


def _subject_matrix_configure_root(manifest: dict[str, Any], matrix: dict[str, Any]) -> str:
    native_entries = _subject_matrix_native_entries(manifest, matrix)
    for entry in native_entries:
        configure_root = str(entry.get("configureRoot") or "")
        if configure_root:
            return configure_root
    return str(matrix.get("configureRoot") or "")


def _subject_variant(manifest: dict[str, Any], options: dict[str, object]) -> str:
    variant = _text_option(options, "variant")
    selection = subjects_module.resolve_validation_selection(manifest, variant=variant or None)
    return str(selection["variant"])


def _subject_generated_run_id_override(options: dict[str, object]) -> str:
    return _text_option(options, "generated-run-id")


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


def _subject_matrix_pipeline_id(matrix: dict[str, Any]) -> str:
    return str(matrix.get("pipelineId") or "")


def _subject_matrix_target_platform(matrix: dict[str, Any]) -> str:
    execution_context = dict(matrix.get("executionContext") or {})
    return str(execution_context.get("targetPlatform") or "")


def _subject_matrix_generated_stage_kind(manifest: dict[str, Any], matrix: dict[str, Any]) -> str:
    pipeline_id = _subject_matrix_pipeline_id(matrix)
    if not pipeline_id:
        return ""
    pipeline = subjects_module.find_pipeline(manifest, pipeline_id)
    for stage in list(pipeline.get("stages") or []):
        bucket = str(stage.get("bucket") or "")
        kind = str(stage.get("kind") or "")
        if bucket == "generated" or kind.startswith("generated-"):
            return kind
    return ""


def _subject_matrix_has_generated_stage(manifest: dict[str, Any], matrix: dict[str, Any]) -> bool:
    return bool(_subject_matrix_generated_stage_kind(manifest, matrix))


def _subject_matrix_requires_workspace_configure(manifest: dict[str, Any], matrix: dict[str, Any]) -> bool:
    generated_stage_kind = _subject_matrix_generated_stage_kind(manifest, matrix)
    target_platform = _subject_matrix_target_platform(matrix)
    if generated_stage_kind in {"generated-native-proof", "generated-engine-proof"}:
        return target_platform in {"windows-x64", "android-arm64", "linux-x64"}
    if generated_stage_kind == "generated-native-aot":
        return target_platform == "windows-x64"
    return False


def _subject_matrix_supports_workspace_generation(
    repo_root: Path,
    *,
    subject_id: str,
    manifest: dict[str, Any],
    matrix: dict[str, Any],
) -> bool:
    generated_stage_kind = _subject_matrix_generated_stage_kind(manifest, matrix)
    if not generated_stage_kind:
        return True
    if generated_stage_kind == "generated-native-aot":
        return True

    target_platform = _subject_matrix_target_platform(matrix)
    return target_platform in {"windows-x64", "android-arm64", "linux-x64"}


def _subject_supports_workspace_generation(repo_root: Path, subject_id: str, host_platform: str) -> bool:
    manifest = subjects_module.load_subject_manifest(repo_root, subject_id)
    try:
        selected_matrices = _subject_selected_matrices(manifest, host_platform, {"all-targets": True})
    except Exception:
        return False

    if not selected_matrices:
        return False

    for matrix in selected_matrices:
        if not _subject_matrix_supports_workspace_generation(
            repo_root,
            subject_id=subject_id,
            manifest=manifest,
            matrix=matrix,
        ):
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


def _subject_workspace_default_matrix_id(manifest: dict[str, Any], selected_matrices: list[dict[str, Any]]) -> str:
    manifest_default_matrix_id = str(manifest.get("defaultMatrix") or "")
    selected_matrix_ids = {str(matrix.get("matrixId") or "") for matrix in selected_matrices}
    if manifest_default_matrix_id and manifest_default_matrix_id in selected_matrix_ids:
        return manifest_default_matrix_id
    return str(selected_matrices[0].get("matrixId") or "")


def _materialize_subject_native_reference_source(
    repo_root: Path,
    *,
    workspace_root: Path,
    subject_id: str,
    matrix_id: str,
    generated_source_path: Path,
) -> Path:
    def _native_reference_generated_source_paths(primary_generated_source_path: Path) -> list[Path]:
        generated_source_paths = [primary_generated_source_path]
        native_reference_manifest_path = primary_generated_source_path.parent.parent / "native-reference.manifest.json"
        if not native_reference_manifest_path.is_file():
            return generated_source_paths

        native_reference_manifest = read_json(native_reference_manifest_path)
        if not isinstance(native_reference_manifest, dict):
            return generated_source_paths

        discovered_paths: list[Path] = []
        for artifact in list(native_reference_manifest.get("generatedArtifacts") or []):
            if not isinstance(artifact, dict):
                continue
            if str(artifact.get("kind") or "").strip() != "generatedTranslationUnit":
                continue
            artifact_path = str(artifact.get("path") or "").strip()
            if artifact_path:
                discovered_paths.append(primary_generated_source_path.parent.parent / artifact_path)

        if not discovered_paths:
            for page in list(native_reference_manifest.get("translationUnitPages") or []):
                if not isinstance(page, dict):
                    continue
                page_path = str(page.get("path") or "").strip()
                if page_path:
                    discovered_paths.append(primary_generated_source_path.parent.parent / page_path)

        for path in discovered_paths:
            if path not in generated_source_paths:
                generated_source_paths.append(path)
        return generated_source_paths

    def _render_cmake_source_list(paths: list[Path]) -> str:
        entries = [f'    "{path.as_posix()}"' for path in paths]
        if not entries:
            raise RuntimeError("generated source list cannot be empty")
        return "\n".join(entries)

    materialized_root = _subject_materialized_source_root(workspace_root, matrix_id)
    _clear_dir(materialized_root)
    materialized_root.mkdir(parents=True, exist_ok=True)
    (materialized_root / "generated").mkdir(parents=True, exist_ok=True)
    (materialized_root / "proof").mkdir(parents=True, exist_ok=True)
    proof_main_path = materialized_root / "proof" / "main.cpp"
    (materialized_root / "CMakeLists.txt").write_text(
        template_assets_module.read_template(
            owner_file=__file__,
            relative_template_path=_NATIVE_REFERENCE_WORKSPACE_TEMPLATE,
        ),
        encoding="utf-8",
    )
    (materialized_root / "generated" / "CMakeLists.txt").write_text(
        template_assets_module.render_template(
            owner_file=__file__,
            relative_template_path=_NATIVE_GENERATED_TEMPLATE,
            replacements={
                "GENERATED_INPUT_SOURCES": _render_cmake_source_list(
                    _native_reference_generated_source_paths(generated_source_path)
                ),
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
            owner_file=__file__,
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
    (materialized_root / "proof" / "CMakeLists.txt").write_text(
        template_assets_module.render_template(
            owner_file=__file__,
            relative_template_path=_NATIVE_PROOF_TEMPLATE,
            replacements={
                "PROOF_MAIN": proof_main_path.as_posix(),
            },
        ),
        encoding="utf-8",
    )
    (materialized_root / "proof" / "RunSubjectProof.cmake").write_text(
        template_assets_module.read_template(
            owner_file=__file__,
            relative_template_path=_NATIVE_REFERENCE_RUN_SCRIPT_TEMPLATE,
        ),
        encoding="utf-8",
    )
    return materialized_root


def _materialize_subject_native_aot_source(
    repo_root: Path,
    *,
    workspace_root: Path,
    subject_id: str,
    matrix_id: str,
    generated_source_path: Path,
    host_kind: str = "",
) -> Path:
    resolved_repo_root = repo_root.resolve()
    host_subdir = "proof" if host_kind == "proof-host" else "benchmark"
    host_main_path = (
        resolved_repo_root / "src" / "native" / "proof-host" / "native_aot_main.cpp"
        if host_kind == "proof-host"
        else resolved_repo_root / "src" / "native" / "benchmark-host" / "native_aot_main.cpp"
    )

    materialized_root = _subject_materialized_source_root(workspace_root, matrix_id)
    _clear_dir(materialized_root)
    materialized_root.mkdir(parents=True, exist_ok=True)
    (materialized_root / "generated").mkdir(parents=True, exist_ok=True)
    (materialized_root / host_subdir).mkdir(parents=True, exist_ok=True)
    (materialized_root / "CMakeLists.txt").write_text(
        template_assets_module.render_template(
            owner_file=__file__,
            relative_template_path=_NATIVE_AOT_WORKSPACE_TEMPLATE,
            replacements={
                "HOST_SUBDIR": host_subdir,
            },
        ),
        encoding="utf-8",
    )
    (materialized_root / "generated" / "CMakeLists.txt").write_text(
        template_assets_module.render_template(
            owner_file=__file__,
            relative_template_path=_NATIVE_GENERATED_TEMPLATE,
            replacements={
                "GENERATED_INPUT_SOURCES": f'    "{generated_source_path.as_posix()}"',
            },
        ),
        encoding="utf-8",
    )
    (materialized_root / host_subdir / "CMakeLists.txt").write_text(
        template_assets_module.render_template(
            owner_file=__file__,
            relative_template_path=_NATIVE_BENCHMARK_TEMPLATE,
            replacements={
                "BENCHMARK_MAIN": host_main_path.as_posix(),
            },
        ),
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
    entry_selection: dict[str, Any] | None = None,
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
        entry_selection=entry_selection,
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
    generated_stage_kind: str,
    variant: str,
    workspace_root: Path,
    mirrored_generated_root: Path | None,
    configure_root: Path,
    cmake_path: str,
    host_platform: str,
    host_kind: str = "",
) -> tuple[list[str], list[str], str]:
    matrix_id = str(matrix.get("matrixId") or "")
    execution_context = dict(matrix.get("executionContext") or {})
    target_platform = str(execution_context.get("targetPlatform") or "")
    out_root = configure_root / "out"
    runtime_root = configure_root / "runtime"

    if target_platform == "windows-x64":
        if mirrored_generated_root is None:
            raise RuntimeError(f"matrix '{matrix_id}' is missing mirrored generated source")
        generated_source_path = _subject_generated_solution_source_path(
            mirrored_generated_root,
            generated_stage_kind=generated_stage_kind,
        )
        if not generated_source_path.is_absolute():
            generated_source_path = (repo_root.resolve() / generated_source_path).resolve()
        if generated_stage_kind == "generated-native-aot":
            source_root = _materialize_subject_native_aot_source(
                repo_root,
                workspace_root=workspace_root,
                subject_id=subject_id,
                matrix_id=matrix_id,
                generated_source_path=generated_source_path,
                host_kind=host_kind,
            )
            configure_args = [
                cmake_path,
                "-S",
                str(source_root),
                "-B",
                str(configure_root),
                "-G",
                WINDOWS_VISUAL_STUDIO_GENERATOR,
                f"-DCHAOS_SUBJECT_REPO_ROOT={repo_root.resolve().as_posix()}",
                f"-DCHAOS_SUBJECT_VARIANT={variant}",
                f"-DCHAOS_SUBJECT_BUILD_OUT_ROOT={out_root}",
            ]
            return (
                configure_args,
                ["--config", "Release", "--target", SUBJECT_BENCHMARK_NATIVE_TARGET],
                SUBJECT_BENCHMARK_NATIVE_TARGET,
            )
        source_root = _materialize_subject_native_reference_source(
            repo_root,
            workspace_root=workspace_root,
            subject_id=subject_id,
            matrix_id=matrix_id,
            generated_source_path=generated_source_path,
        )
        configure_args = [
            cmake_path,
            "-S",
            str(source_root),
            "-B",
            str(configure_root),
            "-G",
            WINDOWS_VISUAL_STUDIO_GENERATOR,
            f"-DCHAOS_SUBJECT_REPO_ROOT={repo_root.resolve().as_posix()}",
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
    generated_stage_kind: str,
    target_platform: str,
    host_platform: str,
    host_kind: str = "",
) -> list[dict[str, Any]]:
    if host_platform != "windows":
        return []

    if target_platform == "windows-x64":
        if generated_stage_kind == "generated-native-aot":
            native_test_host_kind = host_kind if host_kind in {"proof-host", "benchmark-host"} else "benchmark-host"
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
                            host_kind=native_test_host_kind,
                        ),
                    ),
                    "buildArgs": ["--config", "Release", "--target", SUBJECT_GENERATED_NATIVE_TARGET],
                },
                {
                    "targetId": SUBJECT_BENCHMARK_NATIVE_TARGET,
                    "kind": "proof-native" if native_test_host_kind == "proof-host" else "benchmark-native",
                    "hostKind": native_test_host_kind,
                    "projectPath": _path_text(
                        repo_root,
                        _subject_native_project_file_path(
                            configure_root,
                            target_id=SUBJECT_BENCHMARK_NATIVE_TARGET,
                            target_platform=target_platform,
                            host_platform=host_platform,
                            host_kind=native_test_host_kind,
                        ),
                    ),
                    "buildArgs": ["--config", "Release", "--target", SUBJECT_BENCHMARK_NATIVE_TARGET],
                },
            ]
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
                            host_kind=host_kind,
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
                            host_kind=host_kind,
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
                    host_kind=host_kind,
                ),
            ),
            "buildArgs": [],
        }
    ]


def _find_native_project(native_projects: list[dict[str, Any]], target_id: str) -> dict[str, Any] | None:
    for item in native_projects:
        if _native_project_target_id(item) == target_id:
            return item
    return None


def _find_native_project_by_project_id(native_projects: list[dict[str, Any]], project_id: str) -> dict[str, Any] | None:
    for item in native_projects:
        if str(item.get("projectId") or "") == project_id:
            return item
    return None


def _native_project_target_id(native_project: dict[str, Any]) -> str:
    target_id = str(native_project.get("targetId") or "").strip()
    if target_id:
        return target_id

    build_args = [str(item) for item in list(native_project.get("buildArgs") or []) if str(item)]
    target_id = _build_target_from_args(build_args, "")
    if target_id:
        return target_id

    project_path = str(native_project.get("projectPath") or "").strip()
    if project_path:
        return Path(project_path).stem
    return ""


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


def _subject_solution_native_project_paths(
    native_projects: list[dict[str, Any]],
    *,
    target_platform: str,
    host_platform: str,
) -> list[str]:
    if host_platform == "windows" and target_platform != "windows-x64":
        return []
    return [
        str(item.get("projectPath") or "")
        for item in native_projects
        if str(item.get("projectPath") or "")
    ]


def _subject_managed_test_projects(
    repo_root: Path,
    *,
    subject_id: str,
    manifest: dict[str, Any],
    workspace_root: Path,
    managed_projects: list[dict[str, Any]],
) -> tuple[list[dict[str, Any]], list[str], list[dict[str, str]], list[str]]:
    declared_catalog = compiled_catalog_module.build_subject_declared_test_catalog(
        repo_root=repo_root,
        subject_id=subject_id,
    )
    unit_entries = [dict(item) for item in list(declared_catalog.get("declaredUnitTests") or []) if isinstance(item, dict)]
    benchmark_entries = [dict(item) for item in list(declared_catalog.get("declaredBenchmarks") or []) if isinstance(item, dict)]
    if not unit_entries and not benchmark_entries:
        return [], [], [], []

    managed_tests_root = workspace_root / "managed-tests"
    generated_root = managed_tests_root / "Generated"
    generated_root.mkdir(parents=True, exist_ok=True)
    for obsolete_path in (
        managed_tests_root / f"{subject_id}.DeclaredProofHost.csproj",
        managed_tests_root / f"{subject_id}.DeclaredBenchmarkHost.csproj",
        generated_root / "ChaosGeneratedDeclaredTests.g.cs",
        generated_root / "ChaosGeneratedDeclaredBenchmarks.g.cs",
    ):
        _remove_file_if_exists(obsolete_path)
    collection_path = generated_root / "declared-tests.collection.json"
    write_json(collection_path, declared_catalog)

    records: list[dict[str, Any]] = []
    solution_project_paths: list[str] = []
    important_outputs: list[dict[str, str]] = []
    artifacts = [_path_text(repo_root, collection_path)]
    shared_runtime_project_path = _path_text(
        repo_root,
        repo_root / "src" / "reference" / "Chaos.TestFramework.Runtime" / "Chaos.TestFramework.Runtime.csproj",
    )

    for project_id_suffix, host_kind, entries in (
        ("proof-host", "proof-host", unit_entries),
        ("benchmark-host", "benchmark-host", benchmark_entries),
    ):
        if not entries:
            continue

        records.append(
            {
                "projectId": f"managed-test/{subject_id}/{project_id_suffix}",
                "projectPath": shared_runtime_project_path,
                "assemblyName": "Chaos.TestFramework.Runtime",
                "hostKind": host_kind,
                "collectionPath": _path_text(repo_root, collection_path),
                "executionModel": "shared-runtime-host",
            }
        )

    return records, solution_project_paths, important_outputs, artifacts


def _subject_hotupdate_patch_projects(
    repo_root: Path,
    *,
    subject_id: str,
    manifest: dict[str, Any],
    managed_projects: list[dict[str, Any]],
) -> list[dict[str, Any]]:
    hotupdate = dict(manifest.get("hotUpdate") or manifest.get("hotupdate") or {})
    patch_project_paths = [str(item) for item in list(hotupdate.get("patchProjectPaths") or []) if str(item)]
    if not patch_project_paths:
        return []

    managed_projects_by_path = {
        str(item.get("projectPath") or ""): dict(item)
        for item in managed_projects
        if str(item.get("projectPath") or "")
    }
    records: list[dict[str, Any]] = []
    for project_path in patch_project_paths:
        managed_project = dict(managed_projects_by_path.get(project_path) or {})
        project_id_suffix = _normalize_project_name_fragment(Path(project_path).stem)
        records.append(
            {
                "projectId": f"hotupdate-patch/{subject_id}/{project_id_suffix}",
                "managedProjectId": str(managed_project.get("projectId") or ""),
                "projectPath": project_path,
                "assemblyName": _project_assembly_name(repo_root, project_path),
            }
        )
    return records


def _subject_hotupdate_test_projects(
    repo_root: Path,
    *,
    subject_id: str,
    workspace_root: Path,
    declared_catalog: dict[str, Any],
    hotupdate_patch_projects: list[dict[str, Any]],
) -> tuple[list[dict[str, Any]], list[str], list[dict[str, str]], list[str]]:
    unit_entries = [dict(item) for item in list(declared_catalog.get("declaredUnitTests") or []) if isinstance(item, dict)]
    benchmark_entries = [dict(item) for item in list(declared_catalog.get("declaredBenchmarks") or []) if isinstance(item, dict)]
    if not hotupdate_patch_projects or (not unit_entries and not benchmark_entries):
        return [], [], [], []

    hotupdate_tests_root = workspace_root / "hotupdate-tests"
    generated_root = hotupdate_tests_root / "Generated"
    generated_root.mkdir(parents=True, exist_ok=True)
    collection_path = generated_root / "declared-tests.collection.json"
    write_json(collection_path, declared_catalog)
    binding_manifest_path = generated_root / "declared-tests.binding.json"
    patch_project_ids = [str(item.get("projectId") or "") for item in hotupdate_patch_projects if str(item.get("projectId") or "")]
    patch_assembly_names = [str(item.get("assemblyName") or "") for item in hotupdate_patch_projects if str(item.get("assemblyName") or "")]
    entry_bindings = [
        {
            "hostKind": host_kind,
            "entryIndex": int(entry.get("entryIndex") or 0),
            "assemblyName": str(entry.get("assemblyName") or ""),
            "stableId": str(entry.get("stableId") or ""),
        }
        for host_kind, entries in (
            ("proof-host", unit_entries),
            ("benchmark-host", benchmark_entries),
        )
        for entry in entries
        if str(entry.get("assemblyName") or "")
    ]
    write_json(
        binding_manifest_path,
        {
            "subjectId": subject_id,
            "collectionPath": _path_text(repo_root, collection_path),
            "patchProjectIds": patch_project_ids,
            "patchAssemblyNames": patch_assembly_names,
            "entryBindings": entry_bindings,
        },
    )

    records: list[dict[str, Any]] = []
    solution_project_paths: list[str] = []
    important_outputs: list[dict[str, str]] = []
    artifacts = [
        _path_text(repo_root, binding_manifest_path),
    ]
    shared_runtime_project_path = _path_text(
        repo_root,
        repo_root / "src" / "reference" / "Chaos.TestFramework.Runtime" / "Chaos.TestFramework.Runtime.csproj",
    )

    for host_kind, entries in (
        ("proof-host", unit_entries),
        ("benchmark-host", benchmark_entries),
    ):
        if not entries:
            continue

        record = {
            "projectId": f"hotupdate-test/{subject_id}/{host_kind}",
            "projectPath": shared_runtime_project_path,
            "assemblyName": "Chaos.TestFramework.Runtime",
            "hostKind": host_kind,
            "collectionPath": _path_text(repo_root, collection_path),
            "bindingManifestPath": _path_text(repo_root, binding_manifest_path),
            "patchProjectIds": patch_project_ids,
            "executionModel": "shared-runtime-host",
        }
        records.append(record)
        solution_project_paths.append(record["projectPath"])
        important_outputs.append(
            {
                "label": "Hotupdate proof host project" if host_kind == "proof-host" else "Hotupdate benchmark host project",
                "path": record["projectPath"],
            }
        )
        artifacts.append(record["projectPath"])

    return records, solution_project_paths, important_outputs, artifacts


def _discover_subject_ids(repo_root: Path, host_platform: str) -> list[str]:
    discovered: list[str] = []
    for manifest_path in subjects_module.discover_subject_manifests(repo_root):
        subject_id = str(subjects_module.load_subject_manifest_file(manifest_path).get("subjectId") or manifest_path.parent.name)
        try:
            if not _subject_supports_workspace_generation(repo_root, subject_id, host_platform):
                continue
        except Exception:
            continue
        discovered.append(subject_id)
    return discovered


def generate_subject_workspace(repo_root: Path, host_platform: str, options: dict[str, object], *, progress_callback=None) -> dict[str, Any]:
    subject_id = _subject_id_from_options(options)
    manifest = subjects_module.load_subject_manifest(repo_root, subject_id)
    variant = _subject_variant(manifest, options)
    entry_selection = _declared_entry_selection_option(options)
    declared_host_kind = _entry_selection_host_kind(entry_selection)
    requested_open_native_target = _text_option(options, "open-native-target")
    selected_matrices = _subject_selected_matrices(manifest, host_platform, options)
    refresh_generated = _flag(options, "refresh-generated")
    auto_refresh_missing_generated = _flag(options, "auto-refresh-missing-generated")
    generated_run_id_override = _subject_generated_run_id_override(options)
    generated_matrix_ids = [
        str(matrix.get("matrixId") or "")
        for matrix in selected_matrices
        if _subject_matrix_requires_workspace_configure(manifest, matrix) and _subject_matrix_target_platform(matrix) == "windows-x64"
    ]
    multi_matrix_generated = len(generated_matrix_ids) > 1
    configured_matrices = [
        dict(matrix)
        for matrix in selected_matrices
        if _subject_matrix_requires_workspace_configure(manifest, matrix)
    ]
    cmake_path: str | None = None
    cmake_env: dict[str, str] | None = None
    if configured_matrices:
        cmake_path, cmake_env = tooling_module.cmake_environment(repo_root)
        if cmake_path is None:
            raise RuntimeError("cmake not found")

    managed_projects = _subject_managed_projects(repo_root, manifest)
    workspace_root = _subject_workspace_root(repo_root, subject_id)
    solution_path = workspace_root / f"{subject_id}.sln"
    manifest_path = _subject_workspace_manifest_path(repo_root, subject_id)
    if host_platform == "windows":
        _reset_stale_visual_studio_state(workspace_root, manifest_path)
    _clear_dir(workspace_root / "generated")

    managed_test_projects, solution_managed_test_project_paths, managed_test_outputs, managed_test_artifacts = _subject_managed_test_projects(
        repo_root,
        subject_id=subject_id,
        manifest=manifest,
        workspace_root=workspace_root,
        managed_projects=managed_projects,
    )
    declared_catalog = compiled_catalog_module.build_subject_declared_test_catalog(
        repo_root=repo_root,
        subject_id=subject_id,
    )
    hotupdate_patch_projects = _subject_hotupdate_patch_projects(
        repo_root,
        subject_id=subject_id,
        manifest=manifest,
        managed_projects=managed_projects,
    )
    hotupdate_test_projects, solution_hotupdate_test_project_paths, hotupdate_test_outputs, hotupdate_test_artifacts = _subject_hotupdate_test_projects(
        repo_root,
        subject_id=subject_id,
        workspace_root=workspace_root,
        declared_catalog=declared_catalog,
        hotupdate_patch_projects=hotupdate_patch_projects,
    )
    managed_test_projects_by_host_kind: dict[str, str] = {}
    for item in managed_test_projects:
        host_kind = str(item.get("hostKind") or "")
        project_id = str(item.get("projectId") or "")
        if host_kind and project_id and host_kind not in managed_test_projects_by_host_kind:
            managed_test_projects_by_host_kind[host_kind] = project_id
    hotupdate_test_projects_by_host_kind = {
        str(item.get("hostKind") or ""): str(item.get("projectId") or "")
        for item in hotupdate_test_projects
        if str(item.get("hostKind") or "") and str(item.get("projectId") or "")
    }

    matrix_payloads: list[dict[str, Any]] = []
    solution_native_project_paths: list[str] = []
    workspace_native_projects: list[dict[str, Any]] = []
    workspace_native_test_projects: list[dict[str, Any]] = []
    workspace_hotupdate_patch_projects = list(hotupdate_patch_projects)
    workspace_hotupdate_test_projects = list(hotupdate_test_projects)
    artifacts = [_path_text(repo_root, manifest_path)]
    important_outputs = [
        {"label": "Workspace manifest", "path": _path_text(repo_root, manifest_path)},
        {"label": "Managed solution", "path": _path_text(repo_root, solution_path)},
    ]
    important_outputs.extend(managed_test_outputs)
    important_outputs.extend(hotupdate_test_outputs)
    artifacts.extend(managed_test_artifacts)
    artifacts.extend(hotupdate_test_artifacts)
    console_parts: list[str] = []
    mirrored_generated_roots: dict[str, Path] = {}
    for matrix in selected_matrices:
        matrix_id = str(matrix.get("matrixId") or "")
        _emit_progress(progress_callback, "artifact", f"{subject_id}/{matrix_id}")
        execution_context = dict(matrix.get("executionContext") or {})
        target_platform = str(execution_context.get("targetPlatform") or "")
        generated_stage_kind = _subject_matrix_generated_stage_kind(manifest, matrix)
        subject_entry_selection = _subject_matrix_source_entry_selection(manifest, matrix)
        effective_generated_stage_kind = _effective_generated_stage_kind(
            generated_stage_kind,
            entry_selection,
            subject_entry_selection,
            uses_native_hotupdate_chain=_matrix_uses_native_hotupdate_chain(matrix, manifest),
        )
        matrix_requires_workspace_configure = _subject_matrix_requires_workspace_configure(manifest, matrix)
        mirrored_generated_root: Path | None = None
        if matrix_requires_workspace_configure and target_platform == "windows-x64":
            generated_run_id = (
                generated_run_id_override
                or (_subject_generated_run_id(matrix_id) if multi_matrix_generated else "subject-exec")
            )
            _ensure_subject_generated_source(
                repo_root,
                subject_id=subject_id,
                matrix_id=matrix_id,
                generated_stage_kind=effective_generated_stage_kind,
                variant=variant,
                run_id=generated_run_id,
                refresh_generated=refresh_generated or multi_matrix_generated,
                refresh_if_missing=auto_refresh_missing_generated,
                entry_selection=entry_selection or None,
            )
            mirrored_generated_root = _mirror_subject_generated_run(
                repo_root,
                workspace_root=workspace_root,
                subject_id=subject_id,
                matrix_id=matrix_id,
                generated_stage_kind=effective_generated_stage_kind,
                run_id=generated_run_id,
                multi_matrix=multi_matrix_generated,
            )
            mirrored_generated_roots[matrix_id] = mirrored_generated_root
        configure_root: Path | None = None
        build_args: list[str] = []
        primary_open_target = ""
        native_projects: list[dict[str, Any]] = []
        if matrix_requires_workspace_configure:
            configure_root = workspace_root / "native" / matrix_id
            _clear_dir(configure_root)
            configure_root.parent.mkdir(parents=True, exist_ok=True)
            configure_args, build_args, primary_open_target = _subject_configure_arguments(
                repo_root,
                subject_id=subject_id,
                matrix=matrix,
                generated_stage_kind=effective_generated_stage_kind,
                variant=variant,
                workspace_root=workspace_root,
                mirrored_generated_root=mirrored_generated_root,
                configure_root=configure_root,
                cmake_path=cmake_path or "",
                host_platform=host_platform,
                host_kind=declared_host_kind,
            )
            output = _run_checked(
                configure_args,
                repo_root=repo_root,
                failure_message=f"subject workspace configure failed: {matrix_id}",
                env=cmake_env,
            )
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
                generated_stage_kind=effective_generated_stage_kind,
                target_platform=target_platform,
                host_platform=host_platform,
                host_kind=declared_host_kind,
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
        benchmark_native_project_path = _subject_native_project_path(native_projects, SUBJECT_BENCHMARK_NATIVE_TARGET)
        matrix_native_project_ids: list[str] = []
        matrix_native_test_project_ids: list[str] = []
        matrix_hotupdate_patch_project_ids = [
            str(item.get("projectId") or "")
            for item in hotupdate_patch_projects
            if str(item.get("projectId") or "")
        ]
        matrix_hotupdate_test_project_ids = [
            project_id
            for host_kind, project_id in hotupdate_test_projects_by_host_kind.items()
            if host_kind in {"proof-host", "benchmark-host"} and project_id
        ]
        for native_project in native_projects:
            native_project_path_text = str(native_project.get("projectPath") or "")
            if not native_project_path_text:
                continue
            target_id = str(native_project.get("targetId") or "") or Path(native_project_path_text).stem
            native_project_host_kind = str(native_project.get("hostKind") or "").strip()
            base_payload = {
                "projectPath": native_project_path_text,
                "configureRoot": _path_text(repo_root, configure_root),
                "matrixId": matrix_id,
                "targetPlatform": target_platform,
                "toolchainProfile": str(execution_context.get("toolchainProfile") or ""),
                "buildArgs": [str(item) for item in list(native_project.get("buildArgs") or []) if str(item)],
            }
            if target_id == SUBJECT_GENERATED_NATIVE_TARGET:
                project_id = f"native/{subject_id}/{matrix_id}/generated-native"
                workspace_native_projects.append(
                    {
                        "projectId": project_id,
                        **base_payload,
                        "deliveryKind": "generated-static-library",
                    }
                )
                matrix_native_project_ids.append(project_id)
                continue
            if native_project_host_kind == "proof-host" or target_id == SUBJECT_PROOF_NATIVE_TARGET:
                project_id = f"native-test/{subject_id}/{matrix_id}/proof-host"
                workspace_native_test_projects.append(
                    {
                        "projectId": project_id,
                        **base_payload,
                        "deliveryKind": "direct-run-host",
                        "hostKind": "proof-host",
                        "managedTestProjectId": managed_test_projects_by_host_kind.get("proof-host", ""),
                    }
                )
                matrix_native_test_project_ids.append(project_id)
                continue
            if native_project_host_kind == "benchmark-host" or target_id == SUBJECT_BENCHMARK_NATIVE_TARGET:
                project_id = f"native-test/{subject_id}/{matrix_id}/benchmark-host"
                workspace_native_test_projects.append(
                    {
                        "projectId": project_id,
                        **base_payload,
                        "deliveryKind": "direct-run-host",
                        "hostKind": "benchmark-host",
                        "managedTestProjectId": managed_test_projects_by_host_kind.get("benchmark-host", ""),
                    }
                )
                matrix_native_test_project_ids.append(project_id)
                continue

            project_id = f"native/{subject_id}/{matrix_id}/{_normalize_project_name_fragment(target_id)}"
            workspace_native_projects.append(
                {
                    "projectId": project_id,
                    **base_payload,
                    "deliveryKind": "workspace-primary",
                }
            )
            matrix_native_project_ids.append(project_id)
        matrix_payloads.append(
            {
                "matrixId": matrix_id,
                "goalIds": [str(item) for item in list(matrix.get("supportedGoals") or []) if str(item)],
                "hostPlatform": str(execution_context.get("hostPlatform") or ""),
                "targetPlatform": target_platform,
                "toolchainProfile": str(execution_context.get("toolchainProfile") or ""),
                "managedProjectIds": [str(item.get("projectId") or "") for item in managed_projects if str(item.get("projectId") or "")],
                "managedTestProjectIds": [str(item.get("projectId") or "") for item in managed_test_projects if str(item.get("projectId") or "")],
                "nativeProjectIds": matrix_native_project_ids,
                "nativeTestProjectIds": matrix_native_test_project_ids,
                **(
                    {"hotupdatePatchProjectIds": matrix_hotupdate_patch_project_ids}
                    if matrix_hotupdate_patch_project_ids
                    else {}
                ),
                **(
                    {"hotupdateTestProjectIds": matrix_hotupdate_test_project_ids}
                    if matrix_hotupdate_test_project_ids
                    else {}
                ),
            }
        )
        if mirrored_generated_root is not None:
            artifacts.append(_path_text(repo_root, mirrored_generated_root))
        if configure_root is not None:
            artifacts.append(_path_text(repo_root, configure_root))
        solution_native_project_paths.extend(
            _subject_solution_native_project_paths(
                native_projects,
                target_platform=target_platform,
                host_platform=host_platform,
            )
        )
        label_suffix = f" ({matrix_id})" if len(selected_matrices) > 1 else ""
        if native_project_path:
            important_outputs.append({"label": f"Default native project{label_suffix}", "path": native_project_path})
        if generated_native_project_path:
            important_outputs.append({"label": f"Generated native project{label_suffix}", "path": generated_native_project_path})
        if proof_native_project_path:
            important_outputs.append({"label": f"Proof native project{label_suffix}", "path": proof_native_project_path})
        if benchmark_native_project_path:
            important_outputs.append({"label": f"Benchmark native project{label_suffix}", "path": benchmark_native_project_path})

    _write_solution_file(
        solution_path,
        repo_root,
        [
            *_manifest_project_paths(list(managed_projects)),
            *solution_managed_test_project_paths,
            *solution_hotupdate_test_project_paths,
        ],
        solution_native_project_paths,
    )
    artifacts.insert(1, _path_text(repo_root, solution_path))

    payload = {
        "workspaceVersion": SUBJECT_WORKSPACE_VERSION,
        "kind": "subject-workspace",
        "subjectId": subject_id,
        "variant": variant,
        "defaultMatrixId": _subject_workspace_default_matrix_id(manifest, selected_matrices),
        "managedSolutionPath": _path_text(repo_root, solution_path),
        "managedProjects": managed_projects,
        "managedTestProjects": managed_test_projects,
        "nativeProjects": workspace_native_projects,
        "nativeTestProjects": workspace_native_test_projects,
        "matrices": matrix_payloads,
    }
    if workspace_hotupdate_patch_projects:
        payload["hotupdatePatchProjects"] = workspace_hotupdate_patch_projects
    if workspace_hotupdate_test_projects:
        payload["hotupdateTestProjects"] = workspace_hotupdate_test_projects
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


def generate_core_workspace(repo_root: Path, host_platform: str, options: dict[str, object], *, progress_callback=None) -> dict[str, Any]:
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


def generate_all_workspaces(repo_root: Path, host_platform: str, options: dict[str, object], *, progress_callback=None) -> dict[str, Any]:
    requested_host = _core_requested_host(host_platform, options)
    refresh_generated = _flag(options, "refresh-generated")
    subject_ids = _discover_subject_ids(repo_root, requested_host)

    total_units = len(subject_ids) + 1
    completed_units = 0

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

    _emit_progress(progress_callback, "session-start", f"Generating {len(subject_ids)} subjects + core", 0, total_units)

    for subject_id in subject_ids:
        _emit_progress(progress_callback, "progress", f"subject/{subject_id}", completed_units, total_units, suiteStatus="running")
        outcome = generate_subject_workspace(
            repo_root,
            requested_host,
            {
                "id": f"subject/{subject_id}",
                "all-targets": True,
                "auto-refresh-missing-generated": True,
                "refresh-generated": refresh_generated,
            },
            progress_callback=progress_callback,
        )
        subject_manifest_paths.append(str(outcome["manifestPath"]))
        generated_subject_ids.append(subject_id)
        artifacts.extend(str(item) for item in list(outcome.get("artifacts") or []))
        important_outputs.extend(dict(item) for item in list(outcome.get("importantOutputs") or []) if isinstance(item, dict))
        console_text = str(outcome.get("consoleText") or "")
        if console_text:
            console_parts.append(console_text)
        completed_units += 1
        _emit_progress(progress_callback, "progress", f"subject/{subject_id}", completed_units, total_units, suiteStatus="ok")

    _emit_progress(progress_callback, "progress", "core workspace", completed_units, total_units, suiteStatus="running")
    core_outcome = generate_core_workspace(
        repo_root,
        requested_host,
        {
            "host": requested_host,
            "all-targets": True,
        },
        progress_callback=progress_callback,
    )
    core_manifest_path = str(core_outcome["manifestPath"])
    core_manifest = read_json(repo_root / core_manifest_path)
    if not isinstance(core_manifest, dict):
        raise RuntimeError("core workspace manifest must be an object")
    completed_units += 1
    _emit_progress(progress_callback, "progress", "core workspace", completed_units, total_units, suiteStatus="ok")

    generated_core_targets = [
        str(item.get("targetId") or "")
        for item in list(core_manifest.get("nativeTargets") or [])
        if str(item.get("targetId") or "")
    ]
    verification_all_solution_path = verification_layout_module.verification_all_solution_path(repo_root)
    verification_all_managed_projects, verification_all_native_projects = _verification_all_solution_paths(
        repo_root,
        subject_manifest_paths=subject_manifest_paths,
        core_manifest_path=core_manifest_path,
    )
    _write_solution_file(
        verification_all_solution_path,
        repo_root,
        verification_all_managed_projects,
        native_project_paths=verification_all_native_projects,
    )
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
            _path_text(repo_root, verification_all_solution_path),
        ]
    )
    important_outputs.extend(
        [
            {"label": "Solutions manifest", "path": _path_text(repo_root, manifest_path)},
            {"label": "Generation report", "path": _path_text(repo_root, report_path)},
            {"label": "Verification solution", "path": _path_text(repo_root, verification_all_solution_path)},
        ]
    )
    console_text = str(core_outcome.get("consoleText") or "")
    if console_text:
        console_parts.append(console_text)

    _emit_progress(
        progress_callback,
        "final-summary",
        f"Generated {len(generated_subject_ids)} subjects + {len(generated_core_targets)} core targets",
        total_units,
        total_units,
        suiteStatus="ok",
    )

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
    if not default_target:
        for project_id in [str(item) for item in list(matrix.get("nativeTestProjectIds") or []) if str(item)]:
            native_test_project = _find_native_project_by_project_id(native_projects, project_id)
            default_target = _native_project_target_id(native_test_project or {})
            if default_target:
                break
    if not default_target:
        first_native_project = native_projects[0]
        default_target = _native_project_target_id(first_native_project)
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

    managed_projects = _manifest_project_paths(list(manifest.get("managedProjects") or []))
    managed_test_projects = _manifest_project_paths(list(manifest.get("managedTestProjects") or []))
    hotupdate_test_projects = _manifest_project_paths(list(manifest.get("hotupdateTestProjects") or []))
    matrices = [dict(item) for item in list(manifest.get("matrices") or []) if isinstance(item, dict)]
    selected = _selected_entries(
        matrices,
        explicit_name="matrix",
        all_name="all-targets",
        default_key=str(manifest.get("defaultMatrixId") or manifest.get("defaultMatrix") or ""),
        default_fallback_key="matrixId",
        options=options,
    )

    console_parts = _build_managed_projects(repo_root, [*managed_projects, *managed_test_projects, *hotupdate_test_projects])
    for matrix in selected:
        native_projects = _subject_matrix_native_entries(manifest, matrix)
        configure_root = _subject_matrix_configure_root(manifest, matrix)
        if not native_projects and not configure_root:
            continue
        build_args = [
            "cmake",
            "--build",
            str(repo_root / configure_root),
            *_subject_matrix_build_args(
                {
                    **matrix,
                    "nativeProjects": native_projects,
                },
                options,
            ),
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
            "managedProjects": [*managed_projects, *managed_test_projects, *hotupdate_test_projects],
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
