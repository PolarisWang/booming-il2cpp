from tests.unit.planning.project_workspace_test_support import *


class TestProjectWorkspaceSubjectTargetsOperations(ProjectWorkspaceTestSupport):
    def test_generate_subject_workspace_supports_generated_open_target(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_generate_open_target")
        repo_root = self._make_repo_root("subject-generate-open-target")
        self._write_subject_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")

        try:
            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                write_windows_subject_native_project_stubs(configure_root)
                return completed

            with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                with patch.object(workspace_module, "run_process", side_effect=configure_side_effect):
                    result = workspace_module.generate_subject_workspace(
                        repo_root,
                        "windows",
                        {"id": "subject/FixtureSubject", "open-native-target": "generated"},
                    )

            manifest = json.loads((repo_root / result["manifestPath"]).read_text(encoding="utf-8"))
            self.assertIn(
                {
                    "label": "Default native project",
                    "path": "verification/workspaces/subjects/FixtureSubject/native/windows-dev-output/generated/chaos_subject_generated_native.vcxproj",
                },
                result["importantOutputs"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_build_subject_workspace_supports_manifest_v2_project_routing(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_build_v2")
        repo_root = self._make_repo_root("subject-build-v2")
        self._write_subject_fixture(repo_root)
        workspace_root = repo_root / "verification" / "workspaces" / "subjects" / "FixtureSubject"
        build_root = workspace_root / "native" / "windows-dev-output"
        build_root.mkdir(parents=True, exist_ok=True)
        manifest_path = workspace_root / "workspace.manifest.json"
        write_json(
            manifest_path,
            {
                "workspaceVersion": 2,
                "kind": "subject-workspace",
                "subjectId": "FixtureSubject",
                "variant": "CHECK",
                "defaultMatrixId": "windows-dev-output",
                "managedSolutionPath": "verification/workspaces/subjects/FixtureSubject/FixtureSubject.sln",
                "managedProjects": [
                    {
                        "projectId": "managed/FixtureSubject/FixtureSubject",
                        "projectPath": "subjects/FixtureSubject/source/FixtureSubject.csproj",
                        "assemblyName": "FixtureSubject",
                        "isPrimary": True,
                    }
                ],
                "managedTestProjects": [
                    {
                        "projectId": "managed-test/FixtureSubject/proof-host",
                        "projectPath": "verification/workspaces/subjects/FixtureSubject/managed-tests/FixtureSubject.DeclaredProofHost.csproj",
                        "assemblyName": "FixtureSubject.DeclaredProofHost",
                        "hostKind": "proof-host",
                    "collectionPath": "verification/workspaces/subjects/FixtureSubject/managed-tests/Generated/declared-tests.collection.json",
                        "generatedSourcePath": "verification/workspaces/subjects/FixtureSubject/managed-tests/Generated/ChaosGeneratedDeclaredTests.g.cs",
                    }
                ],
                "nativeProjects": [
                    {
                        "projectId": "native/FixtureSubject/windows-dev-output/generated-native",
                        "matrixId": "windows-dev-output",
                        "projectPath": "verification/workspaces/subjects/FixtureSubject/native/windows-dev-output/generated/chaos_subject_generated_native.vcxproj",
                        "configureRoot": "verification/workspaces/subjects/FixtureSubject/native/windows-dev-output",
                        "targetPlatform": "windows-x64",
                        "toolchainProfile": "msvc-reference",
                        "deliveryKind": "generated-static-library",
                        "buildArgs": ["--config", "Release", "--target", "chaos_subject_generated_native"],
                    }
                ],
                "nativeTestProjects": [
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
                    }
                ],
                "matrices": [
                    {
                        "matrixId": "windows-dev-output",
                        "goalIds": ["correctness.dev"],
                        "hostPlatform": "windows-x64",
                        "targetPlatform": "windows-x64",
                        "toolchainProfile": "msvc-reference",
                        "managedProjectIds": ["managed/FixtureSubject/FixtureSubject"],
                        "managedTestProjectIds": ["managed-test/FixtureSubject/proof-host"],
                        "nativeProjectIds": ["native/FixtureSubject/windows-dev-output/generated-native"],
                        "nativeTestProjectIds": ["native-test/FixtureSubject/windows-dev-output/proof-host"],
                    }
                ],
            },
        )

        managed_test_project_path = repo_root / "verification" / "workspaces" / "subjects" / "FixtureSubject" / "managed-tests" / "FixtureSubject.DeclaredProofHost.csproj"
        managed_test_project_path.parent.mkdir(parents=True, exist_ok=True)
        managed_test_project_path.write_text("<Project />\n", encoding="utf-8")

        try:
            with patch.object(workspace_module.tooling_module, "ensure_dotnet_available", return_value=workspace_module.tooling_module.ToolBootstrapResult(ready=True)):
                with patch.object(
                    workspace_module,
                    "run_process",
                    return_value=subprocess.CompletedProcess(["tool"], 0, "", ""),
                ) as run_process_mock:
                    result = workspace_module.build_subject_workspace(
                        repo_root,
                        "windows",
                        {"id": "subject/FixtureSubject", "matrix": "windows-dev-output"},
                    )

            self.assertEqual("verification/workspaces/subjects/FixtureSubject/build.report.json", result["buildReportPath"])
            report = json.loads((repo_root / result["buildReportPath"]).read_text(encoding="utf-8"))
            self.assertEqual(["windows-dev-output"], report["builtMatrices"])
            self.assertEqual(
                ["dotnet", "build", str(repo_root / "subjects" / "FixtureSubject" / "source" / "FixtureSubject.csproj"), "-c", "Release"],
                run_process_mock.call_args_list[0].args[0],
            )
            self.assertEqual(
                ["dotnet", "build", str(managed_test_project_path), "-c", "Release"],
                run_process_mock.call_args_list[1].args[0],
            )
            self.assertEqual(
                [
                    "cmake",
                    "--build",
                    str(build_root),
                    "--config",
                    "Release",
                    "--target",
                    "chaos_subject_reference_proof",
                ],
                run_process_mock.call_args_list[2].args[0],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)


