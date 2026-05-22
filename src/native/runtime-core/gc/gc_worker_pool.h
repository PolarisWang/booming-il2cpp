#ifndef CHAOS_IL2CPP_GC_WORKER_POOL_H_
#define CHAOS_IL2CPP_GC_WORKER_POOL_H_

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>

namespace chaos::il2cpp::runtime_core {

/// Dedicated worker thread pool for GC parallel work.
///
/// == Motivation ==
/// ThreadPoolQueueUserWorkItem deadlocks inside a GC safepoint because
/// ThreadPool workers are registered managed threads that spin in
/// SafepointPoll and cannot accept new work while generation is odd.
///
/// == Solution ==
/// GcWorkerPool provides dedicated std::thread workers that are NOT
/// registered as managed threads — they never participate in safepoint
/// protocol.  Workers park on a condition_variable when idle and are
/// woken via RunWorkers().
///
/// == Design (CoreCLR-aligned) ==
/// - Thread count is dynamic: default = max(1, hardware_concurrency() - 1),
///   adjustable via SetMaxWorkers().
/// - Workers park on cv with wait_for(60s) timeout.  If idle for 60s, the
///   worker exits.  Next Initialize() call creates replacement threads.
/// - RunWorkers() is re-entrant: workers detect new rounds even if they
///   missed a notify_all() while finishing the previous round.
/// - Thread-creation barrier ensures workers have parked before return.
class GcWorkerPool {
public:
    /// Default max = cores - 1, floor 1.
    static int DefaultMaxWorkers() noexcept {
        int hc = static_cast<int>(std::thread::hardware_concurrency());
        return (hc > 1) ? (hc - 1) : 1;
    }

    /// Idle timeout before a worker thread exits (60 seconds).
    static constexpr auto kIdleTimeout = std::chrono::seconds(60);

    /// Maximum number of worker threads (hard limit).
    static constexpr int kMaxWorkers = 64;

    static GcWorkerPool& Instance() noexcept {
        static GcWorkerPool pool;
        return pool;
    }

    /// Ensure at least `count - 1` pool threads exist (caller is worker 0).
    /// Idempotent: only grows, never shrinks.  Replaces exited idle workers.
    void Initialize(int count) noexcept;

    /// Run a parallel job with `count` participants (including caller).
    /// Caller participates as worker 0; workers 1..count-1 are pool threads.
    /// Blocks until all participants complete.
    /// @param count  Total participants (1 = caller only, no pool dispatch).
    /// @param fn     Work function receiving worker index (0 = caller).
    void RunWorkers(int count, std::function<void(int)> fn) noexcept;

    /// Set the maximum number of pool worker threads.
    /// If count < current workers, applies on next Initialize().
    void SetMaxWorkers(int max_workers) noexcept {
        max_workers_ = (max_workers < 1) ? 1 : max_workers;
    }

    /// Get the current maximum worker count.
    int GetMaxWorkers() const noexcept {
        return max_workers_.load(std::memory_order_relaxed);
    }

    /// Shutdown all worker threads (runtime teardown only).
    void Shutdown() noexcept;

private:
    GcWorkerPool() noexcept {
        max_workers_.store(DefaultMaxWorkers(), std::memory_order_relaxed);
    }
    ~GcWorkerPool() noexcept { Shutdown(); }
    GcWorkerPool(const GcWorkerPool&) = delete;
    GcWorkerPool& operator=(const GcWorkerPool&) = delete;

    void WorkerLoop(int worker_idx) noexcept;

    // Dynamic max workers (adjustable via SetMaxWorkers).
    std::atomic<int> max_workers_;

    // Thread storage — dynamically sized up to max_workers_.
    std::thread workers_[kMaxWorkers];
    std::atomic<bool> active_[kMaxWorkers]{};
    int created_count_{0};

    // Per-round state
    std::function<void(int)> work_fn_;
    std::atomic<int> round_{0};
    std::atomic<int> completed_{0};
    int expected_completed_{0};

    std::mutex mtx_;
    std::condition_variable cv_;
    std::atomic<bool> shutdown_{false};

    // Thread-creation barrier
    std::atomic<int> ready_count_{0};
};

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_WORKER_POOL_H_
