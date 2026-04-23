from tests.unit.planning.project_workspace_test_support import *


class TestProjectWorkspaceRefresh(ProjectWorkspaceTestSupport):
    def test_refresh_subject_generated_root_accepts_generated_stage_variants(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_refresh_generated_stage_variants")
        repo_root = self._make_repo_root("refresh-generated-stage-variants")
        self._write_subject_fixture(repo_root)

        try:
            plan = {
                "stagePlan": [
                    {"stageId": "source-resolve", "kind": "source-resolve", "bucket": "source"},
                    {"stageId": "analysis-frontend", "kind": "analysis-frontend", "bucket": "analysis"},
                    {"stageId": "generated-engine-proof", "kind": "generated-engine-proof", "bucket": "generated"},
                    {"stageId": "runtime-engine-observe", "kind": "runtime-engine-observe", "bucket": "runtime"},
                ],
            }

            with patch.object(workspace_module.subject_planner_module, "build_plan", return_value=plan):
                with patch.object(
                    workspace_module.subject_executor_module,
                    "execute_plan",
                    return_value={"status": "ok"},
                ) as execute_mock:
                    workspace_module.refresh_subject_generated_root(
                        repo_root,
                        "FixtureSubject",
                        "windows-dev-output",
                        "CHECK",
                    )

            executed_plan = execute_mock.call_args.args[1]
            self.assertEqual(
                ["source-resolve", "analysis-frontend", "generated-engine-proof"],
                [stage["kind"] for stage in executed_plan["stagePlan"]],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
    def test_subject_matrix_source_entry_selection_does_not_inherit_manifest_slice_when_matrix_overrides_entry(self) -> None:
        workspace_module = load_module(
            PROJECT_WORKSPACE_MODULE_PATH,
            "chaos_project_workspace_subject_matrix_source_entry_selection_override",
        )
        manifest = {
            "source": {
                "entry": "FixtureSubject/Program::Main()",
                "entrySelection": {
                    "entryKind": 1,
                    "entrySlice": 7,
                },
            }
        }
        matrix = {
            "matrixId": "windows-foundation-dll-translation-native-proof",
            "source": {
                "entry": "",
                "primaryProjectPath": "subjects/FixtureSubject/source/Foundation/App/Foundation.App.csproj",
                "fullAssemblyClosure": True,
            },
        }

        self.assertEqual({}, workspace_module._subject_matrix_source_entry_selection(manifest, matrix))
    def test_refresh_subject_generated_root_clears_existing_run_root_before_planning(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_refresh_generated_root_clears_run_root")
        repo_root = self._make_repo_root("refresh-generated-root-clears-run-root")
        self._write_subject_fixture(repo_root)
        run_id = "subject-exec-windows_native_check"
        stale_manifest_path = (
            repo_root
            / "artifacts"
            / "subjects"
            / "FixtureSubject"
            / "runs"
            / run_id
            / "analysis"
            / "source"
            / "source.manifest.json"
        )
        stale_manifest_path.parent.mkdir(parents=True, exist_ok=True)
        stale_manifest_path.write_text("{}\n", encoding="utf-8")

        try:
            plan = {
                "stagePlan": [
                    {"stageId": "source-resolve", "kind": "source-resolve", "bucket": "source"},
                    {"stageId": "analysis-frontend", "kind": "analysis-frontend", "bucket": "analysis"},
                    {"stageId": "generated-native-proof", "kind": "generated-native-proof", "bucket": "generated"},
                ],
            }

            def build_plan_side_effect(*args, **kwargs):
                self.assertFalse(stale_manifest_path.exists())
                self.assertEqual(run_id, kwargs["run_id"])
                return plan

            with patch.object(workspace_module.subject_planner_module, "build_plan", side_effect=build_plan_side_effect):
                with patch.object(
                    workspace_module.subject_executor_module,
                    "execute_plan",
                    return_value={"status": "ok"},
                ):
                    workspace_module.refresh_subject_generated_root(
                        repo_root,
                        "FixtureSubject",
                        "windows-dev-output",
                        "CHECK",
                        run_id=run_id,
                    )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
    def test_refresh_subject_generated_root_passes_declared_entry_selection_to_planner(self) -> None:
        workspace_module = load_module(
            PROJECT_WORKSPACE_MODULE_PATH,
            "chaos_project_workspace_refresh_generated_root_declared_entry_selection",
        )
        repo_root = self._make_repo_root("refresh-generated-root-declared-entry-selection")
        self._write_subject_fixture(repo_root)
        entry_selection = {
            "family": "declared-unit-test",
            "stableId": "FixtureSubject::FixtureSubject::FixtureSubject.Proofs::Verify()",
            "alias": "fixture-proof",
            "entryIndex": 0,
        }

        try:
            plan = {
                "stagePlan": [
                    {"stageId": "source-resolve", "kind": "source-resolve", "bucket": "source"},
                    {"stageId": "analysis-frontend", "kind": "analysis-frontend", "bucket": "analysis"},
                    {"stageId": "generated-native-proof", "kind": "generated-native-proof", "bucket": "generated"},
                ],
            }

            def build_plan_side_effect(*args, **kwargs):
                self.assertEqual(entry_selection, kwargs["entry_selection"])
                return plan

            with patch.object(workspace_module.subject_planner_module, "build_plan", side_effect=build_plan_side_effect):
                with patch.object(
                    workspace_module.subject_executor_module,
                    "execute_plan",
                    return_value={"status": "ok"},
                ):
                    workspace_module.refresh_subject_generated_root(
                        repo_root,
                        "FixtureSubject",
                        "windows-dev-output",
                        "CHECK",
                        entry_selection=entry_selection,
                    )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
    def test_effective_generated_stage_kind_keeps_native_proof_for_native_hotupdate_chain(self) -> None:
        workspace_module = load_module(
            PROJECT_WORKSPACE_MODULE_PATH,
            "chaos_project_workspace_effective_generated_stage_kind_native_hotupdate_chain",
        )

        manifest = {
            "engineeringProfile": "hot-update-host",
            "source": {
                "type": "dotnet-project",
                "entry": "",
                "fullAssemblyClosure": True,
            },
        }
        matrix = {
            "matrixId": "windows-corelib-reference-native-hotupdate-proof",
            "executionContext": {
                "hostPlatform": "windows-x64",
                "targetPlatform": "windows-x64",
                "toolchainProfile": "msvc-reference",
                "runtimeProfile": "native-hotupdate-proof-output",
            },
        }
        entry_selection = {
            "family": "declared-unit-test",
            "stableId": "FixtureSubject::FixtureSubject.Patch::FixtureSubject.Patch.Proofs::Verify()",
            "alias": "native-hotupdate-proof",
            "entryIndex": 0,
        }

        uses_native_hotupdate_chain = workspace_module._matrix_uses_native_hotupdate_chain(matrix, manifest)
        self.assertTrue(uses_native_hotupdate_chain)
        self.assertEqual(
            "generated-native-proof",
            workspace_module._effective_generated_stage_kind(
                "generated-native-proof",
                entry_selection,
                None,
                uses_native_hotupdate_chain=uses_native_hotupdate_chain,
            ),
        )
