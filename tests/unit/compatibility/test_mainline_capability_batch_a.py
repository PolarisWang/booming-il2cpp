from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
SOURCE_ROOT = REPO_ROOT / "subjects" / "SolutionCorePack" / "source" / "Slices" / "MainlineFeaturePack"
LOADER_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "LoaderStage.cs"
SEMANTIC_WORLD_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.SemanticWorld" / "SemanticWorldStage.cs"
LINKER_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Linker" / "LinkerStage.cs"
LOWERING_PLANNER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "NativeReferenceLoweringPlanner.cs"
EMITTER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "NativeReferenceProofEmitter.cs"
BOOTSTRAP_PATH = REPO_ROOT / "src" / "native" / "bootstrap" / "bootstrap.cpp"
RUNTIME_CORE_PATH = REPO_ROOT / "src" / "native" / "runtime-core" / "runtime_core.cpp"
RUNTIME_CORE_HEADER_PATH = REPO_ROOT / "src" / "native" / "runtime-core" / "runtime_core.h"
DISPATCH_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.DispatchVirtualInstanceMessage.cpp.scriban"
)
ARRAY_BOXING_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.ArrayBoxingReferenceArray.cpp.scriban"
)


class Phase5CapabilityBatchATests(unittest.TestCase):
    def test_mainline_feature_pack_source_tree_realizes_phase5_capability_proof_slices(self) -> None:
        dispatch_source_path = SOURCE_ROOT / "DispatchProof.cs"
        generic_layout_source_path = SOURCE_ROOT / "GenericLayoutProof.cs"
        array_boxing_source_path = SOURCE_ROOT / "ArrayBoxingProof.cs"

        self.assertTrue(dispatch_source_path.is_file(), msg=f"missing dispatch slice: {dispatch_source_path}")
        self.assertTrue(generic_layout_source_path.is_file(), msg=f"missing generic/layout slice: {generic_layout_source_path}")
        self.assertTrue(array_boxing_source_path.is_file(), msg=f"missing array/boxing slice: {array_boxing_source_path}")

        dispatch_source = dispatch_source_path.read_text(encoding="utf-8")
        generic_layout_source = generic_layout_source_path.read_text(encoding="utf-8")
        array_boxing_source = array_boxing_source_path.read_text(encoding="utf-8")

        self.assertIn("internal abstract class DispatchBase", dispatch_source)
        self.assertIn("internal sealed class DispatchLeaf", dispatch_source)
        self.assertIn("internal static class DispatchProofEntry", dispatch_source)
        self.assertIn("DispatchBase instance = new DispatchLeaf(", dispatch_source)

        self.assertIn("internal sealed class GenericBox<T>", generic_layout_source)
        self.assertIn("internal static class GenericEcho", generic_layout_source)
        self.assertIn("internal static class GenericLayoutProofEntry", generic_layout_source)
        self.assertIn("GenericBox<string>", generic_layout_source)

        self.assertIn("internal static class BoxingSink", array_boxing_source)
        self.assertIn("internal static class ArrayBoxingProofEntry", array_boxing_source)
        self.assertIn("BoxingSink.Consume(42);", array_boxing_source)
        self.assertIn("new FeatureBanner[]", array_boxing_source)

    def test_loader_semantic_and_linker_lock_batch_a_surface(self) -> None:
        loader_source = LOADER_STAGE_PATH.read_text(encoding="utf-8")
        semantic_world_source = SEMANTIC_WORLD_PATH.read_text(encoding="utf-8")
        linker_source = LINKER_STAGE_PATH.read_text(encoding="utf-8")

        self.assertIn("ILOpCode.Newarr", loader_source)
        self.assertIn("ILOpCode.Ldlen", loader_source)
        self.assertIn("ILOpCode.Box", loader_source)
        self.assertIn("ILOpCode.Conv_i4", loader_source)
        self.assertIn("ILOpCode.Ldc_r8", loader_source)
        self.assertIn("ILOpCode.Ldloca_s", loader_source)
        self.assertIn("ILOpCode.Ldelema", loader_source)
        self.assertIn("ILOpCode.Stelem_ref", loader_source)
        self.assertIn("ILOpCode.Stelem_i4", loader_source)
        self.assertIn("ILOpCode.Stelem_r8", loader_source)
        self.assertIn('Op = "newarr"', loader_source)
        self.assertIn('Op = "ldlen"', loader_source)
        self.assertIn('Op = "box"', loader_source)
        self.assertIn('Op = "conv.i4"', loader_source)
        self.assertIn('Op = "ldc.r8"', loader_source)
        self.assertIn('Op = "ldloca"', loader_source)
        self.assertIn('Op = "ldelema"', loader_source)
        self.assertIn('Op = "stelem.ref"', loader_source)
        self.assertIn('DecodeTypedArrayInstruction("stelem"', loader_source)
        self.assertIn('DecodeTypedArrayInstruction("ldelem"', loader_source)

        self.assertIn("requires-array-allocation", semantic_world_source)
        self.assertIn("requires-array-length-query", semantic_world_source)
        self.assertIn("requires-value-array-store", semantic_world_source)
        self.assertIn("requires-value-array-load", semantic_world_source)
        self.assertIn("requires-boxing", semantic_world_source)
        self.assertIn("uses-virtual-call-site", semantic_world_source)

        self.assertIn("TryResolveDevirtualizedCallTarget", linker_source)
        self.assertIn("closed-generic-instance", linker_source)
        self.assertIn("callvirt.devirtualized-direct", linker_source)
        self.assertIn('"System.Private.CoreLib/System.Int32" => "boxed-value-type"', linker_source)

    def test_codegen_and_native_side_freeze_batch_a_lowering_and_helper_surface(self) -> None:
        planner_source = LOWERING_PLANNER_PATH.read_text(encoding="utf-8")
        emitter_source = EMITTER_PATH.read_text(encoding="utf-8")
        bootstrap_source = BOOTSTRAP_PATH.read_text(encoding="utf-8")
        runtime_core_source = RUNTIME_CORE_PATH.read_text(encoding="utf-8")
        runtime_core_header_source = RUNTIME_CORE_HEADER_PATH.read_text(encoding="utf-8")

        self.assertTrue(DISPATCH_TEMPLATE_PATH.is_file(), msg=f"missing dispatch template: {DISPATCH_TEMPLATE_PATH}")
        self.assertTrue(
            ARRAY_BOXING_TEMPLATE_PATH.is_file(),
            msg=f"missing array/boxing template: {ARRAY_BOXING_TEMPLATE_PATH}",
        )

        dispatch_template_source = DISPATCH_TEMPLATE_PATH.read_text(encoding="utf-8")
        array_boxing_template_source = ARRAY_BOXING_TEMPLATE_PATH.read_text(encoding="utf-8")

        for lowering_family in [
            "managed-dispatch.virtual-instance-message.minimal",
            "managed-generic.static-forwarder-captured-getter.minimal",
            "managed-arrays-boxing.reference-array-boxed-int.minimal",
        ]:
            self.assertIn(lowering_family, planner_source)
            self.assertIn(lowering_family, emitter_source)

        self.assertIn("DispatchStrategy", planner_source)
        self.assertIn("BoxedValueTypeToken", planner_source)
        self.assertIn('RequireInstructionOpLike(instructions[3], method.SubjectId, 3, "call", "callvirt")', planner_source)
        self.assertIn('RequireMethodContract(method, "static-forwarder", "has-canonical-body");', planner_source)
        self.assertIn('RequireMethodContract(method, "instance-field-getter", "has-canonical-body");', planner_source)
        self.assertIn("DispatchStrategy", emitter_source)
        self.assertIn("BoxedValueTypeToken", emitter_source)

        self.assertIn("bridge->resolve_virtual_method", dispatch_template_source)
        self.assertIn("bridge->invoke_virtual", dispatch_template_source)
        self.assertIn("bridge->box_value", array_boxing_template_source)
        self.assertIn("abi->array_new", array_boxing_template_source)
        self.assertIn("ArrayStoreReference", array_boxing_template_source)
        self.assertIn("ArrayLoadReference", array_boxing_template_source)

        self.assertIn("unresolved_virtual_calls", bootstrap_source)
        self.assertIn("BoxValueObject", bootstrap_source)
        self.assertIn("UnboxValueObject", bootstrap_source)
        self.assertIn("InvokeVirtual", bootstrap_source)

        self.assertIn("ArrayStoreReference", runtime_core_source)
        self.assertIn("ArrayLoadReference", runtime_core_source)
        self.assertIn("BoxValueObject", runtime_core_source)
        self.assertIn("UnboxValueObject", runtime_core_source)
        self.assertIn("ArrayStoreReference", runtime_core_header_source)
        self.assertIn("ArrayLoadReference", runtime_core_header_source)


if __name__ == "__main__":
    unittest.main()
