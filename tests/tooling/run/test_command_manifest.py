from __future__ import annotations

import importlib.util
import json
import unittest
from pathlib import Path

from tests.support import load_public_specs_module, select_public_suite_spec, select_subject_record


REPO_ROOT = Path(__file__).resolve().parents[3]
MANIFEST_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "manifest.py"
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
            ["Quick Start", "Build", "Test And Verify", "Environment", "Inspect"],
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
        hidden_command_ids = {command["id"] for command in manifest_module.list_commands(manifest, include_hidden=True)}
        smoke_spec = select_public_suite_spec(
            "chaos_manifest_command_visibility_smoke",
            host_platform="macos",
            family="smoke",
            required_stages=["build", "run"],
        )
        legacy_build_command_id = str(dict(smoke_spec.get("legacy_commands") or {}).get("build") or "")
        legacy_run_command_id = str(dict(smoke_spec.get("legacy_commands") or {}).get("run") or "")

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
        self.assertTrue(
            {
                "build-platform-windows-reference-desktop",
                "build-platform-macos-reference-desktop",
                "test-contract-analysis-schema",
                "test-contract-managed-closure-bundle",
                "test-contract-trace-schema",
            }.issubset(hidden_command_ids)
        )
        self.assertNotIn("verify-roadmap-0-windows", hidden_command_ids)
        self.assertNotIn("verify-roadmap-0-macos", hidden_command_ids)
        self.assertNotIn(legacy_build_command_id, visible_command_ids)
        self.assertNotIn(legacy_run_command_id, visible_command_ids)
        self.assertNotIn("verify-roadmap-0-windows", visible_command_ids)

    def test_manifest_uses_canonical_tests_contracts_paths(self) -> None:
        manifest = json.loads(RUN_MANIFEST_PATH.read_text(encoding="utf-8"))
        commands = {command["id"]: command for command in manifest["commands"]}

        self.assertEqual("tests/contracts/native/abi", commands["build-native-contract-abi"]["source_dir"])
        self.assertEqual("tests/contracts/native/bridge", commands["build-native-contract-bridge"]["source_dir"])
        self.assertEqual(
            "tests/contracts/trace/snapshots/windows-warmup-trace.snapshot.json",
            commands["build-platform-windows-reference-desktop"]["expected_trace_path"],
        )
        self.assertEqual(
            "tests/contracts/trace/snapshots/macos-warmup-trace.snapshot.json",
            commands["build-platform-macos-reference-desktop"]["expected_trace_path"],
        )
        self.assertEqual(
            "tests/contracts/trace/snapshots/windows-warmup-trace.snapshot.json",
            commands["test-trace-compare-windows"]["expected_trace_path"],
        )
        self.assertEqual(
            "tests/contracts/trace/snapshots/macos-warmup-trace.snapshot.json",
            commands["test-trace-compare-macos"]["expected_trace_path"],
        )

    def test_manifest_points_legacy_smoke_commands_at_subject_sources(self) -> None:
        manifest = json.loads(RUN_MANIFEST_PATH.read_text(encoding="utf-8"))
        commands = {command["id"]: command for command in manifest["commands"]}
        public_specs_module = load_public_specs_module("chaos_manifest_legacy_smoke_specs")

        for spec in list(public_specs_module.PUBLIC_TEST_SPECS):
            if str(spec.get("family") or "") != "smoke":
                continue
            legacy_commands = dict(spec.get("legacy_commands") or {})
            expected_path = f"subjects/{spec['suite']}/source/{spec['suite']}.csproj"
            self.assertEqual(expected_path, commands[str(legacy_commands["build"])]["project_path"])
            self.assertEqual(expected_path, commands[str(legacy_commands["run"])]["project_path"])

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
        subject_record = select_subject_record(
            "chaos_manifest_parse_cli_subject",
            source_type="dotnet-project",
            required_host_platforms=["windows-x64"],
        )
        subject_id = str(subject_record["subjectId"])

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


if __name__ == "__main__":
    unittest.main()
