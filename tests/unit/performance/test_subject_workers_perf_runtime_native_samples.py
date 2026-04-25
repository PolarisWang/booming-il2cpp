from tests.unit.performance.subject_workers_perf_test_support import *


class TestSubjectWorkersPerfRuntimeNativeSamples(SubjectWorkersPerfTestSupport):
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
            write_owner_manifest(
                repo_root,
                subject_id,
                {
                    "subjectId": subject_id,
                    "validation": {
                        "perf": {
                            "kind": "perf",
                            "driver": "native-runtime-perf",
                            "defaultVariant": "PROFILE",
                        }
                    },
                },
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
            write_owner_manifest(
                repo_root,
                subject_id,
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
                },
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
