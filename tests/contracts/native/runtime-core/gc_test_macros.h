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
// B-family: simple CHECK(cond, msg)
// ════════════════════════════════════════════════════════════════════════════

#define GC_CHECK(cond, ...) do {                                              \
    if (!(cond)) {                                                          \
        printf("  FAIL [%s:%d]: ", __FILE__, __LINE__);                     \
        printf(__VA_ARGS__);                                                \
        printf("\n");                                                       \
        ++g_failures;                                                       \
    } else {                                                                \
        printf("  PASS: ");                                                 \
        printf(__VA_ARGS__);                                                \
        printf("\n");                                                       \
    }                                                                       \
} while(0)

// ════════════════════════════════════════════════════════════════════════════
// Deprecated aliases — for backward compatibility with existing test code.
// New code should use GC_TEST / GC_SUBTEST / GC_PASS / GC_FAIL / GC_CHECK.
// ════════════════════════════════════════════════════════════════════════════

// A-family backward compat
#define TEST(name)  GC_TEST(name)
#define SUBTEST(name) GC_SUBTEST(name)
#define PASS()      GC_PASS()
#define FAIL(msg)   GC_FAIL(msg)

// B-family backward compat
#define CHECK(cond, ...) GC_CHECK(cond, __VA_ARGS__)

#endif  // CHAOS_GC_TEST_MACROS_H_
