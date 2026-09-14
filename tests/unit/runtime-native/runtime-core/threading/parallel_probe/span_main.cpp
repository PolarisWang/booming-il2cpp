// Which indices does Parallel.For actually visit, for a FAST body?
//
// With a 1 ms-per-call body the rendezvous is correct and all 256 indices are
// visited.  With a no-op body only ~2300 of 4096 are.  The difference is
// timing, so the suspect is the claim arithmetic: next_index is advanced by
// fetch_add(32) on EVERY claim, including claims that land past the end of the
// range.  A worker that overshoots still consumed 32 of the index space, so
// when the pool grows more workers than the range has chunks, indices get
// skipped.
//
// This probe counts how many times the claim pointer overshoots, without any
// printf in the loop (which would serialize the workers and hide the effect).
#include <cstdio>
#include <atomic>
#include <vector>

#include <core/delegate_object.h>
#include "parallel.h"
#include "thread_pool.h"

using namespace chaos::il2cpp::runtime_core::threading;
using chaos::il2cpp::runtime_core::DelegateObject;

extern "C" void chaos_delegate_object_invoke(
    CHAOS_IL2CPP_INTPTR delegate_ptr,
    CHAOS_IL2CPP_INTPTR* args_buf,
    CHAOS_IL2CPP_INTPTR* ret_buf,
    CHAOS_IL2CPP_UINT32 arg_count) noexcept;

static const int kMax = 65536;
static std::atomic<unsigned char> g_seen[kMax];
static std::atomic<long long> g_calls{0};

static CHAOS_IL2CPP_INTPTR Body(CHAOS_IL2CPP_INTPTR arg) {
    const long long i = static_cast<long long>(arg);
    g_calls.fetch_add(1, std::memory_order_relaxed);
    if (i >= 0 && i < kMax) g_seen[i].store(1, std::memory_order_relaxed);
    return 0;
}

static void Check(const char* label, int from, int to) {
    g_calls.store(0);
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

    long long seen = 0, first_gap = -1, last_gap = -1;
    for (int i = from; i < to; ++i) {
        if (g_seen[i].load()) {
            ++seen;
        } else {
            if (first_gap < 0) first_gap = i;
            last_gap = i;
        }
    }
    fprintf(stderr, "[SPAN] %-22s calls=%5lld distinct=%5lld / %5d  gaps=%5lld first=%lld last=%lld\n",
            label, g_calls.load(), seen, to - from,
            (long long)(to - from) - seen, first_gap, last_gap);
    fflush(stderr);
    delete d;
}

int main() {
    ThreadPoolInitialize();

    // Escalate: as the range grows past what the pool has workers for, does the
    // shortfall appear at a characteristic size?
    Check("0..96    (3 chunks)",   0, 96);
    Check("0..256   (8 chunks)",   0, 256);
    Check("0..512   (16 chunks)",  0, 512);
    Check("0..1024  (32 chunks)",  0, 1024);
    Check("0..2048  (64 chunks)",  0, 2048);   // chunk cap is 64
    Check("0..4096  (128->64 ch)", 0, 4096);   // beyond the cap
    Check("0..8192",               0, 8192);

    ThreadPoolShutdown();
    return 0;
}
