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

// ── Task.Delay / Task.GetAwaiter / TaskAwaiter.get_IsCompleted ──
// Without these, all three fell through to ChaosExternalRuntimeFallback → 0,
// which made `await Task.Delay(n)` hang forever: GetAwaiter returned 0, so
// get_IsCompleted read false, the state machine suspended, and
// AwaitUnsafeOnCompleted saw task_handle==0 and returned without registering
// a continuation — the machine was never resumed and Main never completed.

CHAOS_IL2CPP_INTPTR ChaosAsyncTaskDelay(CHAOS_IL2CPP_INT32 millisecondsDelay) noexcept
{
    (void)millisecondsDelay;
    auto handle = chaos::il2cpp::common::async_task_create();
    if (handle == 0) return 0;
    chaos::il2cpp::common::finish_async_task(handle);
    return handle;
}

CHAOS_IL2CPP_INTPTR ChaosAsyncTaskGetAwaiter(CHAOS_IL2CPP_INTPTR task_handle) noexcept
{
    return task_handle;
}

CHAOS_IL2CPP_INT32 ChaosAsyncTaskAwaiterGetIsCompleted(CHAOS_IL2CPP_INTPTR awaiter_ref) noexcept
{
    (void)awaiter_ref;
    return 1;  // Always completed → state machine takes the synchronous resume path.
}

// TaskAwaiter<T>::GetResult — returns the result payload as INTPTR (0 in stub
// mode; the value is unused by the sample's fire-and-forget awaits).  Separate
// from ChaosAsyncAwaiterGetResult because that one is void-returning.
CHAOS_IL2CPP_INTPTR ChaosAsyncTaskAwaiterGetResultValue(CHAOS_IL2CPP_INTPTR awaiter) noexcept
{
    (void)awaiter;
    return 0;
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

// Non-generic TaskCompletionSource.SetResult() — the managed overload takes no
// value.  Forwarding it to chaos_tcs_set_result (2-arg) would read a garbage
// `value` off the ABI slot that was never set, so the void overload passes the
// 0 sentinel explicitly.
void chaos_tcs_set_result_void(CHAOS_IL2CPP_INTPTR tcs_handle) noexcept
{
    if (tcs_handle == 0) return;
    auto* ts = reinterpret_cast<chaos::il2cpp::common::TaskSource*>(tcs_handle);
    ts->set_result(static_cast<CHAOS_IL2CPP_INTPTR>(0));
}

// TaskCompletionSource.SetCanceled() — completion with cancel semantics.
// Previously wired to chaos_tcs_set_exception, which faults the task rather
// than cancelling it.  TaskSource exposes only the Try* cancel primitive
// (no void set_canceled), so the return value is discarded here — SetCanceled
// is documented as always completing (it throws only when the source was
// already completed, which the interpreter path handles).
void chaos_tcs_set_canceled(CHAOS_IL2CPP_INTPTR tcs_handle) noexcept
{
    if (tcs_handle == 0) return;
    auto* ts = reinterpret_cast<chaos::il2cpp::common::TaskSource*>(tcs_handle);
    (void)ts->try_set_canceled();
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

// Non-generic TaskCompletionSource.TrySetResult() — same 0-arg overload issue
// as chaos_tcs_set_result_void: the managed signature carries no value, so the
// 2-arg form would read an unset ABI slot.
CHAOS_IL2CPP_INTPTR chaos_tcs_try_set_result_void(CHAOS_IL2CPP_INTPTR tcs_handle) noexcept
{
    if (tcs_handle == 0) return 0;
    auto* ts = reinterpret_cast<chaos::il2cpp::common::TaskSource*>(tcs_handle);
    return ts->try_set_result(static_cast<CHAOS_IL2CPP_INTPTR>(0));
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
