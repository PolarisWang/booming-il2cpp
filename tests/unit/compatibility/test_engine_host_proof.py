from __future__ import annotations

import json
import unittest
from pathlib import Path

from tests.support import read_native_reference_planner_source


REPO_ROOT = Path(__file__).resolve().parents[3]
LOWERING_PLANNER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Generator" / "NativeReferenceLoweringPlanner.cs"
EMITTER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Generator" / "NativeReferenceProofEmitter.cs"
CATALOG_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "NativeReferenceProofCatalog.cs"
)
HOST_PROOF_TEMPLATE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Generator" / "Templates" / "NativeReferenceProof.EngineHostProof.cpp.scriban"

ENGINE_HOST_PROOF_ROOT = REPO_ROOT / "tests" / "fixtures" / "subjects" / "EngineHostProof"
ENGINE_HOST_PROOF_MANIFEST_PATH = ENGINE_HOST_PROOF_ROOT / "subject.manifest.json"
ENGINE_HOST_PROOF_PROJECT_PATH = ENGINE_HOST_PROOF_ROOT / "source" / "EngineHostProof.csproj"
ENGINE_HOST_PROOF_PROGRAM_PATH = ENGINE_HOST_PROOF_ROOT / "source" / "Program.cs"
ENGINE_HOST_PROOF_MAIN_PATH = ENGINE_HOST_PROOF_ROOT / "validation" / "proof" / "native-reference" / "main.cpp"
ENGINE_HOST_PROOF_EXPECTED_RUNTIME_PATH = ENGINE_HOST_PROOF_ROOT / "expected" / "runtime" / "windows-reference-trace" / "host-proof.json"


class EngineHostProofTests(unittest.TestCase):
    def test_engine_host_proof_tree_and_expected_runtime_baseline_exist(self) -> None:
        self.assertTrue(ENGINE_HOST_PROOF_ROOT.is_dir(), msg=f"missing EngineHostProof root: {ENGINE_HOST_PROOF_ROOT}")
        self.assertTrue(ENGINE_HOST_PROOF_MANIFEST_PATH.is_file(), msg=f"missing EngineHostProof manifest: {ENGINE_HOST_PROOF_MANIFEST_PATH}")
        self.assertTrue(ENGINE_HOST_PROOF_PROJECT_PATH.is_file(), msg=f"missing EngineHostProof project: {ENGINE_HOST_PROOF_PROJECT_PATH}")
        self.assertTrue(ENGINE_HOST_PROOF_PROGRAM_PATH.is_file(), msg=f"missing EngineHostProof program: {ENGINE_HOST_PROOF_PROGRAM_PATH}")
        self.assertTrue(ENGINE_HOST_PROOF_MAIN_PATH.is_file(), msg=f"missing EngineHostProof proof host: {ENGINE_HOST_PROOF_MAIN_PATH}")
        self.assertTrue(
            ENGINE_HOST_PROOF_EXPECTED_RUNTIME_PATH.is_file(),
            msg=f"missing EngineHostProof expected runtime baseline: {ENGINE_HOST_PROOF_EXPECTED_RUNTIME_PATH}",
        )

    def test_engine_host_proof_codegen_and_worker_surface_lock_minimal_host_chain(self) -> None:
        planner_source = read_native_reference_planner_source(REPO_ROOT)
        emitter_source = EMITTER_PATH.read_text(encoding="utf-8")
        catalog_source = CATALOG_PATH.read_text(encoding="utf-8")
        template_source = HOST_PROOF_TEMPLATE_PATH.read_text(encoding="utf-8")

        self.assertIn("engine.host-proof.minimal", catalog_source)
        self.assertIn("Templates/NativeReferenceProof.EngineHostProof.cpp.scriban", catalog_source)
        self.assertIn("NativeReferenceProofCatalog.EngineHostProofMinimal", planner_source)
        self.assertIn("EngineHostProof/EngineHostEntry::Run()", planner_source)
        self.assertIn("NativeReferenceProofCatalog.EngineHostProofMinimal", emitter_source)
        self.assertIn("NativeReferenceProofCatalog.GetTemplateForPlan(", emitter_source)

        for required_fragment in [
            '#include "engine_bridge.h"',
            "engine_init(",
            "engine_register_callback(",
            "engine_dispatch_callback(",
            "engine_retain_object(",
            "engine_release_object(",
            "engine_shutdown(",
        ]:
            self.assertIn(required_fragment, template_source)

    def test_engine_host_proof_manifest_declares_windows_engine_runtime_surface(self) -> None:
        manifest = json.loads(ENGINE_HOST_PROOF_MANIFEST_PATH.read_text(encoding="utf-8"))
        profile = dict(manifest.get("engineProofProfile") or {})
        pipeline_ids = {
            str(pipeline.get("pipelineId") or "")
            for pipeline in list(manifest.get("executionPipelines") or [])
        }
        matrix_ids = {
            str(matrix.get("matrixId") or "")
            for matrix in list(manifest.get("environmentMatrices") or [])
        }

        self.assertEqual("EngineHostProof", manifest["subjectId"])
        self.assertEqual("windows-dev-output", manifest["defaultMatrix"])
        self.assertEqual("proof-dev", manifest["defaultValidationProfile"])
        self.assertEqual("engine-binding", str(profile.get("proofKind") or ""))
        self.assertEqual("host-proof", str(profile.get("focusArea") or ""))
        self.assertIn("host-proof", list(profile.get("expectedEvidenceKinds") or []))
        self.assertEqual({"engine-runtime-output", "engine-runtime-trace"}, pipeline_ids)
        self.assertEqual({"windows-dev-output", "windows-reference-trace"}, matrix_ids)

        expected_runtime = json.loads(ENGINE_HOST_PROOF_EXPECTED_RUNTIME_PATH.read_text(encoding="utf-8"))
        self.assertEqual(1000, expected_runtime["stressIterations"])


if __name__ == "__main__":
    unittest.main()
