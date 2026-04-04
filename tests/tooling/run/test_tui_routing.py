from __future__ import annotations

import importlib.util
import io
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch


REPO_ROOT = Path(__file__).resolve().parents[3]
RUN_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "run.py"


def load_run_module():
    if not RUN_MODULE_PATH.is_file():
        raise FileNotFoundError(f"run module missing: {RUN_MODULE_PATH}")

    spec = importlib.util.spec_from_file_location("booming_run_main", RUN_MODULE_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load run module: {RUN_MODULE_PATH}")

    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


class TuiRoutingTests(unittest.TestCase):
    def test_bare_interactive_run_returns_to_menu_after_selected_command(self) -> None:
        run_module = load_run_module()
        stdout = io.StringIO()

        with patch.object(run_module.manifest_module, "is_interactive_session", return_value=True):
            with patch.object(run_module.runtime_module, "detect_host_platform", return_value="windows-x64"):
                with patch.object(run_module.tui_module, "supports_fullscreen_tui", return_value=True):
                    with patch.object(
                        run_module.tui_module,
                        "run_fullscreen_menu",
                        return_value=["help"],
                    ) as run_fullscreen_menu:
                        with patch.object(
                            run_module.tui_module,
                            "run_inline_menu",
                            return_value=None,
                            create=True,
                        ) as run_inline_menu:
                            with patch.object(run_module.sys, "stdout", stdout):
                                exit_code = run_module.main([])

        self.assertEqual(0, exit_code)
        self.assertIn("Available commands", stdout.getvalue())
        self.assertEqual(1, run_fullscreen_menu.call_count)
        self.assertEqual(1, run_inline_menu.call_count)
        self.assertTrue(
            stdout.getvalue().startswith("\r\x1b[2K"),
            "output rendered after leaving the fullscreen menu should start on a clean line",
        )

    def test_cancelled_tui_menu_exits_cleanly_without_output(self) -> None:
        run_module = load_run_module()
        stdout = io.StringIO()

        with patch.object(run_module.manifest_module, "is_interactive_session", return_value=True):
            with patch.object(run_module.runtime_module, "detect_host_platform", return_value="windows-x64"):
                with patch.object(run_module.tui_module, "supports_fullscreen_tui", return_value=True):
                    with patch.object(run_module.tui_module, "run_fullscreen_menu", return_value=None):
                        with patch.object(run_module.sys, "stdout", stdout):
                            exit_code = run_module.main([])

        self.assertEqual(0, exit_code)
        self.assertEqual("", stdout.getvalue())

    def test_json_test_command_prefers_event_stream_output(self) -> None:
        run_module = load_run_module()
        stdout = io.StringIO()

        with tempfile.TemporaryDirectory() as temp_dir:
            repo_root = Path(temp_dir)
            manifest_path = repo_root / "build" / "toolchains" / "run" / "run_manifest.json"
            events_path = repo_root / "artifacts" / "logs" / "tests" / "run-1" / "events.jsonl"
            manifest_path.parent.mkdir(parents=True, exist_ok=True)
            events_path.parent.mkdir(parents=True, exist_ok=True)
            manifest_path.write_text(
                (REPO_ROOT / "build" / "toolchains" / "run" / "run_manifest.json").read_text(encoding="utf-8"),
                encoding="utf-8",
            )
            events_path.write_text('{"eventType":"progress"}\n{"eventType":"final-summary"}\n', encoding="utf-8")

            result = run_module.CommandResult.success(
                command="test smoke HelloWorld",
                host_platform="macos",
                target="smoke/HelloWorld",
                payload={"eventsPath": "artifacts/logs/tests/run-1/events.jsonl"},
            )

            with patch.object(run_module, "resolve_repo_root", return_value=repo_root):
                with patch.object(run_module.manifest_module, "is_interactive_session", return_value=False):
                    with patch.object(run_module.runtime_module, "detect_host_platform", return_value="macos-arm64"):
                        with patch.object(run_module, "execute_command", return_value=result):
                            with patch.object(run_module.sys, "stdout", stdout):
                                exit_code = run_module.main(["test", "smoke", "HelloWorld", "--json"])

        self.assertEqual(0, exit_code)
        self.assertEqual('{"eventType":"progress"}\n{"eventType":"final-summary"}\n', stdout.getvalue())

    def test_interactive_test_command_renders_runtime_progress_screen(self) -> None:
        run_module = load_run_module()
        stdout = io.StringIO()

        def fake_handle(command, repo_root, host_platform, command_text, manifest, options, progress_callback=None):
            del command, repo_root, host_platform, command_text, manifest, options
            assert progress_callback is not None
            progress_callback(
                {
                    "eventType": "session-start",
                    "payload": {"command": "test all"},
                }
            )
            progress_callback(
                {
                    "eventType": "progress",
                    "payload": {"completedUnits": 0, "totalUnits": 4, "activeUnit": "smoke/HelloWorld"},
                }
            )
            progress_callback(
                {
                    "eventType": "stage-start",
                    "payload": {"completedUnits": 0, "totalUnits": 4, "activeUnit": "smoke/HelloWorld"},
                }
            )
            progress_callback(
                {
                    "eventType": "progress",
                    "payload": {"completedUnits": 1, "totalUnits": 4, "activeUnit": "smoke/HelloWorld", "suiteStatus": "ok"},
                }
            )
            return run_module.CommandResult.success(
                command="test all",
                host_platform="macos",
                target="all",
                payload={
                    "summaryPath": "artifacts/logs/tests/run-1/summary.json",
                    "eventsPath": "artifacts/logs/tests/run-1/events.jsonl",
                    "sessionPath": "artifacts/logs/tests/run-1/session.json",
                    "telemetryPath": "artifacts/logs/tests/run-1/telemetry.json",
                    "artifacts": [
                        "artifacts/smoke/bin/HelloWorld/Release/net8.0/HelloWorld.dll",
                        "artifacts/run/trace/macos-warmup-trace.runtime.json",
                        "artifacts/verify-roadmap-0/macos",
                    ],
                },
                text="final output\n",
            )

        with patch.object(run_module.manifest_module, "is_interactive_session", return_value=True):
            with patch.object(run_module.runtime_module, "detect_host_platform", return_value="macos-arm64"):
                with patch.object(run_module.test_commands, "handle", side_effect=fake_handle):
                    with patch.object(run_module.sys, "stdout", stdout):
                        exit_code = run_module.main(["test", "all"])

        self.assertEqual(0, exit_code)
        self.assertIn("Unified Test Progress", stdout.getvalue())
        self.assertIn("[\x1b[32m  0%\x1b[0m] queued smoke/HelloWorld", stdout.getvalue())
        self.assertIn("[\x1b[32m 25%\x1b[0m] ok     smoke/HelloWorld", stdout.getvalue())
        self.assertIn("final output", stdout.getvalue())
        self.assertIn("\x1b[1;33mImportant outputs:\x1b[0m", stdout.getvalue())
        self.assertIn("\x1b[1;36mTest report:\x1b[0m", stdout.getvalue())
        self.assertIn("\x1b]8;;file://", stdout.getvalue())
        self.assertIn("artifacts/logs/tests/run-1/summary.json\x1b]8;;\x1b\\", stdout.getvalue())
        self.assertIn("\x1b[1;36mArtifacts (3):\x1b[0m", stdout.getvalue())
        self.assertIn("\x1b[1;36mSmoke binaries (1):\x1b[0m", stdout.getvalue())
        self.assertIn("\x1b[1;36mTrace outputs (1):\x1b[0m", stdout.getvalue())
        self.assertIn("\x1b[1;36mVerify outputs (1):\x1b[0m", stdout.getvalue())
        self.assertIn("artifacts/run/trace/macos-warmup-trace.runtime.json\x1b]8;;\x1b\\", stdout.getvalue())
        self.assertNotIn("\x1b[2J", stdout.getvalue())


if __name__ == "__main__":
    unittest.main()
