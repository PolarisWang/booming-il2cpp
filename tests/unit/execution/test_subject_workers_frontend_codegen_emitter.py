from tests.unit.execution.subject_workers_test_support import *


class TestSubjectWorkersFrontendCodegenEmitter(SubjectWorkersTestSupport):
    def test_native_proof_emitter_prefers_runtime_skeleton_generated_translation_units(self) -> None:
        workers_module = load_module(
            SUBJECT_WORKERS_MODULE_PATH,
            "chaos_subject_workers_native_proof_emitter_runtime_skeleton",
        )
        subject_id = "FixtureGeneratedRuntimeSkeletonSubject"
        run_id = "fixture-run-generated-runtime-skeleton-001"
        request = {
            "selection": {
                "subjectId": subject_id,
            },
            "upstream": {
                "analysis": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "analysis", "analysis.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "analysis", "generated"),
                "manifestPath": subject_run_path(subject_id, run_id, "analysis", "generated", "generated.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("native-proof-emitter-runtime-skeleton")
        try:
            analysis_manifest_path = repo_root / request["upstream"]["analysis"]["manifestPath"]
            analysis_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            analysis_manifest_path.write_text("{}", encoding="utf-8")

            expected_analysis_root = repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "analysis"
            expected_output_root = repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "generated"

            def fake_run_checked(arguments: list[str], *, repo_root: Path, failure_message: str) -> str:
                del failure_message
                self.assertEqual(
                    [
                        "dotnet",
                        str(repo_root / "driver" / "Chaos.IL2CPP.Driver.dll"),
                        "emit-native-reference",
                        str(expected_analysis_root),
                        str(expected_output_root),
                    ],
                    arguments,
                )
                runtime_root = expected_output_root / "generated" / "runtime"
                runtime_root.mkdir(parents=True, exist_ok=True)
                (runtime_root / "native-reference.runtime-skeleton.generated.cpp").write_text(
                    "// generated runtime skeleton",
                    encoding="utf-8",
                )
                (runtime_root / "native-reference.runtime-skeleton.page-0001.cpp").write_text(
                    "// generated runtime page",
                    encoding="utf-8",
                )
                (expected_output_root / "native-reference.manifest.json").write_text(
                    json.dumps(
                        {
                            "generatedArtifacts": [
                                {
                                    "kind": "generatedTranslationUnit",
                                    "path": "generated/runtime/native-reference.runtime-skeleton.generated.cpp",
                                },
                                {
                                    "kind": "generatedTranslationUnit",
                                    "path": "generated/runtime/native-reference.runtime-skeleton.page-0001.cpp",
                                },
                            ]
                        }
                    ),
                    encoding="utf-8",
                )
                (expected_output_root / "native-reference.plan.json").write_text("{}", encoding="utf-8")
                return ""

            with patch.object(workers_module, "_ensure_driver_built", return_value=repo_root / "driver" / "Chaos.IL2CPP.Driver.dll"):
                with patch.object(workers_module, "_run_checked", side_effect=fake_run_checked):
                    result = workers_module.run_native_proof_emitter(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(
                subject_run_path(
                    subject_id,
                    run_id,
                    "analysis",
                    "generated",
                    "generated",
                    "runtime",
                    "native-reference.runtime-skeleton.generated.cpp",
                ),
                manifest["generatedSourcePath"],
            )
            self.assertEqual(
                [
                    subject_run_path(
                        subject_id,
                        run_id,
                        "analysis",
                        "generated",
                        "generated",
                        "runtime",
                        "native-reference.runtime-skeleton.generated.cpp",
                    ),
                    subject_run_path(
                        subject_id,
                        run_id,
                        "analysis",
                        "generated",
                        "generated",
                        "runtime",
                        "native-reference.runtime-skeleton.page-0001.cpp",
                    ),
                ],
                manifest["generatedSourcePaths"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_native_proof_emitter_records_generated_bundle_under_subject_artifacts(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_native_proof_emitter")
        subject_id = "FixtureGeneratedSubject"
        run_id = "fixture-run-generated-001"
        request = {
            "selection": {
                "subjectId": subject_id,
            },
            "upstream": {
                "analysis": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "analysis", "analysis.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "analysis", "generated"),
                "manifestPath": subject_run_path(subject_id, run_id, "analysis", "generated", "generated.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("native-proof-emitter")
        try:
            analysis_manifest_path = repo_root / request["upstream"]["analysis"]["manifestPath"]
            analysis_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            analysis_manifest_path.write_text("{}", encoding="utf-8")

            expected_analysis_root = repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "analysis"
            expected_output_root = repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "generated"

            def fake_run_checked(arguments: list[str], *, repo_root: Path, failure_message: str) -> str:
                del failure_message
                self.assertEqual(
                    [
                        "dotnet",
                        str(repo_root / "driver" / "Chaos.IL2CPP.Driver.dll"),
                        "emit-native-reference",
                        str(expected_analysis_root),
                        str(expected_output_root),
                    ],
                    arguments,
                )
                (expected_output_root / "generated").mkdir(parents=True, exist_ok=True)
                (expected_output_root / "generated" / "native-reference.generated.cpp").write_text("// generated", encoding="utf-8")
                (expected_output_root / "generated" / "runtime").mkdir(parents=True, exist_ok=True)
                (expected_output_root / "generated" / "runtime" / "native-reference.runtime-skeleton.page-0001.cpp").write_text(
                    "// generated page",
                    encoding="utf-8",
                )
                (expected_output_root / "native-reference.manifest.json").write_text(
                    json.dumps(
                        {
                            "generatedArtifacts": [
                                {
                                    "kind": "generatedTranslationUnit",
                                    "path": "generated/native-reference.generated.cpp",
                                },
                                {
                                    "kind": "generatedTranslationUnit",
                                    "path": "generated/runtime/native-reference.runtime-skeleton.page-0001.cpp",
                                },
                            ]
                        }
                    ),
                    encoding="utf-8",
                )
                (expected_output_root / "native-reference.plan.json").write_text("{}", encoding="utf-8")
                return ""

            with patch.object(workers_module, "_ensure_driver_built", return_value=repo_root / "driver" / "Chaos.IL2CPP.Driver.dll"):
                with patch.object(workers_module, "_run_checked", side_effect=fake_run_checked):
                    result = workers_module.run_native_proof_emitter(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(subject_id, manifest["subjectId"])
            self.assertEqual("generated", manifest["bucket"])
            self.assertEqual("CHECK", manifest["variant"])
            self.assertEqual(
                ["CHAOS_VARIANT_CHECK", "CHAOS_VARIANT_NAME=CHECK"],
                manifest["codegenMacros"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "generated", "generated", "native-reference.generated.cpp"),
                manifest["generatedSourcePath"],
            )
            self.assertEqual(
                [
                    subject_run_path(subject_id, run_id, "analysis", "generated", "generated", "native-reference.generated.cpp"),
                    subject_run_path(subject_id, run_id, "analysis", "generated", "generated", "runtime", "native-reference.runtime-skeleton.page-0001.cpp"),
                ],
                manifest["generatedSourcePaths"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "generated", "native-reference.manifest.json"),
                manifest["nativeReferenceManifestPath"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "generated", "native-reference.plan.json"),
                manifest["nativeReferencePlanPath"],
            )
            self.assertNotIn("workloadEntry", manifest)
            self.assertNotIn("nativeProofManifestPath", manifest)
            self.assertNotIn("nativeProofPlanPath", manifest)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_native_proof_emitter_keeps_native_reference_for_native_hotupdate_chain(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_native_proof_emitter_native_hotupdate_chain")
        subject_id = "FixtureGeneratedNativeHotUpdateChainSubject"
        run_id = "fixture-run-generated-native-hotupdate-chain-001"
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": "windows-corelib-reference-native-hotupdate-proof",
                "engineeringProfile": "hot-update-host",
                "entrySelection": {
                    "family": "declared-unit-test",
                    "stableId": f"{subject_id}::{subject_id}.Patch::{subject_id}.Patch.Proofs::Run()",
                    "alias": "native-hotupdate-proof",
                    "entryIndex": 7,
                },
                "source": {
                    "type": "dotnet-project",
                    "entry": "",
                    "fullAssemblyClosure": True,
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "toolchainProfile": "msvc-reference",
                    "runtimeProfile": "native-hotupdate-proof-output",
                },
            },
            "upstream": {
                "analysis": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "analysis", "analysis.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "analysis", "generated"),
                "manifestPath": subject_run_path(subject_id, run_id, "analysis", "generated", "generated.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("native-proof-emitter-native-hotupdate-chain")
        try:
            analysis_manifest_path = repo_root / request["upstream"]["analysis"]["manifestPath"]
            analysis_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            analysis_manifest_path.write_text("{}", encoding="utf-8")

            expected_analysis_root = repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "analysis"
            expected_output_root = repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "generated"

            def fake_run_checked(arguments: list[str], *, repo_root: Path, failure_message: str) -> str:
                del failure_message
                self.assertEqual(
                    [
                        "dotnet",
                        str(repo_root / "driver" / "Chaos.IL2CPP.Driver.dll"),
                        "emit-native-reference",
                        str(expected_analysis_root),
                        str(expected_output_root),
                    ],
                    arguments,
                )
                (expected_output_root / "generated").mkdir(parents=True, exist_ok=True)
                (expected_output_root / "generated" / "native-reference.generated.cpp").write_text("// generated", encoding="utf-8")
                (expected_output_root / "native-reference.manifest.json").write_text("{}", encoding="utf-8")
                (expected_output_root / "native-reference.plan.json").write_text("{}", encoding="utf-8")
                return ""

            with patch.object(workers_module, "_ensure_driver_built", return_value=repo_root / "driver" / "Chaos.IL2CPP.Driver.dll"):
                with patch.object(workers_module, "_run_checked", side_effect=fake_run_checked):
                    result = workers_module.run_native_proof_emitter(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "generated", "generated", "native-reference.generated.cpp"),
                manifest["generatedSourcePath"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "generated", "native-reference.manifest.json"),
                manifest["nativeReferenceManifestPath"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "generated", "native-reference.plan.json"),
                manifest["nativeReferencePlanPath"],
            )
            self.assertNotIn("nativeAotManifestPath", manifest)
            self.assertNotIn("nativeAotPlanPath", manifest)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_native_aot_emitter_surfaces_codegen_metrics_and_baseline_gate(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_native_aot_emitter_codegen_metrics")
        subject_id = "FixtureGeneratedNativeAotMetricsSubject"
        run_id = "fixture-run-generated-native-aot-metrics-001"
        matrix_id = "windows-native-perf"
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "PROFILE",
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "runtimeProfile": "native-perf-profile",
                },
            },
            "upstream": {
                "analysis": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "analysis", "analysis.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "analysis", "generated"),
                "manifestPath": subject_run_path(subject_id, run_id, "analysis", "generated", "generated.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("native-aot-emitter-codegen-metrics")
        try:
            analysis_manifest_path = repo_root / request["upstream"]["analysis"]["manifestPath"]
            analysis_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            analysis_manifest_path.write_text("{}", encoding="utf-8")

            baseline_path = (
                repo_root
                / "subjects"
                / subject_id
                / "baselines"
                / "codegen"
                / matrix_id
                / "windows.json"
            )
            baseline_path.parent.mkdir(parents=True, exist_ok=True)
            baseline_path.write_text(
                json.dumps(
                    {
                        "generatedCppTotalBytes": 256,
                        "generatedSymbolCount": 5,
                        "peakWorkingSetBytes": 1024,
                    }
                ),
                encoding="utf-8",
            )

            expected_analysis_root = repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "analysis"
            expected_output_root = repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "generated"

            def fake_run_checked(arguments: list[str], *, repo_root: Path, failure_message: str) -> str:
                del failure_message
                self.assertEqual(
                    [
                        "dotnet",
                        str(repo_root / "driver" / "Chaos.IL2CPP.Driver.dll"),
                        "emit-native-aot",
                        str(expected_analysis_root),
                        str(expected_output_root),
                    ],
                    arguments,
                )
                (expected_output_root / "generated").mkdir(parents=True, exist_ok=True)
                (expected_output_root / "generated" / "native-aot.generated.cpp").write_text(
                    "extern \"C\" int chaos_method_fixture(std::int32_t chaos_arg_0);\n",
                    encoding="utf-8",
                )
                (expected_output_root / "native-aot.manifest.json").write_text(
                    json.dumps(
                        {
                            "generatedArtifacts": [
                                {
                                    "kind": "generatedTranslationUnit",
                                    "path": "generated/native-aot.generated.cpp",
                                },
                                {
                                    "kind": "codegenMetrics",
                                    "path": "native-aot.codegen-metrics.json",
                                },
                            ]
                        }
                    ),
                    encoding="utf-8",
                )
                (expected_output_root / "native-aot.plan.json").write_text(
                    json.dumps({"planKind": "generic-managed-entry"}),
                    encoding="utf-8",
                )
                (expected_output_root / "native-aot.codegen-metrics.json").write_text(
                    json.dumps(
                        {
                            "artifactKind": "nativeCodegenMetrics",
                            "codegenKind": "native-aot",
                            "planKind": "generic-managed-entry",
                            "generatedCppFileCount": 1,
                            "generatedCppTotalBytes": 512,
                            "largestGeneratedCppBytes": 512,
                            "generatedSymbolCount": 8,
                            "peakWorkingSetBytes": 2048,
                        }
                    ),
                    encoding="utf-8",
                )
                return ""

            with patch.object(workers_module, "_ensure_driver_built", return_value=repo_root / "driver" / "Chaos.IL2CPP.Driver.dll"):
                with patch.object(workers_module, "_run_checked", side_effect=fake_run_checked):
                    result = workers_module.run_native_aot_emitter(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "generated", "native-aot.codegen-metrics.json"),
                manifest["codegenMetricsPath"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "generated", "codegen-baseline-compare.json"),
                manifest["codegenBaselineComparePath"],
            )
            self.assertEqual("regressed", manifest["codegenRegressionStatus"])

            baseline_compare = json.loads(
                (repo_root / manifest["codegenBaselineComparePath"]).read_text(encoding="utf-8")
            )
            self.assertEqual(
                {
                    "generatedCppTotalBytes": 256,
                    "generatedSymbolCount": 5,
                    "peakWorkingSetBytes": 1024,
                },
                baseline_compare["baseline"],
            )
            self.assertEqual("regressed", baseline_compare["regressionStatus"])
            self.assertEqual(
                "regressed",
                result["details"]["codegenPerformance"]["regressionStatus"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
