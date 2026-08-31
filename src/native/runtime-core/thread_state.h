#ifndef CHAOS_IL2CPP_THREAD_STATE_H_
#define CHAOS_IL2CPP_THREAD_STATE_H_

#include <chaos/native_types.h>
#include <chaos/type_info.h>
#include <chaos/pal/pal_sync.h>
#include <atomic>
#include <cstdint>

namespace chaos::il2cpp::runtime_core::threading {

constexpr int32_t kMainThreadId = 1;

// ── Managed thread state (mirrors System.Threading.ThreadState) ─────────

enum class ManagedThreadState : int32_t {
    Running         = 0,
    StopRequested   = 1,
    SuspendRequested = 2,
    Background      = 4,
    Unstarted       = 8,
    Stopped         = 16,
    WaitSleepJoin   = 32,
    Aborted         = 256,
    AbortRequested  = 512,
};

/// Validate a thread state transition in debug builds.
/// Calls CHAOS_IL2CPP_ASSERT if the transition is illegal.
/// No-op in PROFILE and SHIP builds.
inline void ValidateThreadStateTransition(ManagedThreadState from, ManagedThreadState to) noexcept {
    // Strip flag bits (Background) that can coexist with other states.
    auto strip_flags = [](ManagedThreadState s) -> int32_t {
        return static_cast<int32_t>(s) & ~static_cast<int32_t>(ManagedThreadState::Background);
    };

    int32_t f = strip_flags(from);
    int32_t t = strip_flags(to);

    // Legal transitions (from → to):
    //   Unstarted → Running          (thread starts)
    //   Running → WaitSleepJoin      (Monitor.Wait, Thread.Sleep, WaitHandle)
    //   WaitSleepJoin → Running      (woken up)
    //   Running → Stopped            (thread exits)
    //   Unstarted → Stopped          (thread never started, directly stopped)
    //   (same state)                 (always legal — setting Running→Running etc.)

    bool legal = (f == t)
        || (f == static_cast<int32_t>(ManagedThreadState::Unstarted)
            && t == static_cast<int32_t>(ManagedThreadState::Running))
        || (f == static_cast<int32_t>(ManagedThreadState::Unstarted)
            && t == static_cast<int32_t>(ManagedThreadState::Stopped))
        || (f == static_cast<int32_t>(ManagedThreadState::Running)
            && t == static_cast<int32_t>(ManagedThreadState::WaitSleepJoin))
        || (f == static_cast<int32_t>(ManagedThreadState::WaitSleepJoin)
            && t == static_cast<int32_t>(ManagedThreadState::Running))
        || (f == static_cast<int32_t>(ManagedThreadState::Running)
            && t == static_cast<int32_t>(ManagedThreadState::Stopped));

    CHAOS_IL2CPP_ASSERT(legal && "illegal thread state transition");
}

// ── Thread priority (matches System.Threading.ThreadPriority) ───────────

enum class ManagedThreadPriority : int32_t {
    Lowest         = 0,
    BelowNormal    = 1,
    Normal         = 2,
    AboveNormal    = 3,
    Highest        = 4,
};

/// Managed thread descriptor.
///
/// Allocated per thread, linked into a lock-free global list for
/// GC thread enumeration.  TLS pointers provide O(1) self-identity
/// without any lock or atomic operation on the fast path.
///
/// == Safepoint protocol ==
/// Generation-based (even = idle, odd = GC active).  The initiator
/// toggles generation odd and waits for each thread's suspend_ack.
/// Threads check suspend_seq in SafepointPoll, set suspend_ack, and
/// wait on suspend_event.  APC fallback on Windows handles threads
/// stuck in native code without safepoint polls.
struct ManagedThread {
    int32_t                  managed_id;          // Unique thread ID (main = 1)
    void*                    managed_object;      // System.Thread ref (nullable)
    bool                     is_running;          // false after UnregisterThread
    std::atomic<ManagedThread*> next;             // Lock-free list link

    // ── GC mode ──────────────────────────────────────────────────
    std::atomic<uint32_t>    gc_mode{0};          // 0=COOPERATIVE, 1=PREEMPTIVE

    // ── Per-thread safepoint handshake ───────────────────────────
    /// Non-zero = safepoint is requested.  Written by the safepoint
    /// initiator (RequestGlobalSafepoint), read by SafepointPoll.
    /// 0 means no safepoint in progress — the common fast path.
    std::atomic<uint32_t>    suspend_seq{0};

    /// Written by the thread in SafepointPoll to acknowledge suspend_seq.
    /// Read (and cleared at release) by the safepoint initiator.
    std::atomic<uint32_t>    suspend_ack{0};

    /// Nonzero while this thread is inside a write-barrier store→card critical
    /// section.  Set BEFORE the object store, cleared AFTER the card table is
    /// dirtied.  Written by the owning thread (relaxed set / release clear);
    /// read cross-thread by the safepoint coordinator (acquire).  Pairs with
    /// tls_forbid_suspend_depth: the thread acks-and-continues (no deadlock)
    /// inside the scope, yet the coordinator still waits for barrier_inflight==0
    /// before starting young-GC Phase-1, so a store can never be observed with
    /// its card still clean — closes the A2b cross-gen UAF window.
    std::atomic<uint32_t>    barrier_inflight{0};

    /// Cross-thread "forbid hard suspension" counter (CoreCLR m_dwForbidSuspendThread
    /// equivalent).  Incremented by the thread itself while it holds a lock that
    /// must not be preempted by A3 hard-suspension drive (e.g. RegionManager lock,
    /// thread registry, safepoint owner CAS).  Read cross-thread by the safepoint
    /// coordinator BEFORE and AFTER SuspendThread: if non-zero either time, the
    /// coordinator must ResumeThread and retry (or skip) that thread rather than
    /// hold it suspended while it waits on a lock the suspender also needs —
    /// avoiding a suspend-thread-vs-lock-holder deadlock.
    std::atomic<uint32_t>    forbid_suspend_count{0};

    /// Event handle for event-based safepoint wait (infinite wait,
    /// zero CPU).  Created in RegisterThread, closed in UnregisterThread.
    /// Set by ReleaseGlobalSafepoint to wake all waiting threads.
    /// Uses auto-reset PalEvent (pal_sync.h) on all platforms.
    chaos::il2cpp::pal::PalEvent* suspend_event{nullptr};

    // ── Legacy fields (kept for transition compat) ──────────────
    uint32_t                 last_seen_gen{0};     // Legacy: generation-based confirm
    bool                     at_safepoint{false};  // Legacy: safepoint flag

    // ── Thread abort / interrupt ─────────────────────────────────
    std::atomic<bool>        pending_abort{false};    // Thread.Abort pending flag
    std::atomic<bool>        pending_interrupt{false}; // Thread.Interrupt pending flag

    // ── Thread metadata ─────────────────────────────────────────
    bool                     is_background{false};    // Thread.IsBackground flag
    bool                     is_threadpool{false};    // ThreadPool worker flag
    ManagedThreadState       managed_state{ManagedThreadState::Unstarted};
    ManagedThreadPriority    priority{ManagedThreadPriority::Normal};

    // ── OS handle for APC/thread ops ─────────────────────────────┐
    /// OS thread handle (for APC fallback on Windows; SuspendThread/ResumeThread on Windows only when preemptive_suspended).
    /// Used for QueueUserAPC (safepoint fallback on Windows) and for A3 hard
    /// suspension drive (PalSuspendThread / PalResumeThread / PalGetThreadContext).
    void* os_handle{nullptr};

    // ── TLAB state (backed up across young GC safepoint) ────────
    /// TLAB range start; read/written by gc_region.cpp across
    /// GcYoungCollection, cleared by gc_young_collector.cpp via
    /// EnumerateThreads after nursery sweep.
    char* tlab_start{nullptr};
    /// TLAB current bump pointer; same protocol as tlab_start.
    char* tlab_current{nullptr};

    /// Stack bounds for conservative root scanning during full GC.
    /// Populated in RegisterThread, read-only after that.
    void* stack_base{nullptr};   // High address of the thread's stack
    void* stack_limit{nullptr};  // Low address of the thread's stack

    /// Root of the interpreter frame chain (FastFrame or RegisterFrame).
    /// Set by entry_direct.cpp on frame entry, restored on exit.
    /// Read without synchronization during STW GC (target thread is suspended).
    std::atomic<void*> current_interp_frame{nullptr};

    /// Safepoint wait start timestamp (ns since epoch), for timeout detection.
    uint64_t safepoint_wait_start_ns{0};

    /// Set to true when this thread is preemptively suspended (POSIX SIGUSR2).
    std::atomic<bool> preemptive_suspended{false};

    /// PAL capture-slot index for this thread's captured register state (Phase 2
    /// C).  Set by the target thread itself in the preemptive-suspend callback
    /// (PalGetCaptureSlot()) and read cross-thread by the GC.  -1 = no reliable
    /// capture on this platform (Windows APC-park).  The ucontext itself is owned
    /// by the PAL (PalSetPreemptContext / PalCaptureThreadContext), not here.
    int gc_capture_slot{-1};

    // ── Phase 2: GC register window ─────────────────────────────────
    /// Captured physical-GPR value file for this thread at GC suspension,
    /// indexed by physical x64 register number (RAX=0..R15=15).  Filled by
    /// CaptureThreadRegisterWindow from the PAL capture slot, read by
    /// GcScanAllThreadRoots to report safepoint register roots.  gc_num_gprs==0
    /// means no window is available and register-root reporting is skipped
    /// (stack slots remain the scan source).
    uint64_t gc_reg_file[16]{};
    uint32_t gc_num_gprs{0};

    /// OS thread ID for pthread_kill-based preemptive suspend (Linux).
    /// Populated by PalGetCurrentThreadId().  Used on Linux (non-Apple,
    /// non-Android) for SIGUSR2-based preemptive suspend.
    /// Zero on platforms that don't support signal-based suspend.
    uint64_t os_thread_id{0};

    // ── BGC concurrent root change buffer (G-25) ─────────────────
    /// Ring buffer tracking root slot modifications during BGC concurrent mark.
    /// When a root slot (static, GCHandle, etc.) is overwritten during BGC
    /// concurrent mark, the old value may have been the only reference keeping
    /// an object alive.  This buffer records those changes so the STW re-mark
    /// phase can re-scan them and mark any newly-unreachable objects.
    static constexpr uint32_t kRootChangeBufferSize = 64;

    /// One entry: a root slot whose old value may have been lost.
    struct RootChangeEntry {
        void*     old_value;  ///< The object reference that was in the slot before overwrite
        void**    slot;       ///< The root slot that was overwritten
    };

    /// Ring buffer of root change entries.  Producers (mutators) write at
    /// head, consumer (BGC re-mark) reads from tail to head.
    RootChangeEntry root_change_buffer_[kRootChangeBufferSize];

    /// Atomic head index for the ring buffer (producer write position).
    /// Wrapped at kRootChangeBufferSize.  Monotonically increasing (modulo
    /// the buffer size) — use head % kRootChangeBufferSize to get the slot.
    std::atomic<uint32_t> root_change_head_{0};

    /// Atomic tail index for the ring buffer (consumer read position).
    /// Same wrap convention as head.
    std::atomic<uint32_t> root_change_tail_{0};
};

// ── TLS identity (O(1), no lock) ─────────────────────────────────────

extern thread_local ManagedThread* tls_this_thread;
extern thread_local int32_t        tls_this_thread_id;

/// Set thread state with debug validation.
/// In PROFILE/SHIP, compiles to a plain assignment.
inline void SetThreadState(ManagedThread& thread, ManagedThreadState new_state) noexcept {
    ValidateThreadStateTransition(thread.managed_state, new_state);
    thread.managed_state = new_state;
}

/// Current thread's managed ID (1 for the main thread).
inline int32_t GetCurrentThreadId() noexcept {
    return tls_this_thread_id;
}

/// Current thread's ManagedThread descriptor (nullptr for unregistered).
inline ManagedThread* GetCurrentThread() noexcept {
    return tls_this_thread;
}

/// Set the current thread's interpreter frame chain root.
/// Only the owning thread should call this (on frame entry/exit).
/// GC reads this during STW while the thread is suspended.
inline void SetCurrentInterpFrame(void* frame) noexcept {
    if (auto* t = tls_this_thread) {
        t->current_interp_frame.store(frame, std::memory_order_relaxed);
    }
}

/// Get the current thread's interpreter frame chain root.
inline void* GetCurrentInterpFrame() noexcept {
    if (auto* t = tls_this_thread) {
        return t->current_interp_frame.load(std::memory_order_relaxed);
    }
    return nullptr;
}

// ── Lock-free thread registry ────────────────────────────────────────

/// Register the calling thread as a managed thread.
void RegisterThread(int32_t managed_id, void* managed_obj) noexcept;

/// Unregister the calling thread (called at thread exit).
void UnregisterThread() noexcept;

/// Allocate a monotonically increasing thread ID.
int32_t AllocateThreadId() noexcept;

/// Iterate all active registered threads.
/// @param callback  Called for each ManagedThread. If it returns false,
///                  iteration stops early. Called under no lock — the
///                  caller must not unregister threads from the callback.
void EnumerateThreads(bool (*callback)(ManagedThread*)) noexcept;

/// Approximate count of active threads.
int32_t GetThreadCount() noexcept;


// ── Hybrid GC safepoint ───────────────────────────────────────────────

// GC mode constants for ManagedThread::gc_mode.
// COOPERATIVE: thread may access managed heap at any time; must spin-wait
//              at safepoint until GC completes.
// PREEMPTIVE:  thread is in native code and will not access managed heap;
//              confirms safepoint and returns immediately.
constexpr uint32_t kGcModeCooperative = 0;
constexpr uint32_t kGcModePreemptive  = 1;

/// Called at GC safe points (loop back-edges, method calls).
/// If a GC safepoint is active, the thread acknowledges and spins until
/// released.  Threads that are inside native AOT frames (which lack
    // ── Generation-based GC safepoint ───────────────────────────────────

    /// Check if GC safepoint is active — per-thread suspend_seq check.
    /// Returns true if the calling thread should yield for GC.
    /// Cost: TLS load + single atomic load (~1 ns when no GC active).
    inline bool SafepointRequested() noexcept {
        auto* thread = tls_this_thread;
        return thread != nullptr &&
               thread->suspend_seq.load(std::memory_order_acquire) != 0;
    }

    /// Called at GC safe points (loop back-edges, method calls).
    /// If a GC safepoint is active, the thread acknowledges and spins until
    /// released.  Threads that are inside native AOT frames (which lack
    /// explicit polls) are handled by CRAG conservative stack scanning.
    void SafepointPoll() noexcept;

/// Switch the calling thread to cooperative GC mode.
/// In cooperative mode, the thread may access managed heap objects.  At a
/// safepoint, the thread spins until GC completes.
/// Must be called before entering managed code or accessing managed objects.
void EnterCooperativeMode() noexcept;

/// Switch the calling thread to preemptive GC mode.
/// In preemptive mode, the thread will NOT access managed heap objects.
/// At a safepoint, the thread confirms the safepoint (sets last_seen_gen)
/// and returns immediately without spinning.  The GC will not wait for this
/// thread.
/// Must be called before entering native code that may block or run for
/// extended periods without safepoint polls.
void EnterPreemptiveMode() noexcept;

/// Request all managed threads to reach a safepoint.
/// @returns the safepoint generation to pass to ReleaseGlobalSafepoint.
extern "C" uint32_t RequestGlobalSafepoint() noexcept;

/// Release all threads from the given safepoint.
extern "C" void ReleaseGlobalSafepoint(uint32_t generation) noexcept;

/// Hybrid root scanning: scan all threads' managed stacks using GcSlotMap
/// (precise) where available, falling back to conservative scanning.
/// Called after RequestGlobalSafepoint, before GC mark phase.
/// @param callback  Invoked for each precise root (root_addr, is_interior, user_data)
/// @param user_data  Opaque pointer passed through to callback
void GcScanAllThreadRoots(void (*callback)(void* root_addr, bool is_interior, void* user_data), void* user_data) noexcept;

}  // namespace chaos::il2cpp::runtime_core::threading

// extern "C" bridges for threading_stubs — declared outside the
// namespace so they are visible from chaos::il2cpp::runtime_core.
extern "C" void chaos_enumerate_threads(bool (*callback)(chaos::il2cpp::runtime_core::threading::ManagedThread*)) noexcept;
extern "C" chaos::il2cpp::runtime_core::threading::ManagedThread* chaos_get_tls_this_thread() noexcept;

#endif  // CHAOS_IL2CPP_THREAD_STATE_H_
