from __future__ import annotations

import importlib.util
import json
import unittest
from pathlib import Path

from tests.support import load_public_specs_module, select_public_suite_spec


REPO_ROOT = Path(__file__).resolve().parents[3]
MANIFEST_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "core" / "manifest.py"
RUN_MANIFEST_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "run_manifest.json"


def load_manifest_module():
    if not MANIFEST_MODULE_PATH.is_file():
        raise FileNotFoundError(f"manifest module missing: {MANIFEST_MODULE_PATH}")

    spec = importlib.util.spec_from_file_location("chaos_run_manifest", MANIFEST_MODULE_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load manifest module: {MANIFEST_MODULE_PATH}")

    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


class CommandManifestTests(unittest.TestCase):
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
                "test-pipeline",
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

    def test_parse_cli_supports_dynamic_unified_test_commands(self) -> None:
        manifest_module = load_manifest_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)
        smoke_spec = select_public_suite_spec(
            "chaos_manifest_parse_cli_smoke",
            host_platform="macos",
            family="smoke",
            required_stages=["build", "run"],
        )
        family = str(smoke_spec["family"])
        suite_name = str(smoke_spec["suite"])
        suite_id = str(smoke_spec["id"])
        subject_id = "FixtureSubject"

        suite = manifest_module.parse_cli(["test", family, suite_name], False, manifest, "macos")
        self.assertEqual("test-family-suite", suite["command"]["id"])
        self.assertEqual(suite_id, suite["target"])
        self.assertEqual(family, suite["options"]["family"])
        self.assertEqual(suite_name, suite["options"]["suite"])

        explicit_suite = manifest_module.parse_cli(
            ["test", "suite", "--family", family, "--suite", suite_name],
            False,
            manifest,
            "macos",
        )
        self.assertEqual("test-suite", explicit_suite["command"]["id"])
        self.assertEqual(suite_id, explicit_suite["target"])
        self.assertEqual(family, explicit_suite["options"]["family"])
        self.assertEqual(suite_name, explicit_suite["options"]["suite"])

        explicit_suite_id = manifest_module.parse_cli(
            ["test", "suite", "--id", suite_id],
            False,
            manifest,
            "macos",
        )
        self.assertEqual("test-suite", explicit_suite_id["command"]["id"])
        self.assertEqual(suite_id, explicit_suite_id["target"])
        self.assertEqual(suite_id, explicit_suite_id["options"]["id"])

        subject_case = manifest_module.parse_cli(
            ["test", "subject", "--id", f"subject/{subject_id}"],
            False,
            manifest,
            "windows",
        )
        self.assertEqual("test-subject", subject_case["command"]["id"])
        self.assertEqual(f"subject/{subject_id}", subject_case["target"])
        self.assertEqual(f"subject/{subject_id}", subject_case["options"]["id"])

        managed_closure_contract = manifest_module.parse_cli(
            ["test", "contract", "managed-closure-bundle"],
            False,
            manifest,
            "macos",
        )
        self.assertEqual("test-family-suite", managed_closure_contract["command"]["id"])
        self.assertEqual("contract/managed-closure-bundle", managed_closure_contract["target"])
        self.assertEqual("contract", managed_closure_contract["options"]["family"])
        self.assertEqual("managed-closure-bundle", managed_closure_contract["options"]["suite"])

        module_case = manifest_module.parse_cli(
            ["test", "module", "--module", "managed-smoke", "--profile", "basic"],
            False,
            manifest,
            "macos",
        )
        self.assertEqual("test-module", module_case["command"]["id"])
        self.assertEqual("module/managed-smoke/basic", module_case["target"])
        self.assertEqual("managed-smoke", module_case["options"]["module"])
        self.assertEqual("basic", module_case["options"]["profile"])

        system_case = manifest_module.parse_cli(
            ["test", "system", "--id", "system/hosted-runtime-smoke"],
            False,
            manifest,
            "macos",
        )
        self.assertEqual("test-system", system_case["command"]["id"])
        self.assertEqual("system/hosted-runtime-smoke", system_case["target"])

        pipeline_case = manifest_module.parse_cli(
            ["test", "pipeline", "--pipeline", "completion-runtime-core"],
            False,
            manifest,
            "macos",
        )
        self.assertEqual("test-pipeline", pipeline_case["command"]["id"])
        self.assertEqual("pipeline/completion-runtime-core", pipeline_case["target"])
        self.assertEqual("completion-runtime-core", pipeline_case["options"]["pipeline"])

        registry_refresh = manifest_module.parse_cli(["test", "registry", "refresh"], False, manifest, "macos")
        self.assertEqual("test-registry-refresh", registry_refresh["command"]["id"])

        registry_list = manifest_module.parse_cli(["test", "registry", "list"], False, manifest, "macos")
        self.assertEqual("test-registry-list", registry_list["command"]["id"])

        registry_check = manifest_module.parse_cli(
            ["test", "registry", "check-consistency"],
            False,
            manifest,
            "macos",
        )
        self.assertEqual("test-registry-check-consistency", registry_check["command"]["id"])

        list_family = manifest_module.parse_cli(["test", "list", "smoke"], False, manifest, "macos")
        self.assertEqual("test-list", list_family["command"]["id"])
        self.assertEqual("smoke", list_family["target"])
        self.assertEqual("smoke", list_family["options"]["family"])

        watch = manifest_module.parse_cli(["test", "watch"], False, manifest, "macos")
        self.assertEqual("test-watch", watch["command"]["id"])

        summary = manifest_module.parse_cli(["test", "summary"], False, manifest, "macos")
        self.assertEqual("test-summary", summary["command"]["id"])

    def test_parse_cli_rejects_removed_verify_entrypoint(self) -> None:
        manifest_module = load_manifest_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        parsed = manifest_module.parse_cli(["verify", "roadmap-0", "--host", "macos"], False, manifest, "macos")

        self.assertIsNone(parsed["command"])
        self.assertEqual("verify roadmap-0", parsed["command_text"])

    def test_parse_cli_supports_runtime_baseline_prepare_and_workflow_commands(self) -> None:
        manifest_module = load_manifest_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        prepare_runtime_baseline = manifest_module.parse_cli(
            ["prepare", "workflow", "runtime-baseline", "--host", "macos"],
            False,
            manifest,
            "macos",
        )
        self.assertEqual("prepare-workflow-runtime-baseline-macos", prepare_runtime_baseline["command"]["id"])
        self.assertEqual("macos", prepare_runtime_baseline["options"]["host"])

        prepare_android_host = manifest_module.parse_cli(
            ["prepare", "android-host"],
            False,
            manifest,
            "windows",
        )
        self.assertEqual("prepare-android-host", prepare_android_host["command"]["id"])

        workflow_runtime_baseline = manifest_module.parse_cli(
            ["test", "workflow", "runtime-baseline-macos"],
            False,
            manifest,
            "macos",
        )
        self.assertEqual("test-family-suite", workflow_runtime_baseline["command"]["id"])
        self.assertEqual("workflow/runtime-baseline-macos", workflow_runtime_baseline["target"])

    def test_parse_cli_supports_project_and_deploy_commands(self) -> None:
        manifest_module = load_manifest_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)
        subject_id = "FixtureSubject"

        generate_all = manifest_module.parse_cli(
            ["generate", "project", "all", "--host", "windows", "--refresh-generated"],
            False,
            manifest,
            "windows",
        )
        self.assertEqual("generate-project-all", generate_all["command"]["id"])
        self.assertEqual("windows", generate_all["options"]["host"])
        self.assertTrue(generate_all["options"]["refresh-generated"])

        generate_subject = manifest_module.parse_cli(
            [
                "generate",
                "project",
                "subject",
                "--id",
                f"subject/{subject_id}",
                "--open-native-target",
                "generated",
            ],
            False,
            manifest,
            "windows",
        )
        self.assertEqual("generate-project-subject", generate_subject["command"]["id"])
        self.assertEqual(f"subject/{subject_id}", generate_subject["options"]["id"])
        self.assertEqual("generated", generate_subject["options"]["open-native-target"])

        generate_core = manifest_module.parse_cli(
            ["generate", "project", "core", "--host", "windows", "--all-targets"],
            False,
            manifest,
            "windows",
        )
        self.assertEqual("generate-project-core", generate_core["command"]["id"])
        self.assertEqual("windows", generate_core["options"]["host"])
        self.assertTrue(generate_core["options"]["all-targets"])

        build_subject = manifest_module.parse_cli(
            [
                "build",
                "project",
                "subject",
                "--id",
                f"subject/{subject_id}",
                "--matrix",
                "windows-managed-trace",
                "--native-target",
                "proof",
            ],
            False,
            manifest,
            "windows",
        )
        self.assertEqual("build-project-subject", build_subject["command"]["id"])
        self.assertEqual(f"subject/{subject_id}", build_subject["options"]["id"])
        self.assertEqual("windows-managed-trace", build_subject["options"]["matrix"])
        self.assertEqual("proof", build_subject["options"]["native-target"])

        build_core = manifest_module.parse_cli(
            ["build", "project", "core", "--host", "windows", "--target", "windows-x64-reference"],
            False,
            manifest,
            "windows",
        )
        self.assertEqual("build-project-core", build_core["command"]["id"])
        self.assertEqual("windows-x64-reference", build_core["options"]["target"])

        deploy_core = manifest_module.parse_cli(
            ["deploy", "core", "--host", "windows", "--all-targets"],
            False,
            manifest,
            "windows",
        )
        self.assertEqual("deploy-core", deploy_core["command"]["id"])
        self.assertTrue(deploy_core["options"]["all-targets"])


if __name__ == "__main__":
    unittest.main()
