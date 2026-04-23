from tests.unit.performance.subject_workers_perf_test_support import *


class TestSubjectWorkersPerfRuntimeManaged(SubjectWorkersPerfTestSupport):
    def test_runtime_perf_collect_runs_multiple_samples_and_records_perf_details(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_perf_runtime")
        subject_id = "FixtureManagedPerfSubject"
        run_id = "fixture-run-perf-runtime-001"
        matrix_id = "windows-managed-perf"
        perf_project_path = posix_path(
            "src",
            "tools",
            "Chaos.IL2CPP.Tools.Benchmark.WorkloadEntry.PerfHarness",
            "Chaos.IL2CPP.Tools.Benchmark.WorkloadEntry.PerfHarness.csproj",
        )
        perf_harness_dll_path = subject_run_path(
            subject_id,
            run_id,
            "matrices",
            matrix_id,
            "runtime",
            "harness",
            "Chaos.IL2CPP.Tools.Benchmark.WorkloadEntry.PerfHarness.dll",
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
