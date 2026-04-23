from tests.unit.execution.subject_workers_test_support import *


class TestSubjectWorkersRuntimeObserveNativeAot(SubjectWorkersTestSupport):
    def test_runtime_observe_executes_native_aot_proof_host_output_with_declared_entry_arguments(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_runtime_observe_native_aot_proof_host")
        subject_id = "FixtureRuntimeObserveNativeAotProofSubject"
        run_id = "fixture-run-runtime-observe-native-aot-proof-001"
        matrix_id = "windows-native-check"
        collection_path = posix_path(
            "solutions",
            "subjects",
            subject_id,
            "managed-tests",
            "Generated",
            "declared-tests.collection.json",
        )
        executable_path = subject_run_path(
            subject_id,
            run_id,
            "matrices",
            matrix_id,
            "build",
            "out",
            f"{WINDOWS_NATIVE_AOT_BUILD_TARGET}.exe",
        )

        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "CHECK",
                "entrySelection": {
                    "family": "declared-unit-test",
                    "stableId": f"{subject_id}::{subject_id}::{subject_id}.Proofs::Run()",
                    "alias": "fixture-proof",
                    "entryIndex": 7,
                },
                "executionContext": {
                    "runtimeArguments": [
                        "--heartbeat-interval-seconds=5",
                    ],
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

        repo_root = self._make_repo_root("runtime-observe-native-aot-proof")
        try:
            build_manifest_path = repo_root / request["upstream"]["build"]["manifestPath"]
            build_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            build_manifest_path.write_text(
                json.dumps(
                    {
                        "buildStrategy": "windows-native-aot-cmake",
                        "buildKind": "native-aot",
                        "hostKind": "proof-host",
                        "collectionPath": collection_path,
                        "outputs": [executable_path],
                    }
                ),
                encoding="utf-8",
            )

            completed = subprocess.CompletedProcess(
                [str(repo_root / executable_path)],
                0,
                "native aot proof ok\n",
                "",
            )

            with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                result = workers_module.run_runtime_observe(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            run_process_mock.assert_called_once_with(
                [
                    str(repo_root / executable_path),
                    "--heartbeat-interval-seconds=5",
                    f"--collection-path={collection_path}",
                    "--entry-index=7",
                ],
                cwd=repo_root / request["paths"]["bucketRoot"],
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("proof-host", manifest["hostKind"])
            self.assertEqual(collection_path, manifest["collectionPath"])
            self.assertEqual(
                subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "stdout.log"),
                manifest["stdoutPath"],
            )
            self.assertEqual(
                "native aot proof ok\n",
                (repo_root / manifest["stdoutPath"]).read_text(encoding="utf-8"),
            )
            self.assertEqual(
                "0\n",
                (repo_root / manifest["exitCodePath"]).read_text(encoding="utf-8"),
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_runtime_observe_executes_native_aot_benchmark_host_output_for_subject_entry_selection(self) -> None:
        workers_module = load_module(
            SUBJECT_WORKERS_MODULE_PATH,
            "chaos_subject_workers_runtime_observe_native_aot_benchmark_host",
        )
        subject_id = "FixtureRuntimeObserveNativeAotBenchmarkSubject"
        run_id = "fixture-run-runtime-observe-native-aot-benchmark-001"
        matrix_id = "windows-native-check"
        collection_path = posix_path(
            "solutions",
            "subjects",
            subject_id,
            "managed-tests",
            "Generated",
            "declared-tests.collection.json",
        )
        executable_path = subject_run_path(
            subject_id,
            run_id,
            "matrices",
            matrix_id,
            "build",
            "out",
            f"{WINDOWS_NATIVE_AOT_BUILD_TARGET}.exe",
        )

        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "CHECK",
                "source": {
                    "entry": f"{subject_id}/Program::RunSelected()",
                    "entrySelection": {
                        "entryKind": 1,
                        "entrySlice": 11,
                    },
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

        repo_root = self._make_repo_root("runtime-observe-native-aot-benchmark")
        try:
            build_manifest_path = repo_root / request["upstream"]["build"]["manifestPath"]
            build_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            build_manifest_path.write_text(
                json.dumps(
                    {
                        "buildStrategy": "windows-benchmark-cmake",
                        "buildKind": "native-aot",
                        "hostKind": "benchmark-host",
                        "collectionPath": collection_path,
                        "outputs": [executable_path],
                    }
                ),
                encoding="utf-8",
            )

            completed = subprocess.CompletedProcess(
                [str(repo_root / executable_path)],
                0,
                "{\"elapsedMilliseconds\":0.123,\"opsPerSecond\":8123.0,\"checksum\":42}\n",
                "",
            )

            with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                result = workers_module.run_runtime_observe(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            run_process_mock.assert_called_once_with(
                [str(repo_root / executable_path)],
                cwd=repo_root / request["paths"]["bucketRoot"],
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("benchmark-host", manifest["hostKind"])
            self.assertEqual(collection_path, manifest["collectionPath"])
            self.assertEqual(
                subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "stdout.log"),
                manifest["stdoutPath"],
            )
            self.assertEqual(
                "{\"elapsedMilliseconds\":0.123,\"opsPerSecond\":8123.0,\"checksum\":42}\n",
                (repo_root / manifest["stdoutPath"]).read_text(encoding="utf-8"),
            )
            self.assertEqual(
                "0\n",
                (repo_root / manifest["exitCodePath"]).read_text(encoding="utf-8"),
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
