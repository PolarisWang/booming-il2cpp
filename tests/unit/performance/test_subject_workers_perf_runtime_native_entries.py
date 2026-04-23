from tests.unit.performance.subject_workers_perf_test_support import *


class TestSubjectWorkersPerfRuntimeNativeEntries(SubjectWorkersPerfTestSupport):
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
