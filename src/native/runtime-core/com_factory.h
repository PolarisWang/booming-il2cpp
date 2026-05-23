#ifndef CHAOS_IL2CPP_COM_FACTORY_H_
#define CHAOS_IL2CPP_COM_FACTORY_H_

#include <cstdint>
#include <chaos/native_types.h>

namespace chaos::il2cpp::runtime_core {

/// Factory function type: creates a CCW for a managed object and registers
/// the COM interface vtable on it.
/// @param managed_object  The managed object to expose as COM.
/// @param runtime_state   RuntimeState* for GC handle lifecycle.
/// @return The CCW pointer (IUnknown*), or 0 on failure.
using CcwFactoryFn = CHAOS_IL2CPP_INTPTR (*)(void* managed_object, void* runtime_state);

/// Register a CCW factory function for a given interface stable ID.
/// Thread-safe.  If a factory is already registered for this stable_id,
/// it is replaced.
void RegisterCcwFactory(CHAOS_IL2CPP_UINT64 stable_id, CcwFactoryFn factory) noexcept;

/// Unregister a CCW factory function.
/// Thread-safe.  No-op if no factory is registered for this stable_id.
void UnregisterCcwFactory(CHAOS_IL2CPP_UINT64 stable_id) noexcept;

/// Find a CCW factory function by interface stable ID.
/// Thread-safe.  Returns nullptr if no factory is registered.
CcwFactoryFn FindCcwFactory(CHAOS_IL2CPP_UINT64 stable_id) noexcept;

/// Create a CCW using a registered factory for the given interface stable ID.
/// @param stable_id      64-bit FNV-1a hash identifying the COM interface.
/// @param managed_object  The managed object to expose as COM.
/// @param runtime_state   RuntimeState* for GC handle lifecycle.
/// @return The CCW pointer (IUnknown*), or 0 if no factory found or creation fails.
CHAOS_IL2CPP_INTPTR CreateCcwForStableId(CHAOS_IL2CPP_UINT64 stable_id,
                                          void* managed_object,
                                          void* runtime_state) noexcept;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_COM_FACTORY_H_
