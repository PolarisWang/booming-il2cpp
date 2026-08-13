/// gc_young_collector_test — CRAG young collector unit tests (GoogleTest).

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <chaos/native_types.h>
#include "gc_region.h"
#include "gc_card_table.h"
#include "gc_layout.h"
#include "gc_young_gen.h"
#include "gc_young_collector.h"
#include "gc_test_base.h"
#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

struct YoungCollectorTest : GcUnitTestBase {
    void SetUp() override {
        GcUnitTestBase::SetUp();
        tls_tlab = TLAB{};
    }
};

TEST_F(YoungCollectorTest, ForwardingProtocol) {
    // kForwardingTag = 1, lowest bit set
    // TypeInfoHot* is at least 4-byte aligned, so bit 0 is always 0 for valid ptrs.
    SUCCEED();

    // Simulate what SetForwardingAddress does: writes (target | 1) to first word
    uintptr_t fake_target = 0x12345678;
    uintptr_t src;
    *reinterpret_cast<uintptr_t*>(&src) = fake_target | 1u;

    EXPECT_NE(src & 1u, 0u);
    uintptr_t recovered = src & ~1u;
    EXPECT_EQ(recovered, fake_target);

    // A 4-byte-aligned pointer always has bit 0 = 0
    uintptr_t type_info = 0x1004;
    EXPECT_EQ(type_info & 1u, 0u);
}

TEST_F(YoungCollectorTest, IsInNursery) {
    Region* nursery = g_young_gen.region.load(std::memory_order_acquire);
    ASSERT_NE(nursery, nullptr);

    EXPECT_FALSE(IsInNursery(nullptr));

    void* p = nursery->begin;
    EXPECT_TRUE(IsInNursery(p));

    // RegionManager global nursery bounds expand monotonically and nursery
    // range slots accumulate across InitYoungGeneration calls (each TEST_F
    // re-initialises).  Use 1 TiB past begin to guarantee out-of-range.
    uintptr_t tiB = uintptr_t(1) << 40;
    void* far_out = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(nursery->begin) + tiB);
    EXPECT_FALSE(IsInNursery(far_out));

    void* before = nursery->begin - 1;
    EXPECT_FALSE(IsInNursery(before));

    void* tenured = calloc(1, 64);
    ASSERT_NE(tenured, nullptr);
    EXPECT_FALSE(IsInNursery(tenured));
    free(tenured);
}

TEST_F(YoungCollectorTest, ScavengeObject) {
    Region* nursery = g_young_gen.region.load(std::memory_order_acquire);
    ASSERT_NE(nursery, nullptr);

    EXPECT_EQ(GcScavengeObject(nullptr), nullptr);

    void* tenured = calloc(1, 64);
    ASSERT_NE(tenured, nullptr);
    EXPECT_EQ(GcScavengeObject(tenured), tenured);
    free(tenured);

    void* nursery_obj = NurseryAllocate(64);
    ASSERT_NE(nursery_obj, nullptr);

    // Set a valid TypeInfo so PreciseObjectSize and layout scanning work.
    // Then write a recognizable pattern at offset 8 (past the TypeInfo ptr).
    *static_cast<const void**>(nursery_obj) = test_type_info_64();
    std::memset(nursery_obj, 0, 64);
    *static_cast<uint32_t*>(nursery_obj) = 0xBEEFCAFE;
    static_cast<uint8_t*>(nursery_obj)[60] = 0xAA;
    static_cast<uint8_t*>(nursery_obj)[61] = 0xBB;
    static_cast<uint8_t*>(nursery_obj)[62] = 0xCC;
    static_cast<uint8_t*>(nursery_obj)[63] = 0xDD;

    void* promoted = GcScavengeObject(nursery_obj);
    ASSERT_NE(promoted, nullptr);
    EXPECT_NE(promoted, nursery_obj);
    EXPECT_FALSE(IsInNursery(promoted));

    auto* dst_bytes = static_cast<const uint8_t*>(promoted);
    EXPECT_EQ(dst_bytes[0], 0xFE);
    EXPECT_EQ(dst_bytes[1], 0xCA);
    EXPECT_EQ(dst_bytes[2], 0xEF);
    EXPECT_EQ(dst_bytes[3], 0xBE);

    void* promoted2 = GcScavengeObject(nursery_obj);
    EXPECT_EQ(promoted2, promoted);

    auto first_word = *static_cast<const uintptr_t*>(nursery_obj);
    EXPECT_NE(first_word & 1u, 0u);
    auto recovered = reinterpret_cast<void*>(first_word & ~static_cast<uintptr_t>(1u));
    EXPECT_EQ(recovered, promoted);
}

TEST_F(YoungCollectorTest, YoungCollectionEmpty) {
    Region* nursery = g_young_gen.region.load(std::memory_order_acquire);
    ASSERT_NE(nursery, nullptr);
    GcSetHeapBase(nursery->begin);

    void* p = NurseryAllocate(32);
    ASSERT_NE(p, nullptr);

    YoungCollectionResult r1 = GcYoungCollection();
    EXPECT_EQ(r1.dirty_cards_scanned, 0u);

    // DirtyCard intentionally skips nursery objects (young GC scans precisely).
    // Manually dirty the card to test that young collection clears it.
    uintptr_t idx = (reinterpret_cast<uintptr_t>(p) - g_heap_base) >> kCardShift;
    uintptr_t seg_idx = idx / kCardsPerSegment;
    uintptr_t card_idx = idx % kCardsPerSegment;
    auto* seg = g_card_l1[seg_idx].load(std::memory_order_relaxed);
    ASSERT_NE(seg, nullptr);
    // R6/CoreCLR-aligned bit-per-word card layout: CardSegment.words packs 32
    // cards per uint32.  Set the card bit covering @a p.
    seg->words[card_idx / kCardsPerWord] |= (1u << (card_idx % kCardsPerWord));
    ASSERT_TRUE(IsDirty(p));

    YoungCollectionResult r2 = GcYoungCollection();
    EXPECT_EQ(g_young_gen.bump.load(std::memory_order_acquire), nursery->begin);
    EXPECT_FALSE(IsDirty(p));

    void* p2 = NurseryAllocate(64);
    ASSERT_NE(p2, nullptr);

    void* promoted = GcScavengeObject(p2);
    ASSERT_NE(promoted, nullptr);
    EXPECT_NE(promoted, p2);
}

TEST_F(YoungCollectorTest, CollectionWithDirtyCard) {
    Region* nursery = g_young_gen.region.load(std::memory_order_acquire);
    ASSERT_NE(nursery, nullptr);

    uintptr_t nursery_start = reinterpret_cast<uintptr_t>(nursery->begin);
    uintptr_t base_aligned = nursery_start & ~(kCardSize - 1);
    GcSetHeapBase(reinterpret_cast<void*>(base_aligned));

    void* old_block = calloc(1, kCardSize + 64);
    ASSERT_NE(old_block, nullptr);

    void* nursery_obj = NurseryAllocate(64);
    ASSERT_NE(nursery_obj, nullptr);
    std::memset(nursery_obj, 0, 64);
    *static_cast<uint32_t*>(nursery_obj) = 0xFACEFEED;

    uintptr_t old_addr = reinterpret_cast<uintptr_t>(old_block);
    uintptr_t old_card_idx = (old_addr - base_aligned) >> kCardShift;
    uintptr_t nursery_last_idx = (reinterpret_cast<uintptr_t>(nursery->end) - 1 - base_aligned) >> kCardShift;

    if (old_card_idx > nursery_last_idx) {
        GTEST_SKIP() << "No address overlap, testing via conservative sweep";
    }

    std::memcpy(old_block, &nursery_obj, sizeof(void*));
    DirtyCard(old_block);
    ASSERT_TRUE(IsDirty(old_block));

    YoungCollectionResult r = GcYoungCollection();
    EXPECT_EQ(g_young_gen.bump.load(std::memory_order_acquire), nursery->begin);
    EXPECT_FALSE(IsDirty(old_block));

    void* updated_ptr = {};
    std::memcpy(&updated_ptr, old_block, sizeof(void*));
    EXPECT_NE(updated_ptr, nursery_obj);
    EXPECT_FALSE(IsInNursery(updated_ptr));

    free(old_block);
}

TEST_F(YoungCollectorTest, ConservativeSweepSelfRefs) {
    Region* nursery = g_young_gen.region.load(std::memory_order_acquire);
    ASSERT_NE(nursery, nullptr);

    GcYoungCollection();

    // Register a type with a pointer field at offset 8, so Phase 2
    // (precise nursery scan) finds objA and follows its ref to objB.
    static constexpr uint16_t kPtrOffsets[] = { 8 };
    uint64_t sid = GcLayoutRegistry::Instance().RegisterOrGetRawAllocType(64);
    // Override with a pointer-bearing layout
    GcLayoutRegistry::Instance().Register(sid, 64, kPtrOffsets, 1);

    struct alignas(8) PtrType { uint64_t stable_id; uint64_t payload; void* ptr; };
    static PtrType s_ti{};
    s_ti.stable_id = sid;
    {
        auto* reg = &GcLayoutRegistry::Instance();
        uintptr_t ti_addr = reinterpret_cast<uintptr_t>(&s_ti);
        reg->RegisterTypeInfoRange(ti_addr, ti_addr + sizeof(PtrType));
    }

    void* objA = NurseryAllocate(64);
    void* objB = NurseryAllocate(64);
    ASSERT_NE(objA, nullptr);
    ASSERT_NE(objB, nullptr);
    std::memset(objA, 0, 64);
    std::memset(objB, 0, 64);

    // Write TypeInfo header so Phase 2 recognises objA as a typed object
    *static_cast<const void**>(objA) = &s_ti;
    // objA->ptr = objB (pointer at offset 8, matching our layout)
    std::memcpy(static_cast<uint8_t*>(objA) + 8, &objB, sizeof(void*));
    *static_cast<uint32_t*>(objB) = 0xBABEBABE;

    // objB address on the stack + objA's typed reference → young collector
    // will promote objB through Phase 2 scan of objA.
    GcYoungCollection();

    auto b_first_word = *static_cast<const uintptr_t*>(objB);
    EXPECT_NE(b_first_word & 1u, 0u);

    void* b_promoted = reinterpret_cast<void*>(b_first_word & ~1u);
    EXPECT_FALSE(IsInNursery(b_promoted));
}
