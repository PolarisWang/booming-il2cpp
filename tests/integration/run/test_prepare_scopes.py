from __future__ import annotations

import importlib.util
import unittest
from pathlib import Path
from unittest.mock import patch


REPO_ROOT = Path(__file__).resolve().parents[3]
PREPARE_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "commands" / "prepare.py"
RUN_MANIFEST_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "run_manifest.json"


def load_prepare_module():
    if not PREPARE_MODULE_PATH.is_file():
        raise FileNotFoundError(f"prepare module missing: {PREPARE_MODULE_PATH}")

    spec = importlib.util.spec_from_file_location("chaos_run_prepare", PREPARE_MODULE_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load prepare module: {PREPARE_MODULE_PATH}")

    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


class PrepareScopeTests(unittest.TestCase):
    def test_prepare_scope_resolution_covers_global_and_targeted_forms(self) -> None:
        prepare_module = load_prepare_module()

        self.assertEqual("global", prepare_module.resolve_prepare_scope("prepare"))
        self.assertEqual("smoke", prepare_module.resolve_prepare_scope("prepare-smoke"))
        self.assertEqual(
            "workflow-runtime-baseline-windows",
            prepare_module.resolve_prepare_scope("prepare-workflow-runtime-baseline-windows"),
        )
        self.assertEqual(
            "workflow-runtime-baseline-macos",
            prepare_module.resolve_prepare_scope("prepare-workflow-runtime-baseline-macos"),
        )
        with self.assertRaises(KeyError):
            prepare_module.resolve_prepare_scope("prepare-verify-roadmap-0-windows")
        with self.assertRaises(KeyError):
            prepare_module.resolve_prepare_scope("prepare-verify-roadmap-0-macos")

    def test_clean_scope_paths_only_cover_managed_outputs(self) -> None:
        prepare_module = load_prepare_module()
        clean_paths = prepare_module.resolve_clean_paths(REPO_ROOT, "workflow-runtime-baseline-windows")

        self.assertIn(
            REPO_ROOT / "artifacts" / "verify-runtime-baseline" / "windows",
            clean_paths,
        )
        self.assertIn(
            REPO_ROOT / "artifacts" / "run" / "prepare" / "workflow-runtime-baseline-windows.json",
            clean_paths,
        )
        self.assertNotIn(REPO_ROOT / "contracts", clean_paths)
        self.assertNotIn(REPO_ROOT / "third_party", clean_paths)

    def test_prepare_plan_routes_smoke_scope_through_unified_test_build_stage(self) -> None:
        prepare_module = load_prepare_module()

        self.assertEqual(
            [["test", "smoke", "all", "--stage", "build"]],
            prepare_module._prepare_plan("smoke", "macos"),
        )

    def test_prepare_global_scope_avoids_legacy_verify_commands(self) -> None:
        prepare_module = load_prepare_module()

        plan = prepare_module._prepare_plan("global", "macos")

        self.assertIn(["test", "smoke", "all", "--stage", "build"], plan)
        self.assertNotIn(["verify", "roadmap-0", "--host", "macos"], plan)

    def test_prepare_handle_dispatches_unified_test_steps(self) -> None:
        prepare_module = load_prepare_module()
        parsed = {
            "command": {"id": "test-family-all", "handler": "test.dispatch"},
            "command_text": "test smoke all --stage build",
            "target": "smoke",
            "options": {"family": "smoke", "stage": "build"},
            "json": False,
        }
        result = prepare_module.CommandResult.success(
            command="test smoke all --stage build",
            host_platform="macos",
            target="smoke",
            text="prepared smoke build\n",
        )

        with patch.object(prepare_module, "_prepare_plan", return_value=[["test", "smoke", "all", "--stage", "build"]]):
            with patch.object(prepare_module.manifest_module, "parse_cli", return_value=parsed):
                with patch.object(prepare_module.test_commands, "handle", return_value=result) as test_handle:
                    with patch.object(prepare_module, "write_json"):
                        prepare_result = prepare_module.handle(
                            {"id": "prepare-smoke"},
                            REPO_ROOT,
                            "macos",
                            "prepare smoke",
                            {},
                        )

        self.assertEqual("ok", prepare_result.status)
        self.assertEqual(["test smoke all --stage build"], prepare_result.payload["preparedCommands"])
        test_handle.assert_called_once()


if __name__ == "__main__":
    unittest.main()
