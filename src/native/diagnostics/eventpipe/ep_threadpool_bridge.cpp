// ep_threadpool_bridge.cpp — EventPipe ThreadPool bridge (no-op stub)
//
// ThreadPool EventPipe events are emitted directly from
// threadpool_events.cpp via the EpEmitTp* functions declared in ep_event.h.
// This file provides the initialize/shutdown hooks for the bridge.

#include "ep_threadpool_bridge.h"

#if CHAOS_IL2CPP_EVENTPIPE == 1

namespace chaos::il2cpp::diagnostics {

void EpTpBridgeInitialize() noexcept {
    // No registration needed — ThreadPool events call EpEmitTp* directly
    // from threadpool_events.cpp (dual-delivery alongside ETW).
}

void EpTpBridgeShutdown() noexcept {
    // No cleanup needed — EpEmitTp* functions check EpIsInitialized()
    // internally and become no-ops after shutdown.
}

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_EVENTPIPE
