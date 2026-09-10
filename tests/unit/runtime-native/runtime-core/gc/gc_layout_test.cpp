/// gc_layout_test — GcLayoutRegistry unit tests (GoogleTest).
///
/// Tests:
///   1. Register / Lookup basic lifecycle
///   2. RegisterTypeInfoRange for TypeInfo pointer validation
///   3. RawAllocType registration (no-pointer layouts)
///   4. Sentinel initialization for size classes
///   5. Invalid lookup returns nullptr
///   6. RCU protocol: GrowTable → RetireTable → ReclaimRetiredTables
///   7. ReadStableId: TypeInfoHot and FakeTypeInfo paths
///   8. RegisterFinalizerCallback / LookupFinalizer
///   9. Sentinel content: instance_size matches kOldGenSizeClasses
///
/// Note: ScanObjectPointers requires a live nursery and GcUnitTestBase
/// fixture — tested in gc_young_collector integration tests instead.

#include <cstdint>
#include <cstring>

#include <chaos/native_types.h>
#include <chaos/type_info.h>
#include "gc_layout.h"
#include "gc_old_gen.h"

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

// ── Test 5: RCU protocol ────────────────────────────────────────────
// GrowTable → RetireTable → ReclaimRetiredTables lifecycle.
// Initial capacity is 4096, growth triggers at >75% load (~3072 entries).
// We register enough entries to force growth, then verify Lookup works
// and ReclaimRetiredTables safely frees the old table.

TEST(GcLayout, RCUProtocol) {
    auto& registry = GcLayoutRegistry::Instance();

    // Register distinct stable_ids to push the table past the 75% load
    // factor (3072 / 4096).  We use sequential IDs starting from a high
    // base to avoid collision with sentinel registrations from InitSentinels.
    constexpr uint64_t kRcuBase = 0x10000000;
    constexpr int kEntryCount = 3100;  // exceeds 75% of 4096

    for (int i = 0; i < kEntryCount; i++) {
        registry.Register(kRcuBase + i, 32 + (i % 64), nullptr, 0);
    }

    // Each entry should be findable.
    for (int i = 0; i < kEntryCount; i++) {
        const auto* layout = registry.Lookup(kRcuBase + i);
        ASSERT_NE(layout, nullptr) << "missing stable_id at index " << i;
        EXPECT_EQ(layout->instance_size, 32u + (i % 64));
        EXPECT_EQ(layout->pointer_count, 0u);
    }

    // ReclaimRetiredTables must not crash and should free old tables.
    // Safe to call even without a safepoint in single-threaded test.
    registry.ReclaimRetiredTables();

    // After reclamation, Lookup still works.
    const auto* layout = registry.Lookup(kRcuBase);
    ASSERT_NE(layout, nullptr);
    EXPECT_EQ(layout->instance_size, 32u);
}

// ── Test 6: ReadStableId ────────────────────────────────────────────
// Tests both TypeInfoHot layout (stable_id at +16) and FakeTypeInfo
// layout (stable_id at +0).  Also tests null/sentinel/invalid pointers.

namespace {

/// A struct matching the TypeInfoHot memory layout for ReadStableId testing.
/// TypeInfoHot: parent* (8) + vtable_array* (8) + stable_id (8) + ...
/// So stable_id is at offset +16 from the struct base.
struct alignas(8) TestTypeInfoHot {
    void* parent;           // offset +0
    void* vtable_array;     // offset +8
    uint64_t stable_id;     // offset +16
    uint64_t vtable_length; // offset +24
};

/// FakeTypeInfo: stable_id at +0, padding at +8 (used by some AOT stubs).
struct alignas(8) TestFakeTypeInfo {
    uint64_t stable_id;     // offset +0
    uint64_t padding;       // offset +8
};

}  // anonymous namespace

TEST(GcLayout, ReadStableIdTypeInfoHot) {
    auto& registry = GcLayoutRegistry::Instance();

    constexpr uint64_t kTestId = 0xABCD0001;
    TestTypeInfoHot ti{};
    ti.stable_id = kTestId;

    // Register the layout so ReadStableId can find it.
    registry.Register(kTestId, 32, nullptr, 0);

    // Register the TypeInfo address range so IsValidTypeInfoPointer works
    // (ReadStableId internally uses Lookup, but the caller is expected to
    // have validated the pointer beforehand).
    registry.RegisterTypeInfoRange(
        reinterpret_cast<uintptr_t>(&ti),
        reinterpret_cast<uintptr_t>(&ti) + sizeof(ti));

    uint64_t result = registry.ReadStableId(&ti);
    EXPECT_EQ(result, kTestId);

    // Null pointer returns 0.
    EXPECT_EQ(registry.ReadStableId(nullptr), 0u);
}

TEST(GcLayout, ReadStableIdFakeTypeInfo) {
    auto& registry = GcLayoutRegistry::Instance();

    constexpr uint64_t kTestId = 0xABCD0002;
    TestFakeTypeInfo fti{};
    fti.stable_id = kTestId;

    // Register the layout.
    registry.Register(kTestId, 32, nullptr, 0);

    // Register the address range.
    registry.RegisterTypeInfoRange(
        reinterpret_cast<uintptr_t>(&fti),
        reinterpret_cast<uintptr_t>(&fti) + sizeof(fti));

    uint64_t result = registry.ReadStableId(&fti);
    EXPECT_EQ(result, kTestId);
}

TEST(GcLayout, ReadStableIdInvalidPtr) {
    auto& registry = GcLayoutRegistry::Instance();

    // ReadStableId on an unregistered stable_id returns 0.
    // We can pass a valid-looking pointer whose stable_id field
    // contains a value that doesn't match any registered layout.
    TestTypeInfoHot ti{};
    ti.stable_id = 0xDEADBEEF;

    registry.RegisterTypeInfoRange(
        reinterpret_cast<uintptr_t>(&ti),
        reinterpret_cast<uintptr_t>(&ti) + sizeof(ti));

    // The range is valid but the stable_id 0xDEADBEEF is not registered.
    EXPECT_EQ(registry.ReadStableId(&ti), 0u);
}

// ── Test 7: Finalizer callback ──────────────────────────────────────

namespace {
    int g_test_finalizer_count = 0;
    void TestFinCb(void*) { g_test_finalizer_count++; }
    void TestFinCb2(void*) { g_test_finalizer_count += 2; }
}

TEST(GcLayout, RegisterAndLookupFinalizer) {
    auto& registry = GcLayoutRegistry::Instance();
    g_test_finalizer_count = 0;

    constexpr uint64_t kFinStableId = 0xF1A10001;

    // Before registration — lookup returns nullptr.
    auto* cb = registry.LookupFinalizer(kFinStableId);
    EXPECT_EQ(cb, nullptr);

    // Register and verify lookup returns the callback.
    registry.RegisterFinalizerCallback(kFinStableId, TestFinCb);
    cb = registry.LookupFinalizer(kFinStableId);
    ASSERT_NE(cb, nullptr);

    // Invoke the callback and verify it works.
    cb(nullptr);
    EXPECT_EQ(g_test_finalizer_count, 1);

    // Overwrite with a different callback.
    registry.RegisterFinalizerCallback(kFinStableId, TestFinCb2);
    cb = registry.LookupFinalizer(kFinStableId);
    ASSERT_NE(cb, nullptr);
    cb(nullptr);
    EXPECT_EQ(g_test_finalizer_count, 3);

    // Lookup of unregistered stable_id returns nullptr.
    EXPECT_EQ(registry.LookupFinalizer(0xDEAD), nullptr);
}

// ── Test 8: Sentinel content validation ─────────────────────────────
// Verify that each sentinel's GcTypeLayout reports instance_size matching
// kOldGenSizeClasses[sc_idx] with pointer_count = 0.

TEST(GcLayout, SentinelContent) {
    auto& registry = GcLayoutRegistry::Instance();

    for (int sc = 0; sc < kOldGenNumSizeClasses; sc++) {
        uint64_t sid = kSentinelStableIdBase + sc;
        const auto* layout = registry.Lookup(sid);
        ASSERT_NE(layout, nullptr)
            << "sentinel layout missing for size class " << sc;

        EXPECT_EQ(layout->instance_size, kOldGenSizeClasses[sc])
            << "size class mismatch at index " << sc;
        EXPECT_EQ(layout->pointer_count, 0u)
            << "sentinel should be pointer-free at index " << sc;

        // Verify the sentinel TypeInfo has the expected stable_id.
        const auto* sentinel_ti = registry.GetSentinelTypeInfo(sc);
        ASSERT_NE(sentinel_ti, nullptr);
        EXPECT_EQ(sentinel_ti->stable_id, kSentinelStableIdBase + sc);
    }
}
