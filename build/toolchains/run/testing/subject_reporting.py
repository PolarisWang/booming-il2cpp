from __future__ import annotations

import json
from pathlib import Path
from typing import Any


def _copy_execution_context(selection: dict[str, Any]) -> dict[str, Any]:
    execution_context = dict(selection.get("executionContext") or {})
    return {
        "hostPlatform": execution_context.get("hostPlatform"),
        "targetPlatform": execution_context.get("targetPlatform"),
        "toolchainProfile": execution_context.get("toolchainProfile"),
        "runtimeProfile": execution_context.get("runtimeProfile"),
    }


def _normalize_stage_result(stage_result: dict[str, Any]) -> dict[str, Any]:
    performance = dict(dict(stage_result.get("details") or {}).get("performance") or {})
    details: dict[str, Any] = {}
    if performance:
        details["performance"] = {
            "samples": list(performance.get("samples") or []),
            "metrics": dict(performance.get("metrics") or {}),
            "baselinePath": performance.get("baselinePath"),
            "baseline": dict(performance.get("baseline") or {}),
            "baselineUpdated": bool(performance.get("baselineUpdated", False)),
            "regressionStatus": str(performance.get("regressionStatus") or "no-baseline"),
            "regressions": list(performance.get("regressions") or []),
        }
    return {
        "stageId": str(stage_result.get("stageId") or ""),
        "kind": str(stage_result.get("kind") or ""),
        "bucket": str(stage_result.get("bucket") or ""),
        "status": str(stage_result.get("status") or "aborted"),
        "planMode": str(stage_result.get("planMode") or "executed"),
        "actionTaken": str(stage_result.get("actionTaken") or "executed"),
        "invalidation": {
            "applied": bool(dict(stage_result.get("invalidation") or {}).get("applied", False)),
            "reason": dict(stage_result.get("invalidation") or {}).get("reason"),
        },
        "fingerprint": stage_result.get("fingerprint"),
        "durationMs": int(stage_result.get("durationMs") or 0),
        "manifestPath": str(stage_result.get("manifestPath") or ""),
        "reportPaths": list(stage_result.get("reportPaths") or []),
        "primaryEvidencePaths": list(stage_result.get("primaryEvidencePaths") or []),
        "diagnostics": {
            "stdoutPath": dict(stage_result.get("diagnostics") or {}).get("stdoutPath"),
            "stderrPath": dict(stage_result.get("diagnostics") or {}).get("stderrPath"),
        },
        "details": details,
        "failure": stage_result.get("failure"),
    }


def _artifact_results(
    stage_results: list[dict[str, Any]],
    stage_scope_by_id: dict[str, str],
) -> list[dict[str, Any]]:
    ordered_paths: list[str] = []
    artifacts_by_path: dict[str, dict[str, Any]] = {}

    for stage_result in stage_results:
        manifest_path = str(stage_result.get("manifestPath") or "")
        bucket = str(stage_result.get("bucket") or "")
        if not manifest_path or bucket == "report":
            continue
        if manifest_path not in artifacts_by_path:
            ordered_paths.append(manifest_path)
        artifacts_by_path[manifest_path] = {
            "bucket": bucket,
            "scope": stage_scope_by_id.get(str(stage_result.get("stageId") or ""), "matrix"),
            "manifestPath": manifest_path,
            "producedByStageId": str(stage_result.get("stageId") or ""),
        }

    return [artifacts_by_path[path] for path in ordered_paths]


def build_matrix_report(
    plan: dict[str, Any],
    execution_result: dict[str, Any],
    *,
    run_id: str,
    generated_at: str,
) -> dict[str, Any]:
    selection = dict(plan.get("selection") or {})
    stage_plan = list(plan.get("stagePlan") or [])
    stage_scope_by_id = {
        str(stage.get("stageId") or ""): str(stage.get("scope") or "matrix")
        for stage in stage_plan
    }
    normalized_stage_results = [
        _normalize_stage_result(stage_result)
        for stage_result in list(execution_result.get("stageResults") or [])
        if str(stage_result.get("bucket") or "") != "report"
    ]
    performance = next(
        (
            dict(dict(stage_result.get("details") or {}).get("performance") or {})
            for stage_result in normalized_stage_results
            if dict(stage_result.get("details") or {}).get("performance")
        ),
        {},
    )

    report = {
        "reportVersion": "v1",
        "runId": run_id,
        "generatedAt": generated_at,
        "subjectId": str(execution_result.get("subjectId") or selection.get("subjectId") or ""),
        "matrixId": str(execution_result.get("matrixId") or selection.get("matrixId") or ""),
        "goalId": str(execution_result.get("goalId") or selection.get("goalId") or ""),
        "selection": selection,
        "status": str(execution_result.get("status") or "aborted"),
        "terminalStageId": str(execution_result.get("terminalStageId") or ""),
        "terminalBucket": str(execution_result.get("terminalBucket") or ""),
        "stageResults": normalized_stage_results,
        "artifactResults": _artifact_results(normalized_stage_results, stage_scope_by_id),
        "errors": list(execution_result.get("errors") or []),
    }
    if performance:
        report["performance"] = performance
        report["metrics"] = dict(performance.get("metrics") or {})
        report["baseline"] = {
            "path": performance.get("baselinePath"),
            "metrics": dict(performance.get("baseline") or {}),
            "updated": bool(performance.get("baselineUpdated", False)),
            "regressions": list(performance.get("regressions") or []),
        }
        report["regressionStatus"] = str(performance.get("regressionStatus") or "no-baseline")
        report["releaseReportPaths"] = []
    return report


def _empty_status_counts() -> dict[str, int]:
    return {
        "total": 0,
        "ok": 0,
        "fail": 0,
        "skip": 0,
        "aborted": 0,
    }


def _status_key(status: str) -> str:
    return status if status in {"ok", "fail", "skip", "aborted"} else "aborted"


def _aggregate_status(statuses: list[str]) -> str:
    if not statuses:
        return "aborted"
    if any(status == "fail" for status in statuses):
        return "fail"
    if any(status == "aborted" for status in statuses):
        return "aborted"
    if all(status == "skip" for status in statuses):
        return "skip"
    return "ok"


def build_subject_summary(
    *,
    subject_id: str,
    requested_goal_id: str,
    matrix_reports: list[dict[str, Any]],
    matrix_report_paths: dict[str, str],
    run_id: str,
    generated_at: str,
) -> dict[str, Any]:
    counts = _empty_status_counts()
    matrix_results: list[dict[str, Any]] = []
    statuses: list[str] = []

    for report in matrix_reports:
        matrix_id = str(report.get("matrixId") or "")
        status = _status_key(str(report.get("status") or "aborted"))
        counts["total"] += 1
        counts[status] += 1
        statuses.append(status)

        execution_context = _copy_execution_context(dict(report.get("selection") or {}))
        matrix_results.append(
            {
                "matrixId": matrix_id,
                "goalId": str(report.get("goalId") or requested_goal_id),
                "status": status,
                "terminalBucket": str(report.get("terminalBucket") or ""),
                "executionContext": execution_context,
                "reportPath": str(matrix_report_paths[matrix_id]),
                "metrics": dict(report.get("metrics") or {}),
                "regressionStatus": report.get("regressionStatus"),
                "releaseReportPaths": list(report.get("releaseReportPaths") or []),
            }
        )

    return {
        "summaryVersion": "v1",
        "runId": run_id,
        "generatedAt": generated_at,
        "subjectId": subject_id,
        "requestedGoalId": requested_goal_id,
        "status": _aggregate_status(statuses),
        "matrixStatusCounts": counts,
        "matrixResults": matrix_results,
    }


def build_subject_result(
    subject_summary: dict[str, Any],
    *,
    subject_summary_path: str,
) -> dict[str, Any]:
    return {
        "subjectId": str(subject_summary.get("subjectId") or ""),
        "requestedGoalId": str(subject_summary.get("requestedGoalId") or ""),
        "status": str(subject_summary.get("status") or "aborted"),
        "matrixStatusCounts": dict(subject_summary.get("matrixStatusCounts") or _empty_status_counts()),
        "subjectSummaryPath": subject_summary_path,
    }


def _write_json_document(path: Path, payload: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def materialize_matrix_report_artifacts(
    repo_root: Path,
    *,
    matrix_report_path: str,
    matrix_report: dict[str, Any],
) -> list[str]:
    performance = dict(matrix_report.get("performance") or {})
    if str(matrix_report.get("goalId") or "") != "perf.release" or not performance:
        matrix_report["releaseReportPaths"] = list(matrix_report.get("releaseReportPaths") or [])
        return list(matrix_report["releaseReportPaths"])

    report_root = Path(matrix_report_path).parent / "report"
    summary_path = report_root / "summary.json"
    baseline_compare_path = report_root / "baseline-compare.json"
    samples_path = report_root / "samples.json"

    summary_payload = {
        "reportVersion": "v1",
        "subjectId": matrix_report.get("subjectId"),
        "matrixId": matrix_report.get("matrixId"),
        "goalId": matrix_report.get("goalId"),
        "status": matrix_report.get("status"),
        "metrics": dict(matrix_report.get("metrics") or {}),
        "regressionStatus": matrix_report.get("regressionStatus"),
    }
    baseline_compare_payload = {
        "reportVersion": "v1",
        "subjectId": matrix_report.get("subjectId"),
        "matrixId": matrix_report.get("matrixId"),
        "goalId": matrix_report.get("goalId"),
        "metrics": dict(matrix_report.get("metrics") or {}),
        "baseline": dict(matrix_report.get("baseline") or {}),
        "regressionStatus": matrix_report.get("regressionStatus"),
    }
    samples_payload = {
        "reportVersion": "v1",
        "subjectId": matrix_report.get("subjectId"),
        "matrixId": matrix_report.get("matrixId"),
        "goalId": matrix_report.get("goalId"),
        "samples": list(performance.get("samples") or []),
    }

    _write_json_document(repo_root / summary_path, summary_payload)
    _write_json_document(repo_root / baseline_compare_path, baseline_compare_payload)
    _write_json_document(repo_root / samples_path, samples_payload)

    release_report_paths = [
        summary_path.as_posix(),
        baseline_compare_path.as_posix(),
        samples_path.as_posix(),
    ]
    matrix_report["releaseReportPaths"] = release_report_paths
    return release_report_paths
