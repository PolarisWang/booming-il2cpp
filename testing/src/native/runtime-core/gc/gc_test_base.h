/// gc_test_base.h — Shared test fixture hierarchy for GC tests (GoogleTest).
///
/// Four-level hierarchy:
///   GcTestBase       — common SetUp/TearDown (InitYoungGeneration + thread state)
///   GcUnitTestBase   — adds default TypeInfo + GcLayout registration
///   GcStressTestBase — adds multi-threaded stress test helpers
///   GcBenchTestBase  — adds RDTSC timing helpers
///
/// Usage:
///   TEST_F(GcUnitTestBase, MyTest) { ... }
///   TEST_F(GcStressTestBase, ScenarioS) { ... }

#ifndef CHAOS_IL2CPP_GC_TEST_BASE_H_
#define CHAOS_IL2CPP_GC_TEST_BASE_H_

#include <atomic>
#include <cstdint>
#include <functional>
#include <thread>
#include <vector>

#if defined(_MSC_VER)
#include <intrin.h>
#endif

#include <chaos/native_types.h>
#include "gc_layout.h"
#include "gc_old_gen.h"
#include "gc_region.h"
#include "gc_scheduler.h"
#include "gc_young_gen.h"
#include "thread_state.h"

#include <gtest/gtest.h>

namespace chaos { namespace il2cpp { namespace runtime_core {

// ═══════════════════════════════════════════════════════════════════════════
// Level 1: GcTestBase — minimal shared fixture
// ═══════════════════════════════════════════════════════════════════════════
//
// SetUp:
//   1. AllocateThreadId + RegisterThread + EnterCooperativeMode
//   2. InitYoungGeneration()
//
// TearDown:
//   1. UnregisterThread
//
// Provides utility methods:
//   ForceYoungGc()   — calls GcYoungCollection()
//   ForceFullGc()    — calls chaos_gc_collect()
//   AllocNursery()   — calls NurseryAllocate(size)
// ═══════════════════════════════════════════════════════════════════════════

struct GcTestBase : ::testing::Test {
    uint32_t tid = 0;

    void SetUp() override {
        tid = threading::AllocateThreadId();
        threading::RegisterThread(tid, nullptr);
        threading::EnterCooperativeMode();
        InitYoungGeneration();
    }

    void TearDown() override {
        threading::UnregisterThread();
    }

    // ── Utility methods ─────────────────────────────────────────────

    static void* AllocNursery(CHAOS_IL2CPP_SIZE size) {
        return NurseryAllocate(size);
    }
};

// ═══════════════════════════════════════════════════════════════════════════
// Level 2: GcUnitTestBase — adds default type info and layout registration
// ═══════════════════════════════════════════════════════════════════════════
//
// Extends GcTestBase by registering a small set of TypeInfo entries and
// a default GcLayout so that typed allocations are recognised by the GC.
//
// Test files that override SetUp() must call GcUnitTestBase::SetUp() first.
// ═══════════════════════════════════════════════════════════════════════════

/// Default instance sizes for which test TypeInfo entries are registered.
static constexpr CHAOS_IL2CPP_SIZE kTestTypeSizes[] = { 32, 64, 128, 256, 512, 1024, 4096 };

/// A simple TypeInfo struct matching the layout expected by GC code.
struct alignas(8) GcTestTypeInfo {
    uint64_t stable_id;
    uint64_t reserved[3];
};

/// Per-size TestTypeInfo storage (one per entry in kTestTypeSizes).
struct GcTestTypeRegistry {
    GcTestTypeInfo infos[sizeof(kTestTypeSizes) / sizeof(kTestTypeSizes[0])];
    uintptr_t range_begin;
    uintptr_t range_end;
};

/// Register test TypeInfo entries and return the registry.
/// Called once by GcUnitTestBase::SetUp(); safe to call multiple times.
GcTestTypeRegistry& SetupDefaultTestTypes();

struct GcUnitTestBase : GcTestBase {
    void SetUp() override {
        GcTestBase::SetUp();

        // Register test TypeInfo entries.
        auto& reg = SetupDefaultTestTypes();
        range_begin_ = reg.range_begin;
        range_end_ = reg.range_end;

        // Register a standard pointer-bearing layout for 64-byte objects.
        // Offset 8 holds a single pointer field.
        static constexpr uint16_t kPtrOffsets[] = { 8 };
        uint64_t sid = GcLayoutRegistry::Instance().RegisterOrGetRawAllocType(64);
        GcLayoutRegistry::Instance().Register(sid, 64, kPtrOffsets, 1);

        // Warmup: allocate one old-gen object so the old-gen page pool is ready.
        warmup_ = g_old_gen.Allocate(8, true);
    }

    void TearDown() override {
        warmup_ = nullptr;
        GcTestBase::TearDown();
    }

    /// Return a test TypeInfo pointer for the given instance size.
    /// Returns nullptr if size does not match a registered test type.
    static const void* GetTestTypeInfo(CHAOS_IL2CPP_SIZE instance_size);

    /// Initialize an object with a test TypeInfo header.
    static void InitTestObject(void* obj, CHAOS_IL2CPP_SIZE size,
                               uint32_t pattern = 0xCAFEBABE) {
        const void* ti = GetTestTypeInfo(size);
        if (ti) {
            *static_cast<const void**>(obj) = ti;
        }
        if (size >= 12) {
            *reinterpret_cast<uint32_t*>(static_cast<char*>(obj) + 8) = pattern;
        }
    }

    static const void* test_type_info_64() { return GetTestTypeInfo(64); }

    // ── Gen1 benchmark utilities ─────────────────────────────────────

    /// Fill the Gen1 (survivor) area to the given occupancy fraction.
    static void FillSurvivorTo(float occupancy);

    /// Run GcGen1Collection() and return the pause duration in ns.
    static uint64_t MeasureGen1Collection();

    /// Read the x86/x64 TSC (time-stamp counter).
    static inline uint64_t Rdtsc() {
#if defined(_MSC_VER)
        return __rdtsc();
#elif defined(__x86_64__) || defined(__i386__)
        uint32_t lo, hi;
        asm volatile("rdtsc" : "=a"(lo), "=d"(hi));
        return (static_cast<uint64_t>(hi) << 32) | lo;
#else
        return 0;
#endif
    }

    /// Approximate RDTSC ticks to nanoseconds using QPC calibration.
    static uint64_t RdtscToNs(uint64_t ticks);

protected:
    static GcTestTypeRegistry* s_test_types_;
    static void* s_warmup_;

private:
    uintptr_t range_begin_ = 0;
    uintptr_t range_end_ = 0;
    void* warmup_ = nullptr;
};

// ═══════════════════════════════════════════════════════════════════════════
// Level 3: GcStressTestBase — multi-threaded stress test helpers
// ═══════════════════════════════════════════════════════════════════════════
//
// Provides:
//   StressConfig        — struct describing allocation mix + survival strategy
//   RunConcurrentAlloc  — spawn threads, allocate, GC, join
//   VerifyNoCorruption  — check all surviving objects for magic-pattern integrity
// ═══════════════════════════════════════════════════════════════════════════

/// Survival level for stress test objects.
enum class SurvivalLevel {
    GLOBAL,         // Referenced from a global array (survives all GCs)
    THREAD_LOCAL,   // Referenced from a TLS array (survives until thread exits)
    EPHEMERAL,      // No external reference (dies at next GC)
};

/// Size distribution entry: a size class and its probability weight.
struct SizeClass {
    CHAOS_IL2CPP_SIZE size;
    float probability;  // 0.0–1.0; normalised across all entries
};

/// Survival strategy entry.
struct SurvivalClass {
    SurvivalLevel level;
    float probability;  // 0.0–1.0; normalised across all entries
};

/// Configuration for RunConcurrentAlloc.
struct StressConfig {
    int thread_count = 4;
    int allocs_per_thread = 100;
    std::vector<SizeClass> size_distribution = {
        { 64,  0.40f },
        { 512, 0.30f },
        { 4096, 0.20f },
        { 85 * 1024, 0.10f },  // LOH threshold
    };
    std::vector<SurvivalClass> survival_strategy = {
        { SurvivalLevel::GLOBAL,       0.20f },
        { SurvivalLevel::THREAD_LOCAL, 0.50f },
        { SurvivalLevel::EPHEMERAL,    0.30f },
    };
    /// If > 0, force a GC every N allocations per thread.
    int gc_interval = 0;
    /// Magic pattern to write at the start of each object for integrity checks.
    uint32_t magic_pattern = 0xDEADBEEF;
};

struct GcStressTestBase : GcUnitTestBase {
    /// Run a concurrent allocation stress test according to config.
    /// All threads are registered, allocate, and optionally trigger GC.
    /// Returns the number of surviving global objects (for post-hoc checks).
    int RunConcurrentAlloc(const StressConfig& cfg);

    /// Verify that global surviving objects have intact magic patterns.
    /// Call after RunConcurrentAlloc to confirm no memory corruption.
    void VerifyNoCorruption(const StressConfig& cfg);

    /// Thread context passed to each worker thread.
    struct ThreadCtx {
        int id;
        StressConfig cfg;
        std::atomic<int>* ok;
    };
};

// ═══════════════════════════════════════════════════════════════════════════
// Level 4: GcBenchTestBase — benchmark timing helpers
// ═══════════════════════════════════════════════════════════════════════════
//
// Provides RDTSC-based timing and metric recording for GC benchmarks.
// ═══════════════════════════════════════════════════════════════════════════

struct GcBenchTestBase : GcUnitTestBase {
    /// Record a named metric and print to stdout.
    static void RecordMetric(const char* name, uint64_t value_ns);
};

}}}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_TEST_BASE_H_
