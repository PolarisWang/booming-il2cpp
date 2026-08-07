// threading_stress_test.cpp — Phase 2 TDD: multi-threaded stress tests
//
// Stress tests for synchronization primitives with 8+ threads:
//   - RWLock: concurrent readers + writers
//   - Semaphore: concurrent wait/release
//   - Barrier: concurrent phases
//   - CountdownEvent: concurrent signal/wait
//
// These tests verify no crashes, deadlocks, or data races under load.

#include <gtest/gtest.h>

#include <synchronization.h>
#include <thread_pool.h>
#include <thread_state.h>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <thread>
#include <vector>

namespace threading = chaos::il2cpp::runtime_core::threading;

// ── RWLock stress: 8+ threads, mixed read/write ───────────────────────

TEST(ThreadingStress, RWLockMixedReadWrite)
{
    constexpr int kThreads = 8;
    constexpr int kIterations = 1000;

    uint32_t rw = threading::ReaderWriterLockSlimCreate();
    ASSERT_NE(rw, 0u);

    std::atomic<int64_t> shared_counter{0};
    std::atomic<int> ready{0};
    std::atomic<bool> stop{false};

    std::vector<std::thread> threads;
    for (int t = 0; t < kThreads; t++) {
        threads.emplace_back([&, t] {
            threading::RegisterThread(threading::AllocateThreadId(), nullptr);
            ready.fetch_add(1, std::memory_order_release);

            for (int i = 0; i < kIterations; i++) {
                if (t == 0) {
                    // Writer: exclusive access.
                    if (threading::ReaderWriterLockSlimEnterWrite(rw, -1) == 1) {
                        shared_counter.store(shared_counter.load(std::memory_order_relaxed) + 1,
                                             std::memory_order_relaxed);
                        threading::ReaderWriterLockSlimExitWrite(rw);
                    }
                } else {
                    // Reader: shared access.
                    if (threading::ReaderWriterLockSlimEnterRead(rw, -1) == 1) {
                        int64_t val = shared_counter.load(std::memory_order_relaxed);
                        (void)val;
                        threading::ReaderWriterLockSlimExitRead(rw);
                    }
                }
            }
            threading::UnregisterThread();
        });
    }

    // Wait for all to complete.
    for (auto& t : threads) t.join();

    // Cleanup.
    threading::ReaderWriterLockSlimDestroy(rw);

    // Writer incremented kIterations times.
    EXPECT_EQ(shared_counter.load(std::memory_order_relaxed), kIterations);
}

TEST(ThreadingStress, RWLockAllWriters)
{
    // All 8 threads are writers — verifies exclusive access.
    constexpr int kThreads = 8;
    constexpr int kIterations = 500;

    uint32_t rw = threading::ReaderWriterLockSlimCreate();
    ASSERT_NE(rw, 0u);

    std::atomic<int64_t> counter{0};
    std::atomic<int> ready{0};
    std::vector<std::thread> threads;

    for (int t = 0; t < kThreads; t++) {
        threads.emplace_back([&] {
            threading::RegisterThread(threading::AllocateThreadId(), nullptr);
            ready.fetch_add(1, std::memory_order_release);

            for (int i = 0; i < kIterations; i++) {
                if (threading::ReaderWriterLockSlimEnterWrite(rw, -1) == 1) {
                    int64_t val = counter.load(std::memory_order_relaxed);
                    counter.store(val + 1, std::memory_order_relaxed);
                    threading::ReaderWriterLockSlimExitWrite(rw);
                }
            }
            threading::UnregisterThread();
        });
    }

    for (auto& t : threads) t.join();
    threading::ReaderWriterLockSlimDestroy(rw);

    EXPECT_EQ(counter.load(std::memory_order_relaxed), kThreads * kIterations);
}

// ── Semaphore stress: 8 threads ───────────────────────────────────────

TEST(ThreadingStress, SemaphoreMultiThread)
{
    constexpr int kThreads = 8;
    constexpr int kIterations = 500;

    uint32_t sem = threading::SemaphoreSlimCreate(0, kThreads * kIterations);
    ASSERT_NE(sem, 0u);

    std::atomic<int> ready{0};
    std::atomic<int64_t> counter{0};
    std::vector<std::thread> threads;

    for (int t = 0; t < kThreads; t++) {
        threads.emplace_back([&] {
            threading::RegisterThread(threading::AllocateThreadId(), nullptr);
            ready.fetch_add(1, std::memory_order_release);

            for (int i = 0; i < kIterations; i++) {
                EXPECT_EQ(threading::SemaphoreSlimWait(sem, -1), 1);
                counter.fetch_add(1, std::memory_order_relaxed);
            }
            threading::UnregisterThread();
        });
    }

    // Wait for all threads to be ready, then release enough count.
    while (ready.load(std::memory_order_acquire) < kThreads) {
        std::this_thread::yield();
    }
    threading::SemaphoreSlimRelease(sem, kThreads * kIterations);

    for (auto& t : threads) t.join();
    threading::SemaphoreSlimDestroy(sem);

    EXPECT_EQ(counter.load(std::memory_order_relaxed), kThreads * kIterations);
}

// ── Barrier stress: 8 threads, multiple phases ────────────────────────

TEST(ThreadingStress, BarrierMultiplePhases)
{
    constexpr int kThreads = 8;
    constexpr int kPhases = 100;

    uint32_t barrier = threading::BarrierCreate(kThreads);
    ASSERT_NE(barrier, 0u);

    std::atomic<int> ready{0};
    std::vector<std::thread> threads;

    for (int t = 0; t < kThreads; t++) {
        threads.emplace_back([&] {
            threading::RegisterThread(threading::AllocateThreadId(), nullptr);
            ready.fetch_add(1, std::memory_order_release);

            for (int p = 0; p < kPhases; p++) {
                EXPECT_EQ(threading::BarrierSignalAndWait(barrier, -1), 1);
            }
            threading::UnregisterThread();
        });
    }

    for (auto& t : threads) t.join();
    threading::BarrierDestroy(barrier);
}

// ── CountdownEvent stress ────────────────────────────────────────────

TEST(ThreadingStress, CountdownEventMultiSignal)
{
    constexpr int kThreads = 8;

    uint32_t ce = threading::CountdownEventCreate(kThreads);
    ASSERT_NE(ce, 0u);

    std::atomic<int> ready{0};
    std::vector<std::thread> threads;

    for (int t = 0; t < kThreads; t++) {
        threads.emplace_back([&] {
            threading::RegisterThread(threading::AllocateThreadId(), nullptr);
            ready.fetch_add(1, std::memory_order_release);
            threading::CountdownEventSignal(ce, 1);

            // Wait for event to be signaled (count reaches 0).
            EXPECT_EQ(threading::CountdownEventWait(ce, -1), 1);
            threading::UnregisterThread();
        });
    }

    for (auto& t : threads) t.join();
    threading::CountdownEventDestroy(ce);
}

// ── High-frequency queue operations ──────────────────────────────────

TEST(ThreadingStress, RapidQueueDrain)
{
    // Rapidly enqueue items while ThreadPool workers drain them.
    threading::RegisterThread(threading::kMainThreadId, nullptr);
    threading::ThreadPoolInitialize();

    constexpr int kBatchSize = 5000;
    constexpr int kBatches = 5;

    for (int b = 0; b < kBatches; b++) {
        std::atomic<int64_t> counter{0};

        for (int i = 0; i < kBatchSize; i++) {
            threading::ThreadPoolQueueUserWorkItem(
                [](void* ctx) {
                    auto* c = static_cast<std::atomic<int64_t>*>(ctx);
                    c->fetch_add(1, std::memory_order_relaxed);
                }, &counter);
        }

        auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);
        while (counter.load(std::memory_order_acquire) < kBatchSize) {
            if (std::chrono::steady_clock::now() > deadline) {
                GTEST_FAIL() << "Batch " << b << " timed out: "
                             << counter.load(std::memory_order_relaxed) << "/" << kBatchSize;
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        EXPECT_EQ(counter.load(std::memory_order_relaxed), kBatchSize);
    }

    threading::ThreadPoolShutdown();
    threading::UnregisterThread();
}
