#ifndef BOOM_RUNTIME_ABI_H_
#define BOOM_RUNTIME_ABI_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32)
#if defined(BOOM_RUNTIME_ABI_EXPORTS)
#define BOOM_RUNTIME_ABI_EXPORT __declspec(dllexport)
#else
#define BOOM_RUNTIME_ABI_EXPORT __declspec(dllimport)
#endif
#define BOOM_RUNTIME_ABI_CALL __cdecl
#else
#define BOOM_RUNTIME_ABI_EXPORT __attribute__((visibility("default")))
#define BOOM_RUNTIME_ABI_CALL
#endif

#define BOOM_RUNTIME_ABI_V0 0u
#define BOOM_GC_HANDLE_INVALID ((BoomGCHandle)0u)

/* Opaque reflection and runtime handles carried across the ABI boundary. */
typedef struct BoomTypeInfoOpaque* BoomTypeInfoHandle;
typedef struct BoomMethodInfoOpaque* BoomMethodInfoHandle;
typedef struct BoomFieldInfoOpaque* BoomFieldInfoHandle;
typedef struct BoomPropertyInfoOpaque* BoomPropertyInfoHandle;
typedef struct BoomEventInfoOpaque* BoomEventInfoHandle;
typedef struct BoomParameterInfoOpaque* BoomParameterInfoHandle;
typedef struct BoomGenericContextOpaque* BoomGenericContextHandle;
typedef struct BoomAssemblyOpaque* BoomAssemblyHandle;
typedef struct BoomImageOpaque* BoomImageHandle;
typedef struct BoomExceptionOpaque* BoomExceptionHandle;

typedef struct BoomRuntimeState BoomRuntimeState;
typedef struct BoomThreadState BoomThreadState;

typedef uint64_t BoomGCHandle;

typedef enum BoomRuntimeStatus {
    BOOM_RUNTIME_STATUS_OK = 0,
    BOOM_RUNTIME_STATUS_INVALID_ARGUMENT = 1,
    BOOM_RUNTIME_STATUS_INVALID_STATE = 2,
    BOOM_RUNTIME_STATUS_NOT_FOUND = 3,
    BOOM_RUNTIME_STATUS_NOT_SUPPORTED = 4,
    BOOM_RUNTIME_STATUS_MANAGED_EXCEPTION = 5,
    BOOM_RUNTIME_STATUS_INTERNAL_ERROR = 6
} BoomRuntimeStatus;

typedef enum BoomRuntimeInitFlags {
    BOOM_RUNTIME_INIT_NONE = 0,
    BOOM_RUNTIME_INIT_ENABLE_LOGGING = 1 << 0,
    BOOM_RUNTIME_INIT_ENABLE_PROFILING = 1 << 1
} BoomRuntimeInitFlags;

/* Host-supplied allocators are optional and may be null. */
typedef void* (BOOM_RUNTIME_ABI_CALL* BoomAllocatorFn)(size_t size, void* user_data);
typedef void (BOOM_RUNTIME_ABI_CALL* BoomDeallocatorFn)(void* ptr, void* user_data);

typedef struct BoomRuntimeInitParams {
    uint32_t struct_size;
    uint32_t init_flags;
    const char* host_name_utf8;
    const char* runtime_tag_utf8;
    void* host_context;
} BoomRuntimeInitParams;

typedef struct BoomRuntimeConfig {
    uint32_t struct_size;
    BoomAllocatorFn allocator;
    BoomDeallocatorFn deallocator;
    void* allocator_user_data;
    uint32_t reserved_u32[4];
} BoomRuntimeConfig;

/*
 * Process-wide function table for the low-level runtime ABI.
 * Callers cache the table once per process and pass runtime/thread state
 * explicitly on every operation.
 */
typedef struct BoomRuntimeAbiV0 {
    uint32_t abi_version;
    uint32_t struct_size;

    /* Runtime lifecycle. */
    BoomRuntimeStatus (BOOM_RUNTIME_ABI_CALL* runtime_init)(
        const BoomRuntimeInitParams* init_params,
        const BoomRuntimeConfig* config,
        BoomRuntimeState** out_runtime_state);
    void (BOOM_RUNTIME_ABI_CALL* runtime_shutdown)(BoomRuntimeState* runtime_state);

    /* Thread lifecycle. */
    BoomRuntimeStatus (BOOM_RUNTIME_ABI_CALL* thread_attach)(
        BoomRuntimeState* runtime_state,
        BoomThreadState** out_thread_state);
    void (BOOM_RUNTIME_ABI_CALL* thread_detach)(
        BoomRuntimeState* runtime_state,
        BoomThreadState* thread_state);

    /* Allocation helpers. */
    void* (BOOM_RUNTIME_ABI_CALL* object_new)(
        BoomRuntimeState* runtime_state,
        BoomThreadState* thread_state,
        BoomTypeInfoHandle type);
    void* (BOOM_RUNTIME_ABI_CALL* array_new)(
        BoomRuntimeState* runtime_state,
        BoomThreadState* thread_state,
        BoomTypeInfoHandle element_type,
        uintptr_t length);
    void* (BOOM_RUNTIME_ABI_CALL* string_new_utf8)(
        BoomRuntimeState* runtime_state,
        BoomThreadState* thread_state,
        const char* utf8_bytes,
        uintptr_t byte_count);

    BoomRuntimeStatus (BOOM_RUNTIME_ABI_CALL* class_init)(
        BoomRuntimeState* runtime_state,
        BoomTypeInfoHandle type);

    /* GC handle helpers. */
    BoomGCHandle (BOOM_RUNTIME_ABI_CALL* gc_handle_new)(
        BoomRuntimeState* runtime_state,
        void* object_instance,
        bool pinned);
    void (BOOM_RUNTIME_ABI_CALL* gc_handle_free)(
        BoomRuntimeState* runtime_state,
        BoomGCHandle gc_handle);

    void (BOOM_RUNTIME_ABI_CALL* raise_managed_exception)(
        BoomRuntimeState* runtime_state,
        BoomThreadState* thread_state,
        BoomExceptionHandle exception);

    /* Field and method operations are handle-driven and explicitly sized. */
    BoomRuntimeStatus (BOOM_RUNTIME_ABI_CALL* field_get_value)(
        BoomRuntimeState* runtime_state,
        BoomThreadState* thread_state,
        BoomFieldInfoHandle field,
        void* object_instance,
        void* out_value,
        size_t out_value_size);
    BoomRuntimeStatus (BOOM_RUNTIME_ABI_CALL* field_set_value)(
        BoomRuntimeState* runtime_state,
        BoomThreadState* thread_state,
        BoomFieldInfoHandle field,
        void* object_instance,
        const void* value,
        size_t value_size);

    BoomRuntimeStatus (BOOM_RUNTIME_ABI_CALL* method_invoke)(
        BoomRuntimeState* runtime_state,
        BoomThreadState* thread_state,
        BoomMethodInfoHandle method,
        void* object_instance,
        void* const* argv,
        uint32_t argc,
        void* out_return_value,
        size_t out_return_value_size,
        BoomExceptionHandle* out_exception);

    /* Reflection queries return null when the subject cannot be resolved. */
    BoomImageHandle (BOOM_RUNTIME_ABI_CALL* assembly_get_image)(
        BoomAssemblyHandle assembly);
    BoomTypeInfoHandle (BOOM_RUNTIME_ABI_CALL* image_find_type)(
        BoomImageHandle image,
        const char* namespace_utf8,
        const char* type_name_utf8);
    BoomMethodInfoHandle (BOOM_RUNTIME_ABI_CALL* type_find_method)(
        BoomTypeInfoHandle type,
        const char* method_name_utf8,
        int32_t parameter_count);
    BoomFieldInfoHandle (BOOM_RUNTIME_ABI_CALL* type_find_field)(
        BoomTypeInfoHandle type,
        const char* field_name_utf8);
    BoomPropertyInfoHandle (BOOM_RUNTIME_ABI_CALL* type_find_property)(
        BoomTypeInfoHandle type,
        const char* property_name_utf8);
    BoomEventInfoHandle (BOOM_RUNTIME_ABI_CALL* type_find_event)(
        BoomTypeInfoHandle type,
        const char* event_name_utf8);
    BoomParameterInfoHandle (BOOM_RUNTIME_ABI_CALL* method_get_parameter)(
        BoomMethodInfoHandle method,
        uint32_t parameter_index);
    BoomGenericContextHandle (BOOM_RUNTIME_ABI_CALL* method_get_generic_context)(
        BoomMethodInfoHandle method);
} BoomRuntimeAbiV0;

/* Returns the process-wide v0 table or null when the ABI is unavailable. */
BOOM_RUNTIME_ABI_EXPORT const BoomRuntimeAbiV0* BOOM_RUNTIME_ABI_CALL boom_runtime_get_abi_v0(void);

#ifdef __cplusplus
}
#endif

#endif  // BOOM_RUNTIME_ABI_H_
