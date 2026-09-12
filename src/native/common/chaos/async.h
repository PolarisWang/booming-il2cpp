#ifndef CHAOS_IL2CPP_COMMON_ASYNC_H_
#define CHAOS_IL2CPP_COMMON_ASYNC_H_

#include "ptr_tag.h"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <cstdlib>
#include <mutex>

namespace chaos::il2cpp::common {

// Function pointer type for the async_task_run implementation.
// runtime_core registers its ThreadPool-backed TaskRun during RuntimeInit.
// If unregistered (e.g., verification mode), async_task_run returns 0.
using AsyncTaskRunFn = CHAOS_IL2CPP_INTPTR (*)(CHAOS_IL2CPP_INTPTR);

// Global function pointer; defaults to nullptr (stub behavior).
// Set by runtime_core::threading::RegisterAsyncTaskRun during runtime init.
inline AsyncTaskRunFn g_async_task_run_fn = nullptr;

// Called by runtime_core during RuntimeInit to inject the real TaskRun.
inline void register_async_task_run_fn(AsyncTaskRunFn fn) noexcept {
    g_async_task_run_fn = fn;
}

struct AsyncTask;

// Continuation callback = state machine resumption contract.  When a Task
// completes (result or exception), exactly one registered continuation is
// invoked so an awaiting state machine's MoveNext can re-enter.
using AsyncContinueFn = void (*)(CHAOS_IL2CPP_INTPTR task_handle, void* ctx);

// Optional dispatcher: runs a continuation off the completing thread (e.g.
// thread pool).  Defaults to nullptr → continuations fire synchronously on
// the completing thread (semantically correct, just not thread-pool queued).
using AsyncDispatchContinuationFn = void (*)(AsyncContinueFn cb, void* ctx, CHAOS_IL2CPP_INTPTR task_handle);
inline AsyncDispatchContinuationFn g_async_dispatch_continuation_fn = nullptr;
inline void register_async_dispatch_continuation_fn(AsyncDispatchContinuationFn fn) noexcept {
    g_async_dispatch_continuation_fn = fn;
}

struct AsyncTask
{
    // Result/exception payload set by completion setters.
    CHAOS_IL2CPP_INTPTR result = 0;
    CHAOS_IL2CPP_INTPTR exception = 0;

    // Completion flags.  Atomic so a completing worker thread can publish
    // completion that an awaiting thread observes safely.  Implicit conversion
    // keeps existing plain `task->completed` reads/writes source-compatible.
    std::atomic<bool> completed{false};
    std::atomic<bool> faulted{false};

    // Cancellation is a THIRD terminal state, tracked separately from `faulted`.
    // It must not be inferred from (faulted && exception == 0): a legitimate
    // `Task.FromException(null)` or a fault carrying no payload would then be
    // misreported to the awaiter as a TaskCanceledException, and a genuinely
    // cancelled task would surface as a generic fault whenever the payload
    // happened to be non-null.  Mutually exclusive with a plain fault: a task
    // that is cancelled has canceled == true and faulted == false.
    std::atomic<bool> canceled{false};

    // Continuation (single-slot box): registered by AwaitUnsafeOnCompleted.
    // fired exactly once on first completion.
    AsyncContinueFn   continuation_cb = nullptr;
    void*             continuation_ctx = nullptr;
    std::atomic<bool> has_continuation{false};
    std::atomic<bool> continuation_fired{false};

    // ── Blocking-wait park/unpark ────────────────────────────────────────────
    // A thread blocked in ChaosAsyncTaskWait parks here instead of spinning.
    // The waiter holds wait_mtx, re-checks `completed`, and only then waits on
    // wait_cv — so completion published between the check and the wait cannot
    // be missed (the completer must take the same mutex to signal).
    //
    // Spinning instead is not merely wasteful: on a machine with fewer free
    // cores than blocked waiters it starves the worker thread the waited-for
    // task needs, turning a busy-wait into a livelock that presents as a hang.
    CHAOS_IL2CPP_MUTEX              wait_mtx;
    CHAOS_IL2CPP_CONDITION_VARIABLE wait_cv;
};

inline AsyncTask* require_async_task(CHAOS_IL2CPP_INTPTR handle)
{
    if (handle == static_cast<CHAOS_IL2CPP_INTPTR>(0))
    {
        CHAOS_IL2CPP_ABORT();
    }
    return reinterpret_cast<AsyncTask*>(handle);
}

/// Wake any thread parked in ChaosAsyncTaskWait on this task.  Must be called
/// AFTER `completed` is published, so a waiter that re-checks under wait_mtx
/// either sees the flag or is already parked and will be woken here.
///
/// Cheap when nobody is waiting (one uncontended mutex acquire), so it is safe
/// to call unconditionally on every completion path.
inline void notify_task_completed(AsyncTask* task) noexcept
{
    std::lock_guard<CHAOS_IL2CPP_MUTEX> guard(task->wait_mtx);
    task->wait_cv.notify_all();
}

/// Park the calling thread until this task completes, or until `deadline`
/// passes (nullptr = wait forever).  Returns true if the task completed.
///
/// Note this waits on the condition variable rather than spinning: see the
/// wait_mtx/wait_cv comment on AsyncTask.
inline bool park_until_completed(AsyncTask* task,
                                 const std::chrono::steady_clock::time_point* deadline)
{
    std::unique_lock<CHAOS_IL2CPP_MUTEX> lock(task->wait_mtx);
    if (deadline == nullptr) {
        task->wait_cv.wait(lock, [task] {
            return task->completed.load(std::memory_order_acquire);
        });
        return true;
    }
    return task->wait_cv.wait_until(lock, *deadline, [task] {
        return task->completed.load(std::memory_order_acquire);
    });
}

/// Internal: deliver the registered continuation (box resumption) exactly once
/// after the task completes.  Uses the global dispatcher when present, else
/// invokes inline on the completing thread.
///
/// Ordering: checks has_continuation BEFORE claiming continuation_fired, so
/// that if no continuation is registered yet, the registering thread's re-check
/// path (async_task_on_completed) will be the one to deliver.  This prevents
/// the lost-wakeup race: a completing thread claiming continuation_fired before
/// has_continuation is set, then returning without firing, while the registerer
/// later sees continuation_fired already true and also skips.
inline void finish_async_task(CHAOS_IL2CPP_INTPTR handle) noexcept
{
    auto* task = require_async_task(handle);

    // Check if a continuation is registered before claiming the fired flag.
    // If no continuation is registered yet, the registering thread's re-check
    // path (async_task_on_completed) will finish the delivery.
    if (!task->has_continuation.load(std::memory_order_acquire)) return;

    // A continuation is registered.  Claim the exactly-once fired flag.
    if (task->continuation_fired.exchange(true, std::memory_order_acq_rel)) return;

    // has_continuation (acquire) guarantees visibility of cb/ctx writes.
    // The non-atomic reads below are safe because both has_continuation (acquire)
    // and continuation_fired (exchange with acq_rel) order the cb/ctx stores.
    AsyncContinueFn cb = task->continuation_cb;
    void* ctx = task->continuation_ctx;
    if (cb == nullptr) return;
    if (g_async_dispatch_continuation_fn != nullptr) {
        g_async_dispatch_continuation_fn(cb, ctx, handle);
    } else {
        cb(handle, ctx);
    }
}

/// Register a continuation (state-machine MoveNext resumption).  If the task is
/// already completed the continuation is delivered immediately, otherwise it is
/// stored and fired once on completion.  Returns 1 on success, 0 on invalid args.
inline CHAOS_IL2CPP_INTPTR async_task_on_completed(
    CHAOS_IL2CPP_INTPTR handle, AsyncContinueFn cb, void* ctx) noexcept
{
    if (handle == static_cast<CHAOS_IL2CPP_INTPTR>(0) || cb == nullptr) return 0;
    auto* task = require_async_task(handle);

    // Fast path: already completed before any continuation registered.
    if (task->completed.load(std::memory_order_acquire)) {
        cb(handle, ctx);
        return 1;
    }

    task->continuation_cb = cb;
    task->continuation_ctx = ctx;
    task->has_continuation.store(true, std::memory_order_release);

    // Re-check after publishing: the completing thread may have finished between
    // the first completed check and the store above.
    if (task->completed.load(std::memory_order_acquire)) {
        finish_async_task(handle);
    }
    return 1;
}

inline CHAOS_IL2CPP_INTPTR async_task_create()
{
    // Allocation: plain new.  GC-ownership of AsyncTask (so a live continuation
    // can keep the Task rooted and unhungry) is deferred until the translator
    // defines the full lifetime contract (Phase 2); this header is included by
    // standalone subject/fixture TUs that may not link the GC allocator macros.
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(CHAOS_IL2CPP_NEW(AsyncTask){});
}

/// GC-allocated variant — declared here, defined in async_stubs.cpp.
/// Switches the allocation from plain `new` to CHAOS_IL2CPP_NEW_GC so the
/// AsyncTask is rooted by the GC and a live continuation keeps it alive.
/* extern CHAOS_IL2CPP_INTPTR async_task_create_gc() noexcept; — see async_stubs.h */

inline CHAOS_IL2CPP_INTPTR async_task_builder_get_task(CHAOS_IL2CPP_INTPTR builder_ref)
{
    auto* builder_slot = resolve_native_int_slot(builder_ref);
    if (*builder_slot == static_cast<CHAOS_IL2CPP_INTPTR>(0))
    {
        *builder_slot = async_task_create();
    }
    return *builder_slot;
}

inline void async_task_builder_set_result_raw(CHAOS_IL2CPP_INTPTR builder_ref, CHAOS_IL2CPP_INTPTR value)
{
    CHAOS_IL2CPP_INTPTR handle = async_task_builder_get_task(builder_ref);
    auto* task = require_async_task(handle);
    task->result = value;
    task->exception = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    task->faulted.store(false, std::memory_order_relaxed);
    task->completed.store(true, std::memory_order_release);
    notify_task_completed(task);
    finish_async_task(handle);
}

inline void async_task_builder_set_exception(CHAOS_IL2CPP_INTPTR builder_ref, CHAOS_IL2CPP_INTPTR exception)
{
    CHAOS_IL2CPP_INTPTR handle = async_task_builder_get_task(builder_ref);
    auto* task = require_async_task(handle);
    task->exception = exception;
    task->faulted.store(true, std::memory_order_relaxed);
    task->completed.store(true, std::memory_order_release);
    notify_task_completed(task);
    finish_async_task(handle);
}

inline CHAOS_IL2CPP_INTPTR async_yield_create() noexcept
{
    return static_cast<CHAOS_IL2CPP_INTPTR>(1);
}

inline CHAOS_IL2CPP_INTPTR async_yield_get_awaiter(CHAOS_IL2CPP_INTPTR awaiter_ref)
{
    return *resolve_native_int_slot(awaiter_ref);
}

inline CHAOS_IL2CPP_INTPTR async_yield_get_is_completed(CHAOS_IL2CPP_INTPTR awaiter_ref)
{
    (void)awaiter_ref;
    // When a thread-pool dispatcher is registered (runtime init), Task.Yield()
    // is NEVER synchronously complete — it always suspends and resumes via the
    // thread pool.  This makes the codegen-emitted MoveNext take the suspend
    // path (store awaiter, state=0, call AwaitUnsafeOnCompleted → continuation
    // queued to ThreadPool) instead of the fast path (GetResult inline).
    // Without a dispatcher (standalone/smoke TU), yield is instant-complete so
    // the synchronous path (HandCraftedStateMachineOne) works without a pool.
    if (g_async_dispatch_continuation_fn != nullptr)
        return static_cast<CHAOS_IL2CPP_INTPTR>(0);
    return static_cast<CHAOS_IL2CPP_INTPTR>(1);
}

inline void async_yield_get_result(CHAOS_IL2CPP_INTPTR awaiter_ref)
{
    (void)awaiter_ref;
}

inline CHAOS_IL2CPP_INTPTR async_task_get_awaiter(CHAOS_IL2CPP_INTPTR task_handle)
{
    (void)require_async_task(task_handle);
    return task_handle;
}

inline CHAOS_IL2CPP_INTPTR async_task_awaiter_get_is_completed(CHAOS_IL2CPP_INTPTR awaiter_ref)
{
    auto* task = require_async_task(*resolve_native_int_slot(awaiter_ref));
    return task->completed.load(std::memory_order_acquire)
        ? static_cast<CHAOS_IL2CPP_INTPTR>(1)
        : static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

/// Get the result of a completed (non-faulted) task.  If not yet completed, or
/// faulted, returns 0 WITHOUT aborting: a faulted task's payload lives in
/// task->exception and must be retrieved via async_task_awaiter_get_exception
/// and propagated to managed code by the caller (C# await semantics: a faulted
/// await throws, it does not read result or crash).  Callers branch on faulted
/// BEFORE reading result.
inline CHAOS_IL2CPP_INTPTR async_task_awaiter_get_result_raw(CHAOS_IL2CPP_INTPTR awaiter_ref)
{
    auto* task = require_async_task(*resolve_native_int_slot(awaiter_ref));
    if (task->faulted.load(std::memory_order_acquire)) return 0;  // result is undefined
    if (!task->completed.load(std::memory_order_acquire)) return 0; // not done; caller must IsCompleted-gate
    return task->result;
}

/// Retrieve the exception from a faulted task, or 0 if no exception.
/// Provides an escape hatch for callers that need to inspect the exception
/// rather than letting the default awaiter abort on fault.
inline CHAOS_IL2CPP_INTPTR async_task_awaiter_get_exception(CHAOS_IL2CPP_INTPTR awaiter_ref)
{
    auto* task = require_async_task(*resolve_native_int_slot(awaiter_ref));
    return task->exception;
}

/// True if the task behind this awaiter faulted.  Cancellation is reported by
/// `async_task_awaiter_get_is_canceled` instead, so awaiting a cancelled task
/// raises TaskCanceledException and awaiting a faulted one rethrows the stored
/// exception.
inline CHAOS_IL2CPP_INTPTR async_task_awaiter_get_is_faulted(CHAOS_IL2CPP_INTPTR awaiter_ref)
{
    auto* task = require_async_task(*resolve_native_int_slot(awaiter_ref));
    return task->faulted.load(std::memory_order_acquire)
        ? static_cast<CHAOS_IL2CPP_INTPTR>(1)
        : static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

/// True if the task behind this awaiter was cancelled.
inline CHAOS_IL2CPP_INTPTR async_task_awaiter_get_is_canceled(CHAOS_IL2CPP_INTPTR awaiter_ref)
{
    auto* task = require_async_task(*resolve_native_int_slot(awaiter_ref));
    return task->canceled.load(std::memory_order_acquire)
        ? static_cast<CHAOS_IL2CPP_INTPTR>(1)
        : static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

/// Task.Run: queue a delegate for execution on the thread pool.
/// The task is created, queued, and the task handle is returned.
/// When the delegate completes, the task is marked as completed.
///
/// During normal runtime operation, runtime_core registers its
/// ThreadPool-backed implementation via register_async_task_run_fn().
/// In verification/stub mode (no runtime_core loaded), returns 0.
inline CHAOS_IL2CPP_INTPTR async_task_run(CHAOS_IL2CPP_INTPTR delegate_fn) noexcept
{
    if (g_async_task_run_fn != nullptr) {
        return g_async_task_run_fn(delegate_fn);
    }
    (void)delegate_fn;
    return 0;
}

// ══════════════════════════════════════════════════════════════════════════════
// Task already-completed factories (System.Threading.Tasks.Task.From*).
//
// These produce an AsyncTask that is complete at construction time, matching
// the .NET contract: FromResult/FromException/FromCanceled never suspend, and
// an await on them observes the value/fault immediately.
// ══════════════════════════════════════════════════════════════════════════════

/// Task.FromResult<T>(T result): a successfully-completed task carrying `value`.
/// The caller's await resumes synchronously (is_completed == true).
inline CHAOS_IL2CPP_INTPTR async_task_from_result(CHAOS_IL2CPP_INTPTR value) noexcept
{
    CHAOS_IL2CPP_INTPTR handle = async_task_create();
    auto* task = require_async_task(handle);
    task->result = value;
    task->exception = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    task->faulted.store(false, std::memory_order_relaxed);
    task->completed.store(true, std::memory_order_release);
    notify_task_completed(task);
    return handle;
}

/// Task.FromException(Exception ex): a faulted task carrying `exception`.
/// Awaiting it must THROW — see async_task_awaiter_get_is_faulted.
inline CHAOS_IL2CPP_INTPTR async_task_from_exception(CHAOS_IL2CPP_INTPTR exception) noexcept
{
    CHAOS_IL2CPP_INTPTR handle = async_task_create();
    auto* task = require_async_task(handle);
    task->result = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    task->exception = exception;
    task->faulted.store(true, std::memory_order_relaxed);
    task->completed.store(true, std::memory_order_release);
    notify_task_completed(task);
    return handle;
}

/// Task.FromCanceled(CancellationToken): a cancelled task — a distinct terminal
/// state from a fault.  `canceled` is set and `faulted` is left false so the
/// awaiter raises TaskCanceledException specifically, rather than conflating
/// cancellation with a payload-less fault.
inline CHAOS_IL2CPP_INTPTR async_task_from_canceled() noexcept
{
    CHAOS_IL2CPP_INTPTR handle = async_task_create();
    auto* task = require_async_task(handle);
    task->result = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    task->exception = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    task->faulted.store(false, std::memory_order_relaxed);
    task->canceled.store(true, std::memory_order_relaxed);
    task->completed.store(true, std::memory_order_release);
    notify_task_completed(task);
    return handle;
}

/// True when the task was cancelled (as opposed to faulted or completed).
inline CHAOS_IL2CPP_INTPTR async_task_get_is_canceled(CHAOS_IL2CPP_INTPTR task_handle) noexcept
{
    auto* task = require_async_task(task_handle);
    return task->canceled.load(std::memory_order_acquire)
        ? static_cast<CHAOS_IL2CPP_INTPTR>(1)
        : static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

/// True when the task faulted.  Excludes cancellation, so a caller can tell the
/// three terminal states apart: completed / faulted / canceled.
inline CHAOS_IL2CPP_INTPTR async_task_get_is_faulted(CHAOS_IL2CPP_INTPTR task_handle) noexcept
{
    auto* task = require_async_task(task_handle);
    return task->faulted.load(std::memory_order_acquire)
        ? static_cast<CHAOS_IL2CPP_INTPTR>(1)
        : static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

/// True when the task is complete (success or fault) — the synchronous
/// completion oracle used by Task.Wait polling loops.
inline CHAOS_IL2CPP_INTPTR async_task_get_is_completed(CHAOS_IL2CPP_INTPTR task_handle) noexcept
{
    auto* task = require_async_task(task_handle);
    return task->completed.load(std::memory_order_acquire)
        ? static_cast<CHAOS_IL2CPP_INTPTR>(1)
        : static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

// ══════════════════════════════════════════════════════════════════════════════
// Phase 2 (segment B/C): AsyncTaskMethodBuilder<T> operations wired to native.
//
// These help the codegen ShapeRegistry route the IL builder ops
// (Create / Start<TStateMachine> / get_Task / SetResult / SetException /
// AwaitUnsafeOnCompleted<TAwaiter,TStateMachine>) to real AsyncTask semantics
// instead of returning 0 via the external-runtime fallback.
//
// The AsyncTask handle model:
//   - The compiler-generated >d__ struct carries its builder as an INTPTR
//     reference slot (field_<>t__builder).  async_task_builder_* helpers take
//     builder_ref = CHAOS_IL2CPP_INTPTR* slot address (tag-masked by
//     resolve_native_int_slot) and lazily materialize the AsyncTask handle.
//   - A completed/suspended state machine's Task handle is that AsyncTask.
//     MoveNext runs synchronously to first suspension (or completion).  A
//     suspended machine stores a continuation (the MoveNext entry) on the
//     AsyncTask and is re-entered when the task completes (finish_async_task →
//     g_async_dispatch_continuation_fn, thread pool).
// ══════════════════════════════════════════════════════════════════════════════

/// Function pointer type for the compiler-generated state machine's MoveNext.
/// ABI: extern "C" void MoveNext(CHAOS_IL2CPP_INTPTR box).
/// The box is the >d__ struct instance (stack- or GC-allocated).
using AsyncStateMachineMoveNextFn = void (*)(CHAOS_IL2CPP_INTPTR);

/// Continuation data holding the state machine's MoveNext function pointer and
/// its box pointer.  Allocated on the heap when a continuation is registered
/// (AwaitUnsafeOnCompleted) and freed after it fires exactly once.
struct AsyncStateMachineContinuationData {
    AsyncStateMachineMoveNextFn move_next;
    void* sm_box;
};

/// Continuation callback used with async_task_on_completed.
/// Fires the state machine's MoveNext with the box pointer.
inline void AsyncStateMachineContinuationCallback(CHAOS_IL2CPP_INTPTR, void* ctx) noexcept {
    auto* data = static_cast<AsyncStateMachineContinuationData*>(ctx);
    data->move_next(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(data->sm_box));
    delete data;
}

/// Built-in empty Create.  Codegen stores this (0) and the lazy
/// async_task_builder_get_task materializes the handle on first touch.
inline CHAOS_IL2CPP_INTPTR chaos_async_task_builder_create() noexcept
{
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

/// AsyncTaskMethodBuilder (non-generic, Task) SetResult() — void, no value.
inline void async_task_builder_set_result_void(CHAOS_IL2CPP_INTPTR builder_ref) noexcept
{
    async_task_builder_set_result_raw(builder_ref, static_cast<CHAOS_IL2CPP_INTPTR>(0));
}

/// Start<TStateMachine>: force Task creation, then drive the state machine's
/// MoveNext synchronously until its first real suspension or completion.
/// sm_box is the boxed >d__ state machine (this), move_next its native MoveNext.
/// Returns 1 on success, 0 on invalid args.
inline CHAOS_IL2CPP_INTPTR async_task_builder_start(
    CHAOS_IL2CPP_INTPTR builder_ref,
    AsyncStateMachineMoveNextFn move_next, void* sm_box) noexcept
{
    if (builder_ref == static_cast<CHAOS_IL2CPP_INTPTR>(0) || move_next == nullptr) return 0;
    // Start's contract: a Task exists so a later awaited/observed handle is valid.
    async_task_builder_get_task(builder_ref);
    // Drive the first MoveNext synchronously.  sm_box is the >d__ direct pointer.
    move_next(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(sm_box));
    return static_cast<CHAOS_IL2CPP_INTPTR>(1);
}

/// AwaitUnsafeOnCompleted for TaskAwaiter: register the state machine's MoveNext
/// as the continuation of the awaited task.  Returns 1 on success, 0 on failure.
inline CHAOS_IL2CPP_INTPTR async_await_task_resume(
    CHAOS_IL2CPP_INTPTR awaiter_ref,
    AsyncStateMachineMoveNextFn move_next, void* sm_box) noexcept
{
    CHAOS_IL2CPP_INTPTR task_handle = *resolve_native_int_slot(awaiter_ref);
    if (task_handle == static_cast<CHAOS_IL2CPP_INTPTR>(0) || move_next == nullptr) return 0;
    auto* data = new AsyncStateMachineContinuationData{move_next, sm_box};
    async_task_on_completed(task_handle, AsyncStateMachineContinuationCallback, data);
    return static_cast<CHAOS_IL2CPP_INTPTR>(1);
}

/// AwaitUnsafeOnCompleted for Task.Yield-style suspension.  When a real thread-pool
/// dispatcher is registered (runtime init), queue MoveNext to run on a worker thread
/// on resume.  Otherwise (standalone/verification TU), fire the continuation inline.
inline CHAOS_IL2CPP_INTPTR async_await_yield_resume(
    void* sm_box,
    AsyncStateMachineMoveNextFn move_next) noexcept
{
    if (move_next == nullptr) return 0;
    if (g_async_dispatch_continuation_fn != nullptr)
    {
        // Use the thread-pool dispatcher: queue the MoveNext continuation.
        // We need to wrap move_next+sm_box into an AsyncContinueFn.
        auto* data = new AsyncStateMachineContinuationData{move_next, sm_box};
        g_async_dispatch_continuation_fn(AsyncStateMachineContinuationCallback, data,
            static_cast<CHAOS_IL2CPP_INTPTR>(0));
    }
    else
    {
        // No dispatcher (standalone mode): fire MoveNext inline.
        move_next(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(sm_box));
    }
    return static_cast<CHAOS_IL2CPP_INTPTR>(1);
}

} // namespace chaos::il2cpp::common

// ══════════════════════════════════════════════════════════════════════════════
// Phase 3 (P3-1) TaskCompletionSource<T> — native TaskSource proxy.
//
// A TaskSource wraps an AsyncTask handle and provides SetResult/SetException
// /TrySetResult/TrySetCanceled/TrySetException/get_Task operations matching
// the managed TaskCompletionSource<T> contract.
//
// TCS is the "external completion" counterpart of AsyncTaskMethodBuilder:
//   - Builder: the async state machine owns the Task and completes it.
//   - TCS: external code creates a Task source and hands the Task out; TCS
//     is the *completion trigger* for that Task.
//
// The TaskSource itself is NOT GC-owned yet (Phase 5 perf when GC-boxed
// TCS objects are defined). Use task_source_create() for plain-new allocation.
//
// Lifecycle: call task_source_destroy() when the TaskSource is no longer
// needed (its underlying AsyncTask continues to live independently once
// handed out; destroying the TaskSource does NOT destroy the Task — that
// is the caller's responsibility via the returned handle).
// ══════════════════════════════════════════════════════════════════════════════

namespace chaos::il2cpp::common {

/// Lightweight proxy holding an AsyncTask handle and driving completions
/// from external code (non-state-machine).  This is the native equivalent
/// of TaskCompletionSource<T>.
struct TaskSource {
    AsyncTask* task;

    /// Return the underlying AsyncTask handle (the Task that awaiters see).
    CHAOS_IL2CPP_INTPTR get_task() const noexcept {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(task);
    }

    /// SetResult(TResult value): complete the Task successfully.
    /// Fires any registered continuation.  Idempotent-safe: does NOT assert
    /// if the task already completed (the Try* variants are the gate for
    /// "only the first completion wins" semantics).
    void set_result(CHAOS_IL2CPP_INTPTR value) noexcept {
        task->result = value;
        task->exception = static_cast<CHAOS_IL2CPP_INTPTR>(0);
        task->faulted.store(false, std::memory_order_relaxed);
        task->canceled.store(false, std::memory_order_relaxed);
        task->completed.store(true, std::memory_order_release);
        notify_task_completed(task);
        finish_async_task(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(task));
    }

    /// SetException(Exception ex): fault the Task.
    /// Fires any registered continuation.
    void set_exception(CHAOS_IL2CPP_INTPTR exception) noexcept {
        task->exception = exception;
        task->faulted.store(true, std::memory_order_relaxed);
        task->canceled.store(false, std::memory_order_relaxed);
        task->completed.store(true, std::memory_order_release);
        notify_task_completed(task);
        finish_async_task(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(task));
    }

    /// TrySetResult(TResult value): complete if not yet completed.
    /// Returns 1 if this call completed it, 0 if already completed.
    CHAOS_IL2CPP_INTPTR try_set_result(CHAOS_IL2CPP_INTPTR value) noexcept {
        bool expected = false;
        if (task->completed.compare_exchange_strong(
                expected, true, std::memory_order_acq_rel)) {
            task->result = value;
            task->exception = static_cast<CHAOS_IL2CPP_INTPTR>(0);
            task->faulted.store(false, std::memory_order_relaxed);
            task->canceled.store(false, std::memory_order_relaxed);
            notify_task_completed(task);
            finish_async_task(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(task));
            return static_cast<CHAOS_IL2CPP_INTPTR>(1);
        }
        return static_cast<CHAOS_IL2CPP_INTPTR>(0);
    }

    /// TrySetException(Exception ex): fault if not yet completed.
    /// Returns 1 if this call completed it, 0 if already completed.
    CHAOS_IL2CPP_INTPTR try_set_exception(CHAOS_IL2CPP_INTPTR exception) noexcept {
        bool expected = false;
        if (task->completed.compare_exchange_strong(
                expected, true, std::memory_order_acq_rel)) {
            task->exception = exception;
            task->faulted.store(true, std::memory_order_relaxed);
            task->canceled.store(false, std::memory_order_relaxed);
            notify_task_completed(task);
            finish_async_task(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(task));
            return static_cast<CHAOS_IL2CPP_INTPTR>(1);
        }
        return static_cast<CHAOS_IL2CPP_INTPTR>(0);
    }

    /// TrySetCanceled(): cancel if not yet completed.
    /// Returns 1 if this call canceled it, 0 if already completed.
    CHAOS_IL2CPP_INTPTR try_set_canceled() noexcept {
        bool expected = false;
        if (task->completed.compare_exchange_strong(
                expected, true, std::memory_order_acq_rel)) {
            // Cancellation is its own terminal state, NOT a fault: setting both
            // faulted and canceled would collapse the two, which is exactly the
            // conflation ASYNC-P1-4 removed (a cancelled await must surface as
            // TaskCanceledException, not as a generic fault).
            task->canceled.store(true, std::memory_order_relaxed);
            notify_task_completed(task);
            finish_async_task(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(task));
            return static_cast<CHAOS_IL2CPP_INTPTR>(1);
        }
        return static_cast<CHAOS_IL2CPP_INTPTR>(0);
    }
};

/// Allocate a new TaskSource + its underlying AsyncTask.
/// Returns nullptr on allocation failure.
inline TaskSource* task_source_create() noexcept {
    auto* task = new (std::nothrow) AsyncTask();
    if (task == nullptr) return nullptr;
    auto* ts = new (std::nothrow) TaskSource();
    if (ts == nullptr) {
        delete task;
        return nullptr;
    }
    ts->task = task;
    return ts;
}

/// Destroy a TaskSource allocated by task_source_create.
/// Does NOT delete the underlying AsyncTask — it outlives the source
/// once handed out to awaiters.  The caller destroys the Task separately
/// if owning it (in test/standalone contexts).
inline void task_source_destroy(TaskSource* ts) noexcept {
    delete ts;
}

} // namespace chaos::il2cpp::common

#endif // CHAOS_IL2CPP_COMMON_ASYNC_H_
