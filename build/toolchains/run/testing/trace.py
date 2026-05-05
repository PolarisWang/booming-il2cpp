"""Cross-language trace system for Python (standalone module).

Usage:
    from testing.trace import trace_init, trace, trace_call, set_run_id, get_trace_id

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
import uuid
from datetime import datetime, timezone
from functools import wraps
from pathlib import Path
from typing import Any, Callable


_TRACE_PATH: Path | None = None
_TRACE_SESSION_ID: str = ""
_TRACE_ID: str = ""
_RUN_ID: str = ""
_CURRENT_STAGE: str = ""
_NEXT_SPAN_ID: int = 0
_SPAN_STACK: list[str] = []

# Sentinel for idempotent initialization
_INITIALIZED: bool = False


def _utc_timestamp() -> str:
    return datetime.now(timezone.utc).replace(microsecond=0).isoformat().replace("+00:00", "Z")


def set_run_id(run_id: str) -> None:
    """Set the run_id for correlation with the events system."""
    global _RUN_ID
    _RUN_ID = run_id


def get_trace_id() -> str:
    """Return the current traceId (empty string if not initialized)."""
    return _TRACE_ID


def get_session_id() -> str:
    """Return the current session ID (empty string if not initialized)."""
    return _TRACE_SESSION_ID


def trace_init(repo_root: Path, *, stage: str = "") -> str:
    """Initialize a trace session. Creates the JSONL file and exports env vars.

    Idempotent: subsequent calls are no-ops if already initialized.
    """
    global _TRACE_PATH, _TRACE_SESSION_ID, _TRACE_ID, _CURRENT_STAGE, _INITIALIZED

    if _INITIALIZED and _TRACE_PATH is not None:
        return _TRACE_SESSION_ID

    # Generate or inherit traceId for cross-language correlation
    _TRACE_ID = os.environ.get("CHAOS_TRACE_ID", "")
    if not _TRACE_ID:
        _TRACE_ID = uuid.uuid4().hex[:16]
    os.environ["CHAOS_TRACE_ID"] = _TRACE_ID

    timestamp = datetime.now().strftime("%Y%m%d-%H%M%S")
    _TRACE_SESSION_ID = f"{timestamp}-{os.getpid()}"
    _TRACE_PATH = repo_root / "artifacts" / "logs" / "trace" / _TRACE_SESSION_ID / "trace.jsonl"
    _TRACE_PATH.parent.mkdir(parents=True, exist_ok=True)
    _CURRENT_STAGE = stage
    _trace_write({
        "t": _utc_timestamp(),
        "l": "python",
        "o": "session_start",
        "traceId": _TRACE_ID,
        "session": _TRACE_SESSION_ID,
        "stage": stage,
        "f": f"{os.path.relpath(sys._getframe(1).f_code.co_filename, start=os.getcwd())}:{sys._getframe(1).f_lineno}",
    })
    # Update last.json pointer
    pointer = {
        "sessionId": _TRACE_SESSION_ID,
        "traceId": _TRACE_ID,
        "tracePath": _relative_posix(repo_root, _TRACE_PATH),
        "timestamp": _utc_timestamp(),
    }
    pointer_dir = repo_root / "artifacts" / "logs" / "trace"
    pointer_dir.mkdir(parents=True, exist_ok=True)
    (pointer_dir / "last.json").write_text(json.dumps(pointer, ensure_ascii=False), encoding="utf-8")
    # Export env vars so C# / C++ child processes find the same trace file and traceId
    os.environ["CHAOS_TRACE_PATH"] = str(_TRACE_PATH)
    _INITIALIZED = True
    return _TRACE_SESSION_ID


def _relative_posix(repo_root: Path, path: Path) -> str:
    return path.relative_to(repo_root).as_posix()


def _next_span_id() -> str:
    global _NEXT_SPAN_ID
    _NEXT_SPAN_ID += 1
    return f"s{_NEXT_SPAN_ID}"


def trace(op: str, *, stage: str | None = None, span_id: str | None = None, parent_span_id: str | None = None, **kwargs: Any) -> None:
    """Append a single trace record."""
    if _TRACE_PATH is None:
        return
    record: dict[str, Any] = {
        "t": _utc_timestamp(),
        "l": "python",
        "o": op,
        "traceId": _TRACE_ID,
    }
    if _RUN_ID:
        record["runId"] = _RUN_ID
    if span_id:
        record["spanId"] = span_id
    if parent_span_id:
        record["parentSpanId"] = parent_span_id
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
    """Decorator: auto-trace function entry, exit (with duration), and exception.

    Automatically generates spanId and parentSpanId for span hierarchy.
    """
    def decorator(func: Callable) -> Callable:
        func_op = op or func.__name__

        @wraps(func)
        def wrapper(*args: Any, **kwargs: Any) -> Any:
            if _TRACE_PATH is None:
                return func(*args, **kwargs)

            # Span hierarchy: current span becomes parent of child spans
            span_id = _next_span_id()
            parent_span_id = _SPAN_STACK[-1] if _SPAN_STACK else None
            _SPAN_STACK.append(span_id)

            arg_summary: dict[str, Any] = {}
            if log_args:
                sig = inspect.signature(func)
                bound = sig.bind(*args, **kwargs)
                bound.apply_defaults()
                for pname, pval in bound.arguments.items():
                    arg_summary[pname] = _summarize_value(pval)

            t0 = time.time()
            trace(func_op, stage=stage, phase="enter",
                  span_id=span_id, parent_span_id=parent_span_id,
                  **arg_summary)
            try:
                result = func(*args, **kwargs)
            except BaseException as exc:
                elapsed = (time.time() - t0) * 1000
                trace(func_op, stage=stage, phase="exception",
                      span_id=span_id, parent_span_id=parent_span_id,
                      exception=type(exc).__name__, message=str(exc)[:200],
                      dur_ms=round(elapsed, 1))
                _SPAN_STACK.pop()
                raise
            elapsed = (time.time() - t0) * 1000
            trace(func_op, stage=stage, phase="exit",
                  span_id=span_id, parent_span_id=parent_span_id,
                  dur_ms=round(elapsed, 1),
                  result=_summarize_value(result))
            _SPAN_STACK.pop()
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
