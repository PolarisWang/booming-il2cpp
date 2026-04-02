from __future__ import annotations

import shutil
import sys
from pathlib import Path

try:
    from ..result import CommandResult
    from .. import runtime as runtime_module
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from result import CommandResult
    import runtime as runtime_module


def _check_tool(name: str, executable: str, required: bool) -> dict:
    location = shutil.which(executable)
    return {
        "name": name,
        "status": "ok" if location else ("error" if required else "missing"),
        "detail": location or f"{executable} not found",
        "required": required,
    }


def handle(repo_root: Path, host_platform: str, command_text: str) -> CommandResult:
    probe = runtime_module.probe_runtime(repo_root)
    checks = [
        {
            "name": "cached-python-runtime",
            "status": "ok" if probe["isInstalled"] else "error",
            "detail": probe["pythonPath"],
            "required": True,
        },
        _check_tool("cmake", "cmake", True),
        _check_tool("dotnet", "dotnet", True),
    ]

    if host_platform == "windows":
        checks.append(_check_tool("visual-cpp-toolchain", "cl", False))
    elif host_platform == "macos":
        checks.append(_check_tool("xcodebuild", "xcodebuild", False))

    errors = [check["name"] for check in checks if check["status"] == "error"]
    if errors:
        return CommandResult.failure(
            command=command_text,
            host_platform=host_platform,
            target=None,
            errors=[f"doctor detected missing required tooling: {', '.join(errors)}"],
            payload={"checks": checks},
            text="doctor detected missing required tooling\n",
            checks=checks,
        )

    return CommandResult.success(
        command=command_text,
        host_platform=host_platform,
        target=None,
        payload={"checks": checks},
        text="doctor completed successfully\n",
        checks=checks,
    )
