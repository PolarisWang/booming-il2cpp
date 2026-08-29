#!/usr/bin/env python3
"""Git index.lock diagnostics + safe stale-lock cleanup (Windows-first, cross-platform).

Answers "who might be holding .git/index.lock right now" and, in --clean-stale
mode, removes ONLY a lock proven to be orphaned (no live writer process and
stale). It never deletes a lock a live writer could be holding — that would
corrupt the index, the exact failure mode we're guarding against.

Exit/status contract (used by chaos_hygiene.py):
  - clean repo            -> exit 0, print "[PASS]"
  - no lock / advisory    -> exit 0, print "[PASS]" (or "[WARN]" if a lock exists but is fresh/owned)
  - --clean-stale removed -> exit 0, print "[OK] removed"
  - hard error            -> exit 1, print "[FAIL] ..."

Usage:
  python check_git_lock.py                 # read-only diagnostics
  python check_git_lock.py --clean-stale   # also delete an orphaned stale lock
  python check_git_lock.py --stale-after 120  # tune staleness (default 60s)
"""

import subprocess
import sys
import time
from pathlib import Path

# Subcommands that mutate the index (write-lock holders).
WRITE_SUBCOMMANDS = (
    "add", "commit", "reset", "merge", "rebase", "cherry-pick",
    "update-index", "restore", "mv", "rm", "gc", "maintenance", "checkout", "switch",
)
# Subcommands that are read-only and therefore are NOT lock-holders.
READ_SUBCOMMANDS = ("status", "ls-files", "diff", "log", "rev-parse", "show", "ls-tree")


def repo_root() -> Path:
    p = Path(__file__).resolve()
    for _ in range(6):
        p = p.parent
        git = p / ".git"
        # .git is a directory in a normal repo, but only a FILE (containing
        # "gitdir: <path>") in a git worktree. Accept both.
        if git.is_dir() or git.is_file():
            return p
    raise RuntimeError("not inside a git repo tree")


def _git_procs_posix():
    """Return (procs, enumerate_ok) for Linux/macOS using ``ps aux``.

    Falls back to ``psutil`` if available (more reliable), then ``ps aux``.
    """
    procs = []
    enumerate_ok = False
    # Try psutil first (more reliable, avoids parsing races).
    try:
        import psutil  # noqa: F811
        enumerate_ok = True
        for proc in psutil.process_iter(["pid", "name", "cmdline"]):
            try:
                info = proc.info
                name = (info.get("name") or "").lower()
                cmdline = info.get("cmdline")
                if "git" not in name and "git" not in str(cmdline).lower():
                    continue
                cmd = " ".join(cmdline) if cmdline else name
                procs.append({"pid": str(info["pid"]), "cmdline": cmd})
            except (psutil.NoSuchProcess, psutil.AccessDenied):
                continue
        return procs, True
    except ImportError:
        pass
    # Fallback: ps aux filtered for git.
    try:
        out = subprocess.run(
            ["ps", "aux"],
            capture_output=True, text=True, timeout=10, errors="replace",
        ).stdout
        enumerate_ok = bool(out.strip())
    except (subprocess.SubprocessError, OSError, FileNotFoundError):
        return procs, False
    if not enumerate_ok:
        return procs, False
    for line in out.splitlines():
        # ps aux output: USER PID %CPU %MEM VSZ RSS TTY STAT START TIME COMMAND
        parts = line.split(None, 10)
        if len(parts) < 11:
            continue
        pid = parts[1]
        cmd = parts[10]
        if "git" in cmd.lower():
            procs.append({"pid": pid, "cmdline": cmd})
    return procs, True


def _git_procs_auto():
    """Auto-detect platform and dispatch to the right implementation."""
    import platform
    system = platform.system().lower()
    if system == "windows":
        return _git_procs_windows()
    else:
        return _git_procs_posix()


def _git_procs_windows():
    """Return (procs, enumerate_ok).

    procs: list of dicts {pid, cmdline} for live git.exe processes.
    enumerate_ok: True if process enumeration actually worked (even if 0 git
    procs). False means we could not enumerate (platform/tooling failed) and
    must NOT assume "no writer" — conservative, so --clean-stale refuses.
    """
    procs = []
    enumerate_ok = False
    # Prefer wmic (fast); fall back to powershell if wmic is unavailable.
    try:
        out = subprocess.run(
            ["wmic", "process", "where", "Name like '%git%'", "get", "ProcessId,CommandLine", "/format:csv"],
            capture_output=True, text=True, timeout=10, errors="replace",
        ).stdout
        enumerate_ok = bool(out.strip())  # wmic answered something ⇒ enumeration worked
    except (subprocess.SubprocessError, OSError, FileNotFoundError):
        enumerate_ok = False
        out = ""
    if enumerate_ok:
        for line in out.splitlines():
            # csv format: Node,CommandLine,ProcessId
            parts = [p.strip() for p in line.split(",")]
            if len(parts) >= 3 and parts[2].isdigit():
                cmd = parts[1]
                if cmd and "git" in cmd.lower():
                    procs.append({"pid": parts[2], "cmdline": cmd})
        if procs:
            return procs, True
    # fallback: powershell
    try:
        ps = subprocess.run(
            ["powershell", "-NoProfile", "-Command",
             "Get-WmiObject Win32_Process | Where-Object { $_.Name -match '^git' } "
             "| Select-Object ProcessId,CommandLine | Format-List"],
            capture_output=True, text=True, timeout=15, errors="replace",
        ).stdout
        enumerate_ok = enumerate_ok or bool(ps.strip())
        pid = None
        for line in ps.splitlines():
            line = line.strip()
            if line.startswith("ProcessId"):
                pid = line.split(":", 1)[1].strip()
            elif line.startswith("CommandLine") and pid:
                cmd = line.split(":", 1)[1].strip() if ":" in line else ""
                if cmd and "git" in cmd.lower():
                    procs.append({"pid": pid, "cmdline": cmd})
                pid = None
    except (subprocess.SubprocessError, OSError, FileNotFoundError):
        pass
    return procs, enumerate_ok


def classify(cmdline: str) -> str:
    """Return 'write' | 'read' | 'other' for a git command line."""
    low = cmdline.lower()
    # Find the git subcommand token. Skip the binary path and flags.
    toks = low.replace("\\", " ").split()
    for t in toks:
        if t in WRITE_SUBCOMMANDS:
            return "write"
        if t in READ_SUBCOMMANDS:
            return "read"
    if any(w in low for w in ("ls-files", "status", "--porcelain", "rev-parse", "diff")):
        return "read"
    return "other"


def have_active_writer(procs) -> bool:
    """True if any LIVE git process is a potential index-write lock holder."""
    for p in procs:
        if classify(p["cmdline"]) == "write":
            return True
    return False


def _git_dir(root: Path) -> Path:
    """Return the actual git directory (where index.lock lives).

    In a normal repo this is ``root / ".git"``.  In a worktree the ``.git`` file
    contains ``gitdir: <path>`` and the real git dir is elsewhere.
    """
    git = root / ".git"
    if git.is_dir():
        return git
    # git worktree — .git is a file containing "gitdir: <path>"
    try:
        text = git.read_text(encoding="utf-8").strip()
    except OSError:
        return git  # fall back to the default; will fail gracefully later
    if text.startswith("gitdir:"):
        # The path in the file may be relative to the .git file location,
        # but git always writes absolute paths. Handle both for safety.
        d = Path(text.split(":", 1)[1].strip())
        if not d.is_absolute():
            d = (git.parent / d).resolve()
        return d
    return git


def lock_info(root: Path):
    gitd = _git_dir(root)
    lock = gitd / "index.lock"
    if not lock.exists():
        return None, None
    try:
        age = time.time() - lock.stat().st_mtime
    except OSError:
        age = 0.0
    size = lock.stat().st_size if lock.exists() else -1
    return lock, {"age_sec": max(0.0, age), "size": size}


def main() -> int:
    try:
        root = repo_root()
    except RuntimeError as e:
        print(f"[FAIL] {e}")
        return 1

    stale_after = 60
    clean = False
    args = sys.argv[1:]
    for a in args:
        if a == "--clean-stale":
            clean = True
        elif a.startswith("--stale-after"):
            try:
                stale_after = int(a.split("=", 1)[1])
            except (IndexError, ValueError):
                pass

    lock, info = lock_info(root)
    procs, enumerate_ok = _git_procs_auto()
    live_writer = have_active_writer(procs)
    live_writer_cmd = ""
    for p in procs:
        if classify(p["cmdline"]) == "write":
            live_writer_cmd = p["cmdline"][:120]
            break

    if not lock:
        # Also surface any LIVE write process even with no lock (could be staged mid-op).
        if live_writer:
            print(f"[WARN] no index.lock, but a live git writer is running: {live_writer_cmd}")
            return 0
        print("[PASS] no index.lock")
        return 0

    # A lock exists.
    age = info["age_sec"]
    sz = info["size"]
    if live_writer:
        print(
            f"[WARN] .git/index.lock present ({age:.0f}s old) AND a live git writer is running "
            f"(pid {['p'+p['pid'] for p in procs if classify(p['cmdline'])=='write']}): {live_writer_cmd}\n"
            f"  DO NOT remove — a write op may be mid-flight. Wait for it to finish, or it'll self-clear."
        )
        return 0

    # No live writer. Is it an orphaned/stale lock?
    if age >= stale_after or sz == 0:
        if clean:
            if not enumerate_ok:
                print(
                    f"[WARN] index.lock stale ({age:.0f}s) but I could NOT enumerate running processes "
                    "(platform tooling unavailable), so I can't prove no writer exists.\n"
                    f"  Refusing to auto-delete. If you're sure no git write is running, delete "
                    f".git/index.lock manually (or fix process enumeration and retry --clean-stale)."
                )
                return 0
            try:
                lock.unlink()
                print(f"[OK] removed orphaned {'.git/index.lock'} ({age:.0f}s old, {sz}B). "
                      "Stale lock is a leftover from a crashed/killed git write — safe to delete when no live writer runs.")
                return 0
            except OSError as e:
                print(f"[FAIL] failed to remove index.lock: {e}")
                return 1
        print(
            f"[WARN] .git/index.lock present ({age:.0f}s old, {sz}B) with NO live git writer.\n"
            f"  Likely an orphaned/late leftover. Safe to clean: `python scripts/cleanliness/check_git_lock.py --clean-stale`\n"
            f"  Live git procs now: {[p['cmdline'][:60] for p in procs] if procs else 'none'}"
        )
        return 0

    # Fresh lock, no writer (rare race): transient.
    print(f"[WARN] .git/index.lock present ({age:.0f}s old) — transient. Retry in a moment.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
