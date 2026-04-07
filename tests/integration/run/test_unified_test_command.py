from __future__ import annotations

import importlib.util
import sys
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
TEST_COMMAND_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "commands" / "test.py"
MANIFEST_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "manifest.py"
RUN_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "run.py"
RESULT_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "result.py"
RUN_MANIFEST_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "run_manifest.json"


def load_module(path: Path, module_name: str):
    if not path.is_file():
        raise FileNotFoundError(f"module missing: {path}")

    spec = importlib.util.spec_from_file_location(module_name, path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load module: {path}")

    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


class UnifiedTestCommandTests(unittest.TestCase):
    def test_public_suite_resolution_maps_to_legacy_commands(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command")

        self.assertEqual(
            "build-smoke-helloworld",
            test_module.resolve_legacy_test_command_id("smoke", "HelloWorld", stage="build", host_platform="macos"),
        )
        self.assertEqual(
            "test-smoke-helloworld",
            test_module.resolve_legacy_test_command_id("smoke", "HelloWorld", stage="all", host_platform="macos"),
        )
        self.assertEqual(
            "test-contract-analysis-schema",
            test_module.resolve_legacy_test_command_id("contract", "analysis-schema", stage="all", host_platform="macos"),
        )
        self.assertEqual(
            "test-contract-trace-schema",
            test_module.resolve_legacy_test_command_id("contract", "trace-schema", stage="all", host_platform="macos"),
        )
        self.assertEqual(
            "build-native-contract-abi",
            test_module.resolve_legacy_test_command_id("contract", "native-abi", stage="all", host_platform="macos"),
        )
        self.assertEqual(
            "build-native-contract-bridge",
            test_module.resolve_legacy_test_command_id("contract", "native-bridge", stage="all", host_platform="macos"),
        )
        self.assertEqual(
            "test-workflow-roadmap-0-macos",
            test_module.resolve_legacy_test_command_id("workflow", "roadmap-0-macos", stage="all", host_platform="macos"),
        )
        self.assertEqual(
            "test-workflow-roadmap-0-windows",
            test_module.resolve_legacy_test_command_id("workflow", "roadmap-0-windows", stage="all", host_platform="windows"),
        )
        self.assertEqual(
            "build-platform-linux-x64-packaging",
            test_module.resolve_legacy_test_command_id("gate", "linux-x64-packaging", stage="all", host_platform="macos"),
        )
        self.assertEqual(
            "build-platform-ios-arm64-packaging",
            test_module.resolve_legacy_test_command_id("gate", "ios-arm64-packaging", stage="all", host_platform="macos"),
        )
        self.assertEqual(
            "build-platform-macos-reference-desktop",
            test_module.resolve_legacy_test_command_id("gate", "macos-reference-desktop", stage="all", host_platform="macos"),
        )
        self.assertEqual(
            "build-platform-android-arm64-smoke",
            test_module.resolve_legacy_test_command_id("gate", "android-arm64-smoke", stage="all", host_platform="windows"),
        )
        self.assertEqual(
            "build-platform-windows-reference-desktop",
            test_module.resolve_legacy_test_command_id("gate", "windows-reference-desktop", stage="all", host_platform="windows"),
        )

    def test_public_test_list_surfaces_unified_suite_ids(self) -> None:
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_for_public_test")
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_for_public_list")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        items = test_module.list_public_test_suites(manifest, "macos")
        item_ids = {item["id"] for item in items}

        self.assertIn("contract/analysis-schema", item_ids)
        self.assertIn("contract/trace-schema", item_ids)
        self.assertIn("contract/native-abi", item_ids)
        self.assertIn("contract/native-bridge", item_ids)
        self.assertIn("smoke/HelloWorld", item_ids)
        self.assertIn("gate/ios-arm64-packaging", item_ids)
        self.assertIn("gate/linux-x64-packaging", item_ids)
        self.assertIn("gate/macos-reference-desktop", item_ids)
        self.assertIn("workflow/roadmap-0-macos", item_ids)
        self.assertNotIn("test-smoke-helloworld", item_ids)
        self.assertNotIn("test-workflow-roadmap-0-macos", item_ids)

        windows_items = test_module.list_public_test_suites(manifest, "windows")
        windows_item_ids = {item["id"] for item in windows_items}
        self.assertIn("gate/android-arm64-smoke", windows_item_ids)
        self.assertIn("gate/windows-reference-desktop", windows_item_ids)

    def test_legacy_test_commands_gain_migration_guidance(self) -> None:
        run_module = load_module(RUN_MODULE_PATH, "chaos_run_main_for_legacy_migration")
        result_module = load_module(RESULT_MODULE_PATH, "chaos_run_result_for_legacy_migration")
        result = result_module.CommandResult.success(
            command="build smoke HelloWorld",
            host_platform="macos",
            target="HelloWorld",
            payload={"artifacts": ["artifacts/smoke/bin/HelloWorld/Release/net8.0/HelloWorld.dll"]},
            text="build completed\n",
        )

        wrapped = run_module.add_legacy_test_migration_guidance(
            {
                "id": "build-smoke-helloworld",
                "public": False,
                "replacement_syntax": "test smoke HelloWorld --stage build",
            },
            result,
        )

        self.assertIn("Deprecated test command", wrapped.text)
        self.assertEqual("test smoke HelloWorld --stage build", wrapped.payload["migration"]["replacementSyntax"])

    def test_removed_verify_entrypoint_returns_migration_guidance(self) -> None:
        run_module = load_module(RUN_MODULE_PATH, "chaos_run_main_for_removed_verify_migration")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_for_removed_verify_migration")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        result = run_module.execute_command(
            None,
            "verify roadmap-0",
            None,
            "macos",
            manifest,
            REPO_ROOT,
            {"host": "macos"},
        )

        self.assertEqual("error", result.status)
        self.assertIn("Removed command", result.text or "")
        self.assertIn("run test workflow roadmap-0-macos", result.text or "")
        self.assertEqual("verify roadmap-0", result.payload["migration"]["removedCommand"])
        self.assertEqual("test workflow roadmap-0-macos", result.payload["migration"]["replacementSyntax"])


if __name__ == "__main__":
    unittest.main()
