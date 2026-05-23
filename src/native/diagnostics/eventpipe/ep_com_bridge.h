// ep_com_bridge.h — EventPipe COM event bridge
//
// COM events (RCW/CCW lifecycle, ConnectionPoint, DispatchInvoke) are emitted
// directly from the COM source files (com_rcw.cpp, com_ccw.cpp,
// com_connection_point.cpp) via the helper functions declared in ep_event.h.
//
// This bridge provides Initialize/Shutdown hooks called from EpInitialize/
// EpShutdown.  Currently no-op — no callback registration needed since COM
// code emits events directly.  Exists for symmetry with other bridge modules.

#ifndef CHAOS_IL2CPP_EP_COM_BRIDGE_H_
#define CHAOS_IL2CPP_EP_COM_BRIDGE_H_

#include "ep_config.h"

#if CHAOS_IL2CPP_EVENTPIPE == 1

namespace chaos::il2cpp::diagnostics {

/// Initialize the COM event bridge (no-op).
void EpComBridgeInitialize() noexcept;

/// Shutdown the COM event bridge (no-op).
void EpComBridgeShutdown() noexcept;

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_EVENTPIPE
#endif  // CHAOS_IL2CPP_EP_COM_BRIDGE_H_
