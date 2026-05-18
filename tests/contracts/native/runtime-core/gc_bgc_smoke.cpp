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
#include "thread_state.h"

using namespace chaos::il2cpp::runtime_core;

static int g_failures = 0;
#define CHECK(cond, msg) do {                                   \
    if (!(cond)) {                                              \
        printf("  FAIL [%s:%d]: %s\n", __FILE__, __LINE__, msg);\
        ++g_failures;                                           \
    } else {                                                    \
        printf("  PASS: %s\n", msg);                            \
    }                                                           \
} while(0)

// ── Helpers ──────────────────────────────────────────────────────────

/// Allocate directly into old-gen by requesting > kMaxNurseryAlloc bytes.
static void* AllocOldGen(size_t payload_size) {
    size_t alloc_size = payload_size + 64;
    if (alloc_size <= kMaxNurseryAlloc) {
        alloc_size = kMaxNurseryAlloc + 8;
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

    // Pre-populate old-gen.
    for (int i = 0; i < 15; i++) {
        AllocOldGen(256);
    }

    // Start BGC cycle.
    uint32_t gen = threading::RequestGlobalSafepoint();
    BgcController::Instance().StartBgcCycle();
    threading::ReleaseGlobalSafepoint(gen);

    // While BGC is marking, run several young GCs.
    for (int i = 0; i < 10; i++) {
        // Allocate nursery objects.
        for (int j = 0; j < 200; j++) {
            void* p = NurseryAllocate(32);
            if (p) std::memset(p, 0xCC, 32);
        }

        // Run young GC (promotes survivors to old-gen).
        gen = threading::RequestGlobalSafepoint();
        GcYoungCollection(tls_nursery_ctx.nursery);
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

// ── Main ────────────────────────────────────────────────────────────
int main() {
    puts("CRAG BGC smoke test");
    puts("══════════════════════\n");

    // Warm up CRAG globals before starting BGC thread.
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
    TestBgcWithYoungGc();

    // Clean shutdown.
    BgcController::Instance().Stop();

    threading::UnregisterThread();

    printf("\n══ Results: 3 tests, %d failures ══\n", g_failures);

    return g_failures > 0 ? 1 : 0;
}
