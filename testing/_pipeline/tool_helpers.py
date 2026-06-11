# tool_helpers.py -- Shared tool management for verification pipeline
# Extracted from build.py and hotupdate_chunk.py to eliminate duplication.

from __future__ import annotations

import re
import subprocess
from pathlib import Path


def _testing_root() -> Path:
    """Root of the testing/ directory (parent of _pipeline/)."""
    return Path(__file__).resolve().parent.parent


def _tool_dir(tool_name: str) -> Path:
    """Get path to a tool's source directory."""
    return _testing_root() / "src" / "tools" / tool_name


def tool_dll(tool_name: str) -> Path:
    """Get path to a tool's compiled DLL."""
    return (_testing_root() / "src" / "tools" / tool_name
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


def detect_tfm(dll_path: Path) -> str:
    """Detect target framework moniker from the DLL's runtime directory path.

    E.g. ".../shared/Microsoft.NETCore.App/10.0.6/System.Private.CoreLib.dll"
    -> "net10.0".  Falls back to "net8.0".

    Also handles custom DLL paths like ".../dotnet-foundation/net10.0/runtime/...".
    """
    path = str(dll_path).replace("\\", "/")
    # Primary: standard runtime layout
    m = re.search(r"Microsoft\.NETCore\.App/(\d+)\.(\d+)\.", path)
    if m:
        return f"net{m.group(1)}.{m.group(2)}"
    # Fallback: custom layout like dotnet-foundation/netX.Y/runtime/
    m = re.search(r"/net(\d+)\.(\d+)/runtime/", path)
    if m:
        return f"net{m.group(1)}.{m.group(2)}"
    return "net8.0"
