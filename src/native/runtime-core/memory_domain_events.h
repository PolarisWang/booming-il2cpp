// memory_domain_events.h — Memory Domain lifecycle event callback system
//
// Lightweight callback-based event system for memory domain lifecycle events.
// Follows the same pattern as gc_events.h and hotupdate_events.h (fixed-slot
// callback table with atomic CAS registration).
//
// Thread safety: FireEvent iterates the slot table with memory_order_acquire
// loads. The data struct is valid only during the callback invocation —
// callbacks must not store the pointer for later use.

#ifndef CHAOS_IL2CPP_MEMORY_DOMAIN_EVENTS_H_
#define CHAOS_IL2CPP_MEMORY_DOMAIN_EVENTS_H_

#include <atomic>
#include <cstdint>

namespace chaos::il2cpp::memory_domain {

// ── Event types ─────────────────────────────────────────────────────────
enum class MemoryDomainEvent : uint8_t {
    DOMAIN_REGISTERED            = 0,  // New domain created
    DOMAIN_UNLOADED              = 1,  // Domain destroyed
    DOMAIN_USAGE_LIMIT_EXCEEDED  = 2,  // Allocation rejected by usage limit
};

// ── Event payload ───────────────────────────────────────────────────────
struct MemoryDomainEventData {
    uint32_t     domain_id       = 0;
    const char*  module_name     = nullptr;  // Valid only during callback
    uint32_t     module_kind     = 0;
    int64_t      current_usage   = 0;
    int64_t      usage_limit     = 0;
};

// ── Callback table ──────────────────────────────────────────────────────
static constexpr int kMaxMemoryDomainEventCallbacks = 8;

using MemoryDomainEventCallback = void (*)(MemoryDomainEvent event,
                                            const MemoryDomainEventData* data,
                                            void* user_data);

struct MemoryDomainEventSlot {
    std::atomic<MemoryDomainEventCallback> callback{nullptr};
    void* user_data = nullptr;
};

// Global callback table. Inline (C++17) — defined once across all TUs.
inline MemoryDomainEventSlot g_memory_domain_event_slots[kMaxMemoryDomainEventCallbacks] = {};

/// Register a callback for memory domain lifecycle events.
/// Returns true on success, false if the callback table is full.
inline bool MemoryDomainRegisterEventCallback(MemoryDomainEventCallback callback, void* user_data) {
    if (callback == nullptr) return false;

    for (int i = 0; i < kMaxMemoryDomainEventCallbacks; i++) {
        MemoryDomainEventCallback expected = nullptr;
        if (g_memory_domain_event_slots[i].callback.compare_exchange_strong(
                expected, callback,
                std::memory_order_release, std::memory_order_acquire)) {
            g_memory_domain_event_slots[i].user_data = user_data;
            return true;
        }
    }
    return false;
}

/// Fire an event to all registered callbacks.
inline void MemoryDomainFireEvent(MemoryDomainEvent event,
                                   const MemoryDomainEventData& data) noexcept {
    for (int i = 0; i < kMaxMemoryDomainEventCallbacks; i++) {
        auto cb = g_memory_domain_event_slots[i].callback.load(std::memory_order_acquire);
        if (cb != nullptr) {
            cb(event, &data, g_memory_domain_event_slots[i].user_data);
        }
    }
}

}  // namespace chaos::il2cpp::memory_domain

#endif  // CHAOS_IL2CPP_MEMORY_DOMAIN_EVENTS_H_
