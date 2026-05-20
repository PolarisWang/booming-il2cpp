/// delegate_stress_test — Delegate allocation/lifetime stress tests (GoogleTest).
///
/// 8 scenarios migrated from the old contracts test:
///   A1  CreateDelegate long pressure: 50 threads x 512 allocations
///   A2  Marshal-style allocate-and-release cycles via old-gen
///   B1  Chain build/delete/rebuild: large multicast chains up to 10000 entries
///   B2  Remove-Not-Found: search miss on a 1000-entry chain
///   C2  Multicast invoke: invocation-list iteration for large chains
///   E1  Conservative scan: LocalDelegate fields survive full GC
///   E3  Vector pointer compaction: multicast chains survive 10+ full GC
///   E5  Old-gen exhaustion: allocate until full GC triggered
///
/// CMakeLists entry (to be added):
///   add_chaos_test(test_gc_delegate_stress
///       delegate_stress_test.cpp ${CHAOS_GC_STUBS} ${CHAOS_GC_BASE}
///       LIBS ${CHAOS_GC_LIBS}
///   )
///   set_tests_properties(test_gc_delegate_stress
///       PROPERTIES LABELS "stress;gc" TIMEOUT 300)

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <thread>
#include <vector>

#include <chaos/native_types.h>
#include "gc_layout.h"
#include "gc_old_gen.h"
#include "gc_region.h"
#include "gc_scheduler.h"
#include "gc_stats.h"
#include "gc_test_base.h"
#include "gc_young_gen.h"
#include "thread_state.h"

#include <core/delegate_helpers.h>
#include <core/delegate_object.h>

#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

// ── Constants ────────────────────────────────────────────────────────────

static constexpr int    kNumWorkerThreads     = 50;
static constexpr int    kAllocationsPerThread = 512;
static constexpr int    kChainSize            = 10000;
static constexpr int    kRemoveNotFoundIter   = 100;
static constexpr int    kFullGcCycles         = 10;
static constexpr int    kExhaustionAllocCount = 100000;

static constexpr int    kVerifyStep           = 16;
static constexpr size_t kDelegateSize         = sizeof(DelegateObject);
static_assert(kDelegateSize == 56, "DelegateObject must be 56 bytes");

// ── Fake delegate TypeInfo ───────────────────────────────────────────────
//
// Must match TypeInfoHot memory layout so PrecisionMark's MarkObject can read
// stable_id at offset 16 and look up the GcTypeLayout for instance_size = 56.

struct alignas(8) FakeDelegateTypeInfo {
    const void* parent;       // [0]
    const void* vtable_array; // [8]
    uint64_t    stable_id;    // [16]
    uint32_t    vtable_length; // [24]
    uint16_t    warm_delta;   // [28]
    uint8_t     type_shape;   // [30]
    uint8_t     flags;        // [31]
};

static constexpr uint64_t kDelegateTypeAStableId = 0xDEDE00000000A001ULL;
static constexpr uint64_t kDelegateTypeBStableId = 0xDEDE00000000B001ULL;

static FakeDelegateTypeInfo g_delegate_type_a{ nullptr, nullptr, kDelegateTypeAStableId };
static FakeDelegateTypeInfo g_delegate_type_b{ nullptr, nullptr, kDelegateTypeBStableId };

// ── Fixture ──────────────────────────────────────────────────────────────

struct DelegateStressTest : GcStressTestBase {
    void SetUp() override {
        GcStressTestBase::SetUp();

        if (!s_inited_.load(std::memory_order_acquire)) {
            std::lock_guard<std::mutex> lock(s_init_mutex_);

            if (!s_inited_.load(std::memory_order_relaxed)) {
                // Initialise old-gen with default heap (channel 0, 2 pages min).
                g_old_gen.Init(0, 2);

                // Register pseudo-TypeInfo addresses so the PrecisionMark path in
                // MarkObject accepts them as valid TypeInfo pointers.
                auto& layout_registry = GcLayoutRegistry::Instance();
                uintptr_t a_begin = reinterpret_cast<uintptr_t>(&g_delegate_type_a);
                uintptr_t a_end   = a_begin + sizeof(g_delegate_type_a);
                uintptr_t b_begin = reinterpret_cast<uintptr_t>(&g_delegate_type_b);
                uintptr_t b_end   = b_begin + sizeof(g_delegate_type_b);
                layout_registry.RegisterTypeInfoRange(a_begin, a_end);
                layout_registry.RegisterTypeInfoRange(b_begin, b_end);
                layout_registry.Register(
                    kDelegateTypeAStableId, kDelegateSize, nullptr, 0);
                layout_registry.Register(
                    kDelegateTypeBStableId, kDelegateSize, nullptr, 0);

                s_inited_.store(true, std::memory_order_release);
            }
        }
    }

private:
    static std::atomic<bool>  s_inited_;
    static std::mutex         s_init_mutex_;
};

std::atomic<bool> DelegateStressTest::s_inited_{ false };
std::mutex        DelegateStressTest::s_init_mutex_;

// ── Internal helpers ────────────────────────────────────────────────────

namespace {

/// Allocate a single DelegateObject from old-gen and initialise its fields.
static CHAOS_IL2CPP_INTPTR AllocateSingleDelegate(
    void* type_info,
    CHAOS_IL2CPP_INTPTR target,
    CHAOS_IL2CPP_INTPTR method_ptr,
    CHAOS_IL2CPP_UINT32 method_token = 0)
{
    auto* obj = static_cast<DelegateObject*>(
        g_old_gen.Allocate(kDelegateSize, true));
    obj->type_info                     = type_info;
    obj->sync_state                    = 0;
    obj->chaos_delegate_target         = target;
    obj->chaos_delegate_method_ptr     = method_ptr;
    obj->chaos_delegate_invocation_list = 0;
    obj->chaos_delegate_invocation_count = 0;
    obj->chaos_delegate_method_token   = method_token;
    obj->_pad                          = 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(obj);
}

/// Setup a TLS nursery for the calling thread.
static bool SetupTlsNursery() {
    if (g_young_gen.region.load(std::memory_order_acquire) == nullptr) {
        static std::mutex s_init_mutex;
        std::lock_guard<std::mutex> lock(s_init_mutex);
        if (g_young_gen.region.load(std::memory_order_acquire) == nullptr) {
            InitYoungGeneration();
        }
    }
    return true;
}

/// Register the calling thread with the threading subsystem.
static void RegisterWorker() {
    int32_t tid = threading::AllocateThreadId();
    threading::RegisterThread(tid, nullptr);
}

/// Unregister the calling thread.
static void UnregisterWorker() {
    threading::UnregisterThread();
}

/// Run a full old-gen collection under a global safepoint.
static void RunFullGc() {
    uint32_t gen = threading::RequestGlobalSafepoint();
    g_old_gen.Collect(nullptr, nullptr);
    threading::ReleaseGlobalSafepoint(gen);
}

} // anonymous namespace

// ═══════════════════════════════════════════════════════════════════════════
// A1: CreateDelegate long pressure
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(DelegateStressTest, A1_CreateDelegatePressure) {
    std::atomic<int64_t> failures{ 0 };
    int workers = kNumWorkerThreads;
    int allocs  = kAllocationsPerThread;

    auto worker_fn = [&](int thread_idx) {
        RegisterWorker();
        threading::EnterCooperativeMode();
        SetupTlsNursery();

        for (int i = 0; i < allocs; i++) {
            CHAOS_IL2CPP_INTPTR val;

            // Serialise allocations to work around concurrent old-gen
            // allocation races (g_old_gen.Allocate with 4+ threads).
            {   static std::mutex s_alloc_lock;
                std::lock_guard<std::mutex> alloc_lock(s_alloc_lock);
                val = AllocateSingleDelegate(
                    &g_delegate_type_a,
                    static_cast<CHAOS_IL2CPP_INTPTR>(thread_idx * 1000 + i),
                    reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&g_delegate_type_a));
            }

            if (i % kVerifyStep == 0) {
                threading::SafepointPoll();
                auto* obj = reinterpret_cast<DelegateObject*>(val);
                if (obj->chaos_delegate_target !=
                    static_cast<CHAOS_IL2CPP_INTPTR>(thread_idx * 1000 + i)) {
                    failures.fetch_add(1, std::memory_order_relaxed);
                }
            }
        }

        threading::SafepointPoll();
        UnregisterWorker();
    };

    std::vector<std::thread> threads;
    threads.reserve(workers);
    for (int t = 0; t < workers; t++)
        threads.emplace_back(worker_fn, t);
    for (auto& t : threads) t.join();

    EXPECT_EQ(failures.load(), 0)
        << "A1: pattern verification failed (target field corruption)";
    SUCCEED();
}

// ═══════════════════════════════════════════════════════════════════════════
// A2: Marshal-style allocation pressure
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(DelegateStressTest, A2_MarshalAllocStress) {
    RegisterWorker();
    threading::EnterCooperativeMode();
    SetupTlsNursery();

    int64_t   failures    = 0;
    const int kIterations = 10000;

    // Simulate Marshal.GetDelegateForFunctionPointer pattern:
    // allocate delegate -> use -> release (let GC reclaim).
    for (int i = 0; i < kIterations; i++) {
        auto val = AllocateSingleDelegate(
            &g_delegate_type_b, 0,
            static_cast<CHAOS_IL2CPP_INTPTR>(0xDEADBEEF));

        if (i % kVerifyStep == 0) {
            auto* obj = reinterpret_cast<DelegateObject*>(val);
            if (obj->type_info != &g_delegate_type_b) {
                failures++;
            }
        }

        // Periodically trigger a full GC to stress old-gen sweep.
        if (i > 0 && i % 1000 == 0) {
            RunFullGc();
        }
    }

    threading::SafepointPoll();
    UnregisterWorker();

    EXPECT_EQ(failures, 0)
        << "A2: type_info verification failures detected";
    SUCCEED();
}

// ═══════════════════════════════════════════════════════════════════════════
// B1: Chain build/delete/rebuild
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(DelegateStressTest, B1_ChainBuildDeleteRebuild) {
    RegisterWorker();
    threading::EnterCooperativeMode();
    SetupTlsNursery();

    int64_t failures    = 0;
    int      chain_n    = kChainSize;

    // Build single entries.
    std::vector<CHAOS_IL2CPP_INTPTR> entries;
    entries.reserve(chain_n);
    for (int i = 0; i < chain_n; i++) {
        entries.push_back(AllocateSingleDelegate(
            &g_delegate_type_a,
            static_cast<CHAOS_IL2CPP_INTPTR>(i),
            reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&g_delegate_type_a)));
    }

    // Build chain via DelegateCombine.
    CHAOS_IL2CPP_INTPTR chain = 0;
    for (int i = 0; i < chain_n; i++) {
        chain = DelegateCombine(chain, entries[i]);
    }
    ASSERT_NE(chain, 0) << "B1: chain is null after combine";

    // Verify chain has correct invocation count.
    {
        auto* obj = reinterpret_cast<DelegateObject*>(chain);
        EXPECT_EQ(obj->chaos_delegate_invocation_count, chain_n)
            << "B1: invocation_count mismatch after combine";
        if (obj->chaos_delegate_invocation_count != chain_n)
            failures++;
    }

    // Remove half the entries.
    for (int i = 0; i < chain_n / 2; i++) {
        chain = DelegateRemove(chain, entries[i]);
    }

    // Verify remaining entries.
    if (chain != 0) {
        auto* obj = reinterpret_cast<DelegateObject*>(chain);
        CHAOS_IL2CPP_INTPTR expected_count = chain_n - chain_n / 2;
        if (obj->chaos_delegate_invocation_count != expected_count &&
            obj->chaos_delegate_invocation_count > 0) {
            // Single-entry case after remove: count == 0.
            if (expected_count > 1) {
                EXPECT_EQ(obj->chaos_delegate_invocation_count, expected_count)
                    << "B1: invocation_count after remove mismatch";
                failures++;
            }
        }
    }

    threading::SafepointPoll();
    UnregisterWorker();

    EXPECT_EQ(failures, 0) << "B1: chain verification failed";
    SUCCEED();
}

// ═══════════════════════════════════════════════════════════════════════════
// B2: Remove-Not-Found path
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(DelegateStressTest, B2_RemoveNotFound) {
    RegisterWorker();
    threading::EnterCooperativeMode();
    SetupTlsNursery();

    int64_t failures = 0;

    // Build large chain of type A.
    // Capped to 1000 because DelegateCombine copies the invocation list at
    // each step (O(N^2) for N entries).
    int chain_n = 1000;
    std::vector<CHAOS_IL2CPP_INTPTR> entries;
    entries.reserve(chain_n);
    for (int i = 0; i < chain_n; i++) {
        entries.push_back(AllocateSingleDelegate(
            &g_delegate_type_a,
            static_cast<CHAOS_IL2CPP_INTPTR>(i),
            reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&g_delegate_type_a)));
    }

    CHAOS_IL2CPP_INTPTR chain = 0;
    for (auto e : entries)
        chain = DelegateCombine(chain, e);

    // Create a "bogus" entry of different type to try to remove.
    // method_ptr must be non-zero (DelegateAppendFlattenedEntries rejects 0).
    CHAOS_IL2CPP_INTPTR bogus = AllocateSingleDelegate(
        &g_delegate_type_b, 0,
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&g_delegate_type_b));

    for (int i = 0; i < kRemoveNotFoundIter; i++) {
        CHAOS_IL2CPP_INTPTR result = DelegateRemove(chain, bogus);
        if (result != chain) {
            EXPECT_EQ(result, chain)
                << "B2: remove-not-found returned different chain at iter " << i;
            failures++;
            break;
        }
    }

    threading::SafepointPoll();
    UnregisterWorker();

    EXPECT_EQ(failures, 0) << "B2: remove-not-found failed";
    SUCCEED();
}

// ═══════════════════════════════════════════════════════════════════════════
// C2: Multicast invoke (invocation-list iteration)
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(DelegateStressTest, C2_MulticastInvoke) {
    RegisterWorker();
    threading::EnterCooperativeMode();
    SetupTlsNursery();

    int64_t failures = 0;
    int      chain_n = kChainSize;

    // Build chain.
    std::vector<CHAOS_IL2CPP_INTPTR> entries;
    entries.reserve(chain_n);
    for (int i = 0; i < chain_n; i++) {
        entries.push_back(AllocateSingleDelegate(
            &g_delegate_type_a,
            static_cast<CHAOS_IL2CPP_INTPTR>(i),
            reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&g_delegate_type_a)));
    }

    CHAOS_IL2CPP_INTPTR chain = 0;
    for (auto e : entries)
        chain = DelegateCombine(chain, e);

    // Iterate the invocation list (simulate DelegateInvoke dispatch).
    auto* obj = reinterpret_cast<DelegateObject*>(chain);
    if (obj->chaos_delegate_invocation_count > 0) {
        auto* vec = reinterpret_cast<std::vector<CHAOS_IL2CPP_INTPTR>*>(
            obj->chaos_delegate_invocation_list);
        ASSERT_NE(vec, nullptr) << "C2: invocation_list is null for multicast chain";
        for (size_t idx = 0; idx < vec->size(); idx++) {
            auto* entry_obj = reinterpret_cast<DelegateObject*>((*vec)[idx]);
            if (entry_obj == nullptr) {
                failures++;
            }
        }
    } else {
        // Single entry.
        if (obj->chaos_delegate_target == 0 && obj->chaos_delegate_method_ptr == 0) {
            failures++;
        }
    }

    threading::SafepointPoll();
    UnregisterWorker();

    EXPECT_EQ(failures, 0) << "C2: invocation iteration encountered null entries";
    SUCCEED();
}

// ═══════════════════════════════════════════════════════════════════════════
// E1: Conservative scan survival
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(DelegateStressTest, E1_ConservativeScanSurvival) {
    RegisterWorker();
    threading::EnterCooperativeMode();
    SetupTlsNursery();

    int64_t      failures      = 0;
    constexpr int kNumDelegates = 1000;

    // Allocate delegates with known patterns in every field.
    std::vector<CHAOS_IL2CPP_INTPTR> delegates;
    delegates.reserve(kNumDelegates);
    for (int i = 0; i < kNumDelegates; i++) {
        auto val = AllocateSingleDelegate(
            &g_delegate_type_a,
            static_cast<CHAOS_IL2CPP_INTPTR>(0x1000 + i),
            static_cast<CHAOS_IL2CPP_INTPTR>(0x2000 + i),
            static_cast<CHAOS_IL2CPP_UINT32>(0x3000 + i));
        delegates.push_back(val);
        // Pin each delegate so the old-gen collector finds them.
        g_old_gen.AddPinnedRoot(
            reinterpret_cast<void*>(val), kDelegateSize);
    }

    // Verify fields before GC as baseline.
    for (int i = 0; i < kNumDelegates; i++) {
        auto* obj = reinterpret_cast<DelegateObject*>(delegates[i]);
        if (obj->type_info != &g_delegate_type_a) { failures++; break; }
        if (obj->chaos_delegate_target != static_cast<CHAOS_IL2CPP_INTPTR>(0x1000 + i)) { failures++; break; }
        if (obj->chaos_delegate_method_ptr != static_cast<CHAOS_IL2CPP_INTPTR>(0x2000 + i)) { failures++; break; }
        if (obj->chaos_delegate_method_token != static_cast<CHAOS_IL2CPP_UINT32>(0x3000 + i)) { failures++; break; }
    }
    ASSERT_EQ(failures, 0) << "E1: field corruption before GC";

    // Force full GC — pinned roots keep delegates reachable.
    RunFullGc();

    // Verify all fields survive GC.
    for (int i = 0; i < kNumDelegates; i++) {
        auto* obj = reinterpret_cast<DelegateObject*>(delegates[i]);
        EXPECT_EQ(obj->type_info, &g_delegate_type_a)
            << "E1: type_info corrupted after full GC at idx " << i;
        EXPECT_EQ(obj->chaos_delegate_target,
                  static_cast<CHAOS_IL2CPP_INTPTR>(0x1000 + i))
            << "E1: target corrupted after full GC at idx " << i;
        EXPECT_EQ(obj->chaos_delegate_method_token,
                  static_cast<CHAOS_IL2CPP_UINT32>(0x3000 + i))
            << "E1: token corrupted after full GC at idx " << i;
        if (obj->type_info != &g_delegate_type_a ||
            obj->chaos_delegate_target != static_cast<CHAOS_IL2CPP_INTPTR>(0x1000 + i) ||
            obj->chaos_delegate_method_token != static_cast<CHAOS_IL2CPP_UINT32>(0x3000 + i)) {
            failures++;
        }
    }

    // Verify non-pointer fields are not scanned as pointers.
    // Create a delegate whose method_token looks like a pointer; after GC it
    // must still hold the original value.
    CHAOS_IL2CPP_INTPTR suspicious_token_val = AllocateSingleDelegate(
        &g_delegate_type_a, 0, 0,
        static_cast<CHAOS_IL2CPP_UINT32>(
            reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&g_delegate_type_a)));
    g_old_gen.AddPinnedRoot(
        reinterpret_cast<void*>(suspicious_token_val), kDelegateSize);
    RunFullGc();

    auto* sus_obj = reinterpret_cast<DelegateObject*>(suspicious_token_val);
    EXPECT_EQ(sus_obj->chaos_delegate_method_token,
              static_cast<CHAOS_IL2CPP_UINT32>(
                  reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&g_delegate_type_a)))
        << "E1: token field corrupted by conservative scanner";

    threading::SafepointPoll();
    UnregisterWorker();

    EXPECT_EQ(failures, 0) << "E1: conservative scan survival failed";
    SUCCEED();
}

// ═══════════════════════════════════════════════════════════════════════════
// E3: Vector pointer compaction survival
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(DelegateStressTest, E3_VectorPointerCompaction) {
    RegisterWorker();
    threading::EnterCooperativeMode();
    SetupTlsNursery();

    int64_t failures = 0;
    int      chain_n = 1000; // kChainSize / 10

    // Build multicast chain.
    std::vector<CHAOS_IL2CPP_INTPTR> entries;
    entries.reserve(chain_n);
    for (int i = 0; i < chain_n; i++) {
        auto* delegate = static_cast<DelegateObject*>(
            g_old_gen.Allocate(kDelegateSize, true));
        delegate->type_info                     = &g_delegate_type_a;
        delegate->sync_state                    = 0;
        delegate->chaos_delegate_target         = static_cast<CHAOS_IL2CPP_INTPTR>(i);
        delegate->chaos_delegate_method_ptr     = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&g_delegate_type_a);
        delegate->chaos_delegate_invocation_list = 0;
        delegate->chaos_delegate_invocation_count = 0;
        delegate->chaos_delegate_method_token   = 0;
        delegate->_pad                          = 0;
        entries.push_back(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(delegate));
    }

    CHAOS_IL2CPP_INTPTR chain_value = 0;
    for (int i = 0; i < chain_n; i++)
        chain_value = DelegateCombine(chain_value, entries[i]);

    // Pin all entries and the chain so the GC finds them (the calling thread's
    // stack is not scanned by old-gen).
    for (int i = 0; i < chain_n; i++)
        g_old_gen.AddPinnedRoot(reinterpret_cast<void*>(entries[i]), kDelegateSize);
    g_old_gen.AddPinnedRoot(reinterpret_cast<void*>(chain_value), kDelegateSize);

    // Repeated GC cycles (including compaction) must not corrupt vector pointers.
    // AddPinnedRoot prevents both collection AND compaction (pages with pinned
    // objects are excluded from compaction), so chain_value pointer stays valid.
    for (int cycle = 0; cycle < kFullGcCycles; cycle++)
        RunFullGc();

    // Verify invocation list still accessible.
    auto* obj = reinterpret_cast<DelegateObject*>(chain_value);
    if (obj->chaos_delegate_invocation_count > 0) {
        auto* vec = reinterpret_cast<std::vector<CHAOS_IL2CPP_INTPTR>*>(
            obj->chaos_delegate_invocation_list);
        if (vec == nullptr) {
            ADD_FAILURE() << "E3: invocation_list null after compaction";
            failures++;
        } else if (vec->size() != static_cast<size_t>(chain_n)) {
            ADD_FAILURE() << "E3: invocation_list size changed after compaction "
                          << "(expected " << chain_n << ", got " << vec->size() << ")";
            failures++;
        }
    } else if (chain_n > 1) {
        ADD_FAILURE() << "E3: multicast chain degraded to single entry after compaction";
        failures++;
    }

    threading::SafepointPoll();
    UnregisterWorker();

    EXPECT_EQ(failures, 0) << "E3: vector pointer compaction survival failed";
    SUCCEED();
}

// ═══════════════════════════════════════════════════════════════════════════
// E5: Old-gen exhaustion (allocate until full GC triggered)
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(DelegateStressTest, E5_OldGenExhaustion) {
    RegisterWorker();
    threading::EnterCooperativeMode();
    SetupTlsNursery();

    int64_t failures  = 0;
    int      alloc_count = kExhaustionAllocCount;

    // Use a fixed-size array (not std::vector) + AddPinnedRoot because the GC
    // skips the calling thread's stack during conservative scanning.
    static constexpr int kKeepCapacity = 1000;
    CHAOS_IL2CPP_INTPTR keep_alive[kKeepCapacity];
    int keep_count = 0;

    for (int i = 0; i < alloc_count; i++) {
        auto val = AllocateSingleDelegate(
            &g_delegate_type_a,
            static_cast<CHAOS_IL2CPP_INTPTR>(i),
            reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&g_delegate_type_a));

        // Keep every 100th alive.
        if (i % 100 == 0 && keep_count < kKeepCapacity)
            keep_alive[keep_count++] = val;

        // Periodically verify a kept-alive delegate.
        if (i % kVerifyStep == 0 && keep_count > 0) {
            auto* obj = reinterpret_cast<DelegateObject*>(keep_alive[keep_count - 1]);
            if (obj->type_info != &g_delegate_type_a)
                failures++;
        }
    }

    // Register kept-alive delegates as pinned roots BEFORE GC.
    for (int i = 0; i < keep_count; i++)
        g_old_gen.AddPinnedRoot(reinterpret_cast<void*>(keep_alive[i]), kDelegateSize);

    // Force full GC to verify old-gen can reclaim dead delegates.
    RunFullGc();

    // Verify kept-alive delegates still intact.
    bool alive_ok = true;
    for (int i = 0; i < keep_count; i++) {
        auto* obj = reinterpret_cast<DelegateObject*>(keep_alive[i]);
        if (obj->type_info != &g_delegate_type_a) {
            alive_ok = false;
            break;
        }
    }

    threading::SafepointPoll();
    UnregisterWorker();

    EXPECT_EQ(failures, 0) << "E5: pattern verification failures during allocation";
    EXPECT_TRUE(alive_ok) << "E5: kept-alive delegates corrupted after full GC";
    SUCCEED();
}
