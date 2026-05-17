#include "thread_state.h"
#include "thread_pool.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <atomic>
#include <thread>
#include <vector>

// Extern "C" declarations for threading stubs (defined in threading_stubs.cpp,
// linked via chaos_runtime_core.lib).
extern "C" {
CHAOS_IL2CPP_INT32 chaos_thread_reset_abort(void) noexcept;
CHAOS_IL2CPP_INT32 chaos_thread_yield(void) noexcept;
void chaos_thread_sleep(CHAOS_IL2CPP_INT32 timeout_ms) noexcept;
}

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

// ── P2 thread capability tests ───────────────────────────────────────

/// ThreadState: verify state field transitions.
static bool test_thread_state() {
    auto* self = threading::GetCurrentThread();
    if (self == nullptr) {
        std::fprintf(stderr, "FAIL: no current thread\n");
        return false;
    }

    // Default after RegisterThread is Running.
    if (self->managed_state != threading::ManagedThreadState::Running) {
        std::fprintf(stderr, "FAIL: initial state = %d, expected %d (Running)\n",
                     static_cast<int>(self->managed_state),
                     static_cast<int>(threading::ManagedThreadState::Running));
        return false;
    }

    // Set to Stopped and verify.
    self->managed_state = threading::ManagedThreadState::Stopped;
    if (self->managed_state != threading::ManagedThreadState::Stopped) {
        std::fprintf(stderr, "FAIL: state after set = %d, expected %d (Stopped)\n",
                     static_cast<int>(self->managed_state),
                     static_cast<int>(threading::ManagedThreadState::Stopped));
        return false;
    }

    // Restore.
    self->managed_state = threading::ManagedThreadState::Running;
    return true;
}

/// ThreadPriority: set each level and verify readback.
static bool test_thread_priority() {
    auto* self = threading::GetCurrentThread();
    if (self == nullptr) return false;

    // Default is Normal.
    if (self->priority != threading::ManagedThreadPriority::Normal) {
        std::fprintf(stderr, "FAIL: default priority = %d, expected %d (Normal)\n",
                     static_cast<int>(self->priority),
                     static_cast<int>(threading::ManagedThreadPriority::Normal));
        return false;
    }

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
        if (self->priority != l.pri) {
            std::fprintf(stderr, "FAIL: priority after set(%s) = %d\n",
                         l.name, static_cast<int>(self->priority));
            return false;
        }
    }

    return true;
}

/// IsThreadPoolThread: verify flag read/write for non-pool thread.
static bool test_thread_is_threadpool() {
    auto* self = threading::GetCurrentThread();
    if (self == nullptr) return false;

    // Main thread should not be a threadpool thread.
    if (self->is_threadpool) {
        std::fprintf(stderr, "FAIL: main thread should not be threadpool\n");
        return false;
    }

    // Toggle and verify.
    self->is_threadpool = true;
    if (!self->is_threadpool) {
        std::fprintf(stderr, "FAIL: is_threadpool should be true after set\n");
        return false;
    }

    self->is_threadpool = false;
    return true;
}

/// Abort flag: set pending_abort and verify through chaos_thread_reset_abort.
static bool test_thread_abort_flag() {
    auto* self = threading::GetCurrentThread();
    if (self == nullptr) return false;

    // Initially no abort pending.
    self->pending_abort.store(false, std::memory_order_release);

    CHAOS_IL2CPP_INT32 r = chaos_thread_reset_abort();
    if (r != 0) {
        std::fprintf(stderr, "FAIL: reset_abort on clean thread = %d, expected 0\n", r);
        return false;
    }

    // Set abort flag.
    self->pending_abort.store(true, std::memory_order_release);
    if (!self->pending_abort.load(std::memory_order_acquire)) {
        std::fprintf(stderr, "FAIL: pending_abort should be true after set\n");
        return false;
    }

    // Reset abort flag via the public stub.
    r = chaos_thread_reset_abort();
    if (r != 1) {
        std::fprintf(stderr, "FAIL: reset_abort on pending thread = %d, expected 1\n", r);
        return false;
    }

    // Verify flag was cleared.
    if (self->pending_abort.load(std::memory_order_acquire)) {
        std::fprintf(stderr, "FAIL: pending_abort should be false after reset\n");
        return false;
    }

    return true;
}

/// Interrupt flag: set pending_interrupt and verify via direct access.
static bool test_thread_interrupt_flag() {
    auto* self = threading::GetCurrentThread();
    if (self == nullptr) return false;

    self->pending_interrupt.store(false, std::memory_order_release);

    // Set flag.
    self->pending_interrupt.store(true, std::memory_order_release);
    if (!self->pending_interrupt.load(std::memory_order_acquire)) {
        std::fprintf(stderr, "FAIL: pending_interrupt should be true after set\n");
        return false;
    }

    // Clear (as SafepointPoll does on throw).
    self->pending_interrupt.store(false, std::memory_order_release);
    if (self->pending_interrupt.load(std::memory_order_acquire)) {
        std::fprintf(stderr, "FAIL: pending_interrupt should be false after clear\n");
        return false;
    }

    return true;
}

/// Thread.Yield: verify the stub returns nonzero.
static bool test_thread_yield() {
    CHAOS_IL2CPP_INT32 r = chaos_thread_yield();
    if (r == 0) {
        std::fprintf(stderr, "FAIL: yield returned 0, expected nonzero\n");
        return false;
    }
    return true;
}

/// Thread.Sleep: verify the stub does not crash.
static bool test_thread_sleep() {
    chaos_thread_sleep(10);  // 10 ms — should not crash or block forever
    return true;
}

/// Background flag: verify is_background field read/write.
static bool test_thread_background() {
    auto* self = threading::GetCurrentThread();
    if (self == nullptr) return false;

    // Initially false.
    if (self->is_background) {
        std::fprintf(stderr, "FAIL: main thread should not be background by default\n");
        return false;
    }

    // Toggle on.
    self->is_background = true;
    if (!self->is_background) {
        std::fprintf(stderr, "FAIL: is_background should be true after set\n");
        return false;
    }

    // Toggle off.
    self->is_background = false;
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

    // ── P2 thread capability tests ──────────────────────────────────
    threading::RegisterThread(threading::kMainThreadId, nullptr);

    run("thread_state",          test_thread_state());
    run("thread_priority",       test_thread_priority());
    run("thread_is_threadpool",  test_thread_is_threadpool());
    run("thread_abort_flag",     test_thread_abort_flag());
    run("thread_interrupt_flag", test_thread_interrupt_flag());
    run("thread_yield",          test_thread_yield());
    run("thread_sleep",          test_thread_sleep());
    run("thread_background",     test_thread_background());

    threading::UnregisterThread();

    std::printf("\n%d passed, %d failed\n", passed, failed);

    if (failed > 0) {
        std::printf("threading_smoke FAILED\n");
        return 1;
    }

    std::printf("threading_smoke OK\n");
    return 0;
}
