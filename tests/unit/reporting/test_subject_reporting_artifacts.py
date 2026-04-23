from tests.unit.reporting.subject_reporting_test_support import *


class TestSubjectReportingArtifacts(SubjectReportingTestSupport):
    def test_materialize_matrix_report_artifacts_writes_native_hotupdate_audit(self) -> None:
        reporting_module = load_module(SUBJECT_REPORTING_MODULE_PATH, "chaos_subject_reporting_native_hotupdate_audit")
        subject_id = "FixtureNativeHotUpdateSubject"
        matrix_id = "windows-corelib-reference-native-hotupdate-proof"
        run_id = "20260420-fixture-native-hotupdate-001"
        report_path = run_bucket_path(subject_id, run_id, "matrices", matrix_id, "pipeline-report", "report.json")
        host_input_manifest_path = run_bucket_path(subject_id, run_id, "analysis", "host-input", "host-input.manifest.json")
        generated_manifest_path = run_bucket_path(subject_id, run_id, "analysis", "generated", "generated.manifest.json")
        native_reference_manifest_path = run_bucket_path(subject_id, run_id, "analysis", "generated", "native-reference.manifest.json")
        native_reference_plan_path = run_bucket_path(subject_id, run_id, "analysis", "generated", "native-reference.plan.json")
        runtime_page_path = run_bucket_path(
            subject_id,
            run_id,
            "analysis",
            "generated",
            "generated",
            "runtime",
            "native-reference.runtime-skeleton.page-0001.cpp",
        )
        coverage_report_path = run_bucket_path(
            subject_id,
            run_id,
            "analysis",
            "generated",
            "generated",
            "runtime",
            "native-reference.runtime-skeleton.coverage.json",
        )
        build_manifest_path = run_bucket_path(subject_id, run_id, "matrices", matrix_id, "build", "build.manifest.json")
        runtime_manifest_path = run_bucket_path(subject_id, run_id, "matrices", matrix_id, "runtime", "runtime.manifest.json")

        plan = {
            "selection": {
                "subjectId": subject_id,
                "goalId": "correctness.dev",
                "matrixId": matrix_id,
                "validationProfileId": "proof-dev",
                "validationKinds": ["proof"],
                "variant": "CHECK",
                "pipelineId": "native-hotupdate-proof-output",
                "entrySelection": {
                    "family": "declared-unit-test",
                    "stableId": f"{subject_id}::Patch::Patch.Proofs::Run()",
                    "alias": "native-hotupdate-proof",
                    "entryIndex": 60,
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                    "toolchainProfile": "msvc-reference",
                    "runtimeProfile": "native-hotupdate-proof-output",
                },
                "artifactPlan": {
                    "requiredBuckets": ["source", "host-input", "analysis", "generated", "build", "runtime", "report"],
                    "evidenceTerminalBucket": "runtime",
                },
            },
            "stagePlan": [
                {"stageId": "host-input-build", "kind": "host-input-build", "scope": "shared", "bucket": "host-input"},
                {"stageId": "generated-native-proof", "kind": "generated-native-proof", "scope": "shared", "bucket": "generated"},
                {"stageId": "build-target", "kind": "build-target", "scope": "matrix", "bucket": "build"},
                {"stageId": "runtime-managed-output", "kind": "runtime-managed-output", "scope": "matrix", "bucket": "runtime"},
                {"stageId": "report-assemble", "kind": "report-assemble", "scope": "matrix", "bucket": "report"},
            ],
        }
        execution_result = {
            "subjectId": subject_id,
            "matrixId": matrix_id,
            "goalId": "correctness.dev",
            "status": "ok",
            "terminalStageId": "runtime-managed-output",
            "terminalBucket": "runtime",
            "stageResults": [
                {
                    "stageId": "host-input-build",
                    "kind": "host-input-build",
                    "bucket": "host-input",
                    "status": "ok",
                    "manifestPath": host_input_manifest_path,
                    "reportPaths": [],
                    "primaryEvidencePaths": [run_bucket_path(subject_id, run_id, "analysis", "host-input", "NativeProofApp.dll")],
                    "diagnostics": {"stdoutPath": None, "stderrPath": None},
                },
                {
                    "stageId": "generated-native-proof",
                    "kind": "generated-native-proof",
                    "bucket": "generated",
                    "status": "ok",
                    "manifestPath": generated_manifest_path,
                    "reportPaths": [],
                    "primaryEvidencePaths": [run_bucket_path(subject_id, run_id, "analysis", "generated", "generated", "native-reference.generated.cpp")],
                    "diagnostics": {"stdoutPath": None, "stderrPath": None},
                },
                {
                    "stageId": "build-target",
                    "kind": "build-target",
                    "bucket": "build",
                    "status": "ok",
                    "manifestPath": build_manifest_path,
                    "reportPaths": [],
                    "primaryEvidencePaths": [run_bucket_path(subject_id, run_id, "matrices", matrix_id, "build", "out", "chaos_subject_reference_proof.exe")],
                    "diagnostics": {"stdoutPath": None, "stderrPath": None},
                },
                {
                    "stageId": "runtime-managed-output",
                    "kind": "runtime-managed-output",
                    "bucket": "runtime",
                    "status": "ok",
                    "manifestPath": runtime_manifest_path,
                    "reportPaths": [],
                    "primaryEvidencePaths": [run_bucket_path(subject_id, run_id, "matrices", matrix_id, "runtime", "stdout.log")],
                    "diagnostics": {
                        "stdoutPath": run_bucket_path(subject_id, run_id, "matrices", matrix_id, "runtime", "stdout.log"),
                        "stderrPath": run_bucket_path(subject_id, run_id, "matrices", matrix_id, "runtime", "stderr.log"),
                    },
                },
            ],
            "errors": [],
        }

        repo_root = TEST_TMP_ROOT / f"native-hotupdate-audit-{uuid.uuid4().hex}"
        repo_root.mkdir(parents=True, exist_ok=False)
        try:
            write_json(
                repo_root / host_input_manifest_path,
                {
                    "primaryProjectPath": "subjects/Fixture/NativeProofApp.csproj",
                    "primaryAssemblyPath": run_bucket_path(subject_id, run_id, "analysis", "host-input", "NativeProofApp.dll"),
                    "additionalAssemblyPaths": [run_bucket_path(subject_id, run_id, "analysis", "host-input", "Patch.dll")],
                    "managedRuntimeProjectPath": "solutions/subjects/Fixture/hotupdate-tests/Fixture.HotUpdateProofHost.csproj",
                    "managedRuntimeAssemblyPath": run_bucket_path(subject_id, run_id, "analysis", "host-input", "Fixture.HotUpdateProofHost.dll"),
                    "collectionPath": "solutions/subjects/Fixture/hotupdate-tests/Generated/declared-tests.collection.json",
                    "bindingManifestPath": "solutions/subjects/Fixture/hotupdate-tests/Generated/declared-tests.binding.json",
                },
            )
            write_json(
                repo_root / generated_manifest_path,
                {
                    "generatedSourcePath": run_bucket_path(subject_id, run_id, "analysis", "generated", "generated", "native-reference.generated.cpp"),
                    "generatedSourcePaths": [
                        run_bucket_path(subject_id, run_id, "analysis", "generated", "generated", "native-reference.generated.cpp"),
                        runtime_page_path,
                    ],
                    "nativeReferenceManifestPath": native_reference_manifest_path,
                    "nativeReferencePlanPath": native_reference_plan_path,
                },
            )
            write_json(
                repo_root / native_reference_manifest_path,
                {
                    "runtimeExecutionKind": "assembly-bound-native-reference-skeleton",
                    "preferredAssemblyDispatchSubjectId": "Fixture.NativeProofApp/Program::Main:System.Int32()",
                    "translationUnitPageCount": 1,
                    "generatedArtifacts": [
                        {
                            "kind": "generatedTranslationUnit",
                            "path": "generated/runtime/native-reference.runtime-skeleton.page-0001.cpp",
                        },
                        {
                            "kind": "runtimeSkeletonCoverageReport",
                            "path": "generated/runtime/native-reference.runtime-skeleton.coverage.json",
                        },
                    ],
                },
            )
            write_json(
                repo_root / native_reference_plan_path,
                {
                    "planKind": "assembly-full-closure-runtime-skeleton",
                    "translationUnitMode": "runtime-skeleton",
                    "translationUnitMethodCount": 3,
                    "auditStatus": "runtime-skeleton",
                    "auditMessage": "assembly-bound full-closure native-reference runtime skeleton emits covered executable methods only; uncovered methods are reported separately",
                },
            )
            (repo_root / runtime_page_path).parent.mkdir(parents=True, exist_ok=True)
            (repo_root / runtime_page_path).write_text(
                (
                    "int stub0() {\n"
                    "    return CHAOS_BRIDGE_STATUS_OK;\n"
                    "}\n"
                ),
                encoding="utf-8",
            )
            write_json(
                repo_root / coverage_report_path,
                {
                    "formatVersion": "v0",
                    "artifactKind": "nativeReferenceRuntimeSkeletonCoverage",
                    "assemblyName": "Fixture.NativeProofApp",
                    "planKind": "assembly-full-closure-runtime-skeleton",
                    "translationUnitMode": "runtime-skeleton",
                    "requestedMethodCount": 3,
                    "emittedMethodCount": 2,
                    "uncoveredMethodCount": 1,
                    "uncoveredMethodSubjectIds": [
                        "Fixture.NativeProofApp/Program::Helper:System.String()",
                    ],
                },
            )
            write_json(
                repo_root / build_manifest_path,
                {
                    "buildKind": "native-reference",
                    "buildStrategy": "windows-reference-cmake",
                    "hostSourcePath": "solutions/subjects/Fixture/native-source/proof/main.cpp",
                    "generatedSourcePaths": [
                        run_bucket_path(subject_id, run_id, "analysis", "generated", "generated", "native-reference.generated.cpp"),
                    ],
                    "outputs": [
                        run_bucket_path(subject_id, run_id, "matrices", matrix_id, "build", "out", "chaos_subject_reference_proof.exe"),
                    ],
                },
            )
            write_json(
                repo_root / runtime_manifest_path,
                {
                    "managedRuntimeAssemblyPath": run_bucket_path(subject_id, run_id, "analysis", "host-input", "Fixture.HotUpdateProofHost.dll"),
                    "nativePrimaryAssemblyPath": run_bucket_path(subject_id, run_id, "analysis", "host-input", "NativeProofApp.dll"),
                    "nativeGeneratedManifestPath": generated_manifest_path,
                    "nativeBuildManifestPath": build_manifest_path,
                    "bindingManifestPath": "solutions/subjects/Fixture/hotupdate-tests/Generated/declared-tests.binding.json",
                    "arguments": ["--entry-index=60"],
                    "outputLines": ["corelib-reference-hotupdate:System.Private.CoreLib|System.Runtime|System.Console:16:3"],
                    "stdoutPath": run_bucket_path(subject_id, run_id, "matrices", matrix_id, "runtime", "stdout.log"),
                    "stderrPath": run_bucket_path(subject_id, run_id, "matrices", matrix_id, "runtime", "stderr.log"),
                    "exitCodePath": run_bucket_path(subject_id, run_id, "matrices", matrix_id, "runtime", "exit-code.txt"),
                },
            )

            report = reporting_module.build_matrix_report(
                plan,
                execution_result,
                run_id=run_id,
                generated_at="2026-04-20T01:44:18Z",
            )
            report_artifacts = reporting_module.materialize_matrix_report_artifacts(
                repo_root,
                matrix_report_path=report_path,
                matrix_report=report,
            )

            audit_path = run_bucket_path(subject_id, run_id, "matrices", matrix_id, "pipeline-report", "report", "native-hotupdate-audit.json")
            self.assertEqual([audit_path], report_artifacts)
            self.assertEqual(audit_path, report["nativeHotupdateAudit"]["artifactPath"])
            self.assertEqual(native_reference_manifest_path, report["nativeHotupdateAudit"]["nativeReferenceManifestPath"])
            self.assertEqual("assembly-full-closure-runtime-skeleton", report["nativeHotupdateAudit"]["nativeReferencePlanKind"])
            self.assertEqual(3, report["nativeHotupdateAudit"]["translationUnitMethodCount"])
            self.assertEqual(0, report["nativeHotupdateAudit"]["runtimeSkeletonReservedStubCount"])
            self.assertEqual(coverage_report_path, report["nativeHotupdateAudit"]["runtimeSkeletonCoverageReportPath"])
            self.assertEqual(1, report["nativeHotupdateAudit"]["runtimeSkeletonUncoveredMethodCount"])
            self.assertEqual("runtime-skeleton", report["nativeHotupdateAudit"]["auditStatus"])
            self.assertEqual(build_manifest_path, report["nativeHotupdateAudit"]["nativeBuildManifestPath"])
            self.assertEqual(
                ["corelib-reference-hotupdate:System.Private.CoreLib|System.Runtime|System.Console:16:3"],
                report["nativeHotupdateAudit"]["outputLines"],
            )

            audit_payload = json.loads((repo_root / audit_path).read_text(encoding="utf-8"))
            self.assertEqual("native-hotupdate-audit", audit_payload["artifactKind"])
            self.assertEqual("assembly-full-closure-runtime-skeleton", audit_payload["nativeGeneration"]["nativeReferencePlanKind"])
            self.assertEqual("assembly-bound-native-reference-skeleton", audit_payload["nativeGeneration"]["runtimeExecutionKind"])
            self.assertEqual("runtime-skeleton", audit_payload["nativeGeneration"]["translationUnitMode"])
            self.assertEqual(3, audit_payload["nativeGeneration"]["translationUnitMethodCount"])
            self.assertEqual("runtime-skeleton", audit_payload["nativeGeneration"]["auditStatus"])
            self.assertEqual(0, audit_payload["nativeGeneration"]["runtimeSkeletonReservedStubCount"])
            self.assertEqual(coverage_report_path, audit_payload["nativeGeneration"]["runtimeSkeletonCoverageReportPath"])
            self.assertEqual(1, audit_payload["nativeGeneration"]["runtimeSkeletonUncoveredMethodCount"])
            self.assertEqual("native-reference", audit_payload["nativeBuild"]["buildKind"])
            self.assertEqual(False, audit_payload["truthBoundary"]["fullCoreLibTranslated"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_build_matrix_report_surfaces_native_perf_evidence_and_report_artifacts(self) -> None:
        reporting_module = load_module(SUBJECT_REPORTING_MODULE_PATH, "chaos_subject_reporting_native_perf")
        run_id = "20260413-solution-core-native-perf-001"
        subject_id = "SolutionCorePack"
        matrix_id = "windows-native-perf"

        plan = {
            "selection": {
                "subjectId": subject_id,
                "displayName": subject_id,
                "goalId": "perf.release",
                "matrixId": matrix_id,
                "validationProfileId": "perf-profile",
                "validationKinds": ["perf"],
                "validationKind": "perf",
                "variant": "PROFILE",
                "pipelineId": "native-benchmark",
                "source": {
                    "type": "dotnet-project",
                    "path": "subjects/SolutionCorePack/source/SolutionCorePack.sln",
                        "entry": "CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()",
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
                {"stageId": "generated-native-aot", "kind": "generated-native-aot", "scope": "shared", "bucket": "generated"},
                {"stageId": "build-target", "kind": "build-target", "scope": "matrix", "bucket": "build"},
                {"stageId": "native-runtime-perf", "kind": "native-runtime-perf", "scope": "matrix", "bucket": "runtime"},
                {"stageId": "report-assemble", "kind": "report-assemble", "scope": "matrix", "bucket": "report"},
            ],
        }
        execution_result = {
            "subjectId": subject_id,
            "matrixId": matrix_id,
            "goalId": "perf.release",
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
            self.assertEqual("native-benchmark", report["selection"]["pipelineId"])
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

    def test_native_hotupdate_audit_allows_zero_emitted_runtime_skeleton_pages(self) -> None:
        reporting_module = load_module(SUBJECT_REPORTING_MODULE_PATH, "chaos_subject_reporting_native_hotupdate_zero_pages")
        subject_id = "FixtureNativeHotUpdateSubject"
        matrix_id = "windows-corelib-reference-native-hotupdate-proof"
        run_id = "20260421-fixture-native-hotupdate-zero-pages-001"
        report_path = run_bucket_path(subject_id, run_id, "matrices", matrix_id, "pipeline-report", "report.json")
        host_input_manifest_path = run_bucket_path(subject_id, run_id, "analysis", "host-input", "host-input.manifest.json")
        generated_manifest_path = run_bucket_path(subject_id, run_id, "analysis", "generated", "generated.manifest.json")
        native_reference_manifest_path = run_bucket_path(subject_id, run_id, "analysis", "generated", "native-reference.manifest.json")
        native_reference_plan_path = run_bucket_path(subject_id, run_id, "analysis", "generated", "native-reference.plan.json")
        coverage_report_path = run_bucket_path(
            subject_id,
            run_id,
            "analysis",
            "generated",
            "generated",
            "runtime",
            "native-reference.runtime-skeleton.coverage.json",
        )
        build_manifest_path = run_bucket_path(subject_id, run_id, "matrices", matrix_id, "build", "build.manifest.json")
        runtime_manifest_path = run_bucket_path(subject_id, run_id, "matrices", matrix_id, "runtime", "runtime.manifest.json")

        plan = {
            "selection": {
                "subjectId": subject_id,
                "goalId": "correctness.dev",
                "matrixId": matrix_id,
                "validationProfileId": "proof-dev",
                "validationKinds": ["proof"],
                "variant": "CHECK",
                "pipelineId": "native-hotupdate-proof-output",
                "entrySelection": {
                    "family": "declared-unit-test",
                    "stableId": f"{subject_id}::Patch::Patch.Proofs::Run()",
                    "alias": "native-hotupdate-proof",
                    "entryIndex": 60,
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                    "toolchainProfile": "msvc-reference",
                    "runtimeProfile": "native-hotupdate-proof-output",
                },
                "artifactPlan": {
                    "requiredBuckets": ["source", "host-input", "analysis", "generated", "build", "runtime", "report"],
                    "evidenceTerminalBucket": "runtime",
                },
            },
            "stagePlan": [
                {"stageId": "host-input-build", "kind": "host-input-build", "scope": "shared", "bucket": "host-input"},
                {"stageId": "generated-native-proof", "kind": "generated-native-proof", "scope": "shared", "bucket": "generated"},
                {"stageId": "build-target", "kind": "build-target", "scope": "matrix", "bucket": "build"},
                {"stageId": "runtime-managed-output", "kind": "runtime-managed-output", "scope": "matrix", "bucket": "runtime"},
                {"stageId": "report-assemble", "kind": "report-assemble", "scope": "matrix", "bucket": "report"},
            ],
        }
        execution_result = {
            "subjectId": subject_id,
            "matrixId": matrix_id,
            "goalId": "correctness.dev",
            "status": "ok",
            "terminalStageId": "runtime-managed-output",
            "terminalBucket": "runtime",
            "stageResults": [
                {
                    "stageId": "host-input-build",
                    "kind": "host-input-build",
                    "bucket": "host-input",
                    "status": "ok",
                    "manifestPath": host_input_manifest_path,
                    "reportPaths": [],
                    "primaryEvidencePaths": [run_bucket_path(subject_id, run_id, "analysis", "host-input", "NativeProofApp.dll")],
                    "diagnostics": {"stdoutPath": None, "stderrPath": None},
                },
                {
                    "stageId": "generated-native-proof",
                    "kind": "generated-native-proof",
                    "bucket": "generated",
                    "status": "ok",
                    "manifestPath": generated_manifest_path,
                    "reportPaths": [],
                    "primaryEvidencePaths": [run_bucket_path(subject_id, run_id, "analysis", "generated", "generated", "native-reference.generated.cpp")],
                    "diagnostics": {"stdoutPath": None, "stderrPath": None},
                },
                {
                    "stageId": "build-target",
                    "kind": "build-target",
                    "bucket": "build",
                    "status": "ok",
                    "manifestPath": build_manifest_path,
                    "reportPaths": [],
                    "primaryEvidencePaths": [run_bucket_path(subject_id, run_id, "matrices", matrix_id, "build", "out", "chaos_subject_reference_proof.exe")],
                    "diagnostics": {"stdoutPath": None, "stderrPath": None},
                },
                {
                    "stageId": "runtime-managed-output",
                    "kind": "runtime-managed-output",
                    "bucket": "runtime",
                    "status": "ok",
                    "manifestPath": runtime_manifest_path,
                    "reportPaths": [],
                    "primaryEvidencePaths": [run_bucket_path(subject_id, run_id, "matrices", matrix_id, "runtime", "stdout.log")],
                    "diagnostics": {
                        "stdoutPath": run_bucket_path(subject_id, run_id, "matrices", matrix_id, "runtime", "stdout.log"),
                        "stderrPath": run_bucket_path(subject_id, run_id, "matrices", matrix_id, "runtime", "stderr.log"),
                    },
                },
            ],
            "errors": [],
        }

        repo_root = TEST_TMP_ROOT / f"native-hotupdate-zero-pages-{uuid.uuid4().hex}"
        repo_root.mkdir(parents=True, exist_ok=False)
        try:
            write_json(
                repo_root / host_input_manifest_path,
                {
                    "primaryProjectPath": "subjects/Fixture/NativeProofApp.csproj",
                    "primaryAssemblyPath": run_bucket_path(subject_id, run_id, "analysis", "host-input", "NativeProofApp.dll"),
                    "additionalAssemblyPaths": [run_bucket_path(subject_id, run_id, "analysis", "host-input", "Patch.dll")],
                    "managedRuntimeProjectPath": "solutions/subjects/Fixture/hotupdate-tests/Fixture.HotUpdateProofHost.csproj",
                    "managedRuntimeAssemblyPath": run_bucket_path(subject_id, run_id, "analysis", "host-input", "Fixture.HotUpdateProofHost.dll"),
                    "collectionPath": "solutions/subjects/Fixture/hotupdate-tests/Generated/declared-tests.collection.json",
                    "bindingManifestPath": "solutions/subjects/Fixture/hotupdate-tests/Generated/declared-tests.binding.json",
                },
            )
            write_json(
                repo_root / generated_manifest_path,
                {
                    "generatedSourcePath": run_bucket_path(subject_id, run_id, "analysis", "generated", "generated", "native-reference.generated.cpp"),
                    "generatedSourcePaths": [
                        run_bucket_path(subject_id, run_id, "analysis", "generated", "generated", "native-reference.generated.cpp"),
                    ],
                    "nativeReferenceManifestPath": native_reference_manifest_path,
                    "nativeReferencePlanPath": native_reference_plan_path,
                },
            )
            write_json(
                repo_root / native_reference_manifest_path,
                {
                    "runtimeExecutionKind": "assembly-bound-native-reference-skeleton",
                    "preferredAssemblyDispatchSubjectId": "",
                    "translationUnitPageCount": 0,
                    "generatedArtifacts": [
                        {
                            "kind": "runtimeSkeletonCoverageReport",
                            "path": "generated/runtime/native-reference.runtime-skeleton.coverage.json",
                        },
                    ],
                },
            )
            write_json(
                repo_root / native_reference_plan_path,
                {
                    "planKind": "assembly-full-closure-runtime-skeleton",
                    "translationUnitMode": "runtime-skeleton",
                    "translationUnitMethodCount": 3,
                    "auditStatus": "runtime-skeleton",
                    "auditMessage": "assembly-bound full-closure native-reference runtime skeleton emits covered executable methods only; uncovered methods are reported separately",
                },
            )
            write_json(
                repo_root / coverage_report_path,
                {
                    "formatVersion": "v0",
                    "artifactKind": "nativeReferenceRuntimeSkeletonCoverage",
                    "assemblyName": "Fixture.NativeProofApp",
                    "planKind": "assembly-full-closure-runtime-skeleton",
                    "translationUnitMode": "runtime-skeleton",
                    "requestedMethodCount": 3,
                    "emittedMethodCount": 0,
                    "uncoveredMethodCount": 3,
                    "uncoveredMethodSubjectIds": [
                        "Fixture.NativeProofApp/Program::Main:System.Int32()",
                        "Fixture.NativeProofApp/Program::Helper:System.String()",
                        "Fixture.NativeProofApp/Program::Bootstrap:System.Void()",
                    ],
                },
            )
            write_json(
                repo_root / build_manifest_path,
                {
                    "buildKind": "native-reference",
                    "buildStrategy": "windows-reference-cmake",
                    "hostSourcePath": "solutions/subjects/Fixture/native-source/proof/main.cpp",
                    "generatedSourcePaths": [
                        run_bucket_path(subject_id, run_id, "analysis", "generated", "generated", "native-reference.generated.cpp"),
                    ],
                    "outputs": [
                        run_bucket_path(subject_id, run_id, "matrices", matrix_id, "build", "out", "chaos_subject_reference_proof.exe"),
                    ],
                },
            )
            write_json(
                repo_root / runtime_manifest_path,
                {
                    "managedRuntimeAssemblyPath": run_bucket_path(subject_id, run_id, "analysis", "host-input", "Fixture.HotUpdateProofHost.dll"),
                    "nativePrimaryAssemblyPath": run_bucket_path(subject_id, run_id, "analysis", "host-input", "NativeProofApp.dll"),
                    "nativeGeneratedManifestPath": generated_manifest_path,
                    "nativeBuildManifestPath": build_manifest_path,
                    "bindingManifestPath": "solutions/subjects/Fixture/hotupdate-tests/Generated/declared-tests.binding.json",
                    "arguments": ["--entry-index=60"],
                    "outputLines": ["corelib-reference-hotupdate:System.Private.CoreLib|System.Runtime|System.Console:16:3"],
                    "stdoutPath": run_bucket_path(subject_id, run_id, "matrices", matrix_id, "runtime", "stdout.log"),
                    "stderrPath": run_bucket_path(subject_id, run_id, "matrices", matrix_id, "runtime", "stderr.log"),
                    "exitCodePath": run_bucket_path(subject_id, run_id, "matrices", matrix_id, "runtime", "exit-code.txt"),
                },
            )

            report = reporting_module.build_matrix_report(
                plan,
                execution_result,
                run_id=run_id,
                generated_at="2026-04-21T02:44:18Z",
            )
            reporting_module.materialize_matrix_report_artifacts(
                repo_root,
                matrix_report_path=report_path,
                matrix_report=report,
            )

            self.assertEqual("", report["nativeHotupdateAudit"]["preferredAssemblyDispatchSubjectId"])
            self.assertEqual(0, report["nativeHotupdateAudit"]["runtimeSkeletonReservedStubCount"])
            self.assertEqual(coverage_report_path, report["nativeHotupdateAudit"]["runtimeSkeletonCoverageReportPath"])
            self.assertEqual(3, report["nativeHotupdateAudit"]["runtimeSkeletonUncoveredMethodCount"])

            audit_path = run_bucket_path(subject_id, run_id, "matrices", matrix_id, "pipeline-report", "report", "native-hotupdate-audit.json")
            audit_payload = json.loads((repo_root / audit_path).read_text(encoding="utf-8"))
            self.assertEqual("", audit_payload["nativeGeneration"]["preferredAssemblyDispatchSubjectId"])
            self.assertEqual(0, audit_payload["nativeGeneration"]["translationUnitPageCount"])
            self.assertEqual(3, audit_payload["nativeGeneration"]["runtimeSkeletonUncoveredMethodCount"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
