from tests.tooling.run.command_manifest_test_support import *


class CommandManifestStructureTests(CommandManifestTestSupport):
    def test_manifest_groups_match_current_design(self) -> None:
        manifest_module = load_manifest_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        self.assertEqual(
            ["Quick Start", "Project And IDE", "Build", "Test And Verify", "Environment", "Inspect"],
            manifest_module.list_group_titles(manifest),
        )

    def test_project_commands_move_into_project_and_ide_group(self) -> None:
        manifest_module = load_manifest_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        project_command_ids = [
            command["id"]
            for command in manifest_module.list_commands_by_group(manifest, "Project And IDE")
        ]
        build_command_ids = [
            command["id"]
            for command in manifest_module.list_commands_by_group(manifest, "Build")
        ]

        self.assertEqual(
            [
                "generate-project-all",
                "generate-project-subject",
                "generate-project-core",
                "build-project-subject",
                "build-project-core",
            ],
            [command_id for command_id in project_command_ids if command_id.startswith(("generate-project", "build-project"))],
        )
        self.assertIn("deploy-core", build_command_ids)
        self.assertNotIn("generate-project-subject", build_command_ids)
        self.assertNotIn("build-project-core", build_command_ids)

    def test_quick_start_excludes_verify_and_no_package_group_exists(self) -> None:
        manifest_module = load_manifest_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        quick_start_commands = [
            command["id"]
            for command in manifest_module.list_commands_by_group(manifest, "Quick Start")
        ]

        self.assertNotIn("verify-roadmap-0", quick_start_commands)
        self.assertNotIn("Package And Deploy", manifest_module.list_group_titles(manifest))

    def test_every_command_has_required_metadata(self) -> None:
        manifest_module = load_manifest_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        for command in manifest_module.list_commands(manifest):
            self.assertIn("id", command)
            self.assertIn("group", command)
            self.assertIn("handler", command)
            self.assertIn("platforms", command)
            self.assertTrue(command["platforms"])

    def test_manifest_registers_unified_public_test_capabilities(self) -> None:
        manifest_module = load_manifest_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)
        visible_command_ids = {command["id"] for command in manifest_module.list_commands(manifest)}
        all_command_ids = {command["id"] for command in manifest_module.list_commands(manifest, include_hidden=True)}

        self.assertTrue(
            {
                "build-all",
                "build-native-contract-abi",
                "build-native-contract-bridge",
                "build-preset-windows-x64-reference",
                "build-platform-android-arm64-smoke",
                "build-platform-linux-x64-packaging",
                "test-suite",
                "test-subject",
                "test-module",
                "test-system",
                "test-inventory",
                "test-registry-list",
                "test-registry-refresh",
                "test-registry-check-consistency",
                "test-family-suite",
                "test-family-all",
                "test-all",
                "test-list",
                "test-watch",
                "test-summary",
            }.issubset(visible_command_ids)
        )
        self.assertNotIn("test-pipeline", visible_command_ids)
        self.assertNotIn("build-platform-windows-reference-desktop", all_command_ids)
        self.assertNotIn("build-platform-macos-reference-desktop", all_command_ids)
        self.assertNotIn("test-contract-analysis-schema", all_command_ids)
        self.assertNotIn("test-contract-managed-closure-bundle", all_command_ids)
        self.assertNotIn("test-contract-trace-schema", all_command_ids)
        self.assertNotIn("test-workflow-runtime-baseline-windows", all_command_ids)
        self.assertNotIn("test-workflow-runtime-baseline-macos", all_command_ids)
        self.assertNotIn("test-workflow-roadmap-0-windows", all_command_ids)
        self.assertNotIn("test-workflow-roadmap-0-macos", all_command_ids)
        self.assertNotIn("verify-roadmap-0-windows", all_command_ids)
        self.assertNotIn("verify-roadmap-0-macos", all_command_ids)
        self.assertNotIn("build-smoke-helloworld", all_command_ids)
        self.assertNotIn("test-smoke-helloworld", all_command_ids)
        self.assertNotIn("verify-roadmap-0-windows", visible_command_ids)
        self.assertTrue(
            {
                "generate-project-all",
                "generate-project-subject",
                "generate-project-core",
                "build-project-subject",
                "build-project-core",
                "deploy-core",
            }.issubset(visible_command_ids)
        )

    def test_public_specs_route_trace_checks_through_subject_entries(self) -> None:
        public_specs_module = load_public_specs_module("chaos_manifest_legacy_smoke_specs")
        windows_trace = public_specs_module.find_public_test_suite_spec("contract", "trace-compare-windows")
        macos_trace = public_specs_module.find_public_test_suite_spec("contract", "trace-compare-macos")
        windows_gate = public_specs_module.find_public_test_suite_spec("gate", "windows-reference-desktop")
        macos_gate = public_specs_module.find_public_test_suite_spec("gate", "macos-reference-desktop")

        assert windows_trace is not None
        assert macos_trace is not None
        assert windows_gate is not None
        assert macos_gate is not None

        self.assertEqual("SolutionCorePack", windows_trace["execution"]["subject_id"])
        self.assertEqual("correctness.platform", windows_trace["execution"]["goal_id"])
        self.assertEqual("windows-managed-trace", windows_trace["execution"]["matrix_id"])
        self.assertEqual("SolutionCorePack", macos_trace["execution"]["subject_id"])
        self.assertEqual("correctness.platform", macos_trace["execution"]["goal_id"])
        self.assertEqual("macos-managed-trace", macos_trace["execution"]["matrix_id"])
        self.assertEqual("SolutionCorePack", windows_gate["execution"]["subject_id"])
        self.assertEqual("correctness.platform", windows_gate["execution"]["goal_id"])
        self.assertEqual("windows-managed-trace", windows_gate["execution"]["matrix_id"])
        self.assertEqual("SolutionCorePack", macos_gate["execution"]["subject_id"])
        self.assertEqual("correctness.platform", macos_gate["execution"]["goal_id"])
        self.assertEqual("macos-managed-trace", macos_gate["execution"]["matrix_id"])

    def test_public_smoke_specs_point_at_solution_core_slices(self) -> None:
        public_specs_module = load_public_specs_module("chaos_manifest_public_smoke_specs")

        for spec in list(public_specs_module.PUBLIC_TEST_SPECS):
            if str(spec.get("family") or "") != "smoke":
                continue
            execution = dict(spec.get("execution") or {})
            suite_name = str(spec["suite"])
            project_name = str(execution["project_path"]).replace("\\", "/").split("/")[-1].removesuffix(".csproj")
            expected_path = f"tests/fixtures/public-smoke/{project_name}/{project_name}.csproj"
            expected_dll_path = f"tests/fixtures/public-smoke/{project_name}/bin/Release/net8.0/{project_name}.dll"
            self.assertEqual(expected_path, str(execution["project_path"]).replace("\\", "/"))
            self.assertEqual(expected_dll_path, str(execution["dll_path"]).replace("\\", "/"))
            self.assertEqual("SolutionCorePack", str(execution["targetSubjectId"]))
            self.assertEqual(f"smoke/{suite_name}", str(spec["id"]))
