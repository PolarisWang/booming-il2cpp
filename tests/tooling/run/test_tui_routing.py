from __future__ import annotations

import importlib.util
import io
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


if __name__ == "__main__":
    unittest.main()
