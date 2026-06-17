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

}  // extern "C"
