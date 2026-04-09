from __future__ import annotations

from datetime import datetime
from pathlib import Path
from typing import Any
from uuid import uuid4

try:
    from .common import read_json, write_json
    from .testing.events import build_event
except ImportError:
    from common import read_json, write_json
    from testing.events import build_event


def _build_run_id(host_platform: str) -> str:
    timestamp = datetime.now().strftime("%Y%m%d-%H%M%S")
    return f"{timestamp}-{host_platform}-{uuid4().hex[:4]}"


def _logs_root(repo_root: Path) -> Path:
    return repo_root / "artifacts" / "logs" / "run"


def _relative_path(repo_root: Path, path: Path) -> str:
    return path.relative_to(repo_root).as_posix()


def _write_text(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8")


def _append_text(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("a", encoding="utf-8") as handle:
        handle.write(text)


def _finalize_current_pointer(current_path: Path, run_id: str, pointer_payload: dict[str, Any]) -> None:
    if not current_path.is_file():
        return
    try:
        current_payload = read_json(current_path)
    except (OSError, ValueError):
        return
    if current_payload.get("runId") != run_id:
        return
    try:
        current_path.unlink()
        return
    except OSError:
        # Some Windows workspaces deny deleting freshly written files.
        pass
    try:
        write_json(current_path, pointer_payload)
    except OSError:
        pass


def start_operation_report(
    *,
    repo_root: Path,
    host_platform: str,
    command_text: str,
) -> dict[str, Any]:
    run_id = _build_run_id(host_platform)
    session_root = _logs_root(repo_root) / run_id
    session_path = session_root / "session.json"
    summary_path = session_root / "summary.json"
    events_path = session_root / "events.jsonl"
    console_path = session_root / "console.log"
    telemetry_path = session_root / "telemetry.json"

    session_payload = {
        "runId": run_id,
        "command": command_text,
        "hostPlatform": host_platform,
        "status": "running",
        "errors": [],
    }
    write_json(session_path, session_payload)
    _write_text(console_path, "")

    pointer_payload = {
        "runId": run_id,
        "command": command_text,
        "hostPlatform": host_platform,
        "sessionPath": _relative_path(repo_root, session_path),
        "summaryPath": _relative_path(repo_root, summary_path),
        "eventsPath": _relative_path(repo_root, events_path),
        "status": "running",
        "reportKind": "operation",
    }
    write_json(_logs_root(repo_root) / "current.json", pointer_payload)
    write_json(_logs_root(repo_root) / "last.json", pointer_payload)

    return {
        "runId": run_id,
        "sessionRoot": session_root,
        "sessionPath": session_path,
        "summaryPath": summary_path,
        "eventsPath": events_path,
        "consolePath": console_path,
        "telemetryPath": telemetry_path,
    }


def append_operation_event(run_context: dict[str, Any], event: dict[str, Any]) -> None:
    _append_text(Path(run_context["eventsPath"]), _json_line(_normalize_operation_event(run_context, event)))


def _normalize_operation_event(run_context: dict[str, Any], event: dict[str, Any]) -> dict[str, Any]:
    normalized = dict(event)
    run_id = str(run_context["runId"])
    if not normalized.get("runId"):
        normalized["runId"] = run_id
    return normalized


def _json_line(payload: dict[str, Any]) -> str:
    import json

    return json.dumps(payload, ensure_ascii=False) + "\n"


def finalize_operation_report(
    *,
    repo_root: Path,
    host_platform: str,
    command_text: str,
    status: str,
    errors: list[str],
    artifacts: list[Any],
    important_outputs: list[dict[str, str]],
    console_text: str,
    run_context: dict[str, Any],
) -> dict[str, str]:
    session_path = Path(run_context["sessionPath"])
    summary_path = Path(run_context["summaryPath"])
    events_path = Path(run_context["eventsPath"])
    console_path = Path(run_context["consolePath"])
    telemetry_path = Path(run_context["telemetryPath"])
    run_id = str(run_context["runId"])

    final_status = "ok" if status == "ok" else "fail"
    exit_code = 0 if status == "ok" else 1

    write_json(
        session_path,
        {
            "runId": run_id,
            "command": command_text,
            "hostPlatform": host_platform,
            "status": status,
            "errors": list(errors),
        },
    )
    _write_text(console_path, console_text)
    write_json(
        telemetry_path,
        {
            "runId": run_id,
            "artifactCount": len(artifacts),
            "importantOutputCount": len(important_outputs),
            "errorCount": len(errors),
        },
    )

    summary_payload = {
        "runId": run_id,
        "command": command_text,
        "hostPlatform": host_platform,
        "finalStatus": final_status,
        "exitCode": exit_code,
        "errors": list(errors),
        "artifacts": list(artifacts),
        "importantOutputs": list(important_outputs),
        "sessionPath": _relative_path(repo_root, session_path),
        "summaryPath": _relative_path(repo_root, summary_path),
        "eventsPath": _relative_path(repo_root, events_path),
        "consolePath": _relative_path(repo_root, console_path),
        "telemetryPath": _relative_path(repo_root, telemetry_path),
        "reportKind": "operation",
    }
    write_json(summary_path, summary_payload)

    pointer_payload = {
        "runId": run_id,
        "command": command_text,
        "hostPlatform": host_platform,
        "sessionPath": _relative_path(repo_root, session_path),
        "summaryPath": _relative_path(repo_root, summary_path),
        "eventsPath": _relative_path(repo_root, events_path),
        "status": final_status,
        "reportKind": "operation",
    }
    write_json(_logs_root(repo_root) / "last.json", pointer_payload)
    _finalize_current_pointer(_logs_root(repo_root) / "current.json", run_id, pointer_payload)

    return {
        "runId": run_id,
        "sessionPath": _relative_path(repo_root, session_path),
        "summaryPath": _relative_path(repo_root, summary_path),
        "eventsPath": _relative_path(repo_root, events_path),
        "consolePath": _relative_path(repo_root, console_path),
        "telemetryPath": _relative_path(repo_root, telemetry_path),
        "reportKind": "operation",
    }
