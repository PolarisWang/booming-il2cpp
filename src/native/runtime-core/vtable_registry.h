#ifndef CHAOS_IL2CPP_VTABLE_REGISTRY_H_
#define CHAOS_IL2CPP_VTABLE_REGISTRY_H_

#include "runtime_abi.h"
#include <chaos/native_types.h>

namespace chaos::il2cpp::vtable_registry {

/// A single slot in a type's virtual method table.
struct VTableSlot {
    CHAOS_IL2CPP_UINT32 method_token;    ///< Token of the declared (base) virtual method
    void*    method_pointer;  ///< Pointer to the concrete implementation
};

/// Describes the full vtable for one concrete or abstract type.
/// Registered at bootstrap time; looked up at runtime for virtual dispatch.
struct TypeVTable {
    TypeInfoHandle type;         ///< The concrete type this vtable belongs to
    CHAOS_IL2CPP_UINT32       type_token;   ///< Numeric token of `type` (for fast lookup without ABI decode)
    TypeInfoHandle base_type;    ///< Base type (nullptr = no base)
    CHAOS_IL2CPP_UINT32       base_token;   ///< Numeric token of base_type (0 = no base)
    CHAOS_IL2CPP_UINT32       slot_count;
    const VTableSlot* slots;
};

/// Register a type vtable. Called from generated bootstrap code.
/// Returns true if registration succeeded (duplicate registrations are ignored).
bool RegisterTypeVTable(const TypeVTable* vtable);

/// Register a vtable for a runtime-instantiated type (TypeInfoHandle-based).
/// The type_token is extracted from the TypeInfoHandle's descriptor metadata_token.
/// `base_vtable` is the vtable of the open generic definition (copied as template).
/// Returns true if registration succeeded.
bool RegisterRuntimeVTable(
    TypeInfoHandle               type,
    TypeInfoHandle               base_type,
    CHAOS_IL2CPP_UINT32         slot_count,
    const VTableSlot*           slots);

/// Look up the concrete method pointer for a virtual call.
/// Walks the inheritance chain from `instance_type_token` upward until the
/// declared method is found.
/// Returns nullptr if no entry is found.
void* ResolveVirtualMethodPointer(CHAOS_IL2CPP_UINT32 instance_type_token,
                                  CHAOS_IL2CPP_UINT32 declared_method_token);

/// Resolve a virtual method pointer using a TypeInfoHandle.
/// Decodes the handle to extract the type_token, then delegates to
/// ResolveVirtualMethodPointer.
void* ResolveVirtualMethodPointerByHandle(
    TypeInfoHandle               instance_type,
    CHAOS_IL2CPP_UINT32         declared_method_token);

/// Returns the number of registered vtables (useful for diagnostics).
CHAOS_IL2CPP_UINT32 GetRegisteredVTableCount();

}  // namespace chaos::il2cpp::vtable_registry

#endif  // CHAOS_IL2CPP_VTABLE_REGISTRY_H_
