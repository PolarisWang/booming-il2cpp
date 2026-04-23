from tests.unit.planning.project_workspace_test_support import *


class TestProjectWorkspaceSubjectBuildRefresh(ProjectWorkspaceTestSupport):
    def test_generate_subject_workspace_accepts_runtime_skeleton_generated_layout(self) -> None:
        workspace_module = load_module(
            PROJECT_WORKSPACE_MODULE_PATH,
            "chaos_project_workspace_subject_runtime_skeleton_layout",
        )
        repo_root = self._make_repo_root("subject-runtime-skeleton-layout")
        self._write_subject_fixture(repo_root)
        legacy_generated_source_path = (
            repo_root
            / "artifacts"
            / "subjects"
            / "FixtureSubject"
            / "runs"
            / "subject-exec"
            / "analysis"
            / "generated"
            / "generated"
            / "native-reference.generated.cpp"
        )
        if legacy_generated_source_path.exists():
            legacy_generated_source_path.unlink()
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")

        try:
            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                write_windows_subject_native_project_stubs(configure_root)
                return completed

            def refresh_side_effect(
                refresh_repo_root: Path,
                subject_id: str,
                matrix_id: str,
                variant: str,
                *,
                run_id: str | None = None,
            ) -> None:
                self.assertEqual("FixtureSubject", subject_id)
                self.assertEqual("windows-dev-output", matrix_id)
                self.assertEqual("CHECK", variant)
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
                runtime_root = generated_root / "generated" / "runtime"
                runtime_root.mkdir(parents=True, exist_ok=True)
                (runtime_root / "native-reference.runtime-skeleton.generated.cpp").write_text(
                    "int RunNativeReferenceRuntimeSkeleton() { return 0; }\n",
                    encoding="utf-8",
                )
                (generated_root / "native-reference.manifest.json").write_text(
                    json.dumps(
                        {
                            "generatedArtifacts": [
                                {
                                    "kind": "generatedTranslationUnit",
                                    "path": "generated/runtime/native-reference.runtime-skeleton.generated.cpp",
                                }
                            ]
                        }
                    ),
                    encoding="utf-8",
                )

            with patch.object(workspace_module, "refresh_subject_generated_root", side_effect=refresh_side_effect):
                with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                    with patch.object(workspace_module, "run_process", side_effect=configure_side_effect):
                        result = workspace_module.generate_subject_workspace(
                            repo_root,
                            "windows",
                            {"id": "subject/FixtureSubject", "refresh-generated": True},
                        )

            materialized_source_root = repo_root / "verification" / "workspaces" / "subjects" / "FixtureSubject" / "native-source" / "windows-dev-output"
            self.assertIn(
                str(
                    (
                        repo_root
                        / "verification" / "workspaces" / "subjects"
                        / "FixtureSubject"
                        / "generated"
                        / "subject-exec"
                        / "analysis"
                        / "generated"
                        / "generated"
                        / "runtime"
                        / "native-reference.runtime-skeleton.generated.cpp"
                    ).as_posix()
                ),
                (materialized_source_root / "generated" / "CMakeLists.txt").read_text(encoding="utf-8"),
            )
            self.assertTrue((repo_root / result["manifestPath"]).is_file())
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_subject_workspace_refreshes_subject_exec_only_when_requested(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_refresh")
        repo_root = self._make_repo_root("subject-refresh")
        self._write_subject_fixture(repo_root)

        try:
            with patch.object(
                workspace_module,
                "_subject_generated_source_path",
                return_value=repo_root / "artifacts" / "subjects" / "FixtureSubject" / "runs" / "subject-exec" / "analysis" / "generated" / "generated" / "missing.cpp",
            ):
                with self.assertRaisesRegex(RuntimeError, "subject-exec generated root is missing"):
                    workspace_module.generate_subject_workspace(
                        repo_root,
                        "windows",
                        {"id": "subject/FixtureSubject"},
                    )

            def refresh_side_effect(
                repo_root: Path,
                subject_id: str,
                matrix_id: str,
                variant: str,
                *,
                run_id: str | None = None,
            ) -> None:
                del matrix_id, variant
                generated_root = (
                    repo_root
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

            with patch.object(workspace_module, "refresh_subject_generated_root", side_effect=refresh_side_effect) as refresh_mock:
                with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                    with patch.object(workspace_module, "run_process", return_value=subprocess.CompletedProcess(["cmake"], 0, "", "")):
                        result = workspace_module.generate_subject_workspace(
                            repo_root,
                            "windows",
                            {"id": "subject/FixtureSubject", "refresh-generated": True},
                        )

            self.assertTrue((repo_root / result["manifestPath"]).is_file())
            refresh_mock.assert_called_once()
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_subject_workspace_materializes_generic_native_reference_project_when_subject_cmake_missing(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_materialized_native_source")
        repo_root = self._make_repo_root("subject-materialized-native-source")
        self._write_subject_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")
        proof_root = repo_root / "subjects" / "FixtureSubject" / "validation" / "proof" / "native-reference"
        (proof_root / "CMakeLists.txt").unlink()
        (proof_root / "main.cpp").write_text("int main() { return 0; }\n", encoding="utf-8")

        try:
            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                write_windows_subject_native_project_stubs(configure_root)
                return completed

            with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                with patch.object(workspace_module, "run_process", side_effect=configure_side_effect) as run_process_mock:
                    result = workspace_module.generate_subject_workspace(
                        repo_root,
                        "windows",
                        {"id": "subject/FixtureSubject"},
                    )

            materialized_source_root = repo_root / "verification" / "workspaces" / "subjects" / "FixtureSubject" / "native-source" / "windows-dev-output"
            self.assertTrue((materialized_source_root / "CMakeLists.txt").is_file())
            self.assertTrue((materialized_source_root / "generated" / "CMakeLists.txt").is_file())
            self.assertTrue((materialized_source_root / "proof" / "CMakeLists.txt").is_file())
            self.assertTrue((materialized_source_root / "proof" / "RunSubjectProof.cmake").is_file())
            self.assertFalse((materialized_source_root / "SubjectGeneratedNative.cpp.in").exists())
            self.assertIn(
                'add_subdirectory("${REPO_ROOT}/src/native/runtime-core" "runtime-core")',
                (materialized_source_root / "CMakeLists.txt").read_text(encoding="utf-8"),
            )
            self.assertIn(
                'add_subdirectory("${REPO_ROOT}/src/native/support" "support")',
                (materialized_source_root / "CMakeLists.txt").read_text(encoding="utf-8"),
            )
            self.assertIn(
                'add_subdirectory("${REPO_ROOT}/src/native/bootstrap" "bootstrap")',
                (materialized_source_root / "CMakeLists.txt").read_text(encoding="utf-8"),
            )
            self.assertIn(
                "add_subdirectory(generated)",
                (materialized_source_root / "CMakeLists.txt").read_text(encoding="utf-8"),
            )
            self.assertIn(
                "add_subdirectory(proof)",
                (materialized_source_root / "CMakeLists.txt").read_text(encoding="utf-8"),
            )
            self.assertIn(
                str(
                    (
                        repo_root
                        / "verification" / "workspaces" / "subjects"
                        / "FixtureSubject"
                        / "generated"
                        / "subject-exec"
                        / "analysis"
                        / "generated"
                        / "generated"
                        / "native-reference.generated.cpp"
                    ).as_posix()
                ),
                (materialized_source_root / "generated" / "CMakeLists.txt").read_text(encoding="utf-8"),
            )
            self.assertIn(
                "add_library(chaos_subject_generated_native STATIC EXCLUDE_FROM_ALL",
                (materialized_source_root / "generated" / "CMakeLists.txt").read_text(encoding="utf-8"),
            )
            self.assertNotIn(
                "CHAOS_SUBJECT_PROOF_",
                (materialized_source_root / "generated" / "CMakeLists.txt").read_text(encoding="utf-8"),
            )
            self.assertNotIn(
                "RunSubjectProof.cmake",
                (materialized_source_root / "generated" / "CMakeLists.txt").read_text(encoding="utf-8"),
            )
            self.assertIn(
                'set(CHAOS_SUBJECT_PROOF_RUN_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/RunSubjectProof.cmake")',
                (materialized_source_root / "proof" / "CMakeLists.txt").read_text(encoding="utf-8"),
            )
            self.assertEqual(
                [
                    "cmake",
                    "-S",
                    str(materialized_source_root),
                    "-B",
                    str(repo_root / "verification" / "workspaces" / "subjects" / "FixtureSubject" / "native" / "windows-dev-output"),
                    "-G",
                    "Visual Studio 17 2022",
                    f"-DCHAOS_SUBJECT_REPO_ROOT={repo_root.as_posix()}",
                    "-DCHAOS_SUBJECT_VARIANT=CHECK",
                    f"-DCHAOS_SUBJECT_BUILD_OUT_ROOT={repo_root / 'verification' / 'workspaces' / 'subjects' / 'FixtureSubject' / 'native' / 'windows-dev-output' / 'out'}",
                    f"-DCHAOS_SUBJECT_RUNTIME_ROOT={repo_root / 'verification' / 'workspaces' / 'subjects' / 'FixtureSubject' / 'native' / 'windows-dev-output' / 'runtime'}",
                ],
                run_process_mock.call_args.args[0],
            )
            self.assertTrue((repo_root / result["manifestPath"]).is_file())
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_subject_workspace_refreshes_stale_generated_native_reference_output(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_refresh_stale_generated")
        repo_root = self._make_repo_root("subject-refresh-stale-generated")
        self._write_subject_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")
        generated_source_path = (
            repo_root
            / "artifacts"
            / "subjects"
            / "FixtureSubject"
            / "runs"
            / "subject-exec"
            / "analysis"
            / "generated"
            / "generated"
            / "native-reference.generated.cpp"
        )
        generated_source_path.write_text("int RunNativeReferenceProof() { return 0; }\n", encoding="utf-8")

        try:
            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                write_windows_subject_native_project_stubs(configure_root)
                return completed

            def refresh_side_effect(
                refresh_repo_root: Path,
                subject_id: str,
                matrix_id: str,
                variant: str,
                *,
                run_id: str | None = None,
            ) -> None:
                self.assertEqual("FixtureSubject", subject_id)
                self.assertEqual("windows-dev-output", matrix_id)
                self.assertEqual("CHECK", variant)
                self.assertEqual("subject-exec", run_id)
                refreshed_generated_root = (
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
                refreshed_generated_root.mkdir(parents=True, exist_ok=True)
                (refreshed_generated_root / "native-reference.generated.cpp").write_text(
                    "int RunNativeReference() { return 0; }\n",
                    encoding="utf-8",
                )

            with patch.object(workspace_module, "refresh_subject_generated_root", side_effect=refresh_side_effect) as refresh_mock:
                with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                    with patch.object(workspace_module, "run_process", side_effect=configure_side_effect):
                        result = workspace_module.generate_subject_workspace(
                            repo_root,
                            "windows",
                            {"id": "subject/FixtureSubject"},
                        )

            refresh_mock.assert_called_once()
            mirrored_generated_source = (
                repo_root
                / "verification" / "workspaces" / "subjects"
                / "FixtureSubject"
                / "generated"
                / "subject-exec"
                / "analysis"
                / "generated"
                / "generated"
                / "native-reference.generated.cpp"
            )
            self.assertEqual("int RunNativeReference() { return 0; }\n", mirrored_generated_source.read_text(encoding="utf-8"))
            self.assertTrue((repo_root / result["manifestPath"]).is_file())
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_subject_workspace_refreshes_and_mirrors_generated_source_per_matrix_when_all_targets_requested(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_per_matrix_generated_source")
        repo_root = self._make_repo_root("subject-per-matrix-generated-source")
        self._write_subject_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")
        subject_manifest_path = repo_root / "subjects" / "FixtureSubject" / "subject.manifest.json"
        subject_manifest = json.loads(subject_manifest_path.read_text(encoding="utf-8"))
        subject_manifest["environmentMatrices"].append(
            {
                "matrixId": "windows-reference-trace",
                "pipelineId": "proof-runtime-output",
                "supportedGoals": ["correctness.dev"],
                "source": {
                    "entry": "FixtureSubject/TraceEntry::Run()",
                },
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
                del variant
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
                (generated_root / "native-reference.plan.json").write_text(
                    json.dumps({"matrixId": matrix_id}, indent=2) + "\n",
                    encoding="utf-8",
                )
                (generated_root / "generated" / "native-reference.generated.cpp").write_text(
                    f"// generated for {matrix_id}\n",
                    encoding="utf-8",
                )

            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                configure_root.mkdir(parents=True, exist_ok=True)
                write_windows_subject_native_project_stubs(configure_root)
                return completed

            with patch.object(workspace_module, "refresh_subject_generated_root", side_effect=refresh_side_effect) as refresh_mock:
                with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                    with patch.object(workspace_module, "run_process", side_effect=configure_side_effect):
                        result = workspace_module.generate_subject_workspace(
                            repo_root,
                            "windows",
                            {"id": "subject/FixtureSubject", "all-targets": True, "refresh-generated": True},
                        )

            self.assertEqual(
                ["windows-dev-output", "windows-reference-trace"],
                [call.args[2] for call in refresh_mock.call_args_list],
            )
            self.assertEqual(
                [
                    workspace_module._subject_generated_run_id("windows-dev-output"),
                    workspace_module._subject_generated_run_id("windows-reference-trace"),
                ],
                [call.kwargs["run_id"] for call in refresh_mock.call_args_list],
            )

            manifest = json.loads((repo_root / result["manifestPath"]).read_text(encoding="utf-8"))
            matrices_by_id = {matrix["matrixId"]: matrix for matrix in manifest["matrices"]}

            self.assertNotIn("generatedRoot", matrices_by_id["windows-dev-output"])
            self.assertNotIn("generatedRoot", matrices_by_id["windows-reference-trace"])

            workspace_root = repo_root / "verification" / "workspaces" / "subjects" / "FixtureSubject"
            dev_generated_root = workspace_module._subject_generated_solution_root(
                workspace_root,
                matrix_id="windows-dev-output",
                multi_matrix=True,
            )
            trace_generated_root = workspace_module._subject_generated_solution_root(
                workspace_root,
                matrix_id="windows-reference-trace",
                multi_matrix=True,
            )

            dev_generated_source = (
                dev_generated_root
                / "analysis"
                / "generated"
                / "generated"
                / "native-reference.generated.cpp"
            )
            trace_generated_source = (
                trace_generated_root
                / "analysis"
                / "generated"
                / "generated"
                / "native-reference.generated.cpp"
            )
            self.assertEqual("// generated for windows-dev-output\n", dev_generated_source.read_text(encoding="utf-8"))
            self.assertEqual("// generated for windows-reference-trace\n", trace_generated_source.read_text(encoding="utf-8"))
            self.assertIn(
                "generated/windows-dev-output/subject-exec/analysis/generated/generated/native-reference.generated.cpp",
                (
                    repo_root
                    / "verification" / "workspaces" / "subjects"
                    / "FixtureSubject"
                    / "native-source"
                    / "windows-dev-output"
                    / "generated"
                    / "CMakeLists.txt"
                ).read_text(encoding="utf-8"),
            )
            self.assertIn(
                "generated/windows-reference-trace/subject-exec/analysis/generated/generated/native-reference.generated.cpp",
                (
                    repo_root
                    / "verification" / "workspaces" / "subjects"
                    / "FixtureSubject"
                    / "native-source"
                    / "windows-reference-trace"
                    / "generated"
                    / "CMakeLists.txt"
                ).read_text(encoding="utf-8"),
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)



