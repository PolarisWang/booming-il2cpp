# tool_helpers.py -- Shared tool management for verification pipeline
# Extracted from build.py and hotupdate_chunk.py to eliminate duplication.

from __future__ import annotations

import re
import subprocess
import sys
from pathlib import Path


def _repo_root() -> Path:
    """Repository root (dir holding .git). Walks up so it is robust to where the
    _pipeline package is relocated (engine moved to tests/e2e/verification in L6;
    root is 4 dirs up from _pipeline/)."""
    cur = Path(__file__).resolve().parent
    while cur != cur.parent:
        if (cur / ".git").exists() or (cur / "tests" / "runner").is_dir():
            return cur
        cur = cur.parent
    return cur


def _tool_dir(tool_name: str) -> Path:
    """Get path to a tool's source directory."""
    return _repo_root() / "src" / "tools" / tool_name


def tool_dll(tool_name: str) -> Path:
    """Get path to a tool's compiled DLL."""
    return (_repo_root() / "src" / "tools" / tool_name
            / "bin" / "Debug" / "net8.0" / f"{tool_name}.dll")


def ensure_tool_built(tool_name: str) -> bool:
    """Rebuild the tool DLL if the source has changed since last build.

    Uses source timestamp comparison for incremental builds.
    """
    proj = _tool_dir(tool_name) / f"{tool_name}.csproj"
    dll = tool_dll(tool_name)
    if dll.exists() and proj.exists():
        # Check if any source file is newer than the DLL
        src_files = list(proj.parent.rglob("*.cs"))
        if src_files:
            src_time = max(p.stat().st_mtime for p in src_files if p.is_file())
        else:
            src_time = proj.stat().st_mtime
        if src_time <= dll.stat().st_mtime:
            return True
    # Rebuild
    result = subprocess.run(
        ["dotnet", "build", str(proj), "-nologo"],
        capture_output=True, text=True, timeout=120)
    if result.returncode != 0:
        for line in (result.stderr.splitlines() + result.stdout.splitlines())[-5:]:
            print(f"      {line}")
        return False
    return True


# ── SDK root — single authoritative definition ──────────────────────────────
# The native SDK output location is owned here so every consumer (ensure_sdk,
# build_presets.py, and any caller) resolves the same canonical path. Before L11
# this was duplicated as `testing/foundation-dll/sdk` in build_presets.py and
# `tests/e2e/translation/sdk` here; the split drifted when the engine was
# relocated. Keeping one authoritative resolver prevents that recurrence.

def sdk_root(repo_root: Path | None = None) -> Path:
    """Canonical native SDK staging root (contains one subdir per preset)."""
    if repo_root is None:
        repo_root = _repo_root()
    return repo_root / "tests" / "e2e" / "translation" / "sdk"


def build_presets_script(repo_root: Path | None = None) -> Path:
    """Canonical path to the SDK build script (owned alongside the SDK root)."""
    if repo_root is None:
        repo_root = _repo_root()
    return repo_root / "tests" / "e2e" / "translation" / "artifacts" / "build_presets.py"


def ensure_sdk(repo_root: Path | None = None) -> Path:
    """Ensure the native SDK is built, building it if needed.

    Platform detection: on Linux uses linux-x64-profile (static .a),
    on Windows uses windows-x64-reference (static .lib).
    Falls back gracefully when the platform preset is not available.
    """
    import platform as _platform

    if repo_root is None:
        repo_root = _repo_root()

    is_linux = _platform.system() == "Linux"
    sdk_subdir = "linux-x64-profile" if is_linux else "windows-x64-reference"
    lib_ext = ".a" if is_linux else ".lib"

    sdk_dir = sdk_root(repo_root) / sdk_subdir
    lib_name = f"libchaos_runtime_core{lib_ext}" if is_linux else f"chaos_runtime_core{lib_ext}"
    sdk_lib = sdk_dir / "lib" / lib_name
    if sdk_lib.exists():
        return sdk_dir

    print(f"[tool_helpers] SDK ({sdk_subdir}) not found, building presets...")
    script = build_presets_script(repo_root)
    result = subprocess.run(
        [sys.executable, str(script), "--preset", sdk_subdir],
        capture_output=True, text=True, timeout=1200,
    )
    if result.returncode != 0:
        print(f"[tool_helpers] SDK build FAILED for {sdk_subdir} (presets may not support {_platform.system()})")
        print(result.stderr[-500:])
        raise RuntimeError(f"SDK build failed: {result.stderr[-200:]}")

    if not sdk_lib.exists():
        raise RuntimeError(f"SDK built but {sdk_lib} not found")

    print(f"[tool_helpers] SDK ready: {sdk_dir}")
    return sdk_dir


def detect_tfm(dll_path: Path) -> str:
    """Detect target framework moniker from the DLL's runtime directory path.

    E.g. ".../shared/Microsoft.NETCore.App/10.0.6/System.Private.CoreLib.dll"
    -> "net10.0".  Falls back to "net8.0".

    Also handles custom DLL paths like ".../dotnet-foundation/net10.0/runtime/...".
    """
    path = str(dll_path).replace("\\", "/")
    # Primary: standard runtime layout
    m = re.search(r"Microsoft\.NETCore\.App(?:\.Ref)?/(\d+)\.(\d+)\.", path)
    if m:
        return f"net{m.group(1)}.{m.group(2)}"
    # Fallback: custom layout like dotnet-foundation/netX.Y/runtime/
    m = re.search(r"/net(\d+)\.(\d+)/runtime/", path)
    if m:
        return f"net{m.group(1)}.{m.group(2)}"
    return "net10.0"
