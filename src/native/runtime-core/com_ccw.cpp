#include <cstdlib>
#include <cstring>
#include <chaos/log.h>
#include <chaos/native_types.h>
#include <chaos/type_info.h>
#include <runtime_abi.h>
#include "runtime_core.h"
#include "runtime_capability.h"
#include "engine_binding.h"
#include "generated_code_compat.h"
#include "com_ccw.h"
#include "com_typelib.h"
#include "memory_domain.h"

namespace chaos::il2cpp::com_ccw {
namespace {

// All-zero IUnknown GUID.
static const CHAOS_IL2CPP_UINT8 kIidIUnknown[16] = {0};

}  // anonymous namespace

// ── IUnknown method implementations (external linkage for generated code) ──

/// Recover ComCcw* from either a direct CCW pointer or an interface identity pointer.
/// Identity pointers are &ComCcwInterfaceEntry::vtable (the address of the vtable field).
/// Uses address-distance sanity check to avoid dereferencing garbage memory when
/// self is the CCW base pointer (not an interface identity pointer).
inline ComCcw* ResolveCcw(void* self) noexcept {
    // Try interface-identity path: self is &interfaces[i].vtable.
    // Compute the presumed ComCcwInterfaceEntry address and verify ccw_ptr.
    auto* entry = reinterpret_cast<ComCcwInterfaceEntry*>(
        static_cast<char*>(self) - offsetof(ComCcwInterfaceEntry, vtable));
    void* raw_ccw = entry->ccw_ptr;
    if (raw_ccw != nullptr) {
        // Integer-only sanity check before casting: ccw must own entry.
        // A valid ComCcw starts before its interfaces[] array (ccw < entry)
        // and the span is well under 1024 bytes.
        auto e_addr = reinterpret_cast<uintptr_t>(entry);
        auto c_addr = reinterpret_cast<uintptr_t>(raw_ccw);
        constexpr uintptr_t kMaxCcwSpan = 1024u;
        if (c_addr < e_addr && (e_addr - c_addr) < kMaxCcwSpan) {
            return static_cast<ComCcw*>(raw_ccw);
        }
    }
    // Fallback: self is the ComCcw pointer directly.
    return static_cast<ComCcw*>(self);
}

CHAOS_IL2CPP_INT32 CHAOS_RUNTIME_ABI_CALL CcwQueryInterface(
    void* self, const void* iid, void** ppv) noexcept {
    if (self == nullptr || ppv == nullptr) return kE_POINTER;
    auto* ccw = ResolveCcw(self);
    *ppv = nullptr;

    if (iid == nullptr) return kE_POINTER;

    // Aggregation: delegate all QI to the outer controlling IUnknown.
    if (ccw->is_aggregated && ccw->outer_unknown != nullptr) {
        // COM identity rule: QI for IUnknown returns the outer.
        // Non-IUnknown QI is delegated entirely — the outer controls identity.
        auto* outer_vtbl = *static_cast<ComCcwVtbl**>(ccw->outer_unknown);
        return outer_vtbl->QueryInterface(ccw->outer_unknown, iid, ppv);
    }

    // Non-aggregated: scan registered interfaces (entry 0 is always IUnknown).
    for (CHAOS_IL2CPP_SIZE i = 0; i < ccw->interface_count; ++i) {
        if (CHAOS_IL2CPP_MEMCMP(iid, ccw->interfaces[i].guid, 16) == 0) {
            if (i == 0) {
                // IUnknown identity: return self (the CCW object pointer).
                *ppv = ccw;
            } else {
                // Per-interface identity: return &entry.vtable so the caller's
                // first field dereference yields the interface vtable array.
                *ppv = &ccw->interfaces[i].vtable;
            }
            ccw->refcount.fetch_add(1, std::memory_order_relaxed);
            return kS_OK;
        }
    }

    return kE_NOINTERFACE;
}

CHAOS_IL2CPP_UINT32 CHAOS_RUNTIME_ABI_CALL CcwAddRef(void* self) noexcept {
    if (self == nullptr) return 0;
    auto* ccw = ResolveCcw(self);
    if (ccw->is_aggregated && ccw->outer_unknown != nullptr) {
        auto* outer_vtbl = *static_cast<ComCcwVtbl**>(ccw->outer_unknown);
        return outer_vtbl->AddRef(ccw->outer_unknown);
    }
    return ccw->refcount.fetch_add(1, std::memory_order_relaxed) + 1;
}

CHAOS_IL2CPP_UINT32 CHAOS_RUNTIME_ABI_CALL CcwRelease(void* self) noexcept {
    if (self == nullptr) return 0;
    auto* ccw = ResolveCcw(self);
    if (ccw->is_aggregated && ccw->outer_unknown != nullptr) {
        auto* outer_vtbl = *static_cast<ComCcwVtbl**>(ccw->outer_unknown);
        return outer_vtbl->Release(ccw->outer_unknown);
    }
    CHAOS_IL2CPP_UINT32 remaining = ccw->refcount.fetch_sub(1, std::memory_order_acq_rel) - 1;
    if (remaining == 0) {
        CHAOS_IL2CPP_LOG_DEBUG_M("COM", "CCW {0} refcount reached 0, freeing", static_cast<void*>(ccw));

        // Release the GCHandle so the GC can collect the managed object.
        if (ccw->gc_handle != static_cast<CHAOS_IL2CPP_UINT64>(CHAOS_GC_HANDLE_INVALID)) {
            const auto* abi = static_cast<const RuntimeAbiV0*>(chaos_runtime_get_abi_v0());
            if (abi != nullptr && abi->gc_handle_free != nullptr) {
                abi->gc_handle_free(
                    static_cast<RuntimeState*>(ccw->runtime_state),
                    static_cast<GCHandle>(ccw->gc_handle));
            }
        }

        memory_domain::DomainFreeTagged(ccw);
    }
    return remaining;
}

// ── IDispatch helper implementations ──

CHAOS_IL2CPP_INT32 CHAOS_RUNTIME_ABI_CALL CcwGetTypeInfoCount(void* self, CHAOS_IL2CPP_UINT32* pctinfo) noexcept {
    CHAOS_IL2CPP_LOG_WARN_M("COM", "CcwGetTypeInfoCount called — stub (returns 0)");
    if (pctinfo == nullptr) return kE_POINTER;
    *pctinfo = 0;  // No type info available
    return kS_OK;
}

CHAOS_IL2CPP_INT32 CHAOS_RUNTIME_ABI_CALL CcwGetTypeInfo(void* self, CHAOS_IL2CPP_UINT32 iTInfo, CHAOS_IL2CPP_UINT32 lcid, void** ppTInfo) noexcept {
    CHAOS_IL2CPP_LOG_WARN_M("COM", "CcwGetTypeInfo called — stub (returns E_NOTIMPL)");
    if (ppTInfo == nullptr) return kE_POINTER;
    *ppTInfo = nullptr;
    return kE_NOTIMPL;
}

// ── Static vtable ──────────────────────────────────────────────────

namespace {
ComCcwVtbl s_ccw_vtbl = {
    &CcwQueryInterface,
    &CcwAddRef,
    &CcwRelease,
};
}  // anonymous namespace

CHAOS_IL2CPP_INTPTR CreateCcw(void* managed_object, void* runtime_state) noexcept {
    if (managed_object == nullptr) return 0;

    auto* ccw = static_cast<ComCcw*>(memory_domain::DomainCurrentAllocateTagged(sizeof(ComCcw)));
    if (ccw == nullptr) return 0;

    // Allocate a GCHandle to root the managed object.
    CHAOS_IL2CPP_UINT64 gc_handle = static_cast<CHAOS_IL2CPP_UINT64>(CHAOS_GC_HANDLE_INVALID);
    const auto* abi = static_cast<const RuntimeAbiV0*>(chaos_runtime_get_abi_v0());
    if (abi != nullptr && abi->gc_handle_new != nullptr) {
        gc_handle = static_cast<CHAOS_IL2CPP_UINT64>(
            abi->gc_handle_new(
                static_cast<RuntimeState*>(runtime_state),
                managed_object,
                false));  // not pinned
    }

    ccw->vtable = &s_ccw_vtbl;
    ccw->refcount = 1;
    ccw->gc_handle = gc_handle;
    ccw->runtime_state = runtime_state;

    // Initialise interface table: slot 0 = IUnknown.
    ccw->interface_count = 1;
    ccw->interfaces[0].guid = kIidIUnknown;
    ccw->interfaces[0].vtable = &s_ccw_vtbl;
    ccw->interfaces[0].ccw_ptr = ccw;

    // Zero out remaining slots.
    for (CHAOS_IL2CPP_SIZE i = 1; i < kMaxCcwInterfaces; ++i) {
        ccw->interfaces[i].guid = nullptr;
        ccw->interfaces[i].vtable = nullptr;
        ccw->interfaces[i].ccw_ptr = nullptr;
    }

    CHAOS_IL2CPP_LOG_DEBUG_M("COM", "Created CCW {0} for managed object {1} (gc_handle={2})",
                              static_cast<void*>(ccw), managed_object, gc_handle);
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(ccw);
}

CHAOS_IL2CPP_INTPTR CreateCcwAggregated(void* managed_object, void* runtime_state,
                                         void* outer_unknown) noexcept {
    if (managed_object == nullptr || outer_unknown == nullptr) return 0;

    auto* ccw = static_cast<ComCcw*>(memory_domain::DomainCurrentAllocateTagged(sizeof(ComCcw)));
    if (ccw == nullptr) return 0;

    // Allocate a GCHandle to root the managed object.
    CHAOS_IL2CPP_UINT64 gc_handle = static_cast<CHAOS_IL2CPP_UINT64>(CHAOS_GC_HANDLE_INVALID);
    const auto* abi = static_cast<const RuntimeAbiV0*>(chaos_runtime_get_abi_v0());
    if (abi != nullptr && abi->gc_handle_new != nullptr) {
        gc_handle = static_cast<CHAOS_IL2CPP_UINT64>(
            abi->gc_handle_new(
                static_cast<RuntimeState*>(runtime_state),
                managed_object,
                false));  // not pinned
    }

    ccw->vtable = &s_ccw_vtbl;
    ccw->refcount = 1;                      // Inner refcount (not controlling)
    ccw->gc_handle = gc_handle;
    ccw->runtime_state = runtime_state;
    ccw->interface_count = 1;
    ccw->outer_unknown = outer_unknown;
    ccw->is_aggregated = true;

    // interfaces[0] = IUnknown slot (populated for non-QI access).
    ccw->interfaces[0].guid = kIidIUnknown;
    ccw->interfaces[0].vtable = &s_ccw_vtbl;
    ccw->interfaces[0].ccw_ptr = ccw;

    for (CHAOS_IL2CPP_SIZE i = 1; i < kMaxCcwInterfaces; ++i) {
        ccw->interfaces[i].guid = nullptr;
        ccw->interfaces[i].vtable = nullptr;
        ccw->interfaces[i].ccw_ptr = nullptr;
    }

    // Hold a reference on the outer to keep it alive.
    auto* outer_vtbl = *static_cast<ComCcwVtbl**>(outer_unknown);
    outer_vtbl->AddRef(outer_unknown);

    CHAOS_IL2CPP_LOG_DEBUG_M("COM",
        "Created aggregated CCW {0} for managed object {1}, outer={2}",
        static_cast<void*>(ccw), managed_object, outer_unknown);
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(ccw);
}

void DestroyCcw(void* ccw_ptr) noexcept {
    if (ccw_ptr == nullptr) return;
    auto* ccw = static_cast<ComCcw*>(ccw_ptr);

    CHAOS_IL2CPP_LOG_DEBUG_M("COM", "Destroying CCW {0}", ccw_ptr);

    // Release the outer_unknown reference held since creation.
    if (ccw->is_aggregated && ccw->outer_unknown != nullptr) {
        auto* outer_vtbl = *static_cast<ComCcwVtbl**>(ccw->outer_unknown);
        outer_vtbl->Release(ccw->outer_unknown);
    }

    // Release the GCHandle so the GC can collect the managed object.
    if (ccw->gc_handle != static_cast<CHAOS_IL2CPP_UINT64>(CHAOS_GC_HANDLE_INVALID)) {
        const auto* abi = static_cast<const RuntimeAbiV0*>(chaos_runtime_get_abi_v0());
        if (abi != nullptr && abi->gc_handle_free != nullptr) {
            abi->gc_handle_free(
                static_cast<RuntimeState*>(ccw->runtime_state),
                static_cast<GCHandle>(ccw->gc_handle));
        }
    }

    memory_domain::DomainFreeTagged(ccw);
}

bool RegisterCcwInterface(void* ccw_ptr, const CHAOS_IL2CPP_UINT8* guid, void* vtable) noexcept {
    if (ccw_ptr == nullptr || guid == nullptr || vtable == nullptr) return false;
    auto* ccw = static_cast<ComCcw*>(ccw_ptr);

    if (ccw->interface_count >= kMaxCcwInterfaces) return false;

    CHAOS_IL2CPP_SIZE slot = ccw->interface_count++;
    ccw->interfaces[slot].guid = guid;
    ccw->interfaces[slot].vtable = vtable;
    ccw->interfaces[slot].ccw_ptr = ccw;

    CHAOS_IL2CPP_LOG_DEBUG_M("COM", "CCW {0}: registered interface at slot {1}",
                              ccw_ptr, slot);
    return true;
}

void CcwDispatchMethod(void* ccw_ptr, CHAOS_IL2CPP_UINT64 iface_stable_id, CHAOS_IL2CPP_UINT32 method_index) noexcept {
    if (ccw_ptr == nullptr) return;
    auto* ccw = static_cast<ComCcw*>(ccw_ptr);

    // Get the managed object from the GCHandle.
    const auto* abi = static_cast<const RuntimeAbiV0*>(chaos_runtime_get_abi_v0());
    if (abi == nullptr || abi->gc_handle_get == nullptr) return;
    void* obj = abi->gc_handle_get(
        static_cast<RuntimeState*>(ccw->runtime_state),
        static_cast<GCHandle>(ccw->gc_handle));
    if (obj == nullptr) return;

    // Get the type info for the managed object.
    const auto* type_info = chaos_object_get_type_info(obj);
    if (type_info == nullptr) return;

    // Get the warm section for interface map access.
    const auto* warm = reinterpret_cast<const TypeInfoWarm*>(
        reinterpret_cast<const char*>(type_info) + type_info->warm_delta);
    if (warm == nullptr || warm->iface_map == nullptr) return;

    // Walk the interface map to find the target interface.
    for (uint32_t i = 0; i < warm->iface_count; ++i) {
        if (warm->iface_map[i].iface_stable_id == iface_stable_id) {
            if (method_index >= warm->iface_map[i].method_count) return;
            uint32_t vtable_slot = warm->iface_map[i].vtable_offset + method_index;
            if (vtable_slot >= type_info->vtable_length) return;
            auto fn = reinterpret_cast<void (*)(void*)>(
                const_cast<void*>(type_info->vtable_array[vtable_slot]));
            if (fn != nullptr) {
                fn(obj);
            }
            return;
        }
    }
}

CHAOS_IL2CPP_INT32 CHAOS_RUNTIME_ABI_CALL CcwDispatchMethodInvoke(
    void* ccw_ptr, CHAOS_IL2CPP_UINT64 iface_stable_id,
    CHAOS_IL2CPP_INT32 dispIdMember, void* pDispParams, void* pVarResult) noexcept
{
    if (ccw_ptr == nullptr) return ::chaos::il2cpp::com_ccw::kE_POINTER;
    auto* ccw = static_cast<ComCcw*>(ccw_ptr);

    // Validate: TypeLib data must have method info.
    if (ccw->typelib_data == nullptr || dispIdMember < 0 ||
        dispIdMember >= ccw->typelib_data->method_count)
        return ::chaos::il2cpp::com_ccw::kDISP_E_MEMBERNOTFOUND;

    const auto& md = ccw->typelib_data->methods[dispIdMember];

    // Get managed object from GCHandle.
    const auto* abi = static_cast<const RuntimeAbiV0*>(chaos_runtime_get_abi_v0());
    if (abi == nullptr || abi->gc_handle_get == nullptr || abi->method_invoke == nullptr)
        return ::chaos::il2cpp::com_ccw::kE_NOTIMPL;
    void* obj = abi->gc_handle_get(
        static_cast<RuntimeState*>(ccw->runtime_state),
        static_cast<GCHandle>(ccw->gc_handle));
    if (obj == nullptr) return ::chaos::il2cpp::com_ccw::kE_NOTIMPL;

    auto* rs = static_cast<RuntimeState*>(ccw->runtime_state);
    auto* ts = runtime_core::GetCurrentThreadState();
    auto* params = static_cast<com_abi::DISPPARAMS*>(pDispParams);
    uint32_t argc = (params != nullptr) ? params->cArgs : 0;

    // Fast path: no args — use existing CcwDispatchMethod.
    if (argc == 0) {
        CcwDispatchMethod(ccw_ptr, iface_stable_id, static_cast<uint32_t>(dispIdMember));
        return ::chaos::il2cpp::com_ccw::kS_OK;
    }

    // Convert DISPPARAMS to managed object array.
    // Stack-allocate for small arg counts (up to 8).
    void* arg_buffer[8];
    void** managed_args = arg_buffer;
    if (argc > 8) {
        managed_args = static_cast<void**>(std::malloc(argc * sizeof(void*)));
        if (managed_args == nullptr) return static_cast<int32_t>(0x8007000Eu); // E_OUTOFMEMORY
    }

    // DISPPARAMS.rgvarg is in REVERSE order: rgvarg[0] = last parameter.
    // Each VARIANT is 16 bytes (sizeof(VariantLayout)).
    auto* variant_base = static_cast<const char*>(params->rgvarg);
    constexpr uint32_t kVariantSize = 16;
    for (uint32_t i = 0; i < argc; i++) {
        void* variant_ptr = const_cast<char*>(variant_base + (argc - 1 - i) * kVariantSize);
        managed_args[i] = runtime_core::ChaosGetObjectForNativeVariant(
            reinterpret_cast<CHAOS_IL2CPP_INTPTR>(variant_ptr));
    }

    // Call method via method_invoke using metadata token as handle.
    MethodInfoHandle method_handle = static_cast<MethodInfoHandle>(md.method_token);
    uint64_t ret_buf[8] = {};  // 64-byte return buffer (covers SIMD types)
    ExceptionHandle ex = nullptr;

    RuntimeStatus status = abi->method_invoke(
        rs, ts, method_handle, obj,
        const_cast<void* const*>(managed_args), argc,
        ret_buf, sizeof(ret_buf), &ex);

    if (managed_args != arg_buffer) {
        std::free(managed_args);
    }

    // Set return value as VARIANT.
    if (pVarResult != nullptr && status == CHAOS_RUNTIME_STATUS_OK) {
        runtime_core::ChaosGetNativeVariantForObject(
            reinterpret_cast<void*>(ret_buf[0]),
            reinterpret_cast<CHAOS_IL2CPP_INTPTR>(pVarResult),
            1); // destroy_old = true
    }

    if (status == CHAOS_RUNTIME_STATUS_MANAGED_EXCEPTION) {
        // Store exception info in pVarResult as VT_ERROR + scode if available.
        if (pVarResult != nullptr) {
            // Zero out the 16-byte VARIANT, then write vt=VT_ERROR(0x0A) at [0]
            // and scode=DISP_E_EXCEPTION(0x80020009) at [8].
            CHAOS_IL2CPP_MEMSET(pVarResult, 0, kVariantSize);
            uint16_t vt_error = 0x0Au;
            CHAOS_IL2CPP_MEMCPY(pVarResult, &vt_error, sizeof(uint16_t));
            int32_t scode = 0x80020009;
            CHAOS_IL2CPP_MEMCPY(static_cast<char*>(pVarResult) + 8, &scode, sizeof(scode));
        }
        return 0x80020009; // DISP_E_EXCEPTION
    }

    return ::chaos::il2cpp::com_ccw::kS_OK;
}
}  // namespace chaos::il2cpp::com_ccw
