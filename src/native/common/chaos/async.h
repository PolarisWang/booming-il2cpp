#ifndef CHAOS_IL2CPP_COMMON_ASYNC_H_
#define CHAOS_IL2CPP_COMMON_ASYNC_H_

#include "ptr_tag.h"

#include <atomic>
#include <cstdint>
#include <cstdlib>

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

    // Continuation (single-slot box): registered by AwaitUnsafeOnCompleted.
    // fired exactly once on first completion.
    AsyncContinueFn   continuation_cb = nullptr;
    void*             continuation_ctx = nullptr;
    std::atomic<bool> has_continuation{false};
    std::atomic<bool> continuation_fired{false};
};

inline AsyncTask* require_async_task(CHAOS_IL2CPP_INTPTR handle)
{
    if (handle == static_cast<CHAOS_IL2CPP_INTPTR>(0))
    {
        CHAOS_IL2CPP_ABORT();
    }
    return reinterpret_cast<AsyncTask*>(handle);
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
    finish_async_task(handle);
}

inline void async_task_builder_set_exception(CHAOS_IL2CPP_INTPTR builder_ref, CHAOS_IL2CPP_INTPTR exception)
{
    CHAOS_IL2CPP_INTPTR handle = async_task_builder_get_task(builder_ref);
    auto* task = require_async_task(handle);
    task->exception = exception;
    task->faulted.store(true, std::memory_order_relaxed);
    task->completed.store(true, std::memory_order_release);
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

} // namespace chaos::il2cpp::common

#endif // CHAOS_IL2CPP_COMMON_ASYNC_H_
