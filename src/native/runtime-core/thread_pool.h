#ifndef CHAOS_IL2CPP_THREAD_POOL_H_
#define CHAOS_IL2CPP_THREAD_POOL_H_

#include <cstdint>

namespace chaos::il2cpp::runtime_core::threading {

/// Initialize the thread pool (start gate thread, prepare queue).
void ThreadPoolInitialize() noexcept;

/// Shutdown the thread pool (drain queue, join workers, stop gate).
void ThreadPoolShutdown() noexcept;

/// Enqueue a work item for execution on a thread-pool worker.
void ThreadPoolQueueUserWorkItem(void (*callback)(void*), void* context) noexcept;

/// Called by the gate thread at periodic intervals to manage delayed work
/// items and adjust worker thread count.
void ThreadPoolGateTick() noexcept;

/// Number of currently active worker threads.
int32_t ThreadPoolWorkerCount() noexcept;

/// Minimum/maximum worker thread count bounds.
constexpr int32_t kThreadPoolMinWorkerCount = 1;
constexpr int32_t kThreadPoolMaxWorkerCount = 32;

}  // namespace chaos::il2cpp::runtime_core::threading

#endif  // CHAOS_IL2CPP_THREAD_POOL_H_
