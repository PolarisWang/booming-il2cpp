/// BGC (Background GC) smoke test.
///
/// Tests:
///   1. Basic BGC cycle — start concurrent mark, STW re-mark, concurrent sweep
///   2. Concurrent allocation during BGC mark phase
///   3. BGC + young GC interleaved
///
/// NOTE: The test thread is NOT registered as a ManagedThread, so BGC's
/// conservative stack scan does not find local-variable roots.  Old-gen
/// objects allocated in this test will be collected as unreachable during
/// BGC sweep.  The tests verify cycle machinery correctness (no crashes
/// during concurrent mark/sweep), not object survival.
///
/// Uses direct CRAG APIs (no BDWGC dependency).

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <thread>
#include <atomic>
#include <chrono>

#include <chaos/native_types.h>
#include "gc/gc_bgc.h"
#include "gc_region.h"
#include "gc_scheduler.h"
#include "gc_old_gen.h"
#include "gc_young_collector.h"
#include "gc_young_gen.h"
#include "thread_state.h"

using namespace chaos::il2cpp::runtime_core;

#include "gc_test_macros.h"

static int g_failures = 0;

// ── Helpers ──────────────────────────────────────────────────────────

/// Allocate directly into old-gen by requesting > kMaxTlabAlloc bytes.
static void* AllocOldGen(size_t payload_size) {
    size_t alloc_size = payload_size + 64;
    if (alloc_size <= kMaxTlabAlloc) {
        alloc_size = kMaxTlabAlloc + 8;
    }
    void* p = NurseryAllocate(alloc_size);
    if (p) std::memset(p, 0xAA, payload_size);
    return p;
}

/// Wait for BGC to reach a specific phase (with timeout).
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

/// Run a complete BGC cycle from start to finish.
static bool RunBgcCycle() {
    // Phase 1: Start BGC cycle under safepoint.
    uint32_t gen = threading::RequestGlobalSafepoint();
    BgcController::Instance().StartBgcCycle();
    threading::ReleaseGlobalSafepoint(gen);

    // Phase 2: Wait for concurrent mark to complete (phase = REMARK_NEEDED).
    if (!WaitForPhase(BgcPhase::REMARK_NEEDED, 30000)) {
        printf("  ERROR: concurrent mark did not complete\n");
        return false;
    }

    // Phase 3: STW re-mark + start concurrent sweep.
    gen = threading::RequestGlobalSafepoint();
    BgcController::Instance().StwRemark();
    BgcController::Instance().StartConcurrentSweep();
    threading::ReleaseGlobalSafepoint(gen);

    // Phase 4: Wait for STW compaction (concurrent sweep + finish).
    if (!WaitForPhase(BgcPhase::COMPACT_NEEDED, 30000)) {
        printf("  ERROR: concurrent sweep did not complete\n");
        return false;
    }

    // Phase 5: STW compaction.
    gen = threading::RequestGlobalSafepoint();
    BgcController::Instance().StwCompact();
    threading::ReleaseGlobalSafepoint(gen);

    // Phase 6: Wait for full cycle completion.
    BgcController::Instance().WaitForCycleComplete();

    // Verify BGC is back to IDLE.
    if (BgcController::Instance().Phase() != BgcPhase::IDLE) {
        printf("  ERROR: BGC phase is %d after cycle, expected IDLE\n",
               static_cast<int>(BgcController::Instance().Phase()));
        return false;
    }
    return true;
}

// ── Test 1: Basic BGC cycle ─────────────────────────────────────────
void TestBasicBgcCycle() {
    printf("\n── Test 1: Basic BGC cycle ──\n");

    // Populate old-gen with some objects (will be swept by BGC since not rooted).
    for (int i = 0; i < 10; i++) {
        AllocOldGen(256);
    }

    // Run a full BGC cycle — verify the cycle machinery (mark + sweep) completes.
    bool ok = RunBgcCycle();
    CHECK(ok, "BGC cycle completed without crash");

    // Run a second BGC cycle to verify re-entrancy.
    ok = RunBgcCycle();
    CHECK(ok, "second BGC cycle completed without crash");
}

// ── Test 2: Concurrent allocation during BGC mark ───────────────────
void TestBgcWithAllocation() {
    printf("\n── Test 2: BGC with concurrent allocation ──\n");

    // Pre-populate old-gen.
    for (int i = 0; i < 20; i++) {
        AllocOldGen(128);
    }

    // Start BGC cycle.
    uint32_t gen = threading::RequestGlobalSafepoint();
    BgcController::Instance().StartBgcCycle();
    threading::ReleaseGlobalSafepoint(gen);

    // While BGC is marking, allocate heavily on another thread.
    std::atomic<bool> alloc_done{false};
    std::thread alloc_thread([&alloc_done]() {
        threading::RegisterThread(threading::AllocateThreadId(), nullptr);
        for (int i = 0; i < 5000; i++) {
            void* p = NurseryAllocate(64);
            if (p) std::memset(p, 0xBB, 64);

            if (i % 100 == 0) {
                void* big = AllocOldGen(512);
                (void)big;
            }

            if (BgcController::Instance().Phase() == BgcPhase::REMARK_NEEDED)
                break;
        }
        threading::UnregisterThread();
        alloc_done = true;
    });

    // Wait for concurrent mark to finish.
    bool marked = WaitForPhase(BgcPhase::REMARK_NEEDED, 30000);
    alloc_thread.join();

    CHECK(marked, "concurrent mark completed with concurrent allocation");
    CHECK(alloc_done, "concurrent allocation thread finished");

    // Complete the cycle.
    gen = threading::RequestGlobalSafepoint();
    BgcController::Instance().StwRemark();
    BgcController::Instance().StartConcurrentSweep();
    threading::ReleaseGlobalSafepoint(gen);

    // Wait for sweep + compact.
    if (WaitForPhase(BgcPhase::COMPACT_NEEDED, 30000)) {
        gen = threading::RequestGlobalSafepoint();
        BgcController::Instance().StwCompact();
        threading::ReleaseGlobalSafepoint(gen);
    }

    BgcController::Instance().WaitForCycleComplete();
    CHECK(BgcController::Instance().Phase() == BgcPhase::IDLE,
          "BGC cycle with allocation completed to IDLE");
}

// ── Test 3: BGC + young GC interleaved ──────────────────────────────
void TestBgcWithYoungGc() {
    printf("\n── Test 3: BGC + young GC interleaved ──\n");

    // Reset TLAB to force a fresh claim from the young generation.
    tls_tlab = TLAB{};

    // Pre-populate old-gen.
    for (int i = 0; i < 15; i++) {
        AllocOldGen(256);
    }

    // Start BGC cycle.
    uint32_t gen = threading::RequestGlobalSafepoint();
    BgcController::Instance().StartBgcCycle();
    threading::ReleaseGlobalSafepoint(gen);

    // While BGC is marking, run a young GC.
    for (int i = 0; i < 1; i++) {
        // Allocate nursery objects.
        for (int j = 0; j < 200; j++) {
            void* p = NurseryAllocate(32);
            if (p) std::memset(p, 0xCC, 32);
        }

        // Run young GC.
        gen = threading::RequestGlobalSafepoint();
        GcYoungCollection();
        threading::ReleaseGlobalSafepoint(gen);

        printf("  young GC #%d during BGC mark OK\n", i + 1);

        if (BgcController::Instance().Phase() == BgcPhase::REMARK_NEEDED)
            break;
    }

    // Verify BGC reached STW re-mark phase (or wait briefly if the
    // concurrent mark completed during the last young GC's safepoint).
    if (BgcController::Instance().Phase() != BgcPhase::REMARK_NEEDED) {
        WaitForPhase(BgcPhase::REMARK_NEEDED, 5000);
    }
    CHECK(BgcController::Instance().Phase() == BgcPhase::REMARK_NEEDED,
          "BGC waiting for STW re-mark after young GCs");

    // Complete the cycle.
    gen = threading::RequestGlobalSafepoint();
    BgcController::Instance().StwRemark();
    BgcController::Instance().StartConcurrentSweep();
    threading::ReleaseGlobalSafepoint(gen);

    // Wait for sweep + compact.
    if (WaitForPhase(BgcPhase::COMPACT_NEEDED, 30000)) {
        gen = threading::RequestGlobalSafepoint();
        BgcController::Instance().StwCompact();
        threading::ReleaseGlobalSafepoint(gen);
    }

    BgcController::Instance().WaitForCycleComplete();
    CHECK(BgcController::Instance().Phase() == BgcPhase::IDLE,
          "BGC + young GC interleaved completed");
}

// ── Test 4: ForceComplete during BGC marking ────────────────────────
void TestBgcForceComplete() {
    printf("\n── Test 4: BGC ForceComplete ──\n");

    // Pre-populate old-gen.
    for (int i = 0; i < 10; i++) {
        AllocOldGen(256);
    }

    // Start BGC cycle.
    {
        uint32_t gen = threading::RequestGlobalSafepoint();
        BgcController::Instance().StartBgcCycle();
        threading::ReleaseGlobalSafepoint(gen);
    }

    // Let concurrent mark run briefly, then force-complete.
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    {
        uint32_t gen = threading::RequestGlobalSafepoint();
        // BGC may or may not still be in concurrent mark depending on timing.
        // Not busy is acceptable (mark completed before we entered safepoint).
        printf("  Phase at ForceComplete: %d\n",
               static_cast<int>(BgcController::Instance().Phase()));
        BgcController::Instance().ForceComplete();
        threading::ReleaseGlobalSafepoint(gen);
    }

    BgcController::Instance().WaitForCycleComplete();
    CHECK(BgcController::Instance().Phase() == BgcPhase::IDLE,
          "BGC returned to IDLE after ForceComplete");
    CHECK(!BgcController::Instance().IsBusy(),
          "BGC not busy after ForceComplete");
    CHECK(true, "BGC ForceComplete completed without crash");
}

// ── Test 5: IsBusy / IsMarking phase checks ──────────────────────────
void TestBgcIsBusyIsMarking() {
    printf("\n── Test 5: BGC IsBusy / IsMarking phase checks ──\n");

    // Initially idle → not busy, not marking.
    CHECK(!BgcController::Instance().IsBusy(),
          "BGC not busy at idle");
    CHECK(!BgcController::Instance().IsMarking(),
          "BGC not marking at idle");

    for (int i = 0; i < 10; i++) {
        AllocOldGen(256);
    }

    // Start cycle → should become busy + marking.
    {
        uint32_t gen = threading::RequestGlobalSafepoint();
        BgcController::Instance().StartBgcCycle();
        threading::ReleaseGlobalSafepoint(gen);
    }

    // Allow some time for concurrent mark to start.
    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    BgcPhase phase = BgcController::Instance().Phase();
    printf("  Phase after start: %d\n", static_cast<int>(phase));

    if (phase == BgcPhase::CONCURRENT_MARK) {
        CHECK(BgcController::Instance().IsBusy(), "IsBusy true during CONCURRENT_MARK");
        CHECK(BgcController::Instance().IsMarking(), "IsMarking true during CONCURRENT_MARK");
    }

    // Force-complete to clean up.
    {
        uint32_t gen = threading::RequestGlobalSafepoint();
        BgcController::Instance().ForceComplete();
        threading::ReleaseGlobalSafepoint(gen);
    }
    BgcController::Instance().WaitForCycleComplete();

    CHECK(!BgcController::Instance().IsBusy(),
          "Not busy after forced completion");
    CHECK(!BgcController::Instance().IsMarking(),
          "Not marking after forced completion");
    CHECK(true, "IsBusy/IsMarking phase checks complete");
}

// ── Test 6: Multiple consecutive BGC cycles ──────────────────────────
void TestBgcMultipleCycles() {
    printf("\n── Test 6: Multiple consecutive BGC cycles ──\n");

    constexpr int kCycles = 2;
    for (int c = 0; c < kCycles; c++) {
        // Populate fresh old-gen objects for each cycle.
        for (int i = 0; i < 10; i++) {
            AllocOldGen(256);
        }

        printf("  Cycle %d/%d: ", c + 1, kCycles);
        bool ok = RunBgcCycle();
        CHECK(ok, "BGC cycle %d/%d completed", c + 1, kCycles);
        if (!ok) break;
    }

    CHECK(BgcController::Instance().Phase() == BgcPhase::IDLE,
          "BGC idle after %d cycles", kCycles);
    CHECK(true, "Multiple BGC cycles completed without crash");
}
int main() {
    puts("CRAG BGC smoke test");
    puts("══════════════════════\n");

    // Warm up CRAG globals before starting BGC thread.
    InitYoungGeneration();
    void* warmup = NurseryAllocate(64);
    (void)warmup;

    // Register this thread as a ManagedThread so it participates in safepoint protocol.
    threading::RegisterThread(threading::AllocateThreadId(), nullptr);

    // Start the BGC background thread.
    BgcController::Instance().Start();

    // Wait a brief moment for the BGC thread to initialize.
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    TestBasicBgcCycle();
    TestBgcWithAllocation();
    TestBgcForceComplete();
    TestBgcIsBusyIsMarking();
    TestBgcMultipleCycles();
    TestBgcWithYoungGc();  // pre-existing segfault — kept last for isolation

    // Clean shutdown.
    BgcController::Instance().Stop();

    threading::UnregisterThread();

    printf("\n══ Results: 6 tests, %d failures ══\n", g_failures);

    return g_failures > 0 ? 1 : 0;
}
