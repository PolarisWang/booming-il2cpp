from tests.unit.execution.subject_workers_test_support import *


class TestSubjectWorkersFrontendCodegenAnalysis(SubjectWorkersTestSupport):
    def test_frontend_pipeline_worker_records_analysis_bundle_under_subject_artifacts(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_frontend_pipeline")
        subject_id = "FixtureAnalysisSubject"
        run_id = "fixture-run-analysis-001"
        request = {
            "selection": {
                "subjectId": subject_id,
                "variant": "CHECK",
                "source": {
                    "entry": f"{subject_id}/ProofEntry::Run()",
                    "fullAssemblyClosure": True,
                },
            },
            "upstream": {
                "host-input": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "host-input", "host-input.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "analysis", "analysis"),
                "manifestPath": subject_run_path(subject_id, run_id, "analysis", "analysis", "analysis.manifest.json"),
                "reportPaths": [
                    subject_run_path(subject_id, run_id, "analysis", "analysis", "contract-validate.report.json"),
                ],
            },
        }

        repo_root = self._make_repo_root("frontend-pipeline")
        try:
            host_input_manifest_path = repo_root / request["upstream"]["host-input"]["manifestPath"]
            host_input_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            host_input_manifest_path.write_text(
                json.dumps(
                    {
                        "primaryAssemblyPath": subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.dll"),
                        "additionalAssemblyPaths": [
                            subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.Helper.dll"),
                        ],
                    }
                ),
                encoding="utf-8",
            )
            subject_manifest_path = repo_root / "verification" / "catalog" / "owners" / subject_id / "owner.manifest.json"
            subject_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            subject_manifest_path.write_text(
                json.dumps(
                    {
                        "subjectId": subject_id,
                    }
                ),
                encoding="utf-8",
            )

            expected_host_input = (
                repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "host-input" / f"{subject_id}.dll"
            )
            expected_additional_host_input = (
                repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "host-input" / f"{subject_id}.Helper.dll"
            )
            expected_output_root = repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "analysis"

            def fake_run_checked(arguments: list[str], *, repo_root: Path, failure_message: str) -> str:
                del failure_message
                self.assertEqual(
                    [
                        "dotnet",
                        str(repo_root / "driver" / "Chaos.IL2CPP.Driver.dll"),
                        str(expected_host_input),
                        str(expected_output_root),
                        "--entry-point-subject-id",
                        f"{subject_id}/ProofEntry::Run()",
                        "--additional-assembly",
                        str(expected_additional_host_input),
                        "--full-assembly-closure",
                    ],
                    arguments,
                )
                expected_output_root.mkdir(parents=True, exist_ok=True)
                for name in [
                    "typed-il-ir.json",
                    "aot-manifest.json",
                    "metadata-registration.json",
                    "code-registration.json",
                    "generic-instantiation-demand-graph.json",
                    "optimization-facts.json",
                    "preserve-descriptor.json",
                    "closure.manifest.json",
                ]:
                    (expected_output_root / name).write_text("{}", encoding="utf-8")
                return ""

            with patch.object(workers_module, "_ensure_driver_built", return_value=repo_root / "driver" / "Chaos.IL2CPP.Driver.dll"):
                with patch.object(workers_module, "_run_checked", side_effect=fake_run_checked):
                    with patch.object(workers_module.contracts_module, "validate_analysis_contracts") as validate_mock:
                        result = workers_module.run_frontend_pipeline_worker(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            validate_mock.assert_called_once_with(repo_root)

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(subject_id, manifest["subjectId"])
            self.assertEqual("analysis", manifest["bucket"])
            self.assertEqual("proof-input-bundle", manifest["bundleKind"])
            self.assertTrue(manifest["fullAssemblyClosure"])
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "analysis", "typed-il-ir.json"),
                manifest["artifacts"]["typedIlIrPath"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "analysis", "closure.manifest.json"),
                manifest["artifacts"]["closureManifestPath"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "analysis", "optimization-facts.json"),
                manifest["artifacts"]["optimizationFactsPath"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "analysis", "generic-instantiation-demand-graph.json"),
                manifest["artifacts"]["genericInstantiationDemandGraphPath"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "analysis", "preserve-descriptor.json"),
                manifest["artifacts"]["preserveDescriptorPath"],
            )

            report = json.loads((repo_root / request["paths"]["reportPaths"][0]).read_text(encoding="utf-8"))
            self.assertEqual("ok", report["status"])
            self.assertEqual("contracts/artifacts/v0/schemas", report["schemaPath"])
            self.assertEqual("tests/contracts/analysis/v0/snapshots", report["snapshotPath"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_frontend_pipeline_worker_records_supplemental_full_closure_bundles(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_frontend_pipeline_supplemental")
        subject_id = "FixtureAnalysisSubject"
        run_id = "fixture-run-analysis-supplemental-001"
        supplemental_input_path = "src/dll/dotnet-foundation/net8.0/runtime/System.Private.CoreLib.dll"
        request = {
            "selection": {
                "subjectId": subject_id,
                "variant": "CHECK",
                "source": {
                    "entry": f"{subject_id}/ProofEntry::Run()",
                    "fullAssemblyClosure": True,
                    "supplementalFullClosureAssemblies": [
                        {
                            "assemblyName": "System.Private.CoreLib",
                            "inputAssemblyPath": supplemental_input_path,
                        }
                    ],
                },
            },
            "upstream": {
                "host-input": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "host-input", "host-input.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "analysis", "analysis"),
                "manifestPath": subject_run_path(subject_id, run_id, "analysis", "analysis", "analysis.manifest.json"),
                "reportPaths": [
                    subject_run_path(subject_id, run_id, "analysis", "analysis", "contract-validate.report.json"),
                ],
            },
        }

        repo_root = self._make_repo_root("frontend-pipeline-supplemental")
        try:
            host_input_manifest_path = repo_root / request["upstream"]["host-input"]["manifestPath"]
            host_input_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            host_input_manifest_path.write_text(
                json.dumps(
                    {
                        "primaryAssemblyPath": subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.dll"),
                        "additionalAssemblyPaths": [],
                    }
                ),
                encoding="utf-8",
            )
            subject_manifest_path = repo_root / "verification" / "catalog" / "owners" / subject_id / "owner.manifest.json"
            subject_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            subject_manifest_path.write_text(json.dumps({"subjectId": subject_id}), encoding="utf-8")

            expected_host_input = repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "host-input" / f"{subject_id}.dll"
            expected_output_root = repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "analysis"
            expected_supplemental_output_root = expected_output_root / "supplemental-full-closures" / "system-private-corelib"
            observed_arguments: list[list[str]] = []

            def write_analysis_outputs(output_root: Path) -> None:
                output_root.mkdir(parents=True, exist_ok=True)
                for name in [
                    "typed-il-ir.json",
                    "aot-manifest.json",
                    "metadata-registration.json",
                    "code-registration.json",
                    "generic-instantiation-demand-graph.json",
                    "generic-capability-matrix.json",
                    "optimization-facts.json",
                    "preserve-descriptor.json",
                    "closure.manifest.json",
                ]:
                    (output_root / name).write_text("{}", encoding="utf-8")
                (output_root / "native-reference.lowering-plan.json").write_text("{}", encoding="utf-8")
                (output_root / "native-aot.lowering-plan.json").write_text("{}", encoding="utf-8")

            def fake_run_checked(arguments: list[str], *, repo_root: Path, failure_message: str) -> str:
                del failure_message
                observed_arguments.append(arguments)
                if len(observed_arguments) == 1:
                    self.assertEqual(
                        [
                            "dotnet",
                            str(repo_root / "driver" / "Chaos.IL2CPP.Driver.dll"),
                            str(expected_host_input),
                            str(expected_output_root),
                            "--entry-point-subject-id",
                            f"{subject_id}/ProofEntry::Run()",
                            "--full-assembly-closure",
                        ],
                        arguments,
                    )
                    write_analysis_outputs(expected_output_root)
                    return ""
                if len(observed_arguments) == 2:
                    self.assertEqual(
                        [
                            "dotnet",
                            str(repo_root / "driver" / "Chaos.IL2CPP.Driver.dll"),
                            str(repo_root / supplemental_input_path),
                            str(expected_supplemental_output_root),
                            "--full-assembly-closure",
                        ],
                        arguments,
                    )
                    write_analysis_outputs(expected_supplemental_output_root)
                    return ""
                self.fail(f"unexpected driver invocation: {arguments}")

            with patch.object(workers_module, "_ensure_driver_built", return_value=repo_root / "driver" / "Chaos.IL2CPP.Driver.dll"):
                with patch.object(workers_module, "_run_checked", side_effect=fake_run_checked):
                    with patch.object(workers_module.contracts_module, "validate_analysis_contracts") as validate_mock:
                        result = workers_module.run_frontend_pipeline_worker(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            self.assertEqual(2, len(observed_arguments))
            validate_mock.assert_called_once_with(repo_root)

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            supplemental_closures = manifest["supplementalFullAssemblyClosures"]
            self.assertEqual(1, len(supplemental_closures))
            self.assertEqual(
                {
                    "assemblyName": "System.Private.CoreLib",
                    "inputAssemblyPath": supplemental_input_path,
                    "analysisRootPath": subject_run_path(
                        subject_id,
                        run_id,
                        "analysis",
                        "analysis",
                        "supplemental-full-closures",
                        "system-private-corelib",
                    ),
                    "closureManifestPath": subject_run_path(
                        subject_id,
                        run_id,
                        "analysis",
                        "analysis",
                        "supplemental-full-closures",
                        "system-private-corelib",
                        "closure.manifest.json",
                    ),
                    "nativeReferencePlanPath": subject_run_path(
                        subject_id,
                        run_id,
                        "analysis",
                        "analysis",
                        "supplemental-full-closures",
                        "system-private-corelib",
                        "native-reference.lowering-plan.json",
                    ),
                    "nativeAotPlanPath": subject_run_path(
                        subject_id,
                        run_id,
                        "analysis",
                        "analysis",
                        "supplemental-full-closures",
                        "system-private-corelib",
                        "native-aot.lowering-plan.json",
                    ),
                },
                supplemental_closures[0],
            )
            self.assertIn(
                subject_run_path(
                    subject_id,
                    run_id,
                    "analysis",
                    "analysis",
                    "supplemental-full-closures",
                    "system-private-corelib",
                    "closure.manifest.json",
                ),
                result["primaryEvidencePaths"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
