from tests.unit.execution.subject_workers_test_support import *


class TestSubjectWorkersPerfManagedResolution(SubjectWorkersTestSupport):
    def test_runtime_perf_collect_uses_matching_additional_solution_assembly_for_workload_entry(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_perf_solution_slice_assembly")
        subject_id = "SolutionCorePack"
        run_id = "fixture-run-managed-perf-solution-slice-001"
        matrix_id = "windows-managed-perf"
        workload_entry = "CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()"
        intermediate_root = TEST_TMP_ROOT / "dotnet-intermediates" / "fixture-managed-perf-solution-slice"
        perf_project_path = "src/tools/Chaos.IL2CPP.Tools.Benchmark.WorkloadEntry.PerfHarness/Chaos.IL2CPP.Tools.Benchmark.WorkloadEntry.PerfHarness.csproj"
        primary_assembly_path = subject_run_path(subject_id, run_id, "analysis", "host-input", "SolutionCorePack.dll")
        slice_assembly_path = subject_run_path(subject_id, run_id, "analysis", "host-input", "CoreRuntimeBenchmarks.dll")
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "PROFILE",
                "workloadEntry": workload_entry,
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "runtimeProfile": "managed-perf-release",
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

        repo_root = self._make_repo_root("runtime-perf-solution-slice-assembly")
        try:
            host_input_manifest_path = repo_root / request["upstream"]["host-input"]["manifestPath"]
            host_input_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            host_input_manifest_path.write_text(
                json.dumps(
                    {
                        "primaryAssemblyPath": primary_assembly_path,
                        "additionalAssemblyPaths": [slice_assembly_path],
                    }
                ),
                encoding="utf-8",
            )
            subject_manifest_path = (
                repo_root / "verification" / "catalog" / "owners" / subject_id / "owner.manifest.json"
            )
            subject_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            subject_manifest_path.write_text(
                json.dumps(
                    {
                        "subjectId": subject_id,
                        "validation": {
                            "perf": {
                                "kind": "perf",
                                "project": perf_project_path,
                                "driver": "native-runtime-perf",
                                "defaultVariant": "PROFILE",
                            }
                        },
                    }
                ),
                encoding="utf-8",
            )

            perf_result = {
                "metrics": {"meanDurationMs": 12.5, "opsPerSecond": 8000, "checksum": 42},
                "baselinePath": "verification/catalog/owners/SolutionCorePack/benchmark-baselines/perf/windows-managed-perf/windows.json",
                "baseline": {},
                "baselineUpdated": False,
                "regressionStatus": "no-baseline",
                "regressions": [],
            }
            completed = subprocess.CompletedProcess(
                args=["dotnet"],
                returncode=0,
                stdout=json.dumps({"elapsedMilliseconds": 12.5, "opsPerSecond": 8000, "checksum": 42}) + "\n",
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

            with patch.object(workers_module.tooling_module, "allocate_dotnet_intermediate_dir", return_value=intermediate_root):
                with patch.object(workers_module, "_run_checked", return_value=""):
                    with patch.object(workers_module, "_perf_sample_count", return_value=1):
                        with patch.object(workers_module, "_perf_harness_iterations", return_value=4):
                            with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                                with patch.object(workers_module.time, "perf_counter", side_effect=[10.0, 10.5]):
                                    with patch.object(workers_module.perf_module, "evaluate_perf_subject", return_value=perf_result):
                                        result = workers_module.run_runtime_perf_collect(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            run_process_mock.assert_called_once_with(
                [
                    "dotnet",
                    str(harness_dll_path),
                    "4",
                    "--assembly",
                    str(repo_root / slice_assembly_path),
                    "--workload-entry",
                    workload_entry,
                    "--mode",
                    "managed",
                ],
                cwd=repo_root,
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(slice_assembly_path, manifest["workloadAssemblyPath"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_runtime_perf_collect_resolves_declared_benchmark_by_entry_index_for_structured_harness_execution(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_perf_declared_benchmark_entry_index")
        subject_id = "SolutionCorePack"
        run_id = "fixture-run-managed-perf-declared-entry-index-001"
        matrix_id = "windows-managed-perf"
        stable_id = "SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry::RunWorkload()"
        alias = "arithmetic-bench"
        legacy_workload_entry = "LegacyBenchmarks/LegacyEntry::RunWorkload()"
        resolved_workload_entry = "CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()"
        intermediate_root = TEST_TMP_ROOT / "dotnet-intermediates" / "fixture-managed-perf-declared-entry-index"
        perf_project_path = "src/tools/Chaos.IL2CPP.Tools.Benchmark.WorkloadEntry.PerfHarness/Chaos.IL2CPP.Tools.Benchmark.WorkloadEntry.PerfHarness.csproj"
        primary_assembly_path = subject_run_path(subject_id, run_id, "analysis", "host-input", "SolutionCorePack.dll")
        slice_assembly_path = subject_run_path(subject_id, run_id, "analysis", "host-input", "CoreRuntimeBenchmarks.dll")
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "PROFILE",
                "workloadEntry": legacy_workload_entry,
                "entrySelection": {
                    "family": "declared-benchmark",
                    "stableId": stable_id,
                    "alias": alias,
                    "entryIndex": 11,
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "runtimeProfile": "managed-perf-release",
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

        repo_root = self._make_repo_root("runtime-perf-declared-entry-index")
        try:
            host_input_manifest_path = repo_root / request["upstream"]["host-input"]["manifestPath"]
            host_input_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            host_input_manifest_path.write_text(
                json.dumps(
                    {
                        "primaryAssemblyPath": primary_assembly_path,
                        "additionalAssemblyPaths": [slice_assembly_path],
                    }
                ),
                encoding="utf-8",
            )
            subject_manifest_path = (
                repo_root / "verification" / "catalog" / "owners" / subject_id / "owner.manifest.json"
            )
            subject_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            subject_manifest_path.write_text(
                json.dumps(
                    {
                        "subjectId": subject_id,
                        "validation": {
                            "perf": {
                                "kind": "perf",
                                "project": perf_project_path,
                                "driver": "csharp-perf-harness",
                                "defaultVariant": "PROFILE",
                            }
                        },
                    }
                ),
                encoding="utf-8",
            )

            perf_result = {
                "metrics": {"meanDurationMs": 12.5, "opsPerSecond": 8000, "checksum": 42},
                "baselinePath": "verification/catalog/owners/SolutionCorePack/benchmark-baselines/perf/windows-managed-perf/windows.json",
                "baseline": {},
                "baselineUpdated": False,
                "regressionStatus": "no-baseline",
                "regressions": [],
            }
            completed = subprocess.CompletedProcess(
                args=["dotnet"],
                returncode=0,
                stdout=json.dumps({"elapsedMilliseconds": 12.5, "opsPerSecond": 8000, "checksum": 42}) + "\n",
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

            with patch.object(workers_module.tooling_module, "allocate_dotnet_intermediate_dir", return_value=intermediate_root):
                with patch.object(workers_module, "_run_checked", return_value=""):
                    with patch.object(workers_module.workspace_declared_collection_module, "load_workspace_declared_collection", return_value=workspace_catalog):
                        with patch.object(workers_module, "_perf_sample_count", return_value=1):
                            with patch.object(workers_module, "_perf_harness_iterations", return_value=4):
                                with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                                    with patch.object(workers_module.time, "perf_counter", side_effect=[10.0, 10.5]):
                                        with patch.object(workers_module.perf_module, "evaluate_perf_subject", return_value=perf_result):
                                            result = workers_module.run_runtime_perf_collect(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            run_process_mock.assert_called_once_with(
                [
                    "dotnet",
                    str(harness_dll_path),
                    "4",
                    "--assembly",
                    str(repo_root / slice_assembly_path),
                    "--assembly-name",
                    "CoreRuntimeBenchmarks",
                    "--declaring-type",
                    "CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry",
                    "--method-name",
                    "RunWorkload",
                    "--method-signature",
                    "RunWorkload()",
                    "--mode",
                    "managed",
                ],
                cwd=repo_root,
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("declared-reflection", manifest["benchmarkResolutionKind"])
            self.assertEqual(resolved_workload_entry, manifest["workloadEntry"])
            self.assertEqual(slice_assembly_path, manifest["workloadAssemblyPath"])
            self.assertEqual(11, manifest["declaredEntrySelection"]["entryIndex"])
            self.assertEqual(11, manifest["declaredBenchmark"]["entryIndex"])
            self.assertEqual("CoreRuntimeBenchmarks", manifest["declaredBenchmark"]["assemblyName"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
