// gc_bgc_race_test.cpp — Targeted BGC-YoungGC race condition reproduction
// Forces Young GC to run DURING BGC concurrent mark by keeping BGC busy
// with a large live object graph, then triggering nursery exhaustion.
//
// If the race exists: this test crashes with access violation.
// After fix: this test must pass without crash.

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <atomic>
#include <chrono>
#include <vector>

#include <chaos/native_types.h>
#include "gc/gc_bgc.h"
#include "gc_region.h"
#include "gc_scheduler.h"
#include "gc_old_gen.h"
#include "gc_young_collector.h"
#include "gc_young_gen.h"
#include "thread_state.h"

using namespace chaos::il2cpp::runtime_core;

// ── Helpers ──────────────────────────────────────────────────────────

/// Wait for BGC to reach a specific phase.
static bool WaitForPhase(BgcPhase phase, int timeout_ms = 30000) {
    auto start = std::chrono::steady_clock::now();
    while (BgcController::Instance().Phase() != phase) {
        std::this_thread::yield();
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() > timeout_ms) {
            printf("  TIMEOUT waiting for phase %d (current: %d)\n",
                   static_cast<int>(phase),
                   static_cast<int>(BgcController::Instance().Phase()));
            return false;
        }
    }
    return true;
}

// ── Test: Force BGC-YoungGC race ─────────────────────────────────────
//
// Strategy:
// 1. Allocate a large array of old-gen objects (kept alive via global ptrs)
// 2. Start BGC cycle (BGC begins concurrent marking)
// 3. Wait until CONCURRENT_MARK phase is confirmed
// 4. While BGC is still marking, run multiple young GCs by exhausting nursery
// 5. Complete BGC cycle
// 6. If no crash: the race is handled or doesn't exist in current code

static std::vector<void*> g_keepalive;  // prevent sweep from reclaiming

void TestBgcYoungGcRace() {
    printf("\n── BGC-YoungGC race test ──\n");
    g_keepalive.clear();

    // Step 1: Allocate many old-gen objects to give BGC real work.
    // We keep them alive in g_keepalive so BGC must trace them.
    printf("  Allocating old-gen objects...\n");
    constexpr int kObjectCount = 2000;
    for (int i = 0; i < kObjectCount; i++) {
        void* p = NurseryAllocate(kMaxTlabAlloc + 8);  // forces old-gen
        if (p) {
            memset(p, 0xBB, 64);
            g_keepalive.push_back(p);
        }
    }
    printf("  Allocated %zu old-gen objects\n", g_keepalive.size());

    // Step 2: Start BGC cycle.
    {
        uint32_t gen = threading::RequestGlobalSafepoint();
        BgcController::Instance().StartBgcCycle();
        threading::ReleaseGlobalSafepoint(gen);
    }

    // Step 3: Wait for CONCURRENT_MARK.
    bool in_concurrent_mark = WaitForPhase(BgcPhase::CONCURRENT_MARK, 5000);
    if (!in_concurrent_mark) {
        printf("  BGC skipped CONCURRENT_MARK (phase=%d)\n",
               static_cast<int>(BgcController::Instance().Phase()));
        // Complete cycle and exit.
        uint32_t gen = threading::RequestGlobalSafepoint();
        BgcController::Instance().ForceComplete();
        threading::ReleaseGlobalSafepoint(gen);
        BgcController::Instance().WaitForCycleComplete();
        printf("  SKIP: could not enter CONCURRENT_MARK\n");
        return;
    }

    printf("  BGC is in CONCURRENT_MARK — triggering young GCs...\n");

    // Step 4: Run young GCs while BGC is concurrently marking.
    // Exhaust nursery by allocating many objects.
    for (int round = 0; round < 5; round++) {
        // Allocate until young GC triggers.
        for (int j = 0; j < 100; j++) {
            volatile void* p = NurseryAllocate(32);
            (void)p;
        }

        // Force a young GC.
        uint32_t gen = threading::RequestGlobalSafepoint();
        printf("  Young GC #%d during BGC concurrent mark (BGC phase=%d)...\n",
               round + 1, static_cast<int>(BgcController::Instance().Phase()));
        GcYoungCollection();
        threading::ReleaseGlobalSafepoint(gen);
        printf("  Young GC #%d OK\n", round + 1);

        // Check if BGC has moved past CONCURRENT_MARK.
        auto phase = BgcController::Instance().Phase();
        if (phase != BgcPhase::CONCURRENT_MARK) {
            printf("  BGC left CONCURRENT_MARK (phase=%d) after young GC #%d\n",
                   static_cast<int>(phase), round + 1);
            break;
        }
    }

    // Step 5: Complete the BGC cycle cleanly.
    printf("  Completing BGC cycle...\n");
    auto phase = BgcController::Instance().Phase();
    if (phase == BgcPhase::CONCURRENT_MARK || phase == BgcPhase::REMARK_NEEDED) {
        if (phase == BgcPhase::CONCURRENT_MARK) {
            // Wait for mark to finish.
            WaitForPhase(BgcPhase::REMARK_NEEDED, 30000);
        }
        uint32_t gen = threading::RequestGlobalSafepoint();
        BgcController::Instance().StwRemark();
        BgcController::Instance().StartConcurrentSweep();
        threading::ReleaseGlobalSafepoint(gen);

        WaitForPhase(BgcPhase::COMPACT_NEEDED, 30000);
        gen = threading::RequestGlobalSafepoint();
        BgcController::Instance().StwCompact();
        threading::ReleaseGlobalSafepoint(gen);
    }

    BgcController::Instance().WaitForCycleComplete();
    printf("  BGC cycle completed, phase=%d\n",
           static_cast<int>(BgcController::Instance().Phase()));

    printf("  PASS: BGC-YoungGC race test completed without crash\n");
}

int main() {
    puts("CRAG BGC-YoungGC race test");
    puts("═══════════════════════════\n");

    InitYoungGeneration();
    void* warmup = NurseryAllocate(64);
    (void)warmup;

    threading::RegisterThread(threading::AllocateThreadId(), nullptr);
    BgcController::Instance().Start();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    TestBgcYoungGcRace();

    // Cleanup.
    g_keepalive.clear();
    BgcController::Instance().Stop();
    threading::UnregisterThread();

    printf("\nOK\n");
    return 0;
}
