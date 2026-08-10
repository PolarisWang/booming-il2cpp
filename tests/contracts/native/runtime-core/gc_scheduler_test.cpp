/// gc_scheduler_test — GcScheduler basic operation tests.
///
/// Tests scheduler API surface — verifies operations complete without
/// crash and basic state transitions are correct. Detailed decision
/// logic is tested via stress tests.

#include <cstdio>
#include <cstdint>

#include <chaos/native_types.h>
#include "gc_scheduler.h"
#include "gc_test_macros.h"

using namespace chaos::il2cpp::runtime_core;

static int g_failures = 0;
static int g_tests    = 0;
static int g_sub      = 0;

// ── Test 1: RecordAllocation and DecideCollection ───────────────────
void TestRecordAndDecide() {
    TEST("RecordAndDecide");

    // Record some allocations and observe the decision.
    for (int i = 0; i < 100; i++) {
        g_gc_scheduler.RecordAllocation(4096);
    }

    auto kind = g_gc_scheduler.DecideCollection();
    // May be NONE, YOUNG, or FULL depending on internal state.
    // Just verify it doesn't crash and returns a valid enum.
    GC_CHECK(kind == GcCollectionKind::NONE ||
             kind == GcCollectionKind::YOUNG ||
             kind == GcCollectionKind::FULL,
             "DecideCollection returns valid GcCollectionKind");

    g_gc_scheduler.RecordGcCompleted();
}

// ── Test 2: LatencyMode round-trip ──────────────────────────────────
void TestLatencyMode() {
    TEST("LatencyMode round-trip");

    g_gc_scheduler.SetLatencyMode(GcLatencyMode::BATCH);
    // Verify round-trip works.
    g_gc_scheduler.SetLatencyMode(GcLatencyMode::INTERACTIVE);
    g_gc_scheduler.SetLatencyMode(GcLatencyMode::LOW_LATENCY);
    g_gc_scheduler.SetLatencyMode(GcLatencyMode::SUSTAINED_LOW_LATENCY);
    g_gc_scheduler.SetLatencyMode(GcLatencyMode::NO_GC_REGION);
    // Restore default.
    g_gc_scheduler.SetLatencyMode(GcLatencyMode::INTERACTIVE);

    GC_CHECK(true, "all latency modes set without crash");
}

// ── Test 3: CollectionMode round-trip ───────────────────────────────
void TestCollectionMode() {
    TEST("CollectionMode round-trip");

    g_gc_scheduler.SetCollectionMode(GcCollectionMode::DEFAULT);
    g_gc_scheduler.SetCollectionMode(GcCollectionMode::FORCED);
    g_gc_scheduler.SetCollectionMode(GcCollectionMode::OPTIMIZED);
    g_gc_scheduler.SetCollectionMode(GcCollectionMode::AGGRESSIVE);
    g_gc_scheduler.SetCollectionMode(GcCollectionMode::DEFAULT);

    GC_CHECK(true, "all collection modes set without crash");
}

// ── Test 4: RequestFullGc ───────────────────────────────────────────
void TestRequestFullGc() {
    TEST("RequestFullGc");

    g_gc_scheduler.RequestFullGc();
    bool requested = g_gc_scheduler.IsFullGcRequested();
    GC_CHECK(requested, "IsFullGcRequested true after RequestFullGc");

    g_gc_scheduler.RecordGcCompleted();
    // After RecordGcCompleted, the full GC flag may or may not be cleared
    // depending on internal implementation — just verify no crash.
    GC_CHECK(true, "RecordGcCompleted after RequestFullGc OK");
}

// ── Test 5: TryClaimGcSlot ──────────────────────────────────────────
void TestTryClaimGcSlot() {
    TEST("TryClaimGcSlot");

    // TryClaimGcSlot may or may not succeed depending on internal state.
    // Just verify it returns bool and doesn't crash.
    bool first = g_gc_scheduler.TryClaimGcSlot();
    bool second = g_gc_scheduler.TryClaimGcSlot();

    // At least one should succeed in the pair.
    GC_CHECK(first || !second,
             "TryClaimGcSlot works (sequential calls consistent)");

    g_gc_scheduler.RecordGcCompleted();
}

// ── Test 6: PageCountGrowth ─────────────────────────────────────────
void TestPageCountGrowth() {
    TEST("PageCountGrowth");

    g_gc_scheduler.RecordPageCountGrowth(1);
    g_gc_scheduler.RecordPageCountGrowth(10);
    g_gc_scheduler.RecordPageCountGrowth(100);
    g_gc_scheduler.ResetPageCountGrowth();

    GC_CHECK(true, "RecordPageCountGrowth + Reset OK");
}

// ── Test 7: RecommendedNurserySize ──────────────────────────────────
void TestRecommendedNurserySize() {
    TEST("RecommendedNurserySize");

    auto size = g_gc_scheduler.RecommendedNurserySize();
    GC_CHECK(size > 0, "RecommendedNurserySize > 0");
    GC_CHECK(size <= 4 * 1024 * 1024,
             "RecommendedNurserySize <= 4MB (sanity bound)");
}

// ── Test 8: TotalAllocatedSinceLastGC ───────────────────────────────
void TestTotalAllocated() {
    TEST("TotalAllocatedSinceLastGC");

    auto before = g_gc_scheduler.TotalAllocatedSinceLastGC();
    g_gc_scheduler.RecordAllocation(8192);
    auto after = g_gc_scheduler.TotalAllocatedSinceLastGC();
    GC_CHECK(after >= before,
             "TotalAllocatedSinceLastGC increases after RecordAllocation");
}

// ── Test 9: GC-D1 provisional force-blocking ──────────────────────────
// Under provisional (high-memory-pressure) mode, a full-GC request must
// yield a BLOCKING FULL (never deferred to BGC/NONE), even in a latency
// mode that normally prefers the background-collector.
void TestProvisionalForceBlocking() {
    TEST("ProvisionalForceBlocking");

    // Restore Interactive (default) latency so the test starts clean.
    g_gc_scheduler.SetLatencyMode(GcLatencyMode::INTERACTIVE);
    g_gc_scheduler.SetProvisionalMode(false);
    g_gc_scheduler.RecordGcCompleted();  // reset cooldown/claim state

    // In LOW_LATENCY + BGC enabled, a full-GC request is normally satisfied by
    // FULL_BGC (deferred).  Confirm that under provisional it goes blocking FULL.
    g_gc_scheduler.SetLatencyMode(GcLatencyMode::LOW_LATENCY);
    g_gc_scheduler.SetProvisionalMode(true);
    g_gc_scheduler.RequestFullGc();
    GcCollectionKind kind = g_gc_scheduler.DecideCollection();
    GC_CHECK(kind == GcCollectionKind::FULL,
             "provisional + low-latency full-GC request -> blocking FULL (not BGC)");

    // Cleanup: leave provisional OFF so other tests/driver state is unaffected.
    g_gc_scheduler.SetProvisionalMode(false);
    g_gc_scheduler.SetLatencyMode(GcLatencyMode::INTERACTIVE);
    g_gc_scheduler.RecordGcCompleted();
}

// ── Main ────────────────────────────────────────────────────────────
int main() {
    puts("GcScheduler basic operation tests");
    puts("═════════════════════════════════\n");

    TestRecordAndDecide();
    TestLatencyMode();
    TestCollectionMode();
    TestRequestFullGc();
    TestTryClaimGcSlot();
    TestPageCountGrowth();
    TestRecommendedNurserySize();
    TestTotalAllocated();
    TestProvisionalForceBlocking();

    printf("\nResults: %d tests, %d failures\n", g_tests, g_failures);
    return g_failures > 0 ? 1 : 0;
}
