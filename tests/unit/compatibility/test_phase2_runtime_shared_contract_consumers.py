from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
CONTRACTS_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedClosureContracts.cs"
INTERPRETER_IR_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Interpreter" / "InterpreterIR.cs"
IL_TO_IR_LOWERING_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Interpreter" / "ILToIRLowering.cs"
HOT_UPDATE_METHOD_REGISTRY_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.HotUpdate" / "HotUpdateMethodRegistry.cs"
HOT_UPDATE_RUNTIME_MANAGER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.HotUpdate" / "RuntimeManager.cs"
HOT_UPDATE_BINDINGS_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.HotUpdate" / "HotUpdateMethodBindings.cs"
BRIDGE_DISPATCHER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.HotUpdate" / "BridgeDispatcher.cs"
BRIDGE_GENERATOR_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.HotUpdate" / "BridgeGenerator.cs"
AUTO_BRIDGE_GENERATOR_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.HotUpdate" / "AutoBridgeGenerator.cs"
MIXED_EXECUTION_PROOF_ENTRY_PATH = (
    REPO_ROOT
    / "subjects"
    / "MixedExecutionFeaturePack"
    / "source"
    / "ManagedBridge"
    / "Proofs"
    / "MixedExecutionProofEntry.cs"
)
MIXED_DELEGATE_FLOW_PROOF_ENTRY_PATH = (
    REPO_ROOT
    / "subjects"
    / "MixedExecutionFeaturePack"
    / "source"
    / "ManagedBridge"
    / "Proofs"
    / "MixedDelegateFlowProofEntry.cs"
)
HOT_UPDATE_SKELETON_PROOF_PATH = (
    REPO_ROOT
    / "subjects"
    / "HotUpdateHostPack"
    / "source"
    / "Host"
    / "Proofs"
    / "HotUpdateSkeletonProofEntry.cs"
)
METADATA_SUPPLEMENT_PROOF_PATH = (
    REPO_ROOT
    / "subjects"
    / "HotUpdateHostPack"
    / "source"
    / "Host"
    / "Proofs"
    / "MetadataSupplementProofEntry.cs"
)
VERSION_ROLLBACK_PROOF_PATH = (
    REPO_ROOT
    / "subjects"
    / "HotUpdateHostPack"
    / "source"
    / "Host"
    / "Proofs"
    / "VersionRollbackProofEntry.cs"
)
HOT_UPDATE_DISPATCH_BENCHMARK_PATH = (
    REPO_ROOT
    / "subjects"
    / "HotUpdateHostPack"
    / "source"
    / "Host"
    / "Benchmarks"
    / "HotUpdateDispatchBenchmark.cs"
)
HOT_UPDATE_LOAD_BENCHMARK_PATH = (
    REPO_ROOT
    / "subjects"
    / "HotUpdateHostPack"
    / "source"
    / "Host"
    / "Benchmarks"
    / "HotUpdateLoadBenchmark.cs"
)
HOT_UPDATE_ROUNDTRIP_BENCHMARK_PATH = (
    REPO_ROOT
    / "subjects"
    / "HotUpdateHostPack"
    / "source"
    / "Host"
    / "Benchmarks"
    / "HotUpdateRoundtripBenchmark.cs"
)
AUTO_BRIDGE_PROOF_PATH = REPO_ROOT / "tests" / "fixtures" / "subjects" / "AutoBridgeProof" / "source" / "Program.cs"
VERSION_ROLLBACK_FIXTURE_PATH = (
    REPO_ROOT
    / "tests"
    / "fixtures"
    / "subjects"
    / "VersionRollbackProof"
    / "source"
    / "Program.cs"
)
BRIDGE_ROUNDTRIP_FIXTURE_PATH = (
    REPO_ROOT
    / "tests"
    / "fixtures"
    / "subjects"
    / "BridgeRoundtripProof"
    / "source"
    / "Program.cs"
)


class Phase2RuntimeSharedContractConsumerTests(unittest.TestCase):
    def test_contracts_expose_shared_method_identity_helpers_for_runtime_consumers(self) -> None:
        contracts_source = CONTRACTS_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "class ManagedMethodIdentityResolver",
            "Create(ManagedMethodModel method)",
            "Create(string subjectId, string signature",
            "ResolveSubjectId(",
            "ResolveParameterCount(",
        ]:
            self.assertIn(required_fragment, contracts_source)

    def test_interpreter_ir_and_lowering_emit_shared_identity_body_availability_and_dispatch_metadata(self) -> None:
        interpreter_ir_source = INTERPRETER_IR_PATH.read_text(encoding="utf-8")
        lowering_source = IL_TO_IR_LOWERING_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "ManagedMethodIdentityArtifact? Identity",
            "BodyAvailabilityCode? BodyAvailabilityCode",
            "HybridDispatchKind? DispatchKindCode",
        ]:
            self.assertIn(required_fragment, interpreter_ir_source)

        for required_fragment in [
            "Identity = new ManagedMethodIdentityArtifact",
            "BodyAvailabilityCode = BodyAvailabilityResolver.Resolve(method)",
            "HybridDispatchResolver.ResolveInstruction(",
            "ManagedMethodIdentityResolver.ResolveParameterCount(",
        ]:
            self.assertIn(required_fragment, lowering_source)

    def test_hot_update_runtime_and_bridge_accept_shared_method_identity_contracts(self) -> None:
        registry_source = HOT_UPDATE_METHOD_REGISTRY_PATH.read_text(encoding="utf-8")
        runtime_source = HOT_UPDATE_RUNTIME_MANAGER_PATH.read_text(encoding="utf-8")
        bridge_source = BRIDGE_DISPATCHER_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "RegisterConstantInt32(ManagedMethodIdentityArtifact identity",
            "RegisterInt32Unary(ManagedMethodIdentityArtifact identity",
            "RegisterMethod(ManagedMethodIdentityArtifact identity",
            "TryDispatch(ManagedMethodIdentityArtifact identity",
        ]:
            self.assertIn(required_fragment, registry_source)

        for required_fragment in [
            "DispatchInt32(ManagedMethodIdentityArtifact identity",
            "RegisterInt32Unary(ManagedMethodIdentityArtifact identity",
            "RegisterMethod(ManagedMethodIdentityArtifact identity",
            "DispatchInt32Unary(ManagedMethodIdentityArtifact identity",
        ]:
            self.assertIn(required_fragment, runtime_source)

        for required_fragment in [
            "RegisterManagedTarget(ManagedMethodIdentityArtifact identity",
            "RegisterHotUpdateInt32UnaryTarget(ManagedMethodIdentityArtifact identity",
            "RegisterAotInt32BinaryTarget(ManagedMethodIdentityArtifact identity",
            "RegisterEngineInt32UnaryTarget(ManagedMethodIdentityArtifact identity",
        ]:
            self.assertIn(required_fragment, bridge_source)

        for removed_fragment in [
            "public void RegisterConstantInt32(string subjectId",
            "public void RegisterInt32Unary(string subjectId",
            "public void RegisterMethod(string subjectId",
            "public bool TryGet(string subjectId",
            "public bool TryGetInt32Unary(string subjectId",
            "public bool TryDispatchInt32Unary(string subjectId",
            "public bool TryDispatch(string subjectId",
            "public bool LoadPackage(\n        string packageRootPath,\n        string currentAotVersion,\n        IReadOnlyDictionary<string, int>? subjectIdToConstantInt32 = null,",
            "public void LoadPackage(\n        LoadedHotUpdatePackage package,\n        string currentAotVersion,\n        IReadOnlyDictionary<string, int> subjectIdToConstantInt32,",
            "public int DispatchInt32(string subjectId",
            "public int DispatchInt32Unary(string subjectId",
            "public void RegisterInt32Unary(string subjectId",
            "public void RegisterMethod(string subjectId",
            "public void RegisterManagedTarget(string subjectId",
            "public void RegisterHotUpdateInt32UnaryTarget(string subjectId",
            "public void RegisterAotInt32BinaryTarget(string subjectId",
            "public void RegisterEngineInt32UnaryTarget(string subjectId",
        ]:
            self.assertNotIn(removed_fragment, registry_source + runtime_source + bridge_source)

    def test_bridge_plan_and_auto_bridge_generator_carry_typed_target_identity_contracts(self) -> None:
        bridge_generator_source = BRIDGE_GENERATOR_PATH.read_text(encoding="utf-8")
        auto_bridge_source = AUTO_BRIDGE_GENERATOR_PATH.read_text(encoding="utf-8")
        bridge_dispatcher_source = BRIDGE_DISPATCHER_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "ManagedMethodIdentityArtifact? HotUpdateIdentity",
            "ManagedMethodIdentityArtifact? AotIdentity",
            "ManagedMethodIdentityArtifact? EngineIdentity",
            "ManagedMethodIdentityArtifact? TargetIdentity",
            "ResolveSubjectId(",
        ]:
            self.assertIn(required_fragment, bridge_generator_source)

        for required_fragment in [
            "TargetIdentity = ManagedMethodIdentityResolver.Create(method)",
            "TargetSubjectId = method.SubjectId",
        ]:
            self.assertIn(required_fragment, auto_bridge_source)

        for required_fragment in [
            "bridge.TargetIdentity",
            "bridge.HotUpdateIdentity",
            "bridge.AotIdentity",
            "bridge.EngineIdentity",
            "ResolveTargetSubjectId(",
        ]:
            self.assertIn(required_fragment, bridge_dispatcher_source)

    def test_runtime_manager_exposes_typed_binding_set_for_hot_update_load(self) -> None:
        bindings_source = HOT_UPDATE_BINDINGS_PATH.read_text(encoding="utf-8")
        runtime_source = HOT_UPDATE_RUNTIME_MANAGER_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "record HotUpdateConstantInt32Binding",
            "record HotUpdateInt32UnaryBinding",
            "record HotUpdateGenericMethodBinding",
            "record HotUpdateMethodBindingSet",
            "ManagedMethodIdentityArtifact Identity",
            "ConstantInt32Bindings",
            "Int32UnaryBindings",
            "GenericBindings",
        ]:
            self.assertIn(required_fragment, bindings_source)

        for required_fragment in [
            "LoadPackage(",
            "HotUpdateMethodBindingSet bindings",
            "bindings.ConstantInt32Bindings",
            "bindings.Int32UnaryBindings",
            "bindings.GenericBindings",
        ]:
            self.assertIn(required_fragment, runtime_source)

    def test_real_proof_and_bridge_fixture_adopt_identity_based_runtime_consumers(self) -> None:
        mixed_execution_source = MIXED_EXECUTION_PROOF_ENTRY_PATH.read_text(encoding="utf-8")
        auto_bridge_source = AUTO_BRIDGE_PROOF_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "DispatchInt32Unary(addTwentyOneIdentity",
            "RegisterAotInt32BinaryTarget(addMethodIdentity",
        ]:
            self.assertIn(required_fragment, mixed_execution_source)

        self.assertIn(
            "new Dictionary<string, Func<IReadOnlyList<object?>, object?>>(StringComparer.Ordinal)",
            mixed_execution_source,
        )
        self.assertNotIn("if (string.Equals(bridgeId", mixed_execution_source)

        for required_fragment in [
            "RegisterManagedTarget(ManagedMethodIdentityResolver.Create(hotUpdateMethods[0])",
            "RegisterManagedTarget(ManagedMethodIdentityResolver.Create(hotUpdateMethods[1])",
            "RegisterManagedTarget(ManagedMethodIdentityResolver.Create(hotUpdateMethods[2])",
            "RegisterManagedTarget(ManagedMethodIdentityResolver.Create(hotUpdateMethods[3])",
            "RegisterManagedTarget(ManagedMethodIdentityResolver.Create(hotUpdateMethods[4])",
        ]:
            self.assertIn(required_fragment, auto_bridge_source)

    def test_retained_subjects_and_fixtures_use_identity_helpers_and_typed_load_bindings(self) -> None:
        source_paths = [
            MIXED_DELEGATE_FLOW_PROOF_ENTRY_PATH,
            HOT_UPDATE_SKELETON_PROOF_PATH,
            METADATA_SUPPLEMENT_PROOF_PATH,
            VERSION_ROLLBACK_PROOF_PATH,
            HOT_UPDATE_DISPATCH_BENCHMARK_PATH,
            HOT_UPDATE_LOAD_BENCHMARK_PATH,
            HOT_UPDATE_ROUNDTRIP_BENCHMARK_PATH,
            VERSION_ROLLBACK_FIXTURE_PATH,
            BRIDGE_ROUNDTRIP_FIXTURE_PATH,
        ]

        for source_path in source_paths:
            source = source_path.read_text(encoding="utf-8")
            self.assertIn("ManagedMethodIdentityResolver.Create(", source, msg=f"missing identity helper adoption in {source_path}")

        for source_path in [
            HOT_UPDATE_SKELETON_PROOF_PATH,
            METADATA_SUPPLEMENT_PROOF_PATH,
            VERSION_ROLLBACK_PROOF_PATH,
            HOT_UPDATE_LOAD_BENCHMARK_PATH,
            VERSION_ROLLBACK_FIXTURE_PATH,
        ]:
            source = source_path.read_text(encoding="utf-8")
            self.assertIn("new HotUpdateMethodBindingSet", source, msg=f"missing typed load bindings in {source_path}")

        for source_path in [
            MIXED_DELEGATE_FLOW_PROOF_ENTRY_PATH,
            HOT_UPDATE_ROUNDTRIP_BENCHMARK_PATH,
            BRIDGE_ROUNDTRIP_FIXTURE_PATH,
            MIXED_EXECUTION_PROOF_ENTRY_PATH,
        ]:
            source = source_path.read_text(encoding="utf-8")
            self.assertTrue(
                "HotUpdateIdentity =" in source
                or "AotIdentity =" in source
                or "EngineIdentity =" in source
                or "TargetIdentity =" in source,
                msg=f"missing typed bridge plan target adoption in {source_path}",
            )


if __name__ == "__main__":
    unittest.main()
