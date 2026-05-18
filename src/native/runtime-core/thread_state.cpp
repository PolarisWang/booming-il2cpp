#include "thread_state.h"

#include <chaos/log.h>
#include <chaos/profile.h>

#include "gc_region.h"
#include "gc_root_scanner.h"
#include "gc_card_table.h"
#include "generated_code_compat.h"  // chaos_managed_exception for Thread.Abort throw

#include "../codegen/t4_seh_handler.h"    // FindT4CodeByAddress for hybrid GC scanning
#include "../codegen/native_method.h"     // NativeMethod (slot_map_data for GcSlotMapV0)

#include <atomic>
#include <cstdio>
#include <new>
#include <cstdlib>
#include <thread>
#include <cerrno>
#include <cstring>

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    #include <intrin.h>
#else
    #include <pthread.h>
#endif

namespace chaos::il2cpp::runtime_core::threading {

// ── T4 frame layout constant (mirrors code_generator.cpp kFrameSize) ─
// The T4 native prologue establishes:
//   push rbp; mov rbp, rsp; push rbx; push rsi; sub rsp, <frame_size>
// where frame_size = 864 (32 shadow + 512 GPR file + 256 FPR file + 64 CallVirtArgs).
// RSP after prologue = entry_rsp - 32 - 864.
// RBP = entry_rsp - 16.
// For GC scanning: frame_ptr (base of GcSlotMap offsets) = RSP = RBP - 880.
static constexpr uint32_t kT4FrameSize = 864;
static constexpr uint32_t kT4RbpToFramePtr = 16 + 32 + kT4FrameSize;  // 880

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
        CHAOS_IL2CPP_LOG_WARN("Thread", "setpriority failed: {0}", std::strerror(errno));
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
    thread->managed_state  = ManagedThreadState::Running;

#if defined(_WIN32) || defined(_WIN64)
    // Set default OS thread priority to THREAD_PRIORITY_NORMAL.
    ::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_NORMAL);

    // Create auto-reset event for safepoint wait (initially non-signaled).
    thread->suspend_event = ::CreateEventA(nullptr, FALSE, FALSE, nullptr);

    // Store OS thread handle for APC-based safepoint fallback.
    // Duplicate to get a handle with THREAD_SET_CONTEXT access.
    HANDLE hProcess = ::GetCurrentProcess();
    HANDLE hThread  = ::GetCurrentThread();
    ::DuplicateHandle(hProcess, hThread, hProcess, &hThread,
                      THREAD_SET_CONTEXT, FALSE, 0);
    thread->os_handle = hThread;
#else
    // POSIX: allocate and initialize a condition variable + mutex for safepoint wait.
    auto* cv = new pthread_cond_t;
    pthread_cond_init(cv, nullptr);
    thread->suspend_event = cv;
    auto* mtx = new pthread_mutex_t;
    pthread_mutex_init(mtx, nullptr);
    thread->suspend_mutex = mtx;
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

    // Close the safepoint event handle and the OS thread handle.
#if defined(_WIN32) || defined(_WIN64)
    if (thread->suspend_event != nullptr) {
        ::CloseHandle(thread->suspend_event);
        thread->suspend_event = nullptr;
    }
    if (thread->os_handle != nullptr) {
        ::CloseHandle(thread->os_handle);
        thread->os_handle = nullptr;
    }
#else
    if (thread->suspend_event != nullptr) {
        auto* cv = static_cast<pthread_cond_t*>(thread->suspend_event);
        pthread_cond_destroy(cv);
        delete cv;
        thread->suspend_event = nullptr;
    }
    if (thread->suspend_mutex != nullptr) {
        auto* mtx = static_cast<pthread_mutex_t*>(thread->suspend_mutex);
        pthread_mutex_destroy(mtx);
        delete mtx;
        thread->suspend_mutex = nullptr;
    }
#endif

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

    // Cooperative mode: wait on event (zero CPU, infinite wait).
    // ReleaseGlobalSafepoint will set the event when all threads are done.
#if defined(_WIN32) || defined(_WIN64)
    if (thread->suspend_event != nullptr) {
        ::WaitForSingleObject(thread->suspend_event, INFINITE);
    } else {
        // Fallback: spin if event not available.
        while (thread->suspend_seq.load(std::memory_order_acquire) != 0) {
            _mm_pause();
        }
    }
#else
    // POSIX: wait on condition variable (zero CPU, woken by ReleaseGlobalSafepoint
    // which broadcasts on all threads' suspend_event condvars after clearing
    // suspend_seq).  Without this, the thread would spin-yield and burn CPU
    // during every GC pause.
    if (thread->suspend_event != nullptr && thread->suspend_mutex != nullptr) {
        auto* cv = static_cast<pthread_cond_t*>(thread->suspend_event);
        auto* mtx = static_cast<pthread_mutex_t*>(thread->suspend_mutex);
        pthread_mutex_lock(mtx);
        // Re-check suspend_seq under mutex: ReleaseGlobalSafepoint may have
        // cleared it and broadcast between the check above and the lock.
        while (thread->suspend_seq.load(std::memory_order_acquire) != 0) {
            pthread_cond_wait(cv, mtx);
        }
        pthread_mutex_unlock(mtx);
    } else {
        // Fallback: spin if event not available.
        while (thread->suspend_seq.load(std::memory_order_acquire) != 0) {
            sched_yield();
        }
    }
#endif
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

#if defined(_WIN32) || defined(_WIN64)
/// APC callback: runs in the target thread's context, forcing a suspended
/// thread to acknowledge the safepoint and wait on suspend_event.
static void __stdcall SuspendApf(ULONG_PTR param) {
    auto* thread = tls_this_thread;
    if (thread == nullptr) return;
    uint32_t epoch = static_cast<uint32_t>(param);
    thread->suspend_ack.store(epoch, std::memory_order_release);
    if (thread->suspend_event != nullptr) {
        ::WaitForSingleObject(thread->suspend_event, INFINITE);
    }
}
#endif

uint32_t RequestGlobalSafepoint() noexcept {
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
            for (;;) {
                SafepointPoll();
                _mm_pause();
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
    constexpr int kSpinYieldThreshold = 32768;
    {
        static uint32_t s_confirm_epoch = 0;
        static int s_remaining = 0;
        s_confirm_epoch = epoch;

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

            if (spin < kSpinYieldThreshold) {
                _mm_pause();
            } else {
#if defined(_WIN32) || defined(_WIN64)
                Sleep(0);
#else
                std::this_thread::yield();
#endif
                // After yielding for ~100ms with no response, try APC fallback.
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
            }
        }
    }

    return epoch;
}

void ReleaseGlobalSafepoint(uint32_t /*epoch*/) noexcept {
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
#if defined(_WIN32) || defined(_WIN64)
        if (t->suspend_event != nullptr) {
            ::SetEvent(t->suspend_event);
        }
#else
        if (t->suspend_event != nullptr && t->suspend_mutex != nullptr) {
            auto* cv = static_cast<pthread_cond_t*>(t->suspend_event);
            pthread_cond_broadcast(cv);
        }
#endif
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

        // ── Phase 1: Full-stack conservative scan ─────────────────
        for (uintptr_t slot = start_aligned; slot < end_aligned; slot += sizeof(void*)) {
            auto* val_ptr = reinterpret_cast<void**>(slot);
            if (*val_ptr != nullptr &&
                reinterpret_cast<uintptr_t>(*val_ptr) >= g_heap_base) {
                s_callback(reinterpret_cast<void*>(slot), /*is_interior=*/false, s_user_data);
            }
        }

        // ── Phase 2: Precise T4 frame scanning ───────────────────
        // Scan for return addresses that fall within registered T4 code
        // ranges. Each T4 frame has a known stack layout:
        //   [rbp + 8] = return address    (slot we're scanning)
        //   [rbp + 0] = saved old RBP     (must be a stack address)
        //   frame_ptr = RSP = rbp - 880   (base for GcSlotMap offsets)
        for (uintptr_t slot = start_aligned; slot < end_aligned; slot += sizeof(void*)) {
            void* val = *reinterpret_cast<void**>(slot);
            const auto* nm = chaos::il2cpp::codegen::FindT4CodeByAddress(val);
            if (nm == nullptr) continue;
            if (nm->slot_map_data == nullptr) continue;

            if (slot < start_aligned + sizeof(void*)) continue;
            uintptr_t saved_rbp = *reinterpret_cast<uintptr_t*>(slot - sizeof(void*));
            if (saved_rbp < reinterpret_cast<uintptr_t>(scan_start) ||
                saved_rbp > reinterpret_cast<uintptr_t>(scan_end)) continue;

            uintptr_t t4_rbp = slot - sizeof(void*);
            // Use per-method RBP-to-RSP offset (register caching changes the
            // distance between RBP and the stack frame base).
            uint32_t rbpoff = nm->rbp_to_rsp_offset;
            if (rbpoff == 0) rbpoff = kT4RbpToFramePtr;  // legacy fallback
            void* frame_ptr = reinterpret_cast<uint8_t*>(t4_rbp) - rbpoff;
            auto* sm = static_cast<const GcSlotMapV0*>(nm->slot_map_data);

            ManagedFrameInfo info;
            info.frame_ptr = frame_ptr;
            info.frame_size = sm->frame_size;
            info.return_address = val;
            GcScanPreciseFrame(info, *sm, s_callback, s_user_data);
        }

        return true;  // continue enumeration
    });
}}  // namespace chaos::il2cpp::runtime_core::threading
