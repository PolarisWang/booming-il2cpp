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
TRACE_SUBJECT_ID = "FixtureTraceSubject"
TRACE_MATRIX_ID = "windows-reference-trace"
TRACE_GOAL_ID = "correctness.platform"
PERF_SUBJECT_ID = "FixturePerfSubject"
PERF_MATRIX_ID = "windows-perf-release"
PERF_GOAL_ID = "perf.release"


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


def run_bucket_path(subject_id: str, run_id: str, *parts: str) -> str:
    return Path("artifacts", "subjects", subject_id, "runs", run_id, *parts).as_posix()


def source_project_path(subject_id: str) -> str:
    return Path("subjects", subject_id, "source", f"{subject_id}.csproj").as_posix()


def source_entry(subject_id: str) -> str:
    return f"{subject_id}/Program::Main(System.String[])"


def perf_baseline_path(subject_id: str, matrix_id: str) -> str:
    return Path("subjects", subject_id, "baselines", "perf", matrix_id, "windows.json").as_posix()


class SubjectReportingTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)

    def test_build_matrix_report_keeps_selection_and_stage_diagnostics(self) -> None:
        reporting_module = load_module(SUBJECT_REPORTING_MODULE_PATH, "chaos_subject_reporting_matrix")
        run_id = "20260406-fixture-trace-001"

        plan = {
            "selection": {
                "subjectId": TRACE_SUBJECT_ID,
                "displayName": TRACE_SUBJECT_ID,
                "goalId": TRACE_GOAL_ID,
                "matrixId": TRACE_MATRIX_ID,
                "validationProfileId": "trace-platform",
                "validationKinds": ["proof"],
                "validationKind": "proof",
                "variant": "CHECK",
                "pipelineId": "proof-runtime-trace",
                "source": {
                    "type": "dotnet-project",
                    "path": source_project_path(TRACE_SUBJECT_ID),
                    "entry": source_entry(TRACE_SUBJECT_ID),
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
            "subjectId": TRACE_SUBJECT_ID,
            "matrixId": TRACE_MATRIX_ID,
            "goalId": TRACE_GOAL_ID,
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
                    "manifestPath": run_bucket_path(TRACE_SUBJECT_ID, run_id, "analysis", "source", "source.manifest.json"),
                    "reportPaths": [],
                    "primaryEvidencePaths": [source_project_path(TRACE_SUBJECT_ID)],
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
                    "manifestPath": run_bucket_path(TRACE_SUBJECT_ID, run_id, "analysis", "analysis", "analysis.manifest.json"),
                    "reportPaths": [
                        run_bucket_path(TRACE_SUBJECT_ID, run_id, "analysis", "analysis", "contract-validate.report.json")
                    ],
                    "primaryEvidencePaths": [
                        run_bucket_path(TRACE_SUBJECT_ID, run_id, "analysis", "analysis", "typed-il-ir.json")
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
                    "manifestPath": run_bucket_path(TRACE_SUBJECT_ID, run_id, "matrices", TRACE_MATRIX_ID, "runtime", "runtime.manifest.json"),
                    "reportPaths": [
                        run_bucket_path(TRACE_SUBJECT_ID, run_id, "matrices", TRACE_MATRIX_ID, "runtime", "trace-compare.report.json")
                    ],
                    "primaryEvidencePaths": [
                        run_bucket_path(TRACE_SUBJECT_ID, run_id, "matrices", TRACE_MATRIX_ID, "runtime", "trace.runtime.json")
                    ],
                    "fingerprint": "f-runtime",
                    "durationMs": 540,
                    "diagnostics": {
                        "stdoutPath": run_bucket_path(TRACE_SUBJECT_ID, run_id, "matrices", TRACE_MATRIX_ID, "runtime", "stdout.log"),
                        "stderrPath": run_bucket_path(TRACE_SUBJECT_ID, run_id, "matrices", TRACE_MATRIX_ID, "runtime", "stderr.log"),
                    },
                    "failure": None,
                },
            ],
            "errors": [],
        }

        report = reporting_module.build_matrix_report(
            plan,
            execution_result,
            run_id=run_id,
            generated_at="2026-04-06T14:20:00Z",
        )

        self.assertEqual("v1", report["reportVersion"])
        self.assertEqual(run_id, report["runId"])
        self.assertEqual(TRACE_SUBJECT_ID, report["subjectId"])
        self.assertEqual(TRACE_MATRIX_ID, report["matrixId"])
        self.assertEqual("trace-platform", report["validationProfileId"])
        self.assertEqual(["proof"], report["validationKinds"])
        self.assertEqual("proof", report["validationKind"])
        self.assertEqual("CHECK", report["variant"])
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
            [run_bucket_path(TRACE_SUBJECT_ID, run_id, "analysis", "analysis", "typed-il-ir.json")],
            report["stageResults"][1]["primaryEvidencePaths"],
        )
        self.assertEqual(
            "fingerprint-mismatch:driver-changed",
            report["stageResults"][1]["invalidation"]["reason"],
        )
        self.assertEqual(
            run_bucket_path(TRACE_SUBJECT_ID, run_id, "matrices", TRACE_MATRIX_ID, "runtime", "runtime.manifest.json"),
            report["artifactResults"][-1]["manifestPath"],
        )
        self.assertEqual("runtime-trace-compare", report["artifactResults"][-1]["producedByStageId"])

    def test_build_matrix_report_surfaces_perf_metrics_and_release_report_paths(self) -> None:
        reporting_module = load_module(SUBJECT_REPORTING_MODULE_PATH, "chaos_subject_reporting_perf")
        run_id = "20260407-fixture-perf-001"

        plan = {
            "selection": {
                "subjectId": PERF_SUBJECT_ID,
                "displayName": PERF_SUBJECT_ID,
                "goalId": PERF_GOAL_ID,
                "matrixId": PERF_MATRIX_ID,
                "validationProfileId": "perf-release",
                "validationKinds": ["perf"],
                "validationKind": "perf",
                "variant": "SHIP",
                "pipelineId": "managed-runtime-perf",
                "source": {
                    "type": "dotnet-project",
                    "path": source_project_path(PERF_SUBJECT_ID),
                    "entry": source_entry(PERF_SUBJECT_ID),
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
            "subjectId": PERF_SUBJECT_ID,
            "matrixId": PERF_MATRIX_ID,
            "goalId": PERF_GOAL_ID,
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
                    "manifestPath": run_bucket_path(PERF_SUBJECT_ID, run_id, "matrices", PERF_MATRIX_ID, "runtime", "runtime.manifest.json"),
                    "reportPaths": [],
                    "primaryEvidencePaths": [
                        run_bucket_path(PERF_SUBJECT_ID, run_id, "matrices", PERF_MATRIX_ID, "runtime", "stdout.log")
                    ],
                    "fingerprint": "f-runtime",
                    "durationMs": 120,
                    "diagnostics": {
                        "stdoutPath": run_bucket_path(PERF_SUBJECT_ID, run_id, "matrices", PERF_MATRIX_ID, "runtime", "stdout.log"),
                        "stderrPath": run_bucket_path(PERF_SUBJECT_ID, run_id, "matrices", PERF_MATRIX_ID, "runtime", "stderr.log"),
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
                            "baselinePath": perf_baseline_path(PERF_SUBJECT_ID, PERF_MATRIX_ID),
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
                run_id=run_id,
                generated_at="2026-04-07T03:20:00Z",
            )
            release_paths = reporting_module.materialize_matrix_report_artifacts(
                repo_root,
                matrix_report_path=run_bucket_path(PERF_SUBJECT_ID, run_id, "matrices", PERF_MATRIX_ID, "pipeline-report", "report.json"),
                matrix_report=report,
            )

            self.assertEqual(
                {"sampleCount": 2, "meanDurationMs": 13.0, "minDurationMs": 12.0, "maxDurationMs": 14.0},
                report["metrics"],
            )
            self.assertEqual("SHIP", report["variant"])
            self.assertEqual(
                {
                    "path": perf_baseline_path(PERF_SUBJECT_ID, PERF_MATRIX_ID),
                    "metrics": {"meanDurationMs": 11.0},
                    "updated": False,
                    "regressions": [{"metric": "meanDurationMs", "baseline": 11.0, "actual": 13.0, "delta": 2.0}],
                },
                report["baseline"],
            )
            self.assertEqual("regressed", report["regressionStatus"])
            self.assertEqual(
                [
                    run_bucket_path(PERF_SUBJECT_ID, run_id, "matrices", PERF_MATRIX_ID, "validations", "perf", "summary.json"),
                    run_bucket_path(PERF_SUBJECT_ID, run_id, "matrices", PERF_MATRIX_ID, "validations", "perf", "baseline-compare.json"),
                    run_bucket_path(PERF_SUBJECT_ID, run_id, "matrices", PERF_MATRIX_ID, "validations", "perf", "samples.json"),
                ],
                release_paths,
            )
            self.assertEqual(release_paths, report["releaseReportPaths"])

            samples_path = (
                repo_root
                / "artifacts"
                / "subjects"
                / PERF_SUBJECT_ID
                / "runs"
                / run_id
                / "matrices"
                / PERF_MATRIX_ID
                / "validations"
                / "perf"
                / "samples.json"
            )
            self.assertTrue(samples_path.is_file())
            samples_payload = json.loads(samples_path.read_text(encoding="utf-8"))
            self.assertEqual(2, len(samples_payload["samples"]))
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_build_subject_summary_and_subject_result_aggregate_matrix_reports(self) -> None:
        reporting_module = load_module(SUBJECT_REPORTING_MODULE_PATH, "chaos_subject_reporting_summary")

        matrix_reports = [
            {
                "subjectId": TRACE_SUBJECT_ID,
                "goalId": TRACE_GOAL_ID,
                "matrixId": TRACE_MATRIX_ID,
                "validationProfileId": "trace-platform",
                "variant": "CHECK",
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
                "subjectId": TRACE_SUBJECT_ID,
                "goalId": TRACE_GOAL_ID,
                "matrixId": "windows-linux-buildable",
                "validationProfileId": "build-platform",
                "variant": "CHECK",
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
            TRACE_MATRIX_ID: run_bucket_path(TRACE_SUBJECT_ID, "20260406-fixture-trace-001", "matrices", TRACE_MATRIX_ID, "pipeline-report", "report.json"),
            "windows-linux-buildable": run_bucket_path(TRACE_SUBJECT_ID, "20260406-fixture-trace-001", "matrices", "windows-linux-buildable", "pipeline-report", "report.json"),
        }

        summary = reporting_module.build_subject_summary(
            subject_id=TRACE_SUBJECT_ID,
            requested_goal_id=TRACE_GOAL_ID,
            matrix_reports=matrix_reports,
            matrix_report_paths=matrix_report_paths,
            run_id="20260406-fixture-trace-001",
            generated_at="2026-04-06T14:20:05Z",
        )
        subject_result = reporting_module.build_subject_result(
            summary,
            subject_summary_path=run_bucket_path(TRACE_SUBJECT_ID, "20260406-fixture-trace-001", "subject-report", "summary.json"),
        )

        self.assertEqual("v1", summary["summaryVersion"])
        self.assertEqual("fail", summary["status"])
        self.assertEqual(2, summary["matrixStatusCounts"]["total"])
        self.assertEqual(1, summary["matrixStatusCounts"]["ok"])
        self.assertEqual(1, summary["matrixStatusCounts"]["fail"])
        self.assertEqual(
            run_bucket_path(TRACE_SUBJECT_ID, "20260406-fixture-trace-001", "matrices", TRACE_MATRIX_ID, "pipeline-report", "report.json"),
            summary["matrixResults"][0]["reportPath"],
        )
        self.assertEqual("CHECK", summary["matrixResults"][0]["variant"])
        self.assertEqual("trace-platform", summary["matrixResults"][0]["validationProfileId"])
        self.assertEqual("windows-x64", summary["matrixResults"][0]["executionContext"]["targetPlatform"])
        self.assertEqual("fail", subject_result["status"])
        self.assertEqual(
            run_bucket_path(TRACE_SUBJECT_ID, "20260406-fixture-trace-001", "subject-report", "summary.json"),
            subject_result["subjectSummaryPath"],
        )
        self.assertEqual(2, subject_result["matrixStatusCounts"]["total"])

    def test_build_subject_summary_preserves_entry_selection_for_family_specific_reports(self) -> None:
        reporting_module = load_module(SUBJECT_REPORTING_MODULE_PATH, "chaos_subject_reporting_entry_selection")

        summary = reporting_module.build_subject_summary(
            subject_id="PerformanceFeaturePack",
            requested_goal_id="perf.release",
            matrix_reports=[
                {
                    "subjectId": "PerformanceFeaturePack",
                    "goalId": "perf.release",
                    "matrixId": "windows-native-perf",
                    "validationProfileId": "perf-profile",
                    "validationKind": "perf",
                    "variant": "PROFILE",
                    "status": "ok",
                    "terminalBucket": "report",
                    "selection": {
                        "executionContext": {
                            "hostPlatform": "windows-x64",
                            "targetPlatform": "windows-x64",
                            "toolchainProfile": "msvc-reference",
                            "runtimeProfile": "native-perf-profile",
                        }
                    },
                }
            ],
            matrix_report_paths={
                "windows-native-perf": run_bucket_path(
                    "PerformanceFeaturePack",
                    "20260413-generic-bench-selection-001",
                    "matrices",
                    "windows-native-perf",
                    "declared",
                    "benchmark",
                    "generic-bench",
                    "report.json",
                )
            },
            run_id="20260413-generic-bench-selection-001",
            generated_at="2026-04-13T04:20:00Z",
            entry_selection={
                "family": "declared-benchmark",
                "stableId": "PerformanceFeaturePack::PerformanceFeaturePack::PerformanceFeaturePack.GenericBenchmarkEntry::RunWorkload()",
                "alias": "generic-bench",
            },
        )
        subject_result = reporting_module.build_subject_result(
            summary,
            subject_summary_path=run_bucket_path(
                "PerformanceFeaturePack",
                "20260413-generic-bench-selection-001",
                "declared",
                "benchmark",
                "generic-bench",
                "summary.json",
            ),
        )

        self.assertEqual(
            {
                "family": "declared-benchmark",
                "stableId": "PerformanceFeaturePack::PerformanceFeaturePack::PerformanceFeaturePack.GenericBenchmarkEntry::RunWorkload()",
                "alias": "generic-bench",
            },
            summary["entrySelection"],
        )
        self.assertEqual("declared-benchmark", subject_result["entrySelection"]["family"])
        self.assertEqual("generic-bench", subject_result["entrySelection"]["alias"])
        self.assertEqual(
            run_bucket_path(
                "PerformanceFeaturePack",
                "20260413-generic-bench-selection-001",
                "declared",
                "benchmark",
                "generic-bench",
                "summary.json",
            ),
            subject_result["subjectSummaryPath"],
        )

    def test_build_matrix_report_surfaces_native_perf_evidence_and_report_artifacts(self) -> None:
        reporting_module = load_module(SUBJECT_REPORTING_MODULE_PATH, "chaos_subject_reporting_native_perf")
        run_id = "20260409-fixture-mainline-native-perf-001"
        subject_id = "MainlineFeaturePack"
        matrix_id = "windows-native-profile"

        plan = {
            "selection": {
                "subjectId": subject_id,
                "displayName": subject_id,
                "goalId": "perf.profile",
                "matrixId": matrix_id,
                "validationProfileId": "perf-profile",
                "validationKinds": ["perf"],
                "validationKind": "perf",
                "variant": "PROFILE",
                "pipelineId": "native-runtime-perf",
                "source": {
                    "type": "dotnet-project",
                    "path": source_project_path(subject_id),
                    "entry": source_entry(subject_id),
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                    "toolchainProfile": "msvc-reference",
                    "runtimeProfile": "native-perf-profile",
                },
                "validationIntent": {
                    "validationMode": "perf",
                    "adaptationLevel": "native-runtime",
                    "expectedOutcome": "pass",
                },
                "artifactPlan": {
                    "requiredBuckets": ["source", "host-input", "analysis", "generated", "build", "runtime", "report"],
                    "evidenceTerminalBucket": "report",
                },
            },
            "stagePlan": [
                {"stageId": "source-resolve", "kind": "source-resolve", "scope": "shared", "bucket": "source"},
                {"stageId": "host-input-build", "kind": "host-input-build", "scope": "shared", "bucket": "host-input"},
                {"stageId": "analysis-frontend", "kind": "analysis-frontend", "scope": "shared", "bucket": "analysis"},
                {"stageId": "generated-native-proof", "kind": "generated-native-proof", "scope": "shared", "bucket": "generated"},
                {"stageId": "build-target", "kind": "build-target", "scope": "matrix", "bucket": "build"},
                {"stageId": "native-runtime-perf", "kind": "native-runtime-perf", "scope": "matrix", "bucket": "runtime"},
                {"stageId": "report-assemble", "kind": "report-assemble", "scope": "matrix", "bucket": "report"},
            ],
        }
        execution_result = {
            "subjectId": subject_id,
            "matrixId": matrix_id,
            "goalId": "perf.profile",
            "status": "ok",
            "terminalStageId": "report-assemble",
            "terminalBucket": "report",
            "stageResults": [
                {
                    "stageId": "native-runtime-perf",
                    "kind": "native-runtime-perf",
                    "bucket": "runtime",
                    "status": "ok",
                    "planMode": "executed",
                    "actionTaken": "executed",
                    "invalidation": {"applied": False, "reason": None},
                    "manifestPath": run_bucket_path(subject_id, run_id, "matrices", matrix_id, "runtime", "runtime.manifest.json"),
                    "reportPaths": [],
                    "primaryEvidencePaths": [
                        run_bucket_path(subject_id, run_id, "matrices", matrix_id, "runtime", "perf.runtime.json"),
                        run_bucket_path(subject_id, run_id, "matrices", matrix_id, "runtime", "perf.samples.json"),
                    ],
                    "fingerprint": "f-native-runtime-perf",
                    "durationMs": 84,
                    "diagnostics": {
                        "stdoutPath": run_bucket_path(subject_id, run_id, "matrices", matrix_id, "runtime", "stdout.log"),
                        "stderrPath": run_bucket_path(subject_id, run_id, "matrices", matrix_id, "runtime", "stderr.log"),
                    },
                    "details": {
                        "performance": {
                            "samples": [
                                {"sampleIndex": 1, "durationMs": 17.0, "exitCode": 0},
                                {"sampleIndex": 2, "durationMs": 18.0, "exitCode": 0},
                            ],
                            "metrics": {
                                "sampleCount": 2,
                                "meanDurationMs": 17.5,
                                "minDurationMs": 17.0,
                                "maxDurationMs": 18.0,
                            },
                            "baselinePath": perf_baseline_path(subject_id, matrix_id),
                            "baseline": {"meanDurationMs": 16.0},
                            "baselineUpdated": False,
                            "regressionStatus": "regressed",
                            "regressions": [
                                {"metric": "meanDurationMs", "baseline": 16.0, "actual": 17.5, "delta": 1.5}
                            ],
                            "runtimeEvidence": {
                                "runtimePath": run_bucket_path(subject_id, run_id, "matrices", matrix_id, "runtime", "perf.runtime.json"),
                                "samplesPath": run_bucket_path(subject_id, run_id, "matrices", matrix_id, "runtime", "perf.samples.json"),
                            },
                        }
                    },
                    "failure": None,
                }
            ],
            "errors": [],
        }

        repo_root = TEST_TMP_ROOT / f"native-perf-{uuid.uuid4().hex}"
        repo_root.mkdir(parents=True, exist_ok=False)
        try:
            report = reporting_module.build_matrix_report(
                plan,
                execution_result,
                run_id=run_id,
                generated_at="2026-04-09T02:20:00Z",
            )
            report_artifacts = reporting_module.materialize_matrix_report_artifacts(
                repo_root,
                matrix_report_path=run_bucket_path(subject_id, run_id, "matrices", matrix_id, "pipeline-report", "report.json"),
                matrix_report=report,
            )

            self.assertEqual("PROFILE", report["variant"])
            self.assertEqual("native-runtime-perf", report["selection"]["pipelineId"])
            self.assertEqual(
                {
                    "runtimePath": run_bucket_path(subject_id, run_id, "matrices", matrix_id, "runtime", "perf.runtime.json"),
                    "samplesPath": run_bucket_path(subject_id, run_id, "matrices", matrix_id, "runtime", "perf.samples.json"),
                },
                report["performanceEvidence"],
            )
            self.assertEqual(
                [
                    run_bucket_path(subject_id, run_id, "matrices", matrix_id, "pipeline-report", "report", "perf-summary.json"),
                    run_bucket_path(subject_id, run_id, "matrices", matrix_id, "pipeline-report", "report", "perf-baseline-compare.json"),
                    run_bucket_path(subject_id, run_id, "matrices", matrix_id, "pipeline-report", "report", "perf-metrics.json"),
                ],
                report_artifacts,
            )
            self.assertEqual(report_artifacts, report["reportArtifacts"])

            metrics_path = (
                repo_root
                / "artifacts"
                / "subjects"
                / subject_id
                / "runs"
                / run_id
                / "matrices"
                / matrix_id
                / "pipeline-report"
                / "report"
                / "perf-metrics.json"
            )
            self.assertTrue(metrics_path.is_file())
            metrics_payload = json.loads(metrics_path.read_text(encoding="utf-8"))
            self.assertEqual(
                {"sampleCount": 2, "meanDurationMs": 17.5, "minDurationMs": 17.0, "maxDurationMs": 18.0},
                metrics_payload["metrics"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_build_matrix_report_assembles_engine_proof_summary_from_engine_stage_details(self) -> None:
        reporting_module = load_module(SUBJECT_REPORTING_MODULE_PATH, "chaos_subject_reporting_engine")
        run_id = "20260410-fixture-engine-object-handle-001"
        subject_id = "EngineObjectHandleLite"
        matrix_id = "windows-dev-output"

        plan = {
            "selection": {
                "subjectId": subject_id,
                "displayName": subject_id,
                "goalId": "correctness.dev",
                "matrixId": matrix_id,
                "validationProfileId": "proof-dev",
                "validationKinds": ["proof"],
                "validationKind": "proof",
                "variant": "CHECK",
                "pipelineId": "engine-runtime-output",
                "source": {
                    "type": "dotnet-project",
                    "path": source_project_path(subject_id),
                    "entry": source_entry(subject_id),
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                    "toolchainProfile": "msvc-reference",
                    "runtimeProfile": "engine-proof-output",
                },
                "validationIntent": {
                    "validationMode": "output",
                    "adaptationLevel": "observable-output",
                    "expectedOutcome": "pass",
                },
                "artifactPlan": {
                    "requiredBuckets": ["source", "host-input", "analysis", "generated", "build", "runtime", "report"],
                    "evidenceTerminalBucket": "runtime",
                },
            },
            "stagePlan": [
                {"stageId": "analysis-frontend", "kind": "analysis-frontend", "scope": "shared", "bucket": "analysis"},
                {"stageId": "generated-engine-proof", "kind": "generated-engine-proof", "scope": "shared", "bucket": "generated"},
                {"stageId": "runtime-engine-observe", "kind": "runtime-engine-observe", "scope": "matrix", "bucket": "runtime"},
                {"stageId": "report-assemble", "kind": "report-assemble", "scope": "matrix", "bucket": "report"},
            ],
        }
        execution_result = {
            "subjectId": subject_id,
            "matrixId": matrix_id,
            "goalId": "correctness.dev",
            "status": "ok",
            "terminalStageId": "runtime-engine-observe",
            "terminalBucket": "runtime",
            "stageResults": [
                {
                    "stageId": "analysis-frontend",
                    "kind": "analysis-frontend",
                    "bucket": "analysis",
                    "status": "ok",
                    "planMode": "executed",
                    "actionTaken": "executed",
                    "invalidation": {"applied": False, "reason": None},
                    "manifestPath": run_bucket_path(subject_id, run_id, "analysis", "analysis", "analysis.manifest.json"),
                    "reportPaths": [],
                    "primaryEvidencePaths": [],
                    "fingerprint": "f-analysis",
                    "durationMs": 12,
                    "diagnostics": {"stdoutPath": None, "stderrPath": None},
                    "details": {
                        "engineContractSummary": {
                            "proofKind": "engine-binding",
                            "focusArea": "object-handle",
                            "resolvedCapabilityIds": [
                                "engine.object.handle.create",
                                "engine.object.handle.resolve",
                            ],
                            "engineBindingKinds": ["object-handle-create", "object-handle-resolve"],
                            "hostBindingKinds": ["artifact-observe-contract"],
                        }
                    },
                    "failure": None,
                },
                {
                    "stageId": "generated-engine-proof",
                    "kind": "generated-engine-proof",
                    "bucket": "generated",
                    "status": "ok",
                    "planMode": "executed",
                    "actionTaken": "executed",
                    "invalidation": {"applied": False, "reason": None},
                    "manifestPath": run_bucket_path(subject_id, run_id, "analysis", "generated", "generated.manifest.json"),
                    "reportPaths": [],
                    "primaryEvidencePaths": [
                        run_bucket_path(subject_id, run_id, "analysis", "generated", "engine-handle-bridge.cpp")
                    ],
                    "fingerprint": "f-generated",
                    "durationMs": 18,
                    "diagnostics": {"stdoutPath": None, "stderrPath": None},
                    "details": {
                        "engineEmissionSummary": {
                            "proofKind": "engine-binding",
                            "focusArea": "object-handle",
                            "emittedCapabilityIds": [
                                "engine.object.handle.create",
                                "engine.object.handle.resolve",
                            ],
                            "bridgeArtifactPaths": [
                                run_bucket_path(subject_id, run_id, "analysis", "generated", "engine-handle-bridge.cpp")
                            ],
                            "registrationArtifactPaths": [
                                run_bucket_path(subject_id, run_id, "analysis", "generated", "engine-binding-registration.cpp")
                            ],
                        }
                    },
                    "failure": None,
                },
                {
                    "stageId": "runtime-engine-observe",
                    "kind": "runtime-engine-observe",
                    "bucket": "runtime",
                    "status": "ok",
                    "planMode": "executed",
                    "actionTaken": "executed",
                    "invalidation": {"applied": False, "reason": None},
                    "manifestPath": run_bucket_path(subject_id, run_id, "matrices", matrix_id, "runtime", "runtime.manifest.json"),
                    "reportPaths": [
                        run_bucket_path(subject_id, run_id, "matrices", matrix_id, "runtime", "engine-observe.report.json")
                    ],
                    "primaryEvidencePaths": [
                        run_bucket_path(subject_id, run_id, "matrices", matrix_id, "runtime", "handle-roundtrip.json")
                    ],
                    "fingerprint": "f-runtime",
                    "durationMs": 9,
                    "diagnostics": {
                        "stdoutPath": run_bucket_path(subject_id, run_id, "matrices", matrix_id, "runtime", "stdout.log"),
                        "stderrPath": run_bucket_path(subject_id, run_id, "matrices", matrix_id, "runtime", "stderr.log"),
                    },
                    "details": {
                        "engineObservationSummary": {
                            "evidenceItems": [
                                {
                                    "kind": "handle-roundtrip",
                                    "path": run_bucket_path(subject_id, run_id, "matrices", matrix_id, "runtime", "handle-roundtrip.json"),
                                    "format": "json",
                                    "status": "ok",
                                    "relatedCapabilityIds": [
                                        "engine.object.handle.create",
                                        "engine.object.handle.resolve",
                                    ],
                                }
                            ],
                            "localReportPaths": [
                                run_bucket_path(subject_id, run_id, "matrices", matrix_id, "runtime", "engine-observe.report.json")
                            ],
                        }
                    },
                    "failure": None,
                },
            ],
            "errors": [],
        }

        report = reporting_module.build_matrix_report(
            plan,
            execution_result,
            run_id=run_id,
            generated_at="2026-04-10T02:20:00Z",
        )

        self.assertEqual("engine-binding", report["engineContractSummary"]["proofKind"])
        self.assertEqual("object-handle", report["engineEmissionSummary"]["focusArea"])
        self.assertEqual(
            ["engine.object.handle.create", "engine.object.handle.resolve"],
            report["engineProofSummary"]["capabilityIds"],
        )
        self.assertEqual(
            [
                {
                    "kind": "handle-roundtrip",
                    "status": "ok",
                    "primaryPath": run_bucket_path(
                        subject_id,
                        run_id,
                        "matrices",
                        matrix_id,
                        "runtime",
                        "handle-roundtrip.json",
                    ),
                }
            ],
            report["engineProofSummary"]["evidenceResults"],
        )
        self.assertEqual(
            [
                run_bucket_path(subject_id, run_id, "matrices", matrix_id, "runtime", "engine-observe.report.json")
            ],
            report["engineProofSummary"]["localReportPaths"],
        )


if __name__ == "__main__":
    unittest.main()
