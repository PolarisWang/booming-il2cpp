// ── ABI exports: extern "C" linkage ──
// These functions must use C linkage to be callable from managed/NativeAOT stubs.
// (No C++ name mangling on the exported symbol.)

#include "bootstrap.h"
#include "bootstrap_internal.h"

#include "generic_context.h"
#include "memory_domain.h"
#include "metadata_interface.h"
#include "method_replacement.h"
#include "reflection_query_model.h"
#include "module_registry.h"
#include "runtime_core.h"
// NOTE: previously collided with contracts/native/v0/runtime_instantiation.h (C bridge).
// runtime-core renamed to instantiation_engine.h to resolve the collision.
#include "instantiation_engine.h"   // RegisterMethodAotEntries
#include "string_table.h"
#include "support.h"
#include "vtable_registry.h"

#include "t4_seh_handler.h"

#include "gc/gc_old_gen.h"
#include "gc/gc_root_scanner.h"

#include <cstdint>
#include <cstring>
#include <chaos/unordered_dense.h>

// Declared in native-aot.generated.cpp — must be at file scope (MSVC rejects
// extern "C" inside function body: C2598).
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module;

// Declared in interpreter_entry.cpp — file scope for the same reason.
extern "C" void ChaosResolveExternalRuntimeFnTable() noexcept;

namespace chaos::il2cpp::bootstrap {

namespace {


struct UnresolvedVirtualCallEntry {
    CHAOS_IL2CPP_UINT32 instance_type_token;
    CHAOS_IL2CPP_UINT32 declared_method_token;
    void* resolved_method;
};

struct MethodPointerEntry {
    CHAOS_IL2CPP_UINT32 method_token;
    void* method_pointer;
};

struct InvokerEntry {
    CHAOS_IL2CPP_UINT32 method_token;
    void* invoker_pointer;
};

BootstrapState g_bootstrap_state = {};/// Invoker pointer table built at RegisterCodegen time from the
/// code_registration->invoker_pointers array.
/// Keyed by method_token, used by MethodInvoke to dispatch calls correctly
/// regardless of whether the caller passes an opaque token, a reflection-query
/// handle, or a raw function pointer.
static CHAOS_IL2CPP_UNORDERED_DENSE_MAP(CHAOS_IL2CPP_UINT32, void*) g_invoker_table;

template <typename THandle>
THandle MakeOpaqueHandle(CHAOS_IL2CPP_UINT32 token) {
    return static_cast<THandle>(static_cast<CHAOS_IL2CPP_UINTPTR>(token));
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
    return g_bootstrap_state.is_bootstrapped && image != 0;
}

const MethodPointerEntry* GetMethodPointerEntries() {
    if (g_bootstrap_state.code_registration == nullptr
        || g_bootstrap_state.code_registration->method_pointers == nullptr
        || g_bootstrap_state.code_registration->method_pointer_count == 0u) {
        return nullptr;
    }

    return static_cast<const MethodPointerEntry*>(g_bootstrap_state.code_registration->method_pointers);
}

void* FindInvokerPointerByToken(CHAOS_IL2CPP_UINT32 method_token) {
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
    CHAOS_IL2CPP_UINT32 instance_type_token,
    CHAOS_IL2CPP_UINT32 declared_method_token) {
    const auto* entries = GetUnresolvedVirtualCallEntries();
    if (entries == nullptr) {
        return nullptr;
    }

    for (CHAOS_IL2CPP_UINT32 index = 0u; index < g_bootstrap_state.code_registration->unresolved_virtual_call_count; index++) {
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
    if (method == 0) return false;

    // Accept any method pointer that came from the vtable registry.
    // (vtable registry stores raw function pointers, not opaque tokens.)
    // We distinguish vtable pointers from token handles by checking the token range:
    // tokens are <= 0x0FFFFFFF; real function pointers are usually larger addresses.
    const CHAOS_IL2CPP_UINTPTR raw = static_cast<CHAOS_IL2CPP_UINTPTR>(method);
    if (raw > static_cast<CHAOS_IL2CPP_UINTPTR>(0x0FFFFFFFu)) {
        // Likely a real function pointer from vtable registry or unresolved-virtual table.
        return true;
    }

    // Also check the legacy unresolved-virtual table.
    const auto* entries = GetUnresolvedVirtualCallEntries();
    if (entries == nullptr) {
        return false;
    }

    for (CHAOS_IL2CPP_UINT32 index = 0u; index < g_bootstrap_state.code_registration->unresolved_virtual_call_count; index++) {
        if (entries[index].resolved_method == reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(method))) {
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
        for (CHAOS_IL2CPP_UINT32 index = 0u; index < code_registration->invoker_pointer_count; index++) {
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

    // Register T4 SEH vectored exception handler for native code exception dispatch.
    chaos::il2cpp::codegen::RegisterT4SehHandler();

    // ── Register AOT generic type instantiations and method contexts ──
    //
    // The codegen emits GenericTypeRegistrationEntryV0[] and
    // GenericMethodRegistrationEntryV0[] arrays into the MetadataRegistrationV0.
    // We iterate them here, resolve tokens → handles, and populate the
    // GenericContextRegistry (Gen-2 Handle scheme).
    //
    // The source_image is required for token→handle resolution. When no
    // explicit ImageHandle has been provided (aot_image_handle == nullptr),
    // we fall back to MakeOpaqueHandle-style token handles so that basic
    // metadata queries still function during early bootstrap.
    if (g_bootstrap_state.metadata_registration != nullptr
        && g_bootstrap_state.metadata_registration->generic_type_count > 0u)
    {
        ModuleGenericRegistrationV0 aot_reg;
        aot_reg.struct_size         = sizeof(ModuleGenericRegistrationV0);
        aot_reg.module_id           = 0u;          // AOT root module
        aot_reg.module_name_utf8    = g_bootstrap_state.options != nullptr
            ? g_bootstrap_state.options->image_name_utf8 : nullptr;
        aot_reg.source_image        = g_bootstrap_state.aot_image_handle;
        aot_reg.generic_types       = static_cast<const GenericTypeRegistrationEntryV0*>(
            g_bootstrap_state.metadata_registration->generic_types);
        aot_reg.generic_type_count  = g_bootstrap_state.metadata_registration->generic_type_count;
        aot_reg.generic_type_args   = static_cast<const uint32_t*>(
            g_bootstrap_state.metadata_registration->generic_type_args);
        aot_reg.generic_type_arg_count = g_bootstrap_state.metadata_registration->generic_type_arg_count;
        aot_reg.generic_methods     = static_cast<const GenericMethodRegistrationEntryV0*>(
            g_bootstrap_state.metadata_registration->generic_methods);
        aot_reg.generic_method_count = g_bootstrap_state.metadata_registration->generic_method_count;
        aot_reg.generic_method_args = static_cast<const uint32_t*>(
            g_bootstrap_state.metadata_registration->generic_method_args);
        aot_reg.generic_method_arg_count = g_bootstrap_state.metadata_registration->generic_method_arg_count;

        aot_reg.method_aot_entries = static_cast<const GenericMethodAotEntryV0*>(
            g_bootstrap_state.metadata_registration->method_aot_entries);
        aot_reg.method_aot_entry_count = g_bootstrap_state.metadata_registration->method_aot_entry_count;
        aot_reg.method_aot_entry_args = static_cast<const uint32_t*>(
            g_bootstrap_state.metadata_registration->method_aot_entry_args);
        aot_reg.method_aot_entry_arg_count = g_bootstrap_state.metadata_registration->method_aot_entry_arg_count;

        // Route both hotpatch dispatch table and generic data through
        // ModuleLifecycleManager for unified registration.
        chaos::il2cpp::runtime_core::ModuleLifecycleManager::Get()->RegisterAotModuleData(
            chaos_il2cpp_aot_hotpatch_module,
            &aot_reg);

        // Register AOT method entries for runtime QueryAotMethod.
        if (aot_reg.method_aot_entry_count > 0u) {
            chaos::il2cpp::runtime_instantiation::RegisterMethodAotEntries(
                aot_reg.module_id,
                aot_reg.method_aot_entries,
                aot_reg.method_aot_entry_count,
                aot_reg.method_aot_entry_args,
                aot_reg.method_aot_entry_arg_count);
        }
    }

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

    // Resolve the external runtime dispatch table (kChaosExternalRuntimeFnTable).
    // This iterates the codegen-emitted subjectId array and resolves each entry
    // to its direct function pointer via the hotpatch name registry, which has
    // just been populated with all registered modules' dispatch tables.
    // Must happen AFTER RegisterHotpatchModule, BEFORE is_bootstrapped = true.
    ChaosResolveExternalRuntimeFnTable();

    // ── Register AOT VTable descriptors ─────────────────────────────
    // Populates both TypeVTable indexes (by_type_token, by_stable_id)
    // and flat_vtable arrays so that ResolveVirtualMethodPointer works
    // for AOT types (not just interface dispatch).  Must happen after
    // hotpatch registration so that tokens can be resolved.
    if (g_bootstrap_state.code_registration->vtable_descriptors != nullptr &&
        g_bootstrap_state.code_registration->vtable_descriptor_count > 0u) {
        for (uint32_t i = 0u; i < g_bootstrap_state.code_registration->vtable_descriptor_count; ++i) {
            vtable_registry::RegisterCodegenVTable(
                &g_bootstrap_state.code_registration->vtable_descriptors[i]);
        }
    }

    // The AOT module registers its string table via a static initializer in the
    // generated translation unit.  Nothing to do here — g_aot_entries defaults
    // to nullptr, which makes Resolve() fall through to the dynamic table only.

    // ── Register GC slot maps from the AOT codegen section ─────────────
    // The generated C++ emits a contiguous kChaosGcSlotMapsSection with
    // GcSlotMapSectionEntryHdrV0-prefixed entries.  Register them here so
    // that GcScanFrameHybrid can find precise slot maps for AOT frames.
    if (g_bootstrap_state.code_registration->slot_map_section_begin != nullptr &&
        g_bootstrap_state.code_registration->slot_map_section_end != nullptr &&
        g_bootstrap_state.code_registration->slot_map_section_begin !=
            g_bootstrap_state.code_registration->slot_map_section_end) {
        chaos::il2cpp::runtime_core::GcRegisterSlotMapsFromSection(
            g_bootstrap_state.code_registration->slot_map_section_begin,
            g_bootstrap_state.code_registration->slot_map_section_end);
    }

    // ── Ensure aot_image_handle is set ─────────────────────────────────
    // Used downstream by ApplyPatchFromMemory → SetAotBridge, and by
    // BootstrapRuntime itself (source_image for generic context registration).
    // When the host (e.g. game engine) has already set aot_image_handle via
    // BootstrapState, we keep that.  Otherwise, discover it from the first
    // registered module that has types loaded.
    if (g_bootstrap_state.aot_image_handle == 0) {
        for (uint32_t mid = 0; mid < chaos::il2cpp::runtime_core::kMaxModules; ++mid) {
            const auto* module = chaos::il2cpp::runtime_core::LookupModule(mid);
            if (module != nullptr && !module->tombstone &&
                module->image != nullptr && module->type_count > 0) {
                g_bootstrap_state.aot_image_handle =
                    chaos::il2cpp::runtime_core::EncodeReflectionQueryImageHandle(module->image);
                break;
            }
        }
    }

    g_bootstrap_state.is_bootstrapped = true;

    return CHAOS_BRIDGE_STATUS_OK;
}

TypeInfoHandle CHAOS_RUNTIME_ABI_CALL ResolveTypeByToken(
    ImageHandle image,
    CHAOS_IL2CPP_UINT32 type_token) {
    if (!IsBootstrapped(image) || type_token == 0u) {
        return 0;
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

const RuntimeTypeCapabilityEntryV0* FindTypeCapabilityEntryByToken(CHAOS_IL2CPP_UINT32 type_token) {
    const auto* entries = GetTypeCapabilityEntries();
    if (entries == nullptr || type_token == 0u) {
        return nullptr;
    }

    for (CHAOS_IL2CPP_UINT32 index = 0u; index < g_bootstrap_state.code_registration->type_capability_count; ++index) {
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
    CHAOS_IL2CPP_UINT32 method_token) {
    if (!IsBootstrapped(image) || method_token == 0u) {
        return 0;
    }

    if (const auto* reflection_image = chaos::il2cpp::runtime_core::TryDecodeReflectionQueryImageHandle(image)) {
        const auto* method = chaos::il2cpp::runtime_core::FindReflectionQueryMethodByToken(reflection_image, method_token);
        return chaos::il2cpp::runtime_core::EncodeReflectionQueryMethodHandle(method);
    }

    return MakeOpaqueHandle<MethodInfoHandle>(method_token);
}

FieldInfoHandle CHAOS_RUNTIME_ABI_CALL ResolveFieldByToken(
    ImageHandle image,
    CHAOS_IL2CPP_UINT32 field_token) {
    if (!IsBootstrapped(image) || field_token == 0u) {
        return 0;
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
    CHAOS_IL2CPP_SIZE value_size) {
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
    CHAOS_IL2CPP_SIZE out_value_size) {
    return chaos::il2cpp::runtime_core::UnboxValueObject(
        runtime_state,
        boxed_object,
        out_value,
        out_value_size) == CHAOS_RUNTIME_STATUS_OK
        ? CHAOS_BRIDGE_STATUS_OK
        : CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;
}

static CHAOS_IL2CPP_UINT32 DecodeVirtualMethodToken(MethodInfoHandle method) {
    if (const auto* desc = chaos::il2cpp::runtime_core::TryDecodeReflectionQueryMethodHandle(method)) {
        return desc->metadata_token;
    }
    return DecodeOpaqueToken(static_cast<CHAOS_IL2CPP_UINTPTR>(method));
}

static CHAOS_IL2CPP_UINT32 DecodeVirtualTypeToken(TypeInfoHandle type) {
    if (const auto* desc = chaos::il2cpp::runtime_core::TryDecodeReflectionQueryTypeHandle(type)) {
        return desc->metadata_token;
    }
    return DecodeOpaqueToken(static_cast<CHAOS_IL2CPP_UINTPTR>(type));
}

MethodInfoHandle CHAOS_RUNTIME_ABI_CALL ResolveVirtualMethod(
    TypeInfoHandle instance_type,
    MethodInfoHandle declared_method) {
    if (instance_type == 0 || declared_method == 0) {
        return 0;
    }

    const CHAOS_IL2CPP_UINT32 instance_type_token  = DecodeVirtualTypeToken(instance_type);
    const CHAOS_IL2CPP_UINT32 declared_method_token = DecodeVirtualMethodToken(declared_method);

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
    CHAOS_IL2CPP_UINT32 instance_type_token,
    CHAOS_IL2CPP_UINT32 declared_method_token) {
    // 1. Try vtable registry (runtime-registered per-type vtables).
    if (void* vtable_ptr = chaos::il2cpp::vtable_registry::ResolveVirtualMethodPointer(
            instance_type_token, declared_method_token)) {
        return reinterpret_cast<MethodInfoHandle>(vtable_ptr);
    }

    // 2. Fall back to the pre-resolved unresolved-virtual-call table.
    if (const auto* entry = FindUnresolvedVirtualCallEntry(instance_type_token, declared_method_token)) {
        return reinterpret_cast<MethodInfoHandle>(entry->resolved_method);
    }

    return 0;
}

MethodInfoHandle CHAOS_RUNTIME_ABI_CALL ResolveVirtualMethodByToken(
    CHAOS_IL2CPP_UINT32 instance_type_token,
    CHAOS_IL2CPP_UINT32 declared_method_token) {
    if (instance_type_token == 0u || declared_method_token == 0u) {
        return 0;
    }
    return ResolveVirtualMethodTokenImpl(instance_type_token, declared_method_token);
}

BridgeStatus CHAOS_RUNTIME_ABI_CALL InvokeVirtual(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    void* object_instance,
    MethodInfoHandle method,
    void* const* argv,
    CHAOS_IL2CPP_UINT32 argc,
    void* out_return_value,
    CHAOS_IL2CPP_SIZE out_return_value_size,
    ExceptionHandle* out_exception) {
    if (runtime_state == nullptr
        || thread_state == nullptr
        || object_instance == nullptr
        || method == 0
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
    } catch (const chaos_managed_exception& carrier) {
        if (out_exception != nullptr) {
            *out_exception = reinterpret_cast<ExceptionHandle>(carrier.object_value);
        }

        return CHAOS_BRIDGE_STATUS_MANAGED_EXCEPTION;
    }
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

CHAOS_IL2CPP_UINT32 DecodeOpaqueToken(CHAOS_IL2CPP_UINTPTR handle) {
    return static_cast<CHAOS_IL2CPP_UINT32>(handle);
}

const BootstrapState* PeekBootstrapState() {
    return &g_bootstrap_state;
}

const CodegenBridgeV0* GetCodegenBridgeV0() {
    return &kCodegenBridgeV0;
}

const RuntimeTypeCapabilityEntryV0* FindRegisteredTypeCapabilityEntry(CHAOS_IL2CPP_UINT32 type_token) {
    return FindTypeCapabilityEntryByToken(type_token);
}

void* FindMethodPointerByToken(CHAOS_IL2CPP_UINT32 method_token) {
    if (void* replacement = chaos::il2cpp::method_replacement::Resolve(method_token)) {
        return replacement;
    }

    const auto* entries = GetMethodPointerEntries();
    if (entries == nullptr) {
        return nullptr;
    }

    for (CHAOS_IL2CPP_UINT32 index = 0u; index < g_bootstrap_state.code_registration->method_pointer_count; index++) {
        if (entries[index].method_token == method_token) {
            return entries[index].method_pointer;
        }
    }

    return nullptr;
}

void* FindInvokerPointer(CHAOS_IL2CPP_UINT32 method_token) {
    return FindInvokerPointerByToken(method_token);
}

memory_domain::DomainId GetAotDomainId() {
    return g_bootstrap_state.aot_domain_id;
}

}  // namespace chaos::il2cpp::bootstrap

extern "C" CHAOS_RUNTIME_ABI_EXPORT const CodegenBridgeV0* CHAOS_RUNTIME_ABI_CALL chaos_codegen_get_bridge_v0(void) {
    return chaos::il2cpp::bootstrap::GetCodegenBridgeV0();
}
