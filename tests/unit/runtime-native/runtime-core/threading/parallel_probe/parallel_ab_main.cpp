// A/B benchmark harness for Parallel.For dispatch policy (T1.4).
//
// Compiled TWICE against two different parallel.cpp revisions so the two
// policies are measured in separate processes with their own freshly-built
// objects.  Timing two policies inside one binary would let the first run warm
// the pool for the second, which is exactly the confound this is meant to
// avoid.
//
// Reports, per range size:
//   - wall time for a single call on a warmed pool
//   - the thread-pool worker count before and after that call
//
// The worker delta is the headline: a dispatch policy that emits one work item
// per chunk makes the pool create a worker per item, so the caller pays thread
// creations it did not ask for.
#include <cstdio>
#include <chrono>

#include "parallel.h"
#include "thread_pool.h"
#include "core/delegate_object.h"

using chaos::il2cpp::runtime_core::DelegateObject;
using namespace chaos::il2cpp::runtime_core::threading;

static CHAOS_IL2CPP_INTPTR Body(CHAOS_IL2CPP_INTPTR) { return 0; }

/// Progress markers go to stderr unbuffered.  stdout is block-buffered when
/// redirected, so a crash loses every printf that has not filled a buffer —
/// which is exactly the situation this harness needs to distinguish (did the
/// crash happen before the first call, or inside it?).
static void Mark(const char* what) {
    fprintf(stderr, "[AB] %s\n", what);
    fflush(stderr);
}

/// Run one Parallel.For and report wall time + worker delta.
static void Measure(const char* label, CHAOS_IL2CPP_INT32 from, CHAOS_IL2CPP_INT32 to,
                    DelegateObject* d, int reps = 5) {
    long long best_ms = -1;
    int32_t w_before = 0, w_after = 0;

    for (int r = 0; r < reps; ++r) {
        const int32_t before = ThreadPoolWorkerCount();
        const auto t0 = std::chrono::steady_clock::now();
        (void)chaos_parallel_for_range_int(from, to, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(d));
        const auto t1 = std::chrono::steady_clock::now();
        const long long ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
        if (best_ms < 0 || ms < best_ms) best_ms = ms;
        if (r == 0) { w_before = before; w_after = ThreadPoolWorkerCount(); }
    }
    printf("%-24s best=%4lldms   workers %d -> %d\n",
           label, best_ms, (int)w_before, (int)w_after);
}

int main() {
    Mark("start");
    auto* d = new DelegateObject();
    d->type_info = nullptr;
    d->chaos_delegate_target = 0;
    d->chaos_delegate_method_ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(Body);
    d->chaos_delegate_invocation_list = 0;
    d->chaos_delegate_invocation_count = 0;
    d->chaos_delegate_method_token = 0;
    d->_pad = 0;
    Mark("delegate built");

    ThreadPoolInitialize();
    printf("workers after init = %d\n", ThreadPoolWorkerCount());
    fflush(stdout);

    // Warm up so the measurement is steady-state, not the cold ramp.  50 calls
    // of a small range is enough to settle the pool without skewing the deltas
    // reported below (those come from the FIRST measured call of each size).
    Mark("warmup begin");
    for (int i = 0; i < 50; ++i) {
        (void)chaos_parallel_for_range_int(0, 256, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(d));
    }
    Mark("warmup done");
    const int32_t warm = ThreadPoolWorkerCount();
    printf("workers after warmup = %d\n\n", (int)warm);
    fflush(stdout);

    Mark("measure begin");
    Measure("range 0..1000",   0, 1000,  d);
    Measure("range 0..10000",  0, 10000, d);
    Measure("range 0..100000", 0, 100000, d);
    Measure("range 0..31 (1 chunk)", 0, 31, d);
    Mark("measure done");

    printf("\nworkers at exit = %d\n", (int)ThreadPoolWorkerCount());
    fflush(stdout);

    delete d;
    ThreadPoolShutdown();
    return 0;
}
