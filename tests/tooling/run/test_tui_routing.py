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
    def test_bare_interactive_run_executes_selected_tui_command(self) -> None:
        run_module = load_run_module()
        stdout = io.StringIO()

        with patch.object(run_module.manifest_module, "is_interactive_session", return_value=True):
            with patch.object(run_module.runtime_module, "detect_host_platform", return_value="windows-x64"):
                with patch.object(run_module.tui_module, "supports_fullscreen_tui", return_value=True):
                    with patch.object(run_module.tui_module, "run_fullscreen_menu", return_value=["help"]):
                        with patch.object(run_module.sys, "stdout", stdout):
                            exit_code = run_module.main([])

        self.assertEqual(0, exit_code)
        self.assertIn("Available commands", stdout.getvalue())

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


if __name__ == "__main__":
    unittest.main()
