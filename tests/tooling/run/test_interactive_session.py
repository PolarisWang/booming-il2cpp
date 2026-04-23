from __future__ import annotations

import unittest
from unittest.mock import patch

from .command_manifest_test_support import load_manifest_module


class _FakeStream:
    def __init__(self, is_tty: bool) -> None:
        self._is_tty = is_tty

    def isatty(self) -> bool:
        return self._is_tty


class InteractiveSessionTests(unittest.TestCase):
    def test_detects_tty_without_terminal_markers(self) -> None:
        manifest_module = load_manifest_module()

        with patch.dict(manifest_module.os.environ, {}, clear=True):
            with patch.object(manifest_module.sys, "stdin", _FakeStream(True)):
                with patch.object(manifest_module.sys, "stdout", _FakeStream(True)):
                    self.assertTrue(manifest_module.is_interactive_session())

    def test_ci_environment_stays_non_interactive(self) -> None:
        manifest_module = load_manifest_module()

        with patch.dict(manifest_module.os.environ, {"CI": "true"}, clear=True):
            with patch.object(manifest_module.sys, "stdin", _FakeStream(True)):
                with patch.object(manifest_module.sys, "stdout", _FakeStream(True)):
                    self.assertFalse(manifest_module.is_interactive_session())


if __name__ == "__main__":
    unittest.main()
