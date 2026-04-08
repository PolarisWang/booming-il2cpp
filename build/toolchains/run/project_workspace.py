from __future__ import annotations

import shutil
import sys
import uuid
from pathlib import Path
from typing import Any

try:
    from . import tooling as tooling_module
    from .common import combine_process_output, read_json, run_process, write_json
    from .testing import subject_executor as subject_executor_module
    from .testing import subject_planner as subject_planner_module
    from .testing import subjects as subjects_module
except ImportError:
    root = Path(__file__).resolve().parent
    sys.path.insert(0, str(root))
    import tooling as tooling_module
    from common import combine_process_output, read_json, run_process, write_json
    from testing import subject_executor as subject_executor_module
    from testing import subject_planner as subject_planner_module
    from testing import subjects as subjects_module


WINDOWS_VISUAL_STUDIO_GENERATOR = "Visual Studio 17 2022"
CSHARP_PROJECT_TYPE_GUID = "9A19103F-16F7-4668-BE54-9A1E7A4F7556"
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


def _write_solution_file(solution_path: Path, repo_root: Path, project_paths: list[str]) -> None:
    solution_path.parent.mkdir(parents=True, exist_ok=True)
    lines = [
        "Microsoft Visual Studio Solution File, Format Version 12.00",
        "# Visual Studio Version 17",
        "VisualStudioVersion = 17.0.31903.59",
        "MinimumVisualStudioVersion = 10.0.40219.1",
    ]

    project_guids: list[str] = []
    for relative_project_path in project_paths:
        project_name = Path(relative_project_path).stem
        project_guid = _solution_project_guid(relative_project_path)
        project_guids.append(project_guid)
        project_relative_path = _path_text(solution_path.parent, repo_root / relative_project_path)
        lines.append(
            f'Project("{{{CSHARP_PROJECT_TYPE_GUID}}}") = "{project_name}", "{project_relative_path}", "{{{project_guid}}}"'
        )
        lines.append("EndProject")

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
    for project_guid in project_guids:
        lines.extend(
            [
                f"\t\t{{{project_guid}}}.Debug|Any CPU.ActiveCfg = Debug|Any CPU",
                f"\t\t{{{project_guid}}}.Debug|Any CPU.Build.0 = Debug|Any CPU",
                f"\t\t{{{project_guid}}}.Release|Any CPU.ActiveCfg = Release|Any CPU",
                f"\t\t{{{project_guid}}}.Release|Any CPU.Build.0 = Release|Any CPU",
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
    return repo_root / "artifacts" / "projects" / "subjects" / subject_id


def _subject_workspace_manifest_path(repo_root: Path, subject_id: str) -> Path:
    return _subject_workspace_root(repo_root, subject_id) / "workspace.manifest.json"


def _subject_generated_root(repo_root: Path, subject_id: str) -> Path:
    return repo_root / "artifacts" / "subjects" / subject_id / "runs" / "subject-exec" / "analysis" / "generated"


def _subject_generated_source_path(repo_root: Path, subject_id: str) -> Path:
    return _subject_generated_root(repo_root, subject_id) / "generated" / "native-reference.generated.cpp"


def _core_workspace_root(repo_root: Path, host_platform: str) -> Path:
    return repo_root / "artifacts" / "projects" / "core" / host_platform


def _core_workspace_manifest_path(repo_root: Path, host_platform: str) -> Path:
    return _core_workspace_root(repo_root, host_platform) / "workspace.manifest.json"


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


def _subject_matrix_goal(manifest: dict[str, Any], matrix: dict[str, Any]) -> str:
    supported_goals = [str(item) for item in list(matrix.get("supportedGoals") or []) if str(item)]
    default_goal = str(manifest.get("defaultGoal") or "")
    if default_goal and default_goal in supported_goals:
        return default_goal
    if supported_goals:
        return supported_goals[0]
    raise RuntimeError(f"matrix '{matrix.get('matrixId')}' does not declare supportedGoals")


def refresh_subject_generated_root(repo_root: Path, subject_id: str, matrix_id: str, variant: str) -> None:
    manifest = subjects_module.load_subject_manifest(repo_root, subject_id)
    matrix = subjects_module.find_matrix(manifest, matrix_id)
    goal_id = _subject_matrix_goal(manifest, matrix)
    plan = subject_planner_module.build_plan(
        repo_root,
        subject_id,
        goal_id=goal_id,
        matrix_id=matrix_id,
        variant=variant,
        run_id="subject-exec",
    )
    stages = [dict(stage) for stage in list(plan.get("stagePlan") or [])]
    cutoff = next(
        (
            index
            for index, stage in enumerate(stages)
            if str(stage.get("kind") or "") == "generated-native-proof"
        ),
        -1,
    )
    if cutoff < 0:
        raise RuntimeError(f"subject '{subject_id}' does not define generated-native-proof")

    plan["stagePlan"] = stages[: cutoff + 1]
    result = subject_executor_module.execute_plan(repo_root, plan, run_id="subject-exec")
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
    configure_root: Path,
    cmake_path: str,
    host_platform: str,
) -> tuple[list[str], list[str], str]:
    matrix_id = str(matrix.get("matrixId") or "")
    execution_context = dict(matrix.get("executionContext") or {})
    target_platform = str(execution_context.get("targetPlatform") or "")
    generated_root = _subject_generated_root(repo_root, subject_id)
    out_root = configure_root / "out"
    runtime_root = configure_root / "runtime"

    if target_platform == "windows-x64":
        source_root = repo_root / "subjects" / subject_id / "validation" / "proof" / "native-reference"
        if not (source_root / "CMakeLists.txt").is_file():
            raise RuntimeError(f"subject native reference project is missing: {source_root}")
        configure_args = [
            cmake_path,
            "-S",
            str(source_root),
            "-B",
            str(configure_root),
            "-G",
            WINDOWS_VISUAL_STUDIO_GENERATOR,
            f"-DCHAOS_SUBJECT_VARIANT={variant}",
            f"-DCHAOS_SUBJECT_GENERATED_ROOT={generated_root}",
            f"-DCHAOS_SUBJECT_BUILD_OUT_ROOT={out_root}",
            f"-DCHAOS_SUBJECT_RUNTIME_ROOT={runtime_root}",
        ]
        return configure_args, ["--config", "Release", "--target", "chaos_subject_reference_proof"], "chaos_subject_reference_proof"

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


def generate_subject_workspace(repo_root: Path, host_platform: str, options: dict[str, object]) -> dict[str, Any]:
    subject_id = _subject_id_from_options(options)
    manifest = subjects_module.load_subject_manifest(repo_root, subject_id)
    variant = _subject_variant(manifest, options)
    selected_matrices = _subject_selected_matrices(manifest, host_platform, options)

    if _flag(options, "refresh-generated"):
        refresh_subject_generated_root(repo_root, subject_id, str(selected_matrices[0]["matrixId"]), variant)

    generated_root = _subject_generated_root(repo_root, subject_id)
    if not _subject_generated_source_path(repo_root, subject_id).is_file():
        raise RuntimeError(f"subject-exec generated root is missing for subject '{subject_id}'")

    cmake_path, cmake_env = tooling_module.cmake_environment(repo_root)
    if cmake_path is None:
        raise RuntimeError("cmake not found")

    managed_projects = _subject_managed_projects(manifest)
    workspace_root = _subject_workspace_root(repo_root, subject_id)
    solution_path = workspace_root / "managed" / f"{subject_id}.sln"
    manifest_path = _subject_workspace_manifest_path(repo_root, subject_id)
    _write_solution_file(solution_path, repo_root, managed_projects)

    matrix_payloads: list[dict[str, Any]] = []
    artifacts = [
        _path_text(repo_root, manifest_path),
        _path_text(repo_root, solution_path),
    ]
    console_parts: list[str] = []
    for matrix in selected_matrices:
        matrix_id = str(matrix.get("matrixId") or "")
        configure_root = workspace_root / "native" / matrix_id
        _clear_dir(configure_root)
        configure_root.parent.mkdir(parents=True, exist_ok=True)
        configure_args, build_args, primary_open_target = _subject_configure_arguments(
            repo_root,
            subject_id=subject_id,
            matrix=matrix,
            variant=variant,
            configure_root=configure_root,
            cmake_path=cmake_path,
            host_platform=host_platform,
        )
        output = _run_checked(configure_args, repo_root=repo_root, failure_message=f"subject workspace configure failed: {matrix_id}", env=cmake_env)
        if output:
            console_parts.append(output)

        execution_context = dict(matrix.get("executionContext") or {})
        matrix_payloads.append(
            {
                "matrixId": matrix_id,
                "targetPlatform": str(execution_context.get("targetPlatform") or ""),
                "toolchainProfile": str(execution_context.get("toolchainProfile") or ""),
                "configureRoot": _path_text(repo_root, configure_root),
                "buildArgs": list(build_args),
                "primaryOpenTarget": primary_open_target,
            }
        )
        artifacts.append(_path_text(repo_root, configure_root))

    payload = {
        "kind": "subject-workspace",
        "subjectId": subject_id,
        "variant": variant,
        "defaultMatrix": str(selected_matrices[0].get("matrixId") or ""),
        "generatedRoot": _path_text(repo_root, generated_root),
        "managedSolutionPath": _path_text(repo_root, solution_path),
        "managedProjects": managed_projects,
        "matrices": matrix_payloads,
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
    solution_path = workspace_root / "managed" / "chaos-il2cpp-core.sln"
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
        build_args = ["cmake", "--build", str(repo_root / str(matrix["configureRoot"])), *list(matrix.get("buildArgs") or [])]
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
