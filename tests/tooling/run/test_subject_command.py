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
        module_verifications=[],
        system_scenarios=[],
        pipelines=[],
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
        pipelines=[],
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


class SubjectCommandTests(unittest.TestCase):
    def test_subject_dispatch_routes_to_subject_planner_and_executor(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_subject_dispatch")
        repo_root = make_temp_repo_root("subject-command", "dispatch")
        manifest: dict[str, Any] = {}
        subject_id = "FixtureProjectDispatchSubject"
        fixed_run_id = "chaos-run-subject-dispatch"
        observed_selection: dict[str, str] = {}
        subject_runs_root = repo_root / "artifacts" / "subjects" / subject_id / "runs"
        run_root = subject_runs_root / fixed_run_id
        subject_last_path = subject_runs_root / "last.json"
        subject_current_path = subject_runs_root / "current.json"
        global_logs_root = repo_root / "artifacts" / "logs" / "tests"
        global_last_path = global_logs_root / "last.json"
        global_current_path = global_logs_root / "current.json"
        registry_index = make_subject_registry_index(test_module, subject_id=subject_id)
        build_plan = make_build_plan_side_effect(
            observed_selection,
            subject_id=subject_id,
            expected_run_id=fixed_run_id,
        )

        def execute_plan_side_effect(repo_root: Path, plan: dict, **_: object) -> dict:
            del repo_root
            return build_execution_result(
                plan,
                stage_kind="runtime-observe",
                duration_ms=2,
                fingerprint="dispatch-fingerprint",
            )

        with patch.object(
            test_module,
            "_execute_public_test_session",
            side_effect=AssertionError("legacy suite session should not run for test subject"),
        ) as legacy_session:
            try:
                with patch.object(test_module, "_scan_registry", return_value=registry_index):
                    with patch.object(test_module.subject_planner_module, "build_plan", side_effect=build_plan) as build_plan_mock:
                        with patch.object(test_module.reporting_module, "build_run_id", return_value=fixed_run_id):
                            with patch.object(
                                test_module.subject_executor_module,
                                "execute_plan",
                                side_effect=execute_plan_side_effect,
                            ) as execute_plan:
                                with patch.object(
                                    test_module.subject_validations_module,
                                    "run_subject_validations",
                                    side_effect=empty_validation_outcome,
                                ):
                                    result = test_module.handle(
                                        {"id": "test-subject", "handler": "test.dispatch"},
                                        repo_root,
                                        "windows",
                                        f"test subject --id subject/{subject_id}",
                                        manifest,
                                        {"id": f"subject/{subject_id}"},
                                    )

                self.assertEqual("ok", result.status)
                self.assertEqual(f"subject/{subject_id}", result.target)
                self.assertEqual(f"subject/{subject_id}", result.payload["selectedObject"]["id"])
                run_id = result.payload["runId"]
                matrix_id = observed_selection["matrixId"]
                self.assertEqual(
                    f"artifacts/subjects/{subject_id}/runs/{run_id}/subject-report/summary.json",
                    result.payload["subjectResults"][0]["subjectSummaryPath"],
                )
                self.assertEqual(subject_id, result.payload["subjectResults"][0]["subjectId"])
                self.assertIn(
                    f"artifacts/subjects/{subject_id}/runs/{run_id}/matrices/{matrix_id}/pipeline-report/report.json",
                    result.payload["artifacts"],
                )
                self.assertIn(
                    f"artifacts/subjects/{subject_id}/runs/{run_id}/subject-report/summary.json",
                    result.payload["artifacts"],
                )
                self.assertTrue((repo_root / result.payload["subjectResults"][0]["subjectSummaryPath"]).is_file())
                legacy_session.assert_not_called()
                build_plan_mock.assert_called_once()
                execute_plan.assert_called_once()
                self.assertEqual(result.payload["runId"], execute_plan.call_args.kwargs["run_id"])
                self.assertTrue(callable(execute_plan.call_args.kwargs["event_writer"]))
                self.assertEqual(
                    f"artifacts/subjects/{subject_id}/runs/{run_id}/run-report/summary.json",
                    result.payload["summaryPath"],
                )
                subject_last = read_json(subject_last_path)
                self.assertEqual(run_id, subject_last["runId"])
                self.assertEqual(
                    f"artifacts/subjects/{subject_id}/runs/{run_id}/run-report/summary.json",
                    subject_last["summaryPath"],
                )
                global_last = read_json(global_last_path)
                self.assertEqual(run_id, global_last["runId"])
                self.assertEqual(
                    f"artifacts/subjects/{subject_id}/runs/{run_id}/run-report/summary.json",
                    global_last["summaryPath"],
                )
                if subject_current_path.is_file():
                    subject_current = read_json(subject_current_path)
                    self.assertEqual(run_id, subject_current["runId"])
                    self.assertEqual("ok", subject_current["status"])
                if global_current_path.is_file():
                    global_current = read_json(global_current_path)
                    self.assertEqual(run_id, global_current["runId"])
                    self.assertEqual("ok", global_current["status"])
            finally:
                shutil.rmtree(repo_root, ignore_errors=True)

    def test_subject_dispatch_collects_perf_release_report_artifacts_for_perf_subject(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_subject_perf_dispatch")
        repo_root = make_temp_repo_root("subject-command", "managed-perf")
        manifest: dict[str, Any] = {}
        subject_id = "FixtureManagedPerfSubject"
        fixed_run_id = "chaos-run-subject-perf-release"
        observed_selection: dict[str, str] = {}
        subject_runs_root = repo_root / "artifacts" / "subjects" / subject_id / "runs"
        run_root = subject_runs_root / fixed_run_id
        subject_last_path = subject_runs_root / "last.json"
        subject_current_path = subject_runs_root / "current.json"
        global_logs_root = repo_root / "artifacts" / "logs" / "tests"
        global_last_path = global_logs_root / "last.json"
        global_current_path = global_logs_root / "current.json"
        registry_index = make_subject_registry_index(
            test_module,
            subject_id=subject_id,
            default_goal_id="perf.release",
        )
        build_plan = make_build_plan_side_effect(
            observed_selection,
            subject_id=subject_id,
            expected_run_id=fixed_run_id,
            default_goal_id="perf.release",
            stage_kind="runtime-perf-collect",
            default_validation_kind="perf",
        )

        def execute_plan_side_effect(repo_root: Path, plan: dict, **_: object) -> dict:
            del repo_root
            return build_execution_result(
                plan,
                stage_kind="runtime-perf-collect",
                duration_ms=50,
                fingerprint="runtime-fingerprint",
                details={
                    "performance": {
                        "samples": [
                            {"sampleIndex": 1, "durationMs": 12.0, "exitCode": 0},
                            {"sampleIndex": 2, "durationMs": 14.0, "exitCode": 0},
                        ],
                        "metrics": {
                            "sampleCount": 2,
                            "meanDurationMs": 13.0,
                            "minDurationMs": 12.0,
                            "maxDurationMs": 14.0,
                        },
                        "baselinePath": (
                            f"subjects/{observed_selection['subjectId']}/baselines/perf/"
                            f"{observed_selection['matrixId']}/windows.json"
                        ),
                        "baseline": {"meanDurationMs": 11.0},
                        "baselineUpdated": False,
                        "regressionStatus": "regressed",
                        "regressions": [
                            {"metric": "meanDurationMs", "baseline": 11.0, "actual": 13.0, "delta": 2.0}
                        ],
                    }
                },
            )

        try:
            with patch.object(test_module, "_scan_registry", return_value=registry_index):
                with patch.object(test_module.subject_planner_module, "build_plan", side_effect=build_plan):
                    with patch.object(test_module.reporting_module, "build_run_id", return_value=fixed_run_id):
                        with patch.object(
                            test_module.subject_executor_module,
                            "execute_plan",
                            side_effect=execute_plan_side_effect,
                        ):
                            with patch.object(
                                test_module.subject_validations_module,
                                "run_subject_validations",
                                side_effect=empty_validation_outcome,
                            ):
                                result = test_module.handle(
                                    {"id": "test-subject", "handler": "test.dispatch"},
                                    repo_root,
                                    "windows",
                                    f"test subject --id subject/{subject_id} --goal perf.release --validation-profile perf-profile --variant PROFILE",
                                    manifest,
                                    {
                                        "id": f"subject/{subject_id}",
                                        "goal": "perf.release",
                                        "validation_profile": "perf-profile",
                                        "variant": "PROFILE",
                                    },
                                )

            self.assertEqual("ok", result.status)
            self.assertEqual(f"subject/{subject_id}", result.target)
            run_id = result.payload["runId"]
            matrix_id = observed_selection["matrixId"]
            self.assertEqual("perf-profile", observed_selection["validationProfileId"])
            self.assertEqual("PROFILE", observed_selection["variant"])
            self.assertIn(
                f"artifacts/subjects/{subject_id}/runs/{run_id}/matrices/{matrix_id}/pipeline-report/report.json",
                result.payload["artifacts"],
            )
            self.assertIn(
                f"artifacts/subjects/{subject_id}/runs/{run_id}/matrices/{matrix_id}/validations/perf/summary.json",
                result.payload["artifacts"],
            )
            self.assertIn(
                f"artifacts/subjects/{subject_id}/runs/{run_id}/matrices/{matrix_id}/validations/perf/baseline-compare.json",
                result.payload["artifacts"],
            )
            self.assertIn(
                f"artifacts/subjects/{subject_id}/runs/{run_id}/matrices/{matrix_id}/validations/perf/samples.json",
                result.payload["artifacts"],
            )
            self.assertEqual(subject_id, result.payload["subjectResults"][0]["subjectId"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_subject_dispatch_retains_empty_validation_results_when_subject_has_no_subject_owned_validations(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_subject_no_validation_artifacts")
        repo_root = make_temp_repo_root("subject-command", "no-validation")
        manifest: dict[str, Any] = {}
        subject_id = "FixtureNoValidationSubject"
        fixed_run_id = "chaos-run-subject-no-validation-artifacts"
        subject_runs_root = repo_root / "artifacts" / "subjects" / subject_id / "runs"
        run_root = subject_runs_root / fixed_run_id
        subject_last_path = subject_runs_root / "last.json"
        subject_current_path = subject_runs_root / "current.json"
        global_logs_root = repo_root / "artifacts" / "logs" / "tests"
        global_last_path = global_logs_root / "last.json"
        global_current_path = global_logs_root / "current.json"
        registry_index = make_subject_registry_index(test_module, subject_id=subject_id)
        build_plan = make_build_plan_side_effect(
            {},
            subject_id=subject_id,
            expected_run_id=fixed_run_id,
            stage_kind="runtime-observe",
        )

        def execute_plan_side_effect(repo_root: Path, plan: dict, **_: object) -> dict:
            del repo_root
            return build_execution_result(
                plan,
                stage_kind="runtime-observe",
                duration_ms=20,
                fingerprint="runtime-fingerprint",
            )

        try:
            with patch.object(test_module, "_scan_registry", return_value=registry_index):
                with patch.object(test_module.subject_planner_module, "build_plan", side_effect=build_plan):
                    with patch.object(test_module.reporting_module, "build_run_id", return_value=fixed_run_id):
                        with patch.object(
                            test_module.subject_executor_module,
                            "execute_plan",
                            side_effect=execute_plan_side_effect,
                        ):
                            with patch.object(
                                test_module.subject_validations_module,
                                "run_subject_validations",
                                side_effect=empty_validation_outcome,
                            ):
                                result = test_module.handle(
                                    {"id": "test-subject", "handler": "test.dispatch"},
                                    repo_root,
                                    "windows",
                                    f"test subject --id subject/{subject_id}",
                                    manifest,
                                    {"id": f"subject/{subject_id}"},
                                )

            self.assertEqual("ok", result.status)
            matrix_result = result.payload["subjectResults"][0]
            self.assertEqual([], result.payload["validationResults"])
            self.assertFalse(any("/validations/" in artifact for artifact in result.payload["artifacts"]))
            self.assertEqual(subject_id, matrix_result["subjectId"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_subject_dispatch_collects_native_perf_report_artifacts_for_solution_core_pack(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_subject_native_perf_dispatch")
        repo_root = make_temp_repo_root("subject-command", "native-perf")
        manifest: dict[str, Any] = {}
        subject_id = "SolutionCorePack"
        fixed_run_id = "chaos-run-subject-native-perf"
        observed_selection: dict[str, str] = {}
        subject_runs_root = repo_root / "artifacts" / "subjects" / subject_id / "runs"
        run_root = subject_runs_root / fixed_run_id
        subject_last_path = subject_runs_root / "last.json"
        subject_current_path = subject_runs_root / "current.json"
        global_logs_root = repo_root / "artifacts" / "logs" / "tests"
        global_last_path = global_logs_root / "last.json"
        global_current_path = global_logs_root / "current.json"
        registry_index = make_subject_registry_index(
            test_module,
            subject_id=subject_id,
            default_goal_id="perf.release",
        )
        build_plan = make_build_plan_side_effect(
            observed_selection,
            subject_id=subject_id,
            expected_run_id=fixed_run_id,
            default_goal_id="perf.release",
            stage_kind="native-runtime-perf",
            default_validation_kind="perf",
        )

        def execute_plan_side_effect(repo_root: Path, plan: dict, **_: object) -> dict:
            del repo_root
            bucket_root = str(dict(plan["stagePlan"][0]["paths"])["bucketRoot"])
            return build_execution_result(
                plan,
                stage_kind="native-runtime-perf",
                duration_ms=42,
                fingerprint="native-perf-fingerprint",
                primary_evidence_paths=[
                    f"{bucket_root}/perf.runtime.json",
                    f"{bucket_root}/perf.samples.json",
                ],
                details={
                    "performance": {
                        "samples": [
                            {"sampleIndex": 1, "durationMs": 17.0, "exitCode": 0},
                            {"sampleIndex": 2, "durationMs": 18.0, "exitCode": 0},
                        ],
                        "metrics": {
                            "sampleCount": 2,
                            "meanDurationMs": 17.5,
                            "minDurationMs": 17.0,
                            "maxDurationMs": 18.0,
                        },
                        "baselinePath": (
                            f"subjects/{observed_selection['subjectId']}/baselines/perf/"
                            f"{observed_selection['matrixId']}/windows.json"
                        ),
                        "baseline": {"meanDurationMs": 16.0},
                        "baselineUpdated": False,
                        "regressionStatus": "regressed",
                        "regressions": [
                            {"metric": "meanDurationMs", "baseline": 16.0, "actual": 17.5, "delta": 1.5}
                        ],
                        "runtimeEvidence": {
                            "runtimePath": f"{bucket_root}/perf.runtime.json",
                            "samplesPath": f"{bucket_root}/perf.samples.json",
                        },
                    }
                },
                terminal_bucket="report",
            )

        try:
            with patch.object(test_module, "_scan_registry", return_value=registry_index):
                with patch.object(test_module.subject_planner_module, "build_plan", side_effect=build_plan):
                    with patch.object(test_module.reporting_module, "build_run_id", return_value=fixed_run_id):
                        with patch.object(
                            test_module.subject_executor_module,
                            "execute_plan",
                            side_effect=execute_plan_side_effect,
                        ):
                            with patch.object(
                                test_module.subject_validations_module,
                                "run_subject_validations",
                                side_effect=empty_validation_outcome,
                            ):
                                result = test_module.handle(
                                    {"id": "test-subject", "handler": "test.dispatch"},
                                    repo_root,
                                    "windows",
                                    f"test subject --id subject/{subject_id} --goal perf.release --validation-profile perf-profile --variant PROFILE",
                                    manifest,
                                    {
                                        "id": f"subject/{subject_id}",
                                        "goal": "perf.release",
                                        "validation_profile": "perf-profile",
                                        "variant": "PROFILE",
                                    },
                                )

            self.assertEqual("ok", result.status)
            run_id = result.payload["runId"]
            matrix_id = observed_selection["matrixId"]
            self.assertEqual("perf-profile", observed_selection["validationProfileId"])
            self.assertEqual("PROFILE", observed_selection["variant"])
            self.assertIn(
                f"artifacts/subjects/{subject_id}/runs/{run_id}/matrices/{matrix_id}/pipeline-report/report.json",
                result.payload["artifacts"],
            )
            self.assertIn(
                f"artifacts/subjects/{subject_id}/runs/{run_id}/matrices/{matrix_id}/pipeline-report/report/perf-summary.json",
                result.payload["artifacts"],
            )
            self.assertIn(
                f"artifacts/subjects/{subject_id}/runs/{run_id}/matrices/{matrix_id}/pipeline-report/report/perf-baseline-compare.json",
                result.payload["artifacts"],
            )
            self.assertIn(
                f"artifacts/subjects/{subject_id}/runs/{run_id}/matrices/{matrix_id}/pipeline-report/report/perf-metrics.json",
                result.payload["artifacts"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_subject_dispatch_uses_native_perf_defaults_when_validation_override_is_omitted(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_subject_native_perf_defaults")
        repo_root = make_temp_repo_root("subject-command", "native-perf-defaults")
        manifest: dict[str, Any] = {}
        subject_id = "SolutionCorePackDefaults"
        fixed_run_id = "chaos-run-subject-native-perf-defaults"
        observed_selection: dict[str, str] = {}
        subject_runs_root = repo_root / "artifacts" / "subjects" / subject_id / "runs"
        run_root = subject_runs_root / fixed_run_id
        subject_last_path = subject_runs_root / "last.json"
        subject_current_path = subject_runs_root / "current.json"
        global_logs_root = repo_root / "artifacts" / "logs" / "tests"
        global_last_path = global_logs_root / "last.json"
        global_current_path = global_logs_root / "current.json"
        registry_index = make_subject_registry_index(
            test_module,
            subject_id=subject_id,
            default_goal_id="perf.release",
            matrix_id="windows-native-perf",
        )
        build_plan = make_build_plan_side_effect(
            observed_selection,
            subject_id=subject_id,
            expected_run_id=fixed_run_id,
            default_goal_id="perf.release",
            default_matrix_id="windows-native-perf",
            stage_kind="native-runtime-perf",
            default_validation_profile_id="perf-profile",
            default_validation_kind="perf",
        )

        def execute_plan_side_effect(repo_root: Path, plan: dict, **_: object) -> dict:
            del repo_root
            bucket_root = str(dict(plan["stagePlan"][0]["paths"])["bucketRoot"])
            return build_execution_result(
                plan,
                stage_kind="native-runtime-perf",
                duration_ms=42,
                fingerprint="native-perf-defaults-fingerprint",
                primary_evidence_paths=[
                    f"{bucket_root}/perf.runtime.json",
                    f"{bucket_root}/perf.samples.json",
                ],
                details={
                    "performance": {
                        "samples": [
                            {"sampleIndex": 1, "durationMs": 4.5, "exitCode": 0},
                            {"sampleIndex": 2, "durationMs": 4.6, "exitCode": 0},
                        ],
                        "metrics": {
                            "sampleCount": 2,
                            "meanDurationMs": 4.55,
                            "minDurationMs": 4.5,
                            "maxDurationMs": 4.6,
                        },
                        "baselinePath": (
                            f"subjects/{observed_selection['subjectId']}/baselines/perf/"
                            f"{observed_selection['matrixId']}/windows.json"
                        ),
                        "baseline": {"meanDispatchNanoseconds": 5.0},
                        "baselineUpdated": False,
                        "regressionStatus": "ok",
                        "regressions": [],
                        "runtimeEvidence": {
                            "runtimePath": f"{bucket_root}/perf.runtime.json",
                            "samplesPath": f"{bucket_root}/perf.samples.json",
                        },
                    }
                },
                terminal_bucket="report",
            )

        try:
            with patch.object(test_module, "_scan_registry", return_value=registry_index):
                with patch.object(test_module.subject_planner_module, "build_plan", side_effect=build_plan):
                    with patch.object(test_module.reporting_module, "build_run_id", return_value=fixed_run_id):
                        with patch.object(
                            test_module.subject_executor_module,
                            "execute_plan",
                            side_effect=execute_plan_side_effect,
                        ):
                            with patch.object(
                                test_module.subject_validations_module,
                                "run_subject_validations",
                                side_effect=empty_validation_outcome,
                            ):
                                result = test_module.handle(
                                    {"id": "test-subject", "handler": "test.dispatch"},
                                    repo_root,
                                    "windows",
                                    f"test subject --id subject/{subject_id} --goal perf.release",
                                    manifest,
                                    {
                                        "id": f"subject/{subject_id}",
                                        "goal": "perf.release",
                                    },
                                )

            self.assertEqual("ok", result.status)
            self.assertEqual("perf.release", observed_selection["goalId"])
            self.assertEqual("windows-native-perf", observed_selection["matrixId"])
            self.assertEqual("perf-profile", observed_selection["validationProfileId"])
            self.assertEqual("perf", observed_selection["validationKind"])
            self.assertEqual("", observed_selection["variant"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_declared_unit_test_dispatch_passes_source_entry_selection_to_subject_planner(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_declared_unit_alias")
        repo_root = make_temp_repo_root("subject-command", "declared-unit-alias")
        manifest: dict[str, Any] = {}
        subject_id = "SolutionCorePack"
        stable_id = "solution-core-proof"
        alias = "solution-core-proof"
        source_entry = "SolutionCorePack/Launcher.Program::RunProof()"
        fixed_run_id = "chaos-run-declared-unit-alias"
        registry_index = make_declared_registry_index(
            test_module,
            subject_id=subject_id,
            object_type="declared-unit-test",
            stable_id=stable_id,
            alias=alias,
            source_entry=source_entry,
            workload_entry="should-be-cleared-for-unit-tests",
        )
        plan = make_subject_plan(
            subject_id=subject_id,
            run_id=fixed_run_id,
            matrix_id="windows-native-check",
            goal_id="correctness.dev",
            stage_kind="runtime-observe",
        )
        execution_result = build_execution_result(
            plan,
            stage_kind="runtime-observe",
            duration_ms=3,
            fingerprint="declared-unit-alias-fingerprint",
        )

        try:
            with patch.object(test_module, "_scan_registry", return_value=registry_index):
                with patch.object(test_module.subject_planner_module, "build_plan", return_value=plan) as build_plan_mock:
                    with patch.object(test_module.reporting_module, "build_run_id", return_value=fixed_run_id):
                        with patch.object(
                            test_module.subject_executor_module,
                            "execute_plan",
                            return_value=execution_result,
                        ):
                            with patch.object(
                                test_module.subject_validations_module,
                                "run_subject_validations",
                                side_effect=empty_validation_outcome,
                            ):
                                result = test_module.handle(
                                    {
                                        "id": "declared-unit-test-alias",
                                        "kind": "registry-object-alias",
                                        "registry_object_kind": "declared-unit-test",
                                        "registry_object_id": f"declared-unit-test/{stable_id}",
                                    },
                                    repo_root,
                                    "windows",
                                    f"test declared-unit-test --id declared-unit-test/{stable_id}",
                                    manifest,
                                )

            self.assertEqual("ok", result.status)
            self.assertEqual(f"declared-unit-test/{stable_id}", result.target)
            self.assertEqual(
                f"declared-unit-test/{stable_id}",
                result.payload["selectedObject"]["id"],
            )
            build_plan_mock.assert_called_once()
            self.assertEqual(repo_root, build_plan_mock.call_args.args[0])
            self.assertEqual(subject_id, build_plan_mock.call_args.args[1])
            self.assertEqual(source_entry, build_plan_mock.call_args.kwargs["source_entry"])
            self.assertIsNone(build_plan_mock.call_args.kwargs["workload_entry"])
            self.assertEqual(
                {
                    "family": "declared-unit-test",
                    "stableId": stable_id,
                    "alias": alias,
                },
                build_plan_mock.call_args.kwargs["entry_selection"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_declared_benchmark_dispatch_passes_workload_entry_selection_to_subject_planner(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_declared_benchmark_alias")
        repo_root = make_temp_repo_root("subject-command", "declared-benchmark-alias")
        manifest: dict[str, Any] = {}
        subject_id = "SolutionCorePack"
        stable_id = "solution-core-benchmark"
        alias = "solution-core-benchmark"
        source_entry = "SolutionCoreBenchmarks/DispatchBenchmarks::Proof()"
        workload_entry = "SolutionCoreBenchmarks/DispatchBenchmarks::Workload()"
        fixed_run_id = "chaos-run-declared-benchmark-alias"
        registry_index = make_declared_registry_index(
            test_module,
            subject_id=subject_id,
            object_type="declared-benchmark",
            stable_id=stable_id,
            alias=alias,
            source_entry=source_entry,
            workload_entry=workload_entry,
            default_goal_id="perf.release",
            matrix_id="windows-native-perf",
        )
        plan = make_subject_plan(
            subject_id=subject_id,
            run_id=fixed_run_id,
            matrix_id="windows-native-perf",
            goal_id="perf.release",
            stage_kind="runtime-observe",
        )
        execution_result = build_execution_result(
            plan,
            stage_kind="runtime-observe",
            duration_ms=4,
            fingerprint="declared-benchmark-alias-fingerprint",
        )

        try:
            with patch.object(test_module, "_scan_registry", return_value=registry_index):
                with patch.object(test_module.subject_planner_module, "build_plan", return_value=plan) as build_plan_mock:
                    with patch.object(test_module.reporting_module, "build_run_id", return_value=fixed_run_id):
                        with patch.object(
                            test_module.subject_executor_module,
                            "execute_plan",
                            return_value=execution_result,
                        ):
                            with patch.object(
                                test_module.subject_validations_module,
                                "run_subject_validations",
                                side_effect=empty_validation_outcome,
                            ):
                                result = test_module.handle(
                                    {
                                        "id": "declared-benchmark-alias",
                                        "kind": "registry-object-alias",
                                        "registry_object_kind": "declared-benchmark",
                                        "registry_object_id": f"declared-benchmark/{stable_id}",
                                    },
                                    repo_root,
                                    "windows",
                                    f"test declared-benchmark --id declared-benchmark/{stable_id}",
                                    manifest,
                                )

            self.assertEqual("ok", result.status)
            self.assertEqual(f"declared-benchmark/{stable_id}", result.target)
            self.assertEqual(
                f"declared-benchmark/{stable_id}",
                result.payload["selectedObject"]["id"],
            )
            build_plan_mock.assert_called_once()
            self.assertEqual(repo_root, build_plan_mock.call_args.args[0])
            self.assertEqual(subject_id, build_plan_mock.call_args.args[1])
            self.assertEqual(source_entry, build_plan_mock.call_args.kwargs["source_entry"])
            self.assertEqual(workload_entry, build_plan_mock.call_args.kwargs["workload_entry"])
            self.assertEqual(
                {
                    "family": "declared-benchmark",
                    "stableId": stable_id,
                    "alias": alias,
                },
                build_plan_mock.call_args.kwargs["entry_selection"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)


if __name__ == "__main__":
    unittest.main()
