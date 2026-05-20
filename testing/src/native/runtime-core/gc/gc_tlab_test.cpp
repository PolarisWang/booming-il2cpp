/// gc_tlab_test — TLAB (Thread-Local Allocation Buffer) unit tests (GoogleTest).

#include <cstdint>
#include <cstring>

#include <chaos/native_types.h>
#include "gc_scheduler.h"
#include "gc_young_gen.h"
#include "gc_region.h"

#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

// ── Test 1: TlabClaimFromYoungGen ───────────────────────────────────

TEST(GcTlab, ClaimFromYoungGen) {
    InitYoungGeneration();

    TLAB tlab = TlabClaimFromYoungGen();
    ASSERT_NE(tlab.start, nullptr);
    ASSERT_NE(tlab.current, nullptr);
    ASSERT_NE(tlab.end, nullptr);

    EXPECT_EQ(reinterpret_cast<uintptr_t>(tlab.start) & 7, 0u)
        << "TLAB start is 8-byte aligned";

    size_t size = static_cast<size_t>(
        reinterpret_cast<char*>(tlab.end) - reinterpret_cast<char*>(tlab.start));
    EXPECT_GT(size, 0u) << "TLAB has positive size";
    EXPECT_LE(size, 1024 * 1024u) << "TLAB size within reasonable bounds";

    TLAB tlab2 = TlabClaimFromYoungGen();
    EXPECT_NE(tlab2.start, tlab.start) << "second TLAB is different from first";
}

// ── Test 2: FlushTlsAllocCounter ────────────────────────────────────

TEST(GcTlab, FlushCounter) {
    FlushTlsAllocCounter();
}

// ── Test 3: Init / Destroy lifecycle ────────────────────────────────

TEST(GcTlab, InitDestroy) {
    InitYoungGeneration();
    DestroyYoungGeneration();
}

// ── Test 4: TLAB fallback on exhaustion ─────────────────────────────

TEST(GcTlab, ExhaustionFallback) {
    InitYoungGeneration();
    void* large = NurseryAllocate(32 * 1024);
    ASSERT_NE(large, nullptr) << "large allocation succeeds via fallback";
}
