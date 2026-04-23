from tests.unit.execution.subject_workers_test_support import *


class TestSubjectWorkersRuntimeOutputTrace(SubjectWorkersTestSupport):
    def test_runtime_trace_compare_uses_host_input_manifest_and_platform_snapshot(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_runtime_trace_compare")
        subject_id = "FixtureTraceSubject"
        run_id = "fixture-run-trace-compare-001"
        matrix_id = "macos-managed-trace"
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "CHECK",
                "executionContext": {
                    "hostPlatform": "macos-arm64",
                    "targetPlatform": "macos-arm64",
                },
            },
            "upstream": {
                "host-input": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "host-input", "host-input.manifest.json"),
                },
                "runtime": {
                    "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "runtime.manifest.json"),
                },
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "runtime.manifest.json"),
                "reportPaths": [
                    subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "trace-compare.report.json"),
                ],
            },
        }

        repo_root = self._make_repo_root("runtime-trace-compare")
        try:
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

            runtime_manifest_path = repo_root / request["paths"]["manifestPath"]
            runtime_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            runtime_manifest_path.write_text(
                json.dumps(
                    {
                        "subjectId": subject_id,
                        "matrixId": matrix_id,
                        "bucket": "runtime",
                        "stdoutPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "stdout.log"),
                        "stderrPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "stderr.log"),
                        "exitCodePath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "exit-code.txt"),
                        "tracePaths": [],
                    }
                ),
                encoding="utf-8",
            )

            trace_path = repo_root / request["paths"]["bucketRoot"] / "trace.runtime.json"
            calls: list[list[str]] = []

            def fake_run_checked(arguments: list[str], *, repo_root: Path, failure_message: str, env=None) -> str:
                del failure_message, env
                calls.append(arguments)
                if len(calls) == 1:
                    self.assertEqual(
                        [
                            "dotnet",
                            str(repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "host-input" / f"{subject_id}.dll"),
                            "--trace-platform",
                            "macos",
                            "--trace-output",
                            str(trace_path),
                        ],
                        arguments,
                    )
                    trace_path.write_text("{}", encoding="utf-8")
                elif len(calls) == 2:
                    self.assertEqual(
                        [
                            sys.executable,
                            str(repo_root / "tests" / "contracts" / "trace" / "compare-warmup-trace.py"),
                            str(repo_root / "tests" / "contracts" / "trace" / "snapshots" / "macos-warmup-trace.snapshot.json"),
                            str(trace_path),
                        ],
                        arguments,
                    )
                else:
                    self.fail(f"unexpected _run_checked call: {arguments}")
                return ""

            with patch.object(workers_module, "_run_checked", side_effect=fake_run_checked):
                result = workers_module.run_runtime_trace_compare(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            self.assertEqual(
                [subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "trace.runtime.json")],
                result["primaryEvidencePaths"],
            )

            manifest = json.loads(runtime_manifest_path.read_text(encoding="utf-8"))
            self.assertEqual(
                [subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "trace.runtime.json")],
                manifest["tracePaths"],
            )
            self.assertEqual(request["upstream"]["host-input"]["manifestPath"], manifest["hostInputManifestPath"])

            report = json.loads((repo_root / request["paths"]["reportPaths"][0]).read_text(encoding="utf-8"))
            self.assertEqual("tests/contracts/trace/schema/warmup-trace.schema.json", report["schemaPath"])
            self.assertEqual("tests/contracts/trace/snapshots/macos-warmup-trace.snapshot.json", report["expectedSnapshotPath"])
            self.assertEqual(
                subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "trace.runtime.json"),
                report["actualTracePath"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
