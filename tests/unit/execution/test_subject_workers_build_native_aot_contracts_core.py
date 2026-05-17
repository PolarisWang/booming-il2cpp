from tests.unit.execution.subject_workers_test_support import *


class TestSubjectWorkersBuildNativeAotContractsCore(SubjectWorkersTestSupport):
    def test_payload_custom_perf_metrics_accepts_flat_numeric_payload(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_flat_perf_payload")

        metrics = workers_module._payload_custom_perf_metrics(
            {
                "mode": "native",
                "subjectId": "BenchArithmetic",
                "elapsedMilliseconds": 0.125,
                "opsPerSecond": 8000.0,
                "checksum": 42,
            }
        )

        self.assertEqual(
            {
                "elapsedMilliseconds": 0.125,
                "opsPerSecond": 8000.0,
                "checksum": 42.0,
            },
            metrics,
        )

    def test_windows_build_target_reuses_workspace_native_benchmark_configure_root_and_records_contract(self) -> None:
        workers_module = load_module(
            SUBJECT_WORKERS_MODULE_PATH,
            "chaos_subject_workers_windows_native_aot_cmake_build_workspace_reuse",
        )
        subject_id = "FixtureNativeAotSubject"
        run_id = "fixture-run-native-aot-build-001"
        matrix_id = "windows-native-perf"
        stable_id = f"{subject_id}::{subject_id}::{subject_id}.Benchmarks::RunWorkload()"
        alias = "fixture-native-aot-benchmark"
        workload_entry = f"{subject_id}/Benchmarks::RunWorkload()"
        collection_path = posix_path(
            "verification", "workspaces",
            "subjects",
            subject_id,
            "managed-tests",
            "Generated",
            "declared-tests.collection.json",
        )
        expected_cmake_path = self._make_non_repo_path("cmake", "bin", "cmake.exe")
        expected_configure_root = posix_path(
            "verification", "workspaces",
            "subjects",
            subject_id,
            "native",
            matrix_id,
        )
        instance_spec = f"{self._make_non_repo_path('visual-studio', '18', 'Professional')},version=18.4.11626.88"
        expected_env = {
            "Path": r"C:\VS\bin;C:\Windows\System32",
            "INCLUDE": r"C:\VS\include",
            "LIB": r"C:\VS\lib",
        }

        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "PROFILE",
                "workloadEntry": workload_entry,
                "entrySelection": {
                    "family": "declared-benchmark",
                    "stableId": stable_id,
                    "alias": alias,
                    "entryIndex": 11,
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                    "toolchainProfile": "msvc-reference",
                },
            },
            "upstream": {
                "generated": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "generated", "generated.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "build"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "build", "build.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("windows-native-aot-build")
        try:
            for relative_path in [
                Path("src/native/benchmark-host/native_aot_main.cpp"),
                Path("artifacts")
                / "subjects"
                / subject_id
                / "runs"
                / run_id
                / "analysis"
                / "generated"
                / "generated"
                / "native-aot.generated.cpp",
            ]:
                absolute_path = repo_root / relative_path
                absolute_path.parent.mkdir(parents=True, exist_ok=True)
                absolute_path.write_text("// fixture\n", encoding="utf-8")

            generated_manifest_path = repo_root / request["upstream"]["generated"]["manifestPath"]
            generated_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            generated_manifest_path.write_text(
                json.dumps(
                    {
                        "generatedSourcePath": subject_run_path(subject_id, run_id, "analysis", "generated", "generated", "native-aot.generated.cpp"),
                        "nativeAotManifestPath": subject_run_path(subject_id, run_id, "analysis", "generated", "native-aot.manifest.json"),
                    }
                ),
                encoding="utf-8",
            )

            workspace_manifest_path = repo_root / "verification" / "workspaces" / "subjects" / subject_id / "workspace.manifest.json"
            workspace_manifest_path.parent.mkdir(parents=True, exist_ok=True)
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
                                "collectionPath": collection_path,
                            }
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
                                "configureRoot": expected_configure_root,
                                "hostKind": "benchmark-host",
                                "managedTestProjectId": f"managed-test/{subject_id}/benchmark-host",
                            }
                        ],
                    }
                ),
                encoding="utf-8",
            )

            expected_cmake_dir = repo_root / expected_configure_root
            expected_cmake_source_root = repo_root / "verification" / "workspaces" / "subjects" / subject_id / "native-source" / matrix_id
            with patch.object(workers_module.tooling_module, "cmake_environment", return_value=(str(expected_cmake_path), {})):
                with patch.object(workers_module.tooling_module, "windows_developer_environment", return_value=expected_env):
                    with patch.object(workers_module.tooling_module, "detect_visual_studio_generator", return_value="Visual Studio 18 2026"):
                        with patch.object(workers_module.tooling_module, "detect_visual_studio_instance_spec", return_value=instance_spec):
                            with patch.object(workers_module.tooling_module, "allocate_cmake_binary_dir") as allocate_cmake_binary_dir_mock:
                                with patch.object(workers_module, "_workspace_manifest_is_stale", return_value=False):
                                    with patch.object(workers_module, "_run_checked") as run_checked_mock:
                                        result = workers_module.run_build_target(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            self.assertEqual(
                [
                    str(expected_cmake_path),
                    "-S",
                    str(expected_cmake_source_root),
                    "-B",
                    str(expected_cmake_dir),
                    "-G",
                    "Visual Studio 18 2026",
                    f"-DCHAOS_SUBJECT_BENCHMARK_HOST_MAIN={(repo_root / 'src' / 'native' / 'benchmark-host' / 'native_aot_main.cpp').as_posix()}",
                    f"-DCHAOS_SUBJECT_GENERATED_INPUT_SOURCE={(repo_root / 'artifacts' / 'subjects' / subject_id / 'runs' / run_id / 'analysis' / 'generated' / 'generated' / 'native-aot.generated.cpp').as_posix()}",
                    "-DCHAOS_SUBJECT_VARIANT=PROFILE",
                    f"-DCHAOS_SUBJECT_BUILD_OUT_ROOT={(repo_root / 'artifacts' / 'subjects' / subject_id / 'runs' / run_id / 'matrices' / matrix_id / 'build' / 'out').as_posix()}",
                    f"-DCMAKE_GENERATOR_INSTANCE={instance_spec}",
                ],
                run_checked_mock.call_args_list[0].args[0],
            )
            self.assertEqual(
                [
                    str(expected_cmake_path),
                    "--build",
                    str(expected_cmake_dir),
                    "--config",
                    "Release",
                    "--target",
                    WINDOWS_NATIVE_AOT_BUILD_TARGET,
                ],
                run_checked_mock.call_args_list[1].args[0],
            )
            self.assertEqual(expected_env, run_checked_mock.call_args_list[0].kwargs["env"])
            self.assertEqual(expected_env, run_checked_mock.call_args_list[1].kwargs["env"])
            allocate_cmake_binary_dir_mock.assert_not_called()

            cmake_source_root = expected_cmake_source_root
            workspace_cmakelists = (cmake_source_root / "CMakeLists.txt").read_text(encoding="utf-8")
            benchmark_cmakelists = (cmake_source_root / "benchmark" / "CMakeLists.txt").read_text(encoding="utf-8")
            self.assertNotIn("third_party/bdwgc", workspace_cmakelists)
            self.assertIn("src/native/hot-update", workspace_cmakelists)
            self.assertIn("chaos_hot_update", benchmark_cmakelists)

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("PROFILE", manifest["variant"])
            self.assertEqual("windows-benchmark-cmake", manifest["buildStrategy"])
            self.assertEqual("native-aot", manifest["buildKind"])
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "generated", "generated", "native-aot.generated.cpp"),
                manifest["generatedSourcePath"],
            )
            self.assertEqual(
                posix_path("src", "native", "benchmark-host", "native_aot_main.cpp"),
                manifest["hostSourcePath"],
            )
            self.assertEqual(
                [subject_run_path(subject_id, run_id, "matrices", matrix_id, "build", "out", f"{WINDOWS_NATIVE_AOT_BUILD_TARGET}.exe")],
                manifest["outputs"],
            )
            self.assertEqual("benchmark-host", manifest["hostKind"])
            self.assertEqual(collection_path, manifest["collectionPath"])
            self.assertEqual(
                f"managed-test/{subject_id}/benchmark-host",
                manifest["managedTestProjectId"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "matrices", matrix_id, "build", "benchmark.dispatch.manifest.json"),
                manifest["dispatchManifestPath"],
            )
            dispatch_manifest = json.loads((repo_root / manifest["dispatchManifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(subject_id, dispatch_manifest["subjectId"])
            self.assertEqual(matrix_id, dispatch_manifest["matrixId"])
            self.assertEqual("benchmark-host", dispatch_manifest["hostKind"])
            self.assertEqual(collection_path, dispatch_manifest["collectionPath"])
            self.assertNotIn("workloadEntry", dispatch_manifest)
            self.assertEqual("RunNativeAot", dispatch_manifest["nativeEntryFunctionName"])
            self.assertEqual(11, dispatch_manifest["entrySelection"]["entryIndex"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)


