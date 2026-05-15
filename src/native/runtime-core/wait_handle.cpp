#include "wait_handle.h"
#include "thread_state.h"
#include "gc_transition.h"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <list>
#include <mutex>

namespace chaos::il2cpp::runtime_core::threading {

// ── Internal wait handle entry ──────────────────────────────────────────

struct WaitHandleEntry {
    uint32_t id;
    WaitHandleType type;
    bool signalled;
    bool active;
    CHAOS_IL2CPP_MUTEX mutex;
    CHAOS_IL2CPP_CONDITION_VARIABLE cv;
};

// ── Global state ────────────────────────────────────────────────────────

namespace {

CHAOS_IL2CPP_MUTEX s_handle_table_mutex;
std::list<WaitHandleEntry> s_handles;
uint32_t s_next_handle_id = 1;

WaitHandleEntry* FindHandle(uint32_t id) noexcept {
    for (auto& h : s_handles) {
        if (h.id == id && h.active) return &h;
    }
    return nullptr;
}

/// Allocate a new handle entry (list-stable: pointers survive insertion).
WaitHandleEntry* AllocateHandle() noexcept {
    s_handles.emplace_back();
    return &s_handles.back();
}

/// Wait on a handle with timeout, respecting GC mode transitions.
/// Caller must NOT hold s_handle_table_mutex.
int32_t WaitOnHandle(WaitHandleEntry* entry, int32_t timeout_ms) noexcept {
    if (entry == nullptr) return -1;

    // Set WaitSleepJoin state before blocking.
    auto* thread = GetCurrentThread();
    if (thread) thread->managed_state = ManagedThreadState::WaitSleepJoin;

    GC_TRANSITION_TO_PREEMPTIVE();

    std::unique_lock<CHAOS_IL2CPP_MUTEX> lock(entry->mutex);

    if (entry->signalled) {
        if (entry->type == WaitHandleType::AutoResetEvent) {
            entry->signalled = false;  // AutoReset: consume the signal
        }
        GC_TRANSITION_TO_COOPERATIVE();
        if (thread) thread->managed_state = ManagedThreadState::Running;
        return 1;  // signalled
    }

    if (timeout_ms == 0) {
        GC_TRANSITION_TO_COOPERATIVE();
        if (thread) thread->managed_state = ManagedThreadState::Running;
        return 0;  // poll: not signalled
    }

    int32_t result;
    if (timeout_ms < 0) {
        // Infinite wait.
        entry->cv.wait(lock, [entry] { return entry->signalled; });
        result = 1;
    } else {
        bool signalled = entry->cv.wait_for(lock, std::chrono::milliseconds(timeout_ms),
            [entry] { return entry->signalled; });
        result = signalled ? 1 : 0;
    }

    if (result == 1 && entry->type == WaitHandleType::AutoResetEvent) {
        entry->signalled = false;  // AutoReset: consume the signal
    }

    GC_TRANSITION_TO_COOPERATIVE();
    if (thread) thread->managed_state = ManagedThreadState::Running;
    return result;
}

}  // anonymous namespace

// ── Public API ──────────────────────────────────────────────────────────

uint32_t WaitHandleCreate(bool initial_state, WaitHandleType type) noexcept {
    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_handle_table_mutex);

    if (s_handles.size() >= kMaxWaitHandles) {
        return kInvalidWaitHandle;
    }

    uint32_t id = s_next_handle_id++;
    if (id == kInvalidWaitHandle) id = s_next_handle_id++;

    s_handles.emplace_back();
    auto& entry = s_handles.back();
    entry.id = id;
    entry.type = type;
    entry.signalled = initial_state;
    entry.active = true;

    return id;
}

bool WaitHandleClose(uint32_t handle_id) noexcept {
    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_handle_table_mutex);

    auto* entry = FindHandle(handle_id);
    if (entry == nullptr) return false;

    entry->active = false;
    entry->cv.notify_all();  // Wake any waiters (they'll see !active and return error).
    return true;
}

bool WaitHandleSet(uint32_t handle_id) noexcept {
    WaitHandleEntry* entry;
    {
        std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_handle_table_mutex);
        entry = FindHandle(handle_id);
        if (entry == nullptr) return false;
    }

    {
        std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(entry->mutex);
        entry->signalled = true;
    }

    if (entry->type == WaitHandleType::AutoResetEvent) {
        entry->cv.notify_one();  // Wake one waiter.
    } else {
        entry->cv.notify_all();  // Wake all waiters.
    }

    return true;
}

bool WaitHandleReset(uint32_t handle_id) noexcept {
    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_handle_table_mutex);

    auto* entry = FindHandle(handle_id);
    if (entry == nullptr) return false;

    std::lock_guard<CHAOS_IL2CPP_MUTEX> entry_lock(entry->mutex);
    entry->signalled = false;
    return true;
}

int32_t WaitHandleWaitOne(uint32_t handle_id, int32_t timeout_ms) noexcept {
    WaitHandleEntry* entry;
    {
        std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_handle_table_mutex);
        entry = FindHandle(handle_id);
    }

    return WaitOnHandle(entry, timeout_ms);
}

int32_t WaitHandleWaitAny(const uint32_t* handle_ids, uint32_t count, int32_t timeout_ms) noexcept {
    if (handle_ids == nullptr || count == 0) return -1;

    auto deadline = (timeout_ms >= 0)
        ? std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms)
        : std::chrono::steady_clock::time_point::max();

    // Collect entries under the table lock.
    std::vector<WaitHandleEntry*> entries(count, nullptr);
    {
        std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_handle_table_mutex);
        for (uint32_t i = 0; i < count; i++) {
            entries[i] = FindHandle(handle_ids[i]);
        }
    }

    // Poll once first (fast path).
    for (uint32_t i = 0; i < count; i++) {
        if (entries[i] == nullptr) continue;
        std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(entries[i]->mutex);
        if (entries[i]->signalled) {
            if (entries[i]->type == WaitHandleType::AutoResetEvent) {
                entries[i]->signalled = false;
            }
            return static_cast<int32_t>(i);
        }
    }

    if (timeout_ms == 0) return -1;  // Poll: none signalled.

    // Blocking wait: use condition variables for O(1) wakeup instead of polling.
    // When any handle is Set, its CV notify_one/notify_all wakes us immediately.
    GC_TRANSITION_TO_PREEMPTIVE();

    int32_t result = -1;
    while (std::chrono::steady_clock::now() < deadline) {
        // Recheck all handles under their locks.
        for (uint32_t i = 0; i < count; i++) {
            if (entries[i] == nullptr) continue;
            std::unique_lock<CHAOS_IL2CPP_MUTEX> lock(entries[i]->mutex);
            if (entries[i]->signalled) {
                if (entries[i]->type == WaitHandleType::AutoResetEvent) {
                    entries[i]->signalled = false;
                }
                result = static_cast<int32_t>(i);
                GC_TRANSITION_TO_COOPERATIVE();
                return result;
            }
        }

        // No handle signalled — wait on the first valid handle's CV.
        // When any handle is Set, its CV notify wakes us and we recheck all.
        // The predicate rechecks ALL handles to handle the case where a
        // different handle was Set (not the one we're waiting on).
        auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(
            deadline - std::chrono::steady_clock::now());
        if (remaining.count() <= 0) break;

        for (uint32_t i = 0; i < count; i++) {
            if (entries[i] == nullptr) continue;
            std::unique_lock<CHAOS_IL2CPP_MUTEX> lock(entries[i]->mutex);
            entries[i]->cv.wait_for(lock, remaining, [&entries, count]() {
                for (uint32_t j = 0; j < count; j++) {
                    if (entries[j] != nullptr && entries[j]->signalled) return true;
                }
                return false;
            });
            // Recheck after wakeup.
            for (uint32_t j = 0; j < count; j++) {
                if (entries[j] == nullptr) continue;
                if (entries[j]->signalled) {
                    if (entries[j]->type == WaitHandleType::AutoResetEvent) {
                        entries[j]->signalled = false;
                    }
                    result = static_cast<int32_t>(j);
                    GC_TRANSITION_TO_COOPERATIVE();
                    return result;
                }
            }
            break;  // Only wait on one CV per iteration; loop rechecks all.
        }
    }

    GC_TRANSITION_TO_COOPERATIVE();
    return -1;  // Timeout.
}

int32_t WaitHandleWaitAll(const uint32_t* handle_ids, uint32_t count, int32_t timeout_ms) noexcept {
    if (handle_ids == nullptr || count == 0) return -1;

    auto deadline = (timeout_ms >= 0)
        ? std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms)
        : std::chrono::steady_clock::time_point::max();

    // Collect entries.
    std::vector<WaitHandleEntry*> entries(count, nullptr);
    {
        std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_handle_table_mutex);
        for (uint32_t i = 0; i < count; i++) {
            entries[i] = FindHandle(handle_ids[i]);
        }
    }

    // Poll once first.
    bool all_signalled = true;
    for (uint32_t i = 0; i < count; i++) {
        if (entries[i] == nullptr) continue;
        std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(entries[i]->mutex);
        if (!entries[i]->signalled) {
            all_signalled = false;
            break;
        }
    }
    if (all_signalled) return 0;

    if (timeout_ms == 0) return -1;

    // Blocking wait: use CV for O(1) wakeup.
    GC_TRANSITION_TO_PREEMPTIVE();

    while (std::chrono::steady_clock::now() < deadline) {
        // Recheck all handles.
        bool all = true;
        for (uint32_t i = 0; i < count; i++) {
            if (entries[i] == nullptr) continue;
            std::unique_lock<CHAOS_IL2CPP_MUTEX> lock(entries[i]->mutex);
            if (!entries[i]->signalled) {
                all = false;
                break;
            }
        }
        if (all) {
            GC_TRANSITION_TO_COOPERATIVE();
            return 0;
        }

        auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(
            deadline - std::chrono::steady_clock::now());
        if (remaining.count() <= 0) break;

        // Wait on the first non-signalled handle's CV.  When any handle is Set,
        // its CV notify wakes us and we recheck all handles.
        for (uint32_t i = 0; i < count; i++) {
            if (entries[i] == nullptr) continue;
            std::unique_lock<CHAOS_IL2CPP_MUTEX> lock(entries[i]->mutex);
            if (!entries[i]->signalled) {
                entries[i]->cv.wait_for(lock, remaining, [&entries, count]() {
                    for (uint32_t j = 0; j < count; j++) {
                        if (entries[j] != nullptr && !entries[j]->signalled) return false;
                    }
                    return true;  // All signalled.
                });
                break;  // Only wait on one CV per iteration.
            }
        }
    }

    GC_TRANSITION_TO_COOPERATIVE();
    return -1;  // Timeout.
}

}  // namespace chaos::il2cpp::runtime_core::threading