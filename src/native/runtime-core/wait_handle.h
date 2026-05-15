#ifndef CHAOS_IL2CPP_WAIT_HANDLE_H_
#define CHAOS_IL2CPP_WAIT_HANDLE_H_

#include <cstdint>

namespace chaos::il2cpp::runtime_core::threading {

/// Maximum number of wait handles (supports 200+ DLL scenarios).
constexpr uint32_t kMaxWaitHandles = 2048;

/// Invalid handle ID.
constexpr uint32_t kInvalidWaitHandle = 0u;

/// Wait handle types.
enum class WaitHandleType : uint8_t {
    ManualResetEvent = 0,
    AutoResetEvent   = 1,
};

/// Create a wait handle (ManualResetEvent or AutoResetEvent).
/// @param initial_state  true = signalled initially.
/// @param type           ManualResetEvent or AutoResetEvent.
/// @return Handle ID (kInvalidWaitHandle on failure).
uint32_t WaitHandleCreate(bool initial_state, WaitHandleType type) noexcept;

/// Close/destroy a wait handle.
/// @param handle_id  Handle ID from WaitHandleCreate.
/// @return true if the handle was found and closed.
bool WaitHandleClose(uint32_t handle_id) noexcept;

/// Set a wait handle to the signalled state.
/// For AutoResetEvent, wakes one waiting thread.
/// For ManualResetEvent, wakes all waiting threads.
/// @param handle_id  Handle ID from WaitHandleCreate.
/// @return true if the handle was found.
bool WaitHandleSet(uint32_t handle_id) noexcept;

/// Reset a wait handle to the non-signalled state.
/// @param handle_id  Handle ID from WaitHandleCreate.
/// @return true if the handle was found.
bool WaitHandleReset(uint32_t handle_id) noexcept;

/// Wait for a single handle to become signalled.
/// @param handle_id   Handle ID from WaitHandleCreate.
/// @param timeout_ms  Timeout in milliseconds (-1 = infinite, 0 = poll).
/// @return 0 = timeout, 1 = signalled, -1 = error.
int32_t WaitHandleWaitOne(uint32_t handle_id, int32_t timeout_ms) noexcept;

/// Wait for any of the given handles to become signalled.
/// @param handle_ids  Array of handle IDs.
/// @param count       Number of handles.
/// @param timeout_ms  Timeout in milliseconds (-1 = infinite).
/// @return Index of the signalled handle, or -1 on timeout/error.
int32_t WaitHandleWaitAny(const uint32_t* handle_ids, uint32_t count, int32_t timeout_ms) noexcept;

/// Wait for all given handles to become signalled.
/// @param handle_ids  Array of handle IDs.
/// @param count       Number of handles.
/// @param timeout_ms  Timeout in milliseconds (-1 = infinite).
/// @return 0 = all signalled, -1 = timeout/error.
int32_t WaitHandleWaitAll(const uint32_t* handle_ids, uint32_t count, int32_t timeout_ms) noexcept;

}  // namespace chaos::il2cpp::runtime_core::threading

#endif  // CHAOS_IL2CPP_WAIT_HANDLE_H_