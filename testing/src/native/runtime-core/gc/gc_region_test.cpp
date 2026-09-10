/// gc_region_test — CRAG region framework + card table unit tests (GoogleTest).
///
/// Exercises:
///   - Region struct layout and kind semantics
///   - RegionManager: AllocateRegion / FreeRegion / ReleaseDomainRegions
///   - NurseryAllocate inline fast path + slow path fallback
///   - Card table: DirtyCard / IsDirty / ClearCard / ScanDirtyCards / GcSetHeapBase

#include <chaos/native_types.h>

#include "gc_region.h"
#include "gc_card_table.h"
#include "gc_young_gen.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>

using namespace chaos::il2cpp::runtime_core;

// ════════════════════════════════════════════════════════════════════════════
// Region struct layout and kind semantics
// ════════════════════════════════════════════════════════════════════════════

TEST(GcRegion, KindEnumValues) {
    EXPECT_EQ(static_cast<int>(RegionKind::REGION_NURSERY), 0);
    EXPECT_EQ(static_cast<int>(RegionKind::REGION_TENURED), 1);
    EXPECT_EQ(static_cast<int>(RegionKind::REGION_DOMAIN),  2);
    EXPECT_EQ(static_cast<int>(RegionKind::REGION_RAW),     3);
    EXPECT_EQ(static_cast<int>(RegionKind::REGION_FOH),     4);
}

TEST(GcRegion, RegionIdSentinel) {
    EXPECT_EQ(kRegionIdInvalid, 0);
}

TEST(GcRegion, FieldAssignment) {
    Region r{};
    r.id = 42;
    r.kind = RegionKind::REGION_TENURED;
    r.domain_id = 7;
    r.begin = reinterpret_cast<char*>(0x1000);
    r.end = reinterpret_cast<char*>(0x2000);
    r.current = r.begin;
    r.gc_state = {};
    r.next = nullptr;

    EXPECT_EQ(r.id, 42u);
    EXPECT_EQ(r.kind, RegionKind::REGION_TENURED);
    EXPECT_EQ(r.domain_id, 7u);
    EXPECT_EQ(r.begin, r.current);
}

// ════════════════════════════════════════════════════════════════════════════
// RegionManager: AllocateRegion / FreeRegion / ReleaseDomainRegions
// ════════════════════════════════════════════════════════════════════════════

TEST(GcRegion, AllocateNurseryRegion) {
    auto& mgr = RegionManager::Instance();
    Region* nursery = mgr.AllocateRegion(RegionKind::REGION_NURSERY, 0);
    ASSERT_NE(nursery, nullptr);
    EXPECT_NE(nursery->id, kRegionIdInvalid);
    EXPECT_EQ(nursery->kind, RegionKind::REGION_NURSERY);
    EXPECT_NE(nursery->begin, nullptr);
    EXPECT_GT(nursery->end, nursery->begin);
    EXPECT_EQ(nursery->current, nursery->begin);
    EXPECT_EQ(nursery->domain_id, 0u);
}

TEST(GcRegion, AllocateTenuredRegion) {
    auto& mgr = RegionManager::Instance();
    Region* tenured = mgr.AllocateRegion(RegionKind::REGION_TENURED, 0, 5);
    ASSERT_NE(tenured, nullptr);
    EXPECT_EQ(tenured->kind, RegionKind::REGION_TENURED);
    EXPECT_EQ(tenured->domain_id, 5u);
}

TEST(GcRegion, AllocateDomainRegion) {
    auto& mgr = RegionManager::Instance();
    Region* domain = mgr.AllocateRegion(RegionKind::REGION_DOMAIN, 0, 3);
    ASSERT_NE(domain, nullptr);
    EXPECT_EQ(domain->kind, RegionKind::REGION_DOMAIN);
    EXPECT_EQ(domain->domain_id, 3u);
}

TEST(GcRegion, FreeAndReallocate) {
    auto& mgr = RegionManager::Instance();
    Region* nursery = mgr.AllocateRegion(RegionKind::REGION_NURSERY, 0);
    ASSERT_NE(nursery, nullptr);
    RegionId id = nursery->id;

    mgr.FreeRegion(id);
    Region* realloc = mgr.AllocateRegion(RegionKind::REGION_NURSERY, 0);
    ASSERT_NE(realloc, nullptr);
    EXPECT_NE(realloc->id, kRegionIdInvalid);
}

TEST(GcRegion, ReleaseDomainRegions) {
    auto& mgr = RegionManager::Instance();
    Region* d2 = mgr.AllocateRegion(RegionKind::REGION_DOMAIN, 0, 3);
    ASSERT_NE(d2, nullptr);

    mgr.ReleaseDomainRegions(3);

    Region* after = mgr.AllocateRegion(RegionKind::REGION_DOMAIN, 0, 3);
    ASSERT_NE(after, nullptr);
    EXPECT_NE(after->id, kRegionIdInvalid);
    EXPECT_GE(mgr.ActiveRegionCount(), 3u);
}

// ════════════════════════════════════════════════════════════════════════════
// NurseryAllocate inline fast path
// ════════════════════════════════════════════════════════════════════════════

TEST(GcRegion, NurseryAllocateSmallReturnsZeroedMemory) {
    InitYoungGeneration();
    void* p1 = NurseryAllocate(16);
    ASSERT_NE(p1, nullptr);
    auto* bytes = static_cast<uint8_t*>(p1);
    for (size_t i = 0; i < 16; i++) {
        EXPECT_EQ(bytes[i], 0) << "byte " << i << " not zeroed";
    }
}

TEST(GcRegion, NurseryAllocateConsecutiveNoOverlap) {
    void* p2 = NurseryAllocate(32);
    void* p3 = NurseryAllocate(64);
    ASSERT_NE(p2, nullptr);
    ASSERT_NE(p3, nullptr);

    std::memset(p2, 0xAB, 32);
    std::memset(p3, 0xCD, 64);

    auto* b2 = static_cast<uint8_t*>(p2);
    auto* b3 = static_cast<uint8_t*>(p3);

    EXPECT_EQ(b2[0], 0xAB);
    EXPECT_EQ(b2[31], 0xAB);
    EXPECT_EQ(b3[0], 0xCD);
    EXPECT_EQ(b3[63], 0xCD);

    auto addr2 = reinterpret_cast<uintptr_t>(p2);
    auto addr3 = reinterpret_cast<uintptr_t>(p3);
    auto end2 = addr2 + 32;
    auto end3 = addr3 + 64;
    EXPECT_FALSE(addr2 < end3 && addr3 < end2) << "overlapping allocations";
}

TEST(GcRegion, NurseryAllocateAlignment) {
    void* p4 = NurseryAllocate(1);
    ASSERT_NE(p4, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(p4) & 7, 0u) << "misaligned";
}

TEST(GcRegion, NurseryAllocateOversizedBypasses) {
    void* large = NurseryAllocate(kMaxTlabAlloc + 1);
    ASSERT_NE(large, nullptr);
    auto* blarge = static_cast<uint8_t*>(large);
    EXPECT_EQ(blarge[0], 0);
    EXPECT_EQ(blarge[kMaxTlabAlloc], 0);
}

// ════════════════════════════════════════════════════════════════════════════
// Card table: DirtyCard / IsDirty / ClearCard / ScanDirtyCards / GcSetHeapBase
// ════════════════════════════════════════════════════════════════════════════

struct CardTableTest : ::testing::Test {
    void* heap = nullptr;
    uintptr_t base = 0;

    void SetUp() override {
        heap = std::calloc(1, 1024 * 1024);
        ASSERT_NE(heap, nullptr);
        GcSetHeapBase(heap);
        base = reinterpret_cast<uintptr_t>(heap);
    }

    void TearDown() override {
        std::free(heap);
    }
};

TEST_F(CardTableTest, DirtyAndIsDirty) {
    void* obj = reinterpret_cast<void*>(base + 4096);
    EXPECT_FALSE(IsDirty(obj)) << "should be clean initially";
    DirtyCard(obj);
    EXPECT_TRUE(IsDirty(obj)) << "should be dirty after DirtyCard";
}

TEST_F(CardTableTest, ClearCard) {
    void* obj = reinterpret_cast<void*>(base + 4096);
    DirtyCard(obj);
    ClearCard(obj);
    EXPECT_FALSE(IsDirty(obj)) << "should be clean after ClearCard";
}

TEST_F(CardTableTest, ClearAllCards) {
    void* obj = reinterpret_cast<void*>(base + 4096);
    DirtyCard(obj);
    ClearAllCards();
    EXPECT_FALSE(IsDirty(obj)) << "should be clean after ClearAllCards";
}

TEST_F(CardTableTest, ScanDirtyCardsFindsDirty) {
    void* obj = reinterpret_cast<void*>(base + 4096);
    DirtyCard(obj);

    bool found = false;
    ScanDirtyCards(base, base + 65536, [&](uintptr_t, uintptr_t, uintptr_t) {
        found = true;
    });
    EXPECT_TRUE(found) << "ScanDirtyCards did not find dirty card";
}

TEST_F(CardTableTest, ScanDirtyCardsEmptyRange) {
    ClearAllCards();
    bool found = false;
    ScanDirtyCards(base, base + 65536, [&](uintptr_t, uintptr_t, uintptr_t) {
        found = true;
    });
    EXPECT_FALSE(found) << "ScanDirtyCards found something in clean range";
}

TEST_F(CardTableTest, DirtyCardOutOfRangeIsSafe) {
    DirtyCard(reinterpret_cast<void*>(base - 4096));
    DirtyCard(reinterpret_cast<void*>(base + (static_cast<uintptr_t>(kCardL1Entries) << kCardShift) + 65536));
}
