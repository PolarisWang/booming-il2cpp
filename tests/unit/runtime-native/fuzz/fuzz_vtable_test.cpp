/// fuzz_vtable_test.cpp — VTable dispatch fuzz tester.
///
/// Generates random type tokens and stable IDs, then exercises the VTable
/// lookup paths. This catches out-of-bounds, null-deref, and type confusion
/// during VTable resolution under random inputs.
///
/// Not a coverage-guided fuzzer — simple random oracle that passes through
/// the TryGetTypeVTable / FindVTable / ResolveVirtualMethodPointer paths.
///
/// Uses fixed seeds for deterministic, reproducible runs.  Each TEST first
/// registers one synthetic vtable (kKnownTypeToken) so the lookup helpers are
/// exercised against a non-empty registry and the non-null return path is
/// proven reachable — otherwise (void)-discarding results against an empty
/// registry would let a hard-coded nullptr implementation pass silently.

#include <cstdint>
#include <cstdlib>
#include <random>
#include <vector>

#include <gtest/gtest.h>
#include <chaos/native_types.h>
#include "vtable_registry.h"

using namespace chaos::il2cpp::vtable_registry;

// Fixed seeds for deterministic, reproducible fuzz runs.
inline constexpr unsigned kFuzzSeed = 42;
// A synthetic token/stable-id known to be registered by each TEST below.
inline constexpr CHAOS_IL2CPP_UINT32 kKnownTypeToken = 0xBEEF0001;
inline constexpr CHAOS_IL2CPP_UINT64 kKnownStableId = 0xBEEF000000000001ULL;
inline constexpr CHAOS_IL2CPP_UINT32 kKnownMethodToken = 0x00010001;

/// Register a single synthetic vtable so lookups hit a non-empty registry.
/// Uses static storage so the data outlives all TESTs in the suite.
/// RegisterTypeVTable is idempotent (same type_token → no-op on repeat), so
/// call this from each test freely — only the first call actually registers.
void RegisterKnownVTable() {
    static std::vector<VTableSlot> s_slots = {
        VTableSlot{kKnownMethodToken, reinterpret_cast<void*>(0x1000)}
    };
    static TypeVTable s_vt = []{
        TypeVTable v{};
        v.type_token = kKnownTypeToken;
        v.stable_id = kKnownStableId;
        v.slot_count = static_cast<CHAOS_IL2CPP_UINT32>(s_slots.size());
        v.slots = s_slots.data();
        v.vtable_array = nullptr;
        v.vtable_length = 0u;
        v.type_shape = 1u;  // reference type
        return v;
    }();
    RegisterTypeVTable(&s_vt);
}

/// Random type-token lookups to stress the TryGetTypeVTable path.
/// Registers one known type first, proving the non-null return path is
/// reachable; then asserts the vast majority of random tokens resolve to
/// nullptr (as the lookup contract requires for unknown types).
TEST(VTableFuzz, RandomTypeTokenLookups) {
    RegisterKnownVTable();

    // The known token must resolve — proves the helper walks a real registry
    // rather than a hard-coded nullptr return.
    const auto* known = TryGetTypeVTable(kKnownTypeToken);
    ASSERT_NE(known, nullptr) << "Known synthetic type must be resolvable";

    std::mt19937 rng(kFuzzSeed);
    std::uniform_int_distribution<CHAOS_IL2CPP_UINT32> token_dist(0, 1000000);
    uint32_t null_count = 0;
    for (int i = 0; i < 10000; i++) {
        CHAOS_IL2CPP_UINT32 token = token_dist(rng);
        const auto* vtable = TryGetTypeVTable(token);
        (void)vtable;
        if (vtable == nullptr) ++null_count;
    }
    // Random tokens over a 1M range almost never collide with real types, so
    // null is the overwhelming outcome.  This exercises the no-crash/UB path.
    EXPECT_GT(null_count, 9900u) << "~all random tokens must resolve to nullptr";
}

/// Random stable-id lookups to stress the FindVTable path.
TEST(VTableFuzz, RandomStableIdLookups) {
    RegisterKnownVTable();

    const void** known = FindVTable(kKnownStableId);
    // FindVTable only resolves flat registered arrays; our synthetic type has
    // none, so even the known stable id returns nullptr here.  We assert the
    // helper is compiled in and callable, and that random IDs never crash.
    (void)known;

    std::mt19937 rng(kFuzzSeed + 1);
    std::uniform_int_distribution<CHAOS_IL2CPP_UINT64> id_dist(0, UINT64_MAX);
    for (int i = 0; i < 5000; i++) {
        CHAOS_IL2CPP_UINT64 stable_id = id_dist(rng);
        const void** vtable = FindVTable(stable_id);
        (void)vtable;
    }
    // No crash/UB across 5k random 64-bit lookups — the regression guard this
    // test exists for.  (Values are not asserted because the registry state is
    // only exercised, not required to hold specific entries for FindVTable.)
}

/// Random virtual method pointer resolution.
TEST(VTableFuzz, RandomVirtualMethodResolve) {
    RegisterKnownVTable();

    // The known type's method must resolve to a non-null pointer — proves the
    // inheritance-walk path returns real function pointers, not hard nullptr.
    void* known_method = ResolveVirtualMethodPointer(kKnownTypeToken, kKnownMethodToken);
    EXPECT_NE(known_method, nullptr)
        << "Known synthetic type+method must resolve to a real function pointer";

    std::mt19937 rng(kFuzzSeed + 2);
    std::uniform_int_distribution<CHAOS_IL2CPP_UINT32> token_dist(0, 100000);
    for (int i = 0; i < 5000; i++) {
        CHAOS_IL2CPP_UINT32 type_token = token_dist(rng);
        CHAOS_IL2CPP_UINT32 method_token = token_dist(rng);
        void* method = ResolveVirtualMethodPointer(type_token, method_token);
        (void)method;
    }
    // No crash/UB across 5k random resolutions.
}
