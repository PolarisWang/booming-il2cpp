from __future__ import annotations

import json
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
SUBJECT_ROOT = REPO_ROOT / "subjects" / "SolutionCorePack"
MANIFEST_PATH = SUBJECT_ROOT / "subject.manifest.json"
SOURCE_PROJECT_PATH = SUBJECT_ROOT / "source" / "Slices" / "MainlineFeaturePack" / "MainlineFeaturePack.csproj"
SOURCE_PROGRAM_PATH = SUBJECT_ROOT / "source" / "Slices" / "MainlineFeaturePack" / "NestedExceptionProof.cs"
PROOF_CMAKE_PATH = SUBJECT_ROOT / "validation" / "proof" / "native-reference" / "CMakeLists.txt"
PROOF_MAIN_PATH = SUBJECT_ROOT / "validation" / "proof" / "native-reference" / "main.cpp"
PROOF_RUN_SCRIPT_PATH = SUBJECT_ROOT / "validation" / "proof" / "native-reference" / "RunNativeReferenceProof.cmake"
PROFILE_ID = "proof-nested-exception"
MATRIX_ID = "windows-nested-exception-check"

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
    / "NativeReferenceProof.NestedExceptionThrowCatchFinally.cpp.scriban"
)


class Phase2NestedExceptionProofTests(unittest.TestCase):
    def test_nested_exception_subject_tree_realizes_phase2_batch4_slice(self) -> None:
        self.assertTrue(MANIFEST_PATH.is_file(), msg=f"missing subject manifest: {MANIFEST_PATH}")
        self.assertTrue(SOURCE_PROJECT_PATH.is_file(), msg=f"missing source project: {SOURCE_PROJECT_PATH}")
        self.assertTrue(SOURCE_PROGRAM_PATH.is_file(), msg=f"missing source file: {SOURCE_PROGRAM_PATH}")
        self.assertTrue(PROOF_CMAKE_PATH.is_file(), msg=f"missing proof cmake: {PROOF_CMAKE_PATH}")
        self.assertTrue(PROOF_MAIN_PATH.is_file(), msg=f"missing proof host main: {PROOF_MAIN_PATH}")
        self.assertTrue(PROOF_RUN_SCRIPT_PATH.is_file(), msg=f"missing proof run script: {PROOF_RUN_SCRIPT_PATH}")

        manifest = json.loads(MANIFEST_PATH.read_text(encoding="utf-8"))
        source = SOURCE_PROGRAM_PATH.read_text(encoding="utf-8")
        self.assertEqual("SolutionCorePack", manifest["subjectId"])
        self.assertEqual("dotnet-project", manifest["source"]["type"])
        self.assertEqual("subjects/SolutionCorePack/source/SolutionCorePack.sln", manifest["source"]["path"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/Launcher/SolutionCorePack.csproj",
            manifest["source"]["primaryProjectPath"],
        )
        self.assertEqual("MainlineFeaturePack/ProofEntry::Run()", manifest["source"]["entry"])
        self.assertEqual("require", manifest["testDeclarationMode"])
        self.assertEqual("proof", manifest["validation"]["proof"]["kind"])

        self.assertIn("[ChaosUnitTest(", source)
        self.assertIn('Alias = "nested-exception-proof"', source)
        self.assertIn("internal sealed class NestedExceptionHarness", source)
        self.assertIn("catch (InvalidOperationException ex)", source)
        self.assertIn('Assert.Equal("Nested EH native proof: inner caught.", message);', source)
        self.assertIn("throw new InvalidOperationException(", source)
        self.assertGreaterEqual(source.count("try"), 2)
        self.assertGreaterEqual(source.count("finally"), 2)
        self.assertIn("internal static class NestedExceptionProofEntry", source)

    def test_loader_semantic_and_linker_lock_nested_exception_surface(self) -> None:
        loader_source = LOADER_STAGE_PATH.read_text(encoding="utf-8")
        semantic_world_source = SEMANTIC_WORLD_PATH.read_text(encoding="utf-8")
        linker_source = LINKER_STAGE_PATH.read_text(encoding="utf-8")

        self.assertIn("ExceptionRegionKind.Filter", loader_source)
        self.assertIn("ExceptionRegionKind.Fault", loader_source)
        self.assertIn("requires-nested-exception-handler", semantic_world_source)
        self.assertIn("HasNestedExceptionHandlerShape(", linker_source)
        self.assertIn("nested-throw-catch-finally", linker_source)

    def test_codegen_surface_freezes_nested_exception_lowering_family(self) -> None:
        planner_source = LOWERING_PLANNER_PATH.read_text(encoding="utf-8")
        emitter_source = EMITTER_PATH.read_text(encoding="utf-8")

        self.assertTrue(TEMPLATE_PATH.is_file(), msg=f"missing nested exception template: {TEMPLATE_PATH}")
        template_source = TEMPLATE_PATH.read_text(encoding="utf-8")

        self.assertIn("managed-exceptions.nested-throw-catch-finally.minimal", planner_source)
        self.assertIn("MatchesNestedExceptionCandidate(", planner_source)
        self.assertIn("TryCreateNestedExceptionLoweringPlan(", planner_source)
        self.assertIn("managed-exceptions.nested-throw-catch-finally.minimal", emitter_source)
        self.assertIn("case NestedExceptionThrowCatchFinallyMinimal:", emitter_source)
        self.assertIn("ManagedExceptionCarrier", template_source)
        self.assertIn("Nested EH inner finally.", template_source)
        self.assertIn("Nested EH outer finally.", template_source)


if __name__ == "__main__":
    unittest.main()
