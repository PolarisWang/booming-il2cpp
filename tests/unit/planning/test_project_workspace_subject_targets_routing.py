from tests.unit.planning.project_workspace_test_support import *


class TestProjectWorkspaceSubjectTargetsRouting(ProjectWorkspaceTestSupport):
    def test_generate_subject_workspace_declared_unit_test_routes_proof_matrix_through_native_aot_host(self) -> None:
        workspace_module = load_module(
            PROJECT_WORKSPACE_MODULE_PATH,
            "chaos_project_workspace_subject_generate_declared_proof_native_aot_workspace",
        )
        repo_root = self._make_repo_root("subject-generate-declared-proof-native-aot")
        self._write_subject_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")
        entry_selection = {
            "family": "declared-unit-test",
            "stableId": "FixtureSubject::FixtureSubject::FixtureSubject.Proofs::Verify()",
            "alias": "fixture-proof",
            "entryIndex": 0,
        }

        try:
            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                generated_project = configure_root / "generated" / "chaos_subject_generated_native.vcxproj"
                generated_project.parent.mkdir(parents=True, exist_ok=True)
                generated_project.write_text("<Project />\n", encoding="utf-8")
                write_windows_subject_native_aot_project_stub(configure_root, host_subdir="proof")
                return completed

            def refresh_side_effect(
                refresh_repo_root: Path,
                subject_id: str,
                matrix_id: str,
                variant: str,
                *,
                run_id: str | None = None,
                entry_selection: dict[str, object] | None = None,
            ) -> None:
                del variant
                self.assertEqual("FixtureSubject", subject_id)
                self.assertEqual("windows-dev-output", matrix_id)
                self.assertEqual("subject-exec", run_id)
                self.assertEqual(
                    {
                        "family": "declared-unit-test",
                        "stableId": "FixtureSubject::FixtureSubject::FixtureSubject.Proofs::Verify()",
                        "alias": "fixture-proof",
                        "entryIndex": 0,
                    },
                    entry_selection,
                )
                generated_root = (
                    refresh_repo_root
                    / "artifacts"
                    / "subjects"
                    / subject_id
                    / "runs"
                    / (run_id or "subject-exec")
                    / "analysis"
                    / "generated"
                )
                (generated_root / "generated").mkdir(parents=True, exist_ok=True)
                (generated_root / "generated.manifest.json").write_text(
                    json.dumps({"matrixId": matrix_id}, indent=2) + "\n",
                    encoding="utf-8",
                )
                (generated_root / "native-aot.manifest.json").write_text(
                    json.dumps({"matrixId": matrix_id}, indent=2) + "\n",
                    encoding="utf-8",
                )
                (generated_root / "native-aot.plan.json").write_text(
                    json.dumps({"matrixId": matrix_id}, indent=2) + "\n",
                    encoding="utf-8",
                )
                (generated_root / "generated" / "native-aot.generated.cpp").write_text(
                    f"// native aot proof for {matrix_id}\n",
                    encoding="utf-8",
                )

            with patch.object(workspace_module.compiled_catalog_module, "build_subject_declared_test_catalog", return_value=declared_catalog_fixture()):
                with patch.object(workspace_module, "refresh_subject_generated_root", side_effect=refresh_side_effect):
                    with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                        with patch.object(workspace_module, "run_process", side_effect=configure_side_effect):
                            result = workspace_module.generate_subject_workspace(
                                repo_root,
                                "windows",
                                {
                                    "id": "subject/FixtureSubject",
                                    "matrix": "windows-dev-output",
                                    "variant": "CHECK",
                                    "refresh-generated": True,
                                    "entry-selection": entry_selection,
                                },
                            )

            manifest = json.loads((repo_root / result["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("windows-dev-output", manifest["defaultMatrixId"])
            proof_native_test_project = next(
                item
                for item in manifest["nativeTestProjects"]
                if item["projectId"] == "native-test/FixtureSubject/windows-dev-output/proof-host"
            )
            self.assertEqual("proof-host", proof_native_test_project["hostKind"])
            self.assertEqual(
                "managed-test/FixtureSubject/proof-host",
                proof_native_test_project["managedTestProjectId"],
            )
            self.assertEqual(
                "verification/workspaces/subjects/FixtureSubject/native/windows-dev-output/proof/chaos_subject_native_aot.vcxproj",
                proof_native_test_project["projectPath"],
            )
            self.assertTrue((repo_root / proof_native_test_project["projectPath"]).is_file())
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_subject_workspace_subject_entry_selection_routes_proof_matrix_through_benchmark_native_aot_host(self) -> None:
        workspace_module = load_module(
            PROJECT_WORKSPACE_MODULE_PATH,
            "chaos_project_workspace_subject_generate_subject_entry_native_aot_workspace",
        )
        repo_root = self._make_repo_root("subject-generate-subject-entry-native-aot")
        self._write_subject_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")
        subject_manifest_path = repo_root / "subjects" / "FixtureSubject" / "subject.manifest.json"
        subject_manifest = json.loads(subject_manifest_path.read_text(encoding="utf-8"))
        subject_manifest["environmentMatrices"][0]["source"] = {
            "entry": "FixtureSubject/Program::RunSelected()",
            "entrySelection": {
                "entryKind": 1,
                "entrySlice": 3,
            },
        }
        write_json(subject_manifest_path, subject_manifest)

        try:
            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                generated_project = configure_root / "generated" / "chaos_subject_generated_native.vcxproj"
                generated_project.parent.mkdir(parents=True, exist_ok=True)
                generated_project.write_text("<Project />\n", encoding="utf-8")
                write_windows_subject_native_aot_project_stub(configure_root, host_subdir="benchmark")
                return completed

            def refresh_side_effect(
                refresh_repo_root: Path,
                subject_id: str,
                matrix_id: str,
                variant: str,
                *,
                run_id: str | None = None,
                entry_selection: dict[str, object] | None = None,
            ) -> None:
                del variant
                self.assertEqual("FixtureSubject", subject_id)
                self.assertEqual("windows-dev-output", matrix_id)
                self.assertEqual("subject-exec", run_id)
                self.assertIsNone(entry_selection)
                generated_root = (
                    refresh_repo_root
                    / "artifacts"
                    / "subjects"
                    / subject_id
                    / "runs"
                    / (run_id or "subject-exec")
                    / "analysis"
                    / "generated"
                )
                (generated_root / "generated").mkdir(parents=True, exist_ok=True)
                (generated_root / "generated.manifest.json").write_text(
                    json.dumps({"matrixId": matrix_id}, indent=2) + "\n",
                    encoding="utf-8",
                )
                (generated_root / "native-aot.manifest.json").write_text(
                    json.dumps({"matrixId": matrix_id}, indent=2) + "\n",
                    encoding="utf-8",
                )
                (generated_root / "native-aot.plan.json").write_text(
                    json.dumps({"matrixId": matrix_id}, indent=2) + "\n",
                    encoding="utf-8",
                )
                (generated_root / "generated" / "native-aot.generated.cpp").write_text(
                    f"// native aot proof for {matrix_id}\n",
                    encoding="utf-8",
                )

            with patch.object(workspace_module.compiled_catalog_module, "build_subject_declared_test_catalog", return_value=declared_catalog_fixture()):
                with patch.object(workspace_module, "refresh_subject_generated_root", side_effect=refresh_side_effect):
                    with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                        with patch.object(workspace_module, "run_process", side_effect=configure_side_effect):
                            result = workspace_module.generate_subject_workspace(
                                repo_root,
                                "windows",
                                {
                                    "id": "subject/FixtureSubject",
                                    "matrix": "windows-dev-output",
                                    "variant": "CHECK",
                                    "refresh-generated": True,
                                },
                            )

            manifest = json.loads((repo_root / result["manifestPath"]).read_text(encoding="utf-8"))
            benchmark_native_test_project = next(
                item
                for item in manifest["nativeTestProjects"]
                if item["projectId"] == "native-test/FixtureSubject/windows-dev-output/benchmark-host"
            )
            self.assertEqual("benchmark-host", benchmark_native_test_project["hostKind"])
            self.assertEqual(
                "managed-test/FixtureSubject/benchmark-host",
                benchmark_native_test_project["managedTestProjectId"],
            )
            self.assertEqual(
                "verification/workspaces/subjects/FixtureSubject/native/windows-dev-output/benchmark/chaos_subject_native_aot.vcxproj",
                benchmark_native_test_project["projectPath"],
            )
            self.assertTrue((repo_root / benchmark_native_test_project["projectPath"]).is_file())
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_subject_workspace_clears_stale_visual_studio_state_when_manifest_version_changes(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_clear_stale_vs_state")
        repo_root = self._make_repo_root("subject-clear-stale-vs-state")
        self._write_subject_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")
        workspace_root = repo_root / "verification" / "workspaces" / "subjects" / "FixtureSubject"
        stale_vs_state_file = workspace_root / ".vs" / "FixtureSubject" / "v17" / ".suo"
        stale_vs_state_file.parent.mkdir(parents=True, exist_ok=True)
        stale_vs_state_file.write_text("stale\n", encoding="utf-8")
        write_json(
            workspace_root / "workspace.manifest.json",
            {
                "kind": "subject-workspace",
                "subjectId": "FixtureSubject",
                "visualStudioStateVersion": 1,
            },
        )

        try:
            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                configure_root.mkdir(parents=True, exist_ok=True)
                (configure_root / "chaos_subject_generated_native.vcxproj").write_text("<Project />\n", encoding="utf-8")
                (configure_root / "chaos_subject_reference_proof.vcxproj").write_text("<Project />\n", encoding="utf-8")
                return completed

            with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                with patch.object(workspace_module, "run_process", side_effect=configure_side_effect):
                    workspace_module.generate_subject_workspace(
                        repo_root,
                        "windows",
                        {"id": "subject/FixtureSubject"},
                    )

            self.assertFalse((workspace_root / ".vs").exists())
            updated_manifest = json.loads((workspace_root / "workspace.manifest.json").read_text(encoding="utf-8"))
            self.assertEqual(2, updated_manifest["workspaceVersion"])
            self.assertNotIn("visualStudioStateVersion", updated_manifest)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)


