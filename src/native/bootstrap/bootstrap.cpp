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

struct UnresolvedVirtualCallEntry {
    uint32_t instance_type_token;
    uint32_t declared_method_token;
    void* resolved_method;
};

struct MethodPointerEntry {
    uint32_t method_token;
    void* method_pointer;
};

struct DelegateInstance {
    uint32_t method_token;
    void* method_pointer;
    void* target_instance;
};

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

uint32_t DecodeOpaqueToken(const void* handle) {
    return static_cast<uint32_t>(reinterpret_cast<uintptr_t>(handle));
}

const MethodPointerEntry* GetMethodPointerEntries() {
    if (g_bootstrap_state.code_registration == nullptr
        || g_bootstrap_state.code_registration->method_pointers == nullptr
        || g_bootstrap_state.code_registration->method_pointer_count == 0u) {
        return nullptr;
    }

    return static_cast<const MethodPointerEntry*>(g_bootstrap_state.code_registration->method_pointers);
}

void* FindMethodPointerByToken(uint32_t method_token) {
    const auto* entries = GetMethodPointerEntries();
    if (entries == nullptr) {
        return nullptr;
    }

    for (uint32_t index = 0u; index < g_bootstrap_state.code_registration->method_pointer_count; index++) {
        if (entries[index].method_token == method_token) {
            return entries[index].method_pointer;
        }
    }

    return nullptr;
}

const UnresolvedVirtualCallEntry* GetUnresolvedVirtualCallEntries() {
    if (g_bootstrap_state.code_registration == nullptr
        || g_bootstrap_state.code_registration->unresolved_virtual_calls == nullptr
        || g_bootstrap_state.code_registration->unresolved_virtual_call_count == 0u) {
        return nullptr;
    }

    return static_cast<const UnresolvedVirtualCallEntry*>(g_bootstrap_state.code_registration->unresolved_virtual_calls);
}

const UnresolvedVirtualCallEntry* FindUnresolvedVirtualCallEntry(
    TypeInfoHandle instance_type,
    MethodInfoHandle declared_method) {
    const auto* entries = GetUnresolvedVirtualCallEntries();
    if (entries == nullptr) {
        return nullptr;
    }

    const uint32_t instance_type_token = DecodeOpaqueToken(instance_type);
    const uint32_t declared_method_token = DecodeOpaqueToken(declared_method);

    for (uint32_t index = 0u; index < g_bootstrap_state.code_registration->unresolved_virtual_call_count; index++) {
        const auto& entry = entries[index];
        if (entry.instance_type_token == instance_type_token
            && entry.declared_method_token == declared_method_token
            && entry.resolved_method != nullptr) {
            return &entry;
        }
    }

    return nullptr;
}

bool IsKnownResolvedVirtualHandle(MethodInfoHandle method) {
    const auto* entries = GetUnresolvedVirtualCallEntries();
    if (entries == nullptr) {
        return false;
    }

    for (uint32_t index = 0u; index < g_bootstrap_state.code_registration->unresolved_virtual_call_count; index++) {
        if (entries[index].resolved_method == method) {
            return true;
        }
    }

    return false;
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
    return chaos::il2cpp::runtime_core::BoxValueObject(
        runtime_state,
        thread_state,
        value_type,
        value,
        value_size);
}

BridgeStatus CHAOS_RUNTIME_ABI_CALL UnboxValue(
    RuntimeState* runtime_state,
    void* boxed_object,
    void* out_value,
    size_t out_value_size) {
    return chaos::il2cpp::runtime_core::UnboxValueObject(
        runtime_state,
        boxed_object,
        out_value,
        out_value_size) == CHAOS_RUNTIME_STATUS_OK
        ? CHAOS_BRIDGE_STATUS_OK
        : CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;
}

MethodInfoHandle CHAOS_RUNTIME_ABI_CALL ResolveVirtualMethod(
    TypeInfoHandle instance_type,
    MethodInfoHandle declared_method) {
    if (instance_type == nullptr || declared_method == nullptr) {
        return nullptr;
    }

    if (const auto* entry = FindUnresolvedVirtualCallEntry(instance_type, declared_method)) {
        return reinterpret_cast<MethodInfoHandle>(entry->resolved_method);
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
    (void)argv;

    if (runtime_state == nullptr
        || thread_state == nullptr
        || object_instance == nullptr
        || method == nullptr
        || out_return_value == nullptr
        || argc != 0u
        || out_return_value_size != sizeof(void*)
        || !IsKnownResolvedVirtualHandle(method)) {
        return CHAOS_BRIDGE_STATUS_NOT_SUPPORTED;
    }

    if (out_exception != nullptr) {
        *out_exception = nullptr;
    }

    using VirtualInstanceMethodFn = void* (CHAOS_RUNTIME_ABI_CALL*)(
        RuntimeState* runtime,
        ThreadState* thread,
        void* __this);

    try {
        auto* const return_slot = static_cast<void**>(out_return_value);
        *return_slot = reinterpret_cast<VirtualInstanceMethodFn>(method)(runtime_state, thread_state, object_instance);
        return CHAOS_BRIDGE_STATUS_OK;
    } catch (const chaos::il2cpp::runtime_core::ManagedExceptionCarrier& carrier) {
        if (out_exception != nullptr) {
            *out_exception = carrier.exception;
        }

        return CHAOS_BRIDGE_STATUS_MANAGED_EXCEPTION;
    }
}

void* CHAOS_RUNTIME_ABI_CALL CreateDelegate(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    MethodInfoHandle method,
    void* target_instance) {
    (void)runtime_state;
    (void)thread_state;

    if (method == nullptr) {
        return nullptr;
    }

    const uint32_t method_token = DecodeOpaqueToken(method);
    void* const method_pointer = FindMethodPointerByToken(method_token);
    if (method_pointer == nullptr) {
        return nullptr;
    }

    auto* delegate_instance = new DelegateInstance
    {
        method_token,
        method_pointer,
        target_instance,
    };
    return delegate_instance;
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
    if (runtime_state == nullptr
        || thread_state == nullptr
        || delegate_instance == nullptr
        || argv == nullptr
        || argc != 1u
        || out_return_value == nullptr
        || out_return_value_size != sizeof(void*)) {
        return CHAOS_BRIDGE_STATUS_NOT_SUPPORTED;
    }

    if (out_exception != nullptr) {
        *out_exception = nullptr;
    }

    auto* const delegate_handle = static_cast<DelegateInstance*>(delegate_instance);
    if (delegate_handle->method_pointer == nullptr) {
        return CHAOS_BRIDGE_STATUS_NOT_FOUND;
    }

    void* const argument0 = argv[0];
    auto* const return_slot = static_cast<void**>(out_return_value);

    try {
        if (delegate_handle->target_instance != nullptr) {
            using ClosedInstanceDelegateFn = void* (CHAOS_RUNTIME_ABI_CALL*)(
                RuntimeState* runtime,
                ThreadState* thread,
                void* __this,
                void* arg0);
            *return_slot = reinterpret_cast<ClosedInstanceDelegateFn>(delegate_handle->method_pointer)(
                runtime_state,
                thread_state,
                delegate_handle->target_instance,
                argument0);
        } else {
            using StaticDelegateFn = void* (CHAOS_RUNTIME_ABI_CALL*)(
                RuntimeState* runtime,
                ThreadState* thread,
                void* arg0);
            *return_slot = reinterpret_cast<StaticDelegateFn>(delegate_handle->method_pointer)(
                runtime_state,
                thread_state,
                argument0);
        }

        return CHAOS_BRIDGE_STATUS_OK;
    } catch (const chaos::il2cpp::runtime_core::ManagedExceptionCarrier& carrier) {
        if (out_exception != nullptr) {
            *out_exception = carrier.exception;
        }

        return CHAOS_BRIDGE_STATUS_MANAGED_EXCEPTION;
    }
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
