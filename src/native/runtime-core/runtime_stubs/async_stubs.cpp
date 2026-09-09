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
#include "timer_queue.h"

#include <cstdint>
#include <new>

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

// Shared completion state across the N child continuations.
struct WhenState {
    chaos::il2cpp::common::AsyncTask*    aggregate;
    CHAOS_IL2CPP_INTPTR                  aggregate_handle;
    std::atomic<int>                     remaining;
    bool                                 mode_when_all;      // true=WhenAll
    std::atomic<bool>                    won;                // WhenAny single-fire
    CHAOS_IL2CPP_INTPTR*                 children;           // child handles array
    int                                  n;                  // child count
};

// Delivered when a child completes; task_handle = the completing child.
void WhenChildContinuation(CHAOS_IL2CPP_INTPTR task_handle, void* ctx) noexcept {
    using namespace chaos::il2cpp::common;
    auto* st = static_cast<WhenState*>(ctx);

    if (st->mode_when_all) {
        int rem = st->remaining.fetch_sub(1, std::memory_order_acq_rel) - 1;
        if (rem == 0) {
            // All children done: fault iff any faulted.
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
            st->aggregate->completed.store(true, std::memory_order_release);
            finish_async_task(st->aggregate_handle);
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
    finish_async_task(st->aggregate_handle);
    delete st;
}

} // anonymous namespace

/// Shared internal: build aggregate + register a continuation on every child.
static CHAOS_IL2CPP_INTPTR WhenAllAnyInternal(
    CHAOS_IL2CPP_INTPTR* children, CHAOS_IL2CPP_INT32 n, bool when_all) noexcept
{
    using namespace chaos::il2cpp::common;
    if (n < 0) return 0;
    if (children == nullptr && n > 0) return 0;
    auto* agg = new (std::nothrow) AsyncTask();
    if (agg == nullptr) return 0;
    auto* st = new (std::nothrow) WhenState();
    if (st == nullptr) { delete agg; return 0; }
    CHAOS_IL2CPP_INTPTR agg_handle = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(agg);
    st->aggregate = agg;
    st->aggregate_handle = agg_handle;
    st->remaining.store(n, std::memory_order_relaxed);
    st->mode_when_all = when_all;
    st->won.store(false, std::memory_order_relaxed);
    st->children = children;
    st->n = n;

    if (n == 0) {
        // Empty WhenAll completes immediately (empty WhenAny is invalid → caller
        // guards; keep symmetric: complete immediately, result undefined).
        agg->completed.store(true, std::memory_order_release);
        finish_async_task(agg_handle);
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

}  // extern "C"
