/// VTable lookup performance benchmarks.
///
/// Measures three key lookup paths in the vtable_registry:
///   1. TryGetTypeVTable  — type_token → TypeVTable* (shared_lock + hash lookup)
///   2. ResolveVirtualMethodPointer — inheritance chain walk + TCVC
///   3. chaos_find_interface_offset — iface_map scan + IOC
///
/// Uses std::chrono::high_resolution_clock for wall-clock measurement.
/// Outputs ns/op for each benchmarked path.
///
/// Build: cmake --build build/testing --target vtable_lookup_benchmark --config Release
/// Run:   build/testing/runtime-core/Release/vtable_lookup_benchmark.exe

#include <gtest/gtest.h>
#include <chaos/native_types.h>
#include <chaos/type_info.h>

#include <vtable_registry.h>
#include <codegen_bridge.h>

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>

using namespace chaos::il2cpp::common;
using namespace chaos::il2cpp::vtable_registry;

// ── Stubs for AOT codegen symbols ─────────────────────────────────────────
namespace chaos { namespace il2cpp { namespace runtime_core {
extern "C" { void* chaos_il2cpp_aot_hotpatch_module = nullptr; }
extern "C" { const void* kChaosExternalRuntimeSubjects = nullptr; }
extern "C" { const void* kChaosExternalRuntimeFnTable = nullptr; }
extern "C" { int32_t kChaosExternalRuntimeCount = 0; }
}}}

// ════════════════════════════════════════════════════════════════════════════
// Constants
// ════════════════════════════════════════════════════════════════════════════

static constexpr uint32_t kTokenBase    = 0xF100u;
static constexpr uint32_t kTokenMid     = 0xF101u;
static constexpr uint32_t kTokenLeaf    = 0xF102u;
static constexpr uint32_t kTokenIface   = 0xF110u;
static constexpr uint32_t kTokenImpl    = 0xF111u;

static constexpr uint64_t kStableIface  = 0xF100000000000010ULL;

static constexpr uint32_t kMethodVirtual = 0x1000u;
static constexpr uint32_t kMethodOwn     = 0x1001u;

static constexpr int kNumIterations = 100000;

using Clock = std::chrono::high_resolution_clock;

// ── Suite-level vtable storage ───────────────────────────────────────────

static VTableSlot g_base_slots[1];
static VTableSlot g_mid_slots[2];
static VTableSlot g_leaf_slots[1];
static VTableSlot g_iface_impl_slots[2];
static InterfaceMapEntry g_iface_entries[1];
static void* g_iface_impl_flat[4];

static TypeVTable g_base_vtable{};
static TypeVTable g_mid_vtable{};
static TypeVTable g_leaf_vtable{};
static TypeVTable g_iface_impl_vtable{};

// ════════════════════════════════════════════════════════════════════════════
// Fixture
// ════════════════════════════════════════════════════════════════════════════

class VTableLookupBenchmark : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        // Base: single virtual method
        g_base_slots[0] = VTableSlot{ kMethodVirtual, reinterpret_cast<void*>(0x1001) };
        g_base_vtable = TypeVTable{};
        g_base_vtable.type_token  = kTokenBase;
        g_base_vtable.base_token  = 0;
        g_base_vtable.slot_count  = 1;
        g_base_vtable.slots       = g_base_slots;
        g_base_vtable.type_shape  = 1;
        RegisterTypeVTable(&g_base_vtable);

        // Mid: overrides virtual, adds own
        g_mid_slots[0] = VTableSlot{ kMethodVirtual, reinterpret_cast<void*>(0x2001) };
        g_mid_slots[1] = VTableSlot{ kMethodOwn, reinterpret_cast<void*>(0x2002) };
        g_mid_vtable = TypeVTable{};
        g_mid_vtable.type_token  = kTokenMid;
        g_mid_vtable.base_token  = kTokenBase;
        g_mid_vtable.slot_count  = 2;
        g_mid_vtable.slots       = g_mid_slots;
        g_mid_vtable.type_shape  = 1;
        RegisterTypeVTable(&g_mid_vtable);

        // Leaf: inherits from mid
        g_leaf_slots[0] = VTableSlot{ kMethodVirtual, reinterpret_cast<void*>(0x3001) };
        g_leaf_vtable = TypeVTable{};
        g_leaf_vtable.type_token  = kTokenLeaf;
        g_leaf_vtable.base_token  = kTokenMid;
        g_leaf_vtable.slot_count  = 1;
        g_leaf_vtable.slots       = g_leaf_slots;
        g_leaf_vtable.type_shape  = 1;
        RegisterTypeVTable(&g_leaf_vtable);

        // Interface-implementing type (for interface offset benchmark)
        g_iface_entries[0] = InterfaceMapEntry{ kStableIface, 2, 2 };
        g_iface_impl_slots[0] = VTableSlot{ kMethodVirtual, reinterpret_cast<void*>(0x5001) };
        g_iface_impl_slots[1] = VTableSlot{ kMethodOwn, reinterpret_cast<void*>(0x5002) };
        g_iface_impl_flat[0] = reinterpret_cast<void*>(0x5001);
        g_iface_impl_flat[1] = reinterpret_cast<void*>(0x5002);
        g_iface_impl_flat[2] = reinterpret_cast<void*>(0x5003);
        g_iface_impl_flat[3] = reinterpret_cast<void*>(0x5004);
        g_iface_impl_vtable = TypeVTable{};
        g_iface_impl_vtable.type_token    = kTokenImpl;
        g_iface_impl_vtable.slot_count    = 2;
        g_iface_impl_vtable.slots         = g_iface_impl_slots;
        g_iface_impl_vtable.vtable_array  = const_cast<const void**>(g_iface_impl_flat);
        g_iface_impl_vtable.vtable_length = 4;
        g_iface_impl_vtable.iface_map     = g_iface_entries;
        g_iface_impl_vtable.iface_count   = 1;
        g_iface_impl_vtable.type_shape    = 1;
        RegisterTypeVTable(&g_iface_impl_vtable);
    }
};

// ════════════════════════════════════════════════════════════════════════════
// Benchmark helpers
// ════════════════════════════════════════════════════════════════════════════

/// Run @p func @p iterations times, return ns/op.
static double Benchmark(const char* name, int iterations, void(*func)()) {
    // Warmup
    for (int i = 0; i < 1000; i++) func();

    auto start = Clock::now();
    for (int i = 0; i < iterations; i++) func();
    auto end = Clock::now();

    double ns = std::chrono::duration<double, std::nano>(end - start).count();
    double ns_per_op = ns / iterations;
    std::printf("  %-45s %8.1f ns/op  (%d iterations)\n", name, ns_per_op, iterations);
    return ns_per_op;
}

// ════════════════════════════════════════════════════════════════════════════
// Benchmark: TryGetTypeVTable
// ════════════════════════════════════════════════════════════════════════════

TEST_F(VTableLookupBenchmark, TryGetTypeVTable)
{
    std::printf("\n--- TryGetTypeVTable lookup ---\n");

    Benchmark("TryGetTypeVTable (direct hit)", kNumIterations, []() {
        volatile const auto* vt = TryGetTypeVTable(kTokenLeaf);
        (void)vt;
    });

    Benchmark("TryGetTypeVTable (miss)", kNumIterations, []() {
        volatile const auto* vt = TryGetTypeVTable(0xDEAD);
        (void)vt;
    });
}

// ════════════════════════════════════════════════════════════════════════════
// Benchmark: ResolveVirtualMethodPointer
// ════════════════════════════════════════════════════════════════════════════

TEST_F(VTableLookupBenchmark, ResolveVirtualMethodPointer)
{
    std::printf("\n--- ResolveVirtualMethodPointer ---\n");

    // Direct hit — type has the method itself
    Benchmark("Resolve (direct, base type)", kNumIterations, []() {
        volatile void* p = ResolveVirtualMethodPointer(kTokenBase, kMethodVirtual);
        (void)p;
    });

    // Chain walk depth 1 — method in base type
    Benchmark("Resolve (chain depth 1, mid→base)", kNumIterations, []() {
        volatile void* p = ResolveVirtualMethodPointer(kTokenMid, kMethodVirtual);
        (void)p;
    });

    // Chain walk depth 2 — method in grandparent (not in leaf or mid)
    Benchmark("Resolve (chain depth 2, leaf→mid→base)", kNumIterations, []() {
        volatile void* p = ResolveVirtualMethodPointer(kTokenLeaf, kMethodVirtual);
        (void)p;
    });

    // Own method (leaf has no kMethodOwn, resolves in mid)
    Benchmark("Resolve (own slot on mid)", kNumIterations, []() {
        volatile void* p = ResolveVirtualMethodPointer(kTokenMid, kMethodOwn);
        (void)p;
    });

    // Miss
    Benchmark("Resolve (miss)", kNumIterations, []() {
        volatile void* p = ResolveVirtualMethodPointer(kTokenBase, 0xDEAD);
        (void)p;
    });
}

// ════════════════════════════════════════════════════════════════════════════
// Benchmark: chaos_find_interface_offset
// ════════════════════════════════════════════════════════════════════════════

TEST_F(VTableLookupBenchmark, FindInterfaceOffset)
{
    std::printf("\n--- chaos_find_interface_offset ---\n");

    // AOT iface_map hit
    Benchmark("FindIfaceOffset (AOT hit)", kNumIterations, []() {
        volatile uint32_t off = chaos_find_interface_offset(kTokenImpl, kStableIface);
        (void)off;
    });

    // Miss
    Benchmark("FindIfaceOffset (miss)", kNumIterations, []() {
        volatile uint32_t off = chaos_find_interface_offset(kTokenImpl, 0xDEAD);
        (void)off;
    });
}
