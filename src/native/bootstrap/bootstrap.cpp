#include "bootstrap.h"

#include "generic_context.h"
#include "method_replacement.h"
#include "reflection_query_model.h"
#include "runtime_core.h"
#include "support.h"
#include "vtable_registry.h"

#include <cstdint>
#include <cstring>

namespace chaos::il2cpp::bootstrap {

namespace {

constexpr const char* kConsoleWriteLineStringIcall =
    "System.Console/System.Console::WriteLine(System.String)";
constexpr const char* kStringConcatPairIcall =
    "System.Private.CoreLib/System.String::Concat(System.String,System.String)";
constexpr const char* kStringConcatTripleIcall =
    "System.Private.CoreLib/System.String::Concat(System.String,System.String,System.String)";
constexpr const char* kDelegateCombineIcall =
    "System.Private.CoreLib/System.Delegate::Combine(System.Delegate,System.Delegate)";
constexpr const char* kDelegateRemoveIcall =
    "System.Private.CoreLib/System.Delegate::Remove(System.Delegate,System.Delegate)";

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
    DelegateInstance* next;          // multicast chain (nullptr = last in chain)
    uint32_t invocation_count;       // total length of the chain starting at this node
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
    if (void* replacement = chaos::il2cpp::method_replacement::Resolve(method_token)) {
        return replacement;
    }

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
    if (method == nullptr) return false;

    // Accept any method pointer that came from the vtable registry.
    // (vtable registry stores raw function pointers, not opaque tokens.)
    // We distinguish vtable pointers from token handles by checking the token range:
    // tokens are <= 0x0FFFFFFF; real function pointers are usually larger addresses.
    const uintptr_t raw = reinterpret_cast<uintptr_t>(method);
    if (raw > static_cast<uintptr_t>(0x0FFFFFFFu)) {
        // Likely a real function pointer from vtable registry or unresolved-virtual table.
        return true;
    }

    // Also check the legacy unresolved-virtual table.
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

    // Note: MetadataRegistrationV0.generic_types / generic_methods are reserved for
    // generic instantiation registration. When the codegen emits a concrete struct layout
    // for these arrays (Phase A.11), we will iterate them here and call
    // chaos::il2cpp::generic_context::RegisterGenericInstantiation().
    // For now the pointers may be null; this is a no-op.

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

    const uint32_t instance_type_token  = DecodeOpaqueToken(instance_type);
    const uint32_t declared_method_token = DecodeOpaqueToken(declared_method);

    // 1. Try vtable registry (runtime-registered per-type vtables).
    if (void* vtable_ptr = chaos::il2cpp::vtable_registry::ResolveVirtualMethodPointer(
            instance_type_token, declared_method_token)) {
        return reinterpret_cast<MethodInfoHandle>(vtable_ptr);
    }

    // 2. Fall back to the pre-resolved unresolved-virtual-call table (codegen inline).
    if (const auto* entry = FindUnresolvedVirtualCallEntry(instance_type, declared_method)) {
        return reinterpret_cast<MethodInfoHandle>(entry->resolved_method);
    }

    // 3. Return the declared method as last resort (direct call fallback).
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
        nullptr,   // next
        1u,        // invocation_count
    };
    return delegate_instance;
}

/// Clone a delegate chain (deep copy of all nodes).
static DelegateInstance* CloneChain(const DelegateInstance* src) {
    if (src == nullptr) return nullptr;
    auto* copy = new DelegateInstance { src->method_token, src->method_pointer, src->target_instance, nullptr, 1u };
    DelegateInstance* tail = copy;
    const DelegateInstance* cur = src->next;
    while (cur != nullptr) {
        auto* node = new DelegateInstance { cur->method_token, cur->method_pointer, cur->target_instance, nullptr, 1u };
        tail->next = node;
        tail = node;
        cur = cur->next;
    }
    return copy;
}

/// Returns true if two delegate nodes refer to the same (method, target) pair.
static bool DelegateNodesEqual(const DelegateInstance* a, const DelegateInstance* b) {
    return a != nullptr && b != nullptr
        && a->method_token  == b->method_token
        && a->target_instance == b->target_instance;
}

/// Rebuild invocation_count for each node in the chain.
static void RebuildCounts(DelegateInstance* head) {
    // Count chain length
    uint32_t len = 0u;
    for (const DelegateInstance* n = head; n != nullptr; n = n->next) { ++len; }
    // Assign descending counts
    DelegateInstance* n = head;
    while (n != nullptr) {
        n->invocation_count = len--;
        n = n->next;
    }
}

void* CHAOS_RUNTIME_ABI_CALL CombineDelegate(
    void* left_delegate,
    void* right_delegate) {
    if (left_delegate == nullptr)  return right_delegate;
    if (right_delegate == nullptr) return left_delegate;

    // Clone left chain and append a clone of the right node (single-node append).
    auto* new_head = CloneChain(static_cast<DelegateInstance*>(left_delegate));
    // Find tail of new_head
    DelegateInstance* tail = new_head;
    while (tail->next != nullptr) { tail = tail->next; }
    // Append clone of right (single node only — multicast combine is left + right)
    auto* right = static_cast<DelegateInstance*>(right_delegate);
    auto* right_clone = new DelegateInstance
        { right->method_token, right->method_pointer, right->target_instance, nullptr, 1u };
    tail->next = right_clone;
    RebuildCounts(new_head);
    return new_head;
}

void* CHAOS_RUNTIME_ABI_CALL RemoveDelegate(
    void* source_delegate,
    void* target_delegate) {
    if (source_delegate == nullptr || target_delegate == nullptr) return source_delegate;

    auto* src  = static_cast<const DelegateInstance*>(source_delegate);
    auto* tgt  = static_cast<const DelegateInstance*>(target_delegate);

    // Find the LAST node in the chain matching target.
    // Build a cloned chain without that node.
    auto* clone_head = CloneChain(src);

    // Find the last matching node in the clone
    DelegateInstance* prev        = nullptr;
    DelegateInstance* last_match  = nullptr;
    DelegateInstance* last_prev   = nullptr;
    DelegateInstance* cur         = clone_head;
    while (cur != nullptr) {
        if (DelegateNodesEqual(cur, tgt)) {
            last_match = cur;
            last_prev  = prev;
        }
        prev = cur;
        cur  = cur->next;
    }

    if (last_match == nullptr) {
        // No match — return original (clone is wasted, but safe)
        return clone_head;
    }

    // Remove last_match from clone chain
    if (last_prev == nullptr) {
        // Removing head
        DelegateInstance* new_head = last_match->next;
        last_match->next = nullptr;
        delete last_match;
        if (new_head != nullptr) RebuildCounts(new_head);
        return new_head;
    } else {
        last_prev->next = last_match->next;
        last_match->next = nullptr;
        delete last_match;
        RebuildCounts(clone_head);
        return clone_head;
    }
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

    auto* delegate_handle = static_cast<DelegateInstance*>(delegate_instance);

    void* const argument0 = argv[0];
    auto* const return_slot = static_cast<void**>(out_return_value);
    *return_slot = nullptr;

    // Walk the multicast chain; last return value wins (C# semantics).
    try {
        while (delegate_handle != nullptr) {
            if (delegate_handle->method_pointer == nullptr) {
                return CHAOS_BRIDGE_STATUS_NOT_FOUND;
            }

            if (delegate_handle->target_instance != nullptr) {
                using ClosedInstanceDelegateFn = void* (CHAOS_RUNTIME_ABI_CALL*)(
                    RuntimeState* runtime, ThreadState* thread, void* __this, void* arg0);
                *return_slot = reinterpret_cast<ClosedInstanceDelegateFn>(delegate_handle->method_pointer)(
                    runtime_state, thread_state, delegate_handle->target_instance, argument0);
            } else {
                using StaticDelegateFn = void* (CHAOS_RUNTIME_ABI_CALL*)(
                    RuntimeState* runtime, ThreadState* thread, void* arg0);
                *return_slot = reinterpret_cast<StaticDelegateFn>(delegate_handle->method_pointer)(
                    runtime_state, thread_state, argument0);
            }

            delegate_handle = delegate_handle->next;
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

    if (std::strcmp(icall_name_utf8, kStringConcatTripleIcall) == 0) {
        return reinterpret_cast<void*>(&chaos::il2cpp::support::ConcatStringTriple);
    }

    if (std::strcmp(icall_name_utf8, kDelegateCombineIcall) == 0) {
        return reinterpret_cast<void*>(&CombineDelegate);
    }

    if (std::strcmp(icall_name_utf8, kDelegateRemoveIcall) == 0) {
        return reinterpret_cast<void*>(&RemoveDelegate);
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
