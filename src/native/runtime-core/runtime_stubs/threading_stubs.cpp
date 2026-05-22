// ABI exports: extern "C" linkage for managed/NativeAOT callability.

// threading_stubs.cpp — Threading stub implementations
#include <chaos/native_types.h>
#include <chaos/thread.h>
#include "generated_code_compat.h"

#include "thread_state.h"
#include "execution_context.h"
#include "runtime_core.h"
#include "bootstrap/bootstrap.h"

#include <memory>
#include <thread>

namespace chaos::il2cpp::runtime_core {

// NOTE: MSVC generates C-linkage (undecorated) references when names are
// used inside extern "C" blocks, even if brought in by using-declarations
// or nested namespace wrappers.  The calls to EnumerateThreads and access
// to tls_this_thread are therefore delegated to bridge functions declared
// extern "C" in thread_state.h and defined in thread_state.cpp:
//
//   chaos_enumerate_threads()  — wraps threading::EnumerateThreads
//   chaos_get_tls_this_thread() — wraps threading::tls_this_thread

extern "C" {

void chaos_thread_ctor(
    CHAOS_IL2CPP_INTPTR thread_obj,
    CHAOS_IL2CPP_INTPTR thread_start_delegate) noexcept
{
    fprintf(stderr, "[TRACE] chaos_thread_ctor: thread_obj=0x%llx delegate=0x%llx\n",
            (unsigned long long)thread_obj, (unsigned long long)thread_start_delegate);
    auto& entry = chaos::il2cpp::common::require_thread_runtime_entry(thread_obj);
    entry.thread_start_delegate = thread_start_delegate;
    entry.managed_thread_id = chaos::il2cpp::common::allocate_managed_thread_id();
}

void chaos_thread_start(CHAOS_IL2CPP_INTPTR thread_obj) noexcept
{
    fprintf(stderr, "[TRACE] chaos_thread_start: thread_obj=0x%llx\n",
            (unsigned long long)thread_obj);
    auto* entry = chaos::il2cpp::common::try_get_thread_runtime_entry(thread_obj);
    if (entry == nullptr) {
        fprintf(stderr, "[TRACE] chaos_thread_start: entry NOT FOUND!\n");
        return;
    }
    fprintf(stderr, "[TRACE] chaos_thread_start: entry FOUND, delegate=0x%llx\n",
            (unsigned long long)entry->thread_start_delegate);

    auto* runtime_state = GetCurrentRuntimeState();
    if (runtime_state == nullptr) {
        fprintf(stderr, "[TRACE] chaos_thread_start: GetCurrentRuntimeState() returned null!\n");
        return;
    }

    const CHAOS_IL2CPP_INTPTR delegate = entry->thread_start_delegate;

    // Capture the current ExecutionContext to flow to the new thread.
    auto* captured_ctx = chaos_execution_context_capture();

    entry->worker = std::make_unique<std::thread>([runtime_state, delegate, captured_ctx]() {
        fprintf(stderr, "[TRACE] worker thread started, delegate=0x%llx\n",
                (unsigned long long)delegate);
        // Attach this thread to the runtime.
        ThreadState* thread_state = nullptr;
        if (ThreadAttach(runtime_state, &thread_state) != CHAOS_RUNTIME_STATUS_OK) {
            fprintf(stderr, "[TRACE] worker thread: ThreadAttach FAILED\n");
            chaos_execution_context_free(captured_ctx);
            return;
        }
        fprintf(stderr, "[TRACE] worker thread: ThreadAttach OK\n");

        // Invoke the delegate under the captured ExecutionContext.
        chaos_execution_context_run(captured_ctx, [](void* d) {
            auto del = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(d);
            fprintf(stderr, "[TRACE] EC run callback: delegate=0x%llx\n",
                    (unsigned long long)del);
            if (del != 0) {
                fprintf(stderr, "[TRACE] about to call chaos_delegate_object_invoke\n");
                chaos_delegate_object_invoke(del, nullptr, nullptr, 0);
                fprintf(stderr, "[TRACE] chaos_delegate_object_invoke returned\n");
            }
        }, reinterpret_cast<void*>(delegate));
        chaos_execution_context_free(captured_ctx);

        // Detach from runtime.
        ThreadDetach(runtime_state, thread_state);
    });
}

void chaos_thread_join(CHAOS_IL2CPP_INTPTR thread_obj) noexcept
{
    auto* entry = chaos::il2cpp::common::try_get_thread_runtime_entry(thread_obj);
    if (entry == nullptr) return;
    if (entry->worker && entry->worker->joinable()) {
        entry->worker->join();
    }
}

void chaos_thread_interrupt(CHAOS_IL2CPP_INTPTR thread_obj) noexcept
{
    using threading::ManagedThread;

    if (thread_obj == 0) return;

    static CHAOS_IL2CPP_INTPTR s_target = 0;
    s_target = thread_obj;

    chaos_enumerate_threads([](ManagedThread* mt) -> bool {
        if (mt != nullptr && mt->managed_object == reinterpret_cast<void*>(s_target)) {
            mt->pending_interrupt.store(true, std::memory_order_release);
            s_target = 0;
            return false;  // Found, stop enumeration.
        }
        return true;
    });
}

void chaos_thread_abort(CHAOS_IL2CPP_INTPTR thread_obj) noexcept
{
    using threading::ManagedThread;

    if (thread_obj == 0) return;

    static CHAOS_IL2CPP_INTPTR s_target = 0;
    s_target = thread_obj;

    chaos_enumerate_threads([](ManagedThread* mt) -> bool {
        if (mt != nullptr && mt->managed_object == reinterpret_cast<void*>(s_target)) {
            mt->pending_abort.store(true, std::memory_order_release);
            s_target = 0;
            return false;
        }
        return true;
    });
}

CHAOS_IL2CPP_INT32 chaos_thread_reset_abort(void) noexcept
{
    auto* thread = chaos_get_tls_this_thread();
    if (thread == nullptr) return 0;

    bool was_pending = thread->pending_abort.load(std::memory_order_acquire);
    if (was_pending) {
        thread->pending_abort.store(false, std::memory_order_release);
        return 1;
    }
    return 0;
}

CHAOS_IL2CPP_INT32 chaos_thread_yield(void) noexcept
{
    std::this_thread::yield();
    return 1;
}

void chaos_thread_sleep(CHAOS_IL2CPP_INT32 timeout_ms) noexcept
{
    std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));
}

CHAOS_IL2CPP_INT32 chaos_thread_is_background(CHAOS_IL2CPP_INTPTR thread_obj) noexcept
{
    using threading::ManagedThread;

    if (thread_obj == 0) return 0;

    static CHAOS_IL2CPP_INTPTR s_target = 0;
    static CHAOS_IL2CPP_INT32 s_result = 0;
    s_target = thread_obj;
    s_result = 0;

    chaos_enumerate_threads([](ManagedThread* mt) -> bool {
        if (mt != nullptr && mt->managed_object == reinterpret_cast<void*>(s_target)) {
            s_result = mt->is_background ? 1 : 0;
            s_target = 0;
            return false;
        }
        return true;
    });
    return s_result;
}

void chaos_thread_set_background(CHAOS_IL2CPP_INTPTR thread_obj, CHAOS_IL2CPP_INT32 value) noexcept
{
    using threading::ManagedThread;

    if (thread_obj == 0) return;

    static CHAOS_IL2CPP_INTPTR s_target = 0;
    static CHAOS_IL2CPP_INT32 s_new_bg = 0;
    s_target = thread_obj;
    s_new_bg = value;

    chaos_enumerate_threads([](ManagedThread* mt) -> bool {
        if (mt != nullptr && mt->managed_object == reinterpret_cast<void*>(s_target)) {
            mt->is_background = (s_new_bg != 0);
            s_target = 0;
            return false;
        }
        return true;
    });
}

CHAOS_IL2CPP_INT32 chaos_thread_get_state(CHAOS_IL2CPP_INTPTR thread_obj) noexcept
{
    using threading::ManagedThread;

    if (thread_obj == 0) return 0;

    static CHAOS_IL2CPP_INTPTR s_target = 0;
    static CHAOS_IL2CPP_INT32 s_result = 0;
    s_target = thread_obj;
    s_result = 0;

    chaos_enumerate_threads([](ManagedThread* mt) -> bool {
        if (mt != nullptr && mt->managed_object == reinterpret_cast<void*>(s_target)) {
            s_result = static_cast<CHAOS_IL2CPP_INT32>(mt->managed_state);
            s_target = 0;
            return false;
        }
        return true;
    });
    return s_result;
}

CHAOS_IL2CPP_INT32 chaos_thread_get_priority(CHAOS_IL2CPP_INTPTR thread_obj) noexcept
{
    using threading::ManagedThread;
    using threading::ManagedThreadPriority;

    if (thread_obj == 0) return static_cast<CHAOS_IL2CPP_INT32>(ManagedThreadPriority::Normal);

    static CHAOS_IL2CPP_INTPTR s_target = 0;
    static CHAOS_IL2CPP_INT32 s_result = 0;
    s_target = thread_obj;
    s_result = static_cast<CHAOS_IL2CPP_INT32>(ManagedThreadPriority::Normal);

    chaos_enumerate_threads([](ManagedThread* mt) -> bool {
        if (mt != nullptr && mt->managed_object == reinterpret_cast<void*>(s_target)) {
            s_result = static_cast<CHAOS_IL2CPP_INT32>(mt->priority);
            s_target = 0;
            return false;
        }
        return true;
    });
    return s_result;
}

void chaos_thread_set_priority(CHAOS_IL2CPP_INTPTR thread_obj, CHAOS_IL2CPP_INT32 value) noexcept
{
    using threading::ManagedThread;
    using threading::ManagedThreadPriority;

    if (thread_obj == 0) return;

    static CHAOS_IL2CPP_INTPTR s_target = 0;
    static CHAOS_IL2CPP_INT32 s_new_pri = 0;
    s_target = thread_obj;
    s_new_pri = value;

    chaos_enumerate_threads([](ManagedThread* mt) -> bool {
        if (mt != nullptr && mt->managed_object == reinterpret_cast<void*>(s_target)) {
            mt->priority = static_cast<ManagedThreadPriority>(s_new_pri);
            s_target = 0;
            return false;
        }
        return true;
    });
}

void chaos_monitor_enter(CHAOS_IL2CPP_INTPTR obj, CHAOS_IL2CPP_INTPTR lockTaken) noexcept
{
    // Minimal monitor enter: always succeeds, marks lock as taken.
    // codegen passes ref bool as intptr_t (pointer-sized), cast back.
    if (lockTaken != 0) {
        *reinterpret_cast<CHAOS_IL2CPP_INT32*>(lockTaken) = 1;
    }
}

void chaos_monitor_exit(CHAOS_IL2CPP_INTPTR obj) noexcept
{
    // Minimal monitor exit: no-op for now.
}

CHAOS_IL2CPP_INT32 chaos_thread_is_threadpool(CHAOS_IL2CPP_INTPTR thread_obj) noexcept
{
    using threading::ManagedThread;

    if (thread_obj == 0) return 0;

    static CHAOS_IL2CPP_INTPTR s_target = 0;
    static CHAOS_IL2CPP_INT32 s_result = 0;
    s_target = thread_obj;
    s_result = 0;

    chaos_enumerate_threads([](ManagedThread* mt) -> bool {
        if (mt != nullptr && mt->managed_object == reinterpret_cast<void*>(s_target)) {
            s_result = mt->is_threadpool ? 1 : 0;
            s_target = 0;
            return false;
        }
        return true;
    });
    return s_result;
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core
