from tests.unit.execution.subject_workers_test_support import *


class TestSubjectWorkersHostInputBuildWorkspaceSubjectEntry(SubjectWorkersTestSupport):
    def test_host_input_build_keeps_source_project_for_subject_entry_selection_managed_output(self) -> None:
        workers_module = load_module(
            SUBJECT_WORKERS_MODULE_PATH,
            "chaos_subject_workers_host_input_subject_entry_managed_output",
        )
        subject_id = "FixtureWorkspaceManagedEntryOutputSubject"
        run_id = "fixture-run-host-input-subject-entry-managed-output-001"
        intermediate_root = TEST_TMP_ROOT / "dotnet-intermediates" / "fixture-host-input-subject-entry-managed-output-1234"
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": "windows-managed-output",
                "source": {
                    "type": "dotnet-project",
                    "path": subject_source_path(subject_id),
                    "entry": f"{subject_id}/Program::Main(System.String[])",
                    "entrySelection": {
                        "entryKind": 1,
                        "entrySlice": 3,
                    },
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

        repo_root = self._make_repo_root("host-input-build-subject-entry-managed-output")
        try:
            source_root = repo_root / "subjects" / subject_id / "source"
            source_root.mkdir(parents=True, exist_ok=True)
            source_project_path = source_root / f"{subject_id}.csproj"
            source_project_path.write_text("<Project />\n", encoding="utf-8")

            workspace_root = repo_root / "solutions" / "subjects" / subject_id
            managed_tests_root = workspace_root / "managed-tests"
            generated_root = managed_tests_root / "Generated"
            generated_root.mkdir(parents=True, exist_ok=True)
            benchmark_host_project_path = managed_tests_root / f"{subject_id}.DeclaredBenchmarkHost.csproj"
            benchmark_host_project_path.write_text("<Project />\n", encoding="utf-8")
            collection_path = generated_root / "declared-tests.collection.json"
            collection_path.write_text('{"declaredBenchmarks":[{"entryIndex":11}]}', encoding="utf-8")
            workspace_manifest_path = workspace_root / "workspace.manifest.json"
            workspace_manifest_path.write_text(
                json.dumps(
                    {
                        "workspaceVersion": 2,
                        "subjectId": subject_id,
                        "managedTestProjects": [
                            {
                                "projectId": f"managed-test/{subject_id}/benchmark-host",
                                "projectPath": posix_path(
                                    "solutions",
                                    "subjects",
                                    subject_id,
                                    "managed-tests",
                                    f"{subject_id}.DeclaredBenchmarkHost.csproj",
                                ),
                                "assemblyName": f"{subject_id}.DeclaredBenchmarkHost",
                                "hostKind": "benchmark-host",
                                "collectionPath": posix_path(
                                    "solutions",
                                    "subjects",
                                    subject_id,
                                    "managed-tests",
                                    "Generated",
                                    "declared-tests.collection.json",
                                ),
                            }
                        ],
                    }
                ),
                encoding="utf-8",
            )
            expected_output_root = repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "host-input"

            def fake_run_checked(arguments: list[str], *, repo_root: Path, failure_message: str) -> str:
                del failure_message
                self.assertEqual(
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
                    arguments,
                )
                expected_output_root.mkdir(parents=True, exist_ok=True)
                for file_name in [
                    f"{subject_id}.dll",
                    f"{subject_id}.deps.json",
                    f"{subject_id}.pdb",
                    "Chaos.TestFramework.Sdk.dll",
                ]:
                    (expected_output_root / file_name).write_text("", encoding="utf-8")
                return ""

            with patch.object(workers_module.tooling_module, "allocate_dotnet_intermediate_dir", return_value=intermediate_root):
                with patch.object(workers_module, "_workspace_manifest_is_stale", return_value=False):
                    with patch.object(workers_module, "_run_checked", side_effect=fake_run_checked):
                        result = workers_module.run_dotnet_host_input_builder(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(subject_source_path(subject_id), manifest["primaryProjectPath"])
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.dll"),
                manifest["primaryAssemblyPath"],
            )
            self.assertEqual([], manifest["additionalAssemblyPaths"])
            self.assertNotIn("hostKind", manifest)
            self.assertNotIn("collectionPath", manifest)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
