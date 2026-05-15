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

void chaos_thread_yield(void) noexcept
{
    std::this_thread::yield();
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

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core