/// gc_poh_test — Pinned Object Heap comprehensive tests (GoogleTest).
///
/// Uses CRAG directly. No BDWGC dependency.

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <atomic>
#include <thread>
#include <vector>

#include <chaos/native_types.h>
#include "gc_region.h"
#include "gc_events.h"
#include "gc_old_gen.h"
#include "gc_test_base.h"

#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

// Forward declarations from engine_lifecycle.h
namespace chaos { namespace il2cpp { namespace runtime_core {
CHAOS_IL2CPP_UINT64 GcCreateStrongHandle(void* object_instance) noexcept;
CHAOS_IL2CPP_UINT64 GcCreateWeakHandle(void* object_instance) noexcept;
void GcFreeHandle(CHAOS_IL2CPP_UINT64 handle_id) noexcept;
void* GcGetHandleTarget(CHAOS_IL2CPP_UINT64 handle_id) noexcept;
}}}

struct GcPohTest : GcUnitTestBase {
    void TearDown() override {
        // POH tests permanently allocate POH regions, so skip the region count
        // leak check (it's expected, not a leak). Only check thread count + TLAB.
        tls_tlab = TLAB{};
        const char* test_name = "?";
        if (auto* info = ::testing::UnitTest::GetInstance()->current_test_info()) {
            test_name = info->name();
        }
        int32_t tc = threading::GetThreadCount();
        EXPECT_EQ(tc, snapshot_.thread_count)
            << "[" << test_name << "] Thread leak detected";
        threading::UnregisterThread();
    }
};

// ── Test 1: Basic POH allocation ─────────────────────────────────

TEST_F(GcPohTest, BasicAlloc) {
    void* p = PohAllocate(64);
    ASSERT_NE(p, nullptr);
    EXPECT_TRUE(IsPohPointer(p));

    for (size_t i = 0; i < 64; i++) {
        EXPECT_EQ(static_cast<char*>(p)[i], 0) << "POH memory is zeroed at byte " << i;
    }

    std::memset(p, 0xAB, 64);
    EXPECT_EQ(static_cast<unsigned char*>(p)[0], 0xAB);

    void* p2 = PohAllocate(4096);
    ASSERT_NE(p2, nullptr);
    EXPECT_TRUE(IsPohPointer(p2));
    std::memset(p2, 0xCD, 4096);
    EXPECT_EQ(static_cast<unsigned char*>(p2)[0], 0xCD);
}

// ── Test 2: POH region overflow ───────────────────────────────────

TEST_F(GcPohTest, RegionOverflow) {
    constexpr int kChunkSize = 40 * 1024;
    constexpr int kNumChunks = 6;
    void* chunks[kNumChunks];

    for (int i = 0; i < kNumChunks; i++) {
        chunks[i] = PohAllocate(kChunkSize);
        ASSERT_NE(chunks[i], nullptr);
        std::memset(chunks[i], 0xAB + i, kChunkSize);
    }

    int region_count = RegionManager::Instance().GetPohRegionCount();
    EXPECT_GE(region_count, 2) << "Multiple POH regions created (count=" << region_count << ")";

    for (int i = 0; i < kNumChunks; i++) {
        auto* bytes = static_cast<unsigned char*>(chunks[i]);
        for (int j = 0; j < kChunkSize; j++) {
            ASSERT_EQ(bytes[j], static_cast<unsigned char>(0xAB + i))
                << "POH chunk " << i << " corruption at byte " << j;
        }
    }
}

// ── Test 3: POH + strong GCHandle ──────────────────────────────────

TEST_F(GcPohTest, StrongHandle) {
    void* poh_obj = PohAllocate(256);
    ASSERT_NE(poh_obj, nullptr);
    std::memset(poh_obj, 0xEF, 256);

    CHAOS_IL2CPP_UINT64 h = GcCreateStrongHandle(poh_obj);
    ASSERT_NE(h, 0u);

    poh_obj = nullptr;

    void* retrieved = GcGetHandleTarget(h);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(static_cast<unsigned char*>(retrieved)[0], 0xEF);

    GcFreeHandle(h);
}

// ── Test 4: POH + weak GCHandle ─────────────────────────────────────

TEST_F(GcPohTest, WeakHandle) {
    void* poh_obj = PohAllocate(128);
    ASSERT_NE(poh_obj, nullptr);
    std::memset(poh_obj, 0xBE, 128);

    CHAOS_IL2CPP_UINT64 h = GcCreateWeakHandle(poh_obj);
    ASSERT_NE(h, 0u);

    void* retrieved = GcGetHandleTarget(h);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(static_cast<unsigned char*>(retrieved)[0], 0xBE);

    GcFreeHandle(h);
}

// ── Test 5: POH address stability ──────────────────────────────────

TEST_F(GcPohTest, AddressStability) {
    constexpr int kCount = 30;
    void* addrs[kCount];
    for (int i = 0; i < kCount; i++) {
        addrs[i] = PohAllocate(64);
        ASSERT_NE(addrs[i], nullptr);
        std::memset(addrs[i], 0xCA + i, 64);
    }

    for (int i = 0; i < kCount; i++) {
        EXPECT_TRUE(IsPohPointer(addrs[i])) << "POH address region membership at " << i;
        EXPECT_EQ(static_cast<unsigned char*>(addrs[i])[0],
                  static_cast<unsigned char>(0xCA + i));
    }
}

// ── Test 6: POH oversized fallback ────────────────────────────────

TEST_F(GcPohTest, OversizedFallback) {
    void* big = PohAllocate(128 * 1024);
    ASSERT_NE(big, nullptr);
    EXPECT_FALSE(IsPohPointer(big)) << "128KB POH fallback is NOT in POH region";

    for (size_t i = 0; i < 128; i++) {
        EXPECT_EQ(static_cast<char*>(big)[i], 0) << "POH fallback zeroed at byte " << i;
    }

    std::memset(big, 0xDD, 128);
    EXPECT_EQ(static_cast<unsigned char*>(big)[0], 0xDD);
}

// ── Test 7: POH concurrent allocation ──────────────────────────────

TEST_F(GcPohTest, ConcurrentAlloc) {
    std::atomic<int> ok{1};

    std::vector<std::thread> threads;
    for (int t = 0; t < 8; t++) {
        threads.emplace_back([&ok]() {
            for (int i = 0; i < 100; i++) {
                void* p = PohAllocate(128);
                if (!p) { ok.store(0); return; }
                std::memset(p, 0xFD, 128);
            }
        });
    }
    for (auto& th : threads) th.join();
    EXPECT_EQ(ok.load(), 1) << "8 threads x 100 POH allocs OK";
}

// ── Test 8: POH + Full GC pressure ─────────────────────────────────

TEST_F(GcPohTest, FullGcPressure) {
    void* poh1 = PohAllocate(128);
    void* poh2 = PohAllocate(256);
    ASSERT_NE(poh1, nullptr);
    ASSERT_NE(poh2, nullptr);
    EXPECT_TRUE(IsPohPointer(poh1));
    EXPECT_TRUE(IsPohPointer(poh2));

    std::memset(poh1, 0x11, 128);
    std::memset(poh2, 0x22, 256);

    for (int g = 0; g < 3; g++) {
        for (int i = 0; i < 500; i++) {
            volatile void* tmp = NurseryAllocate(32);
            (void)tmp;
        }
    }

    EXPECT_TRUE(IsPohPointer(poh1));
    EXPECT_TRUE(IsPohPointer(poh2));
    EXPECT_EQ(static_cast<unsigned char*>(poh1)[0], 0x11);
    EXPECT_EQ(static_cast<unsigned char*>(poh2)[0], 0x22);
}

// ── Test 9: POH oversized at LOH boundary ──────────────────────────

TEST_F(GcPohTest, OversizedBoundary) {
    void* at_boundary = PohAllocate(85 * 1024);
    ASSERT_NE(at_boundary, nullptr);
    EXPECT_FALSE(IsPohPointer(at_boundary))
        << "85KB POH allocation falls back to old-gen";
    std::memset(at_boundary, 0xAA, 256);
    EXPECT_EQ(static_cast<unsigned char*>(at_boundary)[0], 0xAA);

    void* past_boundary = PohAllocate(85 * 1024 + 1);
    ASSERT_NE(past_boundary, nullptr);
    EXPECT_FALSE(IsPohPointer(past_boundary))
        << "85KB+1 POH allocation falls back to old-gen";
    std::memset(past_boundary, 0xBB, 256);
    EXPECT_EQ(static_cast<unsigned char*>(past_boundary)[0], 0xBB);
}
