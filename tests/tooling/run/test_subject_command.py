from __future__ import annotations

import importlib.util
import sys
import unittest
from pathlib import Path
from unittest.mock import patch


REPO_ROOT = Path(__file__).resolve().parents[3]
TEST_COMMAND_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "commands" / "test.py"
MANIFEST_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "manifest.py"
RUN_MANIFEST_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "run_manifest.json"


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


class SubjectCommandTests(unittest.TestCase):
    def test_subject_dispatch_routes_to_subject_planner_and_executor(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "booming_run_test_command_subject_dispatch")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "booming_run_manifest_subject_dispatch")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        execution_result = {
            "subjectId": "HelloWorldObject",
            "matrixId": "windows-dev-output",
            "goalId": "correctness.dev",
            "status": "ok",
            "terminalStageId": "runtime-observe",
            "terminalBucket": "runtime",
            "stageResults": [
                {
                    "stageId": "source-resolve",
                    "kind": "source-resolve",
                    "bucket": "source",
                    "status": "ok",
                    "planMode": "executed",
                    "actionTaken": "executed",
                    "invalidation": {"applied": False, "reason": None},
                    "manifestPath": "artifacts/subjects/HelloWorldObject/shared/source/source.manifest.json",
                    "reportPaths": [],
                    "primaryEvidencePaths": [],
                    "fingerprint": "source-fingerprint",
                    "durationMs": 1,
                    "diagnostics": {},
                    "failure": None,
                },
                {
                    "stageId": "runtime-observe",
                    "kind": "runtime-observe",
                    "bucket": "runtime",
                    "status": "ok",
                    "planMode": "executed",
                    "actionTaken": "executed",
                    "invalidation": {"applied": False, "reason": None},
                    "manifestPath": "artifacts/subjects/HelloWorldObject/matrices/windows-dev-output/runtime/runtime.manifest.json",
                    "reportPaths": [],
                    "primaryEvidencePaths": [
                        "artifacts/subjects/HelloWorldObject/matrices/windows-dev-output/runtime/stdout.log",
                    ],
                    "fingerprint": "runtime-fingerprint",
                    "durationMs": 2,
                    "diagnostics": {},
                    "failure": None,
                },
            ],
            "errors": [],
            "events": [],
        }

        with patch.object(
            test_module,
            "_execute_public_test_session",
            side_effect=AssertionError("legacy suite session should not run for test subject"),
        ) as legacy_session:
            with patch.object(test_module.subject_executor_module, "execute_plan", return_value=execution_result) as execute_plan:
                result = test_module.handle(
                    {"id": "test-subject", "handler": "test.dispatch"},
                    REPO_ROOT,
                    "windows",
                    "test subject --id subject/HelloWorldObject",
                    manifest,
                    {"id": "subject/HelloWorldObject"},
                )

        self.assertEqual("ok", result.status)
        self.assertEqual("subject/HelloWorldObject", result.target)
        self.assertEqual("subject/HelloWorldObject", result.payload["selectedObject"]["id"])
        self.assertEqual(
            "artifacts/subjects/HelloWorldObject/subject-report/summary.json",
            result.payload["subjectResults"][0]["subjectSummaryPath"],
        )
        self.assertEqual("HelloWorldObject", result.payload["subjectResults"][0]["subjectId"])
        self.assertIn("artifacts/subjects/HelloWorldObject/matrices/windows-dev-output/report.json", result.payload["artifacts"])
        self.assertIn("artifacts/subjects/HelloWorldObject/subject-report/summary.json", result.payload["artifacts"])
        self.assertTrue((REPO_ROOT / result.payload["subjectResults"][0]["subjectSummaryPath"]).is_file())
        legacy_session.assert_not_called()
        execute_plan.assert_called_once()
        self.assertEqual(result.payload["runId"], execute_plan.call_args.kwargs["run_id"])
        self.assertTrue(callable(execute_plan.call_args.kwargs["event_writer"]))

    def test_subject_dispatch_collects_perf_release_report_artifacts_for_generic_echo(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "booming_run_test_command_subject_perf_dispatch")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "booming_run_manifest_subject_perf_dispatch")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        execution_result = {
            "subjectId": "GenericEcho",
            "matrixId": "windows-perf-release",
            "goalId": "perf.release",
            "status": "ok",
            "terminalStageId": "report-assemble",
            "terminalBucket": "report",
            "stageResults": [
                {
                    "stageId": "runtime-perf-collect",
                    "kind": "runtime-perf-collect",
                    "bucket": "runtime",
                    "status": "ok",
                    "planMode": "executed",
                    "actionTaken": "executed",
                    "invalidation": {"applied": False, "reason": None},
                    "manifestPath": "artifacts/subjects/GenericEcho/matrices/windows-perf-release/runtime/runtime.manifest.json",
                    "reportPaths": [],
                    "primaryEvidencePaths": [
                        "artifacts/subjects/GenericEcho/matrices/windows-perf-release/runtime/stdout.log",
                    ],
                    "fingerprint": "runtime-fingerprint",
                    "durationMs": 50,
                    "diagnostics": {},
                    "details": {
                        "performance": {
                            "samples": [
                                {"sampleIndex": 1, "durationMs": 12.0, "exitCode": 0},
                                {"sampleIndex": 2, "durationMs": 14.0, "exitCode": 0},
                            ],
                            "metrics": {
                                "sampleCount": 2,
                                "meanDurationMs": 13.0,
                                "minDurationMs": 12.0,
                                "maxDurationMs": 14.0,
                            },
                            "baselinePath": "tests/perf/subjects/GenericEcho/windows-perf-release/baselines/windows.json",
                            "baseline": {"meanDurationMs": 11.0},
                            "baselineUpdated": False,
                            "regressionStatus": "regressed",
                            "regressions": [
                                {"metric": "meanDurationMs", "baseline": 11.0, "actual": 13.0, "delta": 2.0}
                            ],
                        }
                    },
                    "failure": None,
                },
                {
                    "stageId": "report-assemble",
                    "kind": "report-assemble",
                    "bucket": "report",
                    "status": "ok",
                    "planMode": "executed",
                    "actionTaken": "executed",
                    "invalidation": {"applied": False, "reason": None},
                    "manifestPath": "artifacts/subjects/GenericEcho/matrices/windows-perf-release/report.json",
                    "reportPaths": [],
                    "primaryEvidencePaths": [],
                    "fingerprint": "report-fingerprint",
                    "durationMs": 1,
                    "diagnostics": {},
                    "failure": None,
                },
            ],
            "errors": [],
            "events": [],
        }

        with patch.object(test_module.subject_executor_module, "execute_plan", return_value=execution_result):
            result = test_module.handle(
                {"id": "test-subject", "handler": "test.dispatch"},
                REPO_ROOT,
                "windows",
                "test subject --id subject/GenericEcho --goal perf.release",
                manifest,
                {"id": "subject/GenericEcho", "goal": "perf.release"},
            )

        self.assertEqual("ok", result.status)
        self.assertEqual("subject/GenericEcho", result.target)
        self.assertIn("artifacts/subjects/GenericEcho/matrices/windows-perf-release/report.json", result.payload["artifacts"])
        self.assertIn(
            "artifacts/subjects/GenericEcho/matrices/windows-perf-release/report/summary.json",
            result.payload["artifacts"],
        )
        self.assertIn(
            "artifacts/subjects/GenericEcho/matrices/windows-perf-release/report/baseline-compare.json",
            result.payload["artifacts"],
        )
        self.assertIn(
            "artifacts/subjects/GenericEcho/matrices/windows-perf-release/report/samples.json",
            result.payload["artifacts"],
        )
        self.assertEqual("GenericEcho", result.payload["subjectResults"][0]["subjectId"])


if __name__ == "__main__":
    unittest.main()
