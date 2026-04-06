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


class SubjectWorkersPerfTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)

    def _make_repo_root(self, prefix: str) -> Path:
        repo_root = TEST_TMP_ROOT / f"{prefix}-{uuid.uuid4().hex}"
        repo_root.mkdir(parents=True, exist_ok=False)
        return repo_root

    def test_runtime_perf_collect_runs_multiple_samples_and_records_perf_details(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "booming_subject_workers_perf_runtime")
        request = {
            "selection": {
                "subjectId": "GenericEcho",
                "matrixId": "windows-perf-release",
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "runtimeProfile": "managed-perf-release",
                },
            },
            "upstream": {
                "host-input": {
                    "manifestPath": "artifacts/subjects/GenericEcho/shared/host-input/host-input.manifest.json",
                }
            },
            "paths": {
                "bucketRoot": "artifacts/subjects/GenericEcho/matrices/windows-perf-release/runtime",
                "manifestPath": "artifacts/subjects/GenericEcho/matrices/windows-perf-release/runtime/runtime.manifest.json",
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("runtime-perf-collect")
        try:
            host_input_manifest_path = repo_root / request["upstream"]["host-input"]["manifestPath"]
            host_input_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            host_input_manifest_path.write_text(
                json.dumps(
                    {
                        "primaryAssemblyPath": "artifacts/smoke/bin/GenericEcho/Release/net8.0/GenericEcho.dll",
                    }
                ),
                encoding="utf-8",
            )

            completed = subprocess.CompletedProcess(
                ["dotnet", str(repo_root / "artifacts" / "smoke" / "bin" / "GenericEcho" / "Release" / "net8.0" / "GenericEcho.dll")],
                0,
                "roadmap0\n42\nroadmap0:roadmap0\n",
                "",
            )
            perf_result = {
                "baselinePath": "tests/perf/subjects/GenericEcho/windows-perf-release/baselines/windows.json",
                "baseline": {"meanDurationMs": 12.0},
                "metrics": {"sampleCount": 10, "meanDurationMs": 16.0, "minDurationMs": 10.0, "maxDurationMs": 20.0},
                "baselineUpdated": False,
                "regressionStatus": "regressed",
                "regressions": [{"metric": "meanDurationMs", "baseline": 12.0, "actual": 16.0, "delta": 4.0}],
            }

            perf_counter_values = [
                0.0, 0.010,
                1.0, 1.020,
                2.0, 2.012,
                3.0, 3.018,
                4.0, 4.014,
                5.0, 5.016,
                6.0, 6.012,
                7.0, 7.020,
                8.0, 8.018,
                9.0, 9.020,
            ]

            with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                with patch.object(workers_module.time, "perf_counter", side_effect=perf_counter_values):
                    with patch.object(workers_module.perf_module, "evaluate_perf_subject", return_value=perf_result) as evaluate_mock:
                        result = workers_module.run_runtime_perf_collect(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            self.assertEqual(10, run_process_mock.call_count)
            self.assertEqual(
                [
                    "dotnet",
                    str(repo_root / "artifacts" / "smoke" / "bin" / "GenericEcho" / "Release" / "net8.0" / "GenericEcho.dll"),
                ],
                run_process_mock.call_args.args[0],
            )
            evaluate_mock.assert_called_once_with(
                repo_root=repo_root,
                subject_id="GenericEcho",
                matrix_id="windows-perf-release",
                host_platform="windows",
                metrics={"sampleCount": 10, "meanDurationMs": 16.0, "minDurationMs": 10.0, "maxDurationMs": 20.0},
                update_baseline=False,
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("GenericEcho", manifest["subjectId"])
            self.assertEqual("windows-perf-release", manifest["matrixId"])
            self.assertEqual(10, len(manifest["samples"]))
            self.assertEqual({"meanDurationMs": 12.0}, manifest["baseline"])
            self.assertEqual("regressed", manifest["regressionStatus"])
            self.assertEqual(
                {"sampleCount": 10, "meanDurationMs": 16.0, "minDurationMs": 10.0, "maxDurationMs": 20.0},
                manifest["summaryMetrics"],
            )
            self.assertEqual(
                "artifacts/subjects/GenericEcho/matrices/windows-perf-release/runtime/stdout.log",
                result["diagnostics"]["stdoutPath"],
            )
            self.assertEqual(
                "regressed",
                result["details"]["performance"]["regressionStatus"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)


if __name__ == "__main__":
    unittest.main()
