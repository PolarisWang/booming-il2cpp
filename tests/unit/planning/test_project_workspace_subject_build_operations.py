from tests.unit.planning.project_workspace_test_support import *


class TestProjectWorkspaceSubjectBuildOperations(ProjectWorkspaceTestSupport):
    def test_build_subject_workspace_supports_generated_native_target_selection(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_build_generated_target")
        repo_root = self._make_repo_root("subject-build-generated-target")
        self._write_subject_fixture(repo_root)
        workspace_root = repo_root / "verification" / "workspaces" / "subjects" / "FixtureSubject"
        build_root = workspace_root / "native" / "windows-dev-output"
        build_root.mkdir(parents=True, exist_ok=True)
        manifest_path = workspace_root / "workspace.manifest.json"
        write_json(
            manifest_path,
            {
                "kind": "subject-workspace",
                "subjectId": "FixtureSubject",
                "defaultMatrix": "windows-dev-output",
                "managedSolutionPath": "verification/workspaces/subjects/FixtureSubject/FixtureSubject.sln",
                "managedProjects": [
                    "subjects/FixtureSubject/source/FixtureSubject.csproj",
                    "subjects/FixtureSubject/validation/unit/FixtureSubject.Subject.UnitTests/FixtureSubject.Subject.UnitTests.csproj",
                ],
                "matrices": [
                    {
                        "matrixId": "windows-dev-output",
                        "targetPlatform": "windows-x64",
                        "configureRoot": "verification/workspaces/subjects/FixtureSubject/native/windows-dev-output",
                        "buildArgs": ["--config", "Release", "--target", "chaos_subject_reference_proof"],
                        "defaultBuildNativeProject": "chaos_subject_reference_proof",
                        "nativeProjects": [
                            {
                                "targetId": "chaos_subject_generated_native",
                                "kind": "generated-native",
                                "projectPath": "verification/workspaces/subjects/FixtureSubject/native/windows-dev-output/generated/chaos_subject_generated_native.vcxproj",
                                "buildArgs": ["--config", "Release", "--target", "chaos_subject_generated_native"],
                            },
                            {
                                "targetId": "chaos_subject_reference_proof",
                                "kind": "proof-native",
                                "projectPath": "verification/workspaces/subjects/FixtureSubject/native/windows-dev-output/proof/chaos_subject_reference_proof.vcxproj",
                                "buildArgs": ["--config", "Release", "--target", "chaos_subject_reference_proof"],
                            },
                        ],
                    }
                ],
            },
        )

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
                        {"id": "subject/FixtureSubject", "matrix": "windows-dev-output", "native-target": "generated"},
                    )

            self.assertEqual("verification/workspaces/subjects/FixtureSubject/build.report.json", result["buildReportPath"])
            self.assertEqual(
                [
                    "cmake",
                    "--build",
                    str(build_root),
                    "--config",
                    "Release",
                    "--target",
                    "chaos_subject_generated_native",
                ],
                run_process_mock.call_args_list[2].args[0],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_build_subject_workspace_uses_manifest_and_selected_matrix(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_build")
        repo_root = self._make_repo_root("subject-build")
        self._write_subject_fixture(repo_root)
        workspace_root = repo_root / "verification" / "workspaces" / "subjects" / "FixtureSubject"
        build_root = workspace_root / "native" / "windows-dev-output"
        build_root.mkdir(parents=True, exist_ok=True)
        manifest_path = workspace_root / "workspace.manifest.json"
        write_json(
            manifest_path,
            {
                "kind": "subject-workspace",
                "subjectId": "FixtureSubject",
                "managedSolutionPath": "verification/workspaces/subjects/FixtureSubject/FixtureSubject.sln",
                "managedProjects": [
                    "subjects/FixtureSubject/source/FixtureSubject.csproj",
                    "subjects/FixtureSubject/validation/unit/FixtureSubject.Subject.UnitTests/FixtureSubject.Subject.UnitTests.csproj",
                ],
                "matrices": [
                    {
                        "matrixId": "windows-dev-output",
                        "configureRoot": "verification/workspaces/subjects/FixtureSubject/native/windows-dev-output",
                        "buildArgs": ["--config", "Release", "--target", "chaos_subject_reference_proof"],
                    },
                    {
                        "matrixId": "windows-linux-buildable",
                        "configureRoot": "verification/workspaces/subjects/FixtureSubject/native/windows-linux-buildable",
                        "buildArgs": [],
                    },
                ],
            },
        )

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

    def test_build_subject_workspace_skips_cmake_for_managed_only_matrix(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_build_managed_only")
        repo_root = self._make_repo_root("subject-build-managed-only")
        self._write_managed_only_subject_fixture(repo_root)
        workspace_root = repo_root / "verification" / "workspaces" / "subjects" / "ManagedOnlySubject"
        manifest_path = workspace_root / "workspace.manifest.json"
        managed_test_project_path = workspace_root / "managed-tests" / "ManagedOnlySubject.DeclaredBenchmarkHost.csproj"
        managed_test_project_path.parent.mkdir(parents=True, exist_ok=True)
        managed_test_project_path.write_text("<Project />\n", encoding="utf-8")
        write_json(
            manifest_path,
            {
                "kind": "subject-workspace",
                "subjectId": "ManagedOnlySubject",
                "defaultMatrix": "windows-perf-dev",
                "managedSolutionPath": "verification/workspaces/subjects/ManagedOnlySubject/ManagedOnlySubject.sln",
                "managedProjects": [
                    {
                        "projectId": "managed/ManagedOnlySubject/ManagedOnlySubject",
                        "projectPath": "subjects/ManagedOnlySubject/source/ManagedOnlySubject.csproj",
                    }
                ],
                "managedTestProjects": [
                    {
                        "projectId": "managed-test/ManagedOnlySubject/benchmark-host",
                        "projectPath": "verification/workspaces/subjects/ManagedOnlySubject/managed-tests/ManagedOnlySubject.DeclaredBenchmarkHost.csproj",
                    }
                ],
                "nativeProjects": [],
                "nativeTestProjects": [],
                "matrices": [
                    {
                        "matrixId": "windows-perf-dev",
                        "managedProjectIds": ["managed/ManagedOnlySubject/ManagedOnlySubject"],
                        "managedTestProjectIds": ["managed-test/ManagedOnlySubject/benchmark-host"],
                        "nativeProjectIds": [],
                        "nativeTestProjectIds": [],
                    }
                ],
            },
        )

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
                        {"id": "subject/ManagedOnlySubject", "matrix": "windows-perf-dev"},
                    )

            self.assertEqual("verification/workspaces/subjects/ManagedOnlySubject/build.report.json", result["buildReportPath"])
            report = json.loads((repo_root / result["buildReportPath"]).read_text(encoding="utf-8"))
            self.assertEqual(["windows-perf-dev"], report["builtMatrices"])
            self.assertEqual(2, run_process_mock.call_count)
            self.assertEqual(
                ["dotnet", "build", str(repo_root / "subjects" / "ManagedOnlySubject" / "source" / "ManagedOnlySubject.csproj"), "-c", "Release"],
                run_process_mock.call_args_list[0].args[0],
            )
            self.assertEqual(
                ["dotnet", "build", str(managed_test_project_path), "-c", "Release"],
                run_process_mock.call_args_list[1].args[0],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)


