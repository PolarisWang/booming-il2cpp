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
    uint8_t _owns_slots         : 1; ///< Set when slots was heap-allocated (may be freed)
    uint8_t _owns_vtable_array  : 1; ///< Set when vtable_array was heap-allocated (may be freed)
    uint8_t             _pad[2];      ///< Explicit padding to keep struct size aligned
    /// Optional interface map: describes which interfaces this type implements
    /// and where their methods live in the flat vtable_array.
    const void* iface_map;            ///< Pointer to array of InterfaceMapEntry (optional)
    CHAOS_IL2CPP_UINT32 iface_count;  ///< Number of entries in iface_map (0 = none)
    /// Optional runtime-only interface map: hot-update additions via
    /// ChaosTypeAddInterface / RegisterTypeVTableRuntimeInterface.
    /// Heap-allocated, grown by RegisterTypeVTableRuntimeInterface.
    const void* runtime_iface_map;            ///< Pointer to array of InterfaceMapEntry
    CHAOS_IL2CPP_UINT32 runtime_iface_count;  ///< Number of entries (0 = none)
};

// ── Registration ─────────────────────────────────────────────────────────

/// Register a type vtable from a pre-constructed TypeVTable (bootstrap/test).
/// The caller must keep `vtable` alive for the process lifetime.
/// Duplicate registrations are silently ignored.
bool RegisterTypeVTable(const TypeVTable* vtable) noexcept;

/// Register a vtable for a runtime-instantiated type (TypeInfoHandle-based).
/// The type_token and stable_id are extracted from the handle.
bool RegisterRuntimeVTable(
    TypeInfoHandle               type,
    TypeInfoHandle               base_type,
    CHAOS_IL2CPP_UINT32         slot_count,
    const VTableSlot*           slots) noexcept;

/// Register a flat vtable array (emitted by AOT codegen).
/// Duplicate registrations are silently ignored.
void RegisterVTableArray(CHAOS_IL2CPP_UINT64 stable_id,
                         const void** vtable,
                         CHAOS_IL2CPP_UINT32 length) noexcept;

// ── Lookup ───────────────────────────────────────────────────────────────

/// Register a runtime-added interface on a type's vtable.
/// Appends to the type's runtime_iface_map (heap-reallocated).
/// The type's vtable must already be registered.
/// Idempotent: re-adding the same iface_stable_id is a no-op.
/// @return true on success, false if type_token not found or allocation fails.
bool RegisterTypeVTableRuntimeInterface(
    CHAOS_IL2CPP_UINT32       type_token,
    CHAOS_IL2CPP_UINT64       iface_stable_id,
    CHAOS_IL2CPP_UINT32       vtable_offset,
    CHAOS_IL2CPP_UINT32       method_count) noexcept;

/// Find the vtable_offset for a given interface on a given type's vtable.
/// Searches both AOT iface_map and runtime_iface_map.
/// Uses the IOC (Interface Offset Cache) internally for repeated lookups.
/// @return vtable_offset, or UINT32_MAX if the interface is not found.
CHAOS_IL2CPP_UINT32 chaos_find_interface_offset(
    CHAOS_IL2CPP_UINT32       type_token,
    CHAOS_IL2CPP_UINT64       iface_stable_id) noexcept;

/// Resolve a virtual method pointer by walking the inheritance chain.
/// Uses secondary token→stable_id index, then walks by base_stable_id.
/// Also checks interface vtable map if base chain walk fails.
void* ResolveVirtualMethodPointer(CHAOS_IL2CPP_UINT32 instance_type_token,
                                  CHAOS_IL2CPP_UINT32 declared_method_token) noexcept;

/// Look up TypeVTable by type_token (for interpreter CastClass/IsInst).
const TypeVTable* TryGetTypeVTable(CHAOS_IL2CPP_UINT32 type_token) noexcept;

/// Look up TypeVTable by stable_id.
const TypeVTable* TryGetTypeVTableByStableId(CHAOS_IL2CPP_UINT64 stable_id) noexcept;

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
    CHAOS_IL2CPP_UINT32         declared_method_token) noexcept;

/// Register a codegen-emitted vtable descriptor during BootstrapRuntime.
/// Populates both the TypeVTable index (by_type_token/by_stable_id) and
/// the flat vtable array.  Called once per type during single-threaded
/// bootstrap; no locking required.
/// @param desc  Pointer to a VTableDescriptorV0 (defined in codegen_bridge.h).
void RegisterCodegenVTable(const void* desc) noexcept;

/// Register a vtable for a hot-update type. Copies the parent's flat vtable
/// array, applies override slots, and registers a heap-allocated TypeVTable.
/// @param stable_id       Stable ID of the new type
/// @param type_token      Metadata token of the new type
/// @param parent_token    Token of the parent type (0 = no parent)
/// @param override_slots  Slot overrides for methods this type overrides
/// @param override_count  Number of override entries
/// @param type_shape      Type shape (1=reference, 2=value, 3=interface)
/// @return true on success
bool RegisterHotUpdateVTable(
    CHAOS_IL2CPP_UINT64        stable_id,
    CHAOS_IL2CPP_UINT32        type_token,
    CHAOS_IL2CPP_UINT32        parent_token,
    const VTableSlot*          override_slots,
    CHAOS_IL2CPP_UINT32        override_count,
    CHAOS_IL2CPP_UINT8         type_shape) noexcept;

/// Update all slots matching a method_token across all registered TypeVTables.
/// Also updates the corresponding entries in flat vtable arrays.
/// Used by method_replacement to keep VTable in sync with method patches.
/// @return number of slots updated
CHAOS_IL2CPP_UINT32 UpdateVTableSlotByMethodToken(
    CHAOS_IL2CPP_UINT32       method_token,
    void*                     new_method) noexcept;

/// Scan all registered TypeVTables and return the first method_pointer
/// matching the given method_token.  Used by method_replacement to capture
/// the original pointer before applying a replacement thunk.
/// @return method_pointer, or nullptr if not found in any VTable.
void* FindMethodPointerByMethodToken(
    CHAOS_IL2CPP_UINT32       method_token) noexcept;

/// Remove a TypeVTable from the registry and free allocated memory.
void UnregisterTypeVTable(CHAOS_IL2CPP_UINT32 type_token) noexcept;
void UnregisterTypeVTableByStableId(CHAOS_IL2CPP_UINT64 stable_id) noexcept;

/// Returns the number of registered vtables (for diagnostics).
CHAOS_IL2CPP_UINT32 GetRegisteredVTableCount() noexcept;

/// Clear iface_map and vtable_array pointers for any TypeVTable entries
/// that reference memory within the domain being unloaded.
/// Called from UnloadDomain during the STW safepoint window — no concurrent
/// readers are active.  Nulled pointers prevent use-after-free when threads
/// resume and the domain heap is destroyed.
/// @param domain_id  The domain whose regions are being released.
void ClearDomainPointers(CHAOS_IL2CPP_UINT32 domain_id) noexcept;

}  // namespace chaos::il2cpp::vtable_registry

#endif  // CHAOS_IL2CPP_VTABLE_REGISTRY_H_
