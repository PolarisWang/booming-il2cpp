// threading_benchmark — native microbenchmarks matching managed benchmark workloads
//   monitor-locking-bench: 10000 lock/unlock per sample
//   task-scheduling-bench: not directly comparable (Task.Run + ContinueWith is higher-level)
//   For completeness we also measure raw ThreadPool queue + thread create/join.

#include "thread_state.h"
#include "thread_pool.h"

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <atomic>
#include <memory>
#include <thread>
#include <vector>

// ── Stubs for symbols normally emitted by AOT codegen ─────────────────────
extern "C" const void* chaos_il2cpp_aot_hotpatch_module = nullptr;
extern "C" const char* const kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int32_t kChaosExternalRuntimeCount = 0;

// Forward-declare MonitorEnter/MonitorExit (declared in engine_binding.h but we avoid
// including it here due to ValueTypeKernelBackendKind redefinition in this TU).
#include "runtime_abi.h"
namespace chaos::il2cpp::runtime_core {
    bool MonitorEnter(void* monitor_target);
    bool MonitorExit(void* monitor_target);
}

namespace threading = chaos::il2cpp::runtime_core::threading;
namespace rt = chaos::il2cpp::runtime_core;

using Clock = std::chrono::high_resolution_clock;
static constexpr size_t kObjectHeaderSize = 56;  // vtable(8)+type_info(8)+sync_state(8)+field_storage(32)

// Force a real memory read — prevents compiler from optimizing away repeated loads
// in benchmark loops. The returned value is opaque to the optimizer.
template <typename T>
inline T VolatileRead(const T& addr) noexcept {
    return *static_cast<const volatile T*>(&addr);
}

// ── Benchmark 1: Thin lock uncontended ──────────────────────────────────
// Matches managed MonitorAndLockingBenchmark: 10000 lock/unlock per sample

// A minimal TypeInfoHot stub for the lock benchmark object.
// Must have flags with kTypeInfoHeaderKindThin (0x01) so that
// GetSyncStatePtr() correctly locates the sync_state field at offset 8.
// The buffer is zeroed except for this pointer at offset 0.
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
    // Set up a valid TypeInfoHot* at offset 0 so MonitorEnter can read flags.
    // Without this, GetSyncStatePtr dereferences nullptr -> SIGSEGV.
    *static_cast<const void**>(g_lock_obj) = &g_lock_type_info_stub;
    g_lock_checksum = 0;
}

static void lock_teardown() {
    std::free(g_lock_obj);
    g_lock_obj = nullptr;
    threading::UnregisterThread();
}

static double run_lock_bench(int iterations) {
    // warmup
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

// ── Benchmark 2: ThreadPool queue + execute (raw throughput) ────────────

static std::atomic<int> s_pool_count{0};
static void pool_cb(void*) {
    s_pool_count.fetch_add(1, std::memory_order_relaxed);
}

static double run_pool_bench(int iterations) {
    threading::RegisterThread(threading::kMainThreadId, nullptr);
    threading::ThreadPoolInitialize();
    // warmup
    s_pool_count.store(0, std::memory_order_relaxed);
    for (int i = 0; i < 10; ++i) threading::ThreadPoolQueueUserWorkItem(pool_cb, nullptr);
    while (s_pool_count.load(std::memory_order_relaxed) < 10) std::this_thread::yield();

    constexpr int kSamples = 3;
    double total_ms = 0;

    for (int s = 0; s < kSamples; ++s) {
        s_pool_count.store(0, std::memory_order_relaxed);
        auto t0 = Clock::now();
        for (int i = 0; i < iterations; ++i)
            threading::ThreadPoolQueueUserWorkItem(pool_cb, nullptr);
        int spins = 0;
        while (s_pool_count.load(std::memory_order_relaxed) < iterations && spins < 200000) {
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

// ── Benchmark 3: ThreadPool continuation model ─────────────────────────
// Models C# TaskSchedulingBenchmark:
//   For each of N items:
//     1. Create TaskCompletionSource (simulated by PoolTask state)
//     2. Chain continuation: task.ContinueWith(cb)
//     3. Task.Run(() => tcs.SetResult(i))  — queued to ThreadPool
//     4. GetAwaiter().GetResult() × 2      — blocks on result + continuation
// Total ThreadPool ops: 2 × N (result + continuation per item)

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
    // SetResult: signal the task completion
    task->result_done.store(true, std::memory_order_release);
    // Queue continuation (ContinueWith scheduled by TCS.SetResult)
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
        // Phase 1: queue all work items (Task.Run equivalent)
        for (int i = 0; i < iterations; ++i)
            threading::ThreadPoolQueueUserWorkItem(pool_result_cb, &tasks[i]);

        // Phase 2: wait for each task (GetAwaiter().GetResult() × 2)
        uint64_t cs = 0;
        for (int i = 0; i < iterations; ++i) {
            // First GetResult: wait for Task completion
            while (!tasks[i].result_done.load(std::memory_order_acquire))
                std::this_thread::yield();
            cs += static_cast<uint64_t>(i & 7);
            // Second GetResult: wait for continuation (should already be
            // queued by SetResult, may or may not have executed yet)
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

// ── Benchmark 3: Thread create/join ─────────────────────────────────────

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

// ── Benchmark 4: Thread enumeration ─────────────────────────────────────

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

// ── Benchmark 5: Thread state get ───────────────────────────────────

static double run_state_get_bench(int iterations) {
    threading::RegisterThread(threading::kMainThreadId, nullptr);
    auto* self = threading::GetCurrentThread();

    // Warmup — VolatileRead forces real memory access so compiler cannot
    // eliminate the loop.
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

// ── Benchmark 6: Thread priority get/set ─────────────────────────────

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
            *static_cast<volatile threading::ManagedThreadPriority*>(&self->priority) = levels[i % 5];
            cs += static_cast<int>(VolatileRead(self->priority));
        }
        auto t1 = Clock::now();
        total_ms += std::chrono::duration<double, std::milli>(t1 - t0).count();
    }

    threading::UnregisterThread();
    s_pri_checksum.store(cs, std::memory_order_relaxed);
    return total_ms / kSamples;
}

// ── DeepInlineCallees stub ────────────────────────────────────────────────
// Required by entry_direct.cpp (included in interpreter_entry.cpp unity build).
// The real implementation (in ir_optimizer.cpp) is not compiled in this tree.
namespace chaos::il2cpp::interpreter { struct IRMethod; }
namespace chaos::il2cpp::runtime_core { struct PatchMethod; }
namespace chaos::il2cpp::runtime_core {
bool DeepInlineCallees(
    interpreter::IRMethod& /*ir*/,
    PatchMethod& /*patch_method*/,
    uint32_t /*max_levels*/,
    uint32_t /*max_instructions*/) noexcept
{
    return false;
}
}

// ── main ─────────────────────────────────────────────────────────────────

int main() {
    std::printf("========================================================\n");
    std::printf("  Native threading benchmarks\n");
    std::printf("  Platform: Windows x64, Release build\n");
    std::printf("  3 samples per benchmark, matching managed iteration counts\n");
    std::printf("========================================================\n\n");

    struct {
        const char* name;
        double      mean_ms;
        int         iterations;
        const char* note;
    } results[8];
    int ri = 0;

    // 1) Thin lock — 10000 iters (matches managed monitor-locking-bench)
    lock_setup();
    results[ri].name       = "thin-lock-uncontended";
    results[ri].mean_ms    = run_lock_bench(10000);
    results[ri].iterations = 10000;
    results[ri].note       = "matches MonitorAndLockingBenchmark (10000 lock)";
    lock_teardown();
    ri++;

    // 2) ThreadPool queue — 1000 items
    results[ri].name       = "thread-pool-queue";
    results[ri].mean_ms    = run_pool_bench(1000);
    results[ri].iterations = 1000;
    results[ri].note       = "raw ThreadPoolQueueUserWorkItem + spin";
    ri++;

    // 3) ThreadPool continuation — 16 items (matches C# TaskSchedulingBenchmark)
    results[ri].name       = "thread-pool-continuation-16";
    results[ri].mean_ms    = run_continuation_bench(16);
    results[ri].iterations = 16;
    results[ri].note       = "2×queue + 2×block per item (models Task.Run+ContinueWith+GetResult)";
    ri++;

    // 4) Thread create/join — 50 threads
    results[ri].name       = "thread-create-join";
    results[ri].mean_ms    = run_thread_bench(50);
    results[ri].iterations = 50;
    results[ri].note       = "native std::thread + Register/Unregister";
    ri++;

    // 5) Thread enumeration — 500 iterations
    results[ri].name       = "thread-enumeration";
    results[ri].mean_ms    = run_enum_bench(500);
    results[ri].iterations = 500;
    results[ri].note       = "EnumerateThreads (lock-free list walk)";
    ri++;

    // 6) Thread state get — 5000 iterations
    results[ri].name       = "thread-state-get";
    results[ri].mean_ms    = run_state_get_bench(5000);
    results[ri].iterations = 5000;
    results[ri].note       = "ManagedThreadState field read";
    ri++;

    // 7) Thread priority get/set — 5000 iterations
    results[ri].name       = "thread-priority-get-set";
    results[ri].mean_ms    = run_priority_get_set_bench(5000);
    results[ri].iterations = 5000;
    results[ri].note       = "ManagedThreadPriority field write+read";
    ri++;

    // ── Print results ──
    std::printf("  %-26s  %8s  %8s  %12s  %s\n",
                "Benchmark", "iters", "mean_ms", "ns/op", "Note");
    std::printf("  %-26s  %8s  %8s  %12s  %s\n",
                "-------------------------", "--------", "--------", "----------", "----");

    for (int i = 0; i < ri; ++i) {
        double ns_per_op = (results[i].mean_ms * 1e6) / results[i].iterations;
        std::printf("  %-26s  %7d  %8.3f  %10.1f  %s\n",
                    results[i].name, results[i].iterations,
                    results[i].mean_ms, ns_per_op, results[i].note);
    }

    // ── Managed comparison reference ──
    std::printf("\n\n========================================================\n");
    std::printf("  MANAGED COMPARISON (SolutionCorePack managed-perf)\n");
    std::printf("========================================================\n");
    std::printf("\n");
    std::printf("  monitor-locking-bench (managed)\n");
    std::printf("    10000 lock/unlock, mean=0.936 ms, 93.6 ns/op\n");
    std::printf("    (Source: manual managed run, no committed JSON report)\n");
    std::printf("  thin-lock-uncontended (native)\n");
    std::printf("    10000 lock/unlock, mean=%.3f ms, %.1f ns/op\n",
                results[0].mean_ms, (results[0].mean_ms * 1e6) / results[0].iterations);
    double speedup = 0.936 / results[0].mean_ms;
    std::printf("    native speedup: %.1fx over managed\n\n", speedup);

    std::printf("  task-scheduling-bench (managed)\n");
    std::printf("    16 Task.Run+ContinueWith, mean=2.417 ms, 151.1 us/op\n");
    std::printf("    (Source: nativeization-throughput-benchmark-v1-01.json, 3 samples)\n");
    std::printf("    (Task.Run ~ ThreadPool.QueueUserWorkItem + scheduling overhead)\n");
    std::printf("  thread-pool-continuation-16 (native)\n");
    std::printf("    16 QueueUserWorkItem+block, mean=%.3f ms, %.1f us/op\n",
                results[2].mean_ms, (results[2].mean_ms * 1e3) / results[2].iterations);
    std::printf("    (native = 2×queue + 2×block per item, managed includes Task object\n");
    std::printf("     allocation, closure capture, TCS state machine, BDN framework)\n");

    std::printf("\ndone.\n");
    return 0;
}
