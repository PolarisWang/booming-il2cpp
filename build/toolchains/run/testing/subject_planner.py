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


def _stable_fingerprint(payload: dict[str, Any]) -> str:
    encoded = json.dumps(payload, sort_keys=True, separators=(",", ":")).encode("utf-8")
    return hashlib.sha256(encoded).hexdigest()[:16]


def _stage_exists(repo_root: Path, stage: dict[str, Any]) -> bool:
    return (repo_root / stage["paths"]["manifestPath"]).is_file()


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
    artifacts_root = subjects_module.subject_artifact_roots(
        subject_id,
        selected_matrix_id,
        run_id=selected_run_id,
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

    selected_source = dict(manifest["source"])
    selected_source.update(dict(matrix.get("source") or {}))

    return {
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
            "goalId": selected_goal_id,
            "matrixId": selected_matrix_id,
            "validationProfileId": str(validation_selection["validationProfileId"]),
            "validationKinds": list(validation_selection["validationKinds"]),
            "validationKind": validation_selection["validationKind"],
            "defaultVariant": str(validation_selection["defaultVariant"]),
            "variant": str(validation_selection["variant"]),
            "pipelineId": str(matrix["pipelineId"]),
            "source": selected_source,
            "executionContext": dict(matrix["executionContext"]),
            "validationIntent": dict(matrix["validationIntent"]),
            "artifactPlan": dict(matrix["artifactPlan"]),
        },
        "artifactsRoot": artifacts_root,
        "stagePlan": stage_plan,
    }
