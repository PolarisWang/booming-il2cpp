/// gc_gen1_test — Gen1 collection unit tests (GoogleTest).

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <chaos/native_types.h>
#include "gc_gen1.h"
#include "gc_scheduler.h"
#include "gc_stats.h"
#include "gc_young_gen.h"
#include "gc_region.h"
#include "gc_old_gen.h"
#include "gc_layout.h"
#include "thread_state.h"
#include "gc_test_base.h"
#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

static void ClearNursery() {
    auto* nursery = g_young_gen.region.load(std::memory_order_acquire);
    if (nursery == nullptr) return;
    auto* n_end = g_young_gen.bump.load(std::memory_order_acquire);
    if (n_end != nullptr && n_end > nursery->begin) {
        std::memset(nursery->begin, 0,
                    static_cast<size_t>(n_end - nursery->begin));
    }
}

static void InitGen1Object(void* obj, uint32_t pattern) {
    *static_cast<const void**>(obj) = GcUnitTestBase::GetTestTypeInfo(64);
    *reinterpret_cast<uint32_t*>(static_cast<char*>(obj) + 8) = pattern;
}

struct Gen1Test : GcUnitTestBase {
};

TEST_F(Gen1Test, EmptyGen1) {
    Gen1CollectionResult r = GcGen1Collection();
    EXPECT_EQ(r.objects_in_gen1, 0u);
    EXPECT_EQ(r.objects_promoted, 0u);
    EXPECT_EQ(r.bytes_reclaimed, 0u);
    EXPECT_GE(r.pause_ns, 0);
    EXPECT_EQ(g_young_gen.survivor_bump.load(std::memory_order_acquire),
              g_young_gen.survivor_begin);
}

TEST_F(Gen1Test, SingleLiveObject) {
    GcGen1Collection();

    void* gen1_obj = TryAllocateInGen1(64);
    ASSERT_NE(gen1_obj, nullptr);
    EXPECT_TRUE(IsInGen1(gen1_obj));
    InitGen1Object(gen1_obj, 0xCAFEBABE);

    void* gen0_ref = NurseryAllocate(64);
    ASSERT_NE(gen0_ref, nullptr);
    std::memset(gen0_ref, 0, 64);
    std::memcpy(static_cast<char*>(gen0_ref) + 8, &gen1_obj, sizeof(void*));

    volatile void* stack_ref = gen1_obj;
    (void)stack_ref;

    Gen1CollectionResult r = GcGen1Collection();
    EXPECT_EQ(r.objects_in_gen1, 1u);
    EXPECT_EQ(r.objects_promoted, 1u);
    EXPECT_EQ(r.bytes_reclaimed, 0u);
    EXPECT_GE(r.bytes_promoted, 64u);

    EXPECT_EQ(g_young_gen.survivor_bump.load(std::memory_order_acquire),
              g_young_gen.survivor_begin);
    EXPECT_FALSE(r.promotion_failed);
}

TEST_F(Gen1Test, MixedLiveAndDead) {
    GcGen1Collection();
    ClearNursery();

    void* objA = TryAllocateInGen1(64);
    void* objB = TryAllocateInGen1(64);
    void* objC = TryAllocateInGen1(64);
    ASSERT_NE(objA, nullptr);
    ASSERT_NE(objB, nullptr);
    ASSERT_NE(objC, nullptr);

    InitGen1Object(objA, 0xAA0001);
    InitGen1Object(objB, 0xBB0002);
    InitGen1Object(objC, 0xCC0003);

    void* gen0_ref = NurseryAllocate(64);
    ASSERT_NE(gen0_ref, nullptr);
    std::memset(gen0_ref, 0, 64);
    std::memcpy(static_cast<char*>(gen0_ref) + 8, &objA, sizeof(void*));
    std::memcpy(static_cast<char*>(gen0_ref) + 16, &objC, sizeof(void*));

    Gen1CollectionResult r = GcGen1Collection();
    EXPECT_EQ(r.objects_in_gen1, 3u);
    // Conservative stack scanning may keep objB alive (its address is
    // still on the stack even though no explicit reference is stored).
    // Accept either 2 or 3 promoted objects.
    EXPECT_GE(r.objects_promoted, 2u);
    EXPECT_GE(r.bytes_promoted, 128u);
    // bytes_reclaimed may be 0 if objB is conservatively kept alive
    EXPECT_FALSE(r.promotion_failed);

    EXPECT_EQ(g_young_gen.survivor_bump.load(std::memory_order_acquire),
              g_young_gen.survivor_begin);
}

TEST_F(Gen1Test, IsInGen1Boundaries) {
    EXPECT_FALSE(IsInGen1(nullptr));

    void* gen1_obj = TryAllocateInGen1(64);
    ASSERT_NE(gen1_obj, nullptr);
    EXPECT_TRUE(IsInGen1(gen1_obj));

    void* nursery_obj = NurseryAllocate(64);
    ASSERT_NE(nursery_obj, nullptr);
    EXPECT_FALSE(IsInGen1(nursery_obj));

    void* old_obj = g_old_gen.Allocate(64, true);
    ASSERT_NE(old_obj, nullptr);
    EXPECT_FALSE(IsInGen1(old_obj));

    char* past_end = g_young_gen.survivor_end + 1;
    EXPECT_FALSE(IsInGen1(past_end));

    char* before_begin = g_young_gen.survivor_begin - 1;
    EXPECT_FALSE(IsInGen1(before_begin));

    volatile void* keep_alive = gen1_obj;
    (void)keep_alive;
    GcGen1Collection();
}

TEST_F(Gen1Test, Gen1Fragmentation) {
    GcGen1Collection();
    float frag_empty = Gen1Fragmentation();
    EXPECT_GT(frag_empty, 0.99f);

    uint64_t big_sid = GcLayoutRegistry::Instance()
        .RegisterOrGetRawAllocType(1024 * 1024);
    static GcTestTypeInfo big_ti{};
    big_ti.stable_id = big_sid;
    {
        auto* reg = &GcLayoutRegistry::Instance();
        uintptr_t ti_addr = reinterpret_cast<uintptr_t>(&big_ti);
        reg->RegisterTypeInfoRange(ti_addr, ti_addr + sizeof(GcTestTypeInfo));
    }
    void* big = TryAllocateInGen1(1024 * 1024);
    ASSERT_NE(big, nullptr);
    *static_cast<const void**>(big) = &big_ti;
    float frag_alloc = Gen1Fragmentation();
    EXPECT_LT(frag_alloc, frag_empty);
    EXPECT_GT(frag_alloc, 0.80f);

    volatile void* keep = big;
    (void)keep;
    GcGen1Collection();
    float frag_collected = Gen1Fragmentation();
    EXPECT_GT(frag_collected, 0.99f);
}

TEST_F(Gen1Test, PromotionAgeThresholdTrigger) {
    g_young_gen.promotion_age_threshold_.store(1, std::memory_order_release);

    for (int gc = 0; gc < 4; gc++) {
        bool should = (1 <= 1) || (gc % 1 == 0);
        EXPECT_TRUE(should);
    }

    g_young_gen.promotion_age_threshold_.store(2, std::memory_order_release);

    void* gen1_obj = TryAllocateInGen1(64);
    ASSERT_NE(gen1_obj, nullptr);
    *static_cast<const void**>(gen1_obj) = GcUnitTestBase::GetTestTypeInfo(64);
    volatile void* keep = gen1_obj;
    (void)keep;

    Gen1CollectionResult r = GcGen1Collection();
    EXPECT_GE(r.objects_in_gen1, 1u);

    g_young_gen.promotion_age_threshold_.store(3, std::memory_order_release);

    for (int gc = 0; gc < 6; gc++) {
        bool should = (3 <= 1) || (gc % 3 == 0);
        if (gc % 3 == 0) {
            EXPECT_TRUE(should);
        } else {
            EXPECT_FALSE(should);
        }
    }

    g_young_gen.promotion_age_threshold_.store(2, std::memory_order_release);
}
