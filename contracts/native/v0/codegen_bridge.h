#ifndef CHAOS_CODEGEN_BRIDGE_H_
#define CHAOS_CODEGEN_BRIDGE_H_

#include "runtime_abi.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CHAOS_CODEGEN_BRIDGE_V0 0u

/* High-level status codes returned by generated-code helpers. Deferred helpers may return NOT_SUPPORTED. */
typedef int32_t BridgeStatus;

enum {
    CHAOS_BRIDGE_STATUS_OK = 0,
    CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT = 1,
    CHAOS_BRIDGE_STATUS_NOT_READY = 2,
    CHAOS_BRIDGE_STATUS_NOT_FOUND = 3,
    CHAOS_BRIDGE_STATUS_NOT_SUPPORTED = 4,
    CHAOS_BRIDGE_STATUS_MANAGED_EXCEPTION = 5,
    CHAOS_BRIDGE_STATUS_INTERNAL_ERROR = 6
};

typedef struct CodeRegistrationV0 {
    uint32_t struct_size;
    const void* method_pointers;
    uint32_t method_pointer_count;
    const void* reverse_pinvoke_wrappers;
    uint32_t reverse_pinvoke_wrapper_count;
    const void* invoker_pointers;
    uint32_t invoker_pointer_count;
    const void* unresolved_virtual_calls;
    uint32_t unresolved_virtual_call_count;
} CodeRegistrationV0;

typedef struct MetadataRegistrationV0 {
    uint32_t struct_size;
    const void* generic_types;
    uint32_t generic_type_count;
    const void* generic_methods;
    uint32_t generic_method_count;
    const void* field_offsets;
    uint32_t field_offset_count;
    const void* metadata_usages;
    uint32_t metadata_usage_count;
} MetadataRegistrationV0;

typedef struct CodegenRegistrationOptionsV0 {
    uint32_t struct_size;
    uint32_t registration_flags;
    const char* image_name_utf8;
} CodegenRegistrationOptionsV0;

/*
 * Process-wide helper surface consumed by generated C++.
 * Query helpers return null or status codes, while managed semantic failures
 * surface through controlled exception exits.
 */
typedef struct CodegenBridgeV0 {
    uint32_t abi_version;
    uint32_t struct_size;

    /* Registration and bootstrap. */
    BridgeStatus (CHAOS_RUNTIME_ABI_CALL* register_codegen)(
        const CodeRegistrationV0* code_registration,
        const MetadataRegistrationV0* metadata_registration,
        const CodegenRegistrationOptionsV0* options);
    BridgeStatus (CHAOS_RUNTIME_ABI_CALL* bootstrap_runtime)(void);

    /* Runtime metadata helpers. */
    TypeInfoHandle (CHAOS_RUNTIME_ABI_CALL* resolve_type_by_token)(
        ImageHandle image,
        uint32_t type_token);
    MethodInfoHandle (CHAOS_RUNTIME_ABI_CALL* resolve_method_by_token)(
        ImageHandle image,
        uint32_t method_token);
    FieldInfoHandle (CHAOS_RUNTIME_ABI_CALL* resolve_field_by_token)(
        ImageHandle image,
        uint32_t field_token);

    /* Boxing helpers. */
    void* (CHAOS_RUNTIME_ABI_CALL* box_value)(
        RuntimeState* runtime_state,
        ThreadState* thread_state,
        TypeInfoHandle value_type,
        const void* value,
        size_t value_size);
    BridgeStatus (CHAOS_RUNTIME_ABI_CALL* unbox_value)(
        RuntimeState* runtime_state,
        void* boxed_object,
        void* out_value,
        size_t out_value_size);

    /* Virtual dispatch helpers. */
    MethodInfoHandle (CHAOS_RUNTIME_ABI_CALL* resolve_virtual_method)(
        TypeInfoHandle instance_type,
        MethodInfoHandle declared_method);
    BridgeStatus (CHAOS_RUNTIME_ABI_CALL* invoke_virtual)(
        RuntimeState* runtime_state,
        ThreadState* thread_state,
        void* object_instance,
        MethodInfoHandle method,
        void* const* argv,
        uint32_t argc,
        void* out_return_value,
        size_t out_return_value_size,
        ExceptionHandle* out_exception);

    /* Delegate helpers. */
    void* (CHAOS_RUNTIME_ABI_CALL* create_delegate)(
        RuntimeState* runtime_state,
        ThreadState* thread_state,
        MethodInfoHandle method,
        void* target_instance);
    BridgeStatus (CHAOS_RUNTIME_ABI_CALL* delegate_invoke)(
        RuntimeState* runtime_state,
        ThreadState* thread_state,
        void* delegate_instance,
        void* const* argv,
        uint32_t argc,
        void* out_return_value,
        size_t out_return_value_size,
        ExceptionHandle* out_exception);

    /* Internal call resolution. */
    void* (CHAOS_RUNTIME_ABI_CALL* resolve_icall)(const char* icall_name_utf8);
} CodegenBridgeV0;

/* Returns the process-wide v0 bridge table or null when unavailable. */
CHAOS_RUNTIME_ABI_EXPORT const CodegenBridgeV0* CHAOS_RUNTIME_ABI_CALL chaos_codegen_get_bridge_v0(void);

#ifdef __cplusplus
}
#endif

#endif  // CHAOS_CODEGEN_BRIDGE_H_

