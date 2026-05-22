#ifndef CHAOS_IL2CPP_SEMAPHORE_SLIM_H_
#define CHAOS_IL2CPP_SEMAPHORE_SLIM_H_

#include <cstdint>

namespace chaos::il2cpp::runtime_core::threading {

/// Create a semaphore with the given initial count and max count.
/// @param initial_count  Initial count (must be >= 0).
/// @param max_count      Maximum count (must be > 0).
/// @return Semaphore handle (0 on failure).
uint32_t SemaphoreSlimCreate(int32_t initial_count, int32_t max_count) noexcept;

/// Destroy a semaphore.
/// @param sem_id  Handle from SemaphoreSlimCreate.
/// @return true if found and destroyed.
bool SemaphoreSlimDestroy(uint32_t sem_id) noexcept;

/// Wait on a semaphore (decrement count, block if zero).
/// @param sem_id     Handle from SemaphoreSlimCreate.
/// @param timeout_ms -1 = infinite, 0 = poll, >0 = bounded wait.
/// @return 1 = acquired, 0 = timeout, -1 = error.
int32_t SemaphoreSlimWait(uint32_t sem_id, int32_t timeout_ms) noexcept;

/// Release the semaphore (increment count, wake waiters).
/// @param sem_id  Handle from SemaphoreSlimCreate.
/// @param count   Number to release (typically 1).
/// @return 0 = success, -1 = error (would exceed max count).
int32_t SemaphoreSlimRelease(uint32_t sem_id, int32_t count) noexcept;

/// Reader-writer lock handle.
uint32_t ReaderWriterLockSlimCreate() noexcept;

/// Destroy a reader-writer lock.
/// @param rw_id  Handle from ReaderWriterLockSlimCreate.
/// @return true if found and destroyed.
bool ReaderWriterLockSlimDestroy(uint32_t rw_id) noexcept;

/// Enter read mode (shared). Multiple readers allowed.
/// @param rw_id      Handle from ReaderWriterLockSlimCreate.
/// @param timeout_ms -1 = infinite, 0 = poll, >0 = bounded.
/// @return 1 = acquired, 0 = timeout.
int32_t ReaderWriterLockSlimEnterRead(uint32_t rw_id, int32_t timeout_ms) noexcept;

/// Exit read mode.
/// @param rw_id  Handle from ReaderWriterLockSlimCreate.
/// @return true if successful.
bool ReaderWriterLockSlimExitRead(uint32_t rw_id) noexcept;

/// Enter write mode (exclusive).
/// @param rw_id      Handle from ReaderWriterLockSlimCreate.
/// @param timeout_ms -1 = infinite, 0 = poll, >0 = bounded.
/// @return 1 = acquired, 0 = timeout.
int32_t ReaderWriterLockSlimEnterWrite(uint32_t rw_id, int32_t timeout_ms) noexcept;

/// Exit write mode.
/// @param rw_id  Handle from ReaderWriterLockSlimCreate.
/// @return true if successful.
bool ReaderWriterLockSlimExitWrite(uint32_t rw_id) noexcept;

/// Enter upgradeable read mode (shared with readers, exclusive with other
/// upgradeable readers and writers). At most one thread can hold the
/// upgradeable read lock at a time. Regular readers can still enter.
/// @param rw_id      Handle from ReaderWriterLockSlimCreate.
/// @param timeout_ms -1 = infinite, 0 = poll, >0 = bounded.
/// @return 1 = acquired, 0 = timeout, -1 = error.
int32_t ReaderWriterLockSlimEnterUpgradeableRead(uint32_t rw_id, int32_t timeout_ms) noexcept;

/// Exit upgradeable read mode.
/// @param rw_id  Handle from ReaderWriterLockSlimCreate.
/// @return true if successful.
bool ReaderWriterLockSlimExitUpgradeableRead(uint32_t rw_id) noexcept;

/// Upgrade from upgradeable-read to write mode. Caller must already hold
/// the upgradeable read lock. Blocks until all readers drain.
/// @param rw_id      Handle from ReaderWriterLockSlimCreate.
/// @param timeout_ms -1 = infinite, 0 = poll, >0 = bounded.
/// @return 1 = acquired write, 0 = timeout, -1 = error (not upgradeable reader).
int32_t ReaderWriterLockSlimUpgradeToWrite(uint32_t rw_id, int32_t timeout_ms) noexcept;

/// Downgrade from write back to upgradeable-read mode. Caller must hold
/// both the write lock AND the upgradeable read lock (i.e. must have called
/// UpgradeToWrite from upgradeable-read state first). After return, the
/// write lock is released but the upgradeable read lock remains held.
/// @param rw_id  Handle from ReaderWriterLockSlimCreate.
/// @return true if successful.
bool ReaderWriterLockSlimDowngradeFromWrite(uint32_t rw_id) noexcept;

// ── Barrier ────────────────────────────────────────────────────────────

/// Create a Barrier that synchronizes `participant_count` threads.
/// @param participant_count  Number of participants (> 1).
/// @return Barrier handle (0 on failure).
uint32_t BarrierCreate(int32_t participant_count) noexcept;

/// Destroy a Barrier.
/// @param barrier_id  Handle from BarrierCreate.
/// @return true if found and destroyed.
bool BarrierDestroy(uint32_t barrier_id) noexcept;

/// Signal arrival at the barrier and wait for all participants.
/// When the last participant arrives, all are released and the phase
/// number advances.
/// @param barrier_id  Handle from BarrierCreate.
/// @param timeout_ms  -1 = infinite, 0 = poll, >0 = bounded wait in ms.
/// @return 1 = phase completed (all participants arrived),
///         0 = timeout,
///         -1 = error (barrier not found).
int32_t BarrierSignalAndWait(uint32_t barrier_id, int32_t timeout_ms) noexcept;

/// Get the number of participants still unaccounted for in the current phase.
/// @param barrier_id  Handle from BarrierCreate.
/// @return Remaining count (0 if all arrived), -1 on error.
int32_t BarrierGetRemainingParticipants(uint32_t barrier_id) noexcept;

/// Get the current phase number (0-based, incremented after each complete phase).
/// @param barrier_id  Handle from BarrierCreate.
/// @return Current phase number, -1 on error.
int64_t BarrierGetCurrentPhaseNumber(uint32_t barrier_id) noexcept;

// ── CountdownEvent ─────────────────────────────────────────────────────

/// Create a CountdownEvent with the given initial count.
/// @param initial_count  Initial count (> 0).
/// @return CountdownEvent handle (0 on failure).
uint32_t CountdownEventCreate(int32_t initial_count) noexcept;

/// Destroy a CountdownEvent.
/// @param ce_id  Handle from CountdownEventCreate.
/// @return true if found and destroyed.
bool CountdownEventDestroy(uint32_t ce_id) noexcept;

/// Signal (decrement) the count by the specified amount.
/// @param ce_id  Handle from CountdownEventCreate.
/// @param count  Amount to decrement (must be > 0).
/// @return 1 = count reached zero (event is now signaled),
///         0 = count not yet zero,
///         -1 = error (not found or invalid count).
int32_t CountdownEventSignal(uint32_t ce_id, int32_t count) noexcept;

/// Wait until the count reaches zero.
/// @param ce_id      Handle from CountdownEventCreate.
/// @param timeout_ms -1 = infinite, 0 = poll, >0 = bounded wait in ms.
/// @return 1 = signaled (count reached zero), 0 = timeout, -1 = error.
int32_t CountdownEventWait(uint32_t ce_id, int32_t timeout_ms) noexcept;

/// Get the current remaining count.
/// @param ce_id  Handle from CountdownEventCreate.
/// @return Current count, -1 on error.
int32_t CountdownEventGetCurrentCount(uint32_t ce_id) noexcept;

/// Reset the count to a new value (re-arms the event).
/// @param ce_id  Handle from CountdownEventCreate.
/// @param count  New count (must be > 0).
/// @return true if successful.
bool CountdownEventReset(uint32_t ce_id, int32_t count) noexcept;

}  // namespace chaos::il2cpp::runtime_core::threading

#endif  // CHAOS_IL2CPP_SEMAPHORE_SLIM_H_