// ep_reflection_bridge.cpp — EventPipe bridge for Reflection diagnostics events
//
// Currently provides a no-op initialization point.  When a centralized
// reflection event callback system is introduced, this bridge will register
// a callback with it (matching the pattern used by ep_gc_bridge.cpp and
// ep_hotupdate_bridge.cpp).

#include "ep_reflection_bridge.h"

#if CHAOS_IL2CPP_EVENTPIPE == 1

namespace chaos::il2cpp::diagnostics {

void EpReflectionBridgeInitialize() noexcept {
    // No centralized reflection event callback system exists yet.
    // When one is introduced, register the callback here, e.g.:
    //   ReflectionRegisterEventCallback(EpReflectionCallback, nullptr);
}

void EpReflectionBridgeShutdown() noexcept {
    // Reflection event callbacks cannot be individually unregistered.
    // The reflection subsystem will stop firing during shutdown naturally.
}

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_EVENTPIPE
