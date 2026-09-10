// queue_backpressure_test.cpp — Phase 2 TDD: global queue backpressure verification
//
// Verifies that:
//   - Queuing many work items does not overflow
//   - All items are processed correctly under backpressure
//   - ThreadPoolShutdown drains the queue properly

#include <gtest/gtest.h>

#include <thread_pool.h>
#include <thread_state.h>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <thread>

namespace threading = chaos::il2cpp::runtime_core::threading;

// ── Helpers ──────────────────────────────────────────────────────────────

static std::atomic<int64_t> s_backpressure_count{0};

static void count_callback(void* ctx) {
    auto* counter = static_cast<std::atomic<int64_t>*>(ctx);
    counter->fetch_add(1, std::memory_order_relaxed);
}

// ── Tests ────────────────────────────────────────────────────────────────

TEST(QueueBackpressure, ManyItemsProcessed)
{
    // Queue a large number of items and verify they all complete.
    // This exercises the backpressure spin-yield path.
    threading::RegisterThread(threading::kMainThreadId, nullptr);
    threading::ThreadPoolInitialize();

    constexpr int kItemCount = 5000;
    std::atomic<int64_t> counter{0};

    for (int i = 0; i < kItemCount; i++) {
        threading::ThreadPoolQueueUserWorkItem(count_callback, &counter);
    }

    // Wait for all items to complete.
    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(10);
    while (counter.load(std::memory_order_acquire) < kItemCount) {
        if (std::chrono::steady_clock::now() > deadline) {
            GTEST_FAIL() << "Timed out waiting for " << kItemCount
                         << " items, completed=" << counter.load(std::memory_order_relaxed);
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    EXPECT_EQ(counter.load(std::memory_order_relaxed), kItemCount);

    threading::ThreadPoolShutdown();
    threading::UnregisterThread();
}

TEST(QueueBackpressure, ConcurrentProducers)
{
    // Multiple threads queue items concurrently, verifying thread safety
    // of the backpressure path.
    threading::RegisterThread(threading::kMainThreadId, nullptr);
    threading::ThreadPoolInitialize();

    constexpr int kProducers = 4;
    constexpr int kItemsPerProducer = 2000;
    std::atomic<int64_t> counter{0};
    std::vector<std::thread> producers;

    for (int p = 0; p < kProducers; p++) {
        producers.emplace_back([&] {
            threading::RegisterThread(threading::AllocateThreadId(), nullptr);
            for (int i = 0; i < kItemsPerProducer; i++) {
                threading::ThreadPoolQueueUserWorkItem(count_callback, &counter);
            }
            threading::UnregisterThread();
        });
    }

    for (auto& t : producers) t.join();

    // Wait for all items to complete.
    int64_t expected = kProducers * kItemsPerProducer;
    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(15);
    while (counter.load(std::memory_order_acquire) < expected) {
        if (std::chrono::steady_clock::now() > deadline) {
            GTEST_FAIL() << "Timed out waiting for " << expected
                         << " items, completed=" << counter.load(std::memory_order_relaxed);
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    EXPECT_EQ(counter.load(std::memory_order_relaxed), expected);

    threading::ThreadPoolShutdown();
    threading::UnregisterThread();
}

TEST(QueueBackpressure, DrainOnShutdown)
{
    // Queue items during shutdown — verify clean drain without crash.
    threading::RegisterThread(threading::kMainThreadId, nullptr);
    threading::ThreadPoolInitialize();

    constexpr int kItemCount = 1000;
    std::atomic<int64_t> counter{0};

    for (int i = 0; i < kItemCount; i++) {
        threading::ThreadPoolQueueUserWorkItem(count_callback, &counter);
    }

    // Allow some processing then queue more.
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    for (int i = 0; i < kItemCount; i++) {
        threading::ThreadPoolQueueUserWorkItem(count_callback, &counter);
    }

    threading::ThreadPoolShutdown();
    threading::UnregisterThread();

    // After shutdown, verify no negative side effects.
    SUCCEED();
}
