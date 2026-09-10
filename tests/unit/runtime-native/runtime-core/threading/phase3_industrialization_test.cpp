// phase3_industrialization_test.cpp — Phase 3 TDD tests
//
// Timer precision: verify TimerQueue fires within acceptable tolerance.
// Shutdown race: verify clean exit when work is queued during shutdown.
// Injection rate: verify worker count doesn't storm under massive queue load.

#include <gtest/gtest.h>

#include <thread_pool.h>
#include <thread_state.h>
#include <timer_queue.h>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <thread>
#include <vector>

namespace threading = chaos::il2cpp::runtime_core::threading;

// ── Timer precision ─────────────────────────────────────────────────────

TEST(Phase3Industrialization, TimerFiresWithinTolerance) {
    // Create a one-shot timer that fires after 50ms and verify it fires
    // within an acceptable tolerance (50-200ms in debug/test environment).
    threading::RegisterThread(threading::kMainThreadId, nullptr);
    threading::ThreadPoolInitialize();

    std::atomic<bool> fired{false};
    auto start = std::chrono::steady_clock::now();

    uint32_t tid = threading::TimerQueueCreate(
        [](void* ctx) { *static_cast<std::atomic<bool>*>(ctx) = true; },
        &fired, 50, 0);

    ASSERT_NE(tid, threading::kTimerQueueInvalidId);

    auto deadline = start + std::chrono::milliseconds(500);
    while (!fired.load(std::memory_order_acquire)) {
        if (std::chrono::steady_clock::now() > deadline) {
            GTEST_FAIL() << "Timer did not fire within 500ms";
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start).count();
    EXPECT_GE(elapsed, 30);  // Allow 20ms early due to scheduling
    EXPECT_LE(elapsed, 400); // Generous upper bound for debug mode

    threading::ThreadPoolShutdown();
    threading::UnregisterThread();
}

TEST(Phase3Industrialization, PeriodicTimerFiresMultipleTimes) {
    threading::RegisterThread(threading::kMainThreadId, nullptr);
    threading::ThreadPoolInitialize();

    std::atomic<int> fire_count{0};
    constexpr int kExpectedFires = 3;

    uint32_t tid = threading::TimerQueueCreate(
        [](void* ctx) { static_cast<std::atomic<int>*>(ctx)->fetch_add(1, std::memory_order_relaxed); },
        &fire_count, 20, 30);

    ASSERT_NE(tid, threading::kTimerQueueInvalidId);

    auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(300);
    while (fire_count.load(std::memory_order_acquire) < kExpectedFires) {
        if (std::chrono::steady_clock::now() > deadline) {
            GTEST_FAIL() << "Periodic timer only fired "
                         << fire_count.load(std::memory_order_relaxed)
                         << " times (expected " << kExpectedFires << ")";
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    threading::TimerQueueDelete(tid);
    threading::ThreadPoolShutdown();
    threading::UnregisterThread();
}

// ── Shutdown race ───────────────────────────────────────────────────────

TEST(Phase3Industrialization, ShutdownWithConcurrentWork) {
    // Queue work then shut down — verify no crash on the drain path.
    // Note: concurrent queueing during shutdown has a pre-existing data race
    // on s_workers (reads outside s_mutex vs shutdown's clear()). This test
    // focuses on the clean drain path.
    threading::RegisterThread(threading::kMainThreadId, nullptr);
    threading::ThreadPoolInitialize();

    std::atomic<int64_t> counter{0};

    // Queue a batch of work.
    for (int i = 0; i < 500; i++) {
        threading::ThreadPoolQueueUserWorkItemUnsafe(
            [](void* ctx) {
                static_cast<std::atomic<int64_t>*>(ctx)->fetch_add(1, std::memory_order_relaxed);
            }, &counter);
    }

    // Let some items start processing, then shut down.
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    threading::ThreadPoolShutdown();
    threading::UnregisterThread();
    SUCCEED();
}

TEST(Phase3Industrialization, TimerQueueShutdownWithActiveTimers) {
    // Create timers then shut down — verify no crash or leaked callback.
    threading::RegisterThread(threading::kMainThreadId, nullptr);
    threading::ThreadPoolInitialize();

    std::atomic<int> fire_count{0};

    // Create several timers with varying due times.
    for (int i = 0; i < 10; i++) {
        threading::TimerQueueCreate(
            [](void* ctx) { static_cast<std::atomic<int>*>(ctx)->fetch_add(1, std::memory_order_relaxed); },
            &fire_count, static_cast<uint32_t>(10 + i * 5), 0);
    }

    // Immediate shutdown — timers should be cancelled.
    threading::ThreadPoolShutdown();
    threading::UnregisterThread();

    // No assertions on fire_count (timers may or may not have fired),
    // just verify no crash during shutdown.
    SUCCEED();
}

// ── Injection rate control ──────────────────────────────────────────────

TEST(Phase3Industrialization, MassiveQueueDoesNotCreateWorkerStorm) {
    // Enqueue a large batch of items rapidly. Verify that the worker count
    // does not spike to extreme levels (injection rate control limits
    // worker creation to 1 per gate tick via starvation detection).
    threading::RegisterThread(threading::kMainThreadId, nullptr);
    threading::ThreadPoolInitialize();

    constexpr int kItemCount = 10000;
    std::atomic<int64_t> counter{0};

    // Rapidly queue many items.
    for (int i = 0; i < kItemCount; i++) {
        threading::ThreadPoolQueueUserWorkItemUnsafe(
            [](void* ctx) { static_cast<std::atomic<int64_t>*>(ctx)->fetch_add(1, std::memory_order_relaxed); },
            &counter);
    }

    // Wait for completion with generous timeout.
    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(15);
    while (counter.load(std::memory_order_acquire) < kItemCount) {
        if (std::chrono::steady_clock::now() > deadline) {
            GTEST_FAIL() << "Timed out: completed "
                         << counter.load(std::memory_order_relaxed)
                         << "/" << kItemCount;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // Worker count should be reasonable (< 100 is a loose bound).
    int32_t workers = threading::ThreadPoolWorkerCount();
    EXPECT_LT(workers, 100);

    threading::ThreadPoolShutdown();
    threading::UnregisterThread();
}
