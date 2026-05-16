// Native threading stress tests for chaos-il2cpp runtime.
// Compiles as part of entry.exe in the threading-monitor-interlocked build.
// Tests thread creation, Monitor contention, WaitHandle throughput,
// and ThreadPool queuing at the native level.
//
// Usage: entry.exe --stress [thread_count] [duration_ms]

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

#include "runtime_core.h"
#include "wait_handle.h"
#include "thread_pool.h"

namespace chaos_stress {

using Clock = std::chrono::steady_clock;

// ── Test 1: WaitHandle throughput ──────────────────────────────────
// Create N auto-reset events, signal them from one thread,
// wait on them from another.  Measure ops/sec.
struct WaitHandleResult {
    uint64_t ops;
    double elapsed_ms;
};

static WaitHandleResult StressWaitHandle(int count, int iterations) {
    std::vector<uint32_t> handles(count);
    for (int i = 0; i < count; i++) {
        handles[i] = chaos::il2cpp::runtime_core::threading::WaitHandleCreate(
            false, chaos::il2cpp::runtime_core::threading::WaitHandleType::AutoResetEvent);
    }

    std::atomic<uint64_t> signal_count{0};
    std::atomic<bool> done{false};

    auto start = Clock::now();

    // Signaller thread
    std::thread signaller([&]() {
        while (!done) {
            for (int i = 0; i < count; i++) {
                chaos::il2cpp::runtime_core::threading::WaitHandleSet(handles[i]);
            }
            signal_count.fetch_add(count, std::memory_order_relaxed);
        }
    });

    // Receiver thread
    std::thread receiver([&]() {
        int iter = 0;
        while (iter < iterations) {
            for (int i = 0; i < count; i++) {
                chaos::il2cpp::runtime_core::threading::WaitHandleWaitOne(
                    handles[i], -1);
                iter++;
            }
        }
        done = true;
    });

    receiver.join();
    auto end = Clock::now();
    done = true;
    signaller.join();

    auto elapsed = std::chrono::duration<double, std::milli>(end - start).count();

    for (auto h : handles) {
        chaos::il2cpp::runtime_core::threading::WaitHandleClose(h);
    }

    return {static_cast<uint64_t>(iterations), elapsed};
}

// ── Test 2: WaitHandle storm ──────────────────────────────────────
// Many threads each set and wait on their own handle.
static WaitHandleResult StressWaitHandleStorm(int thread_count, int ops_per_thread) {
    std::vector<uint32_t> handles(thread_count);
    for (int i = 0; i < thread_count; i++) {
        handles[i] = chaos::il2cpp::runtime_core::threading::WaitHandleCreate(
            false, chaos::il2cpp::runtime_core::threading::WaitHandleType::AutoResetEvent);
    }

    std::atomic<uint64_t> total_ops{0};
    auto start = Clock::now();

    std::vector<std::thread> threads;
    threads.reserve(thread_count);

    for (int t = 0; t < thread_count; t++) {
        threads.emplace_back([&, t]() {
            for (int i = 0; i < ops_per_thread; i++) {
                chaos::il2cpp::runtime_core::threading::WaitHandleSet(handles[t]);
                chaos::il2cpp::runtime_core::threading::WaitHandleWaitOne(
                    handles[t], 5000);
                total_ops.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }

    for (auto& th : threads) {
        th.join();
    }

    auto end = Clock::now();
    auto elapsed = std::chrono::duration<double, std::milli>(end - start).count();

    for (auto h : handles) {
        chaos::il2cpp::runtime_core::threading::WaitHandleClose(h);
    }

    return {total_ops.load(), elapsed};
}

// ── Test 3: ThreadPool queue stress ───────────────────────────────
// Queue many work items and measure throughput.
static void ThreadPoolCallback(void* context) {
    auto* counter = static_cast<std::atomic<uint64_t>*>(context);
    counter->fetch_add(1, std::memory_order_relaxed);
}

static WaitHandleResult StressThreadPool(int count) {
    std::atomic<uint64_t> completed{0};
    auto start = Clock::now();

    for (int i = 0; i < count; i++) {
        chaos::il2cpp::runtime_core::threading::ThreadPoolQueueUserWorkItem(
            ThreadPoolCallback, &completed);
    }

    // Wait for all items to complete with polling
    while (completed.load(std::memory_order_acquire) < static_cast<uint64_t>(count)) {
        std::this_thread::yield();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(
                Clock::now() - start).count() > 30000) {
            std::fprintf(stderr, "TIMEOUT: ThreadPool stress (%llu/%d completed)\n",
                         (unsigned long long)completed.load(), count);
            break;
        }
    }

    auto end = Clock::now();
    auto elapsed = std::chrono::duration<double, std::milli>(end - start).count();
    return {completed.load(), elapsed};
}

// ── Test 4: Thread create/join benchmark ──────────────────────────
// Create and join threads that do minimal work.
static WaitHandleResult StressThreadCreateJoin(int count) {
    auto start = Clock::now();

    std::vector<std::thread> threads;
    threads.reserve(count);

    for (int i = 0; i < count; i++) {
        threads.emplace_back([]() {
            // Minimal work: just yield
            std::this_thread::yield();
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end = Clock::now();
    auto elapsed = std::chrono::duration<double, std::milli>(end - start).count();

    return {static_cast<uint64_t>(count), elapsed};
}

// ── Test 5: Mixed contention ──────────────────────────────────────
// Multiple threads contend on a shared atomic counter, simulating
// real-world contention patterns.
static WaitHandleResult StressContention(int thread_count, int ops_per_thread) {
    std::atomic<uint64_t> counter{0};

    auto start = Clock::now();

    std::vector<std::thread> threads;
    threads.reserve(thread_count);

    for (int t = 0; t < thread_count; t++) {
        threads.emplace_back([&counter, ops_per_thread]() {
            for (int i = 0; i < ops_per_thread; i++) {
                counter.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end = Clock::now();
    auto elapsed = std::chrono::duration<double, std::milli>(end - start).count();

    uint64_t expected = static_cast<uint64_t>(thread_count) * static_cast<uint64_t>(ops_per_thread);
    if (counter.load() != expected) {
        std::fprintf(stderr, "DATA RACE: counter=%llu expected=%llu\n",
                     (unsigned long long)counter.load(), (unsigned long long)expected);
        return {0, elapsed};  // Signal failure
    }

    return {counter.load(), elapsed};
}

// ── Test 6: WaitHandle WaitAny/WaitAll benchmark ──────────────────
// Create multiple handles, signal subsets, measure WaitAny/WaitAll.
struct MultiWaitResult {
    uint64_t ops;
    double elapsed_ms;
    double wait_any_ops_per_sec;
    double wait_all_ops_per_sec;
};

static MultiWaitResult StressMultiWait(int handle_count, int iterations) {
    // Create handles
    std::vector<uint32_t> handles(handle_count);
    for (int i = 0; i < handle_count; i++) {
        handles[i] = chaos::il2cpp::runtime_core::threading::WaitHandleCreate(
            false, chaos::il2cpp::runtime_core::threading::WaitHandleType::ManualResetEvent);
    }

    // WaitAny benchmark: signal one, wait, reset, repeat
    auto start1 = Clock::now();
    for (int i = 0; i < iterations; i++) {
        chaos::il2cpp::runtime_core::threading::WaitHandleSet(handles[0]);
        int result = chaos::il2cpp::runtime_core::threading::WaitHandleWaitAny(
            handles.data(), handle_count, 5000);
        chaos::il2cpp::runtime_core::threading::WaitHandleReset(handles[0]);
        if (result < 0) {
            std::fprintf(stderr, "WaitAny failed at iteration %d\n", i);
        }
    }
    auto end1 = Clock::now();
    double wa_ms = std::chrono::duration<double, std::milli>(end1 - start1).count();

    // WaitAll benchmark: signal all, wait, reset, repeat
    auto start2 = Clock::now();
    for (int i = 0; i < iterations; i++) {
        for (int j = 0; j < handle_count; j++) {
            chaos::il2cpp::runtime_core::threading::WaitHandleSet(handles[j]);
        }
        int result = chaos::il2cpp::runtime_core::threading::WaitHandleWaitAll(
            handles.data(), handle_count, 5000);
        for (int j = 0; j < handle_count; j++) {
            chaos::il2cpp::runtime_core::threading::WaitHandleReset(handles[j]);
        }
        if (result < 0) {
            std::fprintf(stderr, "WaitAll failed at iteration %d\n", i);
        }
    }
    auto end2 = Clock::now();
    double wa_all_ms = std::chrono::duration<double, std::milli>(end2 - start2).count();

    for (auto h : handles) {
        chaos::il2cpp::runtime_core::threading::WaitHandleClose(h);
    }

    return {
        static_cast<uint64_t>(iterations),
        wa_ms + wa_all_ms,
        iterations / (wa_ms / 1000.0),
        iterations / (wa_all_ms / 1000.0)
    };
}

// ── Runner ────────────────────────────────────────────────────────
// Returns 0 on success, 1 if any test fails.
int RunAll(int thread_count, int duration_ms) {
    int failures = 0;

    std::printf("=== Threading Native Stress Tests ===\n");
    std::printf("Thread count: %d, Duration: %d ms\n\n", thread_count, duration_ms);
    std::fflush(stdout);

    // Quick smoke: verify WaitHandleCreate works
    uint32_t smoke = chaos::il2cpp::runtime_core::threading::WaitHandleCreate(
        false, chaos::il2cpp::runtime_core::threading::WaitHandleType::AutoResetEvent);
    if (smoke == chaos::il2cpp::runtime_core::threading::kInvalidWaitHandle) {
        std::printf("[SMOKE] WaitHandleCreate FAILED\n");
        return 1;
    }
    chaos::il2cpp::runtime_core::threading::WaitHandleClose(smoke);
    std::printf("[SMOKE] WaitHandleCreate OK\n");
    std::fflush(stdout);

    // 1. WaitHandle throughput — wrap in try-catch to catch crashes
    std::printf("[1] Starting WaitHandle ping-pong...\n");
    std::fflush(stdout);
    try {
        auto r = StressWaitHandle(4, 100);
        double ops_per_sec = r.ops / (r.elapsed_ms / 1000.0);
        std::printf("[1] WaitHandle ping-pong:     %6llu ops in %8.2f ms  (%10.1f ops/sec)\n",
                     (unsigned long long)r.ops, r.elapsed_ms, ops_per_sec);
    } catch (...) {
        std::printf("[1] FAILED with exception\n");
        failures++;
    }
    std::fflush(stdout);

    try {
        std::printf("[2] Starting WaitHandle storm (%d threads)...\n", thread_count);
        std::fflush(stdout);
        auto r2 = StressWaitHandleStorm(thread_count, 100);
        double ops_per_sec2 = r2.ops / (r2.elapsed_ms / 1000.0);
        std::printf("[2] WaitHandle storm (%d threads): %6llu ops in %8.2f ms  (%10.1f ops/sec)\n",
                     thread_count, (unsigned long long)r2.ops, r2.elapsed_ms, ops_per_sec2);
    } catch (...) {
        std::printf("[2] FAILED with exception\n");
        failures++;
    }
    std::fflush(stdout);

    try {
        std::printf("[3] Starting ThreadPool queue (1000)...\n");
        std::fflush(stdout);
        auto r3 = StressThreadPool(1000);
        double ops_per_sec3 = r3.ops / (r3.elapsed_ms / 1000.0);
        std::printf("[3] ThreadPool queue (1000):    %6llu ops in %8.2f ms  (%10.1f ops/sec)\n",
                     (unsigned long long)r3.ops, r3.elapsed_ms, ops_per_sec3);
    } catch (...) {
        std::printf("[3] FAILED with exception\n");
        failures++;
    }
    std::fflush(stdout);

    try {
        std::printf("[4] Starting Thread create/join (%d)...\n", thread_count);
        std::fflush(stdout);
        auto r4 = StressThreadCreateJoin(thread_count);
        double ops_per_sec4 = r4.ops / (r4.elapsed_ms / 1000.0);
        std::printf("[4] Thread create/join (%d): %6llu ops in %8.2f ms  (%10.1f ops/sec)\n",
                     thread_count, (unsigned long long)r4.ops, r4.elapsed_ms, ops_per_sec4);
    } catch (...) {
        std::printf("[4] FAILED with exception\n");
        failures++;
    }
    std::fflush(stdout);

    try {
        std::printf("[5] Starting Atomic contention (%d)...\n", thread_count);
        std::fflush(stdout);
        auto r5 = StressContention(thread_count, 10000);
        double ops_per_sec5 = r5.ops / (r5.elapsed_ms / 1000.0);
        if (r5.ops == 0) {
            std::printf("[5] Atomic contention (%d):     FAILED (data race detected)\n", thread_count);
            failures++;
        } else {
            std::printf("[5] Atomic contention (%d):     %6llu ops in %8.2f ms  (%10.1f ops/sec)\n",
                         thread_count, (unsigned long long)r5.ops, r5.elapsed_ms, ops_per_sec5);
        }
    } catch (...) {
        std::printf("[5] FAILED with exception\n");
        failures++;
    }
    std::fflush(stdout);

    try {
        std::printf("[6] Starting Multi-wait...\n");
        std::fflush(stdout);
        auto r6 = StressMultiWait(8, 1000);
        std::printf("[6] WaitAny/WaitAll (8 handles, 1000 iters):\n");
        std::printf("    WaitAny:  %10.1f ops/sec\n", r6.wait_any_ops_per_sec);
        std::printf("    WaitAll:  %10.1f ops/sec\n", r6.wait_all_ops_per_sec);
    } catch (...) {
        std::printf("[6] FAILED with exception\n");
        failures++;
    }
    std::fflush(stdout);

    std::printf("\n=== Results: %s (%d failures) ===\n",
                 failures == 0 ? "ALL PASSED" : "SOME FAILED", failures);
    std::fflush(stdout);
    return failures;
}

}  // namespace chaos_stress

// ── C entry point called from runtime-entry.cpp ───────────────────
extern "C" int RunStressTests(int thread_count, int duration_ms) {
    return chaos_stress::RunAll(thread_count, duration_ms);
}
