#include "thread_state.h"

#include <chaos/asan_interface.h>
#include <chaos/log.h>
#include <chaos/profile.h>
#include <chaos/pal/pal_sync.h>
#include <chaos/pal/pal_thread.h>

#include "gc_region.h"
#include "gc_root_scanner.h"
#include "gc_static_roots.h"
#include "gc_card_table.h"
#include "gc_heap_manager.h"
#include "generated_code_compat.h"  // chaos_managed_exception for Thread.Abort throw

#include "forbid_suspend.h"

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    #include <intrin.h>
#else
    #include <pthread.h>
    #include <signal.h>
    #include <sys/resource.h>
#endif

#include "../jit/jit_seh.h"    // FindNativeCodeByAddress for hybrid GC scanning
#include "../jit/jit_method.h"     // JitMethod (slot_map_data for GcSlotMapV0)

#include <atomic>
#include <cstdio>
#include <new>
#include <cstdlib>
#include <thread>
#include <cerrno>
#include <chrono>
#include <cstring>

namespace chaos::il2cpp::runtime_core::threading {

using chaos::il2cpp::pal::PalEvent;
using chaos::il2cpp::pal::PalEventCreate;
using chaos::il2cpp::pal::PalEventDestroy;
using chaos::il2cpp::pal::PalEventSet;
using chaos::il2cpp::pal::PalEventWait;
using chaos::il2cpp::pal::PalGetCurrentThreadId;
using chaos::il2cpp::pal::PalGetStackBounds;

// ── T4 frame layout constant (mirrors code_generator.cpp kFrameSize) ─
// The T4 native prologue establishes:
//   push rbp; mov rbp, rsp; push rbx; push rsi; sub rsp, <frame_size>
// where frame_size = 864 (32 shadow + 512 GPR file + 256 FPR file + 64 CallVirtArgs).
// RSP after prologue = entry_rsp - 32 - 864.
// RBP = entry_rsp - 16.
// For GC scanning: frame_ptr (base of GcSlotMap offsets) = RSP = RBP - 880.
static constexpr uint32_t kJitFrameSize = 864;
static constexpr uint32_t kJitRbpToFramePtr = 16 + 32 + kJitFrameSize;  // 880

// ── TLS definitions ──────────────────────────────────────────────────

thread_local ManagedThread* tls_this_thread  = nullptr;
thread_local int32_t        tls_this_thread_id = kMainThreadId;

thread_local int32_t        tls_forbid_suspend_depth = 0;

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
    // POSIX: map managed priority to nice value (-20..19 for SCHED_OTHER).
    // SCHED_OTHER only allows nice-based prioritization within the same
    // scheduling policy; root privileges are NOT required for nice values.
    int nice_value;
    switch (pri) {
        case ManagedThreadPriority::Lowest:      nice_value = 19;  break;
        case ManagedThreadPriority::BelowNormal: nice_value = 10;  break;
        case ManagedThreadPriority::AboveNormal: nice_value = -10; break;
        case ManagedThreadPriority::Highest:     nice_value = -20; break;
        case ManagedThreadPriority::Normal:
        default:                                 nice_value = 0;   break;
    }
    errno = 0;
    if (setpriority(PRIO_PROCESS, 0, nice_value) != 0 && errno != 0) {
        CHAOS_IL2CPP_LOG_WARN_M("Thread", "setpriority failed: {0}", std::strerror(errno));
    }
    return nice_value;
#endif
}

}  // anonymous namespace

void RegisterThread(int32_t managed_id, void* managed_obj) noexcept {
    auto* thread = new ManagedThread();
    thread->managed_id     = managed_id;
    thread->managed_object = managed_obj;
    thread->is_running     = true;
    SetThreadState(*thread, ManagedThreadState::Running);

    // Create auto-reset event for safepoint wait (initially non-signaled).
    thread->suspend_event = PalEventCreate(false, false);

    // Store OS thread ID for signal-based preemptive suspend (Linux).
    thread->os_thread_id = PalGetCurrentThreadId();

#if defined(_WIN32) || defined(_WIN64)
    // Duplicate OS thread handle for APC-based safepoint fallback.
    HANDLE hProcess = ::GetCurrentProcess();
    HANDLE hThread  = ::GetCurrentThread();
    ::DuplicateHandle(hProcess, hThread, hProcess, &hThread,
                      THREAD_SET_CONTEXT, FALSE, 0);
    thread->os_handle = hThread;
#endif

    // Capture stack bounds for conservative root scanning during full GC.
    PalGetStackBounds(thread->stack_base, thread->stack_limit);

    // Publish to TLS for O(1) self-lookup.
    tls_this_thread    = thread;
    tls_this_thread_id = managed_id;

    // Lock-free insert at head of the global thread list.
    ManagedThread* expected = s_thread_list.load(std::memory_order_acquire);
    do {
        thread->next.store(expected, std::memory_order_relaxed);
    } while (!s_thread_list.compare_exchange_weak(expected, thread,
        std::memory_order_release, std::memory_order_acquire));

    // Server GC: bind this thread to its NUMA-aware heap.
    SetThreadHeap();
}

void UnregisterThread() noexcept {
    auto* thread = tls_this_thread;
    if (thread == nullptr) return;

    thread->is_running = false;
    SetThreadState(*thread, ManagedThreadState::Stopped);

    // Return the TLS nursery to the region manager before clearing TLS.
    // Otherwise the nursery region leaks until process exit.
    TeardownTlsNursery();

    // TLS still points to the entry so EnumerateThreads callbacks can
    // safely query the current thread.  The entry is leaked intentionally
    // (lives for process lifetime like most runtime-instantiated metadata).

    // Close the safepoint event handle.
    if (thread->suspend_event != nullptr) {
        PalEventDestroy(thread->suspend_event);
        thread->suspend_event = nullptr;
    }

    // Server GC: clear heap binding before TLS clear.
    ClearThreadHeap();

    tls_this_thread    = nullptr;
    tls_this_thread_id = 0;

    // Unregister from the profile system so ProfileDump does not read freed
    // thread_local storage (g_tls_profile is destroyed after this function
    // returns).
    chaos::il2cpp::common::UnregisterThread(g_tls_profile);
}

int32_t AllocateThreadId() noexcept {
    return s_next_thread_id.fetch_add(1, std::memory_order_relaxed);
}

void EnumerateThreads(bool (*callback)(ManagedThread*)) noexcept {
    ForbidSuspendScope forbid;
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

// ── extern "C" bridges for threading_stubs ────────────────────────
// MSVC generates C-linkage (undecorated) references from inside
// extern "C" blocks, so threading_stubs.cpp cannot call
// EnumerateThreads or access tls_this_thread directly.  These bridges
// are defined here where C++ name lookup resolves correctly, and the
// extern "C" linkage on the function names matches the C-linkage
// references from threading_stubs.cpp.

extern "C" void chaos_enumerate_threads(bool (*callback)(ManagedThread*)) noexcept {
    EnumerateThreads(callback);
}

extern "C" ManagedThread* chaos_get_tls_this_thread() noexcept {
    return tls_this_thread;
}

// ── Per-thread handshake safepoint ───────────────────────────────────
//
// Each ManagedThread has suspend_seq and suspend_ack fields.
// RequestGlobalSafepoint sets each thread's suspend_seq to a monotonic
// epoch, then waits per-thread for suspend_ack.  SafepointPoll checks
// the thread-local suspend_seq (fast path = 0, ~1 cycle) and if
// non-zero, sets suspend_ack and waits on suspend_event (zero CPU).
// ReleaseGlobalSafepoint clears suspend_seq for all threads and signals
// suspend_event to wake them.
//
// Nesting: thread_local s_safepoint_depth tracks re-entrancy.  Only the
// outermost Request/Release pair performs the full handshake.
//
// Single-owner: s_safepoint_owner CAS prevents two threads from both
// holding the safepoint.

namespace {
std::atomic<ManagedThread*> s_safepoint_owner{nullptr};
thread_local int s_safepoint_depth = 0;

/// Monotonic epoch counter for suspend_seq values.
/// Threads compare against their stored value, not this counter directly.
std::atomic<uint32_t> s_safepoint_epoch{1};

// ── Safepoint timeout constants (matching CoreCLR) ──────────────
/// Default safepoint timeout after which non-responding threads are
/// preemptively suspended. 100ms matches CoreCLR's default timeout.
static constexpr uint64_t kSafepointTimeoutNs = 100ULL * 1000000ULL;  // 100 ms

/// Hard timeout: if a thread doesn't ack even after preemptive suspend,
/// force-release the safepoint and proceed with conservative scanning.
static constexpr uint64_t kSafepointHardTimeoutNs = 500ULL * 1000000ULL;  // 500 ms

/// Spin threshold before yielding (matching existing code).
static constexpr int kSpinYieldThreshold = 32768;
}  // anonymous namespace

void SafepointPoll() noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("SafepointPoll");

    auto* thread = tls_this_thread;
    if (thread == nullptr) return;

    // Fast path: single atomic load of suspend_seq.
    uint32_t seq = thread->suspend_seq.load(std::memory_order_acquire);
    if (seq == 0) [[likely]] {
        // pending_abort check (same as before, ~0.5ns when clear).
        if (thread->pending_abort.load(std::memory_order_acquire)) {
            thread->pending_abort.store(false, std::memory_order_release);
            throw chaos_managed_exception{kManagedExceptionThreadAbort};
        }
        // pending_interrupt check (lower priority than abort).
        if (thread->pending_interrupt.load(std::memory_order_acquire)) {
            thread->pending_interrupt.store(false, std::memory_order_release);
            throw chaos_managed_exception{kManagedExceptionThreadInterrupt};
        }
        return;
    }

    // ── Slow path: safepoint is active ────────────────────────────

    // Acknowledge the safepoint request.
    thread->suspend_ack.store(seq, std::memory_order_release);

    // Preemptive mode: confirm and return immediately.
    // The thread is in native code and will not access managed heap.
    if (thread->gc_mode.load(std::memory_order_acquire) == kGcModePreemptive) {
        return;
    }

    // ForbidSuspend is active: acknowledge but don't block.
    // The critical section must complete without waiting. The GC proceeds
    // thinking this thread is at a safe point; when the ForbidSuspendScope
    // exits, the next SafepointPoll will properly wait if the safepoint
    // is still active.
    if (tls_forbid_suspend_depth > 0) [[unlikely]] {
        return;
    }

    // Cooperative mode: wait on event (zero CPU, infinite wait).
    // ReleaseGlobalSafepoint will set the event when all threads are done.
    if (thread->suspend_event != nullptr) {
        PalEventWait(thread->suspend_event, UINT64_MAX);
    } else {
        // Fallback: spin if event not available.
        while (thread->suspend_seq.load(std::memory_order_acquire) != 0) {
            CHAOS_IL2CPP_PAUSE_HINT();
        }
    }
}

void EnterCooperativeMode() noexcept {
    auto* thread = tls_this_thread;
    if (thread == nullptr) return;
    thread->gc_mode.store(kGcModeCooperative, std::memory_order_release);
    // After switching to cooperative, check if a safepoint is already active.
    // If so, this thread must participate in the safepoint.
    SafepointPoll();
}

void EnterPreemptiveMode() noexcept {
    auto* thread = tls_this_thread;
    if (thread == nullptr) return;
    // Mark as preemptive BEFORE any subsequent SafepointPoll sees the flag.
    thread->gc_mode.store(kGcModePreemptive, std::memory_order_release);
}

#if !defined(_WIN32) && !defined(_WIN64) && !defined(__APPLE__) && !defined(__ANDROID__)
/// Signal handler for SIGUSR2: preemptive safepoint suspend.
/// Runs in the target thread's context. Uses spin-wait because
/// pthread_cond_wait is not async-signal-safe.
static void SafepointSuspendHandler(int sig) {
    if (sig != SIGUSR2) return;
    auto* thread = tls_this_thread;
    if (thread == nullptr) return;

    // Acknowledge safepoint (set suspend_ack to current suspend_seq).
    uint32_t seq = thread->suspend_seq.load(std::memory_order_acquire);
    thread->suspend_ack.store(seq, std::memory_order_release);

    // Spin-wait until suspend_seq is cleared by ReleaseGlobalSafepoint.
    while (thread->suspend_seq.load(std::memory_order_acquire) != 0) {
        sched_yield();
    }
}

/// Install the SIGUSR2 handler for preemptive safepoint suspend.
static std::atomic<bool> s_sigusr1_installed{false};
static void InstallSafepointSignalHandler() noexcept {
    if (s_sigusr1_installed.load(std::memory_order_acquire)) return;
    struct sigaction sa;
    std::memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SafepointSuspendHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NODEFER;
    sigaction(SIGUSR2, &sa, nullptr);
    s_sigusr1_installed.store(true, std::memory_order_release);
}
#endif

#if defined(_WIN32) || defined(_WIN64)
/// APC callback: runs in the target thread's context, forcing a suspended
/// thread to acknowledge the safepoint and wait on suspend_event.
static void __stdcall SuspendApf(ULONG_PTR param) {
    auto* thread = tls_this_thread;
    if (thread == nullptr) return;
    uint32_t epoch = static_cast<uint32_t>(param);
    thread->suspend_ack.store(epoch, std::memory_order_release);
    if (thread->suspend_event != nullptr) {
        PalEventWait(thread->suspend_event, UINT64_MAX);
    }
}
#endif

extern "C" uint32_t RequestGlobalSafepoint() noexcept {
    // Support nesting: if the calling thread already holds the safepoint,
    // just bump the depth counter and return the current epoch.
    if (s_safepoint_depth > 0) {
        s_safepoint_depth++;
        return s_safepoint_epoch.load(std::memory_order_acquire);
    }

    // Acquire process-level safepoint ownership via CAS.
    auto* self = tls_this_thread;
    if (self != nullptr) {
        ManagedThread* expected = nullptr;
        if (!s_safepoint_owner.compare_exchange_strong(expected, self,
                std::memory_order_acq_rel, std::memory_order_acquire)) {
            // Another thread holds the safepoint — spin-wait with pause.
            // Must call SafepointPoll() to acknowledge a pending safepoint
            // from the owner; otherwise this thread and the owner deadlock
            // (owner waits for our ack, we wait for the owner to release).
            //
            // ForbidSuspendScope prevents SafepointPoll from blocking if
            // the owner requests a new safepoint while we're waiting for
            // ownership. Without this, we'd wait on suspend_event and never
            // acquire the safepoint to release it.
            for (;;) {
                {
                    ForbidSuspendScope forbid;
                    SafepointPoll();
                    CHAOS_IL2CPP_PAUSE_HINT();
                }
                expected = nullptr;
                if (s_safepoint_owner.compare_exchange_strong(expected, self,
                        std::memory_order_acq_rel, std::memory_order_acquire)) {
                    break;
                }
            }
        }
    }

    // Bump the epoch counter.  This becomes the suspend_seq value for
    // all cooperative threads.
    uint32_t epoch = s_safepoint_epoch.fetch_add(1, std::memory_order_acq_rel) + 1;
    s_safepoint_depth = 1;

    // Set suspend_seq for every cooperative thread.
    // Preemptive threads are excluded: they don't access managed heap,
    // so we don't need to wait for them.
    // NOTE: EnumerateThreads requires C function pointers, so we use
    // file-static helper variables (no captures).  Safe because only
    // one GC thread runs at a time (safepoint owner CAS guarantees this).
    {
        static uint32_t s_set_epoch = 0;
        s_set_epoch = epoch;
        EnumerateThreads([](ManagedThread* t) -> bool {
            if (t->gc_mode.load(std::memory_order_acquire) == kGcModePreemptive)
                return true;
            t->suspend_seq.store(s_set_epoch, std::memory_order_release);
            return true;
        });
    }

    // Wait for each cooperative thread to acknowledge.
    // Spin with pause for ~1ms, then yield to avoid starving the very
    // threads we're waiting on (critical for oversubscribed scenarios).
    // After kSafepointTimeoutNs (100ms), use preemptive suspend fallback
    // matching CoreCLR: QueueUserAPC on Windows, pthread_kill on POSIX.
    // After kSafepointHardTimeoutNs (500ms), force-release with diagnostic.
    {
        static uint32_t s_confirm_epoch = 0;
        static int s_remaining = 0;
        s_confirm_epoch = epoch;

        auto wait_start = std::chrono::steady_clock::now();
        bool preemptive_attempted = false;
        bool hard_timeout = false;

        for (int spin = 0; ; spin++) {
            s_remaining = 0;
            EnumerateThreads([](ManagedThread* t) -> bool {
                if (t->gc_mode.load(std::memory_order_acquire) == kGcModePreemptive)
                    return true;
                if (t == tls_this_thread) return true;
                if (t->suspend_ack.load(std::memory_order_acquire) != s_confirm_epoch)
                    ++s_remaining;
                return true;
            });
            if (s_remaining == 0) break;
            if (hard_timeout) break;  // force-release after hard timeout

            if (spin < kSpinYieldThreshold) {
                CHAOS_IL2CPP_PAUSE_HINT();
            } else {
#if defined(_WIN32) || defined(_WIN64)
                Sleep(0);
#else
                std::this_thread::yield();
#endif
                // After yielding for ~100ms with no response, try APC fallback
                // (Windows only — cooperative threads stuck in native code).
                if (spin >= kSpinYieldThreshold + 100000) {
#if defined(_WIN32) || defined(_WIN64)
                    EnumerateThreads([](ManagedThread* t) -> bool {
                        if (t->gc_mode.load(std::memory_order_acquire) == kGcModePreemptive)
                            return true;
                        if (t == tls_this_thread) return true;
                        if (t->suspend_ack.load(std::memory_order_acquire) != s_confirm_epoch) {
                            ::QueueUserAPC(SuspendApf, t->os_handle, s_confirm_epoch);
                        }
                        return true;
                    });
#endif
                    std::this_thread::sleep_for(std::chrono::microseconds(100));
                }

                // Check real-time timeout every ~5000 iterations (~5ms).
                if (spin % 5000 == 0 && !hard_timeout) {
                    auto elapsed = std::chrono::steady_clock::now() - wait_start;
                    uint64_t elapsed_ns = std::chrono::duration_cast<
                        std::chrono::nanoseconds>(elapsed).count();

                    if (!preemptive_attempted && elapsed_ns >= kSafepointTimeoutNs) {
                        preemptive_attempted = true;
                        CHAOS_IL2CPP_LOG_WARN_M("Safepoint",
                            "safepoint timeout: {0} threads unresponsive after {1}ms, "
                            "attempting preemptive suspend",
                            s_remaining, elapsed_ns / 1000000);

                        EnumerateThreads([](ManagedThread* t) -> bool {
                            if (t->gc_mode.load(std::memory_order_acquire) == kGcModePreemptive)
                                return true;
                            if (t == tls_this_thread) return true;
                            if (t->suspend_ack.load(std::memory_order_acquire) != s_confirm_epoch) {
#if defined(_WIN32) || defined(_WIN64)
                                // Windows: QueueUserAPC is the safe fallback for
                                // unresponsive threads — it runs in the target's
                                // context when it enters an alertable wait.  Do NOT
                                // use SuspendThread here: the thread may hold OS
                                // locks (heap, loader, etc.) and suspension would
                                // corrupt process state upon resume.
                                ::QueueUserAPC(SuspendApf, t->os_handle, s_confirm_epoch);
                                CHAOS_IL2CPP_LOG_DEBUG("Safepoint",
                                    "APC queued for thread {0}", t->managed_id);
#elif !defined(__APPLE__)
                                // POSIX (non-iOS): send SIGUSR2 to force safepoint ack.
                                InstallSafepointSignalHandler();
                                pthread_kill(t->os_thread_id, SIGUSR2);
                                t->preemptive_suspended.store(true, std::memory_order_release);
#else
                                // iOS: no force-suspend available. Log and continue.
                                CHAOS_IL2CPP_LOG_WARN_M("Safepoint",
                                    "thread {0} unresponsive, "
                                    "cannot preemptively suspend on iOS",
                                    t->managed_id);
#endif
                            }
                            return true;
                        });
                    }

                    if (preemptive_attempted && elapsed_ns >= kSafepointHardTimeoutNs) {
                        hard_timeout = true;
                        CHAOS_IL2CPP_LOG_ERROR_M("Safepoint",
                            "safepoint hard timeout: {0} threads still unresponsive "
                            "after {1}ms, forcing release",
                            s_remaining, elapsed_ns / 1000000);
                    }
                }
            }
        }
    }

    return epoch;
}

extern "C" void ReleaseGlobalSafepoint(uint32_t /*epoch*/) noexcept {
    // Support nesting: decrement depth counter.  Only do the full
    // release when the outermost release occurs.
    if (s_safepoint_depth > 1) {
        s_safepoint_depth--;
        return;
    }

    // ── Release all threads from safepoint ────────────────────────────
    // Order is critical:
    //   1. Clear suspend_seq so threads stop waiting
    //   2. Signal events to wake threads
    //   3. THEN release ownership — releasing ownership before clearing
    //      suspend_seq creates a window where a new acquirer sets new
    //      seq values that get cleared by the ongoing iteration, causing
    //      the next safepoint to spin forever waiting for acks that will
    //      never come (the threads see suspend_seq=0 and run freely).

    // Clear suspend_seq for all threads and signal their events.
    EnumerateThreads([](ManagedThread* t) -> bool {
        t->suspend_seq.store(0, std::memory_order_release);
        if (t->suspend_event != nullptr) {
            PalEventSet(t->suspend_event);
        }
        // Resume preemptively suspended threads.
        if (t->preemptive_suspended.load(std::memory_order_acquire)) {
            t->preemptive_suspended.store(false, std::memory_order_release);
        }
        t->safepoint_wait_start_ns = 0;
        return true;
    });

    // Release safepoint ownership AFTER all threads are woken.
    s_safepoint_owner.store(nullptr, std::memory_order_release);

    s_safepoint_depth = 0;
}

void GcScanAllThreadRoots(void (*callback)(void* root_addr, bool is_interior, void* user_data), void* user_data) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("GcScanAllThreadRoots");

    // Walk all registered threads and conservatively scan their stacks.
    // Since we hold the global safepoint (suspend_seq non-zero), all managed
    // threads are either waiting on suspend_event or have acknowledged the
    // safepoint — their stacks are in a consistent state for scanning.
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

        // If the current thread is calling this, skip self.


        // Conservatively scan the full stack range.
        char* scan_start = static_cast<char*>(thread->stack_limit);
        char* scan_end   = static_cast<char*>(thread->stack_base);


        uintptr_t start_aligned = (reinterpret_cast<uintptr_t>(scan_start) + sizeof(void*) - 1)
            & ~static_cast<uintptr_t>(sizeof(void*) - 1);
        uintptr_t end_aligned = reinterpret_cast<uintptr_t>(scan_end)
            & ~static_cast<uintptr_t>(sizeof(void*) - 1);

        // ── Phase 1: Full-stack conservative scan ─────────────────
        for (uintptr_t slot = start_aligned; slot < end_aligned; slot += sizeof(void*)) {
            auto* val_ptr = reinterpret_cast<void**>(slot);
            if (auto* read = static_cast<void*>(
                    chaos::il2cpp::common::AsanReadPtrNoCheck(val_ptr));
                read != nullptr &&
                reinterpret_cast<uintptr_t>(read) >= g_heap_base) {
                s_callback(reinterpret_cast<void*>(slot), /*is_interior=*/false, s_user_data);
            }
        }


        // Scan for return addresses that fall within registered T4 code
        // ranges. Each T4 frame has a known stack layout:
        //   [rbp + 8] = return address    (slot we're scanning)
        //   [rbp + 0] = saved old RBP     (must be a stack address)
        //   frame_ptr = RSP = rbp - 880   (base for GcSlotMap offsets)
        for (uintptr_t slot = start_aligned; slot < end_aligned; slot += sizeof(void*)) {
            void* val = static_cast<void*>(
                chaos::il2cpp::common::AsanReadPtrNoCheck(
                    reinterpret_cast<void*>(slot)));
            const auto* nm = chaos::il2cpp::jit::FindNativeCodeByAddress(val);
            if (nm == nullptr) continue;
            if (nm->slot_map_data == nullptr) continue;

            if (slot < start_aligned + sizeof(void*)) continue;
            uintptr_t saved_rbp = reinterpret_cast<uintptr_t>(
                chaos::il2cpp::common::AsanReadPtrNoCheck(
                    reinterpret_cast<void*>(slot - sizeof(void*))));
            if (saved_rbp < reinterpret_cast<uintptr_t>(scan_start) ||
                saved_rbp > reinterpret_cast<uintptr_t>(scan_end)) continue;

            uintptr_t t4_rbp = slot - sizeof(void*);
            // Use per-method RBP-to-RSP offset (register caching changes the
            // distance between RBP and the stack frame base).
            uint32_t rbpoff = nm->rbp_to_rsp_offset;
            if (rbpoff == 0) rbpoff = kJitRbpToFramePtr;  // legacy fallback
            void* frame_ptr = reinterpret_cast<uint8_t*>(t4_rbp) - rbpoff;
            auto* sm = static_cast<const GcSlotMapV0*>(nm->slot_map_data);

            ManagedFrameInfo info;
            info.frame_ptr = frame_ptr;
            info.frame_size = sm->frame_size;
            info.return_address = val;
            GcScanPreciseFrame(info, *sm, s_callback, s_user_data);
        }

        // ── Phase 2b: Interpreter frame precise scanning ─────────
        // Walk the interpreter frame chain (FastFrame/RegisterFrame) and
        // precisely scan each frame using runtime type tags.
        // Unlike JIT frames (which have static GcSlotMaps), interpreter
        // frames have dynamic stack/locals arrays where each slot carries
        // a ValueTag byte indicating whether it's an ObjectRef.
        auto* interp_scanner = GcGetInterpFrameScanner();
        if (interp_scanner != nullptr) {
            void* interp_root = thread->current_interp_frame.load(
                std::memory_order_acquire);
            if (interp_root != nullptr) {
                interp_scanner(interp_root, s_callback, s_user_data);
            }
        }

        return true;  // continue enumeration
    });

    // Phase 3: Scan registered static root ranges (ALC-isolated static fields).
    GcScanStaticRoots(s_callback, s_user_data);
}}

