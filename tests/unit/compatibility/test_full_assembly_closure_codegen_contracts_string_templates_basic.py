from tests.unit.compatibility.full_assembly_closure_codegen_contracts_test_support import *


class TestFullAssemblyClosureCodegenContractsStringTemplatesBasic(FullAssemblyClosureCodegenContractTestSupport):
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
            "ExecuteRuntimeSkeletonConstructorFieldSetter(",
        ]:
            self.assertIn(required_fragment, constructor_template_source)

        for required_fragment in [
            "_ManagedArgs",
            "request->instance",
            "request->return_value",
            "ExecuteRuntimeSkeletonFieldBackedStringReturn(",
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
            "ExecuteRuntimeSkeletonFieldGetterStringReturn(",
        ]:
            self.assertIn(required_fragment, field_getter_template_source)

    def test_runtime_skeleton_string_return_forwarder_helper_stays_distinct_from_identity_forwarder(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        return_forwarder_template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.RuntimeSkeleton.StaticStringReturnForwarderStub.cpp.scriban"
        ).read_text(encoding="utf-8")
        identity_forwarder_template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.RuntimeSkeleton.StaticStringForwarderStub.cpp.scriban"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildAssemblyBoundStaticStringReturnForwarderStub",
            "TryBuildAssemblyBoundStaticStringForwarderStub",
            "GetRuntimeSkeletonStaticStringReturnForwarderStubTemplate",
            "GetRuntimeSkeletonStaticStringForwarderStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        self.assertIn("RuntimeSkeletonStaticStringReturnForwarderStubTemplateRelativePath", catalog_source)
        self.assertIn("RuntimeSkeletonStaticStringForwarderStubTemplateRelativePath", catalog_source)
        self.assertIn("target_stub_name", return_forwarder_template_source)
        self.assertIn("(void)managed_args;", return_forwarder_template_source)
        self.assertIn("request->value", identity_forwarder_template_source)
        self.assertIn("*request->return_value = request->value;", identity_forwarder_template_source)

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

    def test_runtime_skeleton_static_resource_key_string_return_helper_reuses_literal_template(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        literal_template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "Templates"
            / "NativeReferenceProof.RuntimeSkeleton.StaticLiteralStringReturnStub.cpp.scriban"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildAssemblyBoundStaticResourceKeyStringReturnStub",
            '::GetResourceString:System.String(System.String)"',
            "GetRuntimeSkeletonStaticLiteralStringReturnStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        self.assertIn("RuntimeSkeletonStaticLiteralStringReturnStubTemplateRelativePath", catalog_source)

        for required_fragment in [
            "_ManagedArgs",
            "request->return_value",
            "string_new_utf8",
            "*request->return_value = literal_string;",
        ]:
            self.assertIn(required_fragment, literal_template_source)

    def test_runtime_skeleton_static_checked_byte_convert_helper_has_template(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        template_source = RUNTIME_SKELETON_STATIC_CHECKED_BYTE_CONVERT_TEMPLATE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "TryBuildAssemblyBoundStaticCheckedByteConvertStub",
            "TryResolveRuntimeSkeletonCheckedByteConvertShape",
            "GetRuntimeSkeletonStaticCheckedByteConvertStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        self.assertIn("RuntimeSkeletonStaticCheckedByteConvertStubTemplateRelativePath", catalog_source)

        for required_fragment in [
            "_ManagedArgs",
            "std::uint8_t* return_value",
            "overflow_condition_expression",
            "throw_stub_name",
            "static_cast<std::uint8_t>(request->value)",
        ]:
            self.assertIn(required_fragment, template_source)

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
            "GetRuntimeSkeletonStaticPrimitiveConvertStubTemplate",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        self.assertIn(
            "RuntimeSkeletonStaticPrimitiveConvertStubTemplateRelativePath",
            catalog_source,
        )

        for required_fragment in [
            "_ManagedArgs",
            "input_cpp_type",
            "output_cpp_type",
            "converted_value_expression",
            "*request->return_value = static_cast<",
        ]:
            self.assertIn(required_fragment, template_source)
