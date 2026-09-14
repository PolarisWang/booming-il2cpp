// Instrumented copy of the post-fix parallel.cpp used ONLY by the crash
// reproducer.  It is the fixed source plus stderr counters on the two events
// whose populations the truncation hypothesis turns on:
//
//   - chunks CLAIMED (a worker successfully took a chunk range)
//   - chunks RELEASED (the rendezvous counter decremented)
//
// If claims == releases but the body ran fewer times than the range size, the
// loss is inside the body invocation.  If claims < expected chunks, the
// dispatch loop never produced them.  If the caller returns with releases <
// chunks dispatched, the rendezvous itself is broken.
#include "parallel.h"
#include "thread_pool.h"
#include "core/delegate_helpers.h"

#include <atomic>
#include <cstdint>
#include <cstdio>
#include <new>
#include <thread>

using namespace chaos::il2cpp::runtime_core::threading;

namespace chaos::il2cpp::runtime_core::parallel {

namespace {

struct ForRangeState {
    std::atomic<CHAOS_IL2CPP_INT32> next_index;
    std::atomic<CHAOS_IL2CPP_INT32> remaining;
    CHAOS_IL2CPP_INT32 to_exclusive;
    CHAOS_IL2CPP_INTPTR action_delegate;
};

constexpr CHAOS_IL2CPP_INT32 kChunkSize = 32;
constexpr CHAOS_IL2CPP_INT32 kMaxChunksDispatched = 64;
constexpr CHAOS_IL2CPP_INT32 kMaxClaimAttempts = 4;

CHAOS_IL2CPP_INT32 ChunksFor(CHAOS_IL2CPP_INT32 count) noexcept {
    CHAOS_IL2CPP_INT32 chunks = (count + kChunkSize - 1) / kChunkSize;
    int32_t hw = static_cast<int32_t>(std::thread::hardware_concurrency());
    if (hw < 1) hw = 1;
    const CHAOS_IL2CPP_INT32 target = hw * 4;
    if (chunks > target) chunks = target;
    if (chunks > kMaxChunksDispatched) chunks = kMaxChunksDispatched;
    if (chunks < 1) chunks = 1;
    return chunks;
}

void ForRangeWorker(void* state) noexcept {
    auto* fs = static_cast<ForRangeState*>(state);
    if (fs == nullptr) return;

    CHAOS_IL2CPP_INT32 claims = kMaxClaimAttempts;
    while (claims-- > 0) {
        const CHAOS_IL2CPP_INT32 start =
            fs->next_index.fetch_add(kChunkSize, std::memory_order_acq_rel);
        if (start >= fs->to_exclusive) {
            fprintf(stderr, "[INST] worker exits: drained (start=%d to=%d)\n",
                    (int)start, (int)fs->to_exclusive);
            return;
        }

        CHAOS_IL2CPP_INT32 end = start + kChunkSize;
        if (end > fs->to_exclusive) end = fs->to_exclusive;

        fprintf(stderr, "[INST] claim [%d,%d)\n", (int)start, (int)end);
        for (CHAOS_IL2CPP_INT32 i = start; i < end; ++i) {
            CHAOS_IL2CPP_INTPTR args[1];
            args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(i);
            // Log the index immediately BEFORE and AFTER the invoke.  A pair
            // with no "after" means the invoke did not return; a pair with an
            // "after" but no body effect means the invoke returned without
            // running the body.
            fprintf(stderr, "[INST]   invoke %d ->\n", (int)i);
            chaos_delegate_object_invoke(fs->action_delegate, args, nullptr, 1);
            fprintf(stderr, "[INST]   invoke %d <-\n", (int)i);
        }

        if (fs->remaining.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            fprintf(stderr, "[INST] last chunk released, worker returning\n");
            return;
        }
    }
    fprintf(stderr, "[INST] worker exits: claim budget exhausted (remaining=%d)\n",
            (int)fs->remaining.load());
}

}  // anonymous namespace

}  // namespace chaos::il2cpp::runtime_core::parallel

CHAOS_IL2CPP_INTPTR chaos_parallel_for_range_int(
    CHAOS_IL2CPP_INT32 from, CHAOS_IL2CPP_INT32 to,
    CHAOS_IL2CPP_INTPTR action_delegate) noexcept
{
    using namespace chaos::il2cpp::runtime_core::parallel;

    if (action_delegate == 0) return -1;
    CHAOS_IL2CPP_INT32 count = to - from;
    if (count <= 0) return -1;

    const CHAOS_IL2CPP_INT32 chunk_count = ChunksFor(count);

    auto* fs = new (std::nothrow) ForRangeState();
    if (fs == nullptr) return -1;

    fs->next_index.store(from, std::memory_order_relaxed);
    fs->remaining.store(chunk_count, std::memory_order_relaxed);
    fs->to_exclusive = to;
    fs->action_delegate = action_delegate;

    fprintf(stderr, "[INST] dispatch: range [%d,%d) count=%d chunk_count=%d\n",
            (int)from, (int)to, (int)count, (int)chunk_count);
    fflush(stderr);

    for (CHAOS_IL2CPP_INT32 i = 0; i < chunk_count; ++i) {
        ThreadPoolQueueUserWorkItemUnsafe(ForRangeWorker, fs);
    }

    do {
        if (fs->remaining.load(std::memory_order_acquire) == 0) break;
        std::this_thread::yield();
    } while (true);

    delete fs;
    return -1;
}
