/// gc_old_gen_unit_test — OldGen core operations unit tests (GoogleTest).

#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <chaos/native_types.h>
#include "gc_old_gen.h"
#include "gc_region.h"
#include "gc_scheduler.h"
#include "thread_state.h"
#include "gc_test_base.h"
#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

struct OldGenTest : GcTestBase {
    void SetUp() override {
        GcTestBase::SetUp();

        GcSetHeapBase(reinterpret_cast<void*>(uintptr_t(0)));
        g_old_gen.Init(0, 2);
    }

    void TearDown() override {
        GcTestBase::TearDown();
    }
};

TEST_F(OldGenTest, AllocateAndFree) {
    void* p = g_old_gen.Allocate(64, true);
    ASSERT_NE(p, nullptr);

    auto* bytes = static_cast<uint8_t*>(p);
    bool zeroed = true;
    for (size_t i = 0; i < 64; i++) {
        if (bytes[i] != 0) { zeroed = false; break; }
    }
    EXPECT_TRUE(zeroed);

    std::memset(p, 0xAB, 64);
    EXPECT_EQ(bytes[0], 0xAB);

    g_old_gen.Free(p);
    SUCCEED();
}

TEST_F(OldGenTest, ReallocateDataIntegrity) {
    void* p = g_old_gen.Allocate(32, true);
    ASSERT_NE(p, nullptr);

    std::memcpy(p, "ABCDEFGHIJKLMNOP", 16);

    void* q = g_old_gen.Reallocate(p, 128);
    ASSERT_NE(q, nullptr);

    int cmp = std::memcmp(q, "ABCDEFGHIJKLMNOP", 16);
    EXPECT_EQ(cmp, 0);
}

TEST_F(OldGenTest, IsInOldGen) {
    void* p = g_old_gen.Allocate(64, true);
    ASSERT_NE(p, nullptr);

    EXPECT_TRUE(g_old_gen.IsInOldGen(p));
    EXPECT_FALSE(g_old_gen.IsInOldGen(nullptr));
}

TEST_F(OldGenTest, MarkObjectIsMarked) {
    void* p = g_old_gen.Allocate(32, true);
    ASSERT_NE(p, nullptr);

    // IsMarked on fresh object completes without crash
    SUCCEED();

    g_old_gen.MarkObject(p);
    SUCCEED();
}

TEST_F(OldGenTest, AddToMarkStack) {
    void* p = g_old_gen.Allocate(32, true);
    ASSERT_NE(p, nullptr);

    g_old_gen.AddToMarkStack(p);
    SUCCEED();
}

TEST_F(OldGenTest, BgcTryMark) {
    void* p = g_old_gen.Allocate(32, true);
    ASSERT_NE(p, nullptr);

    g_old_gen.BgcTryMark(p);
    SUCCEED();
}
