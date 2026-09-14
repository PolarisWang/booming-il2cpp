// Minimal reproducer: does Parallel.For crash for a wide range, independent of
// the A/B dispatch policy?  Both binaries crashed at the first measured call
// (range 0..1000), after a warmup of 50 x range 0..256 succeeded.  This narrows
// which property of the call triggers it.
#include <cstdio>
#include <atomic>

#include "parallel.h"
#include "thread_pool.h"
#include "core/delegate_object.h"

using chaos::il2cpp::runtime_core::DelegateObject;
using namespace chaos::il2cpp::runtime_core::threading;

static std::atomic<long long> g_calls{0};
static std::atomic<long long> g_sum{0};

static const int kMax = 65536;
static std::atomic<unsigned char> g_seen[kMax];

static CHAOS_IL2CPP_INTPTR Body(CHAOS_IL2CPP_INTPTR arg) {
    const long long i = static_cast<long long>(arg);
    g_calls.fetch_add(1, std::memory_order_relaxed);
    g_sum.fetch_add(arg, std::memory_order_relaxed);
    if (i >= 0 && i < kMax) g_seen[i].store(1, std::memory_order_relaxed);
    return 0;
}

static void Run(const char* label, int from, int to) {
    fprintf(stderr, "[REPRO] %s: begin\n", label);
    fflush(stderr);
    g_calls.store(0);
    g_sum.store(0);
    for (int i = 0; i < kMax; ++i) g_seen[i].store(0);

    auto* d = new DelegateObject();
    d->type_info = nullptr;
    d->chaos_delegate_target = 0;
    d->chaos_delegate_method_ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(Body);
    d->chaos_delegate_invocation_list = 0;
    d->chaos_delegate_invocation_count = 0;
    d->chaos_delegate_method_token = 0;
    d->_pad = 0;

    chaos_parallel_for_range_int(from, to, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(d));

    const long long n = g_calls.load();
    const long long s = g_sum.load();
    const long long want = (long long)(from + to - 1) * (to - from) / 2;
    long long seen = 0;
    for (int i = from; i < to; ++i) {
        if (g_seen[i].load()) ++seen;
    }
    fprintf(stderr, "[REPRO] %s: done calls=%lld sum=%lld (want calls=%d sum=%lld) distinct=%lld\n",
            label, n, s, to - from, want, seen);
    fflush(stderr);
    delete d;
}

int main() {
    ThreadPoolInitialize();

    // Narrow to the single case that truncates.  A 4096 range is 128 chunks,
    // capped to 64 dispatched; every one of them should run.
    Run("range 0..4096",  0, 4096);

    fprintf(stderr, "[REPRO] all done\n");
    ThreadPoolShutdown();
    return 0;
}
