#include "timer_queue.h"
#include "thread_state.h"
#include "gc_transition.h"
#include "thread_pool.h"

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

// ── Id→Index lookup: open-addressing hash table ────────────────────────
// 2048 slots (2× max entries, power of 2 for fast modulo).
// id == 0 (kTimerQueueInvalidId) means empty slot.
// On deletion, the slot is cleared (tombstone-free): a subsequent lookup
// that probes past a cleared slot may miss an entry that was positioned
// behind it. This is benign: the caller always validates against the heap
// entry, so a miss just means Change/Delete returns false (~timer not found).

constexpr uint32_t kIdIndexSlots = 2048u;
struct IdIndexSlot { uint32_t id; uint32_t heap_index; };
IdIndexSlot s_id_to_idx[kIdIndexSlots]{};

inline uint32_t IdxHash(uint32_t id) noexcept {
    return id & (kIdIndexSlots - 1u);
}

uint32_t IdxFindSlot(uint32_t timer_id) noexcept;
void IdxSet(uint32_t timer_id, uint32_t heap_index) noexcept;
void IdxErase(uint32_t timer_id) noexcept;

uint32_t IdxFindSlot(uint32_t timer_id) noexcept {
    uint32_t h = IdxHash(timer_id);
    for (uint32_t probe = 0; probe < kIdIndexSlots; probe++) {
        uint32_t i = (h + probe) & (kIdIndexSlots - 1u);
        if (s_id_to_idx[i].id == timer_id) return i;
        if (s_id_to_idx[i].id == 0) return UINT32_MAX;
    }
    return UINT32_MAX;
}

void IdxSet(uint32_t timer_id, uint32_t heap_index) noexcept {
    uint32_t h = IdxHash(timer_id);
    for (uint32_t probe = 0; probe < kIdIndexSlots; probe++) {
        uint32_t i = (h + probe) & (kIdIndexSlots - 1u);
        auto& slot = s_id_to_idx[i];
        if (slot.id == 0 || slot.id == timer_id) {
            slot.id = timer_id;
            slot.heap_index = heap_index;
            return;
        }
    }
}

void IdxErase(uint32_t timer_id) noexcept {
    uint32_t slot = IdxFindSlot(timer_id);
    if (slot != UINT32_MAX) {
        s_id_to_idx[slot].id = 0;
        s_id_to_idx[slot].heap_index = 0;
    }
}

// ── Min-heap operations ─────────────────────────────────────────────────
void HeapSiftUp(std::vector<TimerEntry>& heap, uint32_t idx) noexcept {
    while (idx > kRootIndex) {
        uint32_t parent = HeapParent(idx);
        if (heap[idx].next_fire_tick >= heap[parent].next_fire_tick) break;
        // Update id→index map before swap.
        IdxSet(heap[idx].id, parent);
        IdxSet(heap[parent].id, idx);
        std::swap(heap[idx], heap[parent]);
        idx = parent;
    }
}

/// Sift down: move element at idx down until heap property is restored.
/// Updates the id→index lookup for each swapped element.
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
        // Update id→index map before swap.
        IdxSet(heap[idx].id, smallest);
        IdxSet(heap[smallest].id, idx);
        std::swap(heap[idx], heap[smallest]);
        idx = smallest;
    }
}

/// Remove stale (inactive) entries from the heap top.
/// Lazy-clean: only removes inactive entries at the root to avoid O(n) scan.
/// Also removes stale entries from the id→index map.
/// Caller must hold s_timer_mutex.
void HeapCleanRoot(std::vector<TimerEntry>& heap) noexcept {
    while (!heap.empty() && !heap[kRootIndex].active) {
        IdxErase(heap[kRootIndex].id);
        if (heap.size() > 1) {
            IdxSet(heap.back().id, kRootIndex);
        }
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
    // Clear id→index lookup table.
    for (auto& slot : s_id_to_idx) {
        slot.id = 0;
        slot.heap_index = 0;
    }
    s_next_timer_id = 1;
}

void TimerQueueShutdown() noexcept {
    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_timer_mutex);
    s_timer_heap.clear();
    for (auto& slot : s_id_to_idx) {
        slot.id = 0;
        slot.heap_index = 0;
    }
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

    uint32_t idx = static_cast<uint32_t>(s_timer_heap.size());
    s_timer_heap.push_back(TimerEntry{
        id,
        callback,
        state,
        first_fire,
        period_ms,
        true
    });
    IdxSet(id, idx);

    // Restore heap property.
    HeapSiftUp(s_timer_heap, idx);

    return id;
}

bool TimerQueueChange(uint32_t timer_id, uint32_t due_time_ms, uint32_t period_ms) noexcept {
    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_timer_mutex);

    uint32_t slot = IdxFindSlot(timer_id);
    if (slot == UINT32_MAX) return false;

    uint32_t i = s_id_to_idx[slot].heap_index;
    if (i >= s_timer_heap.size() || !s_timer_heap[i].active || s_timer_heap[i].id != timer_id) {
        // Stale entry in lookup table — clean it.
        IdxErase(timer_id);
        return false;
    }

    auto& entry = s_timer_heap[i];
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

bool TimerQueueDelete(uint32_t timer_id) noexcept {
    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_timer_mutex);

    uint32_t slot = IdxFindSlot(timer_id);
    if (slot == UINT32_MAX) return false;

    uint32_t i = s_id_to_idx[slot].heap_index;
    if (i >= s_timer_heap.size() || !s_timer_heap[i].active || s_timer_heap[i].id != timer_id) {
        IdxErase(timer_id);
        return false;
    }

    s_timer_heap[i].active = false;
    IdxErase(timer_id);

    // If this was the root, clean it now. Otherwise it'll be cleaned
    // lazily when it surfaces to the root via sift operations.
    if (i == kRootIndex) {
        HeapCleanRoot(s_timer_heap);
    }
    return true;
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
            IdxErase(root.id);
            due.push_back({root.callback, root.state, root.period_ms});

            if (root.period_ms > 0) {
                // Periodic: reschedule for next period.
                // Advance by full periods to catch up if we missed ticks.
                uint64_t elapsed = now - root.next_fire_tick;
                uint32_t missed_periods = static_cast<uint32_t>(elapsed / root.period_ms);
                root.next_fire_tick += static_cast<uint64_t>(missed_periods + 1) * root.period_ms;

                // Replace root with rescheduled entry and sift down.
                IdxSet(root.id, kRootIndex);
                s_timer_heap[kRootIndex] = root;
                HeapSiftDown(s_timer_heap, kRootIndex);
            } else {
                // One-shot: pop root and sift down (swap with last, pop, sift_down).
                if (s_timer_heap.size() > 1) {
                    IdxSet(s_timer_heap.back().id, kRootIndex);
                }
                s_timer_heap[kRootIndex] = s_timer_heap.back();
                s_timer_heap.pop_back();
                if (!s_timer_heap.empty()) {
                    HeapSiftDown(s_timer_heap, kRootIndex);
                }
            }
        }
    }

    // Fire due timers via thread pool (outside lock) to avoid blocking
    // the gate thread and delaying HillClimbing gate ticks.
    for (auto& d : due) {
        if (d.callback) {
            ThreadPoolQueueUserWorkItemUnsafe(d.callback, d.state);
        }
    }
}

}  // namespace chaos::il2cpp::runtime_core::threading
