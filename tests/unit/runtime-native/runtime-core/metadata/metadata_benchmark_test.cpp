// metadata_benchmark_test.cpp — Repeatable performance baselines for MetadataRegistry
//
// Measures three key MetadataRegistry query paths to establish performance
// baselines and detect regressions:
//   1. GetMethodName — hot-cache lookup (O(1) hash table, cached after first call)
//   2. FindToken — hot-index lookup (O(1) hash table, cached after first build)
//   3. EnumerateMethods — AOT module method enumeration (linear scan per type)
//
// Build: requires runtime-core, bootstrap, interpreter, codegen, hot-update libs.
// Run:   ctest -R metadata_benchmark_test -V

#include <gtest/gtest.h>

#include <metadata_interface.h>

#include <codegen_bridge.h>
#include <patch_data.h>

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include <memory_domain.h>

#include <chaos/native_types.h>
#include <chaos/type_info.h>

namespace rc = chaos::il2cpp::runtime_core;
namespace md = chaos::il2cpp::memory_domain;
namespace cc = chaos::il2cpp::common;

// ════════════════════════════════════════════════════════════════════════════
// Mock AOT constexpr data — 2 types, 3 methods total
// ════════════════════════════════════════════════════════════════════════════

static constexpr rc::ReflectionQueryMethodDescriptor kBenchMockMethods1[] = {
    { 0x06000001, "MockType:TestMethod",  "TestMethod",  "System.Int32", 0, nullptr, 0, nullptr, 0 },
    { 0x06000002, "MockType:MethodTwo",   "MethodTwo",   "System.Void",  0, nullptr, 0, nullptr, 0 }
};

static constexpr rc::ReflectionQueryFieldDescriptor kBenchMockFields1[] = {
    { 0x04000001, "MockType:TestField", "TestField", "System.Int32", 0, 0 }
};

static constexpr rc::ReflectionQueryPropertyDescriptor kBenchMockProps1[] = {
    { "MockType:TestProp", "TestProp", "System.Int32", 0 }
};

static constexpr rc::ReflectionQueryTypeDescriptor kBenchMockTypeDesc1 = {
    0x02000001,
    "MockType",
    nullptr,
    "TestNamespace",
    "BenchTypeOne",
    "BenchTypeOne",
    nullptr,
    kBenchMockFields1, 1,
    kBenchMockProps1, 1,
    nullptr, 0,
    kBenchMockMethods1, 2,
    nullptr, 0, 0
};

static constexpr rc::ReflectionQueryMethodDescriptor kBenchMockMethods2[] = {
    { 0x06000003, "MockType:MethodThree", "MethodThree", "System.String", 0, nullptr, 0, nullptr, 0 }
};

static constexpr rc::ReflectionQueryTypeDescriptor kBenchMockTypeDesc2 = {
    0x02000002,
    "MockType2",
    nullptr,
    "TestNamespace",
    "BenchTypeTwo",
    "BenchTypeTwo",
    nullptr,
    nullptr, 0,
    nullptr, 0,
    nullptr, 0,
    kBenchMockMethods2, 1,
    nullptr, 0, 0
};

static constexpr const rc::ReflectionQueryTypeDescriptor* kBenchMockTypePtrs[] = {
    &kBenchMockTypeDesc1,
    &kBenchMockTypeDesc2
};

static constexpr rc::ReflectionQueryImageDescriptor kBenchMockImage = {
    "BenchmarkTestModule",
    kBenchMockTypePtrs,
    2
};

// ── Domain-heap TypeInfoHot for type_info_ptrs ──
static cc::TypeInfoHot* g_bench_type_info_hot[2] = { nullptr, nullptr };
static const cc::TypeInfoHot* g_bench_type_info_ptrs[2] = { nullptr, nullptr };

static uint32_t g_bench_module_id = rc::kInvalidModuleId;

// ════════════════════════════════════════════════════════════════════════════
// Fixture
// ════════════════════════════════════════════════════════════════════════════

class MetadataBenchmarkTest : public ::testing::Test {
protected:
    static md::MemoryDomain* s_domain;
    static md::DomainId      s_domain_id;

    static void SetUpTestSuite() {
        md::DomainInit init{};
        init.module_name = "MetadataBenchTestDomain";
        init.module_kind = 1;
        init.usage_limit = 0;

        s_domain_id = md::RegisterMemoryDomain(init);
        ASSERT_NE(s_domain_id, md::kDomainIdInvalid);
        s_domain = md::FindDomainById(s_domain_id);
        ASSERT_NE(s_domain, nullptr);

        int push = md::PushDomain(s_domain);

        for (int i = 0; i < 2; i++) {
            auto* raw = CHAOS_IL2CPP_DOMAIN_CURRENT_NEW(cc::TypeInfoV0);
            ASSERT_NE(raw, nullptr);
            std::memset(raw, 0, sizeof(cc::TypeInfoV0));
            raw->hot.stable_id  = cc::chaos_compute_type_stable_id(
                i == 0 ? "BenchTypeOne" : "BenchTypeTwo");
            raw->hot.warm_delta = sizeof(cc::TypeInfoHot);
            raw->hot.type_shape = cc::chaos_type_shape_reference;
            g_bench_type_info_hot[i] = &raw->hot;
            g_bench_type_info_ptrs[i] = &raw->hot;
        }

        rc::ModuleDescriptor mod_desc{};
        mod_desc.name_utf8           = "MetadataBenchTestModule";
        mod_desc.image               = &kBenchMockImage;
        mod_desc.type_info_ptrs      = g_bench_type_info_ptrs;
        mod_desc.type_count          = 2;

        g_bench_module_id = rc::RegisterModule("MetadataBenchTestModule", &mod_desc);
        ASSERT_NE(g_bench_module_id, rc::kInvalidModuleId);

        md::PopDomain(push);
    }

    static void TearDownTestSuite() {
        if (s_domain_id != md::kDomainIdInvalid) {
            md::UnregisterMemoryDomain(s_domain_id);
            s_domain_id = md::kDomainIdInvalid;
            s_domain = nullptr;
        }
    }
};

md::MemoryDomain* MetadataBenchmarkTest::s_domain    = nullptr;
md::DomainId      MetadataBenchmarkTest::s_domain_id = md::kDomainIdInvalid;

// ════════════════════════════════════════════════════════════════════════════
// Benchmark helpers
// ════════════════════════════════════════════════════════════════════════════

/// Run `func` for `iterations` repetitions, return wall time in microseconds.
template <typename F>
static int64_t BenchTimed(uint32_t iterations, F&& func) {
    auto start = std::chrono::high_resolution_clock::now();
    for (uint32_t i = 0; i < iterations; ++i) {
        func();
    }
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

// ════════════════════════════════════════════════════════════════════════════
// Baseline 1: GetMethodName — hot-cache lookup throughput
// ════════════════════════════════════════════════════════════════════════════
//
// After the first call populates the cache, subsequent GetMethodName calls
// are O(1) unordered_dense hash lookups.  Measures 10,000 cached lookups.
TEST_F(MetadataBenchmarkTest, GetMethodNameHotCache) {
    auto& registry = rc::MetadataRegistry::Get();

    // Warm-up: first call builds the method_name_cache_.
    uint64_t handle = (static_cast<uint64_t>(g_bench_module_id) << 32) | 0x06000001u;
    const char* name = registry.GetMethodName(handle);
    ASSERT_NE(name, nullptr);
    ASSERT_STREQ(name, "TestMethod");

    // Benchmark: 10,000 cached lookups.
    constexpr uint32_t kIterations = 10000;
    int64_t total_us = BenchTimed(kIterations, [&]() {
        // Use a volatile sink to prevent optimization.
        volatile const char* n = registry.GetMethodName(handle);
        (void)n;
    });

    double avg_ns = (static_cast<double>(total_us) * 1000.0) / static_cast<double>(kIterations);
    std::printf("[BENCH] GetMethodName hot-cache: %d iterations in %lld us (%.1f ns/iter)\n",
                kIterations, static_cast<long long>(total_us), avg_ns);

    // Sanity: hot-cache lookup should take < 200 ns per call.
    EXPECT_LT(avg_ns, 200.0);
}

// ════════════════════════════════════════════════════════════════════════════
// Baseline 2: FindToken — hot-index lookup throughput
// ════════════════════════════════════════════════════════════════════════════
//
// After the first call builds the token_index_, subsequent FindToken calls
// are O(1) unordered_dense hash lookups.  Measures 10,000 cached lookups.
TEST_F(MetadataBenchmarkTest, FindTokenHotIndex) {
    auto& registry = rc::MetadataRegistry::Get();

    // Warm-up: first call builds the token_index_.
    auto composite = registry.FindToken(0x06000001u);
    ASSERT_NE(composite, 0u);

    // Benchmark: 10,000 cached lookups.
    constexpr uint32_t kIterations = 10000;
    int64_t total_us = BenchTimed(kIterations, [&]() {
        volatile uint64_t c = registry.FindToken(0x06000001u);
        (void)c;
    });

    double avg_ns = (static_cast<double>(total_us) * 1000.0) / static_cast<double>(kIterations);
    std::printf("[BENCH] FindToken hot-index: %d iterations in %lld us (%.1f ns/iter)\n",
                kIterations, static_cast<long long>(total_us), avg_ns);

    // Sanity: hot-index lookup should take < 200 ns per call.
    EXPECT_LT(avg_ns, 200.0);
}

// ════════════════════════════════════════════════════════════════════════════
// Baseline 3: EnumerateMethods — AOT module enumeration throughput
// ════════════════════════════════════════════════════════════════════════════
//
// Enumerates all methods in the mock AOT module (2 types, 3 methods total).
// Measures 10,000 enumerations to establish baseline.
TEST_F(MetadataBenchmarkTest, EnumerateMethodsAotModule) {
    auto& registry = rc::MetadataRegistry::Get();

    // First, verify enumeration returns the correct number of methods.
    void* buf[64];
    uint32_t count = registry.EnumerateMethods(g_bench_module_id, buf, 64);
    ASSERT_EQ(count, 3u);  // 2 methods from type 1 + 1 method from type 2

    // Verify method tokens are correct (real tokens, not placeholders).
    bool found_m1 = false, found_m2 = false, found_m3 = false;
    for (uint32_t i = 0; i < count; ++i) {
        uint64_t handle = reinterpret_cast<uint64_t>(buf[i]);
        uint32_t token = static_cast<uint32_t>(handle & 0xFFFFFFFFu);
        uint32_t mod_id = static_cast<uint32_t>(handle >> 32);
        EXPECT_EQ(mod_id, g_bench_module_id);
        if (token == 0x06000001u) found_m1 = true;
        if (token == 0x06000002u) found_m2 = true;
        if (token == 0x06000003u) found_m3 = true;
    }
    EXPECT_TRUE(found_m1);
    EXPECT_TRUE(found_m2);
    EXPECT_TRUE(found_m3);

    // Benchmark: 10,000 enumerations.
    constexpr uint32_t kIterations = 10000;
    int64_t total_us = BenchTimed(kIterations, [&]() {
        void* tmp[64];
        volatile uint32_t c = registry.EnumerateMethods(g_bench_module_id, tmp, 64);
        (void)c;
    });

    double avg_ns = (static_cast<double>(total_us) * 1000.0) / static_cast<double>(kIterations);
    std::printf("[BENCH] EnumerateMethods AOT module: %d iterations in %lld us (%.1f ns/iter)\n",
                kIterations, static_cast<long long>(total_us), avg_ns);

    // Sanity: enumeration of 3 methods should take < 500 ns.
    EXPECT_LT(avg_ns, 500.0);
}
