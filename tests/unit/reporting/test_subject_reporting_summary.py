from tests.unit.reporting.subject_reporting_test_support import *


class TestSubjectReportingSummary(SubjectReportingTestSupport):
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
            subject_id="SolutionCorePack",
            requested_goal_id="perf.release",
            matrix_reports=[
                {
                    "subjectId": "SolutionCorePack",
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
                    "SolutionCorePack",
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
                    "stableId": "SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.GenericBenchmarkEntry::RunWorkload()",
                "alias": "generic-bench",
            },
        )
        subject_result = reporting_module.build_subject_result(
            summary,
            subject_summary_path=run_bucket_path(
                "SolutionCorePack",
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
                    "stableId": "SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.GenericBenchmarkEntry::RunWorkload()",
                "alias": "generic-bench",
            },
            summary["entrySelection"],
        )
        self.assertEqual("declared-benchmark", subject_result["entrySelection"]["family"])
        self.assertEqual("generic-bench", subject_result["entrySelection"]["alias"])
        self.assertEqual(
            run_bucket_path(
                "SolutionCorePack",
                "20260413-generic-bench-selection-001",
                "declared",
                "benchmark",
                "generic-bench",
                "summary.json",
            ),
            subject_result["subjectSummaryPath"],
        )

    def test_build_subject_summary_surfaces_codegen_and_matrix_proof_linkage_fields(self) -> None:
        reporting_module = load_module(SUBJECT_REPORTING_MODULE_PATH, "chaos_subject_reporting_codegen_summary")

        summary = reporting_module.build_subject_summary(
            subject_id="SolutionCorePack",
            requested_goal_id="correctness.platform",
            matrix_reports=[
                {
                    "subjectId": "SolutionCorePack",
                    "goalId": "correctness.platform",
                    "matrixId": "windows-reference-proof",
                    "validationProfileId": "proof-platform",
                    "validationKind": "proof",
                    "variant": "CHECK",
                    "status": "ok",
                    "terminalBucket": "runtime",
                    "selection": {
                        "executionContext": {
                            "hostPlatform": "windows-x64",
                            "targetPlatform": "windows-x64",
                            "toolchainProfile": "msvc-reference",
                            "runtimeProfile": "reference-proof",
                        }
                    },
                    "codegenMetrics": {
                        "generatedCppTotalBytes": 1024,
                        "generatedSymbolCount": 12,
                        "peakWorkingSetBytes": 4096,
                    },
                    "codegenRegressionStatus": "ok",
                    "codegenReportPaths": [
                        run_bucket_path(
                            "SolutionCorePack",
                            "20260422-summary-codegen-001",
                            "matrices",
                            "windows-reference-proof",
                            "pipeline-report",
                            "report",
                            "codegen-summary.json",
                        )
                    ],
                    "matrixProofLinkage": {
                        "artifactPath": run_bucket_path(
                            "SolutionCorePack",
                            "20260422-summary-codegen-001",
                            "matrices",
                            "windows-reference-proof",
                            "pipeline-report",
                            "report",
                            "generic-matrix-proof-linkage.json",
                        ),
                        "proofKind": "native-proof",
                        "boundaryCaseCount": 2,
                    },
                    "reportArtifacts": [
                        run_bucket_path(
                            "SolutionCorePack",
                            "20260422-summary-codegen-001",
                            "matrices",
                            "windows-reference-proof",
                            "pipeline-report",
                            "report",
                            "generic-matrix-proof-linkage.json",
                        )
                    ],
                }
            ],
            matrix_report_paths={
                "windows-reference-proof": run_bucket_path(
                    "SolutionCorePack",
                    "20260422-summary-codegen-001",
                    "matrices",
                    "windows-reference-proof",
                    "pipeline-report",
                    "report.json",
                )
            },
            run_id="20260422-summary-codegen-001",
            generated_at="2026-04-22T08:30:00Z",
        )

        matrix_result = summary["matrixResults"][0]
        self.assertEqual(
            {
                "generatedCppTotalBytes": 1024,
                "generatedSymbolCount": 12,
                "peakWorkingSetBytes": 4096,
            },
            matrix_result["codegenMetrics"],
        )
        self.assertEqual("ok", matrix_result["codegenRegressionStatus"])
        self.assertEqual(
            [
                run_bucket_path(
                    "SolutionCorePack",
                    "20260422-summary-codegen-001",
                    "matrices",
                    "windows-reference-proof",
                    "pipeline-report",
                    "report",
                    "codegen-summary.json",
                )
            ],
            matrix_result["codegenReportPaths"],
        )
        self.assertEqual("native-proof", matrix_result["matrixProofLinkage"]["proofKind"])
        self.assertEqual(2, matrix_result["matrixProofLinkage"]["boundaryCaseCount"])

    def test_build_subject_summary_surfaces_release_evidence_contract_summary(self) -> None:
        reporting_module = load_module(SUBJECT_REPORTING_MODULE_PATH, "chaos_subject_reporting_release_evidence")

        summary = reporting_module.build_subject_summary(
            subject_id="SolutionCorePack",
            requested_goal_id="correctness.platform",
            matrix_reports=[
                {
                    "subjectId": "SolutionCorePack",
                    "goalId": "correctness.platform",
                    "matrixId": "windows-reference-trace",
                    "validationProfileId": "trace-platform",
                    "validationKind": "proof",
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
                }
            ],
            matrix_report_paths={
                "windows-reference-trace": run_bucket_path(
                    "SolutionCorePack",
                    "20260415-release-evidence-001",
                    "matrices",
                    "windows-reference-trace",
                    "pipeline-report",
                    "report.json",
                )
            },
            run_id="20260415-release-evidence-001",
            generated_at="2026-04-15T13:20:00Z",
            declared_unit_tests=[
                {
                    "subjectId": "SolutionCorePack",
                    "stableId": "solution-core/native-call-proof",
                    "capabilityFamily": 7,
                    "capabilityItem": 31,
                },
                {
                    "subjectId": "SolutionCorePack",
                    "stableId": "solution-core/closure-proof",
                    "capabilityFamily": 12,
                    "capabilityItem": 51,
                },
                {
                    "subjectId": "SolutionCorePack",
                    "stableId": "solution-core/runtime-trace-proof",
                    "capabilityFamily": 11,
                    "capabilityItem": 47,
                },
            ],
            declared_benchmarks=[
                {
                    "subjectId": "SolutionCorePack",
                    "stableId": "solution-core/native-call-bench",
                    "capabilityFamily": 7,
                    "capabilityItem": 31,
                }
            ],
        )
        subject_result = reporting_module.build_subject_result(
            summary,
            subject_summary_path=run_bucket_path(
                "SolutionCorePack",
                "20260415-release-evidence-001",
                "subject-report",
                "summary.json",
            ),
        )

        release_evidence = summary["releaseEvidenceSummary"]
        self.assertEqual("v1", release_evidence["reportVersion"])
        self.assertEqual("fail", release_evidence["status"])
        self.assertEqual(3, release_evidence["statusCounts"]["total"])
        self.assertEqual(2, release_evidence["statusCounts"]["ok"])
        self.assertEqual(1, release_evidence["statusCounts"]["fail"])
        self.assertEqual(1, release_evidence["classificationCounts"]["ok"])
        self.assertEqual(1, release_evidence["classificationCounts"]["missingBenchmark"])
        self.assertEqual(1, release_evidence["classificationCounts"]["proofOptional"])

        results_by_item = {
            int(item["capabilityItem"]): item
            for item in release_evidence["contractResults"]
        }
        self.assertEqual("ok", results_by_item[31]["classification"])
        self.assertEqual("missingBenchmark", results_by_item[51]["classification"])
        self.assertEqual("proofOptional", results_by_item[47]["classification"])
        self.assertTrue(results_by_item[47]["reportingOwned"])
        self.assertEqual(
            "reporting-owned-proof-optional",
            results_by_item[47]["reasonCode"],
        )
        self.assertEqual(
            release_evidence,
            subject_result["releaseEvidenceSummary"],
        )
