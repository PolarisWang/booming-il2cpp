from tests.unit.planning.project_workspace_test_support import *


class TestProjectWorkspaceSubjectTargetsAll(ProjectWorkspaceTestSupport):
    def test_generate_subject_workspace_all_targets_skips_native_configure_for_managed_windows_matrix(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_generate_mixed_windows_matrices")
        repo_root = self._make_repo_root("subject-generate-mixed-windows-matrices")
        self._write_subject_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")
        subject_manifest_path = owner_manifest_path(repo_root, "FixtureSubject")
        subject_manifest = json.loads(subject_manifest_path.read_text(encoding="utf-8"))
        subject_manifest["executionPipelines"].append(
            {
                "pipelineId": "managed-runtime-output",
                "stages": [
                    {"stageId": "source-resolve", "kind": "source-resolve", "scope": "shared", "bucket": "source"},
                    {"stageId": "host-input-build", "kind": "host-input-build", "scope": "shared", "bucket": "host-input"},
                    {"stageId": "runtime-managed-output", "kind": "runtime-managed-output", "scope": "matrix", "bucket": "runtime"},
                ],
            }
        )
        subject_manifest["environmentMatrices"].append(
            {
                "matrixId": "windows-managed-output",
                "pipelineId": "managed-runtime-output",
                "supportedGoals": ["correctness.dev"],
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                    "toolchainProfile": "dotnet-managed",
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
                if str(configure_root).endswith("windows-dev-output"):
                    write_windows_subject_native_project_stubs(configure_root)
                else:
                    (configure_root / "linux-x64-packaging.vcxproj").parent.mkdir(parents=True, exist_ok=True)
                    (configure_root / "linux-x64-packaging.vcxproj").write_text("<Project />\n", encoding="utf-8")
                return completed

            with patch.object(workspace_module.compiled_catalog_module, "build_subject_declared_test_catalog", return_value=declared_catalog_fixture()):
                with patch.object(workspace_module, "refresh_subject_generated_root", return_value=None) as refresh_mock:
                    with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                        with patch.object(workspace_module, "run_process", side_effect=configure_side_effect) as run_process_mock:
                            result = workspace_module.generate_subject_workspace(
                                repo_root,
                                "windows",
                                {"id": "subject/FixtureSubject", "all-targets": True},
                            )

            self.assertEqual(0, refresh_mock.call_count)
            self.assertEqual(2, run_process_mock.call_count)
            manifest = json.loads((repo_root / result["manifestPath"]).read_text(encoding="utf-8"))
            matrices_by_id = {entry["matrixId"]: entry for entry in manifest["matrices"]}
            self.assertEqual([], matrices_by_id["windows-managed-output"]["nativeProjectIds"])
            self.assertEqual([], matrices_by_id["windows-managed-output"]["nativeTestProjectIds"])
            self.assertEqual(
                ["native/FixtureSubject/windows-dev-output/generated-native"],
                matrices_by_id["windows-dev-output"]["nativeProjectIds"],
            )
            self.assertEqual(
                ["native-test/FixtureSubject/windows-dev-output/proof-host"],
                matrices_by_id["windows-dev-output"]["nativeTestProjectIds"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_subject_workspace_all_targets_preserves_manifest_default_matrix(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_generate_preserve_default_matrix")
        repo_root = self._make_repo_root("subject-generate-preserve-default-matrix")
        self._write_subject_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")
        subject_manifest_path = owner_manifest_path(repo_root, "FixtureSubject")
        subject_manifest = json.loads(subject_manifest_path.read_text(encoding="utf-8"))
        subject_manifest["executionPipelines"].append(
            {
                "pipelineId": "managed-runtime-output",
                "stages": [
                    {"stageId": "source-resolve", "kind": "source-resolve", "scope": "shared", "bucket": "source"},
                    {"stageId": "host-input-build", "kind": "host-input-build", "scope": "shared", "bucket": "host-input"},
                    {"stageId": "runtime-managed-output", "kind": "runtime-managed-output", "scope": "matrix", "bucket": "runtime"},
                ],
            }
        )
        subject_manifest["environmentMatrices"] = [
            {
                "matrixId": "windows-managed-output",
                "pipelineId": "managed-runtime-output",
                "supportedGoals": ["correctness.dev"],
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                    "toolchainProfile": "dotnet-managed",
                },
                "artifactPlan": {
                    "evidenceTerminalBucket": "runtime",
                },
            },
            *subject_manifest["environmentMatrices"],
        ]
        write_json(subject_manifest_path, subject_manifest)

        try:
            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                if str(configure_root).endswith("windows-dev-output"):
                    write_windows_subject_native_project_stubs(configure_root)
                else:
                    (configure_root / "linux-x64-packaging.vcxproj").parent.mkdir(parents=True, exist_ok=True)
                    (configure_root / "linux-x64-packaging.vcxproj").write_text("<Project />\n", encoding="utf-8")
                return completed

            with patch.object(workspace_module.compiled_catalog_module, "build_subject_declared_test_catalog", return_value=declared_catalog_fixture()):
                with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                    with patch.object(workspace_module, "run_process", side_effect=configure_side_effect):
                        result = workspace_module.generate_subject_workspace(
                            repo_root,
                            "windows",
                            {"id": "subject/FixtureSubject", "all-targets": True},
                        )

            manifest = json.loads((repo_root / result["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("windows-dev-output", manifest["defaultMatrixId"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_subject_workspace_all_targets_includes_native_aot_workspace_configure(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_generate_native_aot_workspace")
        repo_root = self._make_repo_root("subject-generate-native-aot-matrix")
        self._write_subject_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")
        subject_manifest_path = owner_manifest_path(repo_root, "FixtureSubject")
        subject_manifest = json.loads(subject_manifest_path.read_text(encoding="utf-8"))
        subject_manifest["executionPipelines"].append(
            {
                "pipelineId": "native-aot-benchmark",
                "stages": [
                    {"stageId": "source-resolve", "kind": "source-resolve", "scope": "shared", "bucket": "source"},
                    {"stageId": "host-input-build", "kind": "host-input-build", "scope": "shared", "bucket": "host-input"},
                    {"stageId": "analysis-frontend", "kind": "analysis-frontend", "scope": "shared", "bucket": "analysis"},
                    {"stageId": "generated-native-aot", "kind": "generated-native-aot", "scope": "shared", "bucket": "generated"},
                ],
            }
        )
        subject_manifest["environmentMatrices"].append(
            {
                "matrixId": "windows-native-perf",
                "pipelineId": "native-aot-benchmark",
                "supportedGoals": ["perf.release"],
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                    "toolchainProfile": "msvc-reference",
                },
                "artifactPlan": {
                    "evidenceTerminalBucket": "report",
                },
            }
        )
        write_json(subject_manifest_path, subject_manifest)

        try:
            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                if str(configure_root).endswith("windows-dev-output"):
                    write_windows_subject_native_project_stubs(configure_root)
                elif str(configure_root).endswith("windows-native-perf"):
                    write_windows_subject_native_project_stubs(configure_root, benchmark_text="<Project />\n")
                else:
                    (configure_root / "linux-x64-packaging.vcxproj").parent.mkdir(parents=True, exist_ok=True)
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
                if matrix_id == "windows-native-perf":
                    (generated_root / "native-aot.manifest.json").write_text(
                        json.dumps({"matrixId": matrix_id}, indent=2) + "\n",
                        encoding="utf-8",
                    )
                    (generated_root / "native-aot.plan.json").write_text(
                        json.dumps({"matrixId": matrix_id}, indent=2) + "\n",
                        encoding="utf-8",
                    )
                    (generated_root / "generated" / "native-aot.generated.cpp").write_text(
                        f"// native aot for {matrix_id}\n",
                        encoding="utf-8",
                    )
                    return
                (generated_root / "native-reference.manifest.json").write_text(
                    json.dumps({"matrixId": matrix_id}, indent=2) + "\n",
                    encoding="utf-8",
                )
                (generated_root / "native-reference.plan.json").write_text(
                    json.dumps({"matrixId": matrix_id}, indent=2) + "\n",
                    encoding="utf-8",
                )
                (generated_root / "generated" / "native-reference.generated.cpp").write_text(
                    f"// proof for {matrix_id}\n",
                    encoding="utf-8",
                )

            with patch.object(workspace_module.compiled_catalog_module, "build_subject_declared_test_catalog", return_value=declared_catalog_fixture()):
                with patch.object(workspace_module, "refresh_subject_generated_root", side_effect=refresh_side_effect) as refresh_mock:
                    with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                        with patch.object(workspace_module, "run_process", side_effect=configure_side_effect) as run_process_mock:
                            result = workspace_module.generate_subject_workspace(
                                repo_root,
                                "windows",
                                {"id": "subject/FixtureSubject", "all-targets": True, "refresh-generated": True},
                            )

            self.assertEqual(
                ["windows-dev-output", "windows-native-perf"],
                [call.args[2] for call in refresh_mock.call_args_list],
            )
            self.assertEqual(3, run_process_mock.call_count)
            manifest = json.loads((repo_root / result["manifestPath"]).read_text(encoding="utf-8"))
            matrices_by_id = {entry["matrixId"]: entry for entry in manifest["matrices"]}
            self.assertEqual(
                ["native/FixtureSubject/windows-native-perf/generated-native"],
                matrices_by_id["windows-native-perf"]["nativeProjectIds"],
            )
            self.assertEqual(
                ["native-test/FixtureSubject/windows-native-perf/benchmark-host"],
                matrices_by_id["windows-native-perf"]["nativeTestProjectIds"],
            )
            benchmark_native_test_project = next(
                item
                for item in manifest["nativeTestProjects"]
                if item["projectId"] == "native-test/FixtureSubject/windows-native-perf/benchmark-host"
            )
            self.assertEqual("benchmark-host", benchmark_native_test_project["hostKind"])
            self.assertEqual(
                "managed-test/FixtureSubject/benchmark-host",
                benchmark_native_test_project["managedTestProjectId"],
            )
            self.assertEqual(
                "verification/workspaces/subjects/FixtureSubject/native/windows-native-perf/benchmark/chaos_subject_native_aot.vcxproj",
                benchmark_native_test_project["projectPath"],
            )
            self.assertTrue((repo_root / benchmark_native_test_project["projectPath"]).is_file())
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

