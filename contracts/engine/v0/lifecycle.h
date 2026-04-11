#ifndef CHAOS_IL2CPP_ENGINE_V0_LIFECYCLE_H_
#define CHAOS_IL2CPP_ENGINE_V0_LIFECYCLE_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct engine_runtime_context_t engine_runtime_context_t;

typedef enum engine_status_t {
    ENGINE_STATUS_OK = 0,
    ENGINE_STATUS_INVALID_ARGUMENT = 1,
    ENGINE_STATUS_NOT_SUPPORTED = 2,
    ENGINE_STATUS_INTERNAL_ERROR = 3,
} engine_status_t;

typedef struct engine_init_params_t {
    uint32_t struct_size;
    const char* host_name_utf8;
    void* user_data;
} engine_init_params_t;

engine_status_t engine_init(
    engine_runtime_context_t** out_runtime,
    const engine_init_params_t* init_params);
void engine_shutdown(engine_runtime_context_t* runtime);
engine_status_t engine_reload(engine_runtime_context_t* runtime);
engine_status_t engine_scene_attach(
    engine_runtime_context_t* runtime,
    const char* scene_name_utf8);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // CHAOS_IL2CPP_ENGINE_V0_LIFECYCLE_H_
