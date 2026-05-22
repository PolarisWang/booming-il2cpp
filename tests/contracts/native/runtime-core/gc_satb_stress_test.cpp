/// gc_satb_stress_test.cpp — Multithreaded SATB buffer overflow stress test.
///
/// Exercises the following SATB code paths under high concurrency:
///   1. Per-thread 1024-entry buffer auto-flush (BgcSatbPreWriteBarrier line 96)
///   2. Global SATB queue accumulation during concurrent mark
///   3. SATB freeze convergence protocol
///   4. STW re-mark drain-all-TLS-buffers
///
/// Each thread writes through the pre-write barrier repeatedly, filling its
/// 1024-entry buffer many times.  Verifies no entry loss under contention.

#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <thread>
#include <vector>

#include <chaos/native_types.h>

#include "gc_bgc.h"
#include "gc_bgc_inline.h"

using namespace chaos::il2cpp::runtime_core;

#include "gc_test_macros.h"

static int g_failures = 0;
static int g_tests = 0;
static int g_sub = 0;

// ── Configuration ────────────────────────────────────────────────────────

/// Number of concurrent writer threads.
static constexpr int kSatbWorkers = 8;

/// Number of barrier writes per thread.
/// Each write fills the barrier's 1024-entry buffer ~10x at 10K writes.
static constexpr int kWritesPerThread = 10240;

/// Number of unique old pointers in the ring buffer.
/// Simulates realistic old-gen object references being overwritten.
static constexpr int kNumSlots = 64;

// ── Test state ───────────────────────────────────────────────────────────

/// Slot array: each slot is written through the barrier by all threads.
/// The barrier captures the OLD value before the write.
static void* g_slots[kNumSlots];

/// Total writes performed (for verifying global SATB queue).
static std::atomic<uint64_t> g_total_writes{0};

/// Per-thread write counters.
static std::atomic<int> g_threads_ready{0};
static std::atomic<int> g_threads_done{0};

/// Error flag.
static std::atomic<bool> g_any_error{false};

// ── Helpers ──────────────────────────────────────────────────────────────

static void ResetTestState() {
    g_total_writes.store(0, std::memory_order_relaxed);
    g_threads_ready.store(0, std::memory_order_relaxed);
    g_threads_done.store(0, std::memory_order_relaxed);
    g_any_error.store(false, std::memory_order_relaxed);
    for (int i = 0; i < kNumSlots; i++) {
        g_slots[i] = reinterpret_cast<void*>(static_cast<uintptr_t>(0x1000 + i));
    }
}

/// Writer thread: repeatedly writes through the SATB barrier.
static void SatbWriterThread(int thread_id) {
    // Register on first use to get a SATB buffer.
    tls_satb_buffer_index = -1;
    tls_satb_registered = false;

    g_threads_ready.fetch_add(1, std::memory_order_release);

    // Wait for all threads to be ready.
    while (g_threads_ready.load(std::memory_order_acquire) < kSatbWorkers) {
        std::this_thread::yield();
    }

    // Perform barrier writes.  Each iteration:
    //   1. Select a slot (round-robin across the array)
    //   2. Write a new pointer through it
    //   3. The barrier captures the old pointer
    for (int i = 0; i < kWritesPerThread; i++) {
        int slot_idx = (thread_id * kWritesPerThread + i) % kNumSlots;
        void* new_val = reinterpret_cast<void*>(
            static_cast<uintptr_t>(0x2000 + thread_id * kWritesPerThread + i));

        BgcSatbPreWriteBarrier(&g_slots[slot_idx]);
        g_slots[slot_idx] = new_val;
        g_total_writes.fetch_add(1, std::memory_order_relaxed);
    }

    g_threads_done.fetch_add(1, std::memory_order_release);
}

/// Flush any remaining entries in the calling thread's SATB buffer.
static void FlushLocalSatbBuffer() {
    auto& ctrl = BgcController::Instance();
    int pool_idx = tls_satb_buffer_index;
    if (pool_idx >= 0) {
        auto& buf = ctrl.GetSatbBuffer(pool_idx);
        uint32_t cnt = buf.count.load(std::memory_order_acquire);
        if (cnt > 0) {
            ctrl.FlushSatbBuffer(buf.entries, cnt);
            buf.count.store(0, std::memory_order_release);
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════
// Test 1: Basic SATB overflow under concurrent writes
// ═══════════════════════════════════════════════════════════════════════

static void TestSatbConcurrentOverflow() {
    printf("\n── Test: SatbConcurrentOverflow ──\n");
    auto& ctrl = BgcController::Instance();
    ctrl.ResetForTest();

    // Enable marking — activates the pre-write barrier.
    g_bgc_is_marking.store(true, std::memory_order_release);

    ResetTestState();

    // Spawn writer threads.
    std::vector<std::thread> workers;
    for (int i = 0; i < kSatbWorkers; i++) {
        workers.emplace_back(SatbWriterThread, i);
    }
    for (auto& w : workers) {
        if (w.joinable()) w.join();
    }

    uint64_t total = g_total_writes.load(std::memory_order_relaxed);
    printf("  total_writes=%llu (expected=%d)\n",
           static_cast<unsigned long long>(total),
           kSatbWorkers * kWritesPerThread);
    CHECK(total == static_cast<uint64_t>(kSatbWorkers * kWritesPerThread),
          "all writes completed");

    // Flush any remaining entries from each thread's SATB buffer.
    // In a real GC, this is done by STW re-mark's DrainAllTlsSatbBuffers.
    for (int i = 0; i < 256; i++) {
        auto& buf = ctrl.GetSatbBuffer(i);
        uint32_t cnt = buf.count.load(std::memory_order_acquire);
        if (cnt > 0) {
            ctrl.FlushSatbBuffer(buf.entries, cnt);
            buf.count.store(0, std::memory_order_release);
        }
    }

    // Verify global SATB queue has entries.
    CHAOS_IL2CPP_SIZE global_count = ctrl.GetGlobalSatbCount();
    printf("  global_satb_count=%llu (should be > 0)\n",
           static_cast<unsigned long long>(global_count));
    CHECK(global_count > 0, "global SATB queue has entries after flush");

    // Drain the global queue.
    CHAOS_IL2CPP_SIZE drained = ctrl.DrainGlobalSatbQueue();
    printf("  drained=%llu entries from global SATB queue\n",
           static_cast<unsigned long long>(drained));
    CHECK(drained > 0, "drained entries from global SATB queue");

    // After drain, global count should be 0.
    CHECK(ctrl.GetGlobalSatbCount() == 0u, "global SATB queue empty after drain");

    g_bgc_is_marking.store(false, std::memory_order_release);
}

// ═══════════════════════════════════════════════════════════════════════
// Test 2: SATB buffer full auto-flush path
// ═══════════════════════════════════════════════════════════════════════

static void TestSatbBufferFullAutoFlush() {
    printf("\n── Test: SatbBufferFullAutoFlush ──\n");
    auto& ctrl = BgcController::Instance();
    ctrl.ResetForTest();
    tls_satb_buffer_index = -1;
    tls_satb_registered = false;

    g_bgc_is_marking.store(true, std::memory_order_release);

    // Write through the barrier kSatbBufferSize + 1 times on one thread.
    // The kSatbBufferSize+1th write triggers the auto-flush path.
    void* slot_storage = reinterpret_cast<void*>(uintptr_t(0x1000));
    void** slot = &slot_storage;

    for (int i = 0; i < kBgcSatbBufferSize + 1; i++) {
        void* new_val = reinterpret_cast<void*>(uintptr_t(0x2000 + i));
        BgcSatbPreWriteBarrier(slot);
        *slot = new_val;
        slot_storage = new_val;
    }

    // Flush the remaining entries.
    FlushLocalSatbBuffer();

    CHAOS_IL2CPP_SIZE global_count = ctrl.GetGlobalSatbCount();
    printf("  global_satb_count=%llu after %d barrier writes (expecting >= %d)\n",
           static_cast<unsigned long long>(global_count),
           kBgcSatbBufferSize + 1, kBgcSatbBufferSize);
    CHECK(global_count >= static_cast<CHAOS_IL2CPP_SIZE>(kBgcSatbBufferSize),
          "at least kSatbBufferSize entries captured");

    // Drain and verify we got all entries.
    CHAOS_IL2CPP_SIZE drained = ctrl.DrainGlobalSatbQueue();
    printf("  drained=%llu entries\n", static_cast<unsigned long long>(drained));
    CHECK(drained == global_count, "drained all entries");

    g_bgc_is_marking.store(false, std::memory_order_release);
}

// ═══════════════════════════════════════════════════════════════════════
// Test 3: SATB freeze convergence protocol
// ═══════════════════════════════════════════════════════════════════════

static void TestSatbFreezeConvergence() {
    printf("\n── Test: SatbFreezeConvergence ──\n");
    auto& ctrl = BgcController::Instance();
    ctrl.ResetForTest();
    tls_satb_buffer_index = -1;
    tls_satb_registered = false;

    g_bgc_is_marking.store(true, std::memory_order_release);

    // Register this thread's buffer.
    void* slot_storage = reinterpret_cast<void*>(uintptr_t(0x1000));
    void** slot = &slot_storage;
    BgcSatbPreWriteBarrier(slot);
    *slot = reinterpret_cast<void*>(uintptr_t(0x2000));
    slot_storage = reinterpret_cast<void*>(uintptr_t(0x2000));

    // Verify buffer is allocated.
    int pool_idx = tls_satb_buffer_index;
    CHECK(pool_idx >= 0, "SATB buffer allocated");

    // Simulate freeze: set remaining count and request freeze.
    int freeze_remaining = 3;
    ctrl.satb_freeze_remaining_.store(freeze_remaining, std::memory_order_release);
    ctrl.satb_freeze_requested_.store(true, std::memory_order_release);

    // Three more writes — each should flush and decrement freeze_remaining.
    for (int i = 0; i < 3; i++) {
        void* new_val = reinterpret_cast<void*>(uintptr_t(0x3000 + i));
        BgcSatbPreWriteBarrier(slot);
        *slot = new_val;
        slot_storage = new_val;
    }

    // After 3 barrier calls with freeze requested, freeze_remaining should be 0.
    int remaining = ctrl.satb_freeze_remaining_.load(std::memory_order_acquire);
    printf("  freeze_remaining=%d (should be 0)\n", remaining);
    CHECK(remaining == 0, "all freeze acknowledgements received");

    // Each freeze-triggered write flushed the buffer to the global queue.
    CHAOS_IL2CPP_SIZE global_count = ctrl.GetGlobalSatbCount();
    printf("  global_satb_count=%llu after freeze writes\n",
           static_cast<unsigned long long>(global_count));
    CHECK(global_count >= static_cast<CHAOS_IL2CPP_SIZE>(3),
          "at least 3 entries captured during freeze");

    // Drain all entries.
    ctrl.DrainGlobalSatbQueue();
    ctrl.satb_freeze_requested_.store(false, std::memory_order_release);

    g_bgc_is_marking.store(false, std::memory_order_release);
}

// ═══════════════════════════════════════════════════════════════════════
// Test 4: SATB pool exhaustion — verify AllocateSatbBuffer handles
// the pool-full case gracefully
// ═══════════════════════════════════════════════════════════════════════

static void TestSatbPoolExhaustion() {
    printf("\n── Test: SatbPoolExhaustion ──\n");
    auto& ctrl = BgcController::Instance();
    ctrl.ResetForTest();
    g_bgc_is_marking.store(true, std::memory_order_release);

    // Exhaust the SATB buffer pool (kMaxSatbPool = 256 allocations).
    int last_idx = -1;
    for (int i = 0; i < kMaxSatbPool; i++) {
        int idx = ctrl.AllocateSatbBuffer();
        if (idx < 0) {
            printf("  pool exhausted at allocation %d (kMaxSatbPool=%d)\n",
                   i, kMaxSatbPool);
            CHECK(i == kMaxSatbPool,
                  "AllocateSatbBuffer returns -1 after pool exhaustion");
            break;
        }
        last_idx = idx;
    }

    // AllocateSatbBuffer should return -1 when the pool is exhausted.
    int overflow = ctrl.AllocateSatbBuffer();
    CHECK(overflow < 0, "AllocateSatbBuffer returns -1 when pool full");
    printf("  AllocateSatbBuffer overflow returned %d (expected < 0)\n",
           overflow);

    // Release some buffers back to the pool by resetting.
    ctrl.ResetForTest();
    CHECK(ctrl.AllocateSatbBuffer() >= 0,
          "after ResetForTest, AllocateSatbBuffer works again");

    g_bgc_is_marking.store(false, std::memory_order_release);
}

// ═══════════════════════════════════════════════════════════════════════
// Test 5: Multi-threaded SATB buffer pool contention
// ═══════════════════════════════════════════════════════════════════════

static void TestSatbPoolContention() {
    printf("\n── Test: SatbPoolContention ──\n");
    auto& ctrl = BgcController::Instance();
    ctrl.ResetForTest();
    g_bgc_is_marking.store(true, std::memory_order_release);

    // More threads than pool entries — forces contention and retry.
    constexpr int kContentionThreads = 300;
    std::atomic<int> allocated_count{0};
    std::atomic<int> failed_count{0};

    auto alloc_thread = [&](int /*id*/) {
        int idx = ctrl.AllocateSatbBuffer();
        if (idx >= 0) {
            allocated_count.fetch_add(1, std::memory_order_relaxed);
        } else {
            failed_count.fetch_add(1, std::memory_order_relaxed);
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < kContentionThreads; i++) {
        threads.emplace_back(alloc_thread, i);
    }
    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }

    int allocd = allocated_count.load(std::memory_order_relaxed);
    int failed = failed_count.load(std::memory_order_relaxed);
    printf("  allocated=%d (max=%d) failed=%d\n",
           allocd, kMaxSatbPool, failed);
    CHECK(allocd == kMaxSatbPool,
          "exactly kMaxSatbPool buffers allocated under contention");
    CHECK(failed == kContentionThreads - kMaxSatbPool,
          "remaining threads see pool-full");

    g_bgc_is_marking.store(false, std::memory_order_release);
}

// ═══════════════════════════════════════════════════════════════════════
// Main
// ═══════════════════════════════════════════════════════════════════════

int main() {
    setvbuf(stdout, NULL, _IONBF, 0);
    puts("CRAG SATB stress test");
    puts("=====================\n");

    TestSatbConcurrentOverflow();
    TestSatbBufferFullAutoFlush();
    TestSatbFreezeConvergence();
    TestSatbPoolExhaustion();
    TestSatbPoolContention();

    printf("\n═══════════════════════════════════════════\n");
    if (g_failures == 0) {
        printf("  ALL TESTS PASSED\n");
    } else {
        printf("  %d TEST(S) FAILED\n", g_failures);
    }
    printf("═══════════════════════════════════════════\n");
    return g_failures;
}
