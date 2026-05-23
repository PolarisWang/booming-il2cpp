// ep_gc_bridge.h — EventPipe GC event bridge
//
// Registers a GcEventCallback via the existing GcRegisterEventCallback
// system to forward GC events to the EventPipe transport.
//
// The callback is registered during EpInitialize and fires inside the
// GC safepoint (STW).  Must be extremely lightweight — no allocations,
// no blocking I/O, minimal work.

#ifndef CHAOS_IL2CPP_EP_GC_BRIDGE_H_
#define CHAOS_IL2CPP_EP_GC_BRIDGE_H_

#include "ep_config.h"

#if CHAOS_IL2CPP_EVENTPIPE == 1

namespace chaos::il2cpp::diagnostics {

/// Initialize the GC event bridge.
/// Registers the EventPipe callback with the GC subsystem.
void EpGcBridgeInitialize() noexcept;

/// Shutdown the GC event bridge.
void EpGcBridgeShutdown() noexcept;

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_EVENTPIPE

#endif  // CHAOS_IL2CPP_EP_GC_BRIDGE_H_
