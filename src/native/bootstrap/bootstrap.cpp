#include "bootstrap.h"

#include "reflection_query_model.h"
#include "runtime_core.h"
#include "support.h"

#include <cstdint>
#include <cstring>

namespace chaos::il2cpp::bootstrap {

namespace {

constexpr const char* kConsoleWriteLineStringIcall =
    "System.Console/System.Console::WriteLine(System.String)";
constexpr const char* kStringConcatPairIcall =
    "System.Private.CoreLib/System.String::Concat(System.String,System.String)";

BootstrapState g_bootstrap_state = {};

template <typename THandle>
THandle MakeOpaqueHandle(uint32_t token) {
    return reinterpret_cast<THandle>(static_cast<uintptr_t>(token));
}

bool IsStructSizeValid(
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options) {
    return code_registration != nullptr
        && metadata_registration != nullptr
        && options != nullptr
        && code_registration->struct_size >= sizeof(CodeRegistrationV0)
        && metadata_registration->struct_size >= sizeof(MetadataRegistrationV0)
        && options->struct_size >= sizeof(CodegenRegistrationOptionsV0);
}

bool IsBootstrapped(ImageHandle image) {
    return g_bootstrap_state.is_bootstrapped && image != nullptr;
}

BridgeStatus CHAOS_RUNTIME_ABI_CALL RegisterCodegen(
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options) {
    if (!IsStructSizeValid(code_registration, metadata_registration, options)) {
        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;
    }

    g_bootstrap_state.code_registration = code_registration;
    g_bootstrap_state.metadata_registration = metadata_registration;
    g_bootstrap_state.options = options;
    g_bootstrap_state.is_registered = true;
    g_bootstrap_state.is_bootstrapped = false;
    return CHAOS_BRIDGE_STATUS_OK;
}

BridgeStatus CHAOS_RUNTIME_ABI_CALL BootstrapRuntime(void) {
    if (!g_bootstrap_state.is_registered) {
        return CHAOS_BRIDGE_STATUS_NOT_READY;
    }

    if (chaos::il2cpp::runtime_core::GetRuntimeAbiV0() == nullptr) {
        return CHAOS_BRIDGE_STATUS_INTERNAL_ERROR;
    }

    g_bootstrap_state.is_bootstrapped = true;
    return CHAOS_BRIDGE_STATUS_OK;
}

TypeInfoHandle CHAOS_RUNTIME_ABI_CALL ResolveTypeByToken(
    ImageHandle image,
    uint32_t type_token) {
    if (!IsBootstrapped(image) || type_token == 0u) {
        return nullptr;
    }

    if (const auto* reflection_image = chaos::il2cpp::runtime_core::TryDecodeReflectionQueryImageHandle(image)) {
        const auto* type = chaos::il2cpp::runtime_core::FindReflectionQueryTypeByToken(reflection_image, type_token);
        return chaos::il2cpp::runtime_core::EncodeReflectionQueryTypeHandle(type);
    }

    return MakeOpaqueHandle<TypeInfoHandle>(type_token);
}

MethodInfoHandle CHAOS_RUNTIME_ABI_CALL ResolveMethodByToken(
    ImageHandle image,
    uint32_t method_token) {
    if (!IsBootstrapped(image) || method_token == 0u) {
        return nullptr;
    }

    if (const auto* reflection_image = chaos::il2cpp::runtime_core::TryDecodeReflectionQueryImageHandle(image)) {
        const auto* method = chaos::il2cpp::runtime_core::FindReflectionQueryMethodByToken(reflection_image, method_token);
        return chaos::il2cpp::runtime_core::EncodeReflectionQueryMethodHandle(method);
    }

    return MakeOpaqueHandle<MethodInfoHandle>(method_token);
}

FieldInfoHandle CHAOS_RUNTIME_ABI_CALL ResolveFieldByToken(
    ImageHandle image,
    uint32_t field_token) {
    if (!IsBootstrapped(image) || field_token == 0u) {
        return nullptr;
    }

    if (const auto* reflection_image = chaos::il2cpp::runtime_core::TryDecodeReflectionQueryImageHandle(image)) {
        const auto* field = chaos::il2cpp::runtime_core::FindReflectionQueryFieldByToken(reflection_image, field_token);
        return chaos::il2cpp::runtime_core::EncodeReflectionQueryFieldHandle(field);
    }

    return MakeOpaqueHandle<FieldInfoHandle>(field_token);
}

void* CHAOS_RUNTIME_ABI_CALL BoxValue(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    TypeInfoHandle value_type,
    const void* value,
    size_t value_size) {
    (void)runtime_state;
    (void)thread_state;
    (void)value_type;
    (void)value;
    (void)value_size;
    return nullptr;
}

BridgeStatus CHAOS_RUNTIME_ABI_CALL UnboxValue(
    RuntimeState* runtime_state,
    void* boxed_object,
    void* out_value,
    size_t out_value_size) {
    (void)runtime_state;
    (void)boxed_object;
    (void)out_value;
    (void)out_value_size;
    return CHAOS_BRIDGE_STATUS_NOT_SUPPORTED;
}

MethodInfoHandle CHAOS_RUNTIME_ABI_CALL ResolveVirtualMethod(
    TypeInfoHandle instance_type,
    MethodInfoHandle declared_method) {
    if (instance_type == nullptr || declared_method == nullptr) {
        return nullptr;
    }

    return declared_method;
}

BridgeStatus CHAOS_RUNTIME_ABI_CALL InvokeVirtual(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    void* object_instance,
    MethodInfoHandle method,
    void* const* argv,
    uint32_t argc,
    void* out_return_value,
    size_t out_return_value_size,
    ExceptionHandle* out_exception) {
    (void)runtime_state;
    (void)thread_state;
    (void)object_instance;
    (void)method;
    (void)argv;
    (void)argc;
    (void)out_return_value;
    (void)out_return_value_size;
    (void)out_exception;
    return CHAOS_BRIDGE_STATUS_NOT_SUPPORTED;
}

void* CHAOS_RUNTIME_ABI_CALL CreateDelegate(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    MethodInfoHandle method,
    void* target_instance) {
    (void)runtime_state;
    (void)thread_state;
    (void)method;
    (void)target_instance;
    return nullptr;
}

BridgeStatus CHAOS_RUNTIME_ABI_CALL DelegateInvoke(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    void* delegate_instance,
    void* const* argv,
    uint32_t argc,
    void* out_return_value,
    size_t out_return_value_size,
    ExceptionHandle* out_exception) {
    (void)runtime_state;
    (void)thread_state;
    (void)delegate_instance;
    (void)argv;
    (void)argc;
    (void)out_return_value;
    (void)out_return_value_size;
    (void)out_exception;
    return CHAOS_BRIDGE_STATUS_NOT_SUPPORTED;
}

void* CHAOS_RUNTIME_ABI_CALL ResolveIcall(const char* icall_name_utf8) {
    if (!g_bootstrap_state.is_bootstrapped || icall_name_utf8 == nullptr) {
        return nullptr;
    }

    if (std::strcmp(icall_name_utf8, kConsoleWriteLineStringIcall) == 0) {
        return reinterpret_cast<void*>(&chaos::il2cpp::support::WriteLineString);
    }

    if (std::strcmp(icall_name_utf8, kStringConcatPairIcall) == 0) {
        return reinterpret_cast<void*>(&chaos::il2cpp::support::ConcatStringPair);
    }

    return nullptr;
}

const CodegenBridgeV0 kCodegenBridgeV0 = {
    CHAOS_CODEGEN_BRIDGE_V0,
    sizeof(CodegenBridgeV0),
    &RegisterCodegen,
    &BootstrapRuntime,
    &ResolveTypeByToken,
    &ResolveMethodByToken,
    &ResolveFieldByToken,
    &BoxValue,
    &UnboxValue,
    &ResolveVirtualMethod,
    &InvokeVirtual,
    &CreateDelegate,
    &DelegateInvoke,
    &ResolveIcall,
};

}  // namespace

const BootstrapState* PeekBootstrapState() {
    return &g_bootstrap_state;
}

const CodegenBridgeV0* GetCodegenBridgeV0() {
    return &kCodegenBridgeV0;
}

}  // namespace chaos::il2cpp::bootstrap

extern "C" CHAOS_RUNTIME_ABI_EXPORT const CodegenBridgeV0* CHAOS_RUNTIME_ABI_CALL chaos_codegen_get_bridge_v0(void) {
    return chaos::il2cpp::bootstrap::GetCodegenBridgeV0();
}
