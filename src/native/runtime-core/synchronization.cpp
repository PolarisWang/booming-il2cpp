#include "synchronization.h"
#include "thread_state.h"
#include "gc_transition.h"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <list>
#include <mutex>
#include <new>

#if defined(_MSC_VER)
#include <intrin.h>
#endif

namespace chaos::il2cpp::runtime_core::threading {

// ── SemaphoreSlim ───────────────────────────────────────────────────────

namespace {

struct SemaphoreEntry {
    uint32_t id;
    std::atomic<int32_t> count;
    int32_t max_count;
    bool active;
    CHAOS_IL2CPP_MUTEX mutex;
    CHAOS_IL2CPP_CONDITION_VARIABLE cv;
};

// ── ReaderWriterLockSlim — Interlocked-based ───────────────────────────
//
// Design:
//   Fixed array g_rwlocks[1024] — handle = index, O(1) lookup.
//   State atomic: >=0 = reader count, -1 = writer active.
//   Read lock: InterlockedIncrement on state (zero syscall uncontended).
//   Write lock: CAS state 0 → -1 (zero syscall uncontended).
//   Spin 64 iterations before falling back to mutex+cv.

constexpr uint32_t kMaxRWLockCount = 1024;

struct RWLockEntryFixed {
    std::atomic<int32_t> state{0};  // >=0: readers, -1: writer
    uint32_t id{0};
    bool active{false};
    CHAOS_IL2CPP_MUTEX mutex;
    CHAOS_IL2CPP_CONDITION_VARIABLE cv;
    int32_t waiting_readers{0};
    int32_t waiting_writers{0};
};

struct BarrierEntry {
    uint32_t id;
    bool active;
    CHAOS_IL2CPP_MUTEX mutex;
    CHAOS_IL2CPP_CONDITION_VARIABLE cv;
    int32_t participant_count;
    int32_t remaining;
    int64_t phase_number;
};

struct CountdownEventEntry {
    uint32_t id;
    bool active;
    CHAOS_IL2CPP_MUTEX mutex;
    CHAOS_IL2CPP_CONDITION_VARIABLE cv;
    int32_t count;
};

CHAOS_IL2CPP_MUTEX s_sem_table_mutex;
std::list<SemaphoreEntry> s_semaphores;
uint32_t s_next_sem_id = 1;

// RWLock: fixed array for O(1) lookup. Entries are never freed (only marked
// active=false), so pointer stability is guaranteed.
RWLockEntryFixed g_rwlocks[kMaxRWLockCount];
uint32_t s_next_rw_id = 1;

CHAOS_IL2CPP_MUTEX s_barrier_table_mutex;
std::list<BarrierEntry> s_barriers;
uint32_t s_next_barrier_id = 1;

CHAOS_IL2CPP_MUTEX s_ce_table_mutex;
std::list<CountdownEventEntry> s_countdown_events;
uint32_t s_next_ce_id = 1;

SemaphoreEntry* FindSemaphore(uint32_t id) noexcept {
    for (auto& s : s_semaphores) {
        if (s.id == id && s.active) return &s;
    }
    return nullptr;
}

/// O(1) RWLock lookup: handle is the array index.
RWLockEntryFixed* FindRWLockFixed(uint32_t handle) noexcept {
    if (handle == 0 || handle >= kMaxRWLockCount) return nullptr;
    auto* entry = &g_rwlocks[handle];
    return entry->active ? entry : nullptr;
}

BarrierEntry* FindBarrier(uint32_t id) noexcept {
    for (auto& b : s_barriers) {
        if (b.id == id && b.active) return &b;
    }
    return nullptr;
}

CountdownEventEntry* FindCountdownEvent(uint32_t id) noexcept {
    for (auto& ce : s_countdown_events) {
        if (ce.id == id && ce.active) return &ce;
    }
    return nullptr;
}

}  // anonymous namespace

uint32_t SemaphoreSlimCreate(int32_t initial_count, int32_t max_count) noexcept {
    if (initial_count < 0 || max_count <= 0) return 0;

    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_sem_table_mutex);

    uint32_t id = s_next_sem_id++;
    if (id == 0) id = s_next_sem_id++;

    s_semaphores.emplace_back();
    auto& entry = s_semaphores.back();
    entry.id = id;
    entry.count.store(initial_count, std::memory_order_relaxed);
    entry.max_count = max_count;
    entry.active = true;

    return id;
}

bool SemaphoreSlimDestroy(uint32_t sem_id) noexcept {
    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_sem_table_mutex);
    auto* entry = FindSemaphore(sem_id);
    if (entry == nullptr) return false;
    entry->active = false;
    entry->cv.notify_all();
    return true;
}

int32_t SemaphoreSlimWait(uint32_t sem_id, int32_t timeout_ms) noexcept {
    SemaphoreEntry* entry;
    {
        std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_sem_table_mutex);
        entry = FindSemaphore(sem_id);
    }
    if (entry == nullptr) return -1;

    GC_TRANSITION_TO_PREEMPTIVE();

    std::unique_lock<CHAOS_IL2CPP_MUTEX> lock(entry->mutex);

    // Fast path: count > 0.
    int32_t c = entry->count.load(std::memory_order_relaxed);
    if (c > 0) {
        entry->count.store(c - 1, std::memory_order_relaxed);
        GC_TRANSITION_TO_COOPERATIVE();
        return 1;
    }

    if (timeout_ms == 0) {
        GC_TRANSITION_TO_COOPERATIVE();
        return 0;  // Poll: not available.
    }

    // Blocking wait.
    int32_t result;
    if (timeout_ms < 0) {
        entry->cv.wait(lock, [entry] {
            return entry->count.load(std::memory_order_relaxed) > 0;
        });
        result = 1;
    } else {
        bool acquired = entry->cv.wait_for(lock, std::chrono::milliseconds(timeout_ms),
            [entry] { return entry->count.load(std::memory_order_relaxed) > 0; });
        result = acquired ? 1 : 0;
    }

    if (result == 1) {
        entry->count.fetch_sub(1, std::memory_order_relaxed);
    }

    GC_TRANSITION_TO_COOPERATIVE();
    return result;
}

int32_t SemaphoreSlimRelease(uint32_t sem_id, int32_t count) noexcept {
    if (count <= 0) return -1;

    SemaphoreEntry* entry;
    {
        std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_sem_table_mutex);
        entry = FindSemaphore(sem_id);
    }
    if (entry == nullptr) return -1;

    {
        std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(entry->mutex);
        int32_t current = entry->count.load(std::memory_order_relaxed);
        if (current + count > entry->max_count) return -1;
        entry->count.store(current + count, std::memory_order_relaxed);
    }

    entry->cv.notify_all();
    return 0;
}

// ── ReaderWriterLockSlim — Interlocked-based ────────────────────────────
//
// Fast paths (no contention, zero syscalls):
//   Read lock:  atomic increment of state (InterlockedIncrement)
//   Write lock: CAS state 0 → -1
//   Exit read:  atomic decrement + check waiting_writers
//   Exit write: store 0 + notify
//
// Slow paths (contention): fall back to mutex + condition_variable.
// Spin 64 iterations before blocking.

uint32_t ReaderWriterLockSlimCreate() noexcept {
    // Find a free slot in the fixed array.
    for (uint32_t i = 1; i < kMaxRWLockCount; i++) {
        auto& entry = g_rwlocks[i];
        uint32_t expected_id = 0;
        if (entry.id == 0) {
            // Unused slot — claim it.
            entry.id = s_next_rw_id++;
            if (entry.id == 0) entry.id = s_next_rw_id++;
            entry.active = true;
            entry.state.store(0, std::memory_order_relaxed);
            entry.waiting_readers = 0;
            entry.waiting_writers = 0;
            return i;  // handle = index
        }
    }
    return 0;  // table full
}

bool ReaderWriterLockSlimDestroy(uint32_t rw_handle) noexcept {
    auto* entry = FindRWLockFixed(rw_handle);
    if (entry == nullptr) return false;
    entry->active = false;
    entry->cv.notify_all();
    return true;
}

int32_t ReaderWriterLockSlimEnterRead(uint32_t rw_handle, int32_t timeout_ms) noexcept {
    auto* entry = FindRWLockFixed(rw_handle);
    if (entry == nullptr) return -1;

    // ── Fast path: Interlocked CAS on state ──────────────────────────
    // If state >= 0 (no writer), try to increment.
    int32_t expected = entry->state.load(std::memory_order_acquire);
    if (expected >= 0) {
        if (entry->state.compare_exchange_weak(expected, expected + 1,
                std::memory_order_acquire, std::memory_order_relaxed)) {
            return 1;  // Acquired without any syscall.
        }
    }

    // ── Spin 64 iterations (contention is likely brief) ──────────────
    if (expected < 0) {  // Writer active — spin waiting for it to finish.
        for (int i = 0; i < 64; i++) {
            CHAOS_IL2CPP_PAUSE_HINT();
            expected = entry->state.load(std::memory_order_acquire);
            if (expected >= 0) {
                if (entry->state.compare_exchange_weak(expected, expected + 1,
                        std::memory_order_acquire, std::memory_order_relaxed)) {
                    return 1;
                }
            }
        }
    }

    if (timeout_ms == 0) return 0;

    // ── Slow path: block on mutex + cv ───────────────────────────────
    GC_TRANSITION_TO_PREEMPTIVE();
    {
        std::unique_lock<CHAOS_IL2CPP_MUTEX> lock(entry->mutex);
        entry->waiting_readers++;

        int32_t result;
        if (timeout_ms < 0) {
            entry->cv.wait(lock, [entry] {
                int32_t s = entry->state.load(std::memory_order_acquire);
                return s >= 0 && entry->waiting_writers == 0;
            });
            result = 1;
        } else {
            bool acquired = entry->cv.wait_for(lock, std::chrono::milliseconds(timeout_ms),
                [entry] {
                    int32_t s = entry->state.load(std::memory_order_acquire);
                    return s >= 0 && entry->waiting_writers == 0;
                });
            result = acquired ? 1 : 0;
        }
        entry->waiting_readers--;

        if (result == 1) {
            entry->state.fetch_add(1, std::memory_order_release);
        }
        GC_TRANSITION_TO_COOPERATIVE();
        return result;
    }
}

bool ReaderWriterLockSlimExitRead(uint32_t rw_handle) noexcept {
    auto* entry = FindRWLockFixed(rw_handle);
    if (entry == nullptr) return false;

    int32_t prev = entry->state.fetch_sub(1, std::memory_order_release);
    if (prev <= 0) return false;  // Not a reader.

    // If there are waiting writers and this was the last reader, wake them.
    if (prev == 1 && entry->waiting_writers > 0) {
        entry->cv.notify_all();
    }
    return true;
}

int32_t ReaderWriterLockSlimEnterWrite(uint32_t rw_handle, int32_t timeout_ms) noexcept {
    auto* entry = FindRWLockFixed(rw_handle);
    if (entry == nullptr) return -1;

    // ── Fast path: CAS state 0 → -1 ──────────────────────────────────
    int32_t expected = 0;
    if (entry->state.compare_exchange_strong(expected, -1,
            std::memory_order_acquire, std::memory_order_relaxed)) {
        return 1;  // Acquired without any syscall.
    }

    // ── Spin 64 iterations ───────────────────────────────────────────
    for (int i = 0; i < 64; i++) {
        CHAOS_IL2CPP_PAUSE_HINT();
        expected = 0;
        if (entry->state.compare_exchange_strong(expected, -1,
                std::memory_order_acquire, std::memory_order_relaxed)) {
            return 1;
        }
    }

    if (timeout_ms == 0) return 0;

    // ── Slow path: block on mutex + cv ───────────────────────────────
    GC_TRANSITION_TO_PREEMPTIVE();
    {
        std::unique_lock<CHAOS_IL2CPP_MUTEX> lock(entry->mutex);
        entry->waiting_writers++;

        int32_t result;
        if (timeout_ms < 0) {
            entry->cv.wait(lock, [entry] {
                return entry->state.load(std::memory_order_acquire) == 0;
            });
            result = 1;
        } else {
            bool acquired = entry->cv.wait_for(lock, std::chrono::milliseconds(timeout_ms),
                [entry] {
                    return entry->state.load(std::memory_order_acquire) == 0;
                });
            result = acquired ? 1 : 0;
        }
        entry->waiting_writers--;

        if (result == 1) {
            entry->state.store(-1, std::memory_order_release);
        }
        GC_TRANSITION_TO_COOPERATIVE();
        return result;
    }
}

bool ReaderWriterLockSlimExitWrite(uint32_t rw_handle) noexcept {
    auto* entry = FindRWLockFixed(rw_handle);
    if (entry == nullptr) return false;

    int32_t prev = entry->state.exchange(0, std::memory_order_release);
    if (prev != -1) return false;  // Not the writer.

    // Wake waiters (both readers and writers).
    if (entry->waiting_readers > 0 || entry->waiting_writers > 0) {
        entry->cv.notify_all();
    }
    return true;
}

// ── Barrier ────────────────────────────────────────────────────────────

uint32_t BarrierCreate(int32_t participant_count) noexcept {
    if (participant_count < 1) return 0;

    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_barrier_table_mutex);

    uint32_t id = s_next_barrier_id++;
    if (id == 0) id = s_next_barrier_id++;

    s_barriers.emplace_back();
    auto& entry = s_barriers.back();
    entry.id = id;
    entry.active = true;
    entry.participant_count = participant_count;
    entry.remaining = participant_count;
    entry.phase_number = 0;

    return id;
}

bool BarrierDestroy(uint32_t barrier_id) noexcept {
    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_barrier_table_mutex);
    auto* entry = FindBarrier(barrier_id);
    if (entry == nullptr) return false;
    entry->active = false;
    entry->cv.notify_all();
    return true;
}

int32_t BarrierSignalAndWait(uint32_t barrier_id, int32_t timeout_ms) noexcept {
    BarrierEntry* entry;
    {
        std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_barrier_table_mutex);
        entry = FindBarrier(barrier_id);
    }
    if (entry == nullptr) return -1;

    GC_TRANSITION_TO_PREEMPTIVE();
    std::unique_lock<CHAOS_IL2CPP_MUTEX> lock(entry->mutex);

    // Decrement remaining.  If this is the last participant, signal all.
    int32_t remaining = --entry->remaining;
    if (remaining == 0) {
        // All arrived — advance phase and release everyone.
        entry->phase_number++;
        entry->remaining = entry->participant_count;
        lock.unlock();
        entry->cv.notify_all();
        GC_TRANSITION_TO_COOPERATIVE();
        return 1;
    }

    // Not the last — wait for the phase to complete.
    int64_t current_phase = entry->phase_number;
    int32_t result;
    if (timeout_ms == 0) {
        result = 0;  // Poll: would block.
    } else if (timeout_ms < 0) {
        entry->cv.wait(lock, [entry, current_phase] {
            return entry->phase_number != current_phase || !entry->active;
        });
        result = 1;
    } else {
        bool signaled = entry->cv.wait_for(lock, std::chrono::milliseconds(timeout_ms),
            [entry, current_phase] {
                return entry->phase_number != current_phase || !entry->active;
            });
        result = signaled ? 1 : 0;
    }

    GC_TRANSITION_TO_COOPERATIVE();
    return result;
}

int32_t BarrierGetRemainingParticipants(uint32_t barrier_id) noexcept {
    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_barrier_table_mutex);
    auto* entry = FindBarrier(barrier_id);
    return (entry != nullptr) ? entry->remaining : -1;
}

int64_t BarrierGetCurrentPhaseNumber(uint32_t barrier_id) noexcept {
    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_barrier_table_mutex);
    auto* entry = FindBarrier(barrier_id);
    return (entry != nullptr) ? entry->phase_number : -1;
}

// ── CountdownEvent ─────────────────────────────────────────────────────

uint32_t CountdownEventCreate(int32_t initial_count) noexcept {
    if (initial_count < 1) return 0;

    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_ce_table_mutex);

    uint32_t id = s_next_ce_id++;
    if (id == 0) id = s_next_ce_id++;

    s_countdown_events.emplace_back();
    auto& entry = s_countdown_events.back();
    entry.id = id;
    entry.active = true;
    entry.count = initial_count;

    return id;
}

bool CountdownEventDestroy(uint32_t ce_id) noexcept {
    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_ce_table_mutex);
    auto* entry = FindCountdownEvent(ce_id);
    if (entry == nullptr) return false;
    entry->active = false;
    entry->cv.notify_all();
    return true;
}

int32_t CountdownEventSignal(uint32_t ce_id, int32_t count) noexcept {
    if (count <= 0) return -1;

    CountdownEventEntry* entry;
    {
        std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_ce_table_mutex);
        entry = FindCountdownEvent(ce_id);
    }
    if (entry == nullptr) return -1;

    {
        std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(entry->mutex);
        int32_t current = entry->count;
        if (current == 0) return 1;  // Already signaled.
        int32_t new_count = current - count;
        if (new_count < 0) new_count = 0;
        entry->count = new_count;
        if (new_count == 0) {
            // Count reached zero — event is now signaled.
            entry->cv.notify_all();
            return 1;
        }
    }
    return 0;
}

int32_t CountdownEventWait(uint32_t ce_id, int32_t timeout_ms) noexcept {
    CountdownEventEntry* entry;
    {
        std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_ce_table_mutex);
        entry = FindCountdownEvent(ce_id);
    }
    if (entry == nullptr) return -1;

    GC_TRANSITION_TO_PREEMPTIVE();
    std::unique_lock<CHAOS_IL2CPP_MUTEX> lock(entry->mutex);

    // Fast path: already signaled.
    if (entry->count == 0) {
        GC_TRANSITION_TO_COOPERATIVE();
        return 1;
    }

    if (timeout_ms == 0) {
        GC_TRANSITION_TO_COOPERATIVE();
        return 0;
    }

    int32_t result;
    if (timeout_ms < 0) {
        entry->cv.wait(lock, [entry] { return entry->count == 0 || !entry->active; });
        result = 1;
    } else {
        bool signaled = entry->cv.wait_for(lock, std::chrono::milliseconds(timeout_ms),
            [entry] { return entry->count == 0 || !entry->active; });
        result = signaled ? 1 : 0;
    }

    GC_TRANSITION_TO_COOPERATIVE();
    return result;
}

int32_t CountdownEventGetCurrentCount(uint32_t ce_id) noexcept {
    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_ce_table_mutex);
    auto* entry = FindCountdownEvent(ce_id);
    return (entry != nullptr) ? entry->count : -1;
}

bool CountdownEventReset(uint32_t ce_id, int32_t count) noexcept {
    if (count < 1) return false;

    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_ce_table_mutex);
    auto* entry = FindCountdownEvent(ce_id);
    if (entry == nullptr) return false;

    std::lock_guard<CHAOS_IL2CPP_MUTEX> elock(entry->mutex);
    entry->count = count;
    return true;
}

}  // namespace chaos::il2cpp::runtime_core::threading