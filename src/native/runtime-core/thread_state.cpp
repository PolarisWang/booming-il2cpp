#include "thread_state.h"

#include <chaos/log.h>
#include <chaos/profile.h>

#include "gc_region.h"
#include "gc_root_scanner.h"
#include "generated_code_compat.h"  // chaos_managed_exception for Thread.Abort throw

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

/// Map ManagedThreadPriority to OS thread priority.
/// Called on RegisterThread and chaos_thread_set_priority.
int32_t OsThreadPriorityFromManaged(ManagedThreadPriority pri) noexcept {
#if defined(_WIN32) || defined(_WIN64)
    switch (pri) {
        case ManagedThreadPriority::Lowest:      return THREAD_PRIORITY_LOWEST;
        case ManagedThreadPriority::BelowNormal: return THREAD_PRIORITY_BELOW_NORMAL;
        case ManagedThreadPriority::Normal:      return THREAD_PRIORITY_NORMAL;
        case ManagedThreadPriority::AboveNormal: return THREAD_PRIORITY_ABOVE_NORMAL;
        case ManagedThreadPriority::Highest:     return THREAD_PRIORITY_HIGHEST;
        default:                                 return THREAD_PRIORITY_NORMAL;
    }
#else
    (void)pri;
    return 0;
#endif
}

}  // anonymous namespace

void RegisterThread(int32_t managed_id, void* managed_obj) noexcept {
    auto* thread = new ManagedThread();
    thread->managed_id     = managed_id;
    thread->managed_object = managed_obj;
    thread->is_running     = true;
    thread->managed_state  = ManagedThreadState::Running;

#if defined(_WIN32) || defined(_WIN64)
    // Set default OS thread priority to THREAD_PRIORITY_NORMAL.
    ::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_NORMAL);
#endif

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
    thread->managed_state = ManagedThreadState::Stopped;

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
//
// Nesting support: a thread that already holds the safepoint can safely
// re-enter (e.g., young GC inside a full-GC callback).  A depth counter
// tracks the nesting level; only the outermost ReleaseGlobalSafepoint
// toggles the generation back to even.  s_safepoint_owner prevents two
// distinct threads from both holding the safepoint simultaneously.

namespace {
std::atomic<uint32_t> s_generation{0};
constexpr uint32_t kGcGenerationMask = 1u;
thread_local int s_safepoint_depth = 0;

/// V4-H1: Process-level safepoint owner.
/// Only the thread whose ManagedThread* is stored here may toggle the
/// generation or perform GC work.  CAS arbitration prevents two threads
/// from both holding the safepoint simultaneously.
std::atomic<ManagedThread*> s_safepoint_owner{nullptr};
}  // anonymous namespace

bool SafepointRequested() noexcept {
    uint32_t gen = s_generation.load(std::memory_order_acquire);
    return (gen & kGcGenerationMask) != 0u;
}

void SafepointPoll() noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("SafepointPoll");
    uint32_t gen = s_generation.load(std::memory_order_acquire);
    if ((gen & kGcGenerationMask) == 0u) {
        // ── pending_abort check (unlikely branch, ~0.5ns when clear) ─
        // Thread.Abort integration: check the per-thread abort flag
        // without disturbing the GC fast path.  This branch is only
        // reached when no GC is active — the LSB of s_generation is 0.
        auto* thread = tls_this_thread;
        if (thread != nullptr && thread->pending_abort.load(std::memory_order_acquire)) {
            thread->pending_abort.store(false, std::memory_order_release);
            // Throw at the next catch boundary.  The managed exception
            // dispatch will unwind managed frames to the nearest catch.
            // Implementation detail: this longjmps through the interpreter
            // frame chain; generated AOT code catches via the normal
            // chaos_managed_exception mechanism.
            throw chaos_managed_exception{0};
        }
        // ── pending_interrupt check (unlikely branch) ──────────────
        // Thread.Interrupt integration: check the per-thread interrupt
        // flag.  If set, throw ThreadInterruptedException at the next
        // catch boundary (same mechanism as Thread.Abort).
        if (thread != nullptr && thread->pending_interrupt.load(std::memory_order_acquire)) {
            thread->pending_interrupt.store(false, std::memory_order_release);
            throw chaos_managed_exception{0};
        }
        return;  // fast path: no GC pending, single load + branch
    }

    auto* thread = tls_this_thread;
    if (thread == nullptr) return;

    // Publish confirmation BEFORE entering the spin loop.
    // This tells the safepoint initiator that this thread has acknowledged
    // the odd generation and is about to stop for GC.
    thread->at_safepoint = true;
    thread->last_seen_gen = gen;

    // Spin with yield until generation flips (even = released).
    // V4-M2: No hard timeout — silently breaking out of the safepoint
    // while GC is in progress would corrupt the heap.  If a thread is
    // in a deep native frame that never polls, it won't reach this
    // code path at all (conservative stack scanning handles that case).
    constexpr int kYieldAfter = 10000;
    int spins = 0;
    while ((s_generation.load(std::memory_order_acquire) & kGcGenerationMask) != 0u) {
        if (++spins > kYieldAfter) {
            std::this_thread::yield();
            spins = 0;
        }
    }

    thread->at_safepoint = false;
}

uint32_t RequestGlobalSafepoint() noexcept {
    // Support nesting: if the calling thread already holds the safepoint,
    // just bump the depth counter and return the current generation.
    // This allows safe re-entrancy (e.g., a GC-triggered callback that
    // itself calls RequestGlobalSafepoint).
    if (s_safepoint_depth > 0) {
        s_safepoint_depth++;
        return s_generation.load(std::memory_order_acquire);
    }

    // V4-H1: Acquire process-level safepoint ownership via CAS.
    // Only one thread may hold the safepoint at any time.
    auto* self = tls_this_thread;
    if (self != nullptr) {
        ManagedThread* expected = nullptr;
        if (!s_safepoint_owner.compare_exchange_strong(expected, self,
                std::memory_order_acq_rel, std::memory_order_acquire)) {
            // Another thread holds the safepoint — spin-wait with yield.
            for (int spins = 0; ; spins++) {
                if (spins > 10000) {
                    std::this_thread::yield();
                    spins = 0;
                }
                expected = nullptr;
                if (s_safepoint_owner.compare_exchange_strong(expected, self,
                        std::memory_order_acq_rel, std::memory_order_acquire)) {
                    break;
                }
            }
        }
    }

    // Toggle to odd (GC in progress).
    uint32_t gen = s_generation.load(std::memory_order_acquire);
    uint32_t desired = (gen + 1) | kGcGenerationMask;
    s_generation.store(desired, std::memory_order_release);
    s_safepoint_depth = 1;

    // V4-H2: Bounded confirmation loop — wait for all managed threads to
    // acknowledge the odd generation via their last_seen_gen field.
    // Threads that reach SafepointPoll will set last_seen_gen = odd gen.
    // Threads in deep native frames that never poll remain unconfirmed
    // and are handled by conservative stack scanning as fallback.
    // We spin with yield up to ~10ms to let threads reach a poll point.
    // NOTE: MSVC does not allow capturing lambdas to decay to C function
    // pointers, so we use file-static helper variables (same pattern as
    // GcScanAllThreadRoots) — safe since only one GC thread runs at a time.
    {
        constexpr int kMaxConfirmSpin = 1 << 20;  // ~10ms at 3GHz
        static ManagedThread* s_confirm_self = nullptr;
        static uint32_t s_confirm_desired = 0;
        static int* s_confirm_out = nullptr;
        s_confirm_self = self;
        s_confirm_desired = desired;

        int confirm_spins = 0;
        s_confirm_out = &confirm_spins;
        EnumerateThreads([](ManagedThread* t) -> bool {
            if (t == s_confirm_self) return true;
            if (t->last_seen_gen == s_confirm_desired) return true;
            (*s_confirm_out)++;
            return true;
        });
        if (confirm_spins > 0) {
            int remaining = 0;
            for (int i = 0; i < kMaxConfirmSpin; i++) {
                remaining = 0;
                s_confirm_out = &remaining;
                EnumerateThreads([](ManagedThread* t) -> bool {
                    if (t == s_confirm_self) return true;
                    if (t->last_seen_gen != s_confirm_desired) (*s_confirm_out)++;
                    return true;
                });
                if (remaining == 0) break;
                if (i > 10000) std::this_thread::yield();
            }
        }
        s_confirm_self = nullptr;
        s_confirm_desired = 0;
        s_confirm_out = nullptr;
    }

    return desired;
}

void ReleaseGlobalSafepoint(uint32_t /*generation*/) noexcept {
    // Support nesting: decrement depth counter.  Only toggle the
    // generation back to even when the outermost release occurs.
    if (s_safepoint_depth > 1) {
        s_safepoint_depth--;
        return;
    }

    // V4-H1: Release safepoint ownership BEFORE toggling generation.
    // This ensures the safepoint owner is cleared before threads resume.
    s_safepoint_owner.store(nullptr, std::memory_order_release);

    // Toggle to even (released).
    uint32_t gen = s_generation.load(std::memory_order_acquire);
    s_generation.store((gen + 1) & ~kGcGenerationMask, std::memory_order_release);
    s_safepoint_depth = 0;
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

    // Use file-scoped statics to bridge the C function pointer API of
    // EnumerateThreads.  This is safe because GcScanAllThreadRoots is only
    // ever called from within a STW safepoint (generation is odd), and
    // the safepoint protocol ensures only one GC thread is active at a time.
    // No concurrent clobbering is possible.
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
