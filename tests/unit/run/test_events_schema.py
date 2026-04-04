from __future__ import annotations

import importlib.util
import sys
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
EVENTS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "events.py"


def load_events_module():
    if not EVENTS_MODULE_PATH.is_file():
        raise FileNotFoundError(f"events module missing: {EVENTS_MODULE_PATH}")

    spec = importlib.util.spec_from_file_location("booming_run_events", EVENTS_MODULE_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load events module: {EVENTS_MODULE_PATH}")

    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


class EventsSchemaTests(unittest.TestCase):
    def test_build_event_returns_stable_envelope(self) -> None:
        events_module = load_events_module()

        event = events_module.build_event("final-summary", {"runId": "20260404-120000-macos-abcd"})

        self.assertEqual("final-summary", event["eventType"])
        self.assertIn("timestampUtc", event)
        self.assertEqual("20260404-120000-macos-abcd", event["payload"]["runId"])


if __name__ == "__main__":
    unittest.main()
