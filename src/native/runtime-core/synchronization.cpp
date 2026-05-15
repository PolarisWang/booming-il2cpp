#include "synchronization.h"
#include "thread_state.h"
#include "gc_transition.h"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <list>
#include <mutex>

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

struct RWLockEntry {
    uint32_t id;
    bool active;
    CHAOS_IL2CPP_MUTEX mutex;
    CHAOS_IL2CPP_CONDITION_VARIABLE cv;
    int32_t readers;       // Number of active readers (-1 = writer active).
    int32_t waiting_readers;
    int32_t waiting_writers;
};

CHAOS_IL2CPP_MUTEX s_sem_table_mutex;
std::list<SemaphoreEntry> s_semaphores;
uint32_t s_next_sem_id = 1;

CHAOS_IL2CPP_MUTEX s_rw_table_mutex;
std::list<RWLockEntry> s_rwlocks;
uint32_t s_next_rw_id = 1;

SemaphoreEntry* FindSemaphore(uint32_t id) noexcept {
    for (auto& s : s_semaphores) {
        if (s.id == id && s.active) return &s;
    }
    return nullptr;
}

RWLockEntry* FindRWLock(uint32_t id) noexcept {
    for (auto& rw : s_rwlocks) {
        if (rw.id == id && rw.active) return &rw;
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

// ── ReaderWriterLockSlim ────────────────────────────────────────────────

uint32_t ReaderWriterLockSlimCreate() noexcept {
    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_rw_table_mutex);

    uint32_t id = s_next_rw_id++;
    if (id == 0) id = s_next_rw_id++;

    auto& rw_entry = s_rwlocks.emplace_back();
    rw_entry.id = id;
    rw_entry.active = true;
    rw_entry.readers = 0;
    rw_entry.waiting_readers = 0;
    rw_entry.waiting_writers = 0;

    return id;
}

bool ReaderWriterLockSlimDestroy(uint32_t rw_id) noexcept {
    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_rw_table_mutex);
    auto* entry = FindRWLock(rw_id);
    if (entry == nullptr) return false;
    entry->active = false;
    entry->cv.notify_all();
    return true;
}

int32_t ReaderWriterLockSlimEnterRead(uint32_t rw_id, int32_t timeout_ms) noexcept {
    RWLockEntry* entry;
    {
        std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_rw_table_mutex);
        entry = FindRWLock(rw_id);
    }
    if (entry == nullptr) return -1;

    GC_TRANSITION_TO_PREEMPTIVE();
    std::unique_lock<CHAOS_IL2CPP_MUTEX> lock(entry->mutex);

    // Fast path: no writer active or waiting.
    if (entry->readers >= 0 && entry->waiting_writers == 0) {
        entry->readers++;
        GC_TRANSITION_TO_COOPERATIVE();
        return 1;
    }

    if (timeout_ms == 0) {
        GC_TRANSITION_TO_COOPERATIVE();
        return 0;
    }

    entry->waiting_readers++;
    int32_t result;
    if (timeout_ms < 0) {
        entry->cv.wait(lock, [entry] {
            return entry->readers >= 0 && entry->waiting_writers == 0;
        });
        result = 1;
    } else {
        bool acquired = entry->cv.wait_for(lock, std::chrono::milliseconds(timeout_ms),
            [entry] { return entry->readers >= 0 && entry->waiting_writers == 0; });
        result = acquired ? 1 : 0;
    }
    entry->waiting_readers--;

    if (result == 1) {
        entry->readers++;
    }

    GC_TRANSITION_TO_COOPERATIVE();
    return result;
}

bool ReaderWriterLockSlimExitRead(uint32_t rw_id) noexcept {
    RWLockEntry* entry;
    {
        std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_rw_table_mutex);
        entry = FindRWLock(rw_id);
    }
    if (entry == nullptr) return false;

    {
        std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(entry->mutex);
        if (entry->readers <= 0) return false;
        entry->readers--;
    }

    entry->cv.notify_all();
    return true;
}

int32_t ReaderWriterLockSlimEnterWrite(uint32_t rw_id, int32_t timeout_ms) noexcept {
    RWLockEntry* entry;
    {
        std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_rw_table_mutex);
        entry = FindRWLock(rw_id);
    }
    if (entry == nullptr) return -1;

    GC_TRANSITION_TO_PREEMPTIVE();
    std::unique_lock<CHAOS_IL2CPP_MUTEX> lock(entry->mutex);

    // Fast path: no readers, no writer.
    if (entry->readers == 0) {
        entry->readers = -1;  // Mark writer active.
        GC_TRANSITION_TO_COOPERATIVE();
        return 1;
    }

    if (timeout_ms == 0) {
        GC_TRANSITION_TO_COOPERATIVE();
        return 0;
    }

    entry->waiting_writers++;
    int32_t result;
    if (timeout_ms < 0) {
        entry->cv.wait(lock, [entry] { return entry->readers == 0; });
        result = 1;
    } else {
        bool acquired = entry->cv.wait_for(lock, std::chrono::milliseconds(timeout_ms),
            [entry] { return entry->readers == 0; });
        result = acquired ? 1 : 0;
    }
    entry->waiting_writers--;

    if (result == 1) {
        entry->readers = -1;  // Mark writer active.
    }

    GC_TRANSITION_TO_COOPERATIVE();
    return result;
}

bool ReaderWriterLockSlimExitWrite(uint32_t rw_id) noexcept {
    RWLockEntry* entry;
    {
        std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_rw_table_mutex);
        entry = FindRWLock(rw_id);
    }
    if (entry == nullptr) return false;

    {
        std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(entry->mutex);
        if (entry->readers != -1) return false;
        entry->readers = 0;
    }

    entry->cv.notify_all();
    return true;
}

}  // namespace chaos::il2cpp::runtime_core::threading