from tests.unit.planning.project_workspace_test_support import *


class TestProjectWorkspaceAll(ProjectWorkspaceTestSupport):
    def test_generate_all_workspaces_writes_root_manifest_and_generation_report(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_all_generate")
        repo_root = self._make_repo_root("all-generate")
        self._write_subject_fixture(repo_root)
        self._write_core_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")

        try:
            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                (configure_root / "chaos_subject_reference_proof.vcxproj").parent.mkdir(parents=True, exist_ok=True)
                (configure_root / "chaos_subject_reference_proof.vcxproj").write_text("<Project />\n", encoding="utf-8")
                return completed

            with patch.object(workspace_module, "refresh_subject_generated_root", return_value=None):
                with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                    with patch.object(workspace_module, "run_process", side_effect=configure_side_effect):
                        result = workspace_module.generate_all_workspaces(
                            repo_root,
                            "windows",
                            {"host": "windows"},
                        )

            self.assertEqual("verification/workspaces/verification-all.manifest.json", result["manifestPath"])
            aggregate_manifest = json.loads((repo_root / "verification" / "workspaces" / "verification-all.manifest.json").read_text(encoding="utf-8"))
            generation_report = json.loads((repo_root / "verification" / "workspaces" / "verification-all.generation.report.json").read_text(encoding="utf-8"))
            self.assertEqual("all-workspaces", aggregate_manifest["kind"])
            self.assertEqual("windows", aggregate_manifest["hostPlatform"])
            self.assertEqual(
                ["verification/workspaces/subjects/FixtureSubject/workspace.manifest.json"],
                aggregate_manifest["subjectWorkspaceManifests"],
            )
            self.assertEqual(
                "verification/workspaces/core/windows/workspace.manifest.json",
                aggregate_manifest["coreWorkspaceManifest"],
            )
            self.assertEqual(["FixtureSubject"], generation_report["generatedSubjectIds"])
            self.assertEqual(
                ["windows-x64-reference", "android-arm64", "linux-x64"],
                generation_report["generatedCoreTargets"],
            )
            self.assertEqual("completed", aggregate_manifest["status"])
            self.assertEqual("completed", generation_report["status"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
    def test_generate_all_workspaces_materializes_root_outputs_before_subject_generation(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_all_early_root_outputs")
        repo_root = self._make_repo_root("all-early-root-outputs")
        self._write_subject_fixture(repo_root)
        self._write_core_fixture(repo_root)

        try:
            def subject_side_effect(
                subject_repo_root: Path,
                subject_host_platform: str,
                options: dict[str, object],
                **kwargs: object,
            ) -> dict[str, object]:
                del subject_host_platform, options
                aggregate_manifest_path = subject_repo_root / "verification" / "workspaces" / "verification-all.manifest.json"
                generation_report_path = subject_repo_root / "verification" / "workspaces" / "verification-all.generation.report.json"

                self.assertTrue(aggregate_manifest_path.is_file())
                self.assertTrue(generation_report_path.is_file())

                aggregate_manifest = json.loads(aggregate_manifest_path.read_text(encoding="utf-8"))
                generation_report = json.loads(generation_report_path.read_text(encoding="utf-8"))
                self.assertEqual("running", aggregate_manifest["status"])
                self.assertEqual("running", generation_report["status"])
                self.assertEqual([], aggregate_manifest["subjectWorkspaceManifests"])
                self.assertEqual("", aggregate_manifest["coreWorkspaceManifest"])
                self.assertEqual([], generation_report["generatedSubjectIds"])
                self.assertEqual([], generation_report["generatedCoreTargets"])

                subject_manifest_path = subject_repo_root / "verification" / "workspaces" / "subjects" / "FixtureSubject" / "workspace.manifest.json"
                write_json(subject_manifest_path, {"kind": "subject-workspace"})
                return {
                    "manifestPath": "verification/workspaces/subjects/FixtureSubject/workspace.manifest.json",
                    "artifacts": ["verification/workspaces/subjects/FixtureSubject/workspace.manifest.json"],
                    "importantOutputs": [],
                    "consoleText": "",
                }

            def core_side_effect(
                core_repo_root: Path,
                core_host_platform: str,
                options: dict[str, object],
                **kwargs: object,
            ) -> dict[str, object]:
                del core_host_platform, options
                core_manifest_path = core_repo_root / "verification" / "workspaces" / "core" / "windows" / "workspace.manifest.json"
                write_json(
                    core_manifest_path,
                    {
                        "kind": "core-workspace",
                        "nativeTargets": [{"targetId": "windows-x64-reference"}],
                    },
                )
                return {
                    "manifestPath": "verification/workspaces/core/windows/workspace.manifest.json",
                    "artifacts": ["verification/workspaces/core/windows/workspace.manifest.json"],
                    "importantOutputs": [],
                    "consoleText": "",
                }

            with patch.object(workspace_module, "generate_subject_workspace", side_effect=subject_side_effect):
                with patch.object(workspace_module, "generate_core_workspace", side_effect=core_side_effect):
                    result = workspace_module.generate_all_workspaces(
                        repo_root,
                        "windows",
                        {"host": "windows"},
                    )

            self.assertEqual("verification/workspaces/verification-all.manifest.json", result["manifestPath"])
            aggregate_manifest = json.loads((repo_root / "verification" / "workspaces" / "verification-all.manifest.json").read_text(encoding="utf-8"))
            generation_report = json.loads((repo_root / "verification" / "workspaces" / "verification-all.generation.report.json").read_text(encoding="utf-8"))
            self.assertEqual("completed", aggregate_manifest["status"])
            self.assertEqual("completed", generation_report["status"])
            self.assertEqual(
                ["verification/workspaces/subjects/FixtureSubject/workspace.manifest.json"],
                aggregate_manifest["subjectWorkspaceManifests"],
            )
            self.assertEqual("verification/workspaces/core/windows/workspace.manifest.json", aggregate_manifest["coreWorkspaceManifest"])
            self.assertEqual(["FixtureSubject"], generation_report["generatedSubjectIds"])
            self.assertEqual(["windows-x64-reference"], generation_report["generatedCoreTargets"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
    def test_generate_all_workspaces_refreshes_missing_subject_generated_when_needed(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_all_refresh_missing")
        repo_root = self._make_repo_root("all-refresh-missing")
        self._write_subject_fixture(repo_root)
        self._write_core_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")
        generated_source_path = (
            repo_root
            / "artifacts"
            / "subjects"
            / "FixtureSubject"
            / "runs"
            / "subject-exec"
            / "analysis"
            / "generated"
            / "generated"
            / "native-reference.generated.cpp"
        )
        generated_source_path.unlink()

        try:
            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                (configure_root / "chaos_subject_reference_proof.vcxproj").parent.mkdir(parents=True, exist_ok=True)
                (configure_root / "chaos_subject_reference_proof.vcxproj").write_text("<Project />\n", encoding="utf-8")
                return completed

            def refresh_side_effect(
                repo_root: Path,
                subject_id: str,
                matrix_id: str,
                variant: str,
                *,
                run_id: str | None = None,
            ) -> None:
                self.assertEqual("FixtureSubject", subject_id)
                self.assertEqual("windows-dev-output", matrix_id)
                self.assertEqual("CHECK", variant)
                self.assertEqual("subject-exec", run_id)
                generated_root = (
                    repo_root
                    / "artifacts"
                    / "subjects"
                    / subject_id
                    / "runs"
                    / (run_id or "subject-exec")
                    / "analysis"
                    / "generated"
                    / "generated"
                )
                generated_root.mkdir(parents=True, exist_ok=True)
                (generated_root / "native-reference.generated.cpp").write_text("// generated\n", encoding="utf-8")

            with patch.object(workspace_module, "refresh_subject_generated_root", side_effect=refresh_side_effect) as refresh_mock:
                with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                    with patch.object(workspace_module, "run_process", side_effect=configure_side_effect):
                        result = workspace_module.generate_all_workspaces(
                            repo_root,
                            "windows",
                            {"host": "windows"},
                        )

            self.assertEqual("verification/workspaces/verification-all.manifest.json", result["manifestPath"])
            self.assertTrue((repo_root / "verification" / "workspaces" / "verification-all.manifest.json").is_file())
            refresh_mock.assert_called_once()
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
    def test_generate_all_workspaces_includes_managed_only_subjects_without_native_workspace_generation(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_all_include_managed_only")
        repo_root = self._make_repo_root("all-skip-unsupported")
        self._write_subject_fixture(repo_root)
        self._write_managed_only_subject_fixture(repo_root)
        self._write_core_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")

        try:
            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                (configure_root / "chaos_subject_reference_proof.vcxproj").parent.mkdir(parents=True, exist_ok=True)
                (configure_root / "chaos_subject_reference_proof.vcxproj").write_text("<Project />\n", encoding="utf-8")
                return completed

            with patch.object(workspace_module, "refresh_subject_generated_root", return_value=None):
                with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                    with patch.object(workspace_module, "run_process", side_effect=configure_side_effect):
                        result = workspace_module.generate_all_workspaces(
                            repo_root,
                            "windows",
                            {"host": "windows"},
                        )

            aggregate_manifest = json.loads((repo_root / result["manifestPath"]).read_text(encoding="utf-8"))
            generation_report = json.loads((repo_root / "verification" / "workspaces" / "verification-all.generation.report.json").read_text(encoding="utf-8"))
            self.assertEqual(
                [
                    "verification/workspaces/subjects/FixtureSubject/workspace.manifest.json",
                    "verification/workspaces/subjects/ManagedOnlySubject/workspace.manifest.json",
                ],
                aggregate_manifest["subjectWorkspaceManifests"],
            )
            self.assertEqual(["FixtureSubject", "ManagedOnlySubject"], generation_report["generatedSubjectIds"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
    def test_generate_all_workspaces_discovers_subject_without_legacy_native_reference_source(self) -> None:
        workspace_module = load_module(PROJECT_WORKSPACE_MODULE_PATH, "chaos_project_workspace_all_include_template_proof_host_subject")
        repo_root = self._make_repo_root("all-include-template-proof-host-subject")
        self._write_subject_fixture(repo_root, include_legacy_native_reference=False)
        self._write_core_fixture(repo_root)
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")

        try:
            def configure_side_effect(arguments: list[str], cwd: Path, env: dict[str, str] | None = None):
                del cwd, env
                configure_root = Path(arguments[arguments.index("-B") + 1])
                (configure_root / "chaos_subject_reference_proof.vcxproj").parent.mkdir(parents=True, exist_ok=True)
                (configure_root / "chaos_subject_reference_proof.vcxproj").write_text("<Project />\n", encoding="utf-8")
                return completed

            with patch.object(workspace_module, "refresh_subject_generated_root", return_value=None):
                with patch.object(workspace_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                    with patch.object(workspace_module, "run_process", side_effect=configure_side_effect):
                        result = workspace_module.generate_all_workspaces(
                            repo_root,
                            "windows",
                            {"host": "windows"},
                        )

            aggregate_manifest = json.loads((repo_root / result["manifestPath"]).read_text(encoding="utf-8"))
            generation_report = json.loads((repo_root / "verification" / "workspaces" / "verification-all.generation.report.json").read_text(encoding="utf-8"))
            self.assertEqual(
                ["verification/workspaces/subjects/FixtureSubject/workspace.manifest.json"],
                aggregate_manifest["subjectWorkspaceManifests"],
            )
            self.assertEqual(["FixtureSubject"], generation_report["generatedSubjectIds"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)



