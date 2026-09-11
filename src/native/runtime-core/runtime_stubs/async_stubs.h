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

// ── Task.Delay / TaskAwaiter await path ──
// Without these, Task.Delay / Task.GetAwaiter / TaskAwaiter.get_IsCompleted
// fell through to ChaosExternalRuntimeFallback → 0, so `await Task.Delay(n)`
// suspended forever: GetAwaiter returned 0, IsCompleted read false, and
// AwaitUnsafeOnCompleted saw task_handle==0 → no continuation registered.
CHAOS_IL2CPP_INTPTR ChaosAsyncTaskDelay(CHAOS_IL2CPP_INT32 millisecondsDelay) noexcept;
CHAOS_IL2CPP_INTPTR ChaosAsyncTaskGetAwaiter(CHAOS_IL2CPP_INTPTR task_handle) noexcept;
CHAOS_IL2CPP_INT32 ChaosAsyncTaskAwaiterGetIsCompleted(CHAOS_IL2CPP_INTPTR awaiter_ref) noexcept;
CHAOS_IL2CPP_INTPTR ChaosAsyncTaskAwaiterGetResultValue(CHAOS_IL2CPP_INTPTR awaiter) noexcept;

// ── TaskCompletionSource<T> native helpers (Phase 3 P3-1) ──
// Each takes a TCS object handle as the first argument.
// Implemented in async_stubs.cpp via the async.h TaskSource proxy.
CHAOS_IL2CPP_INTPTR chaos_task_completion_source_create(void) noexcept;
CHAOS_IL2CPP_INTPTR chaos_tcs_get_task(CHAOS_IL2CPP_INTPTR tcs_handle) noexcept;
void chaos_tcs_set_result(CHAOS_IL2CPP_INTPTR tcs_handle, CHAOS_IL2CPP_INTPTR value) noexcept;
void chaos_tcs_set_exception(CHAOS_IL2CPP_INTPTR tcs_handle, CHAOS_IL2CPP_INTPTR exception) noexcept;
CHAOS_IL2CPP_INTPTR chaos_tcs_try_set_result(CHAOS_IL2CPP_INTPTR tcs_handle, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INTPTR chaos_tcs_try_set_exception(CHAOS_IL2CPP_INTPTR tcs_handle, CHAOS_IL2CPP_INTPTR exception) noexcept;
CHAOS_IL2CPP_INTPTR chaos_tcs_try_set_canceled(CHAOS_IL2CPP_INTPTR tcs_handle) noexcept;

// Non-generic TaskCompletionSource overloads whose managed signature carries no
// value argument.  Kept separate from the 2-arg forms above because the ABI slot
// for `value` is never populated on those call sites.
void chaos_tcs_set_result_void(CHAOS_IL2CPP_INTPTR tcs_handle) noexcept;
void chaos_tcs_set_canceled(CHAOS_IL2CPP_INTPTR tcs_handle) noexcept;
CHAOS_IL2CPP_INTPTR chaos_tcs_try_set_result_void(CHAOS_IL2CPP_INTPTR tcs_handle) noexcept;

}  // extern "C"
