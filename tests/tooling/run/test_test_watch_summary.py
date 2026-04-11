from __future__ import annotations

import importlib.util
import json
import shutil
import sys
import unittest
import uuid
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
TEST_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "commands" / "test.py"
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp_test_watch_summary"


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
    def test_test_summary_can_resolve_subject_local_run_by_run_id(self) -> None:
        test_module = load_module(TEST_MODULE_PATH, "chaos_run_test_summary_subject_local")
        subject_id = "fixture-entry-alpha"
        suite_id = "fixture-suite-alpha"

        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)
        repo_root = TEST_TMP_ROOT / f"repo-{uuid.uuid4().hex}"
        repo_root.mkdir(parents=True, exist_ok=False)
        try:
            run_root = (
                repo_root
                / "artifacts"
                / "subjects"
                / subject_id
                / "runs"
                / "run-subject-1"
                / "run-report"
            )
            run_root.mkdir(parents=True, exist_ok=True)
            (run_root / "summary.json").write_text(
                json.dumps(
                    {
                        "runId": "run-subject-1",
                        "command": f"test subject --id subject/{subject_id}",
                        "finalStatus": "ok",
                        "phaseResults": [],
                        "subjectResults": [
                            {
                                "subjectId": subject_id,
                                "requestedGoalId": "correctness.platform",
                                "status": "ok",
                                "subjectSummaryPath": (
                                    f"artifacts/subjects/{subject_id}/runs/run-subject-1/subject-report/summary.json"
                                ),
                            }
                        ],
                        "suiteResults": [{"suiteId": suite_id, "status": "ok"}],
                        "errors": [],
                    }
                ),
                encoding="utf-8",
            )

            result = test_module.handle(
                {"id": "test-summary", "handler": "test.dispatch"},
                repo_root,
                "windows",
                "test summary --run run-subject-1",
                {},
                {"run": "run-subject-1"},
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

        self.assertEqual("ok", result.status)
        self.assertIn("run-subject-1", result.text or "")
        self.assertIn("Subject Breakdown:", result.text or "")
        self.assertIn(f"{subject_id}: goal=correctness.platform | status=ok", result.text or "")
        self.assertIn(f"ok: {suite_id}", result.text or "")

    def test_test_summary_reads_latest_summary(self) -> None:
        test_module = load_module(TEST_MODULE_PATH, "chaos_run_test_summary")
        subject_id = "fixture-entry-beta"
        suite_ids = ["fixture-suite-beta", "workflow/fixture-pipeline"]

        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)
        repo_root = TEST_TMP_ROOT / f"repo-{uuid.uuid4().hex}"
        repo_root.mkdir(parents=True, exist_ok=False)
        try:
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
                        "phaseResults": [
                            {"phaseId": "code", "status": "ok"},
                            {"phaseId": "module", "status": "ok"},
                        ],
                        "subjectResults": [
                            {
                                "subjectId": subject_id,
                                "requestedGoalId": "correctness.platform",
                                "status": "fail",
                                "subjectSummaryPath": (
                                    f"artifacts/subjects/{subject_id}/runs/run-1/subject-report/summary.json"
                                ),
                            }
                        ],
                        "suiteResults": [
                            {"suiteId": suite_ids[0], "status": "ok"},
                            {"suiteId": suite_ids[1], "status": "ok"},
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
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

        self.assertEqual("ok", result.status)
        self.assertIn("run-1", result.text or "")
        self.assertIn("Phases:", result.text or "")
        self.assertIn("ok: code", result.text or "")
        self.assertIn("Subject Breakdown:", result.text or "")
        self.assertIn(f"{subject_id}: goal=correctness.platform | status=fail", result.text or "")
        self.assertIn(suite_ids[1], result.text or "")

    def test_test_watch_reads_latest_events(self) -> None:
        test_module = load_module(TEST_MODULE_PATH, "chaos_run_test_watch")
        subject_id = "fixture-entry-gamma"

        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)
        repo_root = TEST_TMP_ROOT / f"repo-{uuid.uuid4().hex}"
        repo_root.mkdir(parents=True, exist_ok=False)
        try:
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
                        json.dumps(
                            {
                                "eventType": "final-summary",
                                "payload": {
                                    "finalStatus": "ok",
                                    "phaseResults": [
                                        {"phaseId": "code", "status": "ok"},
                                        {"phaseId": "module", "status": "ok"},
                                        {"phaseId": "system", "status": "ok"},
                                    ],
                                "subjectResults": [
                                        {
                                            "subjectId": subject_id,
                                            "requestedGoalId": "correctness.platform",
                                            "status": "ok",
                                            "subjectSummaryPath": (
                                                f"artifacts/subjects/{subject_id}/runs/run-2/subject-report/summary.json"
                                            ),
                                        }
                                    ],
                                },
                            }
                        ),
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
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

        self.assertEqual("ok", result.status)
        self.assertIn("Unified Test Progress", result.text or "")
        self.assertIn("50%", result.text or "")
        self.assertIn("Phases:", result.text or "")
        self.assertIn("ok: code", result.text or "")
        self.assertIn("ok: module", result.text or "")
        self.assertIn("ok: system", result.text or "")
        self.assertIn("Subjects:", result.text or "")
        self.assertIn(f"ok: {subject_id}", result.text or "")


if __name__ == "__main__":
    unittest.main()
