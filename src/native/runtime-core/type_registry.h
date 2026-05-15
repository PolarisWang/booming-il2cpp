#ifndef CHAOS_IL2CPP_TYPE_REGISTRY_H_
#define CHAOS_IL2CPP_TYPE_REGISTRY_H_

#include <chaos/type_info.h>
#include <chaos/native_types.h>

// ── Dynamic type registration ─────────────────────────────────────
//
// Declares the HotUpdate dynamic type registry API.  Implementations
// live in type_registry.cpp.
//
// Phase 0 CoreCLR alignment: TryResolveTypeInfo is augmented with
// TryResolveMethodTable that handles the three-state TypeHandle encoding
// (direct MethodTable ptr, dynamic MethodTable index, module-registry handle).
// Retrofit: existing TryResolveTypeInfo callers continue to work via
// AsMethodTable() on the returned TypeInfoHot*.

namespace chaos::il2cpp::runtime_core {

/// Register a new dynamic type.
///
/// Allocates a MethodTable (hot+warm) on the heap, populates it
/// from the arguments, computes its stable_id via FNV-1a hash of @p name,
/// and stores the pointer in the global dynamic type registry.
///
/// @param name        Fully qualified type name (e.g. "MyAssembly/MyType").
///                    Used only for stable_id computation.
/// @param parent      Pointer to the base type's MethodTable (nullptr for
///                    System.Object).
/// @param type_shape  One of chaos_type_shape_reference/value/interface.
/// @param iface_map   Pointer to array of InterfaceMapEntry for this type (optional).
/// @param iface_count Number of entries in iface_map (0 if none).
/// @param out_stable_id  Optional output parameter; receives the computed
///                       stable_id on success, or 0 on failure.
///
/// @return Pointer to the newly allocated MethodTable, or nullptr if the
///         registry is full.
MethodTable* chaos_register_type(
    const char* name,
    const MethodTable* parent,
    CHAOS_IL2CPP_UINT8 type_shape,
    const InterfaceMapEntry* iface_map = nullptr,
    CHAOS_IL2CPP_UINT32 iface_count = 0,
    CHAOS_IL2CPP_UINT64* out_stable_id = nullptr) noexcept;

/// Find a registered dynamic type by its stable_id.
///
/// Searches the dynamic type registry.  Does NOT search static
/// (inline constexpr) MethodTable instances — static types already have
/// known addresses accessed via `&chaos_method_table_X` symbols.
///
/// @return Pointer to the matching MethodTable, or nullptr if not found.
const MethodTable* chaos_find_type_by_stable_id(
    CHAOS_IL2CPP_UINT64 stable_id) noexcept;

/// Append an interface mapping to an existing MethodTable at runtime (HotUpdate).
///
/// Allocates a heap copy of the runtime_iface_map array (or appends to an
/// existing one).  Thread-safe: uses the global registry mutex.
///
/// @param mt             Pointer to the MethodTable to extend.
/// @param iface_stable_id Stable ID of the interface to add.
/// @param vtable_offset   Starting vtable slot for this interface's methods.
/// @param method_count    Number of methods in this interface.
///
/// @return true on success, false on allocation failure.
bool ChaosTypeAddInterface(
    MethodTable* mt,
    CHAOS_IL2CPP_UINT64 iface_stable_id,
    CHAOS_IL2CPP_UINT32 vtable_offset,
    CHAOS_IL2CPP_UINT32 method_count) noexcept;

/// Resolve TypeInfoHot* from a TypeInfoHandle (legacy path).
///
/// Handles three encoding schemes:
///   1. Direct MethodTable* pointer (bit[63]=1) — casts and returns as TypeInfoHot*.
///   2. Dynamic MethodTable index (bit[62]=1) — looks up from dynamic registry.
///   3. Module-registry handle — looks up from module's type_info_ptrs array.
///
/// @return Pointer to TypeInfoHot, or nullptr if the handle encoding does
///         not carry a TypeInfoHot* or the module's type_info_ptrs is not
///         populated (Phase 3+).
const TypeInfoHot* TryResolveTypeInfo(TypeInfoHandle handle) noexcept;

/// Resolve MethodTable* from a TypeInfoHandle (preferred new path).
///
/// Same semantics as TryResolveTypeInfo but returns MethodTable* directly.
/// For bit[63]=1 handles, this is a direct pointer decode with zero indirection.
///
/// @return Pointer to MethodTable, or nullptr if unresolvable.
inline const MethodTable* TryResolveMethodTable(TypeInfoHandle handle) noexcept {
    auto* ti = TryResolveTypeInfo(handle);
    return ti != nullptr ? ti->AsMethodTable() : nullptr;
}

/// Compute stable_id from a TypeInfoHandle (cross-module type identity).
/// Useful for type equality checks when handles come from different modules
/// (e.g., cross-ALC type comparison).
inline uint64_t GetTypeStableIdFromHandle(TypeInfoHandle handle) noexcept {
    auto* mt = TryResolveMethodTable(handle);
    return mt != nullptr ? mt->stable_id : 0ull;
}

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_TYPE_REGISTRY_H_
