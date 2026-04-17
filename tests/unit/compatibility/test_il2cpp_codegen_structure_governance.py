from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]

NATIVE_REFERENCE_PLANNER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "NativeReferenceLoweringPlanner.cs"
NATIVE_REFERENCE_EMITTER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "NativeReferenceProofEmitter.cs"
NATIVE_AOT_EMITTER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "NativeAotEmitter.cs"
NATIVE_AOT_PLANNER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "NativeAotLoweringPlanner.cs"

NATIVE_REFERENCE_CATALOG_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "NativeReferenceProofCatalog.cs"
)
NATIVE_REFERENCE_ENGINE_AND_FAMILY_SELECTION_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "NativeReferenceLoweringPlanner.EngineAndFamilySelection.cs"
)
NATIVE_REFERENCE_PLAN_BUILDERS_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "NativeReferenceLoweringPlanner.PlanBuilders.cs"
)
NATIVE_REFERENCE_SHAPE_VALIDATION_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "ReferenceProof"
    / "NativeReferenceLoweringPlanner.ShapeValidation.cs"
)
SCRIBAN_RENDERER_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templating"
    / "ScribanTemplateRenderer.cs"
)
NATIVE_AOT_RUNTIME_PRELUDE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "RuntimeSupport"
    / "NativeAotLoweringPlanner.RuntimeSupport.cs"
)
NATIVE_AOT_RUNTIME_PRELUDE_AGGREGATOR_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "RuntimeSupport"
    / "NativeAotLoweringPlanner.RuntimePrelude.cs"
)
NATIVE_AOT_METADATA_SUPPORT_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Planning"
    / "NativeAotLoweringPlanner.MetadataSupport.cs"
)
NATIVE_AOT_INVOCATION_PLANNING_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Planning"
    / "NativeAotLoweringPlanner.InvocationPlanning.cs"
)
NATIVE_AOT_METHOD_EMISSION_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Emission"
    / "NativeAotLoweringPlanner.MethodEmission.cs"
)
NATIVE_AOT_EXCEPTION_EMISSION_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Emission"
    / "NativeAotLoweringPlanner.ExceptionEmission.cs"
)
NATIVE_AOT_EXTERNAL_RUNTIME_HELPERS_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "RuntimeSupport"
    / "NativeAotLoweringPlanner.ExternalRuntimeHelpers.cs"
)
NATIVE_AOT_OBJECT_MODEL_EMISSION_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Emission"
    / "NativeAotLoweringPlanner.ObjectModelEmission.cs"
)
NATIVE_AOT_OBJECT_EQUALITY_EMISSION_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Emission"
    / "NativeAotLoweringPlanner.ObjectEqualityEmission.cs"
)
NATIVE_AOT_REFLECTION_OBJECT_EMISSION_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Emission"
    / "NativeAotLoweringPlanner.ReflectionObjectEmission.cs"
)
NATIVE_AOT_OBJECT_MODEL_UTILITIES_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Emission"
    / "NativeAotLoweringPlanner.ObjectModelUtilities.cs"
)
NATIVE_AOT_EXTERNAL_RUNTIME_STRING_AND_PLATFORM_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "RuntimeSupport"
    / "NativeAotLoweringPlanner.ExternalRuntimeHelpers.StringAndPlatform.cs"
)
NATIVE_AOT_EXTERNAL_RUNTIME_COLLECTION_AND_REFLECTION_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "RuntimeSupport"
    / "NativeAotLoweringPlanner.ExternalRuntimeHelpers.CollectionAndReflection.cs"
)
NATIVE_AOT_EXTERNAL_RUNTIME_TYPE_RESOLUTION_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "RuntimeSupport"
    / "NativeAotLoweringPlanner.ExternalRuntimeHelpers.TypeResolution.cs"
)
NATIVE_AOT_EXTERNAL_RUNTIME_INVOCATION_ABI_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "RuntimeSupport"
    / "NativeAotLoweringPlanner.ExternalRuntimeHelpers.InvocationAbi.cs"
)
LOADER_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "LoaderStage.cs"
LOADER_STAGE_CROSS_ASSEMBLY_PATH = (
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "LoaderStage.CrossAssemblyInstantiation.cs"
)
LOADER_STAGE_ASSEMBLY_LOADING_PATH = (
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "LoaderStage.AssemblyLoading.cs"
)
LOADER_STAGE_INSTRUCTION_DECODING_PATH = (
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "LoaderStage.InstructionDecoding.cs"
)
LOADER_STAGE_METADATA_RESOLUTION_PATH = (
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "LoaderStage.MetadataResolution.cs"
)
LOADER_STAGE_GENERIC_MATERIALIZATION_PATH = (
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "LoaderStage.GenericMaterialization.cs"
)
LINKER_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Linker" / "LinkerStage.cs"
LINKER_STAGE_REACHABILITY_PATH = (
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Linker" / "LinkerStage.Reachability.cs"
)
LINKER_STAGE_OPTIMIZATION_FACTS_PATH = (
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Linker" / "LinkerStage.OptimizationFacts.cs"
)
LINKER_STAGE_DISPATCH_RESOLUTION_PATH = (
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Linker" / "LinkerStage.DispatchResolution.cs"
)
LINKER_STAGE_OUTPUT_PROJECTION_PATH = (
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Linker" / "LinkerStage.OutputProjection.cs"
)
MANAGED_CLOSURE_CONTRACTS_PATH = (
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedClosureContracts.cs"
)
MANAGED_NAMING_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedNaming.cs"
MANAGED_CLOSURE_MODELS_PATH = (
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedClosureModels.cs"
)
MANAGED_METHOD_IDENTITY_CONTRACTS_PATH = (
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedMethodIdentityContracts.cs"
)
MANAGED_SEMANTIC_WORLD_CONTRACTS_PATH = (
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedSemanticWorldContracts.cs"
)
TYPED_IL_AND_AOT_CORE_IR_CONTRACTS_PATH = (
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "TypedIlAndAotCoreIrContracts.cs"
)
MANAGED_CLOSURE_ARTIFACT_MODELS_PATH = (
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedClosureArtifactModels.cs"
)


class Il2CppCodeGenStructureGovernanceTests(unittest.TestCase):
    def test_native_reference_planner_and_emitter_share_catalog(self) -> None:
        planner_source = NATIVE_REFERENCE_PLANNER_PATH.read_text(encoding="utf-8")
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            NATIVE_REFERENCE_CATALOG_PATH.is_file(),
            msg=f"missing NativeReference proof catalog: {NATIVE_REFERENCE_CATALOG_PATH}",
        )

        for source in [planner_source, emitter_source]:
            self.assertIn("NativeReferenceProofCatalog.", source)
            self.assertNotIn(
                'private const string ManagedAsyncAwaitIntMinimal = "managed-async.awaitable-int.minimal";',
                source,
            )
            self.assertNotIn(
                'private const string GeneratedTranslationUnitTemplateRelativePath = "Templates/NativeReferenceProof.cpp.scriban";',
                source,
            )

    def test_codegen_emitters_use_shared_scriban_renderer(self) -> None:
        planner_source = NATIVE_REFERENCE_PLANNER_PATH.read_text(encoding="utf-8")
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        native_aot_emitter_source = NATIVE_AOT_EMITTER_PATH.read_text(encoding="utf-8")
        catalog_source = NATIVE_REFERENCE_CATALOG_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            SCRIBAN_RENDERER_PATH.is_file(),
            msg=f"missing shared Scriban renderer: {SCRIBAN_RENDERER_PATH}",
        )

        self.assertIn("NativeReferenceProofCatalog.", planner_source)
        self.assertIn("ScribanTemplateRenderer", catalog_source)

        for source in [emitter_source, native_aot_emitter_source]:
            self.assertIn("ScribanTemplateRenderer", source)
            self.assertNotIn("private static Template LoadTemplate(", source)

        self.assertNotIn("private static Template LoadTemplate(", planner_source)
        self.assertNotIn("private static string RenderTemplate(", emitter_source)
        self.assertNotIn("private static string RenderTemplate(", native_aot_emitter_source)

    def test_native_reference_planner_helpers_are_split_from_root_planner(self) -> None:
        planner_source = NATIVE_REFERENCE_PLANNER_PATH.read_text(encoding="utf-8")

        for required_path in [
            NATIVE_REFERENCE_ENGINE_AND_FAMILY_SELECTION_PATH,
            NATIVE_REFERENCE_PLAN_BUILDERS_PATH,
            NATIVE_REFERENCE_SHAPE_VALIDATION_PATH,
        ]:
            self.assertTrue(required_path.is_file(), msg=f"missing NativeReference split file: {required_path}")

        self.assertIn("public sealed partial class NativeReferenceLoweringPlanner", planner_source)
        self.assertIn("public NativeReferenceLoweringPlanArtifact Create(", planner_source)

        for required_fragment in [
            "private static bool TryCreateEngineLoweringPlan(",
            "private static string SelectLoweringFamily(",
            "private static NativeReferenceLoweringPlanArtifact CreateLoweringPlan(",
            "private static Template GetTemplateForPlan(",
            "private static IReadOnlyList<TypedIlInstructionArtifact> GetSingleBlockInstructions(",
            "private static string ValidateConstructorThenInstanceCallEntryPointShape(",
        ]:
            self.assertNotIn(required_fragment, planner_source)

        engine_and_family_selection_source = NATIVE_REFERENCE_ENGINE_AND_FAMILY_SELECTION_PATH.read_text(encoding="utf-8")
        for required_fragment in [
            "private static bool TryCreateEngineLoweringPlan(",
            "private static string SelectLoweringFamily(",
            "private static bool MatchesAsyncAwaitIntCandidate(",
            "private static bool MatchesInterfaceDispatchMessageCandidate(",
        ]:
            self.assertIn(required_fragment, engine_and_family_selection_source)

        plan_builders_source = NATIVE_REFERENCE_PLAN_BUILDERS_PATH.read_text(encoding="utf-8")
        for required_fragment in [
            "private static NativeReferenceLoweringPlanArtifact CreateLoweringPlan(",
            "private static bool TryCreateThreadingThreadStaticMonitorLoweringPlan(",
            "private static bool TryCreatePInvokeDllImportMinimalLoweringPlan(",
            "private static Template GetTemplateForPlan(",
            "private static ScriptObject CreateTemplateModel(",
        ]:
            self.assertIn(required_fragment, plan_builders_source)

        shape_validation_source = NATIVE_REFERENCE_SHAPE_VALIDATION_PATH.read_text(encoding="utf-8")
        for required_fragment in [
            "private static IReadOnlyList<TypedIlInstructionArtifact> GetSingleBlockInstructions(",
            "private static bool IsConstructorThenInstanceCallEntryPointShape(",
            "private static string ValidateConstructorThenInstanceCallEntryPointShape(",
            "private static MetadataRegistrationEntry GetRequiredMetadataRegistration(",
            "private static T LoadRequiredJson<T>(",
        ]:
            self.assertIn(required_fragment, shape_validation_source)

    def test_native_aot_runtime_support_helpers_are_split_from_root_planner(self) -> None:
        planner_source = NATIVE_AOT_PLANNER_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            NATIVE_AOT_RUNTIME_PRELUDE_PATH.is_file(),
            msg=f"missing runtime support split file: {NATIVE_AOT_RUNTIME_PRELUDE_PATH}",
        )
        self.assertTrue(
            NATIVE_AOT_RUNTIME_PRELUDE_AGGREGATOR_PATH.is_file(),
            msg=f"missing runtime prelude aggregator split file: {NATIVE_AOT_RUNTIME_PRELUDE_AGGREGATOR_PATH}",
        )
        self.assertIn("public sealed partial class NativeAotLoweringPlanner", planner_source)
        self.assertNotIn("private static void EmitCollectionRuntimePrelude(", planner_source)
        self.assertNotIn("private static void EmitMonitorRuntimePrelude(", planner_source)
        self.assertNotIn("private static void EmitThreadRuntimePrelude(", planner_source)
        self.assertNotIn("private static void EmitAsyncRuntimePrelude(", planner_source)
        self.assertNotIn("private static void EmitRuntimePrelude(", planner_source)
        self.assertNotIn("private static void EmitSpanRuntimePrelude(", planner_source)

        runtime_prelude_source = NATIVE_AOT_RUNTIME_PRELUDE_PATH.read_text(encoding="utf-8")
        self.assertIn("sealed partial class NativeAotLoweringPlanner", runtime_prelude_source)

        for required_fragment in [
            "private static void EmitCollectionRuntimePrelude(",
            "private static void EmitMonitorRuntimePrelude(",
            "private static void EmitThreadRuntimePrelude(",
            "private static void EmitAsyncRuntimePrelude(",
        ]:
            self.assertIn(required_fragment, runtime_prelude_source)

        runtime_prelude_aggregator_source = NATIVE_AOT_RUNTIME_PRELUDE_AGGREGATOR_PATH.read_text(encoding="utf-8")
        self.assertIn("sealed partial class NativeAotLoweringPlanner", runtime_prelude_aggregator_source)
        self.assertIn("private static void EmitRuntimePrelude(", runtime_prelude_aggregator_source)
        self.assertIn("private static void EmitSpanRuntimePrelude(", runtime_prelude_aggregator_source)

    def test_native_aot_metadata_support_builders_are_split_from_root_planner(self) -> None:
        planner_source = NATIVE_AOT_PLANNER_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            NATIVE_AOT_METADATA_SUPPORT_PATH.is_file(),
            msg=f"missing metadata support split file: {NATIVE_AOT_METADATA_SUPPORT_PATH}",
        )

        for required_fragment in [
            "private CustomAttributeSupportModel BuildCustomAttributeSupportModel(",
            "private AssemblyReflectionSupportModel BuildAssemblyReflectionSupportModel(",
            "private ReflectionMemberSupportModel BuildReflectionMemberSupportModel(",
            "private StaticFieldDataSupportModel BuildStaticFieldDataSupportModel(",
        ]:
            self.assertNotIn(required_fragment, planner_source)

        metadata_support_source = NATIVE_AOT_METADATA_SUPPORT_PATH.read_text(encoding="utf-8")
        self.assertIn("sealed partial class NativeAotLoweringPlanner", metadata_support_source)

        for required_fragment in [
            "private CustomAttributeSupportModel BuildCustomAttributeSupportModel(",
            "private AssemblyReflectionSupportModel BuildAssemblyReflectionSupportModel(",
            "private ReflectionMemberSupportModel BuildReflectionMemberSupportModel(",
            "private StaticFieldDataSupportModel BuildStaticFieldDataSupportModel(",
        ]:
            self.assertIn(required_fragment, metadata_support_source)

    def test_native_aot_invocation_planning_helpers_are_split_from_root_planner(self) -> None:
        planner_source = NATIVE_AOT_PLANNER_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            NATIVE_AOT_INVOCATION_PLANNING_PATH.is_file(),
            msg=f"missing invocation planning split file: {NATIVE_AOT_INVOCATION_PLANNING_PATH}",
        )

        for required_fragment in [
            "private IReadOnlyList<AotCoreIrMethodArtifact> CollectReachableMethods(",
            "private IReadOnlyList<AotCoreIrMethodArtifact> ResolveReachableMethods(",
            "private IReadOnlyList<AotCoreIrMethodArtifact> ResolveDirectReachableMethods(",
            "private IReadOnlyList<AotCoreIrMethodArtifact> ResolveVirtualDispatchTargets(",
            "private InvocationTarget ResolveDirectInvocationTarget(",
            "private InvocationTarget? TryResolveDirectInvocationTarget(",
            "private AotCoreIrMethodArtifact? TryGetLowerableMethod(",
        ]:
            self.assertNotIn(required_fragment, planner_source)

        invocation_planning_source = NATIVE_AOT_INVOCATION_PLANNING_PATH.read_text(encoding="utf-8")
        self.assertIn("sealed partial class NativeAotLoweringPlanner", invocation_planning_source)

        for required_fragment in [
            "private IReadOnlyList<AotCoreIrMethodArtifact> CollectReachableMethods(",
            "private IReadOnlyList<AotCoreIrMethodArtifact> ResolveReachableMethods(",
            "private IReadOnlyList<AotCoreIrMethodArtifact> ResolveDirectReachableMethods(",
            "private IReadOnlyList<AotCoreIrMethodArtifact> ResolveVirtualDispatchTargets(",
            "private InvocationTarget ResolveDirectInvocationTarget(",
            "private InvocationTarget? TryResolveDirectInvocationTarget(",
            "private AotCoreIrMethodArtifact? TryGetLowerableMethod(",
        ]:
            self.assertIn(required_fragment, invocation_planning_source)

    def test_native_aot_method_emission_helpers_are_split_from_root_planner(self) -> None:
        planner_source = NATIVE_AOT_PLANNER_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            NATIVE_AOT_METHOD_EMISSION_PATH.is_file(),
            msg=f"missing method emission split file: {NATIVE_AOT_METHOD_EMISSION_PATH}",
        )

        for required_fragment in [
            "private static string FormatMethodDeclaration(",
            "private void EmitManagedMethod(",
            "private void EmitInstruction(",
            "private void EmitNewObject(",
            "private static void EmitNewArray(",
            "private static void EmitInitObj(",
            "private void EmitInstructionRange(",
        ]:
            self.assertNotIn(required_fragment, planner_source)

        method_emission_source = NATIVE_AOT_METHOD_EMISSION_PATH.read_text(encoding="utf-8")
        self.assertIn("sealed partial class NativeAotLoweringPlanner", method_emission_source)

        for required_fragment in [
            "private static string FormatMethodDeclaration(",
            "private void EmitManagedMethod(",
            "private void EmitInstruction(",
            "private void EmitNewObject(",
            "private static void EmitNewArray(",
            "private static void EmitInitObj(",
            "private void EmitInstructionRange(",
        ]:
            self.assertIn(required_fragment, method_emission_source)

    def test_native_aot_exception_emission_helpers_are_split_from_method_emission(self) -> None:
        method_emission_source = NATIVE_AOT_METHOD_EMISSION_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            NATIVE_AOT_EXCEPTION_EMISSION_PATH.is_file(),
            msg=f"missing exception emission split file: {NATIVE_AOT_EXCEPTION_EMISSION_PATH}",
        )

        for required_fragment in [
            "private void EmitCatchOnlyExceptionMethodBody(",
            "private void EmitFilterOnlyExceptionMethodBody(",
            "private void EmitLinearInstructionSequence(",
            "private static IReadOnlyDictionary<int, int?> CreateNextOffsets(",
            "private static bool TryCreateCatchOnlyExceptionMethodShape(",
            "private static bool TryCreateFinallyHandlerEmissionPlan(",
        ]:
            self.assertNotIn(required_fragment, method_emission_source)

        exception_emission_source = NATIVE_AOT_EXCEPTION_EMISSION_PATH.read_text(encoding="utf-8")
        self.assertIn("sealed partial class NativeAotLoweringPlanner", exception_emission_source)

        for required_fragment in [
            "private void EmitCatchOnlyExceptionMethodBody(",
            "private void EmitFilterOnlyExceptionMethodBody(",
            "private void EmitLinearInstructionSequence(",
            "private static IReadOnlyDictionary<int, int?> CreateNextOffsets(",
            "private static bool TryCreateCatchOnlyExceptionMethodShape(",
            "private static bool TryCreateFinallyHandlerEmissionPlan(",
        ]:
            self.assertIn(required_fragment, exception_emission_source)

    def test_native_aot_external_runtime_helpers_are_split_from_root_planner(self) -> None:
        planner_source = NATIVE_AOT_PLANNER_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            NATIVE_AOT_EXTERNAL_RUNTIME_HELPERS_PATH.is_file(),
            msg=f"missing external runtime helper split file: {NATIVE_AOT_EXTERNAL_RUNTIME_HELPERS_PATH}",
        )

        for required_fragment in [
            "private bool TryCreateExternalRuntimeHelperDefinition(",
            "private bool TryCreateStringRuntimeHelperDefinition(",
            "private bool TryCreateCollectionRuntimeHelperDefinition(",
            "private bool TryCreateAsyncRuntimeHelperDefinition(",
            "private string GetVirtualDispatchInstanceExpression(",
            "private static void EmitAbiArgumentInitialization(",
            "private static string FormatAbiInvocationArgumentList(",
        ]:
            self.assertNotIn(required_fragment, planner_source)

        external_runtime_helper_source = NATIVE_AOT_EXTERNAL_RUNTIME_HELPERS_PATH.read_text(encoding="utf-8")
        self.assertIn("sealed partial class NativeAotLoweringPlanner", external_runtime_helper_source)
        self.assertIn("private bool TryCreateExternalRuntimeHelperDefinition(", external_runtime_helper_source)
        for removed_fragment in [
            "private bool TryCreateStringRuntimeHelperDefinition(",
            "private bool TryCreateCollectionRuntimeHelperDefinition(",
            "private bool TryCreateAsyncRuntimeHelperDefinition(",
            "private string GetVirtualDispatchInstanceExpression(",
            "private static void EmitAbiArgumentInitialization(",
            "private static string FormatAbiInvocationArgumentList(",
        ]:
            self.assertNotIn(removed_fragment, external_runtime_helper_source)

    def test_native_aot_external_runtime_helpers_are_further_split_by_family(self) -> None:
        root_source = NATIVE_AOT_EXTERNAL_RUNTIME_HELPERS_PATH.read_text(encoding="utf-8")

        for required_path in [
            NATIVE_AOT_EXTERNAL_RUNTIME_STRING_AND_PLATFORM_PATH,
            NATIVE_AOT_EXTERNAL_RUNTIME_COLLECTION_AND_REFLECTION_PATH,
            NATIVE_AOT_EXTERNAL_RUNTIME_TYPE_RESOLUTION_PATH,
            NATIVE_AOT_EXTERNAL_RUNTIME_INVOCATION_ABI_PATH,
        ]:
            self.assertTrue(required_path.is_file(), msg=f"missing external runtime helper family split file: {required_path}")

        for required_fragment in [
            "private ExternalRuntimeHelperDefinition CreateStringJoinInt32EnumerableRuntimeHelperDefinition(",
            "private bool TryCreateCollectionRuntimeHelperDefinition(",
            "private static bool TryParseAsyncTaskBuilderStartStateMachineType(",
            "private string GetVirtualDispatchInstanceExpression(",
            "private static void EmitAbiArgumentInitialization(",
            "private static T LoadRequiredJson<T>(",
        ]:
            self.assertNotIn(required_fragment, root_source)

        string_and_platform_source = NATIVE_AOT_EXTERNAL_RUNTIME_STRING_AND_PLATFORM_PATH.read_text(encoding="utf-8")
        for required_fragment in [
            "private bool TryCreateStringRuntimeHelperDefinition(",
            "private ExternalRuntimeHelperDefinition CreateStringJoinInt32EnumerableRuntimeHelperDefinition(",
            "private bool TryCreateExceptionRuntimeHelperDefinition(",
            "private bool TryCreatePlatformRuntimeHelperDefinition(",
        ]:
            self.assertIn(required_fragment, string_and_platform_source)

        collection_and_reflection_source = NATIVE_AOT_EXTERNAL_RUNTIME_COLLECTION_AND_REFLECTION_PATH.read_text(encoding="utf-8")
        for required_fragment in [
            "private bool TryCreateSpanRuntimeHelperDefinition(",
            "private bool TryCreateCollectionRuntimeHelperDefinition(",
            "private bool TryCreateAsyncRuntimeHelperDefinition(",
            "private bool TryCreateReflectionRuntimeHelperDefinition(",
        ]:
            self.assertIn(required_fragment, collection_and_reflection_source)

        type_resolution_source = NATIVE_AOT_EXTERNAL_RUNTIME_TYPE_RESOLUTION_PATH.read_text(encoding="utf-8")
        for required_fragment in [
            "private static bool TryParseAsyncTaskBuilderStartStateMachineType(",
            "private static IReadOnlyList<string> SplitTopLevelGenericArguments(",
            "private bool TryResolveReferenceTypeSubjectId(",
        ]:
            self.assertIn(required_fragment, type_resolution_source)

        invocation_abi_source = NATIVE_AOT_EXTERNAL_RUNTIME_INVOCATION_ABI_PATH.read_text(encoding="utf-8")
        for required_fragment in [
            "private string GetVirtualDispatchTargetTypeIdSymbol(",
            "private string GetVirtualDispatchInstanceExpression(",
            "private static void EmitMethodReturn(",
            "private static void EmitAbiArgumentInitialization(",
            "private static string FormatAbiInvocationArgumentList(",
            "private static T LoadRequiredJson<T>(",
        ]:
            self.assertIn(required_fragment, invocation_abi_source)

    def test_native_aot_object_model_emission_helpers_are_split_from_root_planner(self) -> None:
        planner_source = NATIVE_AOT_PLANNER_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            NATIVE_AOT_OBJECT_MODEL_EMISSION_PATH.is_file(),
            msg=f"missing object model emission split file: {NATIVE_AOT_OBJECT_MODEL_EMISSION_PATH}",
        )

        for required_fragment in [
            "private void EmitObjectModelDeclarations(",
            "private void EmitObjectEqualityHelpers(",
            "private void EmitReflectionObjectHelpers(",
            "private static IReadOnlyList<string> GetReferenceTypeEmissionOrder(",
            "private IReadOnlyList<AotCoreIrAbiSlotArtifact> ResolveDelegateInvokeParameterAbis(",
            "private AotCoreIrAbiSlotArtifact ResolveDelegateInvokeReturnAbi(",
            "private static string GetRuntimeTypeIdExpression(",
        ]:
            self.assertNotIn(required_fragment, planner_source)

        object_model_emission_source = NATIVE_AOT_OBJECT_MODEL_EMISSION_PATH.read_text(encoding="utf-8")
        self.assertIn("sealed partial class NativeAotLoweringPlanner", object_model_emission_source)
        self.assertIn("private void EmitObjectModelDeclarations(", object_model_emission_source)
        for removed_fragment in [
            "private void EmitObjectEqualityHelpers(",
            "private void EmitReflectionObjectHelpers(",
            "private static IReadOnlyList<string> GetReferenceTypeEmissionOrder(",
            "private IReadOnlyList<AotCoreIrAbiSlotArtifact> ResolveDelegateInvokeParameterAbis(",
            "private AotCoreIrAbiSlotArtifact ResolveDelegateInvokeReturnAbi(",
            "private static string GetRuntimeTypeIdExpression(",
        ]:
            self.assertNotIn(removed_fragment, object_model_emission_source)

    def test_native_aot_object_model_emission_is_further_split_by_responsibility(self) -> None:
        object_model_source = NATIVE_AOT_OBJECT_MODEL_EMISSION_PATH.read_text(encoding="utf-8")

        for required_path in [
            NATIVE_AOT_OBJECT_EQUALITY_EMISSION_PATH,
            NATIVE_AOT_REFLECTION_OBJECT_EMISSION_PATH,
            NATIVE_AOT_OBJECT_MODEL_UTILITIES_PATH,
        ]:
            self.assertTrue(required_path.is_file(), msg=f"missing object-model split file: {required_path}")

        for required_fragment in [
            "private void EmitObjectEqualityHelpers(",
            "private void EmitReflectionObjectHelpers(",
            "private void EmitExceptionMetadataHelpers(",
            "private static IReadOnlyList<string> GetReferenceTypeEmissionOrder(",
            "private static string GetRuntimeTypeIdExpression(",
        ]:
            self.assertNotIn(required_fragment, object_model_source)

        object_equality_source = NATIVE_AOT_OBJECT_EQUALITY_EMISSION_PATH.read_text(encoding="utf-8")
        self.assertIn("private void EmitObjectEqualityHelpers(", object_equality_source)

        reflection_object_source = NATIVE_AOT_REFLECTION_OBJECT_EMISSION_PATH.read_text(encoding="utf-8")
        for required_fragment in [
            "private void EmitReflectionObjectHelpers(",
            "private void EmitExceptionMetadataHelpers(",
            "private bool RequiresManagedStringFactory(",
            "private bool RequiresManagedStringRuntime(",
        ]:
            self.assertIn(required_fragment, reflection_object_source)

        object_model_utilities_source = NATIVE_AOT_OBJECT_MODEL_UTILITIES_PATH.read_text(encoding="utf-8")
        for required_fragment in [
            "private static IReadOnlyList<string> GetReferenceTypeEmissionOrder(",
            "private IReadOnlyList<AotCoreIrAbiSlotArtifact> ResolveDelegateInvokeParameterAbis(",
            "private static string GetRuntimeTypeIdExpression(",
            "private static string FormatGenericArgumentList(",
        ]:
            self.assertIn(required_fragment, object_model_utilities_source)

    def test_loader_stage_is_split_into_orchestrator_and_helpers(self) -> None:
        loader_source = LOADER_STAGE_PATH.read_text(encoding="utf-8")

        for required_path in [
            LOADER_STAGE_CROSS_ASSEMBLY_PATH,
            LOADER_STAGE_ASSEMBLY_LOADING_PATH,
            LOADER_STAGE_INSTRUCTION_DECODING_PATH,
            LOADER_STAGE_METADATA_RESOLUTION_PATH,
            LOADER_STAGE_GENERIC_MATERIALIZATION_PATH,
        ]:
            self.assertTrue(required_path.is_file(), msg=f"missing loader split file: {required_path}")

        self.assertIn("public sealed partial class LoaderStage", loader_source)

        for required_fragment in [
            "private static List<LoadedAssemblyModel> MaterializeCrossAssemblyMethodInstantiations(",
            "private static LoadedAssemblyModel LoadAssembly(",
            "private static ManagedMethodBodyModel DecodeMethodBody(",
            "private static MethodReferenceSummary ResolveMethodReference(",
            "private static MaterializedGenericModels MaterializeGenericInstantiations(",
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


if __name__ == "__main__":
    unittest.main()
