from __future__ import annotations

import sys
from pathlib import Path

try:
    from .. import tooling as tooling_module
    from ..common import combine_process_output, run_process
    from ..result import CommandResult
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    import tooling as tooling_module
    from common import combine_process_output, run_process
    from result import CommandResult


def handle(command: dict, repo_root: Path, host_platform: str, command_text: str) -> CommandResult:
    bootstrap = tooling_module.ensure_dotnet_available(command_text, host_platform)
    if not bootstrap.ready:
        message = bootstrap.output if bootstrap.output.endswith("\n") else bootstrap.output + "\n"
        return CommandResult.failure(
            command=command_text,
            host_platform=host_platform,
            target=command.get("target"),
            errors=bootstrap.errors,
            text=message,
        )

    host_profile = command["host_profile"]
    if host_profile == "windows":
        arguments = [
            "powershell",
            "-NoLogo",
            "-NoProfile",
            "-NonInteractive",
            "-File",
            str(repo_root / "build" / "scripts" / "verify-roadmap-0.ps1"),
            "-HostProfile",
            "windows",
        ]
    else:
        arguments = [
            sys.executable,
            str(repo_root / "build" / "scripts" / "verify-roadmap-0.py"),
            "--host-profile",
            "macos",
        ]

    completed = run_process(arguments, cwd=repo_root)
    output = "\n".join(part for part in [bootstrap.output.strip(), combine_process_output(completed)] if part)
    if completed.returncode != 0:
        message = output if output else "verify roadmap-0 failed"
        if message and not message.endswith("\n"):
            message += "\n"
        return CommandResult.failure(
            command=command_text,
            host_platform=host_platform,
            target=host_profile,
            errors=["verify roadmap-0 failed"],
            text=message,
        )

    return CommandResult.success(
        command=command_text,
        host_platform=host_platform,
        target=host_profile,
        payload={"artifacts": [str(repo_root / "artifacts" / "verify-roadmap-0" / host_profile)]},
        text=(output + "\n") if output else "",
    )
