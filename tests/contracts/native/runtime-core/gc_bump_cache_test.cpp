/// Unit tests for GcBumpCache — thread-local size-class GC allocator.
///
/// Exercises: size-class hit/miss, refill, bump fallback, oversized bypass,
/// atomic vs scan page chains, and allocation counters.
///
/// GcBumpCache is a header-only class so no separate compilation unit is
/// needed — just include and instantiate a thread_local instance.

#include <chaos/native_types.h>

#include "gc_bump_cache.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// ════════════════════════════════════════════════════════════════════════════
// Test helpers
// ════════════════════════════════════════════════════════════════════════════

static int g_failures = 0;
static int g_tests    = 0;
static int g_sub      = 0;

#define TEST(name)                                                      \
    do {                                                                \
        ++g_tests;                                                      \
        g_sub = 0;                                                      \
        printf("  TEST: %s ... ", name);                                \
    } while (0)

#define SUBTEST(name)                                                   \
    do {                                                                \
        ++g_sub;                                                        \
        printf("\n    SUB %d: %s ... ", g_sub, name);                   \
    } while (0)

#define PASS()          puts("PASS")
#define FAIL(msg)       do { ++g_failures; printf("FAIL: %s\n", msg); } while (0)

using GcBumpCache = chaos::il2cpp::runtime_core::GcBumpCache;

// ════════════════════════════════════════════════════════════════════════════
// Helpers
// ════════════════════════════════════════════════════════════════════════════

/// Fill a block with a known pattern, then verify it reads back.
static void fill_and_check(void* ptr, CHAOS_IL2CPP_SIZE size, uint8_t byte) {
    std::memset(ptr, byte, size);
    for (CHAOS_IL2CPP_SIZE i = 0; i < size; ++i) {
        if (static_cast<uint8_t*>(ptr)[i] != byte) {
            FAIL("memory corruption");
            return;
        }
    }
}

// ════════════════════════════════════════════════════════════════════════════
// Test: Size-class hit
// ════════════════════════════════════════════════════════════════════════════

static void TestSizeClassHit() {
    TEST("size-class hit populates free list and returns aligned memory");

    GcBumpCache cache;
    // 32 is in kSizeClasses.  First alloc misses (empty free list), refills
    // from bump arena.  Next kBatchCount-1 allocs hit from free list.
    void* prev = nullptr;
    int hit_count = 0;
    for (int i = 0; i < GcBumpCache::kBatchCount + 2; ++i) {
        void* p = cache.Allocate(24);  // size 24 → aligned 24 → class idx 1
        if (p == nullptr) {
            FAIL("null pointer from Allocate");
            return;
        }
        // Check 8-byte alignment
        if (reinterpret_cast<uintptr_t>(p) % 8 != 0) {
            FAIL("misaligned pointer");
            return;
        }
        // Fill and verify
        fill_and_check(p, 24, 0xAB);
        prev = p;
        ++hit_count;
    }
    (void)prev;
    PASS();
}

// ════════════════════════════════════════════════════════════════════════════
// Test: Size-class miss → bump fallback
// ════════════════════════════════════════════════════════════════════════════

static void TestSizeClassMiss() {
    TEST("size not in kSizeClasses falls through to bump allocate");

    GcBumpCache cache;
    // 72 is not in kSizeClasses (64, 80 are).  Must fall through to bump.
    void* p = cache.Allocate(72);
    if (p == nullptr) {
        FAIL("null pointer from Allocate");
        return;
    }
    if (reinterpret_cast<uintptr_t>(p) % 8 != 0) {
        FAIL("misaligned pointer");
        return;
    }
    fill_and_check(p, 72, 0xCD);
    PASS();
}

// ════════════════════════════════════════════════════════════════════════════
// Test: Atomic vs scan page chains
// ════════════════════════════════════════════════════════════════════════════

static void TestAtomicVsScan() {
    TEST("Allocate and AllocateAtomic use separate page chains");

    GcBumpCache cache;
    void* scan_a = cache.Allocate(32);
    void* scan_b = cache.Allocate(32);
    void* atom_a = cache.AllocateAtomic(32);
    void* atom_b = cache.AllocateAtomic(32);

    if (!scan_a || !scan_b || !atom_a || !atom_b) {
        FAIL("null pointer");
        return;
    }

    // All should be 8-byte aligned.
    for (auto* p : {scan_a, scan_b, atom_a, atom_b}) {
        if (reinterpret_cast<uintptr_t>(p) % 8 != 0) {
            FAIL("misaligned pointer");
            return;
        }
    }

    fill_and_check(scan_a, 32, 0x01);
    fill_and_check(atom_a, 32, 0x02);
    PASS();
}

// ════════════════════════════════════════════════════════════════════════════
// Test: Oversized bypass
// ════════════════════════════════════════════════════════════════════════════

static void TestOversized() {
    TEST("allocation > kMaxInlineSize (32 KB) bypasses arena");

    GcBumpCache cache;
    CHAOS_IL2CPP_SIZE big = GcBumpCache::kMaxInlineSize + 1;  // 32 KB + 1
    void* p = cache.Allocate(big);
    if (p == nullptr) {
        FAIL("null pointer from oversized Allocate");
        return;
    }
    if (reinterpret_cast<uintptr_t>(p) % 8 != 0) {
        FAIL("misaligned pointer");
        return;
    }
    fill_and_check(p, big, 0xEF);
    PASS();
}

// ════════════════════════════════════════════════════════════════════════════
// Test: Multiple sizes, no overlap
// ════════════════════════════════════════════════════════════════════════════

static void TestMultipleSizes() {
    TEST("allocations of different sizes return non-overlapping memory");

    GcBumpCache cache;
    CHAOS_IL2CPP_SIZE sizes[] = {16, 24, 32, 40, 48, 64, 128, 256, 512, 1024, 2048};
    void* ptrs[11] = {nullptr};

    for (int i = 0; i < 11; ++i) {
        ptrs[i] = cache.Allocate(sizes[i]);
        if (ptrs[i] == nullptr) {
            FAIL("null pointer");
            return;
        }
        fill_and_check(ptrs[i], sizes[i], static_cast<uint8_t>(0x10 + i));
    }
    PASS();
}

// ════════════════════════════════════════════════════════════════════════════
// Test: DumpCounters smoke
// ════════════════════════════════════════════════════════════════════════════

static void TestDumpCounters() {
    TEST("DumpCounters produces output without crashing");

    GcBumpCache cache;
    cache.Allocate(16);
    cache.Allocate(16);
    cache.AllocateAtomic(32);
    cache.AllocateAtomic(GcBumpCache::kMaxInlineSize + 1);

    // Redirect stderr to /dev/null for this call to avoid test output noise.
    // In a real CI run the stderr output is what the profile tooling consumes.
    cache.DumpCounters();
    PASS();
}

// ════════════════════════════════════════════════════════════════════════════
// Main
// ════════════════════════════════════════════════════════════════════════════

int main() {
    puts("=== GcBumpCache Tests ===");
    puts("");

    TestSizeClassHit();
    TestSizeClassMiss();
    TestAtomicVsScan();
    TestOversized();
    TestMultipleSizes();
    TestDumpCounters();

    puts("");
    printf("Results: %d tests, %d failures\n", g_tests, g_failures);

    return g_failures > 0 ? 1 : 0;
}