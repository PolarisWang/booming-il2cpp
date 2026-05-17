#include <cstdlib>
#include <cstring>
#include <chaos/log.h>
#include <chaos/native_types.h>
#include <runtime_abi.h>
#include "com_ccw.h"

namespace chaos::il2cpp::com_ccw {
namespace {

// Minimal HRESULT constants (Win32 COM ABI).
constexpr CHAOS_IL2CPP_INT32 kS_OK          = 0;
constexpr CHAOS_IL2CPP_INT32 kE_NOINTERFACE = static_cast<CHAOS_IL2CPP_INT32>(0x80004002u);
constexpr CHAOS_IL2CPP_INT32 kE_POINTER     = static_cast<CHAOS_IL2CPP_INT32>(0x80004003u);

// All-zero IUnknown GUID.
static const CHAOS_IL2CPP_UINT8 kIidIUnknown[16] = {0};

// ── IUnknown method implementations ────────────────────────────────

CHAOS_IL2CPP_INT32 CHAOS_RUNTIME_ABI_CALL CcwQueryInterface(
    void* self, const void* iid, void** ppv) noexcept {
    if (self == nullptr || ppv == nullptr) return kE_POINTER;
    auto* ccw = static_cast<ComCcw*>(self);
    *ppv = nullptr;

    if (iid == nullptr) return kE_POINTER;

    // Scan registered interfaces (entry 0 is always IUnknown).
    for (CHAOS_IL2CPP_SIZE i = 0; i < ccw->interface_count; ++i) {
        if (CHAOS_IL2CPP_MEMCMP(iid, ccw->interfaces[i].guid, 16) == 0) {
            // For IUnknown identity rule: always return `self` for IUnknown QI.
            // For other interfaces, return the CCW pointer (cast to the interface vtable).
            // V2 note: when codegen emits interface-specific vtables, the CCW
            // may need per-interface identity pointers. For now, return self.
            *ppv = self;
            ccw->refcount++;
            return kS_OK;
        }
    }

    return kE_NOINTERFACE;
}

CHAOS_IL2CPP_UINT32 CHAOS_RUNTIME_ABI_CALL CcwAddRef(void* self) noexcept {
    if (self == nullptr) return 0;
    auto* ccw = static_cast<ComCcw*>(self);
    return ++ccw->refcount;
}

CHAOS_IL2CPP_UINT32 CHAOS_RUNTIME_ABI_CALL CcwRelease(void* self) noexcept {
    if (self == nullptr) return 0;
    auto* ccw = static_cast<ComCcw*>(self);
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

// ── Static vtable ──────────────────────────────────────────────────

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

    // Zero out remaining slots.
    for (CHAOS_IL2CPP_SIZE i = 1; i < kMaxCcwInterfaces; ++i) {
        ccw->interfaces[i].guid = nullptr;
        ccw->interfaces[i].vtable = nullptr;
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

    CHAOS_IL2CPP_LOG_DEBUG_M("COM", "CCW {0}: registered interface at slot {1}",
                              ccw_ptr, slot);
    return true;
}

}  // namespace chaos::il2cpp::com_ccw
