#include "thread_pool.h"
#include "timer_queue.h"
#include "thread_state.h"
#include "execution_context.h"
#include "gc_transition.h"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <algorithm>

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
#endif

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

/// Desired worker thread count (managed by Hill-Climbing gate tick).
std::atomic<int32_t>                      s_desired_workers{kThreadPoolMinWorkerCount};

/// Number of work items queued.
std::atomic<int32_t>                      s_queue_depth{0};

/// Number of work items completed since last gate tick.
std::atomic<int32_t>                      s_completed_since_tick{0};

/// Hill-Climbing controller instance.
HillClimbingController                    s_hill_climbing;

#if defined(_WIN32) || defined(_WIN64)
/// I/O completion port handle.
HANDLE                                    s_iocp_port = INVALID_HANDLE_VALUE;

/// IOCP worker thread.
std::thread                               s_iocp_thread;

/// IOCP completions since last gate tick.
std::atomic<int32_t>                      s_iocp_completions{0};
#endif

void WorkerLoop() noexcept {
    // Register this thread for managed identity.
    int32_t tid = AllocateThreadId();
    RegisterThread(tid, nullptr);
    // Mark as thread-pool worker for IsThreadPoolThread.
    if (auto* mt = GetCurrentThread()) {
        mt->is_threadpool = true;
    }

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

        // Execute the work item outside the lock, flowing ExecutionContext.
        s_busy_workers.fetch_add(1, std::memory_order_relaxed);
        if (item.callback) {
            GC_TRANSITION_TO_COOPERATIVE();
            // Capture and flow ExecutionContext for AsyncLocal consistency.
            auto* ctx = ExecutionContextCapture();
            ExecutionContextRun(ctx, item.callback, item.context);
            ExecutionContextFree(ctx);
            GC_TRANSITION_TO_PREEMPTIVE();
            s_completed_since_tick.fetch_add(1, std::memory_order_relaxed);
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

#if defined(_WIN32) || defined(_WIN64)
void IOCPWorkerLoop() noexcept {
    int32_t tid = AllocateThreadId();
    RegisterThread(tid, nullptr);

    for (;;) {
        DWORD bytes_transferred = 0;
        ULONG_PTR completion_key = 0;
        OVERLAPPED* overlapped = nullptr;

        BOOL ok = GetQueuedCompletionStatus(
            s_iocp_port,
            &bytes_transferred,
            &completion_key,
            &overlapped,
            INFINITE);

        if (s_shutdown.load(std::memory_order_acquire)) {
            break;
        }

        if (ok && completion_key != 0) {
            // completion_key is the callback function pointer.
            auto* callback = reinterpret_cast<void(*)(void*)>(completion_key);
            callback(overlapped);
            s_iocp_completions.fetch_add(1, std::memory_order_relaxed);
        }
    }

    UnregisterThread();
}
#endif

void GateThreadLoop() noexcept {
    RegisterThread(AllocateThreadId(), nullptr);

    while (!s_shutdown.load(std::memory_order_relaxed)) {
        // Sleep ~500 ms between gate ticks.
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        if (s_shutdown.load(std::memory_order_relaxed)) break;

        // Fire due timers.
        TimerQueueOnTick();

        ThreadPoolGateTick();
    }

    UnregisterThread();
}

}  // anonymous namespace

// ── HillClimbingController implementation ─────────────────────────────

int32_t HillClimbingController::OnGateTick(int32_t completed_count, int32_t current_threads) noexcept {
    // Record throughput sample.
    samples_[sample_index_ % kHillClimbingSampleWindow] = completed_count;
    sample_index_++;
    if (sample_count_ < kHillClimbingSampleWindow) {
        sample_count_++;
    }

    // Compute moving average throughput.
    int32_t total = 0;
    for (uint32_t i = 0; i < sample_count_; i++) {
        total += samples_[i];
    }
    int32_t avg_throughput = (sample_count_ > 0) ? (total / static_cast<int32_t>(sample_count_)) : 0;

    // Hill-Climbing heuristic:
    //   - If throughput increased and we added threads, keep adding (ramp up).
    //   - If throughput decreased, pull back one thread.
    //   - If queue is deep, add threads aggressively.
    //   - If idle, slowly shrink to min.
    int32_t suggestion = current_threads;

    if (current_threads <= 1 && completed_count > 0) {
        // Minimum: grow to handle load.
        suggestion = 2;
    } else if (avg_throughput > last_throughput_ && current_threads >= last_thread_count_) {
        // Throughput improving with more threads — keep adding.
        suggestion = (std::min)(current_threads + 1, kHillClimbingMaxWorker);
    } else if (avg_throughput < last_throughput_ && current_threads > kHillClimbingMinWorker) {
        // Throughput dropped — pull back.
        suggestion = (std::max)(current_threads - 1, kHillClimbingMinWorker);
    } else if (completed_count == 0 && current_threads > kHillClimbingMinWorker) {
        // Idle — slowly shrink.
        suggestion = (std::max)(current_threads - 1, kHillClimbingMinWorker);
    }

    last_throughput_ = avg_throughput;
    last_thread_count_ = current_threads;
    return suggestion;
}

void HillClimbingController::Reset() noexcept {
    sample_index_ = 0;
    sample_count_ = 0;
    last_throughput_ = 0;
    last_thread_count_ = kHillClimbingMinWorker;
    for (auto& s : samples_) s = 0;
}

// ── Public API ────────────────────────────────────────────────────────

void ThreadPoolInitialize() noexcept {
    s_shutdown.store(false, std::memory_order_relaxed);
    s_desired_workers.store(kThreadPoolMinWorkerCount, std::memory_order_relaxed);
    s_completed_since_tick.store(0, std::memory_order_relaxed);
    s_hill_climbing.Reset();

    EnsureWorkerCount(kThreadPoolMinWorkerCount);

#if defined(_WIN32) || defined(_WIN64)
    // Create I/O completion port.
    s_iocp_port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 1);
    if (s_iocp_port != INVALID_HANDLE_VALUE) {
        s_iocp_thread = std::thread(IOCPWorkerLoop);
    }
#endif

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

#if defined(_WIN32) || defined(_WIN64)
    // Post a shutdown completion packet to wake the IOCP thread.
    if (s_iocp_port != INVALID_HANDLE_VALUE) {
        PostQueuedCompletionStatus(s_iocp_port, 0, 0, nullptr);
    }
    if (s_iocp_thread.joinable()) {
        s_iocp_thread.join();
    }
    if (s_iocp_port != INVALID_HANDLE_VALUE) {
        CloseHandle(s_iocp_port);
        s_iocp_port = INVALID_HANDLE_VALUE;
    }
#endif

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
    // Read and reset completed count.
    int32_t completed = s_completed_since_tick.exchange(0, std::memory_order_relaxed);

#if defined(_WIN32) || defined(_WIN64)
    // Include IOCP completions in throughput measurement.
    completed += s_iocp_completions.exchange(0, std::memory_order_relaxed);
#endif

    int32_t current = static_cast<int32_t>(s_workers.size());

    // Run Hill-Climbing to determine target worker count.
    int32_t target = s_hill_climbing.OnGateTick(completed, current);

    // Clamp to bounds.
    target = (std::max)(target, kThreadPoolMinWorkerCount);
    target = (std::min)(target, kThreadPoolMaxWorkerCount);

    s_desired_workers.store(target, std::memory_order_relaxed);
    EnsureWorkerCount(target);
}

int32_t ThreadPoolWorkerCount() noexcept {
    return static_cast<int32_t>(s_workers.size());
}

#if defined(_WIN32) || defined(_WIN64)
void ThreadPoolInitializeIOCP() noexcept {
    // Called externally if IOCP needs re-initialization after shutdown.
    if (s_iocp_port == INVALID_HANDLE_VALUE) {
        s_iocp_port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 1);
    }
}

std::atomic<int32_t> g_iocp_completions{0};
#endif

}  // namespace chaos::il2cpp::runtime_core::threading