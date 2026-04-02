from __future__ import annotations

import importlib.util
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
RESULT_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "result.py"


def load_result_module():
    if not RESULT_MODULE_PATH.is_file():
        raise FileNotFoundError(f"result module missing: {RESULT_MODULE_PATH}")

    spec = importlib.util.spec_from_file_location("booming_run_result", RESULT_MODULE_PATH)
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


if __name__ == "__main__":
    unittest.main()
