from tests.unit.performance.benchmark_command_test_support import *


class TestBenchmarkCommandPipeline(BenchmarkCommandTestSupport):
    def test_run_pipeline_and_record_native_appends_record_from_subject_pipeline(self) -> None:
        benchmark_module = load_module(BENCHMARK_MODULE_PATH, "chaos_benchmark_command_native_record")
        repo_root = self._make_repo_root("native-record")
        try:
            self._write_subject_fixture(repo_root, "BenchArithmetic")
            records_module = FakeRecordsModule()
            device = {
                "id": "windows-x64-test-device",
                "name": "Windows Test Device",
            }
            native_pipeline_result = {
                "metrics": {
                    "meanDurationMs": 1.25,
                    "opsPerSecond": 800000,
                    "checksum": 42,
                },
                "regressionFound": False,
            }

            with patch.object(
                benchmark_module,
                "_run_native_benchmark_pipeline",
                return_value=native_pipeline_result,
                create=True,
            ) as native_pipeline_mock:
                with patch.object(subprocess, "check_output", return_value="abc123\n"):
                    result = benchmark_module._run_pipeline_and_record(
                        repo_root=repo_root,
                        subject_id="BenchArithmetic",
                        mode="native",
                        device=device,
                        records_mod=records_module,
                        host_platform="windows",
                    )

            native_pipeline_mock.assert_called_once()
            self.assertNotIn("error", result)
            self.assertIn("record", result)
            self.assertFalse(bool(result.get("regressionFound")))
            self.assertEqual(1, len(records_module.records))

            record = records_module.records[0]
            self.assertEqual("BenchArithmetic", record["subject"])
            self.assertEqual("native", record["mode"])
            self.assertEqual("windows", record["platform"])
            self.assertEqual(device, record["device"])
            self.assertEqual(native_pipeline_result["metrics"], record["metrics"])
            self.assertEqual("abc123", record["gitCommit"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_run_native_benchmark_pipeline_uses_native_matrix_and_extracts_runtime_metrics(self) -> None:
        benchmark_module = load_module(BENCHMARK_MODULE_PATH, "chaos_benchmark_command_native_pipeline")

        class FakeSubjectsModule:
            @staticmethod
            def load_subject_manifest(repo_root: Path, subject_id: str) -> dict[str, object]:
                return {
                    "subjectId": subject_id,
                    "environmentMatrices": [
                        {
                            "matrixId": "windows-managed-perf",
                            "pipelineId": "managed-benchmark",
                            "supportedGoals": ["perf.release"],
                            "executionContext": {"hostPlatform": "windows-x64", "runtimeProfile": "managed-perf-release"},
                        },
                        {
                            "matrixId": "windows-native-perf",
                            "pipelineId": "native-benchmark",
                            "supportedGoals": ["perf.release"],
                            "executionContext": {"hostPlatform": "windows-x64", "runtimeProfile": "native-perf-profile"},
                        },
                    ],
                }

        class FakeExecutorModule:
            def __init__(self) -> None:
                self.calls: list[dict[str, object]] = []

            def execute_subject_matrix(self, repo_root: Path, subject_id: str, **kwargs):
                self.calls.append(
                    {
                        "repo_root": repo_root,
                        "subject_id": subject_id,
                        **kwargs,
                    }
                )
                return {
                    "status": "ok",
                    "errors": [],
                    "stageResults": [
                        {
                            "stageId": "native-runtime-perf",
                            "kind": "native-runtime-perf",
                            "bucket": "runtime",
                            "details": {
                                "performance": {
                                    "metrics": {
                                        "meanDurationMs": 1.25,
                                        "opsPerSecond": 800000,
                                    },
                                    "regressionStatus": "regressed",
                                }
                            },
                        }
                    ],
                }

        executor_module = FakeExecutorModule()

        result = benchmark_module._run_native_benchmark_pipeline(
            repo_root=REPO_ROOT,
            subject_id="BenchArithmetic",
            host_platform="windows",
            subjects_module=FakeSubjectsModule(),
            executor_module=executor_module,
        )

        self.assertEqual(
            "windows-native-perf",
            executor_module.calls[0]["matrix_id"],
        )
        self.assertEqual(
            {"meanDurationMs": 1.25, "opsPerSecond": 800000},
            result["metrics"],
        )
        self.assertTrue(bool(result["regressionFound"]))

    def test_run_pipeline_and_record_managed_appends_record_from_subject_pipeline(self) -> None:
        benchmark_module = load_module(BENCHMARK_MODULE_PATH, "chaos_benchmark_command_managed_pipeline_record")
        repo_root = self._make_repo_root("managed-record")
        try:
            self._write_subject_fixture(repo_root, "BenchArithmetic")
            records_module = FakeRecordsModule()
            device = {
                "id": "windows-x64-test-device",
                "name": "Windows Test Device",
            }
            managed_pipeline_result = {
                "metrics": {
                    "meanDurationMs": 12.5,
                    "opsPerSecond": 8000,
                    "checksum": 42,
                },
                "regressionFound": False,
            }

            with patch.object(
                benchmark_module,
                "_run_subject_benchmark_pipeline",
                return_value=managed_pipeline_result,
                create=True,
            ) as pipeline_mock:
                with patch.object(subprocess, "check_output", return_value="abc123\n"):
                    result = benchmark_module._run_pipeline_and_record(
                        repo_root=repo_root,
                        subject_id="BenchArithmetic",
                        mode="managed",
                        device=device,
                        records_mod=records_module,
                        host_platform="windows",
                    )

            pipeline_mock.assert_called_once_with(
                repo_root=repo_root,
                subject_id="BenchArithmetic",
                mode="managed",
                host_platform="windows",
            )
            self.assertNotIn("error", result)
            self.assertIn("record", result)
            self.assertEqual(1, len(records_module.records))

            record = records_module.records[0]
            self.assertEqual("BenchArithmetic", record["subject"])
            self.assertEqual("managed", record["mode"])
            self.assertEqual("windows", record["platform"])
            self.assertEqual(device, record["device"])
            self.assertEqual("abc123", record["gitCommit"])
            self.assertEqual(managed_pipeline_result["metrics"], record["metrics"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_run_pipeline_and_record_interpreter_appends_record_from_subject_pipeline(self) -> None:
        benchmark_module = load_module(BENCHMARK_MODULE_PATH, "chaos_benchmark_command_interpreter_pipeline_record")
        repo_root = self._make_repo_root("interpreter-record")
        try:
            self._write_subject_fixture(repo_root, "BenchArithmetic")
            records_module = FakeRecordsModule()
            device = {
                "id": "windows-x64-test-device",
                "name": "Windows Test Device",
            }
            interpreter_pipeline_result = {
                "metrics": {
                    "meanDurationMs": 18.25,
                    "opsPerSecond": 5600,
                    "checksum": 84,
                },
                "regressionFound": False,
            }

            with patch.object(
                benchmark_module,
                "_run_subject_benchmark_pipeline",
                return_value=interpreter_pipeline_result,
                create=True,
            ) as pipeline_mock:
                with patch.object(subprocess, "check_output", return_value="abc123\n"):
                    result = benchmark_module._run_pipeline_and_record(
                        repo_root=repo_root,
                        subject_id="BenchArithmetic",
                        mode="interpreter",
                        device=device,
                        records_mod=records_module,
                        host_platform="windows",
                    )

            pipeline_mock.assert_called_once_with(
                repo_root=repo_root,
                subject_id="BenchArithmetic",
                mode="interpreter",
                host_platform="windows",
            )
            self.assertNotIn("error", result)
            self.assertIn("record", result)
            self.assertEqual(1, len(records_module.records))

            record = records_module.records[0]
            self.assertEqual("BenchArithmetic", record["subject"])
            self.assertEqual("interpreter", record["mode"])
            self.assertEqual("windows", record["platform"])
            self.assertEqual(device, record["device"])
            self.assertEqual("abc123", record["gitCommit"])
            self.assertEqual(interpreter_pipeline_result["metrics"], record["metrics"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_run_subject_benchmark_pipeline_returns_error_when_stage_reports_metrics_but_execution_failed(self) -> None:
        benchmark_module = load_module(BENCHMARK_MODULE_PATH, "chaos_benchmark_command_stage_failure_with_metrics")

        class FakeSubjectsModule:
            @staticmethod
            def load_subject_manifest(repo_root: Path, subject_id: str) -> dict[str, object]:
                del repo_root
                return {
                    "subjectId": subject_id,
                    "environmentMatrices": [
                        {
                            "matrixId": "windows-managed-perf",
                            "pipelineId": "managed-benchmark",
                            "supportedGoals": ["perf.release"],
                            "executionContext": {
                                "hostPlatform": "windows-x64",
                                "runtimeProfile": "managed-perf-release",
                            },
                        },
                    ],
                    "executionPipelines": [
                        {
                            "pipelineId": "managed-benchmark",
                            "stages": [
                                {
                                    "stageId": "runtime-perf-collect",
                                    "kind": "runtime-perf-collect",
                                    "scope": "matrix",
                                    "bucket": "runtime",
                                    "dependsOn": [],
                                }
                            ],
                        },
                    ],
                }

        class FakeExecutorModule:
            def execute_subject_matrix(self, repo_root: Path, subject_id: str, **kwargs):
                del repo_root, subject_id, kwargs
                return {
                    "status": "fail",
                    "errors": ["managed perf execution failed: harness.dll"],
                    "stageResults": [
                        {
                            "stageId": "runtime-perf-collect",
                            "kind": "runtime-perf-collect",
                            "bucket": "runtime",
                            "details": {
                                "performance": {
                                    "metrics": {
                                        "meanDurationMs": 12.5,
                                        "meanOpsPerSecond": 8000,
                                    },
                                    "regressionStatus": "ok",
                                }
                            },
                        }
                    ],
                }

        result = benchmark_module._run_subject_benchmark_pipeline(
            repo_root=REPO_ROOT,
            subject_id="SolutionCorePack",
            mode="managed",
            host_platform="windows",
            subjects_module=FakeSubjectsModule(),
            executor_module=FakeExecutorModule(),
        )

        self.assertEqual(
            {"error": "managed perf execution failed: harness.dll"},
            result,
        )
