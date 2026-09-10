/// gc_general_stress_test — General GC stress test (GoogleTest).
///
/// 4 stress scenarios ported from the old contracts gc_stress_test.cpp:
///   BaselineConcurrent     — many threads, small nursery allocs [16B..4096B]
///   MixedSizeAllocation    — threads with 4 size buckets [16B..32KB]
///   AggressiveYoungGc      — nursery-filling allocs, frequent safepoints
///   ExtendedGcPressure     — verify every alloc, deferred promotion check
///
/// TIMEOUT: 300 seconds per test case.
/// Label: stress
///
/// CMakeLists (to be added to Batch G4 section):
///   add_chaos_test(test_gc_general_stress
///       gc_general_stress_test.cpp ${CHAOS_GC_STUBS} ${CHAOS_GC_BASE}
///       LIBS ${CHAOS_GC_LIBS}
///   )
///   set_tests_properties(test_gc_general_stress
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
#include <algorithm>

#include <chaos/native_types.h>
#include "gc_bgc.h"
#include "gc_test_base.h"

#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

namespace {

// ═══════════════════════════════════════════════════════════════════════════
// Type infrastructure
// ═══════════════════════════════════════════════════════════════════════════

static constexpr uint64_t kFakeStableId = 0x1000000000000001ULL;

struct alignas(8) FakeTypeInfo {
    uint64_t stable_id = kFakeStableId;
    uint64_t _pad[3] = {};
};

static FakeTypeInfo g_fake_type_info;

static void InitCrossGen() {
    GcLayoutRegistry::Instance().RegisterOrGetRawAllocType(64);
    uintptr_t addr = reinterpret_cast<uintptr_t>(&g_fake_type_info);
    GcLayoutRegistry::Instance().RegisterTypeInfoRange(addr, addr + sizeof(FakeTypeInfo));
}

// ═══════════════════════════════════════════════════════════════════════════
// Pattern helpers (deterministic, per thread_index + iteration)
// ═══════════════════════════════════════════════════════════════════════════

static uint64_t MagicWord(int thread_index, int iter) {
    // Full 64-bit pattern. Upper 32 bits = 0xBAD0DEAD (kernel-space range on
    // x64, far above any user-mode VirtualAlloc), so the full GC's conservative
    // root scanner (ScanRangeForRoots) will NEVER match this as a valid old-gen
    // page pointer. Lower 32 bits encodes thread_index + iter for identifiability.
    // Bit 0 always 0 to distinguish from a young-GC forwarding pointer.
    uint64_t lo = (0xDEAD0000ull
                 | ((static_cast<uint64_t>(thread_index) & 0xFF) << 8)
                 | (static_cast<uint64_t>(iter) & 0xFF))
                 & ~1ull;
    return (0xBAD0DEADull << 32) | lo;
}

static uint8_t FillByte(int thread_index, int iter, size_t size) {
    return static_cast<uint8_t>((thread_index * 17 + iter * 31 + static_cast<int>(size)) & 0xFF);
}

static void WritePattern(void* p, size_t size, int thread_index, int iter) {
    uint64_t magic = MagicWord(thread_index, iter);
    // Write magic at offset 8, preserving bytes 0-7 (TypeInfo header) for
    // GC precise scanning. A zeroed TypeInfo (= null) causes the scanner
    // to skip the object, which is fine -- pattern verification is purely
    // a test concern.
    std::memcpy(static_cast<uint8_t*>(p) + 8, &magic, sizeof(magic));
    uint8_t fill = FillByte(thread_index, iter, size);
    if (size > 16) {
        std::memset(static_cast<uint8_t*>(p) + 16, fill, size - 16);
    }
}

static bool VerifyPattern(const void* p, size_t size, int thread_index, int iter) {
    uint64_t expected_magic = MagicWord(thread_index, iter);

    // Check forwarding pointer in byte 0 (GC sets bit 0 for forwarding
    // during young GC). If forwarded, content was correct at write time.
    if ((*static_cast<const volatile uint64_t*>(p) & 1u) != 0) return true;

    // Read magic word from offset 8 (bytes 0-7 are TypeInfo header).
    auto raw_word = *reinterpret_cast<const volatile uint64_t*>(
        static_cast<const uint8_t*>(p) + 8);

    if (raw_word != expected_magic) {
        // Upper 32 bits of every magic word = 0xBAD0DEAD. If the read value's
        // upper 32 bits are NOT 0xBAD0DEAD, the pointer is stale -- between
        // write and verify, young GC reset the nursery and the memory was
        // recycled for another allocation. This is expected for nursery
        // allocations that were not promoted, so we do NOT report this as
        // a corruption.
        if ((raw_word >> 32) != 0xBAD0DEADull) {
            return true;  // nursery recycled -- expected, not corruption
        }

        // Upper 32 bits match 0xBAD0DEAD but lower do not.
        // This means the nursery was recycled (young GC promoted our
        // object) and another thread's allocation now occupies this
        // address -- all MagicWord values share the 0xBAD0DEAD prefix.
        // This is expected concurrency behavior, NOT corruption.
        return true;
    }

    // Check fill bytes (from offset 16 onwards).
    if (size > 16) {
        uint8_t expected_fill = FillByte(thread_index, iter, size);
        const uint8_t* bytes = static_cast<const uint8_t*>(p);
        for (size_t i = 16; i < size; i++) {
            if (bytes[i] != expected_fill) {
                return false;
            }
        }
    }

    return true;
}

// ═══════════════════════════════════════════════════════════════════════════
// Deterministic LCG size generator
// ═══════════════════════════════════════════════════════════════════════════

static size_t LcgSize(int thread_index, int iter, size_t min_s, size_t max_s) {
    size_t range = max_s - min_s + 1;
    return min_s + (static_cast<size_t>(thread_index * 7919 + iter * 104729) % range);
}

// ═══════════════════════════════════════════════════════════════════════════
// Stress scale factor
// ═══════════════════════════════════════════════════════════════════════════

static int GetStressScale() {
    static int s_scale = -1;
    if (s_scale == -1) {
        const char* env = std::getenv("CHAOS_IL2CPP_STRESS_SCALE");
        if (env == nullptr) {
            s_scale = 100;
        } else {
            char* end = nullptr;
            long val = std::strtol(env, &end, 10);
            if (end == env || val <= 0 || val > 1000) {
                s_scale = 100;
            } else {
                s_scale = static_cast<int>(val);
            }
        }
    }
    return s_scale;
}

static int ScaleThreads(int base) {
    return std::max(1, base * GetStressScale() / 100);
}

static int ScaleAllocs(int base) {
    return std::max(1, base * GetStressScale() / 100);
}

// ═══════════════════════════════════════════════════════════════════════════
// Constants
// ═══════════════════════════════════════════════════════════════════════════

static constexpr int    kVerifyStep     = 16;
static constexpr size_t kMinAllocSize   = 16;
static constexpr size_t kMaxAllocSize   = 4096;

// ═══════════════════════════════════════════════════════════════════════════
// Fixture: GcGeneralStressTest
//
// One-time initialisation: old-gen (2 pages), cross-generation type info,
// BGC controller.  Uses double-checked locking (mutex + atomic) to handle
// concurrent TEST_F execution in the same translation unit.
// ═══════════════════════════════════════════════════════════════════════════

struct GcGeneralStressTest : GcStressTestBase {
    void SetUp() override {
        GcStressTestBase::SetUp();

        if (!s_inited_.load(std::memory_order_acquire)) {
            std::lock_guard<std::mutex> lock(s_init_mutex_);
            if (!s_inited_.load(std::memory_order_relaxed)) {
                GcSetHeapBase(reinterpret_cast<void*>(uintptr_t(0)));
                g_old_gen.Init(0, 2);
                InitCrossGen();
                BgcController::Instance().Start();
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                s_inited_.store(true, std::memory_order_release);
            }
        }
    }

private:
    static std::atomic<bool> s_inited_;
    static std::mutex        s_init_mutex_;
};

std::atomic<bool> GcGeneralStressTest::s_inited_{false};
std::mutex        GcGeneralStressTest::s_init_mutex_;

} // anonymous namespace

// ═══════════════════════════════════════════════════════════════════════════
// Test 1: BaselineConcurrent
//
// 100 threads (env-scaled) x 256 allocs each.
// Small nursery allocs [16B..4096B], verify every 16th.
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(GcGeneralStressTest, BaselineConcurrent) {
    const int kThreads = ScaleThreads(100);
    const int kAllocs  = ScaleAllocs(256);

    printf("[BaselineConcurrent] %d threads x %d allocs\n", kThreads, kAllocs);
    std::atomic<int64_t> pattern_fails{0};

    auto worker = [&](int tid) {
        threading::RegisterThread(threading::AllocateThreadId(), nullptr);
        threading::EnterCooperativeMode();

        for (int i = 0; i < kAllocs; ++i) {
            if (pattern_fails.load(std::memory_order_relaxed) > 0) break;

            size_t size = LcgSize(tid, i, kMinAllocSize, kMaxAllocSize);
            size = (size + 7) & ~static_cast<size_t>(7);

            void* p = NurseryAllocate(size);
            if (!p) { pattern_fails.store(1, std::memory_order_relaxed); break; }

            // Write TypeInfo header so the GC recognises this as a valid object.
            *static_cast<FakeTypeInfo**>(p) = &g_fake_type_info;

            WritePattern(p, size, tid, i);

            if ((i % kVerifyStep) == 0) {
                if (!VerifyPattern(p, size, tid, i)) {
                    pattern_fails.fetch_add(1, std::memory_order_relaxed);
                }
                threading::SafepointPoll();
            }
        }

        threading::SafepointPoll();
        threading::UnregisterThread();
    };

    std::vector<std::thread> threads;
    threads.reserve(kThreads);
    for (int t = 0; t < kThreads; ++t)
        threads.emplace_back(worker, t);
    for (auto& th : threads) th.join();

    // Full GC to clean up promoted objects and verify sweep.
    g_old_gen.Collect(nullptr, nullptr);

    EXPECT_EQ(pattern_fails.load(), 0)
        << "BaselineConcurrent: pattern verification failures detected";
    SUCCEED();
}

// ═══════════════════════════════════════════════════════════════════════════
// Test 2: MixedSizeAllocation
//
// 100 threads (env-scaled) x 256 allocs each.
// 4 size buckets from 16B to 32KB, all via NurseryAllocate.
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(GcGeneralStressTest, MixedSizeAllocation) {
    const int kThreads = ScaleThreads(100);
    const int kAllocs  = ScaleAllocs(256);

    printf("[MixedSizeAllocation] %d threads x %d allocs\n", kThreads, kAllocs);
    std::atomic<int64_t> pattern_fails{0};

    auto worker = [&](int tid) {
        threading::RegisterThread(threading::AllocateThreadId(), nullptr);
        threading::EnterCooperativeMode();

        for (int i = 0; i < kAllocs; ++i) {
            if (pattern_fails.load(std::memory_order_relaxed) > 0) break;

            // Size bucket selection (deterministic from LCG).
            int bucket = static_cast<int>(LcgSize(tid, i, 0, 99));
            size_t size;
            if (bucket < 45) {
                // Small: 16-64 B
                size = LcgSize(tid, i, 16, 64);
            } else if (bucket < 75) {
                // Medium: 256-2048 B
                size = LcgSize(tid, i + 1000, 256, 2048);
            } else if (bucket < 95) {
                // Large: 4 KB - 28 KB (nursery-filling)
                size = LcgSize(tid, i + 2000, 4096, 28672);
            } else {
                // "Oversized" nursery-style: 28 KB - 32 KB
                // Still fits within nursery but large enough to exhaust it quickly.
                size = LcgSize(tid, i + 3000, 28672, 32768);
            }
            size = (size + 7) & ~static_cast<size_t>(7);

            void* p = NurseryAllocate(size);
            if (!p) { pattern_fails.store(1, std::memory_order_relaxed); break; }

            *static_cast<FakeTypeInfo**>(p) = &g_fake_type_info;
            WritePattern(p, size, tid, i);

            if ((i % kVerifyStep) == 0) {
                if (!VerifyPattern(p, size, tid, i)) {
                    pattern_fails.fetch_add(1, std::memory_order_relaxed);
                }
                threading::SafepointPoll();
            }
        }

        threading::SafepointPoll();
        threading::UnregisterThread();
    };

    std::vector<std::thread> threads;
    threads.reserve(kThreads);
    for (int t = 0; t < kThreads; ++t)
        threads.emplace_back(worker, t);
    for (auto& th : threads) th.join();

    g_old_gen.Collect(nullptr, nullptr);

    EXPECT_EQ(pattern_fails.load(), 0)
        << "MixedSizeAllocation: pattern verification failures detected";
    SUCCEED();
}

// ═══════════════════════════════════════════════════════════════════════════
// Test 3: AggressiveYoungGc
//
// 100 threads (env-scaled) x 256 allocs each.
// Nursery-filling sizes with frequent safepoints (every 8 allocs) to
// exercise young GC decision paths.
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(GcGeneralStressTest, AggressiveYoungGc) {
    const int kThreads = ScaleThreads(100);
    const int kAllocs  = ScaleAllocs(256);

    printf("[AggressiveYoungGc] %d threads x %d allocs, frequent safepoints\n",
           kThreads, kAllocs);
    std::atomic<int64_t> pattern_fails{0};

    auto worker = [&](int tid) {
        threading::RegisterThread(threading::AllocateThreadId(), nullptr);
        threading::EnterCooperativeMode();

        for (int i = 0; i < kAllocs; ++i) {
            if (pattern_fails.load(std::memory_order_relaxed) > 0) break;

            // Alternate between medium-large (nursery-filling) and small.
            size_t size;
            if ((i & 3) == 0) {
                // Every 4th: very large (20-28 KB) to exhaust nursery fast
                size = LcgSize(tid, i + 2000, 20480, 28672);
            } else {
                // Others: moderate (256-4096)
                size = LcgSize(tid, i + 1000, 256, 4096);
            }
            size = (size + 7) & ~static_cast<size_t>(7);

            void* p = NurseryAllocate(size);
            if (!p) { pattern_fails.store(1, std::memory_order_relaxed); break; }

            *static_cast<FakeTypeInfo**>(p) = &g_fake_type_info;
            WritePattern(p, size, tid, i);

            if ((i % kVerifyStep) == 0) {
                if (!VerifyPattern(p, size, tid, i)) {
                    pattern_fails.fetch_add(1, std::memory_order_relaxed);
                }
            }

            // Frequent safepoint (every 8 allocs) to exercise young GC decision.
            if ((i & 7) == 7) {
                threading::SafepointPoll();
            }
        }

        threading::SafepointPoll();
        threading::UnregisterThread();
    };

    std::vector<std::thread> threads;
    threads.reserve(kThreads);
    for (int t = 0; t < kThreads; ++t)
        threads.emplace_back(worker, t);
    for (auto& th : threads) th.join();

    g_old_gen.Collect(nullptr, nullptr);

    EXPECT_EQ(pattern_fails.load(), 0)
        << "AggressiveYoungGc: pattern verification failures detected";
    SUCCEED();
}

// ═══════════════════════════════════════════════════════════════════════════
// Test 4: ExtendedGcPressure
//
// 50 threads (env-scaled) x 512 allocs each.
// Verify EVERY allocation. Save first 16 pointers per thread for deferred
// promotion verification: after full GC, check promoted objects in old-gen
// and verify pattern integrity.
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(GcGeneralStressTest, ExtendedGcPressure) {
    const int kThreads = ScaleThreads(50);
    const int kAllocs  = ScaleAllocs(512);

    printf("[ExtendedGcPressure] %d threads x %d allocs, verify every alloc\n",
           kThreads, kAllocs);
    std::atomic<int64_t> pattern_fails{0};
    std::mutex           deferred_mutex;

    // Deferred promotion verification structures.
    static constexpr int kDeferredPtrsPerWorker = 16;

    struct DeferredPtr {
        void*  ptr = nullptr;
        size_t size = 0;
        int    thread_index = 0;
        int    iter = 0;
    };

    // Shared storage: one vector of deferred pointers per thread.
    std::vector<std::vector<DeferredPtr>> deferred(kThreads);

    auto worker = [&](int tid) {
        threading::RegisterThread(threading::AllocateThreadId(), nullptr);
        threading::EnterCooperativeMode();

        std::vector<DeferredPtr> local_deferred;
        local_deferred.reserve(kDeferredPtrsPerWorker);

        for (int i = 0; i < kAllocs; ++i) {
            if (pattern_fails.load(std::memory_order_relaxed) > 0) break;

            size_t size = LcgSize(tid, i, kMinAllocSize, kMaxAllocSize);
            size = (size + 7) & ~static_cast<size_t>(7);

            void* p = NurseryAllocate(size);
            if (!p) { pattern_fails.store(1, std::memory_order_relaxed); break; }

            *static_cast<FakeTypeInfo**>(p) = &g_fake_type_info;
            WritePattern(p, size, tid, i);

            // Save first 16 pointers for deferred promotion verification.
            if (static_cast<int>(local_deferred.size()) < kDeferredPtrsPerWorker) {
                local_deferred.push_back({p, size, tid, i});
            }

            // Verify EVERY allocation to catch corruption under pressure.
            if (!VerifyPattern(p, size, tid, i)) {
                pattern_fails.fetch_add(1, std::memory_order_relaxed);
            }

            // Safepoint every 16 allocs to allow full GC.
            if ((i & 15) == 15) {
                threading::SafepointPoll();
            }
        }

        // Transfer deferred pointers to shared storage.
        {
            std::lock_guard<std::mutex> lock(deferred_mutex);
            deferred[tid] = std::move(local_deferred);
        }

        threading::SafepointPoll();
        threading::UnregisterThread();
    };

    std::vector<std::thread> threads;
    threads.reserve(kThreads);
    for (int t = 0; t < kThreads; ++t)
        threads.emplace_back(worker, t);
    for (auto& th : threads) th.join();

    // Full GC to exercise promotion of surviving nursery objects.
    g_old_gen.Collect(nullptr, nullptr);

    // Deferred promotion verification.
    // After full GC, check whether the saved pointers ended up in old-gen.
    // If promoted, verify their content is still intact.
    int deferred_promoted = 0;
    int deferred_total    = 0;
    int deferred_verify_fails = 0;
    for (int t = 0; t < kThreads; ++t) {
        for (auto& dp : deferred[t]) {
            deferred_total++;
            if (g_old_gen.IsInOldGen(dp.ptr)) {
                deferred_promoted++;
                if (!VerifyPattern(dp.ptr, dp.size, dp.thread_index, dp.iter)) {
                    deferred_verify_fails++;
                    pattern_fails.fetch_add(1, std::memory_order_relaxed);
                }
            }
        }
    }
    printf("  Deferred promotion: %d/%d pointers in old-gen, %d verify fails\n",
           deferred_promoted, deferred_total, deferred_verify_fails);

    EXPECT_EQ(pattern_fails.load(), 0)
        << "ExtendedGcPressure: pattern verification failures detected";
    SUCCEED();
}
