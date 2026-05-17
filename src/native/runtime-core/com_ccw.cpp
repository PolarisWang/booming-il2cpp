#include <cstdlib>
#include <cstring>
#include <chaos/log.h>
#include <chaos/native_types.h>
#include <chaos/type_info.h>
#include <runtime_abi.h>
#include "generated_code_compat.h"
#include "com_ccw.h"

namespace chaos::il2cpp::com_ccw {
namespace {

// All-zero IUnknown GUID.
static const CHAOS_IL2CPP_UINT8 kIidIUnknown[16] = {0};

}  // anonymous namespace

// ── IUnknown method implementations (external linkage for generated code) ──

/// Recover ComCcw* from either a direct CCW pointer or an interface identity pointer.
/// Identity pointers are &ComCcwInterfaceEntry::vtable (the address of the vtable field).
inline ComCcw* ResolveCcw(void* self) noexcept {
    // Try interface-identity path: self is &interfaces[i].vtable.
    // Compute the presumed ComCcwInterfaceEntry address and verify ccw_ptr.
    auto* entry = reinterpret_cast<ComCcwInterfaceEntry*>(
        static_cast<char*>(self) - offsetof(ComCcwInterfaceEntry, vtable));
    if (entry->ccw_ptr != nullptr) {
        auto* ccw = static_cast<ComCcw*>(entry->ccw_ptr);
        // Verify that entry is within ccw->interfaces[].
        if (entry >= ccw->interfaces && entry < ccw->interfaces + kMaxCcwInterfaces) {
            return ccw;
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

    // Scan registered interfaces (entry 0 is always IUnknown).
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
            ccw->refcount++;
            return kS_OK;
        }
    }

    return kE_NOINTERFACE;
}

CHAOS_IL2CPP_UINT32 CHAOS_RUNTIME_ABI_CALL CcwAddRef(void* self) noexcept {
    if (self == nullptr) return 0;
    auto* ccw = ResolveCcw(self);
    return ++ccw->refcount;
}

CHAOS_IL2CPP_UINT32 CHAOS_RUNTIME_ABI_CALL CcwRelease(void* self) noexcept {
    if (self == nullptr) return 0;
    auto* ccw = ResolveCcw(self);
    CHAOS_IL2CPP_UINT32 remaining = --ccw->refcount;
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

        std::free(ccw);
    }
    return remaining;
}

// ── IDispatch helper implementations ──

CHAOS_IL2CPP_INT32 CHAOS_RUNTIME_ABI_CALL CcwGetTypeInfoCount(void* self, CHAOS_IL2CPP_UINT32* pctinfo) noexcept {
    if (pctinfo == nullptr) return kE_POINTER;
    *pctinfo = 0;  // No type info available
    return kS_OK;
}

CHAOS_IL2CPP_INT32 CHAOS_RUNTIME_ABI_CALL CcwGetTypeInfo(void* self, CHAOS_IL2CPP_UINT32 iTInfo, CHAOS_IL2CPP_UINT32 lcid, void** ppTInfo) noexcept {
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

    auto* ccw = static_cast<ComCcw*>(std::malloc(sizeof(ComCcw)));
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

}  // namespace chaos::il2cpp::com_ccw
