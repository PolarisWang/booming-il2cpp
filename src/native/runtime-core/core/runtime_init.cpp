#include "gc_old_gen.h"
#include "gc_bgc.h"

// T4 VEH handler for SEH dispatch in native-generated code.
#include "../codegen/t4_seh_handler.h"

namespace chaos::il2cpp::runtime_core {

// Forward declarations from task_runner.cpp (threading sub-namespace)
namespace threading {
    void RegisterAsyncTaskRun() noexcept;
}

RuntimeStatus CHAOS_RUNTIME_ABI_CALL RuntimeInit(
    const RuntimeInitParams* init_params,
    const RuntimeConfig* config,
    RuntimeState** out_runtime_state) {
    CHAOS_IL2CPP_LOG_TRACE_INIT();
    CHAOS_IL2CPP_LOG_TRACE("runtime", "RuntimeInit", "");

    std::setvbuf(stdout, nullptr, _IOLBF, 0);

    if (init_params == nullptr || out_runtime_state == nullptr) {
        return CHAOS_RUNTIME_STATUS_INVALID_ARGUMENT;
    }

    *out_runtime_state = nullptr;
    if (init_params->struct_size < sizeof(RuntimeInitParams)) {
        return CHAOS_RUNTIME_STATUS_INVALID_ARGUMENT;
    }

    RuntimeConfig normalized_config = {};
    if (!TryNormalizeConfig(config, &normalized_config)) {
        return CHAOS_RUNTIME_STATUS_INVALID_ARGUMENT;
    }


    RuntimeState* runtime_state = static_cast<RuntimeState*>(AllocateBytes(normalized_config, sizeof(RuntimeState)));
    if (runtime_state == nullptr) {
        return CHAOS_RUNTIME_STATUS_INTERNAL_ERROR;
    }

    runtime_state->config = normalized_config;

    void* internal_mem = AllocateBytes(normalized_config, sizeof(RuntimeInternalState));
    if (internal_mem == nullptr) {
        FreeBytes(normalized_config, runtime_state);
        return CHAOS_RUNTIME_STATUS_INTERNAL_ERROR;
    }

    runtime_state->internal_state = ::new (internal_mem) RuntimeInternalState();

    // Start the BGC background thread for concurrent mark/sweep.
    BgcController::Instance().Start();

    // Register T4 VEH handler for SEH dispatch in native-generated code.
    ::chaos::il2cpp::codegen::RegisterT4SehHandler();

    // Register ThreadPool-backed Task.Run so that async_task_run() in
    // chaos_common delegates to the real implementation instead of stubbing.
    threading::RegisterAsyncTaskRun();

    SetRuntimeMode(RuntimeMode::Aot);
    *out_runtime_state = runtime_state;
    return CHAOS_RUNTIME_STATUS_OK;
}

void CHAOS_RUNTIME_ABI_CALL RuntimeShutdown(RuntimeState* runtime_state) {
    if (runtime_state == nullptr) return;

    SetRuntimeMode(RuntimeMode::Aot);
    if (runtime_state->internal_state != nullptr) {
        runtime_state->internal_state->~RuntimeInternalState();
        FreeBytes(runtime_state->config, runtime_state->internal_state);
        runtime_state->internal_state = nullptr;
    }
}

RuntimeStatus CHAOS_RUNTIME_ABI_CALL ThreadAttach(
    RuntimeState* runtime_state,
    ThreadState** out_thread_state) {
    if (runtime_state == nullptr || out_thread_state == nullptr) {
        return CHAOS_RUNTIME_STATUS_INVALID_ARGUMENT;
    }

    *out_thread_state = nullptr;
    ThreadState* thread_state = static_cast<ThreadState*>(AllocateBytes(runtime_state->config, sizeof(ThreadState)));
    if (thread_state == nullptr) {
        return CHAOS_RUNTIME_STATUS_INTERNAL_ERROR;
    }

    thread_state->runtime_state = runtime_state;

    void* internal_mem = AllocateBytes(runtime_state->config, sizeof(ThreadInternalState));
    if (internal_mem == nullptr) {
        FreeBytes(runtime_state->config, thread_state);
        return CHAOS_RUNTIME_STATUS_INTERNAL_ERROR;
    }

    thread_state->internal_state = ::new (internal_mem) ThreadInternalState();

    // Register approximate thread stack bounds for conservative GC scanning.
    // Use address of a local as near-CFP reference (stack grows downward).
    void* stack_near_cfp = &thread_state;
    void* stack_limit = static_cast<char*>(stack_near_cfp) - (1024 * 1024);
    g_old_gen.RegisterThreadStack(stack_near_cfp, stack_limit);

    *out_thread_state = thread_state;
    SetCurrentThreadState(thread_state);

    threading::RegisterThread(threading::kMainThreadId, nullptr);

    return CHAOS_RUNTIME_STATUS_OK;
}

void CHAOS_RUNTIME_ABI_CALL ThreadDetach(
    RuntimeState* runtime_state,
    ThreadState* thread_state) {
    if (runtime_state == nullptr || thread_state == nullptr) return;

    g_old_gen.UnregisterThreadStack();

    if (thread_state->internal_state != nullptr) {
        thread_state->internal_state->~ThreadInternalState();
        FreeBytes(runtime_state->config, thread_state->internal_state);
        thread_state->internal_state = nullptr;
    }
    SetCurrentThreadState(nullptr);
    threading::UnregisterThread();
    FreeBytes(runtime_state->config, thread_state);
}

}  // namespace chaos::il2cpp::runtime_core