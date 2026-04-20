from __future__ import annotations

import json
from pathlib import Path
from typing import Any
import sys

try:
    from . import release_evidence_contracts as release_evidence_contracts_module
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from testing import release_evidence_contracts as release_evidence_contracts_module


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
    engine_contract_summary = dict(dict(stage_result.get("details") or {}).get("engineContractSummary") or {})
    engine_emission_summary = dict(dict(stage_result.get("details") or {}).get("engineEmissionSummary") or {})
    engine_observation_summary = dict(dict(stage_result.get("details") or {}).get("engineObservationSummary") or {})
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
            "runtimeEvidence": dict(performance.get("runtimeEvidence") or {}),
            "collectorDetails": dict(performance.get("collectorDetails") or {}),
            "collectorEvidencePaths": list(performance.get("collectorEvidencePaths") or []),
        }
    if engine_contract_summary:
        details["engineContractSummary"] = engine_contract_summary
    if engine_emission_summary:
        details["engineEmissionSummary"] = engine_emission_summary
    if engine_observation_summary:
        details["engineObservationSummary"] = engine_observation_summary
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


def _first_stage_detail(stage_results: list[dict[str, Any]], detail_name: str) -> dict[str, Any]:
    for stage_result in stage_results:
        detail = dict(dict(stage_result.get("details") or {}).get(detail_name) or {})
        if detail:
            return detail
    return {}


def _build_engine_proof_summary(
    engine_contract_summary: dict[str, Any],
    engine_emission_summary: dict[str, Any],
    engine_observation_summary: dict[str, Any],
) -> dict[str, Any]:
    if not engine_contract_summary and not engine_emission_summary and not engine_observation_summary:
        return {}

    capability_ids = [
        str(value)
        for value in list(engine_emission_summary.get("emittedCapabilityIds") or [])
        if str(value)
    ]
    if not capability_ids:
        capability_ids = [
            str(value)
            for value in list(engine_contract_summary.get("resolvedCapabilityIds") or [])
            if str(value)
        ]

    evidence_results = [
        {
            "kind": str(dict(item).get("kind") or ""),
            "status": str(dict(item).get("status") or "unknown"),
            "primaryPath": str(dict(item).get("path") or ""),
        }
        for item in list(engine_observation_summary.get("evidenceItems") or [])
    ]

    return {
        "proofKind": str(
            engine_contract_summary.get("proofKind")
            or engine_emission_summary.get("proofKind")
            or "engine-binding"
        ),
        "focusArea": str(
            engine_contract_summary.get("focusArea")
            or engine_emission_summary.get("focusArea")
            or ""
        ),
        "capabilityIds": capability_ids,
        "evidenceResults": evidence_results,
        "localReportPaths": [
            str(value)
            for value in list(engine_observation_summary.get("localReportPaths") or [])
            if str(value)
        ],
    }


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
    engine_contract_summary = _first_stage_detail(normalized_stage_results, "engineContractSummary")
    engine_emission_summary = _first_stage_detail(normalized_stage_results, "engineEmissionSummary")
    engine_observation_summary = _first_stage_detail(normalized_stage_results, "engineObservationSummary")

    report = {
        "reportVersion": "v1",
        "runId": run_id,
        "generatedAt": generated_at,
        "subjectId": str(execution_result.get("subjectId") or selection.get("subjectId") or ""),
        "matrixId": str(execution_result.get("matrixId") or selection.get("matrixId") or ""),
        "goalId": str(execution_result.get("goalId") or selection.get("goalId") or ""),
        "validationProfileId": selection.get("validationProfileId"),
        "validationKinds": list(selection.get("validationKinds") or []),
        "validationKind": selection.get("validationKind"),
        "variant": selection.get("variant"),
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
        report["performanceEvidence"] = dict(performance.get("runtimeEvidence") or {})
        report["releaseReportPaths"] = []
        report["reportArtifacts"] = []
    if engine_contract_summary:
        report["engineContractSummary"] = engine_contract_summary
    if engine_emission_summary:
        report["engineEmissionSummary"] = engine_emission_summary
    if engine_observation_summary:
        report["engineObservationSummary"] = engine_observation_summary
    engine_proof_summary = _build_engine_proof_summary(
        engine_contract_summary,
        engine_emission_summary,
        engine_observation_summary,
    )
    if engine_proof_summary:
        report["engineProofSummary"] = engine_proof_summary
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
    entry_selection: dict[str, Any] | None = None,
    declared_unit_tests: list[dict[str, Any]] | None = None,
    declared_benchmarks: list[dict[str, Any]] | None = None,
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
                "validationProfileId": report.get("validationProfileId"),
                "validationKind": report.get("validationKind"),
                "variant": report.get("variant"),
                "status": status,
                "terminalBucket": str(report.get("terminalBucket") or ""),
                "executionContext": execution_context,
                "reportPath": str(matrix_report_paths[matrix_id]),
                "metrics": dict(report.get("metrics") or {}),
                "regressionStatus": report.get("regressionStatus"),
                "releaseReportPaths": list(report.get("releaseReportPaths") or []),
                "reportArtifacts": list(report.get("reportArtifacts") or []),
            }
        )

    summary = {
        "summaryVersion": "v1",
        "runId": run_id,
        "generatedAt": generated_at,
        "subjectId": subject_id,
        "requestedGoalId": requested_goal_id,
        "status": _aggregate_status(statuses),
        "matrixStatusCounts": counts,
        "matrixResults": matrix_results,
    }
    if entry_selection:
        summary["entrySelection"] = dict(entry_selection)
    if declared_unit_tests or declared_benchmarks:
        summary["releaseEvidenceSummary"] = release_evidence_contracts_module.build_declared_contract_status_report(
            declared_unit_tests=declared_unit_tests,
            declared_benchmarks=declared_benchmarks,
        )
    return summary


def build_subject_result(
    subject_summary: dict[str, Any],
    *,
    subject_summary_path: str,
) -> dict[str, Any]:
    result = {
        "subjectId": str(subject_summary.get("subjectId") or ""),
        "requestedGoalId": str(subject_summary.get("requestedGoalId") or ""),
        "status": str(subject_summary.get("status") or "aborted"),
        "matrixStatusCounts": dict(subject_summary.get("matrixStatusCounts") or _empty_status_counts()),
        "subjectSummaryPath": subject_summary_path,
    }
    entry_selection = dict(subject_summary.get("entrySelection") or {})
    if entry_selection:
        result["entrySelection"] = entry_selection
    release_evidence_summary = dict(subject_summary.get("releaseEvidenceSummary") or {})
    if release_evidence_summary:
        result["releaseEvidenceSummary"] = release_evidence_summary
    return result


def _write_json_document(path: Path, payload: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def _read_json_document(repo_root: Path, relative_path: str) -> dict[str, Any]:
    path = repo_root / relative_path
    if not path.is_file():
        return {}
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError):
        return {}
    return dict(payload) if isinstance(payload, dict) else {}


def _stage_manifest_path(matrix_report: dict[str, Any], stage_id: str) -> str:
    for stage_result in list(matrix_report.get("stageResults") or []):
        if str(dict(stage_result).get("stageId") or "") == stage_id:
            return str(dict(stage_result).get("manifestPath") or "")
    return ""


def _relative_artifact_path(repo_root: Path, path: Path) -> str:
    try:
        return path.relative_to(repo_root).as_posix()
    except ValueError:
        return path.as_posix()


def _native_hotupdate_audit_payload(repo_root: Path, matrix_report: dict[str, Any]) -> dict[str, Any]:
    host_input_manifest_path = _stage_manifest_path(matrix_report, "host-input-build")
    generated_manifest_path = _stage_manifest_path(matrix_report, "generated-native-proof")
    build_manifest_path = _stage_manifest_path(matrix_report, "build-target")
    runtime_manifest_path = _stage_manifest_path(matrix_report, "runtime-managed-output")

    host_input_manifest = _read_json_document(repo_root, host_input_manifest_path)
    generated_manifest = _read_json_document(repo_root, generated_manifest_path)
    build_manifest = _read_json_document(repo_root, build_manifest_path)
    runtime_manifest = _read_json_document(repo_root, runtime_manifest_path)

    native_reference_manifest_path = str(generated_manifest.get("nativeReferenceManifestPath") or "").strip()
    native_reference_manifest = _read_json_document(repo_root, native_reference_manifest_path)

    return {
        "reportVersion": "v1",
        "artifactKind": "native-hotupdate-audit",
        "runId": str(matrix_report.get("runId") or ""),
        "subjectId": str(matrix_report.get("subjectId") or ""),
        "matrixId": str(matrix_report.get("matrixId") or ""),
        "status": str(matrix_report.get("status") or ""),
        "pipelineId": str(dict(matrix_report.get("selection") or {}).get("pipelineId") or ""),
        "entrySelection": dict(dict(matrix_report.get("selection") or {}).get("entrySelection") or {}),
        "hostInput": {
            "manifestPath": host_input_manifest_path,
            "primaryProjectPath": str(host_input_manifest.get("primaryProjectPath") or ""),
            "primaryAssemblyPath": str(host_input_manifest.get("primaryAssemblyPath") or ""),
            "additionalAssemblyPaths": list(host_input_manifest.get("additionalAssemblyPaths") or []),
            "managedRuntimeProjectPath": str(host_input_manifest.get("managedRuntimeProjectPath") or ""),
            "managedRuntimeAssemblyPath": str(host_input_manifest.get("managedRuntimeAssemblyPath") or ""),
            "collectionPath": str(host_input_manifest.get("collectionPath") or ""),
            "bindingManifestPath": str(host_input_manifest.get("bindingManifestPath") or ""),
        },
        "nativeGeneration": {
            "manifestPath": generated_manifest_path,
            "generatedSourcePath": str(generated_manifest.get("generatedSourcePath") or ""),
            "generatedSourcePaths": list(generated_manifest.get("generatedSourcePaths") or []),
            "nativeReferenceManifestPath": native_reference_manifest_path,
            "nativeReferencePlanPath": str(generated_manifest.get("nativeReferencePlanPath") or ""),
            "nativeAotManifestPath": str(generated_manifest.get("nativeAotManifestPath") or ""),
            "runtimeExecutionKind": str(native_reference_manifest.get("runtimeExecutionKind") or ""),
            "preferredAssemblyDispatchSubjectId": str(native_reference_manifest.get("preferredAssemblyDispatchSubjectId") or ""),
            "translationUnitPageCount": native_reference_manifest.get("translationUnitPageCount"),
        },
        "nativeBuild": {
            "manifestPath": build_manifest_path,
            "buildKind": str(build_manifest.get("buildKind") or ""),
            "buildStrategy": str(build_manifest.get("buildStrategy") or ""),
            "hostSourcePath": str(build_manifest.get("hostSourcePath") or ""),
            "generatedSourcePaths": list(build_manifest.get("generatedSourcePaths") or []),
            "outputs": list(build_manifest.get("outputs") or []),
        },
        "hotupdateRuntime": {
            "manifestPath": runtime_manifest_path,
            "managedRuntimeAssemblyPath": str(runtime_manifest.get("managedRuntimeAssemblyPath") or ""),
            "nativePrimaryAssemblyPath": str(runtime_manifest.get("nativePrimaryAssemblyPath") or ""),
            "nativeGeneratedManifestPath": str(runtime_manifest.get("nativeGeneratedManifestPath") or ""),
            "nativeBuildManifestPath": str(runtime_manifest.get("nativeBuildManifestPath") or ""),
            "collectionPath": str(host_input_manifest.get("collectionPath") or ""),
            "bindingManifestPath": str(runtime_manifest.get("bindingManifestPath") or ""),
            "arguments": list(runtime_manifest.get("arguments") or []),
            "outputLines": list(runtime_manifest.get("outputLines") or []),
            "stdoutPath": str(runtime_manifest.get("stdoutPath") or ""),
            "stderrPath": str(runtime_manifest.get("stderrPath") or ""),
            "exitCodePath": str(runtime_manifest.get("exitCodePath") or ""),
        },
        "truthBoundary": {
            "coreLibScope": "narrow-proof-packet",
            "nativeReferenceScope": "assembly-bound-runtime-skeleton",
            "nativeAotScope": "not-used-by-this-combined-proof",
            "fullCoreLibTranslated": False,
        },
    }


def materialize_matrix_report_artifacts(
    repo_root: Path,
    *,
    matrix_report_path: str,
    matrix_report: dict[str, Any],
) -> list[str]:
    performance = dict(matrix_report.get("performance") or {})
    matrix_report["releaseReportPaths"] = list(matrix_report.get("releaseReportPaths") or [])
    matrix_report["reportArtifacts"] = list(matrix_report.get("reportArtifacts") or [])
    pipeline_id = str(dict(matrix_report.get("selection") or {}).get("pipelineId") or "")
    if pipeline_id == "native-hotupdate-proof-output":
        matrix_root = Path(matrix_report_path).parent.parent
        audit_path = matrix_root / "pipeline-report" / "report" / "native-hotupdate-audit.json"
        audit_payload = _native_hotupdate_audit_payload(repo_root, matrix_report)
        _write_json_document(repo_root / audit_path, audit_payload)
        audit_path_text = _relative_artifact_path(repo_root, audit_path)
        if audit_path_text not in matrix_report["reportArtifacts"]:
            matrix_report["reportArtifacts"].append(audit_path_text)
        matrix_report["nativeHotupdateAudit"] = {
            "artifactPath": audit_path_text,
            "status": str(audit_payload.get("status") or ""),
            "nativeReferenceManifestPath": audit_payload["nativeGeneration"]["nativeReferenceManifestPath"],
            "nativeBuildManifestPath": audit_payload["hotupdateRuntime"]["nativeBuildManifestPath"],
            "managedRuntimeAssemblyPath": audit_payload["hotupdateRuntime"]["managedRuntimeAssemblyPath"],
            "stdoutPath": audit_payload["hotupdateRuntime"]["stdoutPath"],
            "outputLines": list(audit_payload["hotupdateRuntime"]["outputLines"]),
        }
    if not performance:
        return list(matrix_report["reportArtifacts"])

    stage_kinds = {
        str(stage_result.get("kind") or "")
        for stage_result in list(matrix_report.get("stageResults") or [])
    }
    is_native_perf = bool({"native-runtime-perf", "mobile-native-perf"} & stage_kinds)
    is_managed_release = str(matrix_report.get("goalId") or "") == "perf.release"
    if not is_native_perf and not is_managed_release:
        matrix_report["releaseReportPaths"] = list(matrix_report.get("releaseReportPaths") or [])
        matrix_report["reportArtifacts"] = list(matrix_report.get("reportArtifacts") or [])
        return list(matrix_report["reportArtifacts"])

    matrix_root = Path(matrix_report_path).parent.parent
    if is_native_perf:
        report_root = matrix_root / "pipeline-report" / "report"
        summary_path = report_root / "perf-summary.json"
        baseline_compare_path = report_root / "perf-baseline-compare.json"
        metrics_path = report_root / "perf-metrics.json"
    else:
        validation_root = matrix_root / "validations" / "perf"
        summary_path = validation_root / "summary.json"
        baseline_compare_path = validation_root / "baseline-compare.json"
        metrics_path = validation_root / "samples.json"

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
    metrics_payload = (
        {
            "reportVersion": "v1",
            "subjectId": matrix_report.get("subjectId"),
            "matrixId": matrix_report.get("matrixId"),
            "goalId": matrix_report.get("goalId"),
            "metrics": dict(matrix_report.get("metrics") or {}),
        }
        if is_native_perf
        else {
            "reportVersion": "v1",
            "subjectId": matrix_report.get("subjectId"),
            "matrixId": matrix_report.get("matrixId"),
            "goalId": matrix_report.get("goalId"),
            "samples": list(performance.get("samples") or []),
        }
    )

    _write_json_document(repo_root / summary_path, summary_payload)
    _write_json_document(repo_root / baseline_compare_path, baseline_compare_payload)
    _write_json_document(repo_root / metrics_path, metrics_payload)

    report_artifacts = [
        summary_path.as_posix(),
        baseline_compare_path.as_posix(),
        metrics_path.as_posix(),
    ]
    matrix_report["reportArtifacts"] = report_artifacts
    if is_managed_release:
        matrix_report["releaseReportPaths"] = report_artifacts
    return report_artifacts
