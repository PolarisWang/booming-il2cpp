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

}  // namespace chaos::il2cpp::runtime_core::threading

#endif  // CHAOS_IL2CPP_SEMAPHORE_SLIM_H_