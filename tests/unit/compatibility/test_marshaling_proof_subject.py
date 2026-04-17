from __future__ import annotations

import json
import unittest
from pathlib import Path

from tests.support import (
    read_contracts_source,
    read_linker_stage_source,
    read_loader_stage_source,
    read_native_reference_planner_source,
)


REPO_ROOT = Path(__file__).resolve().parents[3]
SUBJECT_ROOT = REPO_ROOT / "subjects" / "SolutionCorePack"
MANIFEST_PATH = SUBJECT_ROOT / "subject.manifest.json"
SOURCE_PROJECT_PATH = SUBJECT_ROOT / "source" / "Proofs" / "CoreRuntimeFeatures" / "CoreRuntimeFeatures.csproj"
SOURCE_PROGRAM_PATH = SUBJECT_ROOT / "source" / "Proofs" / "CoreRuntimeFeatures" / "InteropAndMarshaling" / "MarshalingProof.cs"
PROFILE_ID = "proof-marshaling"
MATRIX_ID = "windows-marshaling-check"

CONTRACTS_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedClosureContracts.cs"
LOADER_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "LoaderStage.cs"
SEMANTIC_WORLD_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.SemanticWorld" / "SemanticWorldStage.cs"
LINKER_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Linker" / "LinkerStage.cs"
LOWERING_PLANNER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "NativeReferenceLoweringPlanner.cs"
EMITTER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "NativeReferenceProofEmitter.cs"
CATALOG_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "NativeReferenceProofCatalog.cs"
)
TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.MarshalingUtf8Export.cpp.scriban"
)


class Phase2MarshalingProofTests(unittest.TestCase):
    def test_marshaling_proof_subject_tree_realizes_phase2_batch4_slice(self) -> None:
        self.assertTrue(MANIFEST_PATH.is_file(), msg=f"missing subject manifest: {MANIFEST_PATH}")
        self.assertTrue(SOURCE_PROJECT_PATH.is_file(), msg=f"missing source project: {SOURCE_PROJECT_PATH}")
        self.assertTrue(SOURCE_PROGRAM_PATH.is_file(), msg=f"missing source file: {SOURCE_PROGRAM_PATH}")

        manifest = json.loads(MANIFEST_PATH.read_text(encoding="utf-8"))
        source = SOURCE_PROGRAM_PATH.read_text(encoding="utf-8")
        self.assertEqual("SolutionCorePack", manifest["subjectId"])
        self.assertEqual("dotnet-project", manifest["source"]["type"])
        self.assertEqual("subjects/SolutionCorePack/source/SolutionCorePack.sln", manifest["source"]["path"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/Host/SolutionCorePack.csproj",
            manifest["source"]["primaryProjectPath"],
        )
        self.assertEqual("CoreRuntimeFeatures/ProofEntry::Run()", manifest["source"]["entry"])
        self.assertEqual("require", manifest["testDeclarationMode"])
        self.assertEqual("proof", manifest["validation"]["proof"]["kind"])

        self.assertIn("[ChaosUnitTest(", source)
        self.assertIn('Alias = "marshaling-proof"', source)
        self.assertIn("[DllImport(\"kernel32.dll\", ExactSpelling = true)]", source)
        self.assertIn("[UnmanagedCallersOnly(EntryPoint =", source)
        self.assertIn("Marshal.StringToCoTaskMemUTF8", source)
        self.assertIn("Marshal.PtrToStringUTF8", source)
        self.assertIn("Marshal.FreeCoTaskMem", source)
        self.assertIn("delegate* unmanaged<int, int, int>", source)
        self.assertIn("internal static class MarshalingProofEntry", source)

    def test_contract_loader_semantic_and_linker_lock_marshaling_surface(self) -> None:
        contracts_source = read_contracts_source(REPO_ROOT)
        loader_source = read_loader_stage_source(REPO_ROOT)
        semantic_world_source = SEMANTIC_WORLD_PATH.read_text(encoding="utf-8")
        linker_source = read_linker_stage_source(REPO_ROOT)

        self.assertIn("public bool IsUnmanagedCallersOnly { get; init; }", contracts_source)
        self.assertIn("HasUnmanagedCallersOnlyAttribute(", loader_source)
        self.assertIn("requires-utf8-string-marshaling", semantic_world_source)
        self.assertIn("requires-unmanaged-callers-only-export", semantic_world_source)
        self.assertIn("HasUnmanagedExportShape(", linker_source)
        self.assertIn("utf8-string-marshal", linker_source)

    def test_codegen_surface_freezes_marshaling_lowering_family(self) -> None:
        planner_source = read_native_reference_planner_source(REPO_ROOT)
        emitter_source = EMITTER_PATH.read_text(encoding="utf-8")
        catalog_source = CATALOG_PATH.read_text(encoding="utf-8")

        self.assertTrue(TEMPLATE_PATH.is_file(), msg=f"missing marshaling template: {TEMPLATE_PATH}")
        template_source = TEMPLATE_PATH.read_text(encoding="utf-8")

        self.assertIn("interop.marshaling-utf8-export.minimal", catalog_source)
        self.assertIn("NativeReferenceProofCatalog.MarshalingUtf8ExportMinimal", planner_source)
        self.assertIn("MatchesMarshalingUtf8ExportCandidate(", planner_source)
        self.assertIn("TryCreateMarshalingUtf8ExportLoweringPlan(", planner_source)
        self.assertIn("NativeReferenceProofCatalog.MarshalingUtf8ExportMinimal", emitter_source)
        self.assertIn("case MarshalingUtf8ExportMinimal:", emitter_source)
        self.assertIn("marshal-ok", template_source)
        self.assertIn("export-ok", template_source)


if __name__ == "__main__":
    unittest.main()

