#include "codegen_bridge.h"

#include <type_traits>

namespace chaos::contracts::native::assembly_bound {

extern "C" int32_t CHAOS_RUNTIME_ABI_CALL RunNativeReferenceAssembly(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    void* managed_args);

void CompileOnlyAssemblyBoundNativeReferenceSmoke(const CodegenBridgeV0* bridge) {
    CodeRegistrationV0 code_registration = {};
    code_registration.struct_size = sizeof(code_registration);

    MetadataRegistrationV0 metadata_registration = {};
    metadata_registration.struct_size = sizeof(metadata_registration);

    CodegenRegistrationOptionsV0 options = {};
    options.struct_size = sizeof(options);
    options.image_name_utf8 = "System.Private.CoreLib";

    RuntimeState* runtime = nullptr;
    ThreadState* thread = nullptr;
    int managed_payload = 7;

    NativeReferenceAssemblyDispatchRequestV0 request = {};
    request.subject_id_utf8 = "System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)";
    request.managed_args = &managed_payload;
    request.method_id = 0u;

    if (bridge == nullptr) {
        return;
    }

    (void)RunNativeReferenceAssembly(
        bridge,
        &code_registration,
        &metadata_registration,
        &options,
        runtime,
        thread,
        &request);
}

static_assert(
    std::is_same<decltype(NativeReferenceAssemblyDispatchRequestV0::subject_id_utf8), const char*>::value,
    "assembly-bound dispatch request subject id must remain utf8 string");
static_assert(
    std::is_same<decltype(NativeReferenceAssemblyDispatchRequestV0::managed_args), void*>::value,
    "assembly-bound dispatch request payload must remain opaque pointer");
static_assert(
    std::is_same<decltype(NativeReferenceAssemblyDispatchRequestV0::method_id), uint32_t>::value,
    "assembly-bound dispatch request numeric method id must remain uint32_t");

}  // namespace chaos::contracts::native::assembly_bound
