/// Concurrent/stress tests for vtable_registry.
///
/// Covers:
///   1. Multi-threaded TCVC concurrent read (ResolveVirtualMethodPointer)
///   2. 128+ VTable scalability
///   3. Read-write epoch contention (readers + UpdateVTableSlotByMethodToken)
///   4. Concurrent interface offset lookup (chaos_find_interface_offset)
///
/// Build: cmake --build build/testing --target vtable_concurrent_stress_test --config Release
/// Run:   build/testing/runtime-core/Release/vtable_concurrent_stress_test.exe
///
/// LABELS "stress;core" — excluded from default test run.

#include <gtest/gtest.h>
#include <chaos/native_types.h>
#include <chaos/type_info.h>

#include <vtable_registry.h>
#include <codegen_bridge.h>

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <future>
#include <thread>
#include <vector>

using namespace chaos::il2cpp::common;
using namespace chaos::il2cpp::vtable_registry;

// ── Stubs for AOT codegen symbols ────────────────────────────────────
namespace chaos { namespace il2cpp { namespace runtime_core {
extern "C" { void* chaos_il2cpp_aot_hotpatch_module = nullptr; }
extern "C" { const void* kChaosExternalRuntimeSubjects = nullptr; }
extern "C" { const void* kChaosExternalRuntimeFnTable = nullptr; }
extern "C" { int32_t kChaosExternalRuntimeCount = 0; }
}}}

// ── Constants ────────────────────────────────────────────────────────

static constexpr uint32_t kTokenBase    = 0xF200u;
static constexpr uint32_t kTokenMid     = 0xF201u;
static constexpr uint32_t kTokenLeaf    = 0xF202u;
static constexpr uint32_t kTokenIface   = 0xF210u;
static constexpr uint32_t kTokenImpl    = 0xF211u;
static constexpr uint64_t kStableIface  = 0xF200000000000010ULL;
static constexpr uint32_t kMethodFoo    = 0x2000u;
static constexpr uint32_t kMethodBar    = 0x2001u;
static constexpr uint32_t kMethodBaz    = 0x2002u;

static void* kPtrA = reinterpret_cast<void*>(0xA000);
static void* kPtrB = reinterpret_cast<void*>(0xB000);
static void* kPtrC = reinterpret_cast<void*>(0xC000);

static constexpr int kNumReaderThreads    = 4;
static constexpr int kIterationsPerThread = 10000;
static constexpr int kContentionDurationMs = 2000;
static constexpr int kScalabilityCount    = 150;

// ── Suite-level storage ──────────────────────────────────────────────

// Inheritance chain: Base → Mid → Leaf
static VTableSlot g_base_slots[1];
static VTableSlot g_mid_slots[2];
static VTableSlot g_leaf_slots[1];
static TypeVTable g_base_vt{};
static TypeVTable g_mid_vt{};
static TypeVTable g_leaf_vt{};

// Interface-implementing type
static InterfaceMapEntry g_iface_entry[1];
static VTableSlot g_iface_slots[2];
static TypeVTable g_iface_vt{};

// Scalability: 150 independent types
static TypeVTable  g_scalability_vt[kScalabilityCount];
static VTableSlot  g_scalability_slots[kScalabilityCount];

// ══════════════════════════════════════════════════════════════════════
// Fixture
// ══════════════════════════════════════════════════════════════════════

class VTableConcurrentStressTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        // Inheritance chain: Base
        g_base_slots[0] = VTableSlot{ kMethodFoo, kPtrA };
        g_base_vt = TypeVTable{};
        g_base_vt.type_token  = kTokenBase;
        g_base_vt.base_token  = 0;
        g_base_vt.slot_count  = 1;
        g_base_vt.slots       = g_base_slots;
        g_base_vt.type_shape  = 1;
        RegisterTypeVTable(&g_base_vt);

        // Mid (inherits from Base, overrides kMethodFoo, adds kMethodBar)
        g_mid_slots[0] = VTableSlot{ kMethodFoo, kPtrB };
        g_mid_slots[1] = VTableSlot{ kMethodBar, kPtrC };
        g_mid_vt = TypeVTable{};
        g_mid_vt.type_token  = kTokenMid;
        g_mid_vt.base_token  = kTokenBase;
        g_mid_vt.slot_count  = 2;
        g_mid_vt.slots       = g_mid_slots;
        g_mid_vt.type_shape  = 1;
        RegisterTypeVTable(&g_mid_vt);

        // Leaf (inherits from Mid)
        g_leaf_slots[0] = VTableSlot{ kMethodFoo, reinterpret_cast<void*>(0xD000) };
        g_leaf_vt = TypeVTable{};
        g_leaf_vt.type_token  = kTokenLeaf;
        g_leaf_vt.base_token  = kTokenMid;
        g_leaf_vt.slot_count  = 1;
        g_leaf_vt.slots       = g_leaf_slots;
        g_leaf_vt.type_shape  = 1;
        RegisterTypeVTable(&g_leaf_vt);

        // Interface-implementing type
        g_iface_entry[0] = InterfaceMapEntry{ kStableIface, 0, 2 };
        g_iface_slots[0] = VTableSlot{ kMethodFoo, reinterpret_cast<void*>(0xE000) };
        g_iface_slots[1] = VTableSlot{ kMethodBar, reinterpret_cast<void*>(0xE001) };
        g_iface_vt = TypeVTable{};
        g_iface_vt.type_token    = kTokenImpl;
        g_iface_vt.slot_count    = 2;
        g_iface_vt.slots         = g_iface_slots;
        g_iface_vt.iface_map     = g_iface_entry;
        g_iface_vt.iface_count   = 1;
        g_iface_vt.type_shape    = 1;
        RegisterTypeVTable(&g_iface_vt);

        // 150 scalability types (single-slot, independent)
        for (int i = 0; i < kScalabilityCount; i++) {
            uint32_t token = static_cast<uint32_t>(0xF300u + i);
            g_scalability_slots[i] = VTableSlot{ token, reinterpret_cast<void*>(static_cast<uintptr_t>(0x100000 + i)) };
            g_scalability_vt[i] = TypeVTable{};
            g_scalability_vt[i].type_token = token;
            g_scalability_vt[i].slot_count = 1;
            g_scalability_vt[i].slots = &g_scalability_slots[i];
            g_scalability_vt[i].type_shape = 1;
            RegisterTypeVTable(&g_scalability_vt[i]);
        }
    }
};

// ══════════════════════════════════════════════════════════════════════
// Helper: run N threads with synchronized start
// ══════════════════════════════════════════════════════════════════════

struct ThreadResult {
    int      thread_id;
    uint64_t iterations;
    uint64_t errors;
};

template <typename Func>
static std::vector<ThreadResult> RunConcurrent(int num_threads, uint64_t iterations, Func func) {
    std::promise<void> start_promise;
    std::shared_future<void> start_signal = start_promise.get_future();
    std::vector<ThreadResult> results(num_threads);
    std::vector<std::thread> threads;

    for (int i = 0; i < num_threads; i++) {
        threads.emplace_back([i, &results, &start_signal, iterations, &func]() {
            start_signal.wait();
            uint64_t errors = 0;
            for (uint64_t j = 0; j < iterations; j++) {
                if (!func(i, j)) {
                    errors++;
                }
            }
            results[i] = { i, iterations, errors };
        });
    }

    start_promise.set_value();
    for (auto& t : threads) {
        t.join();
    }
    return results;
}

// ══════════════════════════════════════════════════════════════════════
// Test 1: Concurrent TCVC read — ResolveVirtualMethodPointer
// ══════════════════════════════════════════════════════════════════════

TEST_F(VTableConcurrentStressTest, ConcurrentReadStress)
{
    auto results = RunConcurrent(kNumReaderThreads, kIterationsPerThread,
        [](int /*thread_id*/, uint64_t /*iter*/) -> bool {
            // Mix of direct hits, chain walks, and own-slot lookups
            void* p1 = ResolveVirtualMethodPointer(kTokenLeaf, kMethodFoo);
            void* p2 = ResolveVirtualMethodPointer(kTokenMid, kMethodFoo);
            void* p3 = ResolveVirtualMethodPointer(kTokenBase, kMethodFoo);
            void* p4 = ResolveVirtualMethodPointer(kTokenMid, kMethodBar);
            void* p5 = ResolveVirtualMethodPointer(kTokenLeaf, kMethodBaz);

            // Verify known results
            if (p1 == nullptr) return false;  // Leaf has kMethodFoo
            if (p2 == nullptr) return false;  // Mid has kMethodFoo
            if (p3 == nullptr) return false;  // Base has kMethodFoo
            if (p4 == nullptr) return false;  // Mid has kMethodBar
            if (p5 != nullptr) return false;  // No one has kMethodBaz
            return true;
        });

    uint64_t total_errors = 0;
    std::printf("ConcurrentReadStress (%d threads x %d iters):\n",
                kNumReaderThreads, kIterationsPerThread);
    for (auto& r : results) {
        std::printf("  Thread %d: %llu iterations, %llu errors\n",
                    r.thread_id, (unsigned long long)r.iterations,
                    (unsigned long long)r.errors);
        total_errors += r.errors;
    }
    EXPECT_EQ(0u, total_errors);
}

// ══════════════════════════════════════════════════════════════════════
// Test 2: Concurrent interface offset lookup
// ══════════════════════════════════════════════════════════════════════

TEST_F(VTableConcurrentStressTest, ConcurrentInterfaceLookup)
{
    auto results = RunConcurrent(kNumReaderThreads, kIterationsPerThread,
        [](int /*thread_id*/, uint64_t /*iter*/) -> bool {
            uint32_t off = chaos_find_interface_offset(kTokenImpl, kStableIface);
            if (off != 0) return false;  // offset 0 (iface_map says offset=0)

            uint32_t miss = chaos_find_interface_offset(kTokenImpl, 0xDEAD);
            if (miss != UINT32_MAX) return false;  // miss should return ~0u

            return true;
        });

    uint64_t total_errors = 0;
    std::printf("ConcurrentInterfaceLookup (%d threads x %d iters):\n",
                kNumReaderThreads, kIterationsPerThread);
    for (auto& r : results) {
        std::printf("  Thread %d: %llu iterations, %llu errors\n",
                    r.thread_id, (unsigned long long)r.iterations,
                    (unsigned long long)r.errors);
        total_errors += r.errors;
    }
    EXPECT_EQ(0u, total_errors);
}

// ══════════════════════════════════════════════════════════════════════
// Test 3: Concurrent TryGetTypeVTable
// ══════════════════════════════════════════════════════════════════════

TEST_F(VTableConcurrentStressTest, ConcurrentTryGetTypeVTable)
{
    auto results = RunConcurrent(kNumReaderThreads, kIterationsPerThread,
        [](int /*thread_id*/, uint64_t /*iter*/) -> bool {
            const auto* base  = TryGetTypeVTable(kTokenBase);
            const auto* mid   = TryGetTypeVTable(kTokenMid);
            const auto* leaf  = TryGetTypeVTable(kTokenLeaf);
            const auto* impl  = TryGetTypeVTable(kTokenImpl);
            const auto* miss  = TryGetTypeVTable(0xDEAD);

            if (base  == nullptr) return false;
            if (mid   == nullptr) return false;
            if (leaf  == nullptr) return false;
            if (impl  == nullptr) return false;
            if (miss  != nullptr) return false;

            // Verify type tokens
            if (base->type_token  != kTokenBase)  return false;
            if (mid->type_token   != kTokenMid)   return false;
            if (leaf->type_token  != kTokenLeaf)  return false;
            if (impl->type_token  != kTokenImpl)  return false;

            return true;
        });

    uint64_t total_errors = 0;
    for (auto& r : results) {
        total_errors += r.errors;
    }
    EXPECT_EQ(0u, total_errors);
}

// ══════════════════════════════════════════════════════════════════════
// Test 4: 128+ VTable scalability
// ══════════════════════════════════════════════════════════════════════

TEST_F(VTableConcurrentStressTest, Scalability128Plus)
{
    // Verify all 150 types are findable
    int found = 0;
    for (int i = 0; i < kScalabilityCount; i++) {
        uint32_t token = static_cast<uint32_t>(0xF300u + i);
        const auto* vt = TryGetTypeVTable(token);
        if (vt != nullptr && vt->type_token == token) {
            found++;
        }
    }
    EXPECT_EQ(kScalabilityCount, found);

    // Verify ResolveVirtualMethodPointer works on scalability types
    int resolved = 0;
    for (int i = 0; i < kScalabilityCount; i++) {
        uint32_t token = static_cast<uint32_t>(0xF300u + i);
        void* ptr = ResolveVirtualMethodPointer(token, token);
        if (ptr != nullptr) {
            resolved++;
        }
    }
    EXPECT_EQ(kScalabilityCount, resolved);

    // Total registered count should be at least chain(3) + iface(1) + scalability(150)
    uint32_t total = GetRegisteredVTableCount();
    EXPECT_GE(total, static_cast<uint32_t>(3 + 1 + kScalabilityCount));

    std::printf("Scalability128Plus: found %d/%d, resolved %d/%d, total registered %u\n",
                found, kScalabilityCount, resolved, kScalabilityCount, total);
}

// ══════════════════════════════════════════════════════════════════════
// Test 5: Epoch read-write contention
// ══════════════════════════════════════════════════════════════════════

TEST_F(VTableConcurrentStressTest, ReadWriteContention)
{
    std::atomic<bool> stop{false};
    std::atomic<uint64_t> reader_iterations{0};
    std::atomic<uint64_t> writer_updates{0};

    // Writer thread: periodically update kMethodFoo slot on all types
    auto writer = [&stop, &writer_updates]() {
        while (!stop.load(std::memory_order_relaxed)) {
            UpdateVTableSlotByMethodToken(kMethodFoo, kPtrC);
            writer_updates.fetch_add(1, std::memory_order_relaxed);
            std::this_thread::yield();
        }
    };

    // Reader threads: continuously resolve methods
    auto reader = [&stop, &reader_iterations]() {
        while (!stop.load(std::memory_order_relaxed)) {
            // Resolve from different types to exercise TCVC + chain walk
            volatile void* p1 = ResolveVirtualMethodPointer(kTokenLeaf, kMethodFoo);
            volatile void* p2 = ResolveVirtualMethodPointer(kTokenMid, kMethodFoo);
            volatile void* p3 = ResolveVirtualMethodPointer(kTokenBase, kMethodFoo);
            volatile void* p4 = ResolveVirtualMethodPointer(kTokenLeaf, kMethodBar);
            (void)p1; (void)p2; (void)p3; (void)p4;
            reader_iterations.fetch_add(1, std::memory_order_relaxed);
        }
    };

    std::thread writer_thr(writer);
    std::vector<std::thread> reader_threads;
    for (int i = 0; i < kNumReaderThreads; i++) {
        reader_threads.emplace_back(reader);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(kContentionDurationMs));
    stop.store(true, std::memory_order_release);

    writer_thr.join();
    for (auto& t : reader_threads) {
        t.join();
    }

    uint64_t reads = reader_iterations.load(std::memory_order_relaxed);
    uint64_t writes = writer_updates.load(std::memory_order_relaxed);

    std::printf("ReadWriteContention (%d ms): %llu reader-iterations, %llu writer-updates, no crashes\n",
                kContentionDurationMs,
                (unsigned long long)reads,
                (unsigned long long)writes);

    // No assertion on iteration counts — just verify no crash occurred
    SUCCEED();
}

// ══════════════════════════════════════════════════════════════════════
// Test 6: Concurrent read with interface epoch write contention
// ══════════════════════════════════════════════════════════════════════

TEST_F(VTableConcurrentStressTest, InterfaceEpochContention)
{
    std::atomic<bool> stop{false};
    std::atomic<uint64_t> iface_lookups{0};

    // Register a new runtime interface, then update it repeatedly
    auto writer = [&stop]() {
        while (!stop.load(std::memory_order_relaxed)) {
            RegisterTypeVTableRuntimeInterface(kTokenImpl, kStableIface, 0, 2);
            std::this_thread::yield();
        }
    };

    auto reader = [&stop, &iface_lookups]() {
        while (!stop.load(std::memory_order_relaxed)) {
            volatile uint32_t off = chaos_find_interface_offset(kTokenImpl, kStableIface);
            (void)off;
            iface_lookups.fetch_add(1, std::memory_order_relaxed);
        }
    };

    std::thread writer_thr(writer);
    std::vector<std::thread> reader_threads;
    for (int i = 0; i < kNumReaderThreads; i++) {
        reader_threads.emplace_back(reader);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(kContentionDurationMs));
    stop.store(true, std::memory_order_release);

    writer_thr.join();
    for (auto& t : reader_threads) {
        t.join();
    }

    uint64_t lookups = iface_lookups.load(std::memory_order_relaxed);
    std::printf("InterfaceEpochContention (%d ms): %llu lookups, no crashes\n",
                kContentionDurationMs, (unsigned long long)lookups);

    SUCCEED();
}
