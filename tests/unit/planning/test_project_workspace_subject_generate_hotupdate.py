from tests.unit.planning.project_workspace_test_support import *


class TestProjectWorkspaceSubjectGenerateHotupdate(ProjectWorkspaceTestSupport):
    def test_generate_subject_workspace_writes_hotupdate_patch_and_test_projects(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_subject_generate_hotupdate")
        repo_root = self._make_repo_root("subject-generate-hotupdate")
        self._write_hotupdate_subject_fixture(repo_root)

        try:
            with patch.object(
                workspace_module.compiled_catalog_module,
                "build_subject_declared_test_catalog",
                return_value=hotupdate_declared_catalog_fixture(),
            ):
                result = workspace_module.generate_subject_workspace(
                    repo_root,
                    "windows",
                    {"id": "subject/FixtureHotUpdateSubject"},
                )

            manifest = json.loads((repo_root / result["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(
                [
                    {
                        "projectId": "hotupdate-patch/FixtureHotUpdateSubject/FixtureHotUpdateSubject_Patch",
                        "managedProjectId": "managed/FixtureHotUpdateSubject/FixtureHotUpdateSubject_Patch",
                        "projectPath": "subjects/FixtureHotUpdateSubject/source/Patch/FixtureHotUpdateSubject.Patch.csproj",
                        "assemblyName": "FixtureHotUpdateSubject.Patch",
                    }
                ],
                manifest["hotupdatePatchProjects"],
            )
            self.assertEqual(
                [
                    "hotupdate-test/FixtureHotUpdateSubject/proof-host",
                    "hotupdate-test/FixtureHotUpdateSubject/benchmark-host",
                ],
                [item["projectId"] for item in manifest["hotupdateTestProjects"]],
            )
            self.assertEqual(
                [
                    "hotupdate-patch/FixtureHotUpdateSubject/FixtureHotUpdateSubject_Patch",
                ],
                manifest["matrices"][0]["hotupdatePatchProjectIds"],
            )
            self.assertEqual(
                [
                    "hotupdate-test/FixtureHotUpdateSubject/proof-host",
                    "hotupdate-test/FixtureHotUpdateSubject/benchmark-host",
                ],
                manifest["matrices"][0]["hotupdateTestProjectIds"],
            )
            for host_project in manifest["hotupdateTestProjects"]:
                self.assertTrue((repo_root / host_project["projectPath"]).is_file())
                self.assertTrue((repo_root / host_project["generatedSourcePath"]).is_file())
                self.assertTrue((repo_root / host_project["bindingManifestPath"]).is_file())
                self.assertEqual(
                    [
                        "hotupdate-patch/FixtureHotUpdateSubject/FixtureHotUpdateSubject_Patch",
                    ],
                    host_project["patchProjectIds"],
                )
            proof_project_text = (repo_root / manifest["hotupdateTestProjects"][0]["projectPath"]).read_text(encoding="utf-8")
            benchmark_project_text = (repo_root / manifest["hotupdateTestProjects"][1]["projectPath"]).read_text(encoding="utf-8")
            self.assertIn("<EnableDefaultCompileItems>false</EnableDefaultCompileItems>", proof_project_text)
            self.assertIn(
                '<Compile Include="Generated/ChaosGeneratedHotUpdateProofHost.g.cs" />',
                proof_project_text,
            )
            self.assertIn(
                '<Compile Include="Generated/ChaosGeneratedHotUpdateBenchmarkHost.g.cs" />',
                benchmark_project_text,
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_generate_subject_workspace_overwrites_existing_hotupdate_collection_with_latest_catalog(self) -> None:
        workspace_module = load_module(
            PROJECT_WORKSPACE_MODULE_PATH,
            "chaos_project_workspace_subject_generate_hotupdate_overwrite_collection",
        )
        subject_id = "FixtureHotUpdateSubject"
        repo_root = self._make_repo_root("subject-generate-hotupdate-overwrite-collection")
        self._write_hotupdate_subject_fixture(repo_root, subject_id=subject_id)
        stale_collection_path = (
            repo_root
            / "verification" / "workspaces" / "subjects"
            / subject_id
            / "hotupdate-tests"
            / "Generated"
            / "declared-tests.collection.json"
        )
        write_json(
            stale_collection_path,
            {
                "subjectId": subject_id,
                "declaredUnitTests": [],
                "declaredBenchmarks": [
                    {
                        "stableId": f"{subject_id}::stale",
                        "entryIndex": 0,
                        "alias": "stale-hotupdate-benchmark",
                    }
                ],
            },
        )
        refreshed_catalog = hotupdate_declared_catalog_fixture(subject_id)
        refreshed_catalog["declaredBenchmarks"][0]["entryIndex"] = 3
        refreshed_catalog["declaredBenchmarks"][0]["alias"] = "hotupdate-benchmark-refreshed"

        try:
            with patch.object(
                workspace_module.compiled_catalog_module,
                "build_subject_declared_test_catalog",
                return_value=refreshed_catalog,
            ):
                workspace_module.generate_subject_workspace(
                    repo_root,
                    "windows",
                    {"id": f"subject/{subject_id}"},
                )

            collection_payload = json.loads(stale_collection_path.read_text(encoding="utf-8"))
            binding_manifest_path = stale_collection_path.with_name("declared-tests.binding.json")
            binding_payload = json.loads(binding_manifest_path.read_text(encoding="utf-8"))

            self.assertEqual(refreshed_catalog, collection_payload)
            self.assertEqual(
                3,
                next(
                    entry["entryIndex"]
                    for entry in binding_payload["entryBindings"]
                    if entry["hostKind"] == "benchmark-host"
                ),
            )
            self.assertEqual(
                "hotupdate-benchmark-refreshed",
                collection_payload["declaredBenchmarks"][0]["alias"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

