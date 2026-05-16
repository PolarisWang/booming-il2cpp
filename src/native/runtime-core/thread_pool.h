#ifndef CHAOS_IL2CPP_THREAD_POOL_H_
#define CHAOS_IL2CPP_THREAD_POOL_H_

#include <cstdint>
#include <atomic>
#include <chrono>
#include <deque>
#include <chaos/native_types.h>

namespace chaos::il2cpp::runtime_core::threading {

// ── Hill-Climbing controller ─────────────────────────────────────────
// Full CoreCLR-compatible state machine:
//   Warmup → ClimbExplore → Climbing → Stabilizing → Steady
//
// kSampleWindowSize = 8 (moving average over 8 gate ticks)
// kMaxWorkerCount   = 32767 (CoreCLR default)

constexpr int32_t kHillClimbingSampleWindow = 8;
constexpr int32_t kHillClimbingMaxWorker    = 32767;
constexpr int32_t kHillClimbingMinWorker    = 1;

enum class HillClimbState : uint8_t {
    Warmup,        // Initial ramp-up: +1/tick until min configured
    ClimbExplore,  // Probe throughput slope by injecting threads
    Climbing,      // Gain positive → keep adding threads
    Stabilizing,   // Gain negative → pull back 1 thread
    Steady         // At peak — monitor without changing
};

struct HillClimbingController {
    int32_t OnGateTick(int32_t completed_count, int32_t current_threads) noexcept;
    void Reset() noexcept;

private:
    int32_t samples_[kHillClimbingSampleWindow]{};
    uint32_t sample_index_{0};
    uint32_t sample_count_{0};

    int32_t last_throughput_{0};
    int32_t last_thread_count_{kHillClimbingMinWorker};
    HillClimbState state_{HillClimbState::Warmup};
    int32_t wave_threads_{0};       // Threads added in current wave
    int32_t wave_ticks_{0};         // Ticks since wave started
    int32_t pre_wave_throughput_{0}; // Throughput before wave injection
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

/// Idle timeout before a worker thread exits (30 seconds).
constexpr auto kThreadPoolIdleTimeout = std::chrono::seconds(30);

#if defined(_WIN32) || defined(_WIN64)
void ThreadPoolInitializeIOCP() noexcept;
extern std::atomic<int32_t> g_iocp_completions;
#endif

}  // namespace chaos::il2cpp::runtime_core::threading

#endif  // CHAOS_IL2CPP_THREAD_POOL_H_