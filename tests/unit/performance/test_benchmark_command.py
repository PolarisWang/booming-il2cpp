from __future__ import annotations

import importlib.util
import json
import shutil
import subprocess
import sys
import time
import unittest
import uuid
from pathlib import Path
from unittest.mock import patch


REPO_ROOT = Path(__file__).resolve().parents[3]
BENCHMARK_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "commands" / "benchmark.py"
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "benchmark-command"


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


class FakeRecordsModule:
    def __init__(self) -> None:
        self.records: list[dict[str, object]] = []

    def append_record(self, repo_root: Path, record: dict[str, object]) -> None:
        self.records.append(record)


class BenchmarkCommandTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)

    def _make_repo_root(self, prefix: str) -> Path:
        repo_root = TEST_TMP_ROOT / f"{prefix}-{uuid.uuid4().hex}"
        repo_root.mkdir(parents=True, exist_ok=False)
        return repo_root

    def _write_subject_fixture(
        self,
        repo_root: Path,
        subject_id: str,
        *,
        perf_matrix_ids: list[str] | None = None,
    ) -> None:
        manifest_path = repo_root / "subjects" / subject_id / "subject.manifest.json"
        manifest_path.parent.mkdir(parents=True, exist_ok=True)
        environment_matrices = []
        for matrix_id in perf_matrix_ids or []:
            environment_matrices.append(
                {
                    "matrixId": matrix_id,
                    "pipelineId": "managed-benchmark",
                    "supportedGoals": ["perf.release"],
                    "executionContext": {
                        "hostPlatform": "windows-x64",
                        "runtimeProfile": "managed-perf-release",
                    },
                }
            )
        manifest_path.write_text(
            json.dumps(
                {
                    "subjectId": subject_id,
                    "displayName": subject_id,
                    "environmentMatrices": environment_matrices,
                }
            ),
            encoding="utf-8",
        )

        csproj_path = repo_root / "subjects" / subject_id / "source" / f"{subject_id}.csproj"
        csproj_path.parent.mkdir(parents=True, exist_ok=True)
        csproj_path.write_text("<Project Sdk=\"Microsoft.NET.Sdk\"></Project>", encoding="utf-8")

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

    def test_run_subject_benchmark_pipeline_with_declared_case_override_uses_custom_workload_entry(self) -> None:
        benchmark_module = load_module(BENCHMARK_MODULE_PATH, "chaos_benchmark_command_declared_case")

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

        class FakePlannerModule:
            def __init__(self) -> None:
                self.calls: list[dict[str, object]] = []

            def build_plan(self, repo_root: Path, subject_id: str, **kwargs):
                self.calls.append(
                    {
                        "repo_root": repo_root,
                        "subject_id": subject_id,
                        **kwargs,
                    }
                )
                return {
                    "selection": {
                        "subjectId": subject_id,
                        "matrixId": kwargs["matrix_id"],
                        "goalId": "perf.release",
                        "artifactPlan": {"evidenceTerminalBucket": "runtime"},
                    },
                    "stagePlan": [],
                }

        class FakeExecutorModule:
            def __init__(self) -> None:
                self.calls: list[dict[str, object]] = []

            def execute_plan(self, repo_root: Path, plan: dict[str, object], *, run_id: str | None = None):
                self.calls.append(
                    {
                        "repo_root": repo_root,
                        "plan": plan,
                        "run_id": run_id,
                    }
                )
                return {
                    "status": "ok",
                    "errors": [],
                    "stageResults": [
                        {
                            "stageId": "runtime-perf-collect",
                            "kind": "runtime-perf-collect",
                            "bucket": "runtime",
                            "details": {
                                "performance": {
                                    "metrics": {
                                        "meanDurationMs": 3.25,
                                        "meanOpsPerSecond": 3200,
                                    },
                                    "regressionStatus": "ok",
                                }
                            },
                        }
                    ],
                }

        planner_module = FakePlannerModule()
        executor_module = FakeExecutorModule()
        benchmark_case = {
            "stableId": "bench-arithmetic",
            "alias": "arithmetic-bench",
                    "workloadEntry": "CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()",
        }

        result = benchmark_module._run_subject_benchmark_pipeline(
            repo_root=REPO_ROOT,
            subject_id="SolutionCorePack",
            mode="managed",
            host_platform="windows",
            subjects_module=FakeSubjectsModule(),
            planner_module=planner_module,
            executor_module=executor_module,
            benchmark_case=benchmark_case,
        )

        self.assertEqual(
            "windows-managed-perf",
            planner_module.calls[0]["matrix_id"],
        )
        self.assertEqual(
            benchmark_case["workloadEntry"],
            planner_module.calls[0]["source_entry"],
        )
        self.assertEqual(
            benchmark_case["workloadEntry"],
            planner_module.calls[0]["workload_entry"],
        )
        self.assertEqual(
            {
                "family": "declared-benchmark",
                "stableId": "bench-arithmetic",
                "alias": "arithmetic-bench",
            },
            planner_module.calls[0]["entry_selection"],
        )
        self.assertEqual(
            {"meanDurationMs": 3.25, "meanOpsPerSecond": 3200},
            result["metrics"],
        )
        self.assertFalse(bool(result["regressionFound"]))

    def test_dispatch_record_skips_declared_case_when_case_modes_exclude_requested_mode(self) -> None:
        benchmark_module = load_module(BENCHMARK_MODULE_PATH, "chaos_benchmark_command_case_mode_filter")
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

            scheduled_runs: list[tuple[str, str, str | None]] = []

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
                del repo_root, device, records_mod, host_platform
                scheduled_runs.append((subject_id, mode, None if benchmark_case is None else str(benchmark_case["alias"])))
                return {
                    "record": {
                        "subject": subject_id,
                        "mode": mode,
                        "metrics": {"meanDurationMs": 1.0},
                    },
                    "regressionFound": False,
                }

            with patch.object(benchmark_module, "_load", side_effect=fake_load):
                with patch.object(benchmark_module, "_discover_declared_benchmark_cases", side_effect=fake_discover_declared_benchmark_cases):
                    with patch.object(benchmark_module, "_run_pipeline_and_record", side_effect=fake_run_pipeline_and_record):
                        exit_code = benchmark_module.dispatch(
                            ["--subject", "MixedExecutionFeaturePack", "--mode", "native", "--record"],
                            repo_root,
                            "windows",
                        )

            self.assertEqual(0, exit_code)
            self.assertEqual(
                [
                    ("MixedExecutionFeaturePack", "native", None),
                    ("MixedExecutionFeaturePack", "native", "mixed-execution-native-bench"),
                ],
                scheduled_runs,
            )
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

            self.assertEqual(1, exit_code)
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


if __name__ == "__main__":
    unittest.main()
