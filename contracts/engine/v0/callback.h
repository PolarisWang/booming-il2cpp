#ifndef CHAOS_IL2CPP_ENGINE_V0_CALLBACK_H_
#define CHAOS_IL2CPP_ENGINE_V0_CALLBACK_H_

#include <stddef.h>
#include <stdint.h>

#include "lifecycle.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uintptr_t engine_callback_id_t;

typedef engine_status_t (*engine_callback_fn_t)(
    const char* callback_name_utf8,
    const void* payload,
    size_t payload_size,
    void* user_data);

engine_status_t engine_register_callback(
    engine_runtime_context_t* runtime,
    const char* callback_name_utf8,
    engine_callback_fn_t callback,
    void* user_data,
    engine_callback_id_t* out_callback_id);
engine_status_t engine_dispatch_callback(
    engine_runtime_context_t* runtime,
    engine_callback_id_t callback_id,
    const void* payload,
    size_t payload_size);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // CHAOS_IL2CPP_ENGINE_V0_CALLBACK_H_
