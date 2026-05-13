namespace chaos::il2cpp::runtime_core {
namespace {

bool GcSafepoint(RuntimeState* runtime_state, ThreadState* thread_state) {
    if (!IsAttached(runtime_state, thread_state)) return false;

    auto* thread_internal_state = GetThreadInternalState(thread_state);
    if (thread_internal_state == nullptr) return false;

    thread_internal_state->at_gc_safepoint = true;
    GC_collect_a_little();
    thread_internal_state->at_gc_safepoint = false;
    return true;
}

CHAOS_IL2CPP_SIZE ReportThreadRoot(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    const void* root_address,
    CHAOS_IL2CPP_SIZE root_size) {
    if (!IsAttached(runtime_state, thread_state) || root_address == nullptr || root_size == 0u) return 0u;

    auto* thread_internal_state = GetThreadInternalState(thread_state);
    if (thread_internal_state == nullptr) return 0u;

    thread_internal_state->reported_roots.push_back(
        ThreadRootRecord{ root_address, root_size });
    return thread_internal_state->reported_roots.size();
}

bool EnqueueFinalizer(
    RuntimeState* runtime_state,
    void* object_instance,
    FinalizerCallback finalizer) {
    auto* runtime_internal_state = GetRuntimeInternalState(runtime_state);
    if (runtime_internal_state == nullptr || object_instance == nullptr || finalizer == nullptr) {
        return false;
    }

    {
        CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(runtime_internal_state->finalizer_queue_mutex);
        runtime_internal_state->finalizer_queue.push_back(
            FinalizerWorkItem{ object_instance, finalizer });
    }

    GC_register_finalizer_no_order(
        object_instance,
        [](void* obj, void* client_data) {
            auto* cb = reinterpret_cast<FinalizerCallback>(client_data);
            if (cb) { cb(obj); }
        },
        reinterpret_cast<void*>(finalizer),
        nullptr, nullptr);

    return true;
}

CHAOS_IL2CPP_SIZE DrainFinalizerQueue(RuntimeState* runtime_state) {
    auto* runtime_internal_state = GetRuntimeInternalState(runtime_state);
    if (runtime_internal_state == nullptr) return 0u;

    GC_invoke_finalizers();

    CHAOS_IL2CPP_VECTOR(FinalizerWorkItem) pending_finalizers = {};
    {
        CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(runtime_internal_state->finalizer_queue_mutex);
        pending_finalizers.swap(runtime_internal_state->finalizer_queue);
    }

    for (const auto& work_item : pending_finalizers) {
        work_item.finalizer(work_item.object_instance);
    }

    return pending_finalizers.size();
}

}  // anonymous namespace
}  // namespace chaos::il2cpp::runtime_core