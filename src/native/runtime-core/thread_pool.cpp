#include "thread_pool.h"
#include "timer_queue.h"
#include "thread_state.h"
#include "execution_context.h"
#include "gc_transition.h"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <deque>
#include <mutex>
#include <queue>
#include <random>
#include <thread>
#include <vector>
#include <algorithm>

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
#endif

namespace chaos::il2cpp::runtime_core::threading {

// ── Global thread pool state ──────────────────────────────────────────

namespace {

/// Protects global queue and worker lifecycle.
CHAOS_IL2CPP_MUTEX                        s_mutex;

/// Global FIFO queue for externally-pushed work items.
std::queue<WorkItem>                      s_global_queue;

/// Signalled when new work arrives or shutdown is requested.
CHAOS_IL2CPP_CONDITION_VARIABLE           s_work_available;

/// Active worker threads.
std::vector<std::thread>                  s_workers;

/// Per-worker local queues for work-stealing.
std::vector<WorkerLocalQueue*>            s_worker_queues;

/// Gate thread.
std::thread                               s_gate_thread;

/// Set to true to signal shutdown.
std::atomic<bool>                         s_shutdown{false};

/// Counters.
std::atomic<int32_t>                      s_busy_workers{0};
std::atomic<int32_t>                      s_desired_workers{kThreadPoolMinWorkerCount};
std::atomic<int32_t>                      s_queue_depth{0};
std::atomic<int32_t>                      s_completed_since_tick{0};

/// Hill-Climbing controller.
HillClimbingController                    s_hill_climbing;

/// Thread-local pointer to the current worker's local queue (for LIFO push).
thread_local WorkerLocalQueue*            tls_worker_queue = nullptr;

#if defined(_WIN32) || defined(_WIN64)
HANDLE                                    s_iocp_port = INVALID_HANDLE_VALUE;
std::thread                               s_iocp_thread;
std::atomic<int32_t>                      s_iocp_completions{0};
#endif

// ── Worker-local queue helpers ───────────────────────────────────────

WorkerLocalQueue* CreateWorkerQueue() noexcept {
    auto* q = new WorkerLocalQueue();
    return q;
}

void DestroyWorkerQueue(WorkerLocalQueue* q) noexcept {
    delete q;
}

/// Try to steal a work item from a random victim worker.
bool TryStealFromWorker(WorkItem& out, int32_t victim_count) noexcept {
    if (victim_count <= 1) return false;

    // Pick a random starting victim (thread-local random state avoids lock).
    static thread_local uint32_t tls_rng_state = static_cast<uint32_t>(
        reinterpret_cast<uintptr_t>(&out) ^ static_cast<uintptr_t>(std::hash<std::thread::id>{}(std::this_thread::get_id())));
    tls_rng_state = tls_rng_state * 1103515245u + 12345u;

    int32_t start = static_cast<int32_t>(tls_rng_state % static_cast<uint32_t>(victim_count));
    for (int32_t i = 0; i < victim_count; i++) {
        int32_t idx = (start + i) % victim_count;
        auto* vq = s_worker_queues[idx];
        // Don't steal from ourselves.
        if (vq == tls_worker_queue) continue;
        if (vq->StealFromBack(out)) return true;
    }
    return false;
}

/// Try to pop from the global queue (under s_mutex).
bool TryPopGlobal(WorkItem& out) noexcept {
    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_mutex);
    if (s_global_queue.empty()) return false;
    out = s_global_queue.front();
    s_global_queue.pop();
    s_queue_depth.fetch_sub(1, std::memory_order_relaxed);
    return true;
}

// ── Worker thread main loop ──────────────────────────────────────────

void WorkerLoop() noexcept {
    // Register thread identity.
    int32_t tid = AllocateThreadId();
    RegisterThread(tid, nullptr);
    if (auto* mt = GetCurrentThread()) {
        mt->is_threadpool = true;
    }

    // Assign local queue and register it globally.
    auto* local_q = CreateWorkerQueue();
    tls_worker_queue = local_q;
    {
        std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_mutex);
        s_worker_queues.push_back(local_q);
    }

    auto last_work_time = std::chrono::steady_clock::now();
    bool has_ever_done_work = false;

    for (;;) {
        WorkItem item;

        // 1) Try local queue (LIFO — cache friendly).
        if (local_q->PopFront(item)) {
            last_work_time = std::chrono::steady_clock::now();
            has_ever_done_work = true;
            goto execute;
        }

        // 2) Try stealing from random victim (work-stealing).
        {
            int32_t victim_count;
            {
                std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_mutex);
                victim_count = static_cast<int32_t>(s_worker_queues.size());
            }
            if (TryStealFromWorker(item, victim_count)) {
                last_work_time = std::chrono::steady_clock::now();
                has_ever_done_work = true;
                goto execute;
            }
        }

        // 3) Try global queue.
        if (TryPopGlobal(item)) {
            last_work_time = std::chrono::steady_clock::now();
            has_ever_done_work = true;
            goto execute;
        }

        // 4) No work found — wait with idle reclamation.
        {
            std::unique_lock<CHAOS_IL2CPP_MUTEX> lock(s_mutex);

            // Tier 2D: Idle reclamation — exit if idle for 30s and above min.
            if (has_ever_done_work && s_workers.size() > kThreadPoolMinWorkerCount) {
                auto now = std::chrono::steady_clock::now();
                if (now - last_work_time >= kThreadPoolIdleTimeout) {
                    // Before exiting, check one more time if work appeared.
                    if (s_global_queue.empty() && !s_shutdown.load(std::memory_order_relaxed)) {
                        // Remove our queue from the global list.
                        auto it = std::find(s_worker_queues.begin(), s_worker_queues.end(), local_q);
                        if (it != s_worker_queues.end()) s_worker_queues.erase(it);
                        break;  // Exit worker loop — thread will terminate.
                    }
                }
            }

            // Tier 3E: GC safepoint-aware wait — wake every 100ms to check GC.
            s_work_available.wait_for(lock, std::chrono::milliseconds(100), [] {
                return !s_global_queue.empty() || s_shutdown.load(std::memory_order_acquire);
            });

            if (s_shutdown.load(std::memory_order_acquire)) {
                // Remove our queue before exit.
                auto it = std::find(s_worker_queues.begin(), s_worker_queues.end(), local_q);
                if (it != s_worker_queues.end()) s_worker_queues.erase(it);
                break;
            }

            // Peek at global queue (still under lock) to grab an item immediately.
            if (!s_global_queue.empty()) {
                item = s_global_queue.front();
                s_global_queue.pop();
                s_queue_depth.fetch_sub(1, std::memory_order_relaxed);
                has_ever_done_work = true;
                last_work_time = std::chrono::steady_clock::now();
                goto execute;
            }

            // Work appeared but was taken by another worker — loop back.
            continue;
        }

    execute:
        // Execute work item outside any lock.
        s_busy_workers.fetch_add(1, std::memory_order_relaxed);
        if (item.callback) {
            GC_TRANSITION_TO_COOPERATIVE();
            // EC capture only happens in the standard QueueUserWorkItem path,
            // where the ContextRun wrapper is already set up.
            item.callback(item.context);
            GC_TRANSITION_TO_PREEMPTIVE();
            s_completed_since_tick.fetch_add(1, std::memory_order_relaxed);
        }
        s_busy_workers.fetch_sub(1, std::memory_order_relaxed);
    }

    DestroyWorkerQueue(local_q);
    tls_worker_queue = nullptr;
    UnregisterThread();
}

void EnsureWorkerCount(int32_t desired) noexcept {
    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_mutex);
    while (static_cast<int32_t>(s_workers.size()) < desired) {
        s_workers.emplace_back(WorkerLoop);
    }
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
            s_iocp_port, &bytes_transferred, &completion_key, &overlapped, INFINITE);

        if (s_shutdown.load(std::memory_order_acquire)) break;

        if (ok && completion_key != 0) {
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
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        if (s_shutdown.load(std::memory_order_relaxed)) break;

        TimerQueueOnTick();
        ThreadPoolGateTick();
    }

    UnregisterThread();
}

}  // anonymous namespace

// ── HillClimbingController implementation ─────────────────────────────
// Full state machine: Warmup → ClimbExplore → Climbing → Stabilizing → Steady

int32_t HillClimbingController::OnGateTick(int32_t completed_count, int32_t current_threads) noexcept {
    // Record throughput sample (moving average window).
    samples_[sample_index_ % kHillClimbingSampleWindow] = completed_count;
    sample_index_++;
    if (sample_count_ < kHillClimbingSampleWindow) sample_count_++;

    int32_t total = 0;
    for (uint32_t i = 0; i < sample_count_; i++) total += samples_[i];
    int32_t avg_throughput = (sample_count_ > 0) ? (total / static_cast<int32_t>(sample_count_)) : 0;

    int32_t suggestion = current_threads;

    switch (state_) {
    case HillClimbState::Warmup:
        // Ramp up quickly to minimum configured worker count.
        if (current_threads < kHillClimbingMinWorker + 2 && completed_count > 0) {
            suggestion = current_threads + 1;
        } else {
            // Transition to active state based on load.
            if (avg_throughput > 0 && current_threads >= kThreadPoolMinWorkerCount) {
                state_ = HillClimbState::ClimbExplore;
                wave_ticks_ = 0;
                wave_threads_ = 0;
                pre_wave_throughput_ = avg_throughput;
            }
        }
        break;

    case HillClimbState::ClimbExplore: {
        // Inject 2 threads as a wave, measure throughput change after 2 ticks.
        if (wave_ticks_ == 0 && wave_threads_ == 0) {
            wave_threads_ = 2;
            pre_wave_throughput_ = avg_throughput;
        }

        if (wave_ticks_ < 2) {
            // Still in the wave — keep the injected threads.
            suggestion = (std::min)(current_threads + wave_threads_, kHillClimbingMaxWorker);
            wave_ticks_++;
        } else {
            // Wave complete — compute gain.
            int32_t gain = avg_throughput - pre_wave_throughput_;
            // Avoid division by zero for wave_threads_.
            int32_t adj_gain = (wave_threads_ > 0) ? (gain / wave_threads_) : gain;

            if (adj_gain > 5) {
                state_ = HillClimbState::Climbing;
                suggestion = (std::min)(current_threads + 1, kHillClimbingMaxWorker);
            } else if (adj_gain < -5) {
                state_ = HillClimbState::Stabilizing;
                suggestion = (std::max)(current_threads - wave_threads_, kHillClimbingMinWorker);
            } else {
                state_ = HillClimbState::Steady;
                // Return to pre-wave thread count.
                suggestion = (std::max)(current_threads - wave_threads_, kThreadPoolMinWorkerCount);
            }
            wave_ticks_ = 0;
            wave_threads_ = 0;
        }
        break;
    }

    case HillClimbState::Climbing:
        if (avg_throughput > last_throughput_) {
            // Still improving — keep climbing.
            suggestion = (std::min)(current_threads + 1, kHillClimbingMaxWorker);
        } else {
            // Plateau or drop — stabilize.
            state_ = HillClimbState::Steady;
        }
        break;

    case HillClimbState::Stabilizing:
        if (avg_throughput < last_throughput_ && current_threads > kHillClimbingMinWorker) {
            // Still dropping — pull back more.
            suggestion = (std::max)(current_threads - 1, kHillClimbingMinWorker);
        } else {
            // Recovered — go steady.
            state_ = HillClimbState::Steady;
        }
        break;

    case HillClimbState::Steady:
        // Hold steady. If throughput drops significantly, re-probe.
        if (avg_throughput < last_throughput_ / 2 && completed_count > 0) {
            state_ = HillClimbState::ClimbExplore;
            wave_ticks_ = 0;
            wave_threads_ = 0;
            pre_wave_throughput_ = avg_throughput;
        } else if (completed_count == 0 && current_threads > kHillClimbingMinWorker) {
            // Idle — slowly shrink.
            suggestion = (std::max)(current_threads - 1, kHillClimbingMinWorker);
        }
        break;
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
    state_ = HillClimbState::Warmup;
    wave_threads_ = 0;
    wave_ticks_ = 0;
    pre_wave_throughput_ = 0;
    for (auto& s : samples_) s = 0;
}

// ── Context-free callback wrapper ────────────────────────────────────
// The unsafe QUWI path wraps a callback+context directly.
// The safe QUWI path wraps EC capture+flow+free around the callback.

struct ContextRunItem {
    void (*callback)(void*);
    void* context;
};

static void ContextRunWrapper(void* raw) noexcept {
    auto* item = static_cast<ContextRunItem*>(raw);
    auto* ctx = ExecutionContextCapture();
    ExecutionContextRun(ctx, item->callback, item->context);
    ExecutionContextFree(ctx);
    delete item;
}

// ── Public API ────────────────────────────────────────────────────────

void ThreadPoolInitialize() noexcept {
    s_shutdown.store(false, std::memory_order_relaxed);
    s_desired_workers.store(kThreadPoolMinWorkerCount, std::memory_order_relaxed);
    s_completed_since_tick.store(0, std::memory_order_relaxed);
    s_hill_climbing.Reset();

    EnsureWorkerCount(kThreadPoolMinWorkerCount);

#if defined(_WIN32) || defined(_WIN64)
    s_iocp_port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 1);
    if (s_iocp_port != INVALID_HANDLE_VALUE) {
        s_iocp_thread = std::thread(IOCPWorkerLoop);
    }
#endif

    s_gate_thread = std::thread(GateThreadLoop);
}

void ThreadPoolShutdown() noexcept {
    s_shutdown.store(true, std::memory_order_release);
    s_work_available.notify_all();

#if defined(_WIN32) || defined(_WIN64)
    if (s_iocp_port != INVALID_HANDLE_VALUE) {
        PostQueuedCompletionStatus(s_iocp_port, 0, 0, nullptr);
    }
    if (s_iocp_thread.joinable()) s_iocp_thread.join();
    if (s_iocp_port != INVALID_HANDLE_VALUE) {
        CloseHandle(s_iocp_port);
        s_iocp_port = INVALID_HANDLE_VALUE;
    }
#endif

    for (auto& t : s_workers) {
        if (t.joinable()) t.join();
    }
    s_workers.clear();

    // Clean up worker queues.
    for (auto* q : s_worker_queues) DestroyWorkerQueue(q);
    s_worker_queues.clear();

    if (s_gate_thread.joinable()) s_gate_thread.join();

    {
        std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_mutex);
        while (!s_global_queue.empty()) s_global_queue.pop();
        s_queue_depth.store(0, std::memory_order_relaxed);
    }
}

void ThreadPoolQueueUserWorkItem(void (*callback)(void*), void* context) noexcept {
    if (callback == nullptr) return;

    // Wrap with ExecutionContext capture.
    auto* item = new ContextRunItem{callback, context};
    ThreadPoolQueueUserWorkItemUnsafe(ContextRunWrapper, item);
}

void ThreadPoolQueueUserWorkItemUnsafe(void (*callback)(void*), void* context) noexcept {
    if (callback == nullptr) return;

    {
        std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_mutex);
        s_global_queue.push({callback, context});
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
    int32_t completed = s_completed_since_tick.exchange(0, std::memory_order_relaxed);

#if defined(_WIN32) || defined(_WIN64)
    completed += s_iocp_completions.exchange(0, std::memory_order_relaxed);
#endif

    int32_t current = static_cast<int32_t>(s_workers.size());
    int32_t target = s_hill_climbing.OnGateTick(completed, current);

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
    if (s_iocp_port == INVALID_HANDLE_VALUE) {
        s_iocp_port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 1);
    }
}

std::atomic<int32_t> g_iocp_completions{0};
#endif

}  // namespace chaos::il2cpp::runtime_core::threading
