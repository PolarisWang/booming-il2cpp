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


def _ensure_smoke_build(command: dict, repo_root: Path) -> tuple[bool, str]:
    project_path = repo_root / command["project_path"]
    completed = run_process(["dotnet", "build", str(project_path), "-c", "Release"], cwd=repo_root)
    return completed.returncode == 0, combine_process_output(completed)


def _run_smoke_project(command: dict, repo_root: Path, host_platform: str, command_text: str) -> CommandResult:
    build_ok, build_output = _ensure_smoke_build(command, repo_root)
    if not build_ok:
        return _failure(command_text, host_platform, command.get("target"), build_output, ["smoke build prerequisite failed"])

    dll_path = repo_root / command["dll_path"]
    completed = run_process(["dotnet", str(dll_path)], cwd=repo_root)
    runtime_output = combine_process_output(completed)
    output = "\n".join(part for part in [build_output, runtime_output] if part)
    if completed.returncode != 0:
        return _failure(command_text, host_platform, command.get("target"), output, ["managed smoke execution failed"])

    for expected in command["expected_patterns"]:
        if expected not in runtime_output:
            return _failure(command_text, host_platform, command.get("target"), output, [f"missing expected smoke output: {expected}"])

    return _success(command_text, host_platform, command.get("target"), output, [str(dll_path)])


def _run_trace_compare(command: dict, repo_root: Path, host_platform: str, command_text: str) -> CommandResult:
    host_embedding_project = repo_root / "tests" / "smoke" / "input" / "HostEmbeddingLite" / "HostEmbeddingLite.csproj"
    build = run_process(["dotnet", "build", str(host_embedding_project), "-c", "Release"], cwd=repo_root)
    build_output = combine_process_output(build)
    if build.returncode != 0:
        return _failure(command_text, host_platform, command.get("target"), build_output, ["HostEmbeddingLite build failed"])

    host_embedding_dll = repo_root / "artifacts" / "smoke" / "bin" / "HostEmbeddingLite" / "Release" / "net8.0" / "HostEmbeddingLite.dll"
    trace_output = repo_root / command["actual_trace_path"]
    trace_output.parent.mkdir(parents=True, exist_ok=True)

    export = run_process(
        [
            "dotnet",
            str(host_embedding_dll),
            "--trace-platform",
            command["trace_platform"],
            "--trace-output",
            str(trace_output),
        ],
        cwd=repo_root,
    )
    export_output = combine_process_output(export)
    if export.returncode != 0:
        return _failure(command_text, host_platform, command.get("target"), "\n".join(part for part in [build_output, export_output] if part), ["trace export failed"])

    if host_platform == "windows":
        compare_args = [
            "powershell",
            "-NoLogo",
            "-NoProfile",
            "-NonInteractive",
            "-File",
            str(repo_root / "tests" / "contracts" / "trace" / "compare-warmup-trace.ps1"),
            "-ExpectedPath",
            str(repo_root / command["expected_trace_path"]),
            "-ActualPath",
            str(trace_output),
        ]
    else:
        compare_args = [
            "sh",
            str(repo_root / "tests" / "contracts" / "trace" / "compare-warmup-trace.sh"),
            str(repo_root / command["expected_trace_path"]),
            str(trace_output),
        ]

    compare = run_process(compare_args, cwd=repo_root)
    compare_output = combine_process_output(compare)
    output = "\n".join(part for part in [build_output, export_output, compare_output] if part)
    if compare.returncode != 0:
        return _failure(command_text, host_platform, command.get("target"), output, ["trace compare failed"])

    return _success(command_text, host_platform, command.get("target"), output, [str(trace_output)])


def handle(command: dict, repo_root: Path, host_platform: str, command_text: str) -> CommandResult:
    kind = command["kind"]
    if kind == "smoke-run":
        return _run_smoke_project(command, repo_root, host_platform, command_text)
    if kind == "trace-compare":
        return _run_trace_compare(command, repo_root, host_platform, command_text)

    return _failure(command_text, host_platform, command.get("target"), "", [f"unsupported test kind: {kind}"])
