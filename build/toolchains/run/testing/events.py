from __future__ import annotations

import os
from datetime import datetime, timezone
from typing import Any


SCHEMA_VERSION = "2026-04-03"


def utc_timestamp() -> str:
    return datetime.now(timezone.utc).replace(microsecond=0).isoformat().replace("+00:00", "Z")


def _resolve_trace_id() -> str:
    """Resolve the current traceId from the trace system or env var."""
    try:
        from testing.trace import get_trace_id
        tid = get_trace_id()
        if tid:
            return tid
    except ImportError:
        pass
    return os.environ.get("CHAOS_TRACE_ID", "")


def build_event(
    event_type: str,
    payload: dict[str, Any] | None = None,
    *,
    run_id: str | None = None,
    trace_id: str | None = None,
    suite_id: str | None = None,
    stage: str | None = None,
    status: str | None = None,
    stream_scope: str | None = None,
    subject_id: str | None = None,
    matrix_id: str | None = None,
    goal_id: str | None = None,
    stage_id: str | None = None,
    bucket: str | None = None,
    stage_scope: str | None = None,
) -> dict[str, Any]:
    return {
        "schemaVersion": SCHEMA_VERSION,
        "eventType": event_type,
        "timestampUtc": utc_timestamp(),
        "runId": run_id,
        "traceId": trace_id or _resolve_trace_id(),
        "streamScope": stream_scope,
        "subjectId": subject_id,
        "matrixId": matrix_id,
        "goalId": goal_id,
        "stageId": stage_id,
        "bucket": bucket,
        "stageScope": stage_scope,
        "suiteId": suite_id,
        "stage": stage,
        "status": status,
        "payload": dict(payload or {}),
    }
