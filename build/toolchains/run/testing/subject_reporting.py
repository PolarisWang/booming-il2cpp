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


DEPRECATED_RUNTIME_SKELETON_RESERVED_STUB_COUNT = 0


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
    codegen_performance = dict(dict(stage_result.get("details") or {}).get("codegenPerformance") or {})
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
    if codegen_performance:
        details["codegenPerformance"] = {
            "metrics": dict(codegen_performance.get("metrics") or {}),
            "baselinePath": codegen_performance.get("baselinePath"),
            "baseline": dict(codegen_performance.get("baseline") or {}),
            "baselineUpdated": bool(codegen_performance.get("baselineUpdated", False)),
            "regressionStatus": str(codegen_performance.get("regressionStatus") or "no-baseline"),
            "regressions": list(codegen_performance.get("regressions") or []),
            "metricsArtifactPath": codegen_performance.get("metricsArtifactPath"),
            "baselineComparePath": codegen_performance.get("baselineComparePath"),
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
    codegen_performance = next(
        (
            dict(dict(stage_result.get("details") or {}).get("codegenPerformance") or {})
            for stage_result in normalized_stage_results
            if dict(stage_result.get("details") or {}).get("codegenPerformance")
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
    if codegen_performance:
        report["codegenPerformance"] = codegen_performance
        report["codegenMetrics"] = dict(codegen_performance.get("metrics") or {})
        report["codegenBaseline"] = {
            "path": codegen_performance.get("baselinePath"),
            "metrics": dict(codegen_performance.get("baseline") or {}),
            "updated": bool(codegen_performance.get("baselineUpdated", False)),
            "regressions": list(codegen_performance.get("regressions") or []),
        }
        report["codegenRegressionStatus"] = str(codegen_performance.get("regressionStatus") or "no-baseline")
        report["codegenReportPaths"] = []
        report.setdefault("reportArtifacts", [])
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
                "codegenMetrics": dict(report.get("codegenMetrics") or {}),
                "codegenRegressionStatus": report.get("codegenRegressionStatus"),
                "codegenReportPaths": list(report.get("codegenReportPaths") or []),
                "releaseReportPaths": list(report.get("releaseReportPaths") or []),
                "reportArtifacts": list(report.get("reportArtifacts") or []),
                "matrixProofLinkage": dict(report.get("matrixProofLinkage") or {}),
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


def _build_subject_review_bundle(subject_summary: dict[str, Any]) -> dict[str, Any]:
    matrix_results = [
        dict(matrix_result)
        for matrix_result in list(subject_summary.get("matrixResults") or [])
        if isinstance(matrix_result, dict)
    ]
    artifact_paths = _ordered_unique_paths(
        [
            str(artifact_path)
            for matrix_result in matrix_results
            for artifact_path in list(matrix_result.get("reportArtifacts") or [])
        ]
    )
    proof_kinds = _ordered_unique_paths(
        [
            str(dict(matrix_result.get("matrixProofLinkage") or {}).get("proofKind") or "")
            for matrix_result in matrix_results
        ]
    )

    return {
        "reportVersion": "v1",
        "artifactKind": "subject-review-bundle",
        "runId": str(subject_summary.get("runId") or ""),
        "generatedAt": str(subject_summary.get("generatedAt") or ""),
        "subjectId": str(subject_summary.get("subjectId") or ""),
        "requestedGoalId": str(subject_summary.get("requestedGoalId") or ""),
        "status": str(subject_summary.get("status") or "aborted"),
        "matrixCount": len(matrix_results),
        "artifactCount": len(artifact_paths),
        "artifactPaths": artifact_paths,
        "proofKinds": proof_kinds,
        "matrixReviews": [
            {
                "matrixId": str(matrix_result.get("matrixId") or ""),
                "reportPath": str(matrix_result.get("reportPath") or ""),
                "status": str(matrix_result.get("status") or "aborted"),
                "validationKind": matrix_result.get("validationKind"),
                "terminalBucket": str(matrix_result.get("terminalBucket") or ""),
                "artifactPaths": _ordered_unique_paths(
                    [str(path) for path in list(matrix_result.get("reportArtifacts") or [])]
                ),
                "proofKind": str(dict(matrix_result.get("matrixProofLinkage") or {}).get("proofKind") or ""),
                "boundaryCaseCount": int(
                    dict(matrix_result.get("matrixProofLinkage") or {}).get("boundaryCaseCount") or 0
                ),
            }
            for matrix_result in matrix_results
        ],
    }


def materialize_subject_report_artifacts(
    repo_root: Path,
    *,
    subject_summary_path: str,
    subject_summary: dict[str, Any],
) -> list[str]:
    summary_root = Path(subject_summary_path).parent
    review_bundle_path = summary_root / "review-bundle.json"
    review_bundle_payload = _build_subject_review_bundle(subject_summary)
    _write_json_document(repo_root / review_bundle_path, review_bundle_payload)

    review_bundle_path_text = review_bundle_path.as_posix()
    subject_summary["subjectReportArtifacts"] = [review_bundle_path_text]
    subject_summary["subjectReviewBundle"] = {
        "artifactPath": review_bundle_path_text,
        "matrixCount": int(review_bundle_payload.get("matrixCount") or 0),
        "artifactCount": int(review_bundle_payload.get("artifactCount") or 0),
        "proofKinds": list(review_bundle_payload.get("proofKinds") or []),
    }
    return list(subject_summary["subjectReportArtifacts"])


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
    subject_report_artifacts = list(subject_summary.get("subjectReportArtifacts") or [])
    if subject_report_artifacts:
        result["subjectReportArtifacts"] = subject_report_artifacts
    subject_review_bundle = dict(subject_summary.get("subjectReviewBundle") or {})
    if subject_review_bundle:
        result["subjectReviewBundle"] = subject_review_bundle
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


def _read_json_file(path: Path) -> dict[str, Any]:
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


def _generated_artifact_path(manifest: dict[str, Any], artifact_kind: str) -> str:
    for artifact in list(manifest.get("generatedArtifacts") or []):
        if not isinstance(artifact, dict):
            continue
        if str(artifact.get("kind") or "").strip() != artifact_kind:
            continue
        artifact_path = str(artifact.get("path") or "").strip()
        if artifact_path:
            return artifact_path
    return ""


def _resolve_manifest_relative_artifact_path(
    repo_root: Path,
    *,
    manifest_path: str,
    artifact_path: str,
) -> str:
    normalized_artifact_path = str(artifact_path or "").strip()
    if not normalized_artifact_path:
        return ""

    direct_path = repo_root / normalized_artifact_path
    if direct_path.is_file():
        return normalized_artifact_path

    normalized_manifest_path = str(manifest_path or "").strip()
    if not normalized_manifest_path:
        return ""

    resolved_path = (repo_root / normalized_manifest_path).parent / normalized_artifact_path
    if not resolved_path.is_file():
        return ""
    return _relative_artifact_path(repo_root, resolved_path)


def _load_runtime_skeleton_coverage_report(
    repo_root: Path,
    *,
    native_reference_manifest_path: str,
    native_reference_manifest: dict[str, Any],
) -> tuple[str, dict[str, Any]]:
    coverage_report_artifact_path = _generated_artifact_path(
        native_reference_manifest,
        "runtimeSkeletonCoverageReport",
    )
    coverage_report_path = _resolve_manifest_relative_artifact_path(
        repo_root,
        manifest_path=native_reference_manifest_path,
        artifact_path=coverage_report_artifact_path,
    )
    if not coverage_report_path:
        return "", {}
    return coverage_report_path, _read_json_file(repo_root / coverage_report_path)


def _supplemental_full_closure_summary(repo_root: Path, entry: dict[str, Any]) -> dict[str, Any]:
    native_reference_manifest_path = str(entry.get("nativeReferenceManifestPath") or "").strip()
    native_reference_plan_path = str(entry.get("nativeReferencePlanPath") or "").strip()
    native_aot_manifest_path = str(entry.get("nativeAotManifestPath") or "").strip()
    native_aot_plan_path = str(entry.get("nativeAotPlanPath") or "").strip()
    native_reference_manifest = _read_json_document(repo_root, native_reference_manifest_path)
    native_reference_plan = _read_json_document(repo_root, native_reference_plan_path)
    native_aot_plan = _read_json_document(repo_root, native_aot_plan_path)

    runtime_skeleton_coverage_report_path = str(entry.get("runtimeSkeletonCoverageReportPath") or "").strip()
    runtime_skeleton_coverage_report = _read_json_document(repo_root, runtime_skeleton_coverage_report_path)
    if not runtime_skeleton_coverage_report_path:
        runtime_skeleton_coverage_report_path, runtime_skeleton_coverage_report = _load_runtime_skeleton_coverage_report(
            repo_root,
            native_reference_manifest_path=native_reference_manifest_path,
            native_reference_manifest=native_reference_manifest,
        )
    runtime_skeleton_uncovered_method_count = runtime_skeleton_coverage_report.get("uncoveredMethodCount")
    if not isinstance(runtime_skeleton_uncovered_method_count, int) or runtime_skeleton_uncovered_method_count < 0:
        runtime_skeleton_uncovered_method_count = 0

    return {
        "assemblyName": str(entry.get("assemblyName") or ""),
        "inputAssemblyPath": str(entry.get("inputAssemblyPath") or ""),
        "nativeReferenceManifestPath": native_reference_manifest_path,
        "nativeReferencePlanPath": native_reference_plan_path,
        "nativeReferencePlanKind": str(native_reference_plan.get("planKind") or ""),
        "nativeReferenceTranslationUnitMode": str(native_reference_plan.get("translationUnitMode") or ""),
        "nativeReferenceTranslationUnitMethodCount": native_reference_plan.get("translationUnitMethodCount"),
        "nativeReferenceTranslationUnitPageCount": native_reference_manifest.get("translationUnitPageCount"),
        "runtimeSkeletonCoverageReportPath": runtime_skeleton_coverage_report_path,
        "runtimeSkeletonUncoveredMethodCount": runtime_skeleton_uncovered_method_count,
        "nativeAotManifestPath": native_aot_manifest_path,
        "nativeAotPlanPath": native_aot_plan_path,
        "nativeAotPlanKind": str(native_aot_plan.get("planKind") or ""),
        "nativeAotTranslationUnitMode": str(native_aot_plan.get("translationUnitMode") or ""),
        "nativeAotTranslationUnitMethodCount": native_aot_plan.get("translationUnitMethodCount"),
    }


def _ordered_unique_paths(paths: list[str]) -> list[str]:
    ordered_paths: list[str] = []
    seen_paths: set[str] = set()
    for value in paths:
        normalized_path = str(value or "").strip()
        if not normalized_path or normalized_path in seen_paths:
            continue
        seen_paths.add(normalized_path)
        ordered_paths.append(normalized_path)
    return ordered_paths


def _first_generated_manifest_path(matrix_report: dict[str, Any]) -> str:
    for stage_result in list(matrix_report.get("stageResults") or []):
        stage_result_dict = dict(stage_result)
        if str(stage_result_dict.get("bucket") or "") != "generated":
            continue
        manifest_path = str(stage_result_dict.get("manifestPath") or "").strip()
        if manifest_path:
            return manifest_path
    return ""


def _load_generic_capability_matrix_payload(
    repo_root: Path,
    matrix_report: dict[str, Any],
) -> tuple[str, str, str, dict[str, Any]]:
    generated_manifest_path = _first_generated_manifest_path(matrix_report)
    if not generated_manifest_path:
        return "", "", "", {}

    generated_manifest = _read_json_document(repo_root, generated_manifest_path)
    analysis_manifest_path = str(generated_manifest.get("analysisManifestPath") or "").strip()
    if not analysis_manifest_path:
        return generated_manifest_path, "", "", {}

    analysis_manifest = _read_json_document(repo_root, analysis_manifest_path)
    artifacts = dict(analysis_manifest.get("artifacts") or {})
    generic_capability_matrix_path = str(artifacts.get("genericCapabilityMatrixPath") or "").strip()
    if not generic_capability_matrix_path:
        return generated_manifest_path, analysis_manifest_path, "", {}

    return (
        generated_manifest_path,
        analysis_manifest_path,
        generic_capability_matrix_path,
        _read_json_document(repo_root, generic_capability_matrix_path),
    )


def _collect_matrix_proof_artifact_paths(matrix_report: dict[str, Any]) -> list[str]:
    candidate_paths: list[str] = []

    native_hotupdate_audit = dict(matrix_report.get("nativeHotupdateAudit") or {})
    audit_artifact_path = str(native_hotupdate_audit.get("artifactPath") or "").strip()
    if audit_artifact_path:
        candidate_paths.append(audit_artifact_path)

    engine_proof_summary = dict(matrix_report.get("engineProofSummary") or {})
    for evidence_result in list(engine_proof_summary.get("evidenceResults") or []):
        candidate_paths.append(str(dict(evidence_result).get("primaryPath") or ""))

    for stage_result in list(matrix_report.get("stageResults") or []):
        stage_result_dict = dict(stage_result)
        if str(stage_result_dict.get("bucket") or "") not in {"generated", "build", "runtime"}:
            continue
        candidate_paths.extend(
            str(value)
            for value in list(stage_result_dict.get("primaryEvidencePaths") or [])
            if str(value)
        )
        candidate_paths.extend(
            str(value)
            for value in list(stage_result_dict.get("reportPaths") or [])
            if str(value)
        )

    return _ordered_unique_paths(candidate_paths)


def _infer_matrix_proof_kind(matrix_report: dict[str, Any]) -> str:
    engine_proof_summary = dict(matrix_report.get("engineProofSummary") or {})
    proof_kind = str(engine_proof_summary.get("proofKind") or "").strip()
    if proof_kind:
        return proof_kind

    pipeline_id = str(dict(matrix_report.get("selection") or {}).get("pipelineId") or "").strip()
    if pipeline_id == "native-hotupdate-proof-output":
        return "hotupdate-proof"

    validation_kind = str(matrix_report.get("validationKind") or "").strip()
    if validation_kind == "proof":
        return "native-proof"

    if pipeline_id:
        return pipeline_id

    return "proof"


def _build_generic_matrix_proof_linkage_payload(
    repo_root: Path,
    *,
    matrix_report_path: str,
    matrix_report: dict[str, Any],
) -> tuple[str, dict[str, Any]]:
    (
        generated_manifest_path,
        analysis_manifest_path,
        generic_capability_matrix_path,
        generic_capability_matrix,
    ) = _load_generic_capability_matrix_payload(repo_root, matrix_report)
    if not generic_capability_matrix_path or not generic_capability_matrix:
        return "", {}

    boundary_cases = [
        dict(boundary_case)
        for boundary_case in list(generic_capability_matrix.get("boundaryCases") or [])
        if isinstance(boundary_case, dict)
    ]
    if not boundary_cases:
        return "", {}

    selection = dict(matrix_report.get("selection") or {})
    pipeline_id = str(selection.get("pipelineId") or "")
    proof_kind = _infer_matrix_proof_kind(matrix_report)
    proof_artifact_paths = _collect_matrix_proof_artifact_paths(matrix_report)
    boundary_kinds = sorted(
        {
            str(boundary_case.get("boundaryKind") or "").strip()
            for boundary_case in boundary_cases
            if str(boundary_case.get("boundaryKind") or "").strip()
        }
    )

    return (
        generic_capability_matrix_path,
        {
            "reportVersion": "v1",
            "artifactKind": "generic-matrix-proof-linkage",
            "subjectId": str(matrix_report.get("subjectId") or ""),
            "matrixId": str(matrix_report.get("matrixId") or ""),
            "goalId": str(matrix_report.get("goalId") or ""),
            "status": str(matrix_report.get("status") or ""),
            "pipelineId": pipeline_id,
            "proofKind": proof_kind,
            "matrixReportPath": matrix_report_path,
            "generatedManifestPath": generated_manifest_path,
            "analysisManifestPath": analysis_manifest_path,
            "genericCapabilityMatrixPath": generic_capability_matrix_path,
            "entrySelection": dict(selection.get("entrySelection") or {}),
            "proofArtifactPaths": proof_artifact_paths,
            "boundaryCaseCount": len(boundary_cases),
            "boundaryKinds": boundary_kinds,
            "boundaryCases": [
                {
                    "boundaryKind": str(boundary_case.get("boundaryKind") or ""),
                    "sourceMethodSubjectId": str(boundary_case.get("sourceMethodSubjectId") or ""),
                    "ilOffset": int(boundary_case.get("ilOffset") or 0),
                    "targetSubjectId": str(boundary_case.get("targetSubjectId") or ""),
                    "evidenceKind": str(boundary_case.get("evidenceKind") or ""),
                    "status": str(boundary_case.get("status") or ""),
                    "coverageStatus": "covered-by-matrix-run",
                    "proofKind": proof_kind,
                    "proofArtifactPaths": proof_artifact_paths,
                }
                for boundary_case in boundary_cases
            ],
        },
    )


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
    native_reference_plan_path = str(generated_manifest.get("nativeReferencePlanPath") or "").strip()
    native_reference_plan = _read_json_document(repo_root, native_reference_plan_path)
    supplemental_full_closure_summaries = [
        _supplemental_full_closure_summary(repo_root, dict(entry))
        for entry in list(generated_manifest.get("supplementalFullAssemblyClosures") or [])
        if isinstance(entry, dict)
    ]
    generated_source_paths = list(generated_manifest.get("generatedSourcePaths") or [])
    runtime_skeleton_coverage_report_path, runtime_skeleton_coverage_report = _load_runtime_skeleton_coverage_report(
        repo_root,
        native_reference_manifest_path=native_reference_manifest_path,
        native_reference_manifest=native_reference_manifest,
    )
    runtime_skeleton_uncovered_method_count = runtime_skeleton_coverage_report.get("uncoveredMethodCount")
    if not isinstance(runtime_skeleton_uncovered_method_count, int) or runtime_skeleton_uncovered_method_count < 0:
        runtime_skeleton_uncovered_method_count = 0

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
            "generatedSourcePaths": generated_source_paths,
            "nativeReferenceManifestPath": native_reference_manifest_path,
            "nativeReferencePlanPath": native_reference_plan_path,
            "nativeAotManifestPath": str(generated_manifest.get("nativeAotManifestPath") or ""),
            "nativeReferencePlanKind": str(native_reference_plan.get("planKind") or ""),
            "runtimeExecutionKind": str(native_reference_manifest.get("runtimeExecutionKind") or ""),
            "preferredAssemblyDispatchSubjectId": str(native_reference_manifest.get("preferredAssemblyDispatchSubjectId") or ""),
            "translationUnitMode": str(native_reference_plan.get("translationUnitMode") or ""),
            "translationUnitMethodCount": native_reference_plan.get("translationUnitMethodCount"),
            "translationUnitPageCount": native_reference_manifest.get("translationUnitPageCount"),
            "auditStatus": str(native_reference_plan.get("auditStatus") or ""),
            "auditMessage": str(native_reference_plan.get("auditMessage") or ""),
            "runtimeSkeletonReservedStubCount": DEPRECATED_RUNTIME_SKELETON_RESERVED_STUB_COUNT,
            "runtimeSkeletonCoverageReportPath": runtime_skeleton_coverage_report_path,
            "runtimeSkeletonUncoveredMethodCount": runtime_skeleton_uncovered_method_count,
            "supplementalFullAssemblyClosures": supplemental_full_closure_summaries,
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
            "coreLibScope": (
                "narrow-proof-packet-plus-supplemental-full-closure-evidence"
                if supplemental_full_closure_summaries
                else "narrow-proof-packet"
            ),
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
    codegen_performance = dict(matrix_report.get("codegenPerformance") or {})
    matrix_report["releaseReportPaths"] = list(matrix_report.get("releaseReportPaths") or [])
    matrix_report["reportArtifacts"] = list(matrix_report.get("reportArtifacts") or [])
    matrix_report["codegenReportPaths"] = list(matrix_report.get("codegenReportPaths") or [])
    pipeline_id = str(dict(matrix_report.get("selection") or {}).get("pipelineId") or "")
    matrix_root = Path(matrix_report_path).parent.parent
    report_root = matrix_root / "pipeline-report" / "report"
    if pipeline_id == "native-hotupdate-proof-output":
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
            "nativeReferencePlanKind": audit_payload["nativeGeneration"]["nativeReferencePlanKind"],
            "preferredAssemblyDispatchSubjectId": audit_payload["nativeGeneration"]["preferredAssemblyDispatchSubjectId"],
            "translationUnitMethodCount": audit_payload["nativeGeneration"]["translationUnitMethodCount"],
            "translationUnitPageCount": audit_payload["nativeGeneration"]["translationUnitPageCount"],
            "runtimeSkeletonReservedStubCount": audit_payload["nativeGeneration"]["runtimeSkeletonReservedStubCount"],
            "runtimeSkeletonCoverageReportPath": audit_payload["nativeGeneration"]["runtimeSkeletonCoverageReportPath"],
            "runtimeSkeletonUncoveredMethodCount": audit_payload["nativeGeneration"]["runtimeSkeletonUncoveredMethodCount"],
            "auditStatus": audit_payload["nativeGeneration"]["auditStatus"],
            "supplementalFullAssemblyClosures": list(audit_payload["nativeGeneration"]["supplementalFullAssemblyClosures"]),
            "nativeBuildManifestPath": audit_payload["hotupdateRuntime"]["nativeBuildManifestPath"],
            "managedRuntimeAssemblyPath": audit_payload["hotupdateRuntime"]["managedRuntimeAssemblyPath"],
            "stdoutPath": audit_payload["hotupdateRuntime"]["stdoutPath"],
            "outputLines": list(audit_payload["hotupdateRuntime"]["outputLines"]),
        }
    if codegen_performance:
        codegen_summary_path = report_root / "codegen-summary.json"
        codegen_baseline_compare_path = report_root / "codegen-baseline-compare.json"
        codegen_metrics_path = report_root / "codegen-metrics.json"
        _write_json_document(
            repo_root / codegen_summary_path,
            {
                "reportVersion": "v1",
                "subjectId": matrix_report.get("subjectId"),
                "matrixId": matrix_report.get("matrixId"),
                "goalId": matrix_report.get("goalId"),
                "status": matrix_report.get("status"),
                "metrics": dict(matrix_report.get("codegenMetrics") or {}),
                "regressionStatus": matrix_report.get("codegenRegressionStatus"),
            },
        )
        _write_json_document(
            repo_root / codegen_baseline_compare_path,
            {
                "reportVersion": "v1",
                "subjectId": matrix_report.get("subjectId"),
                "matrixId": matrix_report.get("matrixId"),
                "goalId": matrix_report.get("goalId"),
                "metrics": dict(matrix_report.get("codegenMetrics") or {}),
                "baseline": dict(matrix_report.get("codegenBaseline") or {}),
                "regressionStatus": matrix_report.get("codegenRegressionStatus"),
            },
        )
        _write_json_document(
            repo_root / codegen_metrics_path,
            {
                "reportVersion": "v1",
                "subjectId": matrix_report.get("subjectId"),
                "matrixId": matrix_report.get("matrixId"),
                "goalId": matrix_report.get("goalId"),
                "metrics": dict(matrix_report.get("codegenMetrics") or {}),
            },
        )
        codegen_report_artifacts = [
            codegen_summary_path.as_posix(),
            codegen_baseline_compare_path.as_posix(),
            codegen_metrics_path.as_posix(),
        ]
        matrix_report["codegenReportPaths"] = codegen_report_artifacts
        for artifact_path in codegen_report_artifacts:
            if artifact_path not in matrix_report["reportArtifacts"]:
                matrix_report["reportArtifacts"].append(artifact_path)
    generic_capability_matrix_path, generic_matrix_proof_linkage_payload = _build_generic_matrix_proof_linkage_payload(
        repo_root,
        matrix_report_path=matrix_report_path,
        matrix_report=matrix_report,
    )
    if generic_matrix_proof_linkage_payload:
        linkage_path = report_root / "generic-matrix-proof-linkage.json"
        _write_json_document(repo_root / linkage_path, generic_matrix_proof_linkage_payload)
        linkage_path_text = linkage_path.as_posix()
        if linkage_path_text not in matrix_report["reportArtifacts"]:
            matrix_report["reportArtifacts"].append(linkage_path_text)
        matrix_report["matrixProofLinkage"] = {
            "artifactPath": linkage_path_text,
            "proofKind": generic_matrix_proof_linkage_payload["proofKind"],
            "boundaryCaseCount": generic_matrix_proof_linkage_payload["boundaryCaseCount"],
            "boundaryKinds": list(generic_matrix_proof_linkage_payload["boundaryKinds"]),
            "proofArtifactCount": len(generic_matrix_proof_linkage_payload["proofArtifactPaths"]),
            "genericCapabilityMatrixPath": generic_capability_matrix_path,
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

    performance_report_artifacts = [
        summary_path.as_posix(),
        baseline_compare_path.as_posix(),
        metrics_path.as_posix(),
    ]
    for artifact_path in performance_report_artifacts:
        if artifact_path not in matrix_report["reportArtifacts"]:
            matrix_report["reportArtifacts"].append(artifact_path)
    if is_managed_release:
        matrix_report["releaseReportPaths"] = performance_report_artifacts
    return list(matrix_report["reportArtifacts"])
