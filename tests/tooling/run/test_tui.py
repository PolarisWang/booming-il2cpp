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

        self.assertEqual(["prepare", "build", "test", "clean", "inspect"], syntaxes)
        self.assertEqual(
            ["prepare-menu", "build-menu", "test-menu", "clean-menu", "inspect-menu"],
            [entry.command["id"] for entry in entries],
        )
        self.assertEqual(["Prepare", "Build", "Test", "Clean", "Inspect"], [entry.group_title for entry in entries])
        self.assertIn("workspace", entries[0].command["title"].lower())
        self.assertIn("contracts", entries[1].command["title"].lower())
        self.assertIn("test center", entries[2].command["title"].lower())
        self.assertIn("artifacts", entries[3].command["title"].lower())
        self.assertIn("catalog", entries[4].command["title"].lower())

    def test_jump_group_moves_between_neighbor_groups(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        entries = tui_module.build_menu_entries(manifest, "windows")
        prepare_index = next(index for index, entry in enumerate(entries) if entry.syntax == "prepare")

        build_index = tui_module.jump_group(entries, prepare_index, direction=1)
        test_and_verify_index = tui_module.jump_group(entries, build_index, direction=1)
        build_index_again = tui_module.jump_group(entries, test_and_verify_index, direction=-1)

        self.assertEqual("Build", entries[build_index].group_title)
        self.assertEqual("Test", entries[test_and_verify_index].group_title)
        self.assertEqual(build_index, build_index_again)

    def test_build_prepare_menu_entries_provides_curated_second_level_menu(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        entries = tui_module.build_prepare_menu_entries(manifest, "windows")

        self.assertEqual(
            ["doctor", "prepare", "bootstrap", "prepare-smoke", "prepare-verify-roadmap-0-windows", "menu-back"],
            [entry.command["id"] for entry in entries],
        )
        self.assertEqual(["doctor", "host", "python", "smoke", "roadmap-0", "back"], [entry.syntax for entry in entries])
        self.assertIn("wrapper", entries[2].command["title"].lower())
        self.assertEqual("back", entries[-1].syntax)

    def test_build_build_menu_entries_provides_curated_second_level_menu(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        entries = tui_module.build_build_menu_entries(manifest, "windows")

        self.assertEqual(
            [
                "build-native-contract-abi",
                "build-native-contract-bridge",
                "build-preset-windows-x64-reference",
                "build-platform-android-arm64-smoke",
                "build-platform-linux-x64-packaging",
                "menu-back",
            ],
            [entry.command["id"] for entry in entries],
        )
        self.assertEqual(["abi", "bridge", "reference", "android", "linux", "back"], [entry.syntax for entry in entries])
        self.assertIn("this host", entries[2].command["title"].lower())

    def test_build_clean_menu_entries_provides_curated_second_level_menu(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        entries = tui_module.build_clean_menu_entries(manifest, "windows")

        self.assertEqual(["all", "smoke", "roadmap-0", "back"], [entry.syntax for entry in entries])
        self.assertIn("temporary artifacts", entries[0].command["title"].lower())
        self.assertEqual("menu-back", entries[-1].command["id"])

    def test_build_inspect_menu_entries_provides_curated_second_level_menu(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        entries = tui_module.build_inspect_menu_entries(manifest, "windows")

        self.assertEqual(
            ["help", "capability", "list", "test-list", "menu-back"],
            [entry.command["id"] for entry in entries],
        )
        self.assertEqual(["help", "capability", "catalog", "tests", "back"], [entry.syntax for entry in entries])
        self.assertIn("public test suites", entries[3].command["title"].lower())

    def test_resolve_entry_argv_supports_dynamic_capability_target(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        capability_entry = next(
            entry for entry in tui_module.build_inspect_menu_entries(manifest, "windows") if entry.command["id"] == "capability"
        )

        argv = tui_module.resolve_entry_argv(capability_entry, prompt_value_provider=lambda prompt: "bootstrap")

        self.assertEqual(["capability", "bootstrap"], argv)

    def test_build_test_menu_entries_provides_curated_second_level_menu(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        entries = tui_module.build_test_menu_entries(manifest, "windows")

        self.assertEqual(
            ["test-all", "test-family-all", "test-family-suite", "test-watch", "test-summary", "test-list", "menu-back"],
            [entry.command["id"] for entry in entries],
        )
        self.assertIn("test center", next(entry.command["title"] for entry in entries if entry.command["id"] == "test-all").lower())
        self.assertEqual("back", entries[-1].syntax)
        self.assertIn("previous menu", entries[-1].command["title"].lower())

    def test_menu_help_texts_clarify_navigation_expectations(self) -> None:
        tui_module = load_tui_module()

        self.assertIn("q/Esc to exit", tui_module.PRIMARY_MENU_HELP)
        self.assertIn("Back to return", tui_module.TEST_MENU_HELP)
        self.assertIn("q/Esc to leave this section", tui_module.TEST_MENU_HELP)
        self.assertIn("Back to return", tui_module.SECTION_MENU_HELP)
        self.assertIn("q/Esc to leave this section", tui_module.SECTION_MENU_HELP)

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

    def test_render_menu_screen_inline_preserves_console_history(self) -> None:
        tui_module = load_tui_module()
        entries = [
            tui_module.MenuEntry(
                group_title="Inspect",
                command={"id": "help", "title": "Show help"},
                syntax="help",
                argv=["help"],
            )
        ]

        screen = tui_module.render_menu_screen(
            entries,
            0,
            title="Workspace Control Center",
            help_text="Use Up/Down to move",
            fullscreen=False,
        )

        self.assertIn("Workspace Control Center", screen)
        self.assertNotIn("\x1b[2J\x1b[H", screen)

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

    def test_resolve_entry_argv_supports_test_watch_from_unified_test_menu(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        test_entry = next(
            entry for entry in tui_module.build_menu_entries(manifest, "windows") if entry.syntax == "test"
        )

        argv = tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: "watch")

        self.assertEqual(["test", "watch"], argv)

    def test_resolve_entry_argv_supports_test_summary_from_unified_test_menu(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        test_entry = next(
            entry for entry in tui_module.build_menu_entries(manifest, "windows") if entry.syntax == "test"
        )

        argv = tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: "summary")

        self.assertEqual(["test", "summary"], argv)

    def test_run_fullscreen_menu_routes_test_entry_into_second_level_menu(self) -> None:
        tui_module = load_tui_module()
        test_entry = tui_module.MenuEntry(
            group_title="Validate",
            command={"id": "test-menu", "title": "Open the unified test center"},
            syntax="test",
            argv=["test"],
        )

        class FakeTerminal:
            def __init__(self) -> None:
                self.screens: list[str] = []
                self.keys = iter(["enter"])

            def __enter__(self):
                return self

            def __exit__(self, exc_type, exc, tb) -> None:
                return None

            def render(self, screen: str) -> None:
                self.screens.append(screen)

            def read_key(self) -> str:
                return next(self.keys)

        fake_terminal = FakeTerminal()

        with patch.object(tui_module, "build_menu_entries", return_value=[test_entry]):
            with patch.object(tui_module, "_TerminalSession", return_value=fake_terminal):
                with patch.object(tui_module, "run_test_submenu", return_value=["test", "all"]) as run_test_submenu:
                    argv = tui_module.run_fullscreen_menu({"groups": [], "commands": []}, "windows")

        self.assertEqual(["test", "all"], argv)
        run_test_submenu.assert_called_once()
        self.assertTrue(any("Workspace Control Center" in screen for screen in fake_terminal.screens))

    def test_run_fullscreen_menu_routes_prepare_entry_into_second_level_menu(self) -> None:
        tui_module = load_tui_module()
        prepare_entry = tui_module.MenuEntry(
            group_title="Start",
            command={"id": "prepare-menu", "title": "Check and prepare the local host environment"},
            syntax="prepare",
            argv=["prepare"],
        )

        class FakeTerminal:
            def __init__(self) -> None:
                self.screens: list[str] = []
                self.keys = iter(["enter"])

            def __enter__(self):
                return self

            def __exit__(self, exc_type, exc, tb) -> None:
                return None

            def render(self, screen: str) -> None:
                self.screens.append(screen)

            def read_key(self) -> str:
                return next(self.keys)

        fake_terminal = FakeTerminal()

        with patch.object(tui_module, "build_menu_entries", return_value=[prepare_entry]):
            with patch.object(tui_module, "_TerminalSession", return_value=fake_terminal):
                with patch.object(tui_module, "run_section_submenu", return_value=["doctor"]) as run_section_submenu:
                    argv = tui_module.run_fullscreen_menu({"groups": [], "commands": []}, "windows")

        self.assertEqual(["doctor"], argv)
        run_section_submenu.assert_called_once_with("prepare-menu", {"groups": [], "commands": []}, "windows", terminal=fake_terminal)

    def test_run_inline_menu_reopens_last_selected_section_and_item(self) -> None:
        tui_module = load_tui_module()
        test_entry = tui_module.MenuEntry(
            group_title="Validate",
            command={"id": "test-menu", "title": "Open the unified test center"},
            syntax="test",
            argv=["test"],
        )

        class FakeTerminal:
            def __init__(self) -> None:
                self.screens: list[str] = []
                self.keys = iter([])

            def __enter__(self):
                return self

            def __exit__(self, exc_type, exc, tb) -> None:
                return None

            def render(self, screen: str) -> None:
                self.screens.append(screen)

            def read_key(self) -> str:
                return next(self.keys)

        fake_terminal = FakeTerminal()
        menu_state = tui_module.MenuState(
            primary_command_id="test-menu",
            active_section_command_id="test-menu",
            section_selection_command_id="test-all",
        )

        with patch.object(tui_module, "build_menu_entries", return_value=[test_entry]):
            with patch.object(tui_module, "_InlineTerminalSession", return_value=fake_terminal):
                with patch.object(tui_module, "run_test_submenu", return_value=["test", "all"]) as run_test_submenu:
                    argv = tui_module.run_inline_menu({"groups": [], "commands": []}, "windows", menu_state=menu_state)

        self.assertEqual(["test", "all"], argv)
        run_test_submenu.assert_called_once_with(
            {"groups": [], "commands": []},
            "windows",
            terminal=fake_terminal,
            menu_state=menu_state,
        )
        self.assertEqual([], fake_terminal.screens)

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
                    "eventType": "stage-start",
                    "payload": {"completedUnits": 1, "totalUnits": 2, "activeUnit": "workflow/roadmap-0-macos"},
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
                    "payload": {
                        "finalStatus": "ok",
                        "exitCode": 0,
                        "summaryPath": "artifacts/logs/tests/run-1/summary.json",
                        "eventsPath": "artifacts/logs/tests/run-1/events.jsonl",
                        "telemetryPath": "artifacts/logs/tests/run-1/telemetry.json",
                        "artifacts": [
                            "artifacts/smoke/bin/HelloWorld/Release/net8.0/HelloWorld.dll",
                            "artifacts/run/trace/macos-warmup-trace.runtime.json",
                            "artifacts/verify-roadmap-0/macos",
                        ],
                    },
                },
            ]
            ,
            repo_root=REPO_ROOT,
        )

        self.assertIn("Timeline:", screen)
        self.assertIn("Progress: \x1b[32m50%\x1b[0m", screen)
        self.assertIn("[\x1b[32m 50%\x1b[0m] run    workflow/roadmap-0-macos", screen)
        self.assertIn("[\x1b[32m 50%\x1b[0m] warn   catalog cache skipped", screen)
        self.assertIn("[\x1b[32m 50%\x1b[0m] file   artifacts/logs/tests/run-1/summary.json", screen)
        self.assertIn("[\x1b[32m 50%\x1b[0m] done   ok", screen)
        self.assertIn("\x1b[1;33mImportant outputs:\x1b[0m", screen)
        summary_uri = (REPO_ROOT / "artifacts/logs/tests/run-1/summary.json").resolve().as_uri()
        self.assertIn(f"\x1b]8;;{summary_uri}\x1b\\artifacts/logs/tests/run-1/summary.json\x1b]8;;\x1b\\", screen)
        self.assertIn("\x1b[1;36mTest report:\x1b[0m", screen)
        self.assertIn(
            "\x1b[1;36mPerformance / telemetry:\x1b[0m",
            screen,
        )
        self.assertIn("\x1b[1;36mArtifacts (4):\x1b[0m", screen)
        self.assertIn("\x1b[1;36mSmoke binaries (1):\x1b[0m", screen)
        self.assertIn("\x1b[1;36mTrace outputs (1):\x1b[0m", screen)
        self.assertIn("\x1b[1;36mVerify outputs (1):\x1b[0m", screen)
        self.assertIn("\x1b[1;36mOther artifacts (1):\x1b[0m", screen)
        other_uri = (REPO_ROOT / "artifacts/logs/tests/run-1/summary.json").resolve().as_uri()
        smoke_uri = (REPO_ROOT / "artifacts/smoke/bin/HelloWorld/Release/net8.0/HelloWorld.dll").resolve().as_uri()
        self.assertIn(f"  1. \x1b]8;;{smoke_uri}\x1b\\artifacts/smoke/bin/HelloWorld/Release/net8.0/HelloWorld.dll\x1b]8;;\x1b\\", screen)
        self.assertIn(f"  1. \x1b]8;;{other_uri}\x1b\\artifacts/logs/tests/run-1/summary.json\x1b]8;;\x1b\\", screen)


if __name__ == "__main__":
    unittest.main()
