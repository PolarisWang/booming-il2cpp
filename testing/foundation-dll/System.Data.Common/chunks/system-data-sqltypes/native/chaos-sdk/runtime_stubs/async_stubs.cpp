// async_stubs.cpp — Async yield stub implementations.
// These provide no-op stubs for SimpleForward runtime helpers registered
// in RuntimeHelperShapeRegistry (Task.Yield / YieldAwaitable).
//
// Managed code calling Task.Yield() will exercise these stubs through
// generated external-runtime-helper wrapper functions.  The stubs return
// zero/null values sufficient for the test dispatch path.
//
// NOTE: These stubs are NOT suitable for production use — real async yield
// requires thread-pool integration (TaskScheduler::Yield).

#include <chaos/native_types.h>

extern "C" {

CHAOS_IL2CPP_INTPTR chaos_async_yield_create(void) noexcept
{
    return 0;  // Not a real yield; test entry points don't await.
}

CHAOS_IL2CPP_INTPTR chaos_async_yield_get_awaiter(CHAOS_IL2CPP_INTPTR yield_awaiter) noexcept
{
    (void)yield_awaiter;
    return 0;
}

CHAOS_IL2CPP_INT32 chaos_async_yield_get_is_completed(CHAOS_IL2CPP_INTPTR yield_awaiter) noexcept
{
    (void)yield_awaiter;
    return 1;  // Always complete — YieldAwaitable.IsCompleted returns true in tests.
}

void chaos_async_yield_get_result(CHAOS_IL2CPP_INTPTR yield_awaiter) noexcept
{
    (void)yield_awaiter;
    // No-op: YieldAwaitable.GetResult() returns void.
}

}  // extern "C"
