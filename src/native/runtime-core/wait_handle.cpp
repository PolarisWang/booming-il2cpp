#include "wait_handle.h"
#include "thread_state.h"
#include "gc_transition.h"

#include <chaos/pal/pal_sync.h>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <memory>
#include <shared_mutex>
#include <unordered_map>
#include <vector>

namespace chaos::il2cpp::runtime_core::threading {

using chaos::il2cpp::pal::PalEvent;
using chaos::il2cpp::pal::PalEventCreate;
using chaos::il2cpp::pal::PalEventDestroy;
using chaos::il2cpp::pal::PalEventSet;
using chaos::il2cpp::pal::PalEventReset;
using chaos::il2cpp::pal::PalEventWait;
using chaos::il2cpp::pal::PalEventWaitAny;
using chaos::il2cpp::pal::PalEventWaitAll;

// ── Internal wait handle entry ──────────────────────────────────────────

struct WaitHandleEntry {
    uint32_t id;
    bool active;
    PalEvent* event;
};

// ── Global state ────────────────────────────────────────────────────────

namespace {

std::shared_mutex s_handle_table_mutex;
std::unordered_map<uint32_t, std::unique_ptr<WaitHandleEntry>> s_handles;
uint32_t s_next_handle_id = 1;

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

    uint64_t pal_timeout = (timeout_ms < 0) ? UINT64_MAX : static_cast<uint64_t>(timeout_ms);
    bool ok = PalEventWait(entry->event, pal_timeout);

    GC_TRANSITION_TO_COOPERATIVE();
    if (thread) thread->managed_state = ManagedThreadState::Running;
    return ok ? 1 : 0;
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
    entry->active = true;
    entry->event = PalEventCreate(initial_state, type == WaitHandleType::ManualResetEvent);
    if (entry->event == nullptr) {
        return kInvalidWaitHandle;
    }

    s_handles[id] = std::move(entry);
    return id;
}

bool WaitHandleClose(uint32_t handle_id) noexcept {
    std::unique_lock<std::shared_mutex> lock(s_handle_table_mutex);

    auto* entry = FindHandle(handle_id);
    if (entry == nullptr) return false;

    entry->active = false;
    if (entry->event) {
        PalEventDestroy(entry->event);
        entry->event = nullptr;
    }
    return true;
}

bool WaitHandleSet(uint32_t handle_id) noexcept {
    WaitHandleEntry* entry;
    {
        std::shared_lock<std::shared_mutex> lock(s_handle_table_mutex);
        entry = FindHandle(handle_id);
    }
    if (entry == nullptr || entry->event == nullptr) return false;

    PalEventSet(entry->event);
    return true;
}

bool WaitHandleReset(uint32_t handle_id) noexcept {
    WaitHandleEntry* entry;
    {
        std::shared_lock<std::shared_mutex> lock(s_handle_table_mutex);
        entry = FindHandle(handle_id);
    }
    if (entry == nullptr || entry->event == nullptr) return false;

    PalEventReset(entry->event);
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

    // Gather event pointers under shared lock.
    std::vector<PalEvent*> events(count, nullptr);
    {
        std::shared_lock<std::shared_mutex> lock(s_handle_table_mutex);
        for (uint32_t i = 0; i < count; i++) {
            auto it = s_handles.find(handle_ids[i]);
            if (it != s_handles.end() && it->second->active && it->second->event != nullptr) {
                events[i] = it->second->event;
            } else {
                return -1;
            }
        }
    }

    auto* thread = GetCurrentThread();
    if (thread) thread->managed_state = ManagedThreadState::WaitSleepJoin;

    GC_TRANSITION_TO_PREEMPTIVE();

    uint64_t pal_timeout = (timeout_ms < 0) ? UINT64_MAX : static_cast<uint64_t>(timeout_ms);
    int32_t result = PalEventWaitAny(events.data(), count, pal_timeout);

    GC_TRANSITION_TO_COOPERATIVE();
    if (thread) thread->managed_state = ManagedThreadState::Running;
    return result;
}

int32_t WaitHandleWaitAll(const uint32_t* handle_ids, uint32_t count, int32_t timeout_ms) noexcept {
    if (handle_ids == nullptr || count == 0) return -1;

    // Gather event pointers under shared lock.
    std::vector<PalEvent*> events(count, nullptr);
    {
        std::shared_lock<std::shared_mutex> lock(s_handle_table_mutex);
        for (uint32_t i = 0; i < count; i++) {
            auto it = s_handles.find(handle_ids[i]);
            if (it != s_handles.end() && it->second->active && it->second->event != nullptr) {
                events[i] = it->second->event;
            } else {
                return -1;
            }
        }
    }

    auto* thread = GetCurrentThread();
    if (thread) thread->managed_state = ManagedThreadState::WaitSleepJoin;

    GC_TRANSITION_TO_PREEMPTIVE();

    uint64_t pal_timeout = (timeout_ms < 0) ? UINT64_MAX : static_cast<uint64_t>(timeout_ms);
    int32_t result = PalEventWaitAll(events.data(), count, pal_timeout);

    GC_TRANSITION_TO_COOPERATIVE();
    if (thread) thread->managed_state = ManagedThreadState::Running;
    return result;
}

}  // namespace chaos::il2cpp::runtime_core::threading
