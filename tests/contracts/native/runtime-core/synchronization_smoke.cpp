// synchronization_smoke.cpp — Smoke tests for SemaphoreSlim,
// ReaderWriterLockSlim, WaitHandle (ManualResetEvent + AutoResetEvent),
// and TimerQueue.
//
// Build: requires CHAOS_RUNTIME_ABI_EXPORTS for internal linkage.

#include "synchronization.h"
#include "wait_handle.h"
#include "timer_queue.h"
#include "thread_state.h"

#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <vector>

namespace threading = chaos::il2cpp::runtime_core::threading;

// ── SemaphoreSlim tests ────────────────────────────────────────────────

static bool test_semaphore_create_destroy() {
    uint32_t sem = threading::SemaphoreSlimCreate(1, 5);
    if (sem == 0) {
        std::fprintf(stderr, "FAIL: SemaphoreSlimCreate returned 0\n");
        return false;
    }
    if (!threading::SemaphoreSlimDestroy(sem)) {
        std::fprintf(stderr, "FAIL: SemaphoreSlimDestroy returned false\n");
        return false;
    }
    // Double destroy must return false.
    if (threading::SemaphoreSlimDestroy(sem)) {
        std::fprintf(stderr, "FAIL: double destroy returned true\n");
        return false;
    }
    return true;
}

static bool test_semaphore_wait_release() {
    uint32_t sem = threading::SemaphoreSlimCreate(0, 5);
    if (sem == 0) return false;

    // Poll: should timeout (count == 0).
    if (threading::SemaphoreSlimWait(sem, 0) != 0) {
        std::fprintf(stderr, "FAIL: poll should timeout\n");
        threading::SemaphoreSlimDestroy(sem);
        return false;
    }

    // Release 2.
    if (threading::SemaphoreSlimRelease(sem, 2) != 0) {
        std::fprintf(stderr, "FAIL: release failed\n");
        threading::SemaphoreSlimDestroy(sem);
        return false;
    }

    // Acquire 1.
    if (threading::SemaphoreSlimWait(sem, 100) != 1) {
        std::fprintf(stderr, "FAIL: wait after release should succeed\n");
        threading::SemaphoreSlimDestroy(sem);
        return false;
    }

    // Release more than max.
    if (threading::SemaphoreSlimRelease(sem, 5) != -1) {
        std::fprintf(stderr, "FAIL: release beyond max should fail\n");
        threading::SemaphoreSlimDestroy(sem);
        return false;
    }

    threading::SemaphoreSlimDestroy(sem);
    return true;
}

static bool test_semaphore_multi_thread() {
    uint32_t sem = threading::SemaphoreSlimCreate(0, 10);
    if (sem == 0) return false;

    std::atomic<int> consumed{0};
    constexpr int kItems = 5;

    std::thread consumer([&] {
        for (int i = 0; i < kItems; ++i) {
            int r = threading::SemaphoreSlimWait(sem, 2000);
            if (r != 1) {
                std::fprintf(stderr, "FAIL: consumer wait returned %d\n", r);
                return;
            }
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

    if (consumed.load() != kItems) {
        std::fprintf(stderr, "FAIL: consumed %d, expected %d\n", consumed.load(), kItems);
        return false;
    }
    return true;
}

// ── ReaderWriterLockSlim tests ─────────────────────────────────────────

static bool test_rwlock_basic() {
    uint32_t rw = threading::ReaderWriterLockSlimCreate();
    if (rw == 0) return false;

    // Multiple readers.
    if (threading::ReaderWriterLockSlimEnterRead(rw, 100) != 1) {
        std::fprintf(stderr, "FAIL: first read enter failed\n");
        threading::ReaderWriterLockSlimDestroy(rw);
        return false;
    }
    if (threading::ReaderWriterLockSlimEnterRead(rw, 100) != 1) {
        std::fprintf(stderr, "FAIL: second read enter failed\n");
        threading::ReaderWriterLockSlimDestroy(rw);
        return false;
    }

    // Exit both readers.
    if (!threading::ReaderWriterLockSlimExitRead(rw)) {
        std::fprintf(stderr, "FAIL: first read exit failed\n");
        threading::ReaderWriterLockSlimDestroy(rw);
        return false;
    }
    if (!threading::ReaderWriterLockSlimExitRead(rw)) {
        std::fprintf(stderr, "FAIL: second read exit failed\n");
        threading::ReaderWriterLockSlimDestroy(rw);
        return false;
    }

    // Write lock.
    if (threading::ReaderWriterLockSlimEnterWrite(rw, 100) != 1) {
        std::fprintf(stderr, "FAIL: write enter failed\n");
        threading::ReaderWriterLockSlimDestroy(rw);
        return false;
    }
    if (!threading::ReaderWriterLockSlimExitWrite(rw)) {
        std::fprintf(stderr, "FAIL: write exit failed\n");
        threading::ReaderWriterLockSlimDestroy(rw);
        return false;
    }

    threading::ReaderWriterLockSlimDestroy(rw);
    return true;
}

static bool test_rwlock_exclusion() {
    uint32_t rw = threading::ReaderWriterLockSlimCreate();
    if (rw == 0) return false;

    // Acquire write lock.
    if (threading::ReaderWriterLockSlimEnterWrite(rw, 100) != 1) {
        std::fprintf(stderr, "FAIL: write enter failed\n");
        return false;
    }

    // Try read lock while writer active — should timeout (poll).
    if (threading::ReaderWriterLockSlimEnterRead(rw, 0) != 0) {
        std::fprintf(stderr, "FAIL: read should block during write\n");
        threading::ReaderWriterLockSlimExitWrite(rw);
        threading::ReaderWriterLockSlimDestroy(rw);
        return false;
    }

    threading::ReaderWriterLockSlimExitWrite(rw);
    threading::ReaderWriterLockSlimDestroy(rw);
    return true;
}

static bool test_rwlock_destroy() {
    uint32_t rw = threading::ReaderWriterLockSlimCreate();
    if (rw == 0) return false;
    if (!threading::ReaderWriterLockSlimDestroy(rw)) {
        std::fprintf(stderr, "FAIL: destroy returned false\n");
        return false;
    }
    if (threading::ReaderWriterLockSlimDestroy(rw)) {
        std::fprintf(stderr, "FAIL: double destroy returned true\n");
        return false;
    }
    // Operations on destroyed lock should error.
    if (threading::ReaderWriterLockSlimEnterRead(rw, 0) != -1) {
        std::fprintf(stderr, "FAIL: enter on destroyed lock should error\n");
        return false;
    }
    return true;
}

// ── WaitHandle (ManualResetEvent) tests ────────────────────────────────

static bool test_manual_reset_basic() {
    uint32_t wh = threading::WaitHandleCreate(false, threading::WaitHandleType::ManualResetEvent);
    if (wh == threading::kInvalidWaitHandle) {
        std::fprintf(stderr, "FAIL: WaitHandleCreate failed\n");
        return false;
    }

    // Not signalled — poll should timeout.
    if (threading::WaitHandleWaitOne(wh, 0) != 0) {
        std::fprintf(stderr, "FAIL: poll should timeout\n");
        threading::WaitHandleClose(wh);
        return false;
    }

    // Set and wait.
    if (!threading::WaitHandleSet(wh)) {
        std::fprintf(stderr, "FAIL: Set failed\n");
        threading::WaitHandleClose(wh);
        return false;
    }
    if (threading::WaitHandleWaitOne(wh, 100) != 1) {
        std::fprintf(stderr, "FAIL: wait after set should succeed\n");
        threading::WaitHandleClose(wh);
        return false;
    }

    // ManualResetEvent stays signalled — second wait should also succeed.
    if (threading::WaitHandleWaitOne(wh, 100) != 1) {
        std::fprintf(stderr, "FAIL: ManualReset should stay signalled\n");
        threading::WaitHandleClose(wh);
        return false;
    }

    // Reset.
    if (!threading::WaitHandleReset(wh)) {
        std::fprintf(stderr, "FAIL: Reset failed\n");
        threading::WaitHandleClose(wh);
        return false;
    }
    if (threading::WaitHandleWaitOne(wh, 0) != 0) {
        std::fprintf(stderr, "FAIL: after reset should timeout\n");
        threading::WaitHandleClose(wh);
        return false;
    }

    threading::WaitHandleClose(wh);
    return true;
}

static bool test_auto_reset_basic() {
    uint32_t wh = threading::WaitHandleCreate(true, threading::WaitHandleType::AutoResetEvent);
    if (wh == threading::kInvalidWaitHandle) return false;

    // Initially signalled — wait should consume and succeed.
    if (threading::WaitHandleWaitOne(wh, 100) != 1) {
        std::fprintf(stderr, "FAIL: initial signal not consumed\n");
        threading::WaitHandleClose(wh);
        return false;
    }

    // Signal consumed — poll should timeout.
    if (threading::WaitHandleWaitOne(wh, 0) != 0) {
        std::fprintf(stderr, "FAIL: AutoReset should consume signal\n");
        threading::WaitHandleClose(wh);
        return false;
    }

    threading::WaitHandleClose(wh);
    return true;
}

static bool test_wait_any() {
    uint32_t wh1 = threading::WaitHandleCreate(false, threading::WaitHandleType::ManualResetEvent);
    uint32_t wh2 = threading::WaitHandleCreate(false, threading::WaitHandleType::ManualResetEvent);
    if (wh1 == 0 || wh2 == 0) return false;

    uint32_t handles[2] = { wh1, wh2 };

    // Neither signalled — poll returns -1.
    if (threading::WaitHandleWaitAny(handles, 2, 0) != -1) {
        std::fprintf(stderr, "FAIL: wait any poll should timeout\n");
        threading::WaitHandleClose(wh1);
        threading::WaitHandleClose(wh2);
        return false;
    }

    // Signal wh2.
    threading::WaitHandleSet(wh2);
    int idx = threading::WaitHandleWaitAny(handles, 2, 100);
    if (idx != 1) {
        std::fprintf(stderr, "FAIL: wait any should return index 1, got %d\n", idx);
        threading::WaitHandleClose(wh1);
        threading::WaitHandleClose(wh2);
        return false;
    }

    threading::WaitHandleClose(wh1);
    threading::WaitHandleClose(wh2);
    return true;
}

static bool test_wait_all() {
    uint32_t wh1 = threading::WaitHandleCreate(false, threading::WaitHandleType::ManualResetEvent);
    uint32_t wh2 = threading::WaitHandleCreate(false, threading::WaitHandleType::ManualResetEvent);
    if (wh1 == 0 || wh2 == 0) return false;

    uint32_t handles[2] = { wh1, wh2 };

    // Poll not all.
    if (threading::WaitHandleWaitAll(handles, 2, 0) != -1) {
        std::fprintf(stderr, "FAIL: wait all poll should timeout\n");
        threading::WaitHandleClose(wh1);
        threading::WaitHandleClose(wh2);
        return false;
    }

    // Signal one.
    threading::WaitHandleSet(wh1);
    if (threading::WaitHandleWaitAll(handles, 2, 0) != -1) {
        std::fprintf(stderr, "FAIL: wait all should fail with one signalled\n");
        threading::WaitHandleClose(wh1);
        threading::WaitHandleClose(wh2);
        return false;
    }

    // Signal both.
    threading::WaitHandleSet(wh2);
    if (threading::WaitHandleWaitAll(handles, 2, 100) != 0) {
        std::fprintf(stderr, "FAIL: wait all should succeed\n");
        threading::WaitHandleClose(wh1);
        threading::WaitHandleClose(wh2);
        return false;
    }

    threading::WaitHandleClose(wh1);
    threading::WaitHandleClose(wh2);
    return true;
}

// ── TimerQueue tests ───────────────────────────────────────────────────

static std::atomic<int> s_timer_fired{0};
static void timer_callback(void* ctx) {
    int val = static_cast<int>(reinterpret_cast<std::intptr_t>(ctx));
    s_timer_fired.fetch_add(val, std::memory_order_relaxed);
}

static bool test_timer_one_shot() {
    threading::TimerQueueInitialize();

    s_timer_fired.store(0, std::memory_order_relaxed);
    uint32_t tid = threading::TimerQueueCreate(timer_callback,
        reinterpret_cast<void*>(static_cast<std::intptr_t>(1)), 50, 0);
    if (tid == threading::kTimerQueueInvalidId) {
        std::fprintf(stderr, "FAIL: TimerQueueCreate failed\n");
        threading::TimerQueueShutdown();
        return false;
    }

    // Tick 3 times with 30ms gaps to cover the 50ms due time.
    for (int i = 0; i < 3; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        threading::TimerQueueOnTick();
    }

    threading::TimerQueueShutdown();

    if (s_timer_fired.load() != 1) {
        std::fprintf(stderr, "FAIL: timer fired %d times, expected 1\n",
                     s_timer_fired.load());
        return false;
    }
    return true;
}

static bool test_timer_periodic() {
    threading::TimerQueueInitialize();

    s_timer_fired.store(0, std::memory_order_relaxed);
    uint32_t tid = threading::TimerQueueCreate(timer_callback,
        reinterpret_cast<void*>(static_cast<std::intptr_t>(1)), 30, 50);
    if (tid == threading::kTimerQueueInvalidId) {
        threading::TimerQueueShutdown();
        return false;
    }

    // Tick enough times to cover 2-3 fires.
    for (int i = 0; i < 5; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
        threading::TimerQueueOnTick();
    }

    int fired = s_timer_fired.load();
    threading::TimerQueueShutdown();

    if (fired < 2) {
        std::fprintf(stderr, "FAIL: periodic timer fired %d times, expected >= 2\n", fired);
        return false;
    }
    return true;
}

// ── Main ───────────────────────────────────────────────────────────────

int main() {
    int passed = 0;
    int failed = 0;

    auto run = [&](const char* name, bool ok) {
        std::printf("  %s: %s\n", ok ? "PASS" : "FAIL", name);
        if (ok) ++passed; else ++failed;
    };

    std::printf("synchronization smoke tests\n");
    std::printf("---------------------------\n");

    // SemaphoreSlim
    run("sem_create_destroy",     test_semaphore_create_destroy());
    run("sem_wait_release",       test_semaphore_wait_release());
    run("sem_multi_thread",       test_semaphore_multi_thread());

    // ReaderWriterLockSlim
    run("rw_basic",               test_rwlock_basic());
    run("rw_exclusion",           test_rwlock_exclusion());
    run("rw_destroy",             test_rwlock_destroy());

    // WaitHandle
    run("manual_reset_basic",     test_manual_reset_basic());
    run("auto_reset_basic",       test_auto_reset_basic());
    run("wait_any",               test_wait_any());
    run("wait_all",               test_wait_all());

    // TimerQueue
    run("timer_one_shot",         test_timer_one_shot());
    run("timer_periodic",         test_timer_periodic());

    std::printf("\n%d passed, %d failed\n", passed, failed);

    if (failed > 0) {
        std::printf("synchronization_smoke FAILED\n");
        return 1;
    }

    std::printf("synchronization_smoke OK\n");
    return 0;
}