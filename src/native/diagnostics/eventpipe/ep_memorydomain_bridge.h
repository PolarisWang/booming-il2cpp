// ep_memorydomain_bridge.h — EventPipe bridge for MemoryDomain diagnostics events
//
// Bridges MemoryDomainEvent callbacks to EventPipe emission.
// Follows the same pattern as ep_gc_bridge.h and ep_hotupdate_bridge.h.
//
// Initialized during EpInitialize — registers a callback with the
// MemoryDomain event system so that domain lifecycle events (register,
// unload, usage-limit exceeded) are forwarded to EventPipe.

#ifndef CHAOS_IL2CPP_EP_MEMORYDOMAIN_BRIDGE_H_
#define CHAOS_IL2CPP_EP_MEMORYDOMAIN_BRIDGE_H_

#include "ep_config.h"

#if CHAOS_IL2CPP_EVENTPIPE == 1

namespace chaos::il2cpp::diagnostics {

/// Initialize the memory domain bridge — registers EventPipe callback for
/// MemoryDomainEvent notifications.
void EpMemoryDomainBridgeInitialize() noexcept;

/// Shutdown the memory domain bridge (no-op, callbacks cannot be unregistered).
void EpMemoryDomainBridgeShutdown() noexcept;

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_EVENTPIPE

#endif  // CHAOS_IL2CPP_EP_MEMORYDOMAIN_BRIDGE_H_
