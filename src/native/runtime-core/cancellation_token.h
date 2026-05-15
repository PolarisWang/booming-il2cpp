#ifndef CHAOS_IL2CPP_CANCELLATION_TOKEN_H_
#define CHAOS_IL2CPP_CANCELLATION_TOKEN_H_

#include <chaos/native_types.h>

#include <atomic>
#include <cstdint>

namespace chaos::il2cpp::runtime_core::threading {

// ── CancellationTokenSource ───────────────────────────────────────────
// Manages a cancellation signal with optional timer-based cancellation.
struct CancellationTokenSource {
    /// Unique ID for this source.
    uint32_t                    id{0};
    /// True if cancellation has been requested.
    std::atomic<bool>           cancelled{false};
    /// Timer ID for timer-based cancellation (0 = no timer).
    uint32_t                    timer_id{0};
    /// True if this source is still active (not disposed).
    std::atomic<bool>           active{true};
};

// ── CancellationTokenRegistration ─────────────────────────────────────
// A registered callback that fires when cancellation is requested.
struct CancellationTokenRegistration {
    uint32_t                    id{0};
    uint32_t                    source_id{0};
    void                        (*callback)(void*){nullptr};
    void*                       state{nullptr};
    bool                        active{true};
};

// ── Public API ────────────────────────────────────────────────────────

/// Create a new CancellationTokenSource.
/// @return Source ID (0 on failure).
uint32_t CancellationTokenSourceCreate() noexcept;

/// Create a CancellationTokenSource with a timer-based cancellation.
/// @param due_time_ms  Milliseconds until cancellation is requested.
/// @return Source ID (0 on failure).
uint32_t CancellationTokenSourceCreateWithTimer(uint32_t due_time_ms) noexcept;

/// Cancel a CancellationTokenSource (fire all registered callbacks).
/// @param source_id  ID from CancellationTokenSourceCreate.
/// @return true if cancellation was requested (first time only).
bool CancellationTokenSourceCancel(uint32_t source_id) noexcept;

/// Check if a CancellationTokenSource has been cancelled.
/// @param source_id  ID from CancellationTokenSourceCreate.
/// @return true if cancelled.
bool CancellationTokenSourceIsCancelled(uint32_t source_id) noexcept;

/// Dispose a CancellationTokenSource (cancel timer, prevent further use).
/// @param source_id  ID from CancellationTokenSourceCreate.
void CancellationTokenSourceDispose(uint32_t source_id) noexcept;

/// Register a callback to fire when cancellation is requested.
/// @param source_id   CancellationTokenSource ID.
/// @param callback    Function to call on cancellation.
/// @param state       User context passed to callback.
/// @return Registration ID (0 on failure).
uint32_t CancellationTokenRegister(uint32_t source_id, void (*callback)(void*), void* state) noexcept;

/// Unregister a previously registered callback.
/// @param registration_id  ID from CancellationTokenRegister.
/// @return true if found and unregistered.
bool CancellationTokenUnregister(uint32_t registration_id) noexcept;

}  // namespace chaos::il2cpp::runtime_core::threading

#endif  // CHAOS_IL2CPP_CANCELLATION_TOKEN_H_