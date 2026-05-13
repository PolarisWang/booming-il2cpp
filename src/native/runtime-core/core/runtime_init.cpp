namespace chaos::il2cpp::runtime_core {

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

    static CHAOS_IL2CPP_ONCE_FLAG s_gc_init_flag;
    CHAOS_IL2CPP_CALL_ONCE(s_gc_init_flag, []() { GC_INIT(); });

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

#if !defined(_WIN32) && !defined(_WIN64)
    struct GC_stack_base sb;
    if (GC_get_stack_base(&sb) == GC_SUCCESS) {
        const int gc_reg_result = GC_register_my_thread(&sb);
        (void)gc_reg_result;
    }
#endif

    *out_thread_state = thread_state;
    SetCurrentThreadState(thread_state);

    threading::RegisterThread(threading::kMainThreadId, nullptr);

    return CHAOS_RUNTIME_STATUS_OK;
}

void CHAOS_RUNTIME_ABI_CALL ThreadDetach(
    RuntimeState* runtime_state,
    ThreadState* thread_state) {
    if (runtime_state == nullptr || thread_state == nullptr) return;

#if !defined(_WIN32) && !defined(_WIN64)
    GC_unregister_my_thread();
#endif

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