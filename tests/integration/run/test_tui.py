from __future__ import annotations

import importlib.util
import sys
import unittest
from pathlib import Path

from tests.support import select_public_suite_spec

from .test_command_manifest import RUN_MANIFEST_PATH, load_manifest_module


REPO_ROOT = Path(__file__).resolve().parents[3]
TUI_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "tui.py"


def load_tui_module():
    if not TUI_MODULE_PATH.is_file():
        raise FileNotFoundError(f"tui module missing: {TUI_MODULE_PATH}")

    spec = importlib.util.spec_from_file_location("chaos_run_tui", TUI_MODULE_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load tui module: {TUI_MODULE_PATH}")

    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


@unittest.skip("legacy assertions superseded by unified test menu coverage")
class LegacyTuiTests(unittest.TestCase):
    def test_build_menu_entries_only_includes_visible_commands_for_host(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        entries = tui_module.build_menu_entries(manifest, "windows")
        syntaxes = [entry.syntax for entry in entries]

        self.assertIn("doctor", syntaxes)
        self.assertIn("test <family> <suite>", syntaxes)
        self.assertNotIn("menu", syntaxes)
        self.assertNotIn("verify roadmap-0 --host windows", syntaxes)

    def test_jump_group_moves_between_neighbor_groups(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        entries = tui_module.build_menu_entries(manifest, "windows")
        doctor_index = next(index for index, entry in enumerate(entries) if entry.syntax == "doctor")

        build_index = tui_module.jump_group(entries, doctor_index, direction=1)
        test_and_verify_index = tui_module.jump_group(entries, build_index, direction=1)
        build_index_again = tui_module.jump_group(entries, test_and_verify_index, direction=-1)

        self.assertEqual("Build", entries[build_index].group_title)
        self.assertEqual("Test And Verify", entries[test_and_verify_index].group_title)
        self.assertEqual(build_index, build_index_again)

    def test_resolve_entry_argv_supports_dynamic_capability_target(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        capability_entry = next(
            entry for entry in tui_module.build_menu_entries(manifest, "windows") if entry.command["id"] == "capability"
        )

        argv = tui_module.resolve_entry_argv(capability_entry, prompt_value_provider=lambda prompt: "bootstrap")

        self.assertEqual(["capability", "bootstrap"], argv)

    def test_resolve_entry_argv_supports_dynamic_test_suite_target(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        test_entry = next(
            entry for entry in tui_module.build_menu_entries(manifest, "windows") if entry.command["id"] == "test-family-suite"
        )

        argv = tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: "smoke HelloWorld")

        self.assertEqual(["test", "smoke", "HelloWorld"], argv)

    def test_resolve_entry_argv_supports_dynamic_test_family_all_target(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        test_entry = next(
            entry for entry in tui_module.build_menu_entries(manifest, "windows") if entry.command["id"] == "test-family-all"
        )

        argv = tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: "smoke")

        self.assertEqual(["test", "smoke", "all"], argv)

    def test_resolve_entry_argv_supports_test_all_without_prompt(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        test_entry = next(
            entry for entry in tui_module.build_menu_entries(manifest, "windows") if entry.command["id"] == "test-all"
        )

        argv = tui_module.resolve_entry_argv(test_entry)

        self.assertEqual(["test", "all"], argv)


class TuiTests(unittest.TestCase):
    def test_primary_menu_exposes_only_primary_sections(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        entries = tui_module.build_menu_entries(manifest, "windows")

        self.assertEqual(["prepare", "project", "build", "test", "clean", "inspect"], [entry.syntax for entry in entries])
        self.assertNotIn("verify", [entry.syntax for entry in entries])

    def test_test_submenu_exposes_new_registry_and_object_entries(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        entries = tui_module.build_test_menu_entries(manifest, "windows")
        command_ids = [entry.command["id"] for entry in entries]

        self.assertIn("test-suite", command_ids)
        self.assertIn("test-module", command_ids)
        self.assertIn("test-system", command_ids)
        self.assertIn("test-pipeline", command_ids)
        self.assertIn("test-registry-list", command_ids)
        self.assertIn("test-registry-refresh", command_ids)
        self.assertIn("test-registry-check-consistency", command_ids)
        self.assertNotIn("test-family-suite", command_ids)
        self.assertNotIn("test-list", command_ids)

    def test_test_menu_resolution_uses_new_run_test_forms(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)
        test_entry = next(entry for entry in tui_module.build_menu_entries(manifest, "windows") if entry.syntax == "test")
        smoke_spec = select_public_suite_spec(
            "chaos_integration_tui_smoke_suite",
            host_platform="windows",
            family="smoke",
            required_stages=["all"],
        )
        suite_name = str(smoke_spec["suite"])

        suite_answers = iter(["suite", f"smoke {suite_name}"])
        suite_argv = tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: next(suite_answers))
        self.assertEqual(["test", "suite", "--family", "smoke", "--suite", suite_name], suite_argv)

        module_answers = iter(["module", "managed-smoke basic"])
        module_argv = tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: next(module_answers))
        self.assertEqual(["test", "module", "--module", "managed-smoke", "--profile", "basic"], module_argv)

        self.assertEqual(["test", "registry", "list"], tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: "registry-list"))
        self.assertEqual(["test", "all"], tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: "all"))


if __name__ == "__main__":
    unittest.main()
