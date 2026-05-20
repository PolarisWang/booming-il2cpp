/// gc_layout_test — GcLayoutRegistry unit tests (GoogleTest).
///
/// Tests:
///   1. Register / Lookup basic lifecycle
///   2. RegisterTypeInfoRange for TypeInfo pointer validation
///   3. RawAllocType registration (no-pointer layouts)
///   4. Sentinel initialization for size classes
///   5. Invalid lookup returns nullptr

#include <cstdint>

#include <chaos/native_types.h>
#include "gc_layout.h"

#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

// ── Test 1: Register / Lookup ───────────────────────────────────────

TEST(GcLayout, RegisterLookup) {
    auto& registry = GcLayoutRegistry::Instance();

    uint16_t offsets[] = {8, 24};
    registry.Register(42, 32, offsets, 2);

    const auto* layout = registry.Lookup(42);
    EXPECT_NE(layout, nullptr);

    // RegisterTypeInfoRange
    char type_data[64] = {};
    uintptr_t begin = reinterpret_cast<uintptr_t>(type_data);
    uintptr_t end = begin + sizeof(type_data);
    registry.RegisterTypeInfoRange(begin, end);

    EXPECT_TRUE(registry.IsValidTypeInfoPointer(type_data));
    EXPECT_FALSE(registry.IsValidTypeInfoPointer(
        reinterpret_cast<void*>(0xDEAD)));
}

// ── Test 2: RawAllocType (no-pointer) registration ───────────────────

TEST(GcLayout, RawAllocType) {
    auto& registry = GcLayoutRegistry::Instance();

    uint32_t id = registry.RegisterOrGetRawAllocType(64);
    EXPECT_GT(id, 0u);

    uint32_t id2 = registry.RegisterOrGetRawAllocType(64);
    EXPECT_EQ(id, id2);
}

// ── Test 3: Sentinel initialization ─────────────────────────────────

TEST(GcLayout, SentinelInit) {
    auto& registry = GcLayoutRegistry::Instance();
    registry.InitSentinels();

    for (int i = 0; i < 32; i++) {
        auto* sentinel = registry.GetSentinelTypeInfo(i);
        (void)sentinel;
    }
}

// ── Test 4: Invalid lookup ──────────────────────────────────────────

TEST(GcLayout, InvalidLookup) {
    auto& registry = GcLayoutRegistry::Instance();

    const auto* layout = registry.Lookup(999999);
    EXPECT_EQ(layout, nullptr);
}
