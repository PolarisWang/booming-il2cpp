/// gc_bgc_smoke_test — BGC smoke test (GoogleTest).
///
/// 6 scenarios testing basic BGC cycle lifecycle, concurrent allocation,
/// force-complete, phase queries, multiple cycles, and young-GC interop.

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

#include <chaos/native_types.h>
#include "gc_bgc.h"
#include "gc_region.h"
#include "gc_scheduler.h"
#include "gc_old_gen.h"
#include "gc_young_collector.h"
#include "gc_young_gen.h"
#include "thread_state.h"
#include "gc_test_base.h"

#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

namespace {

static bool WaitForPhase(BgcPhase phase, int timeout_ms = 30000) {
    auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms);
    while (std::chrono::steady_clock::now() < deadline) {
        if (BgcController::Instance().Phase() == phase) return true;
        std::this_thread::yield();
    }
    return false;
}

static void RunBgcCycle() {
    uint32_t gen = threading::RequestGlobalSafepoint();
    BgcController::Instance().StartBgcCycle();
    threading::ReleaseGlobalSafepoint(gen);

    ASSERT_TRUE(WaitForPhase(BgcPhase::REMARK_NEEDED)) << "Timeout waiting for REMARK_NEEDED";

    gen = threading::RequestGlobalSafepoint();
    BgcController::Instance().StwRemark();
    BgcController::Instance().StartConcurrentSweep();
    threading::ReleaseGlobalSafepoint(gen);

    ASSERT_TRUE(WaitForPhase(BgcPhase::COMPACT_NEEDED)) << "Timeout waiting for COMPACT_NEEDED";

    gen = threading::RequestGlobalSafepoint();
    BgcController::Instance().StwCompact();
    threading::ReleaseGlobalSafepoint(gen);

    BgcController::Instance().WaitForCycleComplete();
    EXPECT_EQ(BgcController::Instance().Phase(), BgcPhase::IDLE);
}

static void* AllocOldGen(size_t payload_size) {
    return NurseryAllocate(payload_size);
}

struct BgcSmokeTest : GcStressTestBase {
    void SetUp() override {
        GcStressTestBase::SetUp();
        if (!s_bgc_started.load()) {
            NurseryAllocate(64);  // warmup
            BgcController::Instance().Start();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            s_bgc_started.store(true);
        }
    }
    void TearDown() override {
        GcStressTestBase::TearDown();
    }

    static std::atomic<bool> s_bgc_started;
};

std::atomic<bool> BgcSmokeTest::s_bgc_started{false};

} // anonymous namespace

TEST_F(BgcSmokeTest, BasicBgcCycle) {
    for (int i = 0; i < 10; i++)
        AllocOldGen(64 + i * 8);

    RunBgcCycle();
    RunBgcCycle();  // re-entrancy
    SUCCEED();
}

TEST_F(BgcSmokeTest, BgcWithAllocation) {
    std::atomic<bool> done{false};
    std::thread worker([&done]() {
        threading::RegisterThread(threading::AllocateThreadId(), nullptr);
        for (int i = 0; i < 5000; i++) {
            volatile void* p = NurseryAllocate(32);
            (void)p;
            if ((i % 100) == 0) {
                AllocOldGen(64);
            }
        }
        threading::UnregisterThread();
        done.store(true);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    uint32_t gen = threading::RequestGlobalSafepoint();
    BgcController::Instance().StartBgcCycle();
    threading::ReleaseGlobalSafepoint(gen);

    WaitForPhase(BgcPhase::REMARK_NEEDED);
    gen = threading::RequestGlobalSafepoint();
    BgcController::Instance().StwRemark();
    BgcController::Instance().StartConcurrentSweep();
    threading::ReleaseGlobalSafepoint(gen);

    WaitForPhase(BgcPhase::COMPACT_NEEDED);
    gen = threading::RequestGlobalSafepoint();
    BgcController::Instance().StwCompact();
    threading::ReleaseGlobalSafepoint(gen);

    BgcController::Instance().WaitForCycleComplete();
    worker.join();
    EXPECT_TRUE(done.load());
}

TEST_F(BgcSmokeTest, BgcForceComplete) {
    uint32_t gen = threading::RequestGlobalSafepoint();
    BgcController::Instance().StartBgcCycle();
    threading::ReleaseGlobalSafepoint(gen);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    gen = threading::RequestGlobalSafepoint();
    BgcController::Instance().ForceComplete();
    threading::ReleaseGlobalSafepoint(gen);

    EXPECT_FALSE(BgcController::Instance().IsBusy());
    EXPECT_EQ(BgcController::Instance().Phase(), BgcPhase::IDLE);
}

TEST_F(BgcSmokeTest, BgcIsBusyIsMarking) {
    EXPECT_FALSE(BgcController::Instance().IsBusy());
    EXPECT_FALSE(BgcController::Instance().IsMarking());

    uint32_t gen = threading::RequestGlobalSafepoint();
    BgcController::Instance().StartBgcCycle();
    threading::ReleaseGlobalSafepoint(gen);

    ASSERT_TRUE(WaitForPhase(BgcPhase::CONCURRENT_MARK));
    EXPECT_TRUE(BgcController::Instance().IsBusy());
    EXPECT_TRUE(BgcController::Instance().IsMarking());

    gen = threading::RequestGlobalSafepoint();
    BgcController::Instance().ForceComplete();
    threading::ReleaseGlobalSafepoint(gen);

    EXPECT_FALSE(BgcController::Instance().IsBusy());
    EXPECT_FALSE(BgcController::Instance().IsMarking());
}

TEST_F(BgcSmokeTest, BgcMultipleCycles) {
    for (int c = 0; c < 2; c++) {
        for (int i = 0; i < 5; i++)
            AllocOldGen(64);
        RunBgcCycle();
    }
    SUCCEED();
}

TEST_F(BgcSmokeTest, BgcWithYoungGc) {
    uint32_t gen = threading::RequestGlobalSafepoint();
    BgcController::Instance().StartBgcCycle();
    threading::ReleaseGlobalSafepoint(gen);

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 50; j++) {
            volatile void* p = NurseryAllocate(32);
            (void)p;
        }
        gen = threading::RequestGlobalSafepoint();
        GcYoungCollection();
        threading::ReleaseGlobalSafepoint(gen);
    }

    gen = threading::RequestGlobalSafepoint();
    BgcController::Instance().ForceComplete();
    threading::ReleaseGlobalSafepoint(gen);

    SUCCEED();
}
