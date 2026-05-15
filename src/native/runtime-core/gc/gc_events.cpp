#include "gc_events.h"
#include "gc_old_gen.h"

#include <chaos/log.h>

namespace chaos::il2cpp::runtime_core {

// Global event callback table.
GcEventSlot g_gc_event_slots[kMaxGcEventCallbacks] = {};

bool GcRegisterEventCallback(GcEventCallback callback, void* user_data) {
    if (callback == nullptr) return false;

    // Find an empty slot via CAS.
    for (int i = 0; i < kMaxGcEventCallbacks; i++) {
        GcEventCallback expected = nullptr;
        if (g_gc_event_slots[i].callback.compare_exchange_strong(expected, callback,
                std::memory_order_release, std::memory_order_acquire)) {
            g_gc_event_slots[i].user_data = user_data;
            CHAOS_IL2CPP_LOG_DEBUG_M("GcEvents", "registered callback {0} at slot {1}",
                reinterpret_cast<void*>(callback), i);
            return true;
        }
    }

    CHAOS_IL2CPP_LOG_WARN_M("GcEvents", "callback table full, cannot register {0}",
        reinterpret_cast<void*>(callback));
    return false;
}

}  // namespace chaos::il2cpp::runtime_core