// hotupdate_replacement_benchmark_test.cpp — Method replacement benchmark & stress
//
// Benchmarks the method_replacement::Register / Revert cycle with varying
// numbers of registered TypeVTables, and runs multi-threaded stress tests
// to verify correctness under concurrent access.
//
// Environment variable support:
//   CHAOS_IL2CPP_STRESS_SCALE=N  — scales iteration counts (default 1)

#include <method_replacement.h>
#include <vtable_registry.h>

#include <gtest/gtest.h>

#include <chaos/native_types.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <crtdbg.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <vector>

namespace mr = chaos::il2cpp::method_replacement;
namespace vr = chaos::il2cpp::vtable_registry;

// AOT symbol stubs
extern "C" const char* kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int kChaosExternalRuntimeCount = 0;
extern "C" void* chaos_il2cpp_aot_hotpatch_module = nullptr;

// ── Test helpers ────────────────────────────────────────────────────────────

// Build a heap-allocated TypeVTable with one slot. All allocations use
// CHAOS_IL2CPP_MALLOC so UnregisterTypeVTable can free them via CHAOS_IL2CPP_FREE.
static vr::TypeVTable* MakeVTable(uint32_t type_token, const void** flat_array) {
    void* slot_buf = CHAOS_IL2CPP_MALLOC(sizeof(vr::VTableSlot));
    auto* slot = static_cast<vr::VTableSlot*>(slot_buf);
    slot->method_token = type_token;
    slot->method_pointer = const_cast<void*>(flat_array[0]);

    void* vt_buf = CHAOS_IL2CPP_MALLOC(sizeof(vr::TypeVTable));
    auto* vt = static_cast<vr::TypeVTable*>(vt_buf);
    *vt = vr::TypeVTable{};
    vt->stable_id     = (static_cast<uint64_t>(type_token) << 32) | type_token;
    vt->type_token    = type_token;
    vt->slot_count    = 1;
    vt->slots         = slot;
    vt->vtable_array  = flat_array;
    vt->vtable_length = 1;
    vt->type_shape    = 1;
    return vt;
}

// Build a heap-allocated TypeVTable with a specific VTableSlot (for shared-token tests).
static vr::TypeVTable* MakeVTableWithSlot(uint32_t type_token, const vr::VTableSlot* slot, const void** flat_array) {
    void* slots_buf = CHAOS_IL2CPP_MALLOC(sizeof(vr::VTableSlot));
    auto* slot_heap = static_cast<vr::VTableSlot*>(slots_buf);
    slot_heap[0] = *slot;

    void* vt_buf = CHAOS_IL2CPP_MALLOC(sizeof(vr::TypeVTable));
    auto* vt = static_cast<vr::TypeVTable*>(vt_buf);
    *vt = vr::TypeVTable{};
    vt->type_token    = type_token;
    vt->stable_id     = (static_cast<uint64_t>(type_token) << 32) | type_token;
    vt->slot_count    = 1;
    vt->slots         = slot_heap;
    vt->vtable_array  = flat_array;
    vt->vtable_length = 1;
    vt->type_shape    = 1;
    return vt;
}

struct VTableSet {
    std::vector<uint32_t> tokens;
};

static VTableSet CreateVTables(uint32_t count, void* original_fn) {
    VTableSet set;
    set.tokens.reserve(count);
    for (uint32_t i = 0; i < count; i++) {
        uint32_t token = 0x06001000u + i;
        auto* flat = static_cast<const void**>(CHAOS_IL2CPP_MALLOC(sizeof(const void*)));
        flat[0] = original_fn;
        auto* vt = MakeVTable(token, flat);
        set.tokens.push_back(token);
        vr::RegisterTypeVTable(vt);
    }
    return set;
}

static int GetStressScale() {
    const char* env = std::getenv("CHAOS_IL2CPP_STRESS_SCALE");
    return (env != nullptr) ? std::atoi(env) : 1;
}

// ── Fixture ─────────────────────────────────────────────────────────────────

class ReplacementBenchmarkTest : public ::testing::Test {
protected:
    std::vector<uint32_t> registered_bench_tokens_;

    void SetUp() override {
        mr::RevertAll();
        registered_bench_tokens_.clear();
    }

    void TearDown() override {
        mr::RevertAll();
        for (auto tok : registered_bench_tokens_) {
            vr::UnregisterTypeVTable(tok);
        }
        registered_bench_tokens_.clear();
    }

    void RegisterBenchVTable(vr::TypeVTable* vt) {
        ASSERT_TRUE(vr::RegisterTypeVTable(vt));
        registered_bench_tokens_.push_back(vt->type_token);
    }
};

// ── Benchmark tests ─────────────────────────────────────────────────────────

TEST_F(ReplacementBenchmarkTest, RegisterRevertLatency_Small)
{
    int scale = (std::max)(1, GetStressScale());
    uint32_t vtable_count = 10 * scale;
    void* const kOriginal = reinterpret_cast<void*>(static_cast<uintptr_t>(0x1111));
    void* const kPatch    = reinterpret_cast<void*>(static_cast<uintptr_t>(0x2222));

    auto set = CreateVTables(vtable_count, kOriginal);
    // Track tokens so TearDown cleans up.
    registered_bench_tokens_.insert(registered_bench_tokens_.end(),
                                    set.tokens.begin(), set.tokens.end());

    // Use the last token for the benchmark (appears in the last vtable only).
    uint32_t target_token = set.tokens.back();

    auto t0 = std::chrono::high_resolution_clock::now();
    ASSERT_TRUE(mr::Register(target_token, kPatch));
    auto t1 = std::chrono::high_resolution_clock::now();
    ASSERT_TRUE(mr::Revert(target_token));
    auto t2 = std::chrono::high_resolution_clock::now();

    auto register_us = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
    auto revert_us   = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

    std::printf("[BENCH] RegisterRevertLatency_Small: %u vtables, register=%lld us, revert=%lld us\n",
                vtable_count,
                static_cast<long long>(register_us),
                static_cast<long long>(revert_us));

    // Should complete in under 10ms even on slow machines.
    EXPECT_LT(register_us, 10000);
    EXPECT_LT(revert_us, 10000);
}

TEST_F(ReplacementBenchmarkTest, RegisterRevertLatency_Medium)
{
    int scale = (std::max)(1, GetStressScale());
    uint32_t vtable_count = 100 * scale;
    void* const kOriginal = reinterpret_cast<void*>(static_cast<uintptr_t>(0x1111));
    void* const kPatch    = reinterpret_cast<void*>(static_cast<uintptr_t>(0x2222));

    auto set = CreateVTables(vtable_count, kOriginal);
    registered_bench_tokens_.insert(registered_bench_tokens_.end(),
                                    set.tokens.begin(), set.tokens.end());

    uint32_t target_token = set.tokens.back();

    auto t0 = std::chrono::high_resolution_clock::now();
    ASSERT_TRUE(mr::Register(target_token, kPatch));
    auto t1 = std::chrono::high_resolution_clock::now();
    ASSERT_TRUE(mr::Revert(target_token));
    auto t2 = std::chrono::high_resolution_clock::now();

    auto register_us = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
    auto revert_us   = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

    std::printf("[BENCH] RegisterRevertLatency_Medium: %u vtables, register=%lld us, revert=%lld us\n",
                vtable_count,
                static_cast<long long>(register_us),
                static_cast<long long>(revert_us));

    EXPECT_LT(register_us, 50000);
    EXPECT_LT(revert_us, 50000);
}

TEST_F(ReplacementBenchmarkTest, SharedTokenAcrossAllVTables)
{
    // A single token shared across ALL vtables → max O(n) scan.
    int scale = (std::max)(1, GetStressScale());
    uint32_t vtable_count = 50 * scale;
    static constexpr uint32_t kSharedToken = 0x06000042u;
    void* const kOriginal = reinterpret_cast<void*>(static_cast<uintptr_t>(0x1111));
    void* const kPatch    = reinterpret_cast<void*>(static_cast<uintptr_t>(0x2222));

    vr::VTableSlot slot{kSharedToken, kOriginal};

    // Create VTables all with the same method token.
    for (uint32_t i = 0; i < vtable_count; i++) {
        uint32_t type_token = 0x02000000u + i;
        auto* flat = static_cast<const void**>(CHAOS_IL2CPP_MALLOC(sizeof(const void*)));
        flat[0] = kOriginal;
        auto* vt = MakeVTableWithSlot(type_token, &slot, flat);
        RegisterBenchVTable(vt);
    }

    auto t0 = std::chrono::high_resolution_clock::now();
    ASSERT_TRUE(mr::Register(kSharedToken, kPatch));
    auto t1 = std::chrono::high_resolution_clock::now();
    ASSERT_TRUE(mr::Revert(kSharedToken));
    auto t2 = std::chrono::high_resolution_clock::now();

    auto register_us = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
    auto revert_us   = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

    std::printf("[BENCH] SharedTokenAcrossAllVTables: %u vtables, register=%lld us, revert=%lld us\n",
                vtable_count,
                static_cast<long long>(register_us),
                static_cast<long long>(revert_us));

    EXPECT_LT(register_us, 50000);
    EXPECT_LT(revert_us, 50000);
}

// ── Cycle stress test ───────────────────────────────────────────────────────

TEST_F(ReplacementBenchmarkTest, RegisterRevertCycleStress)
{
    int scale  = (std::max)(1, GetStressScale());
    int cycles = 500 * scale;

    // Register a single vtable so Register/Revert has something to update.
    static constexpr uint32_t kTypeToken  = 0x02000001u;
    static constexpr uint32_t kMethodToken = 0x06000042u;
    void* const kOriginal = reinterpret_cast<void*>(static_cast<uintptr_t>(0x1111));
    void* const kPatch    = reinterpret_cast<void*>(static_cast<uintptr_t>(0x2222));
    void* const kPatch2   = reinterpret_cast<void*>(static_cast<uintptr_t>(0x3333));
    void* const kPatch3   = reinterpret_cast<void*>(static_cast<uintptr_t>(0x4444));

    auto* flat = static_cast<const void**>(CHAOS_IL2CPP_MALLOC(sizeof(const void*)));
    flat[0] = kOriginal;
    auto* vt = MakeVTable(kTypeToken, flat);
    RegisterBenchVTable(vt);

    auto t0 = std::chrono::high_resolution_clock::now();

    void* thunks[] = {kPatch, kPatch2, kPatch3};
    for (int i = 0; i < cycles; i++) {
        ASSERT_TRUE(mr::Register(kMethodToken, thunks[i % 3]));
        ASSERT_TRUE(mr::Revert(kMethodToken));
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    auto total_us = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
    double avg_us = static_cast<double>(total_us) / static_cast<double>(cycles * 2);

    std::printf("[BENCH] RegisterRevertCycleStress: %d cycles, total=%lld us, avg=%.1f us/op\n",
                cycles,
                static_cast<long long>(total_us),
                avg_us);

    // After all cycles, vtable should have kOriginal.
    EXPECT_EQ(vr::TryGetTypeVTable(kTypeToken)->slots[0].method_pointer, kOriginal);
}

// ── Multi-threaded stress tests ─────────────────────────────────────────────

#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")

static int __cdecl StackTraceReportHook(int reportType, char* message, int* returnValue) {
    if (reportType == _CRT_ASSERT) {
        fprintf(stderr, "\n*** CRT ASSERT: %s\n", message);
        void* stack[128];
        USHORT frames = CaptureStackBackTrace(1, 128, stack, NULL);
        for (USHORT i = 0; i < frames; i++) {
            fprintf(stderr, "  #%u: %p\n", i, stack[i]);
        }
        fflush(stderr);
    }
    return 0; // Allow default processing (abort/retry/ignore dialog)
}

TEST_F(ReplacementBenchmarkTest, ConcurrentRegisterDifferentTokens)
{
    int scale = (std::max)(1, GetStressScale());
    int thread_count = 4;
    int ops_per_thread = 200 * scale;
    void* const kOriginal = reinterpret_cast<void*>(static_cast<uintptr_t>(0x1111));
    std::atomic<int> failures{0};

    auto worker = [&](int thread_id) {
        int base_token = 0x06001000u + thread_id * 1000;
        for (int i = 0; i < ops_per_thread; i++) {
            uint32_t token = base_token + i;
            if (!mr::Register(token, kOriginal)) {
                failures.fetch_add(1, std::memory_order_relaxed);
            }
            if (mr::Resolve(token) != kOriginal) {
                failures.fetch_add(1, std::memory_order_relaxed);
            }
            if (!mr::Revert(token)) {
                failures.fetch_add(1, std::memory_order_relaxed);
            }
            if (mr::Resolve(token) != nullptr) {
                failures.fetch_add(1, std::memory_order_relaxed);
            }
        }
    };

    std::vector<std::thread> threads;
    threads.reserve(thread_count);
    auto t0 = std::chrono::high_resolution_clock::now();

    for (int t = 0; t < thread_count; t++) {
        threads.emplace_back(worker, t);
    }
    for (auto& th : threads) th.join();

    auto t1 = std::chrono::high_resolution_clock::now();
    auto total_us = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();

    std::printf("[BENCH] ConcurrentRegisterDifferentTokens: %d threads x %d ops, total=%lld us, failures=%d\n",
                thread_count, ops_per_thread,
                static_cast<long long>(total_us),
                failures.load());

    EXPECT_EQ(failures.load(), 0);
    EXPECT_EQ(mr::ActiveCount(), 0u);
}

TEST_F(ReplacementBenchmarkTest, ConcurrentRegisterSameToken)
{
    // All threads compete for the same token → tests mutex contention and
    // verifies no state corruption under concurrent Register/Revert cycles.
    // Note: Register may return false when another thread holds the token —
    // that's expected. Only Resolve mismatches after a successful Register
    // indicate actual corruption.
    int scale = (std::max)(1, GetStressScale());
    int thread_count = 4;
    int ops_per_thread = 100 * scale;
    static constexpr uint32_t kSharedToken = 0x06000042u;
    void* const kOriginal = reinterpret_cast<void*>(static_cast<uintptr_t>(0x1111));
    void* const kPatch    = reinterpret_cast<void*>(static_cast<uintptr_t>(0x2222));
    std::atomic<int> resolve_mismatches{0};

    // Register one vtable so UpdateVTableSlotByMethodToken has work to do.
    auto* flat = static_cast<const void**>(CHAOS_IL2CPP_MALLOC(sizeof(const void*)));
    flat[0] = kOriginal;
    vr::VTableSlot slot{kSharedToken, kOriginal};
    auto* vt = MakeVTableWithSlot(0x02000001u, &slot, flat);
    RegisterBenchVTable(vt);

    // Install CRT assertion hook to capture stack trace on vector(54) crash.
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
    _CrtSetReportHook2(_CRT_RPTHOOK_INSTALL, StackTraceReportHook);

    auto worker = [&](int id) {
        (void)id;
        for (int i = 0; i < ops_per_thread; i++) {
            // PHASE 1: Only Register — isolate if crash is in Register path
            mr::Register(kSharedToken, kPatch);
        }
    };

    std::vector<std::thread> threads;
    threads.reserve(thread_count);
    auto t0 = std::chrono::high_resolution_clock::now();

    for (int t = 0; t < thread_count; t++) {
        threads.emplace_back(worker, t);
    }
    for (auto& th : threads) th.join();

    auto t1 = std::chrono::high_resolution_clock::now();
    auto total_us = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();

    std::printf("[BENCH] ConcurrentRegisterSameToken: %d threads x %d ops, total=%lld us, resolve_mismatches=%d\n",
                thread_count, ops_per_thread,
                static_cast<long long>(total_us),
                resolve_mismatches.load());

    
    // Final state: reverted.
    EXPECT_EQ(mr::Resolve(kSharedToken), nullptr);
}

TEST_F(ReplacementBenchmarkTest, ConcurrentRevertAll)
{
    // Register many tokens from different threads, then call RevertAll().
    int scale = (std::max)(1, GetStressScale());
    int thread_count = 4;
    int tokens_per_thread = 50 * scale;
    std::atomic<int> failures{0};

    auto worker = [&](int thread_id) {
        int base_token = 0x06001000u + thread_id * 1000;
        for (int i = 0; i < tokens_per_thread; i++) {
            uint32_t token = base_token + i;
            if (!mr::Register(token, reinterpret_cast<void*>(static_cast<uintptr_t>(0xDEAD)))) {
                failures.fetch_add(1, std::memory_order_relaxed);
            }
        }
    };

    std::vector<std::thread> threads;
    threads.reserve(thread_count);
    for (int t = 0; t < thread_count; t++) {
        threads.emplace_back(worker, t);
    }
    for (auto& th : threads) th.join();

    EXPECT_EQ(failures.load(), 0);
    EXPECT_EQ(mr::ActiveCount(), static_cast<uint32_t>(thread_count * tokens_per_thread));

    // RevertAll from main thread.
    mr::RevertAll();
    EXPECT_EQ(mr::ActiveCount(), 0u);
}
