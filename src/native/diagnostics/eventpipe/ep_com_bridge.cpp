// ep_com_bridge.cpp — EventPipe COM event bridge implementation
//
// COM code emits events directly via EpEmitComXxx() functions (declared in
// ep_event.h).  This bridge is a no-op placeholder for symmetry with other
// bridge modules that require callback registration.

#include "ep_com_bridge.h"

#if CHAOS_IL2CPP_EVENTPIPE == 1

namespace chaos::il2cpp::diagnostics {

void EpComBridgeInitialize() noexcept {
    // No-op: COM events are emitted directly from COM source files.
}

void EpComBridgeShutdown() noexcept {
    // No-op: nothing to unregister.
}

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_EVENTPIPE
