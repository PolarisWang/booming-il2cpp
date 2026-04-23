from tests.unit.execution.subject_executor_test_support import *


class TestSubjectExecutorReuse(SubjectExecutorTestSupport):
    def test_executor_restores_reused_perf_metadata_into_matrix_report(self) -> None:
        executor_module = load_module(EXECUTOR_MODULE_PATH, "chaos_subject_executor_reused_perf")
        previous_run_id = "20260405-fixture-perf-000"
        current_run_id = "20260406-fixture-perf-001"

        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)
        repo_root = TEST_TMP_ROOT / f"repo-{uuid.uuid4().hex}"
        repo_root.mkdir(parents=True, exist_ok=False)
        try:
            runtime_root = repo_root / run_bucket_path(PERF_SUBJECT_ID, previous_run_id, "matrices", PERF_MATRIX_ID, "runtime")
            runtime_root.mkdir(parents=True, exist_ok=True)
            (runtime_root / "stdout.log").write_text("sample-1\nsample-2\n", encoding="utf-8")
            (runtime_root / "stderr.log").write_text("", encoding="utf-8")
            (runtime_root / "exit-code.txt").write_text("0\n", encoding="utf-8")
            (runtime_root / "runtime.manifest.json").write_text(
                json.dumps(
                    {
                        "subjectId": PERF_SUBJECT_ID,
                        "matrixId": PERF_MATRIX_ID,
                        "bucket": "runtime",
                        "hostInputManifestPath": run_bucket_path(PERF_SUBJECT_ID, previous_run_id, "analysis", "host-input", "host-input.manifest.json"),
                        "stdoutPath": run_bucket_path(PERF_SUBJECT_ID, previous_run_id, "matrices", PERF_MATRIX_ID, "runtime", "stdout.log"),
                        "stderrPath": run_bucket_path(PERF_SUBJECT_ID, previous_run_id, "matrices", PERF_MATRIX_ID, "runtime", "stderr.log"),
                        "exitCodePath": run_bucket_path(PERF_SUBJECT_ID, previous_run_id, "matrices", PERF_MATRIX_ID, "runtime", "exit-code.txt"),
                        "outputLines": ["sample-1", "sample-2"],
                        "samples": [
                            {"sampleIndex": 1, "durationMs": 12.0, "exitCode": 0},
                            {"sampleIndex": 2, "durationMs": 14.0, "exitCode": 0},
                        ],
                        "summaryMetrics": {
                            "sampleCount": 2,
                            "meanDurationMs": 13.0,
                            "minDurationMs": 12.0,
                            "maxDurationMs": 14.0,
                        },
                        "baselinePath": Path("subjects", PERF_SUBJECT_ID, "baselines", "perf", PERF_MATRIX_ID, "windows.json").as_posix(),
                        "baseline": {"meanDurationMs": 11.0},
                        "baselineUpdated": False,
                        "regressionStatus": "regressed",
                        "regressions": [
                            {"metric": "meanDurationMs", "baseline": 11.0, "actual": 13.0, "delta": 2.0}
                        ],
                    }
                ),
                encoding="utf-8",
            )

            plan = {
                "planVersion": "v1",
                "selection": {
                    "subjectId": PERF_SUBJECT_ID,
                    "displayName": PERF_SUBJECT_ID,
                    "goalId": PERF_GOAL_ID,
                    "matrixId": PERF_MATRIX_ID,
                    "pipelineId": "managed-runtime-perf",
                    "source": {
                        "type": "dotnet-project",
                        "path": source_project_path(PERF_SUBJECT_ID),
                        "entry": f"{PERF_SUBJECT_ID}/Program::Main(System.String[])",
                    },
                    "validationIntent": {
                        "validationMode": "perf",
                        "adaptationLevel": "profiled",
                        "expectedOutcome": "pass",
                    },
                    "executionContext": {
                        "hostPlatform": "windows-x64",
                        "targetPlatform": "windows-x64",
                        "toolchainProfile": "msvc-release",
                        "runtimeProfile": "perf-release",
                    },
                    "artifactPlan": {"evidenceTerminalBucket": "report"},
                },
                "stagePlan": [
                    {
                        "order": 1,
                        "stageId": "runtime-perf-collect",
                        "kind": "runtime-perf-collect",
                        "scope": "matrix",
                        "bucket": "runtime",
                        "dependsOn": [],
                        "executionMode": "reused",
                        "fingerprint": "f-runtime",
                        "upstreamFingerprints": {},
                        "reuse": {
                            "decision": "match",
                            "reason": "fingerprint-match",
                            "existingManifestPath": run_bucket_path(PERF_SUBJECT_ID, previous_run_id, "matrices", PERF_MATRIX_ID, "runtime", "runtime.manifest.json"),
                        },
                        "paths": {
                            "bucketRoot": run_bucket_path(PERF_SUBJECT_ID, current_run_id, "matrices", PERF_MATRIX_ID, "runtime"),
                            "manifestPath": run_bucket_path(PERF_SUBJECT_ID, current_run_id, "matrices", PERF_MATRIX_ID, "runtime", "runtime.manifest.json"),
                            "reportPaths": [],
                        },
                    },
                    {
                        "order": 2,
                        "stageId": "report-assemble",
                        "kind": "report-assemble",
                        "scope": "matrix",
                        "bucket": "report",
                        "dependsOn": ["runtime-perf-collect"],
                        "executionMode": "executed",
                        "fingerprint": "f-report",
                        "upstreamFingerprints": {"runtime": "f-runtime"},
                        "reuse": {"decision": "absent", "reason": "manifest-missing"},
                        "paths": {
                            "bucketRoot": run_bucket_path(PERF_SUBJECT_ID, current_run_id, "matrices", PERF_MATRIX_ID, "pipeline-report"),
                            "manifestPath": run_bucket_path(PERF_SUBJECT_ID, current_run_id, "matrices", PERF_MATRIX_ID, "pipeline-report", "report.json"),
                            "reportPaths": [],
                        },
                    },
                ],
            }

            result = executor_module.execute_plan(repo_root, plan, run_id=current_run_id)

            self.assertEqual("ok", result["status"])
            self.assertEqual(
                [
                    run_bucket_path(PERF_SUBJECT_ID, current_run_id, "matrices", PERF_MATRIX_ID, "runtime", "stdout.log"),
                    run_bucket_path(PERF_SUBJECT_ID, current_run_id, "matrices", PERF_MATRIX_ID, "runtime", "exit-code.txt"),
                ],
                result["stageResults"][0]["primaryEvidencePaths"],
            )
            report_path = repo_root / run_bucket_path(PERF_SUBJECT_ID, current_run_id, "matrices", PERF_MATRIX_ID, "pipeline-report", "report.json")
            report = json.loads(report_path.read_text(encoding="utf-8"))
            self.assertEqual(
                {
                    "sampleCount": 2,
                    "meanDurationMs": 13.0,
                    "minDurationMs": 12.0,
                    "maxDurationMs": 14.0,
                },
                report["metrics"],
            )
            self.assertEqual("regressed", report["regressionStatus"])
            self.assertEqual(
                [
                    run_bucket_path(PERF_SUBJECT_ID, current_run_id, "matrices", PERF_MATRIX_ID, "validations", "perf", "summary.json"),
                    run_bucket_path(PERF_SUBJECT_ID, current_run_id, "matrices", PERF_MATRIX_ID, "validations", "perf", "baseline-compare.json"),
                    run_bucket_path(PERF_SUBJECT_ID, current_run_id, "matrices", PERF_MATRIX_ID, "validations", "perf", "samples.json"),
                ],
                report["releaseReportPaths"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_executor_restores_reused_host_input_additional_assemblies_into_primary_evidence(self) -> None:
        executor_module = load_module(EXECUTOR_MODULE_PATH, "chaos_subject_executor_reused_host_input")
        previous_run_id = "20260405-fixture-host-input-000"
        current_run_id = "20260406-fixture-host-input-001"
        subject_id = "FixtureSolutionHostInputSubject"
        matrix_id = "windows-solution-proof"
        goal_id = "correctness.solution-proof"

        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)
        repo_root = TEST_TMP_ROOT / f"repo-{uuid.uuid4().hex}"
        repo_root.mkdir(parents=True, exist_ok=False)
        try:
            previous_manifest_path = repo_root / run_bucket_path(
                subject_id,
                previous_run_id,
                "analysis",
                "host-input",
                "host-input.manifest.json",
            )
            previous_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            previous_manifest_path.write_text(
                json.dumps(
                    {
                        "subjectId": subject_id,
                        "bucket": "host-input",
                        "primaryAssemblyPath": run_bucket_path(
                            subject_id,
                            previous_run_id,
                            "analysis",
                            "host-input",
                            "FixtureSolutionHostInput.App.dll",
                        ),
                        "additionalAssemblyPaths": [
                            run_bucket_path(
                                subject_id,
                                previous_run_id,
                                "analysis",
                                "host-input",
                                "FixtureSolutionHostInput.Helper.dll",
                            )
                        ],
                    }
                ),
                encoding="utf-8",
            )

            plan = {
                "planVersion": "v1",
                "selection": {
                    "subjectId": subject_id,
                    "displayName": subject_id,
                    "goalId": goal_id,
                    "matrixId": matrix_id,
                    "pipelineId": "proof-runtime-trace",
                    "source": {
                        "type": "dotnet-project",
                        "path": Path("subjects", subject_id, "source", f"{subject_id}.sln").as_posix(),
                        "primaryProjectPath": Path(
                            "subjects",
                            subject_id,
                            "source",
                            "App",
                            "FixtureSolutionHostInput.App.csproj",
                        ).as_posix(),
                        "entry": "FixtureSolutionHostInput.App/Program::Main(System.String[])",
                    },
                    "validationIntent": {
                        "validationMode": "trace",
                        "adaptationLevel": "traceable",
                        "expectedOutcome": "pass",
                    },
                    "executionContext": {
                        "hostPlatform": "windows-x64",
                        "targetPlatform": "windows-x64",
                        "toolchainProfile": "msvc-reference",
                        "runtimeProfile": "reference-trace",
                    },
                    "artifactPlan": {"evidenceTerminalBucket": "host-input"},
                },
                "stagePlan": [
                    {
                        "order": 1,
                        "stageId": "host-input-build",
                        "kind": "host-input-build",
                        "scope": "shared",
                        "bucket": "host-input",
                        "dependsOn": [],
                        "executionMode": "reused",
                        "fingerprint": "f-host-input",
                        "upstreamFingerprints": {},
                        "reuse": {
                            "decision": "match",
                            "reason": "fingerprint-match",
                            "existingManifestPath": run_bucket_path(
                                subject_id,
                                previous_run_id,
                                "analysis",
                                "host-input",
                                "host-input.manifest.json",
                            ),
                        },
                        "paths": {
                            "bucketRoot": run_bucket_path(subject_id, current_run_id, "analysis", "host-input"),
                            "manifestPath": run_bucket_path(
                                subject_id,
                                current_run_id,
                                "analysis",
                                "host-input",
                                "host-input.manifest.json",
                            ),
                            "reportPaths": [],
                        },
                    },
                    {
                        "order": 2,
                        "stageId": "report-assemble",
                        "kind": "report-assemble",
                        "scope": "matrix",
                        "bucket": "report",
                        "dependsOn": ["host-input-build"],
                        "executionMode": "executed",
                        "fingerprint": "f-report",
                        "upstreamFingerprints": {"host-input": "f-host-input"},
                        "reuse": {"decision": "absent", "reason": "manifest-missing"},
                        "paths": {
                            "bucketRoot": run_bucket_path(subject_id, current_run_id, "matrices", matrix_id, "pipeline-report"),
                            "manifestPath": run_bucket_path(
                                subject_id,
                                current_run_id,
                                "matrices",
                                matrix_id,
                                "pipeline-report",
                                "report.json",
                            ),
                            "reportPaths": [],
                        },
                    },
                ],
            }

            result = executor_module.execute_plan(repo_root, plan, run_id=current_run_id)

            self.assertEqual("ok", result["status"])
            self.assertEqual(
                [
                    run_bucket_path(
                        subject_id,
                        current_run_id,
                        "analysis",
                        "host-input",
                        "FixtureSolutionHostInput.App.dll",
                    ),
                    run_bucket_path(
                        subject_id,
                        current_run_id,
                        "analysis",
                        "host-input",
                        "FixtureSolutionHostInput.Helper.dll",
                    ),
                ],
                result["stageResults"][0]["primaryEvidencePaths"],
            )

            report_path = repo_root / run_bucket_path(
                subject_id,
                current_run_id,
                "matrices",
                matrix_id,
                "pipeline-report",
                "report.json",
            )
            report = json.loads(report_path.read_text(encoding="utf-8"))
            self.assertEqual(
                [
                    run_bucket_path(
                        subject_id,
                        current_run_id,
                        "analysis",
                        "host-input",
                        "FixtureSolutionHostInput.App.dll",
                    ),
                    run_bucket_path(
                        subject_id,
                        current_run_id,
                        "analysis",
                        "host-input",
                        "FixtureSolutionHostInput.Helper.dll",
                    ),
                ],
                report["stageResults"][0]["primaryEvidencePaths"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
