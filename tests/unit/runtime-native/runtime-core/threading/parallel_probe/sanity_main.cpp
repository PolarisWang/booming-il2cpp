// Sanity probe: call the SAME body function directly for all 4096 indices, with
// no Parallel.For involved.
//
// The coverage probe recorded 2365 of 4096 indices through Parallel.For while
// the instrumentation showed 4096 invokes entering and returning.  Before
// concluding anything about the thread pool, rule out the recording itself: if
// a plain sequential loop over the same range also loses indices, the counter
// is at fault, not the runtime.
#include <cstdio>
#include <atomic>
#include <chrono>
#include <thread>

#include <core/delegate_object.h>
#include "parallel.h"
#include "thread_pool.h"

using chaos::il2cpp::runtime_core::DelegateObject;
using namespace chaos::il2cpp::runtime_core::threading;

// Declared locally rather than pulled from core/delegate_helpers.h: this probe
// only needs the ABI signature, and declaring it here keeps the probe
// independent of header-path resolution inside the standalone project.
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

static void Report(const char* label, int from, int to) {
    long long seen = 0;
    for (int i = from; i < to; ++i) {
        if (g_seen[i].load()) ++seen;
    }
    fprintf(stderr, "[SANITY] %s: calls=%lld distinct=%lld / %d\n",
            label, g_calls.load(), seen, to - from);
    fflush(stderr);
}

int main() {
    // Phase 1 — sequential, no pool, no delegate machinery at all.
    g_calls.store(0);
    for (int i = 0; i < kMax; ++i) g_seen[i].store(0);
    for (int i = 0; i < 4096; ++i) Body(static_cast<CHAOS_IL2CPP_INTPTR>(i));
    Report("direct calls", 0, 4096);

    // Phase 2 — through the delegate invoke layer, still single-threaded.
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

    for (int i = 0; i < 4096; ++i) {
        CHAOS_IL2CPP_INTPTR args[1];
        args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(i);
        chaos_delegate_object_invoke(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(d),
                                     args, nullptr, 1);
    }
    Report("delegate invokes", 0, 4096);

    // Phase 3 — through Parallel.For.
    g_calls.store(0);
    for (int i = 0; i < kMax; ++i) g_seen[i].store(0);

    ThreadPoolInitialize();
    chaos_parallel_for_range_int(0, 4096, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(d));
    Report("parallel for (immediately)", 0, 4096);

    // Re-read after a generous settle.  If the numbers GROW, the caller
    // returned while workers were still iterating chunks -- i.e. the
    // rendezvous signaled completion early, and the shortfall above is a race
    // rather than lost work.
    std::this_thread::sleep_for(std::chrono::seconds(3));
    Report("parallel for (after 3s)", 0, 4096);
    ThreadPoolShutdown();

    delete d;
    return 0;
}
