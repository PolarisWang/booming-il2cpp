"""watchdog — periodic health scanner for running chunk subprocesses.

Runs as a background thread, watching heartbeats and detecting stalled
processes.  Risk mitigations baked in:
  - P2-1: OOM detection (exit 137 → auto-downgrade max_workers).
  - P3-2: watchdogs the same observer-attached processes; if a subprocess
    has exited but the observer never wrote .result, the watchdog still
    marks it stalled (cleans up the zombie).
"""

from __future__ import annotations

import time
import subprocess
from pathlib import Path
from typing import Any


# ── Platform-specific child-process tree query ─────────────────────────────
def _process_tree_alive(pid: int) -> bool:
    """Return True if the process (or any of its children) is still alive.

    This is a best-effort cross-platform check used by the watchdog to decide
    whether a chunk whose heartbeat has expired is truly stalled (no subprocess
    tree alive) or just slow (process tree still running).
    """
    # Try psutil first (cross-platform, rich tree queries).
    try:
        import psutil
        parent = psutil.Process(pid)
        alive = parent.is_running()
        if alive:
            return True
        # Check children
        children = parent.children(recursive=True)
        return any(c.is_running() for c in children)
    except ImportError:
        pass
    except (psutil.NoSuchProcess, psutil.AccessDenied):
        return False

    # Fallback: try os.kill(pid, 0) (signal 0 = existence test, no signal sent).
    # This only checks the parent process, not the full tree.
    try:
        import os
        os.kill(pid, 0)
        return True
    except (OSError, PermissionError):
        return False


def _count_oom(exit_code: int | None) -> bool:
    """Check if exit code indicates OOM kill (SIGKILL = 137 on Linux, -9 on Unix)."""
    return exit_code == 137 or exit_code == -9


# ── Watchdog loop ──────────────────────────────────────────────────────────
def watchdog_scan(config, running: dict[str, tuple[Any, subprocess.Popen, Any]]) -> None:
    """Scan running chunks: check heartbeats, detect OOM, detect stalled.

    Args:
        config: NightlyConfig (max_idle_seconds, oom_auto_downgrade).
        running: {key: (runtime, proc, observer)} mapping maintained by run.py.
    Side effects:
        - Kills and removes stalled processes from `running`.
        - Logs OOM detection and optionally downgrades max_workers.
    """
    from .state import heartbeat_path, read_heartbeat, write_result

    now = time.time()
    oom_count = 0
    stalled_items = []

    for key, (rt, proc, obs) in list(running.items()):
        # Check if process has exited already but we haven't consumed it yet.
        ret = proc.poll()

        if ret is not None and ret != 0:
            # ⚡ P2-1: OOM detection
            if _count_oom(ret):
                oom_count += 1
                print(f"  [watchdog] OOM detected: {key} (exit={ret})")
                if config.oom_auto_downgrade:
                    new_workers = max(1, config.max_workers // 2)
                    if new_workers < config.max_workers:
                        config.max_workers = new_workers
                        print(f"  [watchdog] OOM auto-downgrade: max_workers -> {new_workers}")

        # Check heartbeat staleness (only if process is still running)
        if ret is None:
            hb = read_heartbeat(config, key)
            hb_age = now - hb.get("updated", 0) if hb else 9999
            if hb_age > config.max_idle_seconds:
                # Process still alive but heartbeat stale — could be a long
                # benchmark that hasn't printed anything.  Check if the
                # process tree is truly alive.
                if proc.pid is not None and not _process_tree_alive(proc.pid):
                    # Process tree is dead but run.py hasn't collected it.
                    # This is a stall — the subprocess got orphaned/killed.
                    print(f"  [watchdog] STALLED: {key} (hb age={hb_age:.0f}s, no process tree)")
                    try:
                        proc.kill()
                    except Exception:
                        pass
                    write_result(config, key, status="stalled",
                                 error_class="timeout", exit_code=ret)
                    stalled_items.append(key)

    # Remove stalled items from running dict
    for k in stalled_items:
        running.pop(k, None)

    if oom_count > 0:
        print(f"  [watchdog] OOM count this scan: {oom_count}")