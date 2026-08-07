/// Unit tests for GcBumpCache — thread-local size-class GC allocator (GoogleTest).
///
/// Exercises: size-class hit/miss, refill, bump fallback, oversized bypass,
/// atomic vs scan page chains, and allocation counters.

#include <chaos/native_types.h>

#include "gc_bump_cache.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>

using GcBumpCache = chaos::il2cpp::runtime_core::GcBumpCache;

// ════════════════════════════════════════════════════════════════════════════
// Test: Size-class hit
// ════════════════════════════════════════════════════════════════════════════

TEST(GcBumpCache, SizeClassHit) {
    GcBumpCache cache;
    void* prev = nullptr;
    int hit_count = 0;
    for (int i = 0; i < GcBumpCache::kBatchCount + 2; ++i) {
        void* p = cache.Allocate(24);
        ASSERT_NE(p, nullptr) << "null pointer from Allocate at iteration " << i;
        EXPECT_EQ(reinterpret_cast<uintptr_t>(p) % 8, 0u) << "misaligned pointer";
        std::memset(p, 0xAB, 24);
        for (CHAOS_IL2CPP_SIZE j = 0; j < 24; ++j) {
            EXPECT_EQ(static_cast<uint8_t*>(p)[j], 0xAB) << "memory corruption at byte " << j;
        }
        prev = p;
        ++hit_count;
    }
    (void)prev;
}

// ════════════════════════════════════════════════════════════════════════════
// Test: Size-class miss → bump fallback
// ════════════════════════════════════════════════════════════════════════════

TEST(GcBumpCache, SizeClassMiss) {
    GcBumpCache cache;
    void* p = cache.Allocate(72);
    ASSERT_NE(p, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(p) % 8, 0u);
    std::memset(p, 0xCD, 72);
    for (CHAOS_IL2CPP_SIZE i = 0; i < 72; ++i) {
        EXPECT_EQ(static_cast<uint8_t*>(p)[i], 0xCD);
    }
}

// ════════════════════════════════════════════════════════════════════════════
// Test: Atomic vs scan page chains
// ════════════════════════════════════════════════════════════════════════════

TEST(GcBumpCache, AtomicVsScan) {
    GcBumpCache cache;
    void* scan_a = cache.Allocate(32);
    void* scan_b = cache.Allocate(32);
    void* atom_a = cache.AllocateAtomic(32);
    void* atom_b = cache.AllocateAtomic(32);

    ASSERT_NE(scan_a, nullptr);
    ASSERT_NE(scan_b, nullptr);
    ASSERT_NE(atom_a, nullptr);
    ASSERT_NE(atom_b, nullptr);

    for (auto* p : {scan_a, scan_b, atom_a, atom_b}) {
        EXPECT_EQ(reinterpret_cast<uintptr_t>(p) % 8, 0u);
    }

    std::memset(scan_a, 0x01, 32);
    std::memset(atom_a, 0x02, 32);
}

// ════════════════════════════════════════════════════════════════════════════
// Test: Oversized bypass
// ════════════════════════════════════════════════════════════════════════════

TEST(GcBumpCache, Oversized) {
    GcBumpCache cache;
    CHAOS_IL2CPP_SIZE big = GcBumpCache::kMaxInlineSize + 1;
    void* p = cache.Allocate(big);
    ASSERT_NE(p, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(p) % 8, 0u);
    std::memset(p, 0xEF, big);
    for (CHAOS_IL2CPP_SIZE i = 0; i < big; ++i) {
        EXPECT_EQ(static_cast<uint8_t*>(p)[i], 0xEF);
    }
}

// ════════════════════════════════════════════════════════════════════════════
// Test: Multiple sizes, no overlap
// ════════════════════════════════════════════════════════════════════════════

TEST(GcBumpCache, MultipleSizes) {
    GcBumpCache cache;
    CHAOS_IL2CPP_SIZE sizes[] = {16, 24, 32, 40, 48, 64, 128, 256, 512, 1024, 2048};
    void* ptrs[11] = {nullptr};

    for (int i = 0; i < 11; ++i) {
        ptrs[i] = cache.Allocate(sizes[i]);
        ASSERT_NE(ptrs[i], nullptr) << "null at index " << i;
        std::memset(ptrs[i], static_cast<uint8_t>(0x10 + i), sizes[i]);
    }
}

// ════════════════════════════════════════════════════════════════════════════
// Test: DumpCounters smoke
// ════════════════════════════════════════════════════════════════════════════

TEST(GcBumpCache, DumpCounters) {
    GcBumpCache cache;
    cache.Allocate(16);
    cache.Allocate(16);
    cache.AllocateAtomic(32);
    cache.AllocateAtomic(GcBumpCache::kMaxInlineSize + 1);
    cache.DumpCounters();
}
