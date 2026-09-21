// pal_sync.h — Synchronization primitives (mutex, condvar, rwlock, event)
#pragma once

#include <cstddef>
#include <cstdint>

namespace chaos::il2cpp::pal {

/// Opaque handle for PAL synchronization objects.
/// Implemented as CRITICAL_SECTION* (Win32) or pthread_mutex_t* (POSIX).
struct PalMutex;
struct PalCondVar;
struct PalRwLock;
struct PalEvent;

// ── Mutex ─────────────────────────────────────────────────────────────

PalMutex* PalMutexCreate() noexcept;
void PalMutexDestroy(PalMutex* m) noexcept;
void PalMutexLock(PalMutex* m) noexcept;
bool PalMutexTryLock(PalMutex* m) noexcept;
void PalMutexUnlock(PalMutex* m) noexcept;

// ── Condition variable ───────────────────────────────────────────────

PalCondVar* PalCondVarCreate() noexcept;
void PalCondVarDestroy(PalCondVar* cv) noexcept;
void PalCondVarWait(PalCondVar* cv, PalMutex* m) noexcept;
bool PalCondVarTimedWait(PalCondVar* cv, PalMutex* m, uint64_t timeout_ms) noexcept;
void PalCondVarSignal(PalCondVar* cv) noexcept;
void PalCondVarBroadcast(PalCondVar* cv) noexcept;

// ── Read-write lock ──────────────────────────────────────────────────

PalRwLock* PalRwLockCreate() noexcept;
void PalRwLockDestroy(PalRwLock* rwl) noexcept;
void PalRwLockReadLock(PalRwLock* rwl) noexcept;
void PalRwLockWriteLock(PalRwLock* rwl) noexcept;
void PalRwLockUnlock(PalRwLock* rwl) noexcept;

// ── Event (manual-reset or auto-reset) ─────────────────────────────

/// Create an event object.
/// @param initially_set  true = signalled initially.
/// @param manual_reset   true = manual-reset (stays signalled until Reset),
///                       false = auto-reset (consumed by one waiter).
PalEvent* PalEventCreate(bool initially_set, bool manual_reset) noexcept;
void PalEventDestroy(PalEvent* e) noexcept;
void PalEventSet(PalEvent* e) noexcept;
void PalEventReset(PalEvent* e) noexcept;
bool PalEventWait(PalEvent* e, uint64_t timeout_ms) noexcept;

/// Wait for any of the given events to become signalled.
/// @param events      Array of event pointers.
/// @param count       Number of events (must be > 0).
/// @param timeout_ms  Timeout in milliseconds (UINT64_MAX = infinite).
/// @return Index of the signalled event, or -1 on timeout/error.
int32_t PalEventWaitAny(PalEvent* const* events, size_t count, uint64_t timeout_ms) noexcept;

/// Wait for all of the given events to become signalled.
/// @param events      Array of event pointers.
/// @param count       Number of events (must be > 0).
/// @param timeout_ms  Timeout in milliseconds (UINT64_MAX = infinite).
/// @return 0 on success (all signalled), -1 on timeout/error.
int32_t PalEventWaitAll(PalEvent* const* events, size_t count, uint64_t timeout_ms) noexcept;

}  // namespace chaos::il2cpp::pal
