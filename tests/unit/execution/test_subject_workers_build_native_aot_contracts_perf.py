from tests.unit.execution.subject_workers_test_support import *


class TestSubjectWorkersBuildNativeAotContractsPerf(SubjectWorkersTestSupport):
    def test_windows_build_target_infers_benchmark_host_for_native_perf_without_declared_entry_selection(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_windows_native_aot_cmake_summary_build")
        subject_id = "FixtureNativeAotSummarySubject"
        run_id = "fixture-run-native-aot-summary-build-001"
        matrix_id = "windows-native-perf"
        collection_path = posix_path(
            "verification", "workspaces",
            "subjects",
            subject_id,
            "managed-tests",
            "Generated",
            "declared-tests.collection.json",
        )
        expected_cmake_path = self._make_non_repo_path("cmake", "bin", "cmake.exe")
        expected_cmake_dir = self._make_non_repo_path("cmake-builds", "subject-native-aot-summary-1234")
        instance_spec = f"{self._make_non_repo_path('visual-studio', '18', 'Professional')},version=18.4.11626.88"

        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "PROFILE",
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                    "runtimeProfile": "native-perf-profile",
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

        repo_root = self._make_repo_root("windows-native-aot-summary-build")
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
                                "configureRoot": posix_path(
                                    "verification", "workspaces",
                                    "subjects",
                                    subject_id,
                                    "native",
                                    matrix_id,
                                ),
                                "hostKind": "benchmark-host",
                                "managedTestProjectId": f"managed-test/{subject_id}/benchmark-host",
                            }
                        ],
                    }
                ),
                encoding="utf-8",
            )

            with patch.object(workers_module.tooling_module, "cmake_environment", return_value=(str(expected_cmake_path), {})):
                with patch.object(workers_module.tooling_module, "windows_developer_environment", return_value={}):
                    with patch.object(workers_module.tooling_module, "detect_visual_studio_generator", return_value="Visual Studio 18 2026"):
                        with patch.object(workers_module.tooling_module, "detect_visual_studio_instance_spec", return_value=instance_spec):
                            with patch.object(workers_module.tooling_module, "allocate_cmake_binary_dir", return_value=expected_cmake_dir):
                                with patch.object(workers_module, "_workspace_manifest_is_stale", return_value=False):
                                    with patch.object(workers_module, "_run_checked") as run_checked_mock:
                                        result = workers_module.run_build_target(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            self.assertEqual(
                "benchmark-host",
                json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))["hostKind"],
            )
            dispatch_manifest = json.loads(
                (
                    repo_root
                    / subject_run_path(subject_id, run_id, "matrices", matrix_id, "build", "benchmark.dispatch.manifest.json")
                ).read_text(encoding="utf-8")
            )
            self.assertEqual({}, dispatch_manifest["entrySelection"])
            self.assertEqual(2, run_checked_mock.call_count)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_native_runtime_perf_propagates_benchmark_host_contract_from_build_manifest(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_native_perf_workspace_contract")
        subject_id = "FixtureNativeAotSubject"
        run_id = "fixture-run-native-perf-workspace-contract-001"
        matrix_id = "windows-native-perf"
        collection_path = posix_path(
            "verification", "workspaces",
            "subjects",
            subject_id,
            "managed-tests",
            "Generated",
            "declared-tests.collection.json",
        )
        dispatch_manifest_path = subject_run_path(
            subject_id,
            run_id,
            "matrices",
            matrix_id,
            "build",
            "benchmark.dispatch.manifest.json",
        )
        native_executable_path = subject_run_path(
            subject_id,
            run_id,
            "matrices",
            matrix_id,
            "build",
            "out",
            f"{WINDOWS_NATIVE_AOT_BUILD_TARGET}.exe",
        )
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "PROFILE",
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "runtimeProfile": "native-perf-profile",
                },
            },
            "upstream": {
                "build": {
                    "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "build", "build.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "runtime.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("native-perf-workspace-contract")
        try:
            build_manifest_path = repo_root / request["upstream"]["build"]["manifestPath"]
            build_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            build_manifest_path.write_text(
                json.dumps(
                    {
                        "outputs": [native_executable_path],
                        "hostKind": "benchmark-host",
                        "collectionPath": collection_path,
                        "dispatchManifestPath": dispatch_manifest_path,
                    }
                ),
                encoding="utf-8",
            )
            (repo_root / dispatch_manifest_path).parent.mkdir(parents=True, exist_ok=True)
            (repo_root / dispatch_manifest_path).write_text("{}", encoding="utf-8")

            subject_manifest_path = repo_root / "subjects" / subject_id / "subject.manifest.json"
            subject_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            subject_manifest_path.write_text(
                json.dumps(
                    {
                        "subjectId": subject_id,
                        "validation": {
                            "perf": {
                                "kind": "perf",
                                "driver": "native-runtime-perf",
                                "defaultVariant": "PROFILE",
                                "harnessIterations": 4,
                            }
                        },
                    }
                ),
                encoding="utf-8",
            )

            perf_result = {
                "metrics": {"meanDurationMs": 12.5, "opsPerSecond": 8000, "checksum": 42},
                "baselinePath": "subjects/FixtureNativeAotSubject/baselines/perf/windows-native-perf/windows.json",
                "baseline": {},
                "baselineUpdated": False,
                "regressionStatus": "no-baseline",
                "regressions": [],
            }
            completed = subprocess.CompletedProcess(
                args=[str(repo_root / native_executable_path)],
                returncode=0,
                stdout=json.dumps({"elapsedMilliseconds": 12.5, "opsPerSecond": 8000, "checksum": 42}) + "\n",
                stderr="",
            )

            with patch.object(workers_module, "_perf_sample_count", return_value=1):
                with patch.object(workers_module, "_native_perf_warmup_count", return_value=0):
                    with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                        with patch.object(workers_module.time, "perf_counter", side_effect=[10.0, 10.5]):
                            with patch.object(workers_module.perf_module, "evaluate_perf_subject", return_value=perf_result):
                                result = workers_module.run_native_runtime_perf(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            run_process_mock.assert_called_once_with(
                [
                    str(repo_root / native_executable_path),
                    "--iterations",
                    "4",
                ],
                cwd=repo_root,
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("benchmark-host", manifest["hostKind"])
            self.assertEqual(collection_path, manifest["collectionPath"])
            self.assertEqual(dispatch_manifest_path, manifest["dispatchManifestPath"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)


