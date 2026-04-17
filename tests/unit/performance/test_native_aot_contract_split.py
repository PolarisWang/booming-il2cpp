from __future__ import annotations

import importlib.util
import json
import shutil
import sys
import unittest
import uuid
from pathlib import Path
from unittest.mock import patch


REPO_ROOT = Path(__file__).resolve().parents[3]
BENCH_ARITHMETIC_MANIFEST_PATH = REPO_ROOT / "subjects" / "SolutionCorePack" / "subject.manifest.json"
SUBJECT_WORKERS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subject_workers.py"
SUBJECT_EXECUTOR_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subject_executor.py"
MANAGED_CONTRACTS_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedClosureContracts.cs"
MANAGED_ARTIFACT_MODELS_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedClosureArtifactModels.cs"
DRIVER_ENTRY_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "DriverEntry.cs"
CODEGEN_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "CodeGenStage.cs"
NATIVE_AOT_PLANNER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "NativeAotLoweringPlanner.cs"
NATIVE_AOT_METHOD_EMISSION_PATH = (
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "Emission" / "NativeAotLoweringPlanner.MethodEmission.cs"
)
NATIVE_AOT_TRANSLATION_TEMPLATE_PATH = (
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "Templates" / "NativeAot.TranslationUnit.cpp.scriban"
)
NATIVE_AOT_LOWERING_PLAN_SAMPLE_PATH = (
    REPO_ROOT / "tests" / "contracts" / "analysis" / "v0" / "samples" / "native-aot.lowering-plan.min.json"
)
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "phase-b-aot-contract-split"


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


def posix_path(*parts: str) -> str:
    return Path(*parts).as_posix()


def subject_run_path(subject_id: str, run_id: str, *parts: str) -> str:
    return posix_path("artifacts", "subjects", subject_id, "runs", run_id, *parts)


class PhaseBAotContractSplitTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)

    def _make_repo_root(self, prefix: str) -> Path:
        repo_root = TEST_TMP_ROOT / f"{prefix}-{uuid.uuid4().hex}"
        repo_root.mkdir(parents=True, exist_ok=False)
        return repo_root

    def test_bench_arithmetic_native_pipelines_route_through_generated_native_aot(self) -> None:
        manifest = json.loads(BENCH_ARITHMETIC_MANIFEST_PATH.read_text(encoding="utf-8"))
        pipelines = {
            pipeline["pipelineId"]: pipeline
            for pipeline in manifest["executionPipelines"]
        }

        for pipeline_id in ("native-benchmark",):
            pipeline = pipelines[pipeline_id]
            stage_kinds = [stage["kind"] for stage in pipeline["stages"]]
            self.assertIn("generated-native-aot", stage_kinds)
            self.assertNotIn("generated-native-proof", stage_kinds)

            generated_stage = next(stage for stage in pipeline["stages"] if stage["stageId"] == "generated-native-aot")
            self.assertEqual("generated-native-aot", generated_stage["kind"])
            self.assertEqual(["analysis-frontend"], generated_stage["dependsOn"])

            build_stage = next(stage for stage in pipeline["stages"] if stage["stageId"] == "build-target")
            self.assertEqual(["generated-native-aot"], build_stage["dependsOn"])

    def test_subject_workers_emit_native_aot_bundle_and_register_stage_kind(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_native_aot_emitter")
        subject_id = "FixtureGeneratedSubject"
        run_id = "fixture-run-generated-aot-001"
        request = {
            "selection": {
                "subjectId": subject_id,
                "workloadEntry": f"{subject_id}/Program::RunWorkload()",
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

        repo_root = self._make_repo_root("native-aot-emitter")
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
                    result = workers_module.run_native_aot_emitter(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            self.assertIs(workers_module.DEFAULT_STAGE_WORKERS["generated-native-aot"], workers_module.run_native_aot_emitter)

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(subject_id, manifest["subjectId"])
            self.assertEqual("generated", manifest["bucket"])
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
            self.assertNotIn("workloadEntry", manifest)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_subject_executor_rewrites_reused_generated_native_aot_evidence_paths(self) -> None:
        executor_module = load_module(SUBJECT_EXECUTOR_MODULE_PATH, "chaos_subject_executor_generated_native_aot")
        stage = {
            "kind": "generated-native-aot",
            "bucket": "generated",
            "reuse": {
                "existingManifestPath": subject_run_path("FixtureGeneratedSubject", "previous-run", "analysis", "generated", "generated.manifest.json"),
            },
            "paths": {
                "bucketRoot": subject_run_path("FixtureGeneratedSubject", "current-run", "analysis", "generated"),
                "manifestPath": subject_run_path("FixtureGeneratedSubject", "current-run", "analysis", "generated", "generated.manifest.json"),
            },
        }
        manifest = {
            "generatedSourcePath": subject_run_path("FixtureGeneratedSubject", "previous-run", "analysis", "generated", "generated", "native-aot.generated.cpp"),
            "nativeAotManifestPath": subject_run_path("FixtureGeneratedSubject", "previous-run", "analysis", "generated", "native-aot.manifest.json"),
        }

        self.assertEqual(
            [
                subject_run_path("FixtureGeneratedSubject", "current-run", "analysis", "generated", "generated", "native-aot.generated.cpp"),
                subject_run_path("FixtureGeneratedSubject", "current-run", "analysis", "generated", "native-aot.manifest.json"),
            ],
            executor_module._reused_primary_evidence_paths(stage, manifest),
        )

    def test_managed_sources_freeze_native_aot_contract_split_markers(self) -> None:
        contracts_source = MANAGED_CONTRACTS_PATH.read_text(encoding="utf-8")
        artifact_models_source = MANAGED_ARTIFACT_MODELS_PATH.read_text(encoding="utf-8")
        driver_source = DRIVER_ENTRY_PATH.read_text(encoding="utf-8")
        codegen_source = CODEGEN_STAGE_PATH.read_text(encoding="utf-8")

        self.assertIn('NativeAotLoweringPlan = "native-aot.lowering-plan.json"', contracts_source)
        self.assertIn("public sealed record NativeAotRequest", artifact_models_source)
        self.assertIn("public sealed record NativeAotLoweringPlanArtifact", artifact_models_source)
        self.assertIn("public sealed record NativeAotManifestArtifact", artifact_models_source)
        self.assertIn("public sealed record NativeAotResult", artifact_models_source)

        self.assertIn('"emit-native-aot"', driver_source)
        self.assertIn("RunLegacyEmitNativeAot", driver_source)
        self.assertIn("new NativeAotRequest", driver_source)

        self.assertIn("NativeAotLoweringPlanArtifact", codegen_source)
        self.assertIn("CreateNativeAotLoweringPlan(", codegen_source)
        self.assertIn("NativeAotLoweringPlan =", codegen_source)

    def test_native_aot_shared_benchmark_host_contract_supports_int32_entry_bridge(self) -> None:
        planner_source = NATIVE_AOT_PLANNER_PATH.read_text(encoding="utf-8")
        method_emission_source = NATIVE_AOT_METHOD_EMISSION_PATH.read_text(encoding="utf-8")
        template_source = NATIVE_AOT_TRANSLATION_TEMPLATE_PATH.read_text(encoding="utf-8")
        lowering_plan_sample = json.loads(NATIVE_AOT_LOWERING_PLAN_SAMPLE_PATH.read_text(encoding="utf-8"))

        self.assertIn(
            "supports only zero-parameter or single-int32 entry methods",
            planner_source,
        )
        self.assertIn(
            "supports only zero-parameter or single-int32 entry methods",
            method_emission_source,
        )
        self.assertNotIn("must not take parameters", method_emission_source)
        self.assertIn(
            'extern "C" int {{ native_entry_function_name }}(std::int32_t chaos_entry_index)',
            template_source,
        )
        self.assertEqual("int(int32)", lowering_plan_sample["workloadAbi"])


if __name__ == "__main__":
    unittest.main()
