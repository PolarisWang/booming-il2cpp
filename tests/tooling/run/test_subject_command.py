from __future__ import annotations

import importlib.util
import json
import shutil
import sys
import unittest
import uuid
from pathlib import Path
from unittest.mock import patch

from tests.support import select_subject_record


REPO_ROOT = Path(__file__).resolve().parents[3]
TEST_COMMAND_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "commands" / "test.py"
MANIFEST_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "manifest.py"
RUN_MANIFEST_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "run_manifest.json"


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

def snapshot_text(path: Path) -> str | None:
    if not path.is_file():
        return None
    return path.read_text(encoding="utf-8")


def restore_text(path: Path, snapshot: str | None) -> None:
    if snapshot is None:
        try:
            path.unlink()
        except OSError:
            pass
        return
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(snapshot, encoding="utf-8")


class SubjectCommandTests(unittest.TestCase):
    def test_subject_dispatch_routes_to_subject_planner_and_executor(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_subject_dispatch")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_subject_dispatch")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)
        subject_record = select_subject_record(
            "chaos_run_subject_fixture_dispatch",
            source_type="dotnet-project",
            required_host_platforms=["windows-x64"],
        )
        subject_id = str(subject_record["subjectId"])
        fixed_run_id = f"chaos-run-subject-dispatch-{uuid.uuid4().hex}"
        observed_selection: dict[str, str] = {}
        subject_runs_root = REPO_ROOT / "artifacts" / "subjects" / subject_id / "runs"
        run_root = subject_runs_root / fixed_run_id
        subject_last_path = subject_runs_root / "last.json"
        subject_current_path = subject_runs_root / "current.json"
        global_logs_root = REPO_ROOT / "artifacts" / "logs" / "tests"
        global_last_path = global_logs_root / "last.json"
        global_current_path = global_logs_root / "current.json"
        snapshots = {
            subject_last_path: snapshot_text(subject_last_path),
            subject_current_path: snapshot_text(subject_current_path),
            global_last_path: snapshot_text(global_last_path),
            global_current_path: snapshot_text(global_current_path),
        }

        def execute_plan_side_effect(repo_root: Path, plan: dict, **_: object) -> dict:
            del repo_root
            selection = dict(plan.get("selection") or {})
            observed_selection.update(
                {
                    "subjectId": str(selection.get("subjectId") or ""),
                    "matrixId": str(selection.get("matrixId") or ""),
                    "goalId": str(selection.get("goalId") or ""),
                    "terminalBucket": str(dict(selection.get("artifactPlan") or {}).get("evidenceTerminalBucket") or ""),
                }
            )
            non_report_stages = [
                dict(stage)
                for stage in list(plan.get("stagePlan") or [])
                if str(stage.get("bucket") or "") != "report"
            ]
            if not non_report_stages:
                raise AssertionError("expected at least one non-report stage")

            first_stage = non_report_stages[0]
            terminal_stage = non_report_stages[-1]
            stage_results = [
                {
                    "stageId": str(first_stage["stageId"]),
                    "kind": str(first_stage["kind"]),
                    "bucket": str(first_stage["bucket"]),
                    "status": "ok",
                    "planMode": "executed",
                    "actionTaken": "executed",
                    "invalidation": {"applied": False, "reason": None},
                    "manifestPath": str(first_stage["paths"]["manifestPath"]),
                    "reportPaths": list(first_stage["paths"]["reportPaths"]),
                    "primaryEvidencePaths": [],
                    "fingerprint": f"{first_stage['stageId']}-fingerprint",
                    "durationMs": 1,
                    "diagnostics": {},
                    "failure": None,
                }
            ]
            if str(terminal_stage["stageId"]) != str(first_stage["stageId"]):
                stage_results.append(
                    {
                        "stageId": str(terminal_stage["stageId"]),
                        "kind": str(terminal_stage["kind"]),
                        "bucket": str(terminal_stage["bucket"]),
                        "status": "ok",
                        "planMode": "executed",
                        "actionTaken": "executed",
                        "invalidation": {"applied": False, "reason": None},
                        "manifestPath": str(terminal_stage["paths"]["manifestPath"]),
                        "reportPaths": list(terminal_stage["paths"]["reportPaths"]),
                        "primaryEvidencePaths": [f"{terminal_stage['paths']['bucketRoot']}/stdout.log"],
                        "fingerprint": f"{terminal_stage['stageId']}-fingerprint",
                        "durationMs": 2,
                        "diagnostics": {},
                        "failure": None,
                    }
                )
            return {
                "subjectId": observed_selection["subjectId"],
                "matrixId": observed_selection["matrixId"],
                "goalId": observed_selection["goalId"],
                "status": "ok",
                "terminalStageId": str(terminal_stage["stageId"]),
                "terminalBucket": observed_selection["terminalBucket"],
                "stageResults": stage_results,
                "errors": [],
                "events": [],
            }

        with patch.object(
            test_module,
            "_execute_public_test_session",
            side_effect=AssertionError("legacy suite session should not run for test subject"),
        ) as legacy_session:
            try:
                with patch.object(test_module.reporting_module, "build_run_id", return_value=fixed_run_id):
                    with patch.object(
                        test_module.subject_executor_module,
                        "execute_plan",
                        side_effect=execute_plan_side_effect,
                    ) as execute_plan:
                        result = test_module.handle(
                            {"id": "test-subject", "handler": "test.dispatch"},
                            REPO_ROOT,
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
                self.assertTrue((REPO_ROOT / result.payload["subjectResults"][0]["subjectSummaryPath"]).is_file())
                legacy_session.assert_not_called()
                execute_plan.assert_called_once()
                self.assertEqual(result.payload["runId"], execute_plan.call_args.kwargs["run_id"])
                self.assertTrue(callable(execute_plan.call_args.kwargs["event_writer"]))
                self.assertEqual(
                    f"artifacts/subjects/{subject_id}/runs/{run_id}/run-report/summary.json",
                    result.payload["summaryPath"],
                )
                subject_last = json.loads(subject_last_path.read_text(encoding="utf-8"))
                self.assertEqual(run_id, subject_last["runId"])
                self.assertEqual(
                    f"artifacts/subjects/{subject_id}/runs/{run_id}/run-report/summary.json",
                    subject_last["summaryPath"],
                )
                global_last = json.loads(global_last_path.read_text(encoding="utf-8"))
                self.assertEqual(run_id, global_last["runId"])
                self.assertEqual(
                    f"artifacts/subjects/{subject_id}/runs/{run_id}/run-report/summary.json",
                    global_last["summaryPath"],
                )
                if subject_current_path.is_file():
                    subject_current = json.loads(subject_current_path.read_text(encoding="utf-8"))
                    self.assertEqual(run_id, subject_current["runId"])
                    self.assertEqual("ok", subject_current["status"])
                if global_current_path.is_file():
                    global_current = json.loads(global_current_path.read_text(encoding="utf-8"))
                    self.assertEqual(run_id, global_current["runId"])
                    self.assertEqual("ok", global_current["status"])
            finally:
                shutil.rmtree(run_root, ignore_errors=True)
                for path, snapshot in snapshots.items():
                    restore_text(path, snapshot)

    def test_subject_dispatch_collects_perf_release_report_artifacts_for_perf_subject(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_subject_perf_dispatch")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_subject_perf_dispatch")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)
        subject_record = select_subject_record(
            "chaos_run_subject_fixture_perf_dispatch",
            source_type="dotnet-project",
            required_host_platforms=["windows-x64"],
            required_goal_ids=["perf.release"],
            required_validation_kinds=["perf"],
            required_validation_drivers=["csharp-perf-harness"],
        )
        subject_id = str(subject_record["subjectId"])
        fixed_run_id = f"chaos-run-subject-perf-release-{uuid.uuid4().hex}"
        observed_selection: dict[str, str] = {}
        subject_runs_root = REPO_ROOT / "artifacts" / "subjects" / subject_id / "runs"
        run_root = subject_runs_root / fixed_run_id
        subject_last_path = subject_runs_root / "last.json"
        subject_current_path = subject_runs_root / "current.json"
        global_logs_root = REPO_ROOT / "artifacts" / "logs" / "tests"
        global_last_path = global_logs_root / "last.json"
        global_current_path = global_logs_root / "current.json"
        snapshots = {
            subject_last_path: snapshot_text(subject_last_path),
            subject_current_path: snapshot_text(subject_current_path),
            global_last_path: snapshot_text(global_last_path),
            global_current_path: snapshot_text(global_current_path),
        }

        def execute_plan_side_effect(repo_root: Path, plan: dict, **_: object) -> dict:
            del repo_root
            selection = dict(plan.get("selection") or {})
            observed_selection.update(
                {
                    "subjectId": str(selection.get("subjectId") or ""),
                    "matrixId": str(selection.get("matrixId") or ""),
                    "goalId": str(selection.get("goalId") or ""),
                    "validationProfileId": str(selection.get("validationProfileId") or ""),
                    "variant": str(selection.get("variant") or ""),
                }
            )
            perf_stage = next(
                dict(stage)
                for stage in list(plan.get("stagePlan") or [])
                if str(stage.get("kind") or "") == "runtime-perf-collect"
            )
            return {
                "subjectId": observed_selection["subjectId"],
                "matrixId": observed_selection["matrixId"],
                "goalId": observed_selection["goalId"],
                "status": "ok",
                "terminalStageId": str(perf_stage["stageId"]),
                "terminalBucket": str(perf_stage["bucket"]),
                "stageResults": [
                    {
                        "stageId": str(perf_stage["stageId"]),
                        "kind": str(perf_stage["kind"]),
                        "bucket": str(perf_stage["bucket"]),
                        "status": "ok",
                        "planMode": "executed",
                        "actionTaken": "executed",
                        "invalidation": {"applied": False, "reason": None},
                        "manifestPath": str(perf_stage["paths"]["manifestPath"]),
                        "reportPaths": list(perf_stage["paths"]["reportPaths"]),
                        "primaryEvidencePaths": [
                            f"{perf_stage['paths']['bucketRoot']}/stdout.log",
                        ],
                        "fingerprint": "runtime-fingerprint",
                        "durationMs": 50,
                        "diagnostics": {},
                        "details": {
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
                        "failure": None,
                    },
                ],
                "errors": [],
                "events": [],
            }

        try:
            with patch.object(test_module.reporting_module, "build_run_id", return_value=fixed_run_id):
                with patch.object(
                    test_module.subject_executor_module,
                    "execute_plan",
                    side_effect=execute_plan_side_effect,
                ):
                    result = test_module.handle(
                        {"id": "test-subject", "handler": "test.dispatch"},
                        REPO_ROOT,
                        "windows",
                        f"test subject --id subject/{subject_id} --goal perf.release --validation-profile perf-release --variant SHIP",
                        manifest,
                        {
                            "id": f"subject/{subject_id}",
                            "goal": "perf.release",
                            "validation_profile": "perf-release",
                            "variant": "SHIP",
                        },
                    )

            self.assertEqual("ok", result.status)
            self.assertEqual(f"subject/{subject_id}", result.target)
            run_id = result.payload["runId"]
            matrix_id = observed_selection["matrixId"]
            self.assertEqual("perf-release", observed_selection["validationProfileId"])
            self.assertEqual("SHIP", observed_selection["variant"])
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
            shutil.rmtree(run_root, ignore_errors=True)
            for path, snapshot in snapshots.items():
                restore_text(path, snapshot)

    def test_subject_dispatch_runs_subject_owned_xunit_validation_and_surfaces_validation_artifacts(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_subject_unit_validation")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_subject_unit_validation")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)
        subject_record = select_subject_record(
            "chaos_run_subject_fixture_unit_validation",
            category="canonical",
            source_type="dotnet-project",
            required_validation_kinds=["unit"],
            required_validation_profile_ids=["proof-dev"],
            required_validation_frameworks=["xunit"],
            required_host_platforms=["windows-x64"],
        )
        subject_id = str(subject_record["subjectId"])
        fixed_run_id = f"chaos-run-subject-unit-validation-{uuid.uuid4().hex}"
        subject_runs_root = REPO_ROOT / "artifacts" / "subjects" / subject_id / "runs"
        run_root = subject_runs_root / fixed_run_id
        subject_last_path = subject_runs_root / "last.json"
        subject_current_path = subject_runs_root / "current.json"
        global_logs_root = REPO_ROOT / "artifacts" / "logs" / "tests"
        global_last_path = global_logs_root / "last.json"
        global_current_path = global_logs_root / "current.json"
        snapshots = {
            subject_last_path: snapshot_text(subject_last_path),
            subject_current_path: snapshot_text(subject_current_path),
            global_last_path: snapshot_text(global_last_path),
            global_current_path: snapshot_text(global_current_path),
        }

        def execute_plan_side_effect(repo_root: Path, plan: dict, **_: object) -> dict:
            del repo_root
            runtime_stage = next(
                dict(stage)
                for stage in list(plan.get("stagePlan") or [])
                if str(stage.get("bucket") or "") == "runtime"
            )
            return {
                "subjectId": str(plan["selection"]["subjectId"]),
                "matrixId": str(plan["selection"]["matrixId"]),
                "goalId": str(plan["selection"]["goalId"]),
                "status": "ok",
                "terminalStageId": str(runtime_stage["stageId"]),
                "terminalBucket": str(runtime_stage["bucket"]),
                "stageResults": [
                    {
                        "stageId": str(runtime_stage["stageId"]),
                        "kind": str(runtime_stage["kind"]),
                        "bucket": str(runtime_stage["bucket"]),
                        "status": "ok",
                        "planMode": "executed",
                        "actionTaken": "executed",
                        "invalidation": {"applied": False, "reason": None},
                        "manifestPath": str(runtime_stage["paths"]["manifestPath"]),
                        "reportPaths": list(runtime_stage["paths"]["reportPaths"]),
                        "primaryEvidencePaths": [f"{runtime_stage['paths']['bucketRoot']}/stdout.log"],
                        "fingerprint": "runtime-fingerprint",
                        "durationMs": 20,
                        "diagnostics": {},
                        "failure": None,
                    }
                ],
                "errors": [],
                "events": [],
            }

        def validation_side_effect(repo_root: Path, plan: dict, *, run_id: str) -> dict:
            del plan
            summary_path = (
                repo_root
                / "artifacts"
                / "subjects"
                / subject_id
                / "runs"
                / run_id
                / "matrices"
                / "windows-dev-output"
                / "validations"
                / "unit"
                / "summary.json"
            )
            trx_path = summary_path.parent / "results.trx"
            summary_path.parent.mkdir(parents=True, exist_ok=True)
            summary_path.write_text("{}", encoding="utf-8")
            trx_path.write_text("", encoding="utf-8")
            return {
                "status": "ok",
                "validationResults": [
                    {
                        "validationKind": "unit",
                        "framework": "xunit",
                        "matrixId": "windows-dev-output",
                        "status": "ok",
                        "summaryPath": str(summary_path.relative_to(repo_root).as_posix()),
                        "trxPath": str(trx_path.relative_to(repo_root).as_posix()),
                        "artifacts": [
                            str(summary_path.relative_to(repo_root).as_posix()),
                            str(trx_path.relative_to(repo_root).as_posix()),
                        ],
                    }
                ],
                "artifacts": [
                    str(summary_path.relative_to(repo_root).as_posix()),
                    str(trx_path.relative_to(repo_root).as_posix()),
                ],
                "errors": [],
            }

        try:
            with patch.object(test_module.reporting_module, "build_run_id", return_value=fixed_run_id):
                with patch.object(
                    test_module.subject_executor_module,
                    "execute_plan",
                    side_effect=execute_plan_side_effect,
                ):
                    with patch.object(
                        test_module.subject_validations_module,
                        "run_subject_validations",
                        side_effect=validation_side_effect,
                    ):
                        result = test_module.handle(
                            {"id": "test-subject", "handler": "test.dispatch"},
                            REPO_ROOT,
                            "windows",
                            f"test subject --id subject/{subject_id}",
                            manifest,
                            {"id": f"subject/{subject_id}"},
                        )

            self.assertEqual("ok", result.status)
            run_id = result.payload["runId"]
            matrix_result = result.payload["subjectResults"][0]
            self.assertIn("validationResults", result.payload)
            self.assertEqual(1, len(result.payload["validationResults"]))
            validation_result = result.payload["validationResults"][0]
            self.assertEqual("unit", validation_result["validationKind"])
            self.assertEqual("xunit", validation_result["framework"])
            self.assertEqual("ok", validation_result["status"])
            self.assertEqual(
                f"artifacts/subjects/{subject_id}/runs/{run_id}/matrices/{validation_result['matrixId']}/validations/unit/summary.json",
                validation_result["summaryPath"],
            )
            self.assertIn(validation_result["summaryPath"], result.payload["artifacts"])
            self.assertIn(validation_result["trxPath"], result.payload["artifacts"])
            self.assertTrue((REPO_ROOT / validation_result["summaryPath"]).is_file())
            self.assertEqual(subject_id, matrix_result["subjectId"])
        finally:
            shutil.rmtree(run_root, ignore_errors=True)
            for path, snapshot in snapshots.items():
                restore_text(path, snapshot)

    def test_subject_dispatch_collects_native_perf_report_artifacts_for_mainline_subject(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_subject_native_perf_dispatch")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_subject_native_perf_dispatch")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)
        subject_record = select_subject_record(
            "chaos_run_subject_fixture_native_perf_dispatch",
            category="mainline",
            source_type="dotnet-project",
            required_goal_ids=["perf.profile"],
            required_stage_kinds=["native-runtime-perf"],
            required_validation_profile_ids=["perf-profile"],
            required_validation_drivers=["native-runtime-perf"],
        )
        subject_id = str(subject_record["subjectId"])
        fixed_run_id = f"chaos-run-subject-native-perf-{uuid.uuid4().hex}"
        observed_selection: dict[str, str] = {}
        subject_runs_root = REPO_ROOT / "artifacts" / "subjects" / subject_id / "runs"
        run_root = subject_runs_root / fixed_run_id
        subject_last_path = subject_runs_root / "last.json"
        subject_current_path = subject_runs_root / "current.json"
        global_logs_root = REPO_ROOT / "artifacts" / "logs" / "tests"
        global_last_path = global_logs_root / "last.json"
        global_current_path = global_logs_root / "current.json"
        snapshots = {
            subject_last_path: snapshot_text(subject_last_path),
            subject_current_path: snapshot_text(subject_current_path),
            global_last_path: snapshot_text(global_last_path),
            global_current_path: snapshot_text(global_current_path),
        }

        def execute_plan_side_effect(repo_root: Path, plan: dict, **_: object) -> dict:
            del repo_root
            selection = dict(plan.get("selection") or {})
            observed_selection.update(
                {
                    "subjectId": str(selection.get("subjectId") or ""),
                    "matrixId": str(selection.get("matrixId") or ""),
                    "goalId": str(selection.get("goalId") or ""),
                    "validationProfileId": str(selection.get("validationProfileId") or ""),
                    "variant": str(selection.get("variant") or ""),
                }
            )
            native_perf_stage = next(
                dict(stage)
                for stage in list(plan.get("stagePlan") or [])
                if str(stage.get("kind") or "") == "native-runtime-perf"
            )
            return {
                "subjectId": observed_selection["subjectId"],
                "matrixId": observed_selection["matrixId"],
                "goalId": observed_selection["goalId"],
                "status": "ok",
                "terminalStageId": str(native_perf_stage["stageId"]),
                "terminalBucket": "report",
                "stageResults": [
                    {
                        "stageId": str(native_perf_stage["stageId"]),
                        "kind": str(native_perf_stage["kind"]),
                        "bucket": str(native_perf_stage["bucket"]),
                        "status": "ok",
                        "planMode": "executed",
                        "actionTaken": "executed",
                        "invalidation": {"applied": False, "reason": None},
                        "manifestPath": str(native_perf_stage["paths"]["manifestPath"]),
                        "reportPaths": [],
                        "primaryEvidencePaths": [
                            f"{native_perf_stage['paths']['bucketRoot']}/perf.runtime.json",
                            f"{native_perf_stage['paths']['bucketRoot']}/perf.samples.json",
                        ],
                        "fingerprint": "native-perf-fingerprint",
                        "durationMs": 42,
                        "diagnostics": {},
                        "details": {
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
                                    "runtimePath": f"{native_perf_stage['paths']['bucketRoot']}/perf.runtime.json",
                                    "samplesPath": f"{native_perf_stage['paths']['bucketRoot']}/perf.samples.json",
                                },
                            }
                        },
                        "failure": None,
                    },
                ],
                "errors": [],
                "events": [],
            }

        try:
            with patch.object(test_module.reporting_module, "build_run_id", return_value=fixed_run_id):
                with patch.object(
                    test_module.subject_executor_module,
                    "execute_plan",
                    side_effect=execute_plan_side_effect,
                ):
                    result = test_module.handle(
                        {"id": "test-subject", "handler": "test.dispatch"},
                        REPO_ROOT,
                        "windows",
                        f"test subject --id subject/{subject_id} --goal perf.profile --validation-profile perf-profile --variant PROFILE",
                        manifest,
                        {
                            "id": f"subject/{subject_id}",
                            "goal": "perf.profile",
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
            shutil.rmtree(run_root, ignore_errors=True)
            for path, snapshot in snapshots.items():
                restore_text(path, snapshot)


if __name__ == "__main__":
    unittest.main()
