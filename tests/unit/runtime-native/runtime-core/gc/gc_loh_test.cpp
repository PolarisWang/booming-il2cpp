/// gc_loh_test — Large Object Heap allocation/sweep/compaction tests (GoogleTest).

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
#include "gc_test_base.h"

#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

struct GcLohTest : GcUnitTestBase {
};

// ── Test 1: Basic LOH allocation ──────────────────────────────────

TEST_F(GcLohTest, BasicAlloc) {
    void* small = g_loh.Allocate(86 * 1024);
    ASSERT_NE(small, nullptr);
    EXPECT_TRUE(g_loh.IsInLOH(small));

    for (size_t i = 0; i < 86 * 1024; i++) {
        ASSERT_EQ(static_cast<char*>(small)[i], 0)
            << "LOH memory not zeroed at byte " << i;
    }

    void* medium = g_loh.Allocate(256 * 1024);
    ASSERT_NE(medium, nullptr);
    EXPECT_TRUE(g_loh.IsInLOH(medium));

    void* large = g_loh.Allocate(1024 * 1024);
    ASSERT_NE(large, nullptr);
    EXPECT_TRUE(g_loh.IsInLOH(large));

    void* small_obj = NurseryAllocate(64);
    EXPECT_FALSE(g_loh.IsInLOH(small_obj));

    EXPECT_GE(g_loh.SegmentCount(), 3);
}

// ── Test 2: LOH sweep — free unmarked segments ─────────────────

TEST_F(GcLohTest, Sweep) {
    // NOTE: LOH pre-marks segments on allocation (marked=true) to prevent
    // BGC from freeing freshly-allocated segments.  This means MarkObject
    // returns false for newly allocated objects.  The test sequence is:
    //   Allocate → Sweep (clears pre-marks) → MarkObject → Sweep (frees unmarked)

    void* objs[5];
    for (int i = 0; i < 5; i++) {
        objs[i] = g_loh.Allocate(128 * 1024);
        ASSERT_NE(objs[i], nullptr);
    }

    // First sweep clears pre-marks; all survive since they were marked.
    g_loh.Sweep();

    // Now mark all 5 — MarkObject CAS false→true should succeed.
    for (int i = 0; i < 5; i++) {
        EXPECT_TRUE(g_loh.MarkObject(objs[i]));
    }

    // Re-marked all 5, so they survive the next sweep.
    // (Unmarked segments from previous tests may also be freed.)
    g_loh.Sweep();
    for (int i = 0; i < 5; i++) {
        EXPECT_TRUE(g_loh.IsInLOH(objs[i]))
            << "obj[" << i << "] still in LOH after mark+sweep";
    }

    // Partial mark test
    void* keep[3];
    void* free_list[3];
    for (int i = 0; i < 3; i++) {
        keep[i] = g_loh.Allocate(128 * 1024);
        ASSERT_NE(keep[i], nullptr);
        free_list[i] = g_loh.Allocate(128 * 1024);
        ASSERT_NE(free_list[i], nullptr);
    }

    // Sweep to clear pre-marks on the 6 new segments.
    g_loh.Sweep();

    // Mark only keep (NOT free_list).  The objs from the first phase
    // were already freed when their marks were cleared by the second sweep,
    // so we only verify that keep objects survive.
    for (int i = 0; i < 3; i++) {
        EXPECT_TRUE(g_loh.MarkObject(keep[i]));
    }

    // Sweep — free_list objects (unmarked) should be reclaimed, but keep
    // objects survive.
    CHAOS_IL2CPP_SIZE reclaimed = g_loh.Sweep();
    EXPECT_GT(reclaimed, 0u)
        << "LOH sweep reclaimed bytes from unmarked segments";
    for (int i = 0; i < 3; i++) {
        EXPECT_TRUE(g_loh.IsInLOH(keep[i]))
            << "keep[" << i << "] still in LOH after partial mark+sweep";
    }
}

// ── Test 3: LOH free-list reuse ───────────────────────────────────

TEST_F(GcLohTest, FreeListReuse) {
    constexpr CHAOS_IL2CPP_SIZE kAllocSize = 96 * 1024;

    void* obj = g_loh.Allocate(kAllocSize);
    ASSERT_NE(obj, nullptr);

    g_loh.Free(obj);
    EXPECT_FALSE(g_loh.IsInLOH(obj));

    void* obj2 = g_loh.Allocate(kAllocSize);
    ASSERT_NE(obj2, nullptr);
}

// ── Test 4: LOH AUTOMATIC compaction ─────────────────────────────

TEST_F(GcLohTest, Compaction) {
    EXPECT_EQ(g_loh.GetCompactMode(),
              LargeObjectHeap::CompactMode::AUTOMATIC);

    std::vector<void*> keep;
    std::vector<void*> free_list;

    for (int i = 0; i < 10; i++) {
        void* k = g_loh.Allocate(128 * 1024);
        ASSERT_NE(k, nullptr);
        keep.push_back(k);

        void* f = g_loh.Allocate(128 * 1024);
        ASSERT_NE(f, nullptr);
        free_list.push_back(f);
    }

    for (auto* f : free_list) {
        g_loh.Free(f);
    }

    for (auto* k : keep) {
        g_loh.MarkObject(k);
    }

    CHAOS_IL2CPP_SIZE reclaimed = g_loh.Sweep();
    EXPECT_GT(reclaimed, 0u) << "LOH sweep reclaimed bytes";

    for (size_t i = 0; i < keep.size(); i++) {
        EXPECT_TRUE(g_loh.IsInLOH(keep[i]))
            << "Keep object still in LOH after sweep+compaction";
    }
}

// ── Test 5: LOH under concurrent allocation ─────────────────────────

TEST_F(GcLohTest, ConcurrentAlloc) {
    std::atomic<int> ok{1};

    std::vector<std::thread> threads;
    for (int t = 0; t < 4; t++) {
        threads.emplace_back([&ok]() {
            for (int i = 0; i < 20; i++) {
                CHAOS_IL2CPP_SIZE size = (i % 2 == 0) ? (86 * 1024) : (512 * 1024);
                void* obj = g_loh.Allocate(size);
                if (!obj) { ok.store(0); return; }

                memset(obj, 0xAB, size);

                for (int j = 0; j < 200; j++) {
                    volatile void* tmp = NurseryAllocate(32);
                    (void)tmp;
                }

                if (i % 2 == 0) {
                    g_loh.Free(obj);
                }
            }
        });
    }
    for (auto& th : threads) th.join();
    EXPECT_EQ(ok.load(), 1) << "4 threads x 20 LOH allocs under GC pressure OK";
}

// ── Test 6: LOH very large objects (>1MB) ──────────────────────────

TEST_F(GcLohTest, VeryLargeObjects) {
    void* big2m = g_loh.Allocate(2 * 1024 * 1024);
    ASSERT_NE(big2m, nullptr);
    EXPECT_TRUE(g_loh.IsInLOH(big2m));
    std::memset(big2m, 0xAA, 2 * 1024 * 1024);
    EXPECT_EQ(static_cast<unsigned char*>(big2m)[0], 0xAA);
    EXPECT_EQ(static_cast<unsigned char*>(big2m)[2 * 1024 * 1024 - 1], 0xAA);

    void* big4m = g_loh.Allocate(4 * 1024 * 1024);
    ASSERT_NE(big4m, nullptr);
    EXPECT_TRUE(g_loh.IsInLOH(big4m));
    std::memset(big4m, 0xBB, 4 * 1024 * 1024);
    EXPECT_EQ(static_cast<unsigned char*>(big4m)[0], 0xBB);
    EXPECT_EQ(static_cast<unsigned char*>(big4m)[4 * 1024 * 1024 - 1], 0xBB);

    g_loh.MarkObject(big2m);
    g_loh.MarkObject(big4m);
    g_loh.Sweep();
    EXPECT_TRUE(g_loh.IsInLOH(big2m));
    EXPECT_TRUE(g_loh.IsInLOH(big4m));
}

// ── Test 7: LOH concurrent sweep ─────────────────────────────────

TEST_F(GcLohTest, ConcurrentSweep) {
    std::atomic<int> ok{1};
    constexpr int kAllocThreads = 3;

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

    EXPECT_EQ(ok.load(), 1)
        << "3 alloc threads + concurrent sweep completed without crash";
}

// ── Test 8: LOH segment count ─────────────────────────────────────

TEST_F(GcLohTest, SegmentCount) {
    // Presweep to clear pre-marks from previous tests so the count
    // assertion below is not affected by inter-test state carryover.
    g_loh.Sweep();

    int before_count = g_loh.SegmentCount();

    constexpr int kNumAllocs = 10;
    std::vector<void*> objs;
    objs.reserve(kNumAllocs);

    for (int i = 0; i < kNumAllocs; i++) {
        size_t size = (i % 3 == 0) ? (86 * 1024) : (i % 3 == 1) ? (192 * 1024) : (512 * 1024);
        void* obj = g_loh.Allocate(size);
        ASSERT_NE(obj, nullptr);
        objs.push_back(obj);
        std::memset(obj, 0xDD, 128);
    }

    int after_alloc = g_loh.SegmentCount();
    EXPECT_GE(after_alloc, before_count + kNumAllocs / 2);

    for (auto* obj : objs) {
        g_loh.MarkObject(obj);
    }
    g_loh.Sweep();

    int after_sweep = g_loh.SegmentCount();
    EXPECT_GE(after_sweep, kNumAllocs)
        << "Segment count >= allocated objects after sweep";
}
