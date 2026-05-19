#ifndef CHAOS_IL2CPP_GC_BGC_H_
#define CHAOS_IL2CPP_GC_BGC_H_

/// @file gc_bgc.h
/// Background GC (BGC) — concurrent mark + sweep for old-generation collection.
///
/// == Three phases ==
/// 1. Concurrent Mark  — BGC thread tri-color marks reachable objects
///                       while mutators run.  SATB pre-write barrier
///                       preserves snapshot-at-the-beginning invariant.
/// 2. STW Re-Mark      — Brief safepoint drains remaining SATB entries
///                       and dirty cards.  Typically < 5ms.
/// 3. Concurrent Sweep — BGC thread sweeps unmarked pages to free lists.
///
/// == Write barrier ==
/// Pre-write SATB (Snapshot-At-The-Beginning):
///   Before: *slot = new_value
///   Barrier records the OLD value at *slot before it is overwritten.
///
/// == Root set ==
/// Populated under an initial STW safepoint:
///   - Pinned roots (registered via AddPinnedRoot)
///   - All thread stacks (via GcScanAllThreadRoots)
///   - All TLS nurseries (via EnumerateThreads + ScanRangeForRoots)
///   - GCHandle table (via GcIterateHandleTable)
///
/// == Thread safety ==
/// - g_bgc_is_marking: atomic bool, set/cleared under safepoint
/// - Mutator SATB buffer: thread_local (lock-free single-producer)
/// - Global SATB queue: mutex-protected
/// - BGC mark stack: mutex-protected (BGC thread + STW re-mark both access)

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <thread>
#include <vector>

#include <chaos/native_types.h>

#include "gc_old_gen.h"

namespace chaos::il2cpp::runtime_core {

// ======================================================================
// BGC phase enum
// ======================================================================

enum class BgcPhase : uint8_t {
    IDLE             = 0,  ///< No concurrent mark in progress
    ROOT_COLLECT     = 1,  ///< STW: populating root set (under safepoint)
    CONCURRENT_MARK  = 2,  ///< BGC thread tracing reachable objects
    REMARK_NEEDED    = 3,  ///< Concurrent mark done; waiting for STW re-mark
    STW_REMARK       = 4,  ///< STW safepoint: drain SATB + dirty cards
    CONCURRENT_SWEEP = 5,  ///< BGC thread sweeping unmarked pages
    COMPACT_NEEDED   = 6,  ///< Concurrent sweep done; waiting for STW compaction
    FINISHED         = 7,  ///< BGC cycle complete; reset to IDLE
};

// ======================================================================
// SATB entry
// ======================================================================

/// One SATB entry: an object pointer that was reachable before the
/// mutator overwrote the field that pointed to it.
using SatbEntry = void*;

// ======================================================================
// Thread-local SATB buffer
//
// Each mutator thread has a thread_local buffer.  The pre-write barrier
// stores old-object references with a relaxed atomic count increment.
// When the buffer is full, the mutator flushes to the global queue.
// During STW re-mark, all thread-local buffers are drained directly.
// ======================================================================

static constexpr int kBgcSatbBufferSize = 512;

/// SATB buffer (heap-allocated, indexed by tls_satb_buffer_index).
struct SatbThreadBuffer {
    SatbEntry entries[kBgcSatbBufferSize];
    std::atomic<uint32_t> count{0};
};

/// Index into the global SATB buffer pool (replaces 4KB thread_local buffer).
/// -1 = not yet allocated. Lazily allocated on first barrier hit.
extern thread_local int tls_satb_buffer_index;

/// Whether the current thread's SATB buffer is registered in BgcController.
extern thread_local bool tls_satb_registered;

// ======================================================================
// Per-worker state for BGC parallel mark (work-stealing deque).
//
// Each parallel mark worker (including the BGC thread as worker 0) has
// its own deque of grey-object pointers.  Workers push/pop from their
// own deque (under steal_mutex).  When empty, they steal from a random
// victim's deque front.  See P1-1 for full design.
// ======================================================================

/// Batch size for popping grey objects from a worker's deque.
static constexpr int kBgcPopBatchSize = 32;

/// Per-worker state for BGC parallel mark with steal support.
struct BgcMarkWorkerState {
    std::vector<void*> deque;       ///< Local grey-object worklist
    std::mutex         steal_mutex; ///< Guards deque (push, pop, steal)
};

// ======================================================================
// BgcController — singleton managing BGC thread and state
// ======================================================================

class BgcController {
public:
    static BgcController& Instance() {
        static BgcController instance;
        return instance;
    }

    BgcController(const BgcController&) = delete;
    BgcController& operator=(const BgcController&) = delete;

    // ── Lifecycle ──────────────────────────────────────────────────

    /// Start the BGC background thread.  Safe to call multiple times.
    void Start();

    /// Signal the BGC thread to stop and join it.
    void Stop();

    // ── Phase queries ─────────────────────────────────────────────

    BgcPhase Phase() const noexcept {
        return phase_.load(std::memory_order_acquire);
    }

    /// True during concurrent mark or sweep (mutators should activate barrier).
    bool IsBusy() const noexcept {
        auto p = phase_.load(std::memory_order_acquire);
        return p == BgcPhase::CONCURRENT_MARK ||
               p == BgcPhase::CONCURRENT_SWEEP ||
               p == BgcPhase::ROOT_COLLECT;
    }

    /// True when the SATB pre-write barrier should be active.
    bool IsMarking() const noexcept {
        return phase_.load(std::memory_order_acquire) == BgcPhase::CONCURRENT_MARK;
    }

    /// True when concurrent mark has completed and STW re-mark is needed.
    bool IsRemarkNeeded() const noexcept {
        return phase_.load(std::memory_order_acquire) == BgcPhase::REMARK_NEEDED;
    }

    /// True when concurrent sweep has completed and STW compaction is needed.
    bool IsCompactNeeded() const noexcept {
        return phase_.load(std::memory_order_acquire) == BgcPhase::COMPACT_NEEDED;
    }

    // ── SATB buffer flush ─────────────────────────────────────────

    /// Flush the calling thread's SATB buffer to the global queue.
    void FlushSatbBuffer(const SatbEntry* entries, uint32_t count);

    /// Register the calling thread's SATB buffer for cross-thread drain.
    /// Called automatically by the inline barrier on first hit.
    void RegisterThreadSatbBuffer(SatbThreadBuffer* buf);

    /// Allocate a SATB buffer from the global pool for the current thread.
    /// Returns the pool index (≥ 0) on success, or -1 on OOM.
    int AllocateSatbBuffer();

    /// Access a SATB buffer by pool index (for inline barrier fast path).
    SatbThreadBuffer& GetSatbBuffer(int index) {
        return satb_pool_[index];
    }

    // ── BGC cycle control (called from scheduler) ─────────────────

    /// Start a BGC cycle:
    ///   1. Under STW safepoint: populate root set
    ///   2. Signal BGC thread to begin concurrent mark
    /// The scheduler calls this when DecideCollection() == FULL_BGC.
    void StartBgcCycle();

    /// Perform the STW re-mark pause.
    /// Must be called under a safepoint.  Drains all thread-local SATB
    /// buffers, scans dirty cards, and drains the mark stack to completion.
    /// Returns the number of objects (re-)marked.
    CHAOS_IL2CPP_SIZE StwRemark();

    /// Signal BGC thread to begin concurrent sweep after re-mark.
    void StartConcurrentSweep();

    /// Perform STW compaction after concurrent sweep.
    /// Must be called under a safepoint.  Reuses the mark bitmap left
    /// intact by BgcSweep() to decide fragmentation and plan relocation.
    void StwCompact();

    /// Wait for the BGC cycle to complete (busy-wait with yield).
    /// Called when an emergency full STW GC is needed.
    void WaitForCycleComplete();

    /// Forcefully complete any in-progress BGC cycle (under safepoint).
    /// Drains mark stack + SATB + sweeps immediately.
    void ForceComplete();

    /// Collect dead weak handles into bgc_dead_weak_handles_ (post-BgcSweep).
    void CollectDeadWeakHandlesForBgc();

private:
    BgcController() = default;
    ~BgcController() = default;

    /// Populate the initial root set (under safepoint).
    /// Scans pinned roots, thread stacks, TLS nurseries, and GCHandles.
    void PopulateRootSet();

    /// Background thread entry point.
    void BgcThreadMain();

    /// Parallel worker entry point for concurrent mark phase.
    void BgcWorkerMain(int worker_idx);

    /// Process one grey object: scan its GC layout and mark all references.
    /// Pushes newly-marked children to worker 0's deque.
    void ProcessGreyObject(void* obj);

    /// Drain the global SATB queue: for each entry, if the object is in
    /// old-gen and not yet marked, push to worker 0's deque.
    CHAOS_IL2CPP_SIZE DrainGlobalSatbQueue();

    /// Drain all thread-local SATB buffers into the global queue.
    /// SAFE ONLY UNDER SAFEPOINT (threads are stopped).
    CHAOS_IL2CPP_SIZE DrainAllTlsSatbBuffers();

    /// Spawn parallel mark workers during concurrent mark phase.
    /// Returns the number of worker threads spawned.
    int SpawnParallelMarkWorkers();

    /// Signal parallel workers to stop and join them.
    void StopParallelMarkWorkers();

    // ── State ─────────────────────────────────────────────────────

    std::thread bgc_thread_;
    std::atomic<BgcPhase> phase_{BgcPhase::IDLE};
    std::atomic<bool> bgc_running_{false};
    std::atomic<bool> bgc_start_requested_{false};

    // Condition variable for event-driven BGC thread wake-up.
    // Replaces sleep_for polling in BgcThreadMain.
    std::condition_variable bgc_cv_;
    std::mutex bgc_cv_mutex_;

    /// Wake the BGC thread (or parallel workers) from sleep.
    void NotifyBgc() { bgc_cv_.notify_all(); }

    // Per-worker mark deques (concurrent mark uses per-worker deques
    // with steal support, not a shared mark stack).
    // Worker 0 = BGC thread; workers 1..N = parallel mark workers.
    static constexpr int kMaxBgcWorkers = 8;
    BgcMarkWorkerState bgc_workers_[kMaxBgcWorkers]{};
    std::atomic<int> bgc_worker_count_{0};

    // Global SATB queue (accumulated from thread-local buffer flushes).
    std::vector<SatbEntry> global_satb_;
    std::mutex global_satb_mutex_;

    // Completion signal.
    std::atomic<bool> cycle_complete_{false};

    // Thread-local SATB buffer registry (for cross-thread drain during STW).
    // Threads register their tls_satb_buffer address on first barrier hit.
    static constexpr int kMaxSatbThreads = 256;
    SatbThreadBuffer* registered_satb_buffers_[kMaxSatbThreads]{};
    int registered_satb_count_{0};
    std::mutex satb_registry_mutex_;

    // Global SATB buffer pool (heap-allocated, indexed by tls_satb_buffer_index).
    // Replaces the thread_local SatbThreadBuffer (~4KB) with a TLS int index
    // (~4 bytes) for iOS TLS-constrained platforms.
    static constexpr int kMaxSatbPool = 256;
    SatbThreadBuffer satb_pool_[kMaxSatbPool]{};
    std::atomic<int> satb_pool_alloc_{0};

    // ── Parallel mark workers ─────────────────────────────────────

    /// Drain up to @a batch_limit entries from worker @a idx's deque.
    /// Processes each entry and pushes newly-marked children to the same deque.
    /// @param idx        Worker index (0 = BGC thread).
    /// @param batch_limit  Max entries to process (0 = drain all).
    /// @returns Number of entries processed.
    CHAOS_IL2CPP_SIZE DrainWorkerDeque(int idx, CHAOS_IL2CPP_SIZE batch_limit = 0);

    /// Flag: set by BGC thread to signal parallel workers to stop.
    std::atomic<bool> bgc_parallel_done_{false};

    /// Worker threads spawned during concurrent mark.
    std::vector<std::thread> bgc_parallel_workers_;

    // ── SATB freeze protocol (CoreCLR-aligned convergence) ─────────

    /// Set by the BGC thread when concurrent mark is near convergence.
    /// Mutator SATB pre-write barriers check this flag and when set,
    /// flush their current buffer and decrement satb_freeze_remaining_.
    /// This ensures no new SATB entries are generated during the final
    /// drain, guaranteeing convergence.
public:
    /// Set by the BGC thread when concurrent mark is near convergence.
    /// Mutator SATB pre-write barriers check this flag and when set,
    /// flush their current buffer and decrement satb_freeze_remaining_.
    /// This ensures no new SATB entries are generated during the final
    /// drain, guaranteeing convergence.
    std::atomic<bool> satb_freeze_requested_{false};

    /// Number of mutator threads still needing to acknowledge the freeze.
    /// Initialized to the count of registered SATB threads when freeze
    /// is requested.  Each mutator decrements after flushing.
    std::atomic<int> satb_freeze_remaining_{0};

private:
    // ── BGC finalization support ──────────────────────────────────

    /// Dead finalizable entries collected during BgcSweep.
    /// Uses namespace-level FinalizerEntry (defined in gc_old_gen.h).
    std::vector<FinalizerEntry> bgc_dead_finalizables_;

    // ── BGC weak handle support ───────────────────────────────────

    /// Handle ID + original object for weak handles that need nulling.
    /// Collected during BgcSweep (mark bitmap still valid), nulled after
    /// finalization (respecting WeakTrackResurrection semantics).
    struct DeadWeakHandle {
        uint64_t handle_id;
        void*    old_object;  // for debugging / resurrection check
    };
    std::vector<DeadWeakHandle> bgc_dead_weak_handles_;

    // ── Dedicated finalizer thread ─────────────────────────────────

    /// Background thread that runs finalizers collected by BGC.
    /// Runs in preemptive mode so it can allocate without blocking.

    /// Per-finalizer timeout before marking for retry (2 seconds, matching CoreCLR).
    static constexpr int kFinalizerTimeoutMs = 2000;

    /// Maximum retries before permanently skipping a hung finalizer.
    static constexpr int kFinalizerMaxRetries = 3;

    /// Finalizer thread heartbeat timeout: if CV wait exceeds this without
    /// work, log a diagnostic (thread may be hung).
    static constexpr int kFinalizerHeartbeatMs = 10000;

    std::thread finalizer_thread_;
    std::condition_variable finalizer_cv_;
    std::mutex finalizer_mutex_;
    std::vector<TimedFinalizerEntry> pending_timed_finalizers_;
    std::vector<DeadWeakHandle> pending_weak_handles_;
    std::atomic<bool> finalizer_work_pending_{false};
    std::atomic<bool> finalizer_running_{false};

    /// Publish finalization work to the finalizer thread and signal it.
    void PublishFinalizationWork(
        std::vector<FinalizerEntry>& finalizers,
        std::vector<DeadWeakHandle>& weak_handles) noexcept;

    /// Finalizer thread entry point.
    void FinalizerThreadMain() noexcept;
};

// ======================================================================
// Global flag — checked by the inline pre-write barrier
// ======================================================================

extern std::atomic<bool> g_bgc_is_marking;

/// Flush a thread's SATB buffer to the global queue (delegates to controller).
extern void BgcFlushSatbBuffer(const SatbEntry* entries, uint32_t count);

extern void GcAdvanceBgcCycle() noexcept;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_BGC_H_
