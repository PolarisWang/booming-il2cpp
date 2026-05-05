#include "thread_state.h"

#include <atomic>
#include <new>
#include <cstdlib>
#include <thread>

namespace chaos::il2cpp::runtime_core::threading {

// ── TLS definitions ──────────────────────────────────────────────────

thread_local ManagedThread* tls_this_thread  = nullptr;
thread_local int32_t        tls_this_thread_id = kMainThreadId;

namespace {

/// Lock-free singly-linked list of all registered ManagedThread entries.
/// Head is updated atomically on register; entries are never removed
/// (marked !is_running instead) to avoid ABA during concurrent iteration.
std::atomic<ManagedThread*> s_thread_list{nullptr};

/// Monotonically increasing thread ID allocator.
std::atomic<int32_t> s_next_thread_id{kMainThreadId + 1};

}  // anonymous namespace

void RegisterThread(int32_t managed_id, void* managed_obj) noexcept {
    auto* thread = new ManagedThread();
    thread->managed_id     = managed_id;
    thread->managed_object = managed_obj;
    thread->is_running     = true;

    // Publish to TLS for O(1) self-lookup.
    tls_this_thread    = thread;
    tls_this_thread_id = managed_id;

    // Lock-free insert at head of the global thread list.
    ManagedThread* expected = s_thread_list.load(std::memory_order_acquire);
    do {
        thread->next.store(expected, std::memory_order_relaxed);
    } while (!s_thread_list.compare_exchange_weak(expected, thread,
        std::memory_order_release, std::memory_order_acquire));
}

void UnregisterThread() noexcept {
    auto* thread = tls_this_thread;
    if (thread == nullptr) return;

    thread->is_running = false;
    // TLS still points to the entry so EnumerateThreads callbacks can
    // safely query the current thread.  The entry is leaked intentionally
    // (lives for process lifetime like most runtime-instantiated metadata).
    tls_this_thread    = nullptr;
    tls_this_thread_id = 0;
}

int32_t AllocateThreadId() noexcept {
    return s_next_thread_id.fetch_add(1, std::memory_order_relaxed);
}

void EnumerateThreads(bool (*callback)(ManagedThread*)) noexcept {
    for (auto* entry = s_thread_list.load(std::memory_order_acquire);
         entry != nullptr;
         entry = entry->next.load(std::memory_order_acquire)) {
        if (entry->is_running) {
            if (!callback(entry)) break;
        }
    }
}

int32_t GetThreadCount() noexcept {
    int32_t count = 0;
    for (auto* entry = s_thread_list.load(std::memory_order_acquire);
         entry != nullptr;
         entry = entry->next.load(std::memory_order_acquire)) {
        if (entry->is_running) ++count;
    }
    return count;
}

// ── Generation-based GC safepoint ───────────────────────────────────
//
// Even generation = idle (no GC activity).  Odd generation = GC in progress.
// Threads check via single atomic load + compare on every SafepointPoll.

namespace {
std::atomic<uint32_t> s_generation{0};
constexpr uint32_t kGcGenerationMask = 1u;
}  // anonymous namespace

bool SafepointRequested() noexcept {
    uint32_t gen = s_generation.load(std::memory_order_acquire);
    return (gen & kGcGenerationMask) != 0u;
}

void SafepointPoll() noexcept {
    uint32_t gen = s_generation.load(std::memory_order_acquire);
    if ((gen & kGcGenerationMask) == 0u) {
        return;  // fast path: no GC pending, single load + branch
    }

    auto* thread = tls_this_thread;
    if (thread == nullptr) return;

    thread->at_safepoint = true;
    thread->last_seen_gen = gen;

    // Spin with yield until generation flips (even = released).
    // Timeout: after ~10ms without confirmation, GC proceeds with
    // conservative stack scanning anyway (bdwgc fallback).
    int spins = 0;
    while ((s_generation.load(std::memory_order_acquire) & kGcGenerationMask) != 0u) {
        if (++spins > 10000) {
            std::this_thread::yield();
            spins = 0;
        }
    }

    thread->at_safepoint = false;
}

uint32_t RequestGlobalSafepoint() noexcept {
    // Toggle to odd (GC in progress).
    uint32_t gen = s_generation.load(std::memory_order_acquire);
    uint32_t desired = (gen + 1) | kGcGenerationMask;
    s_generation.store(desired, std::memory_order_release);
    return desired;
}

void ReleaseGlobalSafepoint(uint32_t /*generation*/) noexcept {
    // Toggle to even (released).
    uint32_t gen = s_generation.load(std::memory_order_acquire);
    s_generation.store((gen + 1) & ~kGcGenerationMask, std::memory_order_release);
}}  // namespace chaos::il2cpp::runtime_core::threading
