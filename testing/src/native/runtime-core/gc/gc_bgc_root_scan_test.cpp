/// gc_bgc_root_scan_test — BGC root scan test (GoogleTest).
///
/// Validates young-to-old references survive BGC via root scanning.

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
#include "gc_old_gen.h"
#include "gc_young_gen.h"
#include "gc_layout.h"
#include "thread_state.h"
#include "gc_test_base.h"

#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

namespace {

struct alignas(8) TestTypeInfo {
    uint64_t stable_id;
    uint64_t reserved[3];
};

static TestTypeInfo* SetupTestType(uint32_t instance_size) {
    uint64_t sid = GcLayoutRegistry::Instance().RegisterOrGetRawAllocType(instance_size);
    auto* ti = new TestTypeInfo();
    ti->stable_id = sid;
    uintptr_t addr = reinterpret_cast<uintptr_t>(ti);
    GcLayoutRegistry::Instance().RegisterTypeInfoRange(addr, addr + sizeof(TestTypeInfo));
    EXPECT_TRUE(GcLayoutRegistry::Instance().IsValidTypeInfoPointer(ti));
    EXPECT_EQ(GcLayoutRegistry::Instance().ReadStableId(ti), sid);
    return ti;
}

static void MyInitTestObject(void* obj, const void* type_info, uint32_t pattern) {
    *static_cast<const void**>(obj) = type_info;
    *reinterpret_cast<uint32_t*>(static_cast<char*>(obj) + 8) = pattern;
}

static bool MyCheckTestObjectAlive(void* obj, uint32_t pattern) {
    auto val = *reinterpret_cast<volatile uint32_t*>(static_cast<char*>(obj) + 8);
    return val == pattern;
}

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

    ASSERT_TRUE(WaitForPhase(BgcPhase::REMARK_NEEDED));

    gen = threading::RequestGlobalSafepoint();
    BgcController::Instance().StwRemark();
    BgcController::Instance().StartConcurrentSweep();
    threading::ReleaseGlobalSafepoint(gen);

    ASSERT_TRUE(WaitForPhase(BgcPhase::COMPACT_NEEDED));

    gen = threading::RequestGlobalSafepoint();
    BgcController::Instance().StwCompact();
    threading::ReleaseGlobalSafepoint(gen);

    BgcController::Instance().WaitForCycleComplete();
}

struct BgcRootScanTest : GcStressTestBase {
    static TestTypeInfo* type_info_64;

    static bool s_bgc_started;

    void SetUp() override {
        GcStressTestBase::SetUp();
        if (!s_bgc_started) {
            type_info_64 = SetupTestType(64);
            NurseryAllocate(64);
            BgcController::Instance().Start();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            s_bgc_started = true;
        }
    }
    void TearDown() override {
        GcStressTestBase::TearDown();
    }
};

bool BgcRootScanTest::s_bgc_started = false;
TestTypeInfo* BgcRootScanTest::type_info_64 = nullptr;


} // anonymous namespace

TEST_F(BgcRootScanTest, YoungToOldRootScan) {
    // Allocate old-gen object.
    void* old_obj = g_old_gen.Allocate(64, true);
    ASSERT_NE(old_obj, nullptr);
    ASSERT_TRUE(g_old_gen.IsInOldGen(old_obj));
    MyInitTestObject(old_obj, type_info_64, 0xCAFEBABE);

    // Allocate young-gen object holding reference to old_obj.
    void* young_obj = NurseryAllocate(64);
    ASSERT_NE(young_obj, nullptr);
    MyInitTestObject(young_obj, type_info_64, 0xDEADBEAF);
    *reinterpret_cast<void**>(static_cast<char*>(young_obj) + 16) = old_obj;

    // Clear stack reference to old_obj — young obj still references it.
    RunBgcCycle();

    EXPECT_TRUE(MyCheckTestObjectAlive(old_obj, 0xCAFEBABE))
        << "Old-gen object should survive BGC via young-gen root scan";
    EXPECT_TRUE(g_old_gen.IsInOldGen(old_obj));
}

TEST_F(BgcRootScanTest, CrossThreadYoungToOld) {
    void* old_obj = g_old_gen.Allocate(64, true);
    ASSERT_NE(old_obj, nullptr);
    ASSERT_TRUE(g_old_gen.IsInOldGen(old_obj));
    MyInitTestObject(old_obj, type_info_64, 0xFACEFEED);

    std::atomic<bool> worker_done{false};
    std::thread worker([&]() {
        threading::RegisterThread(threading::AllocateThreadId(), nullptr);
        void* young_obj = NurseryAllocate(64);
        ASSERT_NE(young_obj, nullptr);
        *reinterpret_cast<void**>(static_cast<char*>(young_obj) + 16) = old_obj;
        // Keep young_obj on stack (volatile to prevent optimization).
        volatile auto* keep = static_cast<void* volatile*>(&young_obj);
        (void)keep;
        while (!worker_done.load()) {
            threading::SafepointPoll();
            std::this_thread::yield();
        }
        threading::UnregisterThread();
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    RunBgcCycle();

    worker_done.store(true);
    worker.join();

    EXPECT_TRUE(MyCheckTestObjectAlive(old_obj, 0xFACEFEED))
        << "Old-gen object should survive BGC via cross-thread young-gen root scan";
    EXPECT_TRUE(g_old_gen.IsInOldGen(old_obj));
}
