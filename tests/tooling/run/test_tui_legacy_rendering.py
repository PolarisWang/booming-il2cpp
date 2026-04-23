from tests.tooling.run.tui_test_support import *


class TestTuiLegacyRendering(LegacyTuiTestSupport):
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
