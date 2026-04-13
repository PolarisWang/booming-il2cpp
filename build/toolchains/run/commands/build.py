from __future__ import annotations

import json
import os
import stat
import shutil
import sys
import uuid
from pathlib import Path
from typing import Any, Callable

try:
    from ..core.common import combine_process_output, run_process
    from ..core.result import CommandResult
    from ..subject import project_workspace as workspace_module
    from ..core import tooling as tooling_module
    from ..testing import reporting as reporting_module
    from ..testing import subject_executor as subject_executor_module
    from ..testing.events import build_event
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from core.common import combine_process_output, run_process
    from core.result import CommandResult
    from subject import project_workspace as workspace_module
    from core import tooling as tooling_module
    from testing import reporting as reporting_module
    from testing import subject_executor as subject_executor_module
    from testing.events import build_event


WINDOWS_REFERENCE_SUBJECT_ID = "SolutionCorePack"
WINDOWS_REFERENCE_SUBJECT_MATRIX_ID = "windows-native-check"
WINDOWS_REFERENCE_SUBJECT_VARIANT = "CHECK"


def _emit_event(
    progress_callback: Callable[[dict[str, Any]], None] | None,
    *,
    event_type: str,
    completed: int | None = None,
    total: int | None = None,
    active_unit: str | None = None,
    step_status: str | None = None,
    path: str | None = None,
    message: str | None = None,
) -> None:
    if progress_callback is None:
        return

    payload: dict[str, Any] = {}
    if completed is not None:
        payload["completedUnits"] = completed
    if total is not None:
        payload["totalUnits"] = total
    if active_unit is not None:
        payload["activeUnit"] = active_unit
    if step_status is not None:
        payload["suiteStatus"] = step_status
    if path is not None:
        payload["path"] = path
    if message is not None:
        payload["message"] = message

    progress_callback(
        build_event(
            event_type,
            payload,
            status=step_status or "running",
        )
    )


def _success(
    command_text: str,
    host_platform: str,
    target: str | None,
    output: str,
    artifacts: list[str],
    *,
    important_outputs: list[dict[str, str]] | None = None,
) -> CommandResult:
    return CommandResult.success(
        command=command_text,
        host_platform=host_platform,
        target=target,
        payload={
            "artifacts": artifacts,
            "importantOutputs": list(important_outputs or []),
            "consoleText": output,
        },
        text=f"Run completed: {command_text}\n",
    )


def _failure(command_text: str, host_platform: str, target: str | None, output: str, errors: list[str]) -> CommandResult:
    text = f"Run failed: {command_text}\n"
    if output:
        text += output if output.endswith("\n") else output + "\n"
    text += "".join(f"- {error}\n" for error in errors)
    return CommandResult.failure(
        command=command_text,
        host_platform=host_platform,
        target=target,
        errors=errors,
        payload={
            "artifacts": [],
            "importantOutputs": [],
            "consoleText": output,
        },
        text=text,
    )


def _retry_remove_readonly(func, path: str, excinfo) -> None:
    del excinfo
    os.chmod(path, stat.S_IWRITE)
    func(path)


def _reset_binary_dir(binary_dir: Path) -> None:
    if binary_dir.exists():
        shutil.rmtree(binary_dir, onexc=_retry_remove_readonly)


def allocate_run_scoped_binary_dir(base_dir: Path) -> Path:
    parent = base_dir.parent
    scoped_dir = parent / f"{base_dir.name}-{os.getpid()}-{uuid.uuid4().hex[:8]}"
    scoped_dir.mkdir(parents=True, exist_ok=True)
    return scoped_dir


def _ensure_windows_reference_subject_exec_generated(repo_root: Path) -> None:
    generated_source_path = workspace_module._subject_generated_source_path(repo_root, WINDOWS_REFERENCE_SUBJECT_ID)
    if generated_source_path.is_file():
        return

    workspace_module.refresh_subject_generated_root(
        repo_root,
        WINDOWS_REFERENCE_SUBJECT_ID,
        WINDOWS_REFERENCE_SUBJECT_MATRIX_ID,
        WINDOWS_REFERENCE_SUBJECT_VARIANT,
    )


def _prepare_preset_inputs(repo_root: Path, host_platform: str, preset_name: str) -> None:
    if host_platform == "windows" and preset_name == "windows-x64-reference":
        _ensure_windows_reference_subject_exec_generated(repo_root)


def _host_build_plan(host_platform: str) -> list[dict[str, str]]:
    if host_platform == "windows":
        return [
            {"kind": "native-contract", "target": "abi", "source_dir": "tests/contracts/native/abi", "binary_dir": "artifacts/run/native-contract-abi"},
            {"kind": "native-contract", "target": "bridge", "source_dir": "tests/contracts/native/bridge", "binary_dir": "artifacts/run/native-contract-bridge"},
            {"kind": "preset", "target": "windows-x64-reference", "preset": "windows-x64-reference", "binary_dir": "artifacts/presets/windows-x64-reference"},
            {"kind": "platform-gate", "target": "android-arm64-smoke", "preset_target": "android-arm64-smoke", "toolchain_file": "build/toolchains/android-arm64.cmake", "binary_dir": "artifacts/run/platform/android-arm64-smoke"},
            {"kind": "platform-gate", "target": "linux-x64-packaging", "preset_target": "linux-x64-packaging", "toolchain_file": "build/toolchains/linux-x64.cmake", "binary_dir": "artifacts/run/platform/linux-x64-packaging"},
        ]
    if host_platform == "macos":
        return [
            {"kind": "native-contract", "target": "abi", "source_dir": "tests/contracts/native/abi", "binary_dir": "artifacts/run/native-contract-abi"},
            {"kind": "native-contract", "target": "bridge", "source_dir": "tests/contracts/native/bridge", "binary_dir": "artifacts/run/native-contract-bridge"},
            {"kind": "preset", "target": "macos-reference", "preset": "macos-reference", "binary_dir": "artifacts/presets/macos-reference"},
            {"kind": "platform-gate", "target": "ios-arm64-packaging", "preset_target": "ios-arm64-packaging", "toolchain_file": "build/toolchains/ios-arm64.cmake", "binary_dir": "artifacts/run/platform/ios-arm64-packaging"},
            {"kind": "platform-gate", "target": "linux-x64-packaging", "preset_target": "linux-x64-packaging", "toolchain_file": "build/toolchains/linux-x64.cmake", "binary_dir": "artifacts/run/platform/linux-x64-packaging"},
        ]
    return [
        {"kind": "native-contract", "target": "abi", "source_dir": "tests/contracts/native/abi", "binary_dir": "artifacts/run/native-contract-abi"},
        {"kind": "native-contract", "target": "bridge", "source_dir": "tests/contracts/native/bridge", "binary_dir": "artifacts/run/native-contract-bridge"},
        {"kind": "platform-gate", "target": "linux-x64-packaging", "preset_target": "linux-x64-packaging", "toolchain_file": "build/toolchains/linux-x64.cmake", "binary_dir": "artifacts/run/platform/linux-x64-packaging"},
    ]


def _build_native_contract(
    command: dict,
    repo_root: Path,
    host_platform: str,
    command_text: str,
    progress_callback: Callable[[dict[str, Any]], None] | None = None,
) -> CommandResult:
    source_dir = repo_root / command["source_dir"]
    requested_binary_dir = repo_root / command["binary_dir"]
    binary_dir = allocate_run_scoped_binary_dir(requested_binary_dir)
    cmake_path, cmake_env = tooling_module.cmake_environment(repo_root)
    if cmake_path is None:
        return _failure(command_text, host_platform, command.get("target"), "", ["cmake not found"])

    _emit_event(progress_callback, event_type="stage-start", completed=0, total=2, active_unit="configure")
    configure = run_process([cmake_path, "-S", str(source_dir), "-B", str(binary_dir)], cwd=repo_root, env=cmake_env)
    configure_output = combine_process_output(configure)
    if configure.returncode != 0:
        _emit_event(progress_callback, event_type="progress", completed=0, total=2, active_unit="configure", step_status="fail")
        return _failure(command_text, host_platform, command.get("target"), configure_output, ["cmake configure failed"])

    _emit_event(progress_callback, event_type="progress", completed=1, total=2, active_unit="configure", step_status="ok")
    _emit_event(progress_callback, event_type="stage-start", completed=1, total=2, active_unit="build")
    build = run_process([cmake_path, "--build", str(binary_dir)], cwd=repo_root, env=cmake_env)
    build_output = combine_process_output(build)
    output = "\n".join(part for part in [configure_output, build_output] if part)
    if build.returncode != 0:
        _emit_event(progress_callback, event_type="progress", completed=1, total=2, active_unit="build", step_status="fail")
        return _failure(command_text, host_platform, command.get("target"), output, ["cmake build failed"])

    _emit_event(progress_callback, event_type="progress", completed=2, total=2, active_unit="build", step_status="ok")
    _emit_event(progress_callback, event_type="artifact", path=str(binary_dir))
    return _success(command_text, host_platform, command.get("target"), output, [str(binary_dir)])


def _build_batch(
    command: dict,
    repo_root: Path,
    host_platform: str,
    command_text: str,
    progress_callback: Callable[[dict[str, Any]], None] | None = None,
) -> CommandResult:
    steps = _host_build_plan(host_platform)
    total = len(steps)
    artifacts: list[str] = []
    console_parts: list[str] = []

    for index, step in enumerate(steps):
        step_text = f"build {step['target']}"
        _emit_event(progress_callback, event_type="stage-start", completed=index, total=total, active_unit=step_text)
        result = handle(step, repo_root, host_platform, step_text, progress_callback=None)
        console_text = str(result.payload.get("consoleText", result.text or ""))
        if console_text:
            console_parts.append(console_text)
        if result.status != "ok":
            _emit_event(progress_callback, event_type="progress", completed=index, total=total, active_unit=step_text, step_status="fail")
            return CommandResult.failure(
                command=command_text,
                host_platform=host_platform,
                target=command.get("target"),
                errors=[f"build all failed while executing {step_text}"],
                payload={
                    "artifacts": artifacts,
                    "importantOutputs": [],
                    "consoleText": "\n".join(console_parts),
                },
                text=f"Run failed: {command_text}\n- build all failed while executing {step_text}\n",
            )
        _emit_event(progress_callback, event_type="progress", completed=index + 1, total=total, active_unit=step_text, step_status="ok")
        for artifact in list(result.payload.get("artifacts", [])):
            artifact_text = str(artifact)
            artifacts.append(artifact_text)
            _emit_event(progress_callback, event_type="artifact", path=artifact_text)

    return CommandResult.success(
        command=command_text,
        host_platform=host_platform,
        target=command.get("target"),
        payload={
            "artifacts": artifacts,
            "importantOutputs": [],
            "consoleText": "\n".join(console_parts),
        },
        text=f"Run completed: {command_text}\n",
    )


def _build_smoke_project(
    command: dict,
    repo_root: Path,
    host_platform: str,
    command_text: str,
    progress_callback: Callable[[dict[str, Any]], None] | None = None,
) -> CommandResult:
    bootstrap = tooling_module.ensure_dotnet_available(command_text, host_platform)
    if not bootstrap.ready:
        return _failure(command_text, host_platform, command.get("target"), bootstrap.output, bootstrap.errors)

    project_path = repo_root / command["project_path"]
    _emit_event(progress_callback, event_type="stage-start", completed=0, total=1, active_unit="build")
    completed = run_process(["dotnet", "build", str(project_path), "-c", "Release"], cwd=repo_root)
    output = "\n".join(part for part in [bootstrap.output.strip(), combine_process_output(completed)] if part)
    if completed.returncode != 0:
        _emit_event(progress_callback, event_type="progress", completed=0, total=1, active_unit="build", step_status="fail")
        return _failure(command_text, host_platform, command.get("target"), output, ["dotnet build failed"])

    artifact_path = str(repo_root / command["artifact_path"])
    _emit_event(progress_callback, event_type="progress", completed=1, total=1, active_unit="build", step_status="ok")
    _emit_event(progress_callback, event_type="artifact", path=artifact_path)
    return _success(command_text, host_platform, command.get("target"), output, [artifact_path])


def _build_preset(
    command: dict,
    repo_root: Path,
    host_platform: str,
    command_text: str,
    progress_callback: Callable[[dict[str, Any]], None] | None = None,
) -> CommandResult:
    preset_name = command["preset"]
    requested_binary_dir = repo_root / command["binary_dir"]
    binary_dir = allocate_run_scoped_binary_dir(requested_binary_dir)
    cmake_path, cmake_env = tooling_module.cmake_environment(repo_root)
    if cmake_path is None:
        return _failure(command_text, host_platform, command.get("target"), "", ["cmake not found"])

    try:
        _prepare_preset_inputs(repo_root, host_platform, preset_name)
    except Exception as error:
        return _failure(command_text, host_platform, command.get("target"), str(error), [str(error)])

    _emit_event(progress_callback, event_type="stage-start", completed=0, total=2, active_unit="configure")
    configure = run_process([cmake_path, "--preset", preset_name, "-B", str(binary_dir)], cwd=repo_root, env=cmake_env)
    configure_output = combine_process_output(configure)
    if configure.returncode != 0:
        _emit_event(progress_callback, event_type="progress", completed=0, total=2, active_unit="configure", step_status="fail")
        return _failure(command_text, host_platform, command.get("target"), configure_output, ["cmake preset configure failed"])

    _emit_event(progress_callback, event_type="progress", completed=1, total=2, active_unit="configure", step_status="ok")
    _emit_event(progress_callback, event_type="stage-start", completed=1, total=2, active_unit="build")
    build = run_process([cmake_path, "--build", str(binary_dir)], cwd=repo_root, env=cmake_env)
    build_output = combine_process_output(build)
    output = "\n".join(part for part in [configure_output, build_output] if part)
    if build.returncode != 0:
        _emit_event(progress_callback, event_type="progress", completed=1, total=2, active_unit="build", step_status="fail")
        return _failure(command_text, host_platform, command.get("target"), output, ["cmake preset build failed"])

    _emit_event(progress_callback, event_type="progress", completed=2, total=2, active_unit="build", step_status="ok")
    _emit_event(progress_callback, event_type="artifact", path=str(binary_dir))
    return _success(command_text, host_platform, command.get("target"), output, [str(binary_dir)])


def _write_gate_record(
    output_path: Path,
    *,
    gate_name: str,
    host_platform: str,
    preset: str,
    notes: str,
    subject_id: str | None = None,
    matrix_id: str | None = None,
    subject_run_id: str | None = None,
    trace_paths: list[str] | None = None,
) -> None:
    payload: dict[str, Any] = {
        "gateName": gate_name,
        "hostProfile": host_platform,
        "status": "passed",
        "preset": preset,
        "notes": notes,
    }
    if subject_id:
        payload["subjectId"] = subject_id
    if matrix_id:
        payload["matrixId"] = matrix_id
    if subject_run_id:
        payload["subjectRunId"] = subject_run_id
    if trace_paths:
        payload["tracePaths"] = list(trace_paths)

    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(
        json.dumps(
            payload,
            indent=2,
        ),
        encoding="utf-8",
    )


def _build_reference_desktop_gate(
    command: dict,
    repo_root: Path,
    host_platform: str,
    command_text: str,
    progress_callback: Callable[[dict[str, Any]], None] | None = None,
) -> CommandResult:
    bootstrap = tooling_module.ensure_dotnet_available(command_text, host_platform)
    if not bootstrap.ready:
        return _failure(command_text, host_platform, command.get("target"), bootstrap.output, bootstrap.errors)

    cmake_path, cmake_env = tooling_module.cmake_environment(repo_root)
    if cmake_path is None:
        return _failure(command_text, host_platform, command.get("target"), bootstrap.output, ["cmake not found"])

    try:
        _prepare_preset_inputs(repo_root, host_platform, str(command["preset"]))
    except Exception as error:
        return _failure(command_text, host_platform, command.get("target"), str(error), [str(error)])

    requested_binary_dir = repo_root / command["binary_dir"]
    binary_dir = allocate_run_scoped_binary_dir(requested_binary_dir)
    gate_record_path = repo_root / command["gate_record_path"]

    output_parts: list[str] = []
    if bootstrap.output.strip():
        output_parts.append(bootstrap.output.strip())

    _emit_event(progress_callback, event_type="stage-start", completed=0, total=3, active_unit="configure")
    configure = run_process([cmake_path, "--preset", command["preset"], "-B", str(binary_dir)], cwd=repo_root, env=cmake_env)
    configure_output = combine_process_output(configure)
    if configure_output:
        output_parts.append(configure_output)
    if configure.returncode != 0:
        _emit_event(progress_callback, event_type="progress", completed=0, total=3, active_unit="configure", step_status="fail")
        return _failure(command_text, host_platform, command.get("target"), "\n".join(output_parts), ["cmake preset configure failed"])

    _emit_event(progress_callback, event_type="progress", completed=1, total=3, active_unit="configure", step_status="ok")
    _emit_event(progress_callback, event_type="stage-start", completed=1, total=3, active_unit="build")
    build = run_process([cmake_path, "--build", str(binary_dir)], cwd=repo_root, env=cmake_env)
    build_output = combine_process_output(build)
    if build_output:
        output_parts.append(build_output)
    if build.returncode != 0:
        _emit_event(progress_callback, event_type="progress", completed=1, total=3, active_unit="build", step_status="fail")
        return _failure(command_text, host_platform, command.get("target"), "\n".join(output_parts), ["cmake preset build failed"])

    _emit_event(progress_callback, event_type="progress", completed=2, total=3, active_unit="build", step_status="ok")
    _emit_event(progress_callback, event_type="stage-start", completed=2, total=3, active_unit="subject-trace")
    subject_run_id = reporting_module.build_run_id(host_platform)
    try:
        execution_result = subject_executor_module.execute_subject_matrix(
            repo_root,
            str(command["subject_id"]),
            goal_id=str(command.get("goal_id") or "") or None,
            matrix_id=str(command.get("matrix_id") or "") or None,
            run_id=subject_run_id,
        )
    except Exception as error:
        _emit_event(progress_callback, event_type="progress", completed=2, total=3, active_unit="subject-trace", step_status="fail")
        error_text = str(error)
        combined_output = "\n".join(part for part in [*output_parts, error_text] if part)
        return _failure(command_text, host_platform, command.get("target"), combined_output, [error_text])

    execution_errors = [str(item) for item in list(execution_result.get("errors") or []) if str(item)]
    if str(execution_result.get("status") or "fail") != "ok":
        _emit_event(progress_callback, event_type="progress", completed=2, total=3, active_unit="subject-trace", step_status="fail")
        combined_output = "\n".join(part for part in [*output_parts, *execution_errors] if part)
        return _failure(
            command_text,
            host_platform,
            command.get("target"),
            combined_output,
            execution_errors or ["subject trace pipeline failed"],
        )

    trace_paths = subject_executor_module.trace_paths_from_execution(repo_root, execution_result)
    if not trace_paths:
        _emit_event(progress_callback, event_type="progress", completed=2, total=3, active_unit="subject-trace", step_status="fail")
        return _failure(command_text, host_platform, command.get("target"), "\n".join(output_parts), ["subject trace pipeline produced no trace artifacts"])

    _emit_event(progress_callback, event_type="progress", completed=3, total=3, active_unit="subject-trace", step_status="ok")

    _write_gate_record(
        gate_record_path,
        gate_name=command["gate_name"],
        host_platform=host_platform,
        preset=command["gate_preset"],
        notes=command["gate_notes"],
        subject_id=str(command.get("subject_id") or ""),
        matrix_id=str(command.get("matrix_id") or ""),
        subject_run_id=subject_run_id,
        trace_paths=trace_paths,
    )

    trace_artifacts = [str(repo_root / trace_path) for trace_path in trace_paths]
    artifacts = [str(binary_dir), *trace_artifacts, str(gate_record_path)]
    for artifact in artifacts:
        _emit_event(progress_callback, event_type="artifact", path=artifact)

    return _success(
        command_text,
        host_platform,
        command.get("target"),
        "\n".join(output_parts),
        artifacts,
        important_outputs=[
            {"label": "Reference preset output", "path": str(binary_dir)},
            *[
                {"label": f"Subject trace {index + 1}", "path": artifact}
                for index, artifact in enumerate(trace_artifacts)
            ],
            {"label": "Gate record", "path": str(gate_record_path)},
        ],
    )


def _platform_gate_generator(preset_target: str, host_platform: str) -> str:
    if host_platform == "windows":
        return "Visual Studio 17 2022"
    if host_platform == "macos":
        return "Xcode"
    return "Ninja"


def _build_platform_gate(
    command: dict,
    repo_root: Path,
    host_platform: str,
    command_text: str,
    progress_callback: Callable[[dict[str, Any]], None] | None = None,
) -> CommandResult:
    requested_binary_dir = repo_root / command["binary_dir"]
    toolchain_file = repo_root / command["toolchain_file"]
    preset_target = command["preset_target"]
    cmake_path, cmake_env = tooling_module.cmake_environment(repo_root)
    if cmake_path is None:
        return _failure(command_text, host_platform, command.get("target"), "", ["cmake not found"])

    binary_dir = allocate_run_scoped_binary_dir(requested_binary_dir)

    configure_args = [
        cmake_path,
        "-S",
        str(repo_root),
        "-B",
        str(binary_dir),
        "-G",
        _platform_gate_generator(preset_target, host_platform),
        f"-DROADMAP0_PRESET_TARGET={preset_target}",
        "-DROADMAP0_TOOLCHAIN_VALIDATE_ONLY=ON",
        f"-DCMAKE_TOOLCHAIN_FILE={toolchain_file}",
    ]

    _emit_event(progress_callback, event_type="stage-start", completed=0, total=2, active_unit="configure")
    configure = run_process(configure_args, cwd=repo_root, env=cmake_env)
    configure_output = combine_process_output(configure)
    if configure.returncode != 0:
        _emit_event(progress_callback, event_type="progress", completed=0, total=2, active_unit="configure", step_status="fail")
        return _failure(command_text, host_platform, command.get("target"), configure_output, ["cmake routing configure failed"])

    _emit_event(progress_callback, event_type="progress", completed=1, total=2, active_unit="configure", step_status="ok")
    _emit_event(progress_callback, event_type="stage-start", completed=1, total=2, active_unit="build")
    build = run_process([cmake_path, "--build", str(binary_dir)], cwd=repo_root, env=cmake_env)
    build_output = combine_process_output(build)
    output = "\n".join(part for part in [configure_output, build_output] if part)
    if build.returncode != 0:
        _emit_event(progress_callback, event_type="progress", completed=1, total=2, active_unit="build", step_status="fail")
        return _failure(command_text, host_platform, command.get("target"), output, ["cmake routing build failed"])

    _emit_event(progress_callback, event_type="progress", completed=2, total=2, active_unit="build", step_status="ok")
    _emit_event(progress_callback, event_type="artifact", path=str(binary_dir))
    return _success(command_text, host_platform, command.get("target"), output, [str(binary_dir)])


def _build_project_workspace(
    command: dict,
    repo_root: Path,
    host_platform: str,
    command_text: str,
    options: dict[str, object] | None = None,
) -> CommandResult:
    try:
        project_kind = str(command.get("project_kind") or "")
        if project_kind == "subject":
            outcome = workspace_module.build_subject_workspace(repo_root, host_platform, dict(options or {}))
        elif project_kind == "core":
            outcome = workspace_module.build_core_workspace(repo_root, host_platform, dict(options or {}))
        else:
            raise RuntimeError(f"unsupported project workspace kind: {project_kind}")
    except Exception as error:
        return _failure(command_text, host_platform, command.get("target"), str(error), [str(error)])

    return _success(
        command_text,
        host_platform,
        command.get("target"),
        str(outcome.get("consoleText") or ""),
        list(outcome.get("artifacts") or []),
        important_outputs=list(outcome.get("importantOutputs") or []),
    )


def handle(
    command: dict,
    repo_root: Path,
    host_platform: str,
    command_text: str,
    options: dict[str, object] | None = None,
    progress_callback: Callable[[dict[str, Any]], None] | None = None,
) -> CommandResult:
    kind = command["kind"]
    if kind == "batch":
        return _build_batch(command, repo_root, host_platform, command_text, progress_callback=progress_callback)
    if kind == "project-workspace":
        return _build_project_workspace(command, repo_root, host_platform, command_text, options)
    if kind == "native-contract":
        return _build_native_contract(command, repo_root, host_platform, command_text, progress_callback=progress_callback)
    if kind == "smoke-project":
        return _build_smoke_project(command, repo_root, host_platform, command_text, progress_callback=progress_callback)
    if kind == "preset":
        return _build_preset(command, repo_root, host_platform, command_text, progress_callback=progress_callback)
    if kind == "reference-desktop-gate":
        return _build_reference_desktop_gate(command, repo_root, host_platform, command_text, progress_callback=progress_callback)
    if kind == "platform-gate":
        return _build_platform_gate(command, repo_root, host_platform, command_text, progress_callback=progress_callback)

    return _failure(command_text, host_platform, command.get("target"), "", [f"unsupported build kind: {kind}"])
