#ifndef CHAOS_IL2CPP_GC_EVENTS_H_
#define CHAOS_IL2CPP_GC_EVENTS_H_

#include <chaos/native_types.h>

#include <atomic>
#include <cstdint>
#include <cstring>

namespace chaos::il2cpp::runtime_core {

// ======================================================================
// GC Event Callbacks — online GC lifecycle notifications
//
// == Design ==
// CoreCLR-aligned event system: registers up to 8 callbacks that fire at
// key GC phase boundaries.  Used by the engine integration layer for
// metrics, logging, and adaptive tuning.
//
// == Events ==
// GC_START:       Full GC has begun (safepoint acquired, roots being marked)
// MARK_DONE:      Transitive closure marking complete (parallel or sequential)
// SWEEP_DONE:     All pages swept (free lists rebuilt)
// COMPACT_DONE:   Compaction complete (if applicable, otherwise same as SWEEP_DONE)
// GC_END:         Full GC complete (threads resumed)
// ======================================================================

/// GC event types.
enum class GcEvent : uint8_t {
    GC_START       = 0,
    MARK_DONE      = 1,
    SWEEP_DONE     = 2,
    COMPACT_DONE   = 3,
    GC_END         = 4
};

/// Maximum number of registered callbacks.
static constexpr int kMaxGcEventCallbacks = 8;

/// Event callback signature.
/// @param event     The GC event that fired.
/// @param user_data  Opaque user data registered with the callback.
using GcEventCallback = void (*)(GcEvent event, void* user_data);

/// A slot in the global event callback table.
struct GcEventSlot {
    std::atomic<GcEventCallback> callback{nullptr};
    void* user_data = nullptr;
};

/// Global event callback table (defined in gc_events.cpp).
extern GcEventSlot g_gc_event_slots[kMaxGcEventCallbacks];

/// Register a callback for GC events.
/// Returns true on success, false if the callback table is full.
/// Thread-safe (uses atomic CAS on the callback table slot).
/// @param callback   Function pointer to invoke on each event.
/// @param user_data  Opaque data passed back to the callback.
bool GcRegisterEventCallback(GcEventCallback callback, void* user_data);

/// Fire a GC event to all registered callbacks.
/// Called from within the GC safepoint (STW).
inline void GcFireEvent(GcEvent event) noexcept {
    for (int i = 0; i < kMaxGcEventCallbacks; i++) {
        auto cb = g_gc_event_slots[i].callback.load(std::memory_order_acquire);
        if (cb != nullptr) {
            cb(event, g_gc_event_slots[i].user_data);
        }
    }
}

/// Iterate all entries in the GCHandle table (defined in engine_lifecycle.cpp).
/// Calls @a callback for each non-null object instance.
void GcIterateHandleTable(void (*callback)(void* object, void* user_data),
                           void* user_data) noexcept;

/// Process weak GCHandles after a young GC:
///   - Nursery objects that were promoted → update handle to forwarding address
///   - Nursery objects that were NOT promoted → null the handle
/// Called between young GC Phase 3 (BFS done) and Phase 4 (nursery sweep).
void GcProcessWeakHandlesAfterYoungGC() noexcept;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_EVENTS_H_