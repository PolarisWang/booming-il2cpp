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
void ChaosAsyncTaskAwaiterGetResultVoid(CHAOS_IL2CPP_INTPTR awaiter) noexcept;

// ── Task.Delay / TaskAwaiter await path (non-generic Task) ──
// Without these, Task.Delay / Task.GetAwaiter / TaskAwaiter.get_IsCompleted
// fell through to ChaosExternalRuntimeFallback → 0, so `await Task.Delay(n)`
// suspended forever: GetAwaiter returned 0, IsCompleted read false, and
// AwaitUnsafeOnCompleted saw task_handle==0 → no continuation registered.
//
// NOTE: these are the await-path proxies over the real AsyncTask handle, NOT
// constant-return stubs.  ChaosAsyncTaskAwaiterGetIsCompleted reflects the
// task's actual completion state and ChaosAsyncTaskAwaiterGetResultValue
// returns the real result payload — a constant 1/0 here would silently defeat
// suspension and corrupt every `await Task<T>` result.
CHAOS_IL2CPP_INTPTR ChaosAsyncTaskDelay(CHAOS_IL2CPP_INT32 millisecondsDelay) noexcept;
CHAOS_IL2CPP_INTPTR ChaosAsyncTaskGetAwaiter(CHAOS_IL2CPP_INTPTR task_handle) noexcept;
CHAOS_IL2CPP_INT32 ChaosAsyncTaskAwaiterGetIsCompleted(CHAOS_IL2CPP_INTPTR awaiter_ref) noexcept;
CHAOS_IL2CPP_INTPTR ChaosAsyncTaskAwaiterGetResultValue(CHAOS_IL2CPP_INTPTR awaiter) noexcept;

// ── Task.Wait / Task<T>.Result (blocking semantics) ──
// Block the calling thread until the task completes, propagating faults the
// same way `await` does.  ChaosAsyncTaskWait returns 1 on completion, 0 on
// timeout (timeout_ms < 0 means wait indefinitely).
CHAOS_IL2CPP_INT32 ChaosAsyncTaskGetIsCompleted(CHAOS_IL2CPP_INTPTR task_handle) noexcept;
CHAOS_IL2CPP_INT32 ChaosAsyncTaskWait(CHAOS_IL2CPP_INTPTR task_handle, CHAOS_IL2CPP_INT32 timeout_ms) noexcept;
CHAOS_IL2CPP_INTPTR ChaosAsyncTaskGetResultBlocking(CHAOS_IL2CPP_INTPTR task_handle) noexcept;

// ── Three-state query (resolved / faulted / cancelled) ──
// These are mutually exclusive for a completed task: exactly one of
// IsCanceled / IsFaulted is set when IsCompleted is true, and neither is set
// for a successful completion.  Cancellation is never inferred from a null
// exception payload — it is tracked as its own flag on AsyncTask.
CHAOS_IL2CPP_INT32 ChaosAsyncTaskGetIsFaulted(CHAOS_IL2CPP_INTPTR task_handle) noexcept;
CHAOS_IL2CPP_INT32 ChaosAsyncTaskGetIsCanceled(CHAOS_IL2CPP_INTPTR task_handle) noexcept;

// ── TaskCompletionSource<T> native helpers (Phase 3 P3-1) ──
// Each takes a TCS object handle as the first argument.
// Implemented in async_stubs.cpp via the async.h TaskSource proxy.
CHAOS_IL2CPP_INTPTR chaos_task_completion_source_create(void) noexcept;
CHAOS_IL2CPP_INTPTR chaos_tcs_get_task(CHAOS_IL2CPP_INTPTR tcs_handle) noexcept;
void chaos_tcs_set_result(CHAOS_IL2CPP_INTPTR tcs_handle, CHAOS_IL2CPP_INTPTR value) noexcept;
void chaos_tcs_set_exception(CHAOS_IL2CPP_INTPTR tcs_handle, CHAOS_IL2CPP_INTPTR exception) noexcept;
void chaos_tcs_set_canceled(CHAOS_IL2CPP_INTPTR tcs_handle) noexcept;
CHAOS_IL2CPP_INTPTR chaos_tcs_try_set_result(CHAOS_IL2CPP_INTPTR tcs_handle, CHAOS_IL2CPP_INTPTR value) noexcept;
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

// ── Task.WhenEach (Phase 2 / ASYNC-P2-8) ──
// ORDER-PRESERVING completion stream.  Unlike WhenAll (awaits everything, then
// yields the aggregate once) and WhenAny (yields the first completed task once),
// WhenEach yields EACH task as it completes, so a consumer awaiting the returned
// IAsyncEnumerable observes completion ORDER.
//
// Returns a handle to a queue-backed completion source that is re-armed after each
// element is consumed.  Every task in the array is registered with a completion
// continuation at call time, so no completion can be lost between caller drains.
// Returns 0 on invalid arguments or allocation failure.
CHAOS_IL2CPP_INTPTR chaos_task_when_each_array(CHAOS_IL2CPP_INTPTR tasks_handle) noexcept;

// Consumer side of the WhenEach stream.  A caller drains it element by element:
// while when_each_may_have_next() is non-zero, when_each_try_dequeue() returns the
// next completed task in completion order (0 if the queue is momentarily empty —
// await, don't conclude the stream ended).  when_each_destroy frees the stream.
CHAOS_IL2CPP_INT32 chaos_task_when_each_may_have_next(CHAOS_IL2CPP_INTPTR stream) noexcept;
CHAOS_IL2CPP_INTPTR chaos_task_when_each_try_dequeue(CHAOS_IL2CPP_INTPTR stream) noexcept;
void chaos_task_when_each_destroy(CHAOS_IL2CPP_INTPTR stream) noexcept;

// ── Task.ContinueWith native combinator (Phase 2 P2-2) ──
// Registers `continuation` (a DelegateObject taking the antecedent task handle
// and returning a native int) to run when `antecedent` completes — whether it
// has already completed, faults, or is cancelled.  Returns a NEW task handle
// that resolves with the continuation's return value (0 on invalid args).
//
// The continuation is unconditional: it runs for a faulted or cancelled
// antecedent too, so a cleanup/logging continuation is never silently dropped.
CHAOS_IL2CPP_INTPTR chaos_task_continue_with(
    CHAOS_IL2CPP_INTPTR antecedent, CHAOS_IL2CPP_INTPTR continuation) noexcept;

// ── TaskFactory native surface (Phase 2 P2-5) ──
// Task.get_Factory.  The runtime has no TaskFactory object model and does not
// need one: the default factory's StartNew queues on the default scheduler,
// which is what chaos_task_factory_start_new does directly.  Returns a non-null
// opaque token so a caller that stores or passes Task.Factory does not receive
// a bogus 0 handle; the token is never dereferenced.
CHAOS_IL2CPP_INTPTR chaos_task_default_factory() noexcept;

// TaskFactory::StartNew(Action) — the delegate-only overloads.  Semantically
// identical to Task.Run(Action) for the default factory, so this queues the
// delegate on the same ThreadPool runner and returns the task handle (0 on
// invalid args / scheduling failure).
//
// The `factory` handle is accepted for arity only and deliberately unused; see
// the note on chaos_task_default_factory.
CHAOS_IL2CPP_INTPTR chaos_task_factory_start_new(
    CHAOS_IL2CPP_INTPTR factory, CHAOS_IL2CPP_INTPTR delegate_fn) noexcept;

// ── AsyncIteratorMethodBuilder native surface (ASYNC-P2-8 A2) ──
// The 5 ops the `async IAsyncEnumerable<T>` / `async IAsyncEnumerator<T>` state
// machine calls, plus the source-pool accessors codegen needs.  Implemented in
// async_stubs.cpp over chaos/async_iterator.h.
//
// These exist so the codegen ShapeRegistry can route AsyncIteratorMethodBuilder
// calls to real semantics instead of degrading to an external-runtime stub.  A1
// (kUnsupportedAsyncIterator*) detects the shape and refuses to emit silently;
// A2 supplies the runtime it was refusing to fake.
//
// AwaitOnCompleted and AwaitUnsafeOnCompleted are BOTH declared — the original
// plan registered only the former, which is why a `await Task.Yield()` inside an
// iterator would still have fallen through.
//
// The builder handle is a source-pool pointer; `sm_box` is the iterator state
// machine box and `move_next` its native MoveNext.
CHAOS_IL2CPP_INTPTR chaos_async_iterator_builder_create(void) noexcept;
CHAOS_IL2CPP_INTPTR chaos_async_iterator_builder_move_next(
    CHAOS_IL2CPP_INTPTR builder_handle,
    CHAOS_IL2CPP_INTPTR move_next_fn,
    CHAOS_IL2CPP_INTPTR sm_box) noexcept;

// Acquire a pooled source for one MoveNextAsync; the iteration token is written
// through out_token (see below for why it is split out rather than a second return).
// Never returns 0 and never blocks (overflow allocates) — see chaos/async_iterator.h
// for why both properties are contractual.
CHAOS_IL2CPP_INTPTR chaos_async_iterator_source_acquire(
    CHAOS_IL2CPP_INTPTR builder_handle, CHAOS_IL2CPP_INT32* out_token) noexcept;
void chaos_async_iterator_source_release(
    CHAOS_IL2CPP_INTPTR builder_handle, CHAOS_IL2CPP_INTPTR source) noexcept;

// Publish one iteration step's outcome (true = element available, false = done).
void chaos_async_iterator_source_set_result(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INT32 value) noexcept;
void chaos_async_iterator_source_set_exception(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR exception) noexcept;
CHAOS_IL2CPP_INT32 chaos_async_iterator_source_get_status(
    CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INT32 token) noexcept;
CHAOS_IL2CPP_INT32 chaos_async_iterator_source_get_result(
    CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INT32 token) noexcept;

// AwaitOnCompleted / AwaitUnsafeOnCompleted — one native implementation, two
// entry points (native code does not enforce the unsafe/on-completed split).
//
// NOTE ON THE HANDLE.  The await path does NOT take the source handle + token: the
// awaiter really being awaited is `<>v__promiseOfValueOrEnd`'s awaited task (e.g. a
// Task.Yield inside the iterator), not the iterator's own pooled source.  The awaiter
// is therefore an opaque AsyncTask handle, and resumption is registered on it through
// the existing async.h machinery.  Passing a pooled source here instead would register
// the continuation on the wrong object and the state machine would never resume.
CHAOS_IL2CPP_INTPTR chaos_async_iterator_builder_await_on_completed(
    CHAOS_IL2CPP_INTPTR awaiter_handle,
    CHAOS_IL2CPP_INTPTR move_next_fn,
    CHAOS_IL2CPP_INTPTR sm_box) noexcept;

CHAOS_IL2CPP_INTPTR chaos_async_iterator_builder_await_unsafe_on_completed(
    CHAOS_IL2CPP_INTPTR awaiter_handle,
    CHAOS_IL2CPP_INTPTR move_next_fn,
    CHAOS_IL2CPP_INTPTR sm_box) noexcept;

void chaos_async_iterator_builder_complete(CHAOS_IL2CPP_INTPTR builder_handle) noexcept;

// Pairs Create().  Complete() runs at the end of each iteration and deliberately does
// NOT free the builder — a state machine can be enumerated twice.  Destroy() is the
// single release point, called when the state machine's own lifetime ends.
void chaos_async_iterator_builder_destroy(CHAOS_IL2CPP_INTPTR builder_handle) noexcept;

}  // extern "C"
