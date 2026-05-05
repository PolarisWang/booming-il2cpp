#include "thread_state.h"
#include "thread_pool.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <atomic>
#include <thread>
#include <vector>

namespace threading = chaos::il2cpp::runtime_core::threading;

// ── Helpers ───────────────────────────────────────────────────────────

static std::atomic<int> s_enumerate_count{0};
static bool enumerate_callback(threading::ManagedThread* t) {
    s_enumerate_count.fetch_add(1, std::memory_order_relaxed);
    return true;  // continue enumeration
}

static std::atomic<int> s_work_executed{0};
static void work_callback(void* ctx) {
    int val = static_cast<int>(reinterpret_cast<std::intptr_t>(ctx));
    s_work_executed.fetch_add(val, std::memory_order_relaxed);
}

// ── Tests ─────────────────────────────────────────────────────────────

/// Verify that the main thread gets kMainThreadId = 1.
static bool test_main_thread_identity() {
    int32_t tid = threading::GetCurrentThreadId();
    if (tid != threading::kMainThreadId) {
        std::fprintf(stderr, "FAIL: main thread id == %d, expected %d\n",
                     tid, threading::kMainThreadId);
        return false;
    }

    auto* self = threading::GetCurrentThread();
    if (self == nullptr) {
        std::fprintf(stderr, "FAIL: main thread not registered yet\n");
        return false;
    }

    if (self->managed_id != tid) {
        std::fprintf(stderr, "FAIL: ManagedThread::managed_id mismatch\n");
        return false;
    }

    return true;
}

/// Register a thread, verify TLS identity, unregister.
static bool test_thread_register() {
    int32_t tid = threading::AllocateThreadId();
    threading::RegisterThread(tid, nullptr);

    int32_t actual = threading::GetCurrentThreadId();
    if (actual != tid) {
        std::fprintf(stderr, "FAIL: GetCurrentThreadId() = %d, expected %d\n",
                     actual, tid);
        return false;
    }

    threading::UnregisterThread();

    // After unregister, GetCurrentThread() should return nullptr.
    if (threading::GetCurrentThread() != nullptr) {
        std::fprintf(stderr, "FAIL: thread should be null after unregister\n");
        return false;
    }

    return true;
}

/// Thread enumeration across multiple threads.
static bool test_thread_enumeration() {
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
    if (count < kNumWorkers) {
        std::fprintf(stderr, "FAIL: enumerate saw %d threads, expected >= %d\n",
                     count, kNumWorkers);
        return false;
    }

    // Signal workers to exit.
    ready_count.store(0, std::memory_order_release);
    for (auto& t : workers) t.join();

    return true;
}

/// ThreadPool: queue work items and verify execution.
static bool test_thread_pool_basic() {
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
    if (executed != kWorkItems) {
        std::fprintf(stderr, "FAIL: executed %d of %d work items\n",
                     executed, kWorkItems);
        threading::ThreadPoolShutdown();
        return false;
    }

    threading::ThreadPoolShutdown();
    return true;
}

/// ThreadPool: worker count grows with demand.
static bool test_thread_pool_scaling() {
    threading::ThreadPoolInitialize();

    int initial = threading::ThreadPoolWorkerCount();
    if (initial < 1) {
        std::fprintf(stderr, "FAIL: expected at least 1 worker, got %d\n", initial);
        threading::ThreadPoolShutdown();
        return false;
    }

    // Queue many items to trigger growth.
    s_work_executed.store(0, std::memory_order_relaxed);
    for (int i = 0; i < 50; ++i) {
        threading::ThreadPoolQueueUserWorkItem(work_callback, nullptr);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    int grown = threading::ThreadPoolWorkerCount();
    if (grown < initial) {
        std::fprintf(stderr, "FAIL: worker count dropped from %d to %d\n",
                     initial, grown);
        threading::ThreadPoolShutdown();
        return false;
    }

    threading::ThreadPoolShutdown();
    return true;
}

// ── Main ──────────────────────────────────────────────────────────────

int main() {
    // Main thread must be registered for the tests to work.
    // (runtime_core.cpp does this in ThreadAttach, but as a standalone
    //  smoke test we register explicitly.)
    threading::RegisterThread(threading::kMainThreadId, nullptr);

    int passed = 0;
    int failed = 0;

    auto run = [&](const char* name, bool ok) {
        std::printf("  %s: %s\n", ok ? "PASS" : "FAIL", name);
        if (ok) ++passed; else ++failed;
    };

    std::printf("threading smoke tests\n");
    std::printf("---------------------\n");

    run("main_thread_identity", test_main_thread_identity());
    run("thread_register",      test_thread_register());
    run("thread_enumeration",   test_thread_enumeration());

    // Re-register main for pool tests.
    threading::RegisterThread(threading::kMainThreadId, nullptr);

    run("thread_pool_basic",    test_thread_pool_basic());
    run("thread_pool_scaling",  test_thread_pool_scaling());

    threading::UnregisterThread();

    std::printf("\n%d passed, %d failed\n", passed, failed);

    if (failed > 0) {
        std::printf("threading_smoke FAILED\n");
        return 1;
    }

    std::printf("threading_smoke OK\n");
    return 0;
}
