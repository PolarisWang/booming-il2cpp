// ── ThreadPool ETW event provider declarations ─────────────────────────
#pragma once

#include <cstdint>

namespace chaos::il2cpp::runtime_core::threading {

/// Initialize the ThreadPool event provider.
/// Registers the ETW provider with the Windows tracing system.
/// Safe to call multiple times (idempotent after first call).
void ThreadPoolEventProviderInitialize() noexcept;

/// Shutdown the ThreadPool event provider.
/// Unregisters the ETW provider.
void ThreadPoolEventProviderShutdown() noexcept;

/// Emit a worker thread creation event.
/// @param thread_id  The managed thread ID of the new worker.
void ThreadPoolEventEmitWorkerCreate(int32_t thread_id) noexcept;

/// Emit a worker thread destruction event.
/// @param thread_id  The managed thread ID of the exiting worker.
void ThreadPoolEventEmitWorkerDestroy(int32_t thread_id) noexcept;

/// Emit a work item being queued to the global pool.
/// @param work_item_id  Opaque identifier (context pointer as int64).
void ThreadPoolEventEmitWorkItemQueue(int64_t work_item_id) noexcept;

/// Emit a work item being dequeued from the global pool.
/// @param work_item_id  Opaque identifier matching the queue event.
void ThreadPoolEventEmitWorkItemDequeue(int64_t work_item_id) noexcept;

/// Emit a worker count adjustment (hill-climbing decision).
/// @param current_count  Number of active workers before adjustment.
/// @param new_count      Target number of workers after adjustment.
/// @param reason         0=Warmup, 1=Climbing, 2=Stabilizing, 3=Steady,
///                       4=WorkloadSurge.
void ThreadPoolEventEmitWorkerAdjust(int32_t current_count, int32_t new_count, int32_t reason) noexcept;

/// Emit an I/O completion event (Windows IOCP only).
/// @param bytes_transferred  Number of bytes transferred.
void ThreadPoolEventEmitIOCompletion(uint32_t bytes_transferred) noexcept;

}  // namespace chaos::il2cpp::runtime_core::threading
