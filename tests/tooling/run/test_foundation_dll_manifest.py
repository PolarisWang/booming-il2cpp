from __future__ import annotations

import unittest
from pathlib import Path

from tests.tooling.run.command_manifest_test_support import load_manifest_module


REPO_ROOT = Path(__file__).resolve().parents[3]
MANIFEST_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "core" / "manifest.py"


class FoundationDllManifestTests(unittest.TestCase):
    def test_foundation_dll_commands_are_parseable(self) -> None:
        manifest_module = load_manifest_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT)

        cases = [
            (["foundation-dll", "derive", "--scope", "all"], "foundation-dll-derive"),
            (["foundation-dll", "analyze-gaps", "--scope", "System.Private.CoreLib"], "foundation-dll-analyze-gaps"),
            (["foundation-dll", "check-family", "--family", "family/System.Private.CoreLib/convert/char"], "foundation-dll-check-family"),
            (["foundation-dll", "onboard", "--scope", "System.Text.Json", "--promote-approved", "true"], "foundation-dll-onboard"),
            (["foundation-dll", "promote", "--scope", "System.Text.Json", "--promote-approved", "true"], "foundation-dll-promote"),
            (["foundation-dll", "refresh"], "foundation-dll-refresh"),
            (["foundation-dll", "verify-consistency", "--mode", "full"], "foundation-dll-verify-consistency"),
            (["foundation-dll", "full", "--scope", "all", "--promote-approved", "true", "--mode", "full"], "foundation-dll-full"),
        ]

        for argv, command_id in cases:
            parsed = manifest_module.parse_cli(argv, False, manifest, "windows")
            self.assertIsNotNone(parsed["command"], argv)
            self.assertEqual(command_id, parsed["command"]["id"], argv)

    def test_foundation_dll_commands_reject_extra_positional_arguments(self) -> None:
        manifest_module = load_manifest_module()
        manifest = manifest_module.load_run_manifest(REPO_ROOT)

        cases = [
            ["foundation-dll", "derive", "junk"],
            ["foundation-dll", "refresh", "junk"],
            ["foundation-dll", "verify-consistency", "junk"],
            ["foundation-dll", "full", "junk", "--promote-approved", "true"],
        ]

        for argv in cases:
            parsed = manifest_module.parse_cli(argv, False, manifest, "windows")
            self.assertIsNone(parsed["command"], argv)


if __name__ == "__main__":
    unittest.main()
