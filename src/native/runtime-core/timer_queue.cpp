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
    uint64_t next_fire_tick;   // Absolute tick count when this timer should fire next (heap key).
    uint32_t period_ms;        // 0 = one-shot, >0 = periodic.
    bool active;
};

// ── Min-heap helpers ─────────────────────────────────────────────────────
// The heap is keyed by next_fire_tick (smallest = earliest = next to fire).

namespace {

constexpr uint32_t kRootIndex = 0u;

inline uint32_t HeapParent(uint32_t i) noexcept { return (i - 1u) / 2u; }
inline uint32_t HeapLeftChild(uint32_t i) noexcept { return 2u * i + 1u; }
inline uint32_t HeapRightChild(uint32_t i) noexcept { return 2u * i + 2u; }

/// Sift up: move element at idx up until heap property is restored.
/// Caller must hold s_timer_mutex.
void HeapSiftUp(std::vector<TimerEntry>& heap, uint32_t idx) noexcept {
    while (idx > kRootIndex) {
        uint32_t parent = HeapParent(idx);
        if (heap[idx].next_fire_tick >= heap[parent].next_fire_tick) break;
        std::swap(heap[idx], heap[parent]);
        idx = parent;
    }
}

/// Sift down: move element at idx down until heap property is restored.
/// Caller must hold s_timer_mutex.
void HeapSiftDown(std::vector<TimerEntry>& heap, uint32_t idx) noexcept {
    uint32_t n = static_cast<uint32_t>(heap.size());
    for (;;) {
        uint32_t smallest = idx;
        uint32_t left = HeapLeftChild(idx);
        uint32_t right = HeapRightChild(idx);
        if (left < n && heap[left].next_fire_tick < heap[smallest].next_fire_tick) {
            smallest = left;
        }
        if (right < n && heap[right].next_fire_tick < heap[smallest].next_fire_tick) {
            smallest = right;
        }
        if (smallest == idx) break;
        std::swap(heap[idx], heap[smallest]);
        idx = smallest;
    }
}

/// Remove stale (inactive) entries from the heap top.
/// Lazy-clean: only removes inactive entries at the root to avoid O(n) scan.
/// Caller must hold s_timer_mutex.
void HeapCleanRoot(std::vector<TimerEntry>& heap) noexcept {
    while (!heap.empty() && !heap[kRootIndex].active) {
        heap[kRootIndex] = heap.back();
        heap.pop_back();
        if (!heap.empty()) HeapSiftDown(heap, kRootIndex);
    }
}

}  // anonymous namespace

// ── Global timer queue state ────────────────────────────────────────────

namespace {

/// Protects all timer state.
CHAOS_IL2CPP_MUTEX s_timer_mutex;

/// Binary min-heap of timer entries, keyed by next_fire_tick.
/// Root (index 0) is the next timer to fire.
/// Inactive entries are lazily removed from the root.
std::vector<TimerEntry> s_timer_heap;

/// Monotonically increasing timer ID allocator.
uint32_t s_next_timer_id = 1;

/// Current tick count (ms since epoch).
uint64_t GetCurrentTickMs() noexcept {
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
}

}  // anonymous namespace

// ── Public API ──────────────────────────────────────────────────────────

void TimerQueueInitialize() noexcept {
    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_timer_mutex);
    s_timer_heap.clear();
    s_timer_heap.reserve(kTimerQueueMaxEntries);
    s_next_timer_id = 1;
}

void TimerQueueShutdown() noexcept {
    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_timer_mutex);
    s_timer_heap.clear();
}

uint32_t TimerQueueCreate(
    void (*callback)(void*),
    void* state,
    uint32_t due_time_ms,
    uint32_t period_ms) noexcept
{
    if (callback == nullptr) return kTimerQueueInvalidId;

    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_timer_mutex);

    if (s_timer_heap.size() >= kTimerQueueMaxEntries) {
        return kTimerQueueInvalidId;
    }

    uint32_t id = s_next_timer_id++;
    if (id == kTimerQueueInvalidId) id = s_next_timer_id++;  // skip sentinel

    uint64_t now = GetCurrentTickMs();
    uint64_t first_fire = now + due_time_ms;

    s_timer_heap.push_back(TimerEntry{
        id,
        callback,
        state,
        first_fire,
        period_ms,
        true
    });

    // Restore heap property.
    HeapSiftUp(s_timer_heap, static_cast<uint32_t>(s_timer_heap.size() - 1u));

    return id;
}

bool TimerQueueChange(uint32_t timer_id, uint32_t due_time_ms, uint32_t period_ms) noexcept {
    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_timer_mutex);

    // Linear scan to find the entry (rare — only on Timer.Change).
    for (uint32_t i = 0; i < s_timer_heap.size(); i++) {
        auto& entry = s_timer_heap[i];
        if (entry.id == timer_id && entry.active) {
            uint64_t old_tick = entry.next_fire_tick;
            uint64_t now = GetCurrentTickMs();
            entry.next_fire_tick = now + due_time_ms;
            entry.period_ms = period_ms;

            // Restore heap property: sift up or down depending on direction.
            if (entry.next_fire_tick < old_tick) {
                HeapSiftUp(s_timer_heap, i);
            } else if (entry.next_fire_tick > old_tick) {
                HeapSiftDown(s_timer_heap, i);
            }
            return true;
        }
    }
    return false;
}

bool TimerQueueDelete(uint32_t timer_id) noexcept {
    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_timer_mutex);

    // Linear scan to find the entry.
    for (uint32_t i = 0; i < s_timer_heap.size(); i++) {
        if (s_timer_heap[i].id == timer_id && s_timer_heap[i].active) {
            s_timer_heap[i].active = false;
            // If this was the root, clean it now. Otherwise it'll be cleaned
            // lazily when it surfaces to the root via sift operations.
            if (i == kRootIndex) {
                HeapCleanRoot(s_timer_heap);
            }
            return true;
        }
    }
    return false;
}

void TimerQueueOnTick() noexcept {
    uint64_t now = GetCurrentTickMs();

    // Collect due timers under lock, then fire outside lock.
    struct DueEntry {
        void (*callback)(void*);
        void* state;
        uint32_t period_ms;
    };
    std::vector<DueEntry> due;

    {
        std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_timer_mutex);

        // Drain all due timers from the heap root.
        for (;;) {
            HeapCleanRoot(s_timer_heap);
            if (s_timer_heap.empty()) break;
            if (s_timer_heap[kRootIndex].next_fire_tick > now) break;

            TimerEntry root = s_timer_heap[kRootIndex];  // copy
            due.push_back({root.callback, root.state, root.period_ms});

            if (root.period_ms > 0) {
                // Periodic: reschedule for next period.
                // Advance by full periods to catch up if we missed ticks.
                uint64_t elapsed = now - root.next_fire_tick;
                uint32_t missed_periods = static_cast<uint32_t>(elapsed / root.period_ms);
                root.next_fire_tick += static_cast<uint64_t>(missed_periods + 1) * root.period_ms;

                // Replace root with rescheduled entry and sift down.
                s_timer_heap[kRootIndex] = root;
                HeapSiftDown(s_timer_heap, kRootIndex);
            } else {
                // One-shot: pop root and sift down (swap with last, pop, sift_down).
                s_timer_heap[kRootIndex] = s_timer_heap.back();
                s_timer_heap.pop_back();
                if (!s_timer_heap.empty()) {
                    HeapSiftDown(s_timer_heap, kRootIndex);
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
