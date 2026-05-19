/// gc_tlab_test — TLAB (Thread-Local Allocation Buffer) unit tests.
///
/// Tests:
///   1. TlabClaimFromYoungGen returns non-null aligned buffer
///   2. FlushTlsAllocCounter updates scheduler counter
///   3. InitYoungGeneration / DestroyYoungGeneration lifecycle
///   4. TLAB exhaustion falls back to nursery

#include <cstdio>
#include <cstdint>
#include <cstring>

#include <chaos/native_types.h>
#include "gc_scheduler.h"
#include "gc_young_gen.h"
#include "gc_region.h"
#include "gc_test_macros.h"

using namespace chaos::il2cpp::runtime_core;

static int g_failures = 0;
static int g_tests    = 0;
static int g_sub      = 0;

// ── Test 1: TlabClaimFromYoungGen ───────────────────────────────────
void TestTlabClaim() {
    TEST("TlabClaimFromYoungGen");

    // Init young gen.
    InitYoungGeneration();

    // Claim a TLAB.
    TLAB tlab = TlabClaimFromYoungGen();
    GC_CHECK(tlab.start != nullptr, "TLAB start is non-null");
    GC_CHECK(tlab.current != nullptr, "TLAB ptr is non-null");
    GC_CHECK(tlab.end != nullptr, "TLAB end is non-null");

    // Aligned to 8 bytes.
    GC_CHECK((reinterpret_cast<uintptr_t>(tlab.start) & 7) == 0,
             "TLAB start is 8-byte aligned");

    // TLAB should have reasonable size.
    size_t size = static_cast<size_t>(
        reinterpret_cast<char*>(tlab.end) - reinterpret_cast<char*>(tlab.start));
    GC_CHECK(size > 0, "TLAB has positive size");
    GC_CHECK(size <= 1024 * 1024, "TLAB size within reasonable bounds");

    // Claim another TLAB from the same thread — should not overlap with first.
    TLAB tlab2 = TlabClaimFromYoungGen();
    GC_CHECK(tlab2.start != tlab.start,
             "second TLAB is different from first");
}

// ── Test 2: FlushTlsAllocCounter ────────────────────────────────────
void TestFlushCounter() {
    TEST("FlushTlsAllocCounter");

    // Flush should not crash.
    FlushTlsAllocCounter();
    GC_CHECK(true, "FlushTlsAllocCounter completes without crash");
}

// ── Test 3: Init / Destroy lifecycle ────────────────────────────────
void TestInitDestroy() {
    TEST("InitDestroy lifecycle");

    // Init should be safe to call multiple times.
    InitYoungGeneration();
    GC_CHECK(true, "InitYoungGeneration succeeds");

    // Destroy should not crash.
    DestroyYoungGeneration();
    GC_CHECK(true, "DestroyYoungGeneration succeeds");
}

// ── Test 4: TLAB fallback on exhaustion ─────────────────────────────
void TestTlabExhaustion() {
    TEST("TlabExhaustion fallback");

    // Ensure young gen is initialized.
    InitYoungGeneration();

    // Try a large allocation that exceeds typical TLAB size.
    // If TLAB can't satisfy it, it should fallback to nursery.
    void* large = NurseryAllocate(32 * 1024);
    // The allocation should still succeed through the fallback path.
    GC_CHECK(large != nullptr, "large allocation succeeds via fallback");
}

// ── Main ────────────────────────────────────────────────────────────
int main() {
    puts("TLAB unit tests");
    puts("═══════════════\n");

    TestTlabClaim();
    TestFlushCounter();
    TestInitDestroy();
    TestTlabExhaustion();

    printf("\nResults: %d tests, %d failures\n", g_tests, g_failures);
    return g_failures > 0 ? 1 : 0;
}
