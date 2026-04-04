from __future__ import annotations

from datetime import datetime, timezone
from typing import Any


SCHEMA_VERSION = "2026-04-03"


def utc_timestamp() -> str:
    return datetime.now(timezone.utc).replace(microsecond=0).isoformat().replace("+00:00", "Z")


def build_event(
    event_type: str,
    payload: dict[str, Any] | None = None,
    *,
    run_id: str | None = None,
    suite_id: str | None = None,
    stage: str | None = None,
    status: str | None = None,
) -> dict[str, Any]:
    return {
        "schemaVersion": SCHEMA_VERSION,
        "eventType": event_type,
        "timestampUtc": utc_timestamp(),
        "runId": run_id,
        "suiteId": suite_id,
        "stage": stage,
        "status": status,
        "payload": dict(payload or {}),
    }
