from __future__ import annotations

import unittest
from pathlib import Path

from tests.support import read_contracts_source, read_loader_stage_source


REPO_ROOT = Path(__file__).resolve().parents[3]
CONTRACTS_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedClosureContracts.cs"
AOT_CORE_IR_LOWERING_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "AotCoreIrLowering.cs"
NATIVE_AOT_PLANNER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "NativeAotLoweringPlanner.cs"
NATIVE_AOT_EMITTER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "NativeAotEmitter.cs"
NATIVE_AOT_INVOCATION_PLANNING_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "Planning" / "NativeAotLoweringPlanner.InvocationPlanning.cs"
NATIVE_AOT_METHOD_EMISSION_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "Emission" / "NativeAotLoweringPlanner.MethodEmission.cs"
NATIVE_AOT_OBJECT_MODEL_EMISSION_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "Emission" / "NativeAotLoweringPlanner.ObjectModelEmission.cs"
LOADER_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "LoaderStage.cs"
NATIVE_AOT_TRANSLATION_TEMPLATE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "Templates" / "NativeAot.TranslationUnit.cpp.scriban"
NATIVE_AOT_OBJECT_MODEL_TEMPLATE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "Templates" / "NativeAot.ObjectModel.cpp.scriban"
NATIVE_AOT_METHOD_TEMPLATE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "Templates" / "NativeAot.Method.cpp.scriban"
NATIVE_AOT_AUDIT_SUMMARY_TEMPLATE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "Templates" / "NativeAot.AuditSummary.cpp.scriban"
NATIVE_AOT_AUDIT_PAGE_TEMPLATE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "Templates" / "NativeAot.AuditPage.cpp.scriban"


class Phase4BAotCoreIrObjectModelTests(unittest.TestCase):
    def test_contracts_expose_object_reference_and_runtime_service_carriers(self) -> None:
        contracts_source = read_contracts_source(REPO_ROOT)

        for required_fragment in [
            "public enum AotCoreIrReferenceKind : byte",
            "Type = 1",
            "Field = 2",
            "Method = 3",
            "public enum AotCoreIrTypeShapeKind : byte",
            "ReferenceType = 1",
            "ValueType = 2",
            "InterfaceType = 3",
            "public enum AotCoreIrRuntimeServiceKind : byte",
            "NewObject = 1",
            "LoadInstanceField = 2",
            "StoreInstanceField = 3",
            "LoadStaticField = 4",
            "StoreStaticField = 5",
            "NewArray = 6",
            "CastClass = 7",
            "IsInst = 8",
            "Box = 9",
            "Unbox = 10",
            "UnboxAny = 11",
            "LoadArrayElement = 12",
            "StoreArrayElement = 13",
            "InitObject = 14",
            "public sealed record AotCoreIrReferenceArtifact",
            "public required AotCoreIrReferenceKind Kind { get; init; }",
            "public required string AssemblyName { get; init; }",
            "public required string SubjectId { get; init; }",
            "public AotCoreIrTypeShapeKind TypeShape { get; init; }",
            "public string? ArrayElementSubjectId { get; init; }",
            "public AotCoreIrTypeShapeKind ArrayElementTypeShape { get; init; }",
            "public string? BaseTypeSubjectId { get; init; }",
            "public string? DeclaringTypeSubjectId { get; init; }",
            "public AotCoreIrTypeShapeKind DeclaringTypeShape { get; init; }",
            "public AotCoreIrReferenceArtifact? TargetReference { get; init; }",
            "public AotCoreIrRuntimeServiceKind? RuntimeServiceKind { get; init; }",
            "public bool IsValueType { get; init; }",
            "public string? BaseTypeSubjectId { get; init; }",
            "public IReadOnlyList<string>? ImplementedInterfaceSubjectIds { get; init; }",
        ]:
            self.assertIn(required_fragment, contracts_source)

    def test_loader_exposes_value_type_shape_and_base_type_identity_in_managed_type_model(self) -> None:
        contracts_source = read_contracts_source(REPO_ROOT)
        loader_source = read_loader_stage_source(REPO_ROOT)

        for required_fragment in [
            "public bool IsValueType { get; init; }",
            "public string? BaseTypeSubjectId { get; init; }",
            "public IReadOnlyList<string>? ImplementedInterfaceSubjectIds { get; init; }",
            "IsValueType = ResolveIsValueType(",
            "BaseTypeSubjectId = ResolveBaseTypeSubjectId(",
            "ImplementedInterfaceSubjectIds = ResolveImplementedInterfaceSubjectIds(",
            "IsValueType = definitionType.IsValueType,",
            "BaseTypeSubjectId = definitionType.BaseTypeSubjectId is null",
            "ImplementedInterfaceSubjectIds = definitionType.ImplementedInterfaceSubjectIds is null",
            "? null",
            ": SubstituteText(",
            ": definitionType.ImplementedInterfaceSubjectIds",
            ".Select(interfaceSubjectId => SubstituteText(",
            "private static IReadOnlyList<string> ResolveImplementedInterfaceSubjectIds(",
            "foreach (var interfaceImplementationHandle in typeDefinition.GetInterfaceImplementations())",
            "CollectImplementedInterfaceSubjectIds(",
            "private static string? ResolveBaseTypeSubjectId(",
            "private static bool ResolveIsValueType(",
            'return string.Equals(baseTypeIdentity.DefinitionSubjectId, "System.Private.CoreLib/System.ValueType", StringComparison.Ordinal)',
            '|| string.Equals(baseTypeIdentity.DefinitionSubjectId, "System.Private.CoreLib/System.Enum", StringComparison.Ordinal);',
        ]:
            self.assertIn(required_fragment, contracts_source if required_fragment.startswith("public ") else loader_source)

    def test_lowering_maps_object_model_opcodes_to_target_reference_and_runtime_service(self) -> None:
        lowering_source = AOT_CORE_IR_LOWERING_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "TargetReference = ResolveTargetReference(",
            "RuntimeServiceKind = ResolveRuntimeServiceKind(",
            "IReadOnlyDictionary<string, ManagedTypeModel> managedTypes",
            "IReadOnlyDictionary<string, ManagedFieldModel> managedFields",
            'case "newobj":',
            'case "ldfld":',
            'case "stfld":',
            'case "ldsfld":',
            'case "stsfld":',
            'case "newarr":',
            'case "castclass":',
            'case "isinst":',
            'case "box":',
            'case "unbox":',
            'case "unbox.any":',
            'case "initobj":',
            'case "ldelem":',
            'case "ldelem.ref":',
            'case "stelem":',
            'case "stelem.ref":',
            "AotCoreIrReferenceKind.Type",
            "AotCoreIrReferenceKind.Field",
            "AotCoreIrReferenceKind.Method",
            "AotCoreIrTypeShapeKind.ValueType",
            "AotCoreIrTypeShapeKind.ReferenceType",
            "ArrayElementSubjectId = ResolveArrayElementSubjectId(",
            "ArrayElementTypeShape = ResolveArrayElementTypeShape(",
            "BaseTypeSubjectId = managedType?.BaseTypeSubjectId",
            "ImplementedInterfaceSubjectIds = managedType?.ImplementedInterfaceSubjectIds",
            "AotCoreIrRuntimeServiceKind.NewObject",
            "AotCoreIrRuntimeServiceKind.NewArray",
            "AotCoreIrRuntimeServiceKind.CastClass",
            "AotCoreIrRuntimeServiceKind.IsInst",
            "AotCoreIrRuntimeServiceKind.Box",
            "AotCoreIrRuntimeServiceKind.Unbox",
            "AotCoreIrRuntimeServiceKind.UnboxAny",
            "AotCoreIrRuntimeServiceKind.LoadArrayElement",
            "AotCoreIrRuntimeServiceKind.StoreArrayElement",
            "AotCoreIrRuntimeServiceKind.InitObject",
            "DeclaringTypeSubjectId = field.DeclaringTypeSubjectId",
            "DeclaringTypeShape = ResolveTypeShape(",
            "TypeShape = ResolveTypeShape(",
        ]:
            self.assertIn(required_fragment, lowering_source)

    def test_native_aot_planner_exposes_first_batch_object_model_entrypoints(self) -> None:
        planner_source = NATIVE_AOT_PLANNER_PATH.read_text(encoding="utf-8")
        invocation_planning_source = NATIVE_AOT_INVOCATION_PLANNING_PATH.read_text(encoding="utf-8")
        method_emission_source = NATIVE_AOT_METHOD_EMISSION_PATH.read_text(encoding="utf-8")
        object_model_emission_source = NATIVE_AOT_OBJECT_MODEL_EMISSION_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "public sealed partial class NativeAotLoweringPlanner",
            "public NativeAotTemplateModel Create(",
        ]:
            self.assertIn(required_fragment, planner_source)

        for required_fragment in [
            "CollectReachableMethods(",
        ]:
            self.assertIn(required_fragment, invocation_planning_source)

        for required_fragment in [
            "FormatMethodDeclaration(",
            "EmitManagedMethod(",
            "EmitNewObject(",
            "EmitNewArray(",
            "EmitArrayLength(",
            "EmitArrayElementAddress(",
            "EmitCastClass(",
            "EmitIsInst(",
            "EmitLoadLocalAddress(",
            "EmitInitObj(",
            "EmitLoadObjectValue(",
            "EmitStoreObjectValue(",
            "EmitFieldLoad(",
            "EmitFieldStore(",
            "EmitArrayLoad(",
            "EmitArrayStore(",
            "EmitLoadIndirectInt32(",
            "EmitStoreIndirectInt32(",
            "EmitBox(",
            "EmitUnbox(",
            "EmitUnboxAny(",
        ]:
            self.assertIn(required_fragment, method_emission_source)

        for required_fragment in [
            "EmitObjectModelDeclarations(",
            "chaos_is_array_type_compatible(",
            "chaos_is_array_store_compatible(",
        ]:
            self.assertIn(required_fragment, object_model_emission_source)

    def test_native_aot_emitter_uses_scriban_templates_and_planner(self) -> None:
        emitter_source = NATIVE_AOT_EMITTER_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "using Scriban.Runtime;",
            "new NativeAotLoweringPlanner()",
            "NativeAotTemplateCatalog.GetTranslationUnitTemplate()",
            "NativeAotTemplateCatalog.GetObjectModelTemplate()",
            "NativeAotTemplateCatalog.GetMethodTemplate()",
            "ScribanTemplateRenderer.RenderTemplate(",
            "BuildObjectModelSection(",
            "BuildMethodSection(",
            "BuildGeneratedTranslationUnit(",
            "NativeAotTemplateCatalog.GetAssemblyFullClosureAuditSummaryTemplate()",
            "NativeAotTemplateCatalog.GetAssemblyFullClosureAuditPageTemplate()",
        ]:
            self.assertIn(required_fragment, emitter_source)

        for retired_fragment in [
            "using Scriban;",
            "TemplateContext",
            "Render(context)",
            "LoadTemplate(",
        ]:
            self.assertNotIn(retired_fragment, emitter_source)

        self.assertTrue(NATIVE_AOT_PLANNER_PATH.is_file(), msg=f"missing planner source: {NATIVE_AOT_PLANNER_PATH}")
        self.assertTrue(NATIVE_AOT_TRANSLATION_TEMPLATE_PATH.is_file(), msg=f"missing translation template: {NATIVE_AOT_TRANSLATION_TEMPLATE_PATH}")
        self.assertTrue(NATIVE_AOT_OBJECT_MODEL_TEMPLATE_PATH.is_file(), msg=f"missing object model template: {NATIVE_AOT_OBJECT_MODEL_TEMPLATE_PATH}")
        self.assertTrue(NATIVE_AOT_METHOD_TEMPLATE_PATH.is_file(), msg=f"missing method template: {NATIVE_AOT_METHOD_TEMPLATE_PATH}")
        self.assertTrue(NATIVE_AOT_AUDIT_SUMMARY_TEMPLATE_PATH.is_file(), msg=f"missing audit summary template: {NATIVE_AOT_AUDIT_SUMMARY_TEMPLATE_PATH}")
        self.assertTrue(NATIVE_AOT_AUDIT_PAGE_TEMPLATE_PATH.is_file(), msg=f"missing audit page template: {NATIVE_AOT_AUDIT_PAGE_TEMPLATE_PATH}")


if __name__ == "__main__":
    unittest.main()
