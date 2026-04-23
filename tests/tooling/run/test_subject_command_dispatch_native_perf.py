from tests.tooling.run.subject_command_test_support import *


class TestSubjectCommandDispatchNativePerf(SubjectCommandTestSupport):
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
