from __future__ import annotations

import json
import shutil
from pathlib import Path
from typing import Any, Callable
import sys

try:
    from . import events as events_module
    from . import subject_planner as planner_module
    from . import subject_reporting as subject_reporting_module
    from . import subject_workers as subject_workers_module
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from testing import events as events_module
    from testing import subject_planner as planner_module
    from testing import subject_reporting as subject_reporting_module
    from testing import subject_workers as subject_workers_module


Worker = Callable[..., dict[str, Any]]


def _copy_reused_bucket(repo_root: Path, stage: dict[str, Any]) -> None:
    existing_manifest_path = str(dict(stage.get("reuse") or {}).get("existingManifestPath") or "")
    if not existing_manifest_path:
        return

    source_bucket_root = (repo_root / existing_manifest_path).parent
    destination_bucket_root = repo_root / str(stage["paths"]["bucketRoot"])
    if not source_bucket_root.is_dir():
        return

    destination_bucket_root.parent.mkdir(parents=True, exist_ok=True)
    shutil.copytree(source_bucket_root, destination_bucket_root, dirs_exist_ok=True)


def _read_reused_manifest(repo_root: Path, stage: dict[str, Any]) -> dict[str, Any]:
    manifest_path = repo_root / str(stage["paths"]["manifestPath"])
    if not manifest_path.is_file():
        return {}
    try:
        payload = json.loads(manifest_path.read_text(encoding="utf-8"))
    except (OSError, ValueError):
        return {}
    return payload if isinstance(payload, dict) else {}


def _dedupe_non_empty(values: list[str]) -> list[str]:
    unique_values: list[str] = []
    for value in values:
        if value and value not in unique_values:
            unique_values.append(value)
    return unique_values


def _rewrite_reused_bucket_path(stage: dict[str, Any], value: str) -> str:
    if not value:
        return ""

    source_bucket_root = Path(str(dict(stage.get("reuse") or {}).get("existingManifestPath") or "")).parent.as_posix()
    destination_bucket_root = str(stage["paths"]["bucketRoot"])
    if value == source_bucket_root:
        return destination_bucket_root
    if value.startswith(source_bucket_root + "/"):
        return destination_bucket_root + value[len(source_bucket_root) :]
    return value


def _reused_primary_evidence_paths(stage: dict[str, Any], manifest: dict[str, Any]) -> list[str]:
    kind = str(stage.get("kind") or "")
    bucket = str(stage.get("bucket") or "")
    artifacts = dict(manifest.get("artifacts") or {})
    outputs = [str(value) for value in list(manifest.get("outputs") or []) if value]
    trace_paths = [str(value) for value in list(manifest.get("tracePaths") or []) if value]

    if kind == "source-resolve":
        return _dedupe_non_empty([_rewrite_reused_bucket_path(stage, str(manifest.get("sourcePath") or ""))])
    if kind == "host-input-build":
        return _dedupe_non_empty([_rewrite_reused_bucket_path(stage, str(manifest.get("primaryAssemblyPath") or ""))])
    if kind == "analysis-frontend":
        return _dedupe_non_empty(
            [
                _rewrite_reused_bucket_path(stage, str(artifacts.get("typedIlIrPath") or "")),
                _rewrite_reused_bucket_path(stage, str(artifacts.get("optimizationFactsPath") or "")),
                _rewrite_reused_bucket_path(stage, str(artifacts.get("closureManifestPath") or "")),
            ]
        )
    if kind in {"generated-native-proof", "generated-engine-proof", "generated-native-aot"}:
        native_reference_manifest_path = str(
            manifest.get("nativeReferenceManifestPath")
            or manifest.get("nativeAotManifestPath")
            or manifest.get("nativeProofManifestPath")
            or ""
        )
        return _dedupe_non_empty(
            [
                _rewrite_reused_bucket_path(stage, str(manifest.get("generatedSourcePath") or "")),
                _rewrite_reused_bucket_path(stage, native_reference_manifest_path),
            ]
        )
    if kind == "build-target":
        return _dedupe_non_empty([_rewrite_reused_bucket_path(stage, value) for value in outputs])
    if kind in {"runtime-engine-observe", "runtime-engine-trace-compare"}:
        engine_evidence_paths = [str(value) for value in list(manifest.get("engineEvidencePaths") or []) if value]
        return _dedupe_non_empty([_rewrite_reused_bucket_path(stage, value) for value in engine_evidence_paths])
    if kind in {"runtime-observe", "runtime-managed-output", "runtime-perf-collect"}:
        return _dedupe_non_empty(
            [
                _rewrite_reused_bucket_path(stage, str(manifest.get("stdoutPath") or "")),
                _rewrite_reused_bucket_path(stage, str(manifest.get("exitCodePath") or "")),
                *[_rewrite_reused_bucket_path(stage, value) for value in trace_paths],
            ]
        )
    if kind in {"native-runtime-perf", "mobile-native-perf"}:
        collector_evidence_paths = [str(value) for value in list(manifest.get("collectorEvidencePaths") or []) if value]
        return _dedupe_non_empty(
            [
                _rewrite_reused_bucket_path(stage, str(manifest.get("perfRuntimePath") or "")),
                _rewrite_reused_bucket_path(stage, str(manifest.get("perfSamplesPath") or "")),
                *[_rewrite_reused_bucket_path(stage, value) for value in collector_evidence_paths],
            ]
        )
    if kind == "runtime-trace-compare" or bucket == "runtime":
        return _dedupe_non_empty([_rewrite_reused_bucket_path(stage, value) for value in trace_paths])
    return []


def _reused_stage_diagnostics(stage: dict[str, Any], manifest: dict[str, Any]) -> dict[str, Any]:
    return {
        "stdoutPath": _rewrite_reused_bucket_path(stage, str(manifest.get("stdoutPath") or "")) or None,
        "stderrPath": _rewrite_reused_bucket_path(stage, str(manifest.get("stderrPath") or "")) or None,
    }


def _reused_stage_details(stage: dict[str, Any], manifest: dict[str, Any]) -> dict[str, Any]:
    details: dict[str, Any] = {}
    for detail_name in (
        "engineContractSummary",
        "engineEmissionSummary",
        "engineObservationSummary",
    ):
        detail = dict(manifest.get(detail_name) or {})
        if detail:
            details[detail_name] = detail

    if str(stage.get("kind") or "") not in {"runtime-perf-collect", "native-runtime-perf", "mobile-native-perf"}:
        return details

    details["performance"] = {
        "samples": list(manifest.get("samples") or []),
        "metrics": dict(manifest.get("summaryMetrics") or {}),
        "baselinePath": manifest.get("baselinePath"),
        "baseline": dict(manifest.get("baseline") or {}),
        "baselineUpdated": bool(manifest.get("baselineUpdated", False)),
        "regressionStatus": str(manifest.get("regressionStatus") or "no-baseline"),
        "regressions": list(manifest.get("regressions") or []),
        "runtimeEvidence": {
            "runtimePath": manifest.get("perfRuntimePath"),
            "samplesPath": manifest.get("perfSamplesPath"),
        },
        "collectorDetails": dict(manifest.get("collectorDetails") or {}),
        "collectorEvidencePaths": list(manifest.get("collectorEvidencePaths") or []),
    }
    return details


def _reused_stage_metadata(repo_root: Path, stage: dict[str, Any]) -> dict[str, Any]:
    manifest = _read_reused_manifest(repo_root, stage)
    return {
        "primaryEvidencePaths": _reused_primary_evidence_paths(stage, manifest),
        "diagnostics": _reused_stage_diagnostics(stage, manifest),
        "details": _reused_stage_details(stage, manifest),
    }


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
    primary_evidence_paths: list[str] | None = None,
    diagnostics: dict[str, Any] | None = None,
    details: dict[str, Any] | None = None,
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
        "primaryEvidencePaths": list(primary_evidence_paths or []),
        "fingerprint": str(stage["fingerprint"]),
        "durationMs": duration_ms,
        "diagnostics": {
            "stdoutPath": dict(diagnostics or {}).get("stdoutPath"),
            "stderrPath": dict(diagnostics or {}).get("stderrPath"),
        },
        "details": dict(details or {}),
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
    run_id: str | None = None,
    event_writer: Callable[[dict[str, Any]], None] | None = None,
) -> dict[str, Any]:
    registry = dict(subject_workers_module.DEFAULT_STAGE_WORKERS)
    if worker_registry:
        registry.update(worker_registry)

    selection = dict(plan["selection"])
    stages_by_id = {str(stage["stageId"]): dict(stage) for stage in list(plan.get("stagePlan") or [])}
    stage_results: list[dict[str, Any]] = []
    errors: list[str] = []
    emitted_events: list[dict[str, Any]] = []
    final_status = "ok"
    execution_run_id = run_id or "subject-exec"

    def emit_event(
        event_type: str,
        payload: dict[str, Any] | None = None,
        *,
        stage: dict[str, Any] | None = None,
        status: str | None = None,
    ) -> None:
        event = events_module.build_event(
            event_type,
            payload,
            run_id=execution_run_id,
            status=status,
            stream_scope="matrix",
            subject_id=str(selection["subjectId"]),
            matrix_id=str(selection["matrixId"]),
            goal_id=str(selection["goalId"]),
            stage_id=str(stage["stageId"]) if stage is not None else None,
            bucket=str(stage["bucket"]) if stage is not None else None,
            stage_scope=str(stage["scope"]) if stage is not None else None,
        )
        emitted_events.append(event)
        if event_writer is not None:
            event_writer(event)

    for stage in list(plan.get("stagePlan") or []):
        stage_id = str(stage["stageId"])
        if str(stage["kind"]) == "report-assemble":
            report_payload = subject_reporting_module.build_matrix_report(
                plan,
                {
                    "subjectId": str(selection["subjectId"]),
                    "matrixId": str(selection["matrixId"]),
                    "goalId": str(selection["goalId"]),
                    "status": final_status,
                    "terminalStageId": _terminal_stage_id(
                        stage_results,
                        str(selection["artifactPlan"]["evidenceTerminalBucket"]),
                    ),
                    "terminalBucket": str(selection["artifactPlan"]["evidenceTerminalBucket"]),
                    "stageResults": list(stage_results),
                    "errors": list(errors),
                },
                run_id=execution_run_id,
                generated_at=events_module.utc_timestamp(),
            )
            report_artifacts = subject_reporting_module.materialize_matrix_report_artifacts(
                repo_root,
                matrix_report_path=str(stage["paths"]["manifestPath"]),
                matrix_report=report_payload,
            )
            report_path = repo_root / str(stage["paths"]["manifestPath"])
            report_path.parent.mkdir(parents=True, exist_ok=True)
            report_path.write_text(json.dumps(report_payload, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
            stage_results.append(
                _stage_result(
                    stage,
                    status=final_status,
                    action_taken="reused" if str(stage["executionMode"]) == "reused" else "executed",
                    manifest_path=str(stage["paths"]["manifestPath"]),
                    report_paths=list(stage["paths"]["reportPaths"]) + list(report_artifacts),
                )
            )
            emit_event(
                "matrix-summary",
                {
                    "status": final_status,
                    "terminalBucket": str(selection["artifactPlan"]["evidenceTerminalBucket"]),
                    "reportPath": str(stage["paths"]["manifestPath"]),
                },
                status=final_status,
            )
            continue

        if str(stage["executionMode"]) == "reused":
            _copy_reused_bucket(repo_root, stage)
            reused_metadata = _reused_stage_metadata(repo_root, stage)
            stage_results.append(
                _stage_result(
                    stage,
                    status="ok",
                    action_taken="reused",
                    manifest_path=str(stage["paths"]["manifestPath"]),
                    report_paths=list(stage["paths"]["reportPaths"]),
                    primary_evidence_paths=list(reused_metadata["primaryEvidencePaths"]),
                    diagnostics=dict(reused_metadata["diagnostics"]),
                    details=dict(reused_metadata["details"]),
                )
            )
            emit_event(
                "stage-reused",
                {
                    "manifestPath": str(stage["paths"]["manifestPath"]),
                    "reason": str(stage["reuse"]["reason"]),
                },
                stage=stage,
                status="ok",
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

        if str(stage["executionMode"]) == "invalidated":
            emit_event(
                "stage-invalidated",
                {
                    "manifestPath": str(stage["paths"]["manifestPath"]),
                    "reason": str(stage["reuse"]["reason"]),
                },
                stage=stage,
                status="running",
            )

        emit_event(
            "stage-start",
            {
                "manifestPath": str(stage["paths"]["manifestPath"]),
            },
            stage=stage,
            status="running",
        )

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
                    diagnostics={},
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
                primary_evidence_paths=list(worker_result.get("primaryEvidencePaths") or []),
                duration_ms=int(dict(worker_result.get("metrics") or {}).get("durationMs") or 0),
                diagnostics=dict(worker_result.get("diagnostics") or {}),
                details=dict(worker_result.get("details") or {}),
                failure=worker_result.get("failure"),
            )
        )
        emit_event(
            "stage-finished",
            {
                "manifestPath": str(stage_results[-1]["manifestPath"]),
                "reportPaths": list(stage_results[-1]["reportPaths"]),
                "primaryEvidencePaths": list(stage_results[-1]["primaryEvidencePaths"]),
                "failure": stage_results[-1]["failure"],
            },
            stage=stage,
            status=str(worker_result.get("status") or "fail"),
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
        "events": emitted_events,
        "errors": errors,
    }


def execute_subject_matrix(
    repo_root: Path,
    subject_id: str,
    *,
    goal_id: str | None = None,
    matrix_id: str | None = None,
    run_id: str | None = None,
) -> dict[str, Any]:
    plan = planner_module.build_plan(repo_root, subject_id, goal_id=goal_id, matrix_id=matrix_id, run_id=run_id)
    return execute_plan(repo_root, plan, run_id=run_id)


def trace_paths_from_execution(repo_root: Path, execution_result: dict[str, Any]) -> list[str]:
    for stage_result in reversed(list(execution_result.get("stageResults") or [])):
        manifest_path = str(stage_result.get("manifestPath") or "")
        if not manifest_path:
            continue

        manifest_file = repo_root / manifest_path
        if not manifest_file.is_file():
            continue

        try:
            manifest = json.loads(manifest_file.read_text(encoding="utf-8"))
        except (OSError, ValueError):
            continue
        if not isinstance(manifest, dict):
            continue

        trace_paths = [str(value) for value in list(manifest.get("tracePaths") or []) if str(value)]
        if trace_paths:
            return trace_paths

    return []
