from __future__ import annotations

import os
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


def _vswhere_path() -> Path | None:
    candidate = Path(os.environ.get("ProgramFiles(x86)", r"C:\Program Files (x86)")) / "Microsoft Visual Studio" / "Installer" / "vswhere.exe"
    return candidate if candidate.is_file() else None


def _run_vswhere(arguments: list[str]) -> list[str]:
    executable = _vswhere_path()
    if executable is None:
        return []

    completed = subprocess.run(
        [str(executable), *arguments],
        capture_output=True,
        text=True,
        errors="replace",
        check=False,
    )
    if completed.returncode != 0:
        return []
    return [line.strip() for line in completed.stdout.splitlines() if line.strip()]


def find_visual_cpp_executable(which: Callable[[str], str | None] = shutil.which) -> str | None:
    discovered = which("cl")
    if discovered:
        return discovered

    matches = _run_vswhere(
        [
            "-latest",
            "-products",
            "*",
            "-requires",
            "Microsoft.VisualStudio.Component.VC.Tools.x86.x64",
            "-find",
            r"VC\Tools\MSVC\**\bin\Hostx64\x64\cl.exe",
        ]
    )
    return matches[0] if matches else None


def find_cmake_executable(repo_root: Path | None = None, which: Callable[[str], str | None] = shutil.which) -> str | None:
    discovered = which("cmake")
    if discovered:
        return discovered

    candidates: list[Path] = []
    if repo_root is not None:
        candidates.extend(
            [
                repo_root / "artifacts" / "toolchains" / "cmake" / "Lib" / "site-packages" / "cmake" / "data" / "bin" / "cmake.exe",
                repo_root / "artifacts" / "toolchains" / "cmake" / "bin" / "cmake.exe",
                repo_root / "artifacts" / "toolchains" / "cmake" / "Scripts" / "cmake.exe",
            ]
        )

    candidates.extend(
        [
            Path(r"C:\Program Files\CMake\bin\cmake.exe"),
            Path(r"C:\Program Files\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"),
            Path(r"C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"),
            Path(r"D:\Program Files\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"),
            Path(r"D:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"),
        ]
    )

    for candidate in candidates:
        if candidate.is_file():
            return str(candidate)

    matches = _run_vswhere(
        [
            "-latest",
            "-products",
            "*",
            "-find",
            r"Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe",
        ]
    )
    return matches[0] if matches else None


def cmake_environment(repo_root: Path | None = None, which: Callable[[str], str | None] = shutil.which) -> tuple[str | None, dict[str, str]]:
    cmake_path = find_cmake_executable(repo_root, which=which)
    if cmake_path is None:
        return None, {}

    cmake_dir = str(Path(cmake_path).resolve().parent)
    current_path = os.environ.get("PATH", "")
    if not current_path:
        return cmake_path, {"PATH": cmake_dir}
    return cmake_path, {"PATH": cmake_dir + os.pathsep + current_path}


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
