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
extern "C" {

CHAOS_IL2CPP_INTPTR chaos_thread_get_current(void) noexcept
{
    // Fast path: codegen mode — current_thread_object is always set during
    // runtime_init (via s_main_thread_sentinel). Single TLS read, no fallback.
    auto result = chaos::il2cpp::common::current_thread_object;
    if (result != 0) return result;

    // Slow path: interpreter mode or uninitialized thread — check tls_this_thread.
    auto* thread = threading::tls_this_thread;
    if (thread == nullptr) return 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(thread->managed_object);
}

void chaos_thread_ctor(
    CHAOS_IL2CPP_INTPTR thread_obj,
    CHAOS_IL2CPP_INTPTR thread_start_delegate) noexcept
{
    auto& entry = chaos::il2cpp::common::require_thread_runtime_entry(thread_obj);
    entry.thread_start_delegate = thread_start_delegate;
    entry.managed_thread_id = chaos::il2cpp::common::allocate_managed_thread_id();
}

void chaos_thread_start(CHAOS_IL2CPP_INTPTR thread_obj) noexcept
{
    auto* entry = chaos::il2cpp::common::try_get_thread_runtime_entry(thread_obj);
    if (entry == nullptr) return;

    auto* runtime_state = GetCurrentRuntimeState();
    if (runtime_state == nullptr) return;

    const CHAOS_IL2CPP_INTPTR delegate = entry->thread_start_delegate;

    // Capture the current ExecutionContext to flow to the new thread.
    using chaos::il2cpp::runtime_core::threading::ExecutionContext;
    using chaos::il2cpp::runtime_core::threading::ExecutionContextCapture;
    using chaos::il2cpp::runtime_core::threading::ExecutionContextRun;
    using chaos::il2cpp::runtime_core::threading::ExecutionContextFree;
    ExecutionContext* captured_ctx = ExecutionContextCapture();

    entry->worker = std::make_unique<std::thread>([runtime_state, delegate, captured_ctx]() {
        // Attach this thread to the runtime.
        ThreadState* thread_state = nullptr;
        if (ThreadAttach(runtime_state, &thread_state) != CHAOS_RUNTIME_STATUS_OK) {
            ExecutionContextFree(captured_ctx);
            return;
        }

        // Invoke the delegate under the captured ExecutionContext.
        ExecutionContextRun(captured_ctx, [](void* d) {
            auto* bridge = chaos::il2cpp::bootstrap::GetCodegenBridgeV0();
            auto del = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(d);
            if (bridge != nullptr && bridge->delegate_invoke != nullptr && del != 0) {
                void* return_value = nullptr;
                bridge->delegate_invoke(
                    nullptr, nullptr,
                    reinterpret_cast<void*>(del),
                    nullptr, 0,
                    &return_value, sizeof(void*),
                    nullptr);
            }
        }, reinterpret_cast<void*>(delegate));
        ExecutionContextFree(captured_ctx);

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
    using chaos::il2cpp::runtime_core::threading::EnumerateThreads;
    using chaos::il2cpp::runtime_core::threading::ManagedThread;

    if (thread_obj == 0) return;

    // Use a static bridge variable (safe: only called from managed code,
    // which is single-threaded with respect to this specific operation).
    static CHAOS_IL2CPP_INTPTR s_target = 0;
    s_target = thread_obj;

    EnumerateThreads([](ManagedThread* mt) -> bool {
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
    using chaos::il2cpp::runtime_core::threading::EnumerateThreads;
    using chaos::il2cpp::runtime_core::threading::ManagedThread;

    if (thread_obj == 0) return;

    static CHAOS_IL2CPP_INTPTR s_target = 0;
    s_target = thread_obj;

    EnumerateThreads([](ManagedThread* mt) -> bool {
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
    using chaos::il2cpp::runtime_core::threading::tls_this_thread;

    auto* thread = tls_this_thread;
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
    using chaos::il2cpp::runtime_core::threading::EnumerateThreads;
    using chaos::il2cpp::runtime_core::threading::ManagedThread;

    if (thread_obj == 0) return 0;

    static CHAOS_IL2CPP_INTPTR s_target = 0;
    static CHAOS_IL2CPP_INT32 s_result = 0;
    s_target = thread_obj;
    s_result = 0;

    EnumerateThreads([](ManagedThread* mt) -> bool {
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
    using chaos::il2cpp::runtime_core::threading::EnumerateThreads;
    using chaos::il2cpp::runtime_core::threading::ManagedThread;

    if (thread_obj == 0) return;

    static CHAOS_IL2CPP_INTPTR s_target = 0;
    static CHAOS_IL2CPP_INT32 s_new_bg = 0;
    s_target = thread_obj;
    s_new_bg = value;

    EnumerateThreads([](ManagedThread* mt) -> bool {
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
    using chaos::il2cpp::runtime_core::threading::EnumerateThreads;
    using chaos::il2cpp::runtime_core::threading::ManagedThread;

    if (thread_obj == 0) return 0;

    static CHAOS_IL2CPP_INTPTR s_target = 0;
    static CHAOS_IL2CPP_INT32 s_result = 0;
    s_target = thread_obj;
    s_result = 0;

    EnumerateThreads([](ManagedThread* mt) -> bool {
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
    using chaos::il2cpp::runtime_core::threading::EnumerateThreads;
    using chaos::il2cpp::runtime_core::threading::ManagedThread;

    if (thread_obj == 0) return static_cast<CHAOS_IL2CPP_INT32>(threading::ManagedThreadPriority::Normal);

    static CHAOS_IL2CPP_INTPTR s_target = 0;
    static CHAOS_IL2CPP_INT32 s_result = 0;
    s_target = thread_obj;
    s_result = static_cast<CHAOS_IL2CPP_INT32>(threading::ManagedThreadPriority::Normal);

    EnumerateThreads([](ManagedThread* mt) -> bool {
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
    using chaos::il2cpp::runtime_core::threading::EnumerateThreads;
    using chaos::il2cpp::runtime_core::threading::ManagedThread;

    if (thread_obj == 0) return;

    static CHAOS_IL2CPP_INTPTR s_target = 0;
    static CHAOS_IL2CPP_INT32 s_new_pri = 0;
    s_target = thread_obj;
    s_new_pri = value;

    EnumerateThreads([](ManagedThread* mt) -> bool {
        if (mt != nullptr && mt->managed_object == reinterpret_cast<void*>(s_target)) {
            mt->priority = static_cast<threading::ManagedThreadPriority>(s_new_pri);
            s_target = 0;
            return false;
        }
        return true;
    });
}

CHAOS_IL2CPP_INT32 chaos_thread_is_threadpool(CHAOS_IL2CPP_INTPTR thread_obj) noexcept
{
    using chaos::il2cpp::runtime_core::threading::EnumerateThreads;
    using chaos::il2cpp::runtime_core::threading::ManagedThread;

    if (thread_obj == 0) return 0;

    static CHAOS_IL2CPP_INTPTR s_target = 0;
    static CHAOS_IL2CPP_INT32 s_result = 0;
    s_target = thread_obj;
    s_result = 0;

    EnumerateThreads([](ManagedThread* mt) -> bool {
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