from __future__ import annotations

import importlib.util
import json
import shutil
import subprocess
import sys
import unittest
import uuid
from pathlib import Path
from unittest.mock import patch


REPO_ROOT = Path(__file__).resolve().parents[3]
SUBJECT_WORKERS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subject_workers.py"
MOBILE_PERF_COLLECTOR_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "mobile_perf_collector.py"
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "subject-workers-perf"


def load_module(path: Path, module_name: str):
    if not path.is_file():
        raise FileNotFoundError(f"module missing: {path}")

    spec = importlib.util.spec_from_file_location(module_name, path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load module: {path}")

    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def posix_path(*parts: str) -> str:
    return Path(*parts).as_posix()


def subject_run_path(subject_id: str, run_id: str, *parts: str) -> str:
    return posix_path("artifacts", "subjects", subject_id, "runs", run_id, *parts)


class SubjectWorkersPerfTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)

    def test_perf_harness_command_can_use_declared_benchmark_metadata_instead_of_workload_entry(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_perf_harness_declared_metadata")

        command = workers_module._perf_harness_command(
            harness_dll_path=Path("artifacts/harness/Benchmark.WorkloadEntry.PerfHarness.dll"),
            iterations=7,
            assembly_path=Path("artifacts/subjects/SolutionCorePack/CoreRuntimeBenchmarks.dll"),
            workload_entry="CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()",
            mode="managed",
            declared_benchmark={
                "stableId": "SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry::RunWorkload()",
                "entryIndex": 11,
                "assemblyName": "CoreRuntimeBenchmarks",
                "declaringType": "CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry",
                "methodName": "RunWorkload",
                "methodSignature": "RunWorkload()",
            },
        )

        self.assertEqual(
            [
                "dotnet",
                str(Path("artifacts/harness/Benchmark.WorkloadEntry.PerfHarness.dll")),
                "7",
                "--assembly",
                str(Path("artifacts/subjects/SolutionCorePack/CoreRuntimeBenchmarks.dll")),
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
            command,
        )

    def test_selection_declared_entry_selection_preserves_entry_index_for_declared_benchmark(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_declared_entry_selection_entry_index")

        selection = workers_module._selection_declared_entry_selection(
            {
                "entrySelection": {
                    "family": "declared-benchmark",
                    "stableId": "solution-core::arith",
                    "alias": "arithmetic-bench",
                    "entryIndex": 11,
                }
            }
        )

        self.assertEqual(
            {
                "family": "declared-benchmark",
                "stableId": "solution-core::arith",
                "alias": "arithmetic-bench",
                "entryIndex": 11,
            },
            selection,
        )

    def test_native_perf_command_can_include_entry_index(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_native_perf_command_entry_index")

        command = workers_module._native_perf_command(
            native_executable_path=Path("artifacts/subjects/SolutionCorePack/native/chaos_subject_native_aot.exe"),
            iterations=7,
            entry_index=11,
        )

        self.assertEqual(
            [
                str(Path("artifacts/subjects/SolutionCorePack/native/chaos_subject_native_aot.exe")),
                "--iterations",
                "7",
                "--entry-index",
                "11",
            ],
            command,
        )

    def _make_repo_root(self, prefix: str) -> Path:
        repo_root = TEST_TMP_ROOT / f"{prefix}-{uuid.uuid4().hex}"
        repo_root.mkdir(parents=True, exist_ok=False)
        return repo_root

    def _make_non_repo_path(self, *parts: str) -> Path:
        return TEST_TMP_ROOT / "_external" / Path(*parts)

    def test_runtime_perf_collect_runs_multiple_samples_and_records_perf_details(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_perf_runtime")
        subject_id = "FixtureManagedPerfSubject"
        run_id = "fixture-run-perf-runtime-001"
        matrix_id = "windows-managed-perf"
        perf_project_path = posix_path(
            "src",
            "validation",
            "perf",
            "Benchmark.WorkloadEntry.PerfHarness",
            "Benchmark.WorkloadEntry.PerfHarness.csproj",
        )
        perf_harness_dll_path = subject_run_path(
            subject_id,
            run_id,
            "matrices",
            matrix_id,
            "runtime",
            "harness",
            "Benchmark.WorkloadEntry.PerfHarness.dll",
        )
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "PROFILE",
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

        repo_root = self._make_repo_root("runtime-perf-collect")
        try:
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
                                "driver": "csharp-perf-harness",
                                "defaultVariant": "PROFILE",
                            }
                        },
                        "validationProfiles": {"perf-release": ["perf"]},
                        "defaultValidationProfile": "perf-release",
                    }
                ),
                encoding="utf-8",
            )
            host_input_manifest_path = repo_root / request["upstream"]["host-input"]["manifestPath"]
            host_input_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            host_input_manifest_path.write_text(
                json.dumps(
                    {
                        "primaryAssemblyPath": subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.dll"),
                    }
                ),
                encoding="utf-8",
            )

            completed = subprocess.CompletedProcess(
                ["dotnet", str(repo_root / perf_harness_dll_path), "1000"],
                0,
                json.dumps(
                    {
                        "harness": "csharp-perf-harness",
                        "subjectId": subject_id,
                        "iterations": 1000,
                        "elapsedMilliseconds": 16.0,
                        "lastValue": "6:roadmap0",
                    }
                )
                + "\n",
                "",
            )
            perf_result = {
                "baselinePath": posix_path("subjects", subject_id, "baselines", "perf", matrix_id, "windows.json"),
                "baseline": {"meanDurationMs": 12.0},
                "metrics": {"sampleCount": 3, "meanDurationMs": 16.0, "minDurationMs": 10.0, "maxDurationMs": 20.0},
                "baselineUpdated": False,
                "regressionStatus": "regressed",
                "regressions": [{"metric": "meanDurationMs", "baseline": 12.0, "actual": 16.0, "delta": 4.0}],
            }

            perf_counter_values = [
                0.0, 0.010,
                1.0, 1.020,
                2.0, 2.012,
            ]

            with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                with patch.object(workers_module.time, "perf_counter", side_effect=perf_counter_values):
                    with patch.object(workers_module.perf_module, "evaluate_perf_subject", return_value=perf_result) as evaluate_mock:
                        result = workers_module.run_runtime_perf_collect(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            self.assertEqual(4, run_process_mock.call_count)
            self.assertEqual(
                [
                    "dotnet",
                    "build",
                    str(repo_root / perf_project_path),
                    "-c",
                    "Release",
                    "-m:1",
                    "-o",
                    str(
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
                    ),
                ],
                run_process_mock.call_args_list[0].args[0][:8],
            )
            self.assertEqual(
                [
                    "dotnet",
                    str(repo_root / perf_harness_dll_path),
                    "1000",
                    "--assembly",
                    str(
                        repo_root
                        / subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.dll")
                    ),
                    "--mode",
                    "managed",
                ],
                run_process_mock.call_args_list[1].args[0],
            )
            evaluate_mock.assert_called_once_with(
                repo_root=repo_root,
                subject_id=subject_id,
                matrix_id=matrix_id,
                host_platform="windows",
                metrics={
                    "sampleCount": 3,
                    "meanDurationMs": 16.0,
                    "minDurationMs": 16.0,
                    "maxDurationMs": 16.0,
                    "meanElapsedMilliseconds": 16.0,
                    "minElapsedMilliseconds": 16.0,
                    "maxElapsedMilliseconds": 16.0,
                },
                update_baseline=False,
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(subject_id, manifest["subjectId"])
            self.assertEqual(matrix_id, manifest["matrixId"])
            self.assertEqual("PROFILE", manifest["variant"])
            self.assertEqual(3, len(manifest["samples"]))
            self.assertEqual({"meanDurationMs": 12.0}, manifest["baseline"])
            self.assertEqual("regressed", manifest["regressionStatus"])
            self.assertEqual(perf_project_path, manifest["perfHarnessProjectPath"])
            self.assertEqual(perf_harness_dll_path, manifest["perfHarnessDllPath"])
            self.assertEqual(
                {"sampleCount": 3, "meanDurationMs": 16.0, "minDurationMs": 10.0, "maxDurationMs": 20.0},
                manifest["summaryMetrics"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "stdout.log"),
                result["diagnostics"]["stdoutPath"],
            )
            self.assertEqual(
                "regressed",
                result["details"]["performance"]["regressionStatus"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_native_runtime_perf_runs_native_binary_samples_and_records_native_perf_artifacts(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_native_perf_runtime")
        subject_id = "FixtureNativePerfSubject"
        run_id = "fixture-run-native-perf-001"
        matrix_id = "windows-native-perf"
        executable_path = subject_run_path(
            subject_id,
            run_id,
            "matrices",
            matrix_id,
            "build",
            "out",
            "chaos_subject_reference_proof.exe",
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

        repo_root = self._make_repo_root("native-runtime-perf")
        try:
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
                            }
                        },
                    }
                ),
                encoding="utf-8",
            )
            build_manifest_path = repo_root / request["upstream"]["build"]["manifestPath"]
            build_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            build_manifest_path.write_text(
                json.dumps(
                    {
                        "subjectId": subject_id,
                        "matrixId": matrix_id,
                        "outputs": [executable_path],
                    }
                ),
                encoding="utf-8",
            )

            completed = subprocess.CompletedProcess(
                [str(repo_root / executable_path)],
                0,
                "native mainline perf sample\n",
                "",
            )
            perf_result = {
                "baselinePath": posix_path("subjects", subject_id, "baselines", "perf", matrix_id, "windows.json"),
                "baseline": {"meanDurationMs": 15.0},
                "metrics": {"sampleCount": 1, "meanDurationMs": 18.0, "minDurationMs": 18.0, "maxDurationMs": 18.0},
                "baselineUpdated": False,
                "regressionStatus": "regressed",
                "regressions": [{"metric": "meanDurationMs", "baseline": 15.0, "actual": 18.0, "delta": 3.0}],
            }
            perf_counter_values = [
                0.0, 0.120,
                1.0, 1.018,
            ]

            with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                with patch.object(workers_module.time, "perf_counter", side_effect=perf_counter_values):
                    with patch.object(workers_module.perf_module, "evaluate_perf_subject", return_value=perf_result) as evaluate_mock:
                        result = workers_module.run_native_runtime_perf(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            self.assertEqual(2, run_process_mock.call_count)
            self.assertEqual([str(repo_root / executable_path)], run_process_mock.call_args_list[0].args[0])
            evaluate_mock.assert_called_once_with(
                repo_root=repo_root,
                subject_id=subject_id,
                matrix_id=matrix_id,
                host_platform="windows",
                metrics={"sampleCount": 1, "meanDurationMs": 18.0, "minDurationMs": 18.0, "maxDurationMs": 18.0},
                update_baseline=False,
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(subject_id, manifest["subjectId"])
            self.assertEqual(matrix_id, manifest["matrixId"])
            self.assertEqual("PROFILE", manifest["variant"])
            self.assertEqual(executable_path, manifest["nativeExecutablePath"])
            self.assertEqual(
                subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "perf.runtime.json"),
                manifest["perfRuntimePath"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "perf.samples.json"),
                manifest["perfSamplesPath"],
            )
            self.assertEqual(1, manifest["warmupSampleCount"])
            self.assertEqual(2, len(manifest["samples"]))
            self.assertEqual(False, manifest["samples"][0]["countedInSummary"])
            self.assertEqual(True, manifest["samples"][1]["countedInSummary"])
            self.assertEqual("regressed", manifest["regressionStatus"])
            self.assertEqual(
                [
                    subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "perf.runtime.json"),
                    subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "perf.samples.json"),
                ],
                result["primaryEvidencePaths"],
            )
            self.assertEqual(
                {
                    "runtimePath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "perf.runtime.json"),
                    "samplesPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "perf.samples.json"),
                },
                result["details"]["performance"]["runtimeEvidence"],
            )
            self.assertEqual(1, result["details"]["performance"]["warmupSampleCount"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_native_runtime_perf_passes_subject_harness_iterations_override_to_host(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_native_perf_iterations_override")
        subject_id = "FixtureNativePerfIterationsSubject"
        run_id = "fixture-run-native-perf-iterations-override-001"
        matrix_id = "windows-native-perf"
        executable_path = subject_run_path(
            subject_id,
            run_id,
            "matrices",
            matrix_id,
            "build",
            "out",
            "chaos_subject_reference_proof.exe",
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

        repo_root = self._make_repo_root("native-runtime-perf-iterations-override")
        try:
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
                                "harnessIterations": 7,
                            }
                        },
                    }
                ),
                encoding="utf-8",
            )
            build_manifest_path = repo_root / request["upstream"]["build"]["manifestPath"]
            build_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            build_manifest_path.write_text(
                json.dumps(
                    {
                        "subjectId": subject_id,
                        "matrixId": matrix_id,
                        "outputs": [executable_path],
                    }
                ),
                encoding="utf-8",
            )

            completed = subprocess.CompletedProcess(
                [str(repo_root / executable_path), "--iterations", "7"],
                0,
                json.dumps({"elapsedMilliseconds": 42.0, "opsPerSecond": 166.667, "checksum": 7010, "iterations": 7}) + "\n",
                "",
            )
            perf_result = {
                "baselinePath": posix_path("subjects", subject_id, "baselines", "perf", matrix_id, "windows.json"),
                "baseline": {"meanDurationMs": 40.0},
                "metrics": {"sampleCount": 1, "meanDurationMs": 42.0, "minDurationMs": 42.0, "maxDurationMs": 42.0},
                "baselineUpdated": False,
                "regressionStatus": "regressed",
                "regressions": [{"metric": "meanDurationMs", "baseline": 40.0, "actual": 42.0, "delta": 2.0}],
            }

            with patch.object(workers_module, "_perf_sample_count", return_value=1):
                with patch.object(workers_module, "_native_perf_warmup_count", return_value=0):
                    with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                        with patch.object(workers_module.time, "perf_counter", side_effect=[0.0, 0.050]):
                            with patch.object(workers_module.perf_module, "evaluate_perf_subject", return_value=perf_result):
                                result = workers_module.run_native_runtime_perf(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            run_process_mock.assert_called_once_with(
                [str(repo_root / executable_path), "--iterations", "7"],
                cwd=repo_root,
            )
            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(7, manifest["harnessIterations"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_native_runtime_perf_passes_declared_benchmark_entry_index_to_native_host(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_native_perf_entry_index")
        subject_id = "FixtureNativePerfEntryIndexSubject"
        run_id = "fixture-run-native-perf-entry-index-001"
        matrix_id = "windows-native-perf"
        executable_path = subject_run_path(
            subject_id,
            run_id,
            "matrices",
            matrix_id,
            "build",
            "out",
            "chaos_subject_native_aot.exe",
        )
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "PROFILE",
                "workloadEntry": f"{subject_id}/Benchmarks::RunDefault()",
                "entrySelection": {
                    "family": "declared-benchmark",
                    "stableId": "fixture-native-entry-index",
                    "alias": "native-entry-index",
                    "entryIndex": 11,
                },
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

        repo_root = self._make_repo_root("native-runtime-perf-entry-index")
        try:
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
                            }
                        },
                    }
                ),
                encoding="utf-8",
            )
            build_manifest_path = repo_root / request["upstream"]["build"]["manifestPath"]
            build_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            build_manifest_path.write_text(
                json.dumps(
                    {
                        "subjectId": subject_id,
                        "matrixId": matrix_id,
                        "outputs": [executable_path],
                        "hostKind": "benchmark-host",
                        "collectionPath": posix_path("solutions", "subjects", subject_id, "managed-tests", "Generated", "declared-tests.collection.json"),
                        "dispatchManifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "build", "benchmark.dispatch.manifest.json"),
                    }
                ),
                encoding="utf-8",
            )

            completed = subprocess.CompletedProcess(
                [str(repo_root / executable_path), "--entry-index", "11"],
                0,
                json.dumps({"elapsedMilliseconds": 2.5, "opsPerSecond": 400.0, "checksum": 7, "iterations": 1}) + "\n",
                "",
            )
            perf_result = {
                "baselinePath": posix_path("subjects", subject_id, "baselines", "perf", matrix_id, "windows.json"),
                "baseline": {"meanDurationMs": 2.0},
                "metrics": {"sampleCount": 1, "meanDurationMs": 2.5, "minDurationMs": 2.5, "maxDurationMs": 2.5},
                "baselineUpdated": False,
                "regressionStatus": "regressed",
                "regressions": [{"metric": "meanDurationMs", "baseline": 2.0, "actual": 2.5, "delta": 0.5}],
            }

            with patch.object(workers_module, "_perf_sample_count", return_value=1):
                with patch.object(workers_module, "_native_perf_warmup_count", return_value=0):
                    with patch.object(
                        workers_module,
                        "_resolve_declared_benchmark",
                        return_value={
                            "stableId": "fixture-native-entry-index",
                            "alias": "native-entry-index",
                            "workloadEntry": f"{subject_id}/Benchmarks::RunDefault()",
                            "entryIndex": 11,
                        },
                    ):
                        with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                            with patch.object(workers_module.time, "perf_counter", side_effect=[0.0, 0.050]):
                                with patch.object(workers_module.perf_module, "evaluate_perf_subject", return_value=perf_result):
                                    result = workers_module.run_native_runtime_perf(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            run_process_mock.assert_called_once_with(
                [str(repo_root / executable_path), "--entry-index", "11"],
                cwd=repo_root,
            )
            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("benchmark-host", manifest["hostKind"])
            self.assertEqual(11, manifest["entryIndex"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_native_runtime_perf_collects_custom_numeric_metrics_from_payload(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_native_perf_custom_metrics")
        subject_id = "FixtureDispatchPerfSubject"
        run_id = "fixture-run-interface-dispatch-native-perf-001"
        matrix_id = "windows-native-perf"
        executable_path = subject_run_path(
            subject_id,
            run_id,
            "matrices",
            matrix_id,
            "build",
            "out",
            "chaos_subject_reference_proof.exe",
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

        repo_root = self._make_repo_root("native-runtime-perf-custom-metrics")
        try:
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
                            }
                        },
                    }
                ),
                encoding="utf-8",
            )
            build_manifest_path = repo_root / request["upstream"]["build"]["manifestPath"]
            build_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            build_manifest_path.write_text(
                json.dumps(
                    {
                        "subjectId": subject_id,
                        "matrixId": matrix_id,
                        "outputs": [executable_path],
                    }
                ),
                encoding="utf-8",
            )

            completed = subprocess.CompletedProcess(
                [str(repo_root / executable_path)],
                0,
                json.dumps(
                    {
                        "elapsedMilliseconds": 1.25,
                        "metrics": {
                            "dispatchNanoseconds": 4.5,
                        },
                    }
                )
                + "\n",
                "",
            )
            perf_result = {
                "baselinePath": posix_path("subjects", subject_id, "baselines", "perf", matrix_id, "windows.json"),
                "baseline": {"meanDispatchNanoseconds": 5.0},
                "metrics": {
                    "sampleCount": 1,
                    "meanDurationMs": 1.25,
                    "minDurationMs": 1.25,
                    "maxDurationMs": 1.25,
                    "meanDispatchNanoseconds": 4.5,
                    "minDispatchNanoseconds": 4.5,
                    "maxDispatchNanoseconds": 4.5,
                },
                "baselineUpdated": False,
                "regressionStatus": "ok",
                "regressions": [],
            }
            perf_counter_values = [
                0.0, 0.110,
                1.0, 1.110,
            ]

            with patch.object(workers_module, "run_process", return_value=completed):
                with patch.object(workers_module.time, "perf_counter", side_effect=perf_counter_values):
                    with patch.object(workers_module.perf_module, "evaluate_perf_subject", return_value=perf_result) as evaluate_mock:
                        result = workers_module.run_native_runtime_perf(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            evaluate_mock.assert_called_once_with(
                repo_root=repo_root,
                subject_id=subject_id,
                matrix_id=matrix_id,
                host_platform="windows",
                metrics={
                    "sampleCount": 1,
                    "meanDurationMs": 1.25,
                    "minDurationMs": 1.25,
                    "maxDurationMs": 1.25,
                    "meanDispatchNanoseconds": 4.5,
                    "minDispatchNanoseconds": 4.5,
                    "maxDispatchNanoseconds": 4.5,
                },
                update_baseline=False,
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(4.5, manifest["summaryMetrics"]["meanDispatchNanoseconds"])
            self.assertEqual(4.5, manifest["summaryMetrics"]["maxDispatchNanoseconds"])
            self.assertEqual("ok", manifest["regressionStatus"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_mobile_native_perf_android_uses_target_platform_baseline_and_records_collector_details(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_mobile_android_perf")
        subject_id = "FixtureMobilePerfSubject"
        run_id = "fixture-run-mobile-android-perf-001"
        matrix_id = "android-arm64-native-perf"
        executable_path = subject_run_path(
            subject_id,
            run_id,
            "matrices",
            matrix_id,
            "build",
            "out",
            "mobile_hello_world_android_host_runtime",
        )
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "PROFILE",
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "android-arm64",
                    "runtimeProfile": "android-native-perf-profile",
                    "runtimeArguments": ["--subject-id=FixtureMobilePerfSubject"],
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

        repo_root = self._make_repo_root("mobile-native-perf-android")
        try:
            build_manifest_path = repo_root / request["upstream"]["build"]["manifestPath"]
            build_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            build_manifest_path.write_text(
                json.dumps(
                    {
                        "subjectId": subject_id,
                        "matrixId": matrix_id,
                        "buildStrategy": "android-native-cmake",
                        "outputs": [executable_path],
                    }
                ),
                encoding="utf-8",
            )

            collector_result = {
                "exitCode": 0,
                "stdout": "android mobile perf ok\n",
                "stderr": "",
                "samples": [
                    {"sampleIndex": 1, "durationMs": 11.0},
                    {"sampleIndex": 2, "durationMs": 13.0},
                ],
                "details": {"collector": "adb", "platform": "android-arm64"},
                "evidencePaths": [
                    subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "collector-result.json"),
                ],
            }
            perf_result = {
                "baselinePath": posix_path("subjects", subject_id, "baselines", "perf", matrix_id, "android-arm64.json"),
                "baseline": {"meanDurationMs": 10.0},
                "metrics": {"sampleCount": 2, "meanDurationMs": 12.0, "minDurationMs": 11.0, "maxDurationMs": 13.0},
                "baselineUpdated": False,
                "regressionStatus": "regressed",
                "regressions": [{"metric": "meanDurationMs", "baseline": 10.0, "actual": 12.0, "delta": 2.0}],
            }

            with patch.object(workers_module.mobile_perf_collector, "collect_android_perf", return_value=collector_result) as collect_mock:
                with patch.object(workers_module.perf_module, "evaluate_perf_subject", return_value=perf_result) as evaluate_mock:
                    result = workers_module.run_mobile_native_perf(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            collect_mock.assert_called_once()
            evaluate_mock.assert_called_once_with(
                repo_root=repo_root,
                subject_id=subject_id,
                matrix_id=matrix_id,
                host_platform="android-arm64",
                metrics={"sampleCount": 2, "meanDurationMs": 12.0, "minDurationMs": 11.0, "maxDurationMs": 13.0},
                update_baseline=False,
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("android-arm64", manifest["targetPlatform"])
            self.assertEqual("regressed", manifest["regressionStatus"])
            self.assertEqual(
                [subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "collector-result.json")],
                manifest["collectorEvidencePaths"],
            )
            self.assertEqual("adb", manifest["collectorDetails"]["collector"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_mobile_native_perf_ios_uses_cmake_binary_dir_and_target_platform_baseline(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_mobile_ios_perf")
        subject_id = "FixtureMobilePerfSubject"
        run_id = "fixture-run-mobile-ios-perf-001"
        matrix_id = "ios-arm64-native-perf"
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "PROFILE",
                "executionContext": {
                    "hostPlatform": "macos-arm64",
                    "targetPlatform": "ios-arm64",
                    "runtimeProfile": "ios-native-perf-profile",
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

        repo_root = self._make_repo_root("mobile-native-perf-ios")
        try:
            build_manifest_path = repo_root / request["upstream"]["build"]["manifestPath"]
            build_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            build_manifest_path.write_text(
                json.dumps(
                    {
                        "subjectId": subject_id,
                        "matrixId": matrix_id,
                        "cmakeBinaryDir": self._make_non_repo_path("ios", "cmake", "fixture-build").as_posix(),
                        "outputs": [subject_run_path(subject_id, run_id, "matrices", matrix_id, "build", "validate-only.success.txt")],
                    }
                ),
                encoding="utf-8",
            )

            collector_result = {
                "exitCode": 0,
                "stdout": "ios mobile perf ok\n",
                "stderr": "",
                "samples": [
                    {"sampleIndex": 1, "durationMs": 21.0},
                    {"sampleIndex": 2, "durationMs": 22.0},
                ],
                "details": {"collector": "xcodebuild", "platform": "ios-arm64"},
                "evidencePaths": [
                    subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "xcode-test-result.json"),
                ],
            }
            perf_result = {
                "baselinePath": posix_path("subjects", subject_id, "baselines", "perf", matrix_id, "ios-arm64.json"),
                "baseline": {"meanDurationMs": 20.0},
                "metrics": {"sampleCount": 2, "meanDurationMs": 21.5, "minDurationMs": 21.0, "maxDurationMs": 22.0},
                "baselineUpdated": False,
                "regressionStatus": "regressed",
                "regressions": [{"metric": "meanDurationMs", "baseline": 20.0, "actual": 21.5, "delta": 1.5}],
            }

            with patch.object(workers_module.mobile_perf_collector, "collect_ios_perf", return_value=collector_result) as collect_mock:
                with patch.object(workers_module.perf_module, "evaluate_perf_subject", return_value=perf_result) as evaluate_mock:
                    result = workers_module.run_mobile_native_perf(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            collect_mock.assert_called_once()
            evaluate_mock.assert_called_once_with(
                repo_root=repo_root,
                subject_id=subject_id,
                matrix_id=matrix_id,
                host_platform="ios-arm64",
                metrics={"sampleCount": 2, "meanDurationMs": 21.5, "minDurationMs": 21.0, "maxDurationMs": 22.0},
                update_baseline=False,
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("ios-arm64", manifest["targetPlatform"])
            self.assertEqual("xcodebuild", manifest["collectorDetails"]["collector"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)


if __name__ == "__main__":
    unittest.main()
