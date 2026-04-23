from tests.unit.planning.project_workspace_test_support import *


class TestProjectWorkspaceSubjectNativeGraphNames(ProjectWorkspaceTestSupport):
    def test_generate_subject_workspace_disambiguates_duplicate_native_project_names_in_solution(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_generate_duplicate_native_names")
        repo_root = self._make_repo_root("subject-generate-duplicate-native-names")
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
            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                configure_root.mkdir(parents=True, exist_ok=True)
                write_windows_subject_native_project_stubs(configure_root)
                (configure_root / "linux-x64-packaging.vcxproj").write_text("<Project />\n", encoding="utf-8")
                return completed

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
            self.assertIn('"chaos_subject_generated_native (windows-reference-trace)"', solution_text)
            self.assertIn('"chaos_subject_reference_proof (windows-dev-output)"', solution_text)
            self.assertIn('"chaos_subject_reference_proof (windows-reference-trace)"', solution_text)
            self.assertNotIn(
                'Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "chaos_subject_generated_native",',
                solution_text,
            )
            self.assertNotIn(
                'Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "chaos_subject_reference_proof",',
                solution_text,
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_subject_workspace_assigns_unique_native_project_names_per_matrix(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_unique_vcxproj_names")
        repo_root = self._make_repo_root("subject-unique-vcxproj-names")
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

            with patch.object(workspace_module, "refresh_subject_generated_root", side_effect=refresh_side_effect):
                with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                    def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                        del cwd, env
                        configure_root = Path(arguments[arguments.index("-B") + 1])
                        configure_root.mkdir(parents=True, exist_ok=True)
                        write_windows_subject_native_project_stubs(
                            configure_root,
                            generated_text=
                            "<Project><PropertyGroup Label=\"Globals\"><ProjectName>chaos_subject_generated_native</ProjectName></PropertyGroup></Project>\n",
                            proof_text=
                            "<Project><PropertyGroup Label=\"Globals\"><ProjectName>chaos_subject_reference_proof</ProjectName></PropertyGroup></Project>\n",
                        )
                        (configure_root / "linux-x64-packaging.vcxproj").write_text(
                            "<Project><PropertyGroup Label=\"Globals\"><ProjectName>linux-x64-packaging</ProjectName></PropertyGroup></Project>\n",
                            encoding="utf-8",
                        )
                        return completed

                    with patch.object(workspace_module, "run_process", side_effect=configure_side_effect):
                        result = workspace_module.generate_subject_workspace(
                            repo_root,
                            "windows",
                            {"id": "subject/FixtureSubject", "all-targets": True},
                        )

            solution_root = (repo_root / result["manifestPath"]).parent
            generated_dev_output_vcxproj = solution_root / "native" / "windows-dev-output" / "generated" / "chaos_subject_generated_native.vcxproj"
            generated_reference_trace_vcxproj = solution_root / "native" / "windows-reference-trace" / "generated" / "chaos_subject_generated_native.vcxproj"
            proof_dev_output_vcxproj = solution_root / "native" / "windows-dev-output" / "proof" / "chaos_subject_reference_proof.vcxproj"
            proof_reference_trace_vcxproj = solution_root / "native" / "windows-reference-trace" / "proof" / "chaos_subject_reference_proof.vcxproj"

            self.assertIn(
                "<ProjectName>chaos_subject_generated_native_windows_dev_output</ProjectName>",
                generated_dev_output_vcxproj.read_text(encoding="utf-8"),
            )
            self.assertIn(
                "<ProjectName>chaos_subject_generated_native_windows_reference_trace</ProjectName>",
                generated_reference_trace_vcxproj.read_text(encoding="utf-8"),
            )
            self.assertIn(
                "<ProjectName>chaos_subject_reference_proof_windows_dev_output</ProjectName>",
                proof_dev_output_vcxproj.read_text(encoding="utf-8"),
            )
            self.assertIn(
                "<ProjectName>chaos_subject_reference_proof_windows_reference_trace</ProjectName>",
                proof_reference_trace_vcxproj.read_text(encoding="utf-8"),
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_subject_workspace_assigns_unique_internal_names_for_full_windows_native_graph(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_unique_full_native_graph_names")
        repo_root = self._make_repo_root("subject-unique-full-native-graph-names")
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
            all_build_dev_output = solution_root / "native" / "windows-dev-output" / "ALL_BUILD.vcxproj"
            all_build_reference_trace = solution_root / "native" / "windows-reference-trace" / "ALL_BUILD.vcxproj"
            zero_check_dev_output = solution_root / "native" / "windows-dev-output" / "ZERO_CHECK.vcxproj"
            bootstrap_reference_trace = solution_root / "native" / "windows-reference-trace" / "bootstrap" / "chaos_bootstrap.vcxproj"
            runtime_reference_trace = solution_root / "native" / "windows-reference-trace" / "runtime-core" / "chaos_runtime_core.vcxproj"
            support_reference_trace = solution_root / "native" / "windows-reference-trace" / "support" / "chaos_support.vcxproj"
            proof_run_reference_trace = solution_root / "native" / "windows-reference-trace" / "proof" / "chaos_subject_reference_proof_run.vcxproj"

            self.assertIn("<ProjectName>ALL_BUILD_windows_dev_output</ProjectName>", all_build_dev_output.read_text(encoding="utf-8"))
            self.assertIn(
                "<ProjectName>ALL_BUILD_windows_reference_trace</ProjectName>",
                all_build_reference_trace.read_text(encoding="utf-8"),
            )
            self.assertIn("<ProjectName>ZERO_CHECK_windows_dev_output</ProjectName>", zero_check_dev_output.read_text(encoding="utf-8"))
            self.assertIn(
                "<ProjectName>chaos_bootstrap_windows_reference_trace</ProjectName>",
                bootstrap_reference_trace.read_text(encoding="utf-8"),
            )
            self.assertIn(
                "<ProjectName>chaos_runtime_core_windows_reference_trace</ProjectName>",
                runtime_reference_trace.read_text(encoding="utf-8"),
            )
            self.assertIn(
                "<ProjectName>chaos_support_windows_reference_trace</ProjectName>",
                support_reference_trace.read_text(encoding="utf-8"),
            )
            self.assertIn(
                "<ProjectName>chaos_subject_reference_proof_run_windows_reference_trace</ProjectName>",
                proof_run_reference_trace.read_text(encoding="utf-8"),
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
