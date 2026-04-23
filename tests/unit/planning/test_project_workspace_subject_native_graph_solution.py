from tests.unit.planning.project_workspace_test_support import *


class TestProjectWorkspaceSubjectNativeGraphSolution(ProjectWorkspaceTestSupport):
    def test_generate_subject_workspace_rewrites_project_reference_names_for_full_windows_native_graph(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_unique_full_native_reference_names")
        repo_root = self._make_repo_root("subject-unique-full-native-reference-names")
        self._write_subject_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")
        subject_manifest_path = repo_root / "subjects" / "FixtureSubject" / "subject.manifest.json"
        subject_manifest = json.loads(subject_manifest_path.read_text(encoding="utf-8"))
        subject_manifest["environmentMatrices"].append(
            {
                "matrixId": "windows-reference-trace",
                "pipelineId": "proof-runtime-output",
                "supportedGoals": ["correctness.dev"],
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                    "toolchainProfile": "msvc-reference",
                },
                "artifactPlan": {
                    "evidenceTerminalBucket": "runtime",
                },
            }
        )
        write_json(subject_manifest_path, subject_manifest)

        try:
            def refresh_side_effect(
                refresh_repo_root: Path,
                subject_id: str,
                matrix_id: str,
                variant: str,
                *,
                run_id: str | None = None,
            ) -> None:
                del matrix_id, variant
                generated_root = (
                    refresh_repo_root
                    / "artifacts"
                    / "subjects"
                    / subject_id
                    / "runs"
                    / (run_id or "subject-exec")
                    / "analysis"
                    / "generated"
                    / "generated"
                )
                generated_root.mkdir(parents=True, exist_ok=True)
                (generated_root / "native-reference.generated.cpp").write_text("// generated\n", encoding="utf-8")

            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                configure_root.mkdir(parents=True, exist_ok=True)
                if str(configure_root).endswith("windows-linux-buildable"):
                    (configure_root / "linux-x64-packaging.vcxproj").write_text(
                        "<Project><PropertyGroup Label=\"Globals\"><ProjectName>linux-x64-packaging</ProjectName></PropertyGroup></Project>\n",
                        encoding="utf-8",
                    )
                else:
                    write_windows_subject_native_solution_graph_stubs(configure_root)
                return completed

            with patch.object(workspace_module, "refresh_subject_generated_root", side_effect=refresh_side_effect):
                with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                    with patch.object(workspace_module, "run_process", side_effect=configure_side_effect):
                        result = workspace_module.generate_subject_workspace(
                            repo_root,
                            "windows",
                            {"id": "subject/FixtureSubject", "all-targets": True},
                        )

            solution_root = (repo_root / result["manifestPath"]).parent
            proof_reference_trace = solution_root / "native" / "windows-reference-trace" / "proof" / "chaos_subject_reference_proof.vcxproj"
            all_build_reference_trace = solution_root / "native" / "windows-reference-trace" / "ALL_BUILD.vcxproj"
            bootstrap_reference_trace = solution_root / "native" / "windows-reference-trace" / "bootstrap" / "chaos_bootstrap.vcxproj"

            self.assertNotIn("<ProjectReference Include=", proof_reference_trace.read_text(encoding="utf-8"))
            self.assertIn(
                "<Name>chaos_bootstrap_windows_reference_trace</Name>",
                all_build_reference_trace.read_text(encoding="utf-8"),
            )
            self.assertIn(
                "<Name>chaos_runtime_core_windows_reference_trace</Name>",
                bootstrap_reference_trace.read_text(encoding="utf-8"),
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_subject_workspace_strips_project_reference_includes_from_subject_facing_vcxprojs(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_strips_subject_facing_references")
        repo_root = self._make_repo_root("subject-strip-subject-facing-references")
        self._write_subject_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")
        subject_manifest_path = repo_root / "subjects" / "FixtureSubject" / "subject.manifest.json"
        subject_manifest = json.loads(subject_manifest_path.read_text(encoding="utf-8"))
        subject_manifest["environmentMatrices"].append(
            {
                "matrixId": "windows-reference-trace",
                "pipelineId": "proof-runtime-output",
                "supportedGoals": ["correctness.dev"],
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                    "toolchainProfile": "msvc-reference",
                },
                "artifactPlan": {
                    "evidenceTerminalBucket": "runtime",
                },
            }
        )
        write_json(subject_manifest_path, subject_manifest)

        try:
            def refresh_side_effect(
                refresh_repo_root: Path,
                subject_id: str,
                matrix_id: str,
                variant: str,
                *,
                run_id: str | None = None,
            ) -> None:
                del matrix_id, variant
                generated_root = (
                    refresh_repo_root
                    / "artifacts"
                    / "subjects"
                    / subject_id
                    / "runs"
                    / (run_id or "subject-exec")
                    / "analysis"
                    / "generated"
                    / "generated"
                )
                generated_root.mkdir(parents=True, exist_ok=True)
                (generated_root / "native-reference.generated.cpp").write_text("// generated\n", encoding="utf-8")

            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                configure_root.mkdir(parents=True, exist_ok=True)
                if str(configure_root).endswith("windows-linux-buildable"):
                    (configure_root / "linux-x64-packaging.vcxproj").write_text("<Project />\n", encoding="utf-8")
                else:
                    write_windows_subject_native_solution_graph_stubs(configure_root)
                return completed

            with patch.object(workspace_module, "refresh_subject_generated_root", side_effect=refresh_side_effect):
                with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                    with patch.object(workspace_module, "run_process", side_effect=configure_side_effect):
                        result = workspace_module.generate_subject_workspace(
                            repo_root,
                            "windows",
                            {"id": "subject/FixtureSubject", "all-targets": True},
                        )

            solution_root = (repo_root / result["manifestPath"]).parent
            generated_reference_trace = solution_root / "native" / "windows-reference-trace" / "generated" / "chaos_subject_generated_native.vcxproj"
            proof_reference_trace = solution_root / "native" / "windows-reference-trace" / "proof" / "chaos_subject_reference_proof.vcxproj"
            all_build_reference_trace = solution_root / "native" / "windows-reference-trace" / "ALL_BUILD.vcxproj"

            self.assertNotIn("<ProjectReference Include=", generated_reference_trace.read_text(encoding="utf-8"))
            self.assertNotIn("<ProjectReference Include=", proof_reference_trace.read_text(encoding="utf-8"))
            self.assertIn("<ProjectReference Include=", all_build_reference_trace.read_text(encoding="utf-8"))
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_subject_workspace_includes_only_top_level_native_projects_in_solution(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_top_level_native_graph")
        repo_root = self._make_repo_root("subject-full-native-graph")
        self._write_subject_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")
        subject_manifest_path = repo_root / "subjects" / "FixtureSubject" / "subject.manifest.json"
        subject_manifest = json.loads(subject_manifest_path.read_text(encoding="utf-8"))
        subject_manifest["environmentMatrices"].append(
            {
                "matrixId": "windows-reference-trace",
                "pipelineId": "proof-runtime-output",
                "supportedGoals": ["correctness.dev"],
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                    "toolchainProfile": "msvc-reference",
                },
                "artifactPlan": {
                    "evidenceTerminalBucket": "runtime",
                },
            }
        )
        write_json(subject_manifest_path, subject_manifest)

        try:
            def refresh_side_effect(
                refresh_repo_root: Path,
                subject_id: str,
                matrix_id: str,
                variant: str,
                *,
                run_id: str | None = None,
            ) -> None:
                del matrix_id, variant
                generated_root = (
                    refresh_repo_root
                    / "artifacts"
                    / "subjects"
                    / subject_id
                    / "runs"
                    / (run_id or "subject-exec")
                    / "analysis"
                    / "generated"
                    / "generated"
                )
                generated_root.mkdir(parents=True, exist_ok=True)
                (generated_root / "native-reference.generated.cpp").write_text("// generated\n", encoding="utf-8")

            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                configure_root.mkdir(parents=True, exist_ok=True)
                if str(configure_root).endswith("windows-linux-buildable"):
                    (configure_root / "linux-x64-packaging.vcxproj").write_text("<Project />\n", encoding="utf-8")
                else:
                    write_windows_subject_native_solution_graph_stubs(configure_root)
                return completed

            with patch.object(workspace_module, "refresh_subject_generated_root", side_effect=refresh_side_effect):
                with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                    with patch.object(workspace_module, "run_process", side_effect=configure_side_effect):
                        result = workspace_module.generate_subject_workspace(
                            repo_root,
                            "windows",
                            {"id": "subject/FixtureSubject", "all-targets": True},
                        )

            solution_text = (repo_root / result["manifestPath"]).parent.joinpath("FixtureSubject.sln").read_text(encoding="utf-8")
            self.assertIn('"chaos_subject_generated_native (windows-dev-output)"', solution_text)
            self.assertIn('"chaos_subject_reference_proof (windows-dev-output)"', solution_text)
            self.assertIn('"chaos_subject_generated_native (windows-reference-trace)"', solution_text)
            self.assertIn('"chaos_subject_reference_proof (windows-reference-trace)"', solution_text)
            self.assertNotIn('"ALL_BUILD (windows-dev-output)"', solution_text)
            self.assertNotIn('"ALL_BUILD (windows-reference-trace)"', solution_text)
            self.assertNotIn('"ZERO_CHECK (windows-dev-output)"', solution_text)
            self.assertNotIn('"ZERO_CHECK (windows-reference-trace)"', solution_text)
            self.assertNotIn('"chaos_bootstrap (windows-dev-output/bootstrap)"', solution_text)
            self.assertNotIn('"chaos_bootstrap (windows-reference-trace/bootstrap)"', solution_text)
            self.assertNotIn('"chaos_runtime_core (windows-dev-output/runtime-core)"', solution_text)
            self.assertNotIn('"chaos_runtime_core (windows-reference-trace/runtime-core)"', solution_text)
            self.assertNotIn('"chaos_subject_reference_proof_run (windows-dev-output)"', solution_text)
            self.assertNotIn('"chaos_subject_reference_proof_run (windows-reference-trace)"', solution_text)
            self.assertNotIn('"chaos_support (windows-dev-output/support)"', solution_text)
            self.assertNotIn('"chaos_support (windows-reference-trace/support)"', solution_text)
            self.assertNotIn('"native/windows-linux-buildable/linux-x64-packaging.vcxproj"', solution_text)
            project_count = solution_text.count('Project("{')
            folder_count = solution_text.count('Project("{2150E333-8FDC-42A3-9474-1A3956D46DE8}")')
            self.assertEqual(5, project_count - folder_count)
            self.assertIn("NestedProjects", solution_text)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_subject_workspace_uses_native_vcxproj_guids_in_solution(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_native_vcxproj_guids")
        repo_root = self._make_repo_root("subject-native-vcxproj-guids")
        self._write_subject_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")

        try:
            generated_guid = "{11111111-1111-1111-1111-111111111111}"
            proof_guid = "{22222222-2222-2222-2222-222222222222}"

            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                configure_root.mkdir(parents=True, exist_ok=True)
                write_windows_subject_native_project_stubs(
                    configure_root,
                    generated_text=
                    f"<Project><PropertyGroup Label=\"Globals\"><ProjectGuid>{generated_guid}</ProjectGuid></PropertyGroup></Project>\n",
                    proof_text=
                    f"<Project><PropertyGroup Label=\"Globals\"><ProjectGuid>{proof_guid}</ProjectGuid></PropertyGroup></Project>\n",
                )
                return completed

            with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                with patch.object(workspace_module, "run_process", side_effect=configure_side_effect):
                    result = workspace_module.generate_subject_workspace(
                        repo_root,
                        "windows",
                        {"id": "subject/FixtureSubject"},
                    )

            solution_text = (repo_root / result["manifestPath"]).parent.joinpath("FixtureSubject.sln").read_text(encoding="utf-8")
            self.assertIn(f'"native/windows-dev-output/generated/chaos_subject_generated_native.vcxproj", "{generated_guid}"', solution_text)
            self.assertIn(f'"native/windows-dev-output/proof/chaos_subject_reference_proof.vcxproj", "{proof_guid}"', solution_text)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
