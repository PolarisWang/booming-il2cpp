#include <cstdlib>
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

// ── IUnknown method implementations ────────────────────────────────

CHAOS_IL2CPP_INT32 CHAOS_RUNTIME_ABI_CALL CcwQueryInterface(
    void* self, const void* iid, void** ppv) noexcept {
    if (self == nullptr || ppv == nullptr) return kE_POINTER;
    auto* ccw = static_cast<ComCcw*>(self);
    *ppv = nullptr;

    // V1: only support IID_IUnknown (all-zero GUID).
    // Full implementation would check for specific interface GUIDs and
    // return the appropriate vtable.
    static const CHAOS_IL2CPP_UINT8 kIidIUnknown[16] = {0};
    if (iid != nullptr && CHAOS_IL2CPP_MEMCMP(iid, kIidIUnknown, 16) == 0) {
        *ppv = self;
        ccw->refcount++;
        return kS_OK;
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
        // V1: GCHandle cleanup requires the GC API.
        // Future: release the GCHandle to allow GC of the managed object.
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

    ccw->vtable = &s_ccw_vtbl;
    ccw->refcount = 1;
    ccw->managed_object_gchandle = managed_object;  // V1: direct pointer, no GCHandle
    ccw->runtime_state = runtime_state;

    CHAOS_IL2CPP_LOG_DEBUG_M("COM", "Created CCW {0} for managed object {1}",
                              static_cast<void*>(ccw), managed_object);
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(ccw);
}

}  // namespace chaos::il2cpp::com_ccw
