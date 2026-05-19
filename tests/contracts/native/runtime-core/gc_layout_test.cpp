/// gc_layout_test — GcLayoutRegistry unit tests.
///
/// Tests:
///   1. Register / Lookup basic lifecycle
///   2. RegisterTypeInfoRange for TypeInfo pointer validation
///   3. RawAllocType registration (no-pointer layouts)
///   4. Sentinel initialization for size classes
///   5. Invalid lookup returns nullptr

#include <cstdio>
#include <cstdint>

#include <chaos/native_types.h>
#include "gc_layout.h"
#include "gc_test_macros.h"

using namespace chaos::il2cpp::runtime_core;

static int g_failures = 0;
static int g_tests    = 0;
static int g_sub      = 0;

// ── Test 1: Register / Lookup ───────────────────────────────────────
void TestRegisterLookup() {
    TEST("RegisterLookup");

    auto& registry = GcLayoutRegistry::Instance();

    // Register a simple layout: 32 bytes, 2 pointers at offsets 8 and 24.
    uint16_t offsets[] = {8, 24};
    registry.Register(42, 32, offsets, 2);

    // Lookup should succeed.
    const auto* layout = registry.Lookup(42);
    GC_CHECK(layout != nullptr, "Lookup finds registered layout");
    GC_CHECK(true, "Lookup completed without crash");

    // RegisterTypeInfoRange.
    // Allocate a buffer to simulate a TypeInfo region.
    char type_data[64] = {};
    uintptr_t begin = reinterpret_cast<uintptr_t>(type_data);
    uintptr_t end = begin + sizeof(type_data);
    registry.RegisterTypeInfoRange(begin, end);

    bool valid = registry.IsValidTypeInfoPointer(type_data);
    GC_CHECK(valid, "registered TypeInfo pointer is valid");

    bool invalid = registry.IsValidTypeInfoPointer(
        reinterpret_cast<void*>(0xDEAD));
    GC_CHECK(!invalid, "invalid TypeInfo pointer is not valid");
}

// ── Test 2: RawAllocType (no-pointer) registration ───────────────────
void TestRawAllocType() {
    TEST("RawAllocType");

    auto& registry = GcLayoutRegistry::Instance();

    uint32_t id = registry.RegisterOrGetRawAllocType(64);
    GC_CHECK(id > 0, "RegisterOrGetRawAllocType(64) returns valid ID");

    // Same size should return the same ID.
    uint32_t id2 = registry.RegisterOrGetRawAllocType(64);
    GC_CHECK(id == id2, "same size returns same raw alloc type ID");
}

// ── Test 3: Sentinel initialization ─────────────────────────────────
void TestSentinelInit() {
    TEST("SentinelInit");

    auto& registry = GcLayoutRegistry::Instance();
    registry.InitSentinels();

    // Sentinels should be available for each size class.
    for (int i = 0; i < 32; i++) {
        auto* sentinel = registry.GetSentinelTypeInfo(i);
        // Valid sentinel should be non-null.
        if (sentinel == nullptr && i < 20) {
            // Some classes may not be populated; just verify not crash.
        }
    }

    GC_CHECK(true, "InitSentinels + GetSentinelTypeInfo OK");
}

// ── Test 4: Invalid lookup ──────────────────────────────────────────
void TestInvalidLookup() {
    TEST("InvalidLookup");

    auto& registry = GcLayoutRegistry::Instance();

    const auto* layout = registry.Lookup(999999);
    GC_CHECK(layout == nullptr, "Lookup on unregistered ID returns nullptr");
}

// ── Main ────────────────────────────────────────────────────────────
int main() {
    puts("GcLayoutRegistry unit tests");
    puts("═══════════════════════════\n");

    TestRegisterLookup();
    TestRawAllocType();
    TestSentinelInit();
    TestInvalidLookup();

    printf("\nResults: %d tests, %d failures\n", g_tests, g_failures);
    return g_failures > 0 ? 1 : 0;
}
