#ifndef CHAOS_IL2CPP_GC_WORKER_POOL_H_
#define CHAOS_IL2CPP_GC_WORKER_POOL_H_

#include <atomic>
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
/// - Threads created at first Initialize() call, never destroyed until
///   Shutdown() (no create/join per GC cycle).
/// - Workers park on cv with round-generation counter for wake-up.
/// - RunWorkers() is re-entrant: workers detect new rounds even if they
///   missed a notify_all() while finishing the previous round.
/// - Max 7 worker threads (kMaxWorkers), capped for iOS stack budget
///   and to avoid oversubscription on high-core-count machines.
class GcWorkerPool {
public:
    static constexpr int kMaxWorkers = 7;

    static GcWorkerPool& Instance() noexcept {
        static GcWorkerPool pool;
        return pool;
    }

    /// Ensure at least `count - 1` worker threads exist (caller is worker 0).
    /// Idempotent: only grows, never shrinks.
    void Initialize(int count) noexcept;

    /// Run a parallel job with `count` participants (including caller).
    /// Caller participates as worker 0; workers 1..count-1 are pool threads.
    /// Blocks until all participants complete.
    /// @param count  Total participants (1 = caller only, no pool dispatch).
    /// @param fn     Work function receiving worker index (0 = caller).
    void RunWorkers(int count, std::function<void(int)> fn) noexcept;

    /// Shutdown all worker threads (runtime teardown only).
    void Shutdown() noexcept;

private:
    GcWorkerPool() noexcept = default;
    ~GcWorkerPool() noexcept { Shutdown(); }
    GcWorkerPool(const GcWorkerPool&) = delete;
    GcWorkerPool& operator=(const GcWorkerPool&) = delete;

    void WorkerLoop(int worker_idx) noexcept;

    std::thread workers_[kMaxWorkers];
    int created_count_{0};

    // Per-round state
    std::function<void(int)> work_fn_;
    std::atomic<int> round_{0};
    std::atomic<int> completed_{0};
    int expected_completed_{0};

    std::mutex mtx_;
    std::condition_variable cv_;
    std::atomic<bool> shutdown_{false};

    // Thread-creation barrier: incremented by each worker thread the first
    // time it parks in cv_.wait().  Initialize() spins until all newly
    // created threads have reported parked, eliminating the race where a
    // worker misses the round_ bump + notify_all() because it hadn't yet
    // entered the wait.
    std::atomic<int> ready_count_{0};
};

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_WORKER_POOL_H_
