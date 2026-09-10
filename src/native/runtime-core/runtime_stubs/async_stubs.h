// ── Async stub declarations ─────────────────────────────────────
// SimpleForward runtime helpers for Task.Yield / YieldAwaitable.
// These stubs provide minimal implementations sufficient for test
// entry points that reference Task.Yield().
#pragma once

#include <chaos/native_types.h>

// ── Interop valuetype fixes (page8 missing typedefs) ──
typedef CHAOS_IL2CPP_INT32 chaos_valuetype_System_Private_CoreLib_Interop_BCrypt_NTSTATUS;
typedef CHAOS_IL2CPP_INT32 chaos_valuetype_System_Private_CoreLib_Interop_BOOL;
typedef CHAOS_IL2CPP_INT32 chaos_valuetype_System_Private_CoreLib_Interop_Globalization_ResultCode;

extern "C" {

CHAOS_IL2CPP_INTPTR chaos_async_yield_create(void) noexcept;
CHAOS_IL2CPP_INTPTR chaos_async_yield_get_awaiter(CHAOS_IL2CPP_INTPTR yield_awaiter) noexcept;
CHAOS_IL2CPP_INT32 chaos_async_yield_get_is_completed(CHAOS_IL2CPP_INTPTR yield_awaiter) noexcept;
void chaos_async_yield_get_result(CHAOS_IL2CPP_INTPTR yield_awaiter) noexcept;

// TaskAwaiter stubs (DirectNativeSymbol for async state machine dispatch).
// Generated code calls these when lowering async Task.GetResult() patterns.
void ChaosAsyncAwaiterGetResult(CHAOS_IL2CPP_INTPTR awaiter) noexcept;

// ── TaskCompletionSource<T> native helpers (Phase 3 P3-1) ──
// Each takes a TCS object handle as the first argument.
// Implemented in async_stubs.cpp via the async.h TaskSource proxy.
CHAOS_IL2CPP_INTPTR chaos_task_completion_source_create(void) noexcept;
CHAOS_IL2CPP_INTPTR chaos_tcs_get_task(CHAOS_IL2CPP_INTPTR tcs_handle) noexcept;
void chaos_tcs_set_result(CHAOS_IL2CPP_INTPTR tcs_handle, CHAOS_IL2CPP_INTPTR value) noexcept;
void chaos_tcs_set_result_void(CHAOS_IL2CPP_INTPTR tcs_handle) noexcept;
void chaos_tcs_set_exception(CHAOS_IL2CPP_INTPTR tcs_handle, CHAOS_IL2CPP_INTPTR exception) noexcept;
void chaos_tcs_set_canceled(CHAOS_IL2CPP_INTPTR tcs_handle) noexcept;
CHAOS_IL2CPP_INTPTR chaos_tcs_try_set_result(CHAOS_IL2CPP_INTPTR tcs_handle, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INTPTR chaos_tcs_try_set_result_void(CHAOS_IL2CPP_INTPTR tcs_handle) noexcept;
CHAOS_IL2CPP_INTPTR chaos_tcs_try_set_exception(CHAOS_IL2CPP_INTPTR tcs_handle, CHAOS_IL2CPP_INTPTR exception) noexcept;
CHAOS_IL2CPP_INTPTR chaos_tcs_try_set_canceled(CHAOS_IL2CPP_INTPTR tcs_handle) noexcept;

// ── Task.Delay native helpers (Phase 3 P3-2) ──
// TimerQueue-backed delayed task completion (implemented in async_stubs.cpp).
// Requires TimerQueueInitialize (via ThreadPoolInitialize / gate thread).
CHAOS_IL2CPP_INTPTR chaos_task_delay_stub(CHAOS_IL2CPP_INT32 millisecondsTimeout) noexcept;
CHAOS_IL2CPP_INTPTR chaos_task_delay_timespan_stub(CHAOS_IL2CPP_INT64 ticks) noexcept;

// ── Task.WhenAll / WhenAny native combinators (Phase 3 P3-3) ──
// `children` is a contiguous array of CHAOS_IL2CPP_INTPTR Task handles; n = count.
// Each returns a NEW aggregate AsyncTask handle (0 on invalid args/allocation failure).
CHAOS_IL2CPP_INTPTR chaos_task_when_all(CHAOS_IL2CPP_INTPTR* children, CHAOS_IL2CPP_INT32 n) noexcept;
CHAOS_IL2CPP_INTPTR chaos_task_when_any(CHAOS_IL2CPP_INTPTR* children, CHAOS_IL2CPP_INT32 n) noexcept;

// Managed-array overloads (codegen passes Task[] as an opaque CHAOS_IL2CPP_INTPTR):
CHAOS_IL2CPP_INTPTR chaos_task_when_all_array(CHAOS_IL2CPP_INTPTR tasks_handle) noexcept;
CHAOS_IL2CPP_INTPTR chaos_task_when_any_array(CHAOS_IL2CPP_INTPTR tasks_handle) noexcept;

}  // extern "C"
