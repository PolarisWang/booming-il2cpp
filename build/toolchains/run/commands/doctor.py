from __future__ import annotations

import shutil
import sys
import os
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
    lines = [f"Doctor report ({host_platform})", ""]
    for check in checks:
        lines.append(f"{check['name']}: {check['status']} - {check['detail']}")

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
        _check_tool("cmake", "cmake", True),
        _check_tool("dotnet", "dotnet", True),
        _check_osc8_support(),
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
