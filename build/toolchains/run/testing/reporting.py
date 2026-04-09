from __future__ import annotations

import json
import re
import sys
from datetime import datetime
from pathlib import Path
from typing import Any
from uuid import uuid4

try:
    from ..common import read_json, write_json
    from .events import build_event
    from .traffic_light import TRAFFIC_LIGHT_BUCKETS
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from common import read_json, write_json
    from testing.events import build_event
    from testing.traffic_light import TRAFFIC_LIGHT_BUCKETS


ANSI_ESCAPE_RE = re.compile(r"\x1b\[[0-9;]*[A-Za-z]")


def _build_run_id(host_platform: str) -> str:
    timestamp = datetime.now().strftime("%Y%m%d-%H%M%S")
    return f"{timestamp}-{host_platform}-{uuid4().hex[:4]}"


def build_run_id(host_platform: str) -> str:
    return _build_run_id(host_platform)


def _strip_ansi(text: str) -> str:
    return ANSI_ESCAPE_RE.sub("", text)


def _relative_path(repo_root: Path, path: Path) -> str:
    return path.relative_to(repo_root).as_posix()


def _write_text(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8")


def _append_text(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("a", encoding="utf-8") as handle:
        handle.write(text)


def _empty_status_counts() -> dict[str, int]:
    return {
        "total": 0,
        "ok": 0,
        "fail": 0,
        "skip": 0,
        "aborted": 0,
    }


def _empty_traffic_light_counts() -> dict[str, dict[str, int]]:
    return {bucket: _empty_status_counts() for bucket in TRAFFIC_LIGHT_BUCKETS}


def _add_status_count(counts: dict[str, int], status: str) -> None:
    normalized_status = status if status in {"ok", "fail", "skip", "aborted"} else "aborted"
    counts["total"] += 1
    counts[normalized_status] += 1


def _build_case_counts(case_results: list[dict[str, Any]]) -> dict[str, int]:
    counts = _empty_status_counts()
    for case_result in case_results:
        _add_status_count(counts, str(case_result.get("status", "aborted")))
    return counts


def _logs_root(repo_root: Path) -> Path:
    return repo_root / "artifacts" / "logs" / "tests"


def _normalize_pointer_roots(repo_root: Path, pointer_roots: list[Path] | None) -> list[Path]:
    if pointer_roots:
        return [Path(root) for root in pointer_roots]
    return [_logs_root(repo_root)]


def _write_pointer_payloads(pointer_roots: list[Path], pointer_name: str, payload: dict[str, Any]) -> None:
    for pointer_root in pointer_roots:
        write_json(pointer_root / pointer_name, payload)


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


def start_session_report(
    *,
    repo_root: Path,
    host_platform: str,
    command_text: str,
    run_id: str | None = None,
    session_root: Path | None = None,
    pointer_roots: list[Path] | None = None,
) -> dict[str, Any]:
    resolved_run_id = run_id or _build_run_id(host_platform)
    resolved_session_root = session_root or (_logs_root(repo_root) / resolved_run_id)
    resolved_pointer_roots = _normalize_pointer_roots(repo_root, pointer_roots)
    session_path = resolved_session_root / "session.json"
    summary_path = resolved_session_root / "summary.json"
    events_path = resolved_session_root / "events.jsonl"
    console_path = resolved_session_root / "console.log"
    telemetry_path = resolved_session_root / "telemetry.json"

    session_payload = {
        "runId": resolved_run_id,
        "command": command_text,
        "hostPlatform": host_platform,
        "status": "running",
        "errors": [],
    }
    write_json(session_path, session_payload)
    _write_text(console_path, "")
    pointer_payload = {
        "runId": resolved_run_id,
        "command": command_text,
        "hostPlatform": host_platform,
        "sessionPath": _relative_path(repo_root, session_path),
        "summaryPath": _relative_path(repo_root, summary_path),
        "eventsPath": _relative_path(repo_root, events_path),
        "status": "running",
    }
    _write_pointer_payloads(resolved_pointer_roots, "current.json", pointer_payload)
    _write_pointer_payloads(resolved_pointer_roots, "last.json", pointer_payload)
    append_session_event(
        repo_root,
        {
            "runId": resolved_run_id,
            "eventsPath": events_path,
        },
        build_event(
            "session-start",
            {
                "runId": resolved_run_id,
                "command": command_text,
                "hostPlatform": host_platform,
            },
            run_id=resolved_run_id,
            status="running",
        ),
    )
    return {
        "runId": resolved_run_id,
        "sessionRoot": resolved_session_root,
        "sessionPath": session_path,
        "summaryPath": summary_path,
        "eventsPath": events_path,
        "consolePath": console_path,
        "telemetryPath": telemetry_path,
        "pointerRoots": [str(path) for path in resolved_pointer_roots],
    }


def append_session_event(repo_root: Path, run_context: dict[str, Any], event: dict[str, Any]) -> None:
    del repo_root
    _append_text(Path(run_context["eventsPath"]), json.dumps(event, ensure_ascii=False) + "\n")


def _build_traffic_light_counts(case_results: list[dict[str, Any]]) -> dict[str, dict[str, int]]:
    counts = _empty_traffic_light_counts()
    for case_result in case_results:
        bucket = str(case_result.get("trafficLight", "green"))
        bucket_counts = counts.get(bucket)
        if bucket_counts is None:
            continue
        _add_status_count(bucket_counts, str(case_result.get("status", "aborted")))
    return counts


def _merge_status_counts(target: dict[str, int], source: dict[str, int]) -> None:
    for key in ("total", "ok", "fail", "skip", "aborted"):
        target[key] += int(source.get(key, 0))


def _merge_traffic_light_counts(
    target: dict[str, dict[str, int]],
    source: dict[str, dict[str, int]],
) -> None:
    for bucket in TRAFFIC_LIGHT_BUCKETS:
        _merge_status_counts(target[bucket], source.get(bucket, {}))


def _summarize_suite_result(suite_result: dict[str, Any], report_path: Path, repo_root: Path) -> dict[str, Any]:
    case_results = list(suite_result.get("caseResults", []))
    traffic_light_counts = dict(suite_result.get("trafficLightCounts") or _build_traffic_light_counts(case_results))
    case_counts = dict(suite_result.get("caseCounts") or _build_case_counts(case_results))
    return {
        "suiteId": suite_result["suiteId"],
        "status": suite_result["status"],
        "stageResults": dict(suite_result.get("stageResults", {})),
        "artifacts": list(suite_result.get("artifacts", [])),
        "caseCounts": case_counts,
        "trafficLightCounts": traffic_light_counts,
        "reportPath": _relative_path(repo_root, report_path),
    }


def _summarize_subject_results(subject_results: list[dict[str, Any]]) -> dict[str, int]:
    counts = _empty_status_counts()
    for subject_result in subject_results:
        _add_status_count(counts, str(subject_result.get("status", "aborted")))
    return counts


def _resolve_final_status(status: str, subject_results: list[dict[str, Any]]) -> str:
    if status != "ok":
        return "fail"
    if any(str(subject_result.get("status") or "aborted") != "ok" for subject_result in subject_results):
        return "fail"
    return "ok"


def _normalize_nested_status_counts(payload: dict[str, Any] | None) -> dict[str, dict[str, int]]:
    normalized: dict[str, dict[str, int]] = {}
    for key, value in dict(payload or {}).items():
        normalized[str(key)] = {
            "total": int(dict(value or {}).get("total", 0)),
            "ok": int(dict(value or {}).get("ok", 0)),
            "fail": int(dict(value or {}).get("fail", 0)),
            "skip": int(dict(value or {}).get("skip", 0)),
            "aborted": int(dict(value or {}).get("aborted", 0)),
        }
    return normalized


def _enrich_failure_items(
    failure_items: list[dict[str, Any]],
    *,
    suite_results: list[dict[str, Any]],
    subject_results: list[dict[str, Any]],
) -> list[dict[str, Any]]:
    suite_report_paths = {
        str(suite_result.get("suiteId") or ""): str(suite_result.get("reportPath") or "")
        for suite_result in suite_results
        if str(suite_result.get("reportPath") or "")
    }
    subject_summary_paths: dict[str, str] = {}
    for subject_result in subject_results:
        subject_id = str(subject_result.get("subjectId") or "")
        summary_path = str(subject_result.get("subjectSummaryPath") or "")
        if not subject_id or not summary_path:
            continue
        subject_summary_paths[subject_id] = summary_path
        subject_summary_paths[f"subject/{subject_id}"] = summary_path

    normalized_items: list[dict[str, Any]] = []
    for failure_item in failure_items:
        normalized_item = dict(failure_item)
        item_id = str(normalized_item.get("id") or "")
        item_type = str(normalized_item.get("type") or "")
        if item_type == "suite" and not normalized_item.get("reportPath"):
            report_path = suite_report_paths.get(item_id)
            if report_path:
                normalized_item["reportPath"] = report_path
        if item_type == "subject" and not normalized_item.get("subjectSummaryPath"):
            subject_summary_path = subject_summary_paths.get(item_id)
            if subject_summary_path:
                normalized_item["subjectSummaryPath"] = subject_summary_path
        normalized_items.append(normalized_item)
    return normalized_items


def write_session_report(
    *,
    repo_root: Path,
    host_platform: str,
    command_text: str,
    status: str,
    suite_results: list[dict[str, Any]],
    phase_results: list[dict[str, Any]] | None = None,
    text: str,
    errors: list[str],
    artifacts: list[Any],
    subject_results: list[dict[str, Any]] | None = None,
    planned_counts: dict[str, int] | None = None,
    counts_by_type: dict[str, dict[str, int]] | None = None,
    counts_by_family: dict[str, dict[str, int]] | None = None,
    failure_items: list[dict[str, Any]] | None = None,
    run_context: dict[str, Any] | None = None,
) -> dict[str, str]:
    if run_context is None:
        run_context = start_session_report(repo_root=repo_root, host_platform=host_platform, command_text=command_text)

    run_id = str(run_context["runId"])
    session_root = Path(run_context["sessionRoot"])
    session_path = Path(run_context["sessionPath"])
    summary_path = Path(run_context["summaryPath"])
    events_path = Path(run_context["eventsPath"])
    console_path = Path(run_context["consolePath"])
    telemetry_path = Path(run_context["telemetryPath"])
    pointer_roots = [Path(path) for path in list(run_context.get("pointerRoots") or [])]
    if not pointer_roots:
        pointer_roots = [_logs_root(repo_root)]

    normalized_subject_results = list(subject_results or [])
    normalized_planned_counts = {
        "totalUnits": int(dict(planned_counts or {}).get("totalUnits", 0)),
        "suiteCount": int(dict(planned_counts or {}).get("suiteCount", 0)),
        "subjectCount": int(dict(planned_counts or {}).get("subjectCount", 0)),
    }
    normalized_counts_by_type = _normalize_nested_status_counts(counts_by_type)
    normalized_counts_by_family = _normalize_nested_status_counts(counts_by_family)
    final_status = _resolve_final_status(status, normalized_subject_results)
    exit_code = 0 if final_status == "ok" else 1
    summary_suite_results: list[dict[str, Any]] = []
    session_case_counts = _empty_status_counts()
    session_traffic_light_counts = _empty_traffic_light_counts()
    session_payload = {
        "runId": run_id,
        "command": command_text,
        "hostPlatform": host_platform,
        "status": status,
        "errors": list(errors),
    }

    write_json(session_path, session_payload)
    _write_text(console_path, text or "")
    write_json(
        telemetry_path,
        {
            "runId": run_id,
            "suiteCount": len(suite_results),
            "subjectCount": len(normalized_subject_results),
            "artifactCount": len(artifacts),
            "errorCount": len(errors),
        },
    )

    for suite_result in suite_results:
        suite_root = session_root / "suites" / suite_result["suiteId"]
        report_path = suite_root / "report.json"
        stdout_path = suite_root / "stdout.log"
        stderr_path = suite_root / "stderr.log"

        suite_report = dict(suite_result)
        suite_report["reportPath"] = _relative_path(repo_root, report_path)
        suite_case_results = list(suite_report.get("caseResults", []))
        suite_report["caseCounts"] = dict(suite_report.get("caseCounts") or _build_case_counts(suite_case_results))
        suite_report["trafficLightCounts"] = dict(
            suite_report.get("trafficLightCounts") or _build_traffic_light_counts(suite_case_results)
        )
        write_json(report_path, suite_report)
        summary_suite = _summarize_suite_result(suite_report, report_path, repo_root)
        summary_suite_results.append(summary_suite)
        _merge_status_counts(session_case_counts, summary_suite["caseCounts"])
        _merge_traffic_light_counts(session_traffic_light_counts, summary_suite["trafficLightCounts"])
        _write_text(stdout_path, _strip_ansi(text or ""))
        _write_text(stderr_path, "")

    normalized_failure_items = _enrich_failure_items(
        [dict(item) for item in list(failure_items or [])],
        suite_results=summary_suite_results,
        subject_results=normalized_subject_results,
    )

    summary_payload = {
        "runId": run_id,
        "command": command_text,
        "hostPlatform": host_platform,
        "finalStatus": final_status,
        "exitCode": exit_code,
        "errors": list(errors),
        "artifacts": list(artifacts),
        "caseCounts": session_case_counts,
        "trafficLightCounts": session_traffic_light_counts,
        "phaseResults": list(phase_results or []),
        "plannedCounts": normalized_planned_counts,
        "countsByType": normalized_counts_by_type,
        "countsByFamily": normalized_counts_by_family,
        "failureItems": normalized_failure_items,
        "suiteResults": summary_suite_results,
        "subjectStatusCounts": _summarize_subject_results(normalized_subject_results),
        "subjectResults": normalized_subject_results,
    }
    write_json(summary_path, summary_payload)

    final_event_payload = {
        "runId": run_id,
        "finalStatus": final_status,
        "exitCode": exit_code,
        "errors": list(errors),
        "artifacts": list(artifacts),
        "caseCounts": session_case_counts,
        "trafficLightCounts": session_traffic_light_counts,
        "phaseResults": list(phase_results or []),
        "plannedCounts": normalized_planned_counts,
        "countsByType": normalized_counts_by_type,
        "countsByFamily": normalized_counts_by_family,
        "failureItems": normalized_failure_items,
        "subjectStatusCounts": _summarize_subject_results(normalized_subject_results),
        "subjectResults": normalized_subject_results,
        "sessionPath": _relative_path(repo_root, session_path),
        "summaryPath": _relative_path(repo_root, summary_path),
        "eventsPath": _relative_path(repo_root, events_path),
        "consolePath": _relative_path(repo_root, console_path),
        "telemetryPath": _relative_path(repo_root, telemetry_path),
    }
    append_session_event(
        repo_root,
        run_context,
        build_event(
            "final-summary",
            final_event_payload,
            run_id=run_id,
            status=final_status,
        ),
    )
    pointer_payload = {
        "runId": run_id,
        "command": command_text,
        "hostPlatform": host_platform,
        "sessionPath": _relative_path(repo_root, session_path),
        "summaryPath": _relative_path(repo_root, summary_path),
        "eventsPath": _relative_path(repo_root, events_path),
        "status": final_status,
    }
    _write_pointer_payloads(pointer_roots, "last.json", pointer_payload)
    for pointer_root in pointer_roots:
        _finalize_current_pointer(pointer_root / "current.json", run_id, pointer_payload)

    return {
        "runId": run_id,
        "sessionPath": _relative_path(repo_root, session_path),
        "summaryPath": _relative_path(repo_root, summary_path),
        "eventsPath": _relative_path(repo_root, events_path),
        "consolePath": _relative_path(repo_root, console_path),
        "telemetryPath": _relative_path(repo_root, telemetry_path),
    }
