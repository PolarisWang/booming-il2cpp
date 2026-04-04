from __future__ import annotations

import os
import sys
from pathlib import Path

try:
    from ..result import CommandResult
    from .. import runtime as runtime_module
    from .. import tooling as tooling_module
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from result import CommandResult
    import runtime as runtime_module
    import tooling as tooling_module

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

    if required_issues:
        lines.append("")
        lines.append("Next actions:")
        for check in required_issues:
            if check["name"] == "cmake":
                lines.append("1. Install CMake, or make a local cmake.exe discoverable to the unified entrypoint.")
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
    elif host_platform == "macos":
        checks.append(_build_check("xcodebuild", tooling_module.shutil.which("xcodebuild"), False, missing_detail="xcodebuild not found"))

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
