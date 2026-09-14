// Per-index coverage probe.  The aggregate counter said 2248 of 4096 calls,
// but the invoke layer traced 4096 entries -- so either the counter is lying or
// the body is not the one being counted.  Record a per-index bitmap, which
// cannot be fooled by a lost update to a single shared counter.
#include <cstdio>
#include <atomic>
#include <vector>

#include "parallel.h"
#include "thread_pool.h"
#include "core/delegate_object.h"

using chaos::il2cpp::runtime_core::DelegateObject;
using namespace chaos::il2cpp::runtime_core::threading;

static const int kMax = 65536;
static std::atomic<unsigned char> g_seen[kMax];
static std::atomic<long long> g_calls{0};

static CHAOS_IL2CPP_INTPTR Body(CHAOS_IL2CPP_INTPTR arg) {
    const long long i = static_cast<long long>(arg);
    g_calls.fetch_add(1, std::memory_order_relaxed);
    if (i >= 0 && i < kMax) g_seen[i].store(1, std::memory_order_relaxed);
    return 0;
}

int main() {
    ThreadPoolInitialize();

    for (int i = 0; i < kMax; ++i) g_seen[i].store(0);
    g_calls.store(0);

    auto* d = new DelegateObject();
    d->type_info = nullptr;
    d->chaos_delegate_target = 0;
    d->chaos_delegate_method_ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(Body);
    d->chaos_delegate_invocation_list = 0;
    d->chaos_delegate_invocation_count = 0;
    d->chaos_delegate_method_token = 0;
    d->_pad = 0;

    chaos_parallel_for_range_int(0, 4096, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(d));

    // Give any worker still running a moment to land its final increments.
    // The caller's rendezvous should already guarantee this, so a nonzero wait
    // here would itself be a finding.
    fprintf(stderr, "[COV] caller returned\n");
    fflush(stderr);

    long long seen = 0;
    for (int i = 0; i < 4096; ++i) {
        if (g_seen[i].load()) ++seen;
    }
    fprintf(stderr, "[COV] g_calls=%lld  distinct indices seen=%lld / 4096\n",
            g_calls.load(), seen);

    // Where are the gaps?
    int first_gap = -1, last_gap = -1, gaps = 0;
    for (int i = 0; i < 4096; ++i) {
        if (!g_seen[i].load()) {
            if (first_gap < 0) first_gap = i;
            last_gap = i;
            ++gaps;
        }
    }
    fprintf(stderr, "[COV] gaps=%d first=%d last=%d\n", gaps, first_gap, last_gap);
    fflush(stderr);

    delete d;
    ThreadPoolShutdown();
    return 0;
}
