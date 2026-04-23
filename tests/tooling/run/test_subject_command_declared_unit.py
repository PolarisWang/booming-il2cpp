from tests.tooling.run.subject_command_test_support import *


class TestSubjectCommandDeclaredUnit(SubjectCommandTestSupport):
    def test_declared_unit_test_dispatch_uses_workspace_entry_selection_without_source_entry_routing(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_declared_unit_alias")
        repo_root = make_temp_repo_root("subject-command", "declared-unit-alias")
        manifest: dict[str, Any] = {}
        subject_id = "SolutionCorePack"
        stable_id = "solution-core-proof"
        alias = "solution-core-proof"
        source_entry = "CoreRuntimeFeatures/ProofEntry::Run()"
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
        write_subject_workspace(
            repo_root,
            subject_id=subject_id,
            matrix_id="workspace-proof-matrix",
            goal_ids=["correctness.dev"],
            unit_stable_id=stable_id,
            unit_alias=alias,
            unit_entry_index=7,
            benchmark_stable_id=f"{subject_id}::Benchmarks::{subject_id}.Benchmarks::Run()",
            benchmark_alias="workspace-benchmark",
            benchmark_entry_index=9,
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
                                        "id": "test-declared-unit-test",
                                        "handler": "test.dispatch",
                                    },
                                    repo_root,
                                    "windows",
                                    f"test declared-unit-test --id declared-unit-test/{stable_id}",
                                    manifest,
                                    {"id": f"declared-unit-test/{stable_id}"},
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
            self.assertEqual("workspace-proof-matrix", build_plan_mock.call_args.kwargs["matrix_id"])
            self.assertIsNone(build_plan_mock.call_args.kwargs["source_entry"])
            self.assertIsNone(build_plan_mock.call_args.kwargs["workload_entry"])
            self.assertEqual(
                {
                    "family": "declared-unit-test",
                    "stableId": stable_id,
                    "alias": alias,
                    "entryIndex": 7,
                },
                build_plan_mock.call_args.kwargs["entry_selection"],
            )
            self.assertEqual(7, result.payload["workspaceExecution"]["entryIndex"])
            self.assertEqual("proof-host", result.payload["workspaceExecution"]["hostKind"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_declared_unit_test_dispatch_allows_managed_only_workspace_for_proof_host(self) -> None:
        test_module = load_module(
            TEST_COMMAND_MODULE_PATH,
            "chaos_run_test_command_declared_unit_managed_only_proof",
        )
        repo_root = make_temp_repo_root("subject-command", "declared-unit-managed-only-proof")
        manifest: dict[str, Any] = {}
        subject_id = "SolutionCorePack"
        stable_id = "solution-core-proof"
        alias = "solution-core-proof"
        source_entry = "CoreRuntimeFeatures/ProofEntry::Run()"
        fixed_run_id = "chaos-run-declared-unit-managed-only-proof"
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
            fingerprint="declared-unit-managed-only-proof-fingerprint",
        )
        write_subject_workspace(
            repo_root,
            subject_id=subject_id,
            matrix_id="workspace-proof-matrix",
            goal_ids=["correctness.dev"],
            unit_stable_id=stable_id,
            unit_alias=alias,
            unit_entry_index=7,
            benchmark_stable_id=f"{subject_id}::Benchmarks::{subject_id}.Benchmarks::Run()",
            benchmark_alias="workspace-benchmark",
            benchmark_entry_index=9,
            include_native_proof=False,
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
                                        "id": "test-declared-unit-test",
                                        "handler": "test.dispatch",
                                    },
                                    repo_root,
                                    "windows",
                                    f"test declared-unit-test --id declared-unit-test/{stable_id}",
                                    manifest,
                                    {"id": f"declared-unit-test/{stable_id}"},
                                )

            self.assertEqual("ok", result.status)
            build_plan_mock.assert_called_once()
            self.assertEqual("workspace-proof-matrix", build_plan_mock.call_args.kwargs["matrix_id"])
            self.assertEqual(7, result.payload["workspaceExecution"]["entryIndex"])
            self.assertEqual("proof-host", result.payload["workspaceExecution"]["hostKind"])
            self.assertIsNone(result.payload["workspaceExecution"]["nativeTestProject"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_declared_unit_test_dispatch_carries_registry_entry_index_without_workspace_resolution(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_declared_unit_registry_entry_index")
        repo_root = make_temp_repo_root("subject-command", "declared-unit-registry-entry-index")
        manifest: dict[str, Any] = {}
        subject_id = "SolutionCorePack"
        stable_id = "solution-core-proof"
        alias = "solution-core-proof"
        source_entry = "CoreRuntimeFeatures/ProofEntry::Run()"
        fixed_run_id = "chaos-run-declared-unit-registry-entry-index"
        registry_index = make_declared_registry_index(
            test_module,
            subject_id=subject_id,
            object_type="declared-unit-test",
            stable_id=stable_id,
            alias=alias,
            source_entry=source_entry,
            workload_entry="should-be-cleared-for-unit-tests",
            entry_index=5,
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
            fingerprint="declared-unit-registry-entry-index-fingerprint",
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
                                        "id": "test-declared-unit-test",
                                        "handler": "test.dispatch",
                                    },
                                    repo_root,
                                    "windows",
                                    f"test declared-unit-test --id declared-unit-test/{stable_id}",
                                    manifest,
                                    {"id": f"declared-unit-test/{stable_id}"},
                                )

            self.assertEqual("ok", result.status)
            self.assertEqual(source_entry, build_plan_mock.call_args.kwargs["source_entry"])
            self.assertIsNone(build_plan_mock.call_args.kwargs["workload_entry"])
            self.assertEqual(
                {
                    "family": "declared-unit-test",
                    "stableId": stable_id,
                    "alias": alias,
                    "entryIndex": 5,
                },
                build_plan_mock.call_args.kwargs["entry_selection"],
            )
            self.assertNotIn("workspaceExecution", result.payload)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
