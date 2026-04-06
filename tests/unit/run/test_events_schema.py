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
    def test_build_event_returns_stable_envelope_with_subject_fields(self) -> None:
        events_module = load_events_module()

        event = events_module.build_event(
            "stage-finished",
            {"runId": "20260404-120000-macos-abcd"},
            run_id="20260404-120000-macos-abcd",
            suite_id="smoke/HelloWorld",
            stage="run",
            status="ok",
            stream_scope="matrix",
            subject_id="HelloWorldObject",
            matrix_id="windows-reference-trace",
            goal_id="correctness.platform",
            stage_id="runtime-trace-compare",
            bucket="runtime",
            stage_scope="matrix",
        )

        self.assertEqual("stage-finished", event["eventType"])
        self.assertIn("timestampUtc", event)
        self.assertEqual("matrix", event["streamScope"])
        self.assertEqual("HelloWorldObject", event["subjectId"])
        self.assertEqual("windows-reference-trace", event["matrixId"])
        self.assertEqual("correctness.platform", event["goalId"])
        self.assertEqual("runtime-trace-compare", event["stageId"])
        self.assertEqual("runtime", event["bucket"])
        self.assertEqual("matrix", event["stageScope"])
        self.assertEqual("smoke/HelloWorld", event["suiteId"])
        self.assertEqual("run", event["stage"])
        self.assertEqual("20260404-120000-macos-abcd", event["payload"]["runId"])


if __name__ == "__main__":
    unittest.main()
