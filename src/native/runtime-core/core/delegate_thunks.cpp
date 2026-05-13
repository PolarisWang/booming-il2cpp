namespace chaos::il2cpp::runtime_core {
namespace {

struct DelegateThunkEntry {
    const char* type_id;
    void* thunk_fn;
    CHAOS_IL2CPP_INTPTR* target_slot;
};

static constexpr CHAOS_IL2CPP_INT32 kMaxDelegateThunks = 64;
static DelegateThunkEntry g_delegate_thunks[kMaxDelegateThunks];
static CHAOS_IL2CPP_INT32 g_delegate_thunk_count = 0;

void RegisterDelegateThunk(const char* type_id, void* thunk_fn, CHAOS_IL2CPP_INTPTR* target_slot) {
    if (type_id == nullptr || thunk_fn == nullptr || target_slot == nullptr) return;
    if (g_delegate_thunk_count >= kMaxDelegateThunks) return;

    for (CHAOS_IL2CPP_INT32 i = 0; i < g_delegate_thunk_count; i++) {
        if (std::strcmp(g_delegate_thunks[i].type_id, type_id) == 0) {
            g_delegate_thunks[i].thunk_fn = thunk_fn;
            g_delegate_thunks[i].target_slot = target_slot;
            return;
        }
    }

    g_delegate_thunks[g_delegate_thunk_count] = { type_id, thunk_fn, target_slot };
    g_delegate_thunk_count++;
}

void* FindDelegateThunk(const char* type_id) {
    if (type_id == nullptr) return nullptr;
    for (CHAOS_IL2CPP_INT32 i = 0; i < g_delegate_thunk_count; i++) {
        if (std::strcmp(g_delegate_thunks[i].type_id, type_id) == 0)
            return g_delegate_thunks[i].thunk_fn;
    }
    return nullptr;
}

void* MarshalGetFunctionPointerForDelegateImpl(
    RuntimeState* runtime_state, ThreadState* thread_state,
    CHAOS_IL2CPP_INTPTR delegate_obj, const char* delegate_type_id) {
    (void)thread_state;
    if (runtime_state == nullptr || delegate_obj == static_cast<CHAOS_IL2CPP_INTPTR>(0) || delegate_type_id == nullptr)
        return nullptr;

    for (CHAOS_IL2CPP_INT32 i = 0; i < g_delegate_thunk_count; i++) {
        if (std::strcmp(g_delegate_thunks[i].type_id, delegate_type_id) == 0) {
            if (g_delegate_thunks[i].target_slot != nullptr)
                *g_delegate_thunks[i].target_slot = delegate_obj;
            return g_delegate_thunks[i].thunk_fn;
        }
    }
    return nullptr;
}

void* MarshalGetDelegateForFunctionPointerImpl(
    RuntimeState* runtime_state, ThreadState* thread_state,
    CHAOS_IL2CPP_INTPTR native_fn_ptr, const char* delegate_type_id) {
    (void)runtime_state; (void)thread_state; (void)native_fn_ptr; (void)delegate_type_id;
    return nullptr;
}

}  // anonymous namespace
}  // namespace chaos::il2cpp::runtime_core