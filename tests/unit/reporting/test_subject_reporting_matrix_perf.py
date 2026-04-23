from tests.unit.reporting.subject_reporting_test_support import *


class TestSubjectReportingMatrixPerf(SubjectReportingTestSupport):
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
