// convert_benchmark_test.cpp — Microbenchmarks for convert + parse_convert hot paths
//
// Measures throughput for:
//   - chaos_convert_tochar_int32/int64 (range-checked narrowing cast)
//   - ChaosFormatInt32 (fast itoa, returns StringId)
//   - ChaosConvertToInt32 (strtol-based string parse)
//   - ChaosFormatDouble (fmt::format Dragonbox)
//
// Labeled "benchmark" (excluded from default ctest).

#include <gtest/gtest.h>

#include <convert.h>
#include <parse_convert.h>
#include <codegen_bridge.h>
#include <string_table.h>

#include <cstdint>
#include <chrono>
#include <cstring>

// ── AOT codegen stubs ─────────────────────────────────────────────────────
struct HotpatchModuleV0;
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module = nullptr;
extern "C" const char* const kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int32_t kChaosExternalRuntimeCount = 0;

namespace st = chaos::il2cpp::string_table;

using Clock = std::chrono::high_resolution_clock;
static constexpr int kIterations = 50000;

// ═══════════════════════════════════════════════════════════════════════════
// chaos_convert_tochar_int32 — range-checked cast
// ═══════════════════════════════════════════════════════════════════════════

TEST(convert_benchmark, ToCharInt32_Throughput) {
    CHAOS_IL2CPP_INTPTR sum = 0;
    auto start = Clock::now();
    for (int i = 0; i < kIterations; i++) {
        sum += chaos_convert_tochar_int32(i & 0xFFFF);
    }
    auto elapsed = Clock::now() - start;
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count();
    EXPECT_GT(sum, 0u);  // prevent optimizer elimination
    EXPECT_LT(ns, 50000000LL);  // < 50ms for 50k calls (should be <1ms)
    (void)ns;
}

// ═══════════════════════════════════════════════════════════════════════════
// chaos_convert_tochar_int64 — range-checked cast (64-bit)
// ═══════════════════════════════════════════════════════════════════════════

TEST(convert_benchmark, ToCharInt64_Throughput) {
    CHAOS_IL2CPP_INTPTR sum = 0;
    auto start = Clock::now();
    for (int i = 0; i < kIterations; i++) {
        sum += chaos_convert_tochar_int64(i & 0xFFFF);
    }
    auto elapsed = Clock::now() - start;
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count();
    EXPECT_GT(sum, 0u);
    EXPECT_LT(ns, 50000000LL);
    (void)ns;
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosFormatInt32 — fast itoa (returns StringId)
// ═══════════════════════════════════════════════════════════════════════════

class FormatBenchmark : public ::testing::Test {
protected:
    void SetUp() override {
        // Register strings so string_table has internal capacity allocated
        st::Register("0", 1, 0);
        st::Register("12345", 5, 0);
        st::Register("-42", 3, 0);
    }
};

TEST_F(FormatBenchmark, FormatInt32_Throughput) {
    CHAOS_IL2CPP_INTPTR sum = 0;
    auto start = Clock::now();
    for (int i = 0; i < kIterations; i++) {
        auto encoded = ChaosFormatInt32(i - (kIterations / 2));
        sum += chaos_is_string_id(encoded) ? 1 : 0;
    }
    auto elapsed = Clock::now() - start;
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count();
    EXPECT_GT(sum, 0u);
    EXPECT_LT(ns, 100000000LL);  // < 100ms for 50k format calls
    (void)ns;
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosFormatDouble — fmt::format with Dragonbox
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(FormatBenchmark, FormatDouble_Throughput) {
    CHAOS_IL2CPP_INTPTR sum = 0;
    double val = 3.141592653589793;
    auto start = Clock::now();
    for (int i = 0; i < kIterations; i++) {
        auto encoded = ChaosFormatDouble(val);
        sum += chaos_is_string_id(encoded) ? 1 : 0;
        val += 0.001;
    }
    auto elapsed = Clock::now() - start;
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count();
    EXPECT_GT(sum, 0u);
    EXPECT_LT(ns, 300000000LL);  // < 300ms for 50k Dragonbox calls
    (void)ns;
}
