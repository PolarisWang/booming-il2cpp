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

TEST_F(YoungCollectorTest, OldGenToNurseryRefFixedUpViaDirtyCard) {
    // Regression: an old-gen object holding a reference to a nursery object
    // must have that slot rewritten to the promoted address during young GC.
    // The old->young edge is only discoverable through the dirty-card scan
    // (Phase 1); if the card-table base is corrupted by a late g_heap_base
    // override (historic GcSetHeapBase(page) in old-gen page allocation),
    // Phase 1 scans the wrong address range and the slot keeps a stale
    // pointer to collected nursery memory (cross-gen use-after-free).
    //
    // Use a REAL managed old-gen object (g_old_gen.Allocate) so registration
    // with the card table exercises the same path as production mutation.
    Region* nursery = g_young_gen.region.load(std::memory_order_acquire);
    ASSERT_NE(nursery, nullptr);
    Region* gen1 = g_young_gen.gen1_region.load(std::memory_order_acquire);

    // Anchor the card-table base to the nursery through GcRegisterHeapRange —
    // the production base-maintenance path (NOT the raw GcSetHeapBase override,
    // which is precisely the corruption this test guards against).  When the
    // nursery sits below any previously-registered base, GcRegisterHeapRange
    // lowers g_heap_base AND re-keys every tracked segment (gc_card_table.cpp
    // below-base path), so DirtyCard / IsDirty / ScanDirtyCardsInRegistered
    // Segments all stay consistent with the old-gen pages this test mutates,
    // independent of prior tests' global-base pollution.
    GcRegisterHeapRange(reinterpret_cast<uintptr_t>(nursery->begin),
                        reinterpret_cast<uintptr_t>(nursery->end));
    if (gen1 != nullptr) {
        GcRegisterHeapRange(reinterpret_cast<uintptr_t>(gen1->begin),
                            reinterpret_cast<uintptr_t>(gen1->end));
    }
    GcSetCardTableNurseryRange(reinterpret_cast<uintptr_t>(nursery->begin),
                               reinterpret_cast<uintptr_t>(nursery->end));

    void* old_block = g_old_gen.Allocate(64, true);
    ASSERT_NE(old_block, nullptr);
    std::memset(old_block, 0, 64);

    void* nursery_obj = NurseryAllocate(64);
    ASSERT_NE(nursery_obj, nullptr);
    std::memset(nursery_obj, 0, 64);
    *static_cast<const void**>(nursery_obj) = test_type_info_64();
    ASSERT_TRUE(IsInNursery(nursery_obj));

    // Old-gen object field at offset 8 references the nursery object.
    std::memcpy(static_cast<uint8_t*>(old_block) + 8, &nursery_obj, sizeof(void*));
    DirtyCard(static_cast<uint8_t*>(old_block) + 8);

    YoungCollectionResult r = GcYoungCollection();

    // The old-gen slot must no longer point into the (now-collected) nursery.
    void* slot_after = nullptr;
    std::memcpy(&slot_after, static_cast<uint8_t*>(old_block) + 8, sizeof(void*));
    EXPECT_FALSE(IsInNursery(slot_after));

    // The rewritten value must be a live tenured object, not a forwarding-
    // stamped address left pointing at collected nursery memory (the UAF).
    ASSERT_NE(slot_after, nullptr);
    auto first_word = *static_cast<const uintptr_t*>(slot_after);
    EXPECT_EQ(first_word & 1u, 0u) << "slot points at a forwarding-stamped (stale) address";

    // The old-gen field should now equal the tenured copy of the object that
    // Phase 0 promoted from the stack root (both must agree).
    void* promoted = nursery_obj;   // stack root was fixed up to the tenured addr
    EXPECT_EQ(slot_after, promoted) << "old-gen slot not fixed to the promoted object";
}

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
    // Young GC over a barely-populated nursery: the collection must run without
    // over-writing g_heap_base, promote the one stack-rooted nursery object, and
    // reset the shared bump pointer so a fresh allocation re-starts at begin.
    //
    // Historically this test called GcSetHeapBase(nursery->begin) (a raw base
    // override), which violated the documented invariant that GcRegisterHeapRange
    // is the sole owner of g_heap_base (gc_old_gen.cpp:384-394).  On the first
    // GcYoungCollection() the card scan recomputed OLD-gen segment addresses from
    // the non-lowest base, so the L1 index exploded (~2.3 GB offset) and
    // phase1_scan_cb read unmapped memory → SEH 0xC0000005.  It also manually
    // dirtied the card covering a NURSERY object and asserted young collection
    // cleared it — but that is off-model: nursery writes never go through
    // DirtyCard (nursery keeps cards clean; young GC Phase 2 scans precisely,
    // gc_card_table.h:160-163), so no production path ever clears a nursery card.
    // Old→young dirty-card clearing is covered by OldGenToNurseryRefFixedUpVia
    // DirtyCard and CollectionWithDirtyCard (real old-gen / raw-block slots).
    //
    // Re-read g_young_gen.region AFTER the first NurseryAllocate: TlabClaimFromYoungGen
    // lazily re-runs InitYoungGeneration on the first allocation, which picks a fresh
    // REGION_NURSERY and stores it to g_young_gen.region — so the region captured
    // before the allocation may be stale in an isolated (single-test) run.  The bump
    // reset assert must compare against the region the collection actually operates on.
    void* p = NurseryAllocate(32);
    ASSERT_NE(p, nullptr);
    std::memset(p, 0, 32);
    Region* nursery = g_young_gen.region.load(std::memory_order_acquire);
    ASSERT_NE(nursery, nullptr);

    // p is a stack root on this frame — the all-thread conservative stack scan
    // (Phase 0) promotes it and rewrites the local to the tenured address.
    YoungCollectionResult r1 = GcYoungCollection();
    EXPECT_FALSE(IsInNursery(p));
    EXPECT_GE(r1.objects_promoted, 1u);

    // Empty nursery · bump reset to the start of the young region.
    EXPECT_EQ(g_young_gen.bump.load(std::memory_order_acquire), nursery->begin);

    // A fresh allocation on the reset nursery lands back in the nursery.
    void* p2 = NurseryAllocate(64);
    ASSERT_NE(p2, nullptr);
    EXPECT_TRUE(IsInNursery(p2));
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
