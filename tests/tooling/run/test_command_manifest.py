from __future__ import annotations

import importlib.util
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
MANIFEST_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "manifest.py"
RUN_MANIFEST_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "run_manifest.json"


def load_manifest_module():
    if not MANIFEST_MODULE_PATH.is_file():
        raise FileNotFoundError(f"manifest module missing: {MANIFEST_MODULE_PATH}")

    spec = importlib.util.spec_from_file_location("booming_run_manifest", MANIFEST_MODULE_PATH)
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
        command_ids = {command["id"] for command in manifest_module.list_commands(manifest)}

        self.assertTrue(
            {
                "build-all",
                "build-native-contract-abi",
                "build-native-contract-bridge",
                "build-preset-windows-x64-reference",
                "build-platform-android-arm64-smoke",
                "build-platform-linux-x64-packaging",
                "test-family-suite",
                "test-family-all",
                "test-all",
                "test-list",
                "test-watch",
                "test-summary",
            }.issubset(command_ids)
        )
        self.assertNotIn("build-smoke-helloworld", command_ids)
        self.assertNotIn("test-smoke-helloworld", command_ids)
        self.assertNotIn("verify-roadmap-0-windows", command_ids)

    def test_parse_cli_supports_dynamic_unified_test_commands(self) -> None:
        manifest_module = load_manifest_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        suite = manifest_module.parse_cli(["test", "smoke", "HelloWorld"], False, manifest, "macos")
        self.assertEqual("test-family-suite", suite["command"]["id"])
        self.assertEqual("smoke/HelloWorld", suite["target"])
        self.assertEqual("smoke", suite["options"]["family"])
        self.assertEqual("HelloWorld", suite["options"]["suite"])

        list_family = manifest_module.parse_cli(["test", "list", "smoke"], False, manifest, "macos")
        self.assertEqual("test-list", list_family["command"]["id"])
        self.assertEqual("smoke", list_family["target"])
        self.assertEqual("smoke", list_family["options"]["family"])

        watch = manifest_module.parse_cli(["test", "watch"], False, manifest, "macos")
        self.assertEqual("test-watch", watch["command"]["id"])

        summary = manifest_module.parse_cli(["test", "summary"], False, manifest, "macos")
        self.assertEqual("test-summary", summary["command"]["id"])


if __name__ == "__main__":
    unittest.main()
