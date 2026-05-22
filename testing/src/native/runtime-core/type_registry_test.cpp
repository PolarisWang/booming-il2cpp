/// Unit tests for the dynamic type registry API.
///
/// Tests chaos_register_type(), chaos_find_type_by_stable_id(), and
/// ChaosTypeAddInterface() directly by:
///   - Creating a MemoryDomain for allocation context
///   - Registering types with various configurations
///   - Verifying lookup, interface addition, edge cases

#include <gtest/gtest.h>
#include <chaos/native_types.h>
#include <chaos/type_info.h>

#include <memory_domain.h>
#include <type_registry.h>

#include <cstdint>
#include <cstdio>
#include <cstring>

using namespace chaos::il2cpp::runtime_core;
using namespace chaos::il2cpp::memory_domain;
using namespace chaos::il2cpp::common;

// ── Stub definitions for symbols normally emitted by AOT codegen ─────────
// These extern symbols are referenced by bootstrap and interpreter but are
// normally provided by the AOT codegen output.  This test doesn't run
// codegen, so we provide null/empty stubs.
namespace chaos { namespace il2cpp { namespace runtime_core {
extern "C" { void* chaos_il2cpp_aot_hotpatch_module = nullptr; }
extern "C" { const void* kChaosExternalRuntimeSubjects = nullptr; }
extern "C" { const void* kChaosExternalRuntimeFnTable = nullptr; }
extern "C" { int32_t kChaosExternalRuntimeCount = 0; }
}}}

// ════════════════════════════════════════════════════════════════════════════
// Fixture
// ════════════════════════════════════════════════════════════════════════════

class TypeRegistryTest : public ::testing::Test {
protected:
    // Shared domain across all tests: the type_registry is a singleton
    // that persists pointers across test cases, so we must keep the
    // domain alive for the entire suite.
    static MemoryDomain* s_domain;
    static DomainId      s_domain_id;

    static void SetUpTestSuite() {
        DomainInit init{};
        init.module_name = "TypeRegistryTestDomain";
        init.module_kind = 1;  // HotUpdate
        init.usage_limit = 0;  // unlimited

        s_domain_id = RegisterMemoryDomain(init);
        ASSERT_NE(s_domain_id, kDomainIdInvalid);
        s_domain = FindDomainById(s_domain_id);
        ASSERT_NE(s_domain, nullptr);
    }

    static void TearDownTestSuite() {
        if (s_domain_id != kDomainIdInvalid) {
            UnregisterMemoryDomain(s_domain_id);
            s_domain_id = kDomainIdInvalid;
            s_domain = nullptr;
        }
    }

    void SetUp() override {
        // Push domain onto this thread's stack.
        scope_depth_ = PushDomain(s_domain);
    }

    void TearDown() override {
        PopDomain(scope_depth_);
    }

    int scope_depth_ = -1;
};

MemoryDomain* TypeRegistryTest::s_domain = nullptr;
DomainId      TypeRegistryTest::s_domain_id = kDomainIdInvalid;

// ════════════════════════════════════════════════════════════════════════════
// Helper: interface map entry for tests
// ════════════════════════════════════════════════════════════════════════════

static constexpr uint64_t kIfaceStableIdA = 0x1FACE0A0A001ULL;
static constexpr uint64_t kIfaceStableIdB = 0x1FACE0B0B002ULL;

static const InterfaceMapEntry kTestInterfaceMap[] = {
    { kIfaceStableIdA, 0, 3 },  // 3 methods starting at slot 0
    { kIfaceStableIdB, 3, 2 },  // 2 methods starting at slot 3
};
static constexpr uint32_t kTestInterfaceCount = 2;

// ════════════════════════════════════════════════════════════════════════════
// Test cases
// ════════════════════════════════════════════════════════════════════════════

/// Register a simple reference type and find it by stable_id.
TEST_F(TypeRegistryTest, RegisterAndFind)
{
    uint64_t stable_id = 0;

    auto* mt = chaos_register_type("TestType", nullptr,
        chaos_type_shape_reference, nullptr, 0, &stable_id);
    ASSERT_NE(mt, nullptr);
    ASSERT_NE(stable_id, 0u);

    // Verify the returned MethodTable fields
    EXPECT_EQ(mt->stable_id, stable_id);
    EXPECT_EQ(mt->parent_mt, nullptr);
    EXPECT_EQ(mt->type_shape, chaos_type_shape_reference);
    EXPECT_EQ(mt->vtable_length, 0u);
    EXPECT_EQ(mt->iface_count, 0u);
    EXPECT_EQ(mt->runtime_iface_count, 0u);

    // Find by computed stable_id
    const auto* found = chaos_find_type_by_stable_id(stable_id);
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found, mt);
    EXPECT_EQ(found->stable_id, stable_id);
}

/// Register multiple types with different names; each should be findable.
TEST_F(TypeRegistryTest, RegisterMultipleTypes)
{
    static constexpr const char* kNames[] = {
        "TypeA",
        "TypeB",
        "TypeC",
    };
    static constexpr uint32_t kCount = 3;
    uint64_t ids[kCount] = {};

    for (uint32_t i = 0; i < kCount; i++) {
        auto* mt = chaos_register_type(kNames[i], nullptr,
            chaos_type_shape_reference, nullptr, 0, &ids[i]);
        ASSERT_NE(mt, nullptr) << "Failed at index " << i;
        ASSERT_NE(ids[i], 0u);
    }

    // Verify each is findable by its own stable_id.
    for (uint32_t i = 0; i < kCount; i++) {
        const auto* found = chaos_find_type_by_stable_id(ids[i]);
        ASSERT_NE(found, nullptr) << "Not found at index " << i;
        EXPECT_EQ(found->stable_id, ids[i]);
    }
}

/// Searching for a non-existent stable_id returns nullptr.
TEST_F(TypeRegistryTest, FindNonExistent)
{
    const auto* found = chaos_find_type_by_stable_id(0xDEADBEEF);
    EXPECT_EQ(found, nullptr);

    // Also search on an empty registry (no types registered yet)
    uint64_t fake_id = chaos_compute_type_stable_id("NonExistentType");
    found = chaos_find_type_by_stable_id(fake_id);
    EXPECT_EQ(found, nullptr);
}

/// Registering with a null name returns nullptr.
TEST_F(TypeRegistryTest, RegisterNullName)
{
    uint64_t stable_id = 0xDEAD;
    auto* mt = chaos_register_type(nullptr, nullptr,
        chaos_type_shape_reference, nullptr, 0, &stable_id);
    EXPECT_EQ(mt, nullptr);
    EXPECT_EQ(stable_id, 0u);
}

/// Register with a parent MethodTable.
TEST_F(TypeRegistryTest, RegisterWithParent)
{
    // Register parent first.
    uint64_t parent_id = 0;
    auto* parent = chaos_register_type("ParentType", nullptr,
        chaos_type_shape_reference, nullptr, 0, &parent_id);
    ASSERT_NE(parent, nullptr);

    // Register child with parent.
    uint64_t child_id = 0;
    auto* child = chaos_register_type("ChildType", parent,
        chaos_type_shape_reference, nullptr, 0, &child_id);
    ASSERT_NE(child, nullptr);
    ASSERT_NE(child_id, 0u);

    EXPECT_EQ(child->parent_mt, parent);
    EXPECT_NE(child->stable_id, parent_id);
}

/// Register with an interface map.
TEST_F(TypeRegistryTest, RegisterWithInterfaceMap)
{
    auto* mt = chaos_register_type("IfaceType", nullptr,
        chaos_type_shape_reference,
        kTestInterfaceMap, kTestInterfaceCount);
    ASSERT_NE(mt, nullptr);

    EXPECT_EQ(mt->iface_count, kTestInterfaceCount);
    ASSERT_NE(mt->iface_map, nullptr);
    EXPECT_EQ(mt->iface_map[0].iface_stable_id, kIfaceStableIdA);
    EXPECT_EQ(mt->iface_map[0].vtable_offset, 0u);
    EXPECT_EQ(mt->iface_map[0].method_count, 3u);
    EXPECT_EQ(mt->iface_map[1].iface_stable_id, kIfaceStableIdB);
    EXPECT_EQ(mt->iface_map[1].vtable_offset, 3u);
    EXPECT_EQ(mt->iface_map[1].method_count, 2u);
}

/// Append an interface at runtime (HotUpdate path).
TEST_F(TypeRegistryTest, AddInterface)
{
    auto* mt = chaos_register_type("DynamicType", nullptr,
        chaos_type_shape_reference, nullptr, 0);
    ASSERT_NE(mt, nullptr);
    EXPECT_EQ(mt->runtime_iface_count, 0u);
    EXPECT_EQ(mt->runtime_iface_map, nullptr);

    // Add interface A.
    bool ok = ChaosTypeAddInterface(mt, kIfaceStableIdA, 5, 4);
    EXPECT_TRUE(ok);
    EXPECT_EQ(mt->runtime_iface_count, 1u);
    ASSERT_NE(mt->runtime_iface_map, nullptr);
    EXPECT_EQ(mt->runtime_iface_map[0].iface_stable_id, kIfaceStableIdA);
    EXPECT_EQ(mt->runtime_iface_map[0].vtable_offset, 5u);
    EXPECT_EQ(mt->runtime_iface_map[0].method_count, 4u);

    // Add interface B.
    ok = ChaosTypeAddInterface(mt, kIfaceStableIdB, 9, 1);
    EXPECT_TRUE(ok);
    EXPECT_EQ(mt->runtime_iface_count, 2u);
    EXPECT_EQ(mt->runtime_iface_map[1].iface_stable_id, kIfaceStableIdB);
}

/// Adding the same interface twice is idempotent.
TEST_F(TypeRegistryTest, AddInterfaceIdempotent)
{
    auto* mt = chaos_register_type("IdempotentType", nullptr,
        chaos_type_shape_reference, nullptr, 0);
    ASSERT_NE(mt, nullptr);

    // First add should succeed.
    EXPECT_TRUE(ChaosTypeAddInterface(mt, kIfaceStableIdA, 0, 3));
    EXPECT_EQ(mt->runtime_iface_count, 1u);

    // Second add with same stable_id should succeed but not duplicate.
    EXPECT_TRUE(ChaosTypeAddInterface(mt, kIfaceStableIdA, 0, 3));
    EXPECT_EQ(mt->runtime_iface_count, 1u);  // still 1
}

/// Adding an interface already present in AOT iface_map should be a no-op.
TEST_F(TypeRegistryTest, AddInterfaceAlreadyInAotMap)
{
    auto* mt = chaos_register_type("PreMappedType", nullptr,
        chaos_type_shape_reference,
        kTestInterfaceMap, kTestInterfaceCount);
    ASSERT_NE(mt, nullptr);

    // Interface A is already in the AOT map. Adding it should return true
    // without duplicating in runtime_iface_map.
    EXPECT_TRUE(ChaosTypeAddInterface(mt, kIfaceStableIdA, 0, 3));
    EXPECT_EQ(mt->runtime_iface_count, 0u);  // runtime map unchanged
}

/// Calling ChaosTypeAddInterface with null MethodTable returns false.
TEST_F(TypeRegistryTest, AddInterfaceNullMT)
{
    EXPECT_FALSE(ChaosTypeAddInterface(nullptr, kIfaceStableIdA, 0, 3));
}

/// Registering a value type.
TEST_F(TypeRegistryTest, RegisterValueType)
{
    uint64_t stable_id = 0;
    auto* mt = chaos_register_type("MyValueType", nullptr,
        chaos_type_shape_value, nullptr, 0, &stable_id);
    ASSERT_NE(mt, nullptr);
    EXPECT_EQ(mt->type_shape, chaos_type_shape_value);
    EXPECT_NE(stable_id, 0u);

    const auto* found = chaos_find_type_by_stable_id(stable_id);
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->type_shape, chaos_type_shape_value);
}

/// Register up to the max limit and verify overflow returns nullptr.
/// Note: This test is intentionally last because it fills the registry.
/// The type_registry is a singleton, so earlier tests may have left entries.
/// We measure the actual remaining capacity and verify the cap.
TEST_F(TypeRegistryTest, RegisterUntilFull)
{
    static constexpr uint32_t kMaxAttempts = kChaosMaxDynamicTypes + 10;
    uint32_t success_count = 0;

    for (uint32_t i = 0; i < kMaxAttempts; i++) {
        char name[64];
        ::snprintf(name, sizeof(name), "OverflowType_%u", i);

        auto* mt = chaos_register_type(name, nullptr,
            chaos_type_shape_reference, nullptr, 0);
        if (mt == nullptr) {
            break;  // registry full
        }
        success_count++;
    }

    // The total registered (including entries from earlier tests) should
    // be exactly kChaosMaxDynamicTypes when the registry rejects more.
    // Since earlier tests left some entries, success_count is the remainder.
    uint64_t verify_id = chaos_compute_type_stable_id("OverflowType_0");
    const auto* any_overflow = chaos_find_type_by_stable_id(verify_id);
    if (any_overflow != nullptr) {
        // At least some entries from this test were stored.
        // The very next attempt should fail.
        auto* mt = chaos_register_type("FinalCheck", nullptr,
            chaos_type_shape_reference, nullptr, 0);
        EXPECT_EQ(mt, nullptr) << "Registry should be full after " << success_count
                               << " additional registrations";
    }
}
