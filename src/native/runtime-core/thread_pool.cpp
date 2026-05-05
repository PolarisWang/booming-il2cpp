#include "thread_pool.h"
#include "thread_state.h"
#include "gc_transition.h"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace chaos::il2cpp::runtime_core::threading {

// ── Work item ─────────────────────────────────────────────────────────

struct WorkItem {
    void (*callback)(void*) = nullptr;
    void* context           = nullptr;
};

// ── Global thread pool state ──────────────────────────────────────────

namespace {

/// Protects all shared state below.
CHAOS_IL2CPP_MUTEX                        s_mutex;

/// FIFO queue of pending work items.
std::queue<WorkItem>                      s_queue;

/// Signalled when new work arrives or shutdown is requested.
CHAOS_IL2CPP_CONDITION_VARIABLE           s_work_available;

/// Active worker threads.
std::vector<std::thread>                  s_workers;

/// Gate thread (periodic timer management).
std::thread                               s_gate_thread;

/// Set to true to signal shutdown.
std::atomic<bool>                         s_shutdown{false};

/// Counter of threads currently executing work items.
std::atomic<int32_t>                      s_busy_workers{0};

/// Desired worker thread count (managed by gate tick).
std::atomic<int32_t>                      s_desired_workers{kThreadPoolMinWorkerCount};

/// Number of work items queued.
std::atomic<int32_t>                      s_queue_depth{0};

void WorkerLoop() noexcept {
    // Register this thread for managed identity.
    int32_t tid = AllocateThreadId();
    RegisterThread(tid, nullptr);

    for (;;) {
        WorkItem item;

        {
            std::unique_lock<CHAOS_IL2CPP_MUTEX> lock(s_mutex);

            // Wait until there's work or shutdown.
            s_work_available.wait(lock, [] {
                return !s_queue.empty() || s_shutdown.load(std::memory_order_acquire);
            });

            if (s_shutdown.load(std::memory_order_acquire)) {
                break;
            }

            item = s_queue.front();
            s_queue.pop();
            s_queue_depth.fetch_sub(1, std::memory_order_relaxed);
        }

        // Execute the work item outside the lock.
        // Transition to cooperative mode — bridge stubs inside the callback
        // will transition to preemptive as needed.
        s_busy_workers.fetch_add(1, std::memory_order_relaxed);
        if (item.callback) {
            GC_TRANSITION_TO_COOPERATIVE();
            item.callback(item.context);
            GC_TRANSITION_TO_PREEMPTIVE();
        }
        s_busy_workers.fetch_sub(1, std::memory_order_relaxed);
    }

    UnregisterThread();
}

void EnsureWorkerCount(int32_t desired) noexcept {
    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_mutex);
    while (static_cast<int32_t>(s_workers.size()) < desired) {
        s_workers.emplace_back(WorkerLoop);
    }
    // Workers above desired are left to idle — they will exit on shutdown.
}

void GateThreadLoop() noexcept {
    RegisterThread(AllocateThreadId(), nullptr);

    while (!s_shutdown.load(std::memory_order_relaxed)) {
        // Sleep ~500 ms between gate ticks.
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        if (s_shutdown.load(std::memory_order_relaxed)) break;

        ThreadPoolGateTick();
    }

    UnregisterThread();
}

}  // anonymous namespace

// ── Public API ────────────────────────────────────────────────────────

void ThreadPoolInitialize() noexcept {
    s_shutdown.store(false, std::memory_order_relaxed);
    s_desired_workers.store(kThreadPoolMinWorkerCount, std::memory_order_relaxed);

    EnsureWorkerCount(kThreadPoolMinWorkerCount);

    // Start the gate thread.
    s_gate_thread = std::thread(GateThreadLoop);
}

void ThreadPoolShutdown() noexcept {
    s_shutdown.store(true, std::memory_order_release);

    // Wake all workers.
    s_work_available.notify_all();

    // Wake the gate thread.
    {
        std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_mutex);
        s_work_available.notify_all();
    }

    // Join all worker threads.
    for (auto& t : s_workers) {
        if (t.joinable()) t.join();
    }
    s_workers.clear();

    // Join the gate thread.
    if (s_gate_thread.joinable()) {
        s_gate_thread.join();
    }

    // Drain the queue.
    {
        std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_mutex);
        while (!s_queue.empty()) {
            s_queue.pop();
        }
        s_queue_depth.store(0, std::memory_order_relaxed);
    }
}

void ThreadPoolQueueUserWorkItem(void (*callback)(void*), void* context) noexcept {
    if (callback == nullptr) return;

    {
        std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_mutex);
        s_queue.push({callback, context});
        s_queue_depth.fetch_add(1, std::memory_order_relaxed);
    }

    s_work_available.notify_one();

    // Grow workers if queue depth exceeds active worker capacity.
    int32_t depth = s_queue_depth.load(std::memory_order_relaxed);
    int32_t current_workers = static_cast<int32_t>(s_workers.size());
    if (depth > current_workers && current_workers < kThreadPoolMaxWorkerCount) {
        s_desired_workers.store(
            (std::min)(current_workers + 1, kThreadPoolMaxWorkerCount),
            std::memory_order_relaxed);
        EnsureWorkerCount(s_desired_workers.load(std::memory_order_relaxed));
    }
}

void ThreadPoolGateTick() noexcept {
    // Adjust worker count based on backlog.
    int32_t backlog = s_queue_depth.load(std::memory_order_relaxed);
    int32_t current = static_cast<int32_t>(s_workers.size());

    if (backlog > current && current < kThreadPoolMaxWorkerCount) {
        int32_t target = (std::min)(current + 1, kThreadPoolMaxWorkerCount);
        s_desired_workers.store(target, std::memory_order_relaxed);
        EnsureWorkerCount(target);
    } else if (backlog == 0 && current > kThreadPoolMinWorkerCount) {
        // Slowly shrink when idle (hill-climbing style).
        int32_t target = (std::max)(current - 1, kThreadPoolMinWorkerCount);
        s_desired_workers.store(target, std::memory_order_relaxed);
    }
}

int32_t ThreadPoolWorkerCount() noexcept {
    return static_cast<int32_t>(s_workers.size());
}

}  // namespace chaos::il2cpp::runtime_core::threading
