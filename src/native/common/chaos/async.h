#ifndef CHAOS_IL2CPP_COMMON_ASYNC_H_
#define CHAOS_IL2CPP_COMMON_ASYNC_H_

#include "ptr_tag.h"

#include <cstdint>
#include <cstdlib>

namespace ChaosIl2cpp::Common {

struct AsyncTask
{
    std::intptr_t result = 0;
    std::intptr_t exception = 0;
    bool completed = false;
    bool faulted = false;
};

inline AsyncTask* require_async_task(std::intptr_t handle)
{
    if (handle == static_cast<std::intptr_t>(0))
    {
        std::abort();
    }
    return reinterpret_cast<AsyncTask*>(handle);
}

inline std::intptr_t async_task_create()
{
    return reinterpret_cast<std::intptr_t>(new AsyncTask{});
}

inline std::intptr_t async_task_builder_get_task(std::intptr_t builder_ref)
{
    auto* builder_slot = resolve_native_int_slot(builder_ref);
    if (*builder_slot == static_cast<std::intptr_t>(0))
    {
        *builder_slot = async_task_create();
    }
    return *builder_slot;
}

inline void async_task_builder_set_result_raw(std::intptr_t builder_ref, std::intptr_t value)
{
    auto* task = require_async_task(async_task_builder_get_task(builder_ref));
    task->result = value;
    task->exception = static_cast<std::intptr_t>(0);
    task->faulted = false;
    task->completed = true;
}

inline void async_task_builder_set_exception(std::intptr_t builder_ref, std::intptr_t exception)
{
    auto* task = require_async_task(async_task_builder_get_task(builder_ref));
    task->exception = exception;
    task->faulted = true;
    task->completed = true;
}

inline std::intptr_t async_yield_create() noexcept
{
    return static_cast<std::intptr_t>(1);
}

inline std::intptr_t async_yield_get_awaiter(std::intptr_t awaiter_ref)
{
    return *resolve_native_int_slot(awaiter_ref);
}

inline std::intptr_t async_yield_get_is_completed(std::intptr_t awaiter_ref)
{
    (void)awaiter_ref;
    return static_cast<std::intptr_t>(1);
}

inline void async_yield_get_result(std::intptr_t awaiter_ref)
{
    (void)awaiter_ref;
}

inline std::intptr_t async_task_get_awaiter(std::intptr_t task_handle)
{
    (void)require_async_task(task_handle);
    return task_handle;
}

inline std::intptr_t async_task_awaiter_get_is_completed(std::intptr_t awaiter_ref)
{
    auto* task = require_async_task(*resolve_native_int_slot(awaiter_ref));
    return task->completed ? static_cast<std::intptr_t>(1) : static_cast<std::intptr_t>(0);
}

inline std::intptr_t async_task_awaiter_get_result_raw(std::intptr_t awaiter_ref)
{
    auto* task = require_async_task(*resolve_native_int_slot(awaiter_ref));
    if (!task->completed || task->faulted)
    {
        std::abort();
    }
    return task->result;
}

} // namespace ChaosIl2cpp::Common

#endif // CHAOS_IL2CPP_COMMON_ASYNC_H_
