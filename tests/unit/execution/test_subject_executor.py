from __future__ import annotations

import importlib.util
import json
import shutil
import sys
import unittest
import uuid
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
EXECUTOR_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subject_executor.py"
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp_test_subject_executor"
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


class SubjectExecutorTests(unittest.TestCase):
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


if __name__ == "__main__":
    unittest.main()
