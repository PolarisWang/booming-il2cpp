from tests.unit.execution.subject_workers_test_support import *


class TestSubjectWorkersPerfOverrides(SubjectWorkersTestSupport):
    def test_runtime_perf_collect_prefers_subject_harness_iterations_override(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_perf_iteration_override")
        subject_id = "FixtureBenchSubject"
        run_id = "fixture-run-managed-perf-iterations-override-001"
        matrix_id = "windows-managed-perf"
        workload_entry = f"{subject_id}/Program::RunWorkload()"
        perf_project_path = (
            f"subjects/{subject_id}/validation/perf/"
            f"{subject_id}.Subject.PerfHarness/{subject_id}.Subject.PerfHarness.csproj"
        )
        assembly_path = subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.dll")
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "PROFILE",
                "workloadEntry": workload_entry,
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

        repo_root = self._make_repo_root("runtime-perf-iterations-override")
        try:
            host_input_manifest_path = repo_root / request["upstream"]["host-input"]["manifestPath"]
            host_input_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            host_input_manifest_path.write_text(
                json.dumps({"primaryAssemblyPath": assembly_path}),
                encoding="utf-8",
            )
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
                                "harnessIterations": 7,
                            }
                        },
                    }
                ),
                encoding="utf-8",
            )

            perf_result = {
                "metrics": {"meanDurationMs": 12.5, "opsPerSecond": 8000, "checksum": 42},
                "baselinePath": "subjects/FixtureBenchSubject/baselines/perf/windows-managed-perf/windows.json",
                "baseline": {"meanDurationMs": 12.0},
                "baselineUpdated": False,
                "regressionStatus": "no-baseline",
                "regressions": [],
            }
            completed = subprocess.CompletedProcess(
                args=["dotnet"],
                returncode=0,
                stdout=json.dumps({"elapsedMilliseconds": 12.5, "opsPerSecond": 8000, "checksum": 42}) + "\n",
                stderr="",
            )
            harness_dll_path = (
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
                / f"{subject_id}.Subject.PerfHarness.dll"
            )

            with patch.object(workers_module, "_run_checked", return_value=""):
                with patch.object(workers_module, "_perf_sample_count", return_value=1):
                    with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                        with patch.object(workers_module.time, "perf_counter", side_effect=[10.0, 10.5]):
                            with patch.object(workers_module.perf_module, "evaluate_perf_subject", return_value=perf_result):
                                workers_module.run_runtime_perf_collect(repo_root=repo_root, request=request)

            run_process_mock.assert_called_once_with(
                [
                    "dotnet",
                    str(harness_dll_path),
                    "7",
                    "--assembly",
                    str(repo_root / assembly_path),
                    "--workload-entry",
                    workload_entry,
                    "--mode",
                    "managed",
                ],
                cwd=repo_root,
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_interpreter_runtime_perf_prefers_subject_harness_iterations_override(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_interpreter_iteration_override")
        subject_id = "FixtureBenchSubject"
        run_id = "fixture-run-interpreter-perf-iterations-override-001"
        matrix_id = "windows-interpreter-perf"
        workload_entry = f"{subject_id}/Program::RunWorkload()"
        perf_project_path = (
            f"subjects/{subject_id}/validation/perf/"
            f"{subject_id}.Subject.PerfHarness/{subject_id}.Subject.PerfHarness.csproj"
        )
        assembly_path = subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.dll")
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "PROFILE",
                "workloadEntry": workload_entry,
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "runtimeProfile": "interpreter-perf-release",
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

        repo_root = self._make_repo_root("interpreter-perf-iterations-override")
        try:
            host_input_manifest_path = repo_root / request["upstream"]["host-input"]["manifestPath"]
            host_input_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            host_input_manifest_path.write_text(
                json.dumps({"primaryAssemblyPath": assembly_path}),
                encoding="utf-8",
            )
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
                                "driver": "interpreter-runtime-perf",
                                "defaultVariant": "PROFILE",
                                "harnessIterations": 9,
                            }
                        },
                    }
                ),
                encoding="utf-8",
            )

            perf_result = {
                "metrics": {"meanDurationMs": 18.25, "opsPerSecond": 5600, "checksum": 84},
                "baselinePath": "subjects/FixtureBenchSubject/baselines/perf/windows-interpreter-perf/windows.json",
                "baseline": {"meanDurationMs": 18.0},
                "baselineUpdated": False,
                "regressionStatus": "no-baseline",
                "regressions": [],
            }
            completed = subprocess.CompletedProcess(
                args=["dotnet"],
                returncode=0,
                stdout=json.dumps({"elapsedMilliseconds": 18.25, "opsPerSecond": 5600, "checksum": 84}) + "\n",
                stderr="",
            )
            harness_dll_path = (
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
                / f"{subject_id}.Subject.PerfHarness.dll"
            )

            with patch.object(workers_module, "_run_checked", return_value=""):
                with patch.object(workers_module, "_perf_sample_count", return_value=1):
                    with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                        with patch.object(workers_module.time, "perf_counter", side_effect=[20.0, 20.75]):
                            with patch.object(workers_module.perf_module, "evaluate_perf_subject", return_value=perf_result):
                                workers_module.run_interpreter_runtime_perf(repo_root=repo_root, request=request)

            run_process_mock.assert_called_once_with(
                [
                    "dotnet",
                    str(harness_dll_path),
                    "9",
                    "--assembly",
                    str(repo_root / assembly_path),
                    "--workload-entry",
                    workload_entry,
                    "--mode",
                    "interpreter",
                ],
                cwd=repo_root,
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
