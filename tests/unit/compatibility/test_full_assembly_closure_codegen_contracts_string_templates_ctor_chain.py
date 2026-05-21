from tests.unit.compatibility.full_assembly_closure_codegen_contracts_test_support import *


class TestFullAssemblyClosureCodegenContractsStringTemplatesCtorChain(FullAssemblyClosureCodegenContractTestSupport):
    def test_runtime_skeleton_static_string_producer_ctor_getter_write_line_helper_has_template(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        catalog_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.Generator"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        producer_ctor_getter_write_line_template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.Generator"
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
            / "Chaos.IL2CPP.Generator"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        producer_ctor_render_write_line_template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.Generator"
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
            / "Chaos.IL2CPP.Generator"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.Generator"
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
            / "Chaos.IL2CPP.Generator"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.Generator"
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
            / "Chaos.IL2CPP.Generator"
            / "ReferenceProof"
            / "NativeReferenceProofCatalog.cs"
        ).read_text(encoding="utf-8")
        template_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.Generator"
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
