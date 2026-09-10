#ifndef CHAOS_IL2CPP_GC_PARALLEL_MARK_H_
#define CHAOS_IL2CPP_GC_PARALLEL_MARK_H_

#include <chaos/native_types.h>

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <vector>

#include "gc_old_gen.h"

namespace chaos::il2cpp::runtime_core {

// ======================================================================
// Parallel Mark Framework — chunked work-stealing mark for full GC
//
// == Design ==
// CoreCLR-aligned parallel mark using chunked mark entries instead of
// individual object pointers.  Each mark entry is a (page_idx, bitmap_word)
// pair representing up to 64 objects on the same page — 64× less deque
// traffic vs individual object pointers.
//
// == Workers ==
// - Worker 0 = the GC thread (always participates)
// - Workers 1..N = thread-pool workers
// - Worker count = min(pages/32 + 1, hardware_concurrency, 8)
//
// == Work-stealing ==
// Each worker has a mutex-protected deque.  Workers push to their own
// deque (lock-free local), steal from random victims when idle.
//
// == Termination ==
// Distributed active_workers count + global done flag.
// ======================================================================

/// Maximum parallel workers (bound to prevent oversubscription).
/// Runtime-latchable from GcConfig().ParallelMarkWorkers during GC init so the
/// CHAOS_GC_ParallelMarkWorkers knob drives the real mark worker count.
inline int kMaxParallelMarkWorkers = 8;

/// Chunk representing up to 64 objects on the same page.
struct MarkChunk {
    int      page_idx;    ///< Index into the page array
    uint16_t word_index;  ///< Which 64-slot group this bitmap covers (0 = slots 0-63)
    uint64_t bitmap;      ///< Bitmap word: each set bit = one object to process
};

/// Per-worker state for parallel mark.
struct MarkWorkerState {
    /// Local work-stealing deque of chunks.
    std::vector<MarkChunk> deque;

    /// Mutex protecting deque for steal operations.
    /// Local push/pop do NOT lock (single-producer).
    std::mutex steal_mutex;

    /// Pending chunk accumulator: when scanning an object, newly-marked
    /// children are accumulated here.  Flushed to deque when the page
    /// changes or the bitmap word saturates.
    MarkChunk pending;
    bool has_pending{false};

    /// Worker index (0 = GC thread).
    int worker_idx{0};

    MarkWorkerState() {
        std::memset(&pending, 0, sizeof(pending));
    }
};

/// Global parallel mark context, initialized before DrainMarkStackParallel.
struct ParallelMarkContext {
    /// Array of all old-gen pages (snapshotted under lock before parallel phase).
    OldGenPage** pages{nullptr};
    int          page_count{0};

    /// Address-sorted page start array for O(log n) page lookup.
    /// Each entry is the Payload() start address of pages[i].
    uintptr_t* page_starts{nullptr};

    /// Per-worker state (allocated on heap, one per worker).
    MarkWorkerState* workers{nullptr};
    int              worker_count{0};

    /// Active worker count (used for termination detection).
    std::atomic<int> active_workers{0};

    /// Total newly-marked count across all workers.
    std::atomic<uint64_t> total_marked{0};

    /// Set to true when root marking is done and parallel drain should start.
    std::atomic<bool> drain_started{false};

    /// Set to true when parallel mark is complete (termination signal).
    std::atomic<bool> parallel_done{false};

    /// 方案1: done/新-work 信号。降空 worker 由 last-worker 设 done 后 notify_all
    /// 唤醒退出; 生产者(ProcessChunk 推 chunk 溢出自己 deque 给他人 steal 时)也可
    /// notify。有界 wait_for 兜底, 不丢 work。
    std::mutex              mark_mtx_;
    std::condition_variable mark_cv_;
};

/// Initialize parallel mark context with a snapshot of pages.
/// @param pages     Array of OldGenPage pointers (must outlive the parallel phase).
/// @param page_count Number of pages.
/// @param hw_concurrency Hardware concurrency hint for worker count calculation.
/// @return The context (already initialized with workers), or nullptr on OOM.
ParallelMarkContext* InitParallelMarkContext(OldGenPage** pages, int page_count,
                                              int hw_concurrency);

/// Binary search for the page index containing @a addr.
/// Uses the page_starts array (sorted Payload() addresses).
/// Returns -1 if not found.
int FindPageIndexByAddr(ParallelMarkContext* ctx, uintptr_t addr);

/// Destroy parallel mark context and free resources.
void DestroyParallelMarkContext(ParallelMarkContext* ctx);

/// Push a MarkChunk to a worker's local deque (lock-free for local worker).
void PushChunk(MarkWorkerState* worker, const MarkChunk& chunk);

/// Try to pop a chunk from the local worker's deque (lock-free for local worker).
bool PopChunk(MarkWorkerState* worker, MarkChunk* out);

/// Try to steal a chunk from a random victim worker.
bool StealChunk(ParallelMarkContext* ctx, int thief_idx, MarkChunk* out);

/// Flush the pending chunk accumulator to the local deque.
void FlushPending(MarkWorkerState* worker);

/// Process a single MarkChunk: scan each set-bit object's pointer fields,
/// mark children, accumulate newly-marked children into the pending chunk.
void ProcessChunk(ParallelMarkContext* ctx, MarkWorkerState* worker,
                  const MarkChunk& chunk);

/// Worker entry point: steal-loop until all workers are idle and all deques empty.
void ParallelMarkWorkerLoop(ParallelMarkContext* ctx, int worker_idx);

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_PARALLEL_MARK_H_