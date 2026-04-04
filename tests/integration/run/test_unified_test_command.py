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
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "booming_run_test_command")

        self.assertEqual(
            "build-smoke-helloworld",
            test_module.resolve_legacy_test_command_id("smoke", "HelloWorld", stage="build", host_platform="macos"),
        )
        self.assertEqual(
            "test-smoke-helloworld",
            test_module.resolve_legacy_test_command_id("smoke", "HelloWorld", stage="all", host_platform="macos"),
        )
        self.assertEqual(
            "verify-roadmap-0-macos",
            test_module.resolve_legacy_test_command_id("workflow", "roadmap-0-macos", stage="all", host_platform="macos"),
        )

    def test_public_test_list_surfaces_unified_suite_ids(self) -> None:
        manifest_module = load_module(MANIFEST_MODULE_PATH, "booming_run_manifest_for_public_test")
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "booming_run_test_command_for_public_list")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        items = test_module.list_public_test_suites(manifest, "macos")
        item_ids = {item["id"] for item in items}

        self.assertIn("smoke/HelloWorld", item_ids)
        self.assertIn("workflow/roadmap-0-macos", item_ids)
        self.assertNotIn("test-smoke-helloworld", item_ids)
        self.assertNotIn("verify-roadmap-0-macos", item_ids)

    def test_legacy_test_commands_gain_migration_guidance(self) -> None:
        run_module = load_module(RUN_MODULE_PATH, "booming_run_main_for_legacy_migration")
        result_module = load_module(RESULT_MODULE_PATH, "booming_run_result_for_legacy_migration")
        result = result_module.CommandResult.success(
            command="verify roadmap-0 --host macos",
            host_platform="macos",
            target="macos",
            payload={"artifacts": ["artifacts/verify-roadmap-0/macos"]},
            text="verify completed\n",
        )

        wrapped = run_module.add_legacy_test_migration_guidance(
            {
                "id": "verify-roadmap-0-macos",
                "public": False,
                "replacement_syntax": "test workflow roadmap-0-macos",
            },
            result,
        )

        self.assertIn("Deprecated test command", wrapped.text)
        self.assertEqual("test workflow roadmap-0-macos", wrapped.payload["migration"]["replacementSyntax"])


if __name__ == "__main__":
    unittest.main()
