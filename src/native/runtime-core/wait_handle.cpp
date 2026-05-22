#include "wait_handle.h"
#include "thread_state.h"
#include "gc_transition.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <memory>
#include <shared_mutex>
#include <unordered_map>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

namespace chaos::il2cpp::runtime_core::threading {

// ── Per-handle waiter node (non-Windows signal-based dispatch) ──────────
// Used by WaitHandleWaitAny to register on multiple handles' waiter lists.
// A stack-allocated WaiterNode is created per WaitAny call and pushed onto
// each handle's waiter_list.  WaitHandleSet walks the list and notifies
// directly, eliminating the polling loop.
struct WaiterNode {
    std::condition_variable* cv;     // waiter's CV (stack-allocated in WaitAny)
    std::mutex* mtx;                 // waiter's mutex (for CV predicate)
    std::atomic<bool>* wake_flag;    // waiter's wake flag (set by Set)
    uint32_t handle_index;           // index in the waiter's handle array
    WaiterNode* next;
};

// ── Internal wait handle entry ──────────────────────────────────────────

struct WaitHandleEntry {
    uint32_t id;
    WaitHandleType type;
    bool active;
    CHAOS_IL2CPP_MUTEX mutex;
    CHAOS_IL2CPP_CONDITION_VARIABLE cv;
#ifdef _WIN32
    void* os_event{nullptr};  // HANDLE (CreateEvent)
#else
    bool signalled;
    WaiterNode* waiter_list{nullptr};  // Treiber stack of WaitAny waiters
#endif
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

#ifdef _WIN32
    DWORD ms = (timeout_ms < 0) ? INFINITE : static_cast<DWORD>(timeout_ms);
    DWORD ret = WaitForSingleObject(static_cast<HANDLE>(entry->os_event), ms);
    GC_TRANSITION_TO_COOPERATIVE();
    if (thread) thread->managed_state = ManagedThreadState::Running;
    return (ret == WAIT_OBJECT_0) ? 1 : (ret == WAIT_TIMEOUT) ? 0 : -1;
#else
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
#endif
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
    entry->active = true;

#ifdef _WIN32
    bool manual_reset = (type == WaitHandleType::ManualResetEvent);
    entry->os_event = CreateEventA(nullptr, manual_reset ? TRUE : FALSE,
                                   initial_state ? TRUE : FALSE, nullptr);
    if (entry->os_event == nullptr) {
        return kInvalidWaitHandle;
    }
#else
    entry->signalled = initial_state;
#endif

    s_handles[id] = std::move(entry);
    return id;
}

bool WaitHandleClose(uint32_t handle_id) noexcept {
    std::unique_lock<std::shared_mutex> lock(s_handle_table_mutex);

    auto* entry = FindHandle(handle_id);
    if (entry == nullptr) return false;

    entry->active = false;
#ifdef _WIN32
    if (entry->os_event != nullptr) {
        CloseHandle(static_cast<HANDLE>(entry->os_event));
        entry->os_event = nullptr;
    }
#else
    // Wake all registered WaitAny waiters on this handle.
    {
        std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(entry->mutex);
        WaiterNode* w = entry->waiter_list;
        while (w) {
            {
                std::lock_guard<std::mutex> wlock(*w->mtx);
                w->wake_flag->store(true, std::memory_order_relaxed);
            }
            w->cv->notify_one();
            w = w->next;
        }
        entry->waiter_list = nullptr;
    }
    entry->cv.notify_all();
#endif
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

#ifdef _WIN32
    SetEvent(static_cast<HANDLE>(entry->os_event));
#else
    {
        std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(entry->mutex);
        entry->signalled = true;

        // Walk the waiter list and notify registered WaitAny waiters.
        if (entry->type == WaitHandleType::AutoResetEvent) {
            // AutoResetEvent: wake only the first waiter.
            if (entry->waiter_list) {
                WaiterNode* w = entry->waiter_list;
                {
                    std::lock_guard<std::mutex> wlock(*w->mtx);
                    w->wake_flag->store(true, std::memory_order_relaxed);
                }
                w->cv->notify_one();
            }
        } else {
            // ManualResetEvent: wake all registered waiters.
            WaiterNode* w = entry->waiter_list;
            while (w) {
                {
                    std::lock_guard<std::mutex> wlock(*w->mtx);
                    w->wake_flag->store(true, std::memory_order_relaxed);
                }
                w->cv->notify_one();
                w = w->next;
            }
        }
    }

    // Also notify direct WaitOne waiters.
    if (entry->type == WaitHandleType::AutoResetEvent) {
        entry->cv.notify_one();
    } else {
        entry->cv.notify_all();
    }
#endif

    return true;
}

bool WaitHandleReset(uint32_t handle_id) noexcept {
    WaitHandleEntry* entry;
    {
        std::shared_lock<std::shared_mutex> lock(s_handle_table_mutex);
        entry = FindHandle(handle_id);
    }
    if (entry == nullptr) return false;

#ifdef _WIN32
    ResetEvent(static_cast<HANDLE>(entry->os_event));
#else
    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(entry->mutex);
    entry->signalled = false;
#endif
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

#ifdef _WIN32
    // Gather native event handles (O(count) map lookups under shared lock).
    std::vector<HANDLE> events;
    events.reserve(count);
    {
        std::shared_lock<std::shared_mutex> lock(s_handle_table_mutex);
        for (uint32_t i = 0; i < count; i++) {
            auto it = s_handles.find(handle_ids[i]);
            if (it != s_handles.end() && it->second->active && it->second->os_event != nullptr) {
                events.push_back(static_cast<HANDLE>(it->second->os_event));
            } else {
                return -1;
            }
        }
    }

    GC_TRANSITION_TO_PREEMPTIVE();
    DWORD ms = (timeout_ms < 0) ? INFINITE : static_cast<DWORD>(timeout_ms);
    DWORD ret = WaitForMultipleObjects(static_cast<DWORD>(events.size()),
                                        events.data(), FALSE, ms);
    GC_TRANSITION_TO_COOPERATIVE();

    if (ret >= WAIT_OBJECT_0 && ret < WAIT_OBJECT_0 + events.size()) {
        return static_cast<int32_t>(ret - WAIT_OBJECT_0);
    }
    return -1;
#else
    // ── Non-Windows: signal-based dispatch ────────────────────────────
    // Per-handle waiter registration eliminates the polling loop.
    // WaitAny registers on each handle's Treiber stack; WaitHandleSet
    // walks the list and notifies directly.

    // 1. Lookup handles under shared lock.
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

    // 2. Quick poll: check each handle once.
    for (uint32_t i = 0; i < count; i++) {
        if (entries[i] == nullptr) continue;
        std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(entries[i]->mutex);
        if (entries[i]->signalled) {
            if (entries[i]->type == WaitHandleType::AutoResetEvent)
                entries[i]->signalled = false;
            return static_cast<int32_t>(i);
        }
    }

    if (timeout_ms == 0) return -1;

    // 3. Per-waiter CV + mutex + flag (stack-allocated in this frame).
    std::mutex waiter_mtx;
    std::condition_variable waiter_cv;
    std::atomic<bool> wake_flag{false};

    // 4. Register a WaiterNode on each handle's waiter list.
    auto nodes = std::make_unique<WaiterNode[]>(count);
    int registered = 0;
    for (uint32_t i = 0; i < count; i++) {
        if (entries[i] == nullptr) continue;
        WaiterNode* n = &nodes[registered];
        n->cv = &waiter_cv;
        n->mtx = &waiter_mtx;
        n->wake_flag = &wake_flag;
        n->handle_index = i;

        std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(entries[i]->mutex);
        n->next = entries[i]->waiter_list;
        entries[i]->waiter_list = n;
        registered++;
    }

    // 5. Re-check all handles after registration (covers the race where
    //    Set fired between our quick-poll and list registration).
    int already_signalled_idx = -1;
    for (int ri = 0; ri < registered; ri++) {
        uint32_t i = nodes[ri].handle_index;
        if (entries[i] == nullptr) continue;
        std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(entries[i]->mutex);
        if (entries[i]->signalled) {
            already_signalled_idx = static_cast<int32_t>(i);
            break;
        }
    }

    if (already_signalled_idx >= 0) {
        // Unregister and return without waiting.
        for (int ri = 0; ri < registered; ri++) {
            uint32_t i = nodes[ri].handle_index;
            if (entries[i] == nullptr) continue;
            std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(entries[i]->mutex);
            WaiterNode** pp = &entries[i]->waiter_list;
            while (*pp) {
                if (*pp == &nodes[ri]) {
                    *pp = nodes[ri].next;
                    break;
                }
                pp = &(*pp)->next;
            }
        }
        if (entries[already_signalled_idx]->type == WaitHandleType::AutoResetEvent) {
            std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(entries[already_signalled_idx]->mutex);
            entries[already_signalled_idx]->signalled = false;
        }
        return already_signalled_idx;
    }

    // 6. If no valid handles, skip blocking and return timeout.
    if (registered == 0) return -1;

    // 7. Block on waiter CV.
    GC_TRANSITION_TO_PREEMPTIVE();

    bool woken = false;
    {
        std::unique_lock<std::mutex> lock(waiter_mtx);
        if (timeout_ms < 0) {
            waiter_cv.wait(lock, [&wake_flag]() noexcept {
                return wake_flag.load(std::memory_order_relaxed);
            });
            woken = true;
        } else {
            woken = waiter_cv.wait_for(lock, std::chrono::milliseconds(timeout_ms),
                [&wake_flag]() noexcept {
                    return wake_flag.load(std::memory_order_relaxed);
                });
        }
    }

    // 8. Unregister from all handles.
    for (int ri = 0; ri < registered; ri++) {
        uint32_t i = nodes[ri].handle_index;
        if (entries[i] == nullptr) continue;
        std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(entries[i]->mutex);
        WaiterNode** pp = &entries[i]->waiter_list;
        while (*pp) {
            if (*pp == &nodes[ri]) {
                *pp = nodes[ri].next;
                break;
            }
            pp = &(*pp)->next;
        }
    }

    GC_TRANSITION_TO_COOPERATIVE();

    if (woken) {
        // Find which handle signalled.
        for (uint32_t i = 0; i < count; i++) {
            if (entries[i] == nullptr) continue;
            std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(entries[i]->mutex);
            if (entries[i]->signalled) {
                if (entries[i]->type == WaitHandleType::AutoResetEvent)
                    entries[i]->signalled = false;
                return static_cast<int32_t>(i);
            }
        }
    }

    return -1;
#endif
}

int32_t WaitHandleWaitAll(const uint32_t* handle_ids, uint32_t count, int32_t timeout_ms) noexcept {
    if (handle_ids == nullptr || count == 0) return -1;

#ifdef _WIN32
    // Gather native event handles.
    std::vector<HANDLE> events;
    events.reserve(count);
    {
        std::shared_lock<std::shared_mutex> lock(s_handle_table_mutex);
        for (uint32_t i = 0; i < count; i++) {
            auto it = s_handles.find(handle_ids[i]);
            if (it != s_handles.end() && it->second->active && it->second->os_event != nullptr) {
                events.push_back(static_cast<HANDLE>(it->second->os_event));
            } else {
                return -1;
            }
        }
    }

    GC_TRANSITION_TO_PREEMPTIVE();
    DWORD ms = (timeout_ms < 0) ? INFINITE : static_cast<DWORD>(timeout_ms);
    DWORD ret = WaitForMultipleObjects(static_cast<DWORD>(events.size()),
                                        events.data(), TRUE, ms);
    GC_TRANSITION_TO_COOPERATIVE();

    return (ret == WAIT_OBJECT_0) ? 0 : -1;
#else
    // Polling implementation for non-Windows: lock handles in sorted order
    // (by handle_id to avoid deadlock), check all are signalled.
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

    // Sort by pointer to establish consistent lock ordering.
    // Build index array sorted by entry pointer.
    std::vector<uint32_t> sorted_idx(count);
    for (uint32_t i = 0; i < count; i++) sorted_idx[i] = i;
    std::sort(sorted_idx.begin(), sorted_idx.end(),
        [&entries](uint32_t a, uint32_t b) {
            return entries[a] < entries[b];
        });

    // Poll once first.
    {
        bool all_set = true;
        for (uint32_t i = 0; i < count; i++) {
            if (entries[i] == nullptr) { all_set = false; break; }
            std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(entries[i]->mutex);
            if (!entries[i]->signalled) { all_set = false; break; }
        }
        if (all_set) {
            // Consume AutoResetEvent signals.
            for (uint32_t i = 0; i < count; i++) {
                if (entries[i]->type == WaitHandleType::AutoResetEvent) {
                    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(entries[i]->mutex);
                    entries[i]->signalled = false;
                }
            }
            return 0;
        }
    }

    if (timeout_ms == 0) return -1;

    GC_TRANSITION_TO_PREEMPTIVE();

    while (std::chrono::steady_clock::now() < deadline) {
        // Lock all in sorted order and check.
        bool all_set = true;
        for (uint32_t si = 0; si < count; si++) {
            uint32_t i = sorted_idx[si];
            if (entries[i] == nullptr) { all_set = false; break; }
            std::unique_lock<CHAOS_IL2CPP_MUTEX> lock(entries[i]->mutex);
            if (!entries[i]->signalled) {
                all_set = false;
                break;
            }
        }
        if (all_set) {
            // Consume AutoResetEvent signals.
            for (uint32_t si = 0; si < count; si++) {
                uint32_t i = sorted_idx[si];
                if (entries[i] != nullptr && entries[i]->type == WaitHandleType::AutoResetEvent) {
                    std::unique_lock<CHAOS_IL2CPP_MUTEX> lock(entries[i]->mutex);
                    entries[i]->signalled = false;
                }
            }
            GC_TRANSITION_TO_COOPERATIVE();
            return 0;
        }

        // Wait on first handle's CV with remaining time.
        auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(
            deadline - std::chrono::steady_clock::now());
        if (remaining.count() <= 0) break;

        if (entries[sorted_idx[0]] != nullptr) {
            std::unique_lock<CHAOS_IL2CPP_MUTEX> lock(entries[sorted_idx[0]]->mutex);
            entries[sorted_idx[0]]->cv.wait_for(lock, remaining);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    GC_TRANSITION_TO_COOPERATIVE();
    return -1;
#endif
}

}  // namespace chaos::il2cpp::runtime_core::threading
