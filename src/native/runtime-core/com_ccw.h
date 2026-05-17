#ifndef CHAOS_IL2CPP_COM_CCW_H_
#define CHAOS_IL2CPP_COM_CCW_H_

#include <cstdint>
#include <chaos/native_types.h>
#include "com_abi.h"

namespace chaos::il2cpp::com_ccw {

// ── Data structures ────────────────────────────────────────────────

/// CCW vtable — IUnknown only for V1.
/// Extended vtables (for specific COM interfaces) require codegen
/// thunks and are not yet emitted.
struct ComCcwVtbl {
    CHAOS_IL2CPP_INT32 (*QueryInterface)(void* self, const void* iid, void** ppv);
    CHAOS_IL2CPP_UINT32 (*AddRef)(void* self);
    CHAOS_IL2CPP_UINT32 (*Release)(void* self);
};

/// Native CCW object — exposes a managed object as a COM IUnknown.
/// Layout: first field is vtable pointer (true COM object layout).
/// The identity_unknown of any RCW wrapping this CCW will equal
/// the CCW pointer itself (COM identity rule).
struct ComCcw {
    ComCcwVtbl* vtable;                 // Points to s_ccw_vtbl
    CHAOS_IL2CPP_UINT32 refcount;       // External COM reference count
    void* managed_object_gchandle;      // GC handle keeping the managed object alive
    void* runtime_state;                // RuntimeState* for managed dispatch
};

// ── CCW lifecycle ──────────────────────────────────────────────────

/// Create a CCW for a managed object.
/// Returns the CCW pointer (IUnknown*), or 0 on failure.
/// The CCW is allocated via std::malloc and freed when refcount reaches 0.
CHAOS_IL2CPP_INTPTR CreateCcw(void* managed_object, void* runtime_state) noexcept;

}  // namespace chaos::il2cpp::com_ccw

#endif  // CHAOS_IL2CPP_COM_CCW_H_
