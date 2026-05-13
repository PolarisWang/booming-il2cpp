namespace chaos::il2cpp::runtime_core {

thread_local RuntimeState* g_tls_runtime_state = nullptr;

void SetCurrentRuntimeState(RuntimeState* runtime_state) {
    g_tls_runtime_state = runtime_state;
}

RuntimeState* GetCurrentRuntimeState() {
    return g_tls_runtime_state;
}

thread_local ThreadState* g_tls_thread_state = nullptr;

void SetCurrentThreadState(ThreadState* thread_state) {
    g_tls_thread_state = thread_state;
}

ThreadState* GetCurrentThreadState() {
    return g_tls_thread_state;
}

}  // namespace chaos::il2cpp::runtime_core