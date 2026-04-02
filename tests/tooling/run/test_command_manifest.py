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

    def test_manifest_registers_current_real_build_test_and_verify_capabilities(self) -> None:
        manifest_module = load_manifest_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)
        command_ids = {command["id"] for command in manifest_module.list_commands(manifest)}

        self.assertTrue(
            {
                "build-native-contract-abi",
                "build-native-contract-bridge",
                "build-smoke-helloworld",
                "build-smoke-genericecho",
                "build-smoke-reflectionlite",
                "build-smoke-pinvokelite",
                "build-smoke-hostembeddinglite",
                "build-preset-windows-x64-reference",
                "build-platform-android-arm64-smoke",
                "build-platform-linux-x64-packaging",
                "test-smoke-helloworld",
                "test-trace-compare-windows",
                "verify-roadmap-0-windows",
            }.issubset(command_ids)
        )


if __name__ == "__main__":
    unittest.main()
