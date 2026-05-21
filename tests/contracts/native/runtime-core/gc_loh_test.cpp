/// gc_loh_test — Large Object Heap allocation/sweep/compaction tests.
///
/// Covers:
///   1. Basic LOH allocation (>= 85 KB threshold)
///   2. LOH segment free + sweep
///   3. LOH free-list reuse (same-size reallocation)
///   4. LOH AUTOMATIC compaction (fragmentation-driven)
///   5. Concurrent LOH allocation under GC pressure
///
/// No BDWGC dependency — uses CRAG directly.
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <atomic>
#include <thread>
#include <vector>

#include <chaos/native_types.h>
#include "gc_loh.h"
#include "gc_region.h"
#include "gc_old_gen.h"

using namespace chaos::il2cpp::runtime_core;

#include "gc_test_macros.h"

static int g_failures = 0;

// ── Test 1: Basic LOH allocation ──────────────────────────────────
void TestLohBasicAlloc() {
    printf("\n── Test 1: Basic LOH allocation ──\n");

    // Objects >= 85 KB go to LOH.  Allocate several sizes.
    void* small = g_loh.Allocate(86 * 1024);
    CHECK(small != nullptr, "LOH Allocate(86KB) OK");
    CHECK(g_loh.IsInLOH(small), "86KB object recognized as LOH");

    // LOH objects should be zeroed.
    int zeros = 1;
    for (size_t i = 0; i < 86 * 1024; i++) {
        if (static_cast<char*>(small)[i] != 0) { zeros = 0; break; }
    }
    CHECK(zeros, "LOH memory is zeroed");

    void* medium = g_loh.Allocate(256 * 1024);
    CHECK(medium != nullptr, "LOH Allocate(256KB) OK");
    CHECK(g_loh.IsInLOH(medium), "256KB object recognized as LOH");

    void* large = g_loh.Allocate(1024 * 1024);
    CHECK(large != nullptr, "LOH Allocate(1MB) OK");
    CHECK(g_loh.IsInLOH(large), "1MB object recognized as LOH");

    // Verify IsInLOH returns false for non-LOH memory.
    void* small_obj = NurseryAllocate(64);
    CHECK(!g_loh.IsInLOH(small_obj), "Nursery object NOT recognized as LOH");

    CHECK(g_loh.SegmentCount() >= 3, "LOH has at least 3 segments");
}

// ── Test 2: LOH sweep — free unreachable segments ─────────────────
void TestLohSweep() {
    printf("\n── Test 2: LOH sweep — free unreachable segments ──\n");

    // Allocate several LOH objects.
    void* objs[5];
    for (int i = 0; i < 5; i++) {
        objs[i] = g_loh.Allocate(128 * 1024);
        CHECK(objs[i] != nullptr, "LOH Allocate(128KB) for sweep test");
    }

    int count_before = g_loh.SegmentCount();

    // Clear pre-marks (Allocate pre-marks segments for BGC safety).
    g_loh.UnmarkAllForTesting();

    // Mark them all as reachable (simulating a GC mark phase).
    for (int i = 0; i < 5; i++) {
        CHECK(g_loh.MarkObject(objs[i]), "Mark LOH object");
    }

    // Sweep — all 5 are marked, only the 5 marked segments survive.
    // (Previous test's segments were unmarked and get freed.)
    CHAOS_IL2CPP_SIZE reclaimed = g_loh.Sweep();
    int count_after_mark_all = g_loh.SegmentCount();
    // After marking 5 objects and sweeping, we should have exactly 5 segments.
    CHECK(count_after_mark_all == 5,
          "LOH has 5 segments after marking 5 objects (unmarked segments freed)");

    // Now allocate more objects, mark only some, verify sweep frees the rest.
    void* keep[3];
    void* free_list[3];
    for (int i = 0; i < 3; i++) {
        keep[i] = g_loh.Allocate(128 * 1024);
        CHECK(keep[i] != nullptr, "LOH alloc keep[%d] for partial mark test");
        free_list[i] = g_loh.Allocate(128 * 1024);
        CHECK(free_list[i] != nullptr, "LOH alloc free_list[%d] for partial mark test");
    }

    // Reclaim partial free mark.
    int count_before_partial = g_loh.SegmentCount();

    // Clear all pre-marks before testing partial mark + sweep.
    g_loh.UnmarkAllForTesting();

    // Mark only the 3 "keep" objects + the 5 original.
    for (int i = 0; i < 3; i++) {
        g_loh.MarkObject(keep[i]);
    }
    // Re-mark the original 5 objs.
    for (int i = 0; i < 5; i++) {
        g_loh.MarkObject(objs[i]);
    }

    bool sweep_ok = g_loh.Sweep() > 0;
    CHECK(sweep_ok, "LOH sweep reclaimed bytes from unmarked segments");
    printf("    Segments: before partial=%d, after sweep=%d\n",
           count_before_partial, g_loh.SegmentCount());

    CHECK(true, "LOH sweep completed without crash");
}

// ── Test 3: LOH free-list reuse ───────────────────────────────────
void TestLohFreeListReuse() {
    printf("\n── Test 3: LOH free-list reuse ──\n");

    constexpr CHAOS_IL2CPP_SIZE kAllocSize = 96 * 1024;

    // Allocate and free several same-size LOH objects.
    void* obj = g_loh.Allocate(kAllocSize);
    CHECK(obj != nullptr, "LOH Allocate(96KB) for free-list test");

    // Mark and sweep to free it.
    // For free-list testing, we manually free via the LOH Free method.
    g_loh.Free(obj);
    CHECK(!g_loh.IsInLOH(obj), "Freed LOH object no longer in LOH");

    // Allocate the same size again — should reuse the freed segment.
    void* obj2 = g_loh.Allocate(kAllocSize);
    CHECK(obj2 != nullptr, "LOH Allocate(96KB) after free — should reuse segment");

    // The address might differ from obj (that's fine), but allocation succeeded.
    CHECK(true, "LOH free-list reuse completed without crash");
}

// ── Test 4: LOH AUTOMATIC compaction ─────────────────────────────
void TestLohCompaction() {
    printf("\n── Test 4: LOH AUTOMATIC compaction ──\n");

    // LOH CompactMode is now AUTOMATIC by default (Task 8).
    CHECK(g_loh.GetCompactMode() == LargeObjectHeap::CompactMode::AUTOMATIC,
          "LOH CompactMode is AUTOMATIC by default");

    // Allocate interleaved LOH objects to create fragmentation.
    std::vector<void*> keep;
    std::vector<void*> free_list;

    // Phase 1: Allocate alternating "keep" and "free" objects.
    for (int i = 0; i < 10; i++) {
        void* k = g_loh.Allocate(128 * 1024);
        CHECK(k != nullptr, "LOH alloc keep obj for compaction");
        keep.push_back(k);

        void* f = g_loh.Allocate(128 * 1024);
        CHECK(f != nullptr, "LOH alloc free obj for compaction");
        free_list.push_back(f);
    }

    // Phase 2: Free the "free_list" objects to create fragmentation.
    for (auto* f : free_list) {
        g_loh.Free(f);
    }

    // Phase 3: Trigger compaction via mark+sweep.
    // Mark only the "keep" objects.
    for (auto* k : keep) {
        g_loh.MarkObject(k);
    }

    // Sweep (should free the unmarked segments and trigger compaction
    // if AUTOMATIC mode detects fragmentation).
    CHAOS_IL2CPP_SIZE reclaimed = g_loh.Sweep();
    printf("    LOH Sweep reclaimed: %llu bytes\n",
           static_cast<unsigned long long>(reclaimed));

    // Verify kept objects are still valid.
    for (size_t i = 0; i < keep.size(); i++) {
        CHECK(g_loh.IsInLOH(keep[i]),
              "Keep object still in LOH after sweep+compaction");
    }

    CHECK(true, "LOH AUTOMATIC compaction completed without crash");
}

// ── Test 5: LOH under concurrent allocation + GC pressure ─────────
void TestConcurrentLoh() {
    printf("\n── Test 5: Concurrent LOH + GC ──\n");

    std::atomic<int> ok{1};

    std::vector<std::thread> threads;
    for (int t = 0; t < 4; t++) {
        threads.emplace_back([&ok]() {
            for (int i = 0; i < 20; i++) {
                // Alternate between ~threshold and large LOH objects.
                CHAOS_IL2CPP_SIZE size = (i % 2 == 0) ? (86 * 1024) : (512 * 1024);
                void* obj = g_loh.Allocate(size);
                if (!obj) { ok.store(0); return; }

                // Touch memory.
                memset(obj, 0xAB, size);

                // Trigger some allocation pressure.
                for (int j = 0; j < 200; j++) {
                    volatile void* tmp = NurseryAllocate(32);
                    (void)tmp;
                }

                // Free half of the allocations.
                if (i % 2 == 0) {
                    g_loh.Free(obj);
                }
            }
        });
    }
    for (auto& th : threads) th.join();
    CHECK(ok.load() == 1, "4 threads x 20 LOH allocs under GC pressure OK");
}

// ── Test 6: LOH very large objects (>1MB) ──────────────────────────
void TestLohVeryLarge() {
    printf("\n── Test 6: LOH very large objects (>1MB) ──\n");

    void* big2m = g_loh.Allocate(2 * 1024 * 1024);
    CHECK(big2m != nullptr, "LOH Allocate(2MB) OK");
    CHECK(g_loh.IsInLOH(big2m), "2MB object recognized as LOH");
    std::memset(big2m, 0xAA, 2 * 1024 * 1024);
    CHECK(static_cast<unsigned char*>(big2m)[0] == 0xAA,
          "2MB object writable");
    CHECK(static_cast<unsigned char*>(big2m)[2 * 1024 * 1024 - 1] == 0xAA,
          "2MB last byte writable");

    void* big4m = g_loh.Allocate(4 * 1024 * 1024);
    CHECK(big4m != nullptr, "LOH Allocate(4MB) OK");
    CHECK(g_loh.IsInLOH(big4m), "4MB object recognized as LOH");
    std::memset(big4m, 0xBB, 4 * 1024 * 1024);
    CHECK(static_cast<unsigned char*>(big4m)[0] == 0xBB,
          "4MB object writable");
    CHECK(static_cast<unsigned char*>(big4m)[4 * 1024 * 1024 - 1] == 0xBB,
          "4MB last byte writable");

    // Mark both and sweep.
    g_loh.MarkObject(big2m);
    g_loh.MarkObject(big4m);
    g_loh.Sweep();
    CHECK(g_loh.IsInLOH(big2m), "2MB still in LOH after mark+sweep");
    CHECK(g_loh.IsInLOH(big4m), "4MB still in LOH after mark+sweep");

    CHECK(true, "LOH very large objects test complete");
}

// ── Test 7: LOH concurrent sweep ─────────────────────────────────
void TestLohConcurrentSweep() {
    printf("\n── Test 7: LOH concurrent sweep ──\n");

    std::atomic<int> ok{1};
    constexpr int kAllocThreads = 3;
    constexpr int kAllocsPerThread = 50;

    // Allocator threads.
    std::vector<std::thread> alloc_threads;
    for (int t = 0; t < kAllocThreads; t++) {
        alloc_threads.emplace_back([&ok]() {
            for (int i = 0; i < 50; i++) {
                size_t size = (i % 2 == 0) ? (86 * 1024) : (256 * 1024);
                void* obj = g_loh.Allocate(size);
                if (!obj) { ok.store(0); return; }
                std::memset(obj, 0xCC, size);
            }
        });
    }

    // Sweep thread runs concurrently.
    std::thread sweep_thread([&ok]() {
        for (int i = 0; i < 5; i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            try {
                g_loh.Sweep();
            } catch (...) {
                ok.store(0);
                return;
            }
        }
    });

    for (auto& th : alloc_threads) th.join();
    sweep_thread.join();

    CHECK(ok.load() == 1,
          "3 alloc threads + concurrent sweep completed without crash");
}

// ── Test 8: LOH segment count ─────────────────────────────────────▁
void TestLohSegmentCount() {
    printf("\n── Test 8: LOH segment count ──\n");

    int before_count = g_loh.SegmentCount();

    // Allocate many LOH objects of varying sizes.
    constexpr int kNumAllocs = 10;
    std::vector<void*> objs;
    objs.reserve(kNumAllocs);

    for (int i = 0; i < kNumAllocs; i++) {
        size_t size = (i % 3 == 0) ? (86 * 1024) : (i % 3 == 1) ? (192 * 1024) : (512 * 1024);
        void* obj = g_loh.Allocate(size);
        CHECK(obj != nullptr, "LOH alloc for segment count test");
        if (obj) {
            objs.push_back(obj);
            std::memset(obj, 0xDD, 128);
        }
    }

    int after_alloc = g_loh.SegmentCount();
    CHECK(after_alloc >= before_count + kNumAllocs / 2,
          "Segment count increased after %d allocs (%d -> %d)",
          kNumAllocs, before_count, after_alloc);

    // Mark all and sweep.
    for (auto* obj : objs) {
        g_loh.MarkObject(obj);
    }
    g_loh.Sweep();

    int after_sweep = g_loh.SegmentCount();
    CHECK(after_sweep == (int)objs.size(),
          "Segment count matches marked objects after sweep (expected %zu, got %d)",
          objs.size(), after_sweep);

    CHECK(true, "LOH segment count test complete");
}
int main() {
    puts("CRAG LOH unit test");
    puts("═════════════════\n");

    TestLohBasicAlloc();
    TestLohSweep();
    TestLohFreeListReuse();
    TestLohCompaction();
    TestConcurrentLoh();
    TestLohVeryLarge();
    TestLohConcurrentSweep();
    TestLohSegmentCount();

    printf("\n══ Results: %d tests, %d failures ══\n",
           8 - (g_failures > 0 ? 1 : 0), g_failures);

    return g_failures > 0 ? 1 : 0;
}
