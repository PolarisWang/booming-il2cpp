#ifndef CHAOS_CODEGEN_BRIDGE_H_
#define CHAOS_CODEGEN_BRIDGE_H_

#include "runtime_abi.h"

#ifdef __cplusplus
#include <cstdint>
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
    CHAOS_BRIDGE_STATUS_INTERNAL_ERROR = 6,
    CHAOS_BRIDGE_STATUS_METADATA_RESOLUTION_FAILED = 7,
    CHAOS_BRIDGE_STATUS_CLASS_INIT_FAILED = 8,
    CHAOS_BRIDGE_STATUS_RUNTIME_CALL_FAILED = 9
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
    const RuntimeTypeCapabilityEntryV0* type_capabilities;
    uint32_t type_capability_count;
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
 * Assembly-bound native-reference dispatch request consumed by
 * RunNativeReferenceAssembly-style generated entry points.
 */
typedef struct NativeReferenceAssemblyDispatchRequestV0 {
    const char* subject_id_utf8;
    void* managed_args;
    uint32_t method_id;
} NativeReferenceAssemblyDispatchRequestV0;

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
    BridgeStatus (CHAOS_RUNTIME_ABI_CALL* query_type_capability)(
        TypeInfoHandle type,
        RuntimeTypeCapabilityInfoV0* out_capability_info);
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

    /* Virtual dispatch helpers (token-based, for AOT codegen). */
    MethodInfoHandle (CHAOS_RUNTIME_ABI_CALL* resolve_virtual_method_by_token)(
        uint32_t instance_type_token,
        uint32_t declared_method_token);

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

/* ── StringId tag bit helpers ─────────────────────────────────────────
 *
 * StringId values on the native eval stack use bit 63 as a tag to
 * distinguish them from heap object pointers.  AMD64 and ARM64 user-space
 * addresses occupy at most 48 bits, so bit 63 is never set by legitimate
 * pointers.  The static_assert below confirms the pointer size invariant.
 *
 * StringId encoding:
 *   - uint64_t content hash (63 bits) | 1 (ensures non-zero)
 *   - Tagged value: CHAOS_STRING_ID_TAG | id
 */
#define CHAOS_STRING_ID_TAG (static_cast<intptr_t>(1) << (sizeof(intptr_t) * 8 - 1))

static_assert(sizeof(intptr_t) == sizeof(uint64_t),
    "CHAOS_STRING_ID_TAG requires 64-bit intptr_t so that bit 63 "
    "is never set by valid user-space addresses on AMD64 or ARM64.");

inline bool chaos_is_string_id(intptr_t v) noexcept
{
    return (v & CHAOS_STRING_ID_TAG) != 0;
}

inline uint64_t chaos_extract_string_id(intptr_t v) noexcept
{
    return static_cast<uint64_t>(v & ~CHAOS_STRING_ID_TAG);
}

inline intptr_t chaos_make_string_id_value(uint64_t id) noexcept
{
    return CHAOS_STRING_ID_TAG | static_cast<intptr_t>(id);
}

#ifdef __cplusplus
}
#endif

#endif  // CHAOS_CODEGEN_BRIDGE_H_

