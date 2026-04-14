from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
SOURCE_ROOT = REPO_ROOT / "subjects" / "SolutionCorePack" / "source" / "FeatureSlices" / "CoreRuntimeFeatures"
CONTRACTS_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedClosureContracts.cs"
LOADER_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "LoaderStage.cs"
SEMANTIC_WORLD_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.SemanticWorld" / "SemanticWorldStage.cs"
LINKER_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Linker" / "LinkerStage.cs"
LOWERING_PLANNER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "NativeReferenceLoweringPlanner.cs"
EMITTER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "NativeReferenceProofEmitter.cs"
BOOTSTRAP_PATH = REPO_ROOT / "src" / "native" / "bootstrap" / "bootstrap.cpp"
RUNTIME_CORE_PATH = REPO_ROOT / "src" / "native" / "runtime-core" / "runtime_core.cpp"
RUNTIME_CORE_HEADER_PATH = REPO_ROOT / "src" / "native" / "runtime-core" / "runtime_core.h"
DELEGATE_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.DelegateClosedTargetRelay.cpp.scriban"
)
EXCEPTION_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.ExceptionThrowCatchFinally.cpp.scriban"
)
REFLECTION_INTEROP_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.ReflectionInteropClosure.cpp.scriban"
)


class Phase6CapabilityBatchBTests(unittest.TestCase):
    def test_mainline_feature_pack_source_tree_realizes_phase6_capability_proof_slices(self) -> None:
        delegate_source_path = SOURCE_ROOT / "ObjectModelAndDispatch" / "DelegateProof.cs"
        exception_source_path = SOURCE_ROOT / "ExceptionsAndControlFlow" / "ExceptionProof.cs"
        closure_source_path = SOURCE_ROOT / "ReflectionAndMetadata" / "ReflectionInteropClosureProof.cs"

        self.assertTrue(delegate_source_path.is_file(), msg=f"missing delegate slice: {delegate_source_path}")
        self.assertTrue(exception_source_path.is_file(), msg=f"missing exception slice: {exception_source_path}")
        self.assertTrue(closure_source_path.is_file(), msg=f"missing closure slice: {closure_source_path}")

        delegate_source = delegate_source_path.read_text(encoding="utf-8")
        exception_source = exception_source_path.read_text(encoding="utf-8")
        closure_source = closure_source_path.read_text(encoding="utf-8")

        self.assertIn("internal delegate string MessageFormatter(string prefix);", delegate_source)
        self.assertIn("internal delegate string TailFormatter(string value);", delegate_source)
        self.assertIn("internal sealed class DelegateRelay", delegate_source)
        self.assertIn("internal static class DelegateProofEntry", delegate_source)
        self.assertIn("MessageFormatter formatter = banner.BuildMessage;", delegate_source)
        self.assertIn("TailFormatter tail = DelegateStaticTail.AppendBang;", delegate_source)

        self.assertIn("internal sealed class ExceptionThrower", exception_source)
        self.assertIn("internal static class ExceptionProofEntry", exception_source)
        self.assertIn("throw new InvalidOperationException(", exception_source)
        self.assertIn("catch (InvalidOperationException ex)", exception_source)
        self.assertIn("finally", exception_source)

        self.assertIn("[DllImport(\"kernel32.dll\", ExactSpelling = true)]", closure_source)
        self.assertIn("typeof(ReflectionClosureBox<string>)", closure_source)
        self.assertIn("GetField(", closure_source)
        self.assertIn("GetMethod(", closure_source)
        self.assertIn("GetGenericTypeDefinition()", closure_source)
        self.assertIn("GetTickCount64()", closure_source)
        self.assertIn("internal static class ReflectionInteropClosureEntry", closure_source)

    def test_contract_loader_semantic_and_linker_lock_batch_b_surface(self) -> None:
        contracts_source = CONTRACTS_PATH.read_text(encoding="utf-8")
        loader_source = LOADER_STAGE_PATH.read_text(encoding="utf-8")
        semantic_world_source = SEMANTIC_WORLD_PATH.read_text(encoding="utf-8")
        linker_source = LINKER_STAGE_PATH.read_text(encoding="utf-8")

        self.assertIn("public required IReadOnlyList<ManagedExceptionRegionModel> ExceptionRegions { get; init; }", contracts_source)
        self.assertIn("public sealed record ManagedExceptionRegionModel", contracts_source)
        self.assertIn("public string? CatchTypeSubjectId { get; init; }", contracts_source)

        self.assertIn("ILOpCode.Ldftn", loader_source)
        self.assertIn("ILOpCode.Throw", loader_source)
        self.assertIn("ILOpCode.Leave", loader_source)
        self.assertIn("ILOpCode.Brtrue", loader_source)
        self.assertIn("ILOpCode.Brtrue_s", loader_source)
        self.assertIn("ILOpCode.Brfalse", loader_source)
        self.assertIn("ILOpCode.Brfalse_s", loader_source)
        self.assertIn("ILOpCode.Endfinally", loader_source)
        self.assertIn("ILOpCode.Ldsfld", loader_source)
        self.assertIn("ILOpCode.Stsfld", loader_source)
        self.assertIn('Op = "ldftn"', loader_source)
        self.assertIn('Op = "throw"', loader_source)
        self.assertIn('Op = "leave"', loader_source)
        self.assertIn('Op = "brtrue"', loader_source)
        self.assertIn('Op = "brfalse"', loader_source)
        self.assertIn('Op = "endfinally"', loader_source)
        self.assertIn('Op = "ldsfld"', loader_source)
        self.assertIn('Op = "stsfld"', loader_source)
        self.assertIn("ExceptionRegions = bodyBlock.ExceptionRegions", loader_source)
        self.assertIn("NamespaceName = definitionType.NamespaceName", loader_source)

        self.assertIn("requires-delegate-construction", semantic_world_source)
        self.assertIn("requires-delegate-invoke", semantic_world_source)
        self.assertIn("requires-exception-throw", semantic_world_source)
        self.assertIn("requires-exception-handler", semantic_world_source)
        self.assertIn("requires-finally", semantic_world_source)

        self.assertIn("BuildExceptionFacts(", linker_source)
        self.assertIn("delegate.exact-target-direct", linker_source)
        self.assertIn("delegate.runtime-helper-fallback", linker_source)
        self.assertIn("throw-catch-finally", linker_source)
        self.assertIn("ExceptionFacts = BuildExceptionFacts(", linker_source)
        self.assertIn("IncludeGenericTypeDefinitions(", linker_source)
        self.assertIn("IncludeReflectionQuerySurface(", linker_source)
        self.assertIn('string.Equals(instruction.Op, "ldtoken"', linker_source)
        self.assertIn("if (reachableTypeIds.Add(type.DefinitionSubjectId))", linker_source)
        self.assertIn(
            "System.Private.CoreLib/System.InvalidOperationException::.ctor(System.String)",
            linker_source,
        )

    def test_codegen_and_native_side_freeze_batch_b_lowering_and_helper_surface(self) -> None:
        planner_source = LOWERING_PLANNER_PATH.read_text(encoding="utf-8")
        emitter_source = EMITTER_PATH.read_text(encoding="utf-8")
        bootstrap_source = BOOTSTRAP_PATH.read_text(encoding="utf-8")
        runtime_core_source = RUNTIME_CORE_PATH.read_text(encoding="utf-8")
        runtime_core_header_source = RUNTIME_CORE_HEADER_PATH.read_text(encoding="utf-8")

        self.assertTrue(DELEGATE_TEMPLATE_PATH.is_file(), msg=f"missing delegate template: {DELEGATE_TEMPLATE_PATH}")
        self.assertTrue(EXCEPTION_TEMPLATE_PATH.is_file(), msg=f"missing exception template: {EXCEPTION_TEMPLATE_PATH}")
        self.assertTrue(
            REFLECTION_INTEROP_TEMPLATE_PATH.is_file(),
            msg=f"missing reflection/interop closure template: {REFLECTION_INTEROP_TEMPLATE_PATH}",
        )

        for lowering_family in [
            "managed-delegates.closed-target-relay-message.minimal",
            "managed-exceptions.throw-catch-finally-message.minimal",
            "managed-reflection-interop.closure.minimal",
        ]:
            self.assertIn(lowering_family, planner_source)
            self.assertIn(lowering_family, emitter_source)

        self.assertIn("TryCreateDelegateClosedTargetRelayLoweringPlan(", planner_source)
        self.assertIn("TryCreateExceptionThrowCatchFinallyLoweringPlan(", planner_source)
        self.assertIn("TryCreateReflectionInteropClosureLoweringPlan(", planner_source)

        self.assertIn("case DelegateClosedTargetRelayMinimal:", emitter_source)
        self.assertIn("case ExceptionThrowCatchFinallyMinimal:", emitter_source)
        self.assertIn("case ReflectionInteropClosureMinimal:", emitter_source)

        self.assertIn("struct DelegateInstance", bootstrap_source)
        self.assertIn("CreateDelegate(", bootstrap_source)
        self.assertIn("DelegateInvoke(", bootstrap_source)
        self.assertIn("CHAOS_BRIDGE_STATUS_MANAGED_EXCEPTION", bootstrap_source)
        self.assertIn("catch (const chaos::il2cpp::runtime_core::ManagedExceptionCarrier& carrier)", bootstrap_source)

        self.assertIn("struct ManagedExceptionCarrier", runtime_core_source)
        self.assertIn("throw ManagedExceptionCarrier", runtime_core_source)
        self.assertIn("MethodInvoke(", runtime_core_source)
        self.assertIn("catch (const ManagedExceptionCarrier& carrier)", runtime_core_source)
        self.assertIn("struct ManagedExceptionCarrier", runtime_core_header_source)


if __name__ == "__main__":
    unittest.main()
