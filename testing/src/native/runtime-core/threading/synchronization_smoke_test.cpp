// synchronization_smoke_test.cpp — SemaphoreSlim, ReaderWriterLockSlim,
// WaitHandle (ManualResetEvent + AutoResetEvent), WaitAny/WaitAll, TimerQueue
//
// Migration from tests/contracts/native/runtime-core/synchronization_smoke.cpp
// to GoogleTest.  No GC dependency — all tests use raw TEST().

#include <gtest/gtest.h>

#include <synchronization.h>
#include <wait_handle.h>
#include <timer_queue.h>
#include <thread_state.h>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <thread>

namespace threading = chaos::il2cpp::runtime_core::threading;

// ── SemaphoreSlim tests ───────────────────────────────────────────────────

TEST(SynchronizationSmoke, SemaphoreCreateDestroy)
{
    uint32_t sem = threading::SemaphoreSlimCreate(1, 5);
    ASSERT_NE(sem, 0u);

    EXPECT_TRUE(threading::SemaphoreSlimDestroy(sem));

    // Double destroy must return false.
    EXPECT_FALSE(threading::SemaphoreSlimDestroy(sem));
}

TEST(SynchronizationSmoke, SemaphoreWaitRelease)
{
    uint32_t sem = threading::SemaphoreSlimCreate(0, 5);
    ASSERT_NE(sem, 0u);

    // Poll: should timeout (count == 0).
    EXPECT_EQ(threading::SemaphoreSlimWait(sem, 0), 0);

    // Release 2.
    EXPECT_EQ(threading::SemaphoreSlimRelease(sem, 2), 0);

    // Acquire 1.
    EXPECT_EQ(threading::SemaphoreSlimWait(sem, 100), 1);

    // Release more than max.
    EXPECT_EQ(threading::SemaphoreSlimRelease(sem, 5), -1);

    threading::SemaphoreSlimDestroy(sem);
}

TEST(SynchronizationSmoke, SemaphoreMultiThread)
{
    uint32_t sem = threading::SemaphoreSlimCreate(0, 10);
    ASSERT_NE(sem, 0u);

    std::atomic<int> consumed{0};
    constexpr int kItems = 5;

    std::thread consumer([&] {
        for (int i = 0; i < kItems; ++i) {
            int r = threading::SemaphoreSlimWait(sem, 2000);
            ASSERT_EQ(r, 1) << "consumer wait returned " << r;
            consumed.fetch_add(1, std::memory_order_relaxed);
        }
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    for (int i = 0; i < kItems; ++i) {
        threading::SemaphoreSlimRelease(sem, 1);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    consumer.join();
    threading::SemaphoreSlimDestroy(sem);

    EXPECT_EQ(consumed.load(), kItems);
}

// ── ReaderWriterLockSlim tests ────────────────────────────────────────────

TEST(SynchronizationSmoke, RWLockBasic)
{
    uint32_t rw = threading::ReaderWriterLockSlimCreate();
    ASSERT_NE(rw, 0u);

    // Multiple readers.
    EXPECT_EQ(threading::ReaderWriterLockSlimEnterRead(rw, 100), 1);
    EXPECT_EQ(threading::ReaderWriterLockSlimEnterRead(rw, 100), 1);

    // Exit both readers.
    EXPECT_TRUE(threading::ReaderWriterLockSlimExitRead(rw));
    EXPECT_TRUE(threading::ReaderWriterLockSlimExitRead(rw));

    // Write lock.
    EXPECT_EQ(threading::ReaderWriterLockSlimEnterWrite(rw, 100), 1);
    EXPECT_TRUE(threading::ReaderWriterLockSlimExitWrite(rw));

    threading::ReaderWriterLockSlimDestroy(rw);
}

TEST(SynchronizationSmoke, RWLockExclusion)
{
    uint32_t rw = threading::ReaderWriterLockSlimCreate();
    ASSERT_NE(rw, 0u);

    // Acquire write lock.
    EXPECT_EQ(threading::ReaderWriterLockSlimEnterWrite(rw, 100), 1);

    // Try read lock while writer active — should timeout (poll).
    EXPECT_EQ(threading::ReaderWriterLockSlimEnterRead(rw, 0), 0);

    threading::ReaderWriterLockSlimExitWrite(rw);
    threading::ReaderWriterLockSlimDestroy(rw);
}

TEST(SynchronizationSmoke, RWLockDestroy)
{
    uint32_t rw = threading::ReaderWriterLockSlimCreate();
    ASSERT_NE(rw, 0u);

    EXPECT_TRUE(threading::ReaderWriterLockSlimDestroy(rw));

    // Double destroy must return false.
    EXPECT_FALSE(threading::ReaderWriterLockSlimDestroy(rw));

    // Operations on destroyed lock should error.
    EXPECT_EQ(threading::ReaderWriterLockSlimEnterRead(rw, 0), -1);
}

// ── WaitHandle (ManualResetEvent) tests ───────────────────────────────────

TEST(SynchronizationSmoke, ManualResetBasic)
{
    uint32_t wh = threading::WaitHandleCreate(
        false, threading::WaitHandleType::ManualResetEvent);
    ASSERT_NE(wh, threading::kInvalidWaitHandle);

    // Not signalled — poll should timeout.
    EXPECT_EQ(threading::WaitHandleWaitOne(wh, 0), 0);

    // Set and wait.
    EXPECT_TRUE(threading::WaitHandleSet(wh));
    EXPECT_EQ(threading::WaitHandleWaitOne(wh, 100), 1);

    // ManualResetEvent stays signalled — second wait should also succeed.
    EXPECT_EQ(threading::WaitHandleWaitOne(wh, 100), 1);

    // Reset.
    EXPECT_TRUE(threading::WaitHandleReset(wh));
    EXPECT_EQ(threading::WaitHandleWaitOne(wh, 0), 0);

    threading::WaitHandleClose(wh);
}

// ── WaitHandle (AutoResetEvent) tests ─────────────────────────────────────

TEST(SynchronizationSmoke, AutoResetBasic)
{
    uint32_t wh = threading::WaitHandleCreate(
        true, threading::WaitHandleType::AutoResetEvent);
    ASSERT_NE(wh, threading::kInvalidWaitHandle);

    // Initially signalled — wait should consume and succeed.
    EXPECT_EQ(threading::WaitHandleWaitOne(wh, 100), 1);

    // Signal consumed — poll should timeout.
    EXPECT_EQ(threading::WaitHandleWaitOne(wh, 0), 0);

    threading::WaitHandleClose(wh);
}

// ── WaitAny / WaitAll tests ───────────────────────────────────────────────

TEST(SynchronizationSmoke, WaitAny)
{
    uint32_t wh1 = threading::WaitHandleCreate(
        false, threading::WaitHandleType::ManualResetEvent);
    uint32_t wh2 = threading::WaitHandleCreate(
        false, threading::WaitHandleType::ManualResetEvent);
    ASSERT_NE(wh1, threading::kInvalidWaitHandle);
    ASSERT_NE(wh2, threading::kInvalidWaitHandle);

    uint32_t handles[2] = { wh1, wh2 };

    // Neither signalled — poll returns -1.
    EXPECT_EQ(threading::WaitHandleWaitAny(handles, 2, 0), -1);

    // Signal wh2.
    threading::WaitHandleSet(wh2);
    int idx = threading::WaitHandleWaitAny(handles, 2, 100);
    EXPECT_EQ(idx, 1);

    threading::WaitHandleClose(wh1);
    threading::WaitHandleClose(wh2);
}

TEST(SynchronizationSmoke, WaitAnyMultiThread)
{
    // Multi-threaded WaitAny: one thread waits on two AutoResetEvents;
    // a second thread signals one of them.  Verifies signal-based dispatch
    // (waiter registration + Set → direct wakeup).
    uint32_t ev1 = threading::WaitHandleCreate(
        false, threading::WaitHandleType::AutoResetEvent);
    uint32_t ev2 = threading::WaitHandleCreate(
        false, threading::WaitHandleType::AutoResetEvent);
    ASSERT_NE(ev1, threading::kInvalidWaitHandle);
    ASSERT_NE(ev2, threading::kInvalidWaitHandle);

    uint32_t handles[2] = { ev1, ev2 };
    std::atomic<int> result_idx{-2};
    std::atomic<bool> started{false};

    std::thread waiter([&] {
        started.store(true, std::memory_order_release);
        int r = threading::WaitHandleWaitAny(handles, 2, 2000);
        result_idx.store(r, std::memory_order_release);
    });

    // Wait for waiter to register, then signal ev2.
    while (!started.load(std::memory_order_acquire)) {
        std::this_thread::yield();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(threading::WaitHandleSet(ev2));

    waiter.join();

    // Waiter should have woken and found ev2 signalled (index 1).
    EXPECT_EQ(result_idx.load(std::memory_order_acquire), 1);

    // ev2 was consumed by AutoResetEvent — poll should timeout.
    EXPECT_EQ(threading::WaitHandleWaitOne(ev2, 0), 0);

    threading::WaitHandleClose(ev1);
    threading::WaitHandleClose(ev2);
}

TEST(SynchronizationSmoke, WaitAnyTimeout)
{
    // WaitAny with timeout that expires — should return -1.
    uint32_t ev1 = threading::WaitHandleCreate(
        false, threading::WaitHandleType::AutoResetEvent);
    uint32_t ev2 = threading::WaitHandleCreate(
        false, threading::WaitHandleType::AutoResetEvent);
    ASSERT_NE(ev1, threading::kInvalidWaitHandle);
    ASSERT_NE(ev2, threading::kInvalidWaitHandle);

    uint32_t handles[2] = { ev1, ev2 };
    EXPECT_EQ(threading::WaitHandleWaitAny(handles, 2, 50), -1);

    threading::WaitHandleClose(ev1);
    threading::WaitHandleClose(ev2);
}

TEST(SynchronizationSmoke, WaitAnyAutoResetConsumed)
{
    // AutoResetEvent via WaitAny: signal wakes one waiter and consumes
    // the signal; a second waiter should not see it.
    uint32_t ev = threading::WaitHandleCreate(
        false, threading::WaitHandleType::AutoResetEvent);
    ASSERT_NE(ev, threading::kInvalidWaitHandle);

    std::atomic<int> result1{-2};
    std::atomic<int> result2{-2};

    std::thread t1([&] {
        uint32_t h[1] = { ev };
        result1.store(threading::WaitHandleWaitAny(h, 1, 500), std::memory_order_release);
    });
    std::thread t2([&] {
        uint32_t h[1] = { ev };
        result2.store(threading::WaitHandleWaitAny(h, 1, 500), std::memory_order_release);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(threading::WaitHandleSet(ev));

    t1.join();
    t2.join();

    // Exactly one waiter should have woken with index 0; the other timed out.
    int r1 = result1.load(std::memory_order_acquire);
    int r2 = result2.load(std::memory_order_acquire);
    EXPECT_TRUE((r1 == 0 && r2 == -1) || (r1 == -1 && r2 == 0));

    threading::WaitHandleClose(ev);
}

TEST(SynchronizationSmoke, WaitAll)
{
    uint32_t wh1 = threading::WaitHandleCreate(
        false, threading::WaitHandleType::ManualResetEvent);
    uint32_t wh2 = threading::WaitHandleCreate(
        false, threading::WaitHandleType::ManualResetEvent);
    ASSERT_NE(wh1, threading::kInvalidWaitHandle);
    ASSERT_NE(wh2, threading::kInvalidWaitHandle);

    uint32_t handles[2] = { wh1, wh2 };

    // Poll not all.
    EXPECT_EQ(threading::WaitHandleWaitAll(handles, 2, 0), -1);

    // Signal one.
    threading::WaitHandleSet(wh1);
    EXPECT_EQ(threading::WaitHandleWaitAll(handles, 2, 0), -1);

    // Signal both.
    threading::WaitHandleSet(wh2);
    EXPECT_EQ(threading::WaitHandleWaitAll(handles, 2, 100), 0);

    threading::WaitHandleClose(wh1);
    threading::WaitHandleClose(wh2);
}

// ── TimerQueue tests ──────────────────────────────────────────────────────

static std::atomic<int> s_timer_fired{0};
static void timer_callback(void* ctx) {
    int val = static_cast<int>(reinterpret_cast<std::intptr_t>(ctx));
    s_timer_fired.fetch_add(val, std::memory_order_relaxed);
}

TEST(SynchronizationSmoke, TimerOneShot)
{
    threading::TimerQueueInitialize();

    s_timer_fired.store(0, std::memory_order_relaxed);
    uint32_t tid = threading::TimerQueueCreate(
        timer_callback,
        reinterpret_cast<void*>(static_cast<std::intptr_t>(1)),
        50, 0);
    ASSERT_NE(tid, threading::kTimerQueueInvalidId);

    // Tick 3 times with 30ms gaps to cover the 50ms due time.
    for (int i = 0; i < 3; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        threading::TimerQueueOnTick();
    }

    threading::TimerQueueShutdown();

    EXPECT_EQ(s_timer_fired.load(), 1);
}

TEST(SynchronizationSmoke, TimerPeriodic)
{
    threading::TimerQueueInitialize();

    s_timer_fired.store(0, std::memory_order_relaxed);
    uint32_t tid = threading::TimerQueueCreate(
        timer_callback,
        reinterpret_cast<void*>(static_cast<std::intptr_t>(1)),
        30, 50);
    ASSERT_NE(tid, threading::kTimerQueueInvalidId);

    // Tick enough times to cover 2-3 fires.
    for (int i = 0; i < 5; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
        threading::TimerQueueOnTick();
    }

    int fired = s_timer_fired.load();
    threading::TimerQueueShutdown();

    EXPECT_GE(fired, 2);
}
