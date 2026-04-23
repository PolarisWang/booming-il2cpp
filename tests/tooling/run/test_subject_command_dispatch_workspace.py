from tests.tooling.run.subject_command_test_support import *


class TestSubjectCommandDispatchWorkspace(SubjectCommandTestSupport):
    def test_subject_dispatch_prefers_workspace_manifest_v2_for_host_resolution(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_subject_workspace_cutover")
        repo_root = make_temp_repo_root("subject-command", "workspace-cutover")
        manifest: dict[str, Any] = {}
        subject_id = "FixtureWorkspaceSubject"
        fixed_run_id = "chaos-run-subject-workspace-cutover"
        workspace_matrix_id = "workspace-proof-matrix"
        observed_selection: dict[str, str] = {}
        registry_index = make_subject_registry_index(
            test_module,
            subject_id=subject_id,
            default_goal_id="correctness.dev",
            matrix_id="legacy-proof-matrix",
        )
        write_subject_workspace(
            repo_root,
            subject_id=subject_id,
            matrix_id=workspace_matrix_id,
            goal_ids=["correctness.dev"],
            unit_stable_id=f"{subject_id}::Proofs::{subject_id}.Proofs::Run()",
            unit_alias="workspace-proof",
            unit_entry_index=5,
            benchmark_stable_id=f"{subject_id}::Benchmarks::{subject_id}.Benchmarks::Run()",
            benchmark_alias="workspace-benchmark",
            benchmark_entry_index=8,
        )
        build_plan = make_build_plan_side_effect(
            observed_selection,
            subject_id=subject_id,
            expected_run_id=fixed_run_id,
            default_goal_id="correctness.dev",
            default_matrix_id=workspace_matrix_id,
        )

        def execute_plan_side_effect(repo_root: Path, plan: dict, **_: object) -> dict:
            del repo_root
            return build_execution_result(
                plan,
                stage_kind="runtime-observe",
                duration_ms=2,
                fingerprint="workspace-cutover-fingerprint",
            )

        try:
            with patch.object(test_module, "_scan_registry", return_value=registry_index):
                with patch.object(test_module.subject_planner_module, "build_plan", side_effect=build_plan) as build_plan_mock:
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
            build_plan_mock.assert_called_once()
            self.assertEqual(workspace_matrix_id, build_plan_mock.call_args.kwargs["matrix_id"])
            self.assertEqual(workspace_matrix_id, observed_selection["matrixId"])
            self.assertEqual(
                {
                    "subjectId": subject_id,
                    "workspaceManifestPath": f"verification/workspaces/subjects/{subject_id}/workspace.manifest.json",
                    "workspaceVersion": 2,
                    "goalId": "correctness.dev",
                    "matrixId": workspace_matrix_id,
                    "hostKind": "proof-host",
                    "collectionPath": f"verification/workspaces/subjects/{subject_id}/managed-tests/Generated/declared-tests.collection.json",
                    "managedTestProject": {
                        "projectId": f"managed-test/{subject_id}/proof-host",
                        "projectPath": SHARED_RUNTIME_PROJECT_PATH,
                        "assemblyName": SHARED_RUNTIME_ASSEMBLY_NAME,
                        "hostKind": "proof-host",
                        "collectionPath": f"verification/workspaces/subjects/{subject_id}/managed-tests/Generated/declared-tests.collection.json",
                        "executionModel": "shared-runtime-host",
                    },
                    "nativeTestProject": {
                        "projectId": f"native-test/{subject_id}/{workspace_matrix_id}/proof-host",
                        "matrixId": workspace_matrix_id,
                        "projectPath": f"verification/workspaces/subjects/{subject_id}/native/{workspace_matrix_id}/proof/chaos_subject_reference_proof.vcxproj",
                        "configureRoot": f"verification/workspaces/subjects/{subject_id}/native/{workspace_matrix_id}",
                        "targetPlatform": "windows-x64",
                        "toolchainProfile": "msvc-reference",
                        "deliveryKind": "direct-run-host",
                        "hostKind": "proof-host",
                        "managedTestProjectId": f"managed-test/{subject_id}/proof-host",
                        "buildArgs": ["--config", "Release", "--target", "chaos_subject_reference_proof"],
                    },
                },
                result.payload["workspaceExecution"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_subject_dispatch_allows_managed_only_workspace_manifest_for_proof_host(self) -> None:
        test_module = load_module(
            TEST_COMMAND_MODULE_PATH,
            "chaos_run_test_command_subject_workspace_managed_only_proof",
        )
        repo_root = make_temp_repo_root("subject-command", "workspace-managed-only-proof")
        manifest: dict[str, Any] = {}
        subject_id = "FixtureManagedOnlyWorkspaceSubject"
        fixed_run_id = "chaos-run-subject-workspace-managed-only-proof"
        workspace_matrix_id = "workspace-managed-only-proof-matrix"
        observed_selection: dict[str, str] = {}
        registry_index = make_subject_registry_index(
            test_module,
            subject_id=subject_id,
            default_goal_id="correctness.dev",
            matrix_id="legacy-proof-matrix",
        )
        write_subject_workspace(
            repo_root,
            subject_id=subject_id,
            matrix_id=workspace_matrix_id,
            goal_ids=["correctness.dev"],
            unit_stable_id=f"{subject_id}::Proofs::{subject_id}.Proofs::Run()",
            unit_alias="workspace-proof",
            unit_entry_index=5,
            benchmark_stable_id=f"{subject_id}::Benchmarks::{subject_id}.Benchmarks::Run()",
            benchmark_alias="workspace-benchmark",
            benchmark_entry_index=8,
            include_native_proof=False,
        )
        build_plan = make_build_plan_side_effect(
            observed_selection,
            subject_id=subject_id,
            expected_run_id=fixed_run_id,
            default_goal_id="correctness.dev",
            default_matrix_id=workspace_matrix_id,
        )

        def execute_plan_side_effect(repo_root: Path, plan: dict, **_: object) -> dict:
            del repo_root
            return build_execution_result(
                plan,
                stage_kind="runtime-observe",
                duration_ms=2,
                fingerprint="managed-only-proof-fingerprint",
            )

        try:
            with patch.object(test_module, "_scan_registry", return_value=registry_index):
                with patch.object(test_module.subject_planner_module, "build_plan", side_effect=build_plan) as build_plan_mock:
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
            build_plan_mock.assert_called_once()
            self.assertEqual(workspace_matrix_id, build_plan_mock.call_args.kwargs["matrix_id"])
            self.assertEqual(workspace_matrix_id, observed_selection["matrixId"])
            self.assertEqual("proof-host", result.payload["workspaceExecution"]["hostKind"])
            self.assertIsNone(result.payload["workspaceExecution"]["nativeTestProject"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

