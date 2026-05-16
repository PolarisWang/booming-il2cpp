#ifndef CHAOS_IL2CPP_VTABLE_REGISTRY_H_
#define CHAOS_IL2CPP_VTABLE_REGISTRY_H_

// ── Unified VTable Registry ──────────────────────────────────────────────
//
// Merges the legacy vtable_registry (type_token-keyed) and runtime_vtable
// (stable_id-keyed) into a single registry with:
//   - Primary key:  stable_id (FNV-1a 64-bit, universal across AOT/runtime)
//   - Secondary:    type_token → stable_id (for interpreter lookups)
//
// Supports both rich slot descriptors (method_token + method_pointer pairs)
// and flat function-pointer arrays (for AOT codegen direct dispatch).

#include <runtime_abi.h>
#include <chaos/native_types.h>

namespace chaos::il2cpp::vtable_registry {

/// A single slot in a type's virtual method table.
struct VTableSlot {
    CHAOS_IL2CPP_UINT32 method_token;    ///< Token of the declared (base) virtual method
    void*    method_pointer;  ///< Pointer to the concrete implementation
};

/// Describes the full vtable for one concrete or abstract type.
struct TypeVTable {
    TypeInfoHandle type;              ///< The concrete type this vtable belongs to
    CHAOS_IL2CPP_UINT64 stable_id;    ///< FNV-1a stable_id (primary key in unified registry)
    CHAOS_IL2CPP_UINT32 type_token;   ///< Numeric token of `type` (for interpreter without ABI decode)
    TypeInfoHandle base_type;          ///< Base type (nullptr = no base)
    CHAOS_IL2CPP_UINT64 base_stable_id; ///< Stable_id of base type (chain walking)
    CHAOS_IL2CPP_UINT32 base_token;   ///< Numeric token of base_type (0 = no base)
    CHAOS_IL2CPP_UINT32 slot_count;
    const VTableSlot* slots;          ///< Rich slot descriptors (method_token + pointer)
    const void** vtable_array;         ///< Flat function-pointer array (for direct dispatch)
    CHAOS_IL2CPP_UINT32 vtable_length; ///< Length of vtable_array
    CHAOS_IL2CPP_UINT8  type_shape;   ///< 1=reference, 2=value, 3=interface (mirrors TypeInfoHot::type_shape)
    uint8_t             _pad[3];      ///< Explicit padding to keep struct size aligned
    /// Optional interface map: describes which interfaces this type implements
    /// and where their methods live in the flat vtable_array.
    const void* iface_map;            ///< Pointer to array of InterfaceMapEntry (optional)
    CHAOS_IL2CPP_UINT32 iface_count;  ///< Number of entries in iface_map (0 = none)
};

// ── Registration ─────────────────────────────────────────────────────────

/// Register a type vtable from a pre-constructed TypeVTable (bootstrap/test).
/// The caller must keep `vtable` alive for the process lifetime.
/// Duplicate registrations are silently ignored.
bool RegisterTypeVTable(const TypeVTable* vtable);

/// Register a vtable for a runtime-instantiated type (TypeInfoHandle-based).
/// The type_token and stable_id are extracted from the handle.
bool RegisterRuntimeVTable(
    TypeInfoHandle               type,
    TypeInfoHandle               base_type,
    CHAOS_IL2CPP_UINT32         slot_count,
    const VTableSlot*           slots);

/// Register a flat vtable array (emitted by AOT codegen).
/// Duplicate registrations are silently ignored.
void RegisterVTableArray(CHAOS_IL2CPP_UINT64 stable_id,
                         const void** vtable,
                         CHAOS_IL2CPP_UINT32 length) noexcept;

// ── Lookup ───────────────────────────────────────────────────────────────

/// Resolve a virtual method pointer by walking the inheritance chain.
/// Uses secondary token→stable_id index, then walks by base_stable_id.
/// Also checks interface vtable map if base chain walk fails.
void* ResolveVirtualMethodPointer(CHAOS_IL2CPP_UINT32 instance_type_token,
                                  CHAOS_IL2CPP_UINT32 declared_method_token);

/// Look up TypeVTable by type_token (for interpreter CastClass/IsInst).
const TypeVTable* TryGetTypeVTable(CHAOS_IL2CPP_UINT32 type_token);

/// Look up TypeVTable by stable_id.
const TypeVTable* TryGetTypeVTableByStableId(CHAOS_IL2CPP_UINT64 stable_id);

/// Flat vtable array lookup (for AOT codegen direct dispatch).
const void** FindVTable(CHAOS_IL2CPP_UINT64 stable_id) noexcept;

/// Flat vtable array length lookup.
CHAOS_IL2CPP_UINT32 FindVTableLength(CHAOS_IL2CPP_UINT64 stable_id) noexcept;

/// Build and register a vtable for a runtime type by copying the base type's
/// flat vtable array.  Returns the newly-allocated array, or nullptr if the
/// base has no registered vtable.
const void** BuildRuntimeVTable(CHAOS_IL2CPP_UINT64 type_stable_id,
                                 CHAOS_IL2CPP_UINT64 base_stable_id) noexcept;

/// Resolve a virtual method pointer using a TypeInfoHandle.
void* ResolveVirtualMethodPointerByHandle(
    TypeInfoHandle               instance_type,
    CHAOS_IL2CPP_UINT32         declared_method_token);

/// Returns the number of registered vtables (for diagnostics).
CHAOS_IL2CPP_UINT32 GetRegisteredVTableCount();

/// Clear iface_map and vtable_array pointers for any TypeVTable entries
/// that reference memory within the domain being unloaded.
/// Called from UnloadDomain during the STW safepoint window — no concurrent
/// readers are active.  Nulled pointers prevent use-after-free when threads
/// resume and the domain heap is destroyed.
/// @param domain_id  The domain whose regions are being released.
void ClearDomainPointers(CHAOS_IL2CPP_UINT32 domain_id);

}  // namespace chaos::il2cpp::vtable_registry

#endif  // CHAOS_IL2CPP_VTABLE_REGISTRY_H_
