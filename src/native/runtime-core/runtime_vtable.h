#ifndef CHAOS_IL2CPP_RUNTIME_VTABLE_H_
#define CHAOS_IL2CPP_RUNTIME_VTABLE_H_

// ── Forwarding header for backward compatibility ─────────────────────────
//
// All runtime_vtable functionality has been merged into vtable_registry.
// These aliases allow existing code (including codegen-emitted calls) to
// compile without changes.
//
// New code should use chaos::il2cpp::vtable_registry directly.

#include "vtable_registry.h"

namespace chaos::il2cpp::runtime_vtable {

inline void RegisterVTable(CHAOS_IL2CPP_UINT64 stable_id,
                            const void** vtable,
                            CHAOS_IL2CPP_UINT32 length) noexcept {
    vtable_registry::RegisterVTableArray(stable_id, vtable, length);
}

inline const void** FindVTable(CHAOS_IL2CPP_UINT64 stable_id) noexcept {
    return vtable_registry::FindVTable(stable_id);
}

inline CHAOS_IL2CPP_UINT32 FindVTableLength(CHAOS_IL2CPP_UINT64 stable_id) noexcept {
    return vtable_registry::FindVTableLength(stable_id);
}

inline const void** BuildRuntimeVTable(CHAOS_IL2CPP_UINT64 type_stable_id,
                                        CHAOS_IL2CPP_UINT64 base_stable_id) noexcept {
    return vtable_registry::BuildRuntimeVTable(type_stable_id, base_stable_id);
}

}  // namespace chaos::il2cpp::runtime_vtable

#endif  // CHAOS_IL2CPP_RUNTIME_VTABLE_H_
