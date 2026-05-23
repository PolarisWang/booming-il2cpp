// ep_hotupdate_bridge.h — EventPipe bridge for HotUpdate diagnostics events
//
// Bridges HotupdateEvent callbacks to EventPipe emission.
// Follows the same pattern as ep_gc_bridge.h.

#ifndef CHAOS_IL2CPP_EP_HOTUPDATE_BRIDGE_H_
#define CHAOS_IL2CPP_EP_HOTUPDATE_BRIDGE_H_

#include "ep_config.h"

#if CHAOS_IL2CPP_EVENTPIPE == 1

namespace chaos::il2cpp::diagnostics {

/// Initialize the hotupdate bridge — registers EventPipe callback for
/// HotupdateEvent notifications.
void EpHotupdateBridgeInitialize() noexcept;

/// Shutdown the hotupdate bridge (no-op, callbacks cannot be unregistered).
void EpHotupdateBridgeShutdown() noexcept;

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_EVENTPIPE
#endif  // CHAOS_IL2CPP_EP_HOTUPDATE_BRIDGE_H_
