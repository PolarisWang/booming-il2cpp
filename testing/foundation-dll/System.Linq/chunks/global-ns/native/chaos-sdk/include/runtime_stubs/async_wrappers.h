#pragma once
#include <chaos/native_types.h>
#include <coroutine>

extern "C" {
CHAOS_IL2CPP_INT32 chaos_async_task_is_completed(CHAOS_IL2CPP_INTPTR) noexcept;
CHAOS_IL2CPP_INTPTR chaos_async_task_get_result(CHAOS_IL2CPP_INTPTR);
void chaos_async_task_on_completed(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR) noexcept;
CHAOS_IL2CPP_INT32 chaos_async_valuetask_is_completed(CHAOS_IL2CPP_INTPTR) noexcept;
CHAOS_IL2CPP_INTPTR chaos_async_valuetask_get_result(CHAOS_IL2CPP_INTPTR);
void chaos_async_valuetask_on_completed(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR) noexcept;
void chaos_async_yield_suspend(CHAOS_IL2CPP_INTPTR) noexcept;
}

template<typename P> struct TaskAwaiterWrapper {
    CHAOS_IL2CPP_INTPTR _t;
    TaskAwaiterWrapper(CHAOS_IL2CPP_INTPTR t) noexcept : _t(t) {}
    bool await_ready() noexcept { return chaos_async_task_is_completed(_t) != 0; }
    void await_suspend(std::coroutine_handle<P> h) noexcept { chaos_async_task_on_completed(_t, h.address()); }
    void await_resume() { chaos_async_task_get_result(_t); }
};

template<typename P, typename T> struct TaskAwaiterOfTWrapper {
    CHAOS_IL2CPP_INTPTR _t;
    TaskAwaiterOfTWrapper(CHAOS_IL2CPP_INTPTR t) noexcept : _t(t) {}
    bool await_ready() noexcept { return chaos_async_task_is_completed(_t) != 0; }
    void await_suspend(std::coroutine_handle<P> h) noexcept { chaos_async_task_on_completed(_t, h.address()); }
    T await_resume() { return (T)chaos_async_task_get_result(_t); }
};

template<typename P> struct YieldAwaiterWrapper {
    bool await_ready() noexcept { return false; }
    void await_suspend(std::coroutine_handle<P> h) noexcept { chaos_async_yield_suspend(h.address()); }
    void await_resume() noexcept {}
};
