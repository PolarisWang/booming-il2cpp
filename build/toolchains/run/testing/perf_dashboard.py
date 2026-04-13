from __future__ import annotations

from datetime import datetime, timezone
from pathlib import Path
from typing import Any
import sys

try:
    from ..core.common import read_json, write_json
    from . import path_resolver as path_resolver_module
    from . import subjects as subjects_module
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from core.common import read_json, write_json
    from testing import path_resolver as path_resolver_module
    from testing import subjects as subjects_module


def _utc_timestamp() -> str:
    return datetime.now(timezone.utc).replace(microsecond=0).isoformat().replace("+00:00", "Z")


def _relative(repo_root: Path, path: Path) -> str:
    return path.resolve().relative_to(repo_root.resolve()).as_posix()


def _normalize_host_platform(host_platform: str) -> str:
    if host_platform.startswith("windows"):
        return "windows"
    if host_platform.startswith("macos"):
        return "macos"
    if host_platform.startswith("linux"):
        return "linux"
    return host_platform


def _numeric_metric_keys(payload: dict[str, Any]) -> list[str]:
    return sorted(
        key
        for key, value in payload.items()
        if isinstance(key, str) and isinstance(value, (int, float)) and not isinstance(value, bool)
    )


def _status_counts(entries: list[dict[str, Any]]) -> dict[str, int]:
    fail_count = sum(1 for entry in entries if str(entry.get("status") or "fail") != "ok")
    ok_count = len(entries) - fail_count
    return {
        "total": len(entries),
        "ok": ok_count,
        "fail": fail_count,
    }


def build_perf_dashboard_config(repo_root: Path) -> dict[str, Any]:
    entries: list[dict[str, Any]] = []
    for record in subjects_module.discover_perf_subject_records(repo_root):
        subject_id = str(record["subjectId"])
        manifest = dict(record["manifest"])
        for matrix in list(manifest.get("environmentMatrices") or []):
            matrix_payload = dict(matrix)
            matrix_id = str(matrix_payload.get("matrixId") or "")
            execution_context = dict(matrix_payload.get("executionContext") or {})
            supported_goals = [
                str(goal_id)
                for goal_id in list(matrix_payload.get("supportedGoals") or [])
                if str(goal_id).startswith("perf.")
            ]
            if not matrix_id or not supported_goals:
                continue

            host_platform = str(execution_context.get("hostPlatform") or "")
            host_baseline_label = _normalize_host_platform(host_platform)
            baseline_path = path_resolver_module.subject_perf_baseline_path(
                repo_root,
                subject_id,
                matrix_id,
                host_baseline_label,
            )
            baseline_payload = (
                read_json(baseline_path)
                if baseline_path.is_file()
                else {}
            )
            if not isinstance(baseline_payload, dict):
                baseline_payload = {}

            metric_keys = _numeric_metric_keys(baseline_payload)
            baseline_path_text = _relative(repo_root, baseline_path) if baseline_path.is_file() else None

            if not baseline_path_text or not metric_keys:
                continue

            for goal_id in supported_goals:
                entries.append(
                    {
                        "subjectId": subject_id,
                        "matrixId": matrix_id,
                        "goalId": goal_id,
                        "hostPlatform": host_platform,
                        "targetPlatform": str(execution_context.get("targetPlatform") or ""),
                        "toolchainProfile": str(execution_context.get("toolchainProfile") or ""),
                        "runtimeProfile": str(execution_context.get("runtimeProfile") or ""),
                        "baselinePath": baseline_path_text,
                        "metricKeys": metric_keys,
                        "status": "ok",
                    }
                )

    counts = _status_counts(entries)
    return {
        "configVersion": "v1",
        "generatedAt": _utc_timestamp(),
        "status": "ok" if counts["fail"] == 0 else "fail",
        "statusCounts": counts,
        "subjectIds": sorted({str(entry["subjectId"]) for entry in entries}),
        "goalIds": sorted({str(entry["goalId"]) for entry in entries}),
        "hostPlatforms": sorted({str(entry["hostPlatform"]) for entry in entries}),
        "targetPlatforms": sorted({str(entry["targetPlatform"]) for entry in entries}),
        "entries": entries,
    }


def write_perf_dashboard_config(path: Path, payload: dict[str, Any]) -> None:
    write_json(path, payload)
