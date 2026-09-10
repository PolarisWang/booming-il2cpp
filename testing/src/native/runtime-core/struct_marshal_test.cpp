// struct_marshal_test.cpp — Struct marshalling descriptor tests (Phase C1).
//
// Tests descriptor registration/lookup, field name resolution,
// field kind enum coverage, and null safety for all descriptor APIs.
//
// These tests operate on pure data structures (hash table registries)
// and do NOT require a managed GC heap or TypeInfoHot allocation.
// TypeInfoHot mock uses a minimal header with stable_id at offset 16.

#include "marshal_test_fixture.h"

#include <chaos/type_info.h>

#include <cstdint>
#include <cstring>

using namespace chaos::il2cpp::runtime_core;
using namespace chaos::il2cpp::marshal_abi;

// ════════════════════════════════════════════════════════════════════════════
// C1 — Struct marshalling descriptor tests
// ════════════════════════════════════════════════════════════════════════════

class StructMarshalTest : public MarshalTestFixture {
protected:
    // ── Helper: build a simple descriptor with N fields ─────────────────
    static StructMarshallingDescriptorV1* MakeDescriptor(
        uint16_t total_size, uint16_t field_count, uint16_t base_offset = 0)
    {
        const size_t bytes = sizeof(StructMarshallingDescriptorV1)
                           + field_count * sizeof(StructFieldDescriptorV1);
        auto* buf = static_cast<StructMarshallingDescriptorV1*>(std::malloc(bytes));
        std::memset(buf, 0, bytes);
        buf->total_size = total_size;
        buf->field_count = field_count;
        for (uint16_t i = 0; i < field_count; ++i) {
            buf->fields[i].kind = StructFieldKind::Blittable;
            buf->fields[i].offset = base_offset + i * 4;
            buf->fields[i].size = 4;
        }
        return buf;
    }

    // ── Helper: minimal TypeInfoHot with only stable_id populated ───────
    static chaos::il2cpp::common::TypeInfoHot* MakeTypeInfo(CHAOS_IL2CPP_UINT64 stable_id)
    {
        auto* ti = static_cast<chaos::il2cpp::common::TypeInfoHot*>(
            std::malloc(sizeof(chaos::il2cpp::common::TypeInfoHot)));
        std::memset(ti, 0, sizeof(chaos::il2cpp::common::TypeInfoHot));
        ti->stable_id = stable_id;
        return ti;
    }

    // ── Field name arrays for testing ───────────────────────────────────
    static const char* kFieldNamesAB[2];
    static const char* kFieldNamesXYZ[3];
};

const char* StructMarshalTest::kFieldNamesAB[2]  = { "fieldA", "fieldB" };
const char* StructMarshalTest::kFieldNamesXYZ[3] = { "x", "y", "z" };

// ── Registration null/zero guards ──────────────────────────────────────────

TEST_F(StructMarshalTest, RegisterDescriptor_NullDescriptor) {
    // Null descriptor should be a no-op (no crash, no entry created)
    RegisterStaticMarshallingDescriptor(42, nullptr);
    // No direct way to inspect g_static_descriptors from test, so just
    // verify we can still register and resolve a valid one afterwards.
    auto* desc = MakeDescriptor(8, 1);
    RegisterStaticMarshallingDescriptor(42, desc);
    auto* ti = MakeTypeInfo(42);
    EXPECT_NE(ResolveStructMarshallingDescriptor(ti), nullptr);
    std::free(ti);
    std::free(desc);
}

TEST_F(StructMarshalTest, RegisterDescriptor_ZeroStableId) {
    auto* desc = MakeDescriptor(4, 0);
    // stable_id == 0 → registration should be skipped
    RegisterStaticMarshallingDescriptor(0, desc);
    // Bogus stable_id → should NOT find anything registered under 0
    EXPECT_EQ(ResolveStaticMarshallingFieldNames(0), nullptr);
    std::free(desc);
}

TEST_F(StructMarshalTest, RegisterFieldNames_NullNames) {
    // Null field_names pointer → no-op
    RegisterStaticMarshallingFieldNames(100, nullptr);
    EXPECT_EQ(ResolveStaticMarshallingFieldNames(100), nullptr);
}

TEST_F(StructMarshalTest, RegisterFieldNames_ZeroStableId) {
    RegisterStaticMarshallingFieldNames(0, kFieldNamesAB);
    EXPECT_EQ(ResolveStaticMarshallingFieldNames(0), nullptr);
}

// ── Positive roundtrip tests ──────────────────────────────────────────────

TEST_F(StructMarshalTest, RegisterAndResolveDescriptor) {
    auto* desc = MakeDescriptor(16, 2, 0);
    const CHAOS_IL2CPP_UINT64 sid = 0xABCD5678;

    RegisterStaticMarshallingDescriptor(sid, desc);
    auto* ti = MakeTypeInfo(sid);
    auto* resolved = ResolveStructMarshallingDescriptor(ti);
    ASSERT_NE(resolved, nullptr);
    EXPECT_EQ(resolved->total_size, 16);
    EXPECT_EQ(resolved->field_count, 2);
    EXPECT_EQ(resolved->fields[0].offset, 0);
    EXPECT_EQ(resolved->fields[0].size, 4);
    EXPECT_EQ(resolved->fields[1].offset, 4);
    EXPECT_EQ(resolved->fields[1].size, 4);

    std::free(ti);
    std::free(desc);
}

TEST_F(StructMarshalTest, RegisterDescriptor_Duplicate) {
    auto* desc1 = MakeDescriptor(8, 1, 0);
    auto* desc2 = MakeDescriptor(16, 2, 0);
    const CHAOS_IL2CPP_UINT64 sid = 0xD0D0D0D0;

    // First registration wins
    RegisterStaticMarshallingDescriptor(sid, desc1);
    RegisterStaticMarshallingDescriptor(sid, desc2);

    auto* ti = MakeTypeInfo(sid);
    auto* resolved = ResolveStructMarshallingDescriptor(ti);
    ASSERT_NE(resolved, nullptr);
    // Should return desc1 (first registration wins — try_emplace)
    EXPECT_EQ(resolved->total_size, 8);
    EXPECT_EQ(resolved->field_count, 1);

    std::free(ti);
    std::free(desc1);
    std::free(desc2);
}

TEST_F(StructMarshalTest, RegisterAndResolveFieldNames) {
    const CHAOS_IL2CPP_UINT64 sid = 0xF1E1D0123;

    RegisterStaticMarshallingFieldNames(sid, kFieldNamesXYZ);
    auto* names = ResolveStaticMarshallingFieldNames(sid);
    ASSERT_NE(names, nullptr);
    EXPECT_STREQ(names[0], "x");
    EXPECT_STREQ(names[1], "y");
    EXPECT_STREQ(names[2], "z");
}

TEST_F(StructMarshalTest, RegisterFieldNames_MultipleEntries) {
    const CHAOS_IL2CPP_UINT64 sidA = 0xF1E1D0A0;
    const CHAOS_IL2CPP_UINT64 sidB = 0xF1E1D0B0;

    RegisterStaticMarshallingFieldNames(sidA, kFieldNamesAB);
    RegisterStaticMarshallingFieldNames(sidB, kFieldNamesXYZ);

    auto* namesA = ResolveStaticMarshallingFieldNames(sidA);
    ASSERT_NE(namesA, nullptr);
    EXPECT_STREQ(namesA[0], "fieldA");
    EXPECT_STREQ(namesA[1], "fieldB");

    auto* namesB = ResolveStaticMarshallingFieldNames(sidB);
    ASSERT_NE(namesB, nullptr);
    EXPECT_STREQ(namesB[0], "x");
    EXPECT_STREQ(namesB[1], "y");
    EXPECT_STREQ(namesB[2], "z");
}

// ── NotFound / null safety ────────────────────────────────────────────────

TEST_F(StructMarshalTest, ResolveFieldNames_NotFound) {
    EXPECT_EQ(ResolveStaticMarshallingFieldNames(0xDEADBEEF), nullptr);
}

TEST_F(StructMarshalTest, ResolveDescriptor_NotFound) {
    const CHAOS_IL2CPP_UINT64 sidNotFound = 0x0BADBEEF;
    auto* ti = MakeTypeInfo(sidNotFound);
    EXPECT_EQ(ResolveStructMarshallingDescriptor(ti), nullptr);
    std::free(ti);
}

TEST_F(StructMarshalTest, ResolveDescriptor_NullType) {
    EXPECT_EQ(ResolveStructMarshallingDescriptor(nullptr), nullptr);
}

// ── Descriptor field iteration ────────────────────────────────────────────

TEST_F(StructMarshalTest, DescriptorFieldIteration) {
    // Create a descriptor with 3 fields at different offsets
    auto* desc = MakeDescriptor(24, 3, 0);
    desc->fields[0].kind = StructFieldKind::Blittable;
    desc->fields[0].offset = 0;
    desc->fields[0].size = 4;
    desc->fields[1].kind = StructFieldKind::BoolField;
    desc->fields[1].offset = 4;
    desc->fields[1].size = 1;
    desc->fields[2].kind = StructFieldKind::GuidField;
    desc->fields[2].offset = 8;
    desc->fields[2].size = 16;

    const CHAOS_IL2CPP_UINT64 sid = 0xABCD1234;
    RegisterStaticMarshallingDescriptor(sid, desc);

    auto* ti = MakeTypeInfo(sid);
    auto* resolved = ResolveStructMarshallingDescriptor(ti);
    ASSERT_NE(resolved, nullptr);
    EXPECT_EQ(resolved->total_size, 24);
    EXPECT_EQ(resolved->field_count, 3);

    // Check each field
    EXPECT_EQ(resolved->fields[0].kind, StructFieldKind::Blittable);
    EXPECT_EQ(resolved->fields[0].offset, 0);
    EXPECT_EQ(resolved->fields[0].size, 4);

    EXPECT_EQ(resolved->fields[1].kind, StructFieldKind::BoolField);
    EXPECT_EQ(resolved->fields[1].offset, 4);
    EXPECT_EQ(resolved->fields[1].size, 1);

    EXPECT_EQ(resolved->fields[2].kind, StructFieldKind::GuidField);
    EXPECT_EQ(resolved->fields[2].offset, 8);
    EXPECT_EQ(resolved->fields[2].size, 16);

    std::free(ti);
    std::free(desc);
}

// ── StructFieldKind enum coverage ──────────────────────────────────────────

TEST_F(StructMarshalTest, AllFieldKindsCovered) {
    // Verify all 11 StructFieldKind values exist and are distinct
    // (compile-time enum coverage — no runtime allocation needed)
    StructFieldKind kinds[] = {
        StructFieldKind::Blittable,
        StructFieldKind::BoolField,
        StructFieldKind::StringField,
        StructFieldKind::NestedStruct,
        StructFieldKind::ByValArray,
        StructFieldKind::LPArray,
        StructFieldKind::DecimalField,
        StructFieldKind::DateTimeField,
        StructFieldKind::ObjectField,
        StructFieldKind::GuidField,
        StructFieldKind::CustomMarshalerField,
    };
    constexpr int kExpectedCount = 11;
    ASSERT_EQ(sizeof(kinds) / sizeof(kinds[0]), kExpectedCount);

    // Check all are different (cast to int)
    for (int i = 0; i < kExpectedCount; ++i) {
        for (int j = i + 1; j < kExpectedCount; ++j) {
            EXPECT_NE(static_cast<int>(kinds[i]), static_cast<int>(kinds[j]));
        }
    }
}

// ── Descriptor structure layout ───────────────────────────────────────────

TEST_F(StructMarshalTest, DescriptorStructureSizes) {
    // Verify struct sizes are within expected range.
    // Exact sizes depend on platform padding (MSVC x64, GCC, etc.),
    // so only check minimum expected sizes.
    EXPECT_GE(sizeof(StructFieldDescriptorV1), sizeof(void*) * 2u + 6u);
    EXPECT_GE(sizeof(StructMarshallingDescriptorV1), 4u); // total_size(2) + field_count(2)
}
