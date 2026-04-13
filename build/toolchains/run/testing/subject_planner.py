from __future__ import annotations

import hashlib
import json
from pathlib import Path
from typing import Any
import sys

try:
    from . import subjects as subjects_module
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from testing import subjects as subjects_module


ENTRY_ARTIFACT_SEGMENTS: dict[str, tuple[str, str]] = {
    "engineering-validation": ("engineering", "validations"),
    "engineering-workload": ("engineering", "workloads"),
    "declared-unit-test": ("declared", "unit"),
    "declared-benchmark": ("declared", "benchmark"),
}


def _stable_fingerprint(payload: dict[str, Any]) -> str:
    encoded = json.dumps(payload, sort_keys=True, separators=(",", ":")).encode("utf-8")
    return hashlib.sha256(encoded).hexdigest()[:16]


def _stage_exists(repo_root: Path, stage: dict[str, Any]) -> bool:
    return (repo_root / stage["paths"]["manifestPath"]).is_file()


def _sanitize_entry_path_key(value: str) -> str:
    normalized = "".join(character if character.isalnum() or character in {"-", "_", "."} else "-" for character in value)
    normalized = normalized.strip("-._")
    return normalized or "entry"


def _entry_artifact_paths(
    *,
    subject_id: str,
    matrix_id: str,
    run_id: str,
    entry_selection: dict[str, Any],
) -> dict[str, str]:
    family = str(entry_selection.get("family") or "")
    if family not in ENTRY_ARTIFACT_SEGMENTS:
        return {}

    path_key = str(entry_selection.get("alias") or entry_selection.get("kind") or "")
    if not path_key:
        path_key = _sanitize_entry_path_key(str(entry_selection.get("stableId") or ""))
    entry_root_prefix = "/".join(ENTRY_ARTIFACT_SEGMENTS[family])
    run_root = f"artifacts/subjects/{subject_id}/runs/{run_id}"

    return {
        "entryReportPath": f"{run_root}/matrices/{matrix_id}/{entry_root_prefix}/{path_key}/report.json",
        "entrySummaryPath": f"{run_root}/{entry_root_prefix}/{path_key}/summary.json",
    }


def _normalize_source_entry_selection(source: dict[str, Any]) -> dict[str, int]:
    payload = source.get("entrySelection")
    if payload is None:
        return {}
    if not isinstance(payload, dict):
        raise ValueError("source.entrySelection must be an object")

    normalized: dict[str, int] = {}
    for field_name in ("entryKind", "entrySlice"):
        value = payload.get(field_name)
        if value is None:
            continue
        if isinstance(value, bool) or not isinstance(value, int) or value < 0:
            raise ValueError(f"source.entrySelection.{field_name} must be a non-negative integer")
        normalized[field_name] = int(value)

    if normalized and set(normalized) != {"entryKind", "entrySlice"}:
        raise ValueError("source.entrySelection requires both entryKind and entrySlice")
    return normalized


def build_plan(
    repo_root: Path,
    subject_id: str,
    *,
    goal_id: str | None = None,
    matrix_id: str | None = None,
    validation_profile_id: str | None = None,
    validation_kind: str | None = None,
    variant: str | None = None,
    run_id: str | None = None,
    source_entry: str | None = None,
    workload_entry: str | None = None,
    entry_selection: dict[str, Any] | None = None,
) -> dict[str, Any]:
    manifest = subjects_module.load_subject_manifest(repo_root, subject_id)
    selected_goal_id = goal_id or str(manifest["defaultGoal"])
    selected_run_id = run_id or "subject-exec"
    if matrix_id is not None:
        selected_matrix_id = matrix_id
    else:
        default_matrix_id = str(manifest["defaultMatrix"])
        default_matrix = subjects_module.find_matrix(manifest, default_matrix_id)
        if selected_goal_id in list(default_matrix.get("supportedGoals") or []):
            selected_matrix_id = default_matrix_id
        else:
            selected_matrix_id = next(
                (
                    str(candidate.get("matrixId") or "")
                    for candidate in list(manifest.get("environmentMatrices") or [])
                    if selected_goal_id in list(candidate.get("supportedGoals") or [])
                ),
                default_matrix_id,
            )

    matrix = subjects_module.find_matrix(manifest, selected_matrix_id)
    validation_mode = str(dict(matrix.get("validationIntent") or {}).get("validationMode") or "")
    validation_selection = subjects_module.resolve_validation_selection(
        manifest,
        goal_id=selected_goal_id,
        validation_mode=validation_mode,
        validation_profile_id=validation_profile_id,
        validation_kind=validation_kind,
        variant=variant,
    )
    supported_goals = list(matrix.get("supportedGoals") or [])
    if selected_goal_id not in supported_goals:
        raise ValueError(f"matrix '{selected_matrix_id}' does not support goal '{selected_goal_id}'")

    pipeline = subjects_module.find_pipeline(manifest, str(matrix["pipelineId"]))
    artifacts_root = dict(
        subjects_module.subject_artifact_roots(
            subject_id,
            selected_matrix_id,
            run_id=selected_run_id,
        )
    )
    selected_source = dict(manifest["source"])
    selected_source.update(dict(matrix.get("source") or {}))
    if source_entry is not None:
        selected_source["entry"] = source_entry
    normalized_source_entry_selection = _normalize_source_entry_selection(selected_source)
    if normalized_source_entry_selection:
        selected_source["entrySelection"] = normalized_source_entry_selection
    selected_workload_entry = str(
        workload_entry
        or matrix.get("workloadEntry")
        or manifest.get("workloadEntry")
        or ""
    )
    if selected_workload_entry and not str(selected_source.get("entry") or ""):
        selected_source["entry"] = selected_workload_entry
    normalized_entry_selection = dict(entry_selection or {})
    if normalized_entry_selection:
        artifacts_root.update(
            _entry_artifact_paths(
                subject_id=subject_id,
                matrix_id=selected_matrix_id,
                run_id=selected_run_id,
                entry_selection=normalized_entry_selection,
            )
        )

    stage_plan: list[dict[str, Any]] = []
    stage_by_id: dict[str, dict[str, Any]] = {}
    for order, stage in enumerate(list(pipeline.get("stages") or []), start=1):
        stage_id = str(stage["stageId"])
        kind = str(stage["kind"])
        scope = str(stage["scope"])
        bucket = str(stage["bucket"])
        depends_on = [str(item) for item in list(stage.get("dependsOn") or [])]
        paths = subjects_module.stage_paths(
            subject_id,
            selected_matrix_id,
            run_id=selected_run_id,
            bucket=bucket,
            scope=scope,
            kind=kind,
        )
        upstream_fingerprints = {
            str(stage_by_id[dependency_id]["bucket"]): str(stage_by_id[dependency_id]["fingerprint"])
            for dependency_id in depends_on
        }
        fingerprint = _stable_fingerprint(
            {
                "subjectId": subject_id,
                "goalId": selected_goal_id,
                "matrixId": selected_matrix_id,
                "validationProfileId": str(validation_selection["validationProfileId"]),
                "validationKinds": list(validation_selection["validationKinds"]),
                "validationKind": validation_selection["validationKind"],
                "variant": str(validation_selection["variant"]),
                "sourceEntry": str(selected_source.get("entry") or ""),
                "sourceEntrySelection": dict(normalized_source_entry_selection),
                "workloadEntry": selected_workload_entry,
                "stageId": stage_id,
                "kind": kind,
                "bucket": bucket,
                "paths": paths,
                "upstreamFingerprints": upstream_fingerprints,
            }
        )
        planned_stage = {
            "order": order,
            "stageId": stage_id,
            "kind": kind,
            "scope": scope,
            "bucket": bucket,
            "dependsOn": depends_on,
            "fingerprint": fingerprint,
            "upstreamFingerprints": upstream_fingerprints,
            "paths": paths,
        }
        stage_by_id[stage_id] = planned_stage
        stage_plan.append(planned_stage)

    for stage in stage_plan:
        dependency_modes = [stage_by_id[dependency_id]["executionMode"] for dependency_id in stage["dependsOn"]]
        dependencies_reused = all(mode == "reused" for mode in dependency_modes)
        manifest_exists = _stage_exists(repo_root, stage)
        if manifest_exists and dependencies_reused:
            execution_mode = "reused"
            reuse = {
                "decision": "match",
                "reason": "fingerprint-match",
                "existingManifestPath": stage["paths"]["manifestPath"],
            }
        elif manifest_exists:
            execution_mode = "invalidated"
            reuse = {
                "decision": "mismatch",
                "reason": "upstream-invalidated",
                "existingManifestPath": stage["paths"]["manifestPath"],
            }
        else:
            execution_mode = "executed"
            reuse = {
                "decision": "absent",
                "reason": "manifest-missing",
                "existingManifestPath": stage["paths"]["manifestPath"],
            }

        stage["executionMode"] = execution_mode
        stage["reuse"] = reuse

    plan = {
        "planVersion": "v1",
        "request": {
            "subjectId": subject_id,
            "goalId": goal_id,
            "matrixId": matrix_id,
            "validationProfileId": validation_profile_id,
            "validationKind": validation_kind,
            "variant": variant,
            "runId": selected_run_id,
        },
        "selection": {
            "subjectId": subject_id,
            "displayName": str(manifest["displayName"]),
            "sourceModel": str(manifest.get("sourceModel") or ""),
            "dependencyModel": str(manifest.get("dependencyModel") or ""),
            "executablePlan": str(manifest.get("executablePlan") or ""),
            "engineeringProfile": str(manifest.get("engineeringProfile") or ""),
            "orchestration": dict(manifest.get("orchestration") or {}),
            "availability": dict(manifest.get("availability") or {}),
            "compatibility": dict(manifest.get("compatibility") or {}),
            "goalId": selected_goal_id,
            "matrixId": selected_matrix_id,
            "validationProfileId": str(validation_selection["validationProfileId"]),
            "validationKinds": list(validation_selection["validationKinds"]),
            "validationKind": validation_selection["validationKind"],
            "defaultVariant": str(validation_selection["defaultVariant"]),
            "variant": str(validation_selection["variant"]),
            "pipelineId": str(matrix["pipelineId"]),
            "source": selected_source,
            "workloadEntry": selected_workload_entry,
            "executionContext": dict(matrix["executionContext"]),
            "validationIntent": dict(matrix["validationIntent"]),
            "artifactPlan": dict(matrix["artifactPlan"]),
        },
        "artifactsRoot": artifacts_root,
        "stagePlan": stage_plan,
    }
    if normalized_entry_selection:
        plan["selection"]["entrySelection"] = normalized_entry_selection
    return plan
