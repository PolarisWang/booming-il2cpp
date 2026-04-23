from tests.unit.reporting.subject_reporting_test_support import *


class TestSubjectReportingMatrixEngine(SubjectReportingTestSupport):
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
