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

using namespace chaos::il2cpp::runtime_core::threading;

namespace chaos::il2cpp::runtime_core::parallel {

namespace {

/// Shared state for one Parallel.For invocation.
struct ForRangeState {
    std::atomic<CHAOS_IL2CPP_INT32> next_index;  // next unclaimed index
    CHAOS_IL2CPP_INT32 to_exclusive;              // exclusive upper bound
    CHAOS_IL2CPP_INTPTR action_delegate;          // DelegateObject*
    std::atomic<CHAOS_IL2CPP_INT32> remaining;    // chunks left to complete
    bool failed = false;
};

constexpr CHAOS_IL2CPP_INT32 kChunkSize = 32;  // iterations per worker chunk

/// Worker callback: claim a chunk and execute it.
void ForRangeWorker(void* state) noexcept {
    auto* fs = static_cast<ForRangeState*>(state);
    if (fs == nullptr) return;

    while (true) {
        CHAOS_IL2CPP_INT32 start = fs->next_index.fetch_add(kChunkSize, std::memory_order_acq_rel);
        if (start >= fs->to_exclusive) break;

        CHAOS_IL2CPP_INT32 end = start + kChunkSize;
        if (end > fs->to_exclusive) end = fs->to_exclusive;

        for (CHAOS_IL2CPP_INT32 i = start; i < end; ++i) {
            // Call the delegate directly — chaos_delegate_object_invoke
            // interprets the DelegateObject layout and calls method_ptr.
            // Args cross as a pointer-sized array (arg_count = 1).
            CHAOS_IL2CPP_INTPTR args[1];
            args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(i);
            chaos_delegate_object_invoke(fs->action_delegate, args, nullptr, 1);
        }
    }

    CHAOS_IL2CPP_INT32 left = fs->remaining.fetch_sub(1, std::memory_order_acq_rel);
    if (left <= 1) {
        // Last worker to finish — nothing to signal; the caller is polling.
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
    if (count <= 0) return -1;  // empty range

    // Number of worker threads: bounded by the number of chunks.
    CHAOS_IL2CPP_INT32 total_chunks = (count + kChunkSize - 1) / kChunkSize;
    CHAOS_IL2CPP_INT32 worker_count = total_chunks;

    auto* fs = new (std::nothrow) ForRangeState();
    if (fs == nullptr) return -1;

    fs->next_index.store(from, std::memory_order_relaxed);
    fs->to_exclusive = to;
    fs->action_delegate = action_delegate;
    fs->remaining.store(worker_count, std::memory_order_relaxed);
    fs->failed = false;

    // Enqueue chunks on the ThreadPool (fire-and-forget, no EC capture).
    for (CHAOS_IL2CPP_INT32 i = 0; i < worker_count; ++i) {
        using namespace chaos::il2cpp::runtime_core::threading;
        ThreadPoolQueueUserWorkItemUnsafe(ForRangeWorker, fs);
    }

    // The calling thread does NOT block here (ThreadPoolQueueUserWorkItemUnsafe
    // can complete synchronously for an already-ready worker).  Instead we
    // rendezvous on `remaining`: each spawned worker decrements it on exit.
    // This is a spin-wait because the wait is bounded (typically < 1ms).
    while (fs->remaining.load(std::memory_order_acquire) > 0) {
        // Yield to let workers run.  On Windows this switches to another
        // thread; on other platforms it's a hint.
        CHAOS_IL2CPP_PAUSE_HINT();
    }

    CHAOS_IL2CPP_INTPTR result = fs->failed ? static_cast<CHAOS_IL2CPP_INTPTR>(0) : static_cast<CHAOS_IL2CPP_INTPTR>(-1);
    delete fs;
    return result;
}