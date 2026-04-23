from tests.tooling.run.subject_command_test_support import *


class TestSubjectCommandEngineering(SubjectCommandTestSupport):
    def test_engineering_validation_dispatch_keeps_pipeline_semantics_without_workspace_resolution(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_engineering_validation_legacy")
        repo_root = make_temp_repo_root("subject-command", "engineering-validation-legacy")
        manifest: dict[str, Any] = {}
        subject_id = "FixtureEngineeringSubject"
        fixed_run_id = "chaos-run-engineering-validation-legacy"
        observed_selection: dict[str, str] = {}
        object_id = f"engineering-validation/{subject_id}/project-graph"
        subject_registry_index = make_subject_registry_index(
            test_module,
            subject_id=subject_id,
            default_goal_id="correctness.dev",
            matrix_id="legacy-engineering-matrix",
        )
        engineering_object = {
            "id": object_id,
            "type": "engineering-validation",
            "displayName": "project-graph",
            "subjectId": subject_id,
            "defaultGoalId": "correctness.dev",
            "defaultMatrixId": "legacy-engineering-matrix",
            "goalIds": ["correctness.dev"],
            "matrixIds": ["legacy-engineering-matrix"],
            "supportedHosts": ["windows"],
            "level": "subject",
            "primaryModuleId": None,
            "moduleIds": [],
            "subsystemIds": [],
            "docRefs": [],
            "canonicalCommand": f"run test engineering-validation --id {object_id}",
            "kind": "project-graph",
        }
        registry_index = test_module.registry_module.RegistryIndex(
            host_platform="windows",
            suites=[],
            subjects=subject_registry_index.subjects,
            engineering_validations=[engineering_object],
            engineering_workloads=[],
            declared_unit_tests=[],
            declared_benchmarks=[],
            module_verifications=[],
            system_scenarios=[],
            pipelines=[],
            errors=[],
            warnings=[],
        )
        build_plan = make_build_plan_side_effect(
            observed_selection,
            subject_id=subject_id,
            expected_run_id=fixed_run_id,
            default_goal_id="correctness.dev",
            default_matrix_id="legacy-engineering-matrix",
        )

        def execute_plan_side_effect(repo_root: Path, plan: dict, **_: object) -> dict:
            del repo_root
            return build_execution_result(
                plan,
                stage_kind="runtime-observe",
                duration_ms=1,
                fingerprint="engineering-validation-legacy-fingerprint",
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
                                        "id": "test-engineering-validation",
                                        "handler": "test.dispatch",
                                    },
                                    repo_root,
                                    "windows",
                                    f"test engineering-validation --id {object_id}",
                                    manifest,
                                    {"id": object_id},
                                )

            self.assertEqual("ok", result.status)
            build_plan_mock.assert_called_once()
            self.assertEqual("legacy-engineering-matrix", build_plan_mock.call_args.kwargs["matrix_id"])
            self.assertEqual(
                {
                    "family": "engineering-validation",
                    "kind": "project-graph",
                    "alias": "project-graph",
                },
                build_plan_mock.call_args.kwargs["entry_selection"],
            )
            self.assertNotIn("workspaceExecution", result.payload)
            self.assertEqual("legacy-engineering-matrix", observed_selection["matrixId"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
