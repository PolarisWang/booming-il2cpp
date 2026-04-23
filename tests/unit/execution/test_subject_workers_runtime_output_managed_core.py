from tests.unit.execution.subject_workers_test_support import *


class TestSubjectWorkersRuntimeOutputManagedCore(SubjectWorkersTestSupport):
    def test_runtime_managed_output_runs_primary_assembly_and_records_runtime_manifest(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_managed_runtime_output")
        subject_id = "FixtureManagedRuntimeSubject"
        run_id = "fixture-run-managed-output-001"
        matrix_id = "windows-managed-output"
        assembly_path = subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.dll")
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "executionContext": {
                    "hostPlatform": "windows-x64",
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

        repo_root = self._make_repo_root("managed-runtime-output")
        try:
            host_input_manifest_path = repo_root / request["upstream"]["host-input"]["manifestPath"]
            host_input_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            host_input_manifest_path.write_text(
                json.dumps(
                    {
                        "primaryAssemblyPath": assembly_path,
                    }
                ),
                encoding="utf-8",
            )

            completed = subprocess.CompletedProcess(
                ["dotnet", str(repo_root / assembly_path)],
                0,
                "fixture entry reached.\nargs=0\nregister:Main\n",
                "",
            )

            with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                result = workers_module.run_managed_runtime_output(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            run_process_mock.assert_called_once_with(
                [
                    "dotnet",
                    str(repo_root / assembly_path),
                ],
                cwd=repo_root,
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(subject_id, manifest["subjectId"])
            self.assertEqual(matrix_id, manifest["matrixId"])
            self.assertEqual(
                subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "stdout.log"),
                manifest["stdoutPath"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "stderr.log"),
                manifest["stderrPath"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "exit-code.txt"),
                manifest["exitCodePath"],
            )
            self.assertEqual(
                ["fixture entry reached.", "args=0", "register:Main"],
                manifest["outputLines"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "stdout.log"),
                result["diagnostics"]["stdoutPath"],
            )
            self.assertEqual(
                [subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "stdout.log")],
                result["primaryEvidencePaths"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_runtime_managed_output_uses_collection_arguments_for_managed_proof_host(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_managed_runtime_output_proof_host")
        subject_id = "FixtureManagedRuntimeSubject"
        run_id = "fixture-run-managed-output-proof-host-001"
        matrix_id = "windows-managed-output"
        assembly_path = subject_run_path(subject_id, run_id, "analysis", "host-input", "Chaos.TestFramework.Runtime.dll")
        collection_path = posix_path(
            "solutions",
            "subjects",
            subject_id,
            "managed-tests",
            "Generated",
            "declared-tests.collection.json",
        )
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "entrySelection": {
                    "family": "declared-unit-test",
                    "stableId": f"{subject_id}::{subject_id}::{subject_id}.Proofs::Run()",
                    "alias": "managed-proof",
                    "entryIndex": 7,
                },
                "source": {
                    "entrySelection": {
                        "entryKind": 1,
                        "entrySlice": 3,
                    },
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "runtimeArguments": [
                        "--heartbeat-interval-seconds=5",
                    ],
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

        repo_root = self._make_repo_root("managed-runtime-output-proof-host")
        try:
            host_input_manifest_path = repo_root / request["upstream"]["host-input"]["manifestPath"]
            host_input_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            host_input_manifest_path.write_text(
                json.dumps(
                    {
                        "primaryAssemblyPath": assembly_path,
                        "hostKind": "proof-host",
                        "collectionPath": collection_path,
                        "hostExecutionModel": "shared-runtime-host",
                    }
                ),
                encoding="utf-8",
            )

            completed = subprocess.CompletedProcess(
                [
                    "dotnet",
                    str(repo_root / assembly_path),
                    "--heartbeat-interval-seconds=5",
                    "--host-kind=proof",
                    f"--collection-path={collection_path}",
                    "--entry-index=7",
                ],
                0,
                "managed proof host reached.\nargs=3\n",
                "",
            )

            with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                result = workers_module.run_managed_runtime_output(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            run_process_mock.assert_called_once_with(
                [
                    "dotnet",
                    str(repo_root / assembly_path),
                    "--heartbeat-interval-seconds=5",
                    "--host-kind=proof",
                    f"--collection-path={collection_path}",
                    "--entry-index=7",
                ],
                cwd=repo_root,
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(
                [
                    "--heartbeat-interval-seconds=5",
                    "--host-kind=proof",
                    f"--collection-path={collection_path}",
                    "--entry-index=7",
                ],
                manifest["arguments"],
            )
            self.assertEqual(
                {
                    "family": "declared-unit-test",
                    "stableId": f"{subject_id}::{subject_id}::{subject_id}.Proofs::Run()",
                    "alias": "managed-proof",
                    "entryIndex": 7,
                },
                manifest["declaredEntrySelection"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
