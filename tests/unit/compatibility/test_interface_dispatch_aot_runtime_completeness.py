from __future__ import annotations

import json
import unittest
from pathlib import Path

from tests.support import (
    SOLUTION_CORE_PACK_HOST_PROJECT_PATH,
    SOLUTION_CORE_PACK_HOST_SOLUTION_PATH,
    SOLUTION_CORE_PACK_OWNER_MANIFEST_PATH,
    SOLUTION_CORE_PACK_PROOFS_PROJECT_PATH,
    SOLUTION_CORE_PACK_PROOFS_ROOT,
    read_contracts_source,
    read_linker_stage_source,
    read_loader_stage_source,
    read_native_reference_planner_source,
)


REPO_ROOT = Path(__file__).resolve().parents[3]
MANIFEST_PATH = SOLUTION_CORE_PACK_OWNER_MANIFEST_PATH
SOURCE_PROJECT_PATH = SOLUTION_CORE_PACK_PROOFS_PROJECT_PATH
SOURCE_PROGRAM_PATH = SOLUTION_CORE_PACK_PROOFS_ROOT / "ObjectModelAndDispatch" / "InterfaceDispatchProof.cs"
PROFILE_ID = "proof-interface-dispatch"
MATRIX_ID = "windows-interface-dispatch-check"

SEMANTIC_WORLD_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.SemanticWorld" / "SemanticWorldStage.cs"
EMITTER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "NativeReferenceProofEmitter.cs"
CATALOG_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "ReferenceProof" / "NativeReferenceProofCatalog.cs"
TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.InterfaceDispatchMessage.cpp.scriban"
)


class InterfaceDispatchAotRuntimeCompletenessTests(unittest.TestCase):
    def test_interface_dispatch_subject_tree_realizes_proof_slice(self) -> None:
        self.assertTrue(MANIFEST_PATH.is_file(), msg=f"missing subject manifest: {MANIFEST_PATH}")
        self.assertTrue(SOURCE_PROJECT_PATH.is_file(), msg=f"missing source project: {SOURCE_PROJECT_PATH}")
        self.assertTrue(SOURCE_PROGRAM_PATH.is_file(), msg=f"missing source file: {SOURCE_PROGRAM_PATH}")

        manifest = json.loads(MANIFEST_PATH.read_text(encoding="utf-8"))
        source = SOURCE_PROGRAM_PATH.read_text(encoding="utf-8")
        self.assertEqual("SolutionCorePack", manifest["subjectId"])
        self.assertEqual("dotnet-project", manifest["source"]["type"])
        self.assertEqual(SOLUTION_CORE_PACK_HOST_SOLUTION_PATH.relative_to(REPO_ROOT).as_posix(), manifest["source"]["path"])
        self.assertEqual(SOLUTION_CORE_PACK_HOST_PROJECT_PATH.relative_to(REPO_ROOT).as_posix(), manifest["source"]["primaryProjectPath"])
        self.assertEqual("CoreRuntimeFeatures/ProofEntry::Run()", manifest["source"]["entry"])
        self.assertEqual("require", manifest["testDeclarationMode"])
        self.assertEqual("proof", manifest["validation"]["proof"]["kind"])

        self.assertIn("[ChaosUnitTest(", source)
        self.assertIn('Alias = "interface-dispatch-proof"', source)
        self.assertIn("internal interface IDispatchValue<T>", source)
        self.assertIn("internal sealed class InterfaceValue<T>", source)
        self.assertIn("internal static class InterfaceDispatchProofEntry", source)
        self.assertIn("IDispatchValue<int> value = new InterfaceValue<int>(21);", source)
        self.assertIn("Assert.Equal(21, value.ReadValue());", source)

    def test_loader_semantic_and_linker_lock_interface_dispatch_surface(self) -> None:
        contracts_source = read_contracts_source(REPO_ROOT)
        loader_source = read_loader_stage_source(REPO_ROOT)
        semantic_world_source = SEMANTIC_WORLD_PATH.read_text(encoding="utf-8")
        linker_source = read_linker_stage_source(REPO_ROOT)

        self.assertIn("public bool IsInterface { get; init; }", contracts_source)
        self.assertIn("typeDefinition.Attributes.HasFlag(TypeAttributes.Interface)", loader_source)
        self.assertIn("definitionType.IsInterface", loader_source)
        self.assertIn("uses-interface-call-site", semantic_world_source)
        self.assertIn("callvirt.interface-runtime-helper", linker_source)

    def test_codegen_surface_freezes_interface_dispatch_lowering_family(self) -> None:
        planner_source = read_native_reference_planner_source(REPO_ROOT)
        emitter_source = EMITTER_PATH.read_text(encoding="utf-8")
        catalog_source = CATALOG_PATH.read_text(encoding="utf-8")

        self.assertTrue(TEMPLATE_PATH.is_file(), msg=f"missing interface dispatch template: {TEMPLATE_PATH}")
        template_source = TEMPLATE_PATH.read_text(encoding="utf-8")

        self.assertIn("managed-dispatch.interface-message.minimal", catalog_source)
        self.assertIn("ManagedInterfaceDispatchMessageMinimal", planner_source)
        self.assertIn("ManagedInterfaceDispatchMessageMinimal", emitter_source)
        self.assertIn("MatchesInterfaceDispatchMessageCandidate(", planner_source)
        self.assertIn("TryCreateInterfaceDispatchMessageLoweringPlan(", planner_source)
        self.assertIn('DispatchStrategy = "interface-runtime-helper"', planner_source)
        self.assertIn("case ManagedInterfaceDispatchMessageMinimal:", emitter_source)
        self.assertIn("bridge->resolve_virtual_method", template_source)
        self.assertIn("bridge->invoke_virtual", template_source)


if __name__ == "__main__":
    unittest.main()

