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
#define BOOM_GC_HANDLE_INVALID ((GCHandle)0u)

/* Opaque reflection and runtime handles carried across the ABI boundary. */
typedef struct TypeInfoOpaque* TypeInfoHandle;
typedef struct MethodInfoOpaque* MethodInfoHandle;
typedef struct FieldInfoOpaque* FieldInfoHandle;
typedef struct PropertyInfoOpaque* PropertyInfoHandle;
typedef struct EventInfoOpaque* EventInfoHandle;
typedef struct ParameterInfoOpaque* ParameterInfoHandle;
typedef struct GenericContextOpaque* GenericContextHandle;
typedef struct AssemblyOpaque* AssemblyHandle;
typedef struct ImageOpaque* ImageHandle;
typedef struct ExceptionOpaque* ExceptionHandle;

typedef struct RuntimeState RuntimeState;
typedef struct ThreadState ThreadState;

typedef uint64_t GCHandle;
typedef int32_t RuntimeStatus;

enum {
    BOOM_RUNTIME_STATUS_OK = 0,
    BOOM_RUNTIME_STATUS_INVALID_ARGUMENT = 1,
    BOOM_RUNTIME_STATUS_INVALID_STATE = 2,
    BOOM_RUNTIME_STATUS_NOT_FOUND = 3,
    BOOM_RUNTIME_STATUS_NOT_SUPPORTED = 4,
    BOOM_RUNTIME_STATUS_MANAGED_EXCEPTION = 5,
    BOOM_RUNTIME_STATUS_INTERNAL_ERROR = 6
};

typedef enum RuntimeInitFlags {
    BOOM_RUNTIME_INIT_NONE = 0,
    BOOM_RUNTIME_INIT_ENABLE_LOGGING = 1 << 0,
    BOOM_RUNTIME_INIT_ENABLE_PROFILING = 1 << 1
} RuntimeInitFlags;

/* Host-supplied allocators are optional and may be null. */
typedef void* (BOOM_RUNTIME_ABI_CALL* AllocatorFn)(size_t size, void* user_data);
typedef void (BOOM_RUNTIME_ABI_CALL* DeallocatorFn)(void* ptr, void* user_data);

typedef struct RuntimeInitParams {
    uint32_t struct_size;
    uint32_t init_flags;
    const char* host_name_utf8;
    const char* runtime_tag_utf8;
    void* host_context;
} RuntimeInitParams;

typedef struct RuntimeConfig {
    uint32_t struct_size;
    AllocatorFn allocator;
    DeallocatorFn deallocator;
    void* allocator_user_data;
    uint32_t reserved_u32[4];
} RuntimeConfig;

/*
 * Process-wide function table for the low-level runtime ABI.
 * Callers cache the table once per process and pass runtime/thread state
 * explicitly on every operation.
 */
typedef struct RuntimeAbiV0 {
    uint32_t abi_version;
    uint32_t struct_size;

    /* Runtime lifecycle. */
    RuntimeStatus (BOOM_RUNTIME_ABI_CALL* runtime_init)(
        const RuntimeInitParams* init_params,
        const RuntimeConfig* config,
        RuntimeState** out_runtime_state);
    void (BOOM_RUNTIME_ABI_CALL* runtime_shutdown)(RuntimeState* runtime_state);

    /* Thread lifecycle. */
    RuntimeStatus (BOOM_RUNTIME_ABI_CALL* thread_attach)(
        RuntimeState* runtime_state,
        ThreadState** out_thread_state);
    void (BOOM_RUNTIME_ABI_CALL* thread_detach)(
        RuntimeState* runtime_state,
        ThreadState* thread_state);

    /* Allocation helpers. */
    void* (BOOM_RUNTIME_ABI_CALL* object_new)(
        RuntimeState* runtime_state,
        ThreadState* thread_state,
        TypeInfoHandle type);
    void* (BOOM_RUNTIME_ABI_CALL* array_new)(
        RuntimeState* runtime_state,
        ThreadState* thread_state,
        TypeInfoHandle element_type,
        uintptr_t length);
    void* (BOOM_RUNTIME_ABI_CALL* string_new_utf8)(
        RuntimeState* runtime_state,
        ThreadState* thread_state,
        const char* utf8_bytes,
        uintptr_t byte_count);

    RuntimeStatus (BOOM_RUNTIME_ABI_CALL* class_init)(
        RuntimeState* runtime_state,
        TypeInfoHandle type);

    /* GC handle helpers. */
    GCHandle (BOOM_RUNTIME_ABI_CALL* gc_handle_new)(
        RuntimeState* runtime_state,
        void* object_instance,
        bool pinned);
    void (BOOM_RUNTIME_ABI_CALL* gc_handle_free)(
        RuntimeState* runtime_state,
        GCHandle gc_handle);

    void (BOOM_RUNTIME_ABI_CALL* raise_managed_exception)(
        RuntimeState* runtime_state,
        ThreadState* thread_state,
        ExceptionHandle exception);

    /* Field and method operations are handle-driven and explicitly sized. */
    RuntimeStatus (BOOM_RUNTIME_ABI_CALL* field_get_value)(
        RuntimeState* runtime_state,
        ThreadState* thread_state,
        FieldInfoHandle field,
        void* object_instance,
        void* out_value,
        size_t out_value_size);
    RuntimeStatus (BOOM_RUNTIME_ABI_CALL* field_set_value)(
        RuntimeState* runtime_state,
        ThreadState* thread_state,
        FieldInfoHandle field,
        void* object_instance,
        const void* value,
        size_t value_size);

    RuntimeStatus (BOOM_RUNTIME_ABI_CALL* method_invoke)(
        RuntimeState* runtime_state,
        ThreadState* thread_state,
        MethodInfoHandle method,
        void* object_instance,
        void* const* argv,
        uint32_t argc,
        void* out_return_value,
        size_t out_return_value_size,
        ExceptionHandle* out_exception);

    /* Reflection queries return null when the subject cannot be resolved. */
    ImageHandle (BOOM_RUNTIME_ABI_CALL* assembly_get_image)(
        AssemblyHandle assembly);
    TypeInfoHandle (BOOM_RUNTIME_ABI_CALL* image_find_type)(
        ImageHandle image,
        const char* namespace_utf8,
        const char* type_name_utf8);
    MethodInfoHandle (BOOM_RUNTIME_ABI_CALL* type_find_method)(
        TypeInfoHandle type,
        const char* method_name_utf8,
        int32_t parameter_count);
    FieldInfoHandle (BOOM_RUNTIME_ABI_CALL* type_find_field)(
        TypeInfoHandle type,
        const char* field_name_utf8);
    PropertyInfoHandle (BOOM_RUNTIME_ABI_CALL* type_find_property)(
        TypeInfoHandle type,
        const char* property_name_utf8);
    EventInfoHandle (BOOM_RUNTIME_ABI_CALL* type_find_event)(
        TypeInfoHandle type,
        const char* event_name_utf8);
    ParameterInfoHandle (BOOM_RUNTIME_ABI_CALL* method_get_parameter)(
        MethodInfoHandle method,
        uint32_t parameter_index);
    GenericContextHandle (BOOM_RUNTIME_ABI_CALL* method_get_generic_context)(
        MethodInfoHandle method);
} RuntimeAbiV0;

/* Returns the process-wide v0 table or null when the ABI is unavailable. */
BOOM_RUNTIME_ABI_EXPORT const RuntimeAbiV0* BOOM_RUNTIME_ABI_CALL boom_runtime_get_abi_v0(void);

#ifdef __cplusplus
}
#endif

#endif  // BOOM_RUNTIME_ABI_H_
