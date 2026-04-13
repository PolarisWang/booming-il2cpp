from __future__ import annotations

import importlib.util
import sys
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
TEST_COMMAND_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "commands" / "test.py"
MANIFEST_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "core" / "manifest.py"
RUN_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "run.py"
RESULT_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "core" / "result.py"
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
    def test_public_suite_resolution_returns_direct_execution_specs(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command")
        smoke_execution = test_module.resolve_public_test_execution_spec(
            "smoke",
            "managed-entry-basic",
            host_platform="macos",
        )

        self.assertEqual(
            "smoke-run",
            smoke_execution["kind"],
        )
        self.assertEqual(
            "SolutionCorePack",
            smoke_execution["targetSubjectId"],
        )
        self.assertEqual(
            "contract-check",
            test_module.resolve_public_test_execution_spec(
                "contract",
                "analysis-schema",
                host_platform="macos",
            )["kind"],
        )
        self.assertEqual(
            "trace-compare",
            test_module.resolve_public_test_execution_spec(
                "contract",
                "trace-compare-macos",
                host_platform="macos",
            )["kind"],
        )
        self.assertEqual(
            "native-contract",
            test_module.resolve_public_test_execution_spec(
                "contract",
                "native-abi",
                host_platform="macos",
            )["kind"],
        )
        self.assertEqual(
            "registry-object",
            test_module.resolve_public_test_execution_spec(
                "workflow",
                "runtime-baseline-macos",
                host_platform="macos",
            )["kind"],
        )
        self.assertEqual(
            "platform-gate",
            test_module.resolve_public_test_execution_spec(
                "gate",
                "linux-x64-packaging",
                host_platform="macos",
            )["kind"],
        )
        self.assertEqual(
            "reference-desktop-gate",
            test_module.resolve_public_test_execution_spec(
                "gate",
                "macos-reference-desktop",
                host_platform="macos",
            )["kind"],
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
        self.assertIn("smoke/managed-entry-basic", item_ids)
        self.assertIn("gate/ios-arm64-packaging", item_ids)
        self.assertIn("gate/linux-x64-packaging", item_ids)
        self.assertIn("gate/macos-reference-desktop", item_ids)
        self.assertIn("workflow/runtime-baseline-macos", item_ids)

        windows_items = test_module.list_public_test_suites(manifest, "windows")
        windows_item_ids = {item["id"] for item in windows_items}
        self.assertIn("gate/android-arm64-smoke", windows_item_ids)
        self.assertIn("gate/windows-reference-desktop", windows_item_ids)

    def test_removed_verify_entrypoint_returns_unknown_command(self) -> None:
        run_module = load_module(RUN_MODULE_PATH, "chaos_run_main_for_removed_verify_unknown")
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
        self.assertEqual({}, result.payload)
        self.assertEqual(["unknown command: verify roadmap-0"], result.errors)
        self.assertEqual("unknown command: verify roadmap-0\n", result.text)


if __name__ == "__main__":
    unittest.main()
