/// fuzz_gc_test.cpp — GC allocation fuzz tester.
///
/// Generates random allocation patterns (sizes, thread counts, GC triggers)
/// and verifies the GC doesn't crash, assert-fail, or corrupt memory.
///
/// Unlike stress tests (which use predefined patterns), fuzz tests use
/// unbounded random combinations to find edge cases.
///
/// Design note: this test does NOT verify object integrity after GC because
/// the CRAG GC requires explicit root registration. Without registering
/// survivors as roots, the GC may legitimately collect them. The fuzzer
/// instead uses address-sanitizer instrumentation and ASSERT detection to
/// find memory errors — if the GC corrupts memory or crashes, ASan/ASSERT
/// will catch it regardless of what lives or dies.

#include <atomic>
#include <cstdint>
#include <random>
#include <thread>

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
/// Allocates objects with random sizes, writes magic for ASan corruption
/// detection, and returns the number of allocations performed.
void RandomAllocPhase(std::mt19937& rng, int count, uint32_t magic) {
    std::uniform_int_distribution<int> size_dist(8, 128 * 1024); // 8 bytes to 128KB

    for (int i = 0; i < count; i++) {
        CHAOS_IL2CPP_SIZE size = static_cast<CHAOS_IL2CPP_SIZE>(size_dist(rng));

        void* obj;
        if (size >= 85 * 1024) {
            obj = g_loh.Allocate(size);
        } else {
            obj = NurseryAllocate(static_cast<uint32_t>(size));
        }

        if (!obj) continue;

        // Write magic at offset 0 for ASan use-after-free detection:
        // if the GC frees and reuses this memory, the next access will
        // trigger an ASan error.
        if (size >= 8) {
            *reinterpret_cast<uint32_t*>(obj) = magic;
        }
    }
}

}  // anonymous namespace

class GcFuzzTest : public GcUnitTestBase {
protected:
    static constexpr uint32_t kMagic = 0xABCD1234;

    void TearDown() override {
        // Flush TLAB so the resource snapshot doesn't report a false leak
        tls_tlab = TLAB{};
        GcUnitTestBase::TearDown();
    }
};

/// Random allocation sizes and GC triggers on a single thread.
/// Tests that the GC handles random-size allocations without crashing.
/// ASan instrumentation catches any memory errors.
TEST_F(GcFuzzTest, SingleThreadRandomAlloc) {
    std::mt19937 rng(static_cast<unsigned>(std::time(nullptr)));

    // Random phases: allocation + occasional GC
    std::uniform_int_distribution<int> alloc_count_dist(10, 500);
    std::uniform_int_distribution<int> gc_interval_dist(3, 10);

    int phase = 0;
    int since_gc = 0;
    int total_allocs = 0;

    while (total_allocs < 5000 && phase < 50) {
        int count = alloc_count_dist(rng);
        total_allocs += count;

        RandomAllocPhase(rng, count, kMagic);

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
}

/// Multi-threaded random allocation with post-join GC.
/// Background threads allocate with random sizes, then join.
/// After all threads complete, the main thread runs GC cycles.
/// ASan instrumentation catches any memory errors.
TEST_F(GcFuzzTest, MultiThreadedRandomAlloc) {
    std::mt19937 rng(static_cast<unsigned>(std::time(nullptr)));

    // Determine thread count (random 2-6)
    std::uniform_int_distribution<int> thread_dist(2, 6);
    int num_threads = thread_dist(rng);

    // Start background threads
    std::vector<std::thread> threads;
    std::atomic<bool> stop{false};
    std::atomic<bool> start{false};

    for (int t = 0; t < num_threads; t++) {
        threads.emplace_back([&start, &stop, t]() {
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
                }
            }

            threading::UnregisterThread();
        });
    }

    // Signal all threads to start
    start.store(true, std::memory_order_release);

    // Let threads allocate for a fixed duration (no concurrent GC)
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // Stop all threads and join
    stop.store(true, std::memory_order_relaxed);
    for (auto& th : threads) th.join();

    // Run GC cycles AFTER all threads have stopped
    GcYoungCollection();
    GcYoungCollection();
}
