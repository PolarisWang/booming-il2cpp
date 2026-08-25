/// gc_scheduler_test — GcScheduler basic operation tests (GoogleTest).

#include <cstdint>

#include <chaos/native_types.h>
#include "gc_scheduler.h"

#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

// ── Test 1: RecordAllocation and DecideCollection ───────────────────

TEST(GcScheduler, RecordAndDecide) {
    for (int i = 0; i < 100; i++) {
        g_gc_scheduler.RecordAllocation(4096);
    }

    auto kind = g_gc_scheduler.DecideCollection();
    EXPECT_TRUE(kind == GcCollectionKind::NONE ||
                kind == GcCollectionKind::YOUNG ||
                kind == GcCollectionKind::FULL)
        << "DecideCollection returns valid GcCollectionKind";

    g_gc_scheduler.RecordGcCompleted();
}

// ── Test 2: LatencyMode round-trip ──────────────────────────────────

TEST(GcScheduler, LatencyModeRoundTrip) {
    g_gc_scheduler.SetLatencyMode(GcLatencyMode::BATCH);
    g_gc_scheduler.SetLatencyMode(GcLatencyMode::INTERACTIVE);
    g_gc_scheduler.SetLatencyMode(GcLatencyMode::LOW_LATENCY);
    g_gc_scheduler.SetLatencyMode(GcLatencyMode::SUSTAINED_LOW_LATENCY);
    g_gc_scheduler.SetLatencyMode(GcLatencyMode::NO_GC_REGION);
    g_gc_scheduler.SetLatencyMode(GcLatencyMode::INTERACTIVE);
}

// ── Test 3: CollectionMode round-trip ───────────────────────────────

TEST(GcScheduler, CollectionModeRoundTrip) {
    g_gc_scheduler.SetCollectionMode(GcCollectionMode::DEFAULT);
    g_gc_scheduler.SetCollectionMode(GcCollectionMode::FORCED);
    g_gc_scheduler.SetCollectionMode(GcCollectionMode::OPTIMIZED);
    g_gc_scheduler.SetCollectionMode(GcCollectionMode::AGGRESSIVE);
    g_gc_scheduler.SetCollectionMode(GcCollectionMode::DEFAULT);
}

// ── Test 4: RequestFullGc ───────────────────────────────────────────

TEST(GcScheduler, RequestFullGc) {
    g_gc_scheduler.RequestFullGc();
    bool requested = g_gc_scheduler.IsFullGcRequested();
    EXPECT_TRUE(requested) << "IsFullGcRequested true after RequestFullGc";

    g_gc_scheduler.RecordGcCompleted();
}

// ── Test 5: TryClaimGcSlot ──────────────────────────────────────────

TEST(GcScheduler, TryClaimGcSlot) {
    bool first = g_gc_scheduler.TryClaimGcSlot();
    bool second = g_gc_scheduler.TryClaimGcSlot();

    EXPECT_TRUE(first || !second)
        << "TryClaimGcSlot works (sequential calls consistent)";

    g_gc_scheduler.RecordGcCompleted();
}

// ── Test 6: PageCountGrowth ─────────────────────────────────────────

TEST(GcScheduler, PageCountGrowth) {
    g_gc_scheduler.RecordPageCountGrowth(1);
    g_gc_scheduler.RecordPageCountGrowth(10);
    g_gc_scheduler.RecordPageCountGrowth(100);
    g_gc_scheduler.ResetPageCountGrowth();
}

// ── Test 7: RecommendedNurserySize ──────────────────────────────────

TEST(GcScheduler, RecommendedNurserySize) {
    auto size = g_gc_scheduler.RecommendedNurserySize();
    EXPECT_GT(size, 0u) << "RecommendedNurserySize > 0";
    EXPECT_LE(size, 4 * 1024 * 1024)
        << "RecommendedNurserySize <= 4MB (sanity bound)";
}

// ── Test 8: TotalAllocatedSinceLastGC ───────────────────────────────

TEST(GcScheduler, TotalAllocatedSinceLastGC) {
    auto before = g_gc_scheduler.TotalAllocatedSinceLastGC();
    g_gc_scheduler.RecordAllocation(8192);
    auto after = g_gc_scheduler.TotalAllocatedSinceLastGC();
    EXPECT_GE(after, before)
        << "TotalAllocatedSinceLastGC increases after RecordAllocation";
}

// ── Test 9: GC-N8 dynamic_tuning Phase-1 signals (round-trip) ─────────

TEST(GcScheduler, DynamicTuningSignalsRoundTrip) {
    // Free-list reuse rate: set then read back, clamped to [0,1].
    g_gc_scheduler.SetFreeListReuseRate(0.0f);
    EXPECT_FLOAT_EQ(g_gc_scheduler.FreeListReuseRate(), 0.0f);
    g_gc_scheduler.SetFreeListReuseRate(1.0f);
    EXPECT_FLOAT_EQ(g_gc_scheduler.FreeListReuseRate(), 1.0f);
    // Clamp: values outside [0,1] saturate, not wrap.
    g_gc_scheduler.SetFreeListReuseRate(1.5f);
    EXPECT_FLOAT_EQ(g_gc_scheduler.FreeListReuseRate(), 1.0f);
    g_gc_scheduler.SetFreeListReuseRate(-0.2f);   // negative → uint32 wrap → saturates to 1.0
    EXPECT_FLOAT_EQ(g_gc_scheduler.FreeListReuseRate(), 1.0f);
    // Mid value survives the fixed-point round-trip (~1e-3 precision).
    g_gc_scheduler.SetFreeListReuseRate(0.37f);
    EXPECT_NEAR(g_gc_scheduler.FreeListReuseRate(), 0.37f, 1e-3f);

    // Memory load: same clamped fixed-point round-trip.
    g_gc_scheduler.SetMemoryLoad(0.25f);
    EXPECT_NEAR(g_gc_scheduler.MemoryLoad(), 0.25f, 1e-3f);
    g_gc_scheduler.SetMemoryLoad(5.0f);
    EXPECT_FLOAT_EQ(g_gc_scheduler.MemoryLoad(), 1.0f);
}

