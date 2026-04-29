from tests.unit.compatibility.full_assembly_closure_codegen_contracts_test_support import *


class TestFullAssemblyClosureCodegenContractsCore(FullAssemblyClosureCodegenContractTestSupport):
    def test_bridge_contract_exposes_optional_numeric_method_dispatch_and_extended_statuses(self) -> None:
        bridge_contract_source = (
            REPO_ROOT
            / "contracts"
            / "native"
            / "v0"
            / "codegen_bridge.h"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "uint32_t method_id;",
            "subject_id_utf8",
            "CHAOS_BRIDGE_STATUS_METADATA_RESOLUTION_FAILED",
            "CHAOS_BRIDGE_STATUS_CLASS_INIT_FAILED",
            "CHAOS_BRIDGE_STATUS_RUNTIME_CALL_FAILED",
            "RuntimeTypeCapabilityInfoV0",
            "RuntimeTypeCapabilityEntryV0",
            "type_capabilities",
            "type_capability_count",
            "query_type_capability",
        ]:
            self.assertIn(required_fragment, bridge_contract_source)

        runtime_contract_source = (
            REPO_ROOT
            / "contracts"
            / "native"
            / "v0"
            / "runtime_abi.h"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "typedef struct RuntimeTypeCapabilityInfoV0",
            "value_size_bytes",
            "vector_lane_kind",
            "vector_lane_count",
            "vector_width_bytes",
            "scalar_kind",
            "type_query_capability",
        ]:
            self.assertIn(required_fragment, runtime_contract_source)

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
            'TranslationUnitPages = BuildAuditTranslationUnitPages(methodSubjectIds, "generated/audit/native-aot.audit", ".json")',
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
        self.assertIn("runtimeSkeletonEmission.GeneratedArtifacts", native_reference_emitter_source)
        self.assertIn("runtimeSkeletonEmission.PreferredAssemblyDispatchSubjectId", native_reference_emitter_source)
        self.assertIn("runtimeSkeletonEmission.TranslationUnitPages", native_reference_emitter_source)
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
        artifact_models_source = ARTIFACT_MODELS_PATH.read_text(encoding="utf-8")

        self.assertIn('RequireMethodContract(method, "static-forwarder", "has-canonical-body");', native_reference_emitter_source)
        self.assertIn('RequireMethodContract(method, "instance-field-getter", "has-canonical-body");', native_reference_emitter_source)
        self.assertIn('case "staticCallCtorGetter":', native_reference_emitter_source)
        self.assertIn('case "constructorThenInstanceCall":', native_reference_emitter_source)
        self.assertIn("BuildAssemblyBoundDelegateClosedTargetRelayStub", native_reference_emitter_source)
        self.assertIn("TryBuildAssemblyBoundDelegateClosedTargetRelayPlan", native_reference_emitter_source)
        self.assertIn("BuildAssemblyBoundDelegateClosedTargetRelayStub", native_reference_emitter_source)
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
        self.assertIn("BuildAssemblyBoundPInvokeDllImportMinimalStub", native_reference_emitter_source)
        self.assertIn("HasCapability(entryPointMethod, \"requires-imported-call\")", native_reference_emitter_source)
        self.assertIn("HasMethodContract(importMethod, \"imported-method\", \"no-canonical-body\")", native_reference_emitter_source)
        self.assertIn("BuildAssemblyBoundArrayBoxingReferenceArrayStub", native_reference_emitter_source)
        self.assertIn("TryBuildAssemblyBoundArrayBoxingReferenceArrayPlan", native_reference_emitter_source)
        self.assertIn('case "arrayBoxingReferenceArray":', native_reference_emitter_source)
        self.assertIn("BoxedValueTypeToken = CreateTypeTokenLiteral(metadataRegistration, boxedValueTypeSubjectId)", native_reference_emitter_source)
        self.assertIn("private static string CreateTypeTokenLiteral(", native_reference_emitter_source)
        self.assertIn("CreatePseudoTypeToken(subjectId)", native_reference_emitter_source)
        self.assertIn("BuildAssemblyBoundInterfaceDispatchMessageStub", native_reference_emitter_source)
        self.assertIn("TryBuildAssemblyBoundInterfaceDispatchMessagePlan", native_reference_emitter_source)
        self.assertIn('case "interfaceDispatchMessage":', native_reference_emitter_source)
        self.assertIn('"interface-runtime-helper"', native_reference_emitter_source)
        self.assertIn("TryBuildAssemblyBoundConsoleWriteLineStub", native_reference_emitter_source)
        self.assertIn("NestedExceptionThrowCatchFinallyMinimal", native_reference_emitter_source)
        self.assertIn("ExceptionThrowCatchFinallyMinimal", native_reference_emitter_source)
        self.assertIn("MarshalingUtf8ExportMinimal", native_reference_emitter_source)
        self.assertIn("ReflectionInteropClosureMinimal", native_reference_emitter_source)
        self.assertIn("InteropPInvokeDirectCallMinimal", native_reference_emitter_source)
        self.assertIn("StringConcatPairMethodSubjectId", native_reference_emitter_source)
        self.assertIn("StringConcatTripleMethodSubjectId", native_reference_emitter_source)
        self.assertIn("NormalizeStringConcatIcall", native_reference_emitter_source)
        self.assertIn("BuildAssemblyBoundArrayCopyReferenceArrayStub", native_reference_emitter_source)
        self.assertIn("TryBuildAssemblyBoundArrayCopyReferenceArrayPlan", native_reference_emitter_source)
        self.assertIn('case "arrayCopyReferenceArray":', native_reference_emitter_source)
        self.assertIn("GetOrAddRuntimeSkeletonArrayCopyReferenceArrayDescriptor(", native_reference_emitter_source)
        self.assertIn("BuildAssemblyBoundArrayReverseReferenceArrayStub", native_reference_emitter_source)
        self.assertIn("TryBuildAssemblyBoundArrayReverseReferenceArrayPlan", native_reference_emitter_source)
        self.assertIn('case "arrayReverseReferenceArray":', native_reference_emitter_source)
        self.assertIn("GetOrAddRuntimeSkeletonArrayReverseReferenceArrayDescriptor(", native_reference_emitter_source)
        self.assertIn("BuildAssemblyBoundArrayClearReferenceArrayStub", native_reference_emitter_source)
        self.assertIn("TryBuildAssemblyBoundArrayClearReferenceArrayPlan", native_reference_emitter_source)
        self.assertIn('case "arrayClearReferenceArray":', native_reference_emitter_source)
        self.assertIn("GetOrAddRuntimeSkeletonArrayClearReferenceArrayDescriptor(", native_reference_emitter_source)
        self.assertIn("SourceArrayLength", native_reference_emitter_source)
        self.assertIn("TargetArrayLength", native_reference_emitter_source)
        self.assertIn("SourceStoreIndex", native_reference_emitter_source)
        self.assertIn("SourceArrayIndex", native_reference_emitter_source)
        self.assertIn("TargetArrayIndex", native_reference_emitter_source)
        self.assertIn("TargetReadIndex", native_reference_emitter_source)
        self.assertIn("ArrayLength", native_reference_emitter_source)
        self.assertIn("ArrayStoreIndex", native_reference_emitter_source)
        self.assertIn("ReverseStartIndex", native_reference_emitter_source)
        self.assertIn("ReverseLength", native_reference_emitter_source)
        self.assertIn("ArrayReadIndex", native_reference_emitter_source)
        for required_fragment in [
            "public int? ClearStartIndex { get; init; }",
            "public int? ClearLength { get; init; }",
            "public string? StoredLiteral { get; init; }",
            "public int? StoredLiteralByteCount { get; init; }",
            "ClearStartIndex",
            "ClearLength",
            "StoredLiteral",
            "StoredLiteralByteCount",
        ]:
            self.assertTrue(
                required_fragment in native_reference_emitter_source or required_fragment in artifact_models_source,
                msg=f"missing array clear fragment: {required_fragment}",
            )
        self.assertNotIn(
            'RequireMethodContract(method, "static-method", "has-canonical-body");\n        RequireCapability(method, "requires-console-string-output");\n        RequireInstructionCount(method, instructions, 7);',
            native_reference_emitter_source.replace("\r\n", "\n"),
        )

    def test_runtime_skeleton_array_copy_reference_array_uses_runtime_core_helper(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        runtime_core_header_source = (
            REPO_ROOT
            / "src"
            / "native"
            / "runtime-core"
            / "runtime_core.h"
        ).read_text(encoding="utf-8")
        runtime_core_cpp_source = (
            REPO_ROOT
            / "src"
            / "native"
            / "runtime-core"
            / "runtime_core.cpp"
        ).read_text(encoding="utf-8")

        self.assertIn("BuildAssemblyBoundArrayCopyReferenceArrayStub(", native_reference_emitter_source)
        self.assertIn("ArrayCopyReferenceRange(", runtime_core_header_source)
        self.assertIn("ArrayCopyReferenceRange(", runtime_core_cpp_source)
        self.assertIn("CHAOS_IL2CPP_MEMMOVE(", runtime_core_cpp_source)
        self.assertIn("arrayCopyReferenceArrayDescriptors", native_reference_emitter_source)
        self.assertIn('subjectId.Contains("/System.Array::CopyTo:", StringComparison.Ordinal)', native_reference_emitter_source)
        self.assertIn('subjectId.Contains("/System.Array::ConstrainedCopy:", StringComparison.Ordinal)', native_reference_emitter_source)
        self.assertIn('subjectId.EndsWith("(System.Array,System.Int32)", StringComparison.Ordinal)', native_reference_emitter_source)
        self.assertIn('subjectId.EndsWith("(System.Array,System.Int64)", StringComparison.Ordinal)', native_reference_emitter_source)
        self.assertIn('subjectId.EndsWith("(System.Array,System.Array,System.Int64)", StringComparison.Ordinal)', native_reference_emitter_source)
        self.assertIn('subjectId.EndsWith("(System.Array,System.Int64,System.Array,System.Int64,System.Int64)", StringComparison.Ordinal)', native_reference_emitter_source)

    def test_runtime_skeleton_array_reverse_reference_array_uses_runtime_core_helper(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        runtime_core_header_source = (
            REPO_ROOT
            / "src"
            / "native"
            / "runtime-core"
            / "runtime_core.h"
        ).read_text(encoding="utf-8")
        runtime_core_cpp_source = (
            REPO_ROOT
            / "src"
            / "native"
            / "runtime-core"
            / "runtime_core.cpp"
        ).read_text(encoding="utf-8")

        self.assertIn("BuildAssemblyBoundArrayReverseReferenceArrayStub(", native_reference_emitter_source)
        self.assertIn("ArrayReverseReferenceRange(", runtime_core_header_source)
        self.assertIn("ArrayReverseReferenceRange(", runtime_core_cpp_source)
        self.assertIn("arrayReverseReferenceArrayDescriptors", native_reference_emitter_source)

    def test_runtime_skeleton_array_clear_reference_array_uses_runtime_core_helper(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        runtime_core_header_source = (
            REPO_ROOT
            / "src"
            / "native"
            / "runtime-core"
            / "runtime_core.h"
        ).read_text(encoding="utf-8")
        runtime_core_cpp_source = (
            REPO_ROOT
            / "src"
            / "native"
            / "runtime-core"
            / "runtime_core.cpp"
        ).read_text(encoding="utf-8")

        self.assertIn("BuildAssemblyBoundArrayClearReferenceArrayStub(", native_reference_emitter_source)
        self.assertIn("ArrayClearReferenceRange(", runtime_core_header_source)
        self.assertIn("ArrayClearReferenceRange(", runtime_core_cpp_source)
        self.assertIn("arrayClearReferenceArrayDescriptors", native_reference_emitter_source)
        self.assertIn('subjectId.EndsWith("(System.Array)", StringComparison.Ordinal)', native_reference_emitter_source)

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
