// cancellation_token.cpp — CancellationTokenSource + CancellationTokenRegistration
//
// Design:
//   - Table-driven: sources and registrations stored in fixed-size arrays.
//   - Timer-based cancellation via TimerQueueCreate (integrated with gate thread).
//   - Callbacks fired synchronously on Cancel() — caller should not hold locks.
//   - Thread-safe: all operations protected by a single mutex.

#include "cancellation_token.h"
#include "timer_queue.h"

#include <chaos/log.h>

#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <list>
#include <mutex>
#include <vector>

namespace chaos::il2cpp::runtime_core::threading {

// ── Constants ─────────────────────────────────────────────────────────

constexpr uint32_t kMaxCancellationSources = 256;
constexpr uint32_t kMaxCancellationRegistrations = 1024;

// ── Global state ──────────────────────────────────────────────────────

namespace {

CHAOS_IL2CPP_MUTEX s_mutex;

struct CancellationSourceEntry {
    uint32_t id{0};
    std::atomic<bool> cancelled{false};
    uint32_t timer_id{0};
    std::atomic<bool> active{true};
};

struct CancellationRegistrationEntry {
    uint32_t id{0};
    uint32_t source_id{0};
    void (*callback)(void*){nullptr};
    void* state{nullptr};
    bool active{true};
};

std::list<CancellationSourceEntry> s_sources;
std::list<CancellationRegistrationEntry> s_registrations;
uint32_t s_next_source_id = 1;
uint32_t s_next_reg_id = 1;

CancellationSourceEntry* FindSource(uint32_t id) noexcept {
    for (auto& src : s_sources) {
        if (src.id == id && src.active.load(std::memory_order_relaxed)) return &src;
    }
    return nullptr;
}

/// Timer callback: cancel the source when the timer fires.
void TimerCancelCallback(void* state) noexcept {
    uint32_t source_id = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(state));
    CancellationTokenSourceCancel(source_id);
}

/// Helper: create a source entry, add to list, return pointer.
CancellationSourceEntry* AddSource(uint32_t id) noexcept {
    s_sources.emplace_back();
    auto& entry = s_sources.back();
    entry.id = id;
    entry.cancelled.store(false, std::memory_order_relaxed);
    entry.timer_id = 0;
    entry.active.store(true, std::memory_order_relaxed);
    return &entry;
}

/// Helper: create a registration entry, add to list, return pointer.
CancellationRegistrationEntry* AddRegistration(uint32_t id, uint32_t source_id,
                                                void (*callback)(void*), void* state) noexcept {
    s_registrations.emplace_back();
    auto& reg = s_registrations.back();
    reg.id = id;
    reg.source_id = source_id;
    reg.callback = callback;
    reg.state = state;
    reg.active = true;
    return &reg;
}

}  // anonymous namespace

// ── Public API ────────────────────────────────────────────────────────

uint32_t CancellationTokenSourceCreate() noexcept {
    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_mutex);

    uint32_t id = s_next_source_id++;
    if (id == 0) id = s_next_source_id++;

    AddSource(id);

    return id;
}

uint32_t CancellationTokenSourceCreateWithTimer(uint32_t due_time_ms) noexcept {
    uint32_t id = CancellationTokenSourceCreate();
    if (id == 0) return 0;

    // Create a one-shot timer to cancel this source.
    uint32_t timer_id = TimerQueueCreate(
        TimerCancelCallback,
        reinterpret_cast<void*>(static_cast<uintptr_t>(id)),
        due_time_ms,
        0);  // period_ms = 0 → one-shot

    if (timer_id != 0) {
        std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_mutex);
        auto* entry = FindSource(id);
        if (entry != nullptr) {
            entry->timer_id = timer_id;
        }
    }

    return id;
}

bool CancellationTokenSourceCancel(uint32_t source_id) noexcept {
    // Collect registrations under lock, fire outside lock.
    struct FireEntry {
        void (*callback)(void*);
        void* state;
    };
    std::vector<FireEntry> to_fire;

    {
        std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_mutex);

        auto* entry = FindSource(source_id);
        if (entry == nullptr) return false;

        // First cancel only.
        bool expected = false;
        if (!entry->cancelled.compare_exchange_strong(expected, true, std::memory_order_release, std::memory_order_relaxed)) {
            return false;  // Already cancelled.
        }

        // Collect active registrations for this source.
        for (auto& reg : s_registrations) {
            if (reg.active && reg.source_id == source_id) {
                to_fire.push_back({reg.callback, reg.state});
            }
        }
    }

    // Fire callbacks outside the lock.
    for (auto& fe : to_fire) {
        if (fe.callback) {
            fe.callback(fe.state);
        }
    }

    return true;
}

bool CancellationTokenSourceIsCancelled(uint32_t source_id) noexcept {
    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_mutex);
    auto* entry = FindSource(source_id);
    if (entry == nullptr) return false;
    return entry->cancelled.load(std::memory_order_acquire);
}

void CancellationTokenSourceDispose(uint32_t source_id) noexcept {
    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_mutex);

    auto* entry = FindSource(source_id);
    if (entry == nullptr) return;

    // Cancel the timer if one exists.
    if (entry->timer_id != 0) {
        TimerQueueDelete(entry->timer_id);
        entry->timer_id = 0;
    }

    entry->active.store(false, std::memory_order_relaxed);
}

uint32_t CancellationTokenRegister(uint32_t source_id, void (*callback)(void*), void* state) noexcept {
    if (callback == nullptr) return 0;

    std::unique_lock<CHAOS_IL2CPP_MUTEX> lock(s_mutex);

    auto* entry = FindSource(source_id);
    if (entry == nullptr) return 0;

    // If already cancelled, fire immediately.
    if (entry->cancelled.load(std::memory_order_acquire)) {
        lock.unlock();
        callback(state);
        return 0;  // Registration not stored (already fired).
    }

    uint32_t id = s_next_reg_id++;
    if (id == 0) id = s_next_reg_id++;

    AddRegistration(id, source_id, callback, state);

    return id;
}

bool CancellationTokenUnregister(uint32_t registration_id) noexcept {
    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_mutex);

    for (auto& reg : s_registrations) {
        if (reg.id == registration_id && reg.active) {
            reg.active = false;
            return true;
        }
    }
    return false;
}

}  // namespace chaos::il2cpp::runtime_core::threading