// pal_wakeable.h — Wakeable queue for thread pool signaling
#pragma once

#include <cstdint>

namespace chaos::il2cpp::pal {

/// Opaque handle for a wakeable wait queue.
/// On Windows: backed by an IOCP (I/O Completion Port).
/// On Linux: backed by epoll + eventfd.
/// On Apple/Android: not supported (returns nullptr from Create).
struct PalWakeable;

/// Create a wakeable queue.
/// On Windows: creates an IOCP via CreateIoCompletionPort.
/// On Linux: creates epoll fd + eventfd for signaling.
/// On Apple/Android: returns nullptr (unsupported).
/// Returns nullptr on failure.
PalWakeable* PalWakeableCreate() noexcept;

/// Post a wakeup to the queue. The callback+context will be dispatched
/// to the waiting thread.
/// @returns true on success.
bool PalWakeablePost(PalWakeable* w, void (*callback)(void*), void* context) noexcept;

/// Wait for a wakeup. Dispatches any queued callbacks.
/// @returns The number of callbacks dispatched, 0 on timeout, -1 on error.
int PalWakeableWait(PalWakeable* w, int timeout_ms) noexcept;

/// Destroy the queue and release all resources.
void PalWakeableDestroy(PalWakeable* w) noexcept;

}  // namespace chaos::il2cpp::pal
