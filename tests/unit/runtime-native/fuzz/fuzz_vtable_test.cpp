/// fuzz_vtable_test.cpp — VTable dispatch fuzz tester.
///
/// Generates random type tokens and stable IDs, then exercises the VTable
/// lookup paths. This catches out-of-bounds, null-deref, and type confusion
/// during VTable resolution under random inputs.
///
/// Not a coverage-guided fuzzer — simple random oracle that passes through
/// the TryGetTypeVTable / FindVTable / ResolveVirtualMethodPointer paths.

#include <cstdint>
#include <cstdlib>
#include <random>
#include <ctime>

#include <gtest/gtest.h>
#include <chaos/native_types.h>
#include "vtable_registry.h"

using namespace chaos::il2cpp::vtable_registry;

/// Random type-token lookups to stress the TryGetTypeVTable path.
/// Most random tokens are invalid; the function must return nullptr
/// (not crash, assert, or UB).
TEST(VTableFuzz, RandomTypeTokenLookups) {
    std::mt19937 rng(static_cast<unsigned>(std::time(nullptr)));
    std::uniform_int_distribution<CHAOS_IL2CPP_UINT32> token_dist(0, 1000000);

    for (int i = 0; i < 10000; i++) {
        CHAOS_IL2CPP_UINT32 token = token_dist(rng);
        const auto* vtable = TryGetTypeVTable(token);
        // Must not crash — nullptr is valid for unresolvable tokens
        (void)vtable;
    }

    SUCCEED();
}

/// Random stable-id lookups to stress the FindVTable path.
/// Most random IDs are invalid; the function must return nullptr.
TEST(VTableFuzz, RandomStableIdLookups) {
    std::mt19937 rng(static_cast<unsigned>(std::time(nullptr)));
    std::uniform_int_distribution<CHAOS_IL2CPP_UINT64> id_dist(0, UINT64_MAX);

    for (int i = 0; i < 5000; i++) {
        CHAOS_IL2CPP_UINT64 stable_id = id_dist(rng);
        const void** vtable = FindVTable(stable_id);
        // Must not crash — nullptr is valid for unresolvable IDs
        (void)vtable;
    }

    SUCCEED();
}

/// Random virtual method pointer resolution.
/// Combines random type tokens with random method tokens.
TEST(VTableFuzz, RandomVirtualMethodResolve) {
    std::mt19937 rng(static_cast<unsigned>(std::time(nullptr)));
    std::uniform_int_distribution<CHAOS_IL2CPP_UINT32> token_dist(0, 100000);

    for (int i = 0; i < 5000; i++) {
        CHAOS_IL2CPP_UINT32 type_token = token_dist(rng);
        CHAOS_IL2CPP_UINT32 method_token = token_dist(rng);

        // ResolveVirtualMethodPointer: instance_type_token, declared_method_token
        void* method = ResolveVirtualMethodPointer(type_token, method_token);
        // Must not crash — nullptr is valid for unresolvable methods
        (void)method;
    }

    SUCCEED();
}