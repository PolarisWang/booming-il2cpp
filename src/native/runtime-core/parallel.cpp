// parallel.cpp — Phase 6 / E2: Parallel.For range partitioner.
//
// Partitions [from, to) across the ThreadPool via atomic work-stealing.
// Each worker chunk invokes the delegate's native method_ptr directly
// (through chaos_delegate_object_invoke), NOT through the interpreter.
//
// WHY THIS IS SAFE
// ----------------
// Parallel.For is a data-parallel construct with NO managed-side state
// beyond the range and the delegate.  The delegate IS a DelegateObject
// whose method_ptr we call directly — the same path Task.Run uses.
// There is no AsyncLocal, no ExecutionContext, no cancellation to wire;
// those are Parallel.ForEachAsync, which is a separate overload.

#include "parallel.h"
#include "thread_pool.h"
#include "core/delegate_helpers.h"

#include <atomic>
#include <cstdint>
#include <new>
#include <thread>

using namespace chaos::il2cpp::runtime_core::threading;

namespace chaos::il2cpp::runtime_core::parallel {

namespace {

/// Shared state for one Parallel.For invocation.
///
/// `remaining` counts CHUNKS, not workers, and that distinction is the whole
/// point.  An earlier implementation seeded it with the worker count and had
/// each worker decrement once on exit — but a worker drains as many chunks as
/// it can claim, so the number of decrements was not the number of chunks.
/// The caller spins on this counter, so any mismatch either hangs it (too few
/// decrements) or frees the state from under a running worker (too many).
struct ForRangeState {
    std::atomic<CHAOS_IL2CPP_INT32> next_index;    // next unclaimed index
    std::atomic<CHAOS_IL2CPP_INT32> remaining;     // chunks still to complete
    CHAOS_IL2CPP_INT32 chunk_span;                 // indices per claim
    CHAOS_IL2CPP_INT32 to_exclusive;               // exclusive upper bound
    CHAOS_IL2CPP_INTPTR action_delegate;           // DelegateObject*
};

constexpr CHAOS_IL2CPP_INT32 kMinChunkSize = 32;             // floor for chunk span
constexpr CHAOS_IL2CPP_INT32 kMaxChunksDispatched = 64;      // work items per call
constexpr CHAOS_IL2CPP_INT32 kMaxClaimAttempts = 4;          // stale-claim bound

/// Indices each claim covers, for a range of `count` iterations dispatched as
/// `chunks` work items.
///
/// The span MUST satisfy `chunks * span >= count`, or the tail of the range is
/// never claimable and those iterations silently never run.  A fixed span of 32
/// with a capped dispatch count breaks that: capping caps the product.
///
/// Why this hid for so long: each worker may claim up to kMaxClaimAttempts
/// chunks, so with a fixed span the reachable index space was
/// `chunks * kMaxClaimAttempts * span` — which for a 4096 range (64 x 4 x 32 =
/// 8192) happens to EXCEED the range.  Coverage then depended on whether the
/// claim budget was spent in the right order, so the same call would cover
/// different amounts on different runs (measured 2197..2454 of 4096 across
/// runs, never once the full range).  Deriving the span from the dispatch count
/// makes coverage hold by construction rather than by budget arithmetic.
CHAOS_IL2CPP_INT32 ChunkSpanFor(CHAOS_IL2CPP_INT32 count,
                                CHAOS_IL2CPP_INT32 chunks) noexcept {
    if (chunks < 1) chunks = 1;
    // Ceiling division: every index must fall inside some chunk.
    CHAOS_IL2CPP_INT32 span = (count + chunks - 1) / chunks;
    if (span < kMinChunkSize) span = kMinChunkSize;
    return span;
}

/// Work items to dispatch for a range of `count` iterations.
///
/// Target ~4 per hardware thread: enough granularity for the pool to balance,
/// few enough that a small pool does not pay a thread creation per chunk.
CHAOS_IL2CPP_INT32 ChunksFor(CHAOS_IL2CPP_INT32 count) noexcept {
    CHAOS_IL2CPP_INT32 chunks = (count + kMinChunkSize - 1) / kMinChunkSize;

    int32_t hw = static_cast<int32_t>(std::thread::hardware_concurrency());
    if (hw < 1) hw = 1;
    const CHAOS_IL2CPP_INT32 target = hw * 4;

    if (chunks > target) chunks = target;
    if (chunks > kMaxChunksDispatched) chunks = kMaxChunksDispatched;
    if (chunks < 1) chunks = 1;
    return chunks;
}

/// Worker callback: claim chunks and execute them until the range is drained.
///
/// A worker keeps claiming until it hits the end of the range or exhausts its
/// claim budget.  The budget exists so that one fast worker cannot serially
/// drain the whole range while its peers sit idle — without it, a wide range
/// degrades toward single-threaded execution.
void ForRangeWorker(void* state) noexcept {
    auto* fs = static_cast<ForRangeState*>(state);
    if (fs == nullptr) return;

    const CHAOS_IL2CPP_INT32 span = fs->chunk_span;

    CHAOS_IL2CPP_INT32 claims = kMaxClaimAttempts;
    while (claims-- > 0) {
        const CHAOS_IL2CPP_INT32 start =
            fs->next_index.fetch_add(span, std::memory_order_acq_rel);
        if (start >= fs->to_exclusive) break;  // range drained

        CHAOS_IL2CPP_INT32 end = start + span;
        if (end > fs->to_exclusive) end = fs->to_exclusive;

        for (CHAOS_IL2CPP_INT32 i = start; i < end; ++i) {
            // Call the delegate directly — chaos_delegate_object_invoke
            // interprets the DelegateObject layout and calls method_ptr.
            // Args cross as a pointer-sized array (arg_count = 1).
            CHAOS_IL2CPP_INTPTR args[1];
            args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(i);
            chaos_delegate_object_invoke(fs->action_delegate, args, nullptr, 1);
        }

        // Release this chunk.  fetch_sub RETURNS the old value, so the thread
        // observing 1 here is the one that completed the last chunk.
        //
        // Note this sits AFTER the drained-`break` above, so only a CLAIMED
        // chunk decrements.  That ordering is load-bearing: if a worker that
        // found the range already drained also decremented, the counter would
        // pass zero and the caller — which reads zero as "everything finished"
        // — would free the state while its peers were still iterating it.
        // With the span sized to cover the range (see ChunkSpanFor), the
        // dispatched claims always supply at least `chunk_count` decrements,
        // so the caller's rendezvous still completes.
        if (fs->remaining.fetch_sub(1, std::memory_order_acq_rel) == 1) return;
    }
}

}  // anonymous namespace

}  // namespace chaos::il2cpp::runtime_core::parallel

// ── extern "C" entry point (what the codegen ShapeRegistry routes to) ──

CHAOS_IL2CPP_INTPTR chaos_parallel_for_range_int(
    CHAOS_IL2CPP_INT32 from, CHAOS_IL2CPP_INT32 to,
    CHAOS_IL2CPP_INTPTR action_delegate) noexcept
{
    using namespace chaos::il2cpp::runtime_core::parallel;

    if (action_delegate == 0) return -1;  // null delegate
    CHAOS_IL2CPP_INT32 count = to - from;
    if (count <= 0) return -1;  // empty or inverted range

    const CHAOS_IL2CPP_INT32 chunk_count = ChunksFor(count);
    const CHAOS_IL2CPP_INT32 chunk_span = ChunkSpanFor(count, chunk_count);

    auto* fs = new (std::nothrow) ForRangeState();
    if (fs == nullptr) return -1;

    fs->next_index.store(from, std::memory_order_relaxed);
    fs->remaining.store(chunk_count, std::memory_order_relaxed);
    fs->chunk_span = chunk_span;
    fs->to_exclusive = to;
    fs->action_delegate = action_delegate;

    // Dispatch one work item per chunk.  The ranges are disjoint, so only the
    // chunk count affects correctness — how many workers end up draining them,
    // and in what order, does not.  `chunk_span` is sized so that the chunks
    // dispatched here cover the WHOLE range; see ChunkSpanFor.
    for (CHAOS_IL2CPP_INT32 i = 0; i < chunk_count; ++i) {
        ThreadPoolQueueUserWorkItemUnsafe(ForRangeWorker, fs);
    }

    // Rendezvous on the chunk counter.  Every chunk exactly once decrements it,
    // so this terminates.
    //
    // The hint must DESCHEDULE, not merely stall the pipeline.  This spin waits
    // on work performed by OTHER threads, so a caller that only issues a
    // pipeline hint (CHAOS_IL2CPP_PAUSE_HINT → _mm_pause) holds a core at 100%
    // while the workers it just dispatched need cores of their own — exactly
    // the inversion that lets a parallel loop run slower than the serial one.
    // std::this_thread::yield() gives up the timeslice instead.
    //
    // If this runs on a POOL WORKER (nested Parallel.For inside another
    // Parallel body, or a Task.Run body), it blocks that worker for the
    // duration.  That is deliberate and is what real Parallel.For does:
    // work-stealing lets the blocked worker's queue be drained by others, so
    // the nested calls are the concurrency rather than a deadlock.  It is only
    // a deadlock if every worker nests at once, which needs a fan-out wider
    // than the pool.
    do {
        if (fs->remaining.load(std::memory_order_acquire) == 0) break;
        std::this_thread::yield();
    } while (true);

    delete fs;
    return -1;  // completed without break
}