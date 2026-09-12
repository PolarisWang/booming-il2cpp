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
#include <chaos/async_iterator.h>
#include "async_stubs.h"
#include "exception_helpers.h"
#include "exception_jmp.h"
#include "core/delegate_helpers.h"
#include "timer_queue.h"
#include "runtime_stubs/stub_common.h"

#include <chrono>
#include <thread>

#include <cstdint>
#include <new>
#include <mutex>
#include <vector>

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

// ── Task.Delay / Task.GetAwaiter / TaskAwaiter.get_IsCompleted (non-generic) ──
// The await path for a Task<T>/Task handle.  These MUST reflect real task state:
// a constant `is_completed → 1` would make every await take the synchronous
// resume path (no real suspension), and a constant `GetResult → 0` would
// silently corrupt the result of every `await Task<T>`.

// chaos_task_delay_stub (the TimerQueue-backed delay) is declared in
// async_stubs.h, which is included above, so it is visible here.
CHAOS_IL2CPP_INTPTR ChaosAsyncTaskDelay(CHAOS_IL2CPP_INT32 millisecondsDelay) noexcept
{
    // Task.Delay(int) contract (System.Threading.Tasks.Task):
    //   -1    → Timeout.Infinite: never completes on its own
    //   < -1  → ArgumentOutOfRangeException
    //   >= 0  → delay, then complete
    if (millisecondsDelay < -1) {
        chaos::il2cpp::runtime_core::RaiseManagedException(
            "System.ArgumentOutOfRangeException",
            "The value needs to be either -1 (signifying an infinite timeout), 0 or"
            " the correct amount of milliseconds otherwise.");
    }

    if (millisecondsDelay == -1) {
        // Infinite: hand back a live, never-completing task.
        return chaos::il2cpp::common::async_task_create();
    }

    // Real timed delay through the existing TimerQueue-backed helper.
    return chaos_task_delay_stub(millisecondsDelay);
}

CHAOS_IL2CPP_INTPTR ChaosAsyncTaskGetAwaiter(CHAOS_IL2CPP_INTPTR task_handle) noexcept
{
    // A TaskAwaiter is represented by the task handle itself (matches
    // async_task_get_awaiter in async.h), so field access resolves through
    // resolve_native_int_slot to the underlying AsyncTask.
    return task_handle;
}

CHAOS_IL2CPP_INT32 ChaosAsyncTaskAwaiterGetIsCompleted(CHAOS_IL2CPP_INTPTR awaiter_ref) noexcept
{
    using namespace chaos::il2cpp::common;
    if (awaiter_ref == 0) return 0;
    auto* task = reinterpret_cast<AsyncTask*>(awaiter_ref);
    return task->completed.load(std::memory_order_acquire)
        ? static_cast<CHAOS_IL2CPP_INT32>(1)
        : static_cast<CHAOS_IL2CPP_INT32>(0);
}

/// TaskAwaiter<T>.GetResult — returns the real result payload of a completed
/// task.  If the task FAULTED, raises the stored exception instead of returning
/// a value — this is what gives C# `await` its throw-on-fault semantics.
///
/// Callers must gate on IsCompleted before calling GetResult; the fault check
/// is the last safety layer (the one that makes `await taskThatFaults()` throw
/// rather than silently return 0).
CHAOS_IL2CPP_INTPTR ChaosAsyncTaskAwaiterGetResultValue(CHAOS_IL2CPP_INTPTR awaiter) noexcept
{
    using namespace chaos::il2cpp::common;
    using namespace chaos::il2cpp::runtime_core;
    if (awaiter == 0) return 0;
    auto* task = reinterpret_cast<AsyncTask*>(awaiter);

    // Cancellation is its own terminal state, checked FIRST and independently of
    // whether an exception payload happens to be present.  Inferring cancellation
    // from (faulted && exception == 0) would misreport a payload-less fault as
    // TaskCanceledException, and a cancelled task carrying a payload as a plain
    // fault.
    if (task->canceled.load(std::memory_order_acquire))
    {
        RaiseManagedException(
            "System.Threading.Tasks.TaskCanceledException",
            "A task was cancelled.");
    }

    // Faulted → propagate the stored exception (C# await throws on faulted
    // tasks, it does NOT return a default value).  This is the critical path
    // for `await taskThatFails()` semantics.
    if (task->faulted.load(std::memory_order_acquire))
    {
        CHAOS_IL2CPP_INTPTR ex = task->exception;
        if (ex != 0)
        {
            chaos_raise_exception(ex);
        }
        // A fault with no payload cannot be rethrown as a managed object.  Surface
        // it as a fault, NOT as cancellation — the two are distinct states.
        RaiseManagedException(
            "System.Exception",
            "A task faulted without an exception payload.");
    }

    if (!task->completed.load(std::memory_order_acquire)) return 0;
    return task->result;
}

/// TaskAwaiter non-generic GetResult — void-returning, same fault propagation.
/// TaskAwaiter non-generic GetResult — void-returning, same three-state
/// fault/cancel propagation as the value-returning form.
void ChaosAsyncTaskAwaiterGetResultVoid(CHAOS_IL2CPP_INTPTR awaiter) noexcept
{
    using namespace chaos::il2cpp::common;
    using namespace chaos::il2cpp::runtime_core;
    if (awaiter == 0) return;
    auto* task = reinterpret_cast<AsyncTask*>(awaiter);

    if (task->canceled.load(std::memory_order_acquire))
    {
        RaiseManagedException(
            "System.Threading.Tasks.TaskCanceledException",
            "A task was cancelled.");
    }

    if (task->faulted.load(std::memory_order_acquire))
    {
        CHAOS_IL2CPP_INTPTR ex = task->exception;
        if (ex != 0) { chaos_raise_exception(ex); }
        RaiseManagedException(
            "System.Exception",
            "A task faulted without an exception payload.");
    }
}

/// True when the task is complete (success or fault) — the synchronous
/// completion oracle used by Task.Wait polling loops.
CHAOS_IL2CPP_INT32 ChaosAsyncTaskGetIsCompleted(CHAOS_IL2CPP_INTPTR task_handle) noexcept
{
    using namespace chaos::il2cpp::common;
    if (task_handle == 0) return 0;
    auto* task = reinterpret_cast<AsyncTask*>(task_handle);
    return task->completed.load(std::memory_order_acquire)
        ? static_cast<CHAOS_IL2CPP_INT32>(1)
        : static_cast<CHAOS_IL2CPP_INT32>(0);
}

/// True when the task faulted.  Cancellation is a separate state and reports
/// false here, so a caller can distinguish faulted from cancelled even when the
/// fault carries no exception payload.
CHAOS_IL2CPP_INT32 ChaosAsyncTaskGetIsFaulted(CHAOS_IL2CPP_INTPTR task_handle) noexcept
{
    using namespace chaos::il2cpp::common;
    if (task_handle == 0) return 0;
    auto* task = reinterpret_cast<AsyncTask*>(task_handle);
    return task->faulted.load(std::memory_order_acquire)
        ? static_cast<CHAOS_IL2CPP_INT32>(1)
        : static_cast<CHAOS_IL2CPP_INT32>(0);
}

/// True when the task was cancelled.
CHAOS_IL2CPP_INT32 ChaosAsyncTaskGetIsCanceled(CHAOS_IL2CPP_INTPTR task_handle) noexcept
{
    using namespace chaos::il2cpp::common;
    if (task_handle == 0) return 0;
    auto* task = reinterpret_cast<AsyncTask*>(task_handle);
    return task->canceled.load(std::memory_order_acquire)
        ? static_cast<CHAOS_IL2CPP_INT32>(1)
        : static_cast<CHAOS_IL2CPP_INT32>(0);
}

/// Task.Wait() / Task<T>.Result — block the calling thread until the task
/// completes, then propagate its outcome exactly like the await path:
///   - faulted  → raise the stored exception (TaskCanceledException if none)
///   - success  → return
///
/// Same-thread deadlock note: a task whose continuation is scheduled on the
/// same thread that is blocking here cannot complete.  .NET throws
/// InvalidOperationException in that case; we cannot detect it reliably from
/// the handle alone, so this uses a bounded spin and then reports the timeout
/// rather than hanging forever.
///
/// Returns 1 if the task completed (caller may then read the result), 0 on
/// timeout.
CHAOS_IL2CPP_INT32 ChaosAsyncTaskWait(CHAOS_IL2CPP_INTPTR task_handle, CHAOS_IL2CPP_INT32 timeout_ms) noexcept
{
    using namespace chaos::il2cpp::common;
    using namespace chaos::il2cpp::runtime_core;

    if (task_handle == 0) return 0;
    auto* task = reinterpret_cast<AsyncTask*>(task_handle);

    // Timeout.Infinite (-1) → wait indefinitely; >= 0 → bounded wait.
    //
    // Park on the task's condition variable rather than spinning on yield().
    // A spin loop burns a full core for the whole wait and, when there are more
    // blocked waiters than free cores, starves the worker thread the waited-for
    // task needs — a livelock that presents as a hang.  park_until_completed
    // holds wait_mtx across the completion check, so a completion published
    // between the check and the sleep cannot be lost.
    if (timeout_ms < 0)
    {
        chaos::il2cpp::common::park_until_completed(task, nullptr);
    }
    else
    {
        const auto deadline = std::chrono::steady_clock::now() +
            std::chrono::milliseconds(timeout_ms);
        if (!chaos::il2cpp::common::park_until_completed(task, &deadline))
        {
            return 0;  // timed out — caller decides whether to throw
        }
    }

    // Completed: propagate the terminal state before returning so
    // `task.Wait()` on a failed task throws, matching .NET (which wraps in
    // AggregateException; we raise the original, which `await`-style callers
    // expect).  Cancellation is checked first and independently of the payload.
    if (task->canceled.load(std::memory_order_acquire))
    {
        RaiseManagedException(
            "System.Threading.Tasks.TaskCanceledException",
            "A task was cancelled.");
    }
    if (task->faulted.load(std::memory_order_acquire))
    {
        CHAOS_IL2CPP_INTPTR ex = task->exception;
        if (ex != 0) { chaos_raise_exception(ex); }
        RaiseManagedException(
            "System.Exception",
            "A task faulted without an exception payload.");
    }
    return 1;
}

/// Task<T>.Result / Task.GetAwaiter().GetResult() — block until complete, then
/// return the result payload.  Faults propagate (throw) rather than returning 0.
CHAOS_IL2CPP_INTPTR ChaosAsyncTaskGetResultBlocking(CHAOS_IL2CPP_INTPTR task_handle) noexcept
{
    using namespace chaos::il2cpp::common;
    if (task_handle == 0) return 0;
    auto* task = reinterpret_cast<AsyncTask*>(task_handle);

    // Block indefinitely, then read.  ChaosAsyncTaskWait raises on fault.
    ChaosAsyncTaskWait(task_handle, -1);
    return task->result;
}

// ── TaskCompletionSource<T> native helpers (Phase 3 P3-1) ──// These delegate to the TaskSource proxy in async.h; the "TCS handle" is
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

void chaos_tcs_set_canceled(CHAOS_IL2CPP_INTPTR tcs_handle) noexcept
{
    if (tcs_handle == 0) return;
    auto* ts = reinterpret_cast<chaos::il2cpp::common::TaskSource*>(tcs_handle);
    ts->try_set_canceled();
}

CHAOS_IL2CPP_INTPTR chaos_tcs_try_set_canceled(CHAOS_IL2CPP_INTPTR tcs_handle) noexcept
{
    if (tcs_handle == 0) return 0;
    auto* ts = reinterpret_cast<chaos::il2cpp::common::TaskSource*>(tcs_handle);
    return ts->try_set_canceled();
}

// ── Task.Delay native helpers (Phase 3 P3-2) ──
// These use the existing TimerQueue to schedule delayed completion.
// TimerQueueInitialize must have been called (via ThreadPoolInitialize).

namespace {

struct DelayCompletion {
    chaos::il2cpp::common::AsyncTask* task;
    CHAOS_IL2CPP_INTPTR handle;
};

void DelayTimerCallback(void* state) noexcept {
    auto* dc = static_cast<DelayCompletion*>(state);
    dc->task->completed.store(true, std::memory_order_release);
    chaos::il2cpp::common::notify_task_completed(dc->task);
    chaos::il2cpp::common::finish_async_task(dc->handle);
    delete dc;
}

} // anonymous namespace

/// Core delay internal: create an AsyncTask, register one-shot timer,
/// complete the task when the timer fires.  Returns task handle (0 on failure).
static CHAOS_IL2CPP_INTPTR ChaosTaskDelayCore(uint32_t due_time_ms) noexcept {
    using namespace chaos::il2cpp::common;
    using namespace chaos::il2cpp::runtime_core::threading;
    auto* task = new (std::nothrow) AsyncTask();
    if (task == nullptr) return 0;
    CHAOS_IL2CPP_INTPTR handle = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(task);
    auto* dc = new (std::nothrow) DelayCompletion{task, handle};
    if (dc == nullptr) {
        delete task;
        return 0;
    }
    uint32_t timer_id = TimerQueueCreate(DelayTimerCallback, dc, due_time_ms, 0);
    if (timer_id == kTimerQueueInvalidId) {
        delete dc;
        delete task;
        return 0;
    }
    return handle;
}

CHAOS_IL2CPP_INTPTR chaos_task_delay_stub(CHAOS_IL2CPP_INT32 millisecondsTimeout) noexcept
{
    if (millisecondsTimeout <= 0) {
        return ChaosTaskDelayCore(0);
    }
    return ChaosTaskDelayCore(static_cast<uint32_t>(millisecondsTimeout));
}

CHAOS_IL2CPP_INTPTR chaos_task_delay_timespan_stub(CHAOS_IL2CPP_INT64 ticks) noexcept
{
    constexpr int64_t kTicksPerMs = 10000;
    CHAOS_IL2CPP_INT32 ms = 0;
    if (ticks > 0) {
        int64_t cnt = ticks / kTicksPerMs;
        if (cnt > static_cast<int64_t>(INT32_MAX)) cnt = INT32_MAX;
        ms = static_cast<CHAOS_IL2CPP_INT32>(cnt);
    }
    return ChaosTaskDelayCore(static_cast<uint32_t>(ms));
}

// ── Task.WhenAll / WhenAny native combinators (Phase 3 P3-3) ──
// Given a contiguous array of child AsyncTask handles, produce a NEW aggregate
// AsyncTask handle whose completion the combinator drives:
//   WhenAll: aggregate completes when ALL children complete (faults if any
//            child faulted — first observed exception propagates).
//   WhenAny: aggregate completes when the FIRST child completes; the 1-based
//            winner index is published as aggregate->result.
// The aggregate handle is an ordinary AsyncTask, awaitable via the single-slot
// async_task_on_completed continuation.

namespace {

// Build a managed array holding each child's result, in declaration order.
//
// The elements live contiguously after the 32-byte ManagedArrayAccessor header,
// matching what codegen's array accessors expect (see stub_common.h).  Returns 0
// on allocation failure — the aggregate still completes, it just has no result
// set, which the caller observes as a null rather than a wrong value.
inline CHAOS_IL2CPP_INTPTR BuildResultArrayFromHandles(
    const CHAOS_IL2CPP_INTPTR* children, int n) noexcept
{
    if (n <= 0) return 0;
    const size_t bytes = sizeof(ManagedArrayAccessor)
        + static_cast<size_t>(n) * sizeof(CHAOS_IL2CPP_INTPTR);
    auto* raw = static_cast<uint8_t*>(std::malloc(bytes));
    if (raw == nullptr) return 0;
    auto* arr = reinterpret_cast<ManagedArrayAccessor*>(raw);
    arr->element_type_shape = 0;
    arr->element_type_info = nullptr;
    arr->length = static_cast<CHAOS_IL2CPP_INTPTR>(n);
    auto* elements = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(
        raw + sizeof(ManagedArrayAccessor));
    for (int i = 0; i < n; ++i) {
        elements[i] = chaos::il2cpp::common::require_async_task(children[i])->result;
    }
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(arr);
}

// Shared completion state across the N child continuations.
struct WhenState {
    chaos::il2cpp::common::AsyncTask*    aggregate;
    CHAOS_IL2CPP_INTPTR                  aggregate_handle;
    std::atomic<int>                     remaining;
    bool                                 mode_when_all;      // true=WhenAll
    std::atomic<bool>                    won;                // WhenAny single-fire
    CHAOS_IL2CPP_INTPTR*                 children;           // child handles array
    int                                  n;                  // child count
    // Set when this state owns `children` (the managed-array overload copies the
    // element handles and must keep them alive until the aggregate completes).
    bool                                 owns_children;
};

// Delivered when a child completes; task_handle = the completing child.
void WhenChildContinuation(CHAOS_IL2CPP_INTPTR task_handle, void* ctx) noexcept {
    using namespace chaos::il2cpp::common;
    auto* st = static_cast<WhenState*>(ctx);

    if (st->mode_when_all) {
        int rem = st->remaining.fetch_sub(1, std::memory_order_acq_rel) - 1;
        if (rem == 0) {
            // All children done: fault iff any faulted, and collect the results.
            bool any_faulted = false;
            CHAOS_IL2CPP_INTPTR ex = 0;
            for (int i = 0; i < st->n; ++i) {
                auto* child = require_async_task(st->children[i]);
                if (child->faulted.load(std::memory_order_acquire)) {
                    any_faulted = true;
                    ex = child->exception;
                    break;
                }
            }
            st->aggregate->exception = ex;
            st->aggregate->faulted.store(any_faulted, std::memory_order_relaxed);

            // Result SET: build a managed array holding each child's result in
            // DECLARATION order (not completion order).  Task.WhenAll's contract
            // is that the i-th element is the i-th task's result; appending as
            // children arrive would reorder them.  Skipped when a child faulted —
            // WhenAll's value is not meaningful then, matching .NET, where the
            // caller gets the exception instead.
            if (!any_faulted && st->n > 0) {
                st->aggregate->result = BuildResultArrayFromHandles(st->children, st->n);
            } else {
                st->aggregate->result = static_cast<CHAOS_IL2CPP_INTPTR>(0);
            }

            st->aggregate->completed.store(true, std::memory_order_release);
            chaos::il2cpp::common::notify_task_completed(st->aggregate);
            finish_async_task(st->aggregate_handle);
            if (st->owns_children) delete[] st->children;
            delete st;
        }
        return;
    }

    // WhenAny: first-to-complete wins → find its index by handle.
    if (st->won.exchange(true, std::memory_order_acq_rel)) return;  // lost
    int winner = 0;
    for (int i = 0; i < st->n; ++i) {
        if (st->children[i] == task_handle) { winner = i; break; }
    }
    st->aggregate->result = static_cast<CHAOS_IL2CPP_INTPTR>(winner + 1);  // 1-based
    st->aggregate->exception = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    st->aggregate->faulted.store(false, std::memory_order_relaxed);
    st->aggregate->completed.store(true, std::memory_order_release);
    chaos::il2cpp::common::notify_task_completed(st->aggregate);
    finish_async_task(st->aggregate_handle);
    if (st->owns_children) delete[] st->children;
    delete st;
}

} // anonymous namespace

/// Shared internal: build aggregate + register a continuation on every child.
/// `take_ownership` — the caller allocated `children` and transfers it to the
/// aggregate, which frees it on completion.  The public flat-pointer overload
/// passes false because its caller owns the array.
static CHAOS_IL2CPP_INTPTR WhenAllAnyInternal(
    CHAOS_IL2CPP_INTPTR* children, CHAOS_IL2CPP_INT32 n, bool when_all,
    bool take_ownership = false) noexcept
{
    using namespace chaos::il2cpp::common;
    if (n < 0) { if (take_ownership) delete[] children; return 0; }
    if (children == nullptr && n > 0) { if (take_ownership) delete[] children; return 0; }
    auto* agg = new (std::nothrow) AsyncTask();
    if (agg == nullptr) { if (take_ownership) delete[] children; return 0; }
    auto* st = new (std::nothrow) WhenState();
    if (st == nullptr) { delete agg; if (take_ownership) delete[] children; return 0; }
    CHAOS_IL2CPP_INTPTR agg_handle = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(agg);
    st->aggregate = agg;
    st->aggregate_handle = agg_handle;
    st->remaining.store(n, std::memory_order_relaxed);
    st->mode_when_all = when_all;
    st->won.store(false, std::memory_order_relaxed);
    st->children = children;
    st->n = n;
    st->owns_children = take_ownership;

    if (n == 0) {
        // Empty WhenAll completes immediately (empty WhenAny is invalid → caller
        // guards; keep symmetric: complete immediately, result undefined).
        agg->completed.store(true, std::memory_order_release);
        chaos::il2cpp::common::notify_task_completed(agg);
        finish_async_task(agg_handle);
        if (take_ownership) delete[] children;
        delete st;
        return agg_handle;
    }

    for (int i = 0; i < n; ++i) {
        CHAOS_IL2CPP_INTPTR child = children[i];
        if (child == static_cast<CHAOS_IL2CPP_INTPTR>(0)) { continue; }
        // A fully-synchronous completed child fires its continuation inline here,
        // decrementing `remaining`.  Aggregate handles that correctly.
        async_task_on_completed(child, WhenChildContinuation, st);
    }
    return agg_handle;
}

/// Task.WhenAll(Task[] children, int n).  children is a non-null contiguous
/// array of n AsyncTask handles (owned by caller for the duration; the internal
/// only reads them during synchronous scan on completion).  Returns the
/// aggregate handle (0 on bad args / alloc failure).
CHAOS_IL2CPP_INTPTR chaos_task_when_all(CHAOS_IL2CPP_INTPTR* children, CHAOS_IL2CPP_INT32 n) noexcept
{
    return WhenAllAnyInternal(children, n, /*when_all=*/true);
}

/// Task.WhenAny(Task[] children, int n).  Returns an aggregate handle whose
/// result is 1 + the index of the first child to complete.
CHAOS_IL2CPP_INTPTR chaos_task_when_any(CHAOS_IL2CPP_INTPTR* children, CHAOS_IL2CPP_INT32 n) noexcept
{
    return WhenAllAnyInternal(children, n, /*when_all=*/false);
}

// ── Managed-array overloads used by ShapeRegistry (codegen passes Task[] as INTPTR) ──
// The contraining method unpack the managed handle array and delegate to
// chaos_task_when_all/any which expect a flat element handle array.
static CHAOS_IL2CPP_INTPTR WhenAllAnyManagedArray(
    CHAOS_IL2CPP_INTPTR tasks_handle, bool when_all) noexcept
{
    if (tasks_handle == 0) {
        return when_all ? chaos_task_when_all(nullptr, 0) : 0;
    }
    auto* arr = get_managed_array(tasks_handle);
    if (arr == nullptr) return 0;
    CHAOS_IL2CPP_INT32 n = static_cast<CHAOS_IL2CPP_INT32>(arr->length);
    auto* elements = accessor_get_elements(
        const_cast<ManagedArrayAccessor*>(arr));
    auto* mem = new (std::nothrow) CHAOS_IL2CPP_INTPTR[static_cast<size_t>(n)];
    if (mem == nullptr) return 0;
    for (CHAOS_IL2CPP_INT32 i = 0; i < n; ++i) mem[i] = elements[i];
    // take_ownership: the aggregate's continuation reads these handles when each
    // child completes, which may be LATER than this call.  Freeing `mem` here
    // left the aggregate dereferencing freed memory, so the copy must live as
    // long as the aggregate; the aggregate frees it on completion.
    return WhenAllAnyInternal(mem, n, when_all, /*take_ownership=*/true);
}

/// ShapeRegistry symbol for WhenAll(Task[]): extract from managed array handle.
CHAOS_IL2CPP_INTPTR chaos_task_when_all_array(CHAOS_IL2CPP_INTPTR tasks_handle) noexcept
{
    return WhenAllAnyManagedArray(tasks_handle, /*when_all=*/true);
}

/// ShapeRegistry symbol for WhenAny(Task[]): extract from managed array handle.
CHAOS_IL2CPP_INTPTR chaos_task_when_any_array(CHAOS_IL2CPP_INTPTR tasks_handle) noexcept
{
    return WhenAllAnyManagedArray(tasks_handle, /*when_all=*/false);
}

// ══════════════════════════════════════════════════════════════════════════════
// TaskFactory.ContinueWhenAll / ContinueWhenAny (Phase 2)
// ══════════════════════════════════════════════════════════════════════════════
//
// A composition, not a new mechanism: build the aggregate over the task array
// with the existing combinator, then register the continuation on that
// aggregate through the existing ContinueWith delivery path.
//
// Getting this wrong in the obvious way is worth calling out.  Registering the
// continuation on EACH CHILD would fire it once per task (N times for
// ContinueWhenAll) instead of once — a program that looks like it works on a
// single-element array and multiplies side effects on any other.  The
// continuation is therefore registered on the AGGREGATE, which completes
// exactly once by construction.
static CHAOS_IL2CPP_INTPTR ContinueWhenInternal(
    CHAOS_IL2CPP_INTPTR tasks_handle, CHAOS_IL2CPP_INTPTR continuation,
    bool when_all) noexcept
{
    if (continuation == 0) return 0;
    CHAOS_IL2CPP_INTPTR aggregate = when_all
        ? chaos_task_when_all_array(tasks_handle)
        : chaos_task_when_any_array(tasks_handle);
    if (aggregate == 0) return 0;
    return chaos_task_continue_with(aggregate, continuation);
}

CHAOS_IL2CPP_INTPTR chaos_task_continue_when_all_array(
    CHAOS_IL2CPP_INTPTR tasks_handle, CHAOS_IL2CPP_INTPTR continuation) noexcept
{
    return ContinueWhenInternal(tasks_handle, continuation, /*when_all=*/true);
}

CHAOS_IL2CPP_INTPTR chaos_task_continue_when_any_array(
    CHAOS_IL2CPP_INTPTR tasks_handle, CHAOS_IL2CPP_INTPTR continuation) noexcept
{
    return ContinueWhenInternal(tasks_handle, continuation, /*when_all=*/false);
}

// ══════════════════════════════════════════════════════════════════════════════
// Task.WhenEach (Phase 2 / ASYNC-P2-8)
// ══════════════════════════════════════════════════════════════════════════════
//
// ORDER-PRESERVING completion stream.  Semantics differ from both combinators
// above, which is why this cannot reuse WhenAllAnyInternal:
//
//   WhenAll  — yields ONCE, after every task completed.
//   WhenAny  — yields ONCE, when the first task completed.
//   WhenEach — yields ONCE PER TASK, in COMPLETION order.
//
// Design: the returned handle owns a queue and a completion signal.  Every child
// is registered with a continuation at construction, so no completion can be
// missed in the window between the caller draining one element and re-arming for
// the next.  A child that completes synchronously during registration is appended
// inline (the continuation runs under the same lock), so ordering is by actual
// completion, not by registration.
namespace {

// One element of the completion-ordered stream.
struct WhenEachState {
    std::mutex mtx;
    std::vector<CHAOS_IL2CPP_INTPTR> ready;   // completed task handles, FIFO by completion
    CHAOS_IL2CPP_INT32 remaining = 0;         // tasks not yet completed
    bool faulted = false;
    CHAOS_IL2CPP_INTPTR exception = 0;        // first observed exception, if any
    bool owns_children = false;
};

/// Records the just-completed child into the ready queue.  The continuation
/// receives the task handle as a parameter, which is what we must queue.
static void WhenEachContinuation(CHAOS_IL2CPP_INTPTR task_handle, void* user) noexcept
{
    auto* st = static_cast<WhenEachState*>(user);
    if (st == nullptr) return;
    std::lock_guard<std::mutex> lock(st->mtx);
    if (st->remaining > 0) --st->remaining;
    if (task_handle != 0) {
        st->ready.push_back(task_handle);
    }
}

} // anonymous namespace

/// Task.WhenEach(Task[]) — returns a handle to a completion-ordered queue.
///
/// The handle is NOT an AsyncTask: the consumer drains it repeatedly rather than
/// awaiting it once.  Every child is registered up front so completions that
/// happen while the consumer is between elements are still queued.
CHAOS_IL2CPP_INTPTR chaos_task_when_each_array(CHAOS_IL2CPP_INTPTR tasks_handle) noexcept
{
    if (tasks_handle == 0) return 0;
    auto* arr = get_managed_array(tasks_handle);
    if (arr == nullptr) return 0;
    CHAOS_IL2CPP_INT32 n = static_cast<CHAOS_IL2CPP_INT32>(arr->length);
    if (n <= 0) return 0;

    auto* elements = accessor_get_elements(
        const_cast<ManagedArrayAccessor*>(arr));

    auto* st = new (std::nothrow) WhenEachState();
    if (st == nullptr) return 0;
    st->remaining = n;
    try {
        st->ready.reserve(static_cast<size_t>(n));
    } catch (...) {
        delete st;
        return 0;
    }

    for (CHAOS_IL2CPP_INT32 i = 0; i < n; ++i) {
        CHAOS_IL2CPP_INTPTR child = elements[i];
        if (child == static_cast<CHAOS_IL2CPP_INTPTR>(0)) {
            // A null element still occupies a slot in the stream; account for it so
            // the terminator stays exact rather than hanging on a phantom completion.
            std::lock_guard<std::mutex> lock(st->mtx);
            if (st->remaining > 0) --st->remaining;
            continue;
        }
        // A synchronously-completed child fires this inline, appending under the
        // lock; an asynchronous one appends from whichever thread completes it.
        // Either way the FIFO reflects real completion order.
        async_task_on_completed(child, WhenEachContinuation, st);
    }

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(st);
}

/// True when the WhenEach stream still has elements to hand out (queued now, or
/// awaiting completion).  The consumer polls this to decide whether another
/// MoveNextAsync will produce a value.
CHAOS_IL2CPP_INT32 chaos_task_when_each_may_have_next(CHAOS_IL2CPP_INTPTR stream) noexcept
{
    if (stream == 0) return 0;
    auto* st = reinterpret_cast<WhenEachState*>(stream);
    std::lock_guard<std::mutex> lock(st->mtx);
    return (!st->ready.empty() || st->remaining > 0) ? 1 : 0;
}

/// Pops the next completed task handle in COMPLETION order.  Returns 0 when the
/// queue is momentarily empty; the caller should await rather than conclude the
/// stream ended — use chaos_task_when_each_may_have_next for that decision.
CHAOS_IL2CPP_INTPTR chaos_task_when_each_try_dequeue(CHAOS_IL2CPP_INTPTR stream) noexcept
{
    if (stream == 0) return 0;
    auto* st = reinterpret_cast<WhenEachState*>(stream);
    std::lock_guard<std::mutex> lock(st->mtx);
    if (st->ready.empty()) return 0;
    CHAOS_IL2CPP_INTPTR next = st->ready.front();
    st->ready.erase(st->ready.begin());
    return next;
}

/// Releases a stream once the consumer is done with it (e.g. on DisposeAsync or
/// when the enumerable is abandoned).  Safe to call with 0.
void chaos_task_when_each_destroy(CHAOS_IL2CPP_INTPTR stream) noexcept
{
    if (stream == 0) return;
    auto* st = reinterpret_cast<WhenEachState*>(stream);
    delete st;
}

// ══════════════════════════════════════════════════════════════════════════════
// Task.ContinueWith (Phase 2 P2-2)
// ══════════════════════════════════════════════════════════════════════════════

namespace {

// State for one ContinueWith registration.  Held until the continuation has
// run; the continuation task is owned by the caller (see the comment on
// chaos_task_continue_with).
struct ContinueWithState {
    CHAOS_IL2CPP_INTPTR antecedent;    // task the continuation observes
    CHAOS_IL2CPP_INTPTR continuation;  // DelegateObject* for the body
    CHAOS_IL2CPP_INTPTR continuation_task;
};

// Deliver the continuation exactly once.
//
// The continuation delegate takes the ANTECEDENT handle as its single argument
// (so `t.ContinueWith(a => a.Result)` can inspect the prior task) and returns a
// native int, which becomes the continuation task's result — that is what makes
// ContinueWith chainable rather than a fire-and-forget callback.
void ContinueWithDelivery(CHAOS_IL2CPP_INTPTR /*antecedent_handle*/, void* ctx) noexcept {
    auto* st = static_cast<ContinueWithState*>(ctx);
    if (st == nullptr) return;

    CHAOS_IL2CPP_INTPTR args[1] = {st->antecedent};
    CHAOS_IL2CPP_INTPTR ret = 0;
    chaos::il2cpp::runtime_core::chaos_delegate_object_invoke(
        st->continuation, args, &ret, 1);

    auto* cont = chaos::il2cpp::common::require_async_task(st->continuation_task);
    cont->result = ret;
    cont->exception = 0;
    cont->faulted.store(false, std::memory_order_relaxed);
    cont->canceled.store(false, std::memory_order_relaxed);
    cont->completed.store(true, std::memory_order_release);
    chaos::il2cpp::common::notify_task_completed(cont);
    chaos::il2cpp::common::finish_async_task(st->continuation_task);
    delete st;
}

}  // anonymous namespace

/// Register `continuation` to run when `antecedent` completes.
///
/// Runs unconditionally — a faulted or cancelled antecedent still invokes the
/// continuation (with the antecedent's fault observable through the handle it
/// receives).  Only TaskContinuationOptions.OnlyOn* variants would restrict
/// this, and those are not modelled here.
///
/// Ownership: the returned continuation task is heap-allocated and is NOT
/// reference-counted or GC-owned in this standalone path, so it persists for
/// the process lifetime.  Making AsyncTask GC-owned is defect D1 (Phase 6).
CHAOS_IL2CPP_INTPTR chaos_task_continue_with(
    CHAOS_IL2CPP_INTPTR antecedent, CHAOS_IL2CPP_INTPTR continuation) noexcept
{
    using namespace chaos::il2cpp::common;
    if (antecedent == 0 || continuation == 0) return 0;

    auto* st = new (std::nothrow) ContinueWithState();
    if (st == nullptr) return 0;
    st->antecedent = antecedent;
    st->continuation = continuation;
    st->continuation_task = async_task_create();

    // async_task_on_completed fires inline when the antecedent already
    // completed, and stores + delivers via finish_async_task otherwise — so
    // both completion orders are covered without a branch here.
    if (async_task_on_completed(antecedent, ContinueWithDelivery, st) == 0) {
        delete st;
        return 0;
    }
    return st->continuation_task;
}

// ══════════════════════════════════════════════════════════════════════════════
// TaskFactory (Phase 2 P2-5)
// ══════════════════════════════════════════════════════════════════════════════

/// Task.get_Factory — see the header for why this is a bare non-null token.
CHAOS_IL2CPP_INTPTR chaos_task_default_factory() noexcept
{
    return 1;
}

/// TaskFactory::StartNew(Action) — delegates to the same ThreadPool-backed
/// runner Task.Run uses (async_task_run).
///
/// This is a correct match, not a shortcut: the default TaskFactory schedules
/// StartNew on TaskScheduler.Current, which outside a scheduler context is the
/// default (ThreadPool) scheduler — the very scheduler Task.Run targets.  The
/// non-delegate / options / token overloads are rejected at the codegen registry
/// and never reach here.
CHAOS_IL2CPP_INTPTR chaos_task_factory_start_new(
    CHAOS_IL2CPP_INTPTR /*factory*/, CHAOS_IL2CPP_INTPTR delegate_fn) noexcept
{
    using namespace chaos::il2cpp::common;
    if (delegate_fn == 0) return 0;
    return async_task_run(delegate_fn);
}

// ══════════════════════════════════════════════════════════════════════════════
// AsyncIteratorMethodBuilder (ASYNC-P2-8 A2).
//
// The thin extern "C" shell over chaos/async_iterator.h.  The logic lives in the
// header so the standalone iterator tests can exercise it without linking
// runtime_core; these wrappers exist so codegen-emitted calls have a real symbol
// to bind to instead of an external-runtime stub.
//
// The builder handle IS a source pool; `sm_box` is the iterator state machine and
// `move_next_fn` its native MoveNext.  Both are carried as CHAOS_IL2CPP_INTPTR and
// reinterpret_cast at the boundary, matching how the AsyncTaskMethodBuilder
// helpers in chaos/async.h take their state machine.
// ══════════════════════════════════════════════════════════════════════════════

CHAOS_IL2CPP_INTPTR chaos_async_iterator_builder_create(void) noexcept
{
    return chaos::il2cpp::common::async_iterator_builder_create();
}

CHAOS_IL2CPP_INTPTR chaos_async_iterator_builder_move_next(
    CHAOS_IL2CPP_INTPTR builder_handle,
    CHAOS_IL2CPP_INTPTR move_next_fn,
    CHAOS_IL2CPP_INTPTR sm_box) noexcept
{
    return chaos::il2cpp::common::async_iterator_builder_move_next(
        builder_handle,
        reinterpret_cast<chaos::il2cpp::common::AsyncIteratorMoveNextFn>(move_next_fn),
        reinterpret_cast<void*>(sm_box));
}

CHAOS_IL2CPP_INTPTR chaos_async_iterator_source_acquire(
    CHAOS_IL2CPP_INTPTR builder_handle, CHAOS_IL2CPP_INT32* out_token) noexcept
{
    auto* pool = chaos::il2cpp::common::as_iterator_pool(builder_handle);
    CHAOS_IL2CPP_INT16 token = 0;
    auto* core = pool->Acquire(&token);
    if (out_token != nullptr) *out_token = static_cast<CHAOS_IL2CPP_INT32>(token);
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(core);
}

void chaos_async_iterator_source_release(
    CHAOS_IL2CPP_INTPTR builder_handle, CHAOS_IL2CPP_INTPTR source) noexcept
{
    chaos::il2cpp::common::as_iterator_pool(builder_handle)->Release(
        reinterpret_cast<chaos::il2cpp::common::AsyncIteratorSourceCore*>(source));
}

void chaos_async_iterator_source_set_result(
    CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INT32 value) noexcept
{
    reinterpret_cast<chaos::il2cpp::common::AsyncIteratorSourceCore*>(source)
        ->SetResult(value != 0);
}

void chaos_async_iterator_source_set_exception(
    CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR exception) noexcept
{
    reinterpret_cast<chaos::il2cpp::common::AsyncIteratorSourceCore*>(source)
        ->SetException(exception);
}

CHAOS_IL2CPP_INT32 chaos_async_iterator_source_get_status(
    CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INT32 token) noexcept
{
    return static_cast<CHAOS_IL2CPP_INT32>(
        reinterpret_cast<chaos::il2cpp::common::AsyncIteratorSourceCore*>(source)
            ->GetStatus(static_cast<CHAOS_IL2CPP_INT16>(token)));
}

CHAOS_IL2CPP_INT32 chaos_async_iterator_source_get_result(
    CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INT32 token) noexcept
{
    return static_cast<CHAOS_IL2CPP_INT32>(
        reinterpret_cast<chaos::il2cpp::common::AsyncIteratorSourceCore*>(source)
            ->GetResult(static_cast<CHAOS_IL2CPP_INT16>(token)));
}

// AwaitOnCompleted / AwaitUnsafeOnCompleted.
//
// NOT a second continuation mechanism.  The awaiter here is the thing the iterator
// is currently awaiting (Task.Yield, Task.Delay, ...) — an AsyncTask handle — and
// resumption must be registered on THAT object via async_task_on_completed, exactly
// as the AsyncTaskMethodBuilder path does.  Routing it to the pooled iterator source
// instead would register the continuation against the wrong object and the state
// machine would never resume.
//
// Both entry points share this body: the unsafe/on-completed distinction in .NET
// governs whether the awaiter may resume the state machine on an arbitrary thread,
// which native code does not enforce.
//
// The resume context is a one-shot heap box because the callback signature
// (task_handle, ctx) differs from the builder's (sm_box), and because the same
// state machine may be awaiting from several call sites in sequence — a static
// slot would be clobbered by a later registration before an earlier one fires.

namespace {

struct AsyncIteratorResumeCtx {
    chaos::il2cpp::common::AsyncIteratorMoveNextFn move_next;
    void*                                        sm_box;
};

void AsyncIteratorResumeTrampoline(CHAOS_IL2CPP_INTPTR /*task_handle*/, void* ctx) noexcept
{
    auto* c = static_cast<AsyncIteratorResumeCtx*>(ctx);
    if (c == nullptr) return;
    auto move_next = c->move_next;
    void* sm_box = c->sm_box;
    delete c;
    if (move_next != nullptr) move_next(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(sm_box));
}

CHAOS_IL2CPP_INTPTR AsyncIteratorAwaitOnCompleted(
    CHAOS_IL2CPP_INTPTR awaiter_handle,
    CHAOS_IL2CPP_INTPTR move_next_fn,
    CHAOS_IL2CPP_INTPTR sm_box) noexcept
{
    using namespace chaos::il2cpp::common;
    if (awaiter_handle == 0 || move_next_fn == 0) return 0;

    auto* ctx = new (std::nothrow)
        AsyncIteratorResumeCtx{reinterpret_cast<AsyncIteratorMoveNextFn>(move_next_fn),
                               reinterpret_cast<void*>(sm_box)};
    if (ctx == nullptr) CHAOS_IL2CPP_ABORT();

    // async_task_on_completed returns 0 for an invalid handle — which here means the
    // caller handed us something that is not an awaited task.  Treat that as a loud
    // failure rather than a dropped continuation: a silently dropped resumption is
    // an iterator that stops mid-sequence with no error, the exact class of failure
    // this effort exists to eliminate.
    if (async_task_on_completed(awaiter_handle, &AsyncIteratorResumeTrampoline, ctx) == 0)
    {
        delete ctx;
        CHAOS_IL2CPP_ABORT();
    }
    return 1;
}

}  // namespace

CHAOS_IL2CPP_INTPTR chaos_async_iterator_builder_await_on_completed(
    CHAOS_IL2CPP_INTPTR awaiter_handle,
    CHAOS_IL2CPP_INTPTR move_next_fn,
    CHAOS_IL2CPP_INTPTR sm_box) noexcept
{
    return AsyncIteratorAwaitOnCompleted(awaiter_handle, move_next_fn, sm_box);
}

CHAOS_IL2CPP_INTPTR chaos_async_iterator_builder_await_unsafe_on_completed(
    CHAOS_IL2CPP_INTPTR awaiter_handle,
    CHAOS_IL2CPP_INTPTR move_next_fn,
    CHAOS_IL2CPP_INTPTR sm_box) noexcept
{
    return AsyncIteratorAwaitOnCompleted(awaiter_handle, move_next_fn, sm_box);
}

void chaos_async_iterator_builder_complete(CHAOS_IL2CPP_INTPTR builder_handle) noexcept
{
    chaos::il2cpp::common::async_iterator_builder_complete(builder_handle);
}

void chaos_async_iterator_builder_destroy(CHAOS_IL2CPP_INTPTR builder_handle) noexcept
{
    // Create() heap-allocates the pool; nothing else owns it, so without this the
    // pool leaks once per iterator.  Kept separate from Complete() deliberately: a
    // state machine can be re-enumerated (GetAsyncEnumerator called twice), and
    // Complete() runs at the end of the FIRST iteration — freeing there would leave
    // the second enumeration holding a dangling builder.
    chaos::il2cpp::common::async_iterator_builder_destroy(builder_handle);
}

}  // extern "C"

// ══════════════════════════════════════════════════════════════════════════════
// Phase 4: ValueTask state queries + AsTask
// ══════════════════════════════════════════════════════════════════════════════
//
// A managed ValueTask crosses the ABI as its backing task handle (0 = completed
// void).  The four predicates delegate to the AsyncTask struct's atomic flags,
// and AsTask is the identity (the backing IS the task in this runtime).

CHAOS_IL2CPP_INT32 chaos_value_task_is_completed(CHAOS_IL2CPP_INTPTR vt_handle) noexcept
{
    if (vt_handle == 0) return 1;  // default/void -> trivially completed
    auto* task = require_async_task(vt_handle);
    if (task == nullptr) return 1;
    return task->completed.load(std::memory_order_acquire) ? 1 : 0;
}

CHAOS_IL2CPP_INT32 chaos_value_task_is_completed_successfully(CHAOS_IL2CPP_INTPTR vt_handle) noexcept
{
    if (vt_handle == 0) return 1;
    auto* task = require_async_task(vt_handle);
    if (task == nullptr) return 0;
    bool done = task->completed.load(std::memory_order_acquire);
    bool err = task->faulted.load(std::memory_order_acquire);
    return (done && !err && task->exception == 0) ? 1 : 0;
}

CHAOS_IL2CPP_INT32 chaos_value_task_is_faulted(CHAOS_IL2CPP_INTPTR vt_handle) noexcept
{
    if (vt_handle == 0) return 0;
    auto* task = require_async_task(vt_handle);
    if (task == nullptr) return 0;
    return task->faulted.load(std::memory_order_acquire) ? 1 : 0;
}

CHAOS_IL2CPP_INT32 chaos_value_task_is_canceled(CHAOS_IL2CPP_INTPTR vt_handle) noexcept
{
    if (vt_handle == 0) return 0;
    auto* task = require_async_task(vt_handle);
    if (task == nullptr) return 0;
    bool err = task->faulted.load(std::memory_order_acquire);
    return (err && task->exception == 0) ? 1 : 0;
}

CHAOS_IL2CPP_INTPTR chaos_value_task_as_task(CHAOS_IL2CPP_INTPTR vt_handle) noexcept
{
    return vt_handle;
}

CHAOS_IL2CPP_INTPTR chaos_value_task_get_awaiter(CHAOS_IL2CPP_INTPTR vt_handle) noexcept
{
    return vt_handle;
}
