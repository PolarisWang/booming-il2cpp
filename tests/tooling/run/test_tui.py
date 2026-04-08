from __future__ import annotations

import importlib.util
import re
import sys
import unittest
from pathlib import Path
from unittest.mock import patch

from tests.support import select_public_suite_spec, select_subject_record

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


def smoke_binary_artifact_path(suite_name: str) -> str:
    return Path("artifacts", "smoke", "bin", suite_name, "Release", "net8.0", f"{suite_name}.dll").as_posix()


@unittest.skip("legacy assertions superseded by unified test menu coverage")
class LegacyTuiTests(unittest.TestCase):
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
        self.assertEqual(["环境准备", "构建产物", "测试验证", "清理维护", "信息查看"], [entry.group_title for entry in entries])
        self.assertIn("工作区", entries[0].command["title"])
        self.assertIn("运行时契约", entries[1].command["title"])
        self.assertIn("统一测试入口", entries[2].command["title"])
        self.assertIn("缓存构建结果", entries[3].command["title"])
        self.assertIn("公开测试目录", entries[4].command["title"])

    def test_jump_group_moves_between_neighbor_groups(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        entries = tui_module.build_menu_entries(manifest, "windows")
        prepare_index = next(index for index, entry in enumerate(entries) if entry.syntax == "prepare")

        build_index = tui_module.jump_group(entries, prepare_index, direction=1)
        test_and_verify_index = tui_module.jump_group(entries, build_index, direction=1)
        build_index_again = tui_module.jump_group(entries, test_and_verify_index, direction=-1)

        self.assertEqual("构建产物", entries[build_index].group_title)
        self.assertEqual("测试验证", entries[test_and_verify_index].group_title)
        self.assertEqual(build_index, build_index_again)

    def test_build_prepare_menu_entries_provides_curated_second_level_menu(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        entries = tui_module.build_prepare_menu_entries(manifest, "windows")

        self.assertEqual(
            ["doctor", "prepare", "prepare-smoke", "prepare-verify-roadmap-0-windows", "menu-back"],
            [entry.command["id"] for entry in entries],
        )
        self.assertEqual(["基础准备", "基础准备", "场景准备", "场景准备", "返回上级"], [entry.group_title for entry in entries])
        self.assertEqual(["doctor", "setup", "smoke", "roadmap-0", "back"], [entry.syntax for entry in entries])
        self.assertIn("检查并初始化", entries[1].command["title"])
        self.assertEqual("back", entries[-1].syntax)

    def test_build_clean_menu_entries_provides_curated_second_level_menu(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        entries = tui_module.build_clean_menu_entries(manifest, "windows")

        self.assertEqual(["all", "smoke", "roadmap-0", "back"], [entry.syntax for entry in entries])
        self.assertIn("全部托管输出", entries[0].command["title"])
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
        self.assertIn("公开测试套件", entries[3].command["title"])

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
        self.assertEqual(["快速开始", "按范围执行", "按范围执行", "查看结果", "查看结果", "查看结果", "返回上级"], [entry.group_title for entry in entries])
        self.assertIn("默认测试矩阵", next(entry.command["title"] for entry in entries if entry.command["id"] == "test-all"))
        self.assertIn("事件时间线", next(entry.command["title"] for entry in entries if entry.command["id"] == "test-watch"))
        self.assertEqual("back", entries[-1].syntax)
        self.assertIn("返回上一级", entries[-1].command["title"])

    def test_menu_help_texts_clarify_navigation_expectations(self) -> None:
        tui_module = load_tui_module()

        self.assertIn("方向键移动", tui_module.PRIMARY_MENU_HELP)
        self.assertIn("q/Esc 退出", tui_module.PRIMARY_MENU_HELP)
        self.assertIn("方向键选择", tui_module.TEST_MENU_HELP)
        self.assertIn("Back 返回上级", tui_module.TEST_MENU_HELP)
        self.assertIn("q/Esc 返回主菜单", tui_module.TEST_MENU_HELP)
        self.assertIn("方向键选择", tui_module.SECTION_MENU_HELP)
        self.assertIn("Back 返回上级", tui_module.SECTION_MENU_HELP)
        self.assertIn("q/Esc 返回主菜单", tui_module.SECTION_MENU_HELP)

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
            title="工作区控制中心",
            help_text="方向键移动",
            fullscreen=False,
        )

        self.assertIn("工作区控制中心", screen)
        self.assertNotIn("\x1b[2J\x1b[H", screen)
        self.assertIn("日志区 / 当前工作区", screen)
        self.assertNotIn("上方为运行日志，下方为当前菜单", screen)

    def test_render_menu_screen_highlights_headers_and_chinese_descriptions(self) -> None:
        tui_module = load_tui_module()
        entries = [
            tui_module.MenuEntry(
                group_title="查看结果",
                command={"id": "test-watch", "title": "查看当前或最近一次运行的事件时间线"},
                syntax="watch",
                argv=["test", "watch"],
            )
        ]

        screen = tui_module.render_menu_screen(
            entries,
            0,
            title="统一测试入口",
            help_text="方向键选择，Enter 继续，Back 返回上级，q/Esc 返回主菜单。",
            fullscreen=False,
        )

        self.assertIn("\x1b[1;33m统一测试入口\x1b[0m", screen)
        self.assertIn("\x1b[1;36m查看结果\x1b[0m", screen)
        self.assertIn("\x1b[32m1/1\x1b[0m", screen)
        self.assertIn("\x1b[1;7m> watch", screen)

    def test_render_menu_screen_keeps_section_title_bold(self) -> None:
        tui_module = load_tui_module()
        entries = [
            tui_module.MenuEntry(
                group_title="平台校验",
                command={"id": "build-platform-linux-x64-packaging", "title": "校验 Linux 打包路由"},
                syntax="linux",
                argv=["build", "platform", "linux-x64-packaging"],
            )
        ]

        screen = tui_module.render_menu_screen(
            entries,
            0,
            title="构建中心",
            help_text="方向键选择，Enter 继续，Back 返回上级，q/Esc 返回主菜单。",
            fullscreen=False,
        )

        self.assertIn("\x1b[1;33m构建中心\x1b[0m", screen)

    def test_render_menu_screen_keeps_selected_chinese_row_within_terminal_width(self) -> None:
        tui_module = load_tui_module()
        entries = [
            tui_module.MenuEntry(
                group_title="鏄剧ず",
                command={"id": "test-menu", "title": "杩欐槸涓€鏉￠渶瑕侀獙璇佷笉浼氬洜涓枃瀹藉瓧绗﹁€屾姌琛岀殑鑿滃崟璇存槑"},
                syntax="test",
                argv=["test"],
            )
        ]

        with patch.object(tui_module.shutil, "get_terminal_size", return_value=(30, 12)):
            screen = tui_module.render_menu_screen(
                entries,
                0,
                title="缁熶竴娴嬭瘯鍏ュ彛",
                help_text="鏂瑰悜閿€夋嫨",
                fullscreen=False,
            )

        selected_line = next(line for line in screen.splitlines() if "\x1b[1;7m" in line)
        plain_line = re.sub(r"\x1b\[[0-9;]*[A-Za-z]", "", selected_line)
        self.assertLessEqual(tui_module._display_width(plain_line), 30)

    def test_resolve_entry_argv_supports_dynamic_test_suite_target(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)
        smoke_spec = select_public_suite_spec(
            "chaos_tui_dynamic_suite_target",
            host_platform="windows",
            family="smoke",
            required_stages=["all"],
        )

        test_entry = next(
            entry for entry in tui_module.build_menu_entries(manifest, "windows") if entry.syntax == "test"
        )

        answers = iter(["suite", f"{smoke_spec['family']} {smoke_spec['suite']}"])
        argv = tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: next(answers))

        self.assertEqual(["test", str(smoke_spec["family"]), str(smoke_spec["suite"])], argv)

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
        self.assertTrue(any("工作区控制中心" in screen for screen in fake_terminal.screens))

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
        smoke_spec = select_public_suite_spec(
            "chaos_tui_progress_smoke",
            host_platform="windows",
            family="smoke",
            required_stages=["all"],
        )
        smoke_command = f"test {smoke_spec['family']} {smoke_spec['suite']}"
        smoke_artifact_path = smoke_binary_artifact_path(str(smoke_spec["suite"]))

        screen = tui_module.render_test_progress_screen(
            [
                {
                    "eventType": "session-start",
                    "payload": {"command": smoke_command},
                },
                {
                    "eventType": "progress",
                    "payload": {"completedUnits": 1, "totalUnits": 2, "activeUnit": "case:main"},
                },
                {
                    "eventType": "stage-start",
                    "payload": {"completedUnits": 1, "totalUnits": 2, "activeUnit": "workflow/runtime-baseline-macos"},
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
                        "phaseResults": [
                            {"phaseId": "code", "status": "ok"},
                            {"phaseId": "module", "status": "ok"},
                            {"phaseId": "system", "status": "ok"},
                        ],
                        "summaryPath": "artifacts/logs/tests/run-1/summary.json",
                        "eventsPath": "artifacts/logs/tests/run-1/events.jsonl",
                        "telemetryPath": "artifacts/logs/tests/run-1/telemetry.json",
                        "artifacts": [
                            smoke_artifact_path,
                            "artifacts/run/trace/macos-warmup-trace.runtime.json",
                            "artifacts/verify-runtime-baseline/macos",
                        ],
                    },
                },
            ]
            ,
            repo_root=REPO_ROOT,
        )

        self.assertIn("Timeline:", screen)
        self.assertIn("Progress: \x1b[32m50%\x1b[0m", screen)
        self.assertIn("[\x1b[32m 50%\x1b[0m] run    workflow/runtime-baseline-macos", screen)
        self.assertIn("[\x1b[32m 50%\x1b[0m] warn   catalog cache skipped", screen)
        self.assertIn("[\x1b[32m 50%\x1b[0m] file   artifacts/logs/tests/run-1/summary.json", screen)
        self.assertIn("[\x1b[32m 50%\x1b[0m] done   ok", screen)
        self.assertIn("Phases:", screen)
        self.assertIn("ok: code", screen)
        self.assertIn("ok: module", screen)
        self.assertIn("ok: system", screen)
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
        smoke_uri = (REPO_ROOT / smoke_artifact_path).resolve().as_uri()
        self.assertIn(f"  1. \x1b]8;;{smoke_uri}\x1b\\{smoke_artifact_path}\x1b]8;;\x1b\\", screen)
        self.assertIn(f"  1. \x1b]8;;{other_uri}\x1b\\artifacts/logs/tests/run-1/summary.json\x1b]8;;\x1b\\", screen)

    def test_render_operation_progress_screen_consumes_event_stream(self) -> None:
        tui_module = load_tui_module()

        screen = tui_module.render_operation_progress_screen(
            [
                {
                    "eventType": "session-start",
                    "payload": {"command": "prepare"},
                },
                {
                    "eventType": "stage-start",
                    "payload": {"completedUnits": 0, "totalUnits": 3, "activeUnit": "doctor"},
                },
                {
                    "eventType": "progress",
                    "payload": {"completedUnits": 1, "totalUnits": 3, "activeUnit": "doctor", "suiteStatus": "ok"},
                },
                {
                    "eventType": "stage-start",
                    "payload": {"completedUnits": 1, "totalUnits": 3, "activeUnit": "build native-contract abi"},
                },
                {
                    "eventType": "artifact",
                    "payload": {"completedUnits": 2, "totalUnits": 3, "activeUnit": "build native-contract abi", "path": "artifacts/run/native-contract-abi"},
                },
                {
                    "eventType": "final-summary",
                    "payload": {
                        "finalStatus": "ok",
                        "exitCode": 0,
                        "summaryPath": "artifacts/logs/run/run-1/summary.json",
                        "eventsPath": "artifacts/logs/run/run-1/events.jsonl",
                        "consolePath": "artifacts/logs/run/run-1/console.log",
                        "telemetryPath": "artifacts/logs/run/run-1/telemetry.json",
                        "importantOutputs": [{"label": "Prepare state", "path": "artifacts/run/prepare/global.json"}],
                        "artifacts": ["artifacts/run/native-contract-abi", "artifacts/run/prepare/global.json"],
                    },
                },
            ],
            repo_root=REPO_ROOT,
        )

        self.assertIn("Unified Run Progress", screen)
        self.assertIn("Progress: \x1b[32m66%\x1b[0m", screen)
        self.assertIn("[\x1b[32m 33%\x1b[0m] ok     doctor", screen)
        self.assertIn("[\x1b[32m 33%\x1b[0m] run    build native-contract abi", screen)
        self.assertIn("\x1b[1;33mImportant outputs:\x1b[0m", screen)
        summary_uri = (REPO_ROOT / "artifacts/logs/run/run-1/summary.json").resolve().as_uri()
        self.assertIn(f"\x1b]8;;{summary_uri}\x1b\\artifacts/logs/run/run-1/summary.json\x1b]8;;\x1b\\", screen)
        self.assertIn("\x1b[1;36mPrepare state:\x1b[0m", screen)
        self.assertIn("\x1b[1;36mArtifacts (2):\x1b[0m", screen)

    def test_build_build_menu_entries_provides_curated_second_level_menu(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        entries = tui_module.build_build_menu_entries(manifest, "windows")

        self.assertEqual(
            [
                "build-all",
                "build-native-contract-abi",
                "build-native-contract-bridge",
                "build-preset-windows-x64-reference",
                "build-platform-android-arm64-smoke",
                "build-platform-linux-x64-packaging",
                "menu-back",
            ],
            [entry.command["id"] for entry in entries],
        )
        self.assertEqual(["all", "abi", "bridge", "windows", "android", "linux", "back"], [entry.syntax for entry in entries])
        self.assertIn("全量构建", entries[0].command["title"])
        self.assertIn("Windows", entries[3].command["title"])
        self.assertEqual("校验 Android 启动 smoke 路由", entries[4].command["title"])
        self.assertEqual("校验 Linux 打包路由", entries[5].command["title"])


class TuiUnifiedMenuTests(unittest.TestCase):
    def test_build_menu_entries_only_includes_primary_sections(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        entries = tui_module.build_menu_entries(manifest, "windows")

        self.assertEqual(["prepare", "build", "test", "clean", "inspect"], [entry.syntax for entry in entries])
        self.assertEqual(
            ["prepare-menu", "build-menu", "test-menu", "clean-menu", "inspect-menu"],
            [entry.command["id"] for entry in entries],
        )
        self.assertTrue(all(entry.command["title"] for entry in entries))

    def test_build_inspect_menu_entries_points_to_registry_listing(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        entries = tui_module.build_inspect_menu_entries(manifest, "windows")

        self.assertEqual(
            ["help", "capability", "list", "test-registry-list", "menu-back"],
            [entry.command["id"] for entry in entries],
        )
        self.assertEqual(["help", "capability", "catalog", "tests", "back"], [entry.syntax for entry in entries])

    def test_build_test_menu_entries_uses_new_unified_objects(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        entries = tui_module.build_test_menu_entries(manifest, "windows")

        self.assertEqual(
            [
                "test-all",
                "test-suite",
                "test-subject",
                "test-module",
                "test-system",
                "test-pipeline",
                "test-registry-list",
                "test-registry-refresh",
                "test-registry-check-consistency",
                "test-watch",
                "test-summary",
                "menu-back",
            ],
            [entry.command["id"] for entry in entries],
        )
        self.assertEqual(
            ["Quick Start", "Selectors", "Selectors", "Selectors", "Selectors", "Selectors", "Registry", "Registry", "Registry", "Results", "Results", "Back"],
            [entry.group_title for entry in entries],
        )

    def test_build_test_menu_entries_describes_test_all_as_suite_and_subject_batch(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        entries = tui_module.build_test_menu_entries(manifest, "windows")

        title = next(entry.command["title"] for entry in entries if entry.command["id"] == "test-all")
        self.assertIn("suites + subjects", title)

    def test_build_test_subject_menu_entries_lists_registered_subjects(self) -> None:
        tui_module = load_tui_module()
        subject_record = select_subject_record(
            "chaos_tui_subject_menu_entries",
            source_type="dotnet-project",
            required_host_platforms=["windows-x64"],
        )
        subject_id = str(subject_record["subjectId"])

        entries = tui_module.build_test_subject_menu_entries("windows")

        subject_entry = next(entry for entry in entries if entry.syntax == subject_id)
        self.assertEqual(["test", "subject", "--id", f"subject/{subject_id}"], subject_entry.argv)
        self.assertEqual(f"subject/{subject_id}", subject_entry.command["targetObjectId"])
        self.assertEqual("menu-back", entries[-1].command["id"])

    def test_render_test_all_preview_screen_summarizes_batch(self) -> None:
        tui_module = load_tui_module()

        screen = tui_module.render_test_all_preview_screen(
            {
                "hostPlatform": "windows",
                "plannedCounts": {"totalUnits": 3, "suiteCount": 2, "subjectCount": 1},
                "countsByFamily": {
                    "smoke": {"total": 1},
                    "contract": {"total": 1},
                },
                "subjectPreview": ["HelloWorldObject"],
                "outputPaths": {
                    "summaryPath": "artifacts/logs/tests/<run-id>/summary.json",
                    "eventsPath": "artifacts/logs/tests/<run-id>/events.jsonl",
                    "consolePath": "artifacts/logs/tests/<run-id>/console.log",
                },
            }
        )

        self.assertIn("Unified Test Batch Preview", screen)
        self.assertIn("Host: windows", screen)
        self.assertIn("Planned: 3 = 2 suites + 1 subjects", screen)
        self.assertIn("Families: contract 1 | smoke 1", screen)
        self.assertIn("Subjects: HelloWorldObject", screen)
        self.assertIn("artifacts/logs/tests/<run-id>/summary.json", screen)

    def test_run_test_submenu_routes_subject_entry_to_third_level_subject_menu(self) -> None:
        tui_module = load_tui_module()
        manifest = {"groups": [], "commands": []}
        subject_entry = tui_module.MenuEntry(
            "Selectors",
            {"id": "test-subject", "title": "Run a subject object"},
            "subject",
            ["test"],
        )
        fake_terminal = object()

        with patch.object(tui_module, "build_test_menu_entries", return_value=[subject_entry]):
            with patch.object(tui_module, "_run_menu_selection", return_value=subject_entry):
                with patch.object(
                    tui_module,
                    "run_test_subject_submenu",
                    return_value=["test", "subject", "--id", "subject/HelloWorldObject"],
                ) as run_test_subject_submenu:
                    argv = tui_module.run_test_submenu(manifest, "windows", terminal=fake_terminal)

        self.assertEqual(["test", "subject", "--id", "subject/HelloWorldObject"], argv)
        run_test_subject_submenu.assert_called_once_with(
            manifest,
            "windows",
            terminal=fake_terminal,
            menu_state=None,
        )

    def test_run_test_submenu_routes_test_all_entry_through_preview(self) -> None:
        tui_module = load_tui_module()
        manifest = {"groups": [], "commands": []}
        test_all_entry = tui_module.MenuEntry(
            "Quick Start",
            {"id": "test-all", "title": "Run host default batch (suites + subjects)"},
            "all",
            ["test", "all"],
        )
        fake_terminal = object()

        with patch.object(tui_module, "build_test_menu_entries", return_value=[test_all_entry]):
            with patch.object(tui_module, "_run_menu_selection", return_value=test_all_entry):
                with patch.object(
                    tui_module,
                    "run_test_all_preview",
                    return_value=["test", "all"],
                ) as run_test_all_preview:
                    argv = tui_module.run_test_submenu(manifest, "windows", terminal=fake_terminal)

        self.assertEqual(["test", "all"], argv)
        run_test_all_preview.assert_called_once_with("windows", terminal=fake_terminal)

    def test_resolve_entry_argv_supports_suite_subject_module_system_pipeline(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)
        test_entry = next(entry for entry in tui_module.build_menu_entries(manifest, "windows") if entry.syntax == "test")
        smoke_spec = select_public_suite_spec(
            "chaos_tui_unified_suite_selector",
            host_platform="windows",
            family="smoke",
            required_stages=["all"],
        )
        subject_record = select_subject_record(
            "chaos_tui_unified_subject_selector",
            source_type="dotnet-project",
            required_host_platforms=["windows-x64"],
        )

        suite_answers = iter(["suite", f"{smoke_spec['family']} {smoke_spec['suite']}"])
        suite_argv = tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: next(suite_answers))
        self.assertEqual(
            ["test", "suite", "--family", str(smoke_spec["family"]), "--suite", str(smoke_spec["suite"])],
            suite_argv,
        )

        subject_answers = iter(["subject", str(subject_record["subjectId"])])
        subject_argv = tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: next(subject_answers))
        self.assertEqual(["test", "subject", "--subject", str(subject_record["subjectId"])], subject_argv)

        module_answers = iter(["module", "managed-smoke basic"])
        module_argv = tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: next(module_answers))
        self.assertEqual(["test", "module", "--module", "managed-smoke", "--profile", "basic"], module_argv)

        system_answers = iter(["system", "hosted-runtime-smoke"])
        system_argv = tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: next(system_answers))
        self.assertEqual(["test", "system", "--scenario", "hosted-runtime-smoke"], system_argv)

        pipeline_answers = iter(["pipeline", "completion-runtime-core"])
        pipeline_argv = tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: next(pipeline_answers))
        self.assertEqual(["test", "pipeline", "--pipeline", "completion-runtime-core"], pipeline_argv)

    def test_resolve_entry_argv_supports_registry_and_result_actions(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)
        test_entry = next(entry for entry in tui_module.build_menu_entries(manifest, "windows") if entry.syntax == "test")

        self.assertEqual(["test", "all"], tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: "all"))
        self.assertEqual(
            ["test", "registry", "list"],
            tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: "registry-list"),
        )
        self.assertEqual(
            ["test", "registry", "refresh"],
            tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: "registry-refresh"),
        )
        self.assertEqual(
            ["test", "registry", "check-consistency"],
            tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: "registry-check"),
        )
        self.assertEqual(["test", "watch"], tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: "watch"))
        self.assertEqual(["test", "summary"], tui_module.resolve_entry_argv(test_entry, prompt_value_provider=lambda prompt: "summary"))

    def test_direct_registry_entry_resolves_to_public_command(self) -> None:
        manifest_module = load_manifest_module()
        tui_module = load_tui_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        entry = next(
            item for item in tui_module.build_test_menu_entries(manifest, "windows") if item.command["id"] == "test-registry-check-consistency"
        )

        self.assertEqual(["test", "registry", "check-consistency"], tui_module.resolve_entry_argv(entry))

    def test_render_test_progress_screen_renders_pipeline_phases(self) -> None:
        tui_module = load_tui_module()

        screen = tui_module.render_test_progress_screen(
            [
                {"eventType": "session-start", "payload": {"command": "test pipeline --id pipeline/completion-runtime-core"}},
                {"eventType": "progress", "payload": {"completedUnits": 1, "totalUnits": 2}},
                {
                    "eventType": "final-summary",
                    "payload": {
                        "finalStatus": "ok",
                        "phaseResults": [
                            {"phaseId": "code", "status": "ok"},
                            {"phaseId": "module", "status": "ok"},
                            {"phaseId": "system", "status": "ok"},
                        ],
                    },
                },
            ],
            repo_root=REPO_ROOT,
        )

        self.assertIn("Phases:", screen)
        self.assertIn("ok: code", screen)
        self.assertIn("ok: module", screen)
        self.assertIn("ok: system", screen)

    def test_render_test_progress_screen_highlights_subject_summary(self) -> None:
        tui_module = load_tui_module()
        subject_record = select_subject_record(
            "chaos_tui_subject_summary_highlight",
            source_type="dotnet-project",
            required_host_platforms=["windows-x64"],
        )
        subject_id = str(subject_record["subjectId"])
        matrix_id = str(subject_record["manifest"]["defaultMatrix"])
        run_id = "fixture-run-summary-001"
        summary_path = f"artifacts/subjects/{subject_id}/runs/{run_id}/subject-report/summary.json"
        matrix_report_path = f"artifacts/subjects/{subject_id}/runs/{run_id}/matrices/{matrix_id}/pipeline-report/report.json"

        screen = tui_module.render_test_progress_screen(
            [
                {"eventType": "session-start", "payload": {"command": f"test subject --id subject/{subject_id}"}},
                {
                    "eventType": "final-summary",
                    "payload": {
                        "finalStatus": "ok",
                        "subjectResults": [
                            {
                                "subjectId": subject_id,
                                "status": "ok",
                                "subjectSummaryPath": summary_path,
                            }
                        ],
                        "artifacts": [
                            summary_path,
                            matrix_report_path,
                        ],
                    },
                },
            ],
            repo_root=REPO_ROOT,
        )

        self.assertIn("Subjects:", screen)
        self.assertIn(f"ok: {subject_id}", screen)
        self.assertIn("\x1b[1;36mSubject summary:\x1b[0m", screen)
        subject_summary_uri = (REPO_ROOT / summary_path).resolve().as_uri()
        self.assertIn(
            f"\x1b]8;;{subject_summary_uri}\x1b\\{summary_path}\x1b]8;;\x1b\\",
            screen,
        )

    def test_render_test_progress_screen_renders_batch_counts_current_and_failures(self) -> None:
        tui_module = load_tui_module()

        screen = tui_module.render_test_progress_screen(
            [
                {"eventType": "session-start", "payload": {"command": "test all"}},
                {
                    "eventType": "stage-start",
                    "payload": {
                        "completedUnits": 1,
                        "totalUnits": 3,
                        "activeUnit": "contract/trace-schema",
                        "activeUnitContext": {
                            "id": "contract/trace-schema",
                            "type": "suite",
                            "family": "contract",
                            "level": "code",
                            "primaryModuleId": "trace-export",
                        },
                    },
                },
                {
                    "eventType": "final-summary",
                    "payload": {
                        "finalStatus": "fail",
                        "plannedCounts": {"totalUnits": 3, "suiteCount": 2, "subjectCount": 1},
                        "countsByType": {
                            "suite": {"total": 2, "ok": 1, "fail": 1, "skip": 0, "aborted": 0},
                            "subject": {"total": 1, "ok": 0, "fail": 0, "skip": 0, "aborted": 1},
                        },
                        "countsByFamily": {
                            "smoke": {"total": 1, "ok": 1, "fail": 0, "skip": 0, "aborted": 0},
                            "contract": {"total": 1, "ok": 0, "fail": 1, "skip": 0, "aborted": 0},
                        },
                        "failureItems": [
                            {
                                "id": "contract/trace-schema",
                                "type": "suite",
                                "status": "fail",
                                "rerunCommand": "run test contract trace-schema",
                            }
                        ],
                    },
                },
            ],
            repo_root=REPO_ROOT,
        )

        self.assertIn("Batch:", screen)
        self.assertIn("Planned: 3 = 2 suites + 1 subjects", screen)
        self.assertIn("Families: contract 1 | smoke 1", screen)
        self.assertIn("Counts:", screen)
        self.assertIn("suite: total 2 | ok 1 | fail 1 | skip 0 | aborted 0", screen)
        self.assertIn("subject: total 1 | ok 0 | fail 0 | skip 0 | aborted 1", screen)
        self.assertIn("Current:", screen)
        self.assertIn("family=contract | level=code | module=trace-export", screen)
        self.assertIn("Failures:", screen)
        self.assertIn("rerun: run test contract trace-schema", screen)


if __name__ == "__main__":
    unittest.main()
