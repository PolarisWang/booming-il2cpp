#ifndef BOOM_CODEGEN_BRIDGE_H_
#define BOOM_CODEGEN_BRIDGE_H_

#include "boom_runtime_abi.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BOOM_CODEGEN_BRIDGE_V0 0u

/* High-level status codes returned by generated-code helpers. */
typedef enum BoomBridgeStatus {
    BOOM_BRIDGE_STATUS_OK = 0,
    BOOM_BRIDGE_STATUS_INVALID_ARGUMENT = 1,
    BOOM_BRIDGE_STATUS_NOT_READY = 2,
    BOOM_BRIDGE_STATUS_NOT_FOUND = 3,
    BOOM_BRIDGE_STATUS_MANAGED_EXCEPTION = 4,
    BOOM_BRIDGE_STATUS_INTERNAL_ERROR = 5
} BoomBridgeStatus;

typedef struct BoomCodeRegistrationV0 {
    uint32_t struct_size;
    const void* method_pointers;
    uint32_t method_pointer_count;
    const void* reverse_pinvoke_wrappers;
    uint32_t reverse_pinvoke_wrapper_count;
    const void* invoker_pointers;
    uint32_t invoker_pointer_count;
    const void* unresolved_virtual_calls;
    uint32_t unresolved_virtual_call_count;
} BoomCodeRegistrationV0;

typedef struct BoomMetadataRegistrationV0 {
    uint32_t struct_size;
    const void* generic_types;
    uint32_t generic_type_count;
    const void* generic_methods;
    uint32_t generic_method_count;
    const void* field_offsets;
    uint32_t field_offset_count;
    const void* metadata_usages;
    uint32_t metadata_usage_count;
} BoomMetadataRegistrationV0;

typedef struct BoomCodegenRegistrationOptionsV0 {
    uint32_t struct_size;
    uint32_t registration_flags;
    const char* image_name_utf8;
} BoomCodegenRegistrationOptionsV0;

/*
 * Process-wide helper surface consumed by generated C++.
 * Query helpers return null or status codes, while managed semantic failures
 * surface through controlled exception exits.
 */
typedef struct BoomCodegenBridgeV0 {
    uint32_t abi_version;
    uint32_t struct_size;

    /* Registration and bootstrap. */
    BoomBridgeStatus (BOOM_RUNTIME_ABI_CALL* register_codegen)(
        const BoomCodeRegistrationV0* code_registration,
        const BoomMetadataRegistrationV0* metadata_registration,
        const BoomCodegenRegistrationOptionsV0* options);
    BoomBridgeStatus (BOOM_RUNTIME_ABI_CALL* bootstrap_runtime)(void);

    /* Runtime metadata helpers. */
    BoomTypeInfoHandle (BOOM_RUNTIME_ABI_CALL* resolve_type_by_token)(
        BoomImageHandle image,
        uint32_t type_token);
    BoomMethodInfoHandle (BOOM_RUNTIME_ABI_CALL* resolve_method_by_token)(
        BoomImageHandle image,
        uint32_t method_token);
    BoomFieldInfoHandle (BOOM_RUNTIME_ABI_CALL* resolve_field_by_token)(
        BoomImageHandle image,
        uint32_t field_token);

    /* Boxing helpers. */
    void* (BOOM_RUNTIME_ABI_CALL* box_value)(
        BoomRuntimeState* runtime_state,
        BoomThreadState* thread_state,
        BoomTypeInfoHandle value_type,
        const void* value,
        size_t value_size);
    BoomBridgeStatus (BOOM_RUNTIME_ABI_CALL* unbox_value)(
        BoomRuntimeState* runtime_state,
        void* boxed_object,
        void* out_value,
        size_t out_value_size);

    /* Virtual dispatch helpers. */
    BoomMethodInfoHandle (BOOM_RUNTIME_ABI_CALL* resolve_virtual_method)(
        BoomTypeInfoHandle instance_type,
        BoomMethodInfoHandle declared_method);
    BoomBridgeStatus (BOOM_RUNTIME_ABI_CALL* invoke_virtual)(
        BoomRuntimeState* runtime_state,
        BoomThreadState* thread_state,
        void* object_instance,
        BoomMethodInfoHandle method,
        void* const* argv,
        uint32_t argc,
        void* out_return_value,
        size_t out_return_value_size,
        BoomExceptionHandle* out_exception);

    /* Delegate helpers. */
    void* (BOOM_RUNTIME_ABI_CALL* create_delegate)(
        BoomRuntimeState* runtime_state,
        BoomThreadState* thread_state,
        BoomMethodInfoHandle method,
        void* target_instance);
    BoomBridgeStatus (BOOM_RUNTIME_ABI_CALL* delegate_invoke)(
        BoomRuntimeState* runtime_state,
        BoomThreadState* thread_state,
        void* delegate_instance,
        void* const* argv,
        uint32_t argc,
        void* out_return_value,
        size_t out_return_value_size,
        BoomExceptionHandle* out_exception);

    /* Internal call resolution. */
    void* (BOOM_RUNTIME_ABI_CALL* resolve_icall)(const char* icall_name_utf8);
} BoomCodegenBridgeV0;

/* Returns the process-wide v0 bridge table or null when unavailable. */
BOOM_RUNTIME_ABI_EXPORT const BoomCodegenBridgeV0* BOOM_RUNTIME_ABI_CALL boom_codegen_get_bridge_v0(void);

#ifdef __cplusplus
}
#endif

#endif  // BOOM_CODEGEN_BRIDGE_H_
