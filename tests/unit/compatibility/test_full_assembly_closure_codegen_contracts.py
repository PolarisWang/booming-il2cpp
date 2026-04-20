from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
ARTIFACT_MODELS_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedClosureArtifactModels.cs"
CODEGEN_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "CodeGenStage.cs"
NATIVE_REFERENCE_EMITTER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "NativeReferenceProofEmitter.cs"
NATIVE_AOT_EMITTER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "NativeAotEmitter.cs"
RUNTIME_SKELETON_INTERFACE_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.InterfaceDispatchMessageStub.cpp.scriban"
)
RUNTIME_SKELETON_PAGE_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeletonPage.cpp.scriban"
)
RUNTIME_SKELETON_STATIC_INT_FORWARDER_TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.RuntimeSkeleton.StaticIntForwarderStub.cpp.scriban"
)


class FullAssemblyClosureCodegenContractTests(unittest.TestCase):
    def test_contracts_expose_translation_unit_audit_fields(self) -> None:
        artifact_models_source = ARTIFACT_MODELS_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "public string? TranslationUnitMode { get; init; }",
            "public IReadOnlyList<string>? TranslationUnitMethodSubjectIds { get; init; }",
            "public int? TranslationUnitMethodCount { get; init; }",
            "public int? TranslationUnitPageSize { get; init; }",
            "public int? TranslationUnitPageCount { get; init; }",
            "public IReadOnlyList<AuditTranslationUnitPageArtifact>? TranslationUnitPages { get; init; }",
            "public string? AuditStatus { get; init; }",
            "public string? AuditMessage { get; init; }",
            "public string? PreferredAssemblyDispatchSubjectId { get; init; }",
        ]:
            self.assertIn(required_fragment, artifact_models_source)

    def test_codegen_stage_routes_full_assembly_closure_without_entry_to_runtime_and_audit_plans(self) -> None:
        codegen_source = CODEGEN_STAGE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "if (linkedWorld.FullAssemblyClosure && string.IsNullOrWhiteSpace(linkedWorld.EntryPointSubjectId))",
            'PlanKind = "assembly-full-closure-runtime-skeleton"',
            'NativeEntryFunctionName = "RunNativeReferenceAssembly"',
            'RuntimeExecutionKind = "assembly-bound-native-reference-skeleton"',
            'TranslationUnitMode = "runtime-skeleton"',
            "TranslationUnitMethodSubjectIds = methodSubjectIds",
            "TranslationUnitMethodCount = methodSubjectIds.Count",
            "TranslationUnitPageSize = AuditTranslationUnitPageSize",
            "TranslationUnitPageCount = GetAuditPageCount(methodSubjectIds.Count)",
            'TranslationUnitPages = BuildAuditTranslationUnitPages(methodSubjectIds, "generated/runtime/native-reference.runtime-skeleton")',
            'AuditStatus = "runtime-skeleton"',
            'PlanKind = "assembly-full-closure-audit"',
            'TranslationUnitMode = "audit-only"',
            'TranslationUnitPages = BuildAuditTranslationUnitPages(methodSubjectIds, "generated/audit/native-aot.methods")',
            'AuditStatus = "not-yet-emittable"',
        ]:
            self.assertIn(required_fragment, codegen_source)

    def test_emitters_support_audit_translation_unit_generation_for_assembly_bound_plans(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        native_aot_emitter_source = NATIVE_AOT_EMITTER_PATH.read_text(encoding="utf-8")

        self.assertIn('string.Equals(loweringPlan.PlanKind, "assembly-full-closure-audit", StringComparison.Ordinal)', native_reference_emitter_source)
        self.assertIn('string.Equals(loweringPlan.PlanKind, "assembly-full-closure-audit", StringComparison.Ordinal)', native_aot_emitter_source)
        self.assertIn("BuildAssemblyFullClosureAuditGeneratedSources", native_reference_emitter_source)
        self.assertIn("BuildAssemblyFullClosureAuditGeneratedSources", native_aot_emitter_source)
        self.assertIn("AuditTranslationUnitPageSize = 1024", native_reference_emitter_source)
        self.assertIn("AuditTranslationUnitPageSize = 1024", native_aot_emitter_source)
        self.assertIn("TranslationUnitPages = loweringPlan.TranslationUnitPages", native_reference_emitter_source)
        self.assertIn("TranslationUnitPages = loweringPlan.TranslationUnitPages", native_aot_emitter_source)

    def test_native_reference_emitter_has_room_for_runtime_skeleton_plan_kind(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        artifact_models_source = ARTIFACT_MODELS_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            'string.Equals(loweringPlan.PlanKind, "assembly-full-closure-runtime-skeleton", StringComparison.Ordinal)',
            "BuildAssemblyFullClosureRuntimeSkeletonGeneratedSources",
            "BuildAssemblyFullClosureRuntimeSkeletonMethodStubName",
            "BuildAssemblyFullClosureRuntimeSkeletonPageDispatchFunctionName",
            "BuildPreferredAssemblyDispatchSubjectId",
            "TryBuildAssemblyBoundConsoleWriteLineStub",
            'runtime skeleton emitter expected translation-unit mode \'runtime-skeleton\'',
            'runtime skeleton emitter expected audit status \'runtime-skeleton\'',
            "public string? RuntimeExecutionKind { get; init; }",
        ]:
            self.assertTrue(
                required_fragment in native_reference_emitter_source or required_fragment in artifact_models_source,
                msg=f"missing runtime skeleton fragment: {required_fragment}",
            )

    def test_runtime_skeleton_static_call_ctor_getter_reuses_typed_il_roles(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")

        self.assertIn('RequireMethodContract(method, "static-forwarder", "has-canonical-body");', native_reference_emitter_source)
        self.assertIn('RequireMethodContract(method, "instance-field-getter", "has-canonical-body");', native_reference_emitter_source)
        self.assertIn('case "staticCallCtorGetter":', native_reference_emitter_source)
        self.assertIn('case "constructorThenInstanceCall":', native_reference_emitter_source)
        self.assertIn("BuildAssemblyBoundDelegateClosedTargetRelayStub", native_reference_emitter_source)
        self.assertIn("TryBuildAssemblyBoundDelegateClosedTargetRelayPlan", native_reference_emitter_source)
        self.assertIn('string.Equals(executableLoweringPlan.PlanKind, "delegateClosedTargetRelayMinimal", StringComparison.Ordinal)', native_reference_emitter_source)
        self.assertIn("BuildAssemblyBoundExceptionThrowCatchFinallyStub", native_reference_emitter_source)
        self.assertIn("TryBuildAssemblyBoundExceptionThrowCatchFinallyPlan", native_reference_emitter_source)
        self.assertIn("BuildAssemblyBoundNestedExceptionThrowCatchFinallyStub", native_reference_emitter_source)
        self.assertIn("TryBuildAssemblyBoundNestedExceptionThrowCatchFinallyPlan", native_reference_emitter_source)
        self.assertIn("BuildAssemblyBoundMarshalingUtf8ExportStub", native_reference_emitter_source)
        self.assertIn("TryBuildAssemblyBoundMarshalingUtf8ExportPlan", native_reference_emitter_source)
        self.assertIn("BuildAssemblyBoundReflectionInteropClosureStub", native_reference_emitter_source)
        self.assertIn("TryBuildAssemblyBoundReflectionInteropClosurePlan", native_reference_emitter_source)
        self.assertIn("BuildAssemblyBoundPInvokeDllImportMinimalStub", native_reference_emitter_source)
        self.assertIn("TryBuildAssemblyBoundPInvokeDirectCallPlan", native_reference_emitter_source)
        self.assertIn('string.Equals(executableLoweringPlan.PlanKind, InteropPInvokeDirectCallMinimal, StringComparison.Ordinal)', native_reference_emitter_source)
        self.assertIn("RequireCapability(entryPointMethod, \"requires-imported-call\");", native_reference_emitter_source)
        self.assertIn("RequireMethodContract(importMethod, \"imported-method\", \"no-canonical-body\");", native_reference_emitter_source)
        self.assertIn("BuildAssemblyBoundArrayBoxingReferenceArrayStub", native_reference_emitter_source)
        self.assertIn("TryBuildAssemblyBoundArrayBoxingReferenceArrayPlan", native_reference_emitter_source)
        self.assertIn('string.Equals(executableLoweringPlan.PlanKind, "arrayBoxingReferenceArray", StringComparison.Ordinal)', native_reference_emitter_source)
        self.assertIn('case "arrayBoxingReferenceArray":', native_reference_emitter_source)
        self.assertIn("BoxedValueTypeToken = CreateTypeTokenLiteral(metadataRegistration, boxedValueTypeSubjectId)", native_reference_emitter_source)
        self.assertIn("private static string CreateTypeTokenLiteral(", native_reference_emitter_source)
        self.assertIn("CreatePseudoTypeToken(subjectId)", native_reference_emitter_source)
        self.assertIn("BuildAssemblyBoundInterfaceDispatchMessageStub", native_reference_emitter_source)
        self.assertIn("TryBuildAssemblyBoundInterfaceDispatchMessagePlan", native_reference_emitter_source)
        self.assertIn('string.Equals(executableLoweringPlan.PlanKind, "interfaceDispatchMessage", StringComparison.Ordinal)', native_reference_emitter_source)
        self.assertIn('case "interfaceDispatchMessage":', native_reference_emitter_source)
        self.assertIn('"interface-runtime-helper"', native_reference_emitter_source)
        self.assertLess(
            native_reference_emitter_source.index("TryBuildAssemblyBoundMarshalingUtf8ExportPlan"),
            native_reference_emitter_source.index("TryBuildAssemblyBoundConsoleWriteLineStub"),
        )
        self.assertIn("NestedExceptionThrowCatchFinallyMinimal", native_reference_emitter_source)
        self.assertIn("ExceptionThrowCatchFinallyMinimal", native_reference_emitter_source)
        self.assertIn("MarshalingUtf8ExportMinimal", native_reference_emitter_source)
        self.assertIn("ReflectionInteropClosureMinimal", native_reference_emitter_source)
        self.assertIn("InteropPInvokeDirectCallMinimal", native_reference_emitter_source)
        self.assertIn("StringConcatPairMethodSubjectId", native_reference_emitter_source)
        self.assertIn("StringConcatTripleMethodSubjectId", native_reference_emitter_source)
        self.assertIn("NormalizeStringConcatIcall", native_reference_emitter_source)
        self.assertNotIn(
            'RequireMethodContract(method, "static-method", "has-canonical-body");\n        RequireCapability(method, "requires-console-string-output");\n        RequireInstructionCount(method, instructions, 7);',
            native_reference_emitter_source.replace("\r\n", "\n"),
        )

    def test_runtime_skeleton_interface_dispatch_registers_local_virtual_target(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        interface_template_source = RUNTIME_SKELETON_INTERFACE_TEMPLATE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            '["message_prefix_literal"] = loweringPlan.MessagePrefixLiteral',
            '["message_prefix_literal_byte_count"] = loweringPlan.MessagePrefixLiteralByteCount ?? 0',
            '["message_suffix_literal"] = loweringPlan.MessageSuffixLiteral',
            '["message_suffix_literal_byte_count"] = loweringPlan.MessageSuffixLiteralByteCount ?? 0',
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        for required_fragment in [
            "struct UnresolvedVirtualCallEntry",
            "InterfaceDispatchResolvedRender",
            "registered_code_registration.unresolved_virtual_calls = unresolved_virtual_calls",
            "registered_code_registration.unresolved_virtual_call_count",
            "bridge->register_codegen(&registered_code_registration, metadata_registration, options)",
            "{ {{ reference_type_token }}, {{ instance_method_token }}, reinterpret_cast<void*>(&{{ stub_name }}_InterfaceDispatchResolvedRender) }",
            "void* captured_value = nullptr;",
            "abi->field_get_value(",
            "{{ message_prefix_literal }}",
            "{{ message_suffix_literal }}",
            "BridgeStatus invoke_status = bridge->invoke_virtual(",
            "return invoke_status;",
        ]:
            self.assertIn(required_fragment, interface_template_source)

    def test_runtime_skeleton_static_int_forwarder_calls_target_stub(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        page_template_source = RUNTIME_SKELETON_PAGE_TEMPLATE_PATH.read_text(encoding="utf-8")
        forwarder_template_source = RUNTIME_SKELETON_STATIC_INT_FORWARDER_TEMPLATE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "BuildAssemblyFullClosureRuntimeSkeletonMethodStubDeclaration",
            "method_stub_declarations",
            "methodStubNamesBySubjectId",
            "TryBuildAssemblyBoundStaticIntForwarderStub",
            "GetRuntimeSkeletonStaticIntForwarderStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        self.assertIn("RuntimeSkeletonStaticIntForwarderStubTemplateRelativePath", catalog_source)
        self.assertIn("method_stub_declarations", page_template_source)
        self.assertIn("{{ target_stub_name }}(", forwarder_template_source)
        self.assertIn("return {{ target_stub_name }}(", forwarder_template_source)
        self.assertIn("(void)managed_args;", forwarder_template_source)

    def test_runtime_skeleton_constructor_and_field_backed_string_helpers_have_templates(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        constructor_template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.RuntimeSkeleton.ConstructorFieldSetterStub.cpp.scriban"
        ).read_text(encoding="utf-8")
        render_template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.RuntimeSkeleton.FieldBackedStringReturnStub.cpp.scriban"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildAssemblyBoundConstructorFieldSetterStub",
            "TryBuildAssemblyBoundFieldBackedStringReturnStub",
            "GetRuntimeSkeletonConstructorFieldSetterStubTemplate",
            "GetRuntimeSkeletonFieldBackedStringReturnStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        for required_fragment in [
            "RuntimeSkeletonConstructorFieldSetterStubTemplateRelativePath",
            "RuntimeSkeletonFieldBackedStringReturnStubTemplateRelativePath",
        ]:
            self.assertIn(required_fragment, catalog_source)

        for required_fragment in [
            "_ManagedArgs",
            "bridge == nullptr",
            "request->instance",
            "request->value",
            "field_set_value(",
        ]:
            self.assertIn(required_fragment, constructor_template_source)

        for required_fragment in [
            "_ManagedArgs",
            "request->instance",
            "request->return_value",
            "field_get_value(",
            "*request->return_value = full_message;",
        ]:
            self.assertIn(required_fragment, render_template_source)

    def test_runtime_skeleton_string_forwarder_and_field_getter_helpers_have_templates(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        string_forwarder_template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.RuntimeSkeleton.StaticStringForwarderStub.cpp.scriban"
        ).read_text(encoding="utf-8")
        field_getter_template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.RuntimeSkeleton.FieldGetterStringReturnStub.cpp.scriban"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildAssemblyBoundStaticStringForwarderStub",
            "TryBuildAssemblyBoundFieldGetterStringReturnStub",
            "GetRuntimeSkeletonStaticStringForwarderStubTemplate",
            "GetRuntimeSkeletonFieldGetterStringReturnStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        for required_fragment in [
            "RuntimeSkeletonStaticStringForwarderStubTemplateRelativePath",
            "RuntimeSkeletonFieldGetterStringReturnStubTemplateRelativePath",
        ]:
            self.assertIn(required_fragment, catalog_source)

        for required_fragment in [
            "_ManagedArgs",
            "request->value",
            "request->return_value",
            "*request->return_value = request->value;",
        ]:
            self.assertIn(required_fragment, string_forwarder_template_source)

        for required_fragment in [
            "_ManagedArgs",
            "request->instance",
            "request->return_value",
            "field_get_value(",
            "*request->return_value = captured_value;",
        ]:
            self.assertIn(required_fragment, field_getter_template_source)

    def test_runtime_skeleton_static_string_producer_write_line_helpers_have_templates(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        static_literal_template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.RuntimeSkeleton.StaticLiteralStringReturnStub.cpp.scriban"
        ).read_text(encoding="utf-8")
        write_line_template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.RuntimeSkeleton.StaticStringProducerConsoleWriteLineStub.cpp.scriban"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildAssemblyBoundStaticLiteralStringReturnStub",
            "TryBuildAssemblyBoundStaticStringProducerConsoleWriteLineStub",
            "GetRuntimeSkeletonStaticLiteralStringReturnStubTemplate",
            "GetRuntimeSkeletonStaticStringProducerConsoleWriteLineStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        for required_fragment in [
            "RuntimeSkeletonStaticLiteralStringReturnStubTemplateRelativePath",
            "RuntimeSkeletonStaticStringProducerConsoleWriteLineStubTemplateRelativePath",
        ]:
            self.assertIn(required_fragment, catalog_source)

        for required_fragment in [
            "_ManagedArgs",
            "request->return_value",
            "string_new_utf8",
            "*request->return_value = literal_string;",
        ]:
            self.assertIn(required_fragment, static_literal_template_source)

        for required_fragment in [
            "_ProducerArgs",
            "target_stub_name",
            "producer_args",
            "resolve_icall",
            "write_line_string",
        ]:
            self.assertIn(required_fragment, write_line_template_source)

    def test_runtime_skeleton_static_string_forwarder_write_line_helper_has_template(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        forwarder_write_line_template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.RuntimeSkeleton.StaticStringForwarderConsoleWriteLineStub.cpp.scriban"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildAssemblyBoundStaticStringForwarderConsoleWriteLineStub",
            "GetRuntimeSkeletonStaticStringForwarderConsoleWriteLineStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        self.assertIn(
            "RuntimeSkeletonStaticStringForwarderConsoleWriteLineStubTemplateRelativePath",
            catalog_source,
        )

        for required_fragment in [
            "_ForwarderArgs",
            "target_stub_name",
            "input_value",
            "forwarder_args",
            "resolve_icall",
            "write_line_string",
        ]:
            self.assertIn(required_fragment, forwarder_write_line_template_source)

    def test_runtime_skeleton_static_string_producer_forwarder_write_line_helper_has_template(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        producer_forwarder_write_line_template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.RuntimeSkeleton.StaticStringProducerForwarderConsoleWriteLineStub.cpp.scriban"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildAssemblyBoundStaticStringProducerForwarderConsoleWriteLineStub",
            "GetRuntimeSkeletonStaticStringProducerForwarderConsoleWriteLineStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        self.assertIn(
            "RuntimeSkeletonStaticStringProducerForwarderConsoleWriteLineStubTemplateRelativePath",
            catalog_source,
        )

        for required_fragment in [
            "_ProducerArgs",
            "_ForwarderArgs",
            "producer_stub_name",
            "forwarder_stub_name",
            "producer_args",
            "forwarder_args",
            "resolve_icall",
            "write_line_string",
        ]:
            self.assertIn(required_fragment, producer_forwarder_write_line_template_source)

    def test_runtime_skeleton_static_string_producer_ctor_getter_write_line_helper_has_template(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        producer_ctor_getter_write_line_template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.RuntimeSkeleton.StaticStringProducerCtorGetterConsoleWriteLineStub.cpp.scriban"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildAssemblyBoundStaticStringProducerCtorGetterConsoleWriteLineStub",
            "GetRuntimeSkeletonStaticStringProducerCtorGetterConsoleWriteLineStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        self.assertIn(
            "RuntimeSkeletonStaticStringProducerCtorGetterConsoleWriteLineStubTemplateRelativePath",
            catalog_source,
        )

        for required_fragment in [
            "_ProducerArgs",
            "producer_stub_name",
            "reference_type_token",
            "captured_field_token",
            "produced_value",
            "object_new",
            "field_set_value",
            "field_get_value",
            "write_line_string",
        ]:
            self.assertIn(required_fragment, producer_ctor_getter_write_line_template_source)

    def test_runtime_skeleton_static_string_producer_ctor_render_write_line_helper_has_template(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        producer_ctor_render_write_line_template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.RuntimeSkeleton.StaticStringProducerCtorRenderConsoleWriteLineStub.cpp.scriban"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildAssemblyBoundStaticStringProducerCtorRenderConsoleWriteLineStub",
            "GetRuntimeSkeletonStaticStringProducerCtorRenderConsoleWriteLineStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        self.assertIn(
            "RuntimeSkeletonStaticStringProducerCtorRenderConsoleWriteLineStubTemplateRelativePath",
            catalog_source,
        )

        for required_fragment in [
            "_ProducerArgs",
            "producer_stub_name",
            "reference_type_token",
            "captured_field_token",
            "produced_value",
            "object_new",
            "field_set_value",
            "field_get_value",
            "concat_icall_literal",
            "write_line_string",
        ]:
            self.assertIn(required_fragment, producer_ctor_render_write_line_template_source)

    def test_runtime_skeleton_static_string_producer_ctor_instance_call_forwarder_write_line_helper_has_template(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.RuntimeSkeleton.StaticStringProducerCtorInstanceCallForwarderConsoleWriteLineStub.cpp.scriban"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildAssemblyBoundStaticStringProducerCtorInstanceCallForwarderConsoleWriteLineStub",
            "GetRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderConsoleWriteLineStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        self.assertIn(
            "RuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderConsoleWriteLineStubTemplateRelativePath",
            catalog_source,
        )

        for required_fragment in [
            "_ProducerArgs",
            "_ManagedArgs",
            "_ForwarderArgs",
            "producer_stub_name",
            "instance_call_stub_name",
            "forwarder_stub_name",
            "reference_type_token",
            "captured_field_token",
            "produced_value",
            "object_new",
            "field_set_value",
            "forwarded_message",
            "write_line_string",
        ]:
            self.assertIn(required_fragment, template_source)

    def test_runtime_skeleton_static_string_producer_forwarder_ctor_instance_call_write_line_helper_has_template(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.RuntimeSkeleton.StaticStringProducerForwarderCtorInstanceCallConsoleWriteLineStub.cpp.scriban"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildAssemblyBoundStaticStringProducerForwarderCtorInstanceCallConsoleWriteLineStub",
            "GetRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallConsoleWriteLineStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        self.assertIn(
            "RuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallConsoleWriteLineStubTemplateRelativePath",
            catalog_source,
        )

        for required_fragment in [
            "_ProducerArgs",
            "_ForwarderArgs",
            "_ManagedArgs",
            "producer_stub_name",
            "forwarder_stub_name",
            "instance_call_stub_name",
            "reference_type_token",
            "captured_field_token",
            "produced_value",
            "forwarded_value",
            "object_new",
            "field_set_value",
            "write_line_string",
        ]:
            self.assertIn(required_fragment, template_source)

    def test_runtime_skeleton_static_string_producer_forwarder_ctor_instance_call_forwarder_write_line_helper_has_template(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.RuntimeSkeleton.StaticStringProducerForwarderCtorInstanceCallForwarderConsoleWriteLineStub.cpp.scriban"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildAssemblyBoundStaticStringProducerForwarderCtorInstanceCallForwarderConsoleWriteLineStub",
            "GetRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderConsoleWriteLineStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        self.assertIn(
            "RuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderConsoleWriteLineStubTemplateRelativePath",
            catalog_source,
        )

        for required_fragment in [
            "_ProducerArgs",
            "_ForwarderArgs",
            "_ManagedArgs",
            "producer_stub_name",
            "initial_forwarder_stub_name",
            "instance_call_stub_name",
            "terminal_forwarder_stub_name",
            "reference_type_token",
            "captured_field_token",
            "produced_value",
            "initial_forwarded_value",
            "forwarded_message",
            "object_new",
            "field_set_value",
            "write_line_string",
        ]:
            self.assertIn(required_fragment, template_source)

    def test_runtime_skeleton_static_string_producer_ctor_instance_call_forwarder_forwarder_write_line_helper_has_template(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.RuntimeSkeleton.StaticStringProducerCtorInstanceCallForwarderForwarderConsoleWriteLineStub.cpp.scriban"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildAssemblyBoundStaticStringProducerCtorInstanceCallForwarderForwarderConsoleWriteLineStub",
            "GetRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderForwarderConsoleWriteLineStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        self.assertIn(
            "RuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderForwarderConsoleWriteLineStubTemplateRelativePath",
            catalog_source,
        )

        for required_fragment in [
            "_ProducerArgs",
            "_ManagedArgs",
            "_ForwarderArgs",
            "producer_stub_name",
            "instance_call_stub_name",
            "initial_forwarder_stub_name",
            "terminal_forwarder_stub_name",
            "reference_type_token",
            "captured_field_token",
            "produced_value",
            "initial_forwarded_message",
            "forwarded_message",
            "field_set_value",
            "write_line_string",
        ]:
            self.assertIn(required_fragment, template_source)

    def test_runtime_skeleton_static_string_producer_forwarder_ctor_instance_call_forwarder_forwarder_write_line_helper_has_template(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.RuntimeSkeleton.StaticStringProducerForwarderCtorInstanceCallForwarderForwarderConsoleWriteLineStub.cpp.scriban"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildAssemblyBoundStaticStringProducerForwarderCtorInstanceCallForwarderForwarderConsoleWriteLineStub",
            "GetRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderForwarderConsoleWriteLineStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        self.assertIn(
            "RuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderForwarderConsoleWriteLineStubTemplateRelativePath",
            catalog_source,
        )

        for required_fragment in [
            "_ProducerArgs",
            "_ForwarderArgs",
            "_ManagedArgs",
            "producer_stub_name",
            "initial_forwarder_stub_name",
            "instance_call_stub_name",
            "middle_forwarder_stub_name",
            "terminal_forwarder_stub_name",
            "reference_type_token",
            "captured_field_token",
            "produced_value",
            "initial_forwarded_value",
            "middle_forwarded_message",
            "forwarded_message",
            "object_new",
            "field_set_value",
            "write_line_string",
        ]:
            self.assertIn(required_fragment, template_source)

    def test_runtime_skeleton_static_string_producer_ctor_instance_call_forwarder_forwarder_forwarder_write_line_helper_has_template(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.RuntimeSkeleton.StaticStringProducerCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineStub.cpp.scriban"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildAssemblyBoundStaticStringProducerCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineStub",
            "GetRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        self.assertIn(
            "RuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineStubTemplateRelativePath",
            catalog_source,
        )

        for required_fragment in [
            "_ProducerArgs",
            "_ManagedArgs",
            "_ForwarderArgs",
            "producer_stub_name",
            "instance_call_stub_name",
            "initial_forwarder_stub_name",
            "middle_forwarder_stub_name",
            "terminal_forwarder_stub_name",
            "reference_type_token",
            "captured_field_token",
            "produced_value",
            "initial_forwarded_message",
            "middle_forwarded_message",
            "forwarded_message",
            "field_set_value",
            "write_line_string",
        ]:
            self.assertIn(required_fragment, template_source)

    def test_runtime_skeleton_static_string_producer_forwarder_ctor_instance_call_forwarder_forwarder_forwarder_write_line_helper_has_template(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.RuntimeSkeleton.StaticStringProducerForwarderCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineStub.cpp.scriban"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildAssemblyBoundStaticStringProducerForwarderCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineStub",
            "GetRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        self.assertIn(
            "RuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineStubTemplateRelativePath",
            catalog_source,
        )

        for required_fragment in [
            "_ProducerArgs",
            "_ForwarderArgs",
            "_ManagedArgs",
            "producer_stub_name",
            "initial_forwarder_stub_name",
            "instance_call_stub_name",
            "first_terminal_forwarder_stub_name",
            "second_terminal_forwarder_stub_name",
            "third_terminal_forwarder_stub_name",
            "reference_type_token",
            "captured_field_token",
            "produced_value",
            "initial_forwarded_value",
            "first_forwarded_message",
            "second_forwarded_message",
            "forwarded_message",
            "object_new",
            "field_set_value",
            "write_line_string",
        ]:
            self.assertIn(required_fragment, template_source)

if __name__ == "__main__":
    unittest.main()
