/// gc_debug_contract_test — GC debug contract unit tests.
///
/// Tests:
///   1. All extern "C" debug pointers are non-null
///   2. Debug pointers point to valid GC state objects (smoke checks)
///   3. Ring size matches kGcEventRingSize

#include <cstdio>
#include <cstdint>

#include "gc_debug_contract.h"
#include "gc_stats.h"
#include "gc_test_macros.h"

using namespace chaos::il2cpp::runtime_core;

static int g_failures = 0;
static int g_tests    = 0;
static int g_sub      = 0;

// ── Test 1: All debug contract pointers are non-null ──────────────
void TestDebugContractPointersNonNull() {
    TEST("DebugContractPointersNonNull");

    GC_CHECK(g_chaos_gc_stats_ptr != nullptr,
             "g_chaos_gc_stats_ptr is non-null (%p)", g_chaos_gc_stats_ptr);
    GC_CHECK(g_chaos_gc_event_ring_ptr != nullptr,
             "g_chaos_gc_event_ring_ptr is non-null (%p)", g_chaos_gc_event_ring_ptr);
    GC_CHECK(g_chaos_gc_event_ring_head_ptr != nullptr,
             "g_chaos_gc_event_ring_head_ptr is non-null (%p)", g_chaos_gc_event_ring_head_ptr);
    GC_CHECK(g_chaos_gc_pause_histogram_ptr != nullptr,
             "g_chaos_gc_pause_histogram_ptr is non-null (%p)", g_chaos_gc_pause_histogram_ptr);
    GC_CHECK(g_chaos_gc_pause_bucket_bounds_ptr != nullptr,
             "g_chaos_gc_pause_bucket_bounds_ptr is non-null (%p)", g_chaos_gc_pause_bucket_bounds_ptr);
}

// ── Test 2: Ring buffer size matches constant ────────────────────
void TestDebugContractRingSize() {
    TEST("DebugContractRingSize");

    GC_CHECK(g_chaos_gc_event_ring_size == kGcEventRingSize,
             "Ring size: %d == %d", g_chaos_gc_event_ring_size, kGcEventRingSize);
    GC_CHECK(g_chaos_gc_pause_bucket_count == kGcPauseBucketCount,
             "Bucket count: %d == %d", g_chaos_gc_pause_bucket_count, kGcPauseBucketCount);
}

// ── Test 3: Debug pointers point to the real global state ────────
void TestDebugContractPointsToRealState() {
    TEST("DebugContractPointsToRealState");

    // Verify stats pointer == &g_gc_stats.
    GC_CHECK(g_chaos_gc_stats_ptr == &g_gc_stats,
             "g_chaos_gc_stats_ptr points to g_gc_stats (%p == %p)",
             g_chaos_gc_stats_ptr, &g_gc_stats);

    // Verify ring pointer == &g_gc_event_ring[0].
    GC_CHECK(g_chaos_gc_event_ring_ptr == &g_gc_event_ring[0],
             "g_chaos_gc_event_ring_ptr points to ring[0] (%p == %p)",
             g_chaos_gc_event_ring_ptr, &g_gc_event_ring[0]);

    // Verify head pointer == &g_gc_event_ring_head.
    GC_CHECK(g_chaos_gc_event_ring_head_ptr == &g_gc_event_ring_head,
             "g_chaos_gc_event_ring_head_ptr points to head (%p == %p)",
             g_chaos_gc_event_ring_head_ptr, &g_gc_event_ring_head);

    // Verify histogram pointer.
    GC_CHECK(g_chaos_gc_pause_histogram_ptr == &g_gc_pause_histogram[0],
             "g_chaos_gc_pause_histogram_ptr points to histogram[0] (%p == %p)",
             g_chaos_gc_pause_histogram_ptr, &g_gc_pause_histogram[0]);

    // Verify bucket bounds: compare VALUES (not addresses) since
    // kGcPauseBucketsNs has static (internal) linkage — each TU has
    // its own copy at a different address.
    auto* bounds = static_cast<const uint64_t*>(g_chaos_gc_pause_bucket_bounds_ptr);
    bool bounds_match = true;
    for (int i = 0; i < kGcPauseBucketCount; i++) {
        if (bounds[i] != kGcPauseBucketsNs[i]) {
            bounds_match = false;
            break;
        }
    }
    GC_CHECK(bounds_match,
             "g_chaos_gc_pause_bucket_bounds_ptr values match");
}

// ── Test 4: Stats can be read via the debug pointer ──────────────
void TestDebugContractReadStats() {
    TEST("DebugContractReadStats");

    // Cast the debug pointer to GcStats* and verify we can read counters.
    auto* stats = static_cast<GcStats*>(g_chaos_gc_stats_ptr);

    // These should be valid values (>= 0, readable memory).
    uint64_t young_collections = stats->young_collections.load(std::memory_order_relaxed);
    uint64_t full_collections  = stats->full_collections.load(std::memory_order_relaxed);
    uint64_t alloc_total       = stats->alloc_total.load(std::memory_order_relaxed);

    GC_CHECK(true,
             "Debug stats readable: young=%llu full=%llu allocs=%llu",
             static_cast<unsigned long long>(young_collections),
             static_cast<unsigned long long>(full_collections),
             static_cast<unsigned long long>(alloc_total));
}

// ── Main ───────────────────────────────────────────────────────────
int main() {
    puts("CRAG GC debug contract test suite");
    puts("════════════════════════════════\n");

    TestDebugContractPointersNonNull();
    TestDebugContractRingSize();
    TestDebugContractPointsToRealState();
    TestDebugContractReadStats();

    printf("\n══ Results: %d tests, %d failures ══\n", g_tests, g_failures);
    return g_failures > 0 ? 1 : 0;
}
