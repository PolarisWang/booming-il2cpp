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
// INTPTR, not void: the managed YieldAwaiter.GetResult() is void, but the
// generated shape dispatch forwards every helper through
// reinterpret_cast<CHAOS_IL2CPP_INTPTR>(...), which is ill-formed over a void
// expression.  Same ABI constraint as ChaosAsyncTaskAwaiterGetResultVoid below.
CHAOS_IL2CPP_INTPTR chaos_async_yield_get_result(CHAOS_IL2CPP_INTPTR yield_awaiter) noexcept;

// ── Hot BCL no-ops ──
// IDisposable::Dispose() lowered to a native no-op (the GC owns the lifetime,
// not managed Dispose). Registered as ShapeKind.SimpleForward with the native
// symbol `chaos_noop_void`, so generated code calls this identifier DIRECTLY
// and unqualified. The definition lives in async_stubs.cpp; without a
// declaration reachable from the generated TU it fails to compile with
// C3861: 'chaos_noop_void': identifier not found.
extern "C" void chaos_noop_void(CHAOS_IL2CPP_INTPTR dispose_target) noexcept;
extern "C" void chaos_noop_void3(CHAOS_IL2CPP_INTPTR a, CHAOS_IL2CPP_INTPTR b, CHAOS_IL2CPP_INTPTR c) noexcept;

// TaskAwaiter stubs (DirectNativeSymbol for async state machine dispatch).
// Generated code calls these when lowering async Task.GetResult() patterns.
//
// ABI note: both RETURN CHAOS_IL2CPP_INTPTR even though the managed members are
// `void TaskAwaiter.GetResult()`.  The generated shape dispatch in
// runtime_helper_shapes.h wraps every helper call as
//     return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(<symbol>(args...));
// so a `void`-returning symbol is ill-formed there (reinterpret_cast from void)
// — and the caller site further wraps that in `const auto chaos_result = ...`,
// which would deduce `const void` (C3313/C3536).  The slot type recorded in
// ShapeRegistry cannot help: it types the *wrapper*, not this declaration.
// Every other helper in this header already returns INTPTR for this reason.
// The implementation returns 0; nothing consumes it (fault/cancel propagation
// happens via RaiseManagedException inside).
CHAOS_IL2CPP_INTPTR ChaosAsyncAwaiterGetResult(CHAOS_IL2CPP_INTPTR awaiter) noexcept;
CHAOS_IL2CPP_INTPTR ChaosAsyncTaskAwaiterGetResultVoid(CHAOS_IL2CPP_INTPTR awaiter) noexcept;

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

/// `Task.Wait()` — the parameterless overload, i.e. an INFINITE wait.
///
/// This exists as its own symbol (rather than registering the 2-arg
/// ChaosAsyncTaskWait with a -1 baked into an inline body) because a
/// GenericShapeResolution carrying `DirectNativeSymbol` makes codegen emit a
/// DIRECT call to that symbol with the call site's own arguments — the inline
/// body's `-1` is discarded.  The result was `ChaosAsyncTaskWait(handle)`
/// against a 2-parameter definition (C2660).
///
/// Registering the THUNK as the direct symbol keeps the ABI honest: one slot
/// in, one slot out.
CHAOS_IL2CPP_INT32 ChaosAsyncTaskWaitInfinite(CHAOS_IL2CPP_INTPTR task_handle) noexcept;

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

// Non-generic TaskCompletionSource overloads whose managed signature carries no
// value argument.  Kept separate from the 2-arg forms above because the ABI slot
// for `value` is never populated on those call sites.
void chaos_tcs_set_result_void(CHAOS_IL2CPP_INTPTR tcs_handle) noexcept;
void chaos_tcs_set_canceled(CHAOS_IL2CPP_INTPTR tcs_handle) noexcept;
CHAOS_IL2CPP_INTPTR chaos_tcs_try_set_result_void(CHAOS_IL2CPP_INTPTR tcs_handle) noexcept;

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

/// Task.WhenAny(Task, Task) — two-task specialization; builds a 2-element
/// stack array and delegates to the shared combinator (no ownership transfer).
CHAOS_IL2CPP_INTPTR chaos_task_when_any2(CHAOS_IL2CPP_INTPTR a, CHAOS_IL2CPP_INTPTR b) noexcept;

// ── T3: Task.WaitAll / Task.WaitAny (BLOCKING semantics) ──
//
// Unlike WhenAll/WhenAny (which return an aggregate Task and never block), these
// BLOCK the calling thread until ALL / ANY of the given tasks complete.  The
// managed signatures are static: they take the Task[] array, and optionally a
// timeout (int ms or TimeSpan) and/or a CancellationToken.
//
// ABI (static, receiver-less): the ONLY managed argument the generated shim
// carries is the Task[] array handle.  Timeout/token overloads are accepted for
// arity and left unhonoured (no blocking wait timeout / cancellation plumbing at
// these call sites — same boundary as Task.Wait(token)).
//
// Returns:
//   WaitAll → 1 (waited for all; the caller observes child results), 0 = error
//   WaitAny → 0-based index of the first completed task, -1 = error
// Both return immediately on a null/invalid array.
CHAOS_IL2CPP_INT32 chaos_task_wait_all(CHAOS_IL2CPP_INTPTR tasks_handle) noexcept;
CHAOS_IL2CPP_INT32 chaos_task_wait_any(CHAOS_IL2CPP_INTPTR tasks_handle) noexcept;

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

// ── Phase 4: ValueTask state queries + helpers (ASYNC-P2-8) ──
// A managed ValueTask crosses the ABI as its backing Task handle.
// The four predicates must stay distinct: a faulted ValueTask is
// (completed, !completed_successfully, faulted, !canceled).
CHAOS_IL2CPP_INT32 chaos_value_task_is_completed(CHAOS_IL2CPP_INTPTR vt_handle) noexcept;
CHAOS_IL2CPP_INT32 chaos_value_task_is_completed_successfully(CHAOS_IL2CPP_INTPTR vt_handle) noexcept;
CHAOS_IL2CPP_INT32 chaos_value_task_is_faulted(CHAOS_IL2CPP_INTPTR vt_handle) noexcept;
CHAOS_IL2CPP_INT32 chaos_value_task_is_canceled(CHAOS_IL2CPP_INTPTR vt_handle) noexcept;
CHAOS_IL2CPP_INTPTR chaos_value_task_as_task(CHAOS_IL2CPP_INTPTR vt_handle) noexcept;
CHAOS_IL2CPP_INTPTR chaos_value_task_get_awaiter(CHAOS_IL2CPP_INTPTR vt_handle) noexcept;

// Phase 6: GC-allocated AsyncTask.  Switches allocation from plain `new`
// to CHAOS_IL2CPP_NEW_GC so the GC roots the task and a live continuation
// prevents collection.  Used by concrete paths (async_stubs.cpp, task_runner)
// that link the GC allocator.  The header-only inline (async.h:189) keeps
// plain `new` for standalone TU compatibility.
CHAOS_IL2CPP_INTPTR async_task_create_gc() noexcept;

// TaskExtensions.Unwrap — flatten Task<Task<T>> to Task<T>.  The outer task's
// result is the inner handle, so this is the identity once the outer completed;
// the entry point exists so the call does not fall through to a numeric
// fallback that would replace a real inner task with a null handle.
CHAOS_IL2CPP_INTPTR chaos_task_unwrap(CHAOS_IL2CPP_INTPTR outer) noexcept;

// ── Phase 6 / E2: Parallel.For range partitioner ──
// Partitions [from, to) across the ThreadPool and invokes the Action<int>
// delegate's native method_ptr directly.  Returns -1
// (ParallelLoopResult.lowestBreakIteration for a completed-without-break run).
//
// Declared HERE because the generated TU includes async_stubs.h and calls this
// identifier DIRECTLY (DirectNativeSymbol); without a reachable declaration the
// generated code fails with C3861 'chaos_parallel_for_range_int': identifier
// not found.  Same requirement class as chaos_noop_void above.
CHAOS_IL2CPP_INTPTR chaos_parallel_for_range_int(
    CHAOS_IL2CPP_INT32 from, CHAOS_IL2CPP_INT32 to,
    CHAOS_IL2CPP_INTPTR action_delegate) noexcept;

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

// ── TaskFactory.ContinueWhenAll / ContinueWhenAny (Phase 2) ──
// Compose the existing aggregate combinators with the ContinueWith delivery
// path: build the WhenAll/WhenAny aggregate over `tasks_handle` (a managed
// Task[] handle) and register `continuation` on it.  The continuation observes
// the AGGREGATE, so for ContinueWhenAll it runs once every task finished, and
// for ContinueWhenAny once the first finished — the aggregate's own result
// (the [1,2,3]-style array, or the 1-based winner index) is what it receives.
//
// Returns a NEW task handle resolving with the continuation's result, or 0 on
// invalid arguments.
CHAOS_IL2CPP_INTPTR chaos_task_continue_when_all_array(
    CHAOS_IL2CPP_INTPTR tasks_handle, CHAOS_IL2CPP_INTPTR continuation) noexcept;
CHAOS_IL2CPP_INTPTR chaos_task_continue_when_any_array(
    CHAOS_IL2CPP_INTPTR tasks_handle, CHAOS_IL2CPP_INTPTR continuation) noexcept;

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

// ── T2: Task.Run / TaskFactory::StartNew(delegate, CancellationToken) ──
//
// Task.Run(Action, CancellationToken) and the generic Func overloads are
// STATIC entry points (no receiver), so the ABI is (delegate, token).
//
// The token is accepted but NOT honoured: there is no cancellation plumbing at
// these call sites, the same boundary recorded for Task.Wait(token) and the
// MRES token overloads.  Queueing the delegate is the behaviour these calls had
// before (through the fallback they returned 0, i.e. NO task at all), so wiring
// them is strictly an improvement — a cancelled token simply does not stop the
// work, which is a documented limitation rather than a silent regression.
CHAOS_IL2CPP_INTPTR chaos_task_run(
    CHAOS_IL2CPP_INTPTR delegate_fn, CHAOS_IL2CPP_INTPTR token) noexcept;

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

// ══════════════════════════════════════════════════════════════════════
// T1: Task.Wait(CancellationToken) / ConfigureAwait
// ══════════════════════════════════════════════════════════════════════

/// Task.Wait(CancellationToken) — infinite wait with token.
CHAOS_IL2CPP_INT32 ChaosAsyncTaskWaitToken(CHAOS_IL2CPP_INTPTR task_handle,
                                            CHAOS_IL2CPP_INTPTR token) noexcept;

/// Task.Wait(TimeSpan) / Task.Wait(TimeSpan, CancellationToken) —
/// blocking wait with TimeSpan timeout.
CHAOS_IL2CPP_INT32 ChaosAsyncTaskWaitTimeSpan(CHAOS_IL2CPP_INTPTR task_handle,
                                              CHAOS_IL2CPP_INTPTR timeout_span,
                                              CHAOS_IL2CPP_INTPTR token) noexcept;

/// Task.ConfigureAwait(bool) / Task<T>.ConfigureAwait(bool)
/// Returns the ConfiguredTaskAwaitable as an INTPTR carrier pointer.
CHAOS_IL2CPP_INTPTR ChaosAsyncTaskConfigureAwait(CHAOS_IL2CPP_INTPTR task_handle,
                                                 CHAOS_IL2CPP_INT32 continue_on_captured_context) noexcept;

/// Task.ConfigureAwait(ConfigureAwaitOptions) — flags-based overload.
CHAOS_IL2CPP_INTPTR ChaosAsyncTaskConfigureAwaitOptions(
    CHAOS_IL2CPP_INTPTR task_handle,
    CHAOS_IL2CPP_INT32 options) noexcept;

/// ConfiguredTaskAwaitable.GetAwaiter() / ConfiguredTaskAwaitable<T>.GetAwaiter()
/// Reads the task handle and returns it as the awaiter reference.
CHAOS_IL2CPP_INTPTR ChaosConfiguredTaskAwaitableGetAwaiter(
    CHAOS_IL2CPP_INTPTR awaitable_carrier) noexcept;

/// ConfiguredTaskAwaitable<T>.GetResult() — returns the result value.
CHAOS_IL2CPP_INTPTR ChaosConfiguredTaskAwaitableGetResult(
    CHAOS_IL2CPP_INTPTR awaiter) noexcept;

/// Task<T>.ConfigureAwait(bool) / ConfigureAwait(ConfigureAwaitOptions)
/// Returns the ConfiguredTaskAwaitable<T> carrier pointer.
CHAOS_IL2CPP_INTPTR ChaosAsyncTaskOfTConfigureAwait(
    CHAOS_IL2CPP_INTPTR task_handle,
    CHAOS_IL2CPP_INT32 continue_on_captured_context) noexcept;

/// ValueTask.ConfigureAwait(bool) / ValueTask<T>.ConfigureAwait(bool)
CHAOS_IL2CPP_INTPTR ChaosValueTaskConfigureAwait(
    CHAOS_IL2CPP_INTPTR value_task_ptr,
    CHAOS_IL2CPP_INT32 continue_on_captured_context) noexcept;

/// ConfiguredValueTaskAwaitable.GetAwaiter()
CHAOS_IL2CPP_INTPTR ChaosConfiguredValueTaskAwaitableGetAwaiter(
    CHAOS_IL2CPP_INTPTR awaitable_carrier) noexcept;

/// ConfiguredValueTaskAwaitable.get_IsCompleted()
CHAOS_IL2CPP_INT32 ChaosConfiguredValueTaskAwaitableGetIsCompleted(
    CHAOS_IL2CPP_INTPTR awaiter) noexcept;

/// ConfiguredValueTaskAwaitable.GetResult() (void)
void ChaosConfiguredValueTaskAwaitableGetResultVoid(
    CHAOS_IL2CPP_INTPTR awaiter) noexcept;

/// ConfiguredValueTaskAwaitable<T>.GetResult() -> T
CHAOS_IL2CPP_INTPTR ChaosConfiguredValueTaskAwaitableGetResultValue(
    CHAOS_IL2CPP_INTPTR awaiter) noexcept;

// ══════════════════════════════════════════════════════════════════════
// Page-file symbol declarations (C3861 guard for multi-TU builds)
// ══════════════════════════════════════════════════════════════════════

/// The non-generic TaskAwaiter.GetResult(void) — the symbol page files
/// call directly through the external-runtime dispatch table when the
/// method is split into a separate translation unit.
extern "C" CHAOS_IL2CPP_INTPTR
    chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_TaskAwaiter__GetResult_System_Void__() noexcept;

/// Task.GetAwaiter() → TaskAwaiter — page files call this through the
/// external-runtime symbol when the method is in a different TU.
extern "C" CHAOS_IL2CPP_INTPTR
    chaos_external_runtime_System_Private_CoreLib_System_Threading_Tasks_Task__GetAwaiter_System_Runtime_CompilerServices_TaskAwaiter__() noexcept;

/// Task<T>.GetAwaiter() → TaskAwaiter<T> — generic variant.
extern "C" CHAOS_IL2CPP_INTPTR
    chaos_external_runtime_System_Private_CoreLib_System_Threading_Tasks_Task_System_Int32___GetAwaiter_System_Runtime_CompilerServices_TaskAwaiter_System_Int32___() noexcept;

/// TaskAwaiter<T>.GetResult() -> T — generic awaiter result.
extern "C" CHAOS_IL2CPP_INTPTR
    chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_TaskAwaiter_System_Int32___GetResult_System_Int32__() noexcept;

/// ValueTaskAwaiter.GetResult(void) — non-generic void overload called from
/// page-file external-runtime dispatch.
extern "C" CHAOS_IL2CPP_INTPTR
    chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_ValueTaskAwaiter__GetResult_System_Void__() noexcept;

}  // extern "C"
