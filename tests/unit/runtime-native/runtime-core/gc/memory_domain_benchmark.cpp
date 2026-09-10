/// MemoryDomain benchmark test.
///
/// Measures performance of key domain operations:
///   1. PushDomain+PopDomain — 100,000 iterations
///   2. DomainCurrentAllocateTagged+DomainFreeTagged — 100,000 iterations
///   3. DomainCurrentReallocateTagged — 10,000 iterations
///   4. FindDomainById — 100,000 iterations
///   5. RegisterMemoryDomain+UnregisterMemoryDomain — 10,000 iterations
///
/// All measurements use std::chrono::high_resolution_clock and print
/// results in the format:
///   --- <name> ---
///     <desc>    <iters> iters  <total> us total  <avg_ns> ns/iter

#include <gtest/gtest.h>

#include "memory_domain.h"

#include <chaos/native_types.h>

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>

using namespace chaos::il2cpp::memory_domain;

static constexpr const char* kModuleName   = "benchmark_test.dll";
static constexpr CHAOS_IL2CPP_UINT32 kHotUpdateKind = 1u;

// ═══════════════════════════════════════════════════════════════════════════
// Test fixture — registers a domain once for all benchmarks
// ═══════════════════════════════════════════════════════════════════════════

class MemoryDomainBenchmark : public ::testing::Test {
protected:
    static MemoryDomain* s_domain;
    static DomainId      s_domain_id;

    using Clock = std::chrono::high_resolution_clock;

    static void SetUpTestSuite() {
        DomainInit init{};
        init.module_name = kModuleName;
        init.module_kind = kHotUpdateKind;
        init.usage_limit = 0;

        s_domain_id = RegisterMemoryDomain(init);
        ASSERT_NE(s_domain_id, kDomainIdInvalid)
            << "RegisterMemoryDomain failed for benchmark domain";

        s_domain = FindDomainById(s_domain_id);
        ASSERT_NE(s_domain, nullptr)
            << "FindDomainById returned null for benchmark domain";
    }

    static void TearDownTestSuite() {
        if (s_domain_id != kDomainIdInvalid) {
            UnregisterMemoryDomain(s_domain_id);
            s_domain_id = kDomainIdInvalid;
            s_domain = nullptr;
        }
    }

    /// Run `func` for `iterations` repetitions, return total time in microseconds.
    template <typename F>
    static int64_t BenchTimed(uint32_t iterations, F&& func) {
        auto start = Clock::now();
        for (uint32_t i = 0; i < iterations; ++i) func();
        auto end = Clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    }

    /// Print a single benchmark result.
    static void PrintResult(const char* desc, uint32_t iters, int64_t total_us) {
        double avg_ns = (static_cast<double>(total_us) * 1000.0) / static_cast<double>(iters);
        std::printf("  %-45s %8d iters  %8lld us total  %8.1f ns/iter\n",
                    desc, iters,
                    static_cast<long long>(total_us), avg_ns);
    }
};

MemoryDomain* MemoryDomainBenchmark::s_domain    = nullptr;
DomainId      MemoryDomainBenchmark::s_domain_id = kDomainIdInvalid;

// ═══════════════════════════════════════════════════════════════════════════
// Benchmark 1: PushDomain + PopDomain — 100,000 iterations
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(MemoryDomainBenchmark, PushPop_100kIterations) {
    constexpr uint32_t kIter = 100000;
    std::printf("\n--- PushPop_100kIterations ---\n");

    auto total = BenchTimed(kIter, [this]() {
        int depth = PushDomain(s_domain);
        PopDomain(depth);
    });
    PrintResult("PushDomain+PopDomain", kIter, total);
    double avg_ns = (static_cast<double>(total) * 1000.0) / static_cast<double>(kIter);
    std::printf("BENCH, PushPop_100k,AVG=%.1f,N=%d\n", avg_ns, kIter);
}

// ═══════════════════════════════════════════════════════════════════════════
// Benchmark 2: AllocateTagged + FreeTagged — 100,000 iterations
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(MemoryDomainBenchmark, AllocateTagged_100kIterations) {
    constexpr uint32_t kIter = 100000;
    std::printf("\n--- AllocateTagged_100kIterations ---\n");

    DomainScope scope(s_domain);

    auto total = BenchTimed(kIter, [this]() {
        void* p = DomainCurrentAllocateTagged(64);
        if (p) {
            std::memset(p, 0xAA, 64);
            DomainFreeTagged(p);
        }
    });
    PrintResult("DomainCurrentAllocateTagged+FreeTagged (64 bytes)", kIter, total);
    double avg_ns = (static_cast<double>(total) * 1000.0) / static_cast<double>(kIter);
    std::printf("BENCH, AllocateTaggedFree_100k,AVG=%.1f,N=%d\n", avg_ns, kIter);
}

// ═══════════════════════════════════════════════════════════════════════════
// Benchmark 3: ReallocateTagged — 10,000 iterations
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(MemoryDomainBenchmark, ReallocateTagged_10kIterations) {
    constexpr uint32_t kIter = 10000;
    std::printf("\n--- ReallocateTagged_10kIterations ---\n");

    DomainScope scope(s_domain);

    auto total = BenchTimed(kIter, [this]() {
        void* p = DomainCurrentAllocateTagged(32);
        if (p) {
            void* q = DomainCurrentReallocateTagged(p, 128);
            if (q) {
                DomainFreeTagged(q);
            }
        }
    });
    PrintResult("DomainCurrentReallocateTagged (32->128 bytes)", kIter, total);
    double avg_ns = (static_cast<double>(total) * 1000.0) / static_cast<double>(kIter);
    std::printf("BENCH, ReallocateTagged_10k,AVG=%.1f,N=%d\n", avg_ns, kIter);
}

// ═══════════════════════════════════════════════════════════════════════════
// Benchmark 4: FindDomainById — 100,000 iterations
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(MemoryDomainBenchmark, FindById_100kIterations) {
    constexpr uint32_t kIter = 100000;
    std::printf("\n--- FindById_100kIterations ---\n");

    DomainId id = s_domain_id;

    auto total = BenchTimed(kIter, [id]() {
        volatile MemoryDomain* d = FindDomainById(id);
        (void)d;
    });
    PrintResult("FindDomainById", kIter, total);
    double avg_ns = (static_cast<double>(total) * 1000.0) / static_cast<double>(kIter);
    std::printf("BENCH, FindById_100k,AVG=%.1f,N=%d\n", avg_ns, kIter);
}

// ═══════════════════════════════════════════════════════════════════════════
// Benchmark 5: RegisterMemoryDomain + UnregisterMemoryDomain — 10,000 iters
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(MemoryDomainBenchmark, RegisterUnregister_10kIterations) {
    constexpr uint32_t kIter = 10000;
    std::printf("\n--- RegisterUnregister_10kIterations ---\n");

    auto total = BenchTimed(kIter, []() {
        DomainInit init{};
        init.module_name = "bench_temp.dll";
        init.module_kind = 0;
        init.usage_limit = 0;
        DomainId id = RegisterMemoryDomain(init);
        if (id != kDomainIdInvalid) {
            UnregisterMemoryDomain(id);
        }
    });
    PrintResult("RegisterMemoryDomain+UnregisterMemoryDomain", kIter, total);
    double avg_ns = (static_cast<double>(total) * 1000.0) / static_cast<double>(kIter);
    std::printf("BENCH, RegisterUnregister_10k,AVG=%.1f,N=%d\n", avg_ns, kIter);
}
