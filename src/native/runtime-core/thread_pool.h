#ifndef CHAOS_IL2CPP_THREAD_POOL_H_
#define CHAOS_IL2CPP_THREAD_POOL_H_

#include <cstdint>
#include <atomic>

namespace chaos::il2cpp::runtime_core::threading {

// ── Hill-Climbing controller ─────────────────────────────────────────
// Implements the CoreCLR-style hill-climbing algorithm for tuning
// worker thread count based on throughput history.
//
// kSampleWindowSize = 8 (moving average over 8 gate ticks)
// kMaxWorkerCount   = 32767 (CoreCLR default)

constexpr int32_t kHillClimbingSampleWindow = 8;
constexpr int32_t kHillClimbingMaxWorker    = 32767;
constexpr int32_t kHillClimbingMinWorker    = 1;

struct HillClimbingController {
    /// Record completed work item count from the last gate tick.
    /// @param completed_count  Number of work items completed since last tick.
    /// @param current_threads  Current worker thread count.
    /// @return Suggested worker thread count (clamped to [kHillClimbingMinWorker, kHillClimbingMaxWorker]).
    int32_t OnGateTick(int32_t completed_count, int32_t current_threads) noexcept;

    /// Reset controller state (e.g., after pool drain).
    void Reset() noexcept;

private:
    /// Rolling sample buffer for throughput measurement.
    int32_t samples_[kHillClimbingSampleWindow]{};
    uint32_t sample_index_{0};
    uint32_t sample_count_{0};
    int32_t last_throughput_{0};
    int32_t last_thread_count_{kHillClimbingMinWorker};
};

/// Initialize the thread pool (start gate thread, prepare queue, IOCP).
void ThreadPoolInitialize() noexcept;

/// Shutdown the thread pool (drain queue, join workers, stop gate).
void ThreadPoolShutdown() noexcept;

/// Enqueue a work item for execution on a thread-pool worker.
void ThreadPoolQueueUserWorkItem(void (*callback)(void*), void* context) noexcept;

/// Called by the gate thread at periodic intervals (500ms) to manage
/// delayed work items and adjust worker thread count via Hill-Climbing.
void ThreadPoolGateTick() noexcept;

/// Number of currently active worker threads.
int32_t ThreadPoolWorkerCount() noexcept;

/// Minimum/maximum worker thread count bounds.
constexpr int32_t kThreadPoolMinWorkerCount = 1;
constexpr int32_t kThreadPoolMaxWorkerCount = 32767;  // CoreCLR default

#if defined(_WIN32) || defined(_WIN64)
/// Initialize the I/O completion port for the thread pool (Windows-only).
/// Called once during ThreadPoolInitialize.
void ThreadPoolInitializeIOCP() noexcept;

/// Number of outstanding I/O completion packets processed since last gate tick.
/// Reset each gate tick for Hill-Climbing input.
extern std::atomic<int32_t> g_iocp_completions;
#endif

}  // namespace chaos::il2cpp::runtime_core::threading

#endif  // CHAOS_IL2CPP_THREAD_POOL_H_