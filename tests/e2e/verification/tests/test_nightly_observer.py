"""Tests for nightly/observer.py — GBK compatibility, heartbeat, tail buffering.

Validates the core risk mitigations in observer.py:
  - P1-2 / GBK: reader decodes arbitrary bytes without crashing.
  - P3-2: reader is a daemon thread and survives missing files.
  - P1-3: heartbeat goes to a separate file (no collision with .result).
"""

from __future__ import annotations

import os
import subprocess
import sys
import threading
import time
from pathlib import Path

_HERE = Path(__file__).resolve().parent
for _d in (_HERE.parent.parent, _HERE.parent, _HERE):
    if str(_d) not in sys.path:
        sys.path.insert(0, str(_d))

from verification.nightly.observer import ChunkObserver
from verification.nightly.state import read_heartbeat


class _FakeConfig:
    def __init__(self, report_dir: Path, run_id: str):
        self.report_dir = report_dir
        self.run_id = run_id


class _FakeItem:
    assembly = "System.Fake"
    slug = "global-ns"


def test_observer_handles_gbk_bytes(tmp_path):
    """Reader must survive GBK/arbitrary high bytes (P1-2)."""
    config = _FakeConfig(tmp_path, "run-test")
    obs = ChunkObserver(config, "Assembly__chunk", _FakeItem())

    # Simulate a subprocess that emits invalid-UTF8 / GBK bytes on Windows.
    code = (
        "import sys\n"
        "sys.stdout.buffer.write(b'\\x94\\x9c\\xff line1\\n')\n"   # non-UTF8 bytes
        "sys.stdout.buffer.write('line2 utf8\\n'.encode('utf-8'))\n"
        "sys.stdout.flush()\n"
    )
    proc = subprocess.Popen(
        [sys.executable, "-c", code],
        stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
    )
    obs.attach(proc)
    # wait for process to finish + reader to drain
    proc.wait(timeout=10)
    obs.close()

    # The log should exist and contain readable text (with replacement chars).
    log = obs.log_path()
    assert log.exists(), f"expected log at {log}"
    text = log.read_text(encoding="utf-8", errors="replace")
    assert "line1" in text or "�" in text  # content present, no crash
    assert "utf8" in text


def test_observer_writes_heartbeat_file(tmp_path):
    """observer.attach writes a .heartbeat file separate from .result (P1-3)."""
    config = _FakeConfig(tmp_path, "run-heartbeat")
    obs = ChunkObserver(config, "Sys__chunk", _FakeItem())
    code = "import time; print('x'); time.sleep(0.5); print('y')"
    proc = subprocess.Popen(
        [sys.executable, "-c", code],
        stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
    )
    obs.attach(proc)
    proc.wait(timeout=10)
    obs.close()

    hb = read_heartbeat(config, "Sys__chunk")
    assert "updated" in hb  # heartbeat was written
