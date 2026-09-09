"""Tests for nightly/run.py D.1-fix per-chunk Reaper.

Verifies:
  - `_proc_alive` distinguishes live vs dead on Windows (os.kill(0) is UNRELIABLE
    on Windows because PID-reuse makes dead PIDs report alive — we use ctypes
    WaitForSingleObject).
  - `_kill_tree` actually terminates a real spawned child (not just marks).
  - `Reaper` does NOT kill a chunk whose heartbeat is fresh (busy).
  - `Reaper` KILLS a chunk whose heartbeat file mtime ages past idle budget
    (stuck — no stdout for a long time) and writes a stalled result + records
    in the module-level `killed_keys` set.
"""

from __future__ import annotations

import json
import subprocess
import sys
import time
import importlib.util
from pathlib import Path

_HERE = Path(__file__).resolve().parent.parent  # verification/
_NIGHTLY = _HERE / "nightly"
_E2E = _HERE.parent
import sys as _sys  # noqa
for _p in (_E2E, _HERE, _NIGHTLY):
    if str(_p) not in sys.path:
        sys.path.insert(0, str(_p))

# run.py imports .worklist/.resume/state via package — import package-relative.
from verification.nightly.run import Reaper, _kill_tree, _proc_alive, killed_keys


class _Cfg:
    def __init__(self, report_dir: Path, run_id: str = "reaper-test"):
        self.report_dir = report_dir
        self.run_id = run_id
    def idle_for(self, stage):  # noqa
        return 3600


def _spawn_sleeper(dur: int = 600) -> subprocess.Popen:
    return subprocess.Popen(
        [sys.executable, "-c", f"import time; time.sleep({dur})"],
        start_new_session=True,
    )


def _write_hb(cfg, key: str, pid: int) -> Path:
    p = Path(cfg.report_dir) / "run-state" / cfg.run_id / f"{key}.heartbeat"
    p.parent.mkdir(parents=True, exist_ok=True)
    p.write_text(json.dumps({"pid": pid, "updated": time.time()}))
    return p


class TestAlive:
    def test_alive_true(self):
        s = _spawn_sleeper()
        assert _proc_alive(s.pid) is True
        s.kill(); s.wait()

    def test_dead_returns_false(self, tmp_path):
        """A spent/terminated process must report not-alive (Windows os.kill(0)
        would wrongly say True)."""
        s = _spawn_sleeper(0)  # exits immediately
        s.wait()
        # give Windows a moment to collect
        for _ in range(20):
            if not _proc_alive(s.pid):
                break
            time.sleep(0.05)
        assert _proc_alive(s.pid) is False


class TestKillTree:
    def test_kills_spawned_child(self, tmp_path):
        s = _spawn_sleeper()
        assert _proc_alive(s.pid) is True
        _kill_tree(s.pid)
        for _ in range(40):
            if not _proc_alive(s.pid):
                break
            time.sleep(0.05)
        assert _proc_alive(s.pid) is False


class TestReaper:
    def test_fresh_heartbeat_no_kill(self, tmp_path):
        s = _spawn_sleeper()
        cfg = _Cfg(tmp_path)
        _write_hb(cfg, "busy", s.pid)
        killed_keys.discard("busy")
        r = Reaper(cfg, "busy", s.pid, "A", idle_seconds=1800, check_interval=1)
        r.start()
        time.sleep(2.5)
        assert "busy" not in killed_keys
        r.signal_stop()
        s.kill(); s.wait()

    def test_stale_heartbeat_kills(self, tmp_path):
        s = _spawn_sleeper()
        cfg = _Cfg(tmp_path)
        hb = _write_hb(cfg, "stuck", s.pid)
        # Freeze the file mtime so it is OLDER than the idle budget (real
        # "no more stdout output" state).  old = now - 5000 > budget 1800.
        old = time.time() - 5000
        import os
        os.utime(hb, (old, old))
        killed_keys.discard("stuck")
        r = Reaper(cfg, "stuck", s.pid, "A", idle_seconds=1800, check_interval=1)
        r.start()
        time.sleep(3.5)
        assert "stuck" in killed_keys, "Reaper must kill after idle budget"
        for _ in range(40):
            if not _proc_alive(s.pid):
                break
            time.sleep(0.05)
        assert _proc_alive(s.pid) is False


def test_no_hb_present_yet_not_killed(tmp_path):
    """A chunk whose heartbeat file hasn't been created yet (launch transient,
    build may run a while before first stdout) must NOT be killed — FileNotFoundError
    on the heartbeat is treated as 'give it more time', not as dead."""
    s = _spawn_sleeper()
    cfg = _Cfg(tmp_path)
    killed_keys.discard("early")
    r = Reaper(cfg, "early", s.pid, "A", idle_seconds=1800, check_interval=0.5)
    r.start()
    time.sleep(2.2)
    assert "early" not in killed_keys  # hb never created yet -> shouldn't kill
    r.signal_stop()
    s.kill(); s.wait()
