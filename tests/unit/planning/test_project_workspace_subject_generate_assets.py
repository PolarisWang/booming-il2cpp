from tests.unit.planning.project_workspace_test_support import *


class TestProjectWorkspaceSubjectGenerateAssets(ProjectWorkspaceTestSupport):
    def test_native_workspace_materialization_is_backed_by_template_assets(self) -> None:
        module_source = PROJECT_WORKSPACE_MODULE_PATH.read_text(encoding="utf-8")

        self.assertIn("templates/native-reference-workspace.cmake.tmpl", module_source)
        self.assertIn("templates/native-generated.cmake.tmpl", module_source)
        self.assertIn("templates/native-proof.cmake.tmpl", module_source)
        self.assertIn("templates/native-proof-main.cpp.tmpl", module_source)
        self.assertIn("templates/native-proof-run.cmake.tmpl", module_source)
        self.assertIn("templates/native-aot-workspace.cmake.tmpl", module_source)
        self.assertIn("templates/native-benchmark.cmake.tmpl", module_source)
        self.assertTrue((SUBJECT_TEMPLATES_ROOT / "native-reference-workspace.cmake.tmpl").is_file())
        self.assertTrue((SUBJECT_TEMPLATES_ROOT / "native-generated.cmake.tmpl").is_file())
        self.assertTrue((SUBJECT_TEMPLATES_ROOT / "native-proof.cmake.tmpl").is_file())
        self.assertTrue((SUBJECT_TEMPLATES_ROOT / "native-proof-main.cpp.tmpl").is_file())
        self.assertTrue((SUBJECT_TEMPLATES_ROOT / "native-proof-run.cmake.tmpl").is_file())
        self.assertTrue((SUBJECT_TEMPLATES_ROOT / "native-aot-workspace.cmake.tmpl").is_file())
        self.assertTrue((SUBJECT_TEMPLATES_ROOT / "native-benchmark.cmake.tmpl").is_file())

    def test_generate_subject_workspace_writes_manifest_solution_and_native_configure_root(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_generate")
        repo_root = self._make_repo_root("subject-generate")
        self._write_subject_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")

        try:
            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                write_windows_subject_native_project_stubs(configure_root)
                return completed

            with patch.object(workspace_module.compiled_catalog_module, "build_subject_declared_test_catalog", return_value=declared_catalog_fixture()):
                with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                    with patch.object(workspace_module, "run_process", side_effect=configure_side_effect) as run_process_mock:
                        result = workspace_module.generate_subject_workspace(
                            repo_root,
                            "windows",
                            {"id": "subject/FixtureSubject"},
                        )

            manifest_path = repo_root / result["manifestPath"]
            self.assertTrue(manifest_path.is_file())
            manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
            self.assertEqual(2, manifest["workspaceVersion"])
            self.assertEqual("FixtureSubject", manifest["subjectId"])
            self.assertEqual("CHECK", manifest["variant"])
            self.assertEqual("windows-dev-output", manifest["defaultMatrixId"])
            self.assertEqual(
                "verification/workspaces/subjects/FixtureSubject/FixtureSubject.sln",
                manifest["managedSolutionPath"],
            )
            self.assertEqual(
                [
                    {
                        "projectId": "managed/FixtureSubject/FixtureSubject",
                        "projectPath": "subjects/FixtureSubject/source/FixtureSubject.csproj",
                        "assemblyName": "FixtureSubject",
                        "isPrimary": True,
                    },
                ],
                manifest["managedProjects"],
            )
            self.assertEqual(
                [
                    {
                        "projectId": "managed-test/FixtureSubject/proof-host",
                        "projectPath": "src/reference/Chaos.TestFramework.Runtime/Chaos.TestFramework.Runtime.csproj",
                        "assemblyName": "Chaos.TestFramework.Runtime",
                        "hostKind": "proof-host",
                        "collectionPath": "verification/workspaces/subjects/FixtureSubject/managed-tests/Generated/declared-tests.collection.json",
                        "executionModel": "shared-runtime-host",
                    },
                    {
                        "projectId": "managed-test/FixtureSubject/benchmark-host",
                        "projectPath": "src/reference/Chaos.TestFramework.Runtime/Chaos.TestFramework.Runtime.csproj",
                        "assemblyName": "Chaos.TestFramework.Runtime",
                        "hostKind": "benchmark-host",
                        "collectionPath": "verification/workspaces/subjects/FixtureSubject/managed-tests/Generated/declared-tests.collection.json",
                        "executionModel": "shared-runtime-host",
                    },
                ],
                manifest["managedTestProjects"],
            )
            self.assertEqual(
                [
                    {
                        "projectId": "native/FixtureSubject/windows-dev-output/generated-native",
                        "matrixId": "windows-dev-output",
                        "projectPath": "verification/workspaces/subjects/FixtureSubject/native/windows-dev-output/generated/chaos_subject_generated_native.vcxproj",
                        "configureRoot": "verification/workspaces/subjects/FixtureSubject/native/windows-dev-output",
                        "targetPlatform": "windows-x64",
                        "toolchainProfile": "msvc-reference",
                        "deliveryKind": "generated-static-library",
                        "buildArgs": ["--config", "Release", "--target", "chaos_subject_generated_native"],
                    },
                ],
                manifest["nativeProjects"],
            )
            self.assertEqual(
                [
                    {
                        "projectId": "native-test/FixtureSubject/windows-dev-output/proof-host",
                        "matrixId": "windows-dev-output",
                        "projectPath": "verification/workspaces/subjects/FixtureSubject/native/windows-dev-output/proof/chaos_subject_reference_proof.vcxproj",
                        "configureRoot": "verification/workspaces/subjects/FixtureSubject/native/windows-dev-output",
                        "targetPlatform": "windows-x64",
                        "toolchainProfile": "msvc-reference",
                        "deliveryKind": "direct-run-host",
                        "hostKind": "proof-host",
                        "managedTestProjectId": "managed-test/FixtureSubject/proof-host",
                        "buildArgs": ["--config", "Release", "--target", "chaos_subject_reference_proof"],
                    },
                ],
                manifest["nativeTestProjects"],
            )
            self.assertEqual(1, len(manifest["matrices"]))
            self.assertEqual(
                {
                    "matrixId": "windows-dev-output",
                    "goalIds": ["correctness.dev"],
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                    "toolchainProfile": "msvc-reference",
                    "managedProjectIds": ["managed/FixtureSubject/FixtureSubject"],
                    "managedTestProjectIds": [
                        "managed-test/FixtureSubject/proof-host",
                        "managed-test/FixtureSubject/benchmark-host",
                    ],
                    "nativeProjectIds": ["native/FixtureSubject/windows-dev-output/generated-native"],
                    "nativeTestProjectIds": ["native-test/FixtureSubject/windows-dev-output/proof-host"],
                },
                manifest["matrices"][0],
            )
            self.assertNotIn("visualStudioStateVersion", manifest)
            self.assertNotIn("generatedRoot", manifest)
            self.assertNotIn("defaultOpenNativeProject", manifest["matrices"][0])
            self.assertNotIn("defaultBuildNativeProject", manifest["matrices"][0])
            self.assertNotIn("generatedNativeProjectPath", manifest["matrices"][0])
            self.assertNotIn("proofNativeProjectPath", manifest["matrices"][0])
            self.assertNotIn("nativeProjectPath", manifest["matrices"][0])
            self.assertTrue((repo_root / manifest["managedSolutionPath"]).is_file())
            self.assertTrue((repo_root / manifest["nativeProjects"][0]["projectPath"]).is_file())
            self.assertTrue((repo_root / manifest["nativeTestProjects"][0]["projectPath"]).is_file())
            self.assertTrue((repo_root / manifest["managedTestProjects"][0]["projectPath"]).is_file())
            self.assertTrue((repo_root / manifest["managedTestProjects"][1]["projectPath"]).is_file())
            self.assertTrue((repo_root / manifest["managedTestProjects"][0]["collectionPath"]).is_file())
            mirrored_subject_exec_root = repo_root / "verification" / "workspaces" / "subjects" / "FixtureSubject" / "generated" / "subject-exec"
            self.assertTrue((mirrored_subject_exec_root / "analysis" / "generated" / "generated.manifest.json").is_file())
            self.assertTrue((mirrored_subject_exec_root / "analysis" / "generated" / "native-reference.plan.json").is_file())
            self.assertTrue(
                (mirrored_subject_exec_root / "analysis" / "generated" / "generated" / "native-reference.generated.cpp").is_file()
            )
            solution_text = (repo_root / manifest["managedSolutionPath"]).read_text(encoding="utf-8")
            self.assertIn("FixtureSubject.csproj", solution_text)
            self.assertNotIn("FixtureSubject.DeclaredProofHost.csproj", solution_text)
            self.assertNotIn("FixtureSubject.DeclaredBenchmarkHost.csproj", solution_text)
            self.assertIn("native/windows-dev-output/generated/chaos_subject_generated_native.vcxproj", solution_text)
            self.assertIn("native/windows-dev-output/proof/chaos_subject_reference_proof.vcxproj", solution_text)
            self.assertEqual(
                [
                    "cmake",
                    "-S",
                    str(repo_root / "verification" / "workspaces" / "subjects" / "FixtureSubject" / "native-source" / "windows-dev-output"),
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
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_subject_workspace_materializes_template_based_proof_host_without_legacy_subject_source(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_generate_template_proof_host")
        repo_root = self._make_repo_root("subject-generate-template-proof-host")
        self._write_subject_fixture(repo_root, include_legacy_native_reference=False)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")

        try:
            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                write_windows_subject_native_project_stubs(configure_root)
                return completed

            with patch.object(workspace_module.compiled_catalog_module, "build_subject_declared_test_catalog", return_value=declared_catalog_fixture()):
                with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                    with patch.object(workspace_module, "run_process", side_effect=configure_side_effect):
                        result = workspace_module.generate_subject_workspace(
                            repo_root,
                            "windows",
                            {"id": "subject/FixtureSubject"},
                        )

            manifest = json.loads((repo_root / result["manifestPath"]).read_text(encoding="utf-8"))
            proof_host_path = (
                repo_root
                / "verification" / "workspaces" / "subjects"
                / "FixtureSubject"
                / "native-source"
                / "windows-dev-output"
                / "proof"
                / "main.cpp"
            )
            self.assertTrue(proof_host_path.is_file())
            self.assertTrue((repo_root / manifest["nativeTestProjects"][0]["projectPath"]).is_file())
            proof_host_text = proof_host_path.read_text(encoding="utf-8")
            self.assertIn('options.image_name_utf8 = "FixtureSubject";', proof_host_text)
            self.assertIn('constexpr const char* kRuntimeTag = "subject-reference-proof";', proof_host_text)
            self.assertIn("constexpr const bool kUseAssemblyBoundDispatch = true;", proof_host_text)
            self.assertIn(
                'constexpr const char* kAssemblyDispatchSubjectId = "FixtureSubject/Program::Main:System.Int32(System.String[])";',
                proof_host_text,
            )
            self.assertIn("NativeReferenceAssemblyDispatchRequestV0 request = {};", proof_host_text)
            self.assertIn("RunNativeReferenceAssembly(", proof_host_text)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)




