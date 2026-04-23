from tests.unit.execution.subject_workers_test_support import *


class TestSubjectWorkersRuntimeOutputManagedEntrySelection(SubjectWorkersTestSupport):
    def test_runtime_managed_output_passes_runtime_and_subject_entry_selection_arguments(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_managed_runtime_output_entry_selection")
        subject_id = "FixtureManagedRuntimeSubject"
        run_id = "fixture-run-managed-output-entry-selection-001"
        matrix_id = "windows-managed-output"
        assembly_path = subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.dll")
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
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

        repo_root = self._make_repo_root("managed-runtime-output-entry-selection")
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
                [
                    "dotnet",
                    str(repo_root / assembly_path),
                    "--heartbeat-interval-seconds=5",
                    "--chaos-entry-kind=1",
                    "--chaos-entry-slice=3",
                ],
                0,
                "fixture selected entry reached.\nargs=3\n",
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
                    "--chaos-entry-kind=1",
                    "--chaos-entry-slice=3",
                ],
                cwd=repo_root,
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(
                [
                    "--heartbeat-interval-seconds=5",
                    "--chaos-entry-kind=1",
                    "--chaos-entry-slice=3",
                ],
                manifest["arguments"],
            )
            self.assertEqual(
                {
                    "entryKind": 1,
                    "entrySlice": 3,
                },
                manifest["subjectEntrySelection"],
            )
            self.assertEqual(
                ["fixture selected entry reached.", "args=3"],
                manifest["outputLines"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_runtime_managed_output_does_not_append_declared_unit_source_entry_argument(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_managed_runtime_output_declared_entry")
        subject_id = "FixtureManagedRuntimeSubject"
        run_id = "fixture-run-managed-output-declared-entry-001"
        matrix_id = "windows-managed-output"
        source_entry = "HotUpdateHostPack/MetadataSupplementProofEntry::Run()"
        assembly_path = subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.dll")
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "entrySelection": {
                    "family": "declared-unit-test",
                    "stableId": "HotUpdateHostPack::HotUpdateHostPack::HotUpdateHostPack.MetadataSupplementProofEntry::Run()",
                    "alias": "metadata-supplement-proof",
                },
                "source": {
                    "entry": source_entry,
                    "entrySelection": {
                        "entryKind": 1,
                        "entrySlice": 1,
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

        repo_root = self._make_repo_root("managed-runtime-output-declared-entry")
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
                [
                    "dotnet",
                    str(repo_root / assembly_path),
                    "--heartbeat-interval-seconds=5",
                    "--chaos-entry-kind=1",
                    "--chaos-entry-slice=1",
                ],
                0,
                "metadata proof selected.\nargs=3\n",
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
                    "--chaos-entry-kind=1",
                    "--chaos-entry-slice=1",
                ],
                cwd=repo_root,
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(
                [
                    "--heartbeat-interval-seconds=5",
                    "--chaos-entry-kind=1",
                    "--chaos-entry-slice=1",
                ],
                manifest["arguments"],
            )
            self.assertEqual(
                ["metadata proof selected.", "args=3"],
                manifest["outputLines"],
            )
            self.assertEqual(
                {
                    "family": "declared-unit-test",
                    "stableId": "HotUpdateHostPack::HotUpdateHostPack::HotUpdateHostPack.MetadataSupplementProofEntry::Run()",
                    "alias": "metadata-supplement-proof",
                },
                manifest["declaredEntrySelection"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
