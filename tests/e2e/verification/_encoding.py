"""Unified console-encoding guard for the verification engine.

Root-cause fix for the recurring Windows GBK-console crash: Python's default
stdout/stderr encoding on a GBK/cp936 (or cp1252) console cannot represent
emoji (✅/❌/⚠️/🟢/🔴) and any `print()` of them raises
`UnicodeEncodeError`. The engine's entry points (chunk_pipeline, ci_smoke,
nightly_summary, benchmark_diff, ...) print such symbols, so on a GBK console
they die before doing any work unless PYTHONIOENCODING=utf-8 is set manually.

Fix: reconfigure sys.stdout/sys.stderr to UTF-8 with a *lossless* fallback
(errors="backslashreplace"). This makes every emoji print safe on any console
codepage and never raises. It runs at import time, so merely importing the
engine (directly or via `python -m` / a wrapper) is sufficient — no caller
needs to set PYTHONIOENCODING. It mirrors the project's other GBK handling in
native.py / python.py / ci_test.py (there `errors='replace'` on subprocess
capture; here on our own output streams).

Safe on non-Windows too (the reconfigure is a no-op if the stream is already
UTF-8; the try/except guards redirected/detached streams).
"""

from __future__ import annotations

import sys


def ensure_utf8_console() -> None:
    """Force stdout/stderr to UTF-8 with backslashreplace so emoji never crash."""
    for _stream_name in ("stdout", "stderr"):
        _stream = getattr(sys, _stream_name, None)
        if _stream is not None and hasattr(_stream, "reconfigure"):
            try:
                _stream.reconfigure(encoding="utf-8", errors="backslashreplace")
            except (ValueError, OSError):
                # Already redirected to a file/buffer that doesn't support
                # reconfigure (e.g. -u mode, IDE, test capture). Leave as-is.
                pass


# Apply immediately on import so any entry point that imports this module is
# protected without an explicit call.
ensure_utf8_console()
