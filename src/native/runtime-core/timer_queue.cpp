#include "timer_queue.h"
#include "thread_state.h"
#include "gc_transition.h"

#include <atomic>
#include <chrono>
#include <cstdint>
#include <mutex>
#include <vector>
#include <algorithm>

namespace chaos::il2cpp::runtime_core::threading {

// ── Timer entry ─────────────────────────────────────────────────────────

struct TimerEntry {
    uint32_t id;
    void (*callback)(void*);
    void* state;
    uint64_t next_fire_tick;   // Absolute tick count when this timer should fire next.
    uint32_t period_ms;        // 0 = one-shot, >0 = periodic.
    bool active;
};

// ── Global timer queue state ────────────────────────────────────────────

namespace {

/// Protects all timer state.
CHAOS_IL2CPP_MUTEX s_timer_mutex;

/// All timer entries (including free slots).
std::vector<TimerEntry> s_timers;

/// Monotonically increasing timer ID allocator.
uint32_t s_next_timer_id = 1;

/// High-resolution tick counter for deadline comparisons.
/// Monotonically increasing, reset on init.
uint64_t s_timer_epoch_ms = 0;

/// Current tick count (ms since epoch).
uint64_t GetCurrentTickMs() noexcept {
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
}

/// Find an active timer by ID.  Caller must hold s_timer_mutex.
TimerEntry* FindTimer(uint32_t id) noexcept {
    for (auto& t : s_timers) {
        if (t.id == id && t.active) return &t;
    }
    return nullptr;
}

}  // anonymous namespace

// ── Public API ──────────────────────────────────────────────────────────

void TimerQueueInitialize() noexcept {
    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_timer_mutex);
    s_timers.clear();
    s_timers.reserve(kTimerQueueMaxEntries);
    s_next_timer_id = 1;
    s_timer_epoch_ms = GetCurrentTickMs();
}

void TimerQueueShutdown() noexcept {
    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_timer_mutex);
    s_timers.clear();
}

uint32_t TimerQueueCreate(
    void (*callback)(void*),
    void* state,
    uint32_t due_time_ms,
    uint32_t period_ms) noexcept
{
    if (callback == nullptr) return kTimerQueueInvalidId;

    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_timer_mutex);

    if (s_timers.size() >= kTimerQueueMaxEntries) {
        return kTimerQueueInvalidId;
    }

    uint32_t id = s_next_timer_id++;
    if (id == kTimerQueueInvalidId) id = s_next_timer_id++;  // skip sentinel

    uint64_t now = GetCurrentTickMs();
    uint64_t first_fire = now + due_time_ms;

    s_timers.push_back(TimerEntry{
        id,
        callback,
        state,
        first_fire,
        period_ms,
        true
    });

    return id;
}

bool TimerQueueChange(uint32_t timer_id, uint32_t due_time_ms, uint32_t period_ms) noexcept {
    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_timer_mutex);

    auto* entry = FindTimer(timer_id);
    if (entry == nullptr) return false;

    uint64_t now = GetCurrentTickMs();
    entry->next_fire_tick = now + due_time_ms;
    entry->period_ms = period_ms;
    return true;
}

bool TimerQueueDelete(uint32_t timer_id) noexcept {
    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_timer_mutex);

    auto* entry = FindTimer(timer_id);
    if (entry == nullptr) return false;

    entry->active = false;
    return true;
}

void TimerQueueOnTick() noexcept {
    uint64_t now = GetCurrentTickMs();

    // Collect due timers under lock, then fire outside lock.
    // This avoids holding the mutex across user callbacks.
    struct DueEntry {
        void (*callback)(void*);
        void* state;
        uint32_t period_ms;
    };
    std::vector<DueEntry> due;

    {
        std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_timer_mutex);

        for (auto& t : s_timers) {
            if (!t.active) continue;
            if (t.next_fire_tick <= now) {
                due.push_back({t.callback, t.state, t.period_ms});

                if (t.period_ms > 0) {
                    // Periodic: reschedule for next period.
                    // Advance by full periods to catch up if we missed ticks.
                    uint64_t elapsed = now - t.next_fire_tick;
                    uint32_t missed_periods = static_cast<uint32_t>(elapsed / t.period_ms);
                    t.next_fire_tick += static_cast<uint64_t>(missed_periods + 1) * t.period_ms;
                } else {
                    // One-shot: mark inactive.
                    t.active = false;
                }
            }
        }
    }

    // Fire due timers (outside lock).
    for (auto& d : due) {
        if (d.callback) {
            GC_TRANSITION_TO_COOPERATIVE();
            d.callback(d.state);
            GC_TRANSITION_TO_PREEMPTIVE();
        }
    }
}

}  // namespace chaos::il2cpp::runtime_core::threading