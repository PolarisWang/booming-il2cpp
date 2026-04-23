from tests.unit.execution.subject_workers_test_support import *


class TestSubjectWorkersHostInputBuildCore(SubjectWorkersTestSupport):
    def test_dotnet_host_input_builder_uses_temp_intermediate_root_on_windows(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_dotnet_host_input")
        subject_id = "FixtureHostInputSubject"
        run_id = "fixture-run-host-input-001"
        intermediate_root = TEST_TMP_ROOT / "dotnet-intermediates" / "fixture-host-input-1234"
        request = {
            "selection": {
                "subjectId": subject_id,
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

        repo_root = self._make_repo_root("host-input-build")
        try:
            with patch.object(workers_module.tooling_module, "allocate_dotnet_intermediate_dir", return_value=intermediate_root):
                with patch.object(workers_module, "_run_checked") as run_checked_mock:
                    result = workers_module.run_dotnet_host_input_builder(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            self.assertEqual(
                [
                    "dotnet",
                    "build",
                    str(repo_root / "subjects" / subject_id / "source" / f"{subject_id}.csproj"),
                    "-c",
                    "Release",
                    "-m:1",
                    "-o",
                    str(repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "host-input"),
                    f"-p:ChaosTempIntermediateRoot={intermediate_root.as_posix()}/",
                ],
                run_checked_mock.call_args.args[0],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_driver_build_uses_chaos_temp_intermediate_root_on_windows(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_driver_build")
        intermediate_root = TEST_TMP_ROOT / "dotnet-intermediates" / "driver-1234abcd"

        with patch.object(workers_module.tooling_module, "allocate_dotnet_intermediate_dir", return_value=intermediate_root):
            with patch.object(workers_module, "_run_checked") as run_checked_mock:
                dll_path = workers_module._ensure_driver_built(REPO_ROOT)

        self.assertEqual(REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "bin" / "Release" / "net8.0" / "Chaos.IL2CPP.Driver.dll", dll_path)
        self.assertEqual(
            [
                "dotnet",
                "build",
                str(REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "Chaos.IL2CPP.Driver.csproj"),
                "-c",
                "Release",
                "-m:1",
                f"-p:ChaosTempIntermediateRoot={intermediate_root.as_posix()}/",
            ],
            run_checked_mock.call_args.args[0],
        )

    def test_host_input_build_uses_primary_solution_project_and_records_primary_project_assembly(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_host_input_solution")
        subject_id = "FixtureSolutionHostInputSubject"
        run_id = "fixture-run-host-input-solution-001"
        intermediate_root = TEST_TMP_ROOT / "dotnet-intermediates" / "fixture-host-input-solution-1234"
        request = {
            "selection": {
                "subjectId": subject_id,
                "source": {
                    "type": "dotnet-project",
                    "path": posix_path("subjects", subject_id, "source", f"{subject_id}.sln"),
                    "primaryProjectPath": posix_path(
                        "subjects",
                        subject_id,
                        "source",
                        "App",
                        "FixtureSolutionHostInput.App.csproj",
                    ),
                    "entry": "FixtureSolutionHostInput.App/Program::Main(System.String[])",
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

        repo_root = self._make_repo_root("host-input-build-solution")
        try:
            source_root = repo_root / "subjects" / subject_id / "source"
            (source_root / "App").mkdir(parents=True, exist_ok=True)
            (source_root / "Library").mkdir(parents=True, exist_ok=True)
            (source_root / f"{subject_id}.sln").write_text(
                "\n".join(
                    [
                        "Project(\"{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}\") = \"FixtureSolutionHostInput.App\", \"App\\\\FixtureSolutionHostInput.App.csproj\", \"{11111111-1111-1111-1111-111111111111}\"",
                        "EndProject",
                        "Project(\"{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}\") = \"FixtureSolutionHostInput.Helper\", \"Library\\\\FixtureSolutionHostInput.Helper.csproj\", \"{22222222-2222-2222-2222-222222222222}\"",
                        "EndProject",
                        "",
                    ]
                ),
                encoding="utf-8",
            )
            (source_root / "App" / "FixtureSolutionHostInput.App.csproj").write_text("<Project />\n", encoding="utf-8")
            (source_root / "Library" / "FixtureSolutionHostInput.Helper.csproj").write_text("<Project />\n", encoding="utf-8")
            expected_output_root = repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "host-input"

            def fake_run_checked(arguments: list[str], *, repo_root: Path, failure_message: str) -> str:
                del failure_message
                self.assertEqual(
                    [
                        "dotnet",
                        "build",
                        str(repo_root / "subjects" / subject_id / "source" / "App" / "FixtureSolutionHostInput.App.csproj"),
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
                    "FixtureSolutionHostInput.App.dll",
                    "FixtureSolutionHostInput.App.deps.json",
                    "FixtureSolutionHostInput.App.pdb",
                    "FixtureSolutionHostInput.Helper.dll",
                    "FixtureSolutionHostInput.Helper.pdb",
                    "Chaos.TestFramework.Sdk.dll",
                    "Newtonsoft.Json.dll",
                ]:
                    (expected_output_root / file_name).write_text("", encoding="utf-8")
                return ""

            with patch.object(workers_module.tooling_module, "allocate_dotnet_intermediate_dir", return_value=intermediate_root):
                with patch.object(workers_module, "_workspace_manifest_is_stale", return_value=False):
                    with patch.object(workers_module, "_run_checked", side_effect=fake_run_checked):
                        result = workers_module.run_dotnet_host_input_builder(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(
                posix_path("subjects", subject_id, "source", "App", "FixtureSolutionHostInput.App.csproj"),
                manifest["primaryProjectPath"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "host-input", "FixtureSolutionHostInput.App.dll"),
                manifest["primaryAssemblyPath"],
            )
            self.assertEqual(
                [
                    subject_run_path(subject_id, run_id, "analysis", "host-input", "FixtureSolutionHostInput.Helper.dll"),
                ],
                manifest["additionalAssemblyPaths"],
            )
            self.assertEqual(
                [
                    subject_run_path(subject_id, run_id, "analysis", "host-input", "Chaos.TestFramework.Sdk.dll"),
                    subject_run_path(subject_id, run_id, "analysis", "host-input", "FixtureSolutionHostInput.App.deps.json"),
                    subject_run_path(subject_id, run_id, "analysis", "host-input", "FixtureSolutionHostInput.App.dll"),
                    subject_run_path(subject_id, run_id, "analysis", "host-input", "FixtureSolutionHostInput.App.pdb"),
                    subject_run_path(subject_id, run_id, "analysis", "host-input", "FixtureSolutionHostInput.Helper.dll"),
                    subject_run_path(subject_id, run_id, "analysis", "host-input", "FixtureSolutionHostInput.Helper.pdb"),
                    subject_run_path(subject_id, run_id, "analysis", "host-input", "Newtonsoft.Json.dll"),
                ],
                manifest["files"],
            )
            self.assertEqual(
                [
                    subject_run_path(subject_id, run_id, "analysis", "host-input", "FixtureSolutionHostInput.App.dll"),
                    subject_run_path(subject_id, run_id, "analysis", "host-input", "FixtureSolutionHostInput.Helper.dll"),
                ],
                result["primaryEvidencePaths"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
