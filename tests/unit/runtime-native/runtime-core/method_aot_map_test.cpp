/// Unit tests for the AOT method map (generic method promotion).
///
/// Tests the QueryAotMethod function directly by:
///   - Registering per-module GenericMethodAotEntryV0[] arrays
///   - Querying with matching (open_token, type_args) → expects closed_token
///   - Querying with non-matching args → expects 0
///   - Verifying empty table and miss behavior

#include <gtest/gtest.h>
#include <chaos/native_types.h>

#include <codegen_bridge.h>    // GenericMethodAotEntryV0
#include <module_registry.h>   // MakeTypeHandle, GetTypeToken

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// ── Forward declarations from runtime_instantiation.h ───────────────────────
// We forward-declare the AOT method map API here instead of including the
// source header directly, because the source header (src/native/runtime-core/)
// and the contracts header (contracts/native/v0/) both define
// runtime_instantiation.h with different content, and the include-path
// ordering required for the source header's internal includes conflicts with
// the test's direct includes.

namespace chaos::il2cpp::runtime_instantiation {

void RegisterMethodAotEntries(
    CHAOS_IL2CPP_UINT32                module_id,
    const GenericMethodAotEntryV0*     entries,
    CHAOS_IL2CPP_UINT32                count,
    const uint32_t*                    args,
    CHAOS_IL2CPP_UINT32                arg_count);

MethodInfoHandle QueryAotMethod(
    CHAOS_IL2CPP_UINT32                module_id,
    CHAOS_IL2CPP_UINT32                open_method_token,
    const TypeInfoHandle*              type_args,
    CHAOS_IL2CPP_UINT32                arg_count);

}  // namespace chaos::il2cpp::runtime_instantiation

// ════════════════════════════════════════════════════════════════════════════
// Fixture
// ════════════════════════════════════════════════════════════════════════════

// We use module 0 (AOT root) for all tests.
static constexpr uint32_t kModuleId = 0u;

// Token values for type arguments (simulate metadata tokens).
static constexpr uint32_t kTokenInt32    = 0x01000002u;  // System.Int32
static constexpr uint32_t kTokenString   = 0x0100000Eu;  // System.String
static constexpr uint32_t kTokenBool     = 0x01000004u;  // System.Boolean

// Open method tokens
static constexpr uint32_t kOpenMethodM   = 0x06000010u;  // M<>
static constexpr uint32_t kOpenMethodN   = 0x06000020u;  // N<>

// Closed method tokens (AOT-compiled instances)
static constexpr uint32_t kClosedMInt32  = 0x06000050u;  // M<int>
static constexpr uint32_t kClosedMString = 0x06000051u;  // M<string>
static constexpr uint32_t kClosedNBool   = 0x06000060u;  // N<bool>

// Flat type-arg token pool.
static const uint32_t s_args_pool[] = {
    kTokenInt32,          // M<int> type_arg[0]
    kTokenString,         // M<string> type_arg[0]
    kTokenBool,           // N<bool> type_arg[0]
};

// Entries MUST be sorted by open_token.
static const GenericMethodAotEntryV0 s_entries[] = {
    { kOpenMethodM, kClosedMInt32,  1, 0 },
    { kOpenMethodM, kClosedMString, 1, 1 },
    { kOpenMethodN, kClosedNBool,   1, 2 },
};

static constexpr uint32_t kEntryCount = sizeof(s_entries) / sizeof(s_entries[0]);
static constexpr uint32_t kArgsCount  = sizeof(s_args_pool) / sizeof(s_args_pool[0]);

class MethodAotMapTest : public ::testing::Test {
protected:
    static void SetUpTestSuite()
    {
        chaos::il2cpp::runtime_instantiation::RegisterMethodAotEntries(
            kModuleId, s_entries, kEntryCount, s_args_pool, kArgsCount);
    }
};

// ════════════════════════════════════════════════════════════════════════════
// Tests
// ════════════════════════════════════════════════════════════════════════════

// Basic hit: M<int> should find closed_token 0x06000050
TEST_F(MethodAotMapTest, HitMInt32)
{
    const TypeInfoHandle type_args[1] = {
        chaos::il2cpp::runtime_core::MakeTypeHandle(kModuleId, kTokenInt32)
    };

    const MethodInfoHandle result = chaos::il2cpp::runtime_instantiation::QueryAotMethod(
        kModuleId, kOpenMethodM, type_args, 1);

    EXPECT_EQ(result, static_cast<MethodInfoHandle>(kClosedMInt32));
}

// Different type args for same open method: M<string> -> 0x06000051
TEST_F(MethodAotMapTest, HitMString)
{
    const TypeInfoHandle type_args[1] = {
        chaos::il2cpp::runtime_core::MakeTypeHandle(kModuleId, kTokenString)
    };

    const MethodInfoHandle result = chaos::il2cpp::runtime_instantiation::QueryAotMethod(
        kModuleId, kOpenMethodM, type_args, 1);

    EXPECT_EQ(result, static_cast<MethodInfoHandle>(kClosedMString));
}

// Second open method: N<bool> -> 0x06000060
TEST_F(MethodAotMapTest, HitNBool)
{
    const TypeInfoHandle type_args[1] = {
        chaos::il2cpp::runtime_core::MakeTypeHandle(kModuleId, kTokenBool)
    };

    const MethodInfoHandle result = chaos::il2cpp::runtime_instantiation::QueryAotMethod(
        kModuleId, kOpenMethodN, type_args, 1);

    EXPECT_EQ(result, static_cast<MethodInfoHandle>(kClosedNBool));
}

// Miss: M<double> (no AOT entry) should return 0
TEST_F(MethodAotMapTest, MissNoEntryForTypeArg)
{
    const TypeInfoHandle type_args[1] = {
        chaos::il2cpp::runtime_core::MakeTypeHandle(kModuleId, 0x0100000Cu)  // System.Double
    };

    const MethodInfoHandle result = chaos::il2cpp::runtime_instantiation::QueryAotMethod(
        kModuleId, kOpenMethodM, type_args, 1);

    EXPECT_EQ(result, 0u);
}

// Miss: unknown open method should return 0
TEST_F(MethodAotMapTest, MissUnknownOpenMethod)
{
    const TypeInfoHandle type_args[1] = {
        chaos::il2cpp::runtime_core::MakeTypeHandle(kModuleId, kTokenInt32)
    };

    const MethodInfoHandle result = chaos::il2cpp::runtime_instantiation::QueryAotMethod(
        kModuleId, 0x06000FFFu, type_args, 1);  // no such method

    EXPECT_EQ(result, 0u);
}

// Empty table: module with no entries should return 0
TEST_F(MethodAotMapTest, EmptyTableReturnsZero)
{
    const TypeInfoHandle type_args[1] = {
        chaos::il2cpp::runtime_core::MakeTypeHandle(1u, kTokenInt32)
    };

    const MethodInfoHandle result = chaos::il2cpp::runtime_instantiation::QueryAotMethod(
        1u, kOpenMethodM, type_args, 1);

    EXPECT_EQ(result, 0u);
}

// Invalid module_id should return 0
TEST_F(MethodAotMapTest, InvalidModuleReturnsZero)
{
    const TypeInfoHandle type_args[1] = { 0 };

    const MethodInfoHandle result = chaos::il2cpp::runtime_instantiation::QueryAotMethod(
        9999u, kOpenMethodM, type_args, 1);  // beyond kMaxModules

    EXPECT_EQ(result, 0u);
}

// Wrong type_arg count should return 0
TEST_F(MethodAotMapTest, WrongArgCountReturnsZero)
{
    const TypeInfoHandle type_args[2] = {
        chaos::il2cpp::runtime_core::MakeTypeHandle(kModuleId, kTokenInt32),
        chaos::il2cpp::runtime_core::MakeTypeHandle(kModuleId, kTokenInt32)
    };

    // M<> entries have arg_count=1, but we pass 2.
    const MethodInfoHandle result = chaos::il2cpp::runtime_instantiation::QueryAotMethod(
        kModuleId, kOpenMethodM, type_args, 2);

    EXPECT_EQ(result, 0u);
}
