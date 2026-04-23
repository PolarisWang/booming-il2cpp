from tests.unit.reporting.subject_reporting_test_support import *


class TestSubjectReportingMatrixSelection(SubjectReportingTestSupport):
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
