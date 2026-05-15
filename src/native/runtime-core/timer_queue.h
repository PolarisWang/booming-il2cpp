#ifndef CHAOS_IL2CPP_TIMER_QUEUE_H_
#define CHAOS_IL2CPP_TIMER_QUEUE_H_

#include <cstdint>

namespace chaos::il2cpp::runtime_core::threading {

/// Maximum number of concurrent timer entries.
/// Supports 200+ DLL registrations with headroom.
constexpr uint32_t kTimerQueueMaxEntries = 1024;

/// Invalid timer ID (returned on failure).
constexpr uint32_t kTimerQueueInvalidId = 0u;

/// Initialize the timer queue.
void TimerQueueInitialize() noexcept;

/// Shutdown the timer queue (cancel all pending timers).
void TimerQueueShutdown() noexcept;

/// Create a one-shot or periodic timer.
/// @param callback    Function to invoke when the timer fires.
/// @param state       User context passed to callback.
/// @param due_time_ms Milliseconds until the first fire (0 = immediate).
/// @param period_ms   Milliseconds between subsequent fires (0 = one-shot).
/// @return Timer ID (kTimerQueueInvalidId on failure).
uint32_t TimerQueueCreate(
    void (*callback)(void*),
    void* state,
    uint32_t due_time_ms,
    uint32_t period_ms) noexcept;

/// Change an existing timer's due time and period.
/// @param timer_id    Timer ID from TimerQueueCreate.
/// @param due_time_ms New due time in milliseconds.
/// @param period_ms   New period in milliseconds (0 = one-shot).
/// @return true if the timer was found and updated.
bool TimerQueueChange(uint32_t timer_id, uint32_t due_time_ms, uint32_t period_ms) noexcept;

/// Delete a timer.  The timer will not fire again.
/// @param timer_id Timer ID from TimerQueueCreate.
/// @return true if the timer was found and deleted.
bool TimerQueueDelete(uint32_t timer_id) noexcept;

/// Called by the gate thread on each tick (~500ms).
/// Fires due timers and reschedules periodic ones.
void TimerQueueOnTick() noexcept;

}  // namespace chaos::il2cpp::runtime_core::threading

#endif  // CHAOS_IL2CPP_TIMER_QUEUE_H_