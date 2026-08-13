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


def _referenced_projects(proj: Path, _seen: set | None = None) -> set[Path]:
    """Resolve the transitive set of .csproj files this project references via
    <ProjectReference>.  Used so a tool rebuild fires when an upstream project
    it depends on (e.g. TPG -> Driver -> Generator) changes, not just the tool's
    own source.  Without this, a modified Generator leaves a stale bundled
    Generator.dll inside the tool's bin, silently running old codegen."""
    if _seen is None:
        _seen = set()
    _seen.add(proj)
    try:
        text = proj.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return _seen
    import re as _re
    for ref in _re.finditer(r'<ProjectReference\s+Include="([^"]+)"', text):
        ref_path = (proj.parent / ref.group(1)).resolve()
        if ref_path.suffix == ".csproj" and ref_path not in _seen:
            _seen.add(ref_path)
            _referenced_projects(ref_path, _seen)
    return _seen


def _project_sources(proj: Path) -> list[Path]:
    """All .cs files under a project, plus the project file itself (used to detect
    csproj edits, e.g. new ProjectReference includes)."""
    return list(proj.parent.rglob("*.cs")) + [proj]


def ensure_tool_built(tool_name: str) -> bool:
    """Rebuild the tool DLL if the source (own OR transitively-referenced) has
    changed since the last build.

    Uses source timestamp comparison for incremental builds.  The transitive
    reference scan stops fast-path invalidation regressions where editing an
    upstream project (Generator) would otherwise leave a stale bundled copy in
    the tool's bin/Debug, causing the pipeline to run old generated code.
    """
    proj = _tool_dir(tool_name) / f"{tool_name}.csproj"
    dll = tool_dll(tool_name)
    if dll.exists() and proj.exists():
        # Newest source across the tool AND its transitive project references.
        src_time = 0.0
        for p in _referenced_projects(proj):
            for s in _project_sources(p):
                try:
                    m = s.stat().st_mtime
                    if m > src_time:
                        src_time = m
                except OSError:
                    pass
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

    # Delegate freshness to build_presets.py (the single authority over the SDK
    # fingerprint). It self-short-circuits via its own .source_hash check, so a
    # fresh SDK returns immediately ("up-to-date") while a stale SDK — one whose
    # lib exists but predates the current native source — is rebuilt.  The old
    # existence-only fast-path here returned a stale lib whenever the file was
    # present, silently masking source changes (e.g. P0-B batch-2 SIMD reducers
    # never linked because the prebuilt lib was several hours older than the
    # current numerics_vectors.cpp).  Delegating here fixes that whole class.
    script = build_presets_script(repo_root)
    result = subprocess.run(
        [sys.executable, str(script), "--preset", sdk_subdir],
        capture_output=True, text=True, timeout=1800,
    )
    if result.returncode != 0:
        print(f"[tool_helpers] SDK build FAILED for {sdk_subdir} (presets may not support {_platform.system()})")
        print(result.stderr[-600:])
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
