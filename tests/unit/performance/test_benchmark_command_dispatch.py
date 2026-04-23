from tests.unit.performance.benchmark_command_test_support import *


class TestBenchmarkCommandDispatch(BenchmarkCommandTestSupport):
    def test_dispatch_record_all_discovers_perf_subjects_from_manifest_caps(self) -> None:
        benchmark_module = load_module(BENCHMARK_MODULE_PATH, "chaos_benchmark_command_dispatch_all")
        repo_root = self._make_repo_root("dispatch-all")
        try:
            self._write_subject_fixture(repo_root, "SolutionCorePack", perf_matrix_ids=["windows-native-perf"])
            self._write_subject_fixture(repo_root, "LegacyBenchSubject")
            self._write_subject_fixture(repo_root, "HotUpdateHostPack", perf_matrix_ids=["windows-managed-perf"])
            self._write_subject_fixture(
                repo_root,
                "MixedExecutionFeaturePack",
                perf_matrix_ids=["windows-managed-perf", "windows-native-perf", "windows-interpreter-perf"],
            )

            records_module = FakeRecordsModule()

            class FakeDetectorModule:
                @staticmethod
                def load_or_detect(repo_root: Path) -> dict[str, object]:
                    del repo_root
                    return {"id": "windows-x64-test-device", "name": "Windows Test Device"}

            class FakeDashboardModule:
                def __init__(self) -> None:
                    self.updated_subject_ids: list[str] = []

                def update_docs(self, repo_root: Path, *, subject_id: str | None = None) -> None:
                    del repo_root
                    if subject_id is not None:
                        self.updated_subject_ids.append(subject_id)

            dashboard_module = FakeDashboardModule()
            scheduled_runs: list[tuple[str, str]] = []

            manifests = {
                "SolutionCorePack": {
                    "subjectId": "SolutionCorePack",
                    "environmentMatrices": [
                        {
                            "matrixId": "windows-archetype-simple-lib-managed-output",
                            "pipelineId": "managed-runtime-output",
                            "supportedGoals": ["correctness.dev"],
                            "executionContext": {
                                "hostPlatform": "windows-x64",
                                "runtimeProfile": "managed-output",
                            },
                        },
                        {
                            "matrixId": "windows-native-perf",
                            "pipelineId": "native-benchmark",
                            "supportedGoals": ["perf.release"],
                            "executionContext": {
                                "hostPlatform": "windows-x64",
                                "runtimeProfile": "native-perf-release",
                            },
                        },
                    ],
                },
                "HotUpdateHostPack": {
                    "subjectId": "HotUpdateHostPack",
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
                },
                "MixedExecutionFeaturePack": {
                    "subjectId": "MixedExecutionFeaturePack",
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
                        {
                            "matrixId": "windows-native-perf",
                            "pipelineId": "native-benchmark",
                            "supportedGoals": ["perf.release"],
                            "executionContext": {
                                "hostPlatform": "windows-x64",
                                "runtimeProfile": "native-perf-release",
                            },
                        },
                        {
                            "matrixId": "windows-interpreter-perf",
                            "pipelineId": "interpreter-benchmark",
                            "supportedGoals": ["perf.release"],
                            "executionContext": {
                                "hostPlatform": "windows-x64",
                                "runtimeProfile": "interpreter-perf-release",
                            },
                        },
                    ],
                },
            }

            class FakeSubjectsModule:
                @staticmethod
                def discover_perf_subject_ids(repo_root: Path) -> list[str]:
                    del repo_root
                    return ["HotUpdateHostPack", "MixedExecutionFeaturePack", "SolutionCorePack"]

                @staticmethod
                def load_subject_manifest(repo_root: Path, subject_id: str) -> dict[str, object]:
                    del repo_root
                    return manifests[subject_id]

            def fake_load(name: str, path: Path):
                del path
                if name == "benchmark_records":
                    return records_module
                if name == "device_detector":
                    return FakeDetectorModule()
                if name == "benchmark_dashboard_generator":
                    return dashboard_module
                if name == "subjects":
                    return FakeSubjectsModule()
                raise AssertionError(f"unexpected module load: {name}")

            def fake_run_pipeline_and_record(
                *,
                repo_root: Path,
                subject_id: str,
                mode: str,
                device: dict[str, object],
                records_mod: FakeRecordsModule,
                host_platform: str,
            ) -> dict[str, object]:
                del repo_root, device, records_mod, host_platform
                scheduled_runs.append((subject_id, mode))
                return {
                    "record": {
                        "subject": subject_id,
                        "mode": mode,
                        "metrics": {"meanDurationMs": 1.0},
                    },
                    "regressionFound": False,
                }

            expected_runs = {
                "managed": [
                    ("HotUpdateHostPack", "managed"),
                    ("MixedExecutionFeaturePack", "managed"),
                ],
                "native": [
                    ("MixedExecutionFeaturePack", "native"),
                    ("SolutionCorePack", "native"),
                ],
                "interpreter": [
                    ("MixedExecutionFeaturePack", "interpreter"),
                ],
            }

            with patch.object(benchmark_module, "_load", side_effect=fake_load):
                with patch.object(benchmark_module, "_run_pipeline_and_record", side_effect=fake_run_pipeline_and_record):
                    for mode, expected in expected_runs.items():
                        scheduled_runs.clear()
                        dashboard_module.updated_subject_ids.clear()
                        exit_code = benchmark_module.dispatch(
                            ["--all", "--mode", mode, "--record"],
                            repo_root,
                            "windows",
                        )
                        self.assertEqual(0, exit_code)
                        self.assertEqual(expected, scheduled_runs)
                        self.assertEqual([subject_id for subject_id, _ in expected], dashboard_module.updated_subject_ids)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_dispatch_record_reports_regression_summary_without_treating_it_as_execution_error(self) -> None:
        benchmark_module = load_module(BENCHMARK_MODULE_PATH, "chaos_benchmark_command_regression_summary")
        repo_root = self._make_repo_root("dispatch-regression-summary")
        try:
            self._write_subject_fixture(repo_root, "MixedExecutionFeaturePack", perf_matrix_ids=["windows-managed-perf"])
            records_module = FakeRecordsModule()

            class FakeDetectorModule:
                @staticmethod
                def load_or_detect(repo_root: Path) -> dict[str, object]:
                    del repo_root
                    return {"id": "windows-x64-test-device", "name": "Windows Test Device"}

            class FakeDashboardModule:
                def __init__(self) -> None:
                    self.updated_subject_ids: list[str] = []

                def update_docs(self, repo_root: Path, *, subject_id: str | None = None) -> None:
                    del repo_root
                    if subject_id is not None:
                        self.updated_subject_ids.append(subject_id)

            dashboard_module = FakeDashboardModule()

            class FakeSubjectsModule:
                @staticmethod
                def discover_perf_subject_ids(repo_root: Path) -> list[str]:
                    del repo_root
                    return ["MixedExecutionFeaturePack"]

                @staticmethod
                def load_subject_manifest(repo_root: Path, subject_id: str) -> dict[str, object]:
                    del repo_root
                    self.assertEqual("MixedExecutionFeaturePack", subject_id)
                    return {
                        "subjectId": "MixedExecutionFeaturePack",
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
                    }

            def fake_load(name: str, path: Path):
                del path
                if name == "benchmark_records":
                    return records_module
                if name == "device_detector":
                    return FakeDetectorModule()
                if name == "benchmark_dashboard_generator":
                    return dashboard_module
                if name == "subjects":
                    return FakeSubjectsModule()
                raise AssertionError(f"unexpected module load: {name}")

            def fake_run_pipeline_and_record(
                *,
                repo_root: Path,
                subject_id: str,
                mode: str,
                device: dict[str, object],
                records_mod: FakeRecordsModule,
                host_platform: str,
                benchmark_case: dict[str, object] | None = None,
            ) -> dict[str, object]:
                del repo_root, device, records_mod, host_platform, benchmark_case
                return {
                    "record": {
                        "subject": subject_id,
                        "mode": mode,
                        "metrics": {"meanDurationMs": 1.0},
                    },
                    "regressionFound": True,
                }

            with patch.object(benchmark_module, "_load", side_effect=fake_load):
                with patch.object(benchmark_module, "_discover_declared_benchmark_cases", return_value=[]):
                    with patch.object(benchmark_module, "_run_pipeline_and_record", side_effect=fake_run_pipeline_and_record):
                        with patch("builtins.print") as print_mock:
                            exit_code = benchmark_module.dispatch(
                                ["--subject", "MixedExecutionFeaturePack", "--mode", "managed", "--record"],
                                repo_root,
                                "windows",
                            )

            self.assertEqual(0, exit_code)
            self.assertEqual(["MixedExecutionFeaturePack"], dashboard_module.updated_subject_ids)
            printed = " ".join(
                " ".join(str(argument) for argument in call.args)
                for call in print_mock.call_args_list
            ).lower()
            self.assertIn("regression verdict", printed)
            self.assertIn("records generated", printed)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_select_benchmark_matrix_id_rejects_correctness_only_managed_output_matrix(self) -> None:
        benchmark_module = load_module(BENCHMARK_MODULE_PATH, "chaos_benchmark_command_select_matrix")

        manifest = {
            "subjectId": "SolutionCorePack",
            "environmentMatrices": [
                {
                    "matrixId": "windows-archetype-simple-lib-managed-output",
                    "pipelineId": "managed-runtime-output",
                    "supportedGoals": ["correctness.dev"],
                    "executionContext": {
                        "hostPlatform": "windows-x64",
                        "runtimeProfile": "managed-output",
                    },
                },
                {
                    "matrixId": "windows-native-perf",
                    "pipelineId": "native-benchmark",
                    "supportedGoals": ["perf.release"],
                    "executionContext": {
                        "hostPlatform": "windows-x64",
                        "runtimeProfile": "native-perf-release",
                    },
                },
            ],
        }

        with self.assertRaisesRegex(ValueError, "no managed benchmark matrix configured"):
            benchmark_module._select_benchmark_matrix_id(
                manifest,
                mode="managed",
                host_platform="windows",
            )

    def test_dispatch_record_subject_returns_error_for_unsupported_mode(self) -> None:
        benchmark_module = load_module(BENCHMARK_MODULE_PATH, "chaos_benchmark_command_dispatch_unsupported_mode")
        repo_root = self._make_repo_root("dispatch-unsupported-mode")
        try:
            self._write_subject_fixture(repo_root, "SolutionCorePack", perf_matrix_ids=["windows-native-perf"])
            records_module = FakeRecordsModule()

            class FakeDetectorModule:
                @staticmethod
                def load_or_detect(repo_root: Path) -> dict[str, object]:
                    del repo_root
                    return {"id": "windows-x64-test-device", "name": "Windows Test Device"}

            class FakeDashboardModule:
                def __init__(self) -> None:
                    self.updated_subject_ids: list[str] = []

                def update_docs(self, repo_root: Path, *, subject_id: str | None = None) -> None:
                    del repo_root
                    if subject_id is not None:
                        self.updated_subject_ids.append(subject_id)

            dashboard_module = FakeDashboardModule()

            class FakeSubjectsModule:
                @staticmethod
                def discover_perf_subject_ids(repo_root: Path) -> list[str]:
                    del repo_root
                    return ["SolutionCorePack"]

                @staticmethod
                def load_subject_manifest(repo_root: Path, subject_id: str) -> dict[str, object]:
                    del repo_root
                    self.assertEqual("SolutionCorePack", subject_id)
                    return {
                        "subjectId": "SolutionCorePack",
                        "environmentMatrices": [
                            {
                                "matrixId": "windows-native-perf",
                                "pipelineId": "native-benchmark",
                                "supportedGoals": ["perf.release"],
                                "executionContext": {
                                    "hostPlatform": "windows-x64",
                                    "runtimeProfile": "native-perf-release",
                                },
                            },
                        ],
                    }

            def fake_load(name: str, path: Path):
                del path
                if name == "benchmark_records":
                    return records_module
                if name == "device_detector":
                    return FakeDetectorModule()
                if name == "benchmark_dashboard_generator":
                    return dashboard_module
                if name == "subjects":
                    return FakeSubjectsModule()
                raise AssertionError(f"unexpected module load: {name}")

            with patch.object(benchmark_module, "_load", side_effect=fake_load):
                exit_code = benchmark_module.dispatch(
                    ["--subject", "SolutionCorePack", "--mode", "managed", "--record"],
                    repo_root,
                    "windows",
                )

            self.assertEqual(2, exit_code)
            self.assertEqual([], records_module.records)
            self.assertEqual([], dashboard_module.updated_subject_ids)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
