from tests.tooling.run.subject_command_test_support import *


class TestSubjectCommandDispatchExecution(SubjectCommandTestSupport):
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
