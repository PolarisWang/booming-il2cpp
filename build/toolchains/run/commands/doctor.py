from __future__ import annotations

import os
import sys
from pathlib import Path

try:
    from ..core.result import CommandResult
    from .. import runtime as runtime_module
    from ..core import tooling as tooling_module
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from core.result import CommandResult
    import runtime as runtime_module
    from core import tooling as tooling_module

ANSI_BOLD_GREEN = "\x1b[1;32m"
ANSI_BOLD_YELLOW = "\x1b[1;33m"
ANSI_BOLD_CYAN = "\x1b[1;36m"
ANSI_RESET = "\x1b[0m"


def _build_check(name: str, location: str | None, required: bool, *, missing_detail: str, discovered_via: str | None = None) -> dict:
    status = "ok" if location else ("error" if required else "missing")
    detail = location or missing_detail
    if location and discovered_via:
        detail = f"{location} ({discovered_via})"
    return {
        "name": name,
        "status": status,
        "detail": detail,
        "required": required,
    }


def _status_label(status: str, required: bool) -> str:
    if status == "ok":
        return f"{ANSI_BOLD_GREEN}[ok]{ANSI_RESET}"
    if required:
        return f"{ANSI_BOLD_YELLOW}[fix]{ANSI_RESET}"
    return f"{ANSI_BOLD_CYAN}[note]{ANSI_RESET}"


def _check_cmake(repo_root: Path) -> dict:
    location = tooling_module.find_cmake_executable(repo_root)
    return _build_check(
        "cmake",
        location,
        True,
        missing_detail="cmake not found in PATH or standard install locations",
    )


def _check_dotnet() -> dict:
    location = tooling_module.shutil.which("dotnet")
    return _build_check("dotnet", location, True, missing_detail="dotnet not found")


def _existing_directory(path_text: str | None) -> str | None:
    if not path_text:
        return None
    candidate = Path(path_text).expanduser()
    if candidate.is_dir():
        return str(candidate)
    return None


def _android_executable_name(name: str) -> str:
    return f"{name}.exe" if os.name == "nt" else name


def _find_latest_ndk_under_sdk(sdk_root: str | None) -> str | None:
    if not sdk_root:
        return None

    ndk_root = Path(sdk_root) / "ndk"
    if ndk_root.is_dir():
        candidates = sorted((candidate for candidate in ndk_root.iterdir() if candidate.is_dir()), reverse=True)
        if candidates:
            return str(candidates[0])

    ndk_bundle = Path(sdk_root) / "ndk-bundle"
    if ndk_bundle.is_dir():
        return str(ndk_bundle)

    return None


def _repo_cached_android_sdk_root(repo_root: Path) -> str | None:
    sdk_root = tooling_module.android_sdk_root(repo_root)
    adb_path = sdk_root / "platform-tools" / _android_executable_name("adb")
    emulator_path = sdk_root / "emulator" / _android_executable_name("emulator")
    ndk_root = sdk_root / "ndk" / tooling_module.ANDROID_NDK_VERSION
    if sdk_root.is_dir() and (adb_path.is_file() or emulator_path.is_file() or ndk_root.is_dir()):
        return str(sdk_root)
    return None


def _locate_android_sdk_root(repo_root: Path) -> tuple[str | None, str | None]:
    candidates = [
        ("ANDROID_SDK_ROOT", os.environ.get("ANDROID_SDK_ROOT")),
        ("ANDROID_HOME", os.environ.get("ANDROID_HOME")),
        ("repo-cache", _repo_cached_android_sdk_root(repo_root)),
    ]

    local_app_data = os.environ.get("LOCALAPPDATA")
    home = os.environ.get("HOME")
    if local_app_data:
        candidates.append(("LOCALAPPDATA", str(Path(local_app_data) / "Android" / "Sdk")))
    if home:
        candidates.append(("HOME", str(Path(home) / "Library" / "Android" / "sdk")))
        candidates.append(("HOME", str(Path(home) / "Android" / "Sdk")))

    for discovered_via, candidate in candidates:
        location = _existing_directory(candidate)
        if location:
            return location, discovered_via
    return None, None


def _locate_android_ndk_root(repo_root: Path, sdk_root: str | None) -> tuple[str | None, str | None]:
    candidates = [
        ("ANDROID_NDK_ROOT", os.environ.get("ANDROID_NDK_ROOT")),
        ("ANDROID_NDK_HOME", os.environ.get("ANDROID_NDK_HOME")),
        ("ANDROID_SDK_ROOT", _find_latest_ndk_under_sdk(sdk_root)),
        (
            "repo-cache",
            str(tooling_module.android_sdk_root(repo_root) / "ndk" / tooling_module.ANDROID_NDK_VERSION),
        ),
    ]

    for discovered_via, candidate in candidates:
        location = _existing_directory(candidate)
        if location:
            return location, discovered_via
    return None, None


def _locate_android_sdk_tool(sdk_root: str | None, executable: str, relative_path: tuple[str, ...]) -> tuple[str | None, str | None]:
    discovered = tooling_module.shutil.which(executable)
    if discovered:
        return discovered, "PATH"

    if sdk_root:
        candidate = Path(sdk_root).joinpath(*relative_path)
        if candidate.is_file():
            return str(candidate), "ANDROID_SDK_ROOT"

    return None, None


def _locate_android_system_image_dir(repo_root: Path, sdk_root: str | None) -> tuple[str | None, str | None]:
    candidates = [
        (
            "ANDROID_SDK_ROOT",
            str(
                Path(sdk_root)
                / "system-images"
                / f"android-{tooling_module.ANDROID_PLATFORM_API}"
                / "google_apis"
                / tooling_module.ANDROID_EMULATOR_ABI
            )
            if sdk_root
            else None,
        ),
        ("repo-cache", str(tooling_module.android_system_image_dir(repo_root))),
    ]

    for discovered_via, candidate in candidates:
        location = _existing_directory(candidate)
        if location:
            return location, discovered_via
    return None, None


def _locate_android_avd_dir(repo_root: Path) -> tuple[str | None, str | None]:
    avd_name = f"{tooling_module.ANDROID_AVD_NAME}.avd"
    candidates = [
        ("ANDROID_AVD_HOME", str(Path(os.environ["ANDROID_AVD_HOME"]) / avd_name) if os.environ.get("ANDROID_AVD_HOME") else None),
        ("ANDROID_USER_HOME", str(Path(os.environ["ANDROID_USER_HOME"]) / "avd" / avd_name) if os.environ.get("ANDROID_USER_HOME") else None),
        ("repo-cache", str(tooling_module.android_avd_home(repo_root) / avd_name)),
    ]

    for discovered_via, candidate in candidates:
        location = _existing_directory(candidate)
        if location:
            return location, discovered_via
    return None, None


def _check_android_runtime_tooling(repo_root: Path) -> list[dict]:
    sdk_root, sdk_via = _locate_android_sdk_root(repo_root)
    ndk_root, ndk_via = _locate_android_ndk_root(repo_root, sdk_root)
    adb_location, adb_via = _locate_android_sdk_tool(
        sdk_root,
        "adb",
        ("platform-tools", _android_executable_name("adb")),
    )
    emulator_location, emulator_via = _locate_android_sdk_tool(
        sdk_root,
        "emulator",
        ("emulator", _android_executable_name("emulator")),
    )
    system_image_location, system_image_via = _locate_android_system_image_dir(repo_root, sdk_root)
    avd_location, avd_via = _locate_android_avd_dir(repo_root)

    return [
        _build_check(
            "android-sdk-root",
            sdk_root,
            False,
            missing_detail="ANDROID_SDK_ROOT / ANDROID_HOME not found",
            discovered_via=sdk_via,
        ),
        _build_check(
            "android-ndk-root",
            ndk_root,
            False,
            missing_detail="ANDROID_NDK_ROOT not found and no side-by-side NDK discovered under Android SDK",
            discovered_via=ndk_via,
        ),
        _build_check(
            "android-adb",
            adb_location,
            False,
            missing_detail="adb not found in PATH or Android SDK platform-tools",
            discovered_via=adb_via,
        ),
        _build_check(
            "android-emulator",
            emulator_location,
            False,
            missing_detail="emulator not found in PATH or Android SDK emulator directory",
            discovered_via=emulator_via,
        ),
        _build_check(
            "android-system-image",
            system_image_location,
            False,
            missing_detail="Android arm64 system image not found under Android SDK or repo cache",
            discovered_via=system_image_via,
        ),
        _build_check(
            "android-avd",
            avd_location,
            False,
            missing_detail="repo-cached Android AVD not found",
            discovered_via=avd_via,
        ),
    ]


def _check_ios_runtime_host(host_platform: str) -> dict:
    location = tooling_module.shutil.which("xcodebuild")
    return _build_check(
        "ios-runtime-host",
        location,
        False,
        missing_detail="xcodebuild not found; iOS runtime evidence requires Xcode plus Simulator or a signed device",
        discovered_via="PATH" if location else None,
    )


def _check_visual_cpp_toolchain() -> dict:
    location = tooling_module.find_visual_cpp_executable()
    discovered_via = None
    if location and not tooling_module.shutil.which("cl"):
        discovered_via = "discovered via Visual Studio"
    return _build_check(
        "visual-cpp-toolchain",
        location,
        False,
        missing_detail="cl not found",
        discovered_via=discovered_via,
    )


def _check_osc8_support() -> dict:
    term_program = (os.environ.get("TERM_PROGRAM") or "").strip().lower()
    term = (os.environ.get("TERM") or "").strip().lower()
    display_name = os.environ.get("TERM_PROGRAM") or os.environ.get("TERM") or "unknown-terminal"

    if os.environ.get("WT_SESSION"):
        return {
            "name": "osc8-hyperlinks",
            "status": "ok",
            "detail": "supported by Windows Terminal session",
            "required": False,
            "terminal": "Windows Terminal",
        }

    supported_programs = {
        "iterm.app": "supported by iTerm2",
        "wezterm": "supported by WezTerm",
        "vscode": "supported by VS Code integrated terminal",
        "warpterminal": "supported by Warp",
        "ghostty": "supported by Ghostty",
    }
    if term_program in supported_programs:
        return {
            "name": "osc8-hyperlinks",
            "status": "ok",
            "detail": supported_programs[term_program],
            "required": False,
            "terminal": display_name,
        }

    if "kitty" in term:
        return {
            "name": "osc8-hyperlinks",
            "status": "ok",
            "detail": "supported by kitty-compatible terminal",
            "required": False,
            "terminal": display_name,
        }

    return {
        "name": "osc8-hyperlinks",
        "status": "missing",
        "detail": "support unknown in this terminal; OSC 8 is a terminal capability and is not installable",
        "required": False,
        "terminal": display_name,
    }


def _render_doctor_text(checks: list[dict], host_platform: str) -> str:
    required_checks = [check for check in checks if check.get("required")]
    optional_checks = [check for check in checks if not check.get("required")]
    required_issues = [check for check in required_checks if check["status"] != "ok"]
    optional_issues = [check for check in optional_checks if check["status"] != "ok"]

    lines = [
        f"Doctor report ({host_platform})",
        "",
        f"Summary: required issues={len(required_issues)}, optional notes={len(optional_issues)}",
        "",
        "Required checks:",
    ]
    for check in required_checks:
        label = _status_label(check["status"], required=True)
        lines.append(f"{label} {check['name']}: {check['detail']}")

    if optional_checks:
        lines.append("")
        lines.append("Optional checks:")
        for check in optional_checks:
            label = _status_label(check["status"], required=False)
            lines.append(f"{label} {check['name']}: {check['detail']}")

    android_checks = [check for check in checks if check["name"].startswith("android-")]
    ios_runtime_check = next((check for check in checks if check["name"] == "ios-runtime-host"), None)
    if android_checks or ios_runtime_check is not None:
        lines.append("")
        lines.append("Mobile runtime host:")
        android_blockers = [check["name"] for check in android_checks if check["status"] != "ok"]
        if android_checks:
            if android_blockers:
                lines.append(f"Android blockers: {', '.join(android_blockers)}")
            else:
                lines.append("Android runtime host tooling looks discoverable from the current host.")
        if ios_runtime_check is not None:
            if ios_runtime_check["status"] == "ok":
                lines.append("iOS host note: xcodebuild is available on this macOS host.")
            else:
                lines.append(f"iOS blockers: {ios_runtime_check['detail']}")

    if required_issues:
        lines.append("")
        lines.append("Next actions:")
        for check in required_issues:
            if check["name"] == "cmake":
                lines.append("1. Run `run prepare` to install a cached CMake, or make a local cmake executable discoverable to the unified entrypoint.")
            else:
                lines.append(f"1. Resolve the missing required tool: {check['name']}.")

    osc8_check = next((check for check in checks if check["name"] == "osc8-hyperlinks"), None)
    if osc8_check is not None:
        lines.append("")
        lines.append("OSC 8 hyperlinks:")
        if osc8_check["status"] == "ok":
            lines.append("Current terminal looks compatible with clickable output links.")
        else:
            lines.append("OSC 8 cannot be installed by run doctor; switch to a compatible terminal to enable clickable links.")
            lines.append(f"Current terminal: {osc8_check.get('terminal', 'unknown-terminal')}")
            lines.append("Recommended terminals: iTerm2, WezTerm, VS Code Terminal, Windows Terminal, Warp, Ghostty")

    return "\n".join(lines) + "\n"


def handle(repo_root: Path, host_platform: str, command_text: str) -> CommandResult:
    probe = runtime_module.probe_runtime(repo_root)
    checks = [
        {
            "name": "cached-python-runtime",
            "status": "ok" if probe["isInstalled"] else "error",
            "detail": probe["pythonPath"],
            "required": True,
        },
        _check_cmake(repo_root),
        _check_dotnet(),
        _check_osc8_support(),
    ]

    if host_platform == "windows":
        checks.append(_check_visual_cpp_toolchain())

    checks.extend(_check_android_runtime_tooling(repo_root))
    if host_platform == "macos":
        checks.append(_check_ios_runtime_host(host_platform))

    errors = [check["name"] for check in checks if check["status"] == "error"]
    if errors:
        return CommandResult.failure(
            command=command_text,
            host_platform=host_platform,
            target=None,
            errors=[f"doctor detected missing required tooling: {', '.join(errors)}"],
            payload={"checks": checks},
            text=_render_doctor_text(checks, host_platform),
            checks=checks,
        )

    return CommandResult.success(
        command=command_text,
        host_platform=host_platform,
        target=None,
        payload={"checks": checks},
        text=_render_doctor_text(checks, host_platform),
        checks=checks,
    )
