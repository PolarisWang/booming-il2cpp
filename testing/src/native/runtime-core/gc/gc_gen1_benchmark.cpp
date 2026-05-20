/// gc_gen1_benchmark.cpp — Gen1 collection benchmark (GoogleTest).
///
/// Measures Gen1 STW pause time at various survival rates.
/// Run with:
///   test_gc_gen1_benchmark --gtest_filter="*Gen1Benchmark*"
///
/// Target: 98% survival + 98% occupancy < 10ms

#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include <chaos/native_types.h>
#include "gc_gen1.h"
#include "gc_scheduler.h"
#include "gc_young_gen.h"
#include "gc_test_base.h"

#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

// ═══════════════════════════════════════════════════════════════════════════
// Benchmark: Gen1 at 98% occupancy with 100% survival
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(GcBenchTestBase, Gen1Benchmark_98PercentOccupancy) {
    // Fill survivor to 98% capacity with 1024-byte objects.
    FillSurvivorTo(0.98f);

    // Measure Gen1 collection pause.
    uint64_t ns = MeasureGen1Collection();

    printf("  [Bench] 98%% occupancy Gen1 collection: %llu ns (target < 10ms)\n",
           static_cast<unsigned long long>(ns));

    // Target: < 10ms for 98% survival + 98% occupancy.
    EXPECT_LT(ns, 10'000'000ULL) << "Gen1 collection exceeded 10ms target";
}

// ═══════════════════════════════════════════════════════════════════════════
// Benchmark: Gen1 at 50% occupancy (partial fill, moderate load)
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(GcBenchTestBase, Gen1Benchmark_50PercentOccupancy) {
    FillSurvivorTo(0.50f);

    uint64_t ns = MeasureGen1Collection();

    printf("  [Bench] 50%% occupancy Gen1 collection: %llu ns\n",
           static_cast<unsigned long long>(ns));

    // At 50% occupancy, target should be well under 5ms.
    EXPECT_LT(ns, 5'000'000ULL) << "Gen1 collection at 50% exceeded 5ms target";
}

// ═══════════════════════════════════════════════════════════════════════════
// Benchmark: Gen1 empty (zero survivor objects)
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(GcBenchTestBase, Gen1Benchmark_Empty) {
    // Ensure Gen1 is cleared first.
    GcGen1Collection();

    uint64_t ns = MeasureGen1Collection();

    printf("  [Bench] Empty Gen1 collection: %llu ns\n",
           static_cast<unsigned long long>(ns));

    // Empty Gen1 should complete extremely fast (< 100μs).
    EXPECT_LT(ns, 100'000ULL) << "Empty Gen1 collection exceeded 100μs target";
}
