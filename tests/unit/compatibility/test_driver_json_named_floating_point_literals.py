from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
DRIVER_ENTRY_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "DriverEntry.cs"


class DriverJsonNamedFloatingPointLiteralTests(unittest.TestCase):
    def test_driver_json_options_allow_named_floating_point_literals(self) -> None:
        driver_source = DRIVER_ENTRY_PATH.read_text(encoding="utf-8")

        self.assertIn("using System.Text.Json.Serialization;", driver_source)
        self.assertIn("NumberHandling = JsonNumberHandling.AllowNamedFloatingPointLiterals,", driver_source)
        self.assertIn("JsonSerializer.Serialize(value, JsonOptions);", driver_source)


if __name__ == "__main__":
    unittest.main()
