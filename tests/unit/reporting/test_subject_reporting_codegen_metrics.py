from tests.unit.reporting.subject_reporting_test_support import *


class TestSubjectReportingCodegenMetrics(SubjectReportingTestSupport):
    def test_build_matrix_report_materializes_codegen_gate_artifacts(self) -> None:
        reporting_module = load_module(SUBJECT_REPORTING_MODULE_PATH, "chaos_subject_reporting_codegen_metrics")
        subject_id = "FixtureCodegenSubject"
        matrix_id = "windows-native-perf"
        goal_id = "correctness.platform"
        run_id = "20260422-fixture-codegen-001"

        plan = {
            "selection": {
                "subjectId": subject_id,
                "displayName": subject_id,
                "goalId": goal_id,
                "matrixId": matrix_id,
                "variant": "PROFILE",
                "pipelineId": "generated-native-aot",
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
            },
            "stagePlan": [
                {"stageId": "source-resolve", "kind": "source-resolve", "scope": "shared", "bucket": "source"},
                {"stageId": "analysis-frontend", "kind": "analysis-frontend", "scope": "shared", "bucket": "analysis"},
                {"stageId": "generated-native-aot", "kind": "generated-native-aot", "scope": "matrix", "bucket": "generated"},
                {"stageId": "report-assemble", "kind": "report-assemble", "scope": "matrix", "bucket": "report"},
            ],
        }
        execution_result = {
            "subjectId": subject_id,
            "matrixId": matrix_id,
            "goalId": goal_id,
            "status": "ok",
            "terminalStageId": "report-assemble",
            "terminalBucket": "report",
            "stageResults": [
                {
                    "stageId": "generated-native-aot",
                    "kind": "generated-native-aot",
                    "bucket": "generated",
                    "status": "ok",
                    "planMode": "executed",
                    "actionTaken": "executed",
                    "invalidation": {"applied": False, "reason": None},
                    "manifestPath": run_bucket_path(subject_id, run_id, "analysis", "generated", "generated.manifest.json"),
                    "reportPaths": [],
                    "primaryEvidencePaths": [
                        run_bucket_path(subject_id, run_id, "analysis", "generated", "native-aot.codegen-metrics.json")
                    ],
                    "fingerprint": "f-generated",
                    "durationMs": 90,
                    "diagnostics": {
                        "stdoutPath": None,
                        "stderrPath": None,
                    },
                    "details": {
                        "codegenPerformance": {
                            "metrics": {
                                "generatedCppTotalBytes": 512,
                                "generatedSymbolCount": 8,
                                "peakWorkingSetBytes": 4096,
                            },
                            "baselinePath": Path("subjects", subject_id, "baselines", "codegen", matrix_id, "windows.json").as_posix(),
                            "baseline": {
                                "generatedCppTotalBytes": 256,
                                "generatedSymbolCount": 5,
                                "peakWorkingSetBytes": 1024,
                            },
                            "baselineUpdated": False,
                            "regressionStatus": "regressed",
                            "regressions": [
                                {"metric": "generatedCppTotalBytes", "baseline": 256, "actual": 512, "delta": 256},
                                {"metric": "generatedSymbolCount", "baseline": 5, "actual": 8, "delta": 3},
                                {"metric": "peakWorkingSetBytes", "baseline": 1024, "actual": 4096, "delta": 3072},
                            ],
                            "metricsArtifactPath": run_bucket_path(subject_id, run_id, "analysis", "generated", "native-aot.codegen-metrics.json"),
                            "baselineComparePath": run_bucket_path(subject_id, run_id, "analysis", "generated", "codegen-baseline-compare.json"),
                        }
                    },
                    "failure": None,
                }
            ],
            "errors": [],
        }

        repo_root = TEST_TMP_ROOT / f"codegen-{uuid.uuid4().hex}"
        repo_root.mkdir(parents=True, exist_ok=False)
        try:
            report = reporting_module.build_matrix_report(
                plan,
                execution_result,
                run_id=run_id,
                generated_at="2026-04-22T03:20:00Z",
            )
            artifact_paths = reporting_module.materialize_matrix_report_artifacts(
                repo_root,
                matrix_report_path=run_bucket_path(subject_id, run_id, "matrices", matrix_id, "pipeline-report", "report.json"),
                matrix_report=report,
            )

            self.assertEqual(
                {
                    "generatedCppTotalBytes": 512,
                    "generatedSymbolCount": 8,
                    "peakWorkingSetBytes": 4096,
                },
                report["codegenMetrics"],
            )
            self.assertEqual("regressed", report["codegenRegressionStatus"])
            self.assertEqual(
                {
                    "path": Path("subjects", subject_id, "baselines", "codegen", matrix_id, "windows.json").as_posix(),
                    "metrics": {
                        "generatedCppTotalBytes": 256,
                        "generatedSymbolCount": 5,
                        "peakWorkingSetBytes": 1024,
                    },
                    "updated": False,
                    "regressions": [
                        {"metric": "generatedCppTotalBytes", "baseline": 256, "actual": 512, "delta": 256},
                        {"metric": "generatedSymbolCount", "baseline": 5, "actual": 8, "delta": 3},
                        {"metric": "peakWorkingSetBytes", "baseline": 1024, "actual": 4096, "delta": 3072},
                    ],
                },
                report["codegenBaseline"],
            )
            self.assertEqual(
                [
                    run_bucket_path(subject_id, run_id, "matrices", matrix_id, "pipeline-report", "report", "codegen-summary.json"),
                    run_bucket_path(subject_id, run_id, "matrices", matrix_id, "pipeline-report", "report", "codegen-baseline-compare.json"),
                    run_bucket_path(subject_id, run_id, "matrices", matrix_id, "pipeline-report", "report", "codegen-metrics.json"),
                ],
                report["codegenReportPaths"],
            )
            self.assertEqual(report["codegenReportPaths"], artifact_paths)

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
                / "codegen-metrics.json"
            )
            self.assertTrue(metrics_path.is_file())
            metrics_payload = json.loads(metrics_path.read_text(encoding="utf-8"))
            self.assertEqual(512, metrics_payload["metrics"]["generatedCppTotalBytes"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
