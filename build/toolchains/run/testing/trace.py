"""Cross-language trace system for Python (standalone module).

Usage:
    from testing.trace import trace_init, trace, trace_call

    trace_init(repo_root, stage="audit")
    trace("my_operation", key1="value1", count=42)

    @trace_call("my_func")
    def my_func(...):
        ...
"""

from __future__ import annotations

import inspect
import json
import os
import sys
import time
from datetime import datetime, timezone
from functools import wraps
from pathlib import Path
from typing import Any, Callable


_TRACE_PATH: Path | None = None
_TRACE_SESSION_ID: str = ""
_CURRENT_STAGE: str = ""


def _utc_timestamp() -> str:
    return datetime.now(timezone.utc).replace(microsecond=0).isoformat().replace("+00:00", "Z")


def trace_init(repo_root: Path, *, stage: str = "") -> str:
    """Initialize a trace session. Creates the JSONL file and exports env var."""
    global _TRACE_PATH, _TRACE_SESSION_ID, _CURRENT_STAGE
    timestamp = datetime.now().strftime("%Y%m%d-%H%M%S")
    _TRACE_SESSION_ID = f"{timestamp}-{os.getpid()}"
    _TRACE_PATH = repo_root / "artifacts" / "logs" / "trace" / _TRACE_SESSION_ID / "trace.jsonl"
    _TRACE_PATH.parent.mkdir(parents=True, exist_ok=True)
    _CURRENT_STAGE = stage
    _trace_write({
        "t": _utc_timestamp(),
        "l": "python",
        "o": "session_start",
        "session": _TRACE_SESSION_ID,
        "stage": stage,
    })
    # Update last.json pointer
    pointer = {
        "sessionId": _TRACE_SESSION_ID,
        "tracePath": _relative_posix(repo_root, _TRACE_PATH),
        "timestamp": _utc_timestamp(),
    }
    pointer_dir = repo_root / "artifacts" / "logs" / "trace"
    pointer_dir.mkdir(parents=True, exist_ok=True)
    (pointer_dir / "last.json").write_text(json.dumps(pointer, ensure_ascii=False), encoding="utf-8")
    # Export env var so C# / C++ child processes find the same trace file
    os.environ["CHAOS_TRACE_PATH"] = str(_TRACE_PATH)
    return _TRACE_SESSION_ID


def _relative_posix(repo_root: Path, path: Path) -> str:
    return path.relative_to(repo_root).as_posix()


def trace(op: str, *, stage: str | None = None, **kwargs: Any) -> None:
    """Append a single trace record."""
    if _TRACE_PATH is None:
        return
    record: dict[str, Any] = {
        "t": _utc_timestamp(),
        "l": "python",
        "o": op,
    }
    if stage:
        record["s"] = stage
    elif _CURRENT_STAGE:
        record["s"] = _CURRENT_STAGE
    # Caller location
    frame = sys._getframe(1)
    record["f"] = f"{os.path.relpath(frame.f_code.co_filename, start=os.getcwd())}:{frame.f_lineno}"
    record.update(kwargs)
    _trace_write(record)


def _trace_write(record: dict[str, Any]) -> None:
    if _TRACE_PATH is None:
        return
    try:
        with _TRACE_PATH.open("a", encoding="utf-8") as f:
            f.write(json.dumps(record, ensure_ascii=False) + "\n")
    except OSError:
        pass


def trace_call(op: str | None = None, *, stage: str | None = None, log_args: bool = True) -> Callable:
    """Decorator: auto-trace function entry, exit (with duration), and exception."""
    def decorator(func: Callable) -> Callable:
        func_op = op or func.__name__

        @wraps(func)
        def wrapper(*args: Any, **kwargs: Any) -> Any:
            if _TRACE_PATH is None:
                return func(*args, **kwargs)

            arg_summary: dict[str, Any] = {}
            if log_args:
                sig = inspect.signature(func)
                bound = sig.bind(*args, **kwargs)
                bound.apply_defaults()
                for pname, pval in bound.arguments.items():
                    arg_summary[pname] = _summarize_value(pval)

            t0 = time.time()
            trace(func_op, stage=stage, phase="enter", **arg_summary)
            try:
                result = func(*args, **kwargs)
            except BaseException as exc:
                elapsed = (time.time() - t0) * 1000
                trace(func_op, stage=stage, phase="exception",
                      exception=type(exc).__name__, message=str(exc)[:200],
                      dur_ms=round(elapsed, 1))
                raise
            elapsed = (time.time() - t0) * 1000
            trace(func_op, stage=stage, phase="exit",
                  dur_ms=round(elapsed, 1),
                  result=_summarize_value(result))
            return result

        return wrapper
    return decorator


def _summarize_value(val: Any) -> Any:
    """Produce a compact trace-friendly summary of a value."""
    if val is None:
        return None
    if isinstance(val, (bool, int, float, str)):
        return val
    if isinstance(val, (list, tuple)):
        if len(val) == 0:
            return "[]"
        if len(val) <= 3:
            return [str(v)[:80] for v in val]
        return f"[{len(val)} items]"
    if isinstance(val, dict):
        if len(val) == 0:
            return "{}"
        keys = list(val.keys())[:5]
        return f"{{{', '.join(str(k) for k in keys)}...}} ({len(val)} keys)"
    if isinstance(val, Path):
        return str(val)
    return type(val).__name__
