/// gc_atomic_alloc_test — NurseryAllocateAtomic unit tests.
///
/// Tests:
///   1. Basic atomic allocation (small objects, TLAB path)
///   2. Oversized atomic allocation (> kMaxTlabAlloc, old-gen fallback)
///   3. Multi-threaded concurrent atomic allocation
///   4. Mixed atomic + non-atomic allocation
///   5. Atomic allocation after young GC
///
/// NurseryAllocateAtomic is the pointer-free variant that skips bitmap
/// scanning.  It should produce valid zeroed memory for both TLAB and
/// oversized paths.

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <thread>
#include <atomic>
#include <vector>

#include <chaos/native_types.h>
#include "gc_region.h"
#include "gc_young_gen.h"
#include "gc_young_collector.h"
#include "gc_scheduler.h"
#include "gc_stats.h"
#include "gc_old_gen.h"
#include "thread_state.h"

using namespace chaos::il2cpp::runtime_core;

#include "gc_test_macros.h"

static int g_failures = 0;

// ── Helpers ──────────────────────────────────────────────────────────

/// Verify that a memory region is zeroed.
static bool IsZeroed(const void* ptr, size_t size) {
    const auto* bytes = static_cast<const unsigned char*>(ptr);
    for (size_t i = 0; i < size; i++) {
        if (bytes[i] != 0) return false;
    }
    return true;
}

/// Fill memory with a pattern and verify it.
static void FillAndVerify(void* ptr, size_t size, unsigned char pattern) {
    std::memset(ptr, pattern, size);
    const auto* bytes = static_cast<const unsigned char*>(ptr);
    for (size_t i = 0; i < size; i++) {
        if (bytes[i] != pattern) {
            printf("  MISMATCH at offset %zu: expected 0x%02x, got 0x%02x\n",
                   i, pattern, bytes[i]);
            g_failures++;
            return;
        }
    }
}

// ── Test 1: Basic atomic allocation (TLAB path) ─────────────────────

void TestBasicAtomicAlloc() {
    printf("\n── Test 1: Basic atomic allocation (TLAB path) ──\n");

    // Reset TLAB.
    tls_tlab = TLAB{};

    // Allocate small pointer-free objects via NurseryAllocateAtomic.
    constexpr int kCount = 100;
    void* ptrs[kCount];
    for (int i = 0; i < kCount; i++) {
        ptrs[i] = NurseryAllocateAtomic(64);
        CHECK(ptrs[i] != nullptr, "atomic alloc %d returned non-null", i);
        if (ptrs[i]) {
            // Must be zeroed.
            CHECK(IsZeroed(ptrs[i], 64), "atomic alloc %d is zeroed", i);
            // Write a pattern and verify.
            FillAndVerify(ptrs[i], 64, static_cast<unsigned char>(i + 1));
        }
    }

    // Verify all still readable.
    for (int i = 0; i < kCount; i++) {
        if (ptrs[i]) {
            const auto* bytes = static_cast<const unsigned char*>(ptrs[i]);
            CHECK(bytes[0] == static_cast<unsigned char>(i + 1),
                  "atomic alloc %d data integrity", i);
            CHECK(bytes[63] == static_cast<unsigned char>(i + 1),
                  "atomic alloc %d last byte integrity", i);
        }
    }

    CHECK(true, "basic atomic allocation completed");
}

// ── Test 2: Oversized atomic allocation (>kMaxTlabAlloc, old-gen) ────

void TestOversizedAtomicAlloc() {
    printf("\n── Test 2: Oversized atomic allocation (>32KB, old-gen) ──\n");

    // Reset TLAB.
    tls_tlab = TLAB{};

    // Allocate objects larger than kMaxTlabAlloc (32KB).
    // These should route to NurseryAllocateAtomicSlow → old-gen.
    constexpr size_t kOversizedSize = kMaxTlabAlloc + 128;  // 32896 bytes

    void* p1 = NurseryAllocateAtomic(kOversizedSize);
    CHECK(p1 != nullptr, "oversized atomic alloc returned non-null");
    if (p1) {
        CHECK(IsZeroed(p1, kOversizedSize), "oversized atomic alloc is zeroed");
        FillAndVerify(p1, kOversizedSize, 0xAB);
    }

    void* p2 = NurseryAllocateAtomic(kMaxTlabAlloc * 2);  // 64KB
    CHECK(p2 != nullptr, "64KB atomic alloc returned non-null");
    if (p2) {
        CHECK(IsZeroed(p2, kMaxTlabAlloc * 2), "64KB atomic alloc is zeroed");
        FillAndVerify(p2, kMaxTlabAlloc * 2, 0xCD);
    }

    // Mix with small atomic allocs after oversized.
    void* p3 = NurseryAllocateAtomic(32);
    CHECK(p3 != nullptr, "small atomic alloc after oversized returned non-null");
    if (p3) {
        CHECK(IsZeroed(p3, 32), "small alloc after oversized is zeroed");
    }

    CHECK(true, "oversized atomic allocation completed");
}

// ── Test 3: Multi-threaded concurrent atomic allocation ──────────────

struct ThreadResult {
    int id;
    int allocs;
    int failures;
};

static void AtomicAllocWorker(ThreadResult* result) {
    threading::RegisterThread(threading::AllocateThreadId(), nullptr);

    constexpr int kAllocsPerThread = 500;
    int local_failures = 0;

    for (int i = 0; i < kAllocsPerThread; i++) {
        // Alternate between small and medium atomic allocs.
        size_t size = (i % 5 == 0) ? 256 : 64;
        void* p = NurseryAllocateAtomic(size);
        if (p == nullptr) {
            local_failures++;
        } else if (!IsZeroed(p, size)) {
            local_failures++;
        } else {
            // Write and re-verify.
            std::memset(p, static_cast<unsigned char>(result->id + i), size);
        }
    }

    result->allocs = kAllocsPerThread;
    result->failures = local_failures;

    threading::UnregisterThread();
}

void TestConcurrentAtomicAlloc() {
    printf("\n── Test 3: Concurrent atomic allocation (%d threads) ──\n", 8);

    constexpr int kThreads = 8;
    std::vector<ThreadResult> results(kThreads);
    std::vector<std::thread> threads;

    for (int i = 0; i < kThreads; i++) {
        results[i].id = i;
        threads.emplace_back(AtomicAllocWorker, &results[i]);
    }

    for (auto& t : threads) {
        t.join();
    }

    int total_allocs = 0;
    int total_failures = 0;
    for (auto& r : results) {
        total_allocs += r.allocs;
        total_failures += r.failures;
        printf("  Thread %d: %d allocs, %d failures\n", r.id, r.allocs, r.failures);
    }

    CHECK(total_failures == 0, "concurrent atomic alloc: 0 failures (%d allocs)", total_allocs);
}

// ── Test 4: Mixed atomic + non-atomic allocation ─────────────────────

void TestMixedAtomicNonAtomic() {
    printf("\n── Test 4: Mixed atomic + non-atomic allocation ──\n");

    tls_tlab = TLAB{};

    // Interleave NurseryAllocate and NurseryAllocateAtomic.
    for (int i = 0; i < 50; i++) {
        void* p_atomic = NurseryAllocateAtomic(64);
        CHECK(p_atomic != nullptr, "mixed atomic alloc %d", i);
        if (p_atomic) std::memset(p_atomic, 0xAA, 64);

        void* p_normal = NurseryAllocate(64);
        CHECK(p_normal != nullptr, "mixed normal alloc %d", i);
        if (p_normal) std::memset(p_normal, 0xBB, 64);

        // Oversized mixed.
        void* p_big_atomic = NurseryAllocateAtomic(kMaxTlabAlloc + 64);
        CHECK(p_big_atomic != nullptr, "mixed oversized atomic %d", i);
        if (p_big_atomic) std::memset(p_big_atomic, 0xCC, 64);

        void* p_big_normal = NurseryAllocate(kMaxTlabAlloc + 64);
        CHECK(p_big_normal != nullptr, "mixed oversized normal %d", i);
        if (p_big_normal) std::memset(p_big_normal, 0xDD, 64);
    }

    CHECK(true, "mixed atomic + non-atomic allocation completed");
}

// ── Test 5: Atomic allocation after young GC ─────────────────────────

void TestAtomicAfterYoungGc() {
    printf("\n── Test 5: Atomic allocation after young GC ──\n");

    tls_tlab = TLAB{};

    // Fill nursery with atomic allocs to trigger young GC.
    constexpr int kFillCount = 10000;
    for (int i = 0; i < kFillCount; i++) {
        void* p = NurseryAllocateAtomic(128);
        if (p == nullptr) break;  // nursery full
    }

    // Run young GC.
    uint32_t gen = threading::RequestGlobalSafepoint();
    GcYoungCollection();
    threading::ReleaseGlobalSafepoint(gen);

    // Allocate after GC — should succeed with fresh nursery.
    void* p = NurseryAllocateAtomic(64);
    CHECK(p != nullptr, "atomic alloc after young GC succeeded");
    if (p) {
        CHECK(IsZeroed(p, 64), "atomic alloc after young GC is zeroed");
    }

    // Allocate oversized after GC.
    void* p_big = NurseryAllocateAtomic(kMaxTlabAlloc + 256);
    CHECK(p_big != nullptr, "oversized atomic alloc after young GC succeeded");
    if (p_big) {
        CHECK(IsZeroed(p_big, 256), "oversized atomic alloc after young GC is zeroed");
    }

    CHECK(true, "atomic allocation after young GC completed");
}

// ── Main ─────────────────────────────────────────────────────────────

int main() {
    puts("CRAG NurseryAllocateAtomic test");
    puts("═══════════════════════════════════\n");

    // Warm up CRAG globals.
    InitYoungGeneration();
    g_old_gen.Init(0, 2);
    void* warmup = NurseryAllocate(64);
    (void)warmup;

    // Register this thread.
    threading::RegisterThread(threading::AllocateThreadId(), nullptr);

    TestBasicAtomicAlloc();
    TestOversizedAtomicAlloc();
    TestConcurrentAtomicAlloc();
    TestMixedAtomicNonAtomic();
    TestAtomicAfterYoungGc();

    threading::UnregisterThread();

    printf("\n══ Results: 5 tests, %d failures ══\n", g_failures);
    return g_failures > 0 ? 1 : 0;
}
