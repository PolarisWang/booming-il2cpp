#include "gc_worker_pool.h"

#include <chaos/log.h>

namespace chaos::il2cpp::runtime_core {

void GcWorkerPool::Initialize(int count) noexcept {
    if (count < 1) count = 1;
    int max_slots = GetMaxWorkers();
    if (count > max_slots + 1) count = max_slots + 1;

    int need = count - 1;
    if (need <= 0) return;

    // Count currently active workers.
    int active_count = 0;
    for (int i = 0; i < created_count_; i++) {
        if (active_[i].load(std::memory_order_acquire)) {
            active_count++;
        }
    }

    int deficit = need - active_count;
    if (deficit <= 0) return;

    int spawned = 0;

    // Phase 1: reuse exited worker slots (inactive, thread has returned).
    for (int i = 0; i < created_count_ && deficit > 0; i++) {
        if (!active_[i].load(std::memory_order_acquire)) {
            if (workers_[i].joinable()) {
                workers_[i].join();
            }
            int idx = i + 1;
            active_[i].store(true, std::memory_order_relaxed);
            workers_[i] = std::thread([this, idx]() noexcept { WorkerLoop(idx); });
            deficit--;
            spawned++;
        }
    }

    // Phase 2: create new workers at the end.
    while (deficit > 0 && created_count_ < max_slots) {
        int i = created_count_++;
        int idx = i + 1;
        active_[i].store(true, std::memory_order_relaxed);
        workers_[i] = std::thread([this, idx]() noexcept { WorkerLoop(idx); });
        deficit--;
        spawned++;
    }

    if (deficit > 0) {
        CHAOS_IL2CPP_LOG_WARN_M("GcPool", "Initialize wanted={0} active={1} max={2} short={3}",
                              need, active_count, max_slots, deficit);
    }

    // Spin until all newly spawned threads have parked in cv_.wait().
    if (spawned > 0) {
        int target = ready_count_.load(std::memory_order_acquire) + spawned;
        while (ready_count_.load(std::memory_order_acquire) < target) {
            std::this_thread::yield();
        }
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

    // Count active workers for the completion barrier.
    int requested_pool = count - 1;
    int active_pool = 0;
    for (int i = 0; i < created_count_; i++) {
        if (active_[i].load(std::memory_order_relaxed)) active_pool++;
    }
    expected_completed_ = (std::min)(requested_pool, active_pool);
    if (requested_pool != expected_completed_) {
        CHAOS_IL2CPP_LOG_WARN_M("GcPool", "RunWorkers requested={0} active_pool={1} expected={2}",
                              requested_pool, active_pool, expected_completed_);
    }

    // Bump round to signal workers to start.
    round_.fetch_add(1, std::memory_order_release);
    cv_.notify_all();

    // Main thread participates as worker 0.
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
    int slot = worker_idx - 1;
    bool first_park = true;
    int observed = round_.load(std::memory_order_acquire);

    while (!shutdown_.load(std::memory_order_acquire)) {
        // Fast-path: if round_ already changed while we weren't holding the
        // mutex (a tight-loop RunWorkers incremented it), skip the wait
        // entirely and go straight to executing.
        int current_round = round_.load(std::memory_order_acquire);
        if (current_round != observed) {
            if (shutdown_.load(std::memory_order_acquire)) break;
            observed = current_round;
            if (work_fn_) {
                work_fn_(worker_idx);
            }
            completed_.fetch_add(1, std::memory_order_release);
            continue;
        }

        {
            std::unique_lock<std::mutex> lock(mtx_);
            if (first_park) {
                ready_count_.fetch_add(1, std::memory_order_release);
                first_park = false;
            }

            // Double-check after acquiring the mutex: another thread may have
            // bumped round_ between the fast-path check and the lock acquisition.
            current_round = round_.load(std::memory_order_acquire);
            if (current_round != observed) {
                // Round changed — don't park, fall through to execute.
            } else {
                auto parked = cv_.wait_for(lock, kIdleTimeout, [this, &observed]() noexcept {
                    int current = round_.load(std::memory_order_acquire);
                    return current != observed || shutdown_.load(std::memory_order_acquire);
                });

                if (!parked) {
                    // Idle timeout: no work for 60s.  Mark slot inactive and exit.
                    active_[slot].store(false, std::memory_order_release);
                    return;
                }
            }
        }

        if (shutdown_.load(std::memory_order_acquire)) break;

        observed = round_.load(std::memory_order_acquire);

        if (work_fn_) {
            work_fn_(worker_idx);
        }

        completed_.fetch_add(1, std::memory_order_release);
    }
}

}  // namespace chaos::il2cpp::runtime_core
