from __future__ import annotations

import importlib.util
import sys
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
TRAFFIC_LIGHT_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "traffic_light.py"


def load_traffic_light_module():
    if not TRAFFIC_LIGHT_MODULE_PATH.is_file():
        raise FileNotFoundError(f"traffic light module missing: {TRAFFIC_LIGHT_MODULE_PATH}")

    spec = importlib.util.spec_from_file_location("booming_run_traffic_light", TRAFFIC_LIGHT_MODULE_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load traffic light module: {TRAFFIC_LIGHT_MODULE_PATH}")

    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


class TrafficLightTests(unittest.TestCase):
    def test_assign_traffic_lights_rejects_overlapping_selectors(self) -> None:
        traffic_light_module = load_traffic_light_module()
        cases = [
            {"caseId": "happy", "sourceId": "suite", "tags": ["smoke", "managed"], "status": "ok"},
            {"caseId": "sad", "sourceId": "suite", "tags": ["smoke", "negative"], "status": "fail"},
        ]

        with self.assertRaises(traffic_light_module.TrafficLightConfigError):
            traffic_light_module.assign_traffic_lights(
                cases,
                {
                    "green": {"tags": ["smoke"]},
                    "red": {"caseIds": ["happy"]},
                    "yellow": {"caseIds": ["sad"]},
                },
                family="smoke",
            )

    def test_assign_traffic_lights_counts_cases_with_include_and_exclude(self) -> None:
        traffic_light_module = load_traffic_light_module()
        cases = [
            {"caseId": "happy", "sourceId": "suite", "tags": ["smoke", "managed"], "status": "ok"},
            {"caseId": "sad", "sourceId": "suite", "tags": ["smoke", "negative"], "status": "fail"},
            {"caseId": "flaky", "sourceId": "suite", "tags": ["smoke", "flaky"], "status": "skip"},
        ]

        assigned = traffic_light_module.assign_traffic_lights(
            cases,
            {
                "green": {"tags": ["smoke"], "exclude": [{"tags": ["negative"]}, {"tags": ["flaky"]}]},
                "red": {"include": [{"tags": ["negative"]}]},
                "yellow": {"caseIds": ["flaky"]},
            },
            family="smoke",
        )

        self.assertTrue(assigned["enforced"])
        self.assertEqual(["green", "red", "yellow"], [item["trafficLight"] for item in assigned["caseResults"]])
        self.assertEqual(1, assigned["trafficLightCounts"]["green"]["ok"])
        self.assertEqual(1, assigned["trafficLightCounts"]["red"]["fail"])
        self.assertEqual(1, assigned["trafficLightCounts"]["yellow"]["skip"])

    def test_workflow_family_defaults_to_non_enforced_green_bucket(self) -> None:
        traffic_light_module = load_traffic_light_module()
        assigned = traffic_light_module.assign_traffic_lights(
            [{"caseId": "main", "sourceId": "workflow", "tags": ["workflow"], "status": "ok"}],
            None,
            family="workflow",
        )

        self.assertFalse(assigned["enforced"])
        self.assertEqual("green", assigned["caseResults"][0]["trafficLight"])


if __name__ == "__main__":
    unittest.main()
