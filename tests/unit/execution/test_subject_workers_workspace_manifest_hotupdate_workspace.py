from tests.unit.execution.subject_workers_test_support import *


class TestSubjectWorkersWorkspaceManifestHotupdateWorkspace(SubjectWorkersTestSupport):
    def test_host_input_build_uses_workspace_hotupdate_host_and_records_binding_manifest(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_host_input_workspace_hotupdate_host")
        subject_id = "FixtureHotUpdateHostSubject"
        run_id = "fixture-run-host-input-workspace-hotupdate-host-001"
        intermediate_root = TEST_TMP_ROOT / "dotnet-intermediates" / "fixture-host-input-workspace-hotupdate-host-1234"
        request = {
            "selection": {
                "subjectId": subject_id,
                "engineeringProfile": "hot-update-host",
                "entrySelection": {
                    "family": "declared-unit-test",
                    "stableId": f"{subject_id}::{subject_id}.Patch::{subject_id}.Patch.Proofs::Run()",
                    "alias": "workspace-hotupdate-proof",
                    "entryIndex": 3,
                },
                "source": {
                    "type": "dotnet-project",
                    "path": posix_path("subjects", subject_id, "source", f"{subject_id}.sln"),
                    "primaryProjectPath": posix_path("subjects", subject_id, "source", "Host", f"{subject_id}.Host.csproj"),
                    "entry": f"{subject_id}.Host/Program::Main()",
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "runtimeProfile": "hot-update-proof",
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

        repo_root = self._make_repo_root("host-input-build-workspace-hotupdate-host")
        try:
            source_root = repo_root / "subjects" / subject_id / "source"
            host_project_path = source_root / "Host" / f"{subject_id}.Host.csproj"
            patch_project_path = source_root / "Patch" / f"{subject_id}.Patch.csproj"
            solution_path = source_root / f"{subject_id}.sln"
            host_project_path.parent.mkdir(parents=True, exist_ok=True)
            patch_project_path.parent.mkdir(parents=True, exist_ok=True)
            host_project_path.write_text(
                "\n".join(
                    [
                        "<Project>",
                        "  <PropertyGroup>",
                        f"    <AssemblyName>{subject_id}.Host</AssemblyName>",
                        "  </PropertyGroup>",
                        "</Project>",
                        "",
                    ]
                ),
                encoding="utf-8",
            )
            patch_project_path.write_text(
                "\n".join(
                    [
                        "<Project>",
                        "  <PropertyGroup>",
                        f"    <AssemblyName>{subject_id}.Patch</AssemblyName>",
                        "  </PropertyGroup>",
                        "</Project>",
                        "",
                    ]
                ),
                encoding="utf-8",
            )
            solution_path.write_text(
                "\n".join(
                    [
                        "Project(\"{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}\") = \"Host\", \"Host\\\\"
                        f"{subject_id}.Host.csproj\", \"{{11111111-1111-1111-1111-111111111111}}\"",
                        "EndProject",
                        "Project(\"{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}\") = \"Patch\", \"Patch\\\\"
                        f"{subject_id}.Patch.csproj\", \"{{22222222-2222-2222-2222-222222222222}}\"",
                        "EndProject",
                        "",
                    ]
                ),
                encoding="utf-8",
            )

            workspace_root = repo_root / "verification" / "workspaces" / "subjects" / subject_id
            hotupdate_tests_root = workspace_root / "hotupdate-tests"
            generated_root = hotupdate_tests_root / "Generated"
            generated_root.mkdir(parents=True, exist_ok=True)
            hotupdate_host_project_path = hotupdate_tests_root / f"{subject_id}.HotUpdateProofHost.csproj"
            hotupdate_host_project_path.write_text("<Project />\n", encoding="utf-8")
            binding_manifest_path = generated_root / "declared-tests.binding.json"
            binding_manifest_path.write_text(
                json.dumps(
                    {
                        "hostKind": "proof-host",
                        "patchAssemblyNames": [f"{subject_id}.Patch"],
                    }
                ),
                encoding="utf-8",
            )
            collection_path = generated_root / "declared-tests.collection.json"
            collection_path.write_text('{"declaredUnitTests":[{"entryIndex":3}]}', encoding="utf-8")
            workspace_manifest_path = workspace_root / "workspace.manifest.json"
            workspace_manifest_path.write_text(
                json.dumps(
                    {
                        "workspaceVersion": 2,
                        "subjectId": subject_id,
                        "hotupdatePatchProjects": [
                            {
                                "projectId": f"hotupdate-patch/{subject_id}/{subject_id}_Patch",
                                "managedProjectId": f"managed/{subject_id}/{subject_id}_Patch",
                                "projectPath": posix_path("subjects", subject_id, "source", "Patch", f"{subject_id}.Patch.csproj"),
                                "assemblyName": f"{subject_id}.Patch",
                            }
                        ],
                        "hotupdateTestProjects": [
                            {
                                "projectId": f"hotupdate-test/{subject_id}/proof-host",
                                "projectPath": posix_path(
                                    "verification", "workspaces",
                                    "subjects",
                                    subject_id,
                                    "hotupdate-tests",
                                    f"{subject_id}.HotUpdateProofHost.csproj",
                                ),
                                "assemblyName": f"{subject_id}.HotUpdateProofHost",
                                "hostKind": "proof-host",
                                "collectionPath": posix_path(
                                    "verification", "workspaces",
                                    "subjects",
                                    subject_id,
                                    "hotupdate-tests",
                                    "Generated",
                                    "declared-tests.collection.json",
                                ),
                                "bindingManifestPath": posix_path(
                                    "verification", "workspaces",
                                    "subjects",
                                    subject_id,
                                    "hotupdate-tests",
                                    "Generated",
                                    "declared-tests.binding.json",
                                ),
                                "patchProjectIds": [f"hotupdate-patch/{subject_id}/{subject_id}_Patch"],
                            }
                        ],
                        "matrices": [
                            {
                                "matrixId": "windows-hotupdate-proof",
                                "hotupdatePatchProjectIds": [f"hotupdate-patch/{subject_id}/{subject_id}_Patch"],
                                "hotupdateTestProjectIds": [f"hotupdate-test/{subject_id}/proof-host"],
                            }
                        ],
                    }
                ),
                encoding="utf-8",
            )
            expected_output_root = repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "host-input"
            captured_arguments: list[list[str]] = []

            def fake_run_checked(arguments: list[str], *, repo_root: Path, failure_message: str) -> str:
                del repo_root, failure_message
                captured_arguments.append(list(arguments))
                expected_output_root.mkdir(parents=True, exist_ok=True)
                project_argument = arguments[2]
                if hotupdate_host_project_path.name in project_argument:
                    for file_name in [
                        f"{subject_id}.HotUpdateProofHost.dll",
                        f"{subject_id}.HotUpdateProofHost.deps.json",
                        f"{subject_id}.HotUpdateProofHost.pdb",
                        "Chaos.TestFramework.Sdk.dll",
                        "Chaos.TestFramework.Runtime.dll",
                    ]:
                        (expected_output_root / file_name).write_text("", encoding="utf-8")
                elif host_project_path.name in project_argument:
                    for file_name in [f"{subject_id}.Host.dll", f"{subject_id}.Host.pdb"]:
                        (expected_output_root / file_name).write_text("", encoding="utf-8")
                elif patch_project_path.name in project_argument:
                    for file_name in [f"{subject_id}.Patch.dll", f"{subject_id}.Patch.pdb"]:
                        (expected_output_root / file_name).write_text("", encoding="utf-8")
                return ""

            with patch.object(workers_module.tooling_module, "allocate_dotnet_intermediate_dir", return_value=intermediate_root):
                with patch.object(workers_module, "_run_checked", side_effect=fake_run_checked):
                    result = workers_module.run_dotnet_host_input_builder(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            self.assertEqual(
                [
                    "dotnet",
                    "build",
                    str(hotupdate_host_project_path),
                    "-c",
                    "Release",
                    "-m:1",
                    "-o",
                    str(expected_output_root),
                    f"-p:ChaosTempIntermediateRoot={intermediate_root.as_posix()}/",
                ],
                captured_arguments[0],
            )
            self.assertEqual(
                [
                    "dotnet",
                    "build",
                    str(patch_project_path),
                    "-c",
                    "Release",
                    "-m:1",
                    "-o",
                    str(expected_output_root),
                    f"-p:ChaosTempIntermediateRoot={intermediate_root.as_posix()}/",
                ],
                captured_arguments[1],
            )
            self.assertEqual(2, len(captured_arguments))

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(
                posix_path("verification", "workspaces", "subjects", subject_id, "hotupdate-tests", f"{subject_id}.HotUpdateProofHost.csproj"),
                manifest["primaryProjectPath"],
            )
            self.assertEqual("proof-host", manifest["hostKind"])
            self.assertEqual(
                posix_path("verification", "workspaces", "subjects", subject_id, "hotupdate-tests", "Generated", "declared-tests.collection.json"),
                manifest["collectionPath"],
            )
            self.assertEqual(
                posix_path("verification", "workspaces", "subjects", subject_id, "hotupdate-tests", "Generated", "declared-tests.binding.json"),
                manifest["bindingManifestPath"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.HotUpdateProofHost.dll"),
                manifest["primaryAssemblyPath"],
            )
            self.assertEqual(
                [
                    subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.Patch.dll"),
                ],
                manifest["additionalAssemblyPaths"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)


