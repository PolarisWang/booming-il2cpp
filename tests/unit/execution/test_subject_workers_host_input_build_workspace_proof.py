from tests.unit.execution.subject_workers_test_support import *


class TestSubjectWorkersHostInputBuildWorkspaceProof(SubjectWorkersTestSupport):
    def test_host_input_build_uses_workspace_managed_proof_host_for_declared_unit_test(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_host_input_workspace_proof_host")
        subject_id = "FixtureWorkspaceProofHostSubject"
        run_id = "fixture-run-host-input-workspace-proof-host-001"
        intermediate_root = TEST_TMP_ROOT / "dotnet-intermediates" / "fixture-host-input-workspace-proof-host-1234"
        request = {
            "selection": {
                "subjectId": subject_id,
                "entrySelection": {
                    "family": "declared-unit-test",
                    "stableId": f"{subject_id}::{subject_id}::{subject_id}.Proofs::Run()",
                    "alias": "workspace-proof",
                    "entryIndex": 7,
                },
                "source": {
                    "type": "dotnet-project",
                    "path": subject_source_path(subject_id),
                    "entry": f"{subject_id}/Program::Main(System.String[])",
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                },
            },
            "upstream": {
                "source": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "source", "source.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "analysis", "host-input"),
                "manifestPath": subject_run_path(subject_id, run_id, "analysis", "host-input", "host-input.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("host-input-build-workspace-proof-host")
        try:
            source_root = repo_root / "subjects" / subject_id / "source"
            source_root.mkdir(parents=True, exist_ok=True)
            (source_root / f"{subject_id}.csproj").write_text("<Project />\n", encoding="utf-8")

            workspace_root = repo_root / "solutions" / "subjects" / subject_id
            managed_tests_root = workspace_root / "managed-tests"
            generated_root = managed_tests_root / "Generated"
            generated_root.mkdir(parents=True, exist_ok=True)
            collection_path = generated_root / "declared-tests.collection.json"
            collection_path.write_text('{"declaredUnitTests":[{"entryIndex":7}]}', encoding="utf-8")
            workspace_manifest_path = workspace_root / "workspace.manifest.json"
            workspace_manifest_path.write_text(
                json.dumps(
                    {
                        "workspaceVersion": 2,
                        "subjectId": subject_id,
                        "managedTestProjects": [
                            {
                                "projectId": f"managed-test/{subject_id}/proof-host",
                                "projectPath": SHARED_RUNTIME_PROJECT_PATH,
                                "assemblyName": SHARED_RUNTIME_ASSEMBLY_NAME,
                                "hostKind": "proof-host",
                                "collectionPath": posix_path(
                                    "solutions",
                                    "subjects",
                                    subject_id,
                                    "managed-tests",
                                    "Generated",
                                    "declared-tests.collection.json",
                                ),
                                "executionModel": "shared-runtime-host",
                            }
                        ],
                    }
                ),
                encoding="utf-8",
            )
            expected_output_root = repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "host-input"
            source_project_path = repo_root / "subjects" / subject_id / "source" / f"{subject_id}.csproj"
            expected_calls: list[list[str]] = [
                [
                    "dotnet",
                    "build",
                    str(source_project_path),
                    "-c",
                    "Release",
                    "-m:1",
                    "-o",
                    str(expected_output_root),
                    f"-p:ChaosTempIntermediateRoot={intermediate_root.as_posix()}/",
                ],
                [
                    "dotnet",
                    "build",
                    str(repo_root / SHARED_RUNTIME_PROJECT_PATH),
                    "-c",
                    "Release",
                    "-m:1",
                    "-o",
                    str(expected_output_root),
                    f"-p:ChaosTempIntermediateRoot={intermediate_root.as_posix()}/",
                ],
            ]

            def fake_run_checked(arguments: list[str], *, repo_root: Path, failure_message: str) -> str:
                del failure_message
                self.assertTrue(expected_calls)
                self.assertEqual(expected_calls.pop(0), arguments)
                expected_output_root.mkdir(parents=True, exist_ok=True)
                for file_name in [
                    f"{subject_id}.dll",
                    f"{subject_id}.pdb",
                    "Chaos.TestFramework.Runtime.deps.json",
                    "Chaos.TestFramework.Sdk.dll",
                    "Chaos.TestFramework.Runtime.dll",
                ]:
                    (expected_output_root / file_name).write_text("", encoding="utf-8")
                return ""

            with patch.object(workers_module.tooling_module, "allocate_dotnet_intermediate_dir", return_value=intermediate_root):
                with patch.object(workers_module, "_workspace_manifest_is_stale", return_value=False):
                    with patch.object(workers_module, "_run_checked", side_effect=fake_run_checked):
                        result = workers_module.run_dotnet_host_input_builder(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            self.assertEqual([], expected_calls)
            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(SHARED_RUNTIME_PROJECT_PATH, manifest["primaryProjectPath"])
            self.assertEqual("proof-host", manifest["hostKind"])
            self.assertEqual(
                posix_path("solutions", "subjects", subject_id, "managed-tests", "Generated", "declared-tests.collection.json"),
                manifest["collectionPath"],
            )
            self.assertEqual("shared-runtime-host", manifest["hostExecutionModel"])
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "host-input", "Chaos.TestFramework.Runtime.dll"),
                manifest["primaryAssemblyPath"],
            )
            self.assertEqual(
                [subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.dll")],
                manifest["additionalAssemblyPaths"],
            )
            self.assertEqual(
                [
                    subject_run_path(subject_id, run_id, "analysis", "host-input", "Chaos.TestFramework.Runtime.dll"),
                    subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.dll"),
                ],
                result["primaryEvidencePaths"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
