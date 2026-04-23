from tests.tooling.run.verify_roadmap0_test_support import *


class VerifyRoadmap0SubjectRuntimeTests(VerifyRoadmap0TestSupport):
    def test_low_level_script_execute_subject_matrix_builds_plan_and_executes_it(self) -> None:
        script_module = load_module(VERIFY_SCRIPT_PATH, "chaos_verify_roadmap0_script_subject_matrix")
        plan = {"selection": {"artifactPlan": {"evidenceTerminalBucket": "runtime"}}}
        result = {"status": "ok", "errors": [], "stageResults": []}

        with patch.object(script_module, "resolve_subject_matrix_subject_id", return_value="FixtureReferenceSubject"):
            with patch.object(script_module, "build_subject_run_id", return_value="fixture-run-001"):
                with patch.object(script_module.subject_planner_module, "build_plan", return_value=plan) as build_plan_mock:
                    with patch.object(script_module.subject_executor_module, "execute_plan", return_value=result) as execute_plan_mock:
                        actual = script_module.execute_subject_matrix(
                            REPO_ROOT,
                            matrix_id="windows-dev-output",
                            goal_id="correctness.dev",
                        )

        self.assertEqual({**result, "runId": "fixture-run-001"}, actual)
        build_plan_mock.assert_called_once_with(
            REPO_ROOT,
            "FixtureReferenceSubject",
            goal_id="correctness.dev",
            matrix_id="windows-dev-output",
            run_id="fixture-run-001",
        )
        execute_plan_mock.assert_called_once_with(REPO_ROOT, plan, run_id="fixture-run-001")

    def test_low_level_script_raises_when_subject_matrix_execution_fails(self) -> None:
        script_module = load_module(VERIFY_SCRIPT_PATH, "chaos_verify_roadmap0_script_subject_matrix_fail")
        plan = {"selection": {"artifactPlan": {"evidenceTerminalBucket": "runtime"}}}
        result = {"status": "fail", "errors": ["worker boom"], "stageResults": []}

        with patch.object(script_module.subject_planner_module, "build_plan", return_value=plan):
            with patch.object(script_module.subject_executor_module, "execute_plan", return_value=result):
                with self.assertRaisesRegex(RuntimeError, "subject matrix failed: windows-dev-output"):
                    script_module.execute_subject_matrix(
                        REPO_ROOT,
                        matrix_id="windows-dev-output",
                        goal_id="correctness.dev",
                    )

    def test_low_level_script_accepts_stage4_proof_run_artifacts_when_runtime_root_contains_expected_files(self) -> None:
        script_module = load_module(VERIFY_SCRIPT_PATH, "chaos_verify_roadmap0_script_stage4_run_artifacts_ok")

        runtime_root = self._make_test_dir("stage4-runtime-ok")
        try:
            (runtime_root / "stdout.log").write_text("", encoding="utf-8")
            (runtime_root / "stderr.log").write_text("", encoding="utf-8")
            (runtime_root / "exit-code.txt").write_text("0\n", encoding="utf-8")

            script_module.validate_stage4_proof_run_artifacts(runtime_root)
        finally:
            shutil.rmtree(runtime_root, ignore_errors=True)

    def test_low_level_script_accepts_stage4_proof_run_artifacts_when_legacy_root_contains_run_subdir(self) -> None:
        script_module = load_module(VERIFY_SCRIPT_PATH, "chaos_verify_roadmap0_script_stage4_run_artifacts_legacy_ok")

        legacy_root = self._make_test_dir("stage4-runtime-legacy")
        try:
            run_root = legacy_root / "run"
            run_root.mkdir(parents=True, exist_ok=True)
            (run_root / "stdout.log").write_text("proof output moved to in-process asserts\n", encoding="utf-8")
            (run_root / "stderr.log").write_text("", encoding="utf-8")
            (run_root / "exit-code.txt").write_text("0\n", encoding="utf-8")

            script_module.validate_stage4_proof_run_artifacts(legacy_root)
        finally:
            shutil.rmtree(legacy_root, ignore_errors=True)

    def test_low_level_script_rejects_stage4_proof_run_exit_code_mismatch(self) -> None:
        script_module = load_module(VERIFY_SCRIPT_PATH, "chaos_verify_roadmap0_script_stage4_run_artifacts_exit")

        runtime_root = self._make_test_dir("stage4-runtime-exit")
        try:
            (runtime_root / "stdout.log").write_text("assertion failure details\n", encoding="utf-8")
            (runtime_root / "stderr.log").write_text("", encoding="utf-8")
            (runtime_root / "exit-code.txt").write_text("1\n", encoding="utf-8")

            with self.assertRaisesRegex(RuntimeError, "exit code"):
                script_module.validate_stage4_proof_run_artifacts(runtime_root)
        finally:
            shutil.rmtree(runtime_root, ignore_errors=True)

    def test_low_level_script_accepts_stage4_proof_run_with_noncanonical_stdout(self) -> None:
        script_module = load_module(VERIFY_SCRIPT_PATH, "chaos_verify_roadmap0_script_stage4_run_artifacts_stdout")

        runtime_root = self._make_test_dir("stage4-runtime-stdout")
        try:
            (runtime_root / "stdout.log").write_text("unexpected output\n", encoding="utf-8")
            (runtime_root / "stderr.log").write_text("", encoding="utf-8")
            (runtime_root / "exit-code.txt").write_text("0\n", encoding="utf-8")

            script_module.validate_stage4_proof_run_artifacts(runtime_root)
        finally:
            shutil.rmtree(runtime_root, ignore_errors=True)
