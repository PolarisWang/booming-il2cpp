/// common_type_info_test — Unit tests for type_info.h TypeInfo/MethodTable layout.
///
/// Tests static_assert sizes, offset compatibility between TypeInfoHot and
/// MethodTable, GetWarmPtr, AsMethodTable conversion, and core type constants.

#include <gtest/gtest.h>
#include <chaos/common.h>
#include <chaos/type_info.h>
#include <cstddef>  // offsetof

using namespace chaos::il2cpp::common;

// ═══════════════════════════════════════════════════════════════════════════
// Struct sizes (compile-time assertions re-verified at runtime)
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonTypeInfo, TypeInfoHotSize) {
    EXPECT_EQ(32, sizeof(TypeInfoHot));
}

TEST(CommonTypeInfo, TypeInfoWarmSize) {
    EXPECT_EQ(32, sizeof(TypeInfoWarm));
}

TEST(CommonTypeInfo, TypeInfoV0Size) {
    EXPECT_EQ(64, sizeof(TypeInfoV0));
}

TEST(CommonTypeInfo, TypeInfoV2Size) {
    EXPECT_EQ(112, sizeof(TypeInfoV2));
}

TEST(CommonTypeInfo, InterfaceMapEntrySize) {
    EXPECT_EQ(16, sizeof(InterfaceMapEntry));
}

TEST(CommonTypeInfo, MethodTableSize) {
    EXPECT_EQ(64, sizeof(MethodTable));
}

// ═══════════════════════════════════════════════════════════════════════════
// TypeInfoHot field offsets
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonTypeInfo, TypeInfoHotFieldOffsets) {
    EXPECT_EQ(0, offsetof(TypeInfoHot, parent));
    EXPECT_EQ(8, offsetof(TypeInfoHot, vtable_array));
    EXPECT_EQ(16, offsetof(TypeInfoHot, stable_id));
    EXPECT_EQ(24, offsetof(TypeInfoHot, vtable_length));
    EXPECT_EQ(28, offsetof(TypeInfoHot, warm_delta));
    EXPECT_EQ(30, offsetof(TypeInfoHot, type_shape));
    EXPECT_EQ(31, offsetof(TypeInfoHot, flags));
}

// ═══════════════════════════════════════════════════════════════════════════
// MethodTable hot section offset compatibility with TypeInfoHot
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonTypeInfo, MethodTableOffsetCompat) {
    EXPECT_EQ(offsetof(MethodTable, parent_mt), offsetof(TypeInfoHot, parent));
    EXPECT_EQ(offsetof(MethodTable, vtable_array), offsetof(TypeInfoHot, vtable_array));
    EXPECT_EQ(offsetof(MethodTable, stable_id), offsetof(TypeInfoHot, stable_id));
    EXPECT_EQ(offsetof(MethodTable, vtable_length), offsetof(TypeInfoHot, vtable_length));
    EXPECT_EQ(offsetof(MethodTable, warm_delta), offsetof(TypeInfoHot, warm_delta));
    EXPECT_EQ(offsetof(MethodTable, type_shape), offsetof(TypeInfoHot, type_shape));
    EXPECT_EQ(offsetof(MethodTable, flags), offsetof(TypeInfoHot, flags));
}

// ═══════════════════════════════════════════════════════════════════════════
// TypeInfoV0 warm section offset
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonTypeInfo, TypeInfoV0WarmOffset) {
    EXPECT_EQ(sizeof(TypeInfoHot), offsetof(TypeInfoV0, warm));
}

// ═══════════════════════════════════════════════════════════════════════════
// TypeInfoV2 inline_slots offset (warm + sizeof(TypeInfoWarm))
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonTypeInfo, TypeInfoV2InlineSlotsOffset) {
    EXPECT_EQ(offsetof(TypeInfoV0, warm) + sizeof(TypeInfoWarm),
              offsetof(TypeInfoV2, inline_slots));
}

// ═══════════════════════════════════════════════════════════════════════════
// GetWarmPtr — construct TypeInfoV0 on stack, set warm_delta=32
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonTypeInfo, GetWarmPtrFromTypeInfoV0) {
    TypeInfoV0 v0{};
    v0.hot.warm_delta = 32;  // sizeof(TypeInfoHot) = distance to warm

    const auto* warm = GetWarmPtr(&v0.hot);
    EXPECT_EQ(reinterpret_cast<const void*>(&v0.warm),
              reinterpret_cast<const void*>(warm));
}

TEST(CommonTypeInfo, GetWarmPtrMutable) {
    TypeInfoV0 v0{};
    v0.hot.warm_delta = 32;

    auto* warm = GetWarmPtr(&v0.hot);
    warm->iface_count = 42;
    EXPECT_EQ(42, v0.warm.iface_count);
}

// ═══════════════════════════════════════════════════════════════════════════
// AsMethodTable conversion
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonTypeInfo, AsMethodTable) {
    MethodTable mt{};
    mt.stable_id = 0xDEADBEEF;
    mt.iface_count = 7;

    const TypeInfoHot* hot = mt.AsTypeInfoHot();
    EXPECT_EQ(0xDEADBEEF, hot->stable_id);

    const MethodTable* back = hot->AsMethodTable();
    EXPECT_EQ(7, back->iface_count);
}

// ═══════════════════════════════════════════════════════════════════════════
// InterfaceMapEntry field layout
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonTypeInfo, InterfaceMapEntryLayout) {
    InterfaceMapEntry entry{};
    entry.iface_stable_id = 0xAABBCCDD;
    entry.vtable_offset = 5;
    entry.method_count = 3;

    EXPECT_EQ(0xAABBCCDD, entry.iface_stable_id);
    EXPECT_EQ(5u, entry.vtable_offset);
    EXPECT_EQ(3u, entry.method_count);

    EXPECT_EQ(16, sizeof(entry));
}

// ═══════════════════════════════════════════════════════════════════════════
// TypeInfo flags and constants
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonTypeInfo, TypeShapeConstants) {
    EXPECT_EQ(1, chaos_type_shape_reference);
    EXPECT_EQ(2, chaos_type_shape_value);
    EXPECT_EQ(3, chaos_type_shape_interface);
}

TEST(CommonTypeInfo, TypeInfoFlags) {
    EXPECT_EQ(0x03, kTypeInfoHeaderKindMask);
    EXPECT_EQ(0x00, kTypeInfoHeaderKindPure);
    EXPECT_EQ(0x01, kTypeInfoHeaderKindThin);
    EXPECT_EQ(0x02, kTypeInfoHeaderKindFat);
    EXPECT_EQ(0x04, kTypeInfoHasFinalizer);
}

TEST(CommonTypeInfo, MaxDynamicTypes) {
    EXPECT_EQ(256, kChaosMaxDynamicTypes);
}

// ═══════════════════════════════════════════════════════════════════════════
// FNV-1a stable ID hash
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonTypeInfo, StableIdEmptyString) {
    // FNV-1a offset basis for empty string
    uint64_t hash = chaos_compute_type_stable_id("");
    EXPECT_EQ(14695981039346656037ULL, hash);
}

TEST(CommonTypeInfo, StableIdNonEmpty) {
    uint64_t h1 = chaos_compute_type_stable_id("System.Int32");
    uint64_t h2 = chaos_compute_type_stable_id("System.Int32");
    EXPECT_EQ(h1, h2);
    // Reasonably non-zero for typical names
    EXPECT_NE(14695981039346656037ULL, h1);
}

TEST(CommonTypeInfo, StableIdDifferent) {
    uint64_t h1 = chaos_compute_type_stable_id("A");
    uint64_t h2 = chaos_compute_type_stable_id("B");
    EXPECT_NE(h1, h2);
}

TEST(CommonTypeInfo, StableIdConstexpr) {
    // Verify constexpr evaluation at compile time
    constexpr uint64_t h = chaos_compute_type_stable_id("System.Object");
    (void)h;
    EXPECT_TRUE(true);
}

// ═══════════════════════════════════════════════════════════════════════════
// TypeInfoV0 and TypeInfoV2 default construction
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonTypeInfo, TypeInfoV0DefaultMembers) {
    TypeInfoV0 v0{};
    EXPECT_EQ(nullptr, v0.hot.parent);
    EXPECT_EQ(nullptr, v0.hot.vtable_array);
    EXPECT_EQ(0u, v0.hot.stable_id);
    EXPECT_EQ(0u, v0.hot.vtable_length);
    EXPECT_EQ(nullptr, v0.warm.iface_map);
    EXPECT_EQ(nullptr, v0.warm.runtime_iface_map);
}

TEST(CommonTypeInfo, TypeInfoV2InlineSlotCount) {
    TypeInfoV2 v2{};
    EXPECT_EQ(6u, sizeof(v2.inline_slots) / sizeof(const void*));
}

// ═══════════════════════════════════════════════════════════════════════════
// InterfaceMapEntry stable_id + offset + method_count round trip
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonTypeInfo, InterfaceMapEntryRoundTrip) {
    InterfaceMapEntry e;
    e.iface_stable_id = 0x1234567890ABCDEFULL;
    e.vtable_offset = 10;
    e.method_count = 5;

    ASSERT_EQ(0x1234567890ABCDEFULL, e.iface_stable_id);
    ASSERT_EQ(10u, e.vtable_offset);
    ASSERT_EQ(5u, e.method_count);

    // Verify packed layout: stable_id(8) + vtable_offset(4) + method_count(4)
    EXPECT_EQ(16, sizeof(e));
}

// ═══════════════════════════════════════════════════════════════════════════
// TypeInfoV0 warm section via TypeInfoV2 (shared first 64B layout)
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonTypeInfo, TypeInfoV2HotWarmCompatibleWithV0) {
    TypeInfoV2 v2{};
    v2.hot.warm_delta = 32;
    v2.warm.iface_count = 7;

    // The first 64 bytes of V2 should match V0 layout
    auto* as_v0 = reinterpret_cast<TypeInfoV0*>(&v2);
    EXPECT_EQ(7u, as_v0->warm.iface_count);
}
