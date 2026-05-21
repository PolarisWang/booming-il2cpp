from tests.unit.compatibility.full_assembly_closure_codegen_contracts_test_support import *


class TestFullAssemblyClosureCodegenContractsStringTemplatesForwarderChain(FullAssemblyClosureCodegenContractTestSupport):
    def test_runtime_skeleton_static_string_producer_ctor_instance_call_forwarder_forwarder_write_line_helper_has_template(self) -> None:
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
