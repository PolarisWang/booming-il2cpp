// threading_benchmark_test.cpp — Native microbenchmarks matching managed
// benchmark workloads:
//   thin-lock-uncontended:  10000 lock/unlock per sample
//   thread-pool-queue:      raw ThreadPoolQueueUserWorkItem + spin
//   thread-pool-continuation: models Task.Run + ContinueWith + GetResult
//   thread-create-join:     std::thread + Register/Unregister
//   thread-enumeration:     lock-free list walk
//   thread-state-get:       ManagedThreadState field read
//   thread-priority-get-set: ManagedThreadPriority field write+read
//
// Labeled as benchmark (excluded from default ctest run).
// No GC dependency — uses raw TEST().

#include <gtest/gtest.h>

#include <thread_state.h>
#include <thread_pool.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <atomic>
#include <memory>
#include <thread>
#include <vector>

// ── AOT codegen stubs ─────────────────────────────────────────────────────
// Normally provided by codegen output; required because chaos_runtime_core.lib
// references them.

extern "C" const void* chaos_il2cpp_aot_hotpatch_module = nullptr;
extern "C" const char* const kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int32_t kChaosExternalRuntimeCount = 0;

// ── MonitorEnter/MonitorExit ──────────────────────────────────────────────
// Forward-declared from engine_binding.h (namespace
// chaos::il2cpp::runtime_core), linked via chaos_runtime_core.lib.

namespace chaos::il2cpp::runtime_core {
    bool MonitorEnter(void* monitor_target);
    bool MonitorExit(void* monitor_target);
}

namespace threading = chaos::il2cpp::runtime_core::threading;
namespace rt = chaos::il2cpp::runtime_core;

using Clock = std::chrono::high_resolution_clock;
static constexpr size_t kObjectHeaderSize = 56;

// ── VolatileRead helper ───────────────────────────────────────────────────
// Forces a real memory read to prevent the compiler from optimizing away
// repeated loads in benchmark loops.

template <typename T>
inline T VolatileRead(const T& addr) noexcept {
    return *static_cast<const volatile T*>(&addr);
}

// ─── Thin lock benchmark ──────────────────────────────────────────────────
// Matches managed MonitorAndLockingBenchmark: 10000 lock/unlock per sample.

static const chaos::il2cpp::common::TypeInfoHot g_lock_type_info_stub = {
    nullptr,  // parent
    nullptr,  // vtable_array
    0,        // stable_id
    0,        // vtable_length
    0,        // warm_delta
    0,        // type_shape
    chaos::il2cpp::common::kTypeInfoHeaderKindThin  // flags = ThinLockable
};

static void* g_lock_obj = nullptr;
static uint64_t g_lock_checksum = 0;

static void lock_setup() {
    threading::RegisterThread(threading::kMainThreadId, nullptr);
    g_lock_obj = std::malloc(kObjectHeaderSize);
    std::memset(g_lock_obj, 0, kObjectHeaderSize);
    *static_cast<const void**>(g_lock_obj) = &g_lock_type_info_stub;
    g_lock_checksum = 0;
}

static void lock_teardown() {
    std::free(g_lock_obj);
    g_lock_obj = nullptr;
    threading::UnregisterThread();
}

static double run_lock_bench(int iterations) {
    // Warmup
    for (int i = 0; i < 100; ++i) {
        if (rt::MonitorEnter(g_lock_obj)) {
            g_lock_checksum += (uint64_t)(i & 7);
            rt::MonitorExit(g_lock_obj);
        }
    }

    constexpr int kSamples = 3;
    double total_ms = 0;
    uint64_t total_cs = 0;

    for (int s = 0; s < kSamples; ++s) {
        uint64_t cs = 0;
        auto t0 = Clock::now();
        for (int i = 0; i < iterations; ++i) {
            if (rt::MonitorEnter(g_lock_obj)) {
                cs += (uint64_t)(i & 7);
                rt::MonitorExit(g_lock_obj);
            }
        }
        auto t1 = Clock::now();
        double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
        total_ms += ms;
        total_cs += cs;
    }

    g_lock_checksum = total_cs;
    return total_ms / kSamples;
}

// ─── ThreadPool queue benchmark ───────────────────────────────────────────

static std::atomic<int> s_pool_count{0};
static void pool_cb(void*) {
    s_pool_count.fetch_add(1, std::memory_order_relaxed);
}

static double run_pool_bench(int iterations) {
    threading::RegisterThread(threading::kMainThreadId, nullptr);
    threading::ThreadPoolInitialize();

    // Warmup
    s_pool_count.store(0, std::memory_order_relaxed);
    for (int i = 0; i < 10; ++i) {
        threading::ThreadPoolQueueUserWorkItem(pool_cb, nullptr);
    }
    while (s_pool_count.load(std::memory_order_relaxed) < 10) {
        std::this_thread::yield();
    }

    constexpr int kSamples = 3;
    double total_ms = 0;

    for (int s = 0; s < kSamples; ++s) {
        s_pool_count.store(0, std::memory_order_relaxed);
        auto t0 = Clock::now();
        for (int i = 0; i < iterations; ++i) {
            threading::ThreadPoolQueueUserWorkItem(pool_cb, nullptr);
        }
        int spins = 0;
        while (s_pool_count.load(std::memory_order_relaxed) < iterations &&
               spins < 200000) {
            std::this_thread::yield();
            ++spins;
        }
        auto t1 = Clock::now();
        total_ms += std::chrono::duration<double, std::milli>(t1 - t0).count();
    }

    threading::ThreadPoolShutdown();
    threading::UnregisterThread();
    return total_ms / kSamples;
}

// ─── ThreadPool continuation benchmark ────────────────────────────────────
// Models C# TaskSchedulingBenchmark:
//   For each of N items:
//     1. Queue result work item (simulates Task.Run)
//     2. Queue continuation work item (simulates ContinueWith)
//     3. Block on both (simulates GetResult x2)

struct alignas(64) PoolTask {
    std::atomic<bool> result_done{false};
    std::atomic<bool> cont_done{false};
};

static void pool_cont_cb(void* ctx) {
    auto* task = static_cast<PoolTask*>(ctx);
    task->cont_done.store(true, std::memory_order_release);
}

static void pool_result_cb(void* ctx) {
    auto* task = static_cast<PoolTask*>(ctx);
    task->result_done.store(true, std::memory_order_release);
    threading::ThreadPoolQueueUserWorkItem(pool_cont_cb, ctx);
}

static double run_continuation_bench(int iterations) {
    threading::RegisterThread(threading::kMainThreadId, nullptr);
    threading::ThreadPoolInitialize();

    auto tasks = std::make_unique<PoolTask[]>(iterations);

    // Warmup
    for (int i = 0; i < 4; ++i) {
        tasks[i].result_done.store(false, std::memory_order_relaxed);
        tasks[i].cont_done.store(false, std::memory_order_relaxed);
        threading::ThreadPoolQueueUserWorkItem(pool_result_cb, &tasks[i]);
    }
    for (int i = 0; i < 4; ++i) {
        while (!tasks[i].result_done.load(std::memory_order_acquire))
            std::this_thread::yield();
        while (!tasks[i].cont_done.load(std::memory_order_acquire))
            std::this_thread::yield();
    }

    constexpr int kSamples = 3;
    double total_ms = 0;

    for (int s = 0; s < kSamples; ++s) {
        // Reset all tasks
        for (int i = 0; i < iterations; ++i) {
            tasks[i].result_done.store(false, std::memory_order_relaxed);
            tasks[i].cont_done.store(false, std::memory_order_relaxed);
        }

        auto t0 = Clock::now();
        // Phase 1: queue all work items
        for (int i = 0; i < iterations; ++i) {
            threading::ThreadPoolQueueUserWorkItem(pool_result_cb, &tasks[i]);
        }

        // Phase 2: wait for each task (GetResult x2)
        uint64_t cs = 0;
        for (int i = 0; i < iterations; ++i) {
            while (!tasks[i].result_done.load(std::memory_order_acquire))
                std::this_thread::yield();
            cs += static_cast<uint64_t>(i & 7);
            while (!tasks[i].cont_done.load(std::memory_order_acquire))
                std::this_thread::yield();
            cs += static_cast<uint64_t>(i & 3);
        }
        auto t1 = Clock::now();
        total_ms += std::chrono::duration<double, std::milli>(t1 - t0).count();
        (void)cs;
    }

    threading::ThreadPoolShutdown();
    threading::UnregisterThread();
    return total_ms / kSamples;
}

// ─── Thread create/join benchmark ─────────────────────────────────────────

static double run_thread_bench(int iterations) {
    constexpr int kSamples = 3;
    double total_ms = 0;
    uint64_t cs = 0;

    for (int s = 0; s < kSamples; ++s) {
        auto t0 = Clock::now();
        for (int i = 0; i < iterations; ++i) {
            std::thread t([&cs] {
                int32_t tid = threading::AllocateThreadId();
                threading::RegisterThread(tid, nullptr);
                cs += threading::GetCurrentThreadId();
                threading::UnregisterThread();
            });
            t.join();
        }
        auto t1 = Clock::now();
        total_ms += std::chrono::duration<double, std::milli>(t1 - t0).count();
    }
    (void)cs;
    return total_ms / kSamples;
}

// ─── Thread enumeration benchmark ─────────────────────────────────────────

static std::atomic<int> s_enum_count{0};
static bool enum_cb(threading::ManagedThread*) {
    s_enum_count.fetch_add(1, std::memory_order_relaxed);
    return true;
}

static double run_enum_bench(int iterations) {
    threading::RegisterThread(threading::kMainThreadId, nullptr);
    constexpr int kSamples = 3;
    double total_ms = 0;
    uint64_t cs = 0;

    for (int s = 0; s < kSamples; ++s) {
        auto t0 = Clock::now();
        for (int i = 0; i < iterations; ++i) {
            s_enum_count.store(0, std::memory_order_relaxed);
            threading::EnumerateThreads(enum_cb);
            cs += s_enum_count.load(std::memory_order_relaxed);
        }
        auto t1 = Clock::now();
        total_ms += std::chrono::duration<double, std::milli>(t1 - t0).count();
    }

    threading::UnregisterThread();
    (void)cs;
    return total_ms / kSamples;
}

// ─── Thread state get benchmark ───────────────────────────────────────────

static double run_state_get_bench(int iterations) {
    threading::RegisterThread(threading::kMainThreadId, nullptr);
    auto* self = threading::GetCurrentThread();

    // Warmup — VolatileRead forces real memory access
    for (int i = 0; i < 100; ++i) {
        volatile auto s = VolatileRead(self->managed_state);
        (void)s;
    }

    constexpr int kSamples = 3;
    double total_ms = 0;
    uint64_t cs = 0;

    for (int s = 0; s < kSamples; ++s) {
        auto t0 = Clock::now();
        for (int i = 0; i < iterations; ++i) {
            cs += static_cast<uint64_t>(static_cast<int>(
                VolatileRead(self->managed_state)));
        }
        auto t1 = Clock::now();
        total_ms += std::chrono::duration<double, std::milli>(t1 - t0).count();
    }

    threading::UnregisterThread();
    (void)cs;
    return total_ms / kSamples;
}

// ─── Thread priority get/set benchmark ────────────────────────────────────

static std::atomic<int> s_pri_checksum{0};

static double run_priority_get_set_bench(int iterations) {
    threading::RegisterThread(threading::kMainThreadId, nullptr);
    auto* self = threading::GetCurrentThread();

    // Warmup
    for (int i = 0; i < 100; ++i) {
        self->priority = threading::ManagedThreadPriority::Normal;
        volatile auto p = self->priority;
        (void)p;
    }

    constexpr int kSamples = 3;
    double total_ms = 0;
    int cs = 0;

    threading::ManagedThreadPriority levels[] = {
        threading::ManagedThreadPriority::Lowest,
        threading::ManagedThreadPriority::BelowNormal,
        threading::ManagedThreadPriority::Normal,
        threading::ManagedThreadPriority::AboveNormal,
        threading::ManagedThreadPriority::Highest,
    };

    for (int s = 0; s < kSamples; ++s) {
        auto t0 = Clock::now();
        for (int i = 0; i < iterations; ++i) {
            *static_cast<volatile threading::ManagedThreadPriority*>(&self->priority) =
                levels[i % 5];
            cs += static_cast<int>(VolatileRead(self->priority));
        }
        auto t1 = Clock::now();
        total_ms += std::chrono::duration<double, std::milli>(t1 - t0).count();
    }

    threading::UnregisterThread();
    s_pri_checksum.store(cs, std::memory_order_relaxed);
    return total_ms / kSamples;
}

// ── Tests ─────────────────────────────────────────────────────────────────

TEST(ThreadingBenchmark, ThinLock)
{
    lock_setup();
    double ms = run_lock_bench(10000);
    lock_teardown();

    std::printf("  [bench] thin-lock-uncontended: 10000 iters, %.3f ms, %.1f ns/op\n",
                ms, (ms * 1e6) / 10000);
    EXPECT_GT(ms, 0.0);
}

TEST(ThreadingBenchmark, ThreadPoolQueue)
{
    double ms = run_pool_bench(1000);

    std::printf("  [bench] thread-pool-queue: 1000 iters, %.3f ms, %.1f ns/op\n",
                ms, (ms * 1e6) / 1000);
    EXPECT_GT(ms, 0.0);
}

TEST(ThreadingBenchmark, ThreadPoolContinuation)
{
    double ms = run_continuation_bench(16);

    std::printf("  [bench] thread-pool-continuation-16: 16 iters, %.3f ms, %.1f us/op\n",
                ms, (ms * 1e3) / 16);
    EXPECT_GT(ms, 0.0);
}

TEST(ThreadingBenchmark, ThreadCreateJoin)
{
    double ms = run_thread_bench(50);

    std::printf("  [bench] thread-create-join: 50 iters, %.3f ms, %.1f us/op\n",
                ms, (ms * 1e3) / 50);
    EXPECT_GT(ms, 0.0);
}

TEST(ThreadingBenchmark, ThreadEnumeration)
{
    double ms = run_enum_bench(500);

    std::printf("  [bench] thread-enumeration: 500 iters, %.3f ms, %.1f us/op\n",
                ms, (ms * 1e3) / 500);
    EXPECT_GT(ms, 0.0);
}

TEST(ThreadingBenchmark, ThreadStateGet)
{
    double ms = run_state_get_bench(5000);

    std::printf("  [bench] thread-state-get: 5000 iters, %.3f ms, %.1f ns/op\n",
                ms, (ms * 1e6) / 5000);
    EXPECT_GT(ms, 0.0);
}

TEST(ThreadingBenchmark, ThreadPriorityGetSet)
{
    double ms = run_priority_get_set_bench(5000);

    std::printf("  [bench] thread-priority-get-set: 5000 iters, %.3f ms, %.1f ns/op\n",
                ms, (ms * 1e6) / 5000);
    EXPECT_GT(ms, 0.0);
}
