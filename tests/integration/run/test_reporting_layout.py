from __future__ import annotations

import importlib.util
import json
import shutil
import sys
import unittest
import uuid
from pathlib import Path
from unittest.mock import patch


REPO_ROOT = Path(__file__).resolve().parents[3]
REPORTING_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "reporting.py"
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp_test_reporting_layout"


def load_reporting_module():
    if not REPORTING_MODULE_PATH.is_file():
        raise FileNotFoundError(f"reporting module missing: {REPORTING_MODULE_PATH}")

    spec = importlib.util.spec_from_file_location("chaos_run_reporting", REPORTING_MODULE_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load reporting module: {REPORTING_MODULE_PATH}")

    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


class ReportingLayoutTests(unittest.TestCase):
    def test_reporting_layout_writes_fixed_session_files(self) -> None:
        reporting_module = load_reporting_module()
        subject_id = "fixture-layout-entry"

        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)
        repo_root = TEST_TMP_ROOT / f"repo-{uuid.uuid4().hex}"
        repo_root.mkdir(parents=True, exist_ok=False)
        try:
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
                phase_results=[
                    {
                        "phaseId": "code",
                        "title": "Code tests",
                        "status": "ok",
                        "memberCounts": {"total": 1, "ok": 1, "fail": 0, "aborted": 0},
                        "memberResults": [
                            {
                                "objectId": "smoke/HelloWorld",
                                "objectType": "suite",
                                "status": "ok",
                                "resolvedSuiteIds": ["smoke/HelloWorld"],
                            }
                        ],
                    }
                ],
                text="managed smoke ok\n",
                errors=[],
                artifacts=[],
                subject_results=[
                    {
                        "subjectId": subject_id,
                        "requestedGoalId": "correctness.platform",
                        "status": "fail",
                        "matrixStatusCounts": {"total": 2, "ok": 1, "fail": 1, "skip": 0, "aborted": 0},
                        "subjectSummaryPath": f"artifacts/subjects/{subject_id}/runs/run-1/subject-report/summary.json",
                    }
                ],
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
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

        self.assertEqual("final-summary", events[-1]["eventType"])
        self.assertNotIn("suiteResults", events[-1]["payload"])
        self.assertEqual(1, summary["subjectStatusCounts"]["fail"])
        self.assertEqual(subject_id, summary["subjectResults"][0]["subjectId"])
        self.assertEqual(subject_id, events[-1]["payload"]["subjectResults"][0]["subjectId"])
        self.assertEqual("code", summary["phaseResults"][0]["phaseId"])
        self.assertEqual("code", events[-1]["payload"]["phaseResults"][0]["phaseId"])
        self.assertEqual(1, summary["trafficLightCounts"]["green"]["ok"])
        self.assertEqual(1, summary["trafficLightCounts"]["red"]["fail"])
        self.assertEqual(1, summary["trafficLightCounts"]["yellow"]["skip"])
        self.assertEqual("green", suite_report["caseResults"][0]["trafficLight"])

    def test_reporting_layout_rewrites_current_pointer_when_delete_is_denied(self) -> None:
        reporting_module = load_reporting_module()

        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)
        repo_root = TEST_TMP_ROOT / f"repo-{uuid.uuid4().hex}"
        repo_root.mkdir(parents=True, exist_ok=False)
        try:
            subject_root = repo_root / "artifacts" / "subjects" / "fixture-layout-entry" / "runs"
            global_root = repo_root / "artifacts" / "logs" / "tests"
            run_context = reporting_module.start_session_report(
                repo_root=repo_root,
                host_platform="windows",
                command_text="test subject",
                run_id="run-1",
                session_root=subject_root / "run-1" / "run-report",
                pointer_roots=[subject_root, global_root],
            )

            with patch("pathlib.Path.unlink", side_effect=PermissionError(13, "Access is denied")):
                reporting_module.write_session_report(
                    repo_root=repo_root,
                    host_platform="windows",
                    command_text="test subject",
                    status="ok",
                    suite_results=[],
                    phase_results=[],
                    text="",
                    errors=[],
                    artifacts=[],
                    subject_results=[],
                    run_context=run_context,
                )

            subject_current = json.loads((subject_root / "current.json").read_text(encoding="utf-8"))
            global_current = json.loads((global_root / "current.json").read_text(encoding="utf-8"))
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

        self.assertEqual("ok", subject_current["status"])
        self.assertEqual("run-1", subject_current["runId"])
        self.assertEqual("ok", global_current["status"])
        self.assertEqual("run-1", global_current["runId"])


if __name__ == "__main__":
    unittest.main()
