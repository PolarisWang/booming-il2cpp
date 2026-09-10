// task_runner.cpp — Task.Run + Task<T> runtime support
//
// Provides the native-side implementation for Task.Run, which queues
// a delegate on the ThreadPool and returns an AsyncTask handle.
// ExecutionContext is flowed from the caller to the async task.

#include "thread_pool.h"
#include "execution_context.h"
#include "core/delegate_helpers.h"
#include "bootstrap/bootstrap.h"

#include <chaos/async.h>
#include <chaos/log.h>

#include <cstdlib>
#include <new>

namespace chaos::il2cpp::runtime_core::threading {

// ── Task.Run ──────────────────────────────────────────────────────────

struct TaskRunContext {
    chaos::il2cpp::common::AsyncTask* task;
    CHAOS_IL2CPP_INTPTR delegate;
    ExecutionContext* ctx;
};

static void TaskRunCallback(void* state) noexcept {
    auto* rc = static_cast<TaskRunContext*>(state);
    if (rc == nullptr) return;

    // Invoke the delegate under the captured ExecutionContext.
    ExecutionContextRun(rc->ctx, [](void* s) {
        auto* inner = static_cast<TaskRunContext*>(s);
        if (inner->delegate != 0) {
            chaos_delegate_object_invoke(inner->delegate, nullptr, nullptr, 0);
        }
        inner->task->completed = true;
    }, rc);

    ExecutionContextFree(rc->ctx);
    delete rc;
}

CHAOS_IL2CPP_INTPTR TaskRun(CHAOS_IL2CPP_INTPTR delegate_fn) noexcept {
    if (delegate_fn == 0) return 0;

    auto* task = new (std::nothrow) chaos::il2cpp::common::AsyncTask();
    if (task == nullptr) return 0;

    auto* ctx = new (std::nothrow) TaskRunContext();
    if (ctx == nullptr) {
        delete task;
        return 0;
    }

    ctx->task = task;
    ctx->delegate = delegate_fn;
    ctx->ctx = ExecutionContextCapture();

    // Queue without EC capture — TaskRunCallback manages EC internally.
    ThreadPoolQueueUserWorkItemUnsafe(TaskRunCallback, ctx);

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(task);
}

void RegisterAsyncTaskRun() noexcept {
    ::chaos::il2cpp::common::register_async_task_run_fn(TaskRun);
}

}  // namespace chaos::il2cpp::runtime_core::threading