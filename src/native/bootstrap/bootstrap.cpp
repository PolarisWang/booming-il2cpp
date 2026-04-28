#include "bootstrap.h"

#include "generic_context.h"
#include "memory_domain.h"
#include "method_replacement.h"
#include "reflection_query_model.h"
#include "runtime_core.h"
#include "string_table.h"
#include "support.h"
#include "vtable_registry.h"

#include <gc.h>

#include <cstdint>
#include <cstring>
#include <unordered_map>

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

struct InvokerEntry {
    uint32_t method_token;
    void* invoker_pointer;
};

struct DelegateInstance {
    uint32_t method_token;
    void* method_pointer;
    void* target_instance;
    DelegateInstance* next;          // multicast chain (nullptr = last in chain)
    // NOTE: nodes are GC-allocated and immutable after creation.
    // Combine clones left prefix, shares right suffix.
    // Remove clones prefix past removed node, shares suffix.
};

BootstrapState g_bootstrap_state = {};/// Invoker pointer table built at RegisterCodegen time from the
/// code_registration->invoker_pointers array.
/// Keyed by method_token, used by MethodInvoke to dispatch calls correctly
/// regardless of whether the caller passes an opaque token, a reflection-query
/// handle, or a raw function pointer.
static CHAOS_IL2CPP_UNORDERED_MAP(uint32_t, void*) g_invoker_table;

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

void* FindInvokerPointerByToken(uint32_t method_token) {
    auto iter = g_invoker_table.find(method_token);
    return iter != g_invoker_table.end() ? iter->second : nullptr;
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
    uint32_t instance_type_token,
    uint32_t declared_method_token) {
    const auto* entries = GetUnresolvedVirtualCallEntries();
    if (entries == nullptr) {
        return nullptr;
    }

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

    // Build invoker pointer table for MethodInvoke dispatch.
    g_invoker_table.clear();
    if (code_registration->invoker_pointers != nullptr && code_registration->invoker_pointer_count > 0u) {
        const auto* entries = static_cast<const InvokerEntry*>(code_registration->invoker_pointers);
        for (uint32_t index = 0u; index < code_registration->invoker_pointer_count; index++) {
            g_invoker_table[entries[index].method_token] = entries[index].invoker_pointer;
        }
    }

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

    // Register an AOT root memory domain for this module so that marshal
    // allocations originating from the main AOT module are domain-tracked.
    const char* module_name = g_bootstrap_state.options != nullptr
        && g_bootstrap_state.options->image_name_utf8 != nullptr
        ? g_bootstrap_state.options->image_name_utf8
        : "AotRoot";
    memory_domain::DomainInit domain_init{};
    domain_init.module_name = module_name;
    domain_init.module_kind = 0u;       // 0 = AOT
    domain_init.usage_limit = 0;        // unlimited
    domain_init.runtime_config = nullptr;
    g_bootstrap_state.aot_domain_id = memory_domain::RegisterMemoryDomain(domain_init);

    // Push the AOT domain as the ambient domain on this thread so that
    // CurrentDomain() returns it for any marshal allocation originating
    // from the main module.  Hot-update DLLs push their own domain when
    // their code runs (via DomainScope in their entry points).
    if (g_bootstrap_state.aot_domain_id != memory_domain::kDomainIdInvalid) {
        auto* aot_domain = memory_domain::FindDomainById(g_bootstrap_state.aot_domain_id);
        if (aot_domain != nullptr) {
            memory_domain::PushDomain(aot_domain);
        }
    }

    g_bootstrap_state.is_bootstrapped = true;

    // Native-reference proof hosts do not guarantee an AOT-baked string table.
    // Initialize the runtime table with an empty AOT view; generated entrypoints
    // can still intern strings dynamically when they need them.
    chaos::il2cpp::string_table::InitializeFromAot(
        nullptr,
        0u);

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

const RuntimeTypeCapabilityEntryV0* GetTypeCapabilityEntries() {
    if (g_bootstrap_state.code_registration == nullptr ||
        g_bootstrap_state.code_registration->type_capabilities == nullptr ||
        g_bootstrap_state.code_registration->type_capability_count == 0u) {
        return nullptr;
    }

    return g_bootstrap_state.code_registration->type_capabilities;
}

const RuntimeTypeCapabilityEntryV0* FindTypeCapabilityEntryByToken(uint32_t type_token) {
    const auto* entries = GetTypeCapabilityEntries();
    if (entries == nullptr || type_token == 0u) {
        return nullptr;
    }

    for (uint32_t index = 0u; index < g_bootstrap_state.code_registration->type_capability_count; ++index) {
        if (entries[index].type_token == type_token) {
            return &entries[index];
        }
    }

    return nullptr;
}

BridgeStatus CHAOS_RUNTIME_ABI_CALL QueryTypeCapability(
    TypeInfoHandle type,
    RuntimeTypeCapabilityInfoV0* out_capability_info) {
    return chaos::il2cpp::runtime_core::TypeQueryCapability(type, out_capability_info) == CHAOS_RUNTIME_STATUS_OK
        ? CHAOS_BRIDGE_STATUS_OK
        : CHAOS_BRIDGE_STATUS_NOT_SUPPORTED;
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

static uint32_t DecodeVirtualMethodToken(MethodInfoHandle method) {
    if (const auto* desc = chaos::il2cpp::runtime_core::TryDecodeReflectionQueryMethodHandle(method)) {
        return desc->metadata_token;
    }
    return DecodeOpaqueToken(method);
}

static uint32_t DecodeVirtualTypeToken(TypeInfoHandle type) {
    if (const auto* desc = chaos::il2cpp::runtime_core::TryDecodeReflectionQueryTypeHandle(type)) {
        return desc->metadata_token;
    }
    return DecodeOpaqueToken(type);
}

MethodInfoHandle CHAOS_RUNTIME_ABI_CALL ResolveVirtualMethod(
    TypeInfoHandle instance_type,
    MethodInfoHandle declared_method) {
    if (instance_type == nullptr || declared_method == nullptr) {
        return nullptr;
    }

    const uint32_t instance_type_token  = DecodeVirtualTypeToken(instance_type);
    const uint32_t declared_method_token = DecodeVirtualMethodToken(declared_method);

    // 1. Try vtable registry (runtime-registered per-type vtables).
    if (void* vtable_ptr = chaos::il2cpp::vtable_registry::ResolveVirtualMethodPointer(
            instance_type_token, declared_method_token)) {
        return reinterpret_cast<MethodInfoHandle>(vtable_ptr);
    }

    // 2. Fall back to the pre-resolved unresolved-virtual-call table (codegen inline).
    if (const auto* entry = FindUnresolvedVirtualCallEntry(instance_type_token, declared_method_token)) {
        return reinterpret_cast<MethodInfoHandle>(entry->resolved_method);
    }

    // 3. Return the declared method as last resort (direct call fallback).
    return declared_method;
}

static MethodInfoHandle ResolveVirtualMethodTokenImpl(
    uint32_t instance_type_token,
    uint32_t declared_method_token) {
    // 1. Try vtable registry (runtime-registered per-type vtables).
    if (void* vtable_ptr = chaos::il2cpp::vtable_registry::ResolveVirtualMethodPointer(
            instance_type_token, declared_method_token)) {
        return reinterpret_cast<MethodInfoHandle>(vtable_ptr);
    }

    // 2. Fall back to the pre-resolved unresolved-virtual-call table.
    if (const auto* entry = FindUnresolvedVirtualCallEntry(instance_type_token, declared_method_token)) {
        return reinterpret_cast<MethodInfoHandle>(entry->resolved_method);
    }

    return nullptr;
}

MethodInfoHandle CHAOS_RUNTIME_ABI_CALL ResolveVirtualMethodByToken(
    uint32_t instance_type_token,
    uint32_t declared_method_token) {
    if (instance_type_token == 0u || declared_method_token == 0u) {
        return nullptr;
    }
    return ResolveVirtualMethodTokenImpl(instance_type_token, declared_method_token);
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
    if (runtime_state == nullptr
        || thread_state == nullptr
        || object_instance == nullptr
        || method == nullptr
        || (argv == nullptr && argc > 0u)
        || argc > 4u
        || out_return_value == nullptr
        || out_return_value_size != sizeof(void*)
        || !IsKnownResolvedVirtualHandle(method)) {
        return CHAOS_BRIDGE_STATUS_NOT_SUPPORTED;
    }

    if (out_exception != nullptr) {
        *out_exception = nullptr;
    }

    using Vfn0 = void* (CHAOS_RUNTIME_ABI_CALL*)(RuntimeState*, ThreadState*, void*);
    using Vfn1 = void* (CHAOS_RUNTIME_ABI_CALL*)(RuntimeState*, ThreadState*, void*, void*);
    using Vfn2 = void* (CHAOS_RUNTIME_ABI_CALL*)(RuntimeState*, ThreadState*, void*, void*, void*);
    using Vfn3 = void* (CHAOS_RUNTIME_ABI_CALL*)(RuntimeState*, ThreadState*, void*, void*, void*, void*);
    using Vfn4 = void* (CHAOS_RUNTIME_ABI_CALL*)(RuntimeState*, ThreadState*, void*, void*, void*, void*, void*);

    try {
        auto* const return_slot = static_cast<void**>(out_return_value);
        switch (argc) {
            case 0u:
                *return_slot = reinterpret_cast<Vfn0>(method)(runtime_state, thread_state, object_instance);
                break;
            case 1u:
                *return_slot = reinterpret_cast<Vfn1>(method)(runtime_state, thread_state, object_instance, argv[0]);
                break;
            case 2u:
                *return_slot = reinterpret_cast<Vfn2>(method)(runtime_state, thread_state, object_instance, argv[0], argv[1]);
                break;
            case 3u:
                *return_slot = reinterpret_cast<Vfn3>(method)(runtime_state, thread_state, object_instance, argv[0], argv[1], argv[2]);
                break;
            case 4u:
                *return_slot = reinterpret_cast<Vfn4>(method)(runtime_state, thread_state, object_instance, argv[0], argv[1], argv[2], argv[3]);
                break;
            default:
                return CHAOS_BRIDGE_STATUS_NOT_SUPPORTED;
        }
        return CHAOS_BRIDGE_STATUS_OK;
    } catch (const chaos::il2cpp::runtime_core::ManagedExceptionCarrier& carrier) {
        if (out_exception != nullptr) {
            *out_exception = carrier.exception;
        }

        return CHAOS_BRIDGE_STATUS_MANAGED_EXCEPTION;
    }
}

static DelegateInstance* AllocateDelegateNode(
    uint32_t method_token,
    void* method_pointer,
    void* target_instance,
    DelegateInstance* next) {
    // GC_MALLOC is required here (not domain heap) because DelegateInstance
    // nodes contain managed object pointers (target_instance, next) that
    // BDWGC must scan for reachability.  Domain heaps are raw allocators
    // with no GC tracing.
    auto* node = static_cast<DelegateInstance*>(GC_MALLOC(sizeof(DelegateInstance)));
    node->method_token = method_token;
    node->method_pointer = method_pointer;
    node->target_instance = target_instance;
    node->next = next;
    return node;
}

static DelegateInstance* ClonePrefixTo(const DelegateInstance* head, const DelegateInstance* exclusive_end) {
    if (head == nullptr || head == exclusive_end) {
        return nullptr;
    }

    auto* clone_head = AllocateDelegateNode(head->method_token, head->method_pointer, head->target_instance, nullptr);
    DelegateInstance* tail = clone_head;
    const DelegateInstance* cur = head->next;
    while (cur != nullptr && cur != exclusive_end) {
        auto* clone = AllocateDelegateNode(cur->method_token, cur->method_pointer, cur->target_instance, nullptr);
        tail->next = clone;
        tail = clone;
        cur = cur->next;
    }
    return clone_head;
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

    return AllocateDelegateNode(method_token, method_pointer, target_instance, nullptr);
}

/// Returns true if two delegate nodes refer to the same (method, target) pair.
static bool DelegateNodesEqual(const DelegateInstance* a, const DelegateInstance* b) {
    return a != nullptr && b != nullptr
        && a->method_token  == b->method_token
        && a->target_instance == b->target_instance;
}

/// Finds the last node matching `tgt` in the chain starting at `head`.
/// Returns { prev, match } pointers (prev = nullptr if match is head).
static void FindLastMatch(
    const DelegateInstance* head,
    const DelegateInstance* tgt,
    const DelegateInstance** out_prev,
    const DelegateInstance** out_match) {
    *out_match = nullptr;
    *out_prev = nullptr;
    const DelegateInstance* prev = nullptr;
    for (auto* cur = head; cur != nullptr; prev = cur, cur = cur->next) {
        if (DelegateNodesEqual(cur, tgt)) {
            *out_match = cur;
            *out_prev = prev;
        }
    }
}

void* CHAOS_RUNTIME_ABI_CALL CombineDelegate(
    void* left_delegate,
    void* right_delegate) {
    if (left_delegate == nullptr)  return right_delegate;
    if (right_delegate == nullptr) return left_delegate;

    // Build: clone(left) -> share(right as suffix)
    // The right chain is shared by reference (immutable nodes), so this is
    // O(|left|) — we only copy the left prefix, not the right chain.
    auto* left = static_cast<DelegateInstance*>(left_delegate);
    auto* right = static_cast<DelegateInstance*>(right_delegate);

    auto* clone_head = ClonePrefixTo(left, nullptr);
    if (clone_head == nullptr) {
        return right;  // left was empty
    }

    // Attach shared right suffix to the tail of the cloned left.
    DelegateInstance* tail = clone_head;
    while (tail->next != nullptr) {
        tail = tail->next;
    }
    tail->next = right;
    return clone_head;
}

void* CHAOS_RUNTIME_ABI_CALL RemoveDelegate(
    void* source_delegate,
    void* target_delegate) {
    if (source_delegate == nullptr || target_delegate == nullptr) return source_delegate;

    auto* src = static_cast<DelegateInstance*>(source_delegate);
    auto* tgt = static_cast<DelegateInstance*>(target_delegate);

    // 1. Find the last matching node in the original chain (without cloning).
    const DelegateInstance* match = nullptr;
    const DelegateInstance* match_prev = nullptr;
    FindLastMatch(src, tgt, &match_prev, &match);

    if (match == nullptr) {
        return source_delegate;
    }

    // 2. Clone the prefix up to (but not including) the match node.
    if (match_prev == nullptr) {
        // match is the head — no prefix to clone, skip directly to match->next
        return match->next;
    }

    auto* clone_head = ClonePrefixTo(src, match);
    // Attach shared suffix (match->next) to the tail of the cloned prefix.
    DelegateInstance* tail = clone_head;
    while (tail->next != nullptr) {
        tail = tail->next;
    }
    tail->next = match->next;
    return clone_head;
}

/// Template helper: dispatch delegate call with `has_target` (closed/static)
/// and `argc` (0-4) to the correct function pointer signature.
template <bool has_target>
static void DfnDispatch(
    void** return_slot,
    void* method_pointer,
    uint32_t argc,
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    void* target_instance,
    void* const* argv) {
    if constexpr (has_target) {
        using Dfn0 = void* (CHAOS_RUNTIME_ABI_CALL*)(RuntimeState*, ThreadState*, void*);
        using Dfn1 = void* (CHAOS_RUNTIME_ABI_CALL*)(RuntimeState*, ThreadState*, void*, void*);
        using Dfn2 = void* (CHAOS_RUNTIME_ABI_CALL*)(RuntimeState*, ThreadState*, void*, void*, void*);
        using Dfn3 = void* (CHAOS_RUNTIME_ABI_CALL*)(RuntimeState*, ThreadState*, void*, void*, void*, void*);
        using Dfn4 = void* (CHAOS_RUNTIME_ABI_CALL*)(RuntimeState*, ThreadState*, void*, void*, void*, void*, void*);
        switch (argc) {
            case 0u: *return_slot = reinterpret_cast<Dfn0>(method_pointer)(runtime_state, thread_state, target_instance); break;
            case 1u: *return_slot = reinterpret_cast<Dfn1>(method_pointer)(runtime_state, thread_state, target_instance, argv[0]); break;
            case 2u: *return_slot = reinterpret_cast<Dfn2>(method_pointer)(runtime_state, thread_state, target_instance, argv[0], argv[1]); break;
            case 3u: *return_slot = reinterpret_cast<Dfn3>(method_pointer)(runtime_state, thread_state, target_instance, argv[0], argv[1], argv[2]); break;
            case 4u: *return_slot = reinterpret_cast<Dfn4>(method_pointer)(runtime_state, thread_state, target_instance, argv[0], argv[1], argv[2], argv[3]); break;
        }
    } else {
        using Dfn0 = void* (CHAOS_RUNTIME_ABI_CALL*)(RuntimeState*, ThreadState*);
        using Dfn1 = void* (CHAOS_RUNTIME_ABI_CALL*)(RuntimeState*, ThreadState*, void*);
        using Dfn2 = void* (CHAOS_RUNTIME_ABI_CALL*)(RuntimeState*, ThreadState*, void*, void*);
        using Dfn3 = void* (CHAOS_RUNTIME_ABI_CALL*)(RuntimeState*, ThreadState*, void*, void*, void*);
        using Dfn4 = void* (CHAOS_RUNTIME_ABI_CALL*)(RuntimeState*, ThreadState*, void*, void*, void*, void*);
        switch (argc) {
            case 0u: *return_slot = reinterpret_cast<Dfn0>(method_pointer)(runtime_state, thread_state); break;
            case 1u: *return_slot = reinterpret_cast<Dfn1>(method_pointer)(runtime_state, thread_state, argv[0]); break;
            case 2u: *return_slot = reinterpret_cast<Dfn2>(method_pointer)(runtime_state, thread_state, argv[0], argv[1]); break;
            case 3u: *return_slot = reinterpret_cast<Dfn3>(method_pointer)(runtime_state, thread_state, argv[0], argv[1], argv[2]); break;
            case 4u: *return_slot = reinterpret_cast<Dfn4>(method_pointer)(runtime_state, thread_state, argv[0], argv[1], argv[2], argv[3]); break;
        }
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
        || argc > 4u
        || out_return_value == nullptr
        || out_return_value_size != sizeof(void*)) {
        return CHAOS_BRIDGE_STATUS_NOT_SUPPORTED;
    }

    if (out_exception != nullptr) {
        *out_exception = nullptr;
    }

    auto* delegate_handle = static_cast<DelegateInstance*>(delegate_instance);
    auto* const return_slot = static_cast<void**>(out_return_value);
    *return_slot = nullptr;

    // Walk the multicast chain; last return value wins (C# semantics).
    try {
        while (delegate_handle != nullptr) {
            if (delegate_handle->method_pointer == nullptr) {
                return CHAOS_BRIDGE_STATUS_NOT_FOUND;
            }

            if (delegate_handle->target_instance != nullptr) {
                DfnDispatch<true>(return_slot, delegate_handle->method_pointer, argc,
                    runtime_state, thread_state, delegate_handle->target_instance, argv);
            } else {
                DfnDispatch<false>(return_slot, delegate_handle->method_pointer, argc,
                    runtime_state, thread_state, nullptr, argv);
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

    if (CHAOS_IL2CPP_STRCMP(icall_name_utf8, kConsoleWriteLineStringIcall) == 0) {
        return reinterpret_cast<void*>(&chaos::il2cpp::support::WriteLineString);
    }

    if (CHAOS_IL2CPP_STRCMP(icall_name_utf8, kStringConcatPairIcall) == 0) {
        return reinterpret_cast<void*>(&chaos::il2cpp::support::ConcatStringPair);
    }

    if (CHAOS_IL2CPP_STRCMP(icall_name_utf8, kStringConcatTripleIcall) == 0) {
        return reinterpret_cast<void*>(&chaos::il2cpp::support::ConcatStringTriple);
    }

    if (CHAOS_IL2CPP_STRCMP(icall_name_utf8, kDelegateCombineIcall) == 0) {
        return reinterpret_cast<void*>(&CombineDelegate);
    }

    if (CHAOS_IL2CPP_STRCMP(icall_name_utf8, kDelegateRemoveIcall) == 0) {
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
    &QueryTypeCapability,
    &ResolveMethodByToken,
    &ResolveFieldByToken,
    &BoxValue,
    &UnboxValue,
    &ResolveVirtualMethod,
    &InvokeVirtual,
    &ResolveVirtualMethodByToken,
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

const RuntimeTypeCapabilityEntryV0* FindRegisteredTypeCapabilityEntry(uint32_t type_token) {
    return FindTypeCapabilityEntryByToken(type_token);
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

void* FindInvokerPointer(uint32_t method_token) {
    return FindInvokerPointerByToken(method_token);
}

memory_domain::DomainId GetAotDomainId() {
    return g_bootstrap_state.aot_domain_id;
}

}  // namespace chaos::il2cpp::bootstrap

extern "C" CHAOS_RUNTIME_ABI_EXPORT const CodegenBridgeV0* CHAOS_RUNTIME_ABI_CALL chaos_codegen_get_bridge_v0(void) {
    return chaos::il2cpp::bootstrap::GetCodegenBridgeV0();
}
