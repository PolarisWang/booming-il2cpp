#ifndef CHAOS_IL2CPP_COMMON_ASYNC_H_
#define CHAOS_IL2CPP_COMMON_ASYNC_H_

#include "ptr_tag.h"

#include <cstdint>
#include <cstdlib>

namespace chaos::il2cpp::common {

struct AsyncTask
{
    CHAOS_IL2CPP_INTPTR result = 0;
    CHAOS_IL2CPP_INTPTR exception = 0;
    bool completed = false;
    bool faulted = false;
};

inline AsyncTask* require_async_task(CHAOS_IL2CPP_INTPTR handle)
{
    if (handle == static_cast<CHAOS_IL2CPP_INTPTR>(0))
    {
        CHAOS_IL2CPP_ABORT();
    }
    return reinterpret_cast<AsyncTask*>(handle);
}

inline CHAOS_IL2CPP_INTPTR async_task_create()
{
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
    auto* task = require_async_task(async_task_builder_get_task(builder_ref));
    task->result = value;
    task->exception = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    task->faulted = false;
    task->completed = true;
}

inline void async_task_builder_set_exception(CHAOS_IL2CPP_INTPTR builder_ref, CHAOS_IL2CPP_INTPTR exception)
{
    auto* task = require_async_task(async_task_builder_get_task(builder_ref));
    task->exception = exception;
    task->faulted = true;
    task->completed = true;
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
    return task->completed ? static_cast<CHAOS_IL2CPP_INTPTR>(1) : static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

inline CHAOS_IL2CPP_INTPTR async_task_awaiter_get_result_raw(CHAOS_IL2CPP_INTPTR awaiter_ref)
{
    auto* task = require_async_task(*resolve_native_int_slot(awaiter_ref));
    if (!task->completed || task->faulted)
    {
        CHAOS_IL2CPP_ABORT();
    }
    return task->result;
}

/// Task.Run: queue a delegate for execution on the thread pool.
/// The task is created, queued, and the task handle is returned.
/// When the delegate completes, the task is marked as completed.
inline CHAOS_IL2CPP_INTPTR async_task_run(CHAOS_IL2CPP_INTPTR delegate_fn) noexcept
{
#if defined(CHAOS_IL2CPP_VERIFY_MODE)
    // Stub for standalone entry builds (verification pipeline).
    // Full implementation requires bootstrap/thread_pool services not available here.
    (void)delegate_fn;
    return 0;
#else
    // Full implementation requires bootstrap/thread_pool services not available
    // in chaos_common (dependency-free lib). Stub until ThreadPool is integrated.
#   pragma message("TODO(" __FILE__ "): implement async_task_run for non-verification builds")
    return 0;
#endif
}

} // namespace chaos::il2cpp::common

#endif // CHAOS_IL2CPP_COMMON_ASYNC_H_
