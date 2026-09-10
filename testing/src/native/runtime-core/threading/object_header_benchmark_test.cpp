// object_header_benchmark_test.cpp — Type System Phase 1 benchmarks
//
// Benchmarks for the 8B unified object header + ThinLockTable:
//   thin-lock-uncontended:  10000 lock/unlock per sample (P4.2)
//   header-alloc-latency:   malloc + zero-init for 8B header (P4.4)
//
// Labeled as benchmark (excluded from default ctest run).

#include <gtest/gtest.h>

#include <thread_state.h>
#include <thread_pool.h>

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <new>
#include <thread>
#include <vector>

// ── AOT codegen stubs ─────────────────────────────────────────────────────
extern "C" const void* chaos_il2cpp_aot_hotpatch_module = nullptr;
extern "C" const char* const kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int32_t kChaosExternalRuntimeCount = 0;

namespace chaos::il2cpp::runtime_core {
    bool MonitorEnter(void* monitor_target);
    bool MonitorExit(void* monitor_target);
    bool MonitorTryEnter(void* monitor_target);
}

namespace threading = chaos::il2cpp::runtime_core::threading;
namespace rt = chaos::il2cpp::runtime_core;

using Clock = std::chrono::high_resolution_clock;
static constexpr size_t kObjectHeaderSize = 8;  // TypeInfo* only (Type System Phase 1)

// ── VolatileRead helper ───────────────────────────────────────────────────
template <typename T>
inline T VolatileRead(const T& addr) noexcept {
    return *static_cast<const volatile T*>(&addr);
}

// ── Shared TypeInfoHot stub for lock benchmarks ───────────────────────────
// Uses kTypeInfoHeaderKindThin so the object appears as a ThinLockable.
static const chaos::il2cpp::common::TypeInfoHot g_lock_type_info_stub = {
    nullptr,  nullptr, 0, 0, 0, 0,
    chaos::il2cpp::common::kTypeInfoHeaderKindThin
};

// ══════════════════════════════════════════════════════════════════════════
// Benchmark 1: Thin-lock uncontended
// Matches managed MonitorAndLockingBenchmark: 10000 lock/unlock per sample.
// P4.2: Thin-lock no-contention latency.
// ══════════════════════════════════════════════════════════════════════════

static void* g_lock_obj = nullptr;
static uint64_t g_lock_checksum = 0;

static void lock_setup() {
    threading::RegisterThread(threading::kMainThreadId, nullptr);
    g_lock_obj = std::malloc(kObjectHeaderSize);
    std::memset(g_lock_obj, 0, kObjectHeaderSize);
    *static_cast<const void**>(g_lock_obj) = &g_lock_type_info_stub;
    g_lock_checksum = 0;
}

static void lock_teardown() {
    std::free(g_lock_obj);
    g_lock_obj = nullptr;
    threading::UnregisterThread();
}

static double run_lock_bench(int iterations) {
    // Warmup
    for (int i = 0; i < 100; ++i) {
        if (rt::MonitorEnter(g_lock_obj)) {
            g_lock_checksum += (uint64_t)(i & 7);
            rt::MonitorExit(g_lock_obj);
        }
    }

    constexpr int kSamples = 3;
    double total_ms = 0;
    uint64_t total_cs = 0;

    for (int s = 0; s < kSamples; ++s) {
        uint64_t cs = 0;
        auto t0 = Clock::now();
        for (int i = 0; i < iterations; ++i) {
            if (rt::MonitorEnter(g_lock_obj)) {
                cs += (uint64_t)(i & 7);
                rt::MonitorExit(g_lock_obj);
            }
        }
        auto t1 = Clock::now();
        double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
        total_ms += ms;
        total_cs += cs;
    }

    // Consume checksum to prevent optimization.
    VolatileRead(g_lock_checksum);
    g_lock_checksum += total_cs;
    VolatileRead(g_lock_checksum);

    return total_ms / kSamples;
}

TEST(ObjectHeaderBenchmark, ThinLockUncontended)
{
    lock_setup();
    constexpr int kIterations = 10000;
    double avg_ms = run_lock_bench(kIterations);
    double avg_ns = (avg_ms * 1e6) / kIterations;

    printf("[BENCH] ThinLockUncontended: %.2f ms for %d iterations (%.0f ns/iter)\n",
           avg_ms, kIterations, avg_ns);

    // Verify the lock mechanism works correctly under load.
    EXPECT_GT(g_lock_checksum, 0ull);
    lock_teardown();
}

// ══════════════════════════════════════════════════════════════════════════
// Benchmark 2: Header allocation latency
// P4.4: Measures malloc + zero-init for 8B header (simulates ObjectNew).
// ══════════════════════════════════════════════════════════════════════════

static double run_alloc_bench(int iterations) {
    constexpr int kSamples = 3;
    double total_ms = 0;

    for (int s = 0; s < kSamples; ++s) {
        auto t0 = Clock::now();
        for (int i = 0; i < iterations; ++i) {
            void* p = std::malloc(kObjectHeaderSize);
            if (p) {
                std::memset(p, 0, kObjectHeaderSize);
                // Simulate writing TypeInfo* at offset 0.
                *static_cast<const void**>(p) = &g_lock_type_info_stub;
                // Prevent optimization: volatile read.
                VolatileRead(*static_cast<const volatile void**>(p));
                std::free(p);
            }
        }
        auto t1 = Clock::now();
        total_ms += std::chrono::duration<double, std::milli>(t1 - t0).count();
    }

    return total_ms / kSamples;
}

TEST(ObjectHeaderBenchmark, HeaderAllocLatency)
{
    constexpr int kIterations = 100000;
    double avg_ms = run_alloc_bench(kIterations);
    double avg_ns = (avg_ms * 1e6) / kIterations;

    printf("[BENCH] HeaderAllocLatency: %.2f ms for %d allocs (%.0f ns/alloc)\n",
           avg_ms, kIterations, avg_ns);

    // Sanity: allocation should complete in reasonable time.
    EXPECT_LT(avg_ms, 5000.0);  // <5s for 100K allocs
}
