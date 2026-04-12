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

    def _write_subject_fixture(self, repo_root: Path, subject_id: str) -> None:
        manifest_path = repo_root / "subjects" / subject_id / "subject.manifest.json"
        manifest_path.parent.mkdir(parents=True, exist_ok=True)
        manifest_path.write_text(
            json.dumps({"subjectId": subject_id, "displayName": subject_id}),
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
                            "executionContext": {"hostPlatform": "windows-x64", "runtimeProfile": "managed-perf-release"},
                        },
                        {
                            "matrixId": "windows-native-perf",
                            "pipelineId": "native-benchmark",
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


if __name__ == "__main__":
    unittest.main()
