// Does the Parallel.For caller return before the chunks it dispatched have
// finished?
//
// The sanity probe showed the caller returning with only ~2300 of 4096 indices
// visited, and adding a sleep after the call turned that into a SEGFAULT --
// which is what a use-after-free looks like when the still-running workers
// touch a ForRangeState the caller already deleted.
//
// This probe answers the question without printf in the hot loop (printf would
// serialize the workers and hide the race).  It brackets the call: a flag the
// workers set on entry, and an atomic count of chunks that are still in
// flight.  If, at the instant the caller returns, workers have already claimed
// chunks whose bodies have not all completed, the rendezvous is broken.
#include <cstdio>
#include <atomic>
#include <chrono>
#include <thread>

#include <core/delegate_object.h>
#include "parallel.h"
#include "thread_pool.h"

using namespace chaos::il2cpp::runtime_core::threading;
using chaos::il2cpp::runtime_core::DelegateObject;

// Declared locally: the probe needs only the ABI signature, and this keeps it
// independent of header-path resolution in the standalone project.
extern "C" void chaos_delegate_object_invoke(
    CHAOS_IL2CPP_INTPTR delegate_ptr,
    CHAOS_IL2CPP_INTPTR* args_buf,
    CHAOS_IL2CPP_INTPTR* ret_buf,
    CHAOS_IL2CPP_UINT32 arg_count) noexcept;

/// Worker body.  Sleeps briefly so that a chunk is still in flight when the
/// caller decides to return, if the rendezvous is not actually waiting for it.
static std::atomic<long long> g_completed_chunks{0};
static std::atomic<long long> g_started_chunks{0};

static CHAOS_IL2CPP_INTPTR SlowBody(CHAOS_IL2CPP_INTPTR arg) {
    // 1 ms per body call: 32 calls per chunk means a chunk takes ~32 ms, far
    // longer than any of the bookkeeping around it.
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    g_completed_chunks.fetch_add(1, std::memory_order_relaxed);
    (void)arg;
    return 0;
}

int main() {
    ThreadPoolInitialize();

    auto* d = new DelegateObject();
    d->type_info = nullptr;
    d->chaos_delegate_target = 0;
    d->chaos_delegate_method_ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(SlowBody);
    d->chaos_delegate_invocation_list = 0;
    d->chaos_delegate_invocation_count = 0;
    d->chaos_delegate_method_token = 0;
    d->_pad = 0;

    g_completed_chunks.store(0);

    // A range that yields several chunks, so more than one worker is involved.
    const auto t0 = std::chrono::steady_clock::now();
    chaos_parallel_for_range_int(0, 256, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(d));
    const auto t1 = std::chrono::steady_clock::now();
    const long long at_return = g_completed_chunks.load();

    std::this_thread::sleep_for(std::chrono::seconds(3));
    const long long after_settle = g_completed_chunks.load();

    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    fprintf(stderr,
            "[RACE] range 0..256 (expect 256 calls)\n"
            "[RACE]   caller returned after %lld ms with %lld calls done\n"
            "[RACE]   after a 3 s settle the count is %lld\n"
            "[RACE]   VERDICT: %s\n",
            (long long)ms, at_return, after_settle,
            at_return == 256 ? "rendezvous correct"
                             : "CALLER RETURNED EARLY -- work still running at return");

    (void)g_started_chunks;
    delete d;
    ThreadPoolShutdown();
    return 0;
}
