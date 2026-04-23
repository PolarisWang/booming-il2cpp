#include "codegen_bridge.h"

namespace chaos::contracts::native::examples {

extern "C" int32_t CHAOS_RUNTIME_ABI_CALL RunNativeReferenceAssembly(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    void* managed_args);

void AssemblyBoundDispatchExample(
    RuntimeState* runtime,
    ThreadState* thread,
    const char* subject_id_utf8,
    void* subject_args) {
    const CodegenBridgeV0* bridge = chaos_codegen_get_bridge_v0();
    if (bridge == nullptr || subject_id_utf8 == nullptr) {
        return;
    }

    CodeRegistrationV0 code_registration = {};
    code_registration.struct_size = sizeof(code_registration);

    MetadataRegistrationV0 metadata_registration = {};
    metadata_registration.struct_size = sizeof(metadata_registration);

    CodegenRegistrationOptionsV0 options = {};
    options.struct_size = sizeof(options);
    options.image_name_utf8 = "System.Private.CoreLib";

    NativeReferenceAssemblyDispatchRequestV0 request = {};
    request.subject_id_utf8 = subject_id_utf8;
    request.managed_args = subject_args;
    request.method_id = 0u;

    (void)RunNativeReferenceAssembly(
        bridge,
        &code_registration,
        &metadata_registration,
        &options,
        runtime,
        thread,
        &request);
}

}  // namespace chaos::contracts::native::examples
