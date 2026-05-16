/// Minimal GcWorkerPool smoke test: verifies parallel GC operations.
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <atomic>
#include <thread>

#include "gc_region.h"
#include "gc_old_gen.h"
#include "gc_scheduler.h"
#include "gc_stats.h"
#include "gc_worker_pool.h"
#include "thread_state.h"

using namespace chaos::il2cpp::runtime_core;

static std::atomic<int> s_work_count{0};

int main() {
    setvbuf(stdout, nullptr, _IONBF, 0);
    puts("GcWorkerPool smoke test starting...");

    // 1. Basic worker pool test: run 4 workers on a simple function.
    {
        auto& pool = GcWorkerPool::Instance();
        s_work_count.store(0);
        pool.RunWorkers(4, [](int idx) {
            s_work_count.fetch_add(1);
            printf("  worker %d ran\n", idx);
        });
        int total = s_work_count.load();
        printf("  RunWorkers(4) completed, count=%d (main+3=4)\n", total);
        if (total < 1) {
            puts("FAIL: no workers ran");
            return 1;
        }
    }

    // 2. Reuse pool for another run.
    {
        auto& pool = GcWorkerPool::Instance();
        s_work_count.store(0);
        pool.RunWorkers(3, [](int idx) {
            s_work_count.fetch_add(1);
        });
        int total = s_work_count.load();
        printf("  RunWorkers(3) reuse OK, count=%d\n", total);
        if (total < 1) {
            puts("FAIL: no workers on reuse");
            return 1;
        }
    }

    // 3. Old-gen alloc + collect (exercises parallel mark + sweep via GcWorkerPool).
    {
        puts("  Testing old-gen alloc + collect...");
        void* p1 = g_old_gen.Allocate(128, true);
        void* p2 = g_old_gen.Allocate(256, true);
        void* p3 = g_old_gen.Allocate(64, true);
        if (!p1 || !p2 || !p3) {
            puts("FAIL: old-gen alloc");
            return 1;
        }
        std::memset(p1, 0xAB, 128);
        std::memset(p2, 0xCD, 256);
        std::memset(p3, 0xEF, 64);
        puts("  allocs OK");

        // Full collection with parallel mark+sweep.
        g_old_gen.Collect(nullptr, nullptr);
        puts("  collect OK (parallel mark+sweep via GcWorkerPool)");

        // Alloc again after collection.
        void* p4 = g_old_gen.Allocate(512, true);
        if (!p4) {
            puts("FAIL: alloc after collect");
            return 1;
        }
        std::memset(p4, 0x11, 512);
        puts("  post-collect alloc OK");
    }

    puts("All GcWorkerPool smoke tests OK");
    return 0;
}
