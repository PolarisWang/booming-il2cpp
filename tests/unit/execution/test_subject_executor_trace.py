from tests.unit.execution.subject_executor_test_support import *


class TestSubjectExecutorTrace(SubjectExecutorTestSupport):
    def test_trace_paths_from_execution_reads_runtime_trace_artifact_from_stage_manifest(self) -> None:
        executor_module = load_module(EXECUTOR_MODULE_PATH, "chaos_subject_executor_trace_paths")
        run_id = "20260406-fixture-trace-002"

        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)
        repo_root = TEST_TMP_ROOT / f"repo-{uuid.uuid4().hex}"
        repo_root.mkdir(parents=True, exist_ok=False)
        try:
            analysis_manifest_path = repo_root / run_bucket_path(TRACE_SUBJECT_ID, run_id, "analysis", "analysis", "analysis.manifest.json")
            analysis_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            analysis_manifest_path.write_text("{}", encoding="utf-8")

            runtime_manifest_path = repo_root / run_bucket_path(TRACE_SUBJECT_ID, run_id, "matrices", TRACE_MATRIX_ID, "runtime", "runtime.manifest.json")
            runtime_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            runtime_manifest_path.write_text(
                json.dumps(
                    {
                        "tracePaths": [
                            run_bucket_path(TRACE_SUBJECT_ID, run_id, "matrices", TRACE_MATRIX_ID, "runtime", "trace.runtime.json")
                        ]
                    }
                ),
                encoding="utf-8",
            )

            execution_result = {
                "stageResults": [
                    {"manifestPath": analysis_manifest_path.relative_to(repo_root).as_posix()},
                    {"manifestPath": runtime_manifest_path.relative_to(repo_root).as_posix()},
                ]
            }

            self.assertEqual(
                [run_bucket_path(TRACE_SUBJECT_ID, run_id, "matrices", TRACE_MATRIX_ID, "runtime", "trace.runtime.json")],
                executor_module.trace_paths_from_execution(repo_root, execution_result),
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_executor_emits_matrix_events_and_writes_enriched_matrix_report(self) -> None:
        executor_module = load_module(EXECUTOR_MODULE_PATH, "chaos_subject_executor")
        worker_calls: list[dict] = []
        emitted_events: list[dict] = []
        previous_run_id = "20260405-fixture-trace-000"
        current_run_id = "20260406-fixture-trace-001"

        def analysis_worker(*, repo_root: Path, request: dict) -> dict:
            worker_calls.append({"kind": request["stage"]["kind"], "request": request})
            manifest_path = repo_root / request["paths"]["manifestPath"]
            manifest_path.parent.mkdir(parents=True, exist_ok=True)
            manifest_path.write_text("{}", encoding="utf-8")
            report_path = repo_root / request["paths"]["reportPaths"][0]
            report_path.write_text("{}", encoding="utf-8")
            evidence_path = repo_root / request["paths"]["bucketRoot"] / "typed-il-ir.json"
            evidence_path.write_text("{}", encoding="utf-8")
            return {
                "status": "ok",
                "bucketManifestPath": request["paths"]["manifestPath"],
                "reportPaths": list(request["paths"]["reportPaths"]),
                "primaryEvidencePaths": [str(evidence_path.relative_to(repo_root).as_posix())],
                "metrics": {"durationMs": 7},
                "diagnostics": {"stdoutPath": None, "stderrPath": None},
                "failure": None,
            }

        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)
        repo_root = TEST_TMP_ROOT / f"repo-{uuid.uuid4().hex}"
        repo_root.mkdir(parents=True, exist_ok=False)
        try:
            source_manifest = repo_root / run_bucket_path(TRACE_SUBJECT_ID, previous_run_id, "analysis", "source", "source.manifest.json")
            source_manifest.parent.mkdir(parents=True, exist_ok=True)
            source_manifest.write_text(
                json.dumps(
                    {
                        "subjectId": TRACE_SUBJECT_ID,
                        "bucket": "source",
                        "sourceType": "dotnet-project",
                        "sourcePath": source_project_path(TRACE_SUBJECT_ID),
                        "entry": f"{TRACE_SUBJECT_ID}/Program::Main(System.String[])",
                        "inputs": [Path("subjects", TRACE_SUBJECT_ID, "source", "Program.cs").as_posix()],
                    }
                ),
                encoding="utf-8",
            )

            plan = {
                "planVersion": "v1",
                "selection": {
                    "subjectId": TRACE_SUBJECT_ID,
                    "displayName": TRACE_SUBJECT_ID,
                    "goalId": TRACE_GOAL_ID,
                    "matrixId": TRACE_MATRIX_ID,
                    "pipelineId": "proof-runtime-trace",
                    "source": {
                        "type": "dotnet-project",
                        "path": source_project_path(TRACE_SUBJECT_ID),
                        "entry": f"{TRACE_SUBJECT_ID}/Program::Main(System.String[])",
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
                    "artifactPlan": {"evidenceTerminalBucket": "runtime"},
                },
                "stagePlan": [
                    {
                        "order": 1,
                        "stageId": "source-resolve",
                        "kind": "source-resolve",
                        "scope": "shared",
                        "bucket": "source",
                        "dependsOn": [],
                        "executionMode": "reused",
                        "fingerprint": "f-source",
                        "upstreamFingerprints": {},
                        "reuse": {
                            "decision": "match",
                            "reason": "fingerprint-match",
                            "existingManifestPath": run_bucket_path(TRACE_SUBJECT_ID, previous_run_id, "analysis", "source", "source.manifest.json"),
                        },
                        "paths": {
                            "bucketRoot": run_bucket_path(TRACE_SUBJECT_ID, current_run_id, "analysis", "source"),
                            "manifestPath": run_bucket_path(TRACE_SUBJECT_ID, current_run_id, "analysis", "source", "source.manifest.json"),
                            "reportPaths": [],
                        },
                    },
                    {
                        "order": 2,
                        "stageId": "analysis-frontend",
                        "kind": "analysis-frontend",
                        "scope": "shared",
                        "bucket": "analysis",
                        "dependsOn": ["source-resolve"],
                        "executionMode": "invalidated",
                        "fingerprint": "f-analysis",
                        "upstreamFingerprints": {"source": "f-source"},
                        "reuse": {
                            "decision": "mismatch",
                            "reason": "fingerprint-mismatch:driver-changed",
                            "existingManifestPath": run_bucket_path(TRACE_SUBJECT_ID, previous_run_id, "analysis", "analysis", "analysis.manifest.json"),
                        },
                        "paths": {
                            "bucketRoot": run_bucket_path(TRACE_SUBJECT_ID, current_run_id, "analysis", "analysis"),
                            "manifestPath": run_bucket_path(TRACE_SUBJECT_ID, current_run_id, "analysis", "analysis", "analysis.manifest.json"),
                            "reportPaths": [
                                run_bucket_path(TRACE_SUBJECT_ID, current_run_id, "analysis", "analysis", "contract-validate.report.json")
                            ],
                        },
                    },
                    {
                        "order": 3,
                        "stageId": "report-assemble",
                        "kind": "report-assemble",
                        "scope": "matrix",
                        "bucket": "report",
                        "dependsOn": ["analysis-frontend"],
                        "executionMode": "executed",
                        "fingerprint": "f-report",
                        "upstreamFingerprints": {"analysis": "f-analysis"},
                        "reuse": {"decision": "absent", "reason": "manifest-missing"},
                        "paths": {
                            "bucketRoot": run_bucket_path(TRACE_SUBJECT_ID, current_run_id, "matrices", TRACE_MATRIX_ID, "pipeline-report"),
                            "manifestPath": run_bucket_path(TRACE_SUBJECT_ID, current_run_id, "matrices", TRACE_MATRIX_ID, "pipeline-report", "report.json"),
                            "reportPaths": [],
                        },
                    },
                ],
            }

            result = executor_module.execute_plan(
                repo_root,
                plan,
                worker_registry={"analysis-frontend": analysis_worker},
                run_id=current_run_id,
                event_writer=emitted_events.append,
            )

            self.assertEqual("ok", result["status"])
            self.assertEqual("runtime", result["terminalBucket"])
            self.assertEqual(["source-resolve", "analysis-frontend", "report-assemble"], [stage["stageId"] for stage in result["stageResults"]])
            self.assertEqual("reused", result["stageResults"][0]["actionTaken"])
            self.assertEqual(
                [source_project_path(TRACE_SUBJECT_ID)],
                result["stageResults"][0]["primaryEvidencePaths"],
            )
            self.assertEqual("executed", result["stageResults"][1]["actionTaken"])
            self.assertEqual("executed", result["stageResults"][2]["actionTaken"])
            self.assertEqual(1, len(worker_calls))
            self.assertEqual("analysis-frontend", worker_calls[0]["kind"])
            self.assertEqual(
                run_bucket_path(TRACE_SUBJECT_ID, current_run_id, "analysis", "source", "source.manifest.json"),
                worker_calls[0]["request"]["upstream"]["source"]["manifestPath"],
            )
            self.assertTrue((repo_root / run_bucket_path(TRACE_SUBJECT_ID, current_run_id, "analysis", "source", "source.manifest.json")).is_file())

            report_path = repo_root / run_bucket_path(TRACE_SUBJECT_ID, current_run_id, "matrices", TRACE_MATRIX_ID, "pipeline-report", "report.json")
            self.assertTrue(report_path.is_file(), msg=f"missing matrix report: {report_path}")
            report = json.loads(report_path.read_text(encoding="utf-8"))
            self.assertEqual("v1", report["reportVersion"])
            self.assertEqual(current_run_id, report["runId"])
            self.assertEqual(TRACE_MATRIX_ID, report["matrixId"])
            self.assertEqual(TRACE_GOAL_ID, report["goalId"])
            self.assertEqual("runtime", report["terminalBucket"])
            self.assertEqual("proof-runtime-trace", report["selection"]["pipelineId"])
            self.assertEqual(
                [run_bucket_path(TRACE_SUBJECT_ID, current_run_id, "analysis", "analysis", "typed-il-ir.json")],
                report["stageResults"][1]["primaryEvidencePaths"],
            )
            self.assertEqual(
                "fingerprint-mismatch:driver-changed",
                report["stageResults"][1]["invalidation"]["reason"],
            )
            self.assertEqual(
                "analysis",
                report["artifactResults"][-1]["bucket"],
            )
            self.assertEqual(
                "analysis-frontend",
                next(event["stageId"] for event in emitted_events if event["eventType"] == "stage-finished"),
            )
            self.assertEqual(
                "matrix",
                emitted_events[0]["streamScope"],
            )
            self.assertEqual(
                ["stage-reused", "stage-invalidated", "stage-start", "stage-finished", "matrix-summary"],
                [event["eventType"] for event in emitted_events],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
