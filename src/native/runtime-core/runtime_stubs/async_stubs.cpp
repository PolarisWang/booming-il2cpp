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
#include <chaos/async.h>

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

// ── TaskAwaiter.GetResult stub ─────────────────────────────────
// Called from async state machine dispatch code.  The awaiter is a
// managed TaskAwaiter object; this stub simply marks await as complete.
void ChaosAsyncAwaiterGetResult(CHAOS_IL2CPP_INTPTR awaiter) noexcept
{
    (void)awaiter;
    // No-op: TaskAwaiter.GetResult() propagates exceptions for failed tasks.
    // For test pipeline, assume the task completed successfully.
}

// ── TaskCompletionSource<T> native helpers (Phase 3 P3-1) ──
// These delegate to the TaskSource proxy in async.h; the "TCS handle" is
// a CHAOS_IL2CPP_INTPTR pointing to a TaskSource allocated in async_stubs.cpp.

CHAOS_IL2CPP_INTPTR chaos_task_completion_source_create(void) noexcept
{
    auto* ts = chaos::il2cpp::common::task_source_create();
    if (ts == nullptr) return 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(ts);
}

CHAOS_IL2CPP_INTPTR chaos_tcs_get_task(CHAOS_IL2CPP_INTPTR tcs_handle) noexcept
{
    if (tcs_handle == 0) return 0;
    auto* ts = reinterpret_cast<chaos::il2cpp::common::TaskSource*>(tcs_handle);
    return ts->get_task();
}

void chaos_tcs_set_result(CHAOS_IL2CPP_INTPTR tcs_handle, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (tcs_handle == 0) return;
    auto* ts = reinterpret_cast<chaos::il2cpp::common::TaskSource*>(tcs_handle);
    ts->set_result(value);
}

void chaos_tcs_set_exception(CHAOS_IL2CPP_INTPTR tcs_handle, CHAOS_IL2CPP_INTPTR exception) noexcept
{
    if (tcs_handle == 0) return;
    auto* ts = reinterpret_cast<chaos::il2cpp::common::TaskSource*>(tcs_handle);
    ts->set_exception(exception);
}

CHAOS_IL2CPP_INTPTR chaos_tcs_try_set_result(CHAOS_IL2CPP_INTPTR tcs_handle, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (tcs_handle == 0) return 0;
    auto* ts = reinterpret_cast<chaos::il2cpp::common::TaskSource*>(tcs_handle);
    return ts->try_set_result(value);
}

CHAOS_IL2CPP_INTPTR chaos_tcs_try_set_exception(CHAOS_IL2CPP_INTPTR tcs_handle, CHAOS_IL2CPP_INTPTR exception) noexcept
{
    if (tcs_handle == 0) return 0;
    auto* ts = reinterpret_cast<chaos::il2cpp::common::TaskSource*>(tcs_handle);
    return ts->try_set_exception(exception);
}

CHAOS_IL2CPP_INTPTR chaos_tcs_try_set_canceled(CHAOS_IL2CPP_INTPTR tcs_handle) noexcept
{
    if (tcs_handle == 0) return 0;
    auto* ts = reinterpret_cast<chaos::il2cpp::common::TaskSource*>(tcs_handle);
    return ts->try_set_canceled();
}

}  // extern "C"
