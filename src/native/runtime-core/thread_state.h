#ifndef CHAOS_IL2CPP_THREAD_STATE_H_
#define CHAOS_IL2CPP_THREAD_STATE_H_

#include <chaos/native_types.h>
#include <chaos/type_info.h>
#include <atomic>

namespace chaos::il2cpp::runtime_core {
struct NurseryContext;
}

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
struct ManagedThread {
    int32_t                  managed_id;          // Unique thread ID (main = 1)
    void*                    managed_object;      // System.Thread ref (nullable)
    bool                     is_running;          // false after UnregisterThread
    std::atomic<ManagedThread*> next;             // Lock-free list link
    /// Generation-based GC safepoint cooperation (Scheme C).
    std::atomic<uint32_t>    gc_mode{0};          // 0=COOPERATIVE, 1=PREEMPTIVE
    uint32_t                 last_seen_gen{0};     // Last confirmed generation
    std::atomic<bool>        pending_abort{false};    // Thread.Abort pending flag
    std::atomic<bool>        pending_interrupt{false}; // Thread.Interrupt pending flag
    bool                     is_background{false};    // Thread.IsBackground flag
    bool                     is_threadpool{false};    // ThreadPool worker flag (IsThreadPoolThread)
    ManagedThreadState       managed_state{ManagedThreadState::Unstarted};  // ThreadState
    ManagedThreadPriority    priority{ManagedThreadPriority::Normal};       // ThreadPriority
    bool                     at_safepoint{false}; // Currently paused at safepoint (legacy, kept for compat)
    uint32_t                 safepoint_generation{0}; // Last completed GC generation (legacy)
    /// TLS nursery context (for cross-thread nursery scanning in full GC).
    /// Set in NurseryAllocateSlow, cleared in TeardownTlsNursery.
    chaos::il2cpp::runtime_core::NurseryContext* nursery_ctx{nullptr};
    /// Stack bounds for conservative root scanning during full GC.
    /// Populated in RegisterThread, read-only after that.
    void* stack_base{nullptr};   // High address of the thread's stack
    void* stack_limit{nullptr};  // Low address of the thread's stack
};

// ── TLS identity (O(1), no lock) ─────────────────────────────────────

extern thread_local ManagedThread* tls_this_thread;
extern thread_local int32_t        tls_this_thread_id;

/// Current thread's managed ID (1 for the main thread).
inline int32_t GetCurrentThreadId() noexcept {
    return tls_this_thread_id;
}

/// Current thread's ManagedThread descriptor (nullptr for unregistered).
inline ManagedThread* GetCurrentThread() noexcept {
    return tls_this_thread;
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

    /// Check if GC needs attention — generation-based fast path.
    /// Returns true if the calling thread should yield for GC.
    /// Cost: single atomic load + compare (L1 hit < 1 ns).
    bool SafepointRequested() noexcept;

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
uint32_t RequestGlobalSafepoint() noexcept;

/// Release all threads from the given safepoint.
void ReleaseGlobalSafepoint(uint32_t generation) noexcept;

/// Hybrid root scanning: scan all threads' managed stacks using GcSlotMap
/// (precise) where available, falling back to conservative scanning.
/// Called after RequestGlobalSafepoint, before GC mark phase.
/// @param callback  Invoked for each precise root (root_addr, is_interior, user_data)
/// @param user_data  Opaque pointer passed through to callback
void GcScanAllThreadRoots(void (*callback)(void* root_addr, bool is_interior, void* user_data), void* user_data) noexcept;

}  // namespace chaos::il2cpp::runtime_core::threading

#endif  // CHAOS_IL2CPP_THREAD_STATE_H_
