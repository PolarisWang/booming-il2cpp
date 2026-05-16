// parallel_stubs.cpp — System.Threading.Tasks.Parallel runtime stubs
//
// Provides native implementations for Parallel.For and Parallel.Invoke,
// partitioning work across thread pool workers with an atomic completion
// counter for barrier synchronization.
//
// ABI exports: extern "C" linkage for managed/NativeAOT callability.
#include "parallel_stubs.h"
#include "thread_pool.h"
#include "bootstrap/bootstrap.h"

#include <atomic>
#include <cstdlib>
#include <new>

namespace chaos::il2cpp::runtime_core {
extern "C" {

// ── Parallel.For context ─────────────────────────────────────────────────

struct ParallelForChunk {
    int32_t from;
    int32_t to;
    CHAOS_IL2CPP_INTPTR body_delegate;
    std::atomic<int32_t>* completed;
};

static void ParallelForWorker(void* state) noexcept {
    auto* chunk = static_cast<ParallelForChunk*>(state);
    auto* bridge = chaos::il2cpp::bootstrap::GetCodegenBridgeV0();

    for (int32_t i = chunk->from; i < chunk->to; i++) {
        void* return_value = nullptr;
        bridge->delegate_invoke(
            nullptr, nullptr,
            reinterpret_cast<void*>(chunk->body_delegate),
            reinterpret_cast<void* const*>(&i), sizeof(int32_t),
            &return_value, sizeof(void*),
            nullptr);
    }

    chunk->completed->fetch_add(1, std::memory_order_release);
}

CHAOS_IL2CPP_INT32 ChaosParallelFor(
    CHAOS_IL2CPP_INT32 from_inclusive,
    CHAOS_IL2CPP_INT32 to_exclusive,
    CHAOS_IL2CPP_INTPTR body_delegate) noexcept
{
    if (body_delegate == 0) return -1;
    if (to_exclusive <= from_inclusive) return 0;

    const int32_t total = to_exclusive - from_inclusive;
    const int32_t worker_count = threading::ThreadPoolWorkerCount();
    const int32_t num_chunks = (worker_count > 1) ? worker_count : 1;
    const int32_t chunk_size = total / num_chunks;
    const int32_t remainder = total % num_chunks;

    std::atomic<int32_t> completed{0};

    // Allocate chunk contexts on the heap (owned by the thread pool workers).
    int32_t current = from_inclusive;
    for (int32_t i = 0; i < num_chunks; i++) {
        int32_t chunk_end = current + chunk_size + (i < remainder ? 1 : 0);
        if (chunk_end <= current) continue;

        auto* chunk = new (std::nothrow) ParallelForChunk{
            current, chunk_end, body_delegate, &completed};
        if (chunk == nullptr) continue;

        threading::ThreadPoolQueueUserWorkItem(ParallelForWorker, chunk);
        current = chunk_end;
    }

    // Wait for all chunks to complete.
    while (completed.load(std::memory_order_acquire) < num_chunks) {
        std::this_thread::yield();
    }

    return 0;
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core
