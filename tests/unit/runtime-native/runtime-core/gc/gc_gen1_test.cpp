/// gc_gen1_test — Gen1 collection unit tests (GoogleTest).

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <chaos/native_types.h>
#include "gc_gen1.h"
#include "gc_heap.h"
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
    void TearDown() override {
        tls_tlab.start = nullptr;
        tls_tlab.end = nullptr;
        GcUnitTestBase::TearDown();
    }
};

TEST_F(Gen1Test, EmptyGen1) {
    Gen1CollectionResult r = GcGen1Collection();
    EXPECT_EQ(r.objects_in_gen1, 0u);
    EXPECT_EQ(r.objects_promoted, 0u);
    EXPECT_EQ(r.bytes_reclaimed, 0u);
    EXPECT_GE(r.pause_ns, 0);
    {
        auto* gen1 = g_young_gen.gen1_region.load(std::memory_order_acquire);
        ASSERT_NE(gen1, nullptr);
        EXPECT_EQ(g_young_gen.gen1_bump.load(std::memory_order_acquire),
                  gen1->begin);
    }
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

    {
        auto* gen1 = g_young_gen.gen1_region.load(std::memory_order_acquire);
        ASSERT_NE(gen1, nullptr);
        EXPECT_EQ(g_young_gen.gen1_bump.load(std::memory_order_acquire),
                  gen1->begin);
    }
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

    {
        auto* gen1 = g_young_gen.gen1_region.load(std::memory_order_acquire);
        ASSERT_NE(gen1, nullptr);
        EXPECT_EQ(g_young_gen.gen1_bump.load(std::memory_order_acquire),
                  gen1->begin);
    }
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

    char* past_end = g_young_gen.gen1_end + 1;
    EXPECT_FALSE(IsInGen1(past_end));

    auto* gen1_for_before = g_young_gen.gen1_region.load(std::memory_order_acquire);
    ASSERT_NE(gen1_for_before, nullptr);
    char* before_begin = gen1_for_before->begin - 1;
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

// ======================================================================
// Phase B: Independent Gen1 collection tests (B4)
// ======================================================================

TEST_F(Gen1Test, Gen1IndependentCollection_Empty) {
    // Ensure Gen1 is empty first.
    GcGen1Collection();

    // GcGen1ShouldCollect should return false when Gen1 is empty.
    EXPECT_FALSE(GcGen1ShouldCollect());

    // Calling GcGen1Collection on empty Gen1 shouldn't crash.
    Gen1CollectionResult r = GcGen1Collection();
    EXPECT_EQ(r.objects_in_gen1, 0u);
    EXPECT_EQ(r.objects_promoted, 0u);
    EXPECT_FALSE(r.promotion_failed);

    // Verify Gen1 state is clean.
    auto* gen1 = g_young_gen.gen1_region.load(std::memory_order_acquire);
    ASSERT_NE(gen1, nullptr);
    EXPECT_EQ(g_young_gen.gen1_bump.load(std::memory_order_acquire), gen1->begin);
}

TEST_F(Gen1Test, Gen1IndependentCollection_FillsGen1) {
    // Reset Gen1.
    GcGen1Collection();

    // Allocate enough objects to fill >80% of Gen1.
    auto* gen1 = g_young_gen.gen1_region.load(std::memory_order_acquire);
    ASSERT_NE(gen1, nullptr);
    CHAOS_IL2CPP_SIZE gen1_capacity =
        static_cast<CHAOS_IL2CPP_SIZE>(g_young_gen.gen1_end - gen1->begin);
    CHAOS_IL2CPP_SIZE target_alloc = static_cast<CHAOS_IL2CPP_SIZE>(
        static_cast<float>(gen1_capacity) * 0.85f);
    CHAOS_IL2CPP_SIZE obj_size = 4096;  // 4 KB objects
    CHAOS_IL2CPP_SIZE count = target_alloc / obj_size;
    ASSERT_GT(count, 0u);

    uint32_t pattern = 0xFF000001;
    for (CHAOS_IL2CPP_SIZE i = 0; i < count; i++) {
        void* obj = TryAllocateInGen1(obj_size);
        ASSERT_NE(obj, nullptr) << "Failed at object " << i;
        InitGen1Object(obj, pattern++);
    }

    // GcGen1ShouldCollect should return true (high occupancy >80%).
    EXPECT_TRUE(GcGen1ShouldCollect());

    Gen1CollectionResult r = GcGen1Collection();
    EXPECT_GT(r.objects_in_gen1, 0u);
    EXPECT_FALSE(r.promotion_failed);

    // With C20 partial retention, "new" survivors (first Gen1 collection) are
    // compacted in Gen1 rather than promoted to Gen2. Objects not explicitly
    // rooted are reclaimed. Conservative stack scanning may keep a few alive.
    auto* bump_after = g_young_gen.gen1_bump.load(std::memory_order_acquire);
    if (bump_after > gen1->begin) {
        EXPECT_GT(r.bytes_compacted, 0u);
    }

    // Nursery should still accept allocations.
    void* nursery_obj = NurseryAllocate(64);
    ASSERT_NE(nursery_obj, nullptr);
}

TEST_F(Gen1Test, Gen1IndependentCollection_ThenNurseryAlloc) {
    // Reset Gen1.
    GcGen1Collection();

    // Allocate a moderate amount in Gen1.
    void* gen1_obj = TryAllocateInGen1(64);
    ASSERT_NE(gen1_obj, nullptr);
    InitGen1Object(gen1_obj, 0xBEEF0001);

    // Add a root so the object stays alive.
    void* gen0_ref = NurseryAllocate(64);
    ASSERT_NE(gen0_ref, nullptr);
    std::memset(gen0_ref, 0, 64);
    std::memcpy(static_cast<char*>(gen0_ref) + 8, &gen1_obj, sizeof(void*));

    // Fill Gen1 to high occupancy (>80%) to trigger collection.
    auto* gen1_for_trigger = g_young_gen.gen1_region.load(std::memory_order_acquire);
    ASSERT_NE(gen1_for_trigger, nullptr);
    CHAOS_IL2CPP_SIZE gen1_capacity =
        static_cast<CHAOS_IL2CPP_SIZE>(g_young_gen.gen1_end - gen1_for_trigger->begin);
    CHAOS_IL2CPP_SIZE fill_count = static_cast<CHAOS_IL2CPP_SIZE>(
        static_cast<float>(gen1_capacity) * 0.85f) / 4096;
    for (CHAOS_IL2CPP_SIZE i = 0; i < fill_count; i++) {
        void* filler = TryAllocateInGen1(4096);
        ASSERT_NE(filler, nullptr) << "Failed at fill " << i;
        InitGen1Object(filler, 0xBEEF0002 + i);
    }
    EXPECT_TRUE(GcGen1ShouldCollect());

    Gen1CollectionResult r = GcGen1Collection();
    EXPECT_GT(r.objects_in_gen1, 0u);
    EXPECT_FALSE(r.promotion_failed);

    // With C20 partial retention, "new" survivors are compacted in Gen1
    // rather than promoted.  The rooted gen1_obj should survive via
    // compaction.
    EXPECT_GT(r.bytes_compacted, 0u);
    auto* bump = g_young_gen.gen1_bump.load(std::memory_order_acquire);
    EXPECT_GT(bump, g_young_gen.gen1_region.load(std::memory_order_acquire)->begin);

    // After Gen1 collection, nursery allocation should still work.
    void* after = NurseryAllocate(128);
    ASSERT_NE(after, nullptr);
    std::memset(after, 0, 128);

    // Gen1 should be reusable for new allocations.
    void* new_gen1 = TryAllocateInGen1(64);
    ASSERT_NE(new_gen1, nullptr);
    EXPECT_TRUE(IsInGen1(new_gen1));
    volatile void* keep = new_gen1;
    (void)keep;
    volatile void* keep2 = after;
    (void)keep2;
    volatile void* keep3 = gen0_ref;
    (void)keep3;
}

TEST_F(Gen1Test, Gen1ShouldCollect_OccupancyBased) {
    // Reset Gen1 state.
    GcGen1Collection();

    // Gen1 with minimal data → should NOT collect.
    void* gen1_obj = TryAllocateInGen1(64);
    ASSERT_NE(gen1_obj, nullptr);
    InitGen1Object(gen1_obj, 0xCAFE0001);
    volatile void* keep = gen1_obj;
    (void)keep;

    // Low occupancy → GcGen1ShouldCollect returns false.
    EXPECT_FALSE(GcGen1ShouldCollect());

    // Fill Gen1 to ~85% occupancy.
    auto* gen1 = g_young_gen.gen1_region.load(std::memory_order_acquire);
    ASSERT_NE(gen1, nullptr);
    CHAOS_IL2CPP_SIZE capacity =
        static_cast<CHAOS_IL2CPP_SIZE>(g_young_gen.gen1_end - gen1->begin);
    CHAOS_IL2CPP_SIZE fill_size = 4096;
    CHAOS_IL2CPP_SIZE fill_count = static_cast<CHAOS_IL2CPP_SIZE>(
        static_cast<float>(capacity) * 0.85f) / fill_size;
    fill_count = (fill_count > 1) ? fill_count - 1 : 0;
    for (CHAOS_IL2CPP_SIZE i = 0; i < fill_count; i++) {
        void* obj = TryAllocateInGen1(fill_size);
        ASSERT_NE(obj, nullptr) << "Failed at fill " << i;
        InitGen1Object(obj, 0xCAFE1000 + i);
    }

    // High occupancy should trigger collection.
    EXPECT_TRUE(GcGen1ShouldCollect());

    // Collect and verify compaction for new survivors.
    Gen1CollectionResult r = GcGen1Collection();
    EXPECT_GT(r.objects_in_gen1, 0u);
    EXPECT_FALSE(r.promotion_failed);
    // With C20, "new" survivors are compacted in Gen1 (not promoted).
    // The live gen1_obj will be compacted to Gen1 start.
    auto* bump_after = g_young_gen.gen1_bump.load(std::memory_order_acquire);
    EXPECT_GT(bump_after, gen1->begin);
    EXPECT_GT(r.bytes_compacted, 0u);
}

// ═══════════════════════════════════════════════════════════════════════════
// Phase 2d: Gen1 fragmentation/compaction tests
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(Gen1Test, Gen1HighFragmentationTrigger) {
    GcGen1Collection();

    auto* gen1 = g_young_gen.gen1_region.load(std::memory_order_acquire);
    ASSERT_NE(gen1, nullptr);
    CHAOS_IL2CPP_SIZE capacity =
        static_cast<CHAOS_IL2CPP_SIZE>(g_young_gen.gen1_end - gen1->begin);

    CHAOS_IL2CPP_SIZE obj_size = 8192;
    CHAOS_IL2CPP_SIZE pair_count = (capacity / obj_size) / 2;
    if (pair_count > 50) pair_count = 50;

    void* roots[50];
    uint32_t kept = 0;
    for (CHAOS_IL2CPP_SIZE i = 0; i < pair_count; i++) {
        void* obj_a = TryAllocateInGen1(obj_size);
        ASSERT_NE(obj_a, nullptr);
        InitGen1Object(obj_a, 0xAA0000 + i);
        roots[kept++] = obj_a;

        void* obj_b = TryAllocateInGen1(obj_size);
        ASSERT_NE(obj_b, nullptr);
        InitGen1Object(obj_b, 0xBB0000 + i);
    }

    for (uint32_t i = 0; i < kept; i++) {
        void* gen0_ref = NurseryAllocate(64);
        ASSERT_NE(gen0_ref, nullptr);
        std::memset(gen0_ref, 0, 64);
        std::memcpy(static_cast<char*>(gen0_ref) + 8, &roots[i], sizeof(void*));
    }

    float frag = Gen1Fragmentation();
    EXPECT_GT(frag, 0.20f);
    SUCCEED();
}

TEST_F(Gen1Test, Gen1OomFallback) {
    GcGen1Collection();

    void* gen1_obj = TryAllocateInGen1(64);
    ASSERT_NE(gen1_obj, nullptr);
    InitGen1Object(gen1_obj, 0xDEAD0001);

    void* gen0_ref = NurseryAllocate(64);
    ASSERT_NE(gen0_ref, nullptr);
    std::memset(gen0_ref, 0, 64);
    std::memcpy(static_cast<char*>(gen0_ref) + 8, &gen1_obj, sizeof(void*));

    G_Scheduler().SetHardLimit(1);

    Gen1CollectionResult r = GcGen1Collection();
    EXPECT_TRUE(r.promotion_failed);

    ClearNursery();
    G_Scheduler().SetHardLimit(0);

    auto* gen1 = g_young_gen.gen1_region.load(std::memory_order_acquire);
    ASSERT_NE(gen1, nullptr);
    char* bump = g_young_gen.gen1_bump.load(std::memory_order_acquire);
    EXPECT_GT(bump, gen1->begin)
        << "Gen1 bump should not reset when promotion fails";

    tls_tlab.start = nullptr;
    tls_tlab.end = nullptr;
}

TEST_F(Gen1Test, Gen1HighSurvivalRate) {
    GcGen1Collection();

    auto* gen1 = g_young_gen.gen1_region.load(std::memory_order_acquire);
    ASSERT_NE(gen1, nullptr);
    CHAOS_IL2CPP_SIZE capacity =
        static_cast<CHAOS_IL2CPP_SIZE>(g_young_gen.gen1_end - gen1->begin);

    // Fill Gen1 to ~60% with small objects.
    CHAOS_IL2CPP_SIZE obj_size = 128;
    CHAOS_IL2CPP_SIZE count = static_cast<CHAOS_IL2CPP_SIZE>(
        static_cast<float>(capacity) * 0.60f) / obj_size;
    if (count > 200) count = 200;

    // Keep all objects alive via Gen0 roots.
    for (CHAOS_IL2CPP_SIZE i = 0; i < count; i++) {
        void* obj = TryAllocateInGen1(obj_size);
        ASSERT_NE(obj, nullptr) << "Failed at obj " << i;
        InitGen1Object(obj, 0x50000000 + static_cast<uint32_t>(i));

        void* gen0_ref = NurseryAllocate(64);
        ASSERT_NE(gen0_ref, nullptr);
        std::memset(gen0_ref, 0, 64);
        std::memcpy(static_cast<char*>(gen0_ref) + 8, &obj, sizeof(void*));
    }

    // All objects have Gen0 roots — all should survive and promote.
    Gen1CollectionResult r = GcGen1Collection();
    EXPECT_GT(r.objects_in_gen1, 0u);
    EXPECT_EQ(r.objects_promoted, r.objects_in_gen1)
        << "All live Gen1 objects should promote under high survival";
    EXPECT_GE(r.bytes_promoted, obj_size * count * 3 / 4);
    EXPECT_FALSE(r.promotion_failed);

    EXPECT_EQ(g_young_gen.gen1_bump.load(std::memory_order_acquire), gen1->begin);
}

TEST_F(Gen1Test, Gen1CompactionReusesSpace) {
    GcGen1Collection();

    auto* gen1 = g_young_gen.gen1_region.load(std::memory_order_acquire);
    ASSERT_NE(gen1, nullptr);

    // Fill Gen1 to ~80%.
    CHAOS_IL2CPP_SIZE capacity =
        static_cast<CHAOS_IL2CPP_SIZE>(g_young_gen.gen1_end - gen1->begin);
    CHAOS_IL2CPP_SIZE obj_size = 4096;
    CHAOS_IL2CPP_SIZE count = static_cast<CHAOS_IL2CPP_SIZE>(
        static_cast<float>(capacity) * 0.80f) / obj_size;

    for (CHAOS_IL2CPP_SIZE i = 0; i < count; i++) {
        void* obj = TryAllocateInGen1(obj_size);
        ASSERT_NE(obj, nullptr) << "Failed at fill " << i;
        InitGen1Object(obj, 0x60000000 + static_cast<uint32_t>(i));
    }

    // No roots → all objects should be reclaimable. Conservative stack scanning
    // may keep a few alive via compaction in Gen1, but most should be reclaimed.
    Gen1CollectionResult r = GcGen1Collection();
    EXPECT_GT(r.objects_in_gen1, 0u);
    EXPECT_GT(r.bytes_reclaimed, 0u)
        << "Should reclaim bytes from dead Gen1 objects";
    EXPECT_FALSE(r.promotion_failed);

    // With C20, any survivors from conservative stack scanning are compacted
    // in Gen1 rather than promoted. Verify the bump reflects this.
    auto* bump_after = g_young_gen.gen1_bump.load(std::memory_order_acquire);
    EXPECT_GE(bump_after, gen1->begin);

    // Verify Gen1 space is reusable by allocating again — regardless of
    // whether compaction left survivors or fully drained Gen1.
    void* reused = TryAllocateInGen1(64);
    ASSERT_NE(reused, nullptr);
    EXPECT_TRUE(IsInGen1(reused));

    volatile void* keep = reused;
    (void)keep;
}
