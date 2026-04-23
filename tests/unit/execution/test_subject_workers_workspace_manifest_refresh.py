from tests.unit.execution.subject_workers_test_support import *


class TestSubjectWorkersWorkspaceManifestRefresh(SubjectWorkersTestSupport):
    def test_workspace_manifest_generation_does_not_reenter_itself(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_workspace_manifest_reentry_guard")
        subject_id = "FixtureWorkspaceManifestReentryGuardSubject"
        selection = {
            "subjectId": subject_id,
            "executionContext": {
                "hostPlatform": "windows-x64",
            },
        }

        repo_root = self._make_repo_root("workspace-manifest-reentry-guard")
        try:
            workspace_manifest_path = repo_root / "solutions" / "subjects" / subject_id / "workspace.manifest.json"
            nested_results: list[tuple[Path, dict[str, object]] | None] = []
            generate_calls = 0

            class FakeProjectWorkspaceModule:
                @staticmethod
                def generate_subject_workspace(
                    repo_root_arg: Path,
                    host_platform_arg: str,
                    options_arg: dict[str, object],
                    **kwargs: object,
                ) -> dict[str, object]:
                    nonlocal generate_calls
                    del kwargs
                    generate_calls += 1
                    self.assertEqual(repo_root, repo_root_arg)
                    self.assertEqual("windows", host_platform_arg)
                    self.assertEqual(f"subject/{subject_id}", options_arg["id"])
                    self.assertTrue(bool(options_arg["all-targets"]))
                    self.assertTrue(bool(options_arg["refresh-generated"]))
                    self.assertTrue(bool(options_arg["auto-refresh-missing-generated"]))

                    nested_results.append(workers_module._ensure_subject_workspace_manifest(repo_root, selection))

                    workspace_manifest_path.parent.mkdir(parents=True, exist_ok=True)
                    workspace_manifest_path.write_text(
                        json.dumps(
                            {
                                "workspaceVersion": 2,
                                "subjectId": subject_id,
                                "managedTestProjects": [],
                                "nativeTestProjects": [],
                                "hotupdateTestProjects": [],
                            }
                        ),
                        encoding="utf-8",
                    )
                    return {
                        "manifestPath": posix_path("solutions", "subjects", subject_id, "workspace.manifest.json"),
                    }

            with patch.object(workers_module, "_load_project_workspace_module", return_value=FakeProjectWorkspaceModule):
                loaded_manifest = workers_module._ensure_subject_workspace_manifest(repo_root, selection)

            self.assertIsNotNone(loaded_manifest)
            self.assertEqual(1, generate_calls)
            self.assertEqual([None], nested_results)
            self.assertEqual(workspace_manifest_path, loaded_manifest[0])
            self.assertEqual(subject_id, loaded_manifest[1]["subjectId"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_workspace_manifest_generation_passes_declared_entry_selection_to_project_workspace(self) -> None:
        workers_module = load_module(
            SUBJECT_WORKERS_MODULE_PATH,
            "chaos_subject_workers_workspace_manifest_declared_entry_selection",
        )
        subject_id = "FixtureWorkspaceDeclaredEntrySelectionSubject"
        selection = {
            "subjectId": subject_id,
            "matrixId": "windows-native-check",
            "variant": "CHECK",
            "entrySelection": {
                "family": "declared-unit-test",
                "stableId": f"{subject_id}::{subject_id}::{subject_id}.Proofs::Run()",
                "alias": "workspace-proof",
                "entryIndex": 7,
            },
            "executionContext": {
                "hostPlatform": "windows-x64",
            },
        }

        repo_root = self._make_repo_root("workspace-manifest-declared-entry-selection")
        try:
            workspace_manifest_path = repo_root / "solutions" / "subjects" / subject_id / "workspace.manifest.json"

            class FakeProjectWorkspaceModule:
                @staticmethod
                def generate_subject_workspace(
                    repo_root_arg: Path,
                    host_platform_arg: str,
                    options_arg: dict[str, object],
                    **kwargs: object,
                ) -> dict[str, object]:
                    del kwargs
                    self.assertEqual(repo_root, repo_root_arg)
                    self.assertEqual("windows", host_platform_arg)
                    self.assertEqual(f"subject/{subject_id}", options_arg["id"])
                    self.assertEqual("windows-native-check", options_arg["matrix"])
                    self.assertEqual("CHECK", options_arg["variant"])
                    self.assertTrue(bool(options_arg["refresh-generated"]))
                    self.assertTrue(bool(options_arg["auto-refresh-missing-generated"]))
                    self.assertEqual(selection["entrySelection"], options_arg["entry-selection"])
                    self.assertNotIn("all-targets", options_arg)

                    workspace_manifest_path.parent.mkdir(parents=True, exist_ok=True)
                    workspace_manifest_path.write_text(
                        json.dumps(
                            {
                                "workspaceVersion": 2,
                                "subjectId": subject_id,
                                "managedTestProjects": [],
                                "nativeTestProjects": [],
                                "hotupdateTestProjects": [],
                            }
                        ),
                        encoding="utf-8",
                    )
                    return {
                        "manifestPath": posix_path("solutions", "subjects", subject_id, "workspace.manifest.json"),
                    }

            with patch.object(workers_module, "_load_project_workspace_module", return_value=FakeProjectWorkspaceModule):
                loaded_manifest = workers_module._ensure_subject_workspace_manifest(repo_root, selection)

            self.assertIsNotNone(loaded_manifest)
            self.assertEqual(workspace_manifest_path, loaded_manifest[0])
            self.assertEqual(subject_id, loaded_manifest[1]["subjectId"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_workspace_manifest_refreshes_when_workspace_templates_are_newer(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_workspace_manifest_template_refresh")
        subject_id = "FixtureWorkspaceTemplateRefreshSubject"
        selection = {
            "subjectId": subject_id,
            "executionContext": {
                "hostPlatform": "windows-x64",
            },
        }

        repo_root = self._make_repo_root("workspace-manifest-template-refresh")
        try:
            source_project_path = repo_root / "subjects" / subject_id / "source" / f"{subject_id}.csproj"
            source_project_path.parent.mkdir(parents=True, exist_ok=True)
            source_project_path.write_text("<Project />\n", encoding="utf-8")

            subject_manifest_path = repo_root / "subjects" / subject_id / "subject.manifest.json"
            subject_manifest_path.write_text(
                json.dumps(
                    {
                        "subjectId": subject_id,
                        "source": {
                            "type": "dotnet-project",
                            "path": posix_path("subjects", subject_id, "source", f"{subject_id}.csproj"),
                            "entry": f"{subject_id}/Program::Main()",
                        },
                    }
                ),
                encoding="utf-8",
            )

            workspace_root = repo_root / "solutions" / "subjects" / subject_id
            generated_root = workspace_root / "managed-tests" / "Generated"
            generated_root.mkdir(parents=True, exist_ok=True)
            project_path = workspace_root / "managed-tests" / f"{subject_id}.DeclaredBenchmarkHost.csproj"
            collection_path = generated_root / "declared-tests.collection.json"
            generated_source_path = generated_root / "ChaosGeneratedDeclaredBenchmarks.g.cs"
            project_path.write_text("<Project />\n", encoding="utf-8")
            collection_path.write_text("{\"declaredBenchmarks\":[]}\n", encoding="utf-8")
            generated_source_path.write_text("// stale host\n", encoding="utf-8")

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
                                "hostKind": "benchmark-host",
                                "collectionPath": posix_path(
                                    "solutions",
                                    "subjects",
                                    subject_id,
                                    "managed-tests",
                                    "Generated",
                                    "declared-tests.collection.json",
                                ),
                                "generatedSourcePath": posix_path(
                                    "solutions",
                                    "subjects",
                                    subject_id,
                                    "managed-tests",
                                    "Generated",
                                    "ChaosGeneratedDeclaredBenchmarks.g.cs",
                                ),
                            }
                        ],
                        "nativeTestProjects": [],
                        "hotupdateTestProjects": [],
                    }
                ),
                encoding="utf-8",
            )

            template_path = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "templates" / "managed-benchmark-host.cs.tmpl"
            stale_time = template_path.stat().st_mtime - 60
            for candidate in (
                source_project_path,
                subject_manifest_path,
                project_path,
                collection_path,
                generated_source_path,
                workspace_manifest_path,
            ):
                os.utime(candidate, (stale_time, stale_time))

            self.assertTrue(
                workers_module._workspace_manifest_is_stale(
                    repo_root,
                    subject_id=subject_id,
                    manifest_path=workspace_manifest_path,
                    manifest=json.loads(workspace_manifest_path.read_text(encoding="utf-8")),
                )
            )

            generate_calls = 0

            class FakeProjectWorkspaceModule:
                @staticmethod
                def generate_subject_workspace(
                    repo_root_arg: Path,
                    host_platform_arg: str,
                    options_arg: dict[str, object],
                    **kwargs: object,
                ) -> dict[str, object]:
                    nonlocal generate_calls
                    del kwargs
                    generate_calls += 1
                    self.assertEqual(repo_root, repo_root_arg)
                    self.assertEqual("windows", host_platform_arg)
                    self.assertEqual(f"subject/{subject_id}", options_arg["id"])

                    workspace_manifest_path.write_text(
                        json.dumps(
                            {
                                "workspaceVersion": 2,
                                "subjectId": subject_id,
                                "managedTestProjects": [],
                                "nativeTestProjects": [],
                                "hotupdateTestProjects": [],
                            }
                        ),
                        encoding="utf-8",
                    )
                    return {
                        "manifestPath": posix_path("solutions", "subjects", subject_id, "workspace.manifest.json"),
                    }

            with patch.object(workers_module, "_load_project_workspace_module", return_value=FakeProjectWorkspaceModule):
                loaded_manifest = workers_module._ensure_subject_workspace_manifest(repo_root, selection)

            self.assertIsNotNone(loaded_manifest)
            self.assertEqual(1, generate_calls)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
