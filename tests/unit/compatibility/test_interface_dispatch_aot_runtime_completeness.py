from __future__ import annotations

import json
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
SUBJECT_ROOT = REPO_ROOT / "subjects" / "MainlineFeaturePack"
MANIFEST_PATH = SUBJECT_ROOT / "subject.manifest.json"
SOURCE_PROJECT_PATH = REPO_ROOT / "subjects" / "MainlineFeaturePack" / "source" / "MainlineFeaturePack.csproj"
SOURCE_PROGRAM_PATH = REPO_ROOT / "subjects" / "MainlineFeaturePack" / "source" / "InterfaceDispatchProof.cs"
PROOF_CMAKE_PATH = SUBJECT_ROOT / "validation" / "proof" / "native-reference" / "CMakeLists.txt"
PROOF_MAIN_PATH = SUBJECT_ROOT / "validation" / "proof" / "native-reference" / "main.cpp"
PROOF_RUN_SCRIPT_PATH = SUBJECT_ROOT / "validation" / "proof" / "native-reference" / "RunNativeReferenceProof.cmake"
PROFILE_ID = "proof-interface-dispatch"
MATRIX_ID = "windows-interface-dispatch-check"

CONTRACTS_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedClosureContracts.cs"
LOADER_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "LoaderStage.cs"
SEMANTIC_WORLD_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.SemanticWorld" / "SemanticWorldStage.cs"
LINKER_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Linker" / "LinkerStage.cs"
LOWERING_PLANNER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "NativeReferenceLoweringPlanner.cs"
EMITTER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "NativeReferenceProofEmitter.cs"
TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.InterfaceDispatchMessage.cpp.scriban"
)


class Phase2AotRuntimeCompletenessTests(unittest.TestCase):
    def test_interface_dispatch_subject_tree_realizes_phase2_batch1_proof_slice(self) -> None:
        self.assertTrue(MANIFEST_PATH.is_file(), msg=f"missing subject manifest: {MANIFEST_PATH}")
        self.assertTrue(SOURCE_PROJECT_PATH.is_file(), msg=f"missing source project: {SOURCE_PROJECT_PATH}")
        self.assertTrue(SOURCE_PROGRAM_PATH.is_file(), msg=f"missing source file: {SOURCE_PROGRAM_PATH}")
        self.assertTrue(PROOF_CMAKE_PATH.is_file(), msg=f"missing proof cmake: {PROOF_CMAKE_PATH}")
        self.assertTrue(PROOF_MAIN_PATH.is_file(), msg=f"missing proof host main: {PROOF_MAIN_PATH}")
        self.assertTrue(PROOF_RUN_SCRIPT_PATH.is_file(), msg=f"missing proof run script: {PROOF_RUN_SCRIPT_PATH}")

        manifest = json.loads(MANIFEST_PATH.read_text(encoding="utf-8"))
        source = SOURCE_PROGRAM_PATH.read_text(encoding="utf-8")
        validation_profiles = dict(manifest.get("validationProfiles") or {})
        matrices = {
            str(matrix["matrixId"]): matrix
            for matrix in list(manifest.get("environmentMatrices") or [])
        }

        self.assertEqual("MainlineFeaturePack", manifest["subjectId"])
        self.assertEqual("dotnet-project", manifest["source"]["type"])
        self.assertEqual("subjects/MainlineFeaturePack/source/MainlineFeaturePack.csproj", manifest["source"]["path"])
        self.assertEqual("MainlineFeaturePack/ProofEntry::Run()", manifest["source"]["entry"])
        self.assertEqual("require", manifest["testDeclarationMode"])
        self.assertEqual(["proof"], validation_profiles[PROFILE_ID])
        self.assertEqual("proof", manifest["validation"]["proof"]["kind"])
        self.assertEqual(
            "MainlineFeaturePack/InterfaceDispatchProofEntry::Run()",
            matrices[MATRIX_ID]["source"]["entry"],
        )

        self.assertIn("[ChaosUnitTest(", source)
        self.assertIn('Alias = "interface-dispatch-proof"', source)
        self.assertIn("internal interface IMessageBuilder", source)
        self.assertIn("internal sealed class InterfaceMessageBuilder", source)
        self.assertIn("internal static class InterfaceDispatchProofEntry", source)
        self.assertIn("((IMessageBuilder)new InterfaceMessageBuilder(", source)

    def test_loader_semantic_and_linker_lock_interface_dispatch_surface(self) -> None:
        contracts_source = CONTRACTS_PATH.read_text(encoding="utf-8")
        loader_source = LOADER_STAGE_PATH.read_text(encoding="utf-8")
        semantic_world_source = SEMANTIC_WORLD_PATH.read_text(encoding="utf-8")
        linker_source = LINKER_STAGE_PATH.read_text(encoding="utf-8")

        self.assertIn("public bool IsInterface { get; init; }", contracts_source)
        self.assertIn("typeDefinition.Attributes.HasFlag(TypeAttributes.Interface)", loader_source)
        self.assertIn("definitionType.IsInterface", loader_source)
        self.assertIn("uses-interface-call-site", semantic_world_source)
        self.assertIn("callvirt.interface-runtime-helper", linker_source)

    def test_codegen_surface_freezes_interface_dispatch_lowering_family(self) -> None:
        planner_source = LOWERING_PLANNER_PATH.read_text(encoding="utf-8")
        emitter_source = EMITTER_PATH.read_text(encoding="utf-8")

        self.assertTrue(TEMPLATE_PATH.is_file(), msg=f"missing interface dispatch template: {TEMPLATE_PATH}")
        template_source = TEMPLATE_PATH.read_text(encoding="utf-8")

        self.assertIn("managed-dispatch.interface-message.minimal", planner_source)
        self.assertIn("managed-dispatch.interface-message.minimal", emitter_source)
        self.assertIn("MatchesInterfaceDispatchMessageCandidate(", planner_source)
        self.assertIn("TryCreateInterfaceDispatchMessageLoweringPlan(", planner_source)
        self.assertIn('DispatchStrategy = "interface-runtime-helper"', planner_source)
        self.assertIn("case ManagedInterfaceDispatchMessageMinimal:", emitter_source)
        self.assertIn("bridge->resolve_virtual_method", template_source)
        self.assertIn("bridge->invoke_virtual", template_source)


if __name__ == "__main__":
    unittest.main()
