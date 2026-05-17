#ifndef CHAOS_IL2CPP_COM_CCW_H_
#define CHAOS_IL2CPP_COM_CCW_H_

#include <cstdint>
#include <cstddef>
#include <chaos/native_types.h>
#include "com_abi.h"

namespace chaos::il2cpp::com_ccw {

// ── Constants ────────────────────────────────────────────────────────

/// Maximum number of COM interface vtables a single CCW can expose
/// (IUnknown + up to 3 additional interfaces for V2).
constexpr CHAOS_IL2CPP_SIZE kMaxCcwInterfaces = 4;

// ── Data structures ───────────────────────────────────────────────────

/// A single registered COM interface on a CCW: GUID + vtable pointer.
struct ComCcwInterfaceEntry {
    const CHAOS_IL2CPP_UINT8* guid;   // 16-byte interface GUID
    void* vtable;                       // Interface-specific vtable (IUnknown-compatible layout)
};

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
    ComCcwVtbl* vtable;                         // Points to s_ccw_vtbl
    CHAOS_IL2CPP_UINT32 refcount;               // External COM reference count
    CHAOS_IL2CPP_UINT64 gc_handle;              // GCHandle keeping the managed object alive
    void* runtime_state;                        // RuntimeState* for GC handle lifecycle
    CHAOS_IL2CPP_SIZE interface_count;           // Number of registered interfaces
    ComCcwInterfaceEntry interfaces[kMaxCcwInterfaces]; // GUID→vtable map (entry 0 = IUnknown)
};

// ── CCW lifecycle ──────────────────────────────────────────────────

/// Create a CCW for a managed object.
/// Allocates a GCHandle to root the managed object.
/// Returns the CCW pointer (IUnknown*), or 0 on failure.
/// The CCW is allocated via std::malloc and freed when refcount reaches 0.
CHAOS_IL2CPP_INTPTR CreateCcw(void* managed_object, void* runtime_state) noexcept;

/// Register an additional COM interface on an existing CCW.
/// @param ccw_ptr      The CCW pointer returned by CreateCcw.
/// @param guid         16-byte interface GUID.
/// @param vtable       Interface vtable pointer (must be a static vtable).
/// @return true if registered, false if the interface table is full or ccw_ptr is null.
bool RegisterCcwInterface(void* ccw_ptr, const CHAOS_IL2CPP_UINT8* guid, void* vtable) noexcept;

}  // namespace chaos::il2cpp::com_ccw

#endif  // CHAOS_IL2CPP_COM_CCW_H_
