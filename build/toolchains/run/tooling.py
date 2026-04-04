from __future__ import annotations

import shutil
import subprocess
import sys
from dataclasses import dataclass, field
from pathlib import Path
from typing import Callable

try:
    from . import manifest as manifest_module
    from .common import combine_process_output, run_process
except ImportError:
    root = Path(__file__).resolve().parent
    sys.path.insert(0, str(root))
    import manifest as manifest_module
    from common import combine_process_output, run_process


@dataclass
class ToolBootstrapResult:
    ready: bool
    output: str = ""
    errors: list[str] = field(default_factory=list)


def _normalize_output(text: str) -> str:
    return text if not text or text.endswith("\n") else text + "\n"


def _manual_install_guidance(command_text: str, host_platform: str, interactive: bool) -> str:
    if host_platform == "macos":
        mode_hint = (
            "non-interactive session detected; install it manually and retry.\n"
            if not interactive
            else "install it manually and retry if you do not want auto-install.\n"
        )
        return (
            f"dotnet SDK is required for `{command_text}` on macOS.\n"
            f"Run `brew install --cask dotnet-sdk`, then retry.\n"
            f"{mode_hint}"
        )

    return (
        f"dotnet SDK is required for `{command_text}` on {host_platform}.\n"
        "Install the .NET SDK manually and retry.\n"
    )


def ensure_dotnet_available(
    command_text: str,
    host_platform: str,
    *,
    interactive: bool | None = None,
    prompt: Callable[[str], str] = input,
    which: Callable[[str], str | None] = shutil.which,
    run: Callable[[list[str], Path | None], subprocess.CompletedProcess[str]] = run_process,
) -> ToolBootstrapResult:
    if which("dotnet"):
        return ToolBootstrapResult(ready=True)

    interactive = manifest_module.is_interactive_session() if interactive is None else interactive
    manual_guidance = _manual_install_guidance(command_text, host_platform, interactive)

    if host_platform != "macos":
        return ToolBootstrapResult(
            ready=False,
            output=manual_guidance,
            errors=["dotnet SDK is not installed"],
        )

    if not interactive:
        return ToolBootstrapResult(
            ready=False,
            output=manual_guidance,
            errors=["dotnet SDK is not installed"],
        )

    if not which("brew"):
        return ToolBootstrapResult(
            ready=False,
            output=(
                f"dotnet SDK is required for `{command_text}` on macOS.\n"
                "Homebrew is required for automatic installation.\n"
                "Install Homebrew from https://brew.sh and retry.\n"
            ),
            errors=["dotnet SDK is not installed"],
        )

    response = prompt("dotnet SDK is required. Install now with Homebrew? [y/N] ").strip().lower()
    if response not in {"y", "yes"}:
        return ToolBootstrapResult(
            ready=False,
            output=manual_guidance,
            errors=["dotnet SDK is not installed"],
        )

    completed = run(["brew", "install", "--cask", "dotnet-sdk"], cwd=None)
    install_output = combine_process_output(completed)
    if completed.returncode != 0:
        output = _normalize_output(install_output)
        output += manual_guidance
        return ToolBootstrapResult(
            ready=False,
            output=output,
            errors=["dotnet SDK bootstrap failed"],
        )

    if not which("dotnet"):
        output = _normalize_output(install_output)
        output += manual_guidance
        return ToolBootstrapResult(
            ready=False,
            output=output,
            errors=["dotnet SDK bootstrap failed"],
        )

    output = _normalize_output(install_output)
    output += "dotnet SDK installed successfully.\n"
    return ToolBootstrapResult(ready=True, output=output)
