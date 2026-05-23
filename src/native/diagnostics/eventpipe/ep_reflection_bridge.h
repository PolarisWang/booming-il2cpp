// ep_reflection_bridge.h — EventPipe bridge for Reflection diagnostics events
//
// Bridges reflection events to EventPipe emission.
// Follows the same pattern as ep_gc_bridge.h and ep_hotupdate_bridge.h.
//
// Currently provides the initialization/shutdown integration point.
// When a centralized reflection event callback system is introduced,
// this bridge will register with it via Initialize (like the GC and
// HotUpdate bridges do).

#ifndef CHAOS_IL2CPP_EP_REFLECTION_BRIDGE_H_
#define CHAOS_IL2CPP_EP_REFLECTION_BRIDGE_H_

#include "ep_config.h"

#if CHAOS_IL2CPP_EVENTPIPE == 1

namespace chaos::il2cpp::diagnostics {

/// Initialize the reflection bridge.
/// No-op in the current implementation — reserved for future callback
/// registration when a centralized reflection event system is available.
void EpReflectionBridgeInitialize() noexcept;

/// Shutdown the reflection bridge (no-op, callbacks cannot be unregistered).
void EpReflectionBridgeShutdown() noexcept;

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_EVENTPIPE
#endif  // CHAOS_IL2CPP_EP_REFLECTION_BRIDGE_H_
