/// Minimal stress test: 10 threads, 16 allocs each — quick validation
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <atomic>
#include <thread>
#include <vector>

#include "gc_region.h"
#include "gc_old_gen.h"
#include "gc_scheduler.h"
#include "gc_stats.h"
#include "gc_worker_pool.h"
#include "thread_state.h"

using namespace chaos::il2cpp::runtime_core;

static constexpr int kThreads = 10;
static constexpr int kAllocs = 16;
static std::atomic<int> s_ok{0};

static void worker(int idx) {
    int32_t tid = threading::AllocateThreadId();
    threading::RegisterThread(tid, nullptr);
    auto& mgr = RegionManager::Instance();
    Region* nursery = mgr.AllocateNursery();
    if (!nursery) { printf("  thread %d: no nursery\n", idx); return; }
    tls_nursery_ctx.nursery = nursery;
    tls_nursery_ctx.limit = nursery->end - kMaxNurseryAlloc;

    for (int i = 0; i < kAllocs; i++) {
        void* p = NurseryAllocate(128);
        if (p) {
            std::memset(p, 0xAB, 128);
            s_ok.fetch_add(1);
        }
        if ((i & 3) == 3) threading::SafepointPoll();
    }
    threading::SafepointPoll();
    threading::UnregisterThread();
}

int main() {
    setvbuf(stdout, nullptr, _IONBF, 0);
    printf("Mini stress: %d threads x %d allocs\n", kThreads, kAllocs);
    GcStatsSnapshot before;
    before.young_collections = g_gc_stats.young_collections.load();

    std::vector<std::thread> threads;
    for (int i = 0; i < kThreads; i++)
        threads.emplace_back(worker, i);
    for (auto& t : threads) t.join();

    GcStatsSnapshot after;
    after.young_collections = g_gc_stats.young_collections.load();
    uint64_t young_gcs = after.young_collections - before.young_collections;
    printf("Allocs: %d, Young GCs: %llu\n", s_ok.load(), (unsigned long long)young_gcs);
    printf("%s\n", s_ok.load() > 0 ? "PASS" : "FAIL");
    return s_ok.load() > 0 ? 0 : 1;
}
