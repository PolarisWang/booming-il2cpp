// blittable_benchmark -- native microbenchmark for blittable struct marshalling throughput
// Measures StructureToPtr/PtrToStructure ops/sec for a large blittable struct (64 bytes)
// Compares with and without the blittable fast-path (memcpy vs field-by-field dispatch)

#include <gtest/gtest.h>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <atomic>
#include <thread>
#include <vector>

// ── Stubs for symbols normally emitted by AOT codegen ─────────────────────
extern "C" const void* chaos_il2cpp_aot_hotpatch_module = nullptr;
extern "C" const char* const kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int32_t kChaosExternalRuntimeCount = 0;

using Clock = std::chrono::high_resolution_clock;

// ── Simulated struct layouts ────────────────────────────────────────────
// BlittableStruct: 64 bytes (16 x int32 fields) -- pure memcpy
struct alignas(8) BlittableStruct {
    int32_t f0, f1, f2, f3, f4, f5, f6, f7;
    int32_t f8, f9, f10, f11, f12, f13, f14, f15;
};

static_assert(sizeof(BlittableStruct) == 64, "BlittableStruct must be 64 bytes");

// ── Simulated marshalling descriptor ────────────────────────────────────
// Mirrors the real StructMarshallingDescriptorV1 layout to benchmark
// the dispatching overhead separately from the actual field work.

enum class StructFieldKind : uint16_t {
    Blittable = 0,
    BoolField = 3,
    StringField = 4,
    NestedStruct = 5,
    ByValArray = 6,
};

struct FieldDescriptor {
    StructFieldKind kind;
    uint16_t offset;
    uint16_t size;
};

struct StructDescriptor {
    uint16_t field_count;
    uint16_t total_size;
    FieldDescriptor fields[16];
};

// ── Fast path: memcpy only (blittable path) ─────────────────────────────
static void MarshalStructManagedToNative_Fast(
    const StructDescriptor* desc,
    unsigned char* native_ptr,
    const unsigned char* managed_ptr) {
    std::memcpy(native_ptr, managed_ptr, desc->total_size);
}

static void MarshalStructNativeToManaged_Fast(
    const StructDescriptor* desc,
    unsigned char* managed_ptr,
    const unsigned char* native_ptr) {
    std::memcpy(managed_ptr, native_ptr, desc->total_size);
}

// ── Slow path: field-by-field dispatch (non-blittable simulation) ──────
static void MarshalFieldManagedToNative_Slow(
    const FieldDescriptor& field,
    unsigned char* native_ptr,
    const unsigned char* managed_ptr) {
    switch (field.kind) {
    case StructFieldKind::Blittable:
        std::memcpy(native_ptr + field.offset, managed_ptr + field.offset, field.size);
        break;
    case StructFieldKind::BoolField: {
        const uint8_t val = *reinterpret_cast<const uint8_t*>(managed_ptr + field.offset);
        *reinterpret_cast<uint32_t*>(native_ptr + field.offset) = (val != 0) ? 1u : 0u;
        break;
    }
    case StructFieldKind::StringField: {
        // Simulate string marshalling: write a pointer (0 = null)
        const void* managed_str = *reinterpret_cast<const void* const*>(managed_ptr + field.offset);
        *reinterpret_cast<const void**>(native_ptr + field.offset) = managed_str;
        break;
    }
    case StructFieldKind::NestedStruct:
    case StructFieldKind::ByValArray:
        std::memcpy(native_ptr + field.offset, managed_ptr + field.offset, field.size);
        break;
    }
}

static void MarshalStructManagedToNative_Slow(
    const StructDescriptor* desc,
    unsigned char* native_ptr,
    const unsigned char* managed_ptr) {
    for (uint16_t i = 0; i < desc->field_count; ++i) {
        MarshalFieldManagedToNative_Slow(desc->fields[i], native_ptr, managed_ptr);
    }
}

static void MarshalFieldNativeToManaged_Slow(
    const FieldDescriptor& field,
    unsigned char* managed_ptr,
    const unsigned char* native_ptr) {
    switch (field.kind) {
    case StructFieldKind::Blittable:
        std::memcpy(managed_ptr + field.offset, native_ptr + field.offset, field.size);
        break;
    case StructFieldKind::BoolField: {
        const uint32_t val = *reinterpret_cast<const uint32_t*>(native_ptr + field.offset);
        *reinterpret_cast<uint8_t*>(managed_ptr + field.offset) = (val != 0) ? 1u : 0u;
        break;
    }
    case StructFieldKind::StringField: {
        const void* native_buf = *reinterpret_cast<const void* const*>(native_ptr + field.offset);
        *reinterpret_cast<const void**>(managed_ptr + field.offset) = native_buf;
        break;
    }
    case StructFieldKind::NestedStruct:
    case StructFieldKind::ByValArray:
        std::memcpy(managed_ptr + field.offset, native_ptr + field.offset, field.size);
        break;
    }
}

static void MarshalStructNativeToManaged_Slow(
    const StructDescriptor* desc,
    unsigned char* managed_ptr,
    const unsigned char* native_ptr) {
    for (uint16_t i = 0; i < desc->field_count; ++i) {
        MarshalFieldNativeToManaged_Slow(desc->fields[i], managed_ptr, native_ptr);
    }
}

// ── Benchmark runners ──────────────────────────────────────────────────

struct BenchmarkResult {
    const char* name;
    double mean_ms;
    int iterations;
    int struct_size;
};

static constexpr int kSamples = 5;
static constexpr int kWarmup = 1000;

static double run_managed_to_native_bench(
    const StructDescriptor* desc,
    int iterations,
    bool use_fast_path) {
    // Allocate managed and native buffers
    BlittableStruct managed_src;
    auto* native_dst = static_cast<unsigned char*>(std::malloc(desc->total_size));

    // Fill with deterministic data
    for (int i = 0; i < 16; ++i) {
        (&managed_src.f0)[i] = i * 0x12345678;
    }
    uint64_t checksum = 0;

    // Warmup
    for (int i = 0; i < kWarmup; ++i) {
        if (use_fast_path) {
            MarshalStructManagedToNative_Fast(desc, native_dst,
                reinterpret_cast<const unsigned char*>(&managed_src));
        } else {
            MarshalStructManagedToNative_Slow(desc, native_dst,
                reinterpret_cast<const unsigned char*>(&managed_src));
        }
        checksum += native_dst[i % desc->total_size];
    }

    double total_ms = 0;
    for (int s = 0; s < kSamples; ++s) {
        auto t0 = Clock::now();
        for (int i = 0; i < iterations; ++i) {
            if (use_fast_path) {
                MarshalStructManagedToNative_Fast(desc, native_dst,
                    reinterpret_cast<const unsigned char*>(&managed_src));
            } else {
                MarshalStructManagedToNative_Slow(desc, native_dst,
                    reinterpret_cast<const unsigned char*>(&managed_src));
            }
            checksum += native_dst[i % desc->total_size];
        }
        auto t1 = Clock::now();
        total_ms += std::chrono::duration<double, std::milli>(t1 - t0).count();
    }

    std::free(native_dst);
    (void)checksum;
    return total_ms / kSamples;
}

static double run_native_to_managed_bench(
    const StructDescriptor* desc,
    int iterations,
    bool use_fast_path) {
    BlittableStruct managed_dst;
    auto* native_src = static_cast<unsigned char*>(std::malloc(desc->total_size));
    std::memset(native_src, 0xAB, desc->total_size);
    uint64_t checksum = 0;

    // Warmup
    for (int i = 0; i < kWarmup; ++i) {
        if (use_fast_path) {
            MarshalStructNativeToManaged_Fast(desc,
                reinterpret_cast<unsigned char*>(&managed_dst), native_src);
        } else {
            MarshalStructNativeToManaged_Slow(desc,
                reinterpret_cast<unsigned char*>(&managed_dst), native_src);
        }
        checksum += managed_dst.f0;
    }

    double total_ms = 0;
    for (int s = 0; s < kSamples; ++s) {
        auto t0 = Clock::now();
        for (int i = 0; i < iterations; ++i) {
            if (use_fast_path) {
                MarshalStructNativeToManaged_Fast(desc,
                    reinterpret_cast<unsigned char*>(&managed_dst), native_src);
            } else {
                MarshalStructNativeToManaged_Slow(desc,
                    reinterpret_cast<unsigned char*>(&managed_dst), native_src);
            }
            checksum += managed_dst.f0;
        }
        auto t1 = Clock::now();
        total_ms += std::chrono::duration<double, std::milli>(t1 - t0).count();
    }

    std::free(native_src);
    (void)checksum;
    return total_ms / kSamples;
}

// ── Build descriptors ──────────────────────────────────────────────────

static StructDescriptor MakeBlittableDescriptor() {
    StructDescriptor desc;
    desc.field_count = 16;
    desc.total_size = 64;
    for (uint16_t i = 0; i < 16; ++i) {
        desc.fields[i].kind = StructFieldKind::Blittable;
        desc.fields[i].offset = static_cast<uint16_t>(i * 4);
        desc.fields[i].size = 4;
    }
    return desc;
}

static StructDescriptor MakeSlowDescriptor() {
    // Mixed field types to force full dispatch: 12 blittable + 2 bool + 2 string
    StructDescriptor desc;
    desc.field_count = 16;
    desc.total_size = 64;
    for (uint16_t i = 0; i < 12; ++i) {
        desc.fields[i].kind = StructFieldKind::Blittable;
        desc.fields[i].offset = static_cast<uint16_t>(i * 4);
        desc.fields[i].size = 4;
    }
    // Bool at offset 48, 52
    desc.fields[12].kind = StructFieldKind::BoolField;
    desc.fields[12].offset = 48;
    desc.fields[12].size = 4;
    desc.fields[13].kind = StructFieldKind::BoolField;
    desc.fields[13].offset = 52;
    desc.fields[13].size = 4;
    // String at offset 56, 60 (just pointer slots)
    desc.fields[14].kind = StructFieldKind::StringField;
    desc.fields[14].offset = 56;
    desc.fields[14].size = 8;
    desc.fields[15].kind = StructFieldKind::StringField;
    desc.fields[15].offset = 56;
    desc.fields[15].size = 8;
    return desc;
}

// ── DeepInlineCallees stub ────────────────────────────────────────────
namespace chaos::il2cpp::interpreter { struct IRMethod; }
namespace chaos::il2cpp::runtime_core { struct PatchMethod; }
namespace chaos::il2cpp::runtime_core {
bool DeepInlineCallees(
    interpreter::IRMethod&,
    PatchMethod&,
    uint32_t,
    uint32_t) noexcept { return false; }
}

// ════════════════════════════════════════════════════════════════════════════
// Benchmark Test
// ════════════════════════════════════════════════════════════════════════════

TEST(BlittableBenchmark, Run)
{
    auto blittable_desc = MakeBlittableDescriptor();
    auto slow_desc = MakeSlowDescriptor();

    struct {
        const char* name;
        double mean_ms;
        int iterations;
    } results[6];
    int ri = 0;

    constexpr int kIter = 100000;

    // 1) ManagedToNative fast path (pure memcpy)
    results[ri].name       = "M->N blittable fast-path";
    results[ri].mean_ms    = run_managed_to_native_bench(&blittable_desc, kIter, true);
    results[ri].iterations = kIter;
    ri++;

    // 2) ManagedToNative slow path (field-by-field, but all blittable fields)
    results[ri].name       = "M->N all-blittable slow-path";
    results[ri].mean_ms    = run_managed_to_native_bench(&blittable_desc, kIter, false);
    results[ri].iterations = kIter;
    ri++;

    // 3) ManagedToNative mixed path (blittable + bool + string dispatch)
    results[ri].name       = "M->N mixed-field slow-path";
    results[ri].mean_ms    = run_managed_to_native_bench(&slow_desc, kIter, false);
    results[ri].iterations = kIter;
    ri++;

    // 4) NativeToManaged fast path (pure memcpy)
    results[ri].name       = "N->M blittable fast-path";
    results[ri].mean_ms    = run_native_to_managed_bench(&blittable_desc, kIter, true);
    results[ri].iterations = kIter;
    ri++;

    // 5) NativeToManaged slow path (field-by-field, all blittable fields)
    results[ri].name       = "N->M all-blittable slow-path";
    results[ri].mean_ms    = run_native_to_managed_bench(&blittable_desc, kIter, false);
    results[ri].iterations = kIter;
    ri++;

    // 6) NativeToManaged mixed field slow path
    results[ri].name       = "N->M mixed-field slow-path";
    results[ri].mean_ms    = run_native_to_managed_bench(&slow_desc, kIter, false);
    results[ri].iterations = kIter;
    ri++;

    // ── Print results ──
    std::printf("\n  %-30s  %8s  %10s  %13s  %13s\n",
                "Benchmark", "iters", "mean_ms", "ns/op", "ops/sec");
    std::printf("  %-30s  %8s  %10s  %13s  %13s\n",
                "------------------------------",
                "--------", "----------", "-------------", "-------------");

    for (int i = 0; i < ri; ++i) {
        double ns_per_op = (results[i].mean_ms * 1e6) / results[i].iterations;
        double ops_per_sec = (results[i].iterations / results[i].mean_ms) * 1e3;
        std::printf("  %-30s  %7d  %10.3f  %11.1f  %13.0f\n",
                    results[i].name, results[i].iterations,
                    results[i].mean_ms, ns_per_op, ops_per_sec);
    }

    // ── Speedup analysis ──
    std::printf("\n\n  SPEEDUP ANALYSIS\n");

    double fast_mn_ns = (results[0].mean_ms * 1e6) / results[0].iterations;
    double slow_mn_ns = (results[1].mean_ms * 1e6) / results[1].iterations;
    double mixed_mn_ns = (results[2].mean_ms * 1e6) / results[2].iterations;
    double fast_nm_ns = (results[3].mean_ms * 1e6) / results[3].iterations;
    double slow_nm_ns = (results[4].mean_ms * 1e6) / results[4].iterations;
    double mixed_nm_ns = (results[5].mean_ms * 1e6) / results[5].iterations;

    double fast_mn_ops = 1e9 / fast_mn_ns;
    double slow_mn_ops = 1e9 / slow_mn_ns;
    double mixed_mn_ops = 1e9 / mixed_mn_ns;
    double fast_nm_ops = 1e9 / fast_nm_ns;
    double slow_nm_ops = 1e9 / slow_nm_ns;
    double mixed_nm_ops = 1e9 / mixed_nm_ns;

    (void)slow_nm_ops;
    (void)mixed_nm_ops;

    // ── Minimum throughput assertions ──
    // These thresholds are conservative and should pass even on slow CI:
    //   - Fast path (pure memcpy 64 bytes): > 10 million ops/sec
    //   - Slow path (field-by-field, all blittable): > 2 million ops/sec
    //   - Mixed path (blittable + bool + string): > 1 million ops/sec
    EXPECT_GT(fast_mn_ops, 10.0e6) << "M->N fast path too slow";
    EXPECT_GT(slow_mn_ops, 2.0e6)  << "M->N slow path too slow";
    EXPECT_GT(mixed_mn_ops, 1.0e6) << "M->N mixed path too slow";
    EXPECT_GT(fast_nm_ops, 10.0e6) << "N->M fast path too slow";

    // Speedup sanity: fast path must be strictly faster than slow path.
    EXPECT_GT(slow_mn_ns / fast_mn_ns, 1.0)
        << "fast path should be faster than slow path (M->N)";
    EXPECT_GT(slow_nm_ns / fast_nm_ns, 1.0)
        << "fast path should be faster than slow path (N->M)";

    std::printf("\n  Interpretation:\n");
    std::printf("    Fast speedup (all-blittable struct): %.1fx -- "
                "pure memcpy vs field dispatch overhead\n",
                slow_mn_ns / fast_mn_ns);
    std::printf("    Mixed slowdown (blittable+bool+string): %.1fx -- "
                "dispatch + conversion overhead\n",
                mixed_mn_ns / fast_mn_ns);
    std::printf("    Blittable fast-path eliminates: branch mispredictions"
                " from switch(field.kind)\n");
    std::printf("    + per-field bounds checks + indirect call through"
                " field dispatch.\n");

    std::printf("\ndone.\n");
}
