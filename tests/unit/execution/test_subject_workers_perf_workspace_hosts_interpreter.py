from tests.unit.execution.subject_workers_test_support import *


class TestSubjectWorkersPerfWorkspaceHostsInterpreter(SubjectWorkersTestSupport):
    def test_interpreter_runtime_perf_uses_collection_arguments_for_workspace_benchmark_host(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_interpreter_perf_workspace_benchmark_host")
        subject_id = "SolutionCorePack"
        run_id = "fixture-run-interpreter-perf-workspace-benchmark-host-001"
        matrix_id = "windows-interpreter-perf"
        stable_id = "SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry::RunWorkload()"
        alias = "arithmetic-bench"
        resolved_workload_entry = "CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()"
        collection_path = posix_path(
            "verification", "workspaces",
            "subjects",
            subject_id,
            "managed-tests",
            "Generated",
            "declared-tests.collection.json",
        )
        perf_project_path = "src/tools/Chaos.IL2CPP.Tools.Benchmark.WorkloadEntry.PerfHarness/Chaos.IL2CPP.Tools.Benchmark.WorkloadEntry.PerfHarness.csproj"
        benchmark_host_assembly_path = subject_run_path(subject_id, run_id, "analysis", "host-input", "Chaos.TestFramework.Runtime.dll")
        slice_assembly_path = subject_run_path(subject_id, run_id, "analysis", "host-input", "CoreRuntimeBenchmarks.dll")
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "PROFILE",
                "workloadEntry": "LegacyBenchmarks/LegacyEntry::RunWorkload()",
                "entrySelection": {
                    "family": "declared-benchmark",
                    "stableId": stable_id,
                    "alias": alias,
                    "entryIndex": 11,
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "runtimeProfile": "interpreter-perf-release",
                },
            },
            "upstream": {
                "host-input": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "host-input", "host-input.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "runtime.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("interpreter-perf-workspace-benchmark-host")
        try:
            host_input_manifest_path = repo_root / request["upstream"]["host-input"]["manifestPath"]
            host_input_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            host_input_manifest_path.write_text(
                json.dumps(
                    {
                        "primaryAssemblyPath": benchmark_host_assembly_path,
                        "additionalAssemblyPaths": [slice_assembly_path],
                        "hostKind": "benchmark-host",
                        "collectionPath": collection_path,
                        "hostExecutionModel": "shared-runtime-host",
                    }
                ),
                encoding="utf-8",
            )
            subject_manifest_path = repo_root / "subjects" / subject_id / "subject.manifest.json"
            subject_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            subject_manifest_path.write_text(
                json.dumps(
                    {
                        "subjectId": subject_id,
                        "validation": {
                            "perf": {
                                "kind": "perf",
                                "project": perf_project_path,
                                "driver": "interpreter-runtime-perf",
                                "defaultVariant": "PROFILE",
                            }
                        },
                    }
                ),
                encoding="utf-8",
            )

            perf_result = {
                "metrics": {"meanDurationMs": 20.0, "opsPerSecond": 5000, "checksum": 42},
                "baselinePath": "subjects/SolutionCorePack/baselines/perf/windows-interpreter-perf/windows.json",
                "baseline": {},
                "baselineUpdated": False,
                "regressionStatus": "no-baseline",
                "regressions": [],
            }
            completed = subprocess.CompletedProcess(
                args=["dotnet"],
                returncode=0,
                stdout=json.dumps({"elapsedMilliseconds": 20.0, "opsPerSecond": 5000, "checksum": 42}) + "\n",
                stderr="",
            )
            harness_dll_path = (
                repo_root
                / "artifacts"
                / "subjects"
                / subject_id
                / "runs"
                / run_id
                / "matrices"
                / matrix_id
                / "runtime"
                / "harness"
                / "Chaos.IL2CPP.Tools.Benchmark.WorkloadEntry.PerfHarness.dll"
            )

            workspace_catalog = {
                "declaredBenchmarks": [
                    {
                        "stableId": stable_id,
                        "entryIndex": 11,
                        "alias": alias,
                        "assemblyName": "CoreRuntimeBenchmarks",
                        "declaringType": "CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry",
                        "methodName": "RunWorkload",
                        "methodSignature": "RunWorkload()",
                    }
                ]
            }

            with patch.object(workers_module, "_run_checked", return_value=""):
                with patch.object(workers_module.workspace_declared_collection_module, "load_workspace_declared_collection", return_value=workspace_catalog):
                    with patch.object(workers_module, "_perf_sample_count", return_value=1):
                        with patch.object(workers_module, "_perf_harness_iterations", return_value=4):
                            with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                                with patch.object(workers_module.time, "perf_counter", side_effect=[20.0, 20.75]):
                                    with patch.object(workers_module.perf_module, "evaluate_perf_subject", return_value=perf_result):
                                        result = workers_module.run_interpreter_runtime_perf(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            run_process_mock.assert_called_once_with(
                [
                    "dotnet",
                    str(harness_dll_path),
                    "4",
                    "--host-assembly",
                    str(repo_root / benchmark_host_assembly_path),
                    "--host-kind",
                    "benchmark",
                    "--collection-path",
                    collection_path,
                    "--entry-index",
                    "11",
                    "--mode",
                    "interpreter",
                ],
                cwd=repo_root,
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("collection-entry", manifest["benchmarkResolutionKind"])
            self.assertEqual(resolved_workload_entry, manifest["workloadEntry"])
            self.assertEqual(slice_assembly_path, manifest["workloadAssemblyPath"])
            self.assertEqual(11, manifest["declaredEntrySelection"]["entryIndex"])
            self.assertEqual(11, manifest["declaredBenchmark"]["entryIndex"])
            self.assertEqual("benchmark-host", manifest["hostKind"])
            self.assertEqual(collection_path, manifest["collectionPath"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_interpreter_runtime_perf_passes_binding_manifest_for_hotupdate_benchmark_host(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_interpreter_perf_hotupdate_benchmark_host")
        subject_id = "HotUpdateHostPack"
        run_id = "fixture-run-interpreter-perf-hotupdate-benchmark-host-001"
        matrix_id = "windows-interpreter-perf"
        stable_id = "HotUpdateHostPack::HotUpdateHostPack.Patch::HotUpdateHostPack.Patch.Benchmarks::RunWorkload()"
        alias = "hotupdate-bench"
        resolved_workload_entry = "HotUpdateHostPack.Patch/Benchmarks::RunWorkload()"
        collection_path = posix_path(
            "verification", "workspaces",
            "subjects",
            subject_id,
            "hotupdate-tests",
            "Generated",
            "declared-tests.collection.json",
        )
        binding_manifest_path = posix_path(
            "verification", "workspaces",
            "subjects",
            subject_id,
            "hotupdate-tests",
            "Generated",
            "declared-tests.binding.json",
        )
        perf_project_path = "src/tools/Chaos.IL2CPP.Tools.Benchmark.WorkloadEntry.PerfHarness/Chaos.IL2CPP.Tools.Benchmark.WorkloadEntry.PerfHarness.csproj"
        benchmark_host_assembly_path = subject_run_path(subject_id, run_id, "analysis", "host-input", "HotUpdateHostPack.HotUpdateBenchmarkHost.dll")
        slice_assembly_path = subject_run_path(subject_id, run_id, "analysis", "host-input", "HotUpdateHostPack.Patch.dll")
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "PROFILE",
                "workloadEntry": "LegacyBenchmarks/LegacyEntry::RunWorkload()",
                "entrySelection": {
                    "family": "declared-benchmark",
                    "stableId": stable_id,
                    "alias": alias,
                    "entryIndex": 8,
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "runtimeProfile": "interpreter-perf-release",
                },
            },
            "upstream": {
                "host-input": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "host-input", "host-input.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "runtime.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("interpreter-perf-hotupdate-benchmark-host")
        try:
            host_input_manifest_path = repo_root / request["upstream"]["host-input"]["manifestPath"]
            host_input_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            host_input_manifest_path.write_text(
                json.dumps(
                    {
                        "primaryAssemblyPath": benchmark_host_assembly_path,
                        "additionalAssemblyPaths": [slice_assembly_path],
                        "hostKind": "benchmark-host",
                        "collectionPath": collection_path,
                        "bindingManifestPath": binding_manifest_path,
                    }
                ),
                encoding="utf-8",
            )
            subject_manifest_path = repo_root / "subjects" / subject_id / "subject.manifest.json"
            subject_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            subject_manifest_path.write_text(
                json.dumps(
                    {
                        "subjectId": subject_id,
                        "validation": {
                            "perf": {
                                "kind": "perf",
                                "project": perf_project_path,
                                "driver": "interpreter-runtime-perf",
                                "defaultVariant": "PROFILE",
                            }
                        },
                    }
                ),
                encoding="utf-8",
            )

            perf_result = {
                "metrics": {"meanDurationMs": 20.0, "opsPerSecond": 5000, "checksum": 42},
                "baselinePath": "subjects/HotUpdateHostPack/baselines/perf/windows-interpreter-perf/windows.json",
                "baseline": {},
                "baselineUpdated": False,
                "regressionStatus": "no-baseline",
                "regressions": [],
            }
            completed = subprocess.CompletedProcess(
                args=["dotnet"],
                returncode=0,
                stdout=json.dumps({"elapsedMilliseconds": 20.0, "opsPerSecond": 5000, "checksum": 42}) + "\n",
                stderr="",
            )
            harness_dll_path = (
                repo_root
                / "artifacts"
                / "subjects"
                / subject_id
                / "runs"
                / run_id
                / "matrices"
                / matrix_id
                / "runtime"
                / "harness"
                / "Chaos.IL2CPP.Tools.Benchmark.WorkloadEntry.PerfHarness.dll"
            )

            workspace_catalog = {
                "declaredBenchmarks": [
                    {
                        "stableId": stable_id,
                        "entryIndex": 8,
                        "alias": alias,
                        "assemblyName": "HotUpdateHostPack.Patch",
                        "declaringType": "HotUpdateHostPack.Patch.Benchmarks",
                        "methodName": "RunWorkload",
                        "methodSignature": "RunWorkload()",
                    }
                ]
            }

            with patch.object(workers_module, "_run_checked", return_value=""):
                with patch.object(workers_module.workspace_declared_collection_module, "load_workspace_declared_collection", return_value=workspace_catalog):
                    with patch.object(workers_module, "_perf_sample_count", return_value=1):
                        with patch.object(workers_module, "_perf_harness_iterations", return_value=4):
                            with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                                with patch.object(workers_module.time, "perf_counter", side_effect=[20.0, 20.75]):
                                    with patch.object(workers_module.perf_module, "evaluate_perf_subject", return_value=perf_result):
                                        result = workers_module.run_interpreter_runtime_perf(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            run_process_mock.assert_called_once_with(
                [
                    "dotnet",
                    str(harness_dll_path),
                    "4",
                    "--host-assembly",
                    str(repo_root / benchmark_host_assembly_path),
                    "--collection-path",
                    collection_path,
                    "--entry-index",
                    "8",
                    "--binding-manifest-path",
                    binding_manifest_path,
                    "--mode",
                    "interpreter",
                ],
                cwd=repo_root,
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(binding_manifest_path, manifest["bindingManifestPath"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

