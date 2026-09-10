#include "thread_pool.h"
#include "timer_queue.h"
#include "thread_state.h"
#include "execution_context.h"
#include "gc_transition.h"
#include "forbid_suspend.h"
#include "runtime_stubs/threadpool_events.h"

#include <chaos/pal/pal_time.h>
#include <chaos/pal/pal_wakeable.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <cstdlib>
#include <deque>
#include <mutex>
#include <queue>
#include <random>
#include <thread>
#include <vector>
#include <algorithm>

namespace chaos::il2cpp::runtime_core::threading {

using chaos::il2cpp::pal::PalWakeableCreate;
using chaos::il2cpp::pal::PalWakeablePost;
using chaos::il2cpp::pal::PalWakeableWait;
using chaos::il2cpp::pal::PalWakeableDestroy;

// ── Global thread pool state ──────────────────────────────────────────

chaos::il2cpp::pal::PalWakeable*           s_wakeable_queue = nullptr;
std::thread                               s_wakeable_thread;
std::atomic<int32_t>                      s_wakeable_completions{0};

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

/// Measure total CPU time consumed by the process (all threads) in nanoseconds.
/// Returns 0 if the platform does not support process-level CPU time measurement.
uint64_t GetProcessCpuTimeNs() noexcept {
    return chaos::il2cpp::pal::PalGetProcessCpuTimeNs();
}

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
    // Register thread identity and start in preemptive mode.
    int32_t tid = AllocateThreadId();
    RegisterThread(tid, nullptr);
    if (auto* mt = GetCurrentThread()) {
        mt->is_threadpool = true;
    }
    EnterPreemptiveMode();

    // ETW: worker created.
    ThreadPoolEventEmitWorkerCreate(tid);

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
            ThreadPoolEventEmitWorkItemDequeue(reinterpret_cast<int64_t>(item.context));
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
                    if (s_global_queue.empty() && !s_shutdown.load(std::memory_order_acquire)) {
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
                ThreadPoolEventEmitWorkItemDequeue(reinterpret_cast<int64_t>(item.context));
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

    // ETW: worker destroyed.
    ThreadPoolEventEmitWorkerDestroy(tid);

    UnregisterThread();
}

void EnsureWorkerCount(int32_t desired) noexcept {
    ForbidSuspendScope forbid;
    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_mutex);
    while (static_cast<int32_t>(s_workers.size()) < desired) {
        s_workers.emplace_back(WorkerLoop);
    }
}

void WakeableWorkerLoop() noexcept {
    int32_t tid = AllocateThreadId();
    RegisterThread(tid, nullptr);
    if (auto* mt = GetCurrentThread()) {
        mt->is_threadpool = true;
    }
    EnterPreemptiveMode();

    for (;;) {
        int n = PalWakeableWait(s_wakeable_queue, -1);  // Infinite timeout.
        if (s_shutdown.load(std::memory_order_acquire)) break;

        if (n > 0) {
            s_wakeable_completions.fetch_add(static_cast<int32_t>(n), std::memory_order_relaxed);
            ThreadPoolEventEmitIOCompletion(0);
        }
    }

    UnregisterThread();
}

void GateThreadLoop() noexcept {
    RegisterThread(AllocateThreadId(), nullptr);
    if (auto* mt = GetCurrentThread()) {
        mt->is_threadpool = true;
    }

    while (!s_shutdown.load(std::memory_order_relaxed)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
        if (s_shutdown.load(std::memory_order_relaxed)) break;

        TimerQueueOnTick();
        ThreadPoolGateTick();
    }

    UnregisterThread();
}

}  // anonymous namespace

// ── HillClimbingController implementation (V2: 9-state + Goertzel + CPU feedback) ──
//
// Design:
//   - 9 states covering warmup, climb, fix, steady, saturate, random
//   - Dual Goertzel filters detect frequency-domain patterns in throughput and CPU
//   - CPU utilization feedback: if CPU is saturated, adding threads won't help
//   - Square-wave injection in Steady state to probe system headroom
//   - Non-linear (sigmoid) gain reduction near CPU core count
//
// Goertzel setup: 8-sample window, target frequency = 1 cycle/window
// (normalized freq = 1/8 = 0.125), coeff = 2*cos(2*PI*0.125) ≈ 1.4142

int32_t HillClimbingController::OnGateTick(int32_t completed_count, int32_t current_threads) noexcept {
    // ── Record throughput sample ──────────────────────────────────
    samples_[sample_index_ % kHillClimbingSampleWindow] = completed_count;
    sample_index_++;
    if (sample_count_ < kHillClimbingSampleWindow) sample_count_++;

    int32_t total = 0;
    for (uint32_t i = 0; i < sample_count_; i++) total += samples_[i];
    int32_t avg_throughput = (sample_count_ > 0) ? (total / static_cast<int32_t>(sample_count_)) : 0;

    // ── Feed Goertzel filters ─────────────────────────────────────
    if (cpu_count_ <= 1) {
        cpu_count_ = static_cast<int32_t>(std::thread::hardware_concurrency());
        if (cpu_count_ < 1) cpu_count_ = 1;
    }
    // Initialize filters on first tick.
    if (throughput_filter_.sample_count == 0) {
        throughput_filter_.Init(1.0f / static_cast<float>(kHillClimbingSampleWindow));
        cpu_filter_.Init(1.0f / static_cast<float>(kHillClimbingSampleWindow));
    }
    throughput_filter_.Feed(static_cast<float>(completed_count));

    // ── Measure CPU utilization ───────────────────────────────────
    {
        auto now_wall = std::chrono::steady_clock::now();
        uint64_t now_wall_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
            now_wall.time_since_epoch()).count();

        uint64_t now_cpu_ns = GetProcessCpuTimeNs();

        if (last_wall_time_ns_ > 0 && last_process_cpu_ns_ > 0 && now_cpu_ns > 0) {
            uint64_t wall_delta = now_wall_ns - last_wall_time_ns_;
            uint64_t cpu_delta = now_cpu_ns - last_process_cpu_ns_;
            if (wall_delta > 0) {
                float measured = static_cast<float>(
                    (std::min)(cpu_delta, wall_delta)) / static_cast<float>(wall_delta);
                cpu_utilization_ = cpu_utilization_ * 0.7f + measured * 0.3f;  // EMA
            }
        } else {
            // Platform does not support process CPU time measurement;
            // fall back to throughput heuristic.
            float busy_ratio = (completed_count > 0)
                ? (std::min)(1.0f, static_cast<float>(completed_count) * 0.01f)
                : 0.0f;
            cpu_utilization_ = cpu_utilization_ * 0.7f + busy_ratio * 0.3f;  // EMA
        }
        cpu_filter_.Feed(cpu_utilization_);

        last_wall_time_ns_ = now_wall_ns;
        if (now_cpu_ns > 0) {
            last_process_cpu_ns_ = now_cpu_ns;
        }
    }

    float throughput_power = throughput_filter_.Power();
    float cpu_power = cpu_filter_.Power();
    float sigmoid_gain = SigmoidGain(current_threads);

    int32_t suggestion = current_threads;

    // ── State machine ─────────────────────────────────────────────
    switch (state_) {
    case HillClimbState::Warmup:
        // Ramp up quickly: +1/tick while there's work and we're below min+2.
        if (current_threads < kThreadPoolMinWorkerCount + 2 && completed_count > 0) {
            suggestion = current_threads + 1;
        } else if (avg_throughput > 0) {
            state_ = HillClimbState::ClimbExplore;
            wave_ticks_ = 0;
            wave_threads_ = 0;
            pre_wave_throughput_ = avg_throughput;
        }
        break;

    case HillClimbState::ClimbExplore: {
        // Inject up to 2 threads, measure throughput change after 2 ticks.
        if (wave_ticks_ == 0 && wave_threads_ == 0) {
            wave_threads_ = (std::min)(2, (std::max)(1, cpu_count_ / 4));
            pre_wave_throughput_ = avg_throughput;
        }

        if (wave_ticks_ < 2) {
            suggestion = (std::min)(current_threads + wave_threads_, kHillClimbingMaxWorker);
            wave_ticks_++;
        } else {
            int32_t gain = avg_throughput - pre_wave_throughput_;
            int32_t adj_gain = (wave_threads_ > 0) ? (gain / wave_threads_) : gain;
            float gp = throughput_power;  // Goertzel power tells us signal confidence

            if (adj_gain > 5 && gp > 0.5f) {
                state_ = HillClimbState::Climbing;
                suggestion = (std::min)(current_threads + (std::max)(1, cpu_count_ / 8),
                                        kHillClimbingMaxWorker);
            } else if (adj_gain < -5 || (gp < 0.3f && avg_throughput < pre_wave_throughput_)) {
                state_ = HillClimbState::Stabilizing;
                suggestion = (std::max)(current_threads - wave_threads_, kHillClimbingMinWorker);
            } else {
                // Goertzel power low or gain neutral → near peak → fix.
                state_ = HillClimbState::ClimbFix;
                suggestion = (std::max)(current_threads - wave_threads_ / 2, kThreadPoolMinWorkerCount);
            }
            wave_ticks_ = 0;
            wave_threads_ = 0;
        }
        break;
    }

    case HillClimbState::Climbing:
        if (avg_throughput > last_throughput_ && cpu_utilization_ < 0.9f) {
            // Still improving and CPU not saturated — keep climbing with sigmoid gain.
            int32_t increment = (std::max)(1, static_cast<int32_t>(sigmoid_gain * static_cast<float>(cpu_count_) / 4.0f));
            suggestion = (std::min)(current_threads + increment, kHillClimbingMaxWorker);
        } else if (avg_throughput > last_throughput_ && cpu_utilization_ >= 0.9f) {
            // Improving but CPU saturated — go to Saturating.
            state_ = HillClimbState::Saturating;
        } else {
            // Plateau or drop — fix and stabilize.
            state_ = HillClimbState::ClimbFix;
        }
        break;

    case HillClimbState::ClimbFix:
        // Hold for 2 ticks to let Goertzel filters settle, then go Steady.
        if (wave_ticks_ < 2) {
            wave_ticks_++;
        } else {
            wave_ticks_ = 0;
            state_ = HillClimbState::Steady;
            steady_hold_ticks_ = 0;
            steady_base_threads_ = current_threads;
        }
        break;

    case HillClimbState::Stabilizing:
        if ((avg_throughput < last_throughput_ || cpu_utilization_ >= 0.95f) &&
            current_threads > kHillClimbingMinWorker) {
            // Still dropping or CPU saturated — pull back more.
            suggestion = (std::max)(current_threads - 1, kHillClimbingMinWorker);
        } else {
            // Recovered — go steady.
            state_ = HillClimbState::Steady;
            steady_hold_ticks_ = 0;
            steady_base_threads_ = current_threads;
        }
        break;

    case HillClimbState::Steady: {
        steady_hold_ticks_++;
        square_wave_phase_ = 0;

        // If throughput dropped significantly, re-probe.
        if (avg_throughput < last_throughput_ / 2 && completed_count > 0) {
            state_ = HillClimbState::ClimbExplore;
            wave_ticks_ = 0;
            wave_threads_ = 0;
            pre_wave_throughput_ = avg_throughput;
            break;
        }

        // Idle shrink.
        if (completed_count == 0 && current_threads > kHillClimbingMinWorker) {
            suggestion = (std::max)(current_threads - 1, kHillClimbingMinWorker);
            break;
        }

        // Square-wave injection: every 4th tick, probe with +1 or -1 thread
        // to test whether system still has headroom.
        if (steady_hold_ticks_ >= 4 && sample_count_ >= kHillClimbingSampleWindow) {
            steady_hold_ticks_ = 0;

            // Check Goertzel power: high power = signal clearly present = system
            // still responding to changes. Proceed with square wave.
            if (throughput_power > 0.3f && cpu_utilization_ < 0.85f) {
                state_ = HillClimbState::SteadyFix;
                square_wave_phase_ = 1;  // +1 probe first
                suggestion = (std::min)(current_threads + 1, kHillClimbingMaxWorker);
                steady_base_threads_ = current_threads;
                pre_wave_throughput_ = avg_throughput;
                wave_ticks_ = 0;
            } else if (cpu_utilization_ >= 0.85f) {
                // CPU high — consider reducing.
                state_ = HillClimbState::Saturating;
            }
        }
        break;
    }

    case HillClimbState::SteadyFix: {
        // Square-wave injection active. After 2 ticks, compare throughput.
        wave_ticks_++;
        if (wave_ticks_ >= 2) {
            int32_t gain = avg_throughput - pre_wave_throughput_;

            if (square_wave_phase_ == 1) {
                // +1 probe done. If gain positive, keep the extra thread.
                // If negative, try -1 probe.
                if (gain > 0) {
                    suggestion = current_threads;  // keep the +1
                    steady_base_threads_ = current_threads;
                } else {
                    suggestion = (std::max)(steady_base_threads_ - 1, kHillClimbingMinWorker);
                    square_wave_phase_ = 2;  // -1 probe next
                    pre_wave_throughput_ = avg_throughput;
                    wave_ticks_ = 0;
                    break;
                }
            } else if (square_wave_phase_ == 2) {
                // -1 probe done. If throughput held up, keep the reduction.
                if (gain >= 0) {
                    suggestion = current_threads;  // keep the -1
                    steady_base_threads_ = current_threads;
                } else {
                    // Throughput dropped with -1 → restore.
                    suggestion = steady_base_threads_;
                }
            }

            // Return to Steady.
            state_ = HillClimbState::Steady;
            steady_hold_ticks_ = 0;
            square_wave_phase_ = 0;
        }
        break;
    }

    case HillClimbState::Saturating:
        // CPU saturated or near-saturated. Conservative: reduce by 1 if CPU > 90%.
        if (cpu_utilization_ >= 0.9f && current_threads > kHillClimbingMinWorker) {
            suggestion = (std::max)(current_threads - 1, kHillClimbingMinWorker);
        } else if (cpu_utilization_ < 0.7f && avg_throughput > 0) {
            // CPU freed up — back to steady for re-evaluation.
            state_ = HillClimbState::Steady;
            steady_hold_ticks_ = 0;
            steady_base_threads_ = current_threads;
        } else if (completed_count == 0 && current_threads > kHillClimbingMinWorker) {
            suggestion = (std::max)(current_threads - 1, kHillClimbingMinWorker);
        }
        break;

    case HillClimbState::Random:
        // Noise-dominated: random perturbation ±1 to escape local minima.
        if (avg_throughput > 0 && current_threads < kHillClimbingMaxWorker) {
            // Simple pseudo-random perturbation based on tick count.
            suggestion = (sample_index_ & 1)
                ? (std::min)(current_threads + 1, kHillClimbingMaxWorker)
                : (std::max)(current_threads - 1, kHillClimbingMinWorker);
        }
        // After 4 ticks in Random, re-enter ClimbExplore for fresh probe.
        if (wave_ticks_ < 4) {
            wave_ticks_++;
        } else {
            wave_ticks_ = 0;
            state_ = HillClimbState::ClimbExplore;
            pre_wave_throughput_ = avg_throughput;
        }
        break;
    }

    last_throughput_ = avg_throughput;
    last_thread_count_ = current_threads;
    return suggestion;
}

float HillClimbingController::SigmoidGain(int32_t thread_count) const noexcept {
    // Sigmoid: 1/(1+exp((threads-cpu)/2))
    // At thread_count == cpu_count:   gain = 0.5
    // At thread_count == cpu_count+4: gain ≈ 0.12
    // At thread_count == cpu_count-4: gain ≈ 0.88
    float x = static_cast<float>(thread_count - cpu_count_) / 2.0f;
    if (x > 0.0f) {
        // For positive x: 1/(1+exp(x)) — avoid exp overflow for large x.
        if (x > 10.0f) return 0.0f;
        return 1.0f / (1.0f + expf(x));
    } else {
        // For negative x: exp(-x)/(exp(-x)+1)
        if (x < -10.0f) return 1.0f;
        float ex = expf(-x);
        return ex / (1.0f + ex);
    }
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

    // V2 reset
    steady_hold_ticks_ = 0;
    steady_base_threads_ = 0;
    square_wave_phase_ = 0;
    last_wall_time_ns_ = 0;
    last_process_cpu_ns_ = 0;
    cpu_utilization_ = 0.0f;
    throughput_filter_.Reset();
    cpu_filter_.Reset();
    cpu_count_ = static_cast<int32_t>(std::thread::hardware_concurrency());
    if (cpu_count_ < 1) cpu_count_ = 1;
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

    s_wakeable_queue = PalWakeableCreate();
    if (s_wakeable_queue != nullptr) {
        s_wakeable_thread = std::thread(WakeableWorkerLoop);
    }

    s_gate_thread = std::thread(GateThreadLoop);

    // Initialize ETW event provider for diagnostics.
    ThreadPoolEventProviderInitialize();
}

void ThreadPoolShutdown() noexcept {
    ThreadPoolEventProviderShutdown();

    s_shutdown.store(true, std::memory_order_release);
    s_work_available.notify_all();

    if (s_wakeable_queue != nullptr) {
        PalWakeablePost(s_wakeable_queue, nullptr, nullptr);
    }
    if (s_wakeable_thread.joinable()) s_wakeable_thread.join();
    if (s_wakeable_queue != nullptr) {
        PalWakeableDestroy(s_wakeable_queue);
        s_wakeable_queue = nullptr;
    }

    {
        ForbidSuspendScope forbid;
        for (auto& t : s_workers) {
            if (t.joinable()) t.join();
        }
        s_workers.clear();

        // Clean up worker queues.
        for (auto* q : s_worker_queues) DestroyWorkerQueue(q);
        s_worker_queues.clear();
    }

    if (s_gate_thread.joinable()) s_gate_thread.join();

    {
        ForbidSuspendScope forbid;
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

    // Backpressure: if the global queue is full, yield-spin to let workers drain.
    // We use yield (not blocking wait) to avoid holding any lock, which prevents
    // deadlock when the consumer (worker) needs the same lock to pop items.
    int spin_count = 0;
    while (s_queue_depth.load(std::memory_order_relaxed) >= kThreadPoolMaxQueueDepth) {
        std::this_thread::yield();
        spin_count++;
        if (spin_count > 1000) {
            CHAOS_IL2CPP_LOG_WARN("ThreadPool", "queue full, spinning");
            spin_count = 0;
        }
    }

    {
        std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_mutex);
        s_global_queue.push({callback, context});
        s_queue_depth.fetch_add(1, std::memory_order_relaxed);
    }

    // ETW: work item queued.
    ThreadPoolEventEmitWorkItemQueue(reinterpret_cast<int64_t>(context));

    s_work_available.notify_one();

    // Rate-limited worker growth: only add workers in extreme starvation
    // (queue depth > 3× workers). Normal growth is managed by the gate tick
    // (every 15 ms) via HillClimbing, which makes optimal decisions based on
    // throughput, CPU utilization, and frequency-domain analysis.
    int32_t depth = s_queue_depth.load(std::memory_order_relaxed);
    int32_t current_workers = static_cast<int32_t>(s_workers.size());
    if (depth > current_workers * 3 && current_workers < kThreadPoolMaxWorkerCount) {
        s_desired_workers.store(
            (std::min)(current_workers + 1, kThreadPoolMaxWorkerCount),
            std::memory_order_relaxed);
        EnsureWorkerCount(s_desired_workers.load(std::memory_order_relaxed));
    }
}

void ThreadPoolGateTick() noexcept {
    int32_t completed = s_completed_since_tick.exchange(0, std::memory_order_relaxed);
    completed += s_wakeable_completions.exchange(0, std::memory_order_relaxed);

    int32_t current = static_cast<int32_t>(s_workers.size());
    int32_t target;

    // Starving detection: if queue depth is more than 2× active workers,
    // bypass hill-climbing analysis and force-add a thread.
    int32_t depth = s_queue_depth.load(std::memory_order_relaxed);
    if (depth > current * 2 && current < kThreadPoolMaxWorkerCount) {
        target = current + 1;
    } else {
        target = s_hill_climbing.OnGateTick(completed, current);
    }

    target = (std::max)(target, kThreadPoolMinWorkerCount);
    target = (std::min)(target, kThreadPoolMaxWorkerCount);

    s_desired_workers.store(target, std::memory_order_relaxed);

    // ETW: emit worker adjust event if target changed.
    if (target != current) {
        // Map hill-climbing state to reason code.
        int32_t reason = 3;  // Default: Steady.
        switch (s_hill_climbing.GetState()) {
            case HillClimbState::Warmup:        reason = 0; break;
            case HillClimbState::ClimbExplore:  reason = 1; break;
            case HillClimbState::Climbing:      reason = 2; break;
            case HillClimbState::ClimbFix:      reason = 3; break;
            case HillClimbState::Stabilizing:   reason = 4; break;
            case HillClimbState::Steady:        reason = 5; break;
            case HillClimbState::SteadyFix:     reason = 6; break;
            case HillClimbState::Saturating:    reason = 7; break;
            case HillClimbState::Random:        reason = 8; break;
        }
        ThreadPoolEventEmitWorkerAdjust(current, target, reason);
    }

    EnsureWorkerCount(target);
}

int32_t ThreadPoolWorkerCount() noexcept {
    return static_cast<int32_t>(s_workers.size());
}

}  // namespace chaos::il2cpp::runtime_core::threading
