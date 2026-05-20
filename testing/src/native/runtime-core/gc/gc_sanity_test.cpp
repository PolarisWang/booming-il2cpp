/// gc_sanity_test — Quick sanity test for POH, ThinLock, GCMemoryInfo (GoogleTest).

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <atomic>
#include <thread>
#include <vector>

#include <chaos/native_types.h>
#include "gc_api.h"
#include "gc_region.h"
#include "gc_stats.h"
#include "gc_events.h"
#include "domain_unloader.h"
#include "thread_state.h"
#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

struct GcSanityTest : ::testing::Test {
    void SetUp() override {
        tid = threading::AllocateThreadId();
        threading::RegisterThread(tid, nullptr);
    }

    void TearDown() override {
        threading::UnregisterThread();
    }

    uint32_t tid;
};

TEST_F(GcSanityTest, PohAllocate) {
    void* p = PohAllocate(64);
    ASSERT_NE(p, nullptr);

    void* p2 = PohAllocate(4096);
    ASSERT_NE(p2, nullptr);

    EXPECT_TRUE(IsPohPointer(p));
    EXPECT_TRUE(IsPohPointer(p2));

    int zeros = 1;
    for (size_t i = 0; i < 64; i++) {
        if (static_cast<char*>(p)[i] != 0) { zeros = 0; break; }
    }
    EXPECT_TRUE(zeros);

    void* big = PohAllocate(128 * 1024);
    ASSERT_NE(big, nullptr);
    EXPECT_FALSE(IsPohPointer(big));
}

TEST_F(GcSanityTest, GcAllocatePinned) {
    void* p = GcAllocatePinned(256);
    ASSERT_NE(p, nullptr);
    EXPECT_TRUE(IsPohPointer(p) || GcIsPohPointer(p));

    void* p2 = GcAllocatePinned(32 * 1024);
    ASSERT_NE(p2, nullptr);

    std::atomic<int> ok{1};
    std::vector<std::thread> threads;
    for (int t = 0; t < 10; t++) {
        threads.emplace_back([&ok]() {
            threading::RegisterThread(threading::AllocateThreadId(), nullptr);
            for (int i = 0; i < 100; i++) {
                void* pp = GcAllocatePinned(64);
                if (!pp) { ok.store(0); break; }
                memset(pp, 0xCD, 64);
            }
            threading::UnregisterThread();
        });
    }
    for (auto& th : threads) th.join();
    EXPECT_EQ(ok.load(), 1);
}

TEST_F(GcSanityTest, LockDrainNoLocks) {
    DrainSyncBlocksForDomain(9999);
    SUCCEED();
}

TEST_F(GcSanityTest, LockDrainWithDomain) {
    CHAOS_IL2CPP_UINT32 domain_id = 42;
    void* dmem = DomainAllocate(domain_id, 128);
    ASSERT_NE(dmem, nullptr);

    DrainSyncBlocksForDomain(domain_id);
    SUCCEED();
}

TEST_F(GcSanityTest, ConcurrentPohDomain) {
    std::atomic<int> ok{1};
    std::vector<std::thread> threads;

    for (int t = 0; t < 8; t++) {
        threads.emplace_back([&ok, t]() {
            threading::RegisterThread(threading::AllocateThreadId(), nullptr);
            for (int i = 0; i < 200; i++) {
                void* pp = PohAllocate(128);
                if (!pp) { ok.store(0); break; }
                memset(pp, 0xEF, 128);

                void* dm = DomainAllocate(t + 100, 64);
                if (!dm) { ok.store(0); break; }

                void* nr = NurseryAllocate(32);
                if (!nr) { ok.store(0); break; }
            }
            threading::UnregisterThread();
        });
    }
    for (auto& th : threads) th.join();
    EXPECT_EQ(ok.load(), 1);
}

TEST_F(GcSanityTest, LockDrainMultiDomain) {
    for (int d = 0; d < 20; d++) {
        void* dm = DomainAllocate(d, 64);
        (void)dm;
    }

    for (int d = 0; d < 20; d++) {
        DrainSyncBlocksForDomain(static_cast<CHAOS_IL2CPP_UINT32>(d));
    }
    SUCCEED();
}

TEST_F(GcSanityTest, GcMemoryInfo) {
    auto snap0 = GcGetSnapshot();
    EXPECT_GE(snap0.alloc_total, 0);

    for (int i = 0; i < 10000; i++) {
        void* p = NurseryAllocate(128);
        ASSERT_NE(p, nullptr);
        memset(p, 0xBB, 128);
    }

    auto snap1 = GcGetSnapshot();
    EXPECT_GT(snap1.alloc_total, snap0.alloc_total);

    alignas(16) char buf[sizeof(void*) + 256];
    memset(buf, 0, sizeof(buf));
    auto obj_ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(buf);
    chaos_gc_get_memory_info(obj_ptr, 0);
    auto* info = reinterpret_cast<const GcMemoryInfoNative*>(buf + sizeof(void*));

    EXPECT_GT(info->heap_size_bytes, 0u);
    EXPECT_GT(info->total_available_memory_bytes, 0u);
    EXPECT_EQ(info->generation, 1);
}
