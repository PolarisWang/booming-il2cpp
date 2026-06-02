#ifndef CHAOS_IL2CPP_THREAD_POOL_H_
#define CHAOS_IL2CPP_THREAD_POOL_H_

#include <cstdint>
#include <atomic>
#include <chrono>
#include <cmath>
#include <deque>
#include <chaos/native_types.h>

namespace chaos::il2cpp::runtime_core::threading {

// ── Hill-Climbing controller (V2 — 9-state + Goertzel + CPU feedback) ──
//
// V2 improvements over V1:
//   9 states (added ClimbFix, SteadyFix, Saturating, Random)
//   Dual Goertzel filters (throughput + CPU) for frequency-domain analysis
//   CPU utilization feedback per gate tick
//   Square-wave injection in Steady to probe system headroom
//   Non-linear gain (sigmoid) near CPU core count
//
// kSampleWindowSize = 8 (moving average over 8 gate ticks)
// kMaxWorkerCount   = 32767 (CoreCLR default)

constexpr int32_t kHillClimbingSampleWindow = 8;
constexpr int32_t kHillClimbingMaxWorker    = 32767;
constexpr int32_t kHillClimbingMinWorker    = 1;

/// 10-state hill-climbing state machine (V2 + Starving).
enum class HillClimbState : uint8_t {
    Warmup,        // Initial ramp-up: +1/tick until min configured
    ClimbExplore,  // Probe throughput slope by injecting threads
    Climbing,      // Gain positive → keep adding threads aggressively
    ClimbFix,      // Climbing overshoot detected → hold while Goertzel settles
    Stabilizing,   // Gain negative → pull back threads
    Steady,        // At peak — monitor without changing
    SteadyFix,     // Steady with square-wave injection active
    Saturating,    // Near CPU capacity — conservative gain only
    Random,        // Noise-dominated signal → random perturbation
    Starving,      // Queue depth exceeds 2× workers — forced growth
};

/// Single Goertzel filter for frequency-domain component detection.
/// Detects the energy at a specific frequency in the last N samples.
struct GoertzelFilter {
    float s1{0.0f};
    float s2{0.0f};
    float coeff{0.0f};
    uint32_t sample_count{0};

    /// Initialize for target_freq / sample_rate.
    /// For an 8-sample window with target_freq=1 (one cycle): coeff=2*cos(pi/4)=1.4142
    void Init(float normalized_freq) noexcept {
        coeff = 2.0f * cosf(2.0f * 3.14159265f * normalized_freq);
        s1 = 0.0f;
        s2 = 0.0f;
        sample_count = 0;
    }

    /// Feed one sample into the filter.
    void Feed(float sample) noexcept {
        float s0 = sample + coeff * s1 - s2;
        s2 = s1;
        s1 = s0;
        sample_count++;
    }

    /// Get the current power (energy) at the target frequency.
    /// Normalized by sample_count to allow comparison across windows.
    float Power() const noexcept {
        if (sample_count < 2) return 0.0f;
        return (s1 * s1 + s2 * s2 - coeff * s1 * s2) / static_cast<float>(sample_count);
    }

    /// Reset the filter state.
    void Reset() noexcept {
        s1 = 0.0f;
        s2 = 0.0f;
        sample_count = 0;
    }
};

struct HillClimbingController {
    int32_t OnGateTick(int32_t completed_count, int32_t current_threads) noexcept;
    void Reset() noexcept;

    /// Current hill-climbing state (public for ETW diagnostics).
    HillClimbState GetState() const noexcept { return state_; }

    /// Current CPU utilization (0.0–1.0, for diagnostics/testing).
    float GetCpuUtilization() const noexcept { return cpu_utilization_; }

private:
    // --- V1 legacy fields (kept for compatibility) ---
    int32_t samples_[kHillClimbingSampleWindow]{};
    uint32_t sample_index_{0};
    uint32_t sample_count_{0};

    int32_t last_throughput_{0};
    int32_t last_thread_count_{kHillClimbingMinWorker};
    HillClimbState state_{HillClimbState::Warmup};
    int32_t wave_threads_{0};        // Threads added in current wave
    int32_t wave_ticks_{0};          // Ticks since wave started
    int32_t pre_wave_throughput_{0}; // Throughput before wave injection

    // --- V2 new fields ---
    int32_t steady_hold_ticks_{0};         // Ticks in Steady state
    int32_t steady_base_threads_{0};       // Thread count before square-wave injection
    int32_t square_wave_phase_{0};         // 0=normal, 1=+1 probe, 2=-1 probe

    // CPU utilization tracking
    uint64_t last_wall_time_ns_{0};        // Wall time at last tick (ns)
    uint64_t last_process_cpu_ns_{0};      // Process CPU time at last tick (ns)
    float cpu_utilization_{0.0f};          // Last measured CPU util (0.0-1.0)

    // Goertzel filters
    GoertzelFilter throughput_filter_;      // Detects throughput oscillation
    GoertzelFilter cpu_filter_;             // Detects CPU saturation oscillation

    // CPU core count (cached)
    int32_t cpu_count_{1};

    /// Sigmoid gain: 1/(1+exp((threads-cpu)/2)), smoothly reduces gain near CPU count.
    float SigmoidGain(int32_t thread_count) const noexcept;
};

// ── Work-stealing queue ──────────────────────────────────────────────
// Each worker thread has one WorkerLocalQueue. Workers push/pop from
// their own queue (LIFO — cache friendly). When empty, they steal from
// a random victim's queue (from bottom, under the victim's mutex).

struct WorkItem {
    void (*callback)(void*) = nullptr;
    void* context           = nullptr;
};

struct alignas(64) WorkerLocalQueue {
    std::deque<WorkItem> deque;
    CHAOS_IL2CPP_MUTEX mutex;

    void PushFront(const WorkItem& item) noexcept {
        std::lock_guard<CHAOS_IL2CPP_MUTEX> guard_(mutex);
        deque.push_front(item);
    }

    bool PopFront(WorkItem& out) noexcept {
        std::lock_guard<CHAOS_IL2CPP_MUTEX> guard_(mutex);
        if (deque.empty()) return false;
        out = deque.front();
        deque.pop_front();
        return true;
    }

    bool StealFromBack(WorkItem& out) noexcept {
        std::lock_guard<CHAOS_IL2CPP_MUTEX> guard_(mutex);
        if (deque.empty()) return false;
        out = deque.back();
        deque.pop_back();
        return true;
    }

    bool IsEmpty() const noexcept {
        // Best-effort: caller must handle race.
        return deque.empty();
    }
};

// ── Thread pool API ──────────────────────────────────────────────────

/// Initialize the thread pool (start gate thread, prepare queue, IOCP).
void ThreadPoolInitialize() noexcept;

/// Shutdown the thread pool (drain queue, join workers, stop gate).
void ThreadPoolShutdown() noexcept;

/// Enqueue a work item with ExecutionContext capture (standard path).
void ThreadPoolQueueUserWorkItem(void (*callback)(void*), void* context) noexcept;

/// Enqueue a work item WITHOUT ExecutionContext capture (fire-and-forget).
void ThreadPoolQueueUserWorkItemUnsafe(void (*callback)(void*), void* context) noexcept;

/// Called by the gate thread at periodic intervals.
void ThreadPoolGateTick() noexcept;

/// Number of currently active worker threads.
int32_t ThreadPoolWorkerCount() noexcept;

constexpr int32_t kThreadPoolMinWorkerCount = 1;
constexpr int32_t kThreadPoolMaxWorkerCount = 32767;

/// Maximum depth of the global work-item queue before backpressure kicks in.
/// Prevents unbounded growth from producer threads outpacing workers.
constexpr int32_t kThreadPoolMaxQueueDepth = 8192;

/// Idle timeout before a worker thread exits (30 seconds).
constexpr auto kThreadPoolIdleTimeout = std::chrono::seconds(30);

}  // namespace chaos::il2cpp::runtime_core::threading

#endif  // CHAOS_IL2CPP_THREAD_POOL_H_