from __future__ import annotations

import importlib.util
import json
import shutil
import sys
import unittest
import uuid
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
SUBJECT_REPORTING_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subject_reporting.py"
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "subject-reporting"


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


class SubjectReportingTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)

    def test_build_matrix_report_keeps_selection_and_stage_diagnostics(self) -> None:
        reporting_module = load_module(SUBJECT_REPORTING_MODULE_PATH, "booming_subject_reporting_matrix")

        plan = {
            "selection": {
                "subjectId": "HelloWorldObject",
                "displayName": "HelloWorldObject",
                "goalId": "correctness.platform",
                "matrixId": "windows-reference-trace",
                "pipelineId": "proof-runtime-trace",
                "source": {
                    "type": "dotnet-project",
                    "path": "subjects/HelloWorldObject/source/HelloWorldObject.csproj",
                    "entry": "HelloWorldObject/Program::Main(System.String[])",
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                    "toolchainProfile": "msvc-reference",
                    "runtimeProfile": "reference-trace",
                },
                "validationIntent": {
                    "validationMode": "trace",
                    "adaptationLevel": "traceable",
                    "expectedOutcome": "pass",
                },
                "artifactPlan": {
                    "requiredBuckets": ["source", "analysis", "runtime", "report"],
                    "evidenceTerminalBucket": "runtime",
                },
            },
            "stagePlan": [
                {
                    "stageId": "source-resolve",
                    "kind": "source-resolve",
                    "scope": "shared",
                    "bucket": "source",
                },
                {
                    "stageId": "analysis-frontend",
                    "kind": "analysis-frontend",
                    "scope": "shared",
                    "bucket": "analysis",
                },
                {
                    "stageId": "runtime-trace-compare",
                    "kind": "runtime-trace-compare",
                    "scope": "matrix",
                    "bucket": "runtime",
                },
                {
                    "stageId": "report-assemble",
                    "kind": "report-assemble",
                    "scope": "matrix",
                    "bucket": "report",
                },
            ],
        }
        execution_result = {
            "subjectId": "HelloWorldObject",
            "matrixId": "windows-reference-trace",
            "goalId": "correctness.platform",
            "status": "ok",
            "terminalStageId": "runtime-trace-compare",
            "terminalBucket": "runtime",
            "stageResults": [
                {
                    "stageId": "source-resolve",
                    "kind": "source-resolve",
                    "bucket": "source",
                    "status": "ok",
                    "planMode": "reused",
                    "actionTaken": "reused",
                    "invalidation": {"applied": False, "reason": None},
                    "manifestPath": "artifacts/subjects/HelloWorldObject/shared/source/source.manifest.json",
                    "reportPaths": [],
                    "primaryEvidencePaths": ["subjects/HelloWorldObject/source/HelloWorldObject.csproj"],
                    "fingerprint": "f-source",
                    "durationMs": 0,
                    "diagnostics": {"stdoutPath": None, "stderrPath": None},
                    "failure": None,
                },
                {
                    "stageId": "analysis-frontend",
                    "kind": "analysis-frontend",
                    "bucket": "analysis",
                    "status": "ok",
                    "planMode": "invalidated",
                    "actionTaken": "executed",
                    "invalidation": {"applied": True, "reason": "fingerprint-mismatch:driver-changed"},
                    "manifestPath": "artifacts/subjects/HelloWorldObject/shared/analysis/analysis.manifest.json",
                    "reportPaths": [
                        "artifacts/subjects/HelloWorldObject/shared/analysis/contract-validate.report.json"
                    ],
                    "primaryEvidencePaths": [
                        "artifacts/subjects/HelloWorldObject/shared/analysis/typed-il-ir.json"
                    ],
                    "fingerprint": "f-analysis",
                    "durationMs": 3280,
                    "diagnostics": {"stdoutPath": None, "stderrPath": None},
                    "failure": None,
                },
                {
                    "stageId": "runtime-trace-compare",
                    "kind": "runtime-trace-compare",
                    "bucket": "runtime",
                    "status": "ok",
                    "planMode": "executed",
                    "actionTaken": "executed",
                    "invalidation": {"applied": False, "reason": None},
                    "manifestPath": "artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/runtime/runtime.manifest.json",
                    "reportPaths": [
                        "artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/runtime/trace-compare.report.json"
                    ],
                    "primaryEvidencePaths": [
                        "artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/runtime/trace.runtime.json"
                    ],
                    "fingerprint": "f-runtime",
                    "durationMs": 540,
                    "diagnostics": {
                        "stdoutPath": "artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/runtime/stdout.log",
                        "stderrPath": "artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/runtime/stderr.log",
                    },
                    "failure": None,
                },
            ],
            "errors": [],
        }

        report = reporting_module.build_matrix_report(
            plan,
            execution_result,
            run_id="20260406-hello-001",
            generated_at="2026-04-06T14:20:00Z",
        )

        self.assertEqual("v1", report["reportVersion"])
        self.assertEqual("20260406-hello-001", report["runId"])
        self.assertEqual("HelloWorldObject", report["subjectId"])
        self.assertEqual("windows-reference-trace", report["matrixId"])
        self.assertEqual("proof-runtime-trace", report["selection"]["pipelineId"])
        self.assertEqual("runtime", report["terminalBucket"])
        self.assertEqual(
            [
                "source-resolve",
                "analysis-frontend",
                "runtime-trace-compare",
            ],
            [stage["stageId"] for stage in report["stageResults"]],
        )
        self.assertEqual(
            ["artifacts/subjects/HelloWorldObject/shared/analysis/typed-il-ir.json"],
            report["stageResults"][1]["primaryEvidencePaths"],
        )
        self.assertEqual(
            "fingerprint-mismatch:driver-changed",
            report["stageResults"][1]["invalidation"]["reason"],
        )
        self.assertEqual(
            "artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/runtime/runtime.manifest.json",
            report["artifactResults"][-1]["manifestPath"],
        )
        self.assertEqual("runtime-trace-compare", report["artifactResults"][-1]["producedByStageId"])

    def test_build_matrix_report_surfaces_perf_metrics_and_release_report_paths(self) -> None:
        reporting_module = load_module(SUBJECT_REPORTING_MODULE_PATH, "booming_subject_reporting_perf")

        plan = {
            "selection": {
                "subjectId": "GenericEcho",
                "displayName": "GenericEcho",
                "goalId": "perf.release",
                "matrixId": "windows-perf-release",
                "pipelineId": "managed-runtime-perf",
                "source": {
                    "type": "dotnet-project",
                    "path": "subjects/GenericEcho/source/GenericEcho.csproj",
                    "entry": "GenericEcho/Program::Main()",
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                    "toolchainProfile": "dotnet-managed",
                    "runtimeProfile": "managed-perf-release",
                },
                "validationIntent": {
                    "validationMode": "perf",
                    "adaptationLevel": "managed-runtime",
                    "expectedOutcome": "pass",
                },
                "artifactPlan": {
                    "requiredBuckets": ["source", "host-input", "runtime", "report"],
                    "evidenceTerminalBucket": "report",
                },
            },
            "stagePlan": [
                {"stageId": "source-resolve", "kind": "source-resolve", "scope": "shared", "bucket": "source"},
                {"stageId": "host-input-build", "kind": "host-input-build", "scope": "shared", "bucket": "host-input"},
                {"stageId": "runtime-perf-collect", "kind": "runtime-perf-collect", "scope": "matrix", "bucket": "runtime"},
                {"stageId": "report-assemble", "kind": "report-assemble", "scope": "matrix", "bucket": "report"},
            ],
        }
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
                        "artifacts/subjects/GenericEcho/matrices/windows-perf-release/runtime/stdout.log"
                    ],
                    "fingerprint": "f-runtime",
                    "durationMs": 120,
                    "diagnostics": {
                        "stdoutPath": "artifacts/subjects/GenericEcho/matrices/windows-perf-release/runtime/stdout.log",
                        "stderrPath": "artifacts/subjects/GenericEcho/matrices/windows-perf-release/runtime/stderr.log",
                    },
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
                }
            ],
            "errors": [],
        }

        repo_root = TEST_TMP_ROOT / f"perf-{uuid.uuid4().hex}"
        repo_root.mkdir(parents=True, exist_ok=False)
        try:
            report = reporting_module.build_matrix_report(
                plan,
                execution_result,
                run_id="20260407-generic-001",
                generated_at="2026-04-07T03:20:00Z",
            )
            release_paths = reporting_module.materialize_matrix_report_artifacts(
                repo_root,
                matrix_report_path="artifacts/subjects/GenericEcho/matrices/windows-perf-release/report.json",
                matrix_report=report,
            )

            self.assertEqual(
                {"sampleCount": 2, "meanDurationMs": 13.0, "minDurationMs": 12.0, "maxDurationMs": 14.0},
                report["metrics"],
            )
            self.assertEqual(
                {
                    "path": "tests/perf/subjects/GenericEcho/windows-perf-release/baselines/windows.json",
                    "metrics": {"meanDurationMs": 11.0},
                    "updated": False,
                    "regressions": [{"metric": "meanDurationMs", "baseline": 11.0, "actual": 13.0, "delta": 2.0}],
                },
                report["baseline"],
            )
            self.assertEqual("regressed", report["regressionStatus"])
            self.assertEqual(
                [
                    "artifacts/subjects/GenericEcho/matrices/windows-perf-release/report/summary.json",
                    "artifacts/subjects/GenericEcho/matrices/windows-perf-release/report/baseline-compare.json",
                    "artifacts/subjects/GenericEcho/matrices/windows-perf-release/report/samples.json",
                ],
                release_paths,
            )
            self.assertEqual(release_paths, report["releaseReportPaths"])

            samples_path = repo_root / "artifacts" / "subjects" / "GenericEcho" / "matrices" / "windows-perf-release" / "report" / "samples.json"
            self.assertTrue(samples_path.is_file())
            samples_payload = json.loads(samples_path.read_text(encoding="utf-8"))
            self.assertEqual(2, len(samples_payload["samples"]))
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_build_subject_summary_and_subject_result_aggregate_matrix_reports(self) -> None:
        reporting_module = load_module(SUBJECT_REPORTING_MODULE_PATH, "booming_subject_reporting_summary")

        matrix_reports = [
            {
                "subjectId": "HelloWorldObject",
                "goalId": "correctness.platform",
                "matrixId": "windows-reference-trace",
                "status": "ok",
                "terminalBucket": "runtime",
                "selection": {
                    "executionContext": {
                        "hostPlatform": "windows-x64",
                        "targetPlatform": "windows-x64",
                        "toolchainProfile": "msvc-reference",
                        "runtimeProfile": "reference-trace",
                    }
                },
            },
            {
                "subjectId": "HelloWorldObject",
                "goalId": "correctness.platform",
                "matrixId": "windows-linux-buildable",
                "status": "fail",
                "terminalBucket": "build",
                "selection": {
                    "executionContext": {
                        "hostPlatform": "windows-x64",
                        "targetPlatform": "linux-x64",
                        "toolchainProfile": "linux-cross",
                        "runtimeProfile": "platform-build",
                    }
                },
            },
        ]
        matrix_report_paths = {
            "windows-reference-trace": "artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/report.json",
            "windows-linux-buildable": "artifacts/subjects/HelloWorldObject/matrices/windows-linux-buildable/report.json",
        }

        summary = reporting_module.build_subject_summary(
            subject_id="HelloWorldObject",
            requested_goal_id="correctness.platform",
            matrix_reports=matrix_reports,
            matrix_report_paths=matrix_report_paths,
            run_id="20260406-hello-001",
            generated_at="2026-04-06T14:20:05Z",
        )
        subject_result = reporting_module.build_subject_result(
            summary,
            subject_summary_path="artifacts/subjects/HelloWorldObject/subject-report/summary.json",
        )

        self.assertEqual("v1", summary["summaryVersion"])
        self.assertEqual("fail", summary["status"])
        self.assertEqual(2, summary["matrixStatusCounts"]["total"])
        self.assertEqual(1, summary["matrixStatusCounts"]["ok"])
        self.assertEqual(1, summary["matrixStatusCounts"]["fail"])
        self.assertEqual(
            "artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/report.json",
            summary["matrixResults"][0]["reportPath"],
        )
        self.assertEqual("windows-x64", summary["matrixResults"][0]["executionContext"]["targetPlatform"])
        self.assertEqual("fail", subject_result["status"])
        self.assertEqual(
            "artifacts/subjects/HelloWorldObject/subject-report/summary.json",
            subject_result["subjectSummaryPath"],
        )
        self.assertEqual(2, subject_result["matrixStatusCounts"]["total"])


if __name__ == "__main__":
    unittest.main()
