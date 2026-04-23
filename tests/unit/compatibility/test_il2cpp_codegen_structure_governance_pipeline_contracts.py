from tests.unit.compatibility.il2cpp_codegen_structure_governance_test_support import *


class TestIl2CppCodeGenStructureGovernancePipelineContracts(Il2CppCodeGenStructureGovernanceTestSupport):
    def test_loader_stage_is_split_into_orchestrator_and_helpers(self) -> None:
        loader_source = LOADER_STAGE_PATH.read_text(encoding="utf-8")

        for required_path in [
            LOADER_STAGE_CROSS_ASSEMBLY_PATH,
            LOADER_STAGE_ASSEMBLY_LOADING_PATH,
            LOADER_STAGE_INSTRUCTION_DECODING_PATH,
            LOADER_STAGE_METADATA_RESOLUTION_PATH,
            LOADER_STAGE_GENERIC_INSTANTIATION_PROJECTION_PATH,
        ]:
            self.assertTrue(required_path.is_file(), msg=f"missing loader split file: {required_path}")

        self.assertIn("public sealed partial class LoaderStage", loader_source)

        for required_fragment in [
            "private static List<LoadedAssemblyModel> ProjectCrossAssemblyMethodInstantiations(",
            "private static LoadedAssemblyModel LoadAssembly(",
            "private static ManagedMethodBodyModel DecodeMethodBody(",
            "private static MethodReferenceSummary ResolveMethodReference(",
            "private static GenericInstantiationProjection BuildGenericInstantiationProjection(",
        ]:
            self.assertNotIn(required_fragment, loader_source)

    def test_linker_stage_is_split_into_orchestrator_and_helpers(self) -> None:
        linker_source = LINKER_STAGE_PATH.read_text(encoding="utf-8")

        for required_path in [
            LINKER_STAGE_REACHABILITY_PATH,
            LINKER_STAGE_OPTIMIZATION_FACTS_PATH,
            LINKER_STAGE_DISPATCH_RESOLUTION_PATH,
            LINKER_STAGE_OUTPUT_PROJECTION_PATH,
        ]:
            self.assertTrue(required_path.is_file(), msg=f"missing linker split file: {required_path}")

        self.assertIn("public sealed partial class LinkerStage", linker_source)

        for required_fragment in [
            "private static ReachableClosure ComputeReachableClosure(",
            "private static OptimizationFactsArtifact BuildOptimizationFacts(",
            "private static string? TryResolveDevirtualizedCallTarget(",
            "private static IReadOnlyList<ManagedTypeModel> OrderTypes(",
        ]:
            self.assertNotIn(required_fragment, linker_source)

    def test_managed_closure_contracts_are_split_by_category(self) -> None:
        contracts_source = MANAGED_CLOSURE_CONTRACTS_PATH.read_text(encoding="utf-8")

        for required_path in [
            MANAGED_NAMING_PATH,
            MANAGED_CLOSURE_MODELS_PATH,
            MANAGED_METHOD_IDENTITY_CONTRACTS_PATH,
            MANAGED_SEMANTIC_WORLD_CONTRACTS_PATH,
            TYPED_IL_AND_AOT_CORE_IR_CONTRACTS_PATH,
            MANAGED_CLOSURE_ARTIFACT_MODELS_PATH,
        ]:
            self.assertTrue(required_path.is_file(), msg=f"missing managed contracts split file: {required_path}")

        for required_fragment in [
            "public static class ManagedNaming",
            "public sealed record ManagedTypeModel",
            "public static class ManagedMethodIdentityResolver",
            "public sealed record SemanticWorldModel",
            "public sealed record TypedIlIrArtifact",
            "public sealed record NativeAotLoweringPlanArtifact",
        ]:
            self.assertNotIn(required_fragment, contracts_source)
