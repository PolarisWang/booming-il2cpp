// profile_benchmarks.h — Native GC allocation benchmarks for profile pipeline
// Provides high-allocation benchmark functions that trigger GC collections,
// enabling the profile pipeline to measure gcPauseNs, nurseryAllocBytes, etc.
//
// Usage: included by runtime-entry.cpp, exposed via --profile CLI mode.
// These benchmarks are standalone C++ functions using Chaos GC APIs directly.
#pragma once

#include <chaos/native_types.h>
#include <gc/gc_api.h>

#ifdef __cplusplus
extern "C" {
#endif

// ── Benchmark entry table ──────────────────────────────────────────
// Each benchmark has a name and a function that performs the allocation.
typedef struct {
    const char* name;
    CHAOS_IL2CPP_INTPTR (*fn)();
} ProfileBenchmarkEntry;

// ── Single large allocation (1MB) ──────────────────────────────────
static CHAOS_IL2CPP_INTPTR Benchmark_AllocLarge() {
    // Allocate a 1MB byte array via GC
    const CHAOS_IL2CPP_SIZE kSize = 1024 * 1024;  // 1MB
    void* ptr = CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_UINT8, kSize);
    if (ptr) {
        // Touch memory to force physical commit
        auto* bytes = static_cast<CHAOS_IL2CPP_UINT8*>(ptr);
        for (CHAOS_IL2CPP_SIZE i = 0; i < kSize; i += 4096)
            bytes[i] = static_cast<CHAOS_IL2CPP_UINT8>(i);
    }
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(ptr);
}

// ── Many small allocations (1000 × 1KB = ~1MB) ─────────────────────
static CHAOS_IL2CPP_INTPTR Benchmark_ManySmall() {
    const int kCount = 1000;
    const CHAOS_IL2CPP_SIZE kSize = 1024;
    CHAOS_IL2CPP_INTPTR total = 0;
    for (int i = 0; i < kCount; i++) {
        void* ptr = CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_UINT8, kSize);
        total += reinterpret_cast<CHAOS_IL2CPP_INTPTR>(ptr);
    }
    return total;
}

// ── Boxing stress (1000 boxed ints) ─────────────────────────────────
static CHAOS_IL2CPP_INTPTR Benchmark_BoxingStress() {
    CHAOS_IL2CPP_INTPTR total = 0;
    for (int i = 0; i < 1000; i++) {
        void* boxed = CHAOS_IL2CPP_NEW_GC(CHAOS_IL2CPP_INT32, i * 42);
        total += reinterpret_cast<CHAOS_IL2CPP_INTPTR>(boxed);
    }
    return total;
}

// ── String-like concatenation (500 × 64 bytes = ~32KB) ─────────────
static CHAOS_IL2CPP_INTPTR Benchmark_StringConcat() {
    CHAOS_IL2CPP_INTPTR total = 0;
    for (int i = 0; i < 500; i++) {
        // Allocate and immediately discard (simulates string concat)
        void* ptr = CHAOS_IL2CPP_MALLOC(64);
        if (ptr) {
            total += reinterpret_cast<CHAOS_IL2CPP_INTPTR>(ptr);
            CHAOS_IL2CPP_FREE(ptr);
        }
    }
    return total;
}

// ── Benchmark table ────────────────────────────────────────────────
static const ProfileBenchmarkEntry kProfileBenchmarks[] = {
    {"AllocLarge",       Benchmark_AllocLarge},
    {"ManySmall",        Benchmark_ManySmall},
    {"BoxingStress",     Benchmark_BoxingStress},
    {"StringConcat",     Benchmark_StringConcat},
    {nullptr, nullptr}  // sentinel
};

static const int kProfileBenchmarkCount =
    sizeof(kProfileBenchmarks) / sizeof(kProfileBenchmarks[0]) - 1;  // exclude sentinel

#ifdef __cplusplus
}
#endif
