#include "thread_state.h"

#include <chaos/profile.h>

#include "gc_region.h"
#include "gc_root_scanner.h"

#include <atomic>
#include <new>
#include <cstdlib>
#include <thread>

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    #include <intrin.h>
#else
    #include <pthread.h>
#endif

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

    // Capture stack bounds for conservative root scanning during full GC.
    // Using stack-based address heuristics: the address of a local variable
    // tells us the current stack pointer, and we estimate the base from
    // platform APIs.  On Windows, we use the NT_TIB.
#if defined(_WIN32) || defined(_WIN64)
    NT_TIB* tib = (NT_TIB*)__readgsqword(0x30);
    thread->stack_base   = tib->StackBase;
    thread->stack_limit  = tib->StackLimit;
#else
    // POSIX: use pthread_getattr_np / pthread_attr_getstack.
    pthread_attr_t attr;
    void* stack_addr;
    size_t stack_size;
    pthread_getattr_np(pthread_self(), &attr);
    pthread_attr_getstack(&attr, &stack_addr, &stack_size);
    pthread_attr_destroy(&attr);
    thread->stack_base   = static_cast<char*>(stack_addr) + stack_size;
    thread->stack_limit  = stack_addr;
#endif

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

    // Return the TLS nursery to the region manager before clearing TLS.
    // Otherwise the nursery region leaks until process exit.
    TeardownTlsNursery();

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
    CHAOS_IL2CPP_PROFILE_SCOPE("SafepointPoll");
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
}

void GcScanAllThreadRoots(void (*callback)(void* root_addr, bool is_interior, void* user_data), void* user_data) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("GcScanAllThreadRoots");

    // Walk all registered threads and conservatively scan their stacks.
    // Since we hold the global safepoint (generation is odd), all managed
    // threads are either spinning in SafepointPoll or have yielded —
    // their stacks are in a consistent state for conservative scanning.
    //
    // We scan every pointer-aligned slot in the stack range and report
    // it as a potential root.  The mark phase's caller will check whether
    // the value at each slot points into GC-managed memory.

    // Use a static helper since EnumerateThreads takes a C function pointer.
    // Single-threaded GC context — safe to use file-scoped globals.
    static void (*s_callback)(void*, bool, void*) = nullptr;
    static void* s_user_data = nullptr;
    s_callback = callback;
    s_user_data = user_data;

    EnumerateThreads([](ManagedThread* thread) -> bool {
        if (thread == nullptr || !thread->is_running) return true;

        // If the current thread is calling this, skip self — its registers
        // and locals are on the current native stack and will be found via
        // conventional means (or the thread is the GC thread itself).
        if (thread == tls_this_thread) return true;

        // Conservatively scan the full stack range.
        // Stack grows downward: stack_limit < stack_base on Windows/linux.
        char* scan_start = static_cast<char*>(thread->stack_limit);
        char* scan_end   = static_cast<char*>(thread->stack_base);

        if (scan_start == nullptr || scan_end == nullptr) return true;
        if (scan_start >= scan_end) return true;

        // Align to pointer boundary.
        uintptr_t start_aligned = (reinterpret_cast<uintptr_t>(scan_start) + sizeof(void*) - 1)
            & ~static_cast<uintptr_t>(sizeof(void*) - 1);
        uintptr_t end_aligned = reinterpret_cast<uintptr_t>(scan_end)
            & ~static_cast<uintptr_t>(sizeof(void*) - 1);

        for (uintptr_t slot = start_aligned; slot < end_aligned; slot += sizeof(void*)) {
            s_callback(reinterpret_cast<void*>(slot), /*is_interior=*/false, s_user_data);
        }

        return true;  // continue enumeration
    });
}}  // namespace chaos::il2cpp::runtime_core::threading
