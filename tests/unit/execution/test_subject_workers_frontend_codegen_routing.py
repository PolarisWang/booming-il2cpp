from tests.unit.execution.subject_workers_test_support import *


class TestSubjectWorkersFrontendCodegenRouting(SubjectWorkersTestSupport):
    def test_native_proof_emitter_routes_declared_unit_test_selection_through_native_aot(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_native_proof_emitter_declared_unit_test")
        subject_id = "FixtureGeneratedDeclaredProofSubject"
        run_id = "fixture-run-generated-declared-proof-001"
        request = {
            "selection": {
                "subjectId": subject_id,
                "entrySelection": {
                    "family": "declared-unit-test",
                    "stableId": f"{subject_id}::{subject_id}::{subject_id}.Proofs::Run()",
                    "alias": "fixture-proof",
                    "entryIndex": 7,
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

        repo_root = self._make_repo_root("native-proof-emitter-declared-proof")
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
                        "emit-native-aot",
                        str(expected_analysis_root),
                        str(expected_output_root),
                    ],
                    arguments,
                )
                (expected_output_root / "generated").mkdir(parents=True, exist_ok=True)
                (expected_output_root / "generated" / "native-aot.generated.cpp").write_text("// generated", encoding="utf-8")
                (expected_output_root / "native-aot.manifest.json").write_text("{}", encoding="utf-8")
                (expected_output_root / "native-aot.plan.json").write_text("{}", encoding="utf-8")
                return ""

            with patch.object(workers_module, "_ensure_driver_built", return_value=repo_root / "driver" / "Chaos.IL2CPP.Driver.dll"):
                with patch.object(workers_module, "_run_checked", side_effect=fake_run_checked):
                    result = workers_module.run_native_proof_emitter(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "generated", "generated", "native-aot.generated.cpp"),
                manifest["generatedSourcePath"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "generated", "native-aot.manifest.json"),
                manifest["nativeAotManifestPath"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "generated", "native-aot.plan.json"),
                manifest["nativeAotPlanPath"],
            )
            self.assertNotIn("nativeReferenceManifestPath", manifest)
            self.assertNotIn("nativeReferencePlanPath", manifest)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_native_proof_emitter_routes_subject_entry_selection_through_native_aot(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_native_proof_emitter_subject_entry_selection")
        subject_id = "FixtureGeneratedManagedEntrySubject"
        run_id = "fixture-run-generated-managed-entry-001"
        request = {
            "selection": {
                "subjectId": subject_id,
                "source": {
                    "entry": f"{subject_id}/Program::Run()",
                    "entrySelection": {
                        "entryKind": 1,
                        "entrySlice": 11,
                    },
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

        repo_root = self._make_repo_root("native-proof-emitter-subject-entry-selection")
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
                        "emit-native-aot",
                        str(expected_analysis_root),
                        str(expected_output_root),
                    ],
                    arguments,
                )
                (expected_output_root / "generated").mkdir(parents=True, exist_ok=True)
                (expected_output_root / "generated" / "native-aot.generated.cpp").write_text("// generated", encoding="utf-8")
                (expected_output_root / "native-aot.manifest.json").write_text("{}", encoding="utf-8")
                (expected_output_root / "native-aot.plan.json").write_text("{}", encoding="utf-8")
                return ""

            with patch.object(workers_module, "_ensure_driver_built", return_value=repo_root / "driver" / "Chaos.IL2CPP.Driver.dll"):
                with patch.object(workers_module, "_run_checked", side_effect=fake_run_checked):
                    result = workers_module.run_native_proof_emitter(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "generated", "generated", "native-aot.generated.cpp"),
                manifest["generatedSourcePath"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "generated", "native-aot.manifest.json"),
                manifest["nativeAotManifestPath"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "generated", "native-aot.plan.json"),
                manifest["nativeAotPlanPath"],
            )
            self.assertNotIn("nativeReferenceManifestPath", manifest)
            self.assertNotIn("nativeReferencePlanPath", manifest)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
