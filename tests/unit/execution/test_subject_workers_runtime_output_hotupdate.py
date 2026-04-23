from tests.unit.execution.subject_workers_test_support import *


class TestSubjectWorkersRuntimeOutputHotupdate(SubjectWorkersTestSupport):
    def test_runtime_managed_output_passes_binding_manifest_for_hotupdate_proof_host(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_managed_runtime_output_hotupdate_proof_host")
        subject_id = "FixtureHotUpdateRuntimeSubject"
        run_id = "fixture-run-managed-output-hotupdate-proof-host-001"
        matrix_id = "windows-hotupdate-output"
        assembly_path = subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.HotUpdateProofHost.dll")
        collection_path = posix_path(
            "verification", "workspaces",
            "subjects",
            subject_id,
            "hotupdate-tests",
            "Generated",
            "declared-tests.collection.json",
        )
        binding_manifest_path = posix_path(
            "verification", "workspaces",
            "subjects",
            subject_id,
            "hotupdate-tests",
            "Generated",
            "declared-tests.binding.json",
        )
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "entrySelection": {
                    "family": "declared-unit-test",
                    "stableId": f"{subject_id}::{subject_id}.Patch::{subject_id}.Patch.Proofs::Run()",
                    "alias": "hotupdate-proof",
                    "entryIndex": 4,
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "runtimeArguments": [
                        "--heartbeat-interval-seconds=3",
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

        repo_root = self._make_repo_root("managed-runtime-output-hotupdate-proof-host")
        try:
            host_input_manifest_path = repo_root / request["upstream"]["host-input"]["manifestPath"]
            host_input_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            host_input_manifest_path.write_text(
                json.dumps(
                    {
                        "primaryAssemblyPath": assembly_path,
                        "hostKind": "proof-host",
                        "collectionPath": collection_path,
                        "bindingManifestPath": binding_manifest_path,
                    }
                ),
                encoding="utf-8",
            )

            completed = subprocess.CompletedProcess(
                [
                    "dotnet",
                    str(repo_root / assembly_path),
                    "--heartbeat-interval-seconds=3",
                    f"--collection-path={collection_path}",
                    f"--binding-manifest-path={binding_manifest_path}",
                    "--entry-index=4",
                ],
                0,
                "managed hotupdate proof host reached.\nargs=4\n",
                "",
            )

            with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                result = workers_module.run_managed_runtime_output(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            run_process_mock.assert_called_once_with(
                [
                    "dotnet",
                    str(repo_root / assembly_path),
                    "--heartbeat-interval-seconds=3",
                    f"--collection-path={collection_path}",
                    f"--binding-manifest-path={binding_manifest_path}",
                    "--entry-index=4",
                ],
                cwd=repo_root,
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(binding_manifest_path, manifest["bindingManifestPath"])
            self.assertEqual(
                [
                    "--heartbeat-interval-seconds=3",
                    f"--collection-path={collection_path}",
                    f"--binding-manifest-path={binding_manifest_path}",
                    "--entry-index=4",
                ],
                manifest["arguments"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_runtime_managed_output_records_native_upstream_for_native_hotupdate_chain(self) -> None:
        workers_module = load_module(
            SUBJECT_WORKERS_MODULE_PATH,
            "chaos_subject_workers_managed_runtime_output_native_hotupdate_chain",
        )
        subject_id = "FixtureHotUpdateRuntimeSubject"
        run_id = "fixture-run-managed-output-native-hotupdate-proof-host-001"
        matrix_id = "windows-corelib-reference-native-hotupdate-proof"
        native_assembly_path = subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.NativeProofApp.dll")
        managed_runtime_assembly_path = subject_run_path(
            subject_id,
            run_id,
            "analysis",
            "host-input",
            f"{subject_id}.HotUpdateProofHost.dll",
        )
        collection_path = posix_path(
            "verification", "workspaces",
            "subjects",
            subject_id,
            "hotupdate-tests",
            "Generated",
            "declared-tests.collection.json",
        )
        binding_manifest_path = posix_path(
            "verification", "workspaces",
            "subjects",
            subject_id,
            "hotupdate-tests",
            "Generated",
            "declared-tests.binding.json",
        )
        generated_manifest_path = subject_run_path(subject_id, run_id, "analysis", "generated", "generated.manifest.json")
        build_manifest_path = subject_run_path(subject_id, run_id, "matrices", matrix_id, "build", "build.manifest.json")
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "entrySelection": {
                    "family": "declared-unit-test",
                    "stableId": f"{subject_id}::{subject_id}.Patch::{subject_id}.Patch.Proofs::Run()",
                    "alias": "hotupdate-proof",
                    "entryIndex": 4,
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "runtimeArguments": [
                        "--heartbeat-interval-seconds=3",
                    ],
                    "runtimeProfile": "native-hotupdate-proof-output",
                },
            },
            "upstream": {
                "host-input": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "host-input", "host-input.manifest.json"),
                },
                "generated": {
                    "manifestPath": generated_manifest_path,
                },
                "build": {
                    "manifestPath": build_manifest_path,
                },
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "runtime.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("managed-runtime-output-native-hotupdate-proof-host")
        try:
            host_input_manifest_path = repo_root / request["upstream"]["host-input"]["manifestPath"]
            host_input_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            host_input_manifest_path.write_text(
                json.dumps(
                    {
                        "primaryAssemblyPath": native_assembly_path,
                        "managedRuntimeAssemblyPath": managed_runtime_assembly_path,
                        "hostKind": "proof-host",
                        "collectionPath": collection_path,
                        "bindingManifestPath": binding_manifest_path,
                    }
                ),
                encoding="utf-8",
            )

            completed = subprocess.CompletedProcess(
                [
                    "dotnet",
                    str(repo_root / managed_runtime_assembly_path),
                    "--heartbeat-interval-seconds=3",
                    f"--collection-path={collection_path}",
                    f"--binding-manifest-path={binding_manifest_path}",
                    "--entry-index=4",
                ],
                0,
                "managed hotupdate proof host reached.\nargs=4\n",
                "",
            )

            with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                result = workers_module.run_managed_runtime_output(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            run_process_mock.assert_called_once_with(
                [
                    "dotnet",
                    str(repo_root / managed_runtime_assembly_path),
                    "--heartbeat-interval-seconds=3",
                    f"--collection-path={collection_path}",
                    f"--binding-manifest-path={binding_manifest_path}",
                    "--entry-index=4",
                ],
                cwd=repo_root,
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(native_assembly_path, manifest["nativePrimaryAssemblyPath"])
            self.assertEqual(managed_runtime_assembly_path, manifest["managedRuntimeAssemblyPath"])
            self.assertEqual(generated_manifest_path, manifest["nativeGeneratedManifestPath"])
            self.assertEqual(build_manifest_path, manifest["nativeBuildManifestPath"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

