from __future__ import annotations

import importlib.util
import json
import sys
import tempfile
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
REPORTING_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "reporting.py"


def load_reporting_module():
    if not REPORTING_MODULE_PATH.is_file():
        raise FileNotFoundError(f"reporting module missing: {REPORTING_MODULE_PATH}")

    spec = importlib.util.spec_from_file_location("booming_run_reporting", REPORTING_MODULE_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load reporting module: {REPORTING_MODULE_PATH}")

    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


class ReportingLayoutTests(unittest.TestCase):
    def test_reporting_layout_writes_fixed_session_files(self) -> None:
        reporting_module = load_reporting_module()

        with tempfile.TemporaryDirectory() as temp_dir:
            repo_root = Path(temp_dir)
            report = reporting_module.write_session_report(
                repo_root=repo_root,
                host_platform="macos",
                command_text="test smoke HelloWorld",
                status="ok",
                suite_results=[
                    {
                        "suiteId": "smoke/HelloWorld",
                        "status": "ok",
                        "stageResults": {
                            "build": {"status": "ok"},
                            "setup": {"status": "not-requested"},
                            "run": {"status": "ok"},
                            "report": {"status": "not-requested"},
                        },
                        "artifacts": [],
                        "caseResults": [
                            {"caseId": "happy", "status": "ok", "trafficLight": "green"},
                            {"caseId": "sad", "status": "fail", "trafficLight": "red"},
                            {"caseId": "flaky", "status": "skip", "trafficLight": "yellow"},
                        ],
                    }
                ],
                text="managed smoke ok\n",
                errors=[],
                artifacts=[],
            )

            root = repo_root / "artifacts" / "logs" / "tests" / report["runId"]
            self.assertTrue((root / "session.json").is_file())
            self.assertTrue((root / "summary.json").is_file())
            self.assertTrue((root / "events.jsonl").is_file())
            summary = json.loads((root / "summary.json").read_text(encoding="utf-8"))
            suite_report = json.loads(
                (root / "suites" / "smoke" / "HelloWorld" / "report.json").read_text(encoding="utf-8")
            )

            events = [
                json.loads(line)
                for line in (root / "events.jsonl").read_text(encoding="utf-8").splitlines()
                if line.strip()
            ]

        self.assertEqual("final-summary", events[-1]["eventType"])
        self.assertNotIn("suiteResults", events[-1]["payload"])
        self.assertEqual(1, summary["trafficLightCounts"]["green"]["ok"])
        self.assertEqual(1, summary["trafficLightCounts"]["red"]["fail"])
        self.assertEqual(1, summary["trafficLightCounts"]["yellow"]["skip"])
        self.assertEqual("green", suite_report["caseResults"][0]["trafficLight"])


if __name__ == "__main__":
    unittest.main()
