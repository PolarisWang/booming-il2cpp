// hotupdate_vtable_sync_test.cpp — Cross-module VTable sync integration test
//
// Tests that method_replacement::Register/Revert correctly updates VTables
// across multiple registered TypeVTables. Registers 2+ synthetic VTables
// with overlapping method tokens, then verifies all are updated atomically.

#include <method_replacement.h>
#include <vtable_registry.h>

#include <gtest/gtest.h>

#include <chaos/native_types.h>

#include <cstdint>
#include <cstring>

namespace mr = chaos::il2cpp::method_replacement;
namespace vr = chaos::il2cpp::vtable_registry;

// ── Constants ───────────────────────────────────────────────────────────────

static constexpr uint32_t kTokenShared  = 0x06000042u;  // method in both vtables
static constexpr uint32_t kTokenAOnly   = 0x06000010u;
static constexpr uint32_t kTokenBOnly   = 0x06000020u;

static constexpr uint32_t kTypeAToken   = 0x02000001u;
static constexpr uint32_t kTypeBToken   = 0x02000002u;
static constexpr uint64_t kTypeAStable  = 0xAAAA000000000001ULL;
static constexpr uint64_t kTypeBStable  = 0xBBBB000000000002ULL;

static constexpr uint32_t kSlotCountA   = 2u;
static constexpr uint32_t kSlotCountB   = 2u;

static void* const kOriginalFn    = reinterpret_cast<void*>(static_cast<uintptr_t>(0x11111111));
static void* const kOriginalAFn   = reinterpret_cast<void*>(static_cast<uintptr_t>(0x22222222));
static void* const kOriginalBFn   = reinterpret_cast<void*>(static_cast<uintptr_t>(0x33333333));
static void* const kReplacementFn = reinterpret_cast<void*>(static_cast<uintptr_t>(0xDEADBEEF));

// AOT symbol stubs (required by linkage)
extern "C" const char* kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int kChaosExternalRuntimeCount = 0;
extern "C" void* chaos_il2cpp_aot_hotpatch_module = nullptr;

// ── Fixture ─────────────────────────────────────────────────────────────────

class VTableSyncTest : public ::testing::Test {
protected:
    std::vector<uint32_t> registered_tokens_;

    void SetUp() override {
        mr::RevertAll();
        registered_tokens_.clear();
    }

    void TearDown() override {
        mr::RevertAll();
        for (auto tok : registered_tokens_) {
            vr::UnregisterTypeVTable(tok);
        }
        registered_tokens_.clear();
    }

    // Build a heap-allocated TypeVTable with given slots. Returns a pointer
    // that the registry owns — freed by UnregisterTypeVTable in TearDown.
    // All allocations use CHAOS_IL2CPP_MALLOC to match CHAOS_IL2CPP_FREE in
    // UnregisterTypeVTable.
    static vr::TypeVTable* MakeVTable(
        uint32_t type_token,
        uint64_t stable_id,
        const vr::VTableSlot* slots,
        uint32_t slot_count)
    {
        // Build heap-allocated flat array from slots.
        void* flat_buf = CHAOS_IL2CPP_MALLOC(slot_count * sizeof(const void*));
        auto** flat = static_cast<const void**>(flat_buf);
        for (uint32_t i = 0; i < slot_count; i++) {
            flat[i] = slots[i].method_pointer;
        }

        // Heap-allocate slots array (registry will free it).
        void* slots_buf = CHAOS_IL2CPP_MALLOC(slot_count * sizeof(vr::VTableSlot));
        auto* slots_heap = static_cast<vr::VTableSlot*>(slots_buf);
        std::memcpy(slots_heap, slots, sizeof(vr::VTableSlot) * slot_count);

        void* vt_buf = CHAOS_IL2CPP_MALLOC(sizeof(vr::TypeVTable));
        auto* vt = static_cast<vr::TypeVTable*>(vt_buf);
        *vt = vr::TypeVTable{};
        vt->stable_id     = stable_id;
        vt->type_token    = type_token;
        vt->base_stable_id = 0;
        vt->base_token    = 0;
        vt->slot_count    = slot_count;
        vt->slots         = slots_heap;
        vt->vtable_array  = const_cast<const void**>(flat);
        vt->vtable_length = slot_count;
        vt->type_shape    = 1;  // reference type
        return vt;
    }

    void RegisterVTable(vr::TypeVTable* vt) {
        ASSERT_TRUE(vr::RegisterTypeVTable(vt));
        registered_tokens_.push_back(vt->type_token);
    }
};

// ── Tests ───────────────────────────────────────────────────────────────────

TEST_F(VTableSyncTest, UpdateSingleVTable)
{
    vr::VTableSlot slots[] = {
        {kTokenShared, kOriginalFn},
    };

    auto* vt = MakeVTable(kTypeAToken, kTypeAStable, slots, 1);
    RegisterVTable(vt);

    // Register replacement → should update the vtable slot.
    ASSERT_TRUE(mr::Register(kTokenShared, kReplacementFn));

    // Verify via TryGetTypeVTable.
    const auto* result = vr::TryGetTypeVTable(kTypeAToken);
    ASSERT_NE(result, nullptr);
    ASSERT_GE(result->slot_count, 1u);
    EXPECT_EQ(result->slots[0].method_pointer, kReplacementFn);

    // Verify flat array also updated.
    ASSERT_NE(result->vtable_array, nullptr);
    EXPECT_EQ(result->vtable_array[0], kReplacementFn);

    // Revert → verify restored.
    ASSERT_TRUE(mr::Revert(kTokenShared));
    result = vr::TryGetTypeVTable(kTypeAToken);
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->slots[0].method_pointer, kOriginalFn);
    EXPECT_EQ(result->vtable_array[0], kOriginalFn);
}

TEST_F(VTableSyncTest, UpdateAcrossTwoVTables)
{
    vr::VTableSlot slots_a[] = {
        {kTokenShared, kOriginalFn},
        {kTokenAOnly,  kOriginalAFn},
    };
    vr::VTableSlot slots_b[] = {
        {kTokenShared, kOriginalFn},
        {kTokenBOnly,  kOriginalBFn},
    };

    auto* vt_a = MakeVTable(kTypeAToken, kTypeAStable, slots_a, kSlotCountA);
    auto* vt_b = MakeVTable(kTypeBToken, kTypeBStable, slots_b, kSlotCountB);
    RegisterVTable(vt_a);
    RegisterVTable(vt_b);

    // Register replacement for the shared token.
    ASSERT_TRUE(mr::Register(kTokenShared, kReplacementFn));

    // Both VTables should have the replacement.
    {
        const auto* r = vr::TryGetTypeVTable(kTypeAToken);
        ASSERT_NE(r, nullptr);
        EXPECT_EQ(r->slots[0].method_pointer, kReplacementFn);
        EXPECT_EQ(r->vtable_array[0], kReplacementFn);

        // Non-shared slot should be unchanged.
        EXPECT_EQ(r->slots[1].method_pointer, kOriginalAFn);
        EXPECT_EQ(r->vtable_array[1], kOriginalAFn);
    }
    {
        const auto* r = vr::TryGetTypeVTable(kTypeBToken);
        ASSERT_NE(r, nullptr);
        EXPECT_EQ(r->slots[0].method_pointer, kReplacementFn);
        EXPECT_EQ(r->vtable_array[0], kReplacementFn);

        EXPECT_EQ(r->slots[1].method_pointer, kOriginalBFn);
        EXPECT_EQ(r->vtable_array[1], kOriginalBFn);
    }

    // Revert → both VTables restored.
    ASSERT_TRUE(mr::Revert(kTokenShared));

    {
        const auto* r = vr::TryGetTypeVTable(kTypeAToken);
        ASSERT_NE(r, nullptr);
        EXPECT_EQ(r->slots[0].method_pointer, kOriginalFn);
        EXPECT_EQ(r->vtable_array[0], kOriginalFn);
    }
    {
        const auto* r = vr::TryGetTypeVTable(kTypeBToken);
        ASSERT_NE(r, nullptr);
        EXPECT_EQ(r->slots[0].method_pointer, kOriginalFn);
        EXPECT_EQ(r->vtable_array[0], kOriginalFn);
    }
}

TEST_F(VTableSyncTest, UpdateWithThreeVTables)
{
    // Three vtables, all sharing kTokenShared at slot 0.
    static constexpr uint32_t kTypeCToken = 0x02000003u;
    static constexpr uint64_t kTypeCStable = 0xCCCC000000000003ULL;

    vr::VTableSlot slots_a[] = {{kTokenShared, kOriginalFn}};
    vr::VTableSlot slots_b[] = {{kTokenShared, kOriginalFn}};
    vr::VTableSlot slots_c[] = {{kTokenShared, kOriginalFn}};

    auto* vt_a = MakeVTable(kTypeAToken, kTypeAStable, slots_a, 1);
    auto* vt_b = MakeVTable(kTypeBToken, kTypeBStable, slots_b, 1);
    auto* vt_c = MakeVTable(kTypeCToken, kTypeCStable, slots_c, 1);
    RegisterVTable(vt_a);
    RegisterVTable(vt_b);
    RegisterVTable(vt_c);

    // Register → all 3 updated.
    ASSERT_TRUE(mr::Register(kTokenShared, kReplacementFn));

    for (uint32_t tok : {kTypeAToken, kTypeBToken, kTypeCToken}) {
        const auto* r = vr::TryGetTypeVTable(tok);
        ASSERT_NE(r, nullptr) << "vtable for token " << tok;
        EXPECT_EQ(r->slots[0].method_pointer, kReplacementFn)
            << "slot 0 for token " << tok;
    }

    // Revert.
    ASSERT_TRUE(mr::Revert(kTokenShared));

    for (uint32_t tok : {kTypeAToken, kTypeBToken, kTypeCToken}) {
        const auto* r = vr::TryGetTypeVTable(tok);
        ASSERT_NE(r, nullptr);
        EXPECT_EQ(r->slots[0].method_pointer, kOriginalFn);
    }
}

TEST_F(VTableSyncTest, RevertThenRegisterNewUpdatesBoth)
{
    vr::VTableSlot slots[] = {
        {kTokenShared, kOriginalFn},
    };

    auto* vt_a = MakeVTable(kTypeAToken, kTypeAStable, slots, 1);
    auto* vt_b = MakeVTable(kTypeBToken, kTypeBStable, slots, 1);
    RegisterVTable(vt_a);
    RegisterVTable(vt_b);

    // Register.
    ASSERT_TRUE(mr::Register(kTokenShared, kReplacementFn));
    EXPECT_EQ(vr::TryGetTypeVTable(kTypeAToken)->slots[0].method_pointer, kReplacementFn);
    EXPECT_EQ(vr::TryGetTypeVTable(kTypeBToken)->slots[0].method_pointer, kReplacementFn);

    // Revert.
    ASSERT_TRUE(mr::Revert(kTokenShared));
    EXPECT_EQ(vr::TryGetTypeVTable(kTypeAToken)->slots[0].method_pointer, kOriginalFn);

    // Register a second replacement (different pointer).
    void* const kSecondReplacement = reinterpret_cast<void*>(static_cast<uintptr_t>(0xCAFEBABE));
    ASSERT_TRUE(mr::Register(kTokenShared, kSecondReplacement));
    EXPECT_EQ(vr::TryGetTypeVTable(kTypeAToken)->slots[0].method_pointer, kSecondReplacement);
    EXPECT_EQ(vr::TryGetTypeVTable(kTypeBToken)->slots[0].method_pointer, kSecondReplacement);

    ASSERT_TRUE(mr::Revert(kTokenShared));
    EXPECT_EQ(vr::TryGetTypeVTable(kTypeAToken)->slots[0].method_pointer, kOriginalFn);
}

TEST_F(VTableSyncTest, FindMethodPointerByMethodTokenReturnsOriginalBeforeRegister)
{
    vr::VTableSlot slots[] = {
        {kTokenShared, kOriginalFn},
    };

    auto* vt = MakeVTable(kTypeAToken, kTypeAStable, slots, 1);
    RegisterVTable(vt);

    // Before Register: FindMethodPointerByMethodToken should return original.
    void* found = vr::FindMethodPointerByMethodToken(kTokenShared);
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found, kOriginalFn);

    // After Register: FindMethodPointerByMethodToken should now return replacement.
    ASSERT_TRUE(mr::Register(kTokenShared, kReplacementFn));
    found = vr::FindMethodPointerByMethodToken(kTokenShared);
    EXPECT_EQ(found, kReplacementFn);

    ASSERT_TRUE(mr::Revert(kTokenShared));
}
