from tests.unit.reporting.subject_reporting_test_support import *


class TestSubjectReportingMatrixProofLinkage(SubjectReportingTestSupport):
    def test_materialize_matrix_report_artifacts_writes_generic_matrix_proof_linkage(self) -> None:
        reporting_module = load_module(
            SUBJECT_REPORTING_MODULE_PATH,
            "chaos_subject_reporting_matrix_proof_linkage",
        )
        subject_id = "FixtureMatrixProofLinkageSubject"
        matrix_id = "windows-reference-proof"
        goal_id = "correctness.platform"
        run_id = "20260422-fixture-matrix-proof-linkage-001"

        report_path = run_bucket_path(subject_id, run_id, "matrices", matrix_id, "pipeline-report", "report.json")
        analysis_manifest_path = run_bucket_path(subject_id, run_id, "analysis", "frontend", "analysis.manifest.json")
        generated_manifest_path = run_bucket_path(subject_id, run_id, "analysis", "generated", "generated.manifest.json")
        generic_matrix_path = run_bucket_path(subject_id, run_id, "analysis", "frontend", "generic-capability-matrix.json")
        build_manifest_path = run_bucket_path(subject_id, run_id, "matrices", matrix_id, "build", "build.manifest.json")
        runtime_manifest_path = run_bucket_path(subject_id, run_id, "matrices", matrix_id, "runtime", "runtime.manifest.json")

        generated_cpp_path = run_bucket_path(subject_id, run_id, "analysis", "generated", "generated", "native-reference.generated.cpp")
        build_output_path = run_bucket_path(subject_id, run_id, "matrices", matrix_id, "build", "out", "chaos_subject_reference_proof.exe")
        runtime_stdout_path = run_bucket_path(subject_id, run_id, "matrices", matrix_id, "runtime", "stdout.log")

        plan = {
            "selection": {
                "subjectId": subject_id,
                "goalId": goal_id,
                "matrixId": matrix_id,
                "validationProfileId": "proof-platform",
                "validationKinds": ["proof"],
                "validationKind": "proof",
                "variant": "CHECK",
                "pipelineId": "native-proof-output",
                "entrySelection": {
                    "family": "declared-unit-test",
                    "stableId": f"{subject_id}::DispatchAndReflectionProof::Run()",
                    "alias": "dispatch-reflection-proof",
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                    "toolchainProfile": "msvc-reference",
                    "runtimeProfile": "reference-proof",
                },
            },
            "stagePlan": [
                {"stageId": "analysis-frontend", "kind": "analysis-frontend", "scope": "shared", "bucket": "analysis"},
                {"stageId": "generated-native-proof", "kind": "generated-native-proof", "scope": "shared", "bucket": "generated"},
                {"stageId": "build-target", "kind": "build-target", "scope": "matrix", "bucket": "build"},
                {"stageId": "runtime-managed-output", "kind": "runtime-managed-output", "scope": "matrix", "bucket": "runtime"},
                {"stageId": "report-assemble", "kind": "report-assemble", "scope": "matrix", "bucket": "report"},
            ],
        }
        execution_result = {
            "subjectId": subject_id,
            "matrixId": matrix_id,
            "goalId": goal_id,
            "status": "ok",
            "terminalStageId": "runtime-managed-output",
            "terminalBucket": "runtime",
            "stageResults": [
                {
                    "stageId": "analysis-frontend",
                    "kind": "analysis-frontend",
                    "bucket": "analysis",
                    "status": "ok",
                    "manifestPath": analysis_manifest_path,
                    "reportPaths": [],
                    "primaryEvidencePaths": [generic_matrix_path],
                    "diagnostics": {"stdoutPath": None, "stderrPath": None},
                },
                {
                    "stageId": "generated-native-proof",
                    "kind": "generated-native-proof",
                    "bucket": "generated",
                    "status": "ok",
                    "manifestPath": generated_manifest_path,
                    "reportPaths": [],
                    "primaryEvidencePaths": [generated_cpp_path],
                    "diagnostics": {"stdoutPath": None, "stderrPath": None},
                },
                {
                    "stageId": "build-target",
                    "kind": "build-target",
                    "bucket": "build",
                    "status": "ok",
                    "manifestPath": build_manifest_path,
                    "reportPaths": [],
                    "primaryEvidencePaths": [build_output_path],
                    "diagnostics": {"stdoutPath": None, "stderrPath": None},
                },
                {
                    "stageId": "runtime-managed-output",
                    "kind": "runtime-managed-output",
                    "bucket": "runtime",
                    "status": "ok",
                    "manifestPath": runtime_manifest_path,
                    "reportPaths": [],
                    "primaryEvidencePaths": [runtime_stdout_path],
                    "diagnostics": {"stdoutPath": runtime_stdout_path, "stderrPath": None},
                },
            ],
            "errors": [],
        }

        repo_root = TEST_TMP_ROOT / f"matrix-proof-linkage-{uuid.uuid4().hex}"
        repo_root.mkdir(parents=True, exist_ok=False)
        try:
            write_json(
                repo_root / analysis_manifest_path,
                {
                    "subjectId": subject_id,
                    "bucket": "analysis",
                    "artifacts": {
                        "genericCapabilityMatrixPath": generic_matrix_path,
                    },
                },
            )
            write_json(
                repo_root / generated_manifest_path,
                {
                    "subjectId": subject_id,
                    "bucket": "generated",
                    "analysisManifestPath": analysis_manifest_path,
                    "generatedSourcePath": generated_cpp_path,
                    "generatedSourcePaths": [generated_cpp_path],
                },
            )
            write_json(
                repo_root / generic_matrix_path,
                {
                    "reportVersion": "v0",
                    "artifactKind": "genericCapabilityMatrix",
                    "ownerSubjectId": subject_id,
                    "entrySubjectId": f"{subject_id}/DispatchAndReflectionProof::Run()",
                    "hotUpdateModes": ["HotUpdateNone", "HotUpdateMetadataAugmented", "HotUpdateGenericSharing"],
                    "gates": {
                        "status": "ok",
                        "unsupportedLeakCount": 0,
                        "missingAuthorityCount": 0,
                        "nonCanonicalHotUpdateNameCount": 0,
                        "leaks": [],
                    },
                    "familyBudgets": [],
                    "entries": [],
                    "boundaryCases": [
                        {
                            "boundaryKind": "DispatchBoundary",
                            "sourceMethodSubjectId": f"{subject_id}/DispatchAndReflectionProof::Run()",
                            "ilOffset": 18,
                            "targetSubjectId": f"{subject_id}/DispatchShape::Area()",
                            "evidenceKind": "virtual-callvirt",
                            "status": "observed",
                        },
                        {
                            "boundaryKind": "ReflectionBoundary",
                            "sourceMethodSubjectId": f"{subject_id}/DispatchAndReflectionProof::Run()",
                            "ilOffset": 42,
                            "targetSubjectId": "System.Private.CoreLib/System.Reflection.MethodInfo::Invoke(System.Object,System.Object[])",
                            "evidenceKind": "call-reflection-target",
                            "status": "observed",
                        },
                    ],
                },
            )
            write_json(repo_root / build_manifest_path, {"buildKind": "native-reference"})
            write_json(repo_root / runtime_manifest_path, {"stdoutPath": runtime_stdout_path})

            report = reporting_module.build_matrix_report(
                plan,
                execution_result,
                run_id=run_id,
                generated_at="2026-04-22T08:10:00Z",
            )
            report_artifacts = reporting_module.materialize_matrix_report_artifacts(
                repo_root,
                matrix_report_path=report_path,
                matrix_report=report,
            )

            linkage_artifact_path = run_bucket_path(
                subject_id,
                run_id,
                "matrices",
                matrix_id,
                "pipeline-report",
                "report",
                "generic-matrix-proof-linkage.json",
            )
            self.assertEqual([linkage_artifact_path], report_artifacts)
            self.assertEqual(linkage_artifact_path, report["matrixProofLinkage"]["artifactPath"])
            self.assertEqual("native-proof", report["matrixProofLinkage"]["proofKind"])
            self.assertEqual(2, report["matrixProofLinkage"]["boundaryCaseCount"])
            self.assertEqual(["DispatchBoundary", "ReflectionBoundary"], report["matrixProofLinkage"]["boundaryKinds"])
            self.assertEqual(3, report["matrixProofLinkage"]["proofArtifactCount"])
            self.assertEqual(generic_matrix_path, report["matrixProofLinkage"]["genericCapabilityMatrixPath"])

            linkage_payload = json.loads((repo_root / linkage_artifact_path).read_text(encoding="utf-8"))
            self.assertEqual("generic-matrix-proof-linkage", linkage_payload["artifactKind"])
            self.assertEqual(generic_matrix_path, linkage_payload["genericCapabilityMatrixPath"])
            self.assertEqual(
                [generated_cpp_path, build_output_path, runtime_stdout_path],
                linkage_payload["proofArtifactPaths"],
            )
            self.assertEqual(2, linkage_payload["boundaryCaseCount"])
            self.assertEqual(["DispatchBoundary", "ReflectionBoundary"], linkage_payload["boundaryKinds"])
            self.assertEqual("covered-by-matrix-run", linkage_payload["boundaryCases"][0]["coverageStatus"])
            self.assertEqual(
                [generated_cpp_path, build_output_path, runtime_stdout_path],
                linkage_payload["boundaryCases"][0]["proofArtifactPaths"],
            )
            self.assertEqual(
                "DispatchBoundary",
                linkage_payload["boundaryCases"][0]["boundaryKind"],
            )
            self.assertEqual(
                "ReflectionBoundary",
                linkage_payload["boundaryCases"][1]["boundaryKind"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
