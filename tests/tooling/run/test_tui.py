from __future__ import annotations

import importlib.util
import sys
import unittest
from pathlib import Path
from unittest.mock import patch

from .test_command_manifest import RUN_MANIFEST_PATH, load_manifest_module


REPO_ROOT = Path(__file__).resolve().parents[3]
TUI_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "tui.py"


def load_tui_module():
    if not TUI_MODULE_PATH.is_file():
        raise FileNotFoundError(f"tui module missing: {TUI_MODULE_PATH}")

    spec = importlib.util.spec_from_file_location("booming_run_tui", TUI_MODULE_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load tui module: {TUI_MODULE_PATH}")

    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


class TuiTests(unittest.TestCase):
    def test_build_menu_entries_only_includes_visible_commands_for_host(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        entries = tui_module.build_menu_entries(manifest, "windows")
        syntaxes = [entry.syntax for entry in entries]

        self.assertIn("doctor", syntaxes)
        self.assertIn("test", syntaxes)
        self.assertNotIn("menu", syntaxes)
        self.assertNotIn("verify roadmap-0 --host windows", syntaxes)
        self.assertEqual(1, sum(1 for entry in entries if entry.group_title == "Test And Verify" and entry.syntax == "test"))

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

    def test_terminal_session_render_normalizes_newlines_on_posix(self) -> None:
        tui_module = load_tui_module()

        class FakeStdout:
            def __init__(self) -> None:
                self.parts: list[str] = []

            def write(self, text: str) -> None:
                self.parts.append(text)

            def flush(self) -> None:
                return None

        fake_stdout = FakeStdout()
        session = tui_module._TerminalSession()

        with patch.object(tui_module.os, "name", "posix"):
            with patch.object(tui_module.sys, "stdout", fake_stdout):
                session.render("row1\nrow2")

        self.assertEqual("row1\r\nrow2", "".join(fake_stdout.parts))

    def test_resolve_entry_argv_supports_dynamic_test_suite_target(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        test_entry = next(
            entry for entry in tui_module.build_menu_entries(manifest, "windows") if entry.syntax == "test"
        )

        answers = iter(["suite", "smoke HelloWorld"])
        argv = tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: next(answers))

        self.assertEqual(["test", "smoke", "HelloWorld"], argv)

    def test_resolve_entry_argv_supports_dynamic_test_family_all_target(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        test_entry = next(
            entry for entry in tui_module.build_menu_entries(manifest, "windows") if entry.syntax == "test"
        )

        answers = iter(["family-all", "smoke"])
        argv = tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: next(answers))

        self.assertEqual(["test", "smoke", "all"], argv)

    def test_resolve_entry_argv_supports_test_all_without_prompt(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        test_entry = next(
            entry for entry in tui_module.build_menu_entries(manifest, "windows") if entry.syntax == "test"
        )

        argv = tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: "all")

        self.assertEqual(["test", "all"], argv)

    def test_resolve_entry_argv_supports_test_list_from_unified_test_menu(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        test_entry = next(
            entry for entry in tui_module.build_menu_entries(manifest, "windows") if entry.syntax == "test"
        )

        answers = iter(["list", "smoke"])
        argv = tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: next(answers))

        self.assertEqual(["test", "list", "smoke"], argv)

    def test_render_test_progress_screen_consumes_event_stream(self) -> None:
        tui_module = load_tui_module()

        screen = tui_module.render_test_progress_screen(
            [
                {
                    "eventType": "session-start",
                    "payload": {"command": "test smoke HelloWorld"},
                },
                {
                    "eventType": "progress",
                    "payload": {"completedUnits": 1, "totalUnits": 2, "activeUnit": "case:main"},
                },
                {
                    "eventType": "warning",
                    "payload": {"message": "catalog cache skipped"},
                },
                {
                    "eventType": "artifact",
                    "payload": {"path": "artifacts/logs/tests/run-1/summary.json"},
                },
                {
                    "eventType": "final-summary",
                    "payload": {"finalStatus": "ok", "exitCode": 0},
                },
            ]
        )

        self.assertIn("50%", screen)
        self.assertIn("catalog cache skipped", screen)
        self.assertIn("artifacts/logs/tests/run-1/summary.json", screen)
        self.assertIn("ok", screen)


if __name__ == "__main__":
    unittest.main()
