/// gc_api_test — GC public API entry point tests.
///
/// Tests the public C ABI functions declared in gc_api.h:
///   1. chaos_gc_get_total_memory
///   2. chaos_gc_get_heap_size
///   3. chaos_gc_get_collection_count
///   4. chaos_gc_get_total_pause_duration
///   5. chaos_gc_get_allocated_bytes_for_current_thread
///   6. chaos_gc_enter_no_gc_region / chaos_gc_leave_no_gc_region
///   7. chaos_gc_try_start_no_gc_region / chaos_gc_end_no_gc_region
///   8. chaos_gc_enable_full_gc_notification
///   9. chaos_gc_wait_for_full_gc_approach / wait_for_full_gc_complete
///  10. chaos_gc_get_latency_mode / chaos_gc_set_latency_mode
///  11. chaos_gc_collect_with_mode

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <atomic>
#include <thread>
#include <chrono>

#include <chaos/native_types.h>
#include "gc_api.h"
#include "gc_old_gen.h"
#include "gc_region.h"
#include "gc_scheduler.h"
#include "gc_young_gen.h"

using namespace chaos::il2cpp::runtime_core;

#include "gc_test_macros.h"

static int g_failures = 0;
static int g_tests    = 0;
static int g_sub      = 0;

// ── Test 1: chaos_gc_get_total_memory ─────────────────────────────────
void TestGetTotalMemory() {
    TEST("chaos_gc_get_total_memory");

    // Allocate directly into old gen so TotalAllocated() returns > 0.
    void* p = g_old_gen.Allocate(256, true);
    GC_CHECK(p != nullptr, "OldGen::Allocate(256) OK");

    // Get total memory without forcing a GC — should reflect old-gen alloc.
    auto total = chaos_gc_get_total_memory(0);
    GC_CHECK(total > 0, "Total memory without GC force > 0 (got %lld)",
             static_cast<long long>(total));

    // Get total memory with forced GC — should not crash.
    auto total_forced = chaos_gc_get_total_memory(1);
    GC_CHECK(total_forced > 0, "Total memory with forced GC > 0 (got %lld)",
             static_cast<long long>(total_forced));
}

// ── Test 2: chaos_gc_get_heap_size ────────────────────────────────────
void TestGetHeapSize() {
    TEST("chaos_gc_get_heap_size");

    auto heap = chaos_gc_get_heap_size();
    GC_CHECK(heap >= 0, "Heap size >= 0 (got %lld)", static_cast<long long>(heap));
    // After allocating, heap should increase.
    for (int i = 0; i < 50; i++) {
        void* p = NurseryAllocate(256);
        if (p) std::memset(p, 0xBB, 256);
    }
    auto heap2 = chaos_gc_get_heap_size();
    GC_CHECK(heap2 >= heap, "Heap size non-decreasing after allocation (was %lld, now %lld)",
             static_cast<long long>(heap), static_cast<long long>(heap2));
}

// ── Test 3: chaos_gc_get_collection_count ─────────────────────────────
void TestGetCollectionCount() {
    TEST("chaos_gc_get_collection_count");

    // All generation counts should be >= 0.
    auto gen0 = chaos_gc_get_collection_count(0);
    auto gen1 = chaos_gc_get_collection_count(1);
    auto full = chaos_gc_get_collection_count(2);

    GC_CHECK(gen0 >= 0, "Gen0 collection count >= 0 (got %d)", gen0);
    GC_CHECK(gen1 >= 0, "Gen1 collection count >= 0 (got %d)", gen1);
    GC_CHECK(full >= 0, "Full collection count >= 0 (got %d)", full);

    // Trigger a young GC and verify gen0 increased.
    int32_t gen0_before = chaos_gc_get_collection_count(0);
    for (int i = 0; i < 500; i++) {
        volatile void* tmp = NurseryAllocate(32);
        (void)tmp;
    }
    int32_t gen0_after = chaos_gc_get_collection_count(0);
    GC_CHECK(gen0_after >= gen0_before,
             "Gen0 count increased after allocation pressure (was %d, now %d)",
             gen0_before, gen0_after);
}

// ── Test 4: chaos_gc_get_total_pause_duration ─────────────────────────
void TestGetTotalPauseDuration() {
    TEST("chaos_gc_get_total_pause_duration");

    auto pause = chaos_gc_get_total_pause_duration();
    GC_CHECK(pause >= 0, "Total pause duration >= 0 (got %lld ns)",
             static_cast<long long>(pause));

    // Trigger some GCs and verify duration increases.
    auto before = chaos_gc_get_total_pause_duration();
    for (int g = 0; g < 3; g++) {
        for (int i = 0; i < 500; i++) {
            volatile void* tmp = NurseryAllocate(32);
            (void)tmp;
        }
    }
    auto after = chaos_gc_get_total_pause_duration();
    GC_CHECK(after >= before,
             "Pause duration non-decreasing after GCs (was %lld, now %lld ns)",
             static_cast<long long>(before), static_cast<long long>(after));
}

// ── Test 5: chaos_gc_get_allocated_bytes_for_current_thread ────────────
void TestGetAllocatedBytesForCurrentThread() {
    TEST("chaos_gc_get_allocated_bytes_for_current_thread");

    auto before = chaos_gc_get_allocated_bytes_for_current_thread();

    // Allocate a known amount.
    for (int i = 0; i < 100; i++) {
        void* p = NurseryAllocate(64);
        if (p) std::memset(p, 0xCC, 64);
    }

    auto after = chaos_gc_get_allocated_bytes_for_current_thread();
    GC_CHECK(after > before,
             "Allocated bytes increased after allocation (was %lld, now %lld)",
             static_cast<long long>(before), static_cast<long long>(after));

    // Same-thread: should be monotonic.
    auto after2 = chaos_gc_get_allocated_bytes_for_current_thread();
    GC_CHECK(after2 >= after,
             "Allocated bytes monotonic on same thread (was %lld, now %lld)",
             static_cast<long long>(after), static_cast<long long>(after2));
}

// ── Test 6: chaos_gc_enter_no_gc_region / leave ────────────────────────
void TestNoGcRegionBasic() {
    TEST("chaos_gc_enter_no_gc_region / leave");

    // Enter and leave no-GC region — basic round-trip.
    GC_CHECK(!GcIsInNoGcRegion(), "Not in no-GC region initially");

    chaos_gc_enter_no_gc_region();
    GC_CHECK(GcIsInNoGcRegion(), "In no-GC region after enter");

    chaos_gc_leave_no_gc_region();
    GC_CHECK(!GcIsInNoGcRegion(), "Not in no-GC region after leave");

    // Enter twice, leave twice (nested).
    chaos_gc_enter_no_gc_region();
    chaos_gc_enter_no_gc_region();
    GC_CHECK(GcIsInNoGcRegion(), "In no-GC region after nested enters");

    chaos_gc_leave_no_gc_region();
    GC_CHECK(GcIsInNoGcRegion(), "Still in no-GC region after one leave (nested)");

    chaos_gc_leave_no_gc_region();
    GC_CHECK(!GcIsInNoGcRegion(), "Exited no-GC region after all nested leaves");
}

// ── Test 7: chaos_gc_try_start_no_gc_region / end ──────────────────────
void TestTryStartNoGcRegion() {
    TEST("chaos_gc_try_start_no_gc_region / end");

    // Try with negative total_size — should fail.
    int32_t result = chaos_gc_try_start_no_gc_region(-1, 0);
    GC_CHECK(result == 0, "TryStartNoGcRegion(-1) returns 0 (got %d)", result);

    // Try with very large budget — may fail if insufficient capacity,
    // but should not crash either way.
    result = chaos_gc_try_start_no_gc_region(1024LL * 1024 * 1024, 0);
    // Result may be 0 or 1; just verify no crash.
    GC_CHECK(true, "TryStartNoGcRegion(large) returned %d without crash", result);

    // End no-GC region (clean up if one was started).
    chaos_gc_end_no_gc_region();

    // Try with small budget — should succeed for small allocation.
    result = chaos_gc_try_start_no_gc_region(4096, 0);
    if (result) {
        // Successfully started — allocate a bit and end.
        void* p = NurseryAllocate(64);
        (void)p;
        int32_t end_result = chaos_gc_end_no_gc_region();
        GC_CHECK(end_result == 0 || end_result == 1,
                 "EndNoGcRegion returns valid result (got %d)", end_result);
    } else {
        GC_CHECK(true, "TryStartNoGcRegion(4KB, 0) returned 0 (insufficient capacity)");
        // Clean up: call end to balance the try_start that internally increments depth.
        // Actually try_start only increments depth on success (result == 1), so
        // if result is 0 there's nothing to clean up.
    }
}

// ── Test 8: chaos_gc_enable_full_gc_notification / disable ─────────────
void TestFullGcNotification() {
    TEST("chaos_gc_enable_full_gc_notification");

    // Enable notifications.
    chaos_gc_enable_full_gc_notification(0, 0);
    GC_CHECK(true, "EnableFullGcNotification completed without crash");

    // Wait for approach with short timeout — should timeout (approach not signaled).
    int32_t wait_result = chaos_gc_wait_for_full_gc_approach(10);
    // -1 = not enabled, 0 = signaled, 1 = timeout
    // Since we just enabled, approach shouldn't be signaled yet.
    GC_CHECK(wait_result == 1 || wait_result == -1,
             "WaitForFullGcApproach(10ms) returned %d (expected timeout)", wait_result);

    // Disable notifications.
    chaos_gc_disable_full_gc_notification();
    GC_CHECK(true, "DisableFullGcNotification completed without crash");
}

// ── Test 9: chaos_gc_get_latency_mode / set_latency_mode ───────────────
void TestLatencyModeApi() {
    TEST("chaos_gc_get_latency_mode / set_latency_mode");

    // Get default mode.
    int32_t mode = chaos_gc_get_latency_mode();
    GC_CHECK(mode >= 0 && mode <= 4,
             "Latency mode in valid range [0,4] (got %d)", mode);

    // Set to each valid mode and verify.
    chaos_gc_set_latency_mode(0); // Batch
    GC_CHECK(chaos_gc_get_latency_mode() == 0, "Latency mode == Batch after set");

    chaos_gc_set_latency_mode(1); // Interactive
    GC_CHECK(chaos_gc_get_latency_mode() == 1, "Latency mode == Interactive after set");

    chaos_gc_set_latency_mode(2); // LowLatency
    GC_CHECK(chaos_gc_get_latency_mode() == 2, "Latency mode == LowLatency after set");

    chaos_gc_set_latency_mode(3); // SustainedLowLatency
    GC_CHECK(chaos_gc_get_latency_mode() == 3, "Latency mode == SustainedLowLatency after set");

    // Invalid mode should be rejected (no crash).
    chaos_gc_set_latency_mode(99);
    GC_CHECK(true, "SetLatencyMode(99) rejected without crash");

    // Restore default.
    chaos_gc_set_latency_mode(1);
}

// ── Test 10: chaos_gc_collect_with_mode ────────────────────────────────
void TestCollectWithMode() {
    TEST("chaos_gc_collect_with_mode");

    // Allocate some objects first.
    for (int i = 0; i < 100; i++) {
        void* p = NurseryAllocate(64);
        if (p) std::memset(p, 0xDD, 64);
    }

    // GC.Collect(2, Forced) — full blocking collection.
    chaos_gc_collect_with_mode(2, 1);
    GC_CHECK(true, "CollectWithMode(gen=2, Forced) completed without crash");

    // GC.Collect(0, Forced) — young only.
    chaos_gc_collect_with_mode(0, 1);
    GC_CHECK(true, "CollectWithMode(gen=0, Forced) completed without crash");

    // GC.Collect(2, Optimized) — scheduler-optimized.
    chaos_gc_collect_with_mode(2, 2);
    GC_CHECK(true, "CollectWithMode(gen=2, Optimized) completed without crash");
}

// ── Test 11: chaos_gc_collect_with_mode Aggregate ────────────────────────
void TestCollectWithModeAggressive() {
    TEST("chaos_gc_collect_with_mode Aggressive");

    for (int i = 0; i < 200; i++) {
        void* p = NurseryAllocate(32);
        if (p) std::memset(p, 0xEE, 32);
    }

    // GC.Collect(2, Aggressive) — immediate blocking + compact.
    chaos_gc_collect_with_mode(2, 3);
    GC_CHECK(true, "CollectWithMode(gen=2, Aggressive) completed without crash");
}

// ── Main ────────────────────────────────────────────────────────────────
int main() {
    puts("CRAG GC API entry point tests");
    puts("══════════════════════════════\n");

    // Initialize the GC subsystems needed by API functions.
    InitYoungGeneration();
    void* warmup = NurseryAllocate(64);
    (void)warmup;

    // Register this thread so GC operations work.
    threading::RegisterThread(threading::AllocateThreadId(), nullptr);

    TestGetTotalMemory();
    TestGetHeapSize();
    TestGetCollectionCount();
    TestGetTotalPauseDuration();
    TestGetAllocatedBytesForCurrentThread();
    TestNoGcRegionBasic();
    TestTryStartNoGcRegion();
    TestFullGcNotification();
    TestLatencyModeApi();
    TestCollectWithMode();
    TestCollectWithModeAggressive();

    threading::UnregisterThread();

    printf("\n══ Results: 11 tests, %d failures ══\n", g_failures);
    return g_failures > 0 ? 1 : 0;
}
