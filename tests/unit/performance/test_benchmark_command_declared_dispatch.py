from tests.unit.performance.benchmark_command_test_support import *


class TestBenchmarkCommandDeclaredDispatch(BenchmarkCommandTestSupport):
    def test_discover_declared_benchmark_cases_prefers_workspace_collection_when_available(self) -> None:
        benchmark_module = load_module(BENCHMARK_MODULE_PATH, "chaos_benchmark_command_workspace_collection")
        repo_root = self._make_repo_root("workspace-collection")

        class FailingCompiledCatalogModule:
            @staticmethod
            def build_subject_declared_test_catalog(*, repo_root: Path, subject_id: str, force_build: bool = False):
                del repo_root, subject_id, force_build
                raise AssertionError("compiled collection fallback should not be used when workspace collection is available")

        try:
            self._write_workspace_benchmark_fixture(
                repo_root,
                subject_id="SolutionCorePack",
                stable_id="solution-core::arith",
                alias="arithmetic-bench",
                entry_index=11,
                assembly_name="CoreRuntimeBenchmarks",
                declaring_type="CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry",
                method_name="RunWorkload",
                method_signature="RunWorkload()",
            )

            cases = benchmark_module._discover_declared_benchmark_cases(
                repo_root,
                "SolutionCorePack",
                compiled_catalog_module=FailingCompiledCatalogModule(),
            )

            self.assertEqual(1, len(cases))
            self.assertEqual("solution-core::arith", cases[0]["stableId"])
            self.assertEqual("arithmetic-bench", cases[0]["alias"])
            self.assertEqual(11, cases[0]["entryIndex"])
            self.assertEqual(
                "CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()",
                cases[0]["workloadEntry"],
            )
            self.assertEqual(["managed", "native"], cases[0]["supportedModes"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_dispatch_record_native_declared_cases_use_shared_native_helper(self) -> None:
        benchmark_module = load_module(BENCHMARK_MODULE_PATH, "chaos_benchmark_command_native_shared_dispatch")
        repo_root = self._make_repo_root("case-mode-filter")
        try:
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
                def update_docs(self, repo_root: Path, *, subject_id: str | None = None) -> None:
                    del repo_root, subject_id

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
                    }

            scheduled_native_cases: list[str] = []

            def fake_load(name: str, path: Path):
                del path
                if name == "benchmark_records":
                    return records_module
                if name == "device_detector":
                    return FakeDetectorModule()
                if name == "benchmark_dashboard_generator":
                    return FakeDashboardModule()
                if name == "subjects":
                    return FakeSubjectsModule()
                raise AssertionError(f"unexpected module load: {name}")

            def fake_discover_declared_benchmark_cases(repo_root: Path, subject_id: str, *, compiled_catalog_module=None):
                del repo_root, compiled_catalog_module
                self.assertEqual("MixedExecutionFeaturePack", subject_id)
                return [
                    {
                        "stableId": "mixed-execution-bench",
                        "alias": "mixed-execution-bench",
                        "workloadEntry": "MixedExecutionFeaturePack/MixedExecutionBenchmarkEntry::RunWorkload()",
                        "supportedModes": ["managed", "interpreter"],
                    },
                    {
                        "stableId": "mixed-execution-native-bench",
                        "alias": "mixed-execution-native-bench",
                        "workloadEntry": "MixedExecutionFeaturePack/MixedExecutionNativeBenchmarkEntry::RunWorkload()",
                        "supportedModes": ["managed", "native", "interpreter"],
                    },
                ]

            def fake_run_native_declared_benchmark_records(
                *,
                repo_root: Path,
                subject_id: str,
                device: dict[str, object],
                records_mod: FakeRecordsModule,
                host_platform: str,
                benchmark_cases: list[dict[str, object]],
            ) -> list[dict[str, object]]:
                del repo_root, device, records_mod, host_platform
                scheduled_native_cases.extend(str(item["alias"]) for item in benchmark_cases)
                return [
                    {
                        "record": {
                            "subject": subject_id,
                            "mode": "native",
                            "metrics": {"meanDurationMs": 1.0},
                        },
                        "regressionFound": False,
                    }
                ]

            with patch.object(benchmark_module, "_load", side_effect=fake_load):
                with patch.object(benchmark_module, "_discover_declared_benchmark_cases", side_effect=fake_discover_declared_benchmark_cases):
                    with patch.object(
                        benchmark_module,
                        "_run_native_declared_benchmark_records",
                        side_effect=fake_run_native_declared_benchmark_records,
                    ):
                        exit_code = benchmark_module.dispatch(
                            ["--subject", "MixedExecutionFeaturePack", "--mode", "native", "--record"],
                            repo_root,
                            "windows",
                        )

            self.assertEqual(0, exit_code)
            self.assertEqual(
                ["mixed-execution-native-bench"],
                scheduled_native_cases,
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
