from tests.unit.planning.project_workspace_test_support import *


class TestProjectWorkspaceSubjectGenerateCleanup(ProjectWorkspaceTestSupport):
    def test_generate_subject_workspace_removes_obsolete_managed_benchmark_host_artifacts(self) -> None:
        workspace_module = load_module(
            PROJECT_WORKSPACE_MODULE_PATH,
            "chaos_project_workspace_subject_generate_managed_benchmark_host_cleanup",
        )
        subject_id = "FixtureSubject"
        repo_root = self._make_repo_root("subject-generate-managed-benchmark-host-refresh")
        self._write_subject_fixture(repo_root, subject_id=subject_id)
        stale_host_path = (
            repo_root
            / "verification" / "workspaces" / "subjects"
            / subject_id
            / "managed-tests"
            / "Generated"
            / "ChaosGeneratedDeclaredBenchmarks.g.cs"
        )
        stale_host_path.parent.mkdir(parents=True, exist_ok=True)
        stale_host_path.write_text(
            "\n".join(
                [
                    "public sealed record DeclaredBenchmarkEntry(int EntryIndex);",
                    "public static class FixtureSubjectDeclaredBenchmarkHost",
                    "{",
                    "    public static DeclaredBenchmarkEntry[] Entries { get; } = new DeclaredBenchmarkEntry[]",
                    "    {",
                    "        new(0),",
                    "    };",
                    "}",
                    "",
                ]
            ),
            encoding="utf-8",
        )
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")

        try:
            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                write_windows_subject_native_project_stubs(configure_root)
                return completed

            with patch.object(
                workspace_module.compiled_catalog_module,
                "build_subject_declared_test_catalog",
                return_value=declared_catalog_fixture(subject_id),
            ):
                with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                    with patch.object(workspace_module, "run_process", side_effect=configure_side_effect):
                        workspace_module.generate_subject_workspace(
                            repo_root,
                            "windows",
                            {"id": f"subject/{subject_id}"},
                        )

            self.assertFalse(stale_host_path.exists())
            self.assertFalse(
                (
                    repo_root
                    / "verification" / "workspaces" / "subjects"
                    / subject_id
                    / "managed-tests"
                    / f"{subject_id}.DeclaredBenchmarkHost.csproj"
                ).exists()
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_subject_workspace_allows_managed_only_windows_subject_without_native_workspace(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_generate_managed_only")
        repo_root = self._make_repo_root("subject-generate-managed-only")
        self._write_managed_only_subject_fixture(repo_root)

        try:
            with patch.object(
                workspace_module.compiled_catalog_module,
                "build_subject_declared_test_catalog",
                return_value=declared_catalog_fixture("ManagedOnlySubject"),
            ):
                with patch.object(
                    workspace_module.tooling_module,
                    "cmake_environment",
                    side_effect=AssertionError("managed-only workspace generation should not request cmake"),
                ):
                    with patch.object(
                        workspace_module,
                        "refresh_subject_generated_root",
                        side_effect=AssertionError("managed-only workspace generation should not refresh generated source"),
                    ):
                        with patch.object(
                            workspace_module,
                            "run_process",
                            side_effect=AssertionError("managed-only workspace generation should not configure native projects"),
                        ):
                            result = workspace_module.generate_subject_workspace(
                                repo_root,
                                "windows",
                                {"id": "subject/ManagedOnlySubject"},
                            )

            manifest_path = repo_root / result["manifestPath"]
            self.assertTrue(manifest_path.is_file())
            manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
            self.assertEqual("ManagedOnlySubject", manifest["subjectId"])
            self.assertEqual("PROFILE", manifest["variant"])
            self.assertEqual("windows-perf-dev", manifest["defaultMatrixId"])
            self.assertEqual([], manifest["nativeProjects"])
            self.assertEqual([], manifest["nativeTestProjects"])
            self.assertEqual(1, len(manifest["matrices"]))
            self.assertEqual(
                {
                    "matrixId": "windows-perf-dev",
                    "goalIds": ["perf.dev"],
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                    "toolchainProfile": "dotnet-managed",
                    "managedProjectIds": ["managed/ManagedOnlySubject/ManagedOnlySubject"],
                    "managedTestProjectIds": [
                        "managed-test/ManagedOnlySubject/proof-host",
                        "managed-test/ManagedOnlySubject/benchmark-host",
                    ],
                    "nativeProjectIds": [],
                    "nativeTestProjectIds": [],
                },
                manifest["matrices"][0],
            )
            self.assertTrue((repo_root / manifest["managedSolutionPath"]).is_file())
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_subject_workspace_adds_native_benchmark_host_when_native_benchmarks_exist(self) -> None:
        workspace_module = load_module(
            PROJECT_WORKSPACE_MODULE_PATH,
            "chaos_project_workspace_subject_generate_native_benchmark_host",
        )
        repo_root = self._make_repo_root("subject-generate-native-benchmark-host")
        self._write_subject_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")
        subject_manifest_path = repo_root / "subjects" / "FixtureSubject" / "subject.manifest.json"
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
        native_catalog = declared_catalog_fixture()
        native_catalog["declaredBenchmarks"] = [
            {
                **native_catalog["declaredBenchmarks"][0],
                "entryIndex": 5,
                "alias": "native-benchmark",
                "declaringType": "FixtureSubject.NativeBenchmarks",
                "methodName": "RunNative",
                "methodSignature": "RunNative()",
                "modes": 3,
            },
            {
                **native_catalog["declaredBenchmarks"][0],
                "entryIndex": 9,
                "alias": "managed-only-benchmark",
                "stableId": "FixtureSubject::FixtureSubject::FixtureSubject.ManagedBenchmarks::RunManaged()",
                "declaringType": "FixtureSubject.ManagedBenchmarks",
                "methodName": "RunManaged",
                "methodSignature": "RunManaged()",
                "modes": 1,
            },
        ]

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
                else:
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

            with patch.object(
                workspace_module.compiled_catalog_module,
                "build_subject_declared_test_catalog",
                return_value=native_catalog,
            ):
                with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                    with patch.object(workspace_module, "run_process", side_effect=configure_side_effect):
                        with patch.object(workspace_module, "refresh_subject_generated_root", side_effect=refresh_side_effect):
                            result = workspace_module.generate_subject_workspace(
                                repo_root,
                                "windows",
                                {"id": "subject/FixtureSubject", "all-targets": True, "refresh-generated": True},
                            )

            manifest = json.loads((repo_root / result["manifestPath"]).read_text(encoding="utf-8"))
            managed_test_projects = {
                item["projectId"]: item
                for item in manifest["managedTestProjects"]
            }
            self.assertIn("managed-test/FixtureSubject/benchmark-host", managed_test_projects)
            self.assertIn("managed-test/FixtureSubject/benchmark-host-native", managed_test_projects)

            native_benchmark_host = managed_test_projects["managed-test/FixtureSubject/benchmark-host-native"]
            native_benchmark_host_source = (
                repo_root / native_benchmark_host["generatedSourcePath"]
            ).read_text(encoding="utf-8")
            self.assertIn("case 5:", native_benchmark_host_source)
            self.assertIn("global::FixtureSubject.NativeBenchmarks.RunNative();", native_benchmark_host_source)
            self.assertNotIn("case 9:", native_benchmark_host_source)
            self.assertNotIn("ManagedBenchmarks.RunManaged", native_benchmark_host_source)

            benchmark_native_test_project = next(
                item
                for item in manifest["nativeTestProjects"]
                if item["projectId"] == "native-test/FixtureSubject/windows-native-perf/benchmark-host"
            )
            self.assertEqual(
                "managed-test/FixtureSubject/benchmark-host-native",
                benchmark_native_test_project["managedTestProjectId"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

