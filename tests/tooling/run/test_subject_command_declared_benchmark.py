from tests.tooling.run.subject_command_test_support import *


class TestSubjectCommandDeclaredBenchmark(SubjectCommandTestSupport):
    def test_declared_benchmark_dispatch_keeps_workload_entry_without_duplicate_source_entry_routing(self) -> None:
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
            matrix_id="workspace-benchmark-matrix",
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
        write_subject_workspace(
            repo_root,
            subject_id=subject_id,
            matrix_id="workspace-benchmark-matrix",
            goal_ids=["perf.release"],
            unit_stable_id=f"{subject_id}::Proofs::{subject_id}.Proofs::Run()",
            unit_alias="workspace-proof",
            unit_entry_index=4,
            benchmark_stable_id=stable_id,
            benchmark_alias=alias,
            benchmark_entry_index=11,
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
                                        "id": "test-declared-benchmark",
                                        "handler": "test.dispatch",
                                    },
                                    repo_root,
                                    "windows",
                                    f"test declared-benchmark --id declared-benchmark/{stable_id}",
                                    manifest,
                                    {"id": f"declared-benchmark/{stable_id}"},
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
            self.assertEqual("workspace-benchmark-matrix", build_plan_mock.call_args.kwargs["matrix_id"])
            self.assertIsNone(build_plan_mock.call_args.kwargs["source_entry"])
            self.assertEqual(workload_entry, build_plan_mock.call_args.kwargs["workload_entry"])
            self.assertEqual(
                {
                    "family": "declared-benchmark",
                    "stableId": stable_id,
                    "alias": alias,
                    "entryIndex": 11,
                },
                build_plan_mock.call_args.kwargs["entry_selection"],
            )
            self.assertEqual(11, result.payload["workspaceExecution"]["entryIndex"])
            self.assertEqual("benchmark-host", result.payload["workspaceExecution"]["hostKind"])
            self.assertIsNone(result.payload["workspaceExecution"]["nativeTestProject"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_declared_benchmark_dispatch_prefers_selected_object_defaults_over_manifest_defaults(self) -> None:
        test_module = load_module(
            TEST_COMMAND_MODULE_PATH,
            "chaos_run_test_command_declared_benchmark_workspace_defaults",
        )
        repo_root = make_temp_repo_root("subject-command", "declared-benchmark-workspace-defaults")
        manifest: dict[str, Any] = {}
        subject_id = "HotUpdateHostPack"
        stable_id = "hotupdate-benchmark"
        alias = "hotupdate-benchmark"
        source_entry = "HotUpdateHostPack/Benchmarks::Proof()"
        workload_entry = "HotUpdateHostPack/Benchmarks::RunWorkload()"
        fixed_run_id = "chaos-run-declared-benchmark-workspace-defaults"
        observed_selection: dict[str, str] = {}
        registry_index = make_declared_registry_index(
            test_module,
            subject_id=subject_id,
            object_type="declared-benchmark",
            stable_id=stable_id,
            alias=alias,
            source_entry=source_entry,
            workload_entry=workload_entry,
            default_goal_id="perf.release",
            matrix_id="windows-managed-perf",
        )
        write_subject_workspace(
            repo_root,
            subject_id=subject_id,
            matrix_id="windows-managed-output",
            goal_ids=["correctness.dev"],
            unit_stable_id=f"{subject_id}::Proofs::{subject_id}.Proofs::Run()",
            unit_alias="workspace-proof",
            unit_entry_index=3,
            benchmark_stable_id=stable_id,
            benchmark_alias=alias,
            benchmark_entry_index=12,
            include_native_proof=False,
        )
        workspace_manifest_path = repo_root / "verification" / "workspaces" / "subjects" / subject_id / "workspace.manifest.json"
        workspace_manifest = read_json(workspace_manifest_path)
        workspace_manifest["defaultMatrixId"] = "windows-managed-output"
        managed_project_ids = list(workspace_manifest["matrices"][0]["managedProjectIds"])
        managed_test_project_ids = list(workspace_manifest["matrices"][0]["managedTestProjectIds"])
        workspace_manifest["matrices"].append(
            {
                "matrixId": "windows-managed-perf",
                "goalIds": ["perf.release"],
                "hostPlatform": "windows-x64",
                "targetPlatform": "windows-x64",
                "toolchainProfile": "msvc-reference",
                "managedProjectIds": managed_project_ids,
                "managedTestProjectIds": managed_test_project_ids,
                "nativeProjectIds": [],
                "nativeTestProjectIds": [],
            }
        )
        write_json(workspace_manifest_path, workspace_manifest)
        build_plan = make_build_plan_side_effect(
            observed_selection,
            subject_id=subject_id,
            expected_run_id=fixed_run_id,
            default_goal_id="unexpected-goal",
            default_matrix_id="unexpected-matrix",
            stage_kind="runtime-observe",
        )

        def execute_plan_side_effect(repo_root: Path, plan: dict, **_: object) -> dict:
            del repo_root
            return build_execution_result(
                plan,
                stage_kind="runtime-observe",
                duration_ms=4,
                fingerprint="declared-benchmark-workspace-defaults-fingerprint",
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
                                    {
                                        "id": "test-declared-benchmark",
                                        "handler": "test.dispatch",
                                    },
                                    repo_root,
                                    "windows",
                                    f"test declared-benchmark --id declared-benchmark/{stable_id}",
                                    manifest,
                                    {"id": f"declared-benchmark/{stable_id}"},
                                )

            self.assertEqual("ok", result.status)
            build_plan_mock.assert_called_once()
            self.assertEqual("windows-managed-perf", build_plan_mock.call_args.kwargs["matrix_id"])
            self.assertEqual("perf.release", build_plan_mock.call_args.kwargs["goal_id"])
            self.assertEqual(
                {
                    "subjectId": subject_id,
                    "matrixId": "windows-managed-perf",
                    "goalId": "perf.release",
                    "validationProfileId": "",
                    "validationKind": "",
                    "variant": "",
                    "terminalBucket": "runtime",
                },
                observed_selection,
            )
            self.assertEqual("windows-managed-perf", result.payload["workspaceExecution"]["matrixId"])
            self.assertEqual("perf.release", result.payload["workspaceExecution"]["goalId"])
            self.assertEqual(12, result.payload["workspaceExecution"]["entryIndex"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_declared_benchmark_dispatch_regenerates_workspace_when_required_matrix_is_missing(self) -> None:
        test_module = load_module(
            TEST_COMMAND_MODULE_PATH,
            "chaos_run_test_command_declared_benchmark_workspace_regenerate",
        )
        repo_root = make_temp_repo_root("subject-command", "declared-benchmark-workspace-regenerate")
        manifest: dict[str, Any] = {}
        subject_id = "HotUpdateHostPack"
        stable_id = "hotupdate-benchmark"
        alias = "hotupdate-benchmark"
        source_entry = "HotUpdateHostPack/Benchmarks::Proof()"
        workload_entry = "HotUpdateHostPack/Benchmarks::RunWorkload()"
        fixed_run_id = "chaos-run-declared-benchmark-workspace-regenerate"
        observed_selection: dict[str, str] = {}
        registry_index = make_declared_registry_index(
            test_module,
            subject_id=subject_id,
            object_type="declared-benchmark",
            stable_id=stable_id,
            alias=alias,
            source_entry=source_entry,
            workload_entry=workload_entry,
            default_goal_id="perf.release",
            matrix_id="windows-managed-perf",
        )
        write_subject_workspace(
            repo_root,
            subject_id=subject_id,
            matrix_id="windows-managed-output",
            goal_ids=["correctness.dev"],
            unit_stable_id=f"{subject_id}::Proofs::{subject_id}.Proofs::Run()",
            unit_alias="workspace-proof",
            unit_entry_index=3,
            benchmark_stable_id=stable_id,
            benchmark_alias=alias,
            benchmark_entry_index=12,
            include_native_proof=False,
        )
        workspace_manifest_path = repo_root / "verification" / "workspaces" / "subjects" / subject_id / "workspace.manifest.json"
        regenerated_calls: list[dict[str, object]] = []
        build_plan = make_build_plan_side_effect(
            observed_selection,
            subject_id=subject_id,
            expected_run_id=fixed_run_id,
            default_goal_id="unexpected-goal",
            default_matrix_id="unexpected-matrix",
            stage_kind="runtime-observe",
        )

        def regenerate_workspace(
            refresh_repo_root: Path,
            refresh_host_platform: str,
            options: dict[str, object],
            *,
            progress_callback: object | None = None,
        ) -> dict[str, Any]:
            del progress_callback
            regenerated_calls.append(
                {
                    "repo_root": refresh_repo_root,
                    "host_platform": refresh_host_platform,
                    "options": dict(options),
                }
            )
            workspace_manifest = read_json(workspace_manifest_path)
            managed_project_ids = list(workspace_manifest["matrices"][0]["managedProjectIds"])
            managed_test_project_ids = list(workspace_manifest["matrices"][0]["managedTestProjectIds"])
            workspace_manifest["matrices"].append(
                {
                    "matrixId": "windows-managed-perf",
                    "goalIds": ["perf.release"],
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                    "toolchainProfile": "dotnet-managed",
                    "managedProjectIds": managed_project_ids,
                    "managedTestProjectIds": managed_test_project_ids,
                    "nativeProjectIds": [],
                    "nativeTestProjectIds": [],
                }
            )
            write_json(workspace_manifest_path, workspace_manifest)
            return {
                "manifestPath": workspace_manifest_path.relative_to(refresh_repo_root).as_posix(),
                "artifacts": [workspace_manifest_path.relative_to(refresh_repo_root).as_posix()],
                "importantOutputs": [],
                "consoleText": "",
            }

        def execute_plan_side_effect(repo_root: Path, plan: dict, **_: object) -> dict:
            del repo_root
            return build_execution_result(
                plan,
                stage_kind="runtime-observe",
                duration_ms=4,
                fingerprint="declared-benchmark-workspace-regenerate-fingerprint",
            )

        try:
            with patch.object(test_module, "_scan_registry", return_value=registry_index):
                with patch.object(
                    test_module,
                    "project_workspace_module",
                    create=True,
                ) as workspace_module_mock:
                    workspace_module_mock.generate_subject_workspace.side_effect = regenerate_workspace
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
                                        {
                                            "id": "test-declared-benchmark",
                                            "handler": "test.dispatch",
                                        },
                                        repo_root,
                                        "windows",
                                        f"test declared-benchmark --id declared-benchmark/{stable_id}",
                                        manifest,
                                        {"id": f"declared-benchmark/{stable_id}"},
                                    )

            self.assertEqual("ok", result.status)
            self.assertEqual(1, len(regenerated_calls))
            self.assertEqual(repo_root, regenerated_calls[0]["repo_root"])
            self.assertEqual("windows", regenerated_calls[0]["host_platform"])
            self.assertEqual(f"subject/{subject_id}", regenerated_calls[0]["options"]["id"])
            self.assertTrue(bool(regenerated_calls[0]["options"]["auto-refresh-missing-generated"]))
            self.assertTrue(bool(regenerated_calls[0]["options"]["refresh-generated"]))
            self.assertEqual("windows-managed-perf", regenerated_calls[0]["options"]["matrix"])
            self.assertEqual(
                {
                    "family": "declared-benchmark",
                    "stableId": stable_id,
                    "alias": alias,
                },
                regenerated_calls[0]["options"]["entry-selection"],
            )
            build_plan_mock.assert_called_once()
            self.assertEqual("windows-managed-perf", build_plan_mock.call_args.kwargs["matrix_id"])
            self.assertEqual("perf.release", build_plan_mock.call_args.kwargs["goal_id"])
            self.assertEqual("windows-managed-perf", result.payload["workspaceExecution"]["matrixId"])
            self.assertEqual("perf.release", result.payload["workspaceExecution"]["goalId"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

