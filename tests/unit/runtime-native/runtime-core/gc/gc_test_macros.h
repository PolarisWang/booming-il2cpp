/// gc_test_macros.h — Shared test macros and helpers for GC tests.
///
/// Two macro families:
///
/// A-family (structured test/suite):
///   TEST("name")    — Declare a test, increment counter
///   SUBTEST("name") — Declare a sub-test inside a test
///   PASS()          — Mark current test as passed
///   FAIL("msg")     — Mark current test as failed
///
/// B-family (simple check):
///   CHECK(cond, "msg") — Pass/fail single assertion
///
/// Required global variables (defined in each .cpp):
///   static int g_failures;   — incremented on FAIL / failed CHECK
///   static int g_tests;      — incremented by TEST (A-family only)
///   static int g_sub;        — incremented by SUBTEST (A-family only)

#ifndef CHAOS_GC_TEST_MACROS_H_
#define CHAOS_GC_TEST_MACROS_H_

#include <cstdio>

#include <cstddef>
#include <tuple>
#include <vector>

#include <gtest/gtest.h>

#include "gc_test_base.h"

// ════════════════════════════════════════════════════════════════════════════
// A-family: structured TEST / SUBTEST / PASS / FAIL
// ════════════════════════════════════════════════════════════════════════════

#define GC_TEST(name)                                                      \
    do {                                                                   \
        ++g_tests;                                                         \
        g_sub = 0;                                                         \
        printf("  TEST: %s ... ", name);                                   \
    } while (0)

#define GC_SUBTEST(name)                                                   \
    do {                                                                   \
        ++g_sub;                                                           \
        printf("\n    SUB %d: %s ... ", g_sub, name);                      \
    } while (0)

#define GC_PASS()          puts("PASS")
#define GC_FAIL(msg)       do { ++g_failures; printf("FAIL: %s\n", msg); } while (0)

// ════════════════════════════════════════════════════════════════════════════
// B-family: simple CHECK(cond, fmt...)
// ════════════════════════════════════════════════════════════════════════════

#define GC_CHECK(cond, ...) do {                                            \
    if (!(cond)) {                                                           \
        printf("  FAIL [%s:%d]: ", __FILE__, __LINE__);                     \
        printf(__VA_ARGS__);                                                 \
        printf("\n");                                                        \
        ++g_failures;                                                        \
    } else {                                                                 \
        printf("  PASS: ");                                                  \
        printf(__VA_ARGS__);                                                 \
        printf("\n");                                                        \
    }                                                                        \
} while(0)

// Backward-compatible aliases for tests migrated from the old contract system.
// Note: gtest defines TEST/FAIL macros too. Since these standalone tests have
// custom main() and don't use gtest's TEST() (which takes 2 args), we undefine
// the gtest versions to avoid macro conflicts.
#ifdef TEST
#undef TEST
#endif
#define TEST GC_TEST

#ifdef FAIL
#undef FAIL
#endif
#define FAIL GC_FAIL

#define CHECK GC_CHECK
#define SUBTEST GC_SUBTEST
#define PASS GC_PASS

// ════════════════════════════════════════════════════════════════════════════
// C-family: gtest-based parameterized test helpers
// ════════════════════════════════════════════════════════════════════════════
//
// These helpers build on gtest's TEST_P / INSTANTIATE_TEST_SUITE_P.
//
// Usage — single parameter (e.g. size class):
//   class AllocationSizeTest : public GcParamTestBase<size_t> {};
//   TEST_P(AllocationSizeTest, Allocate) {
//       size_t size = std::get<0>(GetParam());
//       void* obj = NurseryAllocate(static_cast<uint32_t>(size));
//       EXPECT_NE(obj, nullptr);
//   }
//   INSTANTIATE_TEST_SUITE_P(SizeRange, AllocationSizeTest,
//       ::testing::ValuesIn(GcRange<size_t>(16, 1024, 16)));
//
// Usage — multi-parameter (cartesian product):
//   class AllocWithThreadsTest : public GcParamTestBase<size_t, int> {};
//   TEST_P(AllocWithThreadsTest, ConcurrentAlloc) {
//       auto [size, thread_count] = GetParam();
//       // ...
//   }
//   INSTANTIATE_TEST_SUITE_P(Combinations, AllocWithThreadsTest,
//       ::testing::Combine(
//           ::testing::ValuesIn(GcRange<size_t>(64, 1024, 64)),
//           ::testing::Range(1, 5)));
//
// ════════════════════════════════════════════════════════════════════════════

namespace chaos { namespace il2cpp { namespace runtime_core {

// ── Parameterized Fixture Bases ────────────────────────────────────────

/// GcTestBase with parameter support.
template <typename... Params>
struct GcParamTestBase : GcTestBase,
                         ::testing::WithParamInterface<std::tuple<Params...>> {};

/// GcUnitTestBase with parameter support.
template <typename... Params>
struct GcUnitParamTestBase : GcUnitTestBase,
                             ::testing::WithParamInterface<std::tuple<Params...>> {};

/// GcStressTestBase with parameter support.
template <typename... Params>
struct GcStressParamTestBase : GcStressTestBase,
                               ::testing::WithParamInterface<std::tuple<Params...>> {};

// ── Range Generator ────────────────────────────────────────────────────

/// Generate a vector of values from [start, end] with the given step.
template <typename T>
inline std::vector<T> GcRange(T start, T end, T step = static_cast<T>(1)) {
    std::vector<T> result;
    for (T v = start; v <= end; v += step) {
        result.push_back(v);
    }
    return result;
}

}}}  // namespace chaos::il2cpp::runtime_core

// ── Convenience Instantiation Macro ─────────────────────────────────────
//
/// GC_INSTANTIATE_SIZE_SUITE(TestName, ...) — shorthand for the common
/// pattern of instantiating a size-parameterized test suite.
///
/// Usage:
///   GC_INSTANTIATE_SIZE_SUITE(MySizeTest, 32, 64, 128, 256);
///   // expands to:
///   // INSTANTIATE_TEST_SUITE_P(Size, MySizeTest,
///   //     ::testing::ValuesIn(std::vector<size_t>{32, 64, 128, 256}));

#define GC_INSTANTIATE_SIZE_SUITE(TestName, ...)                               \
    INSTANTIATE_TEST_SUITE_P(                                                  \
        Size, TestName,                                                        \
        ::testing::ValuesIn(                                                   \
            std::vector<size_t>{__VA_ARGS__}))

// ════════════════════════════════════════════════════════════════════════════
// D-family: flaky test retry support
// ════════════════════════════════════════════════════════════════════════════
//
// GcFlakyRun() wraps a flaky-sensitive section with automatic retry.
//
// Usage inside a TEST_F body:
//
//   bool ok = GcFlakyRun(kGcFlakyMaxRetries, "my-section", [&]() -> bool {
//       int val = DoSomethingFlaky();
//       return val == expected;   // return true = pass, false = fail
//   });
//   EXPECT_TRUE(ok);
//
// The lambda should return true on success, false on failure. Do NOT use
// EXPECT_* / ASSERT_* inside — gtest does not clear failure state between
// retries, so use plain bool returns instead.
//
// On success after retries, prints a [FLAKY] warning so the flaky rate can
// be monitored. On total failure, returns false.
//
// ════════════════════════════════════════════════════════════════════════════

namespace chaos { namespace il2cpp { namespace runtime_core {

/// Maximum number of retry attempts for flaky stress tests.
static constexpr int kGcFlakyMaxRetries = 2;

/// Retry a flaky-sensitive test section.
///
/// @param max_retries  Max additional attempts after first failure (0 = no retry).
/// @param name         Section name for logging (used in [FLAKY] messages).
/// @param fn           Lambda that returns true (pass) or false (fail).
/// @return true if any attempt passed.
template <typename Fn>
bool GcFlakyRun(int max_retries, const char* name, Fn&& fn) {
    for (int attempt = 0; attempt <= max_retries; attempt++) {
        if (attempt > 0) {
            printf("[FLAKY] Retry #%d/%d for \"%s\"\n",
                   attempt, max_retries, name ? name : "section");
        }
        if (fn()) {  // lambda returns true = success
            if (attempt > 0) {
                printf("[FLAKY] Retry #%d/%d passed for \"%s\"\n",
                       attempt, max_retries, name ? name : "section");
                fflush(stdout);
            }
            return true;
        }
    }
    printf("[FLAKY] All %d attempts failed for \"%s\"\n",
           max_retries + 1, name ? name : "section");
    fflush(stdout);
    return false;
}

/// Overload without name.
template <typename Fn>
bool GcFlakyRun(int max_retries, Fn&& fn) {
    return GcFlakyRun(max_retries, "flaky-section", std::forward<Fn>(fn));
}

}}}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_GC_TEST_MACROS_H_
