// custom_marshaler_test.cpp — ICustomMarshaler full function tests.
//
// Tests all 5 public ICustomMarshaler API functions:
//   CustomMarshalerNativeToManaged, CustomMarshalerManagedToNative,
//   CustomMarshalerCleanupNativeData, CustomMarshalerCleanupManagedData,
//   ClearMarshalerCache.
//
// Since full ICustomMarshaler resolution requires a managed runtime with
// method_invoke, these tests focus on:
//   1. Null/empty cookie edge cases (early return paths)
//   2. ClearMarshalerCache correctness (no crash, idempotent)
//   3. Stability under concurrent ClearMarshalerCache calls
//   4. Empty cache initial state verification

#include "marshal_test_fixture.h"

#include <engine_binding.h>

#include <atomic>
#include <cstdint>
#include <thread>
#include <vector>

using namespace chaos::il2cpp::runtime_core;

// ════════════════════════════════════════════════════════════════════════════
// E1 — ICustomMarshaler edge case tests
// ════════════════════════════════════════════════════════════════════════════

class CustomMarshalerTest : public MarshalTestFixture {
};

// ── Null/empty cookie edge cases ──────────────────────────────────────────

TEST_F(CustomMarshalerTest, NativeToManagedNullCookie) {
    // Null cookie should safely return 0
    EXPECT_EQ(CustomMarshalerNativeToManaged(nullptr, 0), 0);
    EXPECT_EQ(CustomMarshalerNativeToManaged(nullptr, 0x1234), 0);
}

TEST_F(CustomMarshalerTest, NativeToManagedEmptyCookie) {
    // Empty cookie should safely return 0
    EXPECT_EQ(CustomMarshalerNativeToManaged("", 0), 0);
    EXPECT_EQ(CustomMarshalerNativeToManaged("", 0x5678), 0);
}

TEST_F(CustomMarshalerTest, ManagedToNativeNullCookie) {
    // Null cookie returns managed_obj unchanged (passthrough)
    EXPECT_EQ(CustomMarshalerManagedToNative(nullptr, 0), 0);
    EXPECT_EQ(CustomMarshalerManagedToNative(nullptr, 0xABCD), 0xABCD);
}

TEST_F(CustomMarshalerTest, ManagedToNativeEmptyCookie) {
    // Empty cookie returns managed_obj unchanged (passthrough)
    EXPECT_EQ(CustomMarshalerManagedToNative("", 0), 0);
    EXPECT_EQ(CustomMarshalerManagedToNative("", 0xDEAD), 0xDEAD);
}

TEST_F(CustomMarshalerTest, CleanupNativeDataNullCookie) {
    // Null/empty cookie should be a no-op (no crash)
    CustomMarshalerCleanupNativeData(nullptr, 0);
    CustomMarshalerCleanupNativeData(nullptr, 0x1234);
    SUCCEED();
}

TEST_F(CustomMarshalerTest, CleanupNativeDataEmptyCookie) {
    CustomMarshalerCleanupNativeData("", 0);
    CustomMarshalerCleanupNativeData("", 0x5678);
    SUCCEED();
}

TEST_F(CustomMarshalerTest, CleanupManagedDataNullCookie) {
    CustomMarshalerCleanupManagedData(nullptr, 0);
    CustomMarshalerCleanupManagedData(nullptr, 0x1234);
    SUCCEED();
}

TEST_F(CustomMarshalerTest, CleanupManagedDataEmptyCookie) {
    CustomMarshalerCleanupManagedData("", 0);
    CustomMarshalerCleanupManagedData("", 0x5678);
    SUCCEED();
}

// ── ClearMarshalerCache ───────────────────────────────────────────────────

TEST_F(CustomMarshalerTest, ClearCacheNoCrash) {
    // ClearMarshalerCache should not crash when called on empty cache
    ClearMarshalerCache();
    SUCCEED();
}

TEST_F(CustomMarshalerTest, ClearCacheIdempotent) {
    // Multiple calls to ClearMarshalerCache should be safe
    ClearMarshalerCache();
    ClearMarshalerCache();
    ClearMarshalerCache();
    SUCCEED();
}

TEST_F(CustomMarshalerTest, ClearCacheAfterEdgeCalls) {
    // Clearing cache after calling marshaler functions with invalid cookies
    // should be safe (the functions early-return, cache stays empty)
    CustomMarshalerNativeToManaged("nonexistent_type", 0);
    CustomMarshalerManagedToNative("nonexistent_type", 0);
    CustomMarshalerCleanupNativeData("nonexistent_type", 0);
    CustomMarshalerCleanupManagedData("nonexistent_type", 0);

    ClearMarshalerCache();
    SUCCEED();
}

// ── Unresolvable cookie (no managed runtime) ──────────────────────────────

TEST_F(CustomMarshalerTest, NativeToManagedResolveFailsGracefully) {
    // Without a managed runtime, ResolveOrCreateMarshaler will fail to
    // find the ICustomMarshaler type. The function should return 0
    // gracefully without crashing.
    const auto result = CustomMarshalerNativeToManaged(
        "MyCustomMarshaler", 0x42);
    EXPECT_EQ(result, 0);
}

TEST_F(CustomMarshalerTest, ManagedToNativeResolveFailsGracefully) {
    // Without a managed runtime, should return managed_obj passthrough.
    const auto result = CustomMarshalerManagedToNative(
        "MyCustomMarshaler", 0x42);
    EXPECT_EQ(result, 0x42);
}

TEST_F(CustomMarshalerTest, CleanupNativeDataResolveFailsGracefully) {
    // Should be a no-op without crash.
    CustomMarshalerCleanupNativeData("MyCustomMarshaler", 0x42);
    SUCCEED();
}

TEST_F(CustomMarshalerTest, CleanupManagedDataResolveFailsGracefully) {
    CustomMarshalerCleanupManagedData("MyCustomMarshaler", 0x42);
    SUCCEED();
}

// ── Cookie with separator suffix ──────────────────────────────────────────

TEST_F(CustomMarshalerTest, CookieWithSuffixResolveFailsGracefully) {
    // Cookies can have format "TypeName:CookieSuffix". The resolver
    // extracts the TypeName part. Without managed runtime, still fails
    // gracefully.
    CustomMarshalerNativeToManaged("MyType:MyCookie", 0);
    CustomMarshalerManagedToNative("MyType:MyCookie", 0);
    CustomMarshalerCleanupNativeData("MyType:MyCookie", 0);
    CustomMarshalerCleanupManagedData("MyType:MyCookie", 0);

    ClearMarshalerCache();
    SUCCEED();
}

// ── Concurrent ClearMarshalerCache ────────────────────────────────────────

TEST_F(CustomMarshalerTest, ConcurrentClearCache) {
    // Concurrent ClearMarshalerCache calls should not deadlock or crash.
    // The spinlock protects the cache and clear is O(kMarshalerCacheSize).
    constexpr int kNumThreads = 8;
    constexpr int kIterations = 100;
    std::vector<std::thread> threads;
    std::atomic<int> errors{0};

    for (int i = 0; i < kNumThreads; ++i) {
        threads.emplace_back([&errors]() {
            for (int j = 0; j < kIterations; ++j) {
                try {
                    ClearMarshalerCache();
                } catch (...) {
                    errors.fetch_add(1, std::memory_order_relaxed);
                }
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(errors.load(), 0);
}

// ── Mixed concurrent access (clear + edge calls) ──────────────────────────

TEST_F(CustomMarshalerTest, ConcurrentMixedAccess) {
    // Concurrent calls to ClearMarshalerCache and various CustomMarshaler
    // functions with invalid cookies should not deadlock or crash.
    constexpr int kNumThreads = 6;
    constexpr int kIterations = 50;
    std::vector<std::thread> threads;
    std::atomic<int> errors{0};

    for (int t = 0; t < kNumThreads; ++t) {
        threads.emplace_back([t, &errors]() {
            for (int j = 0; j < kIterations; ++j) {
                try {
                    switch ((t + j) % 6) {
                        case 0:
                            ClearMarshalerCache();
                            break;
                        case 1:
                            CustomMarshalerNativeToManaged(nullptr, 0);
                            break;
                        case 2:
                            CustomMarshalerManagedToNative("", 0x42);
                            break;
                        case 3:
                            CustomMarshalerCleanupNativeData(nullptr, 0);
                            break;
                        case 4:
                            CustomMarshalerCleanupManagedData("", 0);
                            break;
                        case 5:
                            CustomMarshalerNativeToManaged(
                                "SomeType", 0);
                            CustomMarshalerManagedToNative(
                                "SomeType", 0);
                            break;
                    }
                } catch (...) {
                    errors.fetch_add(1, std::memory_order_relaxed);
                }
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(errors.load(), 0);
}
