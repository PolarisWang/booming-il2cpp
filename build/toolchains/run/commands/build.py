from __future__ import annotations

import os
import stat
import shutil
import sys
from pathlib import Path
from typing import Any, Callable

try:
    from ..common import combine_process_output, run_process
    from ..result import CommandResult
    from .. import tooling as tooling_module
    from ..testing.events import build_event
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from common import combine_process_output, run_process
    from result import CommandResult
    import tooling as tooling_module
    from testing.events import build_event


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
        text="".join([f"Run failed: {command_text}\n", *[f"- {error}\n" for error in errors]]),
    )


def _retry_remove_readonly(func, path: str, excinfo) -> None:
    del excinfo
    os.chmod(path, stat.S_IWRITE)
    func(path)


def _reset_binary_dir(binary_dir: Path) -> None:
    if binary_dir.exists():
        shutil.rmtree(binary_dir, onexc=_retry_remove_readonly)


def _host_build_plan(host_platform: str) -> list[dict[str, str]]:
    if host_platform == "windows":
        return [
            {"kind": "native-contract", "target": "abi", "source_dir": "tests/contract/native/abi", "binary_dir": "artifacts/run/native-contract-abi"},
            {"kind": "native-contract", "target": "bridge", "source_dir": "tests/contract/native/bridge", "binary_dir": "artifacts/run/native-contract-bridge"},
            {"kind": "preset", "target": "windows-x64-reference", "preset": "windows-x64-reference", "binary_dir": "artifacts/presets/windows-x64-reference"},
            {"kind": "platform-gate", "target": "android-arm64-smoke", "preset_target": "android-arm64-smoke", "toolchain_file": "build/toolchains/android-arm64.cmake", "binary_dir": "artifacts/run/platform/android-arm64-smoke"},
            {"kind": "platform-gate", "target": "linux-x64-packaging", "preset_target": "linux-x64-packaging", "toolchain_file": "build/toolchains/linux-x64.cmake", "binary_dir": "artifacts/run/platform/linux-x64-packaging"},
        ]
    if host_platform == "macos":
        return [
            {"kind": "native-contract", "target": "abi", "source_dir": "tests/contract/native/abi", "binary_dir": "artifacts/run/native-contract-abi"},
            {"kind": "native-contract", "target": "bridge", "source_dir": "tests/contract/native/bridge", "binary_dir": "artifacts/run/native-contract-bridge"},
            {"kind": "preset", "target": "macos-reference", "preset": "macos-reference", "binary_dir": "artifacts/presets/macos-reference"},
            {"kind": "platform-gate", "target": "ios-arm64-packaging", "preset_target": "ios-arm64-packaging", "toolchain_file": "build/toolchains/ios-arm64.cmake", "binary_dir": "artifacts/run/platform/ios-arm64-packaging"},
            {"kind": "platform-gate", "target": "linux-x64-packaging", "preset_target": "linux-x64-packaging", "toolchain_file": "build/toolchains/linux-x64.cmake", "binary_dir": "artifacts/run/platform/linux-x64-packaging"},
        ]
    return [
        {"kind": "native-contract", "target": "abi", "source_dir": "tests/contract/native/abi", "binary_dir": "artifacts/run/native-contract-abi"},
        {"kind": "native-contract", "target": "bridge", "source_dir": "tests/contract/native/bridge", "binary_dir": "artifacts/run/native-contract-bridge"},
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
    binary_dir = repo_root / command["binary_dir"]
    cmake_path, cmake_env = tooling_module.cmake_environment(repo_root)
    if cmake_path is None:
        return _failure(command_text, host_platform, command.get("target"), "", ["cmake not found"])

    _reset_binary_dir(binary_dir)

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
    binary_dir = repo_root / command["binary_dir"]
    cmake_path, cmake_env = tooling_module.cmake_environment(repo_root)
    if cmake_path is None:
        return _failure(command_text, host_platform, command.get("target"), "", ["cmake not found"])

    _reset_binary_dir(binary_dir)

    _emit_event(progress_callback, event_type="stage-start", completed=0, total=2, active_unit="configure")
    configure = run_process([cmake_path, "--preset", preset_name], cwd=repo_root, env=cmake_env)
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
    binary_dir = repo_root / command["binary_dir"]
    toolchain_file = repo_root / command["toolchain_file"]
    preset_target = command["preset_target"]
    cmake_path, cmake_env = tooling_module.cmake_environment(repo_root)
    if cmake_path is None:
        return _failure(command_text, host_platform, command.get("target"), "", ["cmake not found"])

    _reset_binary_dir(binary_dir)

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


def handle(
    command: dict,
    repo_root: Path,
    host_platform: str,
    command_text: str,
    progress_callback: Callable[[dict[str, Any]], None] | None = None,
) -> CommandResult:
    kind = command["kind"]
    if kind == "batch":
        return _build_batch(command, repo_root, host_platform, command_text, progress_callback=progress_callback)
    if kind == "native-contract":
        return _build_native_contract(command, repo_root, host_platform, command_text, progress_callback=progress_callback)
    if kind == "smoke-project":
        return _build_smoke_project(command, repo_root, host_platform, command_text, progress_callback=progress_callback)
    if kind == "preset":
        return _build_preset(command, repo_root, host_platform, command_text, progress_callback=progress_callback)
    if kind == "platform-gate":
        return _build_platform_gate(command, repo_root, host_platform, command_text, progress_callback=progress_callback)

    return _failure(command_text, host_platform, command.get("target"), "", [f"unsupported build kind: {kind}"])
