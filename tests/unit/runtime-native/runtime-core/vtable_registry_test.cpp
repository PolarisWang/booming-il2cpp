/// Isolation tests for the unified VTable Registry.
///
/// Tests the vtable_registry API directly by constructing TypeVTable instances
/// and verifying registration, lookup, inheritance chain walking, interface
/// offset resolution, epoch propagation, and cleanup.
///
/// IMPORTANT: vtable_registry's UnregisterTypeVTable() frees ALL pointers
/// (slots, vtable_array, runtime_iface_map, and the TypeVTable itself) via
/// CHAOS_IL2CPP_FREE.  Therefore, only vtables that were internally
/// heap-allocated (e.g. via RegisterCodegenVTable) should be unregistered.
/// Tests that use static TypeVTable / static slot arrays must NOT call
/// UnregisterTypeVTable — the OS reclaims everything on process exit.

#include <gtest/gtest.h>
#include <chaos/native_types.h>
#include <chaos/type_info.h>

#include <vtable_registry.h>
#include <codegen_bridge.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>

using namespace chaos::il2cpp::common;
using namespace chaos::il2cpp::vtable_registry;

// ── Stub definitions for symbols normally emitted by AOT codegen ─────────
namespace chaos { namespace il2cpp { namespace runtime_core {
extern "C" { void* chaos_il2cpp_aot_hotpatch_module = nullptr; }
extern "C" { const void* kChaosExternalRuntimeSubjects = nullptr; }
extern "C" { const void* kChaosExternalRuntimeFnTable = nullptr; }
extern "C" { int32_t kChaosExternalRuntimeCount = 0; }
}}}

// ════════════════════════════════════════════════════════════════════════════
// Constants
// ════════════════════════════════════════════════════════════════════════════

static constexpr uint32_t kTokenBase         = 0xF100u;
static constexpr uint32_t kTokenDerived1     = 0xF101u;
static constexpr uint32_t kTokenDerived2     = 0xF102u;
static constexpr uint32_t kTokenIface        = 0xF110u;
static constexpr uint32_t kTokenIfaceImpl    = 0xF111u;
static constexpr uint32_t kTokenHotParent    = 0xF120u;

static constexpr uint64_t kStableBase         = 0xF100000000000001ULL;
static constexpr uint64_t kStableDerived1     = 0xF100000000000002ULL;
static constexpr uint64_t kStableDerived2     = 0xF100000000000003ULL;
static constexpr uint64_t kStableIface        = 0xF100000000000010ULL;
static constexpr uint64_t kStableIfaceImpl    = 0xF100000000000011ULL;
static constexpr uint64_t kStableHotParent    = 0xF100000000000020ULL;

static constexpr uint32_t kMethodFoo = 0x1000u;
static constexpr uint32_t kMethodBar = 0x1001u;
static constexpr uint32_t kMethodBaz = 0x1002u;
static constexpr uint32_t kMethodQux = 0x1003u;

static void* const kPtrA = reinterpret_cast<void*>(static_cast<uintptr_t>(0xAA00u));
static void* const kPtrB = reinterpret_cast<void*>(static_cast<uintptr_t>(0xBB00u));
static void* const kPtrC = reinterpret_cast<void*>(static_cast<uintptr_t>(0xCC00u));
static void* const kPtrD = reinterpret_cast<void*>(static_cast<uintptr_t>(0xDD00u));
static void* const kPtrE = reinterpret_cast<void*>(static_cast<uintptr_t>(0xEE00u));

// ── Suite-level storage ──────────────────────────────────────────────────

#define SLOT_A { kMethodFoo, kPtrA }
#define SLOT_B { kMethodFoo, kPtrB }
#define SLOT_C { kMethodBar, kPtrC }
#define SLOT_D { kMethodBar, kPtrD }
#define SLOT_E { kMethodFoo, kPtrE }

static VTableSlot g_base_slots[1];
static VTableSlot g_derived1_slots[2];
static VTableSlot g_derived2_slots[1];
static VTableSlot g_iface_slots[2];
static VTableSlot g_iface_impl_slots[2];
static InterfaceMapEntry g_iface_entries[1];
static void* g_iface_impl_flat[4];
static VTableSlot g_hot_parent_slots[2];
static void* g_hot_parent_flat[2];

static TypeVTable g_base_vtable{};
static TypeVTable g_derived1_vtable{};
static TypeVTable g_derived2_vtable{};
static TypeVTable g_iface_vtable{};
static TypeVTable g_iface_impl_vtable{};
static TypeVTable g_hot_parent_vtable{};

// ════════════════════════════════════════════════════════════════════════════
// Fixture
// ════════════════════════════════════════════════════════════════════════════

class VTableRegistryTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        g_base_slots[0] = VTableSlot{ kMethodFoo, kPtrA };
        g_base_vtable = TypeVTable{};
        g_base_vtable.type_token  = kTokenBase;
        g_base_vtable.stable_id   = kStableBase;
        g_base_vtable.base_token  = 0;
        g_base_vtable.slot_count  = 1;
        g_base_vtable.slots       = g_base_slots;
        g_base_vtable.type_shape  = 1;
        RegisterTypeVTable(&g_base_vtable);

        g_derived1_slots[0] = VTableSlot{ kMethodFoo, kPtrB };
        g_derived1_slots[1] = VTableSlot{ kMethodBar, kPtrC };
        g_derived1_vtable = TypeVTable{};
        g_derived1_vtable.type_token  = kTokenDerived1;
        g_derived1_vtable.stable_id   = kStableDerived1;
        g_derived1_vtable.base_token  = kTokenBase;
        g_derived1_vtable.slot_count  = 2;
        g_derived1_vtable.slots       = g_derived1_slots;
        g_derived1_vtable.type_shape  = 1;
        RegisterTypeVTable(&g_derived1_vtable);

        g_derived2_slots[0] = VTableSlot{ kMethodBar, kPtrD };
        g_derived2_vtable = TypeVTable{};
        g_derived2_vtable.type_token  = kTokenDerived2;
        g_derived2_vtable.stable_id   = kStableDerived2;
        g_derived2_vtable.base_token  = kTokenDerived1;
        g_derived2_vtable.slot_count  = 1;
        g_derived2_vtable.slots       = g_derived2_slots;
        g_derived2_vtable.type_shape  = 1;
        RegisterTypeVTable(&g_derived2_vtable);

        g_iface_slots[0] = VTableSlot{ kMethodFoo, kPtrA };
        g_iface_slots[1] = VTableSlot{ kMethodBar, kPtrA };
        g_iface_vtable = TypeVTable{};
        g_iface_vtable.type_token  = kTokenIface;
        g_iface_vtable.stable_id   = kStableIface;
        g_iface_vtable.slot_count  = 2;
        g_iface_vtable.slots       = g_iface_slots;
        g_iface_vtable.type_shape  = 3;
        RegisterTypeVTable(&g_iface_vtable);

        g_iface_entries[0] = InterfaceMapEntry{ kStableIface, 2, 2 };
        g_iface_impl_slots[0] = VTableSlot{ kMethodFoo, kPtrB };
        g_iface_impl_slots[1] = VTableSlot{ kMethodBar, kPtrC };
        g_iface_impl_flat[0] = kPtrB;
        g_iface_impl_flat[1] = kPtrC;
        g_iface_impl_flat[2] = kPtrD;
        g_iface_impl_flat[3] = kPtrE;
        g_iface_impl_vtable = TypeVTable{};
        g_iface_impl_vtable.type_token    = kTokenIfaceImpl;
        g_iface_impl_vtable.stable_id     = kStableIfaceImpl;
        g_iface_impl_vtable.slot_count    = 2;
        g_iface_impl_vtable.slots         = g_iface_impl_slots;
        g_iface_impl_vtable.vtable_array  = const_cast<const void**>(g_iface_impl_flat);
        g_iface_impl_vtable.vtable_length = 4;
        g_iface_impl_vtable.iface_map     = g_iface_entries;
        g_iface_impl_vtable.iface_count   = 1;
        g_iface_impl_vtable.type_shape    = 1;
        RegisterTypeVTable(&g_iface_impl_vtable);

        g_hot_parent_slots[0] = VTableSlot{ kMethodFoo, kPtrA };
        g_hot_parent_slots[1] = VTableSlot{ kMethodBar, kPtrB };
        g_hot_parent_flat[0] = kPtrA;
        g_hot_parent_flat[1] = kPtrB;
        g_hot_parent_vtable = TypeVTable{};
        g_hot_parent_vtable.type_token    = kTokenHotParent;
        g_hot_parent_vtable.stable_id     = kStableHotParent;
        g_hot_parent_vtable.slot_count    = 2;
        g_hot_parent_vtable.slots         = g_hot_parent_slots;
        g_hot_parent_vtable.vtable_array  = const_cast<const void**>(g_hot_parent_flat);
        g_hot_parent_vtable.vtable_length = 2;
        g_hot_parent_vtable.type_shape    = 1;
        RegisterTypeVTable(&g_hot_parent_vtable);
    }
    // NOTE: No TearDownTestSuite cleanup.  Static vtables cannot be
    // safely freed by UnregisterTypeVTable since pointers point into
    // static storage, not CHAOS_IL2CPP_MALLOC heap.  OS reclaims on exit.
};

// ════════════════════════════════════════════════════════════════════════════
// Register + Lookup
// ════════════════════════════════════════════════════════════════════════════

TEST_F(VTableRegistryTest, RegisterAndFindByToken)
{
    static constexpr uint32_t kToken = 0xF200u;
    static constexpr uint64_t kStable = 0xF200000000000000ULL;
    static VTableSlot slots[] = { SLOT_E };
    static TypeVTable tv{};
    tv.type_token  = kToken;
    tv.stable_id   = kStable;
    tv.base_token  = kTokenBase;
    tv.slot_count  = 1;
    tv.slots       = slots;
    tv.type_shape  = 1;

    ASSERT_TRUE(RegisterTypeVTable(&tv));

    const auto* found = TryGetTypeVTable(kToken);
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->type_token, kToken);
    EXPECT_EQ(found->stable_id, kStable);
    EXPECT_EQ(found->slots[0].method_token, kMethodFoo);

    found = TryGetTypeVTableByStableId(kStable);
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->type_token, kToken);
}

TEST_F(VTableRegistryTest, RegisterTokenOnly)
{
    static constexpr uint32_t kToken = 0xF201u;
    static VTableSlot slots[] = { SLOT_A };
    static TypeVTable tv{};
    tv.type_token = kToken;
    tv.slot_count = 1;
    tv.slots      = slots;

    ASSERT_TRUE(RegisterTypeVTable(&tv));
    EXPECT_NE(TryGetTypeVTable(kToken), nullptr);
}

TEST_F(VTableRegistryTest, RegisterNullVTable)
{
    EXPECT_FALSE(RegisterTypeVTable(nullptr));
}

TEST_F(VTableRegistryTest, RegisterZeroToken)
{
    static VTableSlot slots[] = { SLOT_A };
    static TypeVTable tv{};
    tv.type_token = 0;
    tv.slot_count = 1;
    tv.slots      = slots;
    EXPECT_FALSE(RegisterTypeVTable(&tv));
}

TEST_F(VTableRegistryTest, RegisterNullSlots)
{
    static TypeVTable tv{};
    tv.type_token = 0xF202u;
    tv.slot_count = 1;
    tv.slots      = nullptr;
    EXPECT_FALSE(RegisterTypeVTable(&tv));
}

TEST_F(VTableRegistryTest, RegisterIdempotent)
{
    static constexpr uint32_t kToken = 0xF203u;
    static VTableSlot slots[] = { SLOT_A };
    static TypeVTable tv{};
    tv.type_token = kToken;
    tv.slot_count = 1;
    tv.slots      = slots;

    EXPECT_TRUE(RegisterTypeVTable(&tv));
    EXPECT_TRUE(RegisterTypeVTable(&tv));
}

TEST_F(VTableRegistryTest, FindNonExistentToken)
{
    EXPECT_EQ(TryGetTypeVTable(0xDEAD), nullptr);
    EXPECT_EQ(TryGetTypeVTable(0), nullptr);
}

TEST_F(VTableRegistryTest, FindNonExistentStableId)
{
    EXPECT_EQ(TryGetTypeVTableByStableId(0xDEADBEEF), nullptr);
    EXPECT_EQ(TryGetTypeVTableByStableId(0), nullptr);
}

// ════════════════════════════════════════════════════════════════════════════
// Flat vtable array
// ════════════════════════════════════════════════════════════════════════════

TEST_F(VTableRegistryTest, RegisterVTableArray)
{
    static constexpr uint64_t kStable = 0xF300000000000000ULL;
    static const void* fake_array[] = { kPtrA, kPtrB, kPtrC };

    RegisterVTableArray(kStable, fake_array, 3);

    const void** found = FindVTable(kStable);
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found[0], kPtrA);
    EXPECT_EQ(found[1], kPtrB);
    EXPECT_EQ(found[2], kPtrC);
    EXPECT_EQ(FindVTableLength(kStable), 3u);
}

TEST_F(VTableRegistryTest, RegisterVTableArrayGuard)
{
    static constexpr uint64_t kStable = 0xF301000000000000ULL;
    static const void* fake_array[] = { kPtrA };

    RegisterVTableArray(0, fake_array, 1);
    RegisterVTableArray(kStable, nullptr, 1);
    RegisterVTableArray(kStable, fake_array, 0);

    EXPECT_EQ(FindVTable(kStable), nullptr);
}

TEST_F(VTableRegistryTest, FindNonExistentVTable)
{
    EXPECT_EQ(FindVTable(0xDEAD), nullptr);
    EXPECT_EQ(FindVTableLength(0xDEAD), 0u);
}

// ════════════════════════════════════════════════════════════════════════════
// Codegen VTable registration
// ════════════════════════════════════════════════════════════════════════════

static constexpr uint32_t kTokenCodegen = 0xF030u;
static constexpr uint64_t kStableCodegen = 0xF030000000000000ULL;

TEST_F(VTableRegistryTest, RegisterCodegenVTable)
{
    static VTableSlot codegen_slots[] = { SLOT_A, SLOT_C };
    static const void* codegen_flat[] = { kPtrA, kPtrB };
    static InterfaceMapEntry codegen_iface_map[] = {
        { kStableIface, 0, 2 }
    };

    VTableDescriptorV0 desc;
    desc.stable_id       = kStableCodegen;
    desc.type_token      = kTokenCodegen;
    desc.base_token      = 0;
    desc.slot_count      = 2;
    desc.slots           = codegen_slots;
    desc.vtable_array    = codegen_flat;
    desc.vtable_length   = 2;
    desc.type_shape      = 1;
    desc._pad[0] = desc._pad[1] = desc._pad[2] = 0;
    desc.iface_map       = codegen_iface_map;
    desc.iface_count     = 1;

    RegisterCodegenVTable(&desc);

    const auto* found = TryGetTypeVTable(kTokenCodegen);
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->stable_id, kStableCodegen);
    EXPECT_EQ(found->slots[0].method_pointer, kPtrA);
    EXPECT_EQ(found->iface_count, 1u);
}

TEST_F(VTableRegistryTest, RegisterCodegenVTableNull)
{
    RegisterCodegenVTable(nullptr);
}

TEST_F(VTableRegistryTest, RegisterCodegenVTableZeroToken)
{
    VTableDescriptorV0 desc{};
    RegisterCodegenVTable(&desc);
}

// ════════════════════════════════════════════════════════════════════════════
// HotUpdate VTable registration
// ════════════════════════════════════════════════════════════════════════════

TEST_F(VTableRegistryTest, RegisterHotUpdateVTable)
{
    static constexpr uint32_t kToken = 0xF400u;
    static constexpr uint64_t kStable = 0xF400000000000000ULL;
    static VTableSlot overrides[] = { SLOT_D };

    ASSERT_TRUE(RegisterHotUpdateVTable(kStable, kToken, 0, overrides, 1, 1));
    ASSERT_NE(TryGetTypeVTable(kToken), nullptr);
    EXPECT_EQ(TryGetTypeVTable(kToken)->slot_count, 1u);
}

TEST_F(VTableRegistryTest, RegisterHotUpdateVTableWithParent)
{
    static constexpr uint32_t kToken = 0xF401u;
    static constexpr uint64_t kStable = 0xF401000000000000ULL;
    static VTableSlot overrides[] = { SLOT_E };

    ASSERT_TRUE(RegisterHotUpdateVTable(kStable, kToken, kTokenHotParent,
                                         overrides, 1, 1));

    const auto* found = TryGetTypeVTable(kToken);
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->base_token, kTokenHotParent);

    const void** flat = FindVTable(kStable);
    ASSERT_NE(flat, nullptr);
    EXPECT_EQ(flat[0], kPtrE);
    EXPECT_EQ(flat[1], kPtrB);

    // HotUpdate vtables use internal CHAOS_IL2CPP_MALLOC, so cleanup is safe.
    // But we skip it here since the test name is unique enough and the
    // growth is bounded.  Dedicated cleanup tests below verify unregister.
}

TEST_F(VTableRegistryTest, RegisterHotUpdateVTableZeroToken)
{
    EXPECT_FALSE(RegisterHotUpdateVTable(0, 0, 0, nullptr, 0, 1));
}

TEST_F(VTableRegistryTest, RegisterHotUpdateVTableIdempotent)
{
    static constexpr uint32_t kToken = 0xF402u;
    static constexpr uint64_t kStable = 0xF402000000000000ULL;
    static VTableSlot overrides[] = { SLOT_D };

    EXPECT_TRUE(RegisterHotUpdateVTable(kStable, kToken, 0, overrides, 1, 1));
    EXPECT_TRUE(RegisterHotUpdateVTable(kStable, kToken, 0, overrides, 1, 1));
}

// ════════════════════════════════════════════════════════════════════════════
// Runtime interface registration
// ════════════════════════════════════════════════════════════════════════════

TEST_F(VTableRegistryTest, RegisterRuntimeInterface)
{
    static constexpr uint32_t kToken = 0xF500u;
    static constexpr uint64_t kStable = 0xF500000000000000ULL;
    static VTableSlot slots[] = { SLOT_A };
    static TypeVTable tv{};
    tv.type_token  = kToken;
    tv.stable_id   = kStable;
    tv.slot_count  = 1;
    tv.slots       = slots;
    tv.type_shape  = 1;
    ASSERT_TRUE(RegisterTypeVTable(&tv));
    ASSERT_TRUE(RegisterTypeVTableRuntimeInterface(kToken, kStableIface, 5, 3));

    EXPECT_EQ(chaos_find_interface_offset(kToken, kStableIface), 5u);
}

TEST_F(VTableRegistryTest, RegisterRuntimeInterfaceIdempotent)
{
    static constexpr uint32_t kToken = 0xF501u;
    static constexpr uint64_t kStable = 0xF501000000000000ULL;
    static VTableSlot slots[] = { SLOT_A };
    static TypeVTable tv{};
    tv.type_token  = kToken;
    tv.stable_id   = kStable;
    tv.slot_count  = 1;
    tv.slots       = slots;
    tv.type_shape  = 1;
    ASSERT_TRUE(RegisterTypeVTable(&tv));

    ASSERT_TRUE(RegisterTypeVTableRuntimeInterface(kToken, kStableIface, 5, 3));
    ASSERT_TRUE(RegisterTypeVTableRuntimeInterface(kToken, kStableIface, 5, 3));

    EXPECT_EQ(chaos_find_interface_offset(kToken, kStableIface), 5u);
}

TEST_F(VTableRegistryTest, RegisterRuntimeInterfaceNotFoundToken)
{
    EXPECT_FALSE(RegisterTypeVTableRuntimeInterface(0xDEAD, kStableIface, 0, 1));
}

TEST_F(VTableRegistryTest, RegisterRuntimeInterfaceZeroArgs)
{
    EXPECT_FALSE(RegisterTypeVTableRuntimeInterface(0, kStableIface, 0, 1));
    EXPECT_FALSE(RegisterTypeVTableRuntimeInterface(1, 0, 0, 1));
}

// ════════════════════════════════════════════════════════════════════════════
// Interface offset lookup
// ════════════════════════════════════════════════════════════════════════════

TEST_F(VTableRegistryTest, FindInterfaceOffsetAot)
{
    EXPECT_EQ(chaos_find_interface_offset(kTokenIfaceImpl, kStableIface), 2u);
}

TEST_F(VTableRegistryTest, FindInterfaceOffsetNotFound)
{
    EXPECT_EQ(chaos_find_interface_offset(kTokenIfaceImpl, 0xDEAD), UINT32_MAX);
    EXPECT_EQ(chaos_find_interface_offset(0xDEAD, kStableIface), UINT32_MAX);
}

TEST_F(VTableRegistryTest, FindInterfaceOffsetZeroArgs)
{
    EXPECT_EQ(chaos_find_interface_offset(0, kStableIface), UINT32_MAX);
    EXPECT_EQ(chaos_find_interface_offset(1, 0), UINT32_MAX);
}

// ════════════════════════════════════════════════════════════════════════════
// Inheritance chain walking
// ════════════════════════════════════════════════════════════════════════════

TEST_F(VTableRegistryTest, ResolveVirtualMethodDirect)
{
    EXPECT_EQ(ResolveVirtualMethodPointer(kTokenBase, kMethodFoo), kPtrA);
}

TEST_F(VTableRegistryTest, ResolveVirtualMethodInheritanceDepth2)
{
    EXPECT_EQ(ResolveVirtualMethodPointer(kTokenDerived1, kMethodFoo), kPtrB);
    EXPECT_EQ(ResolveVirtualMethodPointer(kTokenDerived1, kMethodBar), kPtrC);
}

TEST_F(VTableRegistryTest, ResolveVirtualMethodInheritanceDepth3)
{
    EXPECT_EQ(ResolveVirtualMethodPointer(kTokenDerived2, kMethodFoo), kPtrB);
    EXPECT_EQ(ResolveVirtualMethodPointer(kTokenDerived2, kMethodBar), kPtrD);
    EXPECT_EQ(ResolveVirtualMethodPointer(kTokenDerived2, kMethodBaz), nullptr);
}

TEST_F(VTableRegistryTest, ResolveVirtualMethodIndependent)
{
    void* ptr_base    = ResolveVirtualMethodPointer(kTokenBase, kMethodFoo);
    void* ptr_derived = ResolveVirtualMethodPointer(kTokenDerived1, kMethodFoo);
    EXPECT_EQ(ptr_base, kPtrA);
    EXPECT_EQ(ptr_derived, kPtrB);
    EXPECT_NE(ptr_base, ptr_derived);
}

TEST_F(VTableRegistryTest, ResolveVirtualMethodNotFound)
{
    EXPECT_EQ(ResolveVirtualMethodPointer(kTokenBase, 0xDEAD), nullptr);
}

TEST_F(VTableRegistryTest, ResolveVirtualMethodZeroToken)
{
    EXPECT_EQ(ResolveVirtualMethodPointer(0, kMethodFoo), nullptr);
}

// ════════════════════════════════════════════════════════════════════════════
// BuildRuntimeVTable
// ════════════════════════════════════════════════════════════════════════════

TEST_F(VTableRegistryTest, BuildRuntimeVTable)
{
    static constexpr uint64_t kFlatBase = 0xF610000000000000ULL;
    static constexpr uint64_t kNewStable = 0xF600000000000000ULL;
    static const void* base_flat[] = { kPtrA, kPtrB, kPtrC };

    RegisterVTableArray(kFlatBase, base_flat, 3);

    const void** built = BuildRuntimeVTable(kNewStable, kFlatBase);
    ASSERT_NE(built, nullptr);
    EXPECT_EQ(built[0], kPtrA);
    EXPECT_EQ(built[1], kPtrB);
    EXPECT_EQ(built[2], kPtrC);

    EXPECT_EQ(FindVTable(kNewStable), built);
    EXPECT_EQ(FindVTableLength(kNewStable), 3u);
}

TEST_F(VTableRegistryTest, BuildRuntimeVTableIdempotent)
{
    static constexpr uint64_t kFlatBase = 0xF611000000000000ULL;
    static constexpr uint64_t kNewStable = 0xF601000000000000ULL;
    static const void* base_flat[] = { kPtrA, kPtrB };

    RegisterVTableArray(kFlatBase, base_flat, 2);

    const void** first  = BuildRuntimeVTable(kNewStable, kFlatBase);
    const void** second = BuildRuntimeVTable(kNewStable, kFlatBase);
    ASSERT_NE(first, nullptr);
    EXPECT_EQ(first, second);
}

TEST_F(VTableRegistryTest, BuildRuntimeVTableZeroIds)
{
    EXPECT_EQ(BuildRuntimeVTable(0, kStableBase), nullptr);
    EXPECT_EQ(BuildRuntimeVTable(kStableBase, 0), nullptr);
}

TEST_F(VTableRegistryTest, BuildRuntimeVTableNoBase)
{
    EXPECT_EQ(BuildRuntimeVTable(0xF6000000000000FFULL, 0xDEAD), nullptr);
}

// ════════════════════════════════════════════════════════════════════════════
// UpdateVTableSlotByMethodToken
// ════════════════════════════════════════════════════════════════════════════

TEST_F(VTableRegistryTest, UpdateVTableSlot)
{
    static constexpr uint32_t kToken = 0xF700u;
    static constexpr uint64_t kStable = 0xF700000000000000ULL;
    static VTableSlot slots[] = { SLOT_A, SLOT_C };
    static const void* flat[] = { kPtrA, kPtrB };
    static TypeVTable tv{};
    tv.type_token    = kToken;
    tv.stable_id     = kStable;
    tv.slot_count    = 2;
    tv.slots         = slots;
    tv.vtable_array  = flat;
    tv.vtable_length = 2;
    tv.type_shape    = 1;
    ASSERT_TRUE(RegisterTypeVTable(&tv));

    EXPECT_GE(UpdateVTableSlotByMethodToken(kMethodFoo, kPtrE), 1u);

    EXPECT_EQ(ResolveVirtualMethodPointer(kToken, kMethodFoo), kPtrE);
    EXPECT_EQ(ResolveVirtualMethodPointer(kToken, kMethodBar), kPtrC);
}

TEST_F(VTableRegistryTest, UpdateVTableSlotZeroArgs)
{
    EXPECT_EQ(UpdateVTableSlotByMethodToken(0, kPtrA), 0u);
    EXPECT_EQ(UpdateVTableSlotByMethodToken(kMethodFoo, nullptr), 0u);
}

TEST_F(VTableRegistryTest, UpdateVTableSlotInvalidatesCache)
{
    static constexpr uint32_t kToken = 0xF701u;
    static constexpr uint64_t kStable = 0xF701000000000000ULL;
    static VTableSlot slots[] = { SLOT_A };
    static const void* flat[] = { kPtrA };
    static TypeVTable tv{};
    tv.type_token    = kToken;
    tv.stable_id     = kStable;
    tv.slot_count    = 1;
    tv.slots         = slots;
    tv.vtable_array  = flat;
    tv.vtable_length = 1;
    tv.type_shape    = 1;
    ASSERT_TRUE(RegisterTypeVTable(&tv));

    // Populate TCVC
    EXPECT_EQ(ResolveVirtualMethodPointer(kToken, kMethodFoo), kPtrA);

    // Update — bumps global epoch, invalidating TCVC
    UpdateVTableSlotByMethodToken(kMethodFoo, kPtrE);

    // Should re-resolve and return the new pointer
    EXPECT_EQ(ResolveVirtualMethodPointer(kToken, kMethodFoo), kPtrE);
}

// ════════════════════════════════════════════════════════════════════════════
// FindMethodPointerByMethodToken
// ════════════════════════════════════════════════════════════════════════════

TEST_F(VTableRegistryTest, FindMethodPointerByToken)
{
    ASSERT_NE(FindMethodPointerByMethodToken(kMethodFoo), nullptr);
}

TEST_F(VTableRegistryTest, FindMethodPointerByTokenZero)
{
    EXPECT_EQ(FindMethodPointerByMethodToken(0), nullptr);
}

TEST_F(VTableRegistryTest, FindMethodPointerByTokenNotFound)
{
    EXPECT_EQ(FindMethodPointerByMethodToken(0xDEAD), nullptr);
}

// ════════════════════════════════════════════════════════════════════════════
// Registered count
// ════════════════════════════════════════════════════════════════════════════

TEST_F(VTableRegistryTest, GetVTableCount)
{
    uint32_t before = GetRegisteredVTableCount();

    static constexpr uint32_t kToken = 0xF800u;
    static VTableSlot slots[] = { SLOT_A };
    static TypeVTable tv{};
    tv.type_token = kToken;
    tv.slot_count = 1;
    tv.slots      = slots;
    ASSERT_TRUE(RegisterTypeVTable(&tv));

    EXPECT_EQ(GetRegisteredVTableCount(), before + 1);
}

// ════════════════════════════════════════════════════════════════════════════
// Interface epoch propagation
// ════════════════════════════════════════════════════════════════════════════

TEST_F(VTableRegistryTest, RuntimeInterfaceEpochPropagation)
{
    static constexpr uint32_t kToken = 0xF900u;
    static constexpr uint64_t kStable = 0xF900000000000000ULL;
    static constexpr uint64_t kIfaceId = 0xF900000000000001ULL;
    static VTableSlot slots[] = { SLOT_A };
    static TypeVTable tv{};
    tv.type_token  = kToken;
    tv.stable_id   = kStable;
    tv.slot_count  = 1;
    tv.slots       = slots;
    tv.type_shape  = 1;
    ASSERT_TRUE(RegisterTypeVTable(&tv));

    EXPECT_EQ(chaos_find_interface_offset(kToken, kIfaceId), UINT32_MAX);

    ASSERT_TRUE(RegisterTypeVTableRuntimeInterface(kToken, kIfaceId, 3, 2));

    EXPECT_EQ(chaos_find_interface_offset(kToken, kIfaceId), 3u);
}

// ════════════════════════════════════════════════════════════════════════════
// Unregister / Cleanup — uses RegisterHotUpdateVTable which heap-allocates
// everything (slots, vtable_array, TypeVTable) via CHAOS_IL2CPP_MALLOC,
// making it safe for UnregisterTypeVTable to free them.
// ════════════════════════════════════════════════════════════════════════════

TEST_F(VTableRegistryTest, UnregisterTypeVTable)
{
    static constexpr uint32_t kToken = 0xFFFEu;
    static constexpr uint64_t kStable = 0xFFFE000000000000ULL;
    static VTableSlot overrides[] = { SLOT_A };

    // RegisterHotUpdateVTable heap-allocates slots, vtable_array + TypeVTable.
    ASSERT_TRUE(RegisterHotUpdateVTable(kStable, kToken, 0, overrides, 1, 1));
    ASSERT_NE(TryGetTypeVTable(kToken), nullptr);

    UnregisterTypeVTable(kToken);
    EXPECT_EQ(TryGetTypeVTable(kToken), nullptr);
    EXPECT_EQ(TryGetTypeVTableByStableId(kStable), nullptr);
}

TEST_F(VTableRegistryTest, UnregisterTypeVTableByStableId)
{
    static constexpr uint32_t kToken = 0xFFFDu;
    static constexpr uint64_t kStable = 0xFFFD000000000000ULL;
    static VTableSlot overrides[] = { SLOT_A };

    ASSERT_TRUE(RegisterHotUpdateVTable(kStable, kToken, 0, overrides, 1, 1));
    ASSERT_NE(TryGetTypeVTable(kToken), nullptr);

    UnregisterTypeVTableByStableId(kStable);
    EXPECT_EQ(TryGetTypeVTable(kToken), nullptr);
    EXPECT_EQ(TryGetTypeVTableByStableId(kStable), nullptr);
}

TEST_F(VTableRegistryTest, UnregisterZeroToken)
{
    UnregisterTypeVTable(0);
    UnregisterTypeVTableByStableId(0);
}

TEST_F(VTableRegistryTest, UnregisterNonExistent)
{
    UnregisterTypeVTable(0xDEAD);
    UnregisterTypeVTableByStableId(0xDEAD);
}
