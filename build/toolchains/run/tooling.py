from __future__ import annotations

import os
import re
import shutil
import subprocess
import sys
import tempfile
import uuid
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


_VISUAL_STUDIO_GENERATOR_PATTERN = re.compile(r"^\*?\s*(Visual Studio \d+ \d{4})\s+=")
_VISUAL_STUDIO_GENERATOR_MAJOR_PATTERN = re.compile(r"^Visual Studio (\d+) \d{4}$")


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

    binary_name = "cmake.exe" if os.name == "nt" else "cmake"
    candidates: list[Path] = []
    if repo_root is not None:
        candidates.extend(
            [
                repo_root / "artifacts" / "toolchains" / "cmake" / "cmake" / "data" / "bin" / binary_name,
                repo_root / "artifacts" / "toolchains" / "cmake" / "Lib" / "site-packages" / "cmake" / "data" / "bin" / "cmake.exe",
                repo_root / "artifacts" / "toolchains" / "cmake" / "bin" / binary_name,
                repo_root / "artifacts" / "toolchains" / "cmake" / "Scripts" / binary_name,
            ]
        )

    if os.name == "nt":
        candidates.extend(
            [
                Path(r"C:\Program Files\CMake\bin\cmake.exe"),
                Path(r"C:\Program Files (x86)\CMake\bin\cmake.exe"),
                Path(r"C:\Program Files\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"),
                Path(r"C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"),
                Path(r"D:\Program Files\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"),
                Path(r"D:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"),
            ]
        )

    for candidate in candidates:
        if candidate.is_file():
            return str(candidate)

    if os.name == "nt":
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

    return None


def cmake_environment(repo_root: Path | None = None, which: Callable[[str], str | None] = shutil.which) -> tuple[str | None, dict[str, str]]:
    cmake_path = find_cmake_executable(repo_root, which=which)
    if cmake_path is None:
        return None, {}

    cmake_dir = str(Path(cmake_path).resolve().parent)
    current_path = os.environ.get("PATH", "")
    if not current_path:
        return cmake_path, {"PATH": cmake_dir}
    return cmake_path, {"PATH": cmake_dir + os.pathsep + current_path}


def detect_visual_studio_generator(
    cmake_path: str,
    *,
    env: dict[str, str] | None = None,
    run: Callable[[list[str], Path | None, dict[str, str] | None], subprocess.CompletedProcess[str]] = run_process,
) -> str | None:
    completed = run([cmake_path, "--help"], cwd=None, env=env)
    if completed.returncode != 0:
        return None

    for line in completed.stdout.splitlines():
        match = _VISUAL_STUDIO_GENERATOR_PATTERN.match(line.strip())
        if match is not None:
            return match.group(1)
    return None


def _read_windows_file_version(path: Path) -> str | None:
    if os.name != "nt" or not path.is_file():
        return None

    escaped_path = str(path).replace("'", "''")
    completed = subprocess.run(
        [
            "powershell",
            "-NoLogo",
            "-NoProfile",
            "-NonInteractive",
            "-Command",
            f"(Get-Item -LiteralPath '{escaped_path}').VersionInfo.ProductVersion",
        ],
        capture_output=True,
        text=True,
        errors="replace",
        check=False,
    )
    if completed.returncode != 0:
        return None

    version = completed.stdout.strip()
    return version if re.fullmatch(r"\d+\.\d+\.\d+\.\d+", version) else None


def _candidate_visual_studio_instance_specs() -> list[tuple[Path, str]]:
    candidates: list[tuple[Path, str]] = []
    seen: set[str] = set()

    for base in [
        Path(r"C:\Program Files\Microsoft Visual Studio"),
        Path(r"C:\Program Files (x86)\Microsoft Visual Studio"),
    ]:
        if not base.is_dir():
            continue
        for major_dir in sorted(candidate for candidate in base.iterdir() if candidate.is_dir()):
            for edition_dir in sorted(candidate for candidate in major_dir.iterdir() if candidate.is_dir()):
                devenv_path = edition_dir / "Common7" / "IDE" / "devenv.exe"
                version = _read_windows_file_version(devenv_path)
                if version is None:
                    continue
                key = str(edition_dir).lower()
                if key in seen:
                    continue
                seen.add(key)
                candidates.append((edition_dir, version))

    return candidates


def detect_visual_studio_instance_spec(generator: str) -> str | None:
    if os.name != "nt":
        return None

    match = _VISUAL_STUDIO_GENERATOR_MAJOR_PATTERN.match(generator)
    required_major = match.group(1) if match is not None else None

    for root, version in _candidate_visual_studio_instance_specs():
        if required_major is not None and root.parent.name != required_major:
            continue
        return f"{root},version={version}"
    return None


def allocate_cmake_binary_dir(base_dir: Path, *, host_platform: str, generator: str | None = None) -> Path:
    if host_platform == "windows" and generator and generator.startswith("Visual Studio"):
        temp_root = Path(tempfile.gettempdir())
        scoped_dir = temp_root / f"booming-{base_dir.name}-{os.getpid()}-{uuid.uuid4().hex[:8]}"
        scoped_dir.mkdir(parents=True, exist_ok=False)
        return scoped_dir

    scoped_dir = base_dir.parent / f"{base_dir.name}-{os.getpid()}-{uuid.uuid4().hex[:8]}"
    scoped_dir.mkdir(parents=True, exist_ok=True)
    return scoped_dir


def allocate_dotnet_intermediate_dir(base_name: str, *, host_platform: str) -> Path | None:
    if host_platform != "windows":
        return None

    temp_root = Path(tempfile.gettempdir())
    del base_name
    scoped_dir = temp_root / f"bdn-{uuid.uuid4().hex[:8]}"
    scoped_dir.mkdir(parents=True, exist_ok=False)
    return scoped_dir


def path_text(repo_root: Path, path: Path) -> str:
    try:
        return path.relative_to(repo_root).as_posix()
    except ValueError:
        return path.as_posix()


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


def _manual_cmake_install_guidance(command_text: str, host_platform: str, repo_root: Path) -> str:
    cache_root = repo_root / "artifacts" / "toolchains" / "cmake"
    return (
        f"CMake is required for `{command_text}` on {host_platform}.\n"
        f"Preferred path: run `run prepare` to install a cached CMake into `{cache_root}`.\n"
        "Automatic bootstrap requires a Python interpreter with pip available as `python`, `python3`, or `py -3`.\n"
        "If that is unavailable, install CMake manually and retry.\n"
    )


def _candidate_python_commands(which: Callable[[str], str | None] = shutil.which) -> list[list[str]]:
    candidates: list[list[str]] = []
    discovered = which("python")
    if discovered:
        candidates.append([discovered])

    discovered = which("python3")
    if discovered:
        candidates.append([discovered])

    if os.name == "nt":
        discovered = which("py")
        if discovered:
            candidates.append([discovered, "-3"])

    if sys.executable:
        candidates.append([sys.executable])

    unique: list[list[str]] = []
    seen: set[tuple[str, ...]] = set()
    for candidate in candidates:
        key = tuple(candidate)
        if key in seen:
            continue
        seen.add(key)
        unique.append(candidate)
    return unique


def _find_python_with_pip(
    *,
    which: Callable[[str], str | None] = shutil.which,
    run: Callable[[list[str], Path | None], subprocess.CompletedProcess[str]] = run_process,
) -> list[str] | None:
    for candidate in _candidate_python_commands(which=which):
        probe = run([*candidate, "-m", "pip", "--version"], cwd=None)
        if probe.returncode == 0:
            return candidate
    return None


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


def ensure_cmake_available(
    command_text: str,
    host_platform: str,
    repo_root: Path,
    *,
    which: Callable[[str], str | None] = shutil.which,
    run: Callable[[list[str], Path | None], subprocess.CompletedProcess[str]] = run_process,
    find_cmake: Callable[[Path | None, Callable[[str], str | None]], str | None] = find_cmake_executable,
) -> ToolBootstrapResult:
    cmake_path = find_cmake(repo_root, which=which)
    if cmake_path:
        return ToolBootstrapResult(ready=True)

    manual_guidance = _manual_cmake_install_guidance(command_text, host_platform, repo_root)
    python_command = _find_python_with_pip(which=which, run=run)
    if python_command is None:
        return ToolBootstrapResult(
            ready=False,
            output=manual_guidance,
            errors=["cmake is not installed"],
        )

    cmake_root = repo_root / "artifacts" / "toolchains" / "cmake"
    completed = run(
        [
            *python_command,
            "-m",
            "pip",
            "install",
            "--disable-pip-version-check",
            "--upgrade",
            "--target",
            str(cmake_root),
            "cmake",
        ],
        cwd=repo_root,
    )
    install_output = combine_process_output(completed)
    if completed.returncode != 0:
        output = _normalize_output(install_output)
        output += manual_guidance
        return ToolBootstrapResult(
            ready=False,
            output=output,
            errors=["cmake bootstrap failed"],
        )

    cmake_path = find_cmake(repo_root, which=which)
    if cmake_path is None:
        output = _normalize_output(install_output)
        output += manual_guidance
        return ToolBootstrapResult(
            ready=False,
            output=output,
            errors=["cmake bootstrap failed"],
        )

    output = _normalize_output(install_output)
    output += f"cached CMake ready: {cmake_path}\n"
    return ToolBootstrapResult(ready=True, output=output)
