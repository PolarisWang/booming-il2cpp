from __future__ import annotations

import importlib.util
import unittest
from pathlib import Path

from tests.support import select_public_suite_spec


REPO_ROOT = Path(__file__).resolve().parents[3]
RESULT_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "result.py"


def load_result_module():
    if not RESULT_MODULE_PATH.is_file():
        raise FileNotFoundError(f"result module missing: {RESULT_MODULE_PATH}")

    spec = importlib.util.spec_from_file_location("chaos_run_result", RESULT_MODULE_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load result module: {RESULT_MODULE_PATH}")

    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


class JsonOutputTests(unittest.TestCase):
    def test_success_payload_has_stable_json_shape(self) -> None:
        result_module = load_result_module()
        result = result_module.CommandResult.success(
            command="list",
            host_platform="windows",
            target=None,
            payload={"items": [{"id": "list"}]},
        )

        serialized = result.to_dict()

        self.assertEqual("list", serialized["command"])
        self.assertEqual("ok", serialized["status"])
        self.assertEqual("windows", serialized["hostPlatform"])
        self.assertIsNone(serialized["target"])
        self.assertIn("durationMs", serialized)
        self.assertIn("checks", serialized)
        self.assertIn("errors", serialized)
        self.assertEqual([], serialized["errors"])

    def test_failure_payload_keeps_stable_json_shape(self) -> None:
        result_module = load_result_module()
        result = result_module.CommandResult.failure(
            command="capability",
            host_platform="windows",
            target="missing",
            errors=["unknown capability"],
        )

        serialized = result.to_dict()

        self.assertEqual("capability", serialized["command"])
        self.assertEqual("error", serialized["status"])
        self.assertEqual("windows", serialized["hostPlatform"])
        self.assertEqual("missing", serialized["target"])
        self.assertEqual(["unknown capability"], serialized["errors"])
        self.assertIn("checks", serialized)
        self.assertIn("durationMs", serialized)

    def test_success_payload_can_carry_run_summary_metadata(self) -> None:
        result_module = load_result_module()
        suite_spec = select_public_suite_spec(
            "chaos_json_output_suite",
            host_platform="macos",
            family="smoke",
            required_stages=["all"],
        )
        result = result_module.CommandResult.success(
            command=f"test {suite_spec['family']} {suite_spec['suite']}",
            host_platform="macos",
            target=str(suite_spec["id"]),
            payload={
                "runId": "20260404-120000-macos-abcd",
                "summaryPath": "artifacts/logs/tests/20260404-120000-macos-abcd/summary.json",
            },
        )

        serialized = result.to_dict()

        self.assertEqual("20260404-120000-macos-abcd", serialized["runId"])
        self.assertTrue(serialized["summaryPath"].endswith("summary.json"))


if __name__ == "__main__":
    unittest.main()
