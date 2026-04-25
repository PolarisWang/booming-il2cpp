from tests.unit.execution.subject_workers_test_support import *


class TestSubjectWorkersHostInputBuildRefreshNative(SubjectWorkersTestSupport):
    def test_host_input_build_regenerates_stale_workspace_before_using_declared_benchmark_host(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_host_input_workspace_benchmark_host_refresh")
        subject_id = "FixtureWorkspaceBenchmarkHostRefreshSubject"
        run_id = "fixture-run-host-input-workspace-benchmark-host-refresh-001"
        intermediate_root = TEST_TMP_ROOT / "dotnet-intermediates" / "fixture-host-input-workspace-benchmark-host-refresh-1234"
        request = {
            "selection": {
                "subjectId": subject_id,
                "entrySelection": {
                    "family": "declared-benchmark",
                    "stableId": f"{subject_id}::{subject_id}::{subject_id}.Benchmarks::RunWorkload()",
                    "alias": "workspace-benchmark-refresh",
                    "entryIndex": 11,
                },
                "source": {
                    "type": "dotnet-project",
                    "path": subject_source_path(subject_id),
                    "entry": f"{subject_id}/Program::Main(System.String[])",
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                },
            },
            "upstream": {
                "source": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "source", "source.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "analysis", "host-input"),
                "manifestPath": subject_run_path(subject_id, run_id, "analysis", "host-input", "host-input.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("host-input-build-workspace-benchmark-host-refresh")
        try:
            source_root = repo_root / "subjects" / subject_id / "source"
            source_root.mkdir(parents=True, exist_ok=True)
            (source_root / f"{subject_id}.csproj").write_text("<Project />\n", encoding="utf-8")

            workspace_root = repo_root / "verification" / "workspaces" / "subjects" / subject_id
            stale_managed_tests_root = workspace_root / "managed-tests"
            stale_generated_root = stale_managed_tests_root / "Generated"
            stale_generated_root.mkdir(parents=True, exist_ok=True)
            stale_host_project_path = stale_managed_tests_root / "StaleBenchmarkHost.csproj"
            stale_host_project_path.write_text("<Project />\n", encoding="utf-8")
            stale_collection_path = stale_generated_root / "declared-tests.collection.json"
            stale_collection_path.write_text('{"declaredBenchmarks":[{"entryIndex":1}]}', encoding="utf-8")
            workspace_manifest_path = workspace_root / "workspace.manifest.json"
            workspace_manifest_path.write_text(
                json.dumps(
                    {
                        "workspaceVersion": 2,
                        "subjectId": subject_id,
                        "managedTestProjects": [
                            {
                                "projectId": f"managed-test/{subject_id}/benchmark-host",
                                "projectPath": posix_path(
                                    "verification", "workspaces",
                                    "subjects",
                                    subject_id,
                                    "managed-tests",
                                    "StaleBenchmarkHost.csproj",
                                ),
                                "assemblyName": f"{subject_id}.DeclaredBenchmarkHost",
                                "hostKind": "benchmark-host",
                                "collectionPath": posix_path(
                                    "verification", "workspaces",
                                    "subjects",
                                    subject_id,
                                    "managed-tests",
                                    "Generated",
                                    "declared-tests.collection.json",
                                ),
                            }
                        ],
                    }
                ),
                encoding="utf-8",
            )

            time.sleep(1.1)
            write_owner_manifest(
                repo_root,
                subject_id,
                {
                    "subjectId": subject_id,
                    "source": {
                        "type": "dotnet-project",
                        "path": subject_source_path(subject_id),
                        "primaryProjectPath": subject_source_path(subject_id),
                    },
                },
            )

            expected_output_root = repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "host-input"

            def fake_generate_subject_workspace(
                repo_root_arg: Path,
                host_platform_arg: str,
                options_arg: dict[str, object],
                **kwargs: object,
            ) -> dict[str, object]:
                del kwargs
                self.assertEqual(repo_root, repo_root_arg)
                self.assertEqual("windows", host_platform_arg)
                self.assertEqual(f"subject/{subject_id}", options_arg["id"])
                self.assertTrue(bool(options_arg["all-targets"]))
                self.assertTrue(bool(options_arg["refresh-generated"]))

                stale_collection_path.write_text('{"declaredBenchmarks":[{"entryIndex":11}]}', encoding="utf-8")
                workspace_manifest_path.write_text(
                    json.dumps(
                        {
                            "workspaceVersion": 2,
                            "subjectId": subject_id,
                            "managedTestProjects": [
                                {
                                    "projectId": f"managed-test/{subject_id}/benchmark-host",
                                    "projectPath": SHARED_RUNTIME_PROJECT_PATH,
                                    "assemblyName": SHARED_RUNTIME_ASSEMBLY_NAME,
                                    "hostKind": "benchmark-host",
                                    "collectionPath": posix_path(
                                        "verification", "workspaces",
                                        "subjects",
                                        subject_id,
                                        "managed-tests",
                                        "Generated",
                                        "declared-tests.collection.json",
                                    ),
                                    "executionModel": "shared-runtime-host",
                                }
                            ],
                        }
                    ),
                    encoding="utf-8",
                )
                return {
                    "manifestPath": posix_path("verification", "workspaces", "subjects", subject_id, "workspace.manifest.json"),
                }

            class FakeProjectWorkspaceModule:
                @staticmethod
                def generate_subject_workspace(
                    repo_root_arg: Path,
                    host_platform_arg: str,
                    options_arg: dict[str, object],
                    **kwargs: object,
                ) -> dict[str, object]:
                    return fake_generate_subject_workspace(
                        repo_root_arg,
                        host_platform_arg,
                        options_arg,
                        **kwargs,
                    )

            source_project_path = repo_root / "subjects" / subject_id / "source" / f"{subject_id}.csproj"
            expected_calls = [
                [
                    "dotnet",
                    "build",
                    str(source_project_path),
                    "-c",
                    "Release",
                    "-m:1",
                    "-o",
                    str(expected_output_root),
                    f"-p:ChaosTempIntermediateRoot={intermediate_root.as_posix()}/",
                ],
                [
                    "dotnet",
                    "build",
                    str(repo_root / SHARED_RUNTIME_PROJECT_PATH),
                    "-c",
                    "Release",
                    "-m:1",
                    "-o",
                    str(expected_output_root),
                    f"-p:ChaosTempIntermediateRoot={intermediate_root.as_posix()}/",
                ],
            ]

            def fake_run_checked(arguments: list[str], *, repo_root: Path, failure_message: str) -> str:
                del failure_message
                self.assertTrue(expected_calls)
                self.assertEqual(expected_calls.pop(0), arguments)
                expected_output_root.mkdir(parents=True, exist_ok=True)
                for file_name in [
                    f"{subject_id}.dll",
                    "Chaos.TestFramework.Runtime.deps.json",
                    "Chaos.TestFramework.Sdk.dll",
                    "Chaos.TestFramework.Runtime.dll",
                ]:
                    (expected_output_root / file_name).write_text("", encoding="utf-8")
                return ""

            with patch.object(workers_module.tooling_module, "allocate_dotnet_intermediate_dir", return_value=intermediate_root):
                with patch.object(workers_module, "_load_project_workspace_module", return_value=FakeProjectWorkspaceModule()):
                    with patch.object(workers_module, "_run_checked", side_effect=fake_run_checked):
                        result = workers_module.run_dotnet_host_input_builder(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            self.assertEqual([], expected_calls)
            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(SHARED_RUNTIME_PROJECT_PATH, manifest["primaryProjectPath"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_host_input_build_uses_native_specific_benchmark_host_when_native_test_project_declares_it(self) -> None:
        workers_module = load_module(
            SUBJECT_WORKERS_MODULE_PATH,
            "chaos_subject_workers_host_input_workspace_native_benchmark_host",
        )
        subject_id = "FixtureWorkspaceNativeBenchmarkHostSubject"
        run_id = "fixture-run-host-input-workspace-native-benchmark-host-001"
        matrix_id = "windows-native-perf"
        intermediate_root = TEST_TMP_ROOT / "dotnet-intermediates" / "fixture-host-input-workspace-native-benchmark-host-1234"
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "entrySelection": {
                    "family": "declared-benchmark",
                    "stableId": f"{subject_id}::{subject_id}::{subject_id}.Benchmarks::RunNativeWorkload()",
                    "alias": "workspace-native-benchmark",
                    "entryIndex": 11,
                },
                "source": {
                    "type": "dotnet-project",
                    "path": subject_source_path(subject_id),
                    "entry": f"{subject_id}/Program::Main(System.String[])",
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "runtimeProfile": "native-perf-release",
                    "targetPlatform": "windows-x64",
                },
            },
            "upstream": {
                "source": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "source", "source.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "analysis", "host-input"),
                "manifestPath": subject_run_path(subject_id, run_id, "analysis", "host-input", "host-input.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("host-input-build-workspace-native-benchmark-host")
        try:
            source_root = repo_root / "subjects" / subject_id / "source"
            source_root.mkdir(parents=True, exist_ok=True)
            (source_root / f"{subject_id}.csproj").write_text("<Project />\n", encoding="utf-8")

            workspace_root = repo_root / "verification" / "workspaces" / "subjects" / subject_id
            managed_tests_root = workspace_root / "managed-tests"
            generated_root = managed_tests_root / "Generated"
            generated_root.mkdir(parents=True, exist_ok=True)
            benchmark_host_project_path = managed_tests_root / f"{subject_id}.DeclaredBenchmarkHost.csproj"
            benchmark_host_project_path.write_text("<Project />\n", encoding="utf-8")
            native_benchmark_host_project_path = managed_tests_root / f"{subject_id}.DeclaredBenchmarkNativeHost.csproj"
            native_benchmark_host_project_path.write_text("<Project />\n", encoding="utf-8")
            collection_path = generated_root / "declared-tests.collection.json"
            collection_path.write_text('{"declaredBenchmarks":[{"entryIndex":11}]}', encoding="utf-8")
            workspace_manifest_path = workspace_root / "workspace.manifest.json"
            workspace_manifest_path.write_text(
                json.dumps(
                    {
                        "workspaceVersion": 2,
                        "subjectId": subject_id,
                        "managedTestProjects": [
                            {
                                "projectId": f"managed-test/{subject_id}/benchmark-host",
                                "projectPath": posix_path(
                                    "verification", "workspaces",
                                    "subjects",
                                    subject_id,
                                    "managed-tests",
                                    f"{subject_id}.DeclaredBenchmarkHost.csproj",
                                ),
                                "assemblyName": f"{subject_id}.DeclaredBenchmarkHost",
                                "hostKind": "benchmark-host",
                                "collectionPath": posix_path(
                                    "verification", "workspaces",
                                    "subjects",
                                    subject_id,
                                    "managed-tests",
                                    "Generated",
                                    "declared-tests.collection.json",
                                ),
                            },
                            {
                                "projectId": f"managed-test/{subject_id}/benchmark-host-native",
                                "projectPath": posix_path(
                                    "verification", "workspaces",
                                    "subjects",
                                    subject_id,
                                    "managed-tests",
                                    f"{subject_id}.DeclaredBenchmarkNativeHost.csproj",
                                ),
                                "assemblyName": f"{subject_id}.DeclaredBenchmarkNativeHost",
                                "hostKind": "benchmark-host",
                                "collectionPath": posix_path(
                                    "verification", "workspaces",
                                    "subjects",
                                    subject_id,
                                    "managed-tests",
                                    "Generated",
                                    "declared-tests.collection.json",
                                ),
                            },
                        ],
                        "nativeTestProjects": [
                            {
                                "projectId": f"native-test/{subject_id}/{matrix_id}/benchmark-host",
                                "matrixId": matrix_id,
                                "projectPath": posix_path(
                                    "verification", "workspaces",
                                    "subjects",
                                    subject_id,
                                    "native",
                                    matrix_id,
                                    "benchmark",
                                    "chaos_subject_native_aot.vcxproj",
                                ),
                                "configureRoot": posix_path(
                                    "verification", "workspaces",
                                    "subjects",
                                    subject_id,
                                    "native",
                                    matrix_id,
                                ),
                                "hostKind": "benchmark-host",
                                "managedTestProjectId": f"managed-test/{subject_id}/benchmark-host-native",
                            }
                        ],
                    }
                ),
                encoding="utf-8",
            )
            expected_output_root = repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "host-input"

            def fake_run_checked(arguments: list[str], *, repo_root: Path, failure_message: str) -> str:
                del failure_message
                self.assertEqual(
                    [
                        "dotnet",
                        "build",
                        str(native_benchmark_host_project_path),
                        "-c",
                        "Release",
                        "-m:1",
                        "-o",
                        str(expected_output_root),
                        f"-p:ChaosTempIntermediateRoot={intermediate_root.as_posix()}/",
                    ],
                    arguments,
                )
                expected_output_root.mkdir(parents=True, exist_ok=True)
                for file_name in [
                    f"{subject_id}.DeclaredBenchmarkNativeHost.dll",
                    f"{subject_id}.DeclaredBenchmarkNativeHost.deps.json",
                    f"{subject_id}.DeclaredBenchmarkNativeHost.pdb",
                    f"{subject_id}.dll",
                    "Chaos.TestFramework.Sdk.dll",
                    "Chaos.TestFramework.Runtime.dll",
                ]:
                    (expected_output_root / file_name).write_text("", encoding="utf-8")
                return ""

            with patch.object(workers_module.tooling_module, "allocate_dotnet_intermediate_dir", return_value=intermediate_root):
                with patch.object(workers_module, "_workspace_manifest_is_stale", return_value=False):
                    with patch.object(workers_module, "_run_checked", side_effect=fake_run_checked):
                        result = workers_module.run_dotnet_host_input_builder(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(
                posix_path("verification", "workspaces", "subjects", subject_id, "managed-tests", f"{subject_id}.DeclaredBenchmarkNativeHost.csproj"),
                manifest["primaryProjectPath"],
            )
            self.assertEqual("benchmark-host", manifest["hostKind"])
            self.assertEqual(
                posix_path("verification", "workspaces", "subjects", subject_id, "managed-tests", "Generated", "declared-tests.collection.json"),
                manifest["collectionPath"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.DeclaredBenchmarkNativeHost.dll"),
                manifest["primaryAssemblyPath"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)


