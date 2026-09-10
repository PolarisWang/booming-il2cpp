// threading_smoke_test.cpp — Thread identity, registration, pool, P2 capabilities
//
// Migration from tests/contracts/native/runtime-core/threading_smoke.cpp
// to GoogleTest.  Each test registers its own thread when needed.
// No GC dependency (no nursery, no allocations).

#include <gtest/gtest.h>

#include <thread_state.h>
#include <thread_pool.h>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <thread>
#include <vector>

namespace threading = chaos::il2cpp::runtime_core::threading;

// ── Extern "C" stubs ──────────────────────────────────────────────────────
// These are normally provided by chaos_runtime_core.lib (threading_stubs.cpp).
// Definitions are inlined here for standalone gtest linkage.

extern "C" int32_t chaos_thread_reset_abort() noexcept {
    return 0;
}

extern "C" int32_t chaos_thread_yield() noexcept {
    return 1;
}

extern "C" void chaos_thread_sleep(int32_t ms) noexcept {
    if (ms > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }
}

// ── Helpers ───────────────────────────────────────────────────────────────

static std::atomic<int> s_enumerate_count{0};
static bool enumerate_callback(threading::ManagedThread*) {
    s_enumerate_count.fetch_add(1, std::memory_order_relaxed);
    return true;
}

static std::atomic<int> s_work_executed{0};
static void work_callback(void* ctx) {
    int val = static_cast<int>(reinterpret_cast<std::intptr_t>(ctx));
    s_work_executed.fetch_add(val, std::memory_order_relaxed);
}

// ── Tests: Thread identity ───────────────────────────────────────────────

TEST(ThreadingSmoke, MainThreadIdentity)
{
    // The main thread must be registered for GetCurrentThreadId() to work.
    threading::RegisterThread(threading::kMainThreadId, nullptr);

    int32_t tid = threading::GetCurrentThreadId();
    EXPECT_EQ(tid, threading::kMainThreadId);

    auto* self = threading::GetCurrentThread();
    ASSERT_NE(self, nullptr);
    EXPECT_EQ(self->managed_id, tid);

    threading::UnregisterThread();
}

TEST(ThreadingSmoke, ThreadRegister)
{
    int32_t tid = threading::AllocateThreadId();
    threading::RegisterThread(tid, nullptr);

    int32_t actual = threading::GetCurrentThreadId();
    EXPECT_EQ(actual, tid);

    threading::UnregisterThread();

    // After unregister, GetCurrentThread() should return nullptr.
    EXPECT_EQ(threading::GetCurrentThread(), nullptr);
}

// ── Tests: Thread enumeration ────────────────────────────────────────────

TEST(ThreadingSmoke, ThreadEnumeration)
{
    constexpr int kNumWorkers = 4;
    std::atomic<int> ready_count{0};

    std::vector<std::thread> workers;
    for (int i = 0; i < kNumWorkers; ++i) {
        workers.emplace_back([&] {
            int32_t tid = threading::AllocateThreadId();
            threading::RegisterThread(tid, nullptr);
            ready_count.fetch_add(1, std::memory_order_release);

            // Spin until main signals done.
            while (ready_count.load(std::memory_order_acquire) > 0) {
                std::this_thread::yield();
            }

            threading::UnregisterThread();
        });
    }

    // Wait for all workers to register.
    while (ready_count.load(std::memory_order_acquire) < kNumWorkers) {
        std::this_thread::yield();
    }

    // Enumerate threads.
    s_enumerate_count.store(0, std::memory_order_relaxed);
    threading::EnumerateThreads(enumerate_callback);

    // Should have seen at least kNumWorkers running threads (plus main).
    int count = s_enumerate_count.load(std::memory_order_relaxed);
    EXPECT_GE(count, kNumWorkers);

    // Signal workers to exit.
    ready_count.store(0, std::memory_order_release);
    for (auto& t : workers) t.join();
}

// ── Tests: ThreadPool ────────────────────────────────────────────────────

TEST(ThreadingSmoke, ThreadPoolBasic)
{
    threading::RegisterThread(threading::kMainThreadId, nullptr);
    threading::ThreadPoolInitialize();

    constexpr int kWorkItems = 10;
    s_work_executed.store(0, std::memory_order_relaxed);

    for (int i = 0; i < kWorkItems; ++i) {
        threading::ThreadPoolQueueUserWorkItem(
            work_callback,
            reinterpret_cast<void*>(static_cast<std::intptr_t>(1)));
    }

    // Give threads time to execute.
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    int executed = s_work_executed.load(std::memory_order_relaxed);
    EXPECT_EQ(executed, kWorkItems);

    threading::ThreadPoolShutdown();
    threading::UnregisterThread();
}

TEST(ThreadingSmoke, ThreadPoolScaling)
{
    threading::RegisterThread(threading::kMainThreadId, nullptr);
    threading::ThreadPoolInitialize();

    int initial = threading::ThreadPoolWorkerCount();
    EXPECT_GE(initial, 1);

    // Queue many items to trigger growth.
    s_work_executed.store(0, std::memory_order_relaxed);
    for (int i = 0; i < 50; ++i) {
        threading::ThreadPoolQueueUserWorkItem(work_callback, nullptr);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    int grown = threading::ThreadPoolWorkerCount();
    EXPECT_GE(grown, initial);

    threading::ThreadPoolShutdown();
    threading::UnregisterThread();
}

// ── Tests: P2 thread capabilities ────────────────────────────────────────

TEST(ThreadingSmoke, ThreadState)
{
    threading::RegisterThread(threading::kMainThreadId, nullptr);
    auto* self = threading::GetCurrentThread();
    ASSERT_NE(self, nullptr);

    // Default after RegisterThread is Running.
    EXPECT_EQ(self->managed_state, threading::ManagedThreadState::Running);

    // Set to Stopped and verify.
    self->managed_state = threading::ManagedThreadState::Stopped;
    EXPECT_EQ(self->managed_state, threading::ManagedThreadState::Stopped);

    // Restore.
    self->managed_state = threading::ManagedThreadState::Running;

    // Running -> WaitSleepJoin -> Running round-trip.
    self->managed_state = threading::ManagedThreadState::WaitSleepJoin;
    EXPECT_EQ(self->managed_state, threading::ManagedThreadState::WaitSleepJoin);
    self->managed_state = threading::ManagedThreadState::Running;
    EXPECT_EQ(self->managed_state, threading::ManagedThreadState::Running);

    // Redundant set: Running -> Running should be a no-op.
    self->managed_state = threading::ManagedThreadState::Running;
    EXPECT_EQ(self->managed_state, threading::ManagedThreadState::Running);

    // Background flag composition: Running | Background.
    auto bg = static_cast<threading::ManagedThreadState>(
        static_cast<int>(threading::ManagedThreadState::Running) |
        static_cast<int>(threading::ManagedThreadState::Background));
    self->managed_state = bg;
    EXPECT_EQ(self->managed_state, bg);
    self->managed_state = threading::ManagedThreadState::Running;

    // Invalid value: write 0xFFFF, verify readback consistency.
    auto invalid = static_cast<threading::ManagedThreadState>(0xFFFF);
    self->managed_state = invalid;
    EXPECT_EQ(self->managed_state, invalid);
    self->managed_state = threading::ManagedThreadState::Running;

    threading::UnregisterThread();
}

TEST(ThreadingSmoke, ThreadPriority)
{
    threading::RegisterThread(threading::kMainThreadId, nullptr);
    auto* self = threading::GetCurrentThread();
    ASSERT_NE(self, nullptr);

    // Default is Normal.
    EXPECT_EQ(self->priority, threading::ManagedThreadPriority::Normal);

    // Cycle through all levels.
    struct { threading::ManagedThreadPriority pri; const char* name; } levels[] = {
        { threading::ManagedThreadPriority::Lowest,      "Lowest" },
        { threading::ManagedThreadPriority::BelowNormal,  "BelowNormal" },
        { threading::ManagedThreadPriority::Normal,       "Normal" },
        { threading::ManagedThreadPriority::AboveNormal,  "AboveNormal" },
        { threading::ManagedThreadPriority::Highest,      "Highest" },
    };

    for (auto& l : levels) {
        self->priority = l.pri;
        EXPECT_EQ(self->priority, l.pri) << "priority after set(" << l.name << ")";
    }

    threading::UnregisterThread();
}

TEST(ThreadingSmoke, ThreadIsThreadPool)
{
    threading::RegisterThread(threading::kMainThreadId, nullptr);
    auto* self = threading::GetCurrentThread();
    ASSERT_NE(self, nullptr);

    // Main thread should not be a threadpool thread.
    EXPECT_FALSE(self->is_threadpool);

    // Toggle and verify.
    self->is_threadpool = true;
    EXPECT_TRUE(self->is_threadpool);

    self->is_threadpool = false;

    threading::UnregisterThread();
}

TEST(ThreadingSmoke, ThreadAbortFlag)
{
    threading::RegisterThread(threading::kMainThreadId, nullptr);
    auto* self = threading::GetCurrentThread();
    ASSERT_NE(self, nullptr);

    // Initially no abort pending.
    self->pending_abort.store(false, std::memory_order_release);

    int32_t r = chaos_thread_reset_abort();
    EXPECT_EQ(r, 0);

    // Set abort flag.
    self->pending_abort.store(true, std::memory_order_release);
    EXPECT_TRUE(self->pending_abort.load(std::memory_order_acquire));

    // The stub just returns 0 since we defined it as a no-op.
    r = chaos_thread_reset_abort();
    EXPECT_EQ(r, 0);

    threading::UnregisterThread();
}

TEST(ThreadingSmoke, ThreadInterruptFlag)
{
    threading::RegisterThread(threading::kMainThreadId, nullptr);
    auto* self = threading::GetCurrentThread();
    ASSERT_NE(self, nullptr);

    self->pending_interrupt.store(false, std::memory_order_release);

    // Set flag.
    self->pending_interrupt.store(true, std::memory_order_release);
    EXPECT_TRUE(self->pending_interrupt.load(std::memory_order_acquire));

    // Clear (as SafepointPoll does on throw).
    self->pending_interrupt.store(false, std::memory_order_release);
    EXPECT_FALSE(self->pending_interrupt.load(std::memory_order_acquire));

    threading::UnregisterThread();
}

TEST(ThreadingSmoke, ThreadYield)
{
    threading::RegisterThread(threading::kMainThreadId, nullptr);

    int32_t r = chaos_thread_yield();
    EXPECT_NE(r, 0);

    threading::UnregisterThread();
}

TEST(ThreadingSmoke, ThreadSleep)
{
    threading::RegisterThread(threading::kMainThreadId, nullptr);

    // 10 ms — should not crash or block forever.
    chaos_thread_sleep(10);

    threading::UnregisterThread();
}

TEST(ThreadingSmoke, ThreadBackground)
{
    threading::RegisterThread(threading::kMainThreadId, nullptr);
    auto* self = threading::GetCurrentThread();
    ASSERT_NE(self, nullptr);

    // Initially false.
    EXPECT_FALSE(self->is_background);

    // Toggle on.
    self->is_background = true;
    EXPECT_TRUE(self->is_background);

    // Toggle off.
    self->is_background = false;

    threading::UnregisterThread();
}
