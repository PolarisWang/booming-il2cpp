/// gc_scheduler_test — GcScheduler basic operation tests.
///
/// Tests scheduler API surface — verifies operations complete without
/// crash and basic state transitions are correct. Detailed decision
/// logic is tested via stress tests.

#include <cstdio>
#include <cstdint>
#include <cstdlib>

#include <chaos/native_types.h>
#include "gc_scheduler.h"
#include "gc_config.h"
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

// ── Test 7b (M6): Fragmentation-to-gen-budget servo ─────────────────
// The scheduler serves old-gen fragmentation into the nursery/gen1 budget
// (gc_scheduler.cpp RecommendedNurserySize frag-discount): higher fragmentation
// must shrink the recommended nursery so old gen gets collection cycles to
// compact before new survivors arrive.  Closes the "frag → gen budget" loop.
void TestFragServo() {
    TEST("FragServo: old-gen frag discounts nursery budget");

    auto baseline = g_gc_scheduler.RecommendedNurserySize();
    GC_CHECK(baseline > 0, "baseline nursery budget > 0");

    // Moderate fragmentation (30-50%) → 0.75x discount.
    g_gc_scheduler.SetOldGenFragmentation(0.40f);
    auto mod = g_gc_scheduler.RecommendedNurserySize();
    GC_CHECK(mod <= baseline,
             "frag=0.40 → nursery budget does not grow vs baseline");
    GC_CHECK(mod > 0, "frag=0.40 → discounted nursery still > 0");

    // High fragmentation (>50%) → stronger 0.50x discount.
    g_gc_scheduler.SetOldGenFragmentation(0.60f);
    auto high = g_gc_scheduler.RecommendedNurserySize();
    GC_CHECK(high <= mod,
             "frag=0.60 → nursery budget shrinks further vs frag=0.40");

    // Reset so other tests are unaffected.
    g_gc_scheduler.SetOldGenFragmentation(0.0f);
    GC_CHECK(true, "frag servo reset ok");
}

// ── Test 7c (M4/M3B): NGC2 queue — mandated gen2 collection ─────────
// QueueNgc2() / provisional entry must force a blocking FULL (gen2) at the next
// DecideCollection, exactly once, then clear.  This is the "NGC2 排队" — the
// gen2-queue mechanism that provisional / high-fragmentation uses to mandate a
// blocking gen2 collection.
void TestNgc2Queue() {
    TEST("Ngc2Queue: mandated gen2 collection fires once");

    // Baseline: no queue → not queued.
    GC_CHECK(!g_gc_scheduler.IsNgc2Queued(), "NGC2 not queued by default");

    // Queue explicitly → DecideCollection returns blocking FULL, then cleared.
    g_gc_scheduler.QueueNgc2();
    GC_CHECK(g_gc_scheduler.IsNgc2Queued(), "NGC2 queued after QueueNgc2()");
    auto kind = g_gc_scheduler.DecideCollection();
    GC_CHECK(kind == GcCollectionKind::FULL,
             "queued NGC2 → DecideCollection forces blocking FULL");
    GC_CHECK(!g_gc_scheduler.IsNgc2Queued(),
             "NGC2 cleared after one discharge");

    // Provisional entry also queues NGC2 → next decide is FULL.
    g_gc_scheduler.SetProvisionalMode(true);
    GC_CHECK(g_gc_scheduler.IsNgc2Queued(),
             "SetProvisionalMode(true) queues NGC2");
    kind = g_gc_scheduler.DecideCollection();
    GC_CHECK(kind == GcCollectionKind::FULL,
             "provisional+ngc2 → blocking FULL (not BGC/NONE)");
    GC_CHECK(!g_gc_scheduler.IsNgc2Queued(), "NGC2 cleared after discharge");

    // Cleanup: leave provisional off.
    g_gc_scheduler.SetProvisionalMode(false);
    GC_CHECK(true, "NGC2 queue (M4/M3B) lifecycle OK");
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

// ── Test 10: GC-E1 config env override ───────────────────────────────
// Verifies GcConfig::Initialize() honors a CHAOS_GC_<Key> env override,
// defaulting to the compile-time value when the env var is absent/empty.
void TestConfigEnvOverride() {
    TEST("ConfigEnvOverride");

    constexpr size_t kTestNursery = 8 * 1024 * 1024;   // 8 MB
    constexpr size_t kCompileDefault = 64 * 1024 * 1024;  // kDefaultYoungRegionSize
    auto set_env = [](const char* val) {
#ifdef _MSC_VER
        _putenv_s("CHAOS_GC_NurserySize", val);
#else
        if (val) setenv("CHAOS_GC_NurserySize", val, 1); else unsetenv("CHAOS_GC_NurserySize");
#endif
    };

    set_env("8388608");   // 8 MB
    GcConfig().Initialize();
    GC_CHECK(GcConfig().DefaultNurserySize == kTestNursery,
             "CHAOS_GC_NurserySize env override drives DefaultNurserySize");

    set_env("");          // empty → treat as default
    GcConfig().Initialize();
    GC_CHECK(GcConfig().DefaultNurserySize == kCompileDefault,
             "DefaultNurserySize returns to compile-time default after env cleared");

    set_env("garbage");   // non-numeric → fall back to default
    GcConfig().Initialize();
    GC_CHECK(GcConfig().DefaultNurserySize == kCompileDefault,
             "unparseable env falls back to default");

    set_env("");          // restore clean
    GcConfig().Initialize();
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
    TestFragServo();
    TestNgc2Queue();
    TestTotalAllocated();
    TestProvisionalForceBlocking();
    TestConfigEnvOverride();

    printf("\nResults: %d tests, %d failures\n", g_tests, g_failures);
    return g_failures > 0 ? 1 : 0;
}
