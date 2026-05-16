#include "engine_bridge.h"

#include "runtime_core.h"
#include "bootstrap.h"
#include "memory_domain.h"

#include <mutex>
#include <new>
#include <string>
#include <chaos/unordered_dense.h>

namespace {

struct EngineCallbackRegistration {
    CHAOS_IL2CPP_STRING name = {};
    engine_callback_fn_t callback = nullptr;
    void* user_data = nullptr;
};

struct EngineOwnershipRecord {
    CHAOS_IL2CPP_UINT32 weak_count = 0u;
    CHAOS_IL2CPP_UINT32 strong_count = 0u;
    CHAOS_IL2CPP_STRING owner = {};
};

}  // namespace

struct engine_runtime_context_t {
    const RuntimeAbiV0* abi = nullptr;
    RuntimeState* runtime = nullptr;
    ThreadState* thread = nullptr;
    CHAOS_IL2CPP_MUTEX mutex = {};
    engine_callback_id_t next_callback_id = 1u;
    CHAOS_IL2CPP_UNORDERED_DENSE_MAP(engine_callback_id_t, EngineCallbackRegistration) callbacks = {};
    CHAOS_IL2CPP_UNORDERED_DENSE_MAP(engine_object_ref_t, EngineOwnershipRecord) ownership = {};
};

namespace {

engine_status_t TranslateRuntimeStatus(RuntimeStatus status) {
    switch (status) {
        case CHAOS_RUNTIME_STATUS_OK:
            return ENGINE_STATUS_OK;
        case CHAOS_RUNTIME_STATUS_INVALID_ARGUMENT:
        case CHAOS_RUNTIME_STATUS_NOT_FOUND:
            return ENGINE_STATUS_INVALID_ARGUMENT;
        case CHAOS_RUNTIME_STATUS_NOT_SUPPORTED:
            return ENGINE_STATUS_NOT_SUPPORTED;
        case CHAOS_RUNTIME_STATUS_INTERNAL_ERROR:
        case CHAOS_RUNTIME_STATUS_MANAGED_EXCEPTION:
        default:
            return ENGINE_STATUS_INTERNAL_ERROR;
    }
}

bool IsRecognizedHandleKind(engine_handle_kind_t handle_kind) {
    return handle_kind == ENGINE_HANDLE_KIND_WEAK || handle_kind == ENGINE_HANDLE_KIND_STRONG;
}

EngineOwnershipRecord* TryFindOwnershipRecord(
    engine_runtime_context_t* runtime,
    engine_object_ref_t object_ref) {
    if (runtime == nullptr || object_ref == 0u) {
        return nullptr;
    }

    auto iterator = runtime->ownership.find(object_ref);
    return iterator != runtime->ownership.end() ? &iterator->second : nullptr;
}

engine_status_t DispatchLifecyclePhase(
    engine_runtime_context_t* runtime,
    const char* phase_utf8) {
    if (runtime == nullptr || phase_utf8 == nullptr) {
        return ENGINE_STATUS_INVALID_ARGUMENT;
    }

    (void)chaos::il2cpp::runtime_core::IsMainThreadLane();
    (void)chaos::il2cpp::runtime_core::DispatchEngineLifecycleCallbacks(phase_utf8);
    return ENGINE_STATUS_OK;
}

}  // namespace

/// Depth of the AOT domain push on the engine thread's TLS domain stack.
/// Saved so engine_shutdown can correctly unwind to the pre-init depth.
static int g_engine_domain_depth = -1;

engine_status_t engine_init(
    engine_runtime_context_t** out_runtime,
    const engine_init_params_t* init_params) {
    if (out_runtime == nullptr || init_params == nullptr || init_params->struct_size < sizeof(engine_init_params_t)) {
        return ENGINE_STATUS_INVALID_ARGUMENT;
    }

    *out_runtime = nullptr;
    const RuntimeAbiV0* abi = chaos_runtime_get_abi_v0();
    if (abi == nullptr) {
        return ENGINE_STATUS_INTERNAL_ERROR;
    }

    auto* runtime = new (CHAOS_IL2CPP_NOTROW) engine_runtime_context_t();
    if (runtime == nullptr) {
        return ENGINE_STATUS_INTERNAL_ERROR;
    }

    runtime->abi = abi;

    RuntimeInitParams runtime_init_params = {};
    runtime_init_params.struct_size = sizeof(runtime_init_params);
    runtime_init_params.host_name_utf8 =
        init_params->host_name_utf8 != nullptr ? init_params->host_name_utf8 : "engine-bridge";
    runtime_init_params.runtime_tag_utf8 = "engine-bridge-v0";

    RuntimeStatus status = abi->runtime_init(&runtime_init_params, nullptr, &runtime->runtime);
    if (status != CHAOS_RUNTIME_STATUS_OK || runtime->runtime == nullptr) {
        delete runtime;
        return TranslateRuntimeStatus(status);
    }

    status = abi->thread_attach(runtime->runtime, &runtime->thread);
    if (status != CHAOS_RUNTIME_STATUS_OK || runtime->thread == nullptr) {
        abi->runtime_shutdown(runtime->runtime);
        delete runtime;
        return TranslateRuntimeStatus(status);
    }

    *out_runtime = runtime;

    // Push the AOT root memory domain so marshal allocations during engine
    // callbacks are attributed to the AOT domain.  Saved depth is used in
    // engine_shutdown to unwind correctly.
    namespace md = chaos::il2cpp::memory_domain;
    auto* aot_domain = md::FindDomainById(chaos::il2cpp::bootstrap::GetAotDomainId());
    if (aot_domain != nullptr) {
        g_engine_domain_depth = md::PushDomain(aot_domain);
    }

    return DispatchLifecyclePhase(runtime, "init");
}

void engine_shutdown(engine_runtime_context_t* runtime) {
    if (runtime == nullptr || runtime->abi == nullptr) {
        return;
    }

    (void)DispatchLifecyclePhase(runtime, "shutdown");

    // Pop the AOT domain push from engine_init, restoring the TLS domain
    // stack to its pre-init state.
    if (g_engine_domain_depth >= 0) {
        chaos::il2cpp::memory_domain::PopDomain(g_engine_domain_depth);
        g_engine_domain_depth = -1;
    }

    if (runtime->runtime != nullptr && runtime->thread != nullptr) {
        runtime->abi->thread_detach(runtime->runtime, runtime->thread);
        runtime->thread = nullptr;
    }

    if (runtime->runtime != nullptr) {
        runtime->abi->runtime_shutdown(runtime->runtime);
        runtime->runtime = nullptr;
    }

    delete runtime;
}

engine_status_t engine_reload(engine_runtime_context_t* runtime) {
    return DispatchLifecyclePhase(runtime, "reload");
}

engine_status_t engine_scene_attach(
    engine_runtime_context_t* runtime,
    const char* scene_name_utf8) {
    if (scene_name_utf8 == nullptr || scene_name_utf8[0] == '\0') {
        return ENGINE_STATUS_INVALID_ARGUMENT;
    }

    return DispatchLifecyclePhase(runtime, "scene-attach");
}

engine_status_t engine_retain_object(
    engine_runtime_context_t* runtime,
    engine_object_ref_t object_ref,
    engine_handle_kind_t handle_kind) {
    if (runtime == nullptr || object_ref == 0u || !IsRecognizedHandleKind(handle_kind)) {
        return ENGINE_STATUS_INVALID_ARGUMENT;
    }

    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(runtime->mutex);
    EngineOwnershipRecord& record = runtime->ownership[object_ref];
    if (handle_kind == ENGINE_HANDLE_KIND_STRONG) {
        ++record.strong_count;
    } else {
        ++record.weak_count;
    }

    return ENGINE_STATUS_OK;
}

engine_status_t engine_release_object(
    engine_runtime_context_t* runtime,
    engine_object_ref_t object_ref,
    engine_handle_kind_t handle_kind) {
    if (runtime == nullptr || object_ref == 0u || !IsRecognizedHandleKind(handle_kind)) {
        return ENGINE_STATUS_INVALID_ARGUMENT;
    }

    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(runtime->mutex);
    EngineOwnershipRecord* record = TryFindOwnershipRecord(runtime, object_ref);
    if (record == nullptr) {
        return ENGINE_STATUS_INVALID_ARGUMENT;
    }

    CHAOS_IL2CPP_UINT32* counter = handle_kind == ENGINE_HANDLE_KIND_STRONG
        ? &record->strong_count
        : &record->weak_count;
    if (*counter == 0u) {
        return ENGINE_STATUS_INVALID_ARGUMENT;
    }

    --(*counter);
    if (record->weak_count == 0u && record->strong_count == 0u) {
        runtime->ownership.erase(object_ref);
    }

    return ENGINE_STATUS_OK;
}

engine_status_t engine_transfer_ownership(
    engine_runtime_context_t* runtime,
    engine_object_ref_t object_ref,
    const char* from_owner_utf8,
    const char* to_owner_utf8) {
    if (runtime == nullptr
        || object_ref == 0u
        || from_owner_utf8 == nullptr
        || to_owner_utf8 == nullptr
        || to_owner_utf8[0] == '\0') {
        return ENGINE_STATUS_INVALID_ARGUMENT;
    }

    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(runtime->mutex);
    EngineOwnershipRecord* record = TryFindOwnershipRecord(runtime, object_ref);
    if (record == nullptr) {
        return ENGINE_STATUS_INVALID_ARGUMENT;
    }

    if (!record->owner.empty() && record->owner != from_owner_utf8) {
        return ENGINE_STATUS_INVALID_ARGUMENT;
    }

    record->owner = to_owner_utf8;
    return ENGINE_STATUS_OK;
}

engine_status_t engine_register_callback(
    engine_runtime_context_t* runtime,
    const char* callback_name_utf8,
    engine_callback_fn_t callback,
    void* user_data,
    engine_callback_id_t* out_callback_id) {
    if (runtime == nullptr
        || callback_name_utf8 == nullptr
        || callback_name_utf8[0] == '\0'
        || callback == nullptr
        || out_callback_id == nullptr) {
        return ENGINE_STATUS_INVALID_ARGUMENT;
    }

    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(runtime->mutex);
    const engine_callback_id_t callback_id = runtime->next_callback_id++;
    runtime->callbacks.emplace(
        callback_id,
        EngineCallbackRegistration
        {
            callback_name_utf8,
            callback,
            user_data,
        });
    *out_callback_id = callback_id;
    return ENGINE_STATUS_OK;
}

engine_status_t engine_dispatch_callback(
    engine_runtime_context_t* runtime,
    engine_callback_id_t callback_id,
    const void* payload,
    CHAOS_IL2CPP_SIZE payload_size) {
    if (runtime == nullptr || callback_id == 0u) {
        return ENGINE_STATUS_INVALID_ARGUMENT;
    }

    EngineCallbackRegistration registration = {};
    {
        CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(runtime->mutex);
        const auto iterator = runtime->callbacks.find(callback_id);
        if (iterator == runtime->callbacks.end()) {
            return ENGINE_STATUS_INVALID_ARGUMENT;
        }

        registration = iterator->second;
    }

    return registration.callback(
        registration.name.c_str(),
        payload,
        payload_size,
        registration.user_data);
}
