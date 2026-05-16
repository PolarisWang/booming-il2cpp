#include "wait_handle.h"
#include "thread_state.h"
#include "gc_transition.h"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <memory>
#include <shared_mutex>
#include <unordered_map>

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
// O(1) lookup via hash map — replaces O(n) linear scan of std::list.
// Entries are never removed from the map (only marked active=false), so
// pointer stability is guaranteed after lookup.
// shared_mutex allows concurrent FindHandle lookups while Create/Close
// get exclusive access.

namespace {

std::shared_mutex s_handle_table_mutex;
std::unordered_map<uint32_t, std::unique_ptr<WaitHandleEntry>> s_handles;
uint32_t s_next_handle_id = 1;

// NOTE: Caller must hold at least a shared_lock on s_handle_table_mutex
// when calling FindHandle. The returned pointer is stable because entries
// are never freed from the map — only marked active=false under exclusive
// lock in Close.
WaitHandleEntry* FindHandle(uint32_t id) noexcept {
    auto it = s_handles.find(id);
    if (it != s_handles.end() && it->second->active) return it->second.get();
    return nullptr;
}

int32_t WaitOnHandle(WaitHandleEntry* entry, int32_t timeout_ms) noexcept {
    if (entry == nullptr) return -1;

    auto* thread = GetCurrentThread();
    if (thread) thread->managed_state = ManagedThreadState::WaitSleepJoin;

    GC_TRANSITION_TO_PREEMPTIVE();

    std::unique_lock<CHAOS_IL2CPP_MUTEX> lock(entry->mutex);

    if (entry->signalled) {
        if (entry->type == WaitHandleType::AutoResetEvent) {
            entry->signalled = false;
        }
        GC_TRANSITION_TO_COOPERATIVE();
        if (thread) thread->managed_state = ManagedThreadState::Running;
        return 1;
    }

    if (timeout_ms == 0) {
        GC_TRANSITION_TO_COOPERATIVE();
        if (thread) thread->managed_state = ManagedThreadState::Running;
        return 0;
    }

    int32_t result;
    if (timeout_ms < 0) {
        entry->cv.wait(lock, [entry] { return entry->signalled; });
        result = 1;
    } else {
        bool signalled = entry->cv.wait_for(lock, std::chrono::milliseconds(timeout_ms),
            [entry] { return entry->signalled; });
        result = signalled ? 1 : 0;
    }

    if (result == 1 && entry->type == WaitHandleType::AutoResetEvent) {
        entry->signalled = false;
    }

    GC_TRANSITION_TO_COOPERATIVE();
    if (thread) thread->managed_state = ManagedThreadState::Running;
    return result;
}

}  // anonymous namespace

// ── Public API ──────────────────────────────────────────────────────────

uint32_t WaitHandleCreate(bool initial_state, WaitHandleType type) noexcept {
    std::unique_lock<std::shared_mutex> lock(s_handle_table_mutex);

    if (s_handles.size() >= kMaxWaitHandles) {
        return kInvalidWaitHandle;
    }

    uint32_t id = s_next_handle_id++;
    if (id == kInvalidWaitHandle) id = s_next_handle_id++;

    auto entry = std::make_unique<WaitHandleEntry>();
    entry->id = id;
    entry->type = type;
    entry->signalled = initial_state;
    entry->active = true;

    s_handles[id] = std::move(entry);
    return id;
}

bool WaitHandleClose(uint32_t handle_id) noexcept {
    std::unique_lock<std::shared_mutex> lock(s_handle_table_mutex);

    auto* entry = FindHandle(handle_id);
    if (entry == nullptr) return false;

    entry->active = false;
    entry->cv.notify_all();
    // Note: entry pointer remains valid in the map (unique_ptr not released).
    return true;
}

bool WaitHandleSet(uint32_t handle_id) noexcept {
    WaitHandleEntry* entry;
    {
        std::shared_lock<std::shared_mutex> lock(s_handle_table_mutex);
        entry = FindHandle(handle_id);
    }
    if (entry == nullptr) return false;

    {
        std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(entry->mutex);
        entry->signalled = true;
    }

    if (entry->type == WaitHandleType::AutoResetEvent) {
        entry->cv.notify_one();
    } else {
        entry->cv.notify_all();
    }

    return true;
}

bool WaitHandleReset(uint32_t handle_id) noexcept {
    WaitHandleEntry* entry;
    {
        std::shared_lock<std::shared_mutex> lock(s_handle_table_mutex);
        entry = FindHandle(handle_id);
    }
    if (entry == nullptr) return false;

    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(entry->mutex);
    entry->signalled = false;
    return true;
}

int32_t WaitHandleWaitOne(uint32_t handle_id, int32_t timeout_ms) noexcept {
    WaitHandleEntry* entry;
    {
        std::shared_lock<std::shared_mutex> lock(s_handle_table_mutex);
        entry = FindHandle(handle_id);
    }
    return WaitOnHandle(entry, timeout_ms);
}

int32_t WaitHandleWaitAny(const uint32_t* handle_ids, uint32_t count, int32_t timeout_ms) noexcept {
    if (handle_ids == nullptr || count == 0) return -1;

    auto deadline = (timeout_ms >= 0)
        ? std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms)
        : std::chrono::steady_clock::time_point::max();

    // O(count) O(1) lookups under shared lock.
    std::vector<WaitHandleEntry*> entries(count, nullptr);
    {
        std::shared_lock<std::shared_mutex> lock(s_handle_table_mutex);
        for (uint32_t i = 0; i < count; i++) {
            auto it = s_handles.find(handle_ids[i]);
            if (it != s_handles.end() && it->second->active) {
                entries[i] = it->second.get();
            }
        }
    }

    // Poll once first.
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

    if (timeout_ms == 0) return -1;

    GC_TRANSITION_TO_PREEMPTIVE();

    int32_t result = -1;
    while (std::chrono::steady_clock::now() < deadline) {
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
            break;
        }
    }

    GC_TRANSITION_TO_COOPERATIVE();
    return -1;
}

int32_t WaitHandleWaitAll(const uint32_t* handle_ids, uint32_t count, int32_t timeout_ms) noexcept {
    if (handle_ids == nullptr || count == 0) return -1;

    auto deadline = (timeout_ms >= 0)
        ? std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms)
        : std::chrono::steady_clock::time_point::max();

    // O(count) O(1) lookups under shared lock.
    std::vector<WaitHandleEntry*> entries(count, nullptr);
    {
        std::shared_lock<std::shared_mutex> lock(s_handle_table_mutex);
        for (uint32_t i = 0; i < count; i++) {
            auto it = s_handles.find(handle_ids[i]);
            if (it != s_handles.end() && it->second->active) {
                entries[i] = it->second.get();
            }
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

    GC_TRANSITION_TO_PREEMPTIVE();

    while (std::chrono::steady_clock::now() < deadline) {
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

        for (uint32_t i = 0; i < count; i++) {
            if (entries[i] == nullptr) continue;
            std::unique_lock<CHAOS_IL2CPP_MUTEX> lock(entries[i]->mutex);
            if (!entries[i]->signalled) {
                entries[i]->cv.wait_for(lock, remaining, [&entries, count]() {
                    for (uint32_t j = 0; j < count; j++) {
                        if (entries[j] != nullptr && !entries[j]->signalled) return false;
                    }
                    return true;
                });
                break;
            }
        }
    }

    GC_TRANSITION_TO_COOPERATIVE();
    return -1;
}

}  // namespace chaos::il2cpp::runtime_core::threading
