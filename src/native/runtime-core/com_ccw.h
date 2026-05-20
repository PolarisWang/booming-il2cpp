#ifndef CHAOS_IL2CPP_COM_CCW_H_
#define CHAOS_IL2CPP_COM_CCW_H_

#include <cstdint>
#include <cstddef>
#include <atomic>
#include <chaos/native_types.h>
#include "com_abi.h"

namespace chaos::il2cpp::com_ccw {

// ── Constants ────────────────────────────────────────────────────────

/// Maximum number of COM interface vtables a single CCW can expose
/// (IUnknown + up to 3 additional interfaces for V2).
constexpr CHAOS_IL2CPP_SIZE kMaxCcwInterfaces = 4;

// HRESULT constants (Win32 COM ABI).
constexpr CHAOS_IL2CPP_INT32 kS_OK                 = 0;
constexpr CHAOS_IL2CPP_INT32 kE_NOINTERFACE         = static_cast<CHAOS_IL2CPP_INT32>(0x80004002u);
constexpr CHAOS_IL2CPP_INT32 kE_NOTIMPL             = static_cast<CHAOS_IL2CPP_INT32>(0x80004001u);
constexpr CHAOS_IL2CPP_INT32 kE_POINTER             = static_cast<CHAOS_IL2CPP_INT32>(0x80004003u);
constexpr CHAOS_IL2CPP_INT32 kDISP_E_MEMBERNOTFOUND  = static_cast<CHAOS_IL2CPP_INT32>(0x80020003u);
constexpr CHAOS_IL2CPP_INT32 kDispIdStart            = 0;

// ── Data structures ───────────────────────────────────────────────────

/// A single registered COM interface on a CCW: GUID + vtable pointer.
/// identity: QI returns &entry->vtable (the address of the vtable field).
/// Thunks recover the owning ComCcw via entry->ccw_ptr.
struct ComCcwInterfaceEntry {
    const CHAOS_IL2CPP_UINT8* guid;   // 16-byte interface GUID
    void* vtable;                       // Interface-specific vtable (IUnknown-compatible layout)
    void* ccw_ptr;                      // Back-pointer to owning ComCcw
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
    std::atomic<CHAOS_IL2CPP_UINT32> refcount{};  // External COM reference count (atomic for thread safety)
    CHAOS_IL2CPP_UINT64 gc_handle;              // GCHandle keeping the managed object alive
    void* runtime_state;                        // RuntimeState* for GC handle lifecycle
    CHAOS_IL2CPP_SIZE interface_count;           // Number of registered interfaces
    ComCcwInterfaceEntry interfaces[kMaxCcwInterfaces]; // GUID→vtable map (entry 0 = IUnknown)
};

// ── IUnknown vtable function declarations (external linkage for generated code) ──
CHAOS_IL2CPP_INT32 CHAOS_RUNTIME_ABI_CALL CcwQueryInterface(
    void* self, const void* iid, void** ppv) noexcept;
CHAOS_IL2CPP_UINT32 CHAOS_RUNTIME_ABI_CALL CcwAddRef(void* self) noexcept;
CHAOS_IL2CPP_UINT32 CHAOS_RUNTIME_ABI_CALL CcwRelease(void* self) noexcept;

// ── IDispatch helper declarations (external linkage for generated code) ──
CHAOS_IL2CPP_INT32 CHAOS_RUNTIME_ABI_CALL CcwGetTypeInfoCount(void* self, CHAOS_IL2CPP_UINT32* pctinfo) noexcept;
CHAOS_IL2CPP_INT32 CHAOS_RUNTIME_ABI_CALL CcwGetTypeInfo(void* self, CHAOS_IL2CPP_UINT32 iTInfo, CHAOS_IL2CPP_UINT32 lcid, void** ppTInfo) noexcept;

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

/// Recover the ComCcw pointer from an interface pointer returned by QI.
/// The interface pointer is the address of ComCcwInterfaceEntry::vtable.
/// Uses the ccw_ptr back-pointer stored at registration time.
inline ComCcw* CcwFromInterface(void* iface_ptr) noexcept {
    if (iface_ptr == nullptr) return nullptr;
    auto* entry = reinterpret_cast<ComCcwInterfaceEntry*>(
        static_cast<char*>(iface_ptr) - offsetof(ComCcwInterfaceEntry, vtable));
    return static_cast<ComCcw*>(entry->ccw_ptr);
}

/// Dispatch a COM interface method call on a CCW's managed object.
/// Uses the managed object's type info to find the interface implementation
/// via the iface_map in TypeInfoWarm.
/// @param ccw_ptr           The CCW pointer.
/// @param iface_stable_id   Stable ID (FNV-1a hash) of the COM interface.
/// @param method_index      Index of the interface method (0 = first after IUnknown).
void CcwDispatchMethod(void* ccw_ptr, CHAOS_IL2CPP_UINT64 iface_stable_id, CHAOS_IL2CPP_UINT32 method_index) noexcept;

}  // namespace chaos::il2cpp::com_ccw

#endif  // CHAOS_IL2CPP_COM_CCW_H_
