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

from tests.support import select_subject_record


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


def posix_path(*parts: str) -> str:
    return Path(*parts).as_posix()


def subject_run_path(subject_id: str, run_id: str, *parts: str) -> str:
    return posix_path("artifacts", "subjects", subject_id, "runs", run_id, *parts)


class SubjectWorkersPerfTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)

    def _make_repo_root(self, prefix: str) -> Path:
        repo_root = TEST_TMP_ROOT / f"{prefix}-{uuid.uuid4().hex}"
        repo_root.mkdir(parents=True, exist_ok=False)
        return repo_root

    def test_runtime_perf_collect_runs_multiple_samples_and_records_perf_details(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_perf_runtime")
        subject_record = select_subject_record(
            "chaos_subject_workers_perf_runtime_record",
            category="benchmark",
            source_type="dotnet-project",
            required_goal_ids=["perf.release"],
            required_validation_kinds=["perf"],
            required_validation_drivers=["csharp-perf-harness"],
        )
        subject_id = str(subject_record["subjectId"])
        run_id = "fixture-run-perf-runtime-001"
        matrix_id = "windows-perf-release"
        perf_project_path = posix_path(
            "subjects",
            subject_id,
            "validation",
            "perf",
            f"{subject_id}.Subject.PerfHarness",
            f"{subject_id}.Subject.PerfHarness.csproj",
        )
        perf_harness_dll_path = subject_run_path(
            subject_id,
            run_id,
            "matrices",
            matrix_id,
            "runtime",
            "harness",
            f"{subject_id}.Subject.PerfHarness.dll",
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
                ["dotnet", str(repo_root / perf_harness_dll_path), "10000"],
                0,
                json.dumps(
                    {
                        "harness": "csharp-perf-harness",
                        "subjectId": subject_id,
                        "iterations": 10000,
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
            self.assertEqual(11, run_process_mock.call_count)
            self.assertEqual(
                [
                    "dotnet",
                    "build",
                    str(repo_root / perf_project_path),
                    "-c",
                    "Release",
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
                run_process_mock.call_args_list[0].args[0][:7],
            )
            self.assertEqual(
                [
                    "dotnet",
                    str(repo_root / perf_harness_dll_path),
                    "10000",
                ],
                run_process_mock.call_args_list[1].args[0],
            )
            evaluate_mock.assert_called_once_with(
                repo_root=repo_root,
                subject_id=subject_id,
                matrix_id=matrix_id,
                host_platform="windows",
                metrics={"sampleCount": 10, "meanDurationMs": 16.0, "minDurationMs": 16.0, "maxDurationMs": 16.0},
                update_baseline=False,
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(subject_id, manifest["subjectId"])
            self.assertEqual(matrix_id, manifest["matrixId"])
            self.assertEqual("PROFILE", manifest["variant"])
            self.assertEqual(10, len(manifest["samples"]))
            self.assertEqual({"meanDurationMs": 12.0}, manifest["baseline"])
            self.assertEqual("regressed", manifest["regressionStatus"])
            self.assertEqual(perf_project_path, manifest["perfHarnessProjectPath"])
            self.assertEqual(perf_harness_dll_path, manifest["perfHarnessDllPath"])
            self.assertEqual(
                {"sampleCount": 10, "meanDurationMs": 16.0, "minDurationMs": 10.0, "maxDurationMs": 20.0},
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
        subject_id = "MainlineFeaturePack"
        run_id = "fixture-run-native-perf-001"
        matrix_id = "windows-native-profile"
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
                "metrics": {"sampleCount": 5, "meanDurationMs": 18.0, "minDurationMs": 17.0, "maxDurationMs": 19.0},
                "baselineUpdated": False,
                "regressionStatus": "regressed",
                "regressions": [{"metric": "meanDurationMs", "baseline": 15.0, "actual": 18.0, "delta": 3.0}],
            }
            perf_counter_values = [
                0.0, 0.120,
                1.0, 1.017,
                2.0, 2.018,
                3.0, 3.019,
                4.0, 4.017,
                5.0, 5.019,
            ]

            with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                with patch.object(workers_module.time, "perf_counter", side_effect=perf_counter_values):
                    with patch.object(workers_module.perf_module, "evaluate_perf_subject", return_value=perf_result) as evaluate_mock:
                        result = workers_module.run_native_runtime_perf(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            self.assertEqual(6, run_process_mock.call_count)
            self.assertEqual([str(repo_root / executable_path)], run_process_mock.call_args_list[0].args[0])
            evaluate_mock.assert_called_once_with(
                repo_root=repo_root,
                subject_id=subject_id,
                matrix_id=matrix_id,
                host_platform="windows",
                metrics={"sampleCount": 5, "meanDurationMs": 18.0, "minDurationMs": 17.0, "maxDurationMs": 19.0},
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
            self.assertEqual(6, len(manifest["samples"]))
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


if __name__ == "__main__":
    unittest.main()
