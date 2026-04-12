#ifndef CHAOS_IL2CPP_VTABLE_REGISTRY_H_
#define CHAOS_IL2CPP_VTABLE_REGISTRY_H_

#include "runtime_abi.h"
#include <stdint.h>

namespace chaos::il2cpp::vtable_registry {

/// A single slot in a type's virtual method table.
struct VTableSlot {
    uint32_t method_token;    ///< Token of the declared (base) virtual method
    void*    method_pointer;  ///< Pointer to the concrete implementation
};

/// Describes the full vtable for one concrete or abstract type.
/// Registered at bootstrap time; looked up at runtime for virtual dispatch.
struct TypeVTable {
    TypeInfoHandle type;         ///< The concrete type this vtable belongs to
    uint32_t       type_token;   ///< Numeric token of `type` (for fast lookup without ABI decode)
    TypeInfoHandle base_type;    ///< Base type (nullptr = no base)
    uint32_t       base_token;   ///< Numeric token of base_type (0 = no base)
    uint32_t       slot_count;
    const VTableSlot* slots;
};

/// Register a type vtable. Called from generated bootstrap code.
/// Returns true if registration succeeded (duplicate registrations are ignored).
bool RegisterTypeVTable(const TypeVTable* vtable);

/// Look up the concrete method pointer for a virtual call.
/// Walks the inheritance chain from `instance_type_token` upward until the
/// declared method is found.
/// Returns nullptr if no entry is found.
void* ResolveVirtualMethodPointer(uint32_t instance_type_token,
                                  uint32_t declared_method_token);

/// Returns the number of registered vtables (useful for diagnostics).
uint32_t GetRegisteredVTableCount();

}  // namespace chaos::il2cpp::vtable_registry

#endif  // CHAOS_IL2CPP_VTABLE_REGISTRY_H_
