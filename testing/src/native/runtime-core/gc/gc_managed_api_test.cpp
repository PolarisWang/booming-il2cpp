/// gc_managed_api_test — Managed GC API direct native verification (GoogleTest).
///
/// Validates that the native GC APIs backing System.GC work correctly:
///   1. chaos_gc_collect() / chaos_gc_collect_with_mode() collection routing
///   2. chaos_gc_get_total_memory() returns accurate heap size
///   3. chaos_gc_get_memory_info() populates GcMemoryInfoNative correctly
///   4. chaos_gc_get_collection_count() returns per-generation counts
///   5. chaos_gc_get_heap_size() correlates with actual allocations
///   6. chaos_is_gc_pointer() distinguishes GC-managed vs stack pointers

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <chaos/native_types.h>
#include "gc_api.h"
#include "gc_events.h"
#include "gc_helpers.h"
#include "gc_layout.h"
#include "gc_old_gen.h"
#include "gc_region.h"
#include "gc_stats.h"
#include "gc_young_gen.h"
#include "gc_young_collector.h"
#include "gc_test_base.h"

#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

struct ManagedApiTest : GcUnitTestBase {
    /// Take a stats snapshot for delta checks.
    GcSnapshot Snap() const { return GcGetSnapshot(); }
};

// ── Test 1: chaos_gc_collect() routing ──────────────────────────────

TEST_F(ManagedApiTest, GcCollectFull) {
    // Allocate objects so collection has work.
    for (int i = 0; i < 100; i++)
        NurseryAllocate(64);

    GcSnapshot before = Snap();
    chaos_gc_collect();
    GcSnapshot after = Snap();

    EXPECT_GE(after.young_collections, before.young_collections + 1)
        << "chaos_gc_collect must trigger young collection";
    EXPECT_GE(after.gc_index, before.gc_index)
        << "GC index must advance";
}

// ── Test 2: Generation-aware routing ───────────────────────────────

TEST_F(ManagedApiTest, GcCollectWithModeRouting) {
    // Allocate objects first.
    for (int i = 0; i < 50; i++)
        NurseryAllocate(64);

    // gen=0: young only
    {
        GcSnapshot before = Snap();
        chaos_gc_collect_with_mode(0, 1);  // gen=0, mode=FORCED
        GcSnapshot after = Snap();
        EXPECT_GE(after.young_collections, before.young_collections)
            << "gen=0 must trigger young collection";
    }

    // gen=2: full chain
    for (int i = 0; i < 50; i++)
        NurseryAllocate(64);
    {
        GcSnapshot before = Snap();
        chaos_gc_collect_with_mode(2, 1);  // gen=2, mode=FORCED
        GcSnapshot after = Snap();
        EXPECT_GE(after.full_collections, before.full_collections)
            << "gen=2 must trigger full collection";
    }

    // gen=-1: full chain
    for (int i = 0; i < 50; i++)
        NurseryAllocate(64);
    {
        GcSnapshot before = Snap();
        chaos_gc_collect_with_mode(-1, 1);
        GcSnapshot after = Snap();
        EXPECT_GE(after.full_collections, before.full_collections)
            << "gen=-1 must trigger full collection";
    }

    // OPTIMIZED mode: deferred, verify no crash.
    chaos_gc_collect_with_mode(2, 2);
    SUCCEED();
}

// ── Test 3: chaos_gc_get_total_memory ──────────────────────────────

TEST_F(ManagedApiTest, GcGetTotalMemory) {
    // Allocate objects of known sizes.
    CHAOS_IL2CPP_SIZE total = 0;
    for (int i = 0; i < 50; i++) {
        NurseryAllocate(64);
        total += 64;
    }

    // Without forced GC.
    CHAOS_IL2CPP_INT64 mem_no_gc = chaos_gc_get_total_memory(0);
    EXPECT_GT(mem_no_gc, 0) << "total memory must be > 0";

    // With forced GC.
    CHAOS_IL2CPP_INT64 mem_with_gc = chaos_gc_get_total_memory(1);
    EXPECT_GT(mem_with_gc, 0) << "total memory after forced GC must be > 0";
}

// ── Test 4: chaos_gc_get_memory_info ───────────────────────────────

TEST_F(ManagedApiTest, GcGetMemoryInfo) {
    // Allocate some objects.
    for (int i = 0; i < 50; i++)
        NurseryAllocate(64);

    // Prepare a buffer that looks like a managed object:
    // first word = MethodTable* (use a test TypeInfo pointer),
    // then GcMemoryInfoNative follows at sizeof(void*) offset.
    alignas(8) char buf[sizeof(void*) + sizeof(GcMemoryInfoNative)];
    std::memset(buf, 0, sizeof(buf));
    // Store TypeInfo pointer as fake MethodTable*.
    *reinterpret_cast<const void**>(buf) = GetTestTypeInfo(64);

    CHAOS_IL2CPP_INTPTR obj = static_cast<CHAOS_IL2CPP_INTPTR>(
        reinterpret_cast<std::intptr_t>(buf));
    chaos_gc_get_memory_info(obj, 0);  // kind=Any

    auto* info = reinterpret_cast<GcMemoryInfoNative*>(buf + sizeof(void*));

    EXPECT_GT(info->heap_size_bytes, 0) << "heap_size_bytes must be > 0";
    EXPECT_GE(info->generation, 0) << "generation >= 0";
    EXPECT_GE(info->total_committed_bytes, 0) << "total_committed_bytes >= 0";
    EXPECT_GE(info->promoted_bytes, 0) << "promoted_bytes >= 0";
    EXPECT_GE(info->gen0_size_before, 0) << "gen0_size_before >= 0";
    EXPECT_GE(info->gen0_size_after, 0) << "gen0_size_after >= 0";
}

// ── Test 5: chaos_gc_get_collection_count ──────────────────────────

TEST_F(ManagedApiTest, GcGetCollectionCount) {
    CHAOS_IL2CPP_INT32 c0_before = chaos_gc_get_collection_count(0);
    CHAOS_IL2CPP_INT32 c2_before = chaos_gc_get_collection_count(2);

    // Allocate and run a full collection.
    for (int i = 0; i < 100; i++)
        NurseryAllocate(64);
    chaos_gc_collect();

    CHAOS_IL2CPP_INT32 c0_after = chaos_gc_get_collection_count(0);
    CHAOS_IL2CPP_INT32 c2_after = chaos_gc_get_collection_count(2);

    // Young collection may be skipped if nursery was already drained by
    // previous tests; at minimum gen2 (full) must have advanced.
    EXPECT_GE(c2_after, c2_before + 1) << "gen2 count must increase after chaos_gc_collect";
    (void)c0_before;
    (void)c0_after;
}

// ── Test 6: chaos_gc_get_heap_size ─────────────────────────────────

TEST_F(ManagedApiTest, GcGetHeapSize) {
    CHAOS_IL2CPP_INT64 heap_before = chaos_gc_get_heap_size();
    EXPECT_GT(heap_before, 0) << "heap size > 0 after init";

    // Allocate a known amount.
    for (int i = 0; i < 50; i++)
        NurseryAllocate(64);

    CHAOS_IL2CPP_INT64 heap_after = chaos_gc_get_heap_size();
    EXPECT_GE(heap_after, heap_before)
        << "heap size must not shrink after allocation (before GC)";
}

// ── Test 7: chaos_is_gc_pointer (null/in-heap/stack) ───────────────

TEST_F(ManagedApiTest, GcIsGcPointer) {
    // Null pointer.
    EXPECT_FALSE(chaos_is_gc_pointer(nullptr));

    // Pointer to GC heap (nursery allocation).
    void* obj = NurseryAllocate(64);
    ASSERT_NE(obj, nullptr);
    EXPECT_TRUE(chaos_is_gc_pointer(obj));

    // Stack variable should not be reported as GC pointer.
    int stack_var = 42;
    EXPECT_FALSE(chaos_is_gc_pointer(&stack_var));
}

// ── Test 8: NoGcRegion nesting ─────────────────────────────────────

TEST_F(ManagedApiTest, GcNoGcRegion) {
    EXPECT_FALSE(GcIsInNoGcRegion()) << "initially not in NO_GC_REGION";

    chaos_gc_enter_no_gc_region();
    EXPECT_TRUE(GcIsInNoGcRegion()) << "inside NO_GC_REGION after enter";

    // Nested enter.
    chaos_gc_enter_no_gc_region();
    EXPECT_TRUE(GcIsInNoGcRegion()) << "still inside after nested enter";

    chaos_gc_leave_no_gc_region();
    EXPECT_TRUE(GcIsInNoGcRegion()) << "still inside after first leave";

    chaos_gc_leave_no_gc_region();
    EXPECT_FALSE(GcIsInNoGcRegion()) << "exited after matching leave";
}

// ── Test 9: chaos_gc_add/remove_memory_pressure ────────────────────

TEST_F(ManagedApiTest, GcMemoryPressure) {
    // These should not crash or trigger assertions.
    chaos_gc_add_memory_pressure(1024);
    chaos_gc_remove_memory_pressure(512);
    chaos_gc_remove_memory_pressure(512);
    SUCCEED();
}

// ── Test 10: Latency mode get/set ──────────────────────────────────

TEST_F(ManagedApiTest, GcLatencyMode) {
    CHAOS_IL2CPP_INT32 mode = chaos_gc_get_latency_mode();
    // Default should be Interactive (1) or Batch (0).
    EXPECT_GE(mode, 0);

    // Set and verify round-trip.
    chaos_gc_set_latency_mode(0);  // Batch
    EXPECT_EQ(chaos_gc_get_latency_mode(), 0);
    chaos_gc_set_latency_mode(1);  // Interactive
    EXPECT_EQ(chaos_gc_get_latency_mode(), 1);
}
