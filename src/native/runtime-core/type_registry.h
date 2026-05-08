#ifndef CHAOS_IL2CPP_TYPE_REGISTRY_H_
#define CHAOS_IL2CPP_TYPE_REGISTRY_H_

#include <chaos/type_info.h>
#include <chaos/native_types.h>

// ── Dynamic type registration ─────────────────────────────────────
//
// Declares the HotUpdate dynamic type registry API.  Implementations
// live in type_registry.cpp.
//
// These functions were moved here from type_info.h so that the type
// system header (chaos_common module) contains only type definitions,
// while the runtime registry API lives in the runtime_core module.

namespace chaos::il2cpp::runtime_core {

/// Register a new dynamic type.
///
/// Allocates a TypeInfoHot+TypeInfoWarm pair on the heap, populates them
/// from the arguments, computes its stable_id via FNV-1a hash of @p name,
/// and stores the Hot pointer in the global dynamic type registry.
///
/// @param name        Fully qualified type name (e.g. "MyAssembly/MyType").
///                    Used only for stable_id computation.
/// @param parent      Pointer to the base type's TypeInfoHot (nullptr for
///                    System.Object).
/// @param type_shape  One of chaos_type_shape_reference/value/interface.
/// @param iface_map   Pointer to array of InterfaceMapEntry for this type (optional).
/// @param iface_count Number of entries in iface_map (0 if none).
/// @param out_stable_id  Optional output parameter; receives the computed
///                       stable_id on success, or 0 on failure.
///
/// @return Pointer to the newly allocated TypeInfoHot, or nullptr if the
///         registry is full.
TypeInfoHot* chaos_register_type(
    const char* name,
    const TypeInfoHot* parent,
    CHAOS_IL2CPP_UINT8 type_shape,
    const InterfaceMapEntry* iface_map = nullptr,
    CHAOS_IL2CPP_UINT32 iface_count = 0,
    CHAOS_IL2CPP_UINT64* out_stable_id = nullptr) noexcept;

/// Find a registered dynamic type by its stable_id.
///
/// Searches the dynamic type registry.  Does NOT search static
/// (inline constexpr) TypeInfoHot instances — static types already have
/// known addresses accessed via `&chaos_type_info_X` symbols.
///
/// @return Pointer to the matching TypeInfoHot, or nullptr if not found.
const TypeInfoHot* chaos_find_type_by_stable_id(
    CHAOS_IL2CPP_UINT64 stable_id) noexcept;

/// Append an interface mapping to an existing TypeInfoHot at runtime (HotUpdate).
///
/// Allocates a heap copy of the runtime_iface_map array (or appends to an
/// existing one).  Thread-safe: uses the global registry mutex.
///
/// @param ti              Pointer to the TypeInfoHot to extend.
/// @param iface_stable_id Stable ID of the interface to add.
/// @param vtable_offset   Starting vtable slot for this interface's methods.
/// @param method_count    Number of methods in this interface.
///
/// @return true on success, false on allocation failure.
bool ChaosTypeAddInterface(
    TypeInfoHot* ti,
    CHAOS_IL2CPP_UINT64 iface_stable_id,
    CHAOS_IL2CPP_UINT32 vtable_offset,
    CHAOS_IL2CPP_UINT32 method_count) noexcept;

/// Resolve TypeInfoHot* from a TypeInfoHandle.
///
/// Handles three encoding schemes:
///   1. Tag-encoded (RuntimeInstantiatedType) — no TypeInfo available,
///      returns nullptr.
///   2. Module-registry handle — looks up TypeInfoHot* from the module's
///      type_info_ptrs array.
///   3. Raw metadata token — falls through to nullptr (caller should
///      compute stable_id and use the vtable registry instead).
///
/// @return Pointer to TypeInfoHot, or nullptr if the handle encoding does
///         not carry a TypeInfoHot* or the module's type_info_ptrs is not
///         populated (Phase 3+).
const TypeInfoHot* TryResolveTypeInfo(TypeInfoHandle handle) noexcept;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_TYPE_REGISTRY_H_
