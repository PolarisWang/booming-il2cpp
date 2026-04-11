#ifndef CHAOS_IL2CPP_ENGINE_V0_OWNERSHIP_H_
#define CHAOS_IL2CPP_ENGINE_V0_OWNERSHIP_H_

#include <stdint.h>

#include "lifecycle.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uintptr_t engine_object_ref_t;

typedef enum engine_handle_kind_t {
    ENGINE_HANDLE_KIND_WEAK = 1,
    ENGINE_HANDLE_KIND_STRONG = 2,
} engine_handle_kind_t;

engine_status_t engine_retain_object(
    engine_runtime_context_t* runtime,
    engine_object_ref_t object_ref,
    engine_handle_kind_t handle_kind);
engine_status_t engine_release_object(
    engine_runtime_context_t* runtime,
    engine_object_ref_t object_ref,
    engine_handle_kind_t handle_kind);
engine_status_t engine_transfer_ownership(
    engine_runtime_context_t* runtime,
    engine_object_ref_t object_ref,
    const char* from_owner_utf8,
    const char* to_owner_utf8);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // CHAOS_IL2CPP_ENGINE_V0_OWNERSHIP_H_
