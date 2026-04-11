from __future__ import annotations

import os
import platform
import re
import shutil
import subprocess
import sys
import tempfile
import urllib.request
import uuid
import zipfile
from dataclasses import dataclass, field
from pathlib import Path
from typing import Callable

try:
    from . import manifest as manifest_module
    from .common import _merge_environment, combine_process_output, run_process
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from core import manifest as manifest_module
    from core.common import _merge_environment, combine_process_output, run_process


@dataclass
class ToolBootstrapResult:
    ready: bool
    output: str = ""
    errors: list[str] = field(default_factory=list)


ANDROID_NDK_VERSION = "26.3.11579264"
ANDROID_PLATFORM_API = "36"
ANDROID_COMMAND_LINE_TOOLS_WINDOWS_URL = (
    "https://dl.google.com/android/repository/commandlinetools-win-14742923_latest.zip"
)
ANDROID_WINDOWS_JDK_URL = (
    "https://api.adoptium.net/v3/binary/latest/17/ga/windows/x64/jdk/hotspot/normal/eclipse"
)


def _normalize_android_emulator_host_machine(machine: str | None = None) -> str:
    normalized = (machine or platform.machine()).strip().lower()
    if normalized in {"amd64", "x86_64", "x64"}:
        return "x86_64"
    if normalized in {"arm64", "aarch64"}:
        return "arm64-v8a"
    return "x86_64"


def _android_avd_suffix_for_abi(abi: str) -> str:
    return abi.replace("-v8a", "")


ANDROID_EMULATOR_ABI = _normalize_android_emulator_host_machine()
ANDROID_SYSTEM_IMAGE_PACKAGE = f"system-images;android-{ANDROID_PLATFORM_API};google_apis;{ANDROID_EMULATOR_ABI}"
ANDROID_AVD_NAME = f"chaos-android-{ANDROID_PLATFORM_API}-{_android_avd_suffix_for_abi(ANDROID_EMULATOR_ABI)}"
ANDROID_REQUIRED_PACKAGES = [
    "platform-tools",
    "emulator",
    f"ndk;{ANDROID_NDK_VERSION}",
    f"platforms;android-{ANDROID_PLATFORM_API}",
    ANDROID_SYSTEM_IMAGE_PACKAGE,
]


_VISUAL_STUDIO_GENERATOR_PATTERN = re.compile(r"^\*?\s*(Visual Studio \d+ \d{4})\s+=")
_VISUAL_STUDIO_GENERATOR_MAJOR_PATTERN = re.compile(r"^Visual Studio (\d+) \d{4}$")
_MSVC_TOOLSET_VERSION_PATTERN = re.compile(r"[\\/]+MSVC[\\/]+(\d+\.\d+\.\d+)(?:[\\/]|$)", re.IGNORECASE)
_VISUAL_STUDIO_GENERATOR_YEAR_BY_MAJOR = {
    "15": "2017",
    "16": "2019",
    "17": "2022",
    "18": "2026",
}


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


def _candidate_visual_studio_install_paths() -> list[Path]:
    candidates: list[Path] = []
    seen: set[str] = set()

    install_paths = _run_vswhere(
        [
            "-latest",
            "-products",
            "*",
            "-requires",
            "Microsoft.VisualStudio.Component.VC.Tools.x86.x64",
            "-property",
            "installationPath",
        ]
    )
    for install_path in install_paths:
        candidate = Path(install_path)
        key = str(candidate).lower()
        if key in seen:
            continue
        seen.add(key)
        candidates.append(candidate)

    for root, _version in _candidate_visual_studio_instance_specs():
        key = str(root).lower()
        if key in seen:
            continue
        seen.add(key)
        candidates.append(root)

    return candidates


def find_visual_cpp_executable(which: Callable[[str], str | None] = shutil.which) -> str | None:
    if os.name == "nt":
        developer_env = windows_developer_environment()
        preferred = _visual_cpp_executable_from_developer_environment(developer_env)
        if preferred is not None:
            return preferred

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
    return _prefer_latest_msvc_match(matches)


def _visual_cpp_executable_from_developer_environment(environment: dict[str, str]) -> str | None:
    install_dir = environment.get("VCToolsInstallDir")
    if not install_dir:
        return None

    candidate = Path(install_dir) / "bin" / "Hostx64" / "x64" / "cl.exe"
    if candidate.is_file():
        return str(candidate)
    return None


def _msvc_toolset_version_key(path: str) -> tuple[int, ...]:
    match = _MSVC_TOOLSET_VERSION_PATTERN.search(path)
    if match is None:
        return ()
    return tuple(int(part) for part in match.group(1).split("."))


def _prefer_latest_msvc_match(matches: list[str]) -> str | None:
    if not matches:
        return None
    return max(matches, key=lambda entry: (_msvc_toolset_version_key(entry), entry.lower()))


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
    overrides = android_environment_overrides(repo_root) if repo_root is not None else {}

    path_parts = [cmake_dir]
    android_path = overrides.pop("PATH", "")
    if android_path:
        path_parts.extend(segment for segment in android_path.split(os.pathsep) if segment)
    system_path = os.environ.get("PATH", "")
    if system_path:
        path_parts.extend(segment for segment in system_path.split(os.pathsep) if segment)

    unique_parts: list[str] = []
    seen: set[str] = set()
    for segment in path_parts:
        normalized = segment.lower() if os.name == "nt" else segment
        if normalized in seen:
            continue
        seen.add(normalized)
        unique_parts.append(segment)

    if unique_parts:
        overrides["PATH"] = os.pathsep.join(unique_parts)
    return cmake_path, overrides


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
    required_roots = {required_major} if required_major is not None else set()
    if required_major is not None:
        mapped_year = _VISUAL_STUDIO_GENERATOR_YEAR_BY_MAJOR.get(required_major)
        if mapped_year is not None:
            required_roots.add(mapped_year)

    for root, version in _candidate_visual_studio_instance_specs():
        if required_roots and root.parent.name not in required_roots:
            continue
        return f"{root},version={version}"
    return None


def find_visual_studio_developer_command() -> Path | None:
    if os.name != "nt":
        return None

    matches = _run_vswhere(
        [
            "-latest",
            "-products",
            "*",
            "-requires",
            "Microsoft.VisualStudio.Component.VC.Tools.x86.x64",
            "-find",
            r"Common7\Tools\VsDevCmd.bat",
        ]
    )
    if matches:
        return Path(matches[0])

    for root in _candidate_visual_studio_install_paths():
        candidate = root / "Common7" / "Tools" / "VsDevCmd.bat"
        if candidate.is_file():
            return candidate

    return None


def android_toolchain_root(repo_root: Path) -> Path:
    return repo_root / "artifacts" / "toolchains" / "android"


def android_sdk_root(repo_root: Path) -> Path:
    return android_toolchain_root(repo_root) / "sdk"


def android_jdk_root(repo_root: Path) -> Path:
    return android_toolchain_root(repo_root) / "jdk"


def android_emulator_home(repo_root: Path) -> Path:
    return android_toolchain_root(repo_root) / ".android"


def android_avd_home(repo_root: Path) -> Path:
    return android_emulator_home(repo_root) / "avd"


def android_system_image_dir(repo_root: Path) -> Path:
    return android_sdk_root(repo_root) / "system-images" / f"android-{ANDROID_PLATFORM_API}" / "google_apis" / ANDROID_EMULATOR_ABI


def _android_java_executable_name() -> str:
    return "java.exe" if os.name == "nt" else "java"


def _android_sdkmanager_name() -> str:
    return "sdkmanager.bat" if os.name == "nt" else "sdkmanager"


def _android_avdmanager_name() -> str:
    return "avdmanager.bat" if os.name == "nt" else "avdmanager"


def _repo_cached_java_home(repo_root: Path) -> Path | None:
    cache_root = android_jdk_root(repo_root)
    candidate = cache_root / "bin" / _android_java_executable_name()
    if candidate.is_file():
        return cache_root

    if cache_root.is_dir():
        for child in sorted(candidate for candidate in cache_root.iterdir() if candidate.is_dir()):
            nested_candidate = child / "bin" / _android_java_executable_name()
            if nested_candidate.is_file():
                return child

    return None


def find_java_executable(repo_root: Path | None = None, which: Callable[[str], str | None] = shutil.which) -> str | None:
    java_home = os.environ.get("JAVA_HOME")
    if java_home:
        candidate = Path(java_home) / "bin" / _android_java_executable_name()
        if candidate.is_file():
            return str(candidate)

    if repo_root is not None:
        cached_java_home = _repo_cached_java_home(repo_root)
        if cached_java_home is not None:
            candidate = cached_java_home / "bin" / _android_java_executable_name()
            if candidate.is_file():
                return str(candidate)

    discovered = which("java")
    if discovered:
        return discovered

    return None


def _repo_cached_android_sdkmanager_path(repo_root: Path) -> Path:
    return android_sdk_root(repo_root) / "cmdline-tools" / "latest" / "bin" / _android_sdkmanager_name()


def _repo_cached_android_avdmanager_path(repo_root: Path) -> Path:
    return android_sdk_root(repo_root) / "cmdline-tools" / "latest" / "bin" / _android_avdmanager_name()


def _repo_cached_android_ndk_root(repo_root: Path) -> Path:
    return android_sdk_root(repo_root) / "ndk" / ANDROID_NDK_VERSION


def _repo_cached_android_adb_path(repo_root: Path) -> Path:
    executable = "adb.exe" if os.name == "nt" else "adb"
    return android_sdk_root(repo_root) / "platform-tools" / executable


def _repo_cached_android_emulator_path(repo_root: Path) -> Path:
    executable = "emulator.exe" if os.name == "nt" else "emulator"
    return android_sdk_root(repo_root) / "emulator" / executable


def _repo_cached_android_platform_dir(repo_root: Path) -> Path:
    return android_sdk_root(repo_root) / "platforms" / f"android-{ANDROID_PLATFORM_API}"


def android_sdkmanager_path(repo_root: Path) -> Path:
    return _repo_cached_android_sdkmanager_path(repo_root)


def android_avdmanager_path(repo_root: Path) -> Path:
    return _repo_cached_android_avdmanager_path(repo_root)


def android_adb_path(repo_root: Path) -> Path:
    return _repo_cached_android_adb_path(repo_root)


def android_emulator_path(repo_root: Path) -> Path:
    return _repo_cached_android_emulator_path(repo_root)


def android_environment_overrides(repo_root: Path) -> dict[str, str]:
    sdk_root = android_sdk_root(repo_root)
    ndk_root = _repo_cached_android_ndk_root(repo_root)
    java_home = _repo_cached_java_home(repo_root)
    emulator_home = android_emulator_home(repo_root)
    avd_home = android_avd_home(repo_root)

    path_parts: list[str] = []
    if java_home is not None:
        path_parts.append(str(java_home / "bin"))
        path_parts.append(str(java_home / "bin" / _android_java_executable_name()))
    sdkmanager_dir = _repo_cached_android_sdkmanager_path(repo_root).parent
    if sdkmanager_dir.is_dir():
        path_parts.append(str(sdkmanager_dir))
    adb_dir = _repo_cached_android_adb_path(repo_root).parent
    if adb_dir.is_dir():
        path_parts.append(str(adb_dir))
    emulator_dir = _repo_cached_android_emulator_path(repo_root).parent
    if emulator_dir.is_dir():
        path_parts.append(str(emulator_dir))
    current_path = os.environ.get("PATH", "")
    if current_path:
        path_parts.extend(segment for segment in current_path.split(os.pathsep) if segment)

    unique_path_parts: list[str] = []
    seen_path_parts: set[str] = set()
    for path_part in path_parts:
        normalized = path_part.lower() if os.name == "nt" else path_part
        if normalized in seen_path_parts:
            continue
        seen_path_parts.add(normalized)
        unique_path_parts.append(path_part)

    overrides: dict[str, str] = {}
    if sdk_root.is_dir():
        overrides["ANDROID_SDK_ROOT"] = str(sdk_root)
        overrides["ANDROID_HOME"] = str(sdk_root)
    if ndk_root.is_dir():
        overrides["ANDROID_NDK_ROOT"] = str(ndk_root)
    if java_home is not None:
        overrides["JAVA_HOME"] = str(java_home)
    overrides["ANDROID_USER_HOME"] = str(emulator_home)
    overrides["ANDROID_EMULATOR_HOME"] = str(emulator_home)
    overrides["ANDROID_AVD_HOME"] = str(avd_home)
    if unique_path_parts:
        overrides["PATH"] = os.pathsep.join(unique_path_parts)
    return overrides


def _download_file(url: str, destination: Path) -> None:
    destination.parent.mkdir(parents=True, exist_ok=True)
    request = urllib.request.Request(
        url,
        headers={
            "User-Agent": "Mozilla/5.0 (compatible; chaos-il2cpp-bootstrap/1.0)",
            "Accept": "*/*",
        },
    )
    with urllib.request.urlopen(request) as response, destination.open("wb") as stream:
        shutil.copyfileobj(response, stream)


def _extract_zip(archive_path: Path, destination: Path) -> None:
    if destination.exists():
        shutil.rmtree(destination, ignore_errors=True)
    destination.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(archive_path, "r") as archive:
        archive.extractall(destination)


def _replace_directory(source: Path, destination: Path) -> None:
    if destination.exists():
        shutil.rmtree(destination, ignore_errors=True)
    destination.parent.mkdir(parents=True, exist_ok=True)
    shutil.move(str(source), str(destination))


def _single_directory_or_self(root: Path) -> Path:
    entries = list(root.iterdir()) if root.is_dir() else []
    directories = [entry for entry in entries if entry.is_dir()]
    files = [entry for entry in entries if entry.is_file()]
    if len(directories) == 1 and not files:
        return directories[0]
    return root


def _run_android_tool(
    arguments: list[str],
    *,
    env: dict[str, str] | None = None,
    input_text: str | None = None,
    cwd: Path | None = None,
) -> subprocess.CompletedProcess[str]:
    completed = subprocess.run(
        arguments,
        cwd=str(cwd) if cwd else None,
        env=_merge_environment(env),
        input=input_text,
        capture_output=True,
        text=True,
        errors="replace",
        check=False,
    )
    return completed


def _bootstrap_windows_jdk(
    repo_root: Path,
    *,
    download_file: Callable[[str, Path], None],
    extract_zip: Callable[[Path, Path], None],
) -> Path:
    toolchain_root = android_toolchain_root(repo_root)
    downloads_root = toolchain_root / ".downloads"
    archive_path = downloads_root / "OpenJDK17U-jdk_x64_windows_hotspot.zip"
    extraction_root = toolchain_root / ".tmp-jdk"
    target_root = android_jdk_root(repo_root)

    download_file(ANDROID_WINDOWS_JDK_URL, archive_path)
    extract_zip(archive_path, extraction_root)

    extracted_root = _single_directory_or_self(extraction_root)
    _replace_directory(extracted_root, target_root)
    shutil.rmtree(extraction_root, ignore_errors=True)
    return target_root


def _bootstrap_windows_android_commandline_tools(
    repo_root: Path,
    *,
    download_file: Callable[[str, Path], None],
    extract_zip: Callable[[Path, Path], None],
) -> Path:
    sdk_root = android_sdk_root(repo_root)
    downloads_root = android_toolchain_root(repo_root) / ".downloads"
    archive_path = downloads_root / "commandlinetools-win-latest.zip"
    extraction_root = android_toolchain_root(repo_root) / ".tmp-cmdline-tools"
    latest_root = sdk_root / "cmdline-tools" / "latest"

    download_file(ANDROID_COMMAND_LINE_TOOLS_WINDOWS_URL, archive_path)
    extract_zip(archive_path, extraction_root)

    extracted_root = _single_directory_or_self(extraction_root)
    if (extracted_root / "cmdline-tools").is_dir():
        extracted_root = extracted_root / "cmdline-tools"

    _replace_directory(extracted_root, latest_root)
    shutil.rmtree(extraction_root, ignore_errors=True)
    return latest_root


def _android_repo_sdk_packages_ready(repo_root: Path) -> bool:
    java_home = _repo_cached_java_home(repo_root)
    return all(
        (
            java_home is not None and (java_home / "bin" / _android_java_executable_name()).is_file(),
            _repo_cached_android_sdkmanager_path(repo_root).is_file(),
            _repo_cached_android_adb_path(repo_root).is_file(),
            _repo_cached_android_emulator_path(repo_root).is_file(),
            _repo_cached_android_ndk_root(repo_root).is_dir(),
            _repo_cached_android_platform_dir(repo_root).is_dir(),
            android_system_image_dir(repo_root).is_dir(),
        )
    )


def _android_repo_avd_ready(repo_root: Path) -> bool:
    return (android_avd_home(repo_root) / f"{ANDROID_AVD_NAME}.avd").is_dir()


def ensure_android_host_tooling_available(
    command_text: str,
    host_platform: str,
    repo_root: Path,
    *,
    download_file: Callable[[str, Path], None] = _download_file,
    extract_zip: Callable[[Path, Path], None] = _extract_zip,
    run_android_tool: Callable[..., subprocess.CompletedProcess[str]] = _run_android_tool,
) -> ToolBootstrapResult:
    if host_platform != "windows":
        return ToolBootstrapResult(
            ready=False,
            output=(
                f"Android host bootstrap is currently only automated on windows for `{command_text}`.\n"
                "Use a Windows host to cache Android SDK / NDK / adb / emulator, or install them manually on this platform.\n"
            ),
            errors=["android host bootstrap is not supported on this platform"],
        )

    output_parts: list[str] = []
    if _repo_cached_java_home(repo_root) is None:
        try:
            _bootstrap_windows_jdk(repo_root, download_file=download_file, extract_zip=extract_zip)
            output_parts.append("Bootstrapped cached OpenJDK 17 for Android sdkmanager.")
        except Exception as error:
            return ToolBootstrapResult(
                ready=False,
                output="\n".join(part for part in [*output_parts, str(error)] if part) + "\n",
                errors=["android host bootstrap failed while installing Java"],
            )

    if not _repo_cached_android_sdkmanager_path(repo_root).is_file():
        try:
            _bootstrap_windows_android_commandline_tools(repo_root, download_file=download_file, extract_zip=extract_zip)
            output_parts.append("Bootstrapped Android command-line tools into the repo cache.")
        except Exception as error:
            return ToolBootstrapResult(
                ready=False,
                output="\n".join(part for part in [*output_parts, str(error)] if part) + "\n",
                errors=["android host bootstrap failed while installing Android command-line tools"],
            )

    if not _android_repo_sdk_packages_ready(repo_root):
        sdk_root = android_sdk_root(repo_root)
        env = android_environment_overrides(repo_root)
        sdkmanager_path = _repo_cached_android_sdkmanager_path(repo_root)

        licenses = run_android_tool(
            [str(sdkmanager_path), f"--sdk_root={sdk_root}", "--licenses"],
            env=env,
            input_text="y\n" * 32,
            cwd=repo_root,
        )
        licenses_output = combine_process_output(licenses).strip()
        if licenses_output:
            output_parts.append(licenses_output)
        if licenses.returncode != 0:
            return ToolBootstrapResult(
                ready=False,
                output="\n".join(part for part in output_parts if part) + "\n",
                errors=["android host bootstrap failed while accepting Android SDK licenses"],
            )

        install = run_android_tool(
            [str(sdkmanager_path), f"--sdk_root={sdk_root}", "--install", *ANDROID_REQUIRED_PACKAGES],
            env=env,
            cwd=repo_root,
        )
        install_output = combine_process_output(install).strip()
        if install_output:
            output_parts.append(install_output)
        if install.returncode != 0:
            return ToolBootstrapResult(
                ready=False,
                output="\n".join(part for part in output_parts if part) + "\n",
                errors=["android host bootstrap failed while installing Android SDK packages"],
            )

    if not _android_repo_avd_ready(repo_root):
        sdk_root = android_sdk_root(repo_root)
        env = android_environment_overrides(repo_root)
        emulator_home = android_emulator_home(repo_root)
        avd_home = android_avd_home(repo_root)
        avdmanager_path = _repo_cached_android_avdmanager_path(repo_root)

        emulator_home.mkdir(parents=True, exist_ok=True)
        avd_home.mkdir(parents=True, exist_ok=True)
        create_avd = run_android_tool(
            [
                str(avdmanager_path),
                "create",
                "avd",
                "--force",
                "--name",
                ANDROID_AVD_NAME,
                "--package",
                ANDROID_SYSTEM_IMAGE_PACKAGE,
            ],
            env=env,
            input_text="no\n",
            cwd=repo_root,
        )
        create_avd_output = combine_process_output(create_avd).strip()
        if create_avd_output:
            output_parts.append(create_avd_output)
        if create_avd.returncode != 0:
            return ToolBootstrapResult(
                ready=False,
                output="\n".join(part for part in output_parts if part) + "\n",
                errors=["android host bootstrap failed while creating repo-cached Android AVD"],
            )

    if not _android_repo_sdk_packages_ready(repo_root):
        return ToolBootstrapResult(
            ready=False,
            output="\n".join(part for part in output_parts if part) + "\n",
            errors=["android host bootstrap completed with missing SDK / NDK / adb / emulator / system image artifacts"],
        )

    if not _android_repo_avd_ready(repo_root):
        return ToolBootstrapResult(
            ready=False,
            output="\n".join(part for part in output_parts if part) + "\n",
            errors=["android host bootstrap completed with missing repo-cached Android AVD"],
        )

    env = android_environment_overrides(repo_root)
    output_parts.append(f"Android SDK root ready: {env['ANDROID_SDK_ROOT']}")
    output_parts.append(f"Android NDK root ready: {env['ANDROID_NDK_ROOT']}")
    output_parts.append(f"Android adb ready: {_repo_cached_android_adb_path(repo_root)}")
    output_parts.append(f"Android emulator ready: {_repo_cached_android_emulator_path(repo_root)}")
    output_parts.append(f"Android system image ready: {android_system_image_dir(repo_root)}")
    output_parts.append(f"Android AVD ready: {android_avd_home(repo_root) / f'{ANDROID_AVD_NAME}.avd'}")
    return ToolBootstrapResult(ready=True, output="\n".join(output_parts) + "\n")


def find_ninja_executable(which: Callable[[str], str | None] = shutil.which) -> str | None:
    discovered = which("ninja")
    if discovered:
        return discovered

    binary_name = "ninja.exe" if os.name == "nt" else "ninja"
    candidates: list[Path] = []
    if os.name == "nt":
        candidates.extend(
            [
                Path(r"C:\Program Files\Ninja\ninja.exe"),
                Path(r"C:\Program Files (x86)\Ninja\ninja.exe"),
            ]
        )
        for install_root in _candidate_visual_studio_install_paths():
            candidates.extend(
                [
                    install_root / "Common7" / "IDE" / "CommonExtensions" / "Microsoft" / "CMake" / "Ninja" / binary_name,
                    install_root / "Common7" / "IDE" / "CommonExtensions" / "Microsoft" / "CMake" / "CMake" / "bin" / binary_name,
                ]
            )

    for candidate in candidates:
        if candidate.is_file():
            return str(candidate)

    return None


def windows_developer_environment(*, arch: str = "x64", host_arch: str = "x64") -> dict[str, str]:
    if os.name != "nt":
        return {}

    developer_command = find_visual_studio_developer_command()
    if developer_command is None:
        return {}

    completed = subprocess.run(
        f'call "{developer_command}" -arch={arch} -host_arch={host_arch} >nul && set',
        capture_output=True,
        text=True,
        errors="replace",
        check=False,
        shell=True,
    )
    if completed.returncode != 0:
        return {}

    environment: dict[str, str] = {}
    existing_keys: set[str] = set()
    for line in completed.stdout.splitlines():
        if not line or "=" not in line or line.startswith("="):
            continue
        key, value = line.split("=", 1)
        if key:
            normalized = key.lower()
            if normalized in existing_keys:
                continue
            existing_keys.add(normalized)
            environment[key] = value
    return environment


def allocate_cmake_binary_dir(base_dir: Path, *, host_platform: str, generator: str | None = None) -> Path:
    if host_platform == "windows" and generator and (
        generator.startswith("Visual Studio") or generator.startswith("Ninja")
    ):
        temp_root = Path(tempfile.gettempdir())
        scoped_dir = temp_root / f"chaos-{base_dir.name}-{os.getpid()}-{uuid.uuid4().hex[:8]}"
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
