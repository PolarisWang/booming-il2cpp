#include "bootstrap.h"
#include "runtime_core.h"
#include "support.h"

#include <cstdint>

namespace chaos::il2cpp::generated {

int32_t CHAOS_RUNTIME_ABI_CALL RunNativeReference(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    void* managed_args);

}  // namespace chaos::il2cpp::generated

namespace chaos::il2cpp::proof_host {

constexpr const char* kHostName = "windows-x64-reference";
constexpr const char* kRuntimeTag = "subject-reference-proof";
constexpr const char* kConsoleWriteLineStringIcall =
    "System.Console/System.Console::WriteLine(System.String)";
constexpr const char* kStringConcatPairIcall =
    "System.Private.CoreLib/System.String::Concat(System.String,System.String)";

int32_t RunProofHost() {
    const RuntimeAbiV0* abi = chaos_runtime_get_abi_v0();
    const CodegenBridgeV0* bridge = chaos_codegen_get_bridge_v0();
    if (abi == nullptr || bridge == nullptr) {
        return 1;
    }

    const auto* write_line_string = &chaos::il2cpp::support::WriteLineString;
    const auto* concat_string_pair = &chaos::il2cpp::support::ConcatStringPair;
    (void)write_line_string;
    (void)concat_string_pair;

    RuntimeInitParams init_params = {};
    init_params.struct_size = sizeof(init_params);
    init_params.host_name_utf8 = kHostName;
    init_params.runtime_tag_utf8 = kRuntimeTag;

    RuntimeState* runtime = nullptr;
    if (abi->runtime_init(&init_params, nullptr, &runtime) != CHAOS_RUNTIME_STATUS_OK || runtime == nullptr) {
        return 1;
    }

    ThreadState* thread = nullptr;
    if (abi->thread_attach(runtime, &thread) != CHAOS_RUNTIME_STATUS_OK || thread == nullptr) {
        abi->runtime_shutdown(runtime);
        return 1;
    }

    CodeRegistrationV0 code_registration = {};
    code_registration.struct_size = sizeof(code_registration);

    MetadataRegistrationV0 metadata_registration = {};
    metadata_registration.struct_size = sizeof(metadata_registration);

    CodegenRegistrationOptionsV0 options = {};
    options.struct_size = sizeof(options);
    options.image_name_utf8 = "InterfaceDispatchProof";

    const int32_t exit_code = chaos::il2cpp::generated::RunNativeReference(
        bridge,
        &code_registration,
        &metadata_registration,
        &options,
        runtime,
        thread,
        nullptr);

    (void)bridge->resolve_icall(kConsoleWriteLineStringIcall);
    (void)bridge->resolve_icall(kStringConcatPairIcall);

    abi->thread_detach(runtime, thread);
    abi->runtime_shutdown(runtime);
    return exit_code;
}

}  // namespace chaos::il2cpp::proof_host

int main() {
    return chaos::il2cpp::proof_host::RunProofHost();
}
