#ifndef CHAOS_IL2CPP_RUNTIME_VTABLE_H_
#define CHAOS_IL2CPP_RUNTIME_VTABLE_H_

/// B2+ vtable runtime registry.
///
/// Stores dynamically-allocated vtable arrays for runtime-created types
/// (MakeGenericType closed generics and HotUpdate-registered types).
/// AOT-generated vtables are also registered from bootstrap code so they
/// can be found by stable_id at runtime.
///
/// Keyed by stable_id (FNV-1a 64-bit hash, same as TypeInfo::stable_id),
/// which both AOT and runtime types have universally.

#include <chaos/native_types.h>

namespace chaos::il2cpp::runtime_vtable {

/// Register a vtable array for a type.
/// @param stable_id  FNV-1a hash identifying the type
/// @param vtable     Pointer to the vtable array (borrowed — caller keeps alive)
/// @param length     Number of entries in the vtable array
void RegisterVTable(CHAOS_IL2CPP_UINT64 stable_id,
                    const void** vtable,
                    CHAOS_IL2CPP_UINT32 length) noexcept;

/// Look up a registered vtable by stable_id.
/// @return The vtable array, or nullptr if not found.
const void** FindVTable(CHAOS_IL2CPP_UINT64 stable_id) noexcept;

/// Look up a registered vtable length by stable_id.
/// @return The vtable entry count, or 0 if not found.
CHAOS_IL2CPP_UINT32 FindVTableLength(CHAOS_IL2CPP_UINT64 stable_id) noexcept;

/// Build and register a vtable for a runtime-type by copying the base type's
/// vtable layout.
///
/// For closed generic types (MakeGenericType), the layout is identical to the
/// open generic definition — same slot positions, same function pointers.
/// For HotUpdate types, the parent type's vtable entries are inherited.
///
/// @param type_stable_id  Stable_id of the new runtime type
/// @param base_stable_id  Stable_id of the base (open definition / parent) type
/// @return The newly-allocated vtable, or nullptr if the base has no vtable
///         registered or allocation fails.
const void** BuildRuntimeVTable(CHAOS_IL2CPP_UINT64 type_stable_id,
                                 CHAOS_IL2CPP_UINT64 base_stable_id) noexcept;

}  // namespace chaos::il2cpp::runtime_vtable

#endif  // CHAOS_IL2CPP_RUNTIME_VTABLE_H_
