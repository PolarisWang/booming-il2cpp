#ifndef CHAOS_IL2CPP_HOTUPDATE_EVENTS_H_
#define CHAOS_IL2CPP_HOTUPDATE_EVENTS_H_

// ── Hot-update lifecycle events ──────────────────────────────────────────
//
// Lightweight callback-based event system for hot-update lifecycle events.
// Follows the same pattern as gc_events.h (fixed-slot callback table with
// atomic CAS registration).
//
// Usage:
//   auto cb = [](HotupdateEvent ev, const HotupdateEventData* data, void*) {
//       CHAOS_IL2CPP_LOG_INFO("HotUpdate", "event={0}", static_cast<int>(ev));
//   };
//   HotupdateRegisterEventCallback(cb, nullptr);
//
// Thread safety: FireEvent iterates the slot table with memory_order_acquire
// loads, matching gc_events.h guarantees. The data struct is passed by
// pointer and is valid only during the callback invocation — callbacks must
// not store the pointer for later use.

#include <atomic>
#include <cstdint>

namespace chaos::il2cpp::diagnostics {

// ── Event types ─────────────────────────────────────────────────────────
enum class HotupdateEvent : uint8_t {
    PACKAGE_LOADED      = 0,  // Hot-update package loaded successfully
    PACKAGE_LOAD_FAILED = 1,  // Hot-update package failed to load
    PACKAGE_UNLOADED    = 2,  // Hot-update package unloaded
    METHOD_REPLACED     = 3,  // Method replacement registered
    METHOD_REVERTED     = 4,  // Method replacement reverted
    METHOD_REVERT_ALL   = 5,  // All method replacements reverted
    ASSEMBLY_LOADED     = 6,  // Assembly loaded into ALC
    ASSEMBLY_UNLOADED   = 7,  // Assembly unloaded from ALC
    ERROR               = 8,  // General error during hot-update operation
};

// ── Event payload ───────────────────────────────────────────────────────
struct HotupdateEventData {
    uint32_t     module_id      = 0;   // 0 = not applicable
    uint32_t     method_token   = 0;   // 0 = not applicable
    uint32_t     alc_id         = 0;   // 0 = SharedContext / not applicable
    const char*  package_id     = nullptr;  // Opaque string from manifest
    const char*  assembly_name  = nullptr;  // Assembly/ALC name
    const char*  method_name    = nullptr;  // Full method name for method events
    uint32_t     active_count   = 0;   // Count of active replacements (for METHOD_REVERT_ALL)
    bool         success        = true;
    const char*  error_message  = nullptr;
};

// ── Callback table ───────────────────────────────────────────────────────
static constexpr int kMaxHotupdateEventCallbacks = 8;

using HotupdateEventCallback = void (*)(HotupdateEvent event,
                                        const HotupdateEventData* data,
                                        void* user_data);

struct HotupdateEventSlot {
    std::atomic<HotupdateEventCallback> callback{nullptr};
    void* user_data = nullptr;
};

// Global callback table. Inline (C++17) — defined once across all TUs.
inline HotupdateEventSlot g_hotupdate_event_slots[kMaxHotupdateEventCallbacks] = {};

/// Register a callback for hot-update lifecycle events.
/// Returns true on success, false if the callback table is full.
inline bool HotupdateRegisterEventCallback(HotupdateEventCallback callback, void* user_data) {
    if (callback == nullptr) return false;

    for (int i = 0; i < kMaxHotupdateEventCallbacks; i++) {
        HotupdateEventCallback expected = nullptr;
        if (g_hotupdate_event_slots[i].callback.compare_exchange_strong(
                expected, callback,
                std::memory_order_release, std::memory_order_acquire)) {
            g_hotupdate_event_slots[i].user_data = user_data;
            return true;
        }
    }
    return false;
}

/// Fire an event to all registered callbacks.
inline void HotupdateFireEvent(HotupdateEvent event,
                               const HotupdateEventData& data) noexcept {
    for (int i = 0; i < kMaxHotupdateEventCallbacks; i++) {
        auto cb = g_hotupdate_event_slots[i].callback.load(std::memory_order_acquire);
        if (cb != nullptr) {
            cb(event, &data, g_hotupdate_event_slots[i].user_data);
        }
    }
}

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_HOTUPDATE_EVENTS_H_
