namespace chaos::il2cpp::runtime_core {
namespace {

CHAOS_IL2CPP_INT32 EngineLogWrite(
    const char* category_utf8,
    const char* message_utf8) {
    (void)category_utf8;

    if (message_utf8 == nullptr) return 1;

    CHAOS_IL2CPP_LOG_WRITE_RAW_M("{0}{1}\n", kEngineObservePrefix, message_utf8);
    CHAOS_IL2CPP_LOG_FLUSH_STDOUT();
    return 0;
}

CHAOS_IL2CPP_UINTPTR CreateEngineObjectHandle(void* object_instance) {
    if (object_instance == nullptr) return 0u;

    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(g_engine_binding_mutex);
    const CHAOS_IL2CPP_UINTPTR handle = g_next_engine_handle++;
    g_engine_handles[handle] = object_instance;
    return handle;
}

void* ResolveEngineObjectHandle(CHAOS_IL2CPP_UINTPTR handle) {
    if (handle == 0u) return nullptr;

    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(g_engine_binding_mutex);
    const auto iterator = g_engine_handles.find(handle);
    return iterator != g_engine_handles.end() ? iterator->second : nullptr;
}

bool RegisterEngineLifecycleCallback(
    const char* phase_utf8,
    EngineLifecycleCallback callback,
    void* user_data) {
    if (phase_utf8 == nullptr || callback == nullptr) return false;

    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(g_engine_binding_mutex);
    g_engine_lifecycle_registrations.push_back(
        EngineLifecycleRegistration{ phase_utf8, callback, user_data });
    return true;
}

bool DispatchEngineLifecycleCallbacks(const char* phase_utf8) {
    if (phase_utf8 == nullptr) return false;

    CHAOS_IL2CPP_VECTOR(EngineLifecycleRegistration) callbacks = {};
    {
        CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(g_engine_binding_mutex);
        for (const auto& registration : g_engine_lifecycle_registrations) {
            if (registration.phase == phase_utf8) {
                callbacks.push_back(registration);
            }
        }
    }

    for (const auto& registration : callbacks) {
        registration.callback(phase_utf8, registration.user_data);
    }

    return !callbacks.empty();
}

bool IsMainThreadLane() {
    return CHAOS_IL2CPP_THIS_THREAD_GET_ID() == g_main_thread_id;
}

bool ThreadStaticInt32Add(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    const char* slot_key_utf8,
    CHAOS_IL2CPP_INT32 delta,
    CHAOS_IL2CPP_INT32* out_value) {
    if (!IsAttached(runtime_state, thread_state) || slot_key_utf8 == nullptr || out_value == nullptr) {
        return false;
    }

    auto* thread_internal_state = GetThreadInternalState(thread_state);
    if (thread_internal_state == nullptr) return false;

    CHAOS_IL2CPP_INT32& value = thread_internal_state->thread_static_int32_slots[slot_key_utf8];
    value += delta;
    *out_value = value;
    return true;
}

}  // anonymous namespace
}  // namespace chaos::il2cpp::runtime_core