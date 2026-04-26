from __future__ import annotations

import importlib.util
import unittest
from pathlib import Path

from tests.support import select_public_suite_spec


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
                "build-native-contract-abi",
                "build-native-contract-bridge",
                "build-preset-windows-x64-reference",
                "build-platform-android-arm64-smoke",
                "build-platform-linux-x64-packaging",
                "test-suite",
                "test-module",
                "test-system",
                "test-engineering-validation",
                "test-engineering-workload",
                "test-declared-unit-test",
                "test-declared-benchmark",
                "test-registry-list",
                "test-registry-refresh",
                "test-registry-check-consistency",
                "test-family-suite",
                "test-family-all",
                "test-all",
                "test-list",
                "verify-verification-v1",
            }.issubset(visible_command_ids)
        )
        self.assertNotIn("test-pipeline", visible_command_ids)
        self.assertNotIn("build-platform-windows-reference-desktop", all_command_ids)
        self.assertNotIn("build-platform-macos-reference-desktop", all_command_ids)
        self.assertNotIn("test-workflow-runtime-baseline-windows", all_command_ids)
        self.assertNotIn("test-workflow-runtime-baseline-macos", all_command_ids)
        self.assertNotIn("test-workflow-roadmap-0-windows", all_command_ids)
        self.assertNotIn("test-workflow-roadmap-0-macos", all_command_ids)
        self.assertNotIn("verify-roadmap-0-windows", all_command_ids)
        self.assertNotIn("verify-roadmap-0-macos", all_command_ids)
        self.assertNotIn("build-smoke-helloworld", all_command_ids)
        self.assertNotIn("test-smoke-helloworld", all_command_ids)
        self.assertNotIn("build-smoke-helloworld", visible_command_ids)
        self.assertNotIn("test-smoke-helloworld", visible_command_ids)
        self.assertNotIn("verify-roadmap-0-windows", visible_command_ids)

    def test_run_manifest_registers_foundation_dll_command_family(self) -> None:
        manifest_module = load_manifest_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)
        all_command_ids = {command["id"] for command in manifest_module.list_commands(manifest, include_hidden=True)}

        self.assertTrue(
            {
                "foundation-dll-derive",
                "foundation-dll-analyze-gaps",
                "foundation-dll-check-family",
                "foundation-dll-promote",
                "foundation-dll-onboard",
                "foundation-dll-refresh",
                "foundation-dll-verify-consistency",
                "foundation-dll-full",
            }.issubset(all_command_ids)
        )

    def test_parse_cli_supports_dynamic_unified_test_commands(self) -> None:
        manifest_module = load_manifest_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)
        fixture_engineering_validation_id = "engineering-validation/FixtureSubject/project-graph"
        fixture_engineering_workload_id = "engineering-workload/FixtureSubject/codegen"
        fixture_declared_unit_id = "declared-unit-test/FixtureSubject::FeaturePack::FeaturePack.ProofEntry::Run()"
        fixture_declared_benchmark_id = (
            "declared-benchmark/FixtureSubject::PerformancePack::PerformancePack.BenchmarkEntry::RunWorkload()"
        )
        smoke_spec = select_public_suite_spec(
            "chaos_integration_manifest_parse_cli_smoke",
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

        engineering_validation_case = manifest_module.parse_cli(
            ["test", "engineering-validation", "--id", fixture_engineering_validation_id],
            False,
            manifest,
            "windows",
        )
        self.assertEqual("test-engineering-validation", engineering_validation_case["command"]["id"])
        self.assertEqual(fixture_engineering_validation_id, engineering_validation_case["target"])

        engineering_workload_case = manifest_module.parse_cli(
            ["test", "engineering-workload", "--id", fixture_engineering_workload_id],
            False,
            manifest,
            "windows",
        )
        self.assertEqual("test-engineering-workload", engineering_workload_case["command"]["id"])
        self.assertEqual(fixture_engineering_workload_id, engineering_workload_case["target"])

        declared_unit_case = manifest_module.parse_cli(
            ["test", "declared-unit-test", "--id", fixture_declared_unit_id],
            False,
            manifest,
            "windows",
        )
        self.assertEqual("test-declared-unit-test", declared_unit_case["command"]["id"])
        self.assertEqual(fixture_declared_unit_id, declared_unit_case["target"])

        declared_benchmark_case = manifest_module.parse_cli(
            ["test", "declared-benchmark", "--id", fixture_declared_benchmark_id],
            False,
            manifest,
            "windows",
        )
        self.assertEqual("test-declared-benchmark", declared_benchmark_case["command"]["id"])
        self.assertEqual(fixture_declared_benchmark_id, declared_benchmark_case["target"])

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

        verification_case = manifest_module.parse_cli(
            ["verify", "verification-v1", "--output", "verification/projections/testing-inventory"],
            False,
            manifest,
            "windows",
        )
        self.assertEqual("verify-verification-v1", verification_case["command"]["id"])
        self.assertEqual("verification-v1", verification_case["target"])
        self.assertEqual("verification/projections/testing-inventory", verification_case["options"]["output"])

        list_family = manifest_module.parse_cli(["test", "list", "smoke"], False, manifest, "macos")
        self.assertEqual("test-list", list_family["command"]["id"])
        self.assertEqual("smoke", list_family["target"])
        self.assertEqual("smoke", list_family["options"]["family"])

        removed_pipeline = manifest_module.parse_cli(
            ["test", "pipeline", "--pipeline", "completion-runtime-core"],
            False,
            manifest,
            "macos",
        )
        self.assertIsNone(removed_pipeline["command"])
        self.assertEqual("test pipeline", removed_pipeline["command_text"])

    def test_parse_cli_rejects_extra_positional_arguments_for_fixed_commands(self) -> None:
        manifest_module = load_manifest_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        cases = [
            ["foundation-dll", "derive", "junk"],
            ["foundation-dll", "refresh", "junk"],
            ["verify", "verification-v1", "junk"],
        ]

        for argv in cases:
            parsed = manifest_module.parse_cli(argv, False, manifest, "windows")
            self.assertIsNone(parsed["command"], argv)

    def test_parse_cli_rejects_removed_verify_entrypoint(self) -> None:
        manifest_module = load_manifest_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        parsed = manifest_module.parse_cli(["verify", "roadmap-0", "--host", "macos"], False, manifest, "macos")

        self.assertIsNone(parsed["command"])
        self.assertEqual("verify roadmap-0", parsed["command_text"])


if __name__ == "__main__":
    unittest.main()
