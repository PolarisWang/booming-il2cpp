from tests.unit.compatibility.full_assembly_closure_codegen_contracts_test_support import *


class TestFullAssemblyClosureCodegenContractsRuntimeTemplates(FullAssemblyClosureCodegenContractTestSupport):
    def test_runtime_skeleton_summary_prefers_numeric_method_dispatch_with_string_fallback(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        summary_template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.RuntimeSkeletonSummary.cpp.scriban"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "std::uint32_t method_id;",
            "FindMethodDispatchCatalogEntryByMethodId",
            "request->method_id",
            "DispatchAssemblySubject(",
        ]:
            self.assertIn(required_fragment, summary_template_source)

        for required_fragment in [
            "new RuntimeSkeletonMethodEmission(",
            "method.MethodId",
        ]:
            self.assertIn(required_fragment, emitter_source)

    def test_runtime_skeleton_page_uses_page_level_prebinding_and_generic_executors(self) -> None:
        page_template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.RuntimeSkeletonPage.cpp.scriban"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            '#include "runtime_core.h"',
            "#include <atomic>",
            "external_method_stub_declarations",
            "struct RuntimeSkeletonFieldBindingDescriptor",
            "struct RuntimeSkeletonFieldBindingState",
            "struct RuntimeSkeletonTypeDescriptor",
            "struct RuntimeSkeletonTypeState",
            "kPageFieldBindingDescriptors[]",
            "gPageFieldBindingStates[",
            "kPageTypeDescriptors[]",
            "gPageTypeStates[",
            "ResolveRuntimeSkeletonFieldBinding(",
            "ResolveRuntimeSkeletonType(",
            "std::atomic<void*>",
            "ExecuteRuntimeSkeletonConstructorFieldSetter(",
            "ExecuteRuntimeSkeletonFieldGetterStringReturn(",
            "ExecuteRuntimeSkeletonFieldBackedStringReturn(",
            "ExecuteRuntimeSkeletonStaticCallCtorGetter(",
            "ExecuteRuntimeSkeletonConstructorThenInstanceCall(",
            "ExecuteRuntimeSkeletonDelegateClosedTargetRelay(",
            "ExecuteRuntimeSkeletonArrayBoxingReferenceArray(",
            "ExecuteRuntimeSkeletonArrayReverseReferenceArray(",
            "ExecuteRuntimeSkeletonArrayCopyReferenceArray(",
            "ExecuteRuntimeSkeletonArrayClearReferenceArray(",
            "struct RuntimeSkeletonArrayReverseReferenceArrayDescriptor",
            "kPageArrayReverseReferenceArrayDescriptors[]",
            "struct RuntimeSkeletonArrayCopyReferenceArrayDescriptor",
            "kPageArrayCopyReferenceArrayDescriptors[]",
            "struct RuntimeSkeletonArrayClearReferenceArrayDescriptor",
            "kPageArrayClearReferenceArrayDescriptors[]",
        ]:
            self.assertIn(required_fragment, page_template_source)

    def test_runtime_skeleton_static_byte_forwarder_has_template(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        template_source = RUNTIME_SKELETON_STATIC_BYTE_FORWARDER_TEMPLATE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildAssemblyBoundStaticPrimitiveReturnForwarderStub",
            "TryBuildAssemblyBoundStaticCharReturnForwarderStub",
            "TryResolveRuntimeSkeletonPrimitiveConvertOutputCppType",
            "input_cpp_type",
            "output_cpp_type",
            "GetRuntimeSkeletonStaticByteForwarderStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        self.assertIn("RuntimeSkeletonStaticByteForwarderStubTemplateRelativePath", catalog_source)
        self.assertIn("{{ input_cpp_type }} value;", template_source)
        self.assertIn("{{ output_cpp_type }}* return_value;", template_source)
        self.assertIn("return {{ target_stub_name }}(", template_source)

    def test_runtime_skeleton_field_families_lower_to_descriptor_driven_wrappers(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        page_template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.RuntimeSkeletonPage.cpp.scriban"
        ).read_text(encoding="utf-8")
        constructor_field_setter_template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.RuntimeSkeleton.ConstructorFieldSetterStub.cpp.scriban"
        ).read_text(encoding="utf-8")
        field_getter_template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.RuntimeSkeleton.FieldGetterStringReturnStub.cpp.scriban"
        ).read_text(encoding="utf-8")
        field_backed_template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.RuntimeSkeleton.FieldBackedStringReturnStub.cpp.scriban"
        ).read_text(encoding="utf-8")
        static_call_ctor_getter_template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.RuntimeSkeleton.StaticCallCtorGetterStub.cpp.scriban"
        ).read_text(encoding="utf-8")
        constructor_then_instance_call_template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.RuntimeSkeleton.ConstructorThenInstanceCallStub.cpp.scriban"
        ).read_text(encoding="utf-8")
        delegate_closed_target_relay_template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.RuntimeSkeleton.DelegateClosedTargetRelayStub.cpp.scriban"
        ).read_text(encoding="utf-8")
        array_boxing_reference_array_template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.RuntimeSkeleton.ArrayBoxingReferenceArrayStub.cpp.scriban"
        ).read_text(encoding="utf-8")
        array_reverse_reference_array_template_path = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.RuntimeSkeleton.ArrayReverseReferenceArrayStub.cpp.scriban"
        )
        self.assertTrue(
            array_reverse_reference_array_template_path.is_file(),
            msg=f"missing array reverse runtime skeleton template: {array_reverse_reference_array_template_path}",
        )
        array_reverse_reference_array_template_source = array_reverse_reference_array_template_path.read_text(encoding="utf-8")
        array_copy_reference_array_template_path = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.RuntimeSkeleton.ArrayCopyReferenceArrayStub.cpp.scriban"
        )
        self.assertTrue(
            array_copy_reference_array_template_path.is_file(),
            msg=f"missing array copy runtime skeleton template: {array_copy_reference_array_template_path}",
        )
        array_copy_reference_array_template_source = array_copy_reference_array_template_path.read_text(encoding="utf-8")
        array_clear_reference_array_template_path = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.RuntimeSkeleton.ArrayClearReferenceArrayStub.cpp.scriban"
        )
        self.assertTrue(
            array_clear_reference_array_template_path.is_file(),
            msg=f"missing array clear runtime skeleton template: {array_clear_reference_array_template_path}",
        )
        array_clear_reference_array_template_source = array_clear_reference_array_template_path.read_text(encoding="utf-8")
        array_copy_reference_array_translation_unit_template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.ArrayCopyReferenceArray.cpp.scriban"
        ).read_text(encoding="utf-8")
        array_reverse_reference_array_translation_unit_template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.ArrayReverseReferenceArray.cpp.scriban"
        ).read_text(encoding="utf-8")
        array_clear_reference_array_translation_unit_template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.ArrayClearReferenceArray.cpp.scriban"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "GetOrAddRuntimeSkeletonFieldBindingDescriptor(",
            "GetOrAddRuntimeSkeletonTypeDescriptor(",
            "GetOrAddRuntimeSkeletonConstructorFieldSetterDescriptor(",
            "GetOrAddRuntimeSkeletonFieldGetterStringReturnDescriptor(",
            "GetOrAddRuntimeSkeletonFieldBackedStringReturnDescriptor(",
            "GetOrAddRuntimeSkeletonStaticCallCtorGetterDescriptor(",
            "GetOrAddRuntimeSkeletonConstructorThenInstanceCallDescriptor(",
            "GetOrAddRuntimeSkeletonDelegateClosedTargetRelayDescriptor(",
            "GetOrAddRuntimeSkeletonArrayBoxingReferenceArrayDescriptor(",
            "GetOrAddRuntimeSkeletonArrayReverseReferenceArrayDescriptor(",
            "GetOrAddRuntimeSkeletonArrayCopyReferenceArrayDescriptor(",
            "GetOrAddRuntimeSkeletonArrayClearReferenceArrayDescriptor(",
        ]:
            self.assertIn(required_fragment, emitter_source)

        for template_source, expected_executor in [
            (constructor_field_setter_template_source, "ExecuteRuntimeSkeletonConstructorFieldSetter"),
            (field_getter_template_source, "ExecuteRuntimeSkeletonFieldGetterStringReturn"),
            (field_backed_template_source, "ExecuteRuntimeSkeletonFieldBackedStringReturn"),
            (static_call_ctor_getter_template_source, "ExecuteRuntimeSkeletonStaticCallCtorGetter"),
            (constructor_then_instance_call_template_source, "ExecuteRuntimeSkeletonConstructorThenInstanceCall"),
            (delegate_closed_target_relay_template_source, "ExecuteRuntimeSkeletonDelegateClosedTargetRelay"),
            (array_boxing_reference_array_template_source, "ExecuteRuntimeSkeletonArrayBoxingReferenceArray"),
            (array_reverse_reference_array_template_source, "ExecuteRuntimeSkeletonArrayReverseReferenceArray"),
            (array_copy_reference_array_template_source, "ExecuteRuntimeSkeletonArrayCopyReferenceArray"),
            (array_clear_reference_array_template_source, "ExecuteRuntimeSkeletonArrayClearReferenceArray"),
        ]:
            self.assertIn("descriptor_index", template_source)
            self.assertIn(expected_executor, template_source)
            self.assertNotIn("static RuntimeSkeletonFieldBindingCacheEntry field_binding", template_source)

        self.assertNotIn(
            "static RuntimeSkeletonTypeCacheEntry boxed_value_type_cache",
            array_boxing_reference_array_template_source,
        )
        self.assertNotIn(
            "static RuntimeSkeletonFieldBindingCacheEntry field_binding",
            array_copy_reference_array_template_source,
        )
        self.assertNotIn(
            "static RuntimeSkeletonFieldBindingCacheEntry field_binding",
            array_reverse_reference_array_template_source,
        )
        self.assertNotIn(
            "static RuntimeSkeletonFieldBindingCacheEntry field_binding",
            array_clear_reference_array_template_source,
        )
        for required_fragment in [
            "array_length",
            "array_store_index",
            "reverse_start_index",
            "reverse_length",
            "array_read_index",
        ]:
            self.assertIn(required_fragment, array_reverse_reference_array_translation_unit_template_source)
            self.assertIn(required_fragment, page_template_source)
        for required_fragment in [
            "source_array_length",
            "target_array_length",
            "source_store_index",
            "source_array_index",
            "target_array_index",
            "target_read_index",
            "copy_length",
        ]:
            self.assertIn(required_fragment, array_copy_reference_array_translation_unit_template_source)
            self.assertIn(required_fragment, page_template_source)
        for required_fragment in [
            "array_length",
            "array_store_index",
            "clear_start_index",
            "clear_length",
            "array_read_index",
            "stored_literal",
        ]:
            self.assertIn(required_fragment, array_clear_reference_array_translation_unit_template_source)
            self.assertIn(required_fragment, page_template_source)

    def test_runtime_skeleton_array_copy_reference_array_has_catalog_template(self) -> None:
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "ManagedArraysReverseReferenceArrayMinimal",
            "ArrayReverseReferenceArrayGeneratedTranslationUnitTemplateRelativePath",
            "RuntimeSkeletonArrayReverseReferenceArrayStubTemplateRelativePath",
            '["arrayReverseReferenceArray"] = ArrayReverseReferenceArrayGeneratedTranslationUnitTemplateRelativePath',
            "GetRuntimeSkeletonArrayReverseReferenceArrayStubTemplate()",
            "ManagedArraysCopyReferenceArrayMinimal",
            "ArrayCopyReferenceArrayGeneratedTranslationUnitTemplateRelativePath",
            "RuntimeSkeletonArrayCopyReferenceArrayStubTemplateRelativePath",
            '["arrayCopyReferenceArray"] = ArrayCopyReferenceArrayGeneratedTranslationUnitTemplateRelativePath',
            "GetRuntimeSkeletonArrayCopyReferenceArrayStubTemplate()",
            "ManagedArraysClearReferenceArrayMinimal",
            "ArrayClearReferenceArrayGeneratedTranslationUnitTemplateRelativePath",
            "RuntimeSkeletonArrayClearReferenceArrayStubTemplateRelativePath",
            '["arrayClearReferenceArray"] = ArrayClearReferenceArrayGeneratedTranslationUnitTemplateRelativePath',
            "GetRuntimeSkeletonArrayClearReferenceArrayStubTemplate()",
        ]:
            self.assertIn(required_fragment, catalog_source)

    def test_runtime_skeleton_async_task_factory_helper_uses_generic_family_template(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        template_path = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.RuntimeSkeleton.AsyncTaskFactoryStub.cpp.scriban"
        )

        self.assertTrue(template_path.is_file(), msg=f"missing generic async task factory template: {template_path}")
        template_source = template_path.read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildAssemblyBoundAsyncTaskFactoryStub",
            "TryResolveAsyncTaskFactoryMethodShape",
            "TryResolveAsyncManagedTypeShape",
            "GetRuntimeSkeletonAsyncTaskFactoryStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        self.assertIn(
            "RuntimeSkeletonAsyncTaskFactoryStubTemplateRelativePath",
            catalog_source,
        )

        for forbidden_fragment in [
            "TryBuildAssemblyBoundAsyncTaskIntFactoryStub",
            "TryBuildAssemblyBoundAsyncTaskBoolFactoryStub",
            "TryBuildAssemblyBoundAsyncTaskStringFactoryStub",
            "GetRuntimeSkeletonAsyncTaskIntFactoryStubTemplate",
            "GetRuntimeSkeletonAsyncTaskBoolFactoryStubTemplate",
            "GetRuntimeSkeletonAsyncTaskStringFactoryStubTemplate",
            "RuntimeSkeletonAsyncTaskIntFactoryStubTemplateRelativePath",
            "RuntimeSkeletonAsyncTaskBoolFactoryStubTemplateRelativePath",
            "RuntimeSkeletonAsyncTaskStringFactoryStubTemplateRelativePath",
        ]:
            self.assertNotIn(forbidden_fragment, native_reference_emitter_source)
            self.assertNotIn(forbidden_fragment, catalog_source)

        for forbidden_fragment in [
            "AsyncTaskIntResultShape",
            "AsyncTaskBoolResultShape",
            "AsyncTaskStringResultShape",
            "SupportedAsyncTaskResultShapes",
        ]:
            self.assertNotIn(forbidden_fragment, native_reference_emitter_source)

        for required_fragment in [
            "_ManagedArgs",
            "managed_result_type",
            "arg0_cpp_type",
            "arg1_cpp_type",
            "resolve_method_by_token",
            "method_invoke",
            "target_method_token",
            "return_value",
            "arg0",
            "arg1",
            "argc",
            "out_exception",
        ]:
            self.assertIn(required_fragment, template_source)

        self.assertNotIn("factory_managed_arg_cpp_type", template_source)

    def test_runtime_skeleton_async_get_result_helper_uses_generic_family_template(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        template_path = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.RuntimeSkeleton.AsyncGetResultStub.cpp.scriban"
        )

        self.assertTrue(template_path.is_file(), msg=f"missing generic async get-result template: {template_path}")
        template_source = template_path.read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildAssemblyBoundAsyncGetResultStub",
            "TryResolveAsyncGetResultWrapperShape",
            "TryResolveAsyncManagedTypeShape",
            "GetRuntimeSkeletonAsyncGetResultStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        self.assertIn(
            "RuntimeSkeletonAsyncGetResultStubTemplateRelativePath",
            catalog_source,
        )

        for forbidden_fragment in [
            "TryBuildAssemblyBoundAsyncGetResultIntStub",
            "TryBuildAssemblyBoundAsyncGetResultBoolStub",
            "TryBuildAssemblyBoundAsyncGetResultStringStub",
            "GetRuntimeSkeletonAsyncGetResultIntStubTemplate",
            "GetRuntimeSkeletonAsyncGetResultBoolStubTemplate",
            "GetRuntimeSkeletonAsyncGetResultStringStubTemplate",
            "RuntimeSkeletonAsyncGetResultIntStubTemplateRelativePath",
            "RuntimeSkeletonAsyncGetResultBoolStubTemplateRelativePath",
            "RuntimeSkeletonAsyncGetResultStringStubTemplateRelativePath",
        ]:
            self.assertNotIn(forbidden_fragment, native_reference_emitter_source)
            self.assertNotIn(forbidden_fragment, catalog_source)

        for forbidden_fragment in [
            "AsyncTaskIntResultShape",
            "AsyncTaskBoolResultShape",
            "AsyncTaskStringResultShape",
            "SupportedAsyncTaskResultShapes",
        ]:
            self.assertNotIn(forbidden_fragment, native_reference_emitter_source)

        for required_fragment in [
            "managed_result_type",
            "resolve_method_by_token",
            "method_invoke",
            "target_method_token",
            "return_value",
            "out_exception",
        ]:
            self.assertIn(required_fragment, template_source)

    def test_runtime_skeleton_async_state_machine_no_op_helper_has_template(self) -> None:
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
            / "NativeReferenceProof.RuntimeSkeleton.AsyncStateMachineNoOpStub.cpp.scriban"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildAssemblyBoundAsyncStateMachineNoOpStub",
            "GetRuntimeSkeletonAsyncStateMachineNoOpStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        self.assertIn(
            "RuntimeSkeletonAsyncStateMachineNoOpStubTemplateRelativePath",
            catalog_source,
        )

        for required_fragment in [
            "managed_args",
            "CHAOS_BRIDGE_STATUS_OK",
        ]:
            self.assertIn(required_fragment, template_source)

    def test_runtime_skeleton_static_void_object_sink_no_op_helper_has_template(self) -> None:
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
            / "NativeReferenceProof.RuntimeSkeleton.StaticVoidObjectSinkNoOpStub.cpp.scriban"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildAssemblyBoundStaticVoidObjectSinkNoOpStub",
            "GetRuntimeSkeletonStaticVoidObjectSinkNoOpStubTemplate",
            'string.Equals(method.Parameters[0].Type, "System.Object", StringComparison.Ordinal)',
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        self.assertIn(
            "RuntimeSkeletonStaticVoidObjectSinkNoOpStubTemplateRelativePath",
            catalog_source,
        )

        for required_fragment in [
            "managed_args",
            "CHAOS_BRIDGE_STATUS_OK",
        ]:
            self.assertIn(required_fragment, template_source)

    def test_runtime_skeleton_static_bool_forwarder_reuses_forwarder_stub(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildAssemblyBoundStaticBoolForwarderStub",
            'GetMethodReturnType(method.SubjectId), "System.Boolean"',
            "GetRuntimeSkeletonStaticIntForwarderStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

    def test_runtime_skeleton_static_bool_identity_forwarder_has_dedicated_template(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        template_source = RUNTIME_SKELETON_STATIC_BOOL_IDENTITY_FORWARDER_TEMPLATE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildAssemblyBoundStaticBoolIdentityForwarderStub",
            "ValidateSingleArgumentForwarderShape",
            'string.Equals(method.Parameters[0].Type, "System.Boolean", StringComparison.Ordinal)',
            "GetRuntimeSkeletonStaticBoolIdentityForwarderStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        self.assertIn("RuntimeSkeletonStaticBoolIdentityForwarderStubTemplateRelativePath", catalog_source)
        self.assertIn("bool value;", template_source)
        self.assertIn("bool* return_value;", template_source)
        self.assertIn("*request->return_value = request->value;", template_source)

    def test_runtime_skeleton_static_bool_producer_forwarder_calls_both_stubs(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        template_source = RUNTIME_SKELETON_STATIC_BOOL_PRODUCER_FORWARDER_TEMPLATE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildAssemblyBoundStaticBoolProducerForwarderStub",
            'GetMethodReturnType(method.SubjectId), "System.Boolean"',
            "methodStubNamesBySubjectId",
            "GetRuntimeSkeletonStaticBoolProducerForwarderStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        self.assertIn("RuntimeSkeletonStaticBoolProducerForwarderStubTemplateRelativePath", catalog_source)
        self.assertIn("bool produced_value = false;", template_source)
        self.assertIn("bool forwarded_value = false;", template_source)
        self.assertIn("{{ producer_stub_name }}(", template_source)
        self.assertIn("{{ forwarder_stub_name }}(", template_source)

    def test_runtime_skeleton_static_string_return_forwarder_calls_target_stub(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        template_source = RUNTIME_SKELETON_STATIC_STRING_RETURN_FORWARDER_TEMPLATE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildAssemblyBoundStaticStringReturnForwarderStub",
            'GetMethodReturnType(method.SubjectId), "System.String"',
            "methodStubNamesBySubjectId",
            "GetRuntimeSkeletonStaticStringReturnForwarderStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        self.assertIn("RuntimeSkeletonStaticStringReturnForwarderStubTemplateRelativePath", catalog_source)
        self.assertIn("{{ target_stub_name }}(", template_source)
        self.assertIn("return {{ target_stub_name }}(", template_source)
        self.assertIn("(void)managed_args;", template_source)

    def test_runtime_skeleton_static_primitive_convert_helper_has_template(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        template_source = RUNTIME_SKELETON_STATIC_PRIMITIVE_CONVERT_TEMPLATE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildAssemblyBoundStaticPrimitiveConvertStub",
            "TryResolveRuntimeSkeletonPrimitiveConvertShape",
            "GetRuntimeSkeletonStaticPrimitiveConvertStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        self.assertIn("RuntimeSkeletonStaticPrimitiveConvertStubTemplateRelativePath", catalog_source)
        self.assertIn("{{ input_cpp_type }} value;", template_source)
        self.assertIn("{{ output_cpp_type }}* return_value;", template_source)
        self.assertIn("{{ converted_value_expression }}", template_source)
        self.assertIn("*request->return_value = static_cast<{{ output_cpp_type }}>(", template_source)

    def test_runtime_skeleton_static_exception_throw_literal_helper_has_template(self) -> None:
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
            / "NativeReferenceProof.RuntimeSkeleton.StaticExceptionThrowLiteralStub.cpp.scriban"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildAssemblyBoundStaticExceptionThrowLiteralStub",
            "GetRuntimeSkeletonStaticExceptionThrowLiteralStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        self.assertIn(
            "RuntimeSkeletonStaticExceptionThrowLiteralStubTemplateRelativePath",
            catalog_source,
        )

        for required_fragment in [
            "assembly_name_literal",
            "resolve_type_by_token",
            "exception_type_token",
            "object_new",
            "string_new_utf8",
            "raise_managed_exception",
            "exception_literal",
            "exception_literal_byte_count",
        ]:
            self.assertIn(required_fragment, template_source)

    def test_runtime_skeleton_static_exception_throw_string_producer_helper_has_template(self) -> None:
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
            / "NativeReferenceProof.RuntimeSkeleton.StaticExceptionThrowStringProducerStub.cpp.scriban"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildAssemblyBoundStaticExceptionThrowStringProducerStub",
            "GetRuntimeSkeletonStaticExceptionThrowStringProducerStubTemplate",
            "::.ctor:System.Void(System.String)",
            "methodStubNamesBySubjectId",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        self.assertIn(
            "RuntimeSkeletonStaticExceptionThrowStringProducerStubTemplateRelativePath",
            catalog_source,
        )

        for required_fragment in [
            "_ProducerArgs",
            "producer_stub_name",
            "produced_message",
            "object_new",
            "raise_managed_exception",
        ]:
            self.assertIn(required_fragment, template_source)

    def test_runtime_skeleton_static_boxed_iconvertible_char_invalid_cast_helper_has_template(self) -> None:
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
            / "NativeReferenceProof.RuntimeSkeleton.StaticBoxedIConvertibleCharInvalidCastStub.cpp.scriban"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildAssemblyBoundStaticBoxedIConvertibleCharInvalidCastStub",
            "TryResolveRuntimeSkeletonBoxedIConvertibleCharInvalidCastShape",
            "GetRuntimeSkeletonStaticBoxedIConvertibleCharInvalidCastStubTemplate",
            "System.IConvertible::ToChar:System.Char(System.IFormatProvider)",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        self.assertIn(
            "RuntimeSkeletonStaticBoxedIConvertibleCharInvalidCastStubTemplateRelativePath",
            catalog_source,
        )

        for required_fragment in [
            "input_cpp_type",
            "output_cpp_type",
            "exception_type_token",
            "source_type_name_literal",
            "target_type_name_literal",
            "string_new_utf8",
            "raise_managed_exception",
        ]:
            self.assertIn(required_fragment, template_source)

    def test_runtime_skeleton_static_string_char_provider_helper_has_template(self) -> None:
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
            / "NativeReferenceProof.RuntimeSkeleton.StaticStringCharProviderStub.cpp.scriban"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildAssemblyBoundStaticStringCharProviderStub",
            "TryResolveRuntimeSkeletonStringCharProviderShape",
            "GetRuntimeSkeletonStaticStringCharProviderStubTemplate",
            "System.Convert::ToChar:System.Char(System.String,System.IFormatProvider)",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        self.assertIn(
            "RuntimeSkeletonStaticStringCharProviderStubTemplateRelativePath",
            catalog_source,
        )

        for required_fragment in [
            "void* arg0",
            "void* arg1",
            "target_method_token",
            "method_invoke",
            "std::uint16_t return_value",
        ]:
            self.assertIn(required_fragment, template_source)

    def test_runtime_skeleton_static_object_char_provider_helper_has_template(self) -> None:
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
            / "NativeReferenceProof.RuntimeSkeleton.StaticObjectCharProviderStub.cpp.scriban"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildAssemblyBoundStaticObjectCharProviderStub",
            "TryResolveRuntimeSkeletonObjectCharProviderShape",
            "GetRuntimeSkeletonStaticObjectCharProviderStubTemplate",
            "System.IConvertible::ToChar:System.Char(System.IFormatProvider)",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        self.assertIn(
            "RuntimeSkeletonStaticObjectCharProviderStubTemplateRelativePath",
            catalog_source,
        )

        for required_fragment in [
            "void* arg0",
            "void* arg1",
            "target_method_token",
            "method_invoke",
            "request->arg0 == nullptr",
            "std::uint16_t return_value",
        ]:
            self.assertIn(required_fragment, template_source)

    def test_runtime_skeleton_convert_to_string_helpers_have_templates(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        passthrough_template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.RuntimeSkeleton.StaticStringProviderPassthroughStub.cpp.scriban"
        ).read_text(encoding="utf-8")
        value_type_template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.RuntimeSkeleton.StaticBoxedValueTypeStringInstanceCallStub.cpp.scriban"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildRuntimeSkeletonConvertLikeStringProviderPassthroughHandler",
            "TryBuildAssemblyBoundStaticStringProviderPassthroughStub",
            "TryResolveRuntimeSkeletonStringProviderPassthroughShape",
            "TryBuildRuntimeSkeletonConvertLikeBoxedValueTypeStringInstanceCallHandler",
            "TryBuildAssemblyBoundStaticBoxedValueTypeStringInstanceCallStub",
            "TryResolveRuntimeSkeletonBoxedValueTypeStringInstanceCallShape",
            "GetRuntimeSkeletonStaticStringProviderPassthroughStubTemplate",
            "GetRuntimeSkeletonStaticBoxedValueTypeStringInstanceCallStubTemplate",
            "System.Convert::ToString:System.String(System.String,System.IFormatProvider)",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        for required_fragment in [
            "RuntimeSkeletonStaticStringProviderPassthroughStubTemplateRelativePath",
            "RuntimeSkeletonStaticBoxedValueTypeStringInstanceCallStubTemplateRelativePath",
        ]:
            self.assertIn(required_fragment, catalog_source)

        for required_fragment in [
            "void* arg0",
            "void* arg1",
            "void** return_value",
            "*request->return_value = request->arg0",
        ]:
            self.assertIn(required_fragment, passthrough_template_source)

        for required_fragment in [
            "{{ input_cpp_type }} value;",
            "void* arg1",
            "void** return_value",
            "bridge->box_value",
            "target_method_token",
            "method_invoke",
        ]:
            self.assertIn(required_fragment, value_type_template_source)

    def test_runtime_skeleton_string_managed_invoke_helper_has_template(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofEmitter.StringManagedInvokeFamily.cs"
        ).read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        template_source = RUNTIME_SKELETON_STRING_MANAGED_INVOKE_TEMPLATE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildRuntimeSkeletonStringManagedInvokeFamilyHandler",
            "TryBuildRuntimeSkeletonStringManagedInvokeHandler",
            "TryBuildAssemblyBoundStringManagedInvokeStub",
            "TryBuildAssemblyBoundStringManagedInvokeCore",
            "RuntimeSkeletonStringManagedInvokeAbi.TryCreate(",
            "GetRuntimeSkeletonStringManagedInvokeStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source + "\n" + split_source)

        self.assertIn("RuntimeSkeletonStringManagedInvokeStubTemplateRelativePath", catalog_source)

        for required_fragment in [
            "target_method_token",
            "target_assembly_name_literal",
            "method_invoke",
            "arg_storage_size",
            "return_value_size",
        ]:
            self.assertIn(required_fragment, template_source)

    def test_runtime_skeleton_unsafe_managed_invoke_helper_has_template(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofEmitter.UnsafeManagedInvokeFamily.cs"
        ).read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        template_source = RUNTIME_SKELETON_UNSAFE_MANAGED_INVOKE_TEMPLATE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildRuntimeSkeletonUnsafeManagedInvokeFamilyHandler",
            "TryBuildRuntimeSkeletonUnsafeManagedInvokeHandler",
            "TryBuildAssemblyBoundUnsafeManagedInvokeStub",
            "TryBuildAssemblyBoundUnsafeManagedInvokeCore",
            "RuntimeSkeletonUnsafeManagedInvokeAbi.TryCreate(",
            "GetRuntimeSkeletonUnsafeManagedInvokeStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source + "\n" + split_source)

        self.assertIn("RuntimeSkeletonUnsafeManagedInvokeStubTemplateRelativePath", catalog_source)

        for required_fragment in [
            "target_method_token",
            "target_assembly_name_literal",
            "method_invoke",
            "arg_storage_size",
            "return_value_size",
            "return_value_is_indirect",
        ]:
            self.assertIn(required_fragment, template_source)

    def test_runtime_skeleton_memory_extensions_managed_invoke_helper_has_template(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofEmitter.MemoryExtensionsManagedInvokeFamily.cs"
        ).read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        template_source = RUNTIME_SKELETON_MEMORY_EXTENSIONS_MANAGED_INVOKE_TEMPLATE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildRuntimeSkeletonMemoryExtensionsManagedInvokeFamilyHandler",
            "TryBuildRuntimeSkeletonMemoryExtensionsManagedInvokeHandler",
            "TryBuildAssemblyBoundMemoryExtensionsManagedInvokeStub",
            "TryBuildAssemblyBoundMemoryExtensionsManagedInvokeCore",
            "RuntimeSkeletonMemoryExtensionsManagedInvokeAbi.TryCreate(",
            "GetRuntimeSkeletonMemoryExtensionsManagedInvokeStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source + "\n" + split_source)

        self.assertIn("RuntimeSkeletonMemoryExtensionsManagedInvokeStubTemplateRelativePath", catalog_source)

        for required_fragment in [
            "target_method_token",
            "target_assembly_name_literal",
            "method_invoke",
            "arg_storage_size",
            "return_value_size",
            "return_value_argument",
            "return_value_declaration",
        ]:
            self.assertIn(required_fragment, template_source)

    def test_runtime_skeleton_spanhelpers_kernel_helper_has_template(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofEmitter.SpanHelpersKernelFamily.cs"
        ).read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        template_source = RUNTIME_SKELETON_SPANHELPERS_KERNEL_TEMPLATE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildRuntimeSkeletonSpanHelpersKernelFamilyHandler",
            "TryBuildRuntimeSkeletonSpanHelpersKernelHandler",
            "TryBuildAssemblyBoundSpanHelpersKernelStub",
            "TryBuildAssemblyBoundSpanHelpersKernelCore",
            "RuntimeSkeletonSpanHelpersKernelAbi.TryCreate(",
            "GetRuntimeSkeletonSpanHelpersKernelStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source + "\n" + split_source)

        self.assertIn("RuntimeSkeletonSpanHelpersKernelStubTemplateRelativePath", catalog_source)

        for required_fragment in [
            "target_method_token",
            "target_assembly_name_literal",
            "method_invoke",
            "arg_storage_size",
            "return_value_size",
            "return_value_argument",
            "return_value_declaration",
        ]:
            self.assertIn(required_fragment, template_source)

    def test_runtime_skeleton_array_and_memorymarshal_helper_has_template(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofEmitter.ArrayAndMemoryMarshalFamily.cs"
        ).read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        template_source = RUNTIME_SKELETON_ARRAY_AND_MEMORYMARSHAL_TEMPLATE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildRuntimeSkeletonArrayAndMemoryMarshalFamilyHandler",
            "TryBuildRuntimeSkeletonArrayAndMemoryMarshalHandler",
            "TryBuildAssemblyBoundArrayAndMemoryMarshalStub",
            "TryBuildAssemblyBoundArrayAndMemoryMarshalCore",
            "RuntimeSkeletonArrayAndMemoryMarshalAbi.TryCreate(",
            "GetRuntimeSkeletonArrayAndMemoryMarshalStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source + "\n" + split_source)

        self.assertIn("RuntimeSkeletonArrayAndMemoryMarshalStubTemplateRelativePath", catalog_source)

        for required_fragment in [
            "target_method_token",
            "target_assembly_name_literal",
            "this_argument_expression",
            "method_invoke",
            "arg_storage_size",
            "return_value_size",
            "return_value_is_indirect",
        ]:
            self.assertIn(required_fragment, template_source)

    def test_runtime_skeleton_half_managed_invoke_helper_has_template(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofEmitter.HalfManagedInvokeFamily.cs"
        ).read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        template_source = RUNTIME_SKELETON_HALF_MANAGED_INVOKE_TEMPLATE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildRuntimeSkeletonHalfManagedInvokeFamilyHandler",
            "TryBuildRuntimeSkeletonHalfManagedInvokeHandler",
            "TryBuildAssemblyBoundHalfManagedInvokeStub",
            "TryBuildAssemblyBoundHalfManagedInvokeCore",
            "RuntimeSkeletonHalfManagedInvokeAbi.TryCreate(",
            "GetRuntimeSkeletonHalfManagedInvokeStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source + "\n" + split_source)

        self.assertIn("RuntimeSkeletonHalfManagedInvokeStubTemplateRelativePath", catalog_source)

        for required_fragment in [
            "target_method_token",
            "target_assembly_name_literal",
            "this_argument_expression",
            "method_invoke",
            "arg_storage_size",
            "return_value_size",
            "return_value_is_indirect",
        ]:
            self.assertIn(required_fragment, template_source)

    def test_runtime_skeleton_number_managed_invoke_helper_has_template(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofEmitter.NumberManagedInvokeFamily.cs"
        ).read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        template_source = RUNTIME_SKELETON_NUMBER_MANAGED_INVOKE_TEMPLATE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildRuntimeSkeletonNumberManagedInvokeFamilyHandler",
            "TryBuildRuntimeSkeletonNumberManagedInvokeHandler",
            "TryBuildAssemblyBoundNumberManagedInvokeStub",
            "TryBuildAssemblyBoundNumberManagedInvokeCore",
            "RuntimeSkeletonNumberManagedInvokeAbi.TryCreate(",
            "GetRuntimeSkeletonNumberManagedInvokeStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source + "\n" + split_source)

        self.assertIn("RuntimeSkeletonNumberManagedInvokeStubTemplateRelativePath", catalog_source)

        for required_fragment in [
            "target_method_token",
            "target_assembly_name_literal",
            "method_invoke",
            "arg_storage_size",
            "return_value_size",
            "return_value_argument",
            "return_value_declaration",
        ]:
            self.assertIn(required_fragment, template_source)

    def test_runtime_skeleton_decimal_managed_invoke_helper_has_template(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofEmitter.DecimalManagedInvokeFamily.cs"
        ).read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        template_source = RUNTIME_SKELETON_DECIMAL_MANAGED_INVOKE_TEMPLATE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildRuntimeSkeletonDecimalManagedInvokeFamilyHandler",
            "TryBuildRuntimeSkeletonDecimalManagedInvokeHandler",
            "TryBuildAssemblyBoundDecimalManagedInvokeStub",
            "TryBuildAssemblyBoundDecimalManagedInvokeCore",
            "RuntimeSkeletonDecimalManagedInvokeAbi.TryCreate(",
            "GetRuntimeSkeletonDecimalManagedInvokeStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source + "\n" + split_source)

        self.assertIn("RuntimeSkeletonDecimalManagedInvokeStubTemplateRelativePath", catalog_source)

        for required_fragment in [
            "target_method_token",
            "target_assembly_name_literal",
            "this_argument_expression",
            "method_invoke",
            "arg_storage_size",
            "return_value_size",
            "return_value_is_indirect",
        ]:
            self.assertIn(required_fragment, template_source)

    def test_runtime_skeleton_char_managed_invoke_helper_has_template(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofEmitter.CharManagedInvokeFamily.cs"
        ).read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        template_source = RUNTIME_SKELETON_CHAR_MANAGED_INVOKE_TEMPLATE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildRuntimeSkeletonCharManagedInvokeFamilyHandler",
            "TryBuildRuntimeSkeletonCharManagedInvokeHandler",
            "TryBuildAssemblyBoundCharManagedInvokeStub",
            "TryBuildAssemblyBoundCharManagedInvokeCore",
            "RuntimeSkeletonCharManagedInvokeAbi.TryCreate(",
            "GetRuntimeSkeletonCharManagedInvokeStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source + "\n" + split_source)

        self.assertIn("RuntimeSkeletonCharManagedInvokeStubTemplateRelativePath", catalog_source)

        for required_fragment in [
            "target_method_token",
            "target_assembly_name_literal",
            "this_argument_expression",
            "method_invoke",
            "arg_storage_size",
            "return_value_size",
            "return_value_is_indirect",
        ]:
            self.assertIn(required_fragment, template_source)

    def test_runtime_skeleton_companion_managed_invoke_helper_has_template(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofEmitter.CompanionManagedInvokeFamily.cs"
        ).read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        template_source = RUNTIME_SKELETON_COMPANION_MANAGED_INVOKE_TEMPLATE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildRuntimeSkeletonCompanionManagedInvokeFamilyHandler",
            "TryBuildRuntimeSkeletonCompanionManagedInvokeHandler",
            "TryBuildAssemblyBoundCompanionManagedInvokeStub",
            "TryBuildAssemblyBoundCompanionManagedInvokeCore",
            "RuntimeSkeletonCompanionManagedInvokeAbi.TryCreate(",
            "GetRuntimeSkeletonCompanionManagedInvokeStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source + "\n" + split_source)

        self.assertIn("RuntimeSkeletonCompanionManagedInvokeStubTemplateRelativePath", catalog_source)

        for required_fragment in [
            "target_method_token",
            "target_assembly_name_literal",
            "this_argument_expression",
            "method_invoke",
            "arg_storage_size",
            "return_value_size",
            "return_value_is_indirect",
        ]:
            self.assertIn(required_fragment, template_source)

    def test_runtime_skeleton_datetime_semantic_engine_helper_has_template(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofEmitter.DateTimeSemanticEngineFamily.cs"
        ).read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        template_source = RUNTIME_SKELETON_DATETIME_SEMANTIC_ENGINE_TEMPLATE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildRuntimeSkeletonDateTimeSemanticEngineFamilyHandler",
            "TryBuildRuntimeSkeletonDateTimeSemanticEngineHandler",
            "TryBuildAssemblyBoundDateTimeSemanticEngineStub",
            "TryBuildAssemblyBoundDateTimeSemanticEngineCore",
            "RuntimeSkeletonDateTimeSemanticEngineAbi.TryCreate(",
            "GetRuntimeSkeletonDateTimeSemanticEngineStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source + "\n" + split_source)

        self.assertIn("RuntimeSkeletonDateTimeSemanticEngineStubTemplateRelativePath", catalog_source)

        for required_fragment in [
            "target_method_token",
            "target_assembly_name_literal",
            "this_argument_expression",
            "method_invoke",
            "arg_storage_size",
            "return_value_size",
            "return_value_is_indirect",
        ]:
            self.assertIn(required_fragment, template_source)

    def test_runtime_skeleton_globalization_datetime_support_helper_has_template(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofEmitter.GlobalizationDateTimeSupportFamily.cs"
        ).read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        template_source = RUNTIME_SKELETON_GLOBALIZATION_DATETIME_SUPPORT_TEMPLATE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildRuntimeSkeletonGlobalizationDateTimeSupportFamilyHandler",
            "TryBuildRuntimeSkeletonGlobalizationDateTimeSupportHandler",
            "TryBuildAssemblyBoundGlobalizationDateTimeSupportStub",
            "TryBuildAssemblyBoundGlobalizationDateTimeSupportCore",
            "RuntimeSkeletonGlobalizationDateTimeSupportAbi.TryCreate(",
            "GetRuntimeSkeletonGlobalizationDateTimeSupportStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source + "\n" + split_source)

        self.assertIn("RuntimeSkeletonGlobalizationDateTimeSupportStubTemplateRelativePath", catalog_source)

        for required_fragment in [
            "target_method_token",
            "target_assembly_name_literal",
            "this_argument_expression",
            "method_invoke",
            "arg_storage_size",
            "return_value_size",
            "return_value_is_indirect",
        ]:
            self.assertIn(required_fragment, template_source)

    def test_runtime_skeleton_valuetype_kernel_helper_has_template(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofEmitter.ValueTypeKernelFamily.cs"
        ).read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        template_source = RUNTIME_SKELETON_VALUETYPE_KERNEL_TEMPLATE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildRuntimeSkeletonValueTypeKernelFamilyHandler",
            "TryBuildRuntimeSkeletonValueTypeKernelHandler",
            "TryBuildAssemblyBoundValueTypeKernelStub",
            "TryBuildAssemblyBoundValueTypeKernelCore",
            "RuntimeSkeletonKernelCore.",
            "GetRuntimeSkeletonValueTypeKernelStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source + "\n" + split_source)

        self.assertIn("RuntimeSkeletonValueTypeKernelStubTemplateRelativePath", catalog_source)

        for required_fragment in [
            "chaos::il2cpp::runtime_core::",
            "return CHAOS_BRIDGE_STATUS_OK;",
            "result = ",
        ]:
            self.assertIn(required_fragment, template_source)

    def test_runtime_skeleton_floating_scalar_managed_invoke_helper_has_template(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofEmitter.FloatingScalarManagedInvokeFamily.cs"
        ).read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        template_source = RUNTIME_SKELETON_FLOATING_SCALAR_MANAGED_INVOKE_TEMPLATE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildRuntimeSkeletonFloatingScalarManagedInvokeFamilyHandler",
            "TryBuildRuntimeSkeletonFloatingScalarManagedInvokeHandler",
            "TryBuildAssemblyBoundFloatingScalarManagedInvokeStub",
            "TryBuildAssemblyBoundFloatingScalarManagedInvokeCore",
            "RuntimeSkeletonFloatingScalarManagedInvokeAbi.TryCreate(",
            "GetRuntimeSkeletonFloatingScalarManagedInvokeStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source + "\n" + split_source)

        self.assertIn("RuntimeSkeletonFloatingScalarManagedInvokeStubTemplateRelativePath", catalog_source)

        for required_fragment in [
            "target_method_token",
            "target_assembly_name_literal",
            "this_argument_expression",
            "method_invoke",
            "arg_storage_size",
            "return_value_size",
            "return_value_is_indirect",
        ]:
            self.assertIn(required_fragment, template_source)

    def test_runtime_skeleton_enum_managed_invoke_helper_has_template(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        split_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofEmitter.EnumManagedInvokeFamily.cs"
        ).read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        template_source = RUNTIME_SKELETON_ENUM_MANAGED_INVOKE_TEMPLATE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildRuntimeSkeletonEnumManagedInvokeFamilyHandler",
            "TryBuildRuntimeSkeletonEnumManagedInvokeHandler",
            "TryBuildAssemblyBoundEnumManagedInvokeStub",
            "TryBuildAssemblyBoundEnumManagedInvokeCore",
            "RuntimeSkeletonEnumManagedInvokeAbi.TryCreate(",
            "GetRuntimeSkeletonEnumManagedInvokeStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source + "\n" + split_source)

        self.assertIn("RuntimeSkeletonEnumManagedInvokeStubTemplateRelativePath", catalog_source)

        for required_fragment in [
            "target_method_token",
            "target_assembly_name_literal",
            "this_argument_expression",
            "method_invoke",
            "arg_storage_size",
            "return_value_size",
            "return_value_is_indirect",
        ]:
            self.assertIn(required_fragment, template_source)

    def test_runtime_skeleton_static_boxed_value_type_char_invalid_cast_helper_has_template(self) -> None:
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
            / "NativeReferenceProof.RuntimeSkeleton.StaticBoxedValueTypeCharInvalidCastStub.cpp.scriban"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildAssemblyBoundStaticBoxedValueTypeCharInvalidCastStub",
            "TryResolveRuntimeSkeletonBoxedValueTypeCharInvalidCastShape",
            "GetRuntimeSkeletonStaticBoxedValueTypeCharInvalidCastStubTemplate",
            "bridge->box_value",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source if required_fragment != "bridge->box_value" else template_source)

        self.assertIn("RuntimeSkeletonStaticBoxedValueTypeCharInvalidCastStubTemplateRelativePath", catalog_source)
        self.assertIn("{{ boxed_value_type_token }}", template_source)
        self.assertIn("{{ input_size }}u", template_source)
        self.assertIn("raise_managed_exception", template_source)

    def test_runtime_skeleton_static_exception_catch_string_return_helper_has_template(self) -> None:
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
            / "NativeReferenceProof.RuntimeSkeleton.StaticExceptionCatchStringReturnStub.cpp.scriban"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildAssemblyBoundStaticExceptionCatchStringReturnStub",
            "GetRuntimeSkeletonStaticExceptionCatchStringReturnStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        self.assertIn(
            "RuntimeSkeletonStaticExceptionCatchStringReturnStubTemplateRelativePath",
            catalog_source,
        )

        for required_fragment in [
            "ManagedExceptionCarrier",
            "throw_stub_name",
            "caught_literal",
            "unreachable_literal",
            "return_value",
        ]:
            self.assertIn(required_fragment, template_source)
