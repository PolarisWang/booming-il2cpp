# tool_helpers.py -- Shared tool management for verification pipeline
# Extracted from build.py and hotupdate_chunk.py to eliminate duplication.

from __future__ import annotations

import contextlib
import os
import re
import subprocess
import sys
import time
from pathlib import Path


@contextlib.contextmanager
def _tool_build_lock(tool_name: str):
    """Serialise builds of one tool across processes.

    Parallel chunk pipelines each call ``ensure_tool_built`` for the *same*
    shared project directory, so several ``dotnet build`` invocations race on
    one ``obj/Debug/net8.0/`` and all but the winner fail (MSB4018, or CS2012
    "being used by another process").  The loser's chunk dies before entry.exe
    is produced, which the nightly reports as an opaque ``unknown``.

    The lock is per-tool, not global: different tools still build in parallel.
    Blocking (not failing) is the point — the workers that arrive second are
    waiting on a build that is already producing what they need, so they take
    the lock and then hit the up-to-date fast path.

    POSIX uses ``fcntl.flock`` (advisory, released automatically if the holder
    dies); Windows uses ``msvcrt.locking``.  If neither is available the build
    proceeds unlocked rather than failing — the previous behaviour.
    """
    lock_dir = _repo_root() / "build" / ".tool-build-locks"
    try:
        lock_dir.mkdir(parents=True, exist_ok=True)
    except OSError:
        yield
        return
    lock_path = lock_dir / f"{tool_name}.lock"

    try:
        fd = os.open(lock_path, os.O_CREAT | os.O_RDWR, 0o644)
    except OSError:
        yield
        return

    acquired = False
    try:
        deadline = time.monotonic() + _TOOL_BUILD_LOCK_TIMEOUT_S
        if os.name == "nt":  # pragma: no cover - windows agent only
            import msvcrt
            while True:
                try:
                    msvcrt.locking(fd, msvcrt.LK_NBLCK, 1)
                    acquired = True
                    break
                except OSError:
                    if time.monotonic() > deadline:
                        break
                    time.sleep(0.5)
        else:
            import fcntl
            while True:
                try:
                    fcntl.flock(fd, fcntl.LOCK_EX | fcntl.LOCK_NB)
                    acquired = True
                    break
                except OSError:
                    if time.monotonic() > deadline:
                        break
                    time.sleep(0.5)
        if not acquired:
            print(f"      [tool_helpers] lock timeout for {tool_name}; building unlocked")
        yield
    finally:
        if acquired:
            try:
                if os.name == "nt":  # pragma: no cover
                    import msvcrt
                    msvcrt.locking(fd, msvcrt.LK_UNLCK, 1)
                else:
                    import fcntl
                    fcntl.flock(fd, fcntl.LOCK_UN)
            except OSError:
                pass
        os.close(fd)


# Generous: a cold build of TPG transitively builds Generator + 10 other
# projects.  Exceeding it falls back to an unlocked build rather than hanging
# the nightly forever.
_TOOL_BUILD_LOCK_TIMEOUT_S = 1800.0


def _worktree_root() -> Path | None:
    """The git worktree root that contains this module, or None for a normal
    checkout.

    A linked worktree's ``.git`` is a *file* (``gitdir: <main>/.git/worktrees/
    <name>``) rather than a directory, which is how it is told apart from the
    main checkout.  Without this, ``_repo_root()`` below anchors to whichever
    tree the *file* lives in (always the main checkout, since a worktree's
    ``tests/e2e/verification`` is that same file), so a worktree would build the
    tool from main's sources and silently run stale codegen.
    """
    cur = Path(__file__).resolve().parent
    while cur != cur.parent:
        git_path = cur / ".git"
        if git_path.is_file():
            try:
                first = git_path.read_text(encoding="utf-8", errors="replace").splitlines()[0]
            except (OSError, IndexError):
                return None
            if first.lower().startswith("gitdir:"):
                return cur
            return None
        if git_path.is_dir():
            return None  # normal checkout — behave as before
        cur = cur.parent
    return None


def _repo_root() -> Path:
    """Repository root (dir holding .git). Walks up so it is robust to where the
    _pipeline package is relocated (engine moved to tests/e2e/verification in L6;
    root is 4 dirs up from _pipeline/).

    A git worktree is honoured first so that a worktree builds the tool from its
    *own* sources; see _worktree_root()."""
    wt = _worktree_root()
    if wt is not None:
        return wt
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
    """All hand-written .cs files under a project, plus the project file itself
    (used to detect csproj edits, e.g. new ProjectReference includes).

    `obj/` and `bin/` are excluded: MSBuild writes generated sources
    (AssemblyInfo.cs, *.AssemblyAttributes.cs, ref/refint assemblies) into
    `obj/` *during* the build, so their mtimes land just after the output DLL's.
    Including them made the newest-source timestamp always exceed the DLL's,
    permanently reporting "up to date" and skipping rebuilds — a modified
    Generator then left a stale bundled Generator.dll in the tool's bin and the
    pipeline silently ran old codegen.
    """
    out: list[Path] = []
    for s in proj.parent.rglob("*.cs"):
        parts = s.parts
        if "obj" in parts or "bin" in parts:
            continue
        out.append(s)
    out.append(proj)
    return out


def ensure_tool_built(tool_name: str) -> bool:
    """Rebuild the tool DLL if the source (own OR transitively-referenced) has
    changed since the last build.

    Uses source timestamp comparison for incremental builds.  The transitive
    reference scan stops fast-path invalidation regressions where editing an
    upstream project (Generator) would otherwise leave a stale bundled copy in
    the tool's bin/Debug, causing the pipeline to run old generated code.

    Serialised per tool (see _tool_build_lock): the cache check races as well
    as the build, since a worker can stat the output DLL while another is
    mid-write and conclude it is up to date.
    """
    with _tool_build_lock(tool_name):
        return _ensure_tool_built_locked(tool_name)


def _ensure_tool_built_locked(tool_name: str) -> bool:
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
        # The BUNDLED (copied upstream DLL inside the tool's bin) must also be
        # newer than the upstream source.  `src_time` only compares sources to
        # the tool's OWN dll — if a rebuild ran with the same code but an
        # upstream project's DLL was copied in stale, dll.stat() is fresh while
        # the bundled copy is old, and the check returns True incorrectly.
        bundle_ok = True
        for up in _referenced_projects(proj):
            up_name = up.name.removesuffix(".csproj")
            up_dll = up / "bin" / "Debug" / "net8.0" / f"{up_name}.dll"
            bundled = proj.parent / "bin" / "Debug" / "net8.0" / f"{up_name}.dll"
            if up_dll.exists() and bundled.exists() and bundled.stat().st_mtime < up_dll.stat().st_mtime:
                bundle_ok = False
                break

        if src_time <= dll.stat().st_mtime and bundle_ok:
            return True
    # Release any lingering VBCSCompiler (Roslyn compiler server) handle on the
    # output DLL before rebuilding.  Without this, concurrent chunk pipelines on
    # Windows race: the first build spins VBCSCompiler up and it keeps the DLL
    # open, so a parallel build's csc fails with CS2012 "being used by another
    # process" and the whole chunk dies before entry.exe is produced.
    subprocess.run(
        ["dotnet", "build-server", "shutdown"],
        capture_output=True, text=True, timeout=30)

    # Rebuild.  A cold build of TPG transitively builds Generator + 10 other
    # projects and can far exceed a couple of minutes on a clean tree, so the
    # timeout is generous and a TimeoutExpired is reported rather than raised.
    try:
        result = subprocess.run(
            ["dotnet", "build", str(proj), "-nologo"],
            capture_output=True, text=True, timeout=900)
    except subprocess.TimeoutExpired:
        print(f"      [tool_helpers] TIMEOUT rebuilding {proj.name} (900s)")
        return False
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
