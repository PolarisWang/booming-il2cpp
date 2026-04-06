from __future__ import annotations

from pathlib import Path
from typing import Any, Callable
import sys

try:
    from ..common import write_json
    from . import subject_planner as planner_module
    from . import subject_workers as subject_workers_module
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from common import write_json
    from testing import subject_planner as planner_module
    from testing import subject_workers as subject_workers_module


Worker = Callable[..., dict[str, Any]]


def _stage_upstream(stage: dict[str, Any], stages_by_id: dict[str, dict[str, Any]]) -> dict[str, dict[str, str]]:
    upstream: dict[str, dict[str, str]] = {}
    for dependency_id in list(stage.get("dependsOn") or []):
        dependency = stages_by_id[str(dependency_id)]
        upstream[str(dependency["bucket"])] = {"manifestPath": str(dependency["paths"]["manifestPath"])}
    return upstream


def _stage_result(
    stage: dict[str, Any],
    *,
    status: str,
    action_taken: str,
    manifest_path: str,
    report_paths: list[str],
    duration_ms: int = 0,
    failure: str | None = None,
) -> dict[str, Any]:
    invalidated = str(stage["executionMode"]) == "invalidated"
    return {
        "stageId": str(stage["stageId"]),
        "kind": str(stage["kind"]),
        "bucket": str(stage["bucket"]),
        "status": status,
        "planMode": str(stage["executionMode"]),
        "actionTaken": action_taken,
        "invalidation": {
            "applied": invalidated,
            "reason": str(stage["reuse"]["reason"]) if invalidated else None,
        },
        "manifestPath": manifest_path,
        "reportPaths": report_paths,
        "fingerprint": str(stage["fingerprint"]),
        "durationMs": duration_ms,
        "failure": failure,
    }


def _terminal_stage_id(stage_results: list[dict[str, Any]], terminal_bucket: str) -> str:
    for stage_result in reversed(stage_results):
        if stage_result["bucket"] == terminal_bucket:
            return str(stage_result["stageId"])
    return str(stage_results[-1]["stageId"]) if stage_results else ""


def execute_plan(
    repo_root: Path,
    plan: dict[str, Any],
    *,
    worker_registry: dict[str, Worker] | None = None,
) -> dict[str, Any]:
    registry = dict(subject_workers_module.DEFAULT_STAGE_WORKERS)
    if worker_registry:
        registry.update(worker_registry)

    selection = dict(plan["selection"])
    stages_by_id = {str(stage["stageId"]): dict(stage) for stage in list(plan.get("stagePlan") or [])}
    stage_results: list[dict[str, Any]] = []
    errors: list[str] = []
    final_status = "ok"

    for stage in list(plan.get("stagePlan") or []):
        stage_id = str(stage["stageId"])
        if str(stage["kind"]) == "report-assemble":
            report_payload = {
                "subjectId": str(selection["subjectId"]),
                "matrixId": str(selection["matrixId"]),
                "goalId": str(selection["goalId"]),
                "status": final_status,
                "terminalBucket": str(selection["artifactPlan"]["evidenceTerminalBucket"]),
                "stageResults": list(stage_results),
                "errors": list(errors),
            }
            write_json(repo_root / str(stage["paths"]["manifestPath"]), report_payload)
            stage_results.append(
                _stage_result(
                    stage,
                    status=final_status,
                    action_taken="reused" if str(stage["executionMode"]) == "reused" else "executed",
                    manifest_path=str(stage["paths"]["manifestPath"]),
                    report_paths=list(stage["paths"]["reportPaths"]),
                )
            )
            continue

        if str(stage["executionMode"]) == "reused":
            stage_results.append(
                _stage_result(
                    stage,
                    status="ok",
                    action_taken="reused",
                    manifest_path=str(stage["paths"]["manifestPath"]),
                    report_paths=list(stage["paths"]["reportPaths"]),
                )
            )
            continue

        worker = registry.get(str(stage["kind"]))
        if worker is None:
            final_status = "fail"
            message = f"worker not found for stage kind: {stage['kind']}"
            errors.append(message)
            stage_results.append(
                _stage_result(
                    stage,
                    status="fail",
                    action_taken="executed",
                    manifest_path=str(stage["paths"]["manifestPath"]),
                    report_paths=list(stage["paths"]["reportPaths"]),
                    failure=message,
                )
            )
            break

        request = {
            "selection": selection,
            "stage": {
                "stageId": stage_id,
                "kind": str(stage["kind"]),
                "scope": str(stage["scope"]),
                "bucket": str(stage["bucket"]),
                "dependsOn": list(stage["dependsOn"]),
                "planMode": str(stage["executionMode"]),
                "requestedFingerprint": str(stage["fingerprint"]),
                "upstreamFingerprints": dict(stage["upstreamFingerprints"]),
                "reuse": dict(stage["reuse"]),
            },
            "paths": dict(stage["paths"]),
            "upstream": _stage_upstream(stage, stages_by_id),
        }

        try:
            worker_result = worker(repo_root=repo_root, request=request)
        except Exception as exception:
            final_status = "fail"
            message = str(exception)
            errors.append(message)
            stage_results.append(
                _stage_result(
                    stage,
                    status="fail",
                    action_taken="executed",
                    manifest_path=str(stage["paths"]["manifestPath"]),
                    report_paths=list(stage["paths"]["reportPaths"]),
                    failure=message,
                )
            )
            break

        if str(worker_result.get("status")) != "ok":
            final_status = "fail"
            errors.append(str(worker_result.get("failure") or f"stage failed: {stage_id}"))

        stage_results.append(
            _stage_result(
                stage,
                status=str(worker_result.get("status") or "fail"),
                action_taken="executed",
                manifest_path=str(worker_result.get("bucketManifestPath") or stage["paths"]["manifestPath"]),
                report_paths=list(worker_result.get("reportPaths") or stage["paths"]["reportPaths"]),
                duration_ms=int(dict(worker_result.get("metrics") or {}).get("durationMs") or 0),
                failure=worker_result.get("failure"),
            )
        )
        if final_status != "ok":
            break

    terminal_bucket = str(selection["artifactPlan"]["evidenceTerminalBucket"])
    return {
        "subjectId": str(selection["subjectId"]),
        "matrixId": str(selection["matrixId"]),
        "goalId": str(selection["goalId"]),
        "status": final_status,
        "terminalStageId": _terminal_stage_id(stage_results, terminal_bucket),
        "terminalBucket": terminal_bucket,
        "stageResults": stage_results,
        "errors": errors,
    }


def execute_subject_matrix(
    repo_root: Path,
    subject_id: str,
    *,
    goal_id: str | None = None,
    matrix_id: str | None = None,
) -> dict[str, Any]:
    plan = planner_module.build_plan(repo_root, subject_id, goal_id=goal_id, matrix_id=matrix_id)
    return execute_plan(repo_root, plan)
