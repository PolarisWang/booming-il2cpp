// ep_threadpool_bridge.h — EventPipe ThreadPool event bridge
//
// Declares the EventPipe emission functions for ThreadPool events.
// These are called from threadpool_events.cpp alongside the existing
// ETW emissions (dual-delivery).

#ifndef CHAOS_IL2CPP_EP_THREADPOOL_BRIDGE_H_
#define CHAOS_IL2CPP_EP_THREADPOOL_BRIDGE_H_

#include "ep_config.h"

#if CHAOS_IL2CPP_EVENTPIPE == 1

#include <cstdint>

namespace chaos::il2cpp::diagnostics {

void EpTpBridgeInitialize() noexcept;
void EpTpBridgeShutdown() noexcept;

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_EVENTPIPE

#endif  // CHAOS_IL2CPP_EP_THREADPOOL_BRIDGE_H_
