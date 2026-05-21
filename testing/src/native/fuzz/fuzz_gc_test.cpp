/// fuzz_gc_test.cpp — GC allocation fuzz tester.
///
/// Generates random allocation patterns (sizes, thread counts, GC triggers)
/// and verifies the GC doesn't crash, assert-fail, or corrupt memory.
///
/// Unlike stress tests (which use predefined patterns), fuzz tests use
/// unbounded random combinations to find edge cases.

#include <atomic>
#include <cstdint>
#include <random>
#include <thread>
#include <vector>

#include "gc_test_base.h"
#include "gc_test_macros.h"
#include "gc_young_collector.h"
#include "gc_helpers.h"
#include "gc_loh.h"
#include <chaos/native_types.h>
#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

namespace {

/// Run a random allocation phase on the current thread.
/// Returns the number of surviving global allocations.
int RandomAllocPhase(std::mt19937& rng, int count,
                     std::vector<void*>& globals, std::atomic<int>& global_count,
                     uint32_t magic) {
    std::uniform_int_distribution<int> size_dist(8, 128 * 1024); // 8 bytes to 128KB
    std::uniform_real_distribution<float> survive_dist(0.0f, 1.0f);

    for (int i = 0; i < count; i++) {
        CHAOS_IL2CPP_SIZE size = static_cast<CHAOS_IL2CPP_SIZE>(size_dist(rng));

        void* obj;
        if (size >= 85 * 1024) {
            obj = g_loh.Allocate(size);
        } else {
            obj = NurseryAllocate(static_cast<uint32_t>(size));
        }

        if (!obj) continue;

        // Write magic at offset 0 (header) and at end - 4 for corruption detection
        if (size >= 8) {
            *reinterpret_cast<uint32_t*>(obj) = magic;
        }
        if (size >= 12) {
            *reinterpret_cast<uint32_t*>(static_cast<char*>(obj) + size - 4) = magic ^ 0xFFFFFFFF;
        }

        // Random survival
        if (survive_dist(rng) < 0.1f) {
            int idx = global_count.fetch_add(1, std::memory_order_relaxed);
            if (idx < 10000) {
                if (static_cast<size_t>(idx) >= globals.size()) {
                    globals.resize(static_cast<size_t>(idx) + 1000);
                }
                globals[static_cast<size_t>(idx)] = obj;
            }
        }
    }

    return global_count.load(std::memory_order_relaxed);
}

/// Check all surviving objects for magic corruption.
void VerifyGlobals(const std::vector<void*>& globals, int count, uint32_t magic) {
    for (int i = 0; i < count && i < 10000; i++) {
        void* obj = globals[static_cast<size_t>(i)];
        if (!obj) continue;

        uint32_t header = *reinterpret_cast<uint32_t*>(obj);
        EXPECT_EQ(header, magic) << "Global " << i << " header corrupted";
    }
}

}  // anonymous namespace

class GcFuzzTest : public GcUnitTestBase {
protected:
    static constexpr uint32_t kMagic = 0xABCD1234;
};

/// Random allocation sizes and GC triggers on a single thread.
TEST_F(GcFuzzTest, SingleThreadRandomAlloc) {
    std::mt19937 rng(static_cast<unsigned>(std::time(nullptr)));
    std::vector<void*> globals(10000, nullptr);
    std::atomic<int> global_count{0};

    // Random phases: allocation + occasional GC
    std::uniform_int_distribution<int> alloc_count_dist(10, 500);
    std::uniform_int_distribution<int> gc_interval_dist(3, 10);

    int phase = 0;
    int since_gc = 0;
    int total_allocs = 0;

    while (total_allocs < 5000 && phase < 50) {
        int count = alloc_count_dist(rng);
        total_allocs += count;

        RandomAllocPhase(rng, count, globals, global_count, kMagic);

        since_gc++;
        if (since_gc >= gc_interval_dist(rng)) {
            GcYoungCollection();
            since_gc = 0;
        }

        // Occasionally do a full GC
        if (phase % 13 == 0) {
            chaos_gc_collect();
        }

        phase++;
    }

    VerifyGlobals(globals, global_count.load(std::memory_order_relaxed), kMagic);
}

/// Multi-threaded random allocation with post-join GC verification.
/// Each thread allocates randomly, writes magic, and optionally preserves
/// objects in the shared global array. After all threads join, the main
/// thread runs GC cycles and verifies survivors.
TEST_F(GcFuzzTest, MultiThreadedRandomAlloc) {
    std::mt19937 rng(static_cast<unsigned>(std::time(nullptr)));
    std::vector<void*> globals(10000, nullptr);
    std::atomic<int> global_count{0};

    // Determine thread count
    std::uniform_int_distribution<int> thread_dist(2, 6);
    int num_threads = thread_dist(rng);

    // Start background threads
    std::vector<std::thread> threads;
    std::atomic<bool> stop{false};
    std::atomic<bool> start{false};

    for (int t = 0; t < num_threads; t++) {
        threads.emplace_back([&start, &stop, &globals, &global_count, t]() {
            uint32_t tid = threading::AllocateThreadId();
            threading::RegisterThread(tid, nullptr);
            threading::EnterCooperativeMode();

            std::mt19937 local_rng(static_cast<unsigned>(t + 1) * 0x9E3779B9u);
            std::uniform_int_distribution<int> count_dist(5, 50);
            std::uniform_int_distribution<CHAOS_IL2CPP_SIZE> size_dist(8, 64 * 1024);

            // Wait for start signal
            while (!start.load(std::memory_order_relaxed)) {}

            while (!stop.load(std::memory_order_relaxed)) {
                int count = count_dist(local_rng);
                for (int i = 0; i < count; i++) {
                    CHAOS_IL2CPP_SIZE size = size_dist(local_rng);
                    void* obj;
                    if (size >= 85 * 1024) {
                        obj = g_loh.Allocate(size);
                    } else {
                        obj = NurseryAllocate(static_cast<uint32_t>(size));
                    }
                    if (obj && size >= 8) {
                        *reinterpret_cast<uint32_t*>(obj) = kMagic;
                    }

                    // 5% survive
                    if (obj && (local_rng() % 20 == 0)) {
                        int idx = global_count.fetch_add(1, std::memory_order_relaxed);
                        if (idx < 10000) {
                            globals[static_cast<size_t>(idx)] = obj;
                        }
                    }
                }
            }

            threading::UnregisterThread();
        });
    }

    // Signal all threads to start
    start.store(true, std::memory_order_release);

    // Let threads allocate for a fixed duration
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // Stop all threads and join
    stop.store(true, std::memory_order_relaxed);
    for (auto& th : threads) th.join();

    // Now run GC cycles to verify objects survive collection
    GcYoungCollection();
    GcYoungCollection();

    VerifyGlobals(globals, global_count.load(std::memory_order_relaxed), kMagic);
}
