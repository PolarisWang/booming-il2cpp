"""observer — per-chunk subprocess stdout reader + heartbeat + error-tracking.

Reads a chunk_pipeline subprocess's stdout/stderr, teeing to a per-chunk log and
giving live, structured observability.  Risk mitigations baked in:
  - P1-2 / GBK: read() decodes with utf-8 + errors='replace' (kB) — a subprocess
    that emits GBK/other bytes won't crash the reader.
  - P3-2: reader thread is daemon=True and self-terminates on pipe EOF.
  - P1-3: heartbeat goes to a separate .heartbeat file (never collides with the
    orchestrator's .result write).
"""

from __future__ import annotations

import threading
import time
from pathlib import Path


class ChunkObserver:
    """Read one chunk_pipeline subprocess's output stream (merged stdout) live."""

    def __init__(self, config, key: str, item) -> None:
        self._config = config
        self._key = key
        self._item = item
        self._log_dir = (
            Path(config.report_dir) / "logs" / config.run_id / item.assembly / item.slug
        )
        self._log_path = self._log_dir / "run.log"
        self._tail: list[str] = []
        self._tail_lock = threading.Lock()
        self._reader: threading.Thread | None = None
        self._done = threading.Event()
        self._last_hb = 0.0

    # -- lifecycle ---------------------------------------------------------
    def attach(self, proc) -> None:
        """proc: subprocess.Popen with stdout=PIPE, stderr=STDOUT."""
        self._log_dir.mkdir(parents=True, exist_ok=True)
        self._file = open(self._log_path, "a", encoding="utf-8", errors="replace")
        self._reader = threading.Thread(
            target=self._read_loop, args=(proc,), daemon=True, name=f"obs-{self._key}"
        )
        self._reader.start()

    def _read_loop(self, proc) -> None:
        try:
            stream = proc.stdout
            assert stream is not None
            while True:
                raw = stream.readline(4096)  # type: ignore[union-attr]
                if not raw:
                    break
                # The stream may be in text mode (run.py passes encoding="utf-8",
                # errors="replace") → readline yields str; or in binary mode
                # (tests/standalone) → yields bytes.  Normalize to str safely.
                if isinstance(raw, bytes):
                    text = raw.decode("utf-8", errors="replace")
                else:
                    text = raw
                try:
                    self._file.write(text)
                    self._file.flush()
                except OSError:
                    pass
                # Per-block heartbeat
                now = time.time()
                if now - self._last_hb >= 1.0:
                    from .state import touch_heartbeat
                    touch_heartbeat(self._config, self._key, pid=getattr(proc, "pid", None))
                    self._last_hb = now
                with self._tail_lock:
                    self._tail.append(text)
                    if len(self._tail) > 20:
                        self._tail.pop(0)
        except (ValueError, OSError):
            pass  # pipe closed / file closed
        finally:
            self._done.set()
            try:
                self._file.close()
            except Exception:
                pass

    def tail_text(self) -> str:
        """Last 20 in-memory lines (fast; for progress only)."""
        with self._tail_lock:
            return "".join(self._tail)

    def full_log_text(self, max_chars: int = 200_000) -> str:
        """Read back the whole run.log (up to a cap).  Used by the error
        classifier so a distinguishing error earlier in the log (e.g. the
        hotupdate rebuild note) is not missed when the last 20 lines are just
        the pass/fail summary."""
        try:
            p = self._log_path
            if p and p.is_file():
                return p.read_text(encoding="utf-8", errors="replace")[-max_chars:]
        except OSError:
            pass
        return self.tail_text()

    def log_path(self) -> Path:
        return self._log_path

    def close(self) -> None:
        self._done.wait(timeout=3)  # allow reader to flush on natural exit
        if self._reader is not None and self._reader.is_alive():
            # daemon thread — will die with process; nothing to join forcefully.
            pass
        try:
            if getattr(self, "_file", None) and not self._file.closed:
                self._file.close()
        except Exception:
            pass


class NullObserver:
    """No-op observer used when log capture is disabled (e.g. quiet tests)."""

    def attach(self, proc) -> None:  # noqa: D102
        pass

    def tail_text(self) -> str:  # noqa: D102
        return ""

    def log_path(self):  # noqa: D102
        return None

    def close(self) -> None:  # noqa: D102
        pass
