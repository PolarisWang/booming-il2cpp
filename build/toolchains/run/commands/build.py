from __future__ import annotations

import sys
from pathlib import Path

try:
    from ..common import combine_process_output, run_process
    from ..result import CommandResult
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from common import combine_process_output, run_process
    from result import CommandResult


def _success(command_text: str, host_platform: str, target: str | None, output: str, artifacts: list[str]) -> CommandResult:
    return CommandResult.success(
        command=command_text,
        host_platform=host_platform,
        target=target,
        payload={"artifacts": artifacts},
        text=(output + "\n") if output else "",
    )


def _failure(command_text: str, host_platform: str, target: str | None, output: str, errors: list[str]) -> CommandResult:
    message = output if output else "\n".join(errors)
    if message and not message.endswith("\n"):
        message += "\n"
    return CommandResult.failure(
        command=command_text,
        host_platform=host_platform,
        target=target,
        errors=errors,
        text=message,
    )


def _build_native_contract(command: dict, repo_root: Path, host_platform: str, command_text: str) -> CommandResult:
    source_dir = repo_root / command["source_dir"]
    binary_dir = repo_root / command["binary_dir"]

    configure = run_process(["cmake", "-S", str(source_dir), "-B", str(binary_dir)], cwd=repo_root)
    configure_output = combine_process_output(configure)
    if configure.returncode != 0:
        return _failure(command_text, host_platform, command.get("target"), configure_output, ["cmake configure failed"])

    build = run_process(["cmake", "--build", str(binary_dir)], cwd=repo_root)
    build_output = combine_process_output(build)
    output = "\n".join(part for part in [configure_output, build_output] if part)
    if build.returncode != 0:
        return _failure(command_text, host_platform, command.get("target"), output, ["cmake build failed"])

    return _success(command_text, host_platform, command.get("target"), output, [str(binary_dir)])


def _build_smoke_project(command: dict, repo_root: Path, host_platform: str, command_text: str) -> CommandResult:
    project_path = repo_root / command["project_path"]
    completed = run_process(["dotnet", "build", str(project_path), "-c", "Release"], cwd=repo_root)
    output = combine_process_output(completed)
    if completed.returncode != 0:
        return _failure(command_text, host_platform, command.get("target"), output, ["dotnet build failed"])

    return _success(command_text, host_platform, command.get("target"), output, [str(repo_root / command["artifact_path"])])


def _build_preset(command: dict, repo_root: Path, host_platform: str, command_text: str) -> CommandResult:
    preset_name = command["preset"]
    binary_dir = repo_root / command["binary_dir"]

    configure = run_process(["cmake", "--preset", preset_name], cwd=repo_root)
    configure_output = combine_process_output(configure)
    if configure.returncode != 0:
        return _failure(command_text, host_platform, command.get("target"), configure_output, ["cmake preset configure failed"])

    build = run_process(["cmake", "--build", str(binary_dir)], cwd=repo_root)
    build_output = combine_process_output(build)
    output = "\n".join(part for part in [configure_output, build_output] if part)
    if build.returncode != 0:
        return _failure(command_text, host_platform, command.get("target"), output, ["cmake preset build failed"])

    return _success(command_text, host_platform, command.get("target"), output, [str(binary_dir)])


def _host_generator(host_platform: str) -> str:
    if host_platform == "windows":
        return "Visual Studio 17 2022"
    if host_platform == "macos":
        return "Xcode"
    return "Ninja"


def _build_platform_gate(command: dict, repo_root: Path, host_platform: str, command_text: str) -> CommandResult:
    binary_dir = repo_root / command["binary_dir"]
    toolchain_file = repo_root / command["toolchain_file"]
    preset_target = command["preset_target"]

    configure_args = [
        "cmake",
        "-S",
        str(repo_root),
        "-B",
        str(binary_dir),
        "-G",
        _host_generator(host_platform),
        f"-DROADMAP0_PRESET_TARGET={preset_target}",
        "-DROADMAP0_TOOLCHAIN_VALIDATE_ONLY=ON",
        f"-DCMAKE_TOOLCHAIN_FILE={toolchain_file}",
    ]

    configure = run_process(configure_args, cwd=repo_root)
    configure_output = combine_process_output(configure)
    if configure.returncode != 0:
        return _failure(command_text, host_platform, command.get("target"), configure_output, ["cmake routing configure failed"])

    build = run_process(["cmake", "--build", str(binary_dir)], cwd=repo_root)
    build_output = combine_process_output(build)
    output = "\n".join(part for part in [configure_output, build_output] if part)
    if build.returncode != 0:
        return _failure(command_text, host_platform, command.get("target"), output, ["cmake routing build failed"])

    return _success(command_text, host_platform, command.get("target"), output, [str(binary_dir)])


def handle(command: dict, repo_root: Path, host_platform: str, command_text: str) -> CommandResult:
    kind = command["kind"]
    if kind == "native-contract":
        return _build_native_contract(command, repo_root, host_platform, command_text)
    if kind == "smoke-project":
        return _build_smoke_project(command, repo_root, host_platform, command_text)
    if kind == "preset":
        return _build_preset(command, repo_root, host_platform, command_text)
    if kind == "platform-gate":
        return _build_platform_gate(command, repo_root, host_platform, command_text)

    return _failure(command_text, host_platform, command.get("target"), "", [f"unsupported build kind: {kind}"])
