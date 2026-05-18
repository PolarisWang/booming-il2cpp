#include "gc_worker_pool.h"

#include <chaos/log.h>

#include <cstdint>

namespace chaos::il2cpp::runtime_core {

void GcWorkerPool::Initialize(int count) noexcept {
    if (count < 1) count = 1;
    if (count > kMaxWorkers + 1) count = kMaxWorkers + 1;

    int need = count - 1;  // caller is worker 0
    if (need <= created_count_) return;  // already have enough

    int start = created_count_;
    int target_ready = ready_count_.load(std::memory_order_acquire) + (need - start);
    for (int i = start; i < need; i++) {
        int idx = i + 1;  // worker index 1..N
        workers_[i] = std::thread([this, idx]() noexcept {
            WorkerLoop(idx);
        });
        created_count_ = i + 1;
    }

    // Spin until all newly created threads have parked in cv_.wait().
    // This eliminates the race where a worker thread hasn't yet entered
    // the wait loop and would therefore miss the round_ bump + notify_all.
    while (ready_count_.load(std::memory_order_acquire) < target_ready) {
        std::this_thread::yield();
    }
}

void GcWorkerPool::RunWorkers(int count, std::function<void(int)> fn) noexcept {
    if (count <= 1) {
        fn(0);
        return;
    }

    Initialize(count);

    work_fn_ = std::move(fn);
    completed_.store(0, std::memory_order_relaxed);
    // Use actual worker count after cap — Initialize() may reduce count
    // to kMaxWorkers+1.  Also cap to requested pool size so that calling
    // RunWorkers(4) after RunWorkers(24) doesn't wait for created_count_=7
    // completions when only 3 workers participate (infinite hang).
    int requested_pool = count - 1;
    expected_completed_ = (std::min)(requested_pool, created_count_);
    if (requested_pool != expected_completed_) {
        CHAOS_IL2CPP_LOG_WARN_M("GcPool", "RunWorkers requested={0} pool={1} expected={2}",
                              requested_pool, created_count_, expected_completed_);
    }

    // Bump round to signal workers to start.  acquire-release ensures
    // workers see work_fn_ and completed_ store above.
    round_.fetch_add(1, std::memory_order_release);
    cv_.notify_all();

    // Main thread participates as worker 0.
    // IMPORTANT: use work_fn_ not fn — fn was moved into work_fn_ above!
    work_fn_(0);

    // Wait for all pool workers to complete.
    while (completed_.load(std::memory_order_acquire) < expected_completed_) {
        std::this_thread::yield();
    }
}

void GcWorkerPool::Shutdown() noexcept {
    shutdown_.store(true, std::memory_order_release);
    cv_.notify_all();
    for (int i = 0; i < created_count_; i++) {
        if (workers_[i].joinable()) {
            workers_[i].join();
        }
    }
    created_count_ = 0;
}

void GcWorkerPool::WorkerLoop(int worker_idx) noexcept {
    bool first_park = true;
    int observed = round_.load(std::memory_order_acquire);

    while (!shutdown_.load(std::memory_order_acquire)) {
        // Wait for a new round or shutdown.
        {
            std::unique_lock<std::mutex> lock(mtx_);
            if (first_park) {
                // Signal the creating thread that we've parked.
                // This must happen while holding mtx_ so that the
                // subsequent cv_.wait() is guaranteed to observe any
                // future round_ bump + notify_all().
                ready_count_.fetch_add(1, std::memory_order_release);
                first_park = false;
            }
            cv_.wait(lock, [this, &observed]() noexcept {
                int current = round_.load(std::memory_order_acquire);
                return current != observed || shutdown_.load(std::memory_order_acquire);
            });
        }

        if (shutdown_.load(std::memory_order_acquire)) break;

        // Capture the current round before executing.
        observed = round_.load(std::memory_order_acquire);

        if (work_fn_) {
            work_fn_(worker_idx);
        }

        completed_.fetch_add(1, std::memory_order_release);
    }
}

}  // namespace chaos::il2cpp::runtime_core
