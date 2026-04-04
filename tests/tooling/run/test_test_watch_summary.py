from __future__ import annotations

import importlib.util
import json
import sys
import tempfile
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
TEST_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "commands" / "test.py"


def load_module(path: Path, module_name: str):
    if not path.is_file():
        raise FileNotFoundError(f"module missing: {path}")

    spec = importlib.util.spec_from_file_location(module_name, path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load module: {path}")

    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


class TestWatchSummaryCommandsTests(unittest.TestCase):
    def test_test_summary_reads_latest_summary(self) -> None:
        test_module = load_module(TEST_MODULE_PATH, "booming_run_test_summary")

        with tempfile.TemporaryDirectory() as temp_dir:
            repo_root = Path(temp_dir)
            logs_root = repo_root / "artifacts" / "logs" / "tests"
            run_root = logs_root / "run-1"
            run_root.mkdir(parents=True, exist_ok=True)
            (logs_root / "last.json").write_text(
                json.dumps({"runId": "run-1", "summaryPath": "artifacts/logs/tests/run-1/summary.json"}),
                encoding="utf-8",
            )
            (run_root / "summary.json").write_text(
                json.dumps(
                    {
                        "runId": "run-1",
                        "command": "test all",
                        "finalStatus": "ok",
                        "suiteResults": [
                            {"suiteId": "smoke/HelloWorld", "status": "ok"},
                            {"suiteId": "workflow/roadmap-0-macos", "status": "ok"},
                        ],
                        "errors": [],
                    }
                ),
                encoding="utf-8",
            )

            result = test_module.handle(
                {"id": "test-summary", "handler": "test.dispatch"},
                repo_root,
                "macos",
                "test summary",
                {},
                {},
            )

        self.assertEqual("ok", result.status)
        self.assertIn("run-1", result.text or "")
        self.assertIn("workflow/roadmap-0-macos", result.text or "")

    def test_test_watch_reads_latest_events(self) -> None:
        test_module = load_module(TEST_MODULE_PATH, "booming_run_test_watch")

        with tempfile.TemporaryDirectory() as temp_dir:
            repo_root = Path(temp_dir)
            logs_root = repo_root / "artifacts" / "logs" / "tests"
            run_root = logs_root / "run-2"
            run_root.mkdir(parents=True, exist_ok=True)
            (logs_root / "current.json").write_text(
                json.dumps({"runId": "run-2", "eventsPath": "artifacts/logs/tests/run-2/events.jsonl"}),
                encoding="utf-8",
            )
            (run_root / "events.jsonl").write_text(
                "\n".join(
                    [
                        json.dumps({"eventType": "session-start", "payload": {"command": "test all"}}),
                        json.dumps({"eventType": "progress", "payload": {"completedUnits": 2, "totalUnits": 4}}),
                    ]
                )
                + "\n",
                encoding="utf-8",
            )

            result = test_module.handle(
                {"id": "test-watch", "handler": "test.dispatch"},
                repo_root,
                "macos",
                "test watch",
                {},
                {},
            )

        self.assertEqual("ok", result.status)
        self.assertIn("Unified Test Progress", result.text or "")
        self.assertIn("50%", result.text or "")


if __name__ == "__main__":
    unittest.main()
