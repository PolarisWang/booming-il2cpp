from tests.unit.planning.project_workspace_test_support import *


class TestProjectWorkspaceCore(ProjectWorkspaceTestSupport):
    def test_generate_core_workspace_selects_all_host_targets_and_configures_them(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_core_generate")
        repo_root = self._make_repo_root("core-generate")
        self._write_core_fixture(repo_root)

        try:
            with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                with patch.object(
                    workspace_module,
                    "run_process",
                    return_value=subprocess.CompletedProcess(["cmake"], 0, "", ""),
                ) as run_process_mock:
                    result = workspace_module.generate_core_workspace(
                        repo_root,
                        "windows",
                        {"all-targets": True},
                    )

            manifest = json.loads((repo_root / result["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("core-workspace", manifest["kind"])
            self.assertEqual("windows", manifest["hostPlatform"])
            self.assertEqual("verification/workspaces/core/windows/chaos-il2cpp-core.sln", manifest["managedSolutionPath"])
            self.assertEqual(
                [
                    "windows-x64-reference",
                    "android-arm64",
                    "linux-x64",
                ],
                [item["targetId"] for item in manifest["nativeTargets"]],
            )
            self.assertEqual(
                [
                    "direct-run-host",
                    "project-package-root",
                    "project-package-root",
                ],
                [item["deliveryKind"] for item in manifest["nativeTargets"]],
            )
            self.assertEqual(
                [
                    "verification/workspaces/core/windows/native/windows-x64-reference",
                    "verification/workspaces/core/windows/native/android-arm64",
                    "verification/workspaces/core/windows/native/linux-x64",
                ],
                [item["configureRoot"] for item in manifest["nativeTargets"]],
            )
            self.assertEqual(3, run_process_mock.call_count)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
    def test_build_core_workspace_uses_manifest_and_selected_target(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_core_build")
        repo_root = self._make_repo_root("core-build")
        self._write_core_fixture(repo_root)
        workspace_root = repo_root / "verification" / "workspaces" / "core" / "windows"
        (workspace_root / "native" / "windows-x64-reference").mkdir(parents=True, exist_ok=True)
        manifest_path = workspace_root / "workspace.manifest.json"
        write_json(
            manifest_path,
            {
                "kind": "core-workspace",
                "hostPlatform": "windows",
                "managedSolutionPath": "verification/workspaces/core/windows/chaos-il2cpp-core.sln",
                "managedProjects": [
                    "src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj",
                    "src/managed/Chaos.IL2CPP.Generator/Chaos.IL2CPP.Generator.csproj",
                ],
                "nativeTargets": [
                    {
                        "targetId": "windows-x64-reference",
                        "configureRoot": "verification/workspaces/core/windows/native/windows-x64-reference",
                        "buildArgs": ["--config", "Release"],
                    },
                    {
                        "targetId": "linux-x64",
                        "configureRoot": "verification/workspaces/core/windows/native/linux-x64",
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
                    result = workspace_module.build_core_workspace(
                        repo_root,
                        "windows",
                        {"target": "windows-x64-reference"},
                    )

            self.assertEqual("verification/workspaces/core/windows/build.report.json", result["buildReportPath"])
            report = json.loads((repo_root / result["buildReportPath"]).read_text(encoding="utf-8"))
            self.assertEqual(["windows-x64-reference"], report["builtTargets"])
            self.assertEqual(
                [
                    "cmake",
                    "--build",
                    str(repo_root / "verification" / "workspaces" / "core" / "windows" / "native" / "windows-x64-reference"),
                    "--config",
                    "Release",
                ],
                run_process_mock.call_args_list[2].args[0],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)


