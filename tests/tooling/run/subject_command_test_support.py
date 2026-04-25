from __future__ import annotations

import importlib.util
import json
import shutil
import sys
import unittest
from pathlib import Path
from typing import Any
from unittest.mock import patch

from tests.support import make_temp_repo_root


REPO_ROOT = Path(__file__).resolve().parents[3]
TEST_COMMAND_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "commands" / "test.py"
SHARED_RUNTIME_PROJECT_PATH = "src/reference/Chaos.TestFramework.Runtime/Chaos.TestFramework.Runtime.csproj"
SHARED_RUNTIME_ASSEMBLY_NAME = "Chaos.TestFramework.Runtime"


def load_module(path: Path, module_name: str):
    if not path.is_file():
        raise FileNotFoundError(f"module missing: {path}")

    spec = importlib.util.spec_from_file_location(module_name, path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load module: {path}")

    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def write_json(path: Path, payload: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")


def write_subject_workspace(
    repo_root: Path,
    *,
    subject_id: str,
    matrix_id: str,
    goal_ids: list[str],
    unit_stable_id: str,
    unit_alias: str,
    unit_entry_index: int,
    benchmark_stable_id: str,
    benchmark_alias: str,
    benchmark_entry_index: int,
    include_native_proof: bool = True,
) -> None:
    workspace_root = repo_root / "verification" / "workspaces" / "subjects" / subject_id
    managed_tests_root = workspace_root / "managed-tests"
    generated_root = managed_tests_root / "Generated"
    native_root = workspace_root / "native" / matrix_id

    native_proof_project_path = native_root / "proof" / "chaos_subject_reference_proof.vcxproj"
    collection_path = generated_root / "declared-tests.collection.json"
    manifest_path = workspace_root / "workspace.manifest.json"

    for path in [native_proof_project_path]:
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text("<Project />\n", encoding="utf-8")

    collection_payload = {
        "subjectId": subject_id,
        "frameworkReferenced": True,
        "subjectKind": "declared-test",
        "warningCodes": [],
        "declaredUnitTests": [
            {
                "stableId": unit_stable_id,
                "entryIndex": unit_entry_index,
                "alias": unit_alias,
                "assemblyName": subject_id,
                "declaringType": f"{subject_id}.Proofs",
                "methodName": "Run",
                "methodSignature": "Run()",
                "category": 1,
                "capabilityFamily": 1,
                "capabilityItem": 1,
                "archetype": 1,
                "hotUpdateCapability": 0,
                "requires": 0,
                "evidence": 1,
                "priority": 1,
            }
        ],
        "declaredBenchmarks": [
            {
                "stableId": benchmark_stable_id,
                "entryIndex": benchmark_entry_index,
                "alias": benchmark_alias,
                "assemblyName": subject_id,
                "declaringType": f"{subject_id}.Benchmarks",
                "methodName": "Run",
                "methodSignature": "Run()",
                "category": 1,
                "capabilityFamily": 1,
                "capabilityItem": 1,
                "archetype": 1,
                "hotUpdateCapability": 0,
                "requires": 0,
                "metrics": 1,
                "modes": 1,
                "warmupCount": 2,
                "iterationCount": 5,
                "invocationCount": 10,
            }
        ],
    }
    write_json(collection_path, collection_payload)

    managed_test_projects = [
        {
            "projectId": f"managed-test/{subject_id}/proof-host",
            "projectPath": SHARED_RUNTIME_PROJECT_PATH,
            "assemblyName": SHARED_RUNTIME_ASSEMBLY_NAME,
            "hostKind": "proof-host",
            "collectionPath": collection_path.relative_to(repo_root).as_posix(),
            "executionModel": "shared-runtime-host",
        },
        {
            "projectId": f"managed-test/{subject_id}/benchmark-host",
            "projectPath": SHARED_RUNTIME_PROJECT_PATH,
            "assemblyName": SHARED_RUNTIME_ASSEMBLY_NAME,
            "hostKind": "benchmark-host",
            "collectionPath": collection_path.relative_to(repo_root).as_posix(),
            "executionModel": "shared-runtime-host",
        },
    ]
    native_test_projects = []
    native_test_project_ids: list[str] = []
    if include_native_proof:
        native_test_projects.append(
            {
                "projectId": f"native-test/{subject_id}/{matrix_id}/proof-host",
                "matrixId": matrix_id,
                "projectPath": native_proof_project_path.relative_to(repo_root).as_posix(),
                "configureRoot": native_root.relative_to(repo_root).as_posix(),
                "targetPlatform": "windows-x64",
                "toolchainProfile": "msvc-reference",
                "deliveryKind": "direct-run-host",
                "hostKind": "proof-host",
                "managedTestProjectId": f"managed-test/{subject_id}/proof-host",
                "buildArgs": ["--config", "Release", "--target", "chaos_subject_reference_proof"],
            }
        )
        native_test_project_ids.append(f"native-test/{subject_id}/{matrix_id}/proof-host")

    manifest_payload = {
        "workspaceVersion": 2,
        "kind": "subject-workspace",
        "subjectId": subject_id,
        "variant": "CHECK",
        "defaultMatrixId": matrix_id,
        "managedSolutionPath": f"verification/workspaces/subjects/{subject_id}/{subject_id}.sln",
        "managedProjects": [
            {
                "projectId": f"managed/{subject_id}/{subject_id}",
                "projectPath": f"verification/catalog/owners/{subject_id}/support/host/{subject_id}.csproj",
                "assemblyName": subject_id,
                "isPrimary": True,
            }
        ],
        "managedTestProjects": managed_test_projects,
        "nativeProjects": [],
        "nativeTestProjects": native_test_projects,
        "matrices": [
            {
                "matrixId": matrix_id,
                "goalIds": list(goal_ids),
                "hostPlatform": "windows-x64",
                "targetPlatform": "windows-x64",
                "toolchainProfile": "msvc-reference",
                "managedProjectIds": [f"managed/{subject_id}/{subject_id}"],
                "managedTestProjectIds": [
                    f"managed-test/{subject_id}/proof-host",
                    f"managed-test/{subject_id}/benchmark-host",
                ],
                "nativeProjectIds": [],
                "nativeTestProjectIds": native_test_project_ids,
            }
        ],
    }
    write_json(manifest_path, manifest_payload)


def make_subject_registry_index(
    test_module,
    *,
    subject_id: str,
    default_goal_id: str = "correctness.dev",
    matrix_id: str = "windows-native-check",
):
    return test_module.registry_module.RegistryIndex(
        host_platform="windows",
        suites=[],
        subjects=[
            {
                "id": f"subject/{subject_id}",
                "type": "subject",
                "displayName": subject_id,
                "subjectId": subject_id,
                "defaultGoalId": default_goal_id,
                "defaultMatrixId": matrix_id,
                "goalIds": [default_goal_id],
                "matrixIds": [matrix_id],
                "supportedHosts": ["windows"],
                "level": "subject",
                "primaryModuleId": None,
                "moduleIds": [],
                "subsystemIds": [],
                "docRefs": [],
                "canonicalCommand": f"run test subject --id subject/{subject_id}",
            }
        ],
        engineering_validations=[],
        engineering_workloads=[],
        declared_unit_tests=[],
        declared_benchmarks=[],
        module_verifications=[],
        system_scenarios=[],
        errors=[],
        warnings=[],
    )


def make_declared_registry_index(
    test_module,
    *,
    subject_id: str,
    object_type: str,
    stable_id: str,
    alias: str,
    source_entry: str,
    workload_entry: str = "",
    entry_index: int | None = None,
    default_goal_id: str = "correctness.dev",
    matrix_id: str = "windows-native-check",
):
    subject_index = make_subject_registry_index(
        test_module,
        subject_id=subject_id,
        default_goal_id=default_goal_id,
        matrix_id=matrix_id,
    )
    declared_object = {
        "id": f"{object_type}/{stable_id}",
        "type": object_type,
        "displayName": alias,
        "subjectId": subject_id,
        "defaultGoalId": default_goal_id,
        "defaultMatrixId": matrix_id,
        "goalIds": [default_goal_id],
        "matrixIds": [matrix_id],
        "supportedHosts": ["windows"],
        "level": "subject",
        "primaryModuleId": None,
        "moduleIds": [],
        "subsystemIds": [],
        "docRefs": [],
        "canonicalCommand": f"run test {object_type} --id {object_type}/{stable_id}",
        "stableId": stable_id,
        "alias": alias,
        "sourceEntry": source_entry,
        "workloadEntry": workload_entry,
    }
    if entry_index is not None:
        declared_object["entryIndex"] = entry_index
    return test_module.registry_module.RegistryIndex(
        host_platform="windows",
        suites=[],
        subjects=subject_index.subjects,
        engineering_validations=[],
        engineering_workloads=[],
        declared_unit_tests=[declared_object] if object_type == "declared-unit-test" else [],
        declared_benchmarks=[declared_object] if object_type == "declared-benchmark" else [],
        module_verifications=[],
        system_scenarios=[],
        errors=[],
        warnings=[],
    )


def make_subject_plan(
    *,
    subject_id: str,
    run_id: str,
    matrix_id: str,
    goal_id: str,
    stage_kind: str,
    stage_bucket: str = "runtime",
    validation_profile_id: str | None = None,
    validation_kind: str | None = None,
    variant: str | None = None,
) -> dict[str, Any]:
    runs_root = f"artifacts/subjects/{subject_id}/runs"
    run_root = f"{runs_root}/{run_id}"
    matrix_root = f"{run_root}/matrices/{matrix_id}"
    pipeline_report_root = f"{matrix_root}/pipeline-report"
    subject_report_root = f"{run_root}/subject-report"
    selection = {
        "subjectId": subject_id,
        "matrixId": matrix_id,
        "goalId": goal_id,
        "pipelineId": f"pipeline/{matrix_id}",
        "validationProfileId": validation_profile_id,
        "validationKinds": [validation_kind] if validation_kind else [],
        "validationKind": validation_kind,
        "variant": variant,
        "artifactPlan": {"evidenceTerminalBucket": stage_bucket},
        "executionContext": {
            "hostPlatform": "windows-x64",
            "targetPlatform": "windows-x64",
            "toolchainProfile": "windows-default",
            "runtimeProfile": "coreclr",
        },
    }
    return {
        "selection": selection,
        "stagePlan": [
            {
                "stageId": f"{stage_bucket}:{stage_kind}",
                "kind": stage_kind,
                "bucket": stage_bucket,
                "scope": "matrix",
                "paths": {
                    "bucketRoot": f"{matrix_root}/{stage_bucket}",
                    "manifestPath": f"{matrix_root}/{stage_bucket}/manifest.json",
                    "reportPaths": [],
                },
            }
        ],
        "artifactsRoot": {
            "runsRoot": runs_root,
            "runRoot": run_root,
            "runReportRoot": f"{run_root}/run-report",
            "subjectReportRoot": subject_report_root,
            "pipelineReportRoot": pipeline_report_root,
            "entryReportPath": f"{pipeline_report_root}/report.json",
            "entrySummaryPath": f"{subject_report_root}/summary.json",
        },
    }


def make_build_plan_side_effect(
    observed_selection: dict[str, str],
    *,
    subject_id: str,
    expected_run_id: str,
    default_goal_id: str = "correctness.dev",
    default_matrix_id: str = "windows-native-check",
    stage_kind: str = "runtime-observe",
    stage_bucket: str = "runtime",
    default_validation_profile_id: str | None = None,
    default_validation_kind: str | None = None,
):
    def side_effect(
        repo_root: Path,
        subject_key: str,
        *,
        goal_id: str | None = None,
        matrix_id: str | None = None,
        validation_profile_id: str | None = None,
        validation_kind: str | None = None,
        variant: str | None = None,
        run_id: str,
        source_entry: str | None = None,
        workload_entry: str | None = None,
        entry_selection: dict[str, Any] | None = None,
    ) -> dict[str, Any]:
        del repo_root
        del source_entry
        del workload_entry
        del entry_selection
        if subject_key != subject_id:
            raise AssertionError(f"unexpected subject id: {subject_key}")
        if run_id != expected_run_id:
            raise AssertionError(f"unexpected run id: {run_id}")
        resolved_goal_id = goal_id or default_goal_id
        resolved_matrix_id = matrix_id or default_matrix_id
        resolved_validation_profile_id = validation_profile_id or default_validation_profile_id
        resolved_validation_kind = validation_kind or default_validation_kind
        observed_selection.update(
            {
                "subjectId": subject_id,
                "matrixId": resolved_matrix_id,
                "goalId": resolved_goal_id,
                "validationProfileId": resolved_validation_profile_id or "",
                "validationKind": resolved_validation_kind or "",
                "variant": variant or "",
                "terminalBucket": stage_bucket,
            }
        )
        return make_subject_plan(
            subject_id=subject_id,
            run_id=run_id,
            matrix_id=resolved_matrix_id,
            goal_id=resolved_goal_id,
            stage_kind=stage_kind,
            stage_bucket=stage_bucket,
            validation_profile_id=resolved_validation_profile_id,
            validation_kind=resolved_validation_kind,
            variant=variant,
        )

    return side_effect


def build_execution_result(
    plan: dict[str, Any],
    *,
    stage_kind: str,
    duration_ms: int,
    fingerprint: str,
    details: dict[str, Any] | None = None,
    primary_evidence_paths: list[str] | None = None,
    terminal_bucket: str | None = None,
) -> dict[str, Any]:
    selection = dict(plan.get("selection") or {})
    stage = next(
        dict(candidate)
        for candidate in list(plan.get("stagePlan") or [])
        if str(candidate.get("kind") or "") == stage_kind
    )
    bucket_root = str(stage["paths"]["bucketRoot"])
    return {
        "subjectId": str(selection.get("subjectId") or ""),
        "matrixId": str(selection.get("matrixId") or ""),
        "goalId": str(selection.get("goalId") or ""),
        "status": "ok",
        "terminalStageId": str(stage["stageId"]),
        "terminalBucket": terminal_bucket or str(dict(selection.get("artifactPlan") or {}).get("evidenceTerminalBucket") or stage["bucket"]),
        "stageResults": [
            {
                "stageId": str(stage["stageId"]),
                "kind": str(stage["kind"]),
                "bucket": str(stage["bucket"]),
                "status": "ok",
                "planMode": "executed",
                "actionTaken": "executed",
                "invalidation": {"applied": False, "reason": None},
                "manifestPath": str(stage["paths"]["manifestPath"]),
                "reportPaths": list(stage["paths"]["reportPaths"]),
                "primaryEvidencePaths": primary_evidence_paths or [f"{bucket_root}/stdout.log"],
                "fingerprint": fingerprint,
                "durationMs": duration_ms,
                "diagnostics": {},
                "details": details or {},
                "failure": None,
            }
        ],
        "errors": [],
        "events": [],
    }


def empty_validation_outcome(*args: object, **kwargs: object) -> dict[str, Any]:
    del args
    del kwargs
    return {
        "status": "ok",
        "validationResults": [],
        "artifacts": [],
        "errors": [],
    }

class SubjectCommandTestSupport(unittest.TestCase):
    pass


