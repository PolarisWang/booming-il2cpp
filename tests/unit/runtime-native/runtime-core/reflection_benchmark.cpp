// reflection_benchmark.cpp — Performance baselines for native reflection APIs
//
// Measures four key reflection query paths used by managed/NativeAOT code:
//   1. Type flag queries (IsArray, IsEnum, IsPublic, IsAbstract, IsSealed,
//      IsValueType, IsInterface) — 100,000 iterations each
//   2. GetFields / GetMethods / GetProperties — 10,000 iterations each
//   3. GetElementType — 10,000 iterations
//   4. GetTypeFromHandle — 100,000 iterations
//
// Build: cmake --build build/testing --target reflection_benchmark --config RelWithDebInfo
// Run:   build/testing/runtime-core/RelWithDebInfo/reflection_benchmark.exe

#include <gtest/gtest.h>

#include <module_registry.h>
#include <reflection_api.h>
#include <reflection_query_model.h>
#include <codegen_bridge.h>

#include <memory_domain.h>

#include <chaos/native_types.h>
#include <chaos/type_info.h>

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>

namespace rc = chaos::il2cpp::runtime_core;
namespace md = chaos::il2cpp::memory_domain;
namespace cc = chaos::il2cpp::common;

// ── Stubs for symbols normally emitted by AOT codegen ────────────────────
namespace chaos { namespace il2cpp { namespace runtime_core {
extern "C" { void* chaos_il2cpp_aot_hotpatch_module = nullptr; }
extern "C" { const void* kChaosExternalRuntimeSubjects = nullptr; }
extern "C" { const void* kChaosExternalRuntimeFnTable = nullptr; }
extern "C" { int32_t kChaosExternalRuntimeCount = 0; }
}}}

// ════════════════════════════════════════════════════════════════════════════
// Mock AOT constexpr data — types with diverse flags and members
// ════════════════════════════════════════════════════════════════════════════

// --- Type 1: TestType (class, public) ---
static constexpr rc::ReflectionQueryFieldDescriptor kTestTypeFields[] = {
    { 0x04000001, "TestType:FieldOne",   "FieldOne",   "System.Int32", 0, rc::kFieldFlagIsPublic },
    { 0x04000002, "TestType:FieldTwo",   "FieldTwo",   "System.String", 0, rc::kFieldFlagIsPublic },
};

static constexpr rc::ReflectionQueryMethodDescriptor kTestTypeMethods[] = {
    { 0x06000001, "TestType:MethodOne",  "MethodOne",  "System.Int32", 0, nullptr, 0, nullptr, rc::kMethodFlagIsPublic },
};

static constexpr rc::ReflectionQueryPropertyDescriptor kTestTypeProperties[] = {
    { "TestType:PropOne", "PropOne", "System.Int32", rc::kPropertyFlagCanRead | rc::kPropertyFlagCanWrite },
};

static constexpr rc::ReflectionQueryTypeDescriptor kTestTypeDesc = {
    0x02000001,
    "TestType",
    nullptr,
    "TestNamespace",
    "TestType",
    "TestType",
    nullptr,
    kTestTypeFields, 2,
    kTestTypeProperties, 1,
    nullptr, 0,
    kTestTypeMethods, 1,
    nullptr, 0,
    rc::kFlagIsPublic,
};

// --- Type 2: TestEnum (enum, sealed, public) ---
static constexpr rc::ReflectionQueryFieldDescriptor kTestEnumFields[] = {
    { 0x04000003, "TestEnum:value__", "value__", "System.Int32", 0, rc::kFieldFlagIsPublic },
    { 0x04000004, "TestEnum:Zero",    "Zero",    "TestEnum",     0, rc::kFieldFlagIsPublic | rc::kFieldFlagIsStatic | rc::kFieldFlagIsLiteral },
    { 0x04000005, "TestEnum:One",     "One",     "TestEnum",     1, rc::kFieldFlagIsPublic | rc::kFieldFlagIsStatic | rc::kFieldFlagIsLiteral },
};

static constexpr rc::ReflectionQueryTypeDescriptor kTestEnumDesc = {
    0x02000002,
    "TestEnum",
    nullptr,
    "TestNamespace",
    "TestEnum",
    "TestEnum",
    nullptr,
    kTestEnumFields, 3,
    nullptr, 0,
    nullptr, 0,
    nullptr, 0,
    nullptr, 0,
    rc::kFlagIsEnum | rc::kFlagIsSealed | rc::kFlagIsPublic,
};

// --- Type 3: ElementType (inner type for GetElementType tests) ---
static constexpr rc::ReflectionQueryFieldDescriptor kElementTypeFields[] = {
    { 0x04000006, "ElementType:Value", "Value", "System.Int32", 0, rc::kFieldFlagIsPublic },
};

static constexpr rc::ReflectionQueryTypeDescriptor kElementTypeDesc = {
    0x02000003,
    "ElementType",
    nullptr,
    "TestNamespace",
    "ElementType",
    "ElementType",
    nullptr,
    kElementTypeFields, 1,
    nullptr, 0,
    nullptr, 0,
    nullptr, 0,
    nullptr, 0,
    rc::kFlagIsPublic,
};

// --- Type 4: ElementType[] (array type for GetElementType tests) ---
static constexpr rc::ReflectionQueryTypeDescriptor kElementTypeArrayDesc = {
    0x02000004,
    "ElementType[]",
    nullptr,
    "TestNamespace",
    "ElementType[]",
    "ElementType[]",
    nullptr,
    nullptr, 0,
    nullptr, 0,
    nullptr, 0,
    nullptr, 0,
    nullptr, 0,
    rc::kFlagIsArray | rc::kFlagIsPublic,
};

// Descriptor pointer array and image
static constexpr const rc::ReflectionQueryTypeDescriptor* kTypePtrs[] = {
    &kTestTypeDesc,
    &kTestEnumDesc,
    &kElementTypeDesc,
    &kElementTypeArrayDesc,
};

static constexpr rc::ReflectionQueryImageDescriptor kBenchImage = {
    "ReflectionBenchTestModule",
    kTypePtrs,
    4,
};

// ── Tier-0 arrays for Module Registry ─────────────────────────────────

static constexpr uint32_t kTypeFlags[] = {
    rc::kFlagIsPublic,                                          // TestType
    rc::kFlagIsEnum | rc::kFlagIsSealed | rc::kFlagIsPublic,   // TestEnum
    rc::kFlagIsPublic,                                          // ElementType
    rc::kFlagIsArray | rc::kFlagIsPublic,                       // ElementType[]
};

static constexpr const char* kTypeNames[] = {
    "TestType",
    "TestEnum",
    "ElementType",
    "ElementType[]",
};

static constexpr const char* kTypeNamespaces[] = {
    "TestNamespace",
    "TestNamespace",
    "TestNamespace",
    "TestNamespace",
};

// ════════════════════════════════════════════════════════════════════════════
// Test fixture — registers mock module for all benchmarks
// ════════════════════════════════════════════════════════════════════════════

class ReflectionBenchmarkTest : public ::testing::Test {
protected:
    static md::MemoryDomain* s_domain;
    static md::DomainId      s_domain_id;
    static uint32_t          s_module_id;

    // Type handles (ModuleRegistry format) for benchmark use
    // Stored as CHAOS_IL2CPP_INTPTR (intptr_t) to match reflection API signatures.
    static CHAOS_IL2CPP_INTPTR s_type_handle;
    static CHAOS_IL2CPP_INTPTR s_enum_handle;
    static CHAOS_IL2CPP_INTPTR s_array_handle;

    // Encoded ReflectionQuery handles (tag-bit-63) for member query benchmarks
    static CHAOS_IL2CPP_INTPTR s_type_query_handle;

    // Clock for timing
    using Clock = std::chrono::high_resolution_clock;

    static void SetUpTestSuite() {
        md::DomainInit init{};
        init.module_name = "ReflectionBenchDomain";
        init.module_kind = 1;
        init.usage_limit = 0;

        s_domain_id = md::RegisterMemoryDomain(init);
        ASSERT_NE(s_domain_id, md::kDomainIdInvalid);
        s_domain = md::FindDomainById(s_domain_id);
        ASSERT_NE(s_domain, nullptr);

        // Register module with all Tier-0 and Tier-2 data
        rc::ModuleDescriptor mod_desc{};
        mod_desc.name_utf8          = "ReflectionBenchTestModule";
        mod_desc.image              = &kBenchImage;
        mod_desc.type_flags         = kTypeFlags;
        mod_desc.type_names         = kTypeNames;
        mod_desc.type_namespaces    = kTypeNamespaces;
        mod_desc.type_count         = 4;

        // Set up type_info_ptrs so GetTypeInfoFromHandle returns valid (even though minimal)
        // TypeInfoHot pointers. Without this, GetTypeInfoFromReflectionOrGcHandle falls
        // through to reading the handle value as a GC heap pointer, causing an AV.
        static cc::TypeInfoHot g_type_info_hots[4];
        std::memset(g_type_info_hots, 0, sizeof(g_type_info_hots));
        for (int i = 0; i < 4; i++) {
            g_type_info_hots[i].warm_delta = sizeof(cc::TypeInfoHot);
            g_type_info_hots[i].type_shape = cc::chaos_type_shape_reference;
        }
        static const cc::TypeInfoHot* g_type_info_ptrs[4] = {
            &g_type_info_hots[0],
            &g_type_info_hots[1],
            &g_type_info_hots[2],
            &g_type_info_hots[3],
        };
        mod_desc.type_info_ptrs = g_type_info_ptrs;

        s_module_id = rc::RegisterModule("ReflectionBenchTestModule", &mod_desc);
        ASSERT_NE(s_module_id, rc::kInvalidModuleId);

        // Create Module Registry handles — cast via int64_t to avoid narrowing warning
        s_type_handle  = static_cast<CHAOS_IL2CPP_INTPTR>(
            static_cast<int64_t>(rc::MakeTypeHandle(s_module_id, 0x02000001u)));
        s_enum_handle  = static_cast<CHAOS_IL2CPP_INTPTR>(
            static_cast<int64_t>(rc::MakeTypeHandle(s_module_id, 0x02000002u)));
        s_array_handle = static_cast<CHAOS_IL2CPP_INTPTR>(
            static_cast<int64_t>(rc::MakeTypeHandle(s_module_id, 0x02000004u)));

        // Create ReflectionQuery encoded handle (for member API benchmarks)
        s_type_query_handle = static_cast<CHAOS_IL2CPP_INTPTR>(
            rc::EncodeReflectionQueryTypeHandle(&kTestTypeDesc));
    }

    static void TearDownTestSuite() {
        if (s_domain_id != md::kDomainIdInvalid) {
            md::UnregisterMemoryDomain(s_domain_id);
            s_domain_id = md::kDomainIdInvalid;
            s_domain = nullptr;
        }
    }

    /// Run `func` for `iterations` repetitions, return total time in microseconds.
    template <typename F>
    static int64_t BenchTimed(uint32_t iterations, F&& func) {
        // Warm-up
        for (uint32_t i = 0; i < 100; ++i) func();

        auto start = Clock::now();
        for (uint32_t i = 0; i < iterations; ++i) func();
        auto end = Clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    }

    /// Print a single benchmark result.
    static void PrintResult(const char* name, uint32_t iterations, int64_t total_us) {
        double avg_ns = (static_cast<double>(total_us) * 1000.0) / static_cast<double>(iterations);
        std::printf("  %-45s %8d iters  %8lld us total  %8.1f ns/iter\n",
                    name, iterations,
                    static_cast<long long>(total_us), avg_ns);
    }
};

md::MemoryDomain* ReflectionBenchmarkTest::s_domain    = nullptr;
md::DomainId      ReflectionBenchmarkTest::s_domain_id = md::kDomainIdInvalid;
uint32_t          ReflectionBenchmarkTest::s_module_id = rc::kInvalidModuleId;

CHAOS_IL2CPP_INTPTR ReflectionBenchmarkTest::s_type_handle  = 0;
CHAOS_IL2CPP_INTPTR ReflectionBenchmarkTest::s_enum_handle  = 0;
CHAOS_IL2CPP_INTPTR ReflectionBenchmarkTest::s_array_handle = 0;
CHAOS_IL2CPP_INTPTR ReflectionBenchmarkTest::s_type_query_handle = 0;

// ════════════════════════════════════════════════════════════════════════════
// Benchmark 1: Type flag queries — 100,000 iterations each
// ════════════════════════════════════════════════════════════════════════════

TEST_F(ReflectionBenchmarkTest, TypeFlagQueries) {
    std::printf("\n--- Type Flag Queries (100k iterations each) ---\n");
    constexpr uint32_t kIter = 100000;

    // Note: these call the actual CHAOS_DEFINE_TYPE_FLAG_STUB functions
    // which go through ResolveTypeRef() → ModuleRegistry Tier-0 type_flags[]

    {
        auto total = BenchTimed(kIter, [this]() {
            volatile auto r = rc::ChaosReflectionGetIsArray(s_type_handle);
            (void)r;
        });
        PrintResult("ChaosReflectionGetIsArray (class, false)", kIter, total);
    }

    {
        auto total = BenchTimed(kIter, [this]() {
            volatile auto r = rc::ChaosReflectionGetIsArray(s_array_handle);
            (void)r;
        });
        PrintResult("ChaosReflectionGetIsArray (array, true)", kIter, total);
    }

    {
        auto total = BenchTimed(kIter, [this]() {
            volatile auto r = rc::ChaosReflectionGetIsEnum(s_enum_handle);
            (void)r;
        });
        PrintResult("ChaosReflectionGetIsEnum (enum, true)", kIter, total);
    }

    {
        auto total = BenchTimed(kIter, [this]() {
            volatile auto r = rc::ChaosReflectionGetIsEnum(s_type_handle);
            (void)r;
        });
        PrintResult("ChaosReflectionGetIsEnum (class, false)", kIter, total);
    }

    {
        auto total = BenchTimed(kIter, [this]() {
            volatile auto r = rc::ChaosReflectionGetIsPublic(s_type_handle);
            (void)r;
        });
        PrintResult("ChaosReflectionGetIsPublic (true)", kIter, total);
    }

    {
        auto total = BenchTimed(kIter, [this]() {
            volatile auto r = rc::ChaosReflectionGetIsAbstract(s_type_handle);
            (void)r;
        });
        PrintResult("ChaosReflectionGetIsAbstract (false)", kIter, total);
    }

    {
        auto total = BenchTimed(kIter, [this]() {
            volatile auto r = rc::ChaosReflectionGetIsSealed(s_enum_handle);
            (void)r;
        });
        PrintResult("ChaosReflectionGetIsSealed (enum, true)", kIter, total);
    }

    {
        auto total = BenchTimed(kIter, [this]() {
            volatile auto r = rc::ChaosReflectionGetIsValueType(s_enum_handle);
            (void)r;
        });
        PrintResult("ChaosReflectionGetIsValueType (enum = false via flags)", kIter, total);
    }

    {
        auto total = BenchTimed(kIter, [this]() {
            volatile auto r = rc::ChaosReflectionGetIsInterface(s_type_handle);
            (void)r;
        });
        PrintResult("ChaosReflectionGetIsInterface (false)", kIter, total);
    }
}

// ════════════════════════════════════════════════════════════════════════════
// Benchmark 2: GetFields / GetMethods / GetProperties — 10,000 iterations each
// ════════════════════════════════════════════════════════════════════════════

TEST_F(ReflectionBenchmarkTest, GetFieldsBenchmark) {
    std::printf("\n--- GetFields (10k iterations) ---\n");
    constexpr uint32_t kIter = 10000;

    // ModuleRegistry handle
    {
        auto total = BenchTimed(kIter, [this]() {
            volatile auto r = rc::ChaosReflectionGetFields(s_type_handle);
            (void)r;
        });
        PrintResult("ChaosReflectionGetFields (module handle)", kIter, total);
    }
}

TEST_F(ReflectionBenchmarkTest, GetPropertiesBenchmark) {
    std::printf("\n--- GetProperties (10k iterations) ---\n");
    constexpr uint32_t kIter = 10000;

    {
        auto total = BenchTimed(kIter, [this]() {
            volatile auto r = rc::ChaosReflectionGetProperties(s_type_handle);
            (void)r;
        });
        PrintResult("ChaosReflectionGetProperties (module handle)", kIter, total);
    }
}

TEST_F(ReflectionBenchmarkTest, GetMethodsBenchmark) {
    std::printf("\n--- GetMethods (10k iterations) ---\n");
    constexpr uint32_t kIter = 10000;

    {
        auto total = BenchTimed(kIter, [this]() {
            volatile auto r = rc::ChaosReflectionGetMethods(s_type_handle);
            (void)r;
        });
        PrintResult("ChaosReflectionGetMethods (module handle)", kIter, total);
    }
}

// ════════════════════════════════════════════════════════════════════════════
// Benchmark 3: GetElementType — 10,000 iterations
// ════════════════════════════════════════════════════════════════════════════

TEST_F(ReflectionBenchmarkTest, GetElementType) {
    std::printf("\n--- GetElementType (10k iterations) ---\n");
    constexpr uint32_t kIter = 10000;

    // ModuleRegistry handle for the array type "ElementType[]"
    {
        auto total = BenchTimed(kIter, [this]() {
            volatile auto r = rc::ChaosTypeGetElementType(
                (s_array_handle));
            (void)r;
        });
        PrintResult("ChaosTypeGetElementType (array -> ElementType)", kIter, total);
    }

    // ModuleRegistry handle for a non-array type (returns 0)
    {
        auto total = BenchTimed(kIter, [this]() {
            volatile auto r = rc::ChaosTypeGetElementType(
                (s_type_handle));
            (void)r;
        });
        PrintResult("ChaosTypeGetElementType (class, returns 0)", kIter, total);
    }
}

// ════════════════════════════════════════════════════════════════════════════
// Benchmark 4: GetTypeFromHandle — 100,000 iterations
// ════════════════════════════════════════════════════════════════════════════

TEST_F(ReflectionBenchmarkTest, GetTypeFromHandle) {
    std::printf("\n--- Type.GetTypeFromHandle (100k iterations) ---\n");
    constexpr uint32_t kIter = 100000;

    // Direct ReflectionQuery encoded handle (tag-bit-63)
    // ChaosReflectionGetTypeFromHandle resolves via TypeInfoHandle → descriptor
    {
        auto total = BenchTimed(kIter, [this]() {
            volatile auto r = rc::ChaosReflectionGetTypeFromHandle(
                (s_type_query_handle));
            (void)r;
        });
        PrintResult("ChaosReflectionGetTypeFromHandle (query handle)", kIter, total);
    }

    // ModuleRegistry handle — goes through module lookup
    {
        auto total = BenchTimed(kIter, [this]() {
            volatile auto r = rc::ChaosReflectionGetTypeFromHandle(
                (s_type_handle));
            (void)r;
        });
        PrintResult("ChaosReflectionGetTypeFromHandle (module handle)", kIter, total);
    }

    // Raw metadata token via aot_metadata::FindTypeByMetadataToken
    {
        auto total = BenchTimed(kIter, []() {
            volatile auto r = rc::ChaosReflectionGetTypeFromHandle(0x02000001);
            (void)r;
        });
        PrintResult("ChaosReflectionGetTypeFromHandle (raw token)", kIter, total);
    }
}
