/// Unit tests for the AOT method map (generic method promotion).
///
/// Tests the QueryAotMethod function directly by:
///   - Registering per-module GenericMethodAotEntryV0[] arrays
///   - Querying with matching (open_token, type_args) → expects closed_token
///   - Querying with non-matching args → expects 0
///   - Verifying empty table and miss behavior

#include <chaos/native_types.h>       // CHAOS_IL2CPP_UINT32 etc — MUST be first

#include "codegen_bridge.h"           // GenericMethodAotEntryV0
#include "module_registry.h"          // MakeTypeHandle, GetTypeToken

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
// Test helpers
// ════════════════════════════════════════════════════════════════════════════

static int g_failures = 0;
static int g_tests    = 0;

#define TEST(name)                                                      \
    do {                                                                \
        ++g_tests;                                                      \
        printf("  TEST: %s ... ", name);                                \
    } while (0)

#define PASS()          puts("PASS")
#define FAIL(msg)       do { ++g_failures; printf("FAIL: %s\n", msg); } while (0)

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

// ════════════════════════════════════════════════════════════════════════════
// AOT entry test data
// ════════════════════════════════════════════════════════════════════════════

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

// ════════════════════════════════════════════════════════════════════════════
// Test: Basic hit
// ════════════════════════════════════════════════════════════════════════════

static void TestHit() {
    TEST("M<int> should find closed_token 0x06000050");

    // Build type arguments: TypeInfoHandle encoding (module_id << 32 | token).
    // We encode the type args as module_id=0 (AOT root) | token.
    const TypeInfoHandle type_args[1] = {
        chaos::il2cpp::runtime_core::MakeTypeHandle(kModuleId, kTokenInt32)
    };

    const MethodInfoHandle result = chaos::il2cpp::runtime_instantiation::QueryAotMethod(
        kModuleId, kOpenMethodM, type_args, 1);

    if (result == static_cast<MethodInfoHandle>(kClosedMInt32)) {
        PASS();
    } else {
        char buf[128];
        ::snprintf(buf, sizeof(buf),
            "expected 0x%08X, got 0x%08llX",
            kClosedMInt32, (unsigned long long)result);
        FAIL(buf);
    }
}

// ════════════════════════════════════════════════════════════════════════════
// Test: Different type args for same open method
// ════════════════════════════════════════════════════════════════════════════

static void TestHitString() {
    TEST("M<string> should find closed_token 0x06000051");

    const TypeInfoHandle type_args[1] = {
        chaos::il2cpp::runtime_core::MakeTypeHandle(kModuleId, kTokenString)
    };

    const MethodInfoHandle result = chaos::il2cpp::runtime_instantiation::QueryAotMethod(
        kModuleId, kOpenMethodM, type_args, 1);

    if (result == static_cast<MethodInfoHandle>(kClosedMString)) {
        PASS();
    } else {
        char buf[128];
        ::snprintf(buf, sizeof(buf),
            "expected 0x%08X, got 0x%08llX",
            kClosedMString, (unsigned long long)result);
        FAIL(buf);
    }
}

// ════════════════════════════════════════════════════════════════════════════
// Test: Second open method
// ════════════════════════════════════════════════════════════════════════════

static void TestHitN() {
    TEST("N<bool> should find closed_token 0x06000060");

    const TypeInfoHandle type_args[1] = {
        chaos::il2cpp::runtime_core::MakeTypeHandle(kModuleId, kTokenBool)
    };

    const MethodInfoHandle result = chaos::il2cpp::runtime_instantiation::QueryAotMethod(
        kModuleId, kOpenMethodN, type_args, 1);

    if (result == static_cast<MethodInfoHandle>(kClosedNBool)) {
        PASS();
    } else {
        char buf[128];
        ::snprintf(buf, sizeof(buf),
            "expected 0x%08X, got 0x%08llX",
            kClosedNBool, (unsigned long long)result);
        FAIL(buf);
    }
}

// ════════════════════════════════════════════════════════════════════════════
// Test: Miss (no AOT entry for this type arg)
// ════════════════════════════════════════════════════════════════════════════

static void TestMiss() {
    TEST("M<double> (no AOT entry) should return 0");

    // There's no AOT entry for M<double>.
    const TypeInfoHandle type_args[1] = {
        chaos::il2cpp::runtime_core::MakeTypeHandle(kModuleId, 0x0100000Cu)  // System.Double
    };

    const MethodInfoHandle result = chaos::il2cpp::runtime_instantiation::QueryAotMethod(
        kModuleId, kOpenMethodM, type_args, 1);

    if (result == 0u) {
        PASS();
    } else {
        char buf[128];
        ::snprintf(buf, sizeof(buf),
            "expected 0 (miss), got 0x%08llX",
            (unsigned long long)result);
        FAIL(buf);
    }
}

// ════════════════════════════════════════════════════════════════════════════
// Test: Miss (unknown open method)
// ════════════════════════════════════════════════════════════════════════════

static void TestMissUnknownMethod() {
    TEST("Unknown open method should return 0");

    const TypeInfoHandle type_args[1] = {
        chaos::il2cpp::runtime_core::MakeTypeHandle(kModuleId, kTokenInt32)
    };

    const MethodInfoHandle result = chaos::il2cpp::runtime_instantiation::QueryAotMethod(
        kModuleId, 0x06000FFFu, type_args, 1);  // no such method

    if (result == 0u) {
        PASS();
    } else {
        char buf[128];
        ::snprintf(buf, sizeof(buf),
            "expected 0 (miss), got 0x%08llX",
            (unsigned long long)result);
        FAIL(buf);
    }
}

// ════════════════════════════════════════════════════════════════════════════
// Test: Empty table
// ════════════════════════════════════════════════════════════════════════════

static void TestEmptyTable() {
    TEST("Empty module (no entries) should return 0");

    // Use module_id=1 which should have no entries registered.
    const TypeInfoHandle type_args[1] = {
        chaos::il2cpp::runtime_core::MakeTypeHandle(1u, kTokenInt32)
    };

    const MethodInfoHandle result = chaos::il2cpp::runtime_instantiation::QueryAotMethod(
        1u, kOpenMethodM, type_args, 1);

    if (result == 0u) {
        PASS();
    } else {
        FAIL("expected 0 for empty table");
    }
}

// ════════════════════════════════════════════════════════════════════════════
// Test: Out-of-range module_id
// ════════════════════════════════════════════════════════════════════════════

static void TestInvalidModule() {
    TEST("Invalid module_id should return 0");

    const TypeInfoHandle type_args[1] = { 0 };

    const MethodInfoHandle result = chaos::il2cpp::runtime_instantiation::QueryAotMethod(
        9999u, kOpenMethodM, type_args, 1);  // beyond kMaxModules

    if (result == 0u) {
        PASS();
    } else {
        FAIL("expected 0 for invalid module");
    }
}

// ════════════════════════════════════════════════════════════════════════════
// Test: Wrong arg count
// ════════════════════════════════════════════════════════════════════════════

static void TestWrongArgCount() {
    TEST("Wrong type_arg count should return 0");

    const TypeInfoHandle type_args[2] = {
        chaos::il2cpp::runtime_core::MakeTypeHandle(kModuleId, kTokenInt32),
        chaos::il2cpp::runtime_core::MakeTypeHandle(kModuleId, kTokenInt32)
    };

    // M<> entries have arg_count=1, but we pass 2.
    const MethodInfoHandle result = chaos::il2cpp::runtime_instantiation::QueryAotMethod(
        kModuleId, kOpenMethodM, type_args, 2);

    if (result == 0u) {
        PASS();
    } else {
        FAIL("expected 0 for wrong arg count");
    }
}

// ════════════════════════════════════════════════════════════════════════════
// Main
// ════════════════════════════════════════════════════════════════════════════

int main() {
    // ── Register entries for module 0 ──
    chaos::il2cpp::runtime_instantiation::RegisterMethodAotEntries(
        kModuleId, s_entries, kEntryCount, s_args_pool, kArgsCount);

    puts("=== Method AOT Map Tests ===");
    puts("");

    TestHit();
    TestHitString();
    TestHitN();
    TestMiss();
    TestMissUnknownMethod();
    TestEmptyTable();
    TestInvalidModule();
    TestWrongArgCount();

    puts("");
    printf("Results: %d tests, %d failures\n", g_tests, g_failures);

    return g_failures > 0 ? 1 : 0;
}
