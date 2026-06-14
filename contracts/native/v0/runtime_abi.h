#ifndef CHAOS_RUNTIME_ABI_H_
#define CHAOS_RUNTIME_ABI_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Provide CHAOS_IL2CPP_* type aliases so this header is self-contained.
 * When compiled from C++ code that includes chaos/native_types.h first,
 * these fallbacks are skipped via the #ifndef guard.
 * When compiled standalone (C or C++), we define lightweight aliases
 * that match the C++-only chaos/native_types.h definitions. */
#ifndef CHAOS_IL2CPP_INTPTR
#  ifdef __cplusplus
#    include <cstdint>
#    define CHAOS_IL2CPP_INTPTR   std::intptr_t
#    define CHAOS_IL2CPP_UINTPTR  std::uintptr_t
#    define CHAOS_IL2CPP_INT32    std::int32_t
#    define CHAOS_IL2CPP_UINT32   std::uint32_t
#    define CHAOS_IL2CPP_INT      int
#    define CHAOS_IL2CPP_UINT     unsigned int
#  else
#    define CHAOS_IL2CPP_INTPTR   intptr_t
#    define CHAOS_IL2CPP_UINTPTR  uintptr_t
#    define CHAOS_IL2CPP_INT32    int32_t
#    define CHAOS_IL2CPP_UINT32   uint32_t
#    define CHAOS_IL2CPP_INT      int
#    define CHAOS_IL2CPP_UINT     unsigned int
#  endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32)
/* Static-library consumers collapse import/export decoration to plain declarations. */
#define CHAOS_RUNTIME_ABI_EXPORT
#define CHAOS_RUNTIME_ABI_CALL __cdecl
#else
#define CHAOS_RUNTIME_ABI_EXPORT __attribute__((visibility("default")))
#define CHAOS_RUNTIME_ABI_CALL
#endif

#define CHAOS_RUNTIME_ABI_V0 0u
#define CHAOS_RUNTIME_ABI_V1 1u
#define CHAOS_GC_HANDLE_INVALID ((GCHandle)0u)

/* Reflection and runtime handles — uint64_t encoding for cross-DLL identity.
 * TypeInfoHandle: three-state encoding (Phase 0 CoreCLR alignment):
 *   bit[63]=1:          Direct MethodTable* pointer (AOT, zero-cost dereference)
 *   bit[62]=1,bit[63]=0: Dynamic MethodTable index (for runtime-created types)
 *   bit[62]=0,bit[63]=0: Module-registry handle [module_id:32 << 32 | token:32]
 * Other handles follow the same patterns where applicable.
 * GenericContextHandle/AssemblyHandle/ExceptionHandle remain opaque pointers. */
typedef uint64_t TypeInfoHandle;
typedef uint64_t MethodInfoHandle;
typedef uint64_t FieldInfoHandle;
typedef uint64_t PropertyInfoHandle;
typedef uint64_t EventInfoHandle;
typedef uint64_t ParameterInfoHandle;
typedef struct GenericContextOpaque* GenericContextHandle;
typedef struct AssemblyOpaque* AssemblyHandle;
typedef uint64_t ImageHandle;
typedef struct ExceptionOpaque* ExceptionHandle;

typedef struct RuntimeState RuntimeState;
typedef struct ThreadState ThreadState;

typedef uint64_t GCHandle;
typedef int32_t RuntimeStatus;

typedef enum TypeCapabilityVectorLaneKind {
    CHAOS_TYPE_CAPABILITY_VECTOR_LANE_NONE = 0,
    CHAOS_TYPE_CAPABILITY_VECTOR_LANE_INTEGER = 1,
    CHAOS_TYPE_CAPABILITY_VECTOR_LANE_FLOATING = 2
} TypeCapabilityVectorLaneKind;

typedef enum TypeCapabilityScalarKind {
    CHAOS_TYPE_CAPABILITY_SCALAR_KIND_NONE = 0,
    CHAOS_TYPE_CAPABILITY_SCALAR_KIND_SIGNED_INTEGER = 1,
    CHAOS_TYPE_CAPABILITY_SCALAR_KIND_UNSIGNED_INTEGER = 2,
    CHAOS_TYPE_CAPABILITY_SCALAR_KIND_NATIVE_SIGNED_INTEGER = 3,
    CHAOS_TYPE_CAPABILITY_SCALAR_KIND_NATIVE_UNSIGNED_INTEGER = 4,
    CHAOS_TYPE_CAPABILITY_SCALAR_KIND_FLOATING = 5
} TypeCapabilityScalarKind;

enum {
    CHAOS_RUNTIME_STATUS_OK = 0,
    CHAOS_RUNTIME_STATUS_INVALID_ARGUMENT = 1,
    CHAOS_RUNTIME_STATUS_INVALID_STATE = 2,
    CHAOS_RUNTIME_STATUS_NOT_FOUND = 3,
    CHAOS_RUNTIME_STATUS_NOT_SUPPORTED = 4,
    CHAOS_RUNTIME_STATUS_MANAGED_EXCEPTION = 5,
    CHAOS_RUNTIME_STATUS_INTERNAL_ERROR = 6
};

typedef enum RuntimeInitFlags {
    CHAOS_RUNTIME_INIT_NONE = 0,
    CHAOS_RUNTIME_INIT_ENABLE_LOGGING = 1 << 0,
    CHAOS_RUNTIME_INIT_ENABLE_PROFILING = 1 << 1
} RuntimeInitFlags;

/* Host-supplied allocators are optional and may be null. */
typedef void* (CHAOS_RUNTIME_ABI_CALL* AllocatorFn)(size_t size, void* user_data);
typedef void (CHAOS_RUNTIME_ABI_CALL* DeallocatorFn)(void* ptr, void* user_data);

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

typedef struct RuntimeTypeCapabilityInfoV0 {
    uint32_t struct_size;
    uint32_t capability_bits;
    uint32_t value_size_bytes;
    uint32_t vector_width_bytes;
    uint32_t vector_lane_count;
    uint32_t vector_lane_kind;
    uint32_t scalar_kind;
} RuntimeTypeCapabilityInfoV0;

typedef struct RuntimeTypeCapabilityEntryV0 {
    uint32_t type_token;
    RuntimeTypeCapabilityInfoV0 capability_info;
} RuntimeTypeCapabilityEntryV0;

/*
 * Process-wide function table for the low-level runtime ABI.
 * Callers cache the table once per process and pass runtime/thread state
 * explicitly on every operation.
 */
typedef struct RuntimeAbiV0 {
    uint32_t abi_version;
    uint32_t struct_size;

    /* Runtime lifecycle. */
    RuntimeStatus (CHAOS_RUNTIME_ABI_CALL* runtime_init)(
        const RuntimeInitParams* init_params,
        const RuntimeConfig* config,
        RuntimeState** out_runtime_state);
    void (CHAOS_RUNTIME_ABI_CALL* runtime_shutdown)(RuntimeState* runtime_state);

    /* Thread lifecycle. */
    RuntimeStatus (CHAOS_RUNTIME_ABI_CALL* thread_attach)(
        RuntimeState* runtime_state,
        ThreadState** out_thread_state);
    void (CHAOS_RUNTIME_ABI_CALL* thread_detach)(
        RuntimeState* runtime_state,
        ThreadState* thread_state);

    /* Allocation helpers. */
    void* (CHAOS_RUNTIME_ABI_CALL* object_new)(
        RuntimeState* runtime_state,
        ThreadState* thread_state,
        TypeInfoHandle type);
    void* (CHAOS_RUNTIME_ABI_CALL* array_new)(
        RuntimeState* runtime_state,
        ThreadState* thread_state,
        TypeInfoHandle element_type,
        uintptr_t length);
    void* (CHAOS_RUNTIME_ABI_CALL* string_new_utf8)(
        RuntimeState* runtime_state,
        ThreadState* thread_state,
        const char* utf8_bytes,
        uintptr_t byte_count);

    RuntimeStatus (CHAOS_RUNTIME_ABI_CALL* class_init)(
        RuntimeState* runtime_state,
        TypeInfoHandle type);

    /* GC handle helpers. */
    GCHandle (CHAOS_RUNTIME_ABI_CALL* gc_handle_new)(
        RuntimeState* runtime_state,
        void* object_instance,
        bool pinned);
    void (CHAOS_RUNTIME_ABI_CALL* gc_handle_free)(
        RuntimeState* runtime_state,
        GCHandle gc_handle);

    void (CHAOS_RUNTIME_ABI_CALL* raise_managed_exception)(
        RuntimeState* runtime_state,
        ThreadState* thread_state,
        ExceptionHandle exception);

    /* Field and method operations are handle-driven and explicitly sized. */
    RuntimeStatus (CHAOS_RUNTIME_ABI_CALL* field_get_value)(
        RuntimeState* runtime_state,
        ThreadState* thread_state,
        FieldInfoHandle field,
        void* object_instance,
        void* out_value,
        size_t out_value_size);
    RuntimeStatus (CHAOS_RUNTIME_ABI_CALL* field_set_value)(
        RuntimeState* runtime_state,
        ThreadState* thread_state,
        FieldInfoHandle field,
        void* object_instance,
        const void* value,
        size_t value_size);

    RuntimeStatus (CHAOS_RUNTIME_ABI_CALL* method_invoke)(
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
    ImageHandle (CHAOS_RUNTIME_ABI_CALL* assembly_get_image)(
        AssemblyHandle assembly);
    TypeInfoHandle (CHAOS_RUNTIME_ABI_CALL* image_find_type)(
        ImageHandle image,
        const char* namespace_utf8,
        const char* type_name_utf8);
    MethodInfoHandle (CHAOS_RUNTIME_ABI_CALL* type_find_method)(
        TypeInfoHandle type,
        const char* method_name_utf8,
        int32_t parameter_count);
    FieldInfoHandle (CHAOS_RUNTIME_ABI_CALL* type_find_field)(
        TypeInfoHandle type,
        const char* field_name_utf8);
    PropertyInfoHandle (CHAOS_RUNTIME_ABI_CALL* type_find_property)(
        TypeInfoHandle type,
        const char* property_name_utf8);
    EventInfoHandle (CHAOS_RUNTIME_ABI_CALL* type_find_event)(
        TypeInfoHandle type,
        const char* event_name_utf8);
    TypeInfoHandle (CHAOS_RUNTIME_ABI_CALL* type_get_generic_type_definition)(
        TypeInfoHandle type);
    RuntimeStatus (CHAOS_RUNTIME_ABI_CALL* type_query_capability)(
        TypeInfoHandle type,
        RuntimeTypeCapabilityInfoV0* out_capability_info);
    ParameterInfoHandle (CHAOS_RUNTIME_ABI_CALL* method_get_parameter)(
        MethodInfoHandle method,
        uint32_t parameter_index);
    GenericContextHandle (CHAOS_RUNTIME_ABI_CALL* method_get_generic_context)(
        MethodInfoHandle method);
    uint32_t (CHAOS_RUNTIME_ABI_CALL* generic_context_get_class_arg_count)(
        GenericContextHandle generic_context);
    TypeInfoHandle (CHAOS_RUNTIME_ABI_CALL* generic_context_get_class_arg)(
        GenericContextHandle generic_context,
        uint32_t index);
    uint32_t (CHAOS_RUNTIME_ABI_CALL* generic_context_get_method_arg_count)(
        GenericContextHandle generic_context);
    TypeInfoHandle (CHAOS_RUNTIME_ABI_CALL* generic_context_get_method_arg)(
        GenericContextHandle generic_context,
        uint32_t index);

    /* Extended GC handle helpers (added in ABI v2). */
    GCHandle (CHAOS_RUNTIME_ABI_CALL* gc_handle_new_ex)(
        RuntimeState* runtime_state,
        void* object_instance,
        bool pinned,
        bool weak);
    void* (CHAOS_RUNTIME_ABI_CALL* gc_handle_get)(
        RuntimeState* runtime_state,
        GCHandle gc_handle);
    void (CHAOS_RUNTIME_ABI_CALL* gc_handle_set)(
        RuntimeState* runtime_state,
        GCHandle gc_handle,
        void* object_instance);

    /* GC memory introspection (added in ABI v3). */
    int64_t (CHAOS_RUNTIME_ABI_CALL* gc_get_total_memory)(
        RuntimeState* runtime_state);
    void (CHAOS_RUNTIME_ABI_CALL* gc_add_memory_pressure)(
        RuntimeState* runtime_state,
        int64_t bytes);
    void (CHAOS_RUNTIME_ABI_CALL* gc_remove_memory_pressure)(
        RuntimeState* runtime_state,
        int64_t bytes);

    
    /* ── V2 extended helpers ── */
    
    /* GC allocation. */
    void* (CHAOS_RUNTIME_ABI_CALL* gc_alloc)(size_t size, int kind);
    void* (CHAOS_RUNTIME_ABI_CALL* gc_alloc_atomic)(size_t size);

    /* COM / RCW interop. */
    bool (CHAOS_RUNTIME_ABI_CALL* marshal_is_rcw_handle)(CHAOS_IL2CPP_INTPTR handle);
    void* (CHAOS_RUNTIME_ABI_CALL* marshal_get_rcw_unknown)(CHAOS_IL2CPP_INTPTR handle);
    void (CHAOS_RUNTIME_ABI_CALL* throw_com_exception_for_hr)(int32_t hr);

    /* Delegate / hotpatch fallback. */
    bool (CHAOS_RUNTIME_ABI_CALL* delegate_hotpatch_checkpoint)(void* delegate);

    /* Marshal helpers. */
    void (CHAOS_RUNTIME_ABI_CALL* marshal_free_co_task_mem)(void* ptr);
    void* (CHAOS_RUNTIME_ABI_CALL* marshal_ptr_to_string_utf8)(const char* ptr);
    void* (CHAOS_RUNTIME_ABI_CALL* marshal_ptr_to_string_wide)(const uint16_t* ptr);
    void (CHAOS_RUNTIME_ABI_CALL* marshal_struct_managed_to_native)(
        void* managed, void* native, void* type);
    void (CHAOS_RUNTIME_ABI_CALL* marshal_struct_native_to_managed)(
        void* native, void* managed, void* type);

    /* Native library loading. */
    void* (CHAOS_RUNTIME_ABI_CALL* native_library_load)(const char* path);
    void* (CHAOS_RUNTIME_ABI_CALL* native_library_get_proc_address)(
        void* handle, const char* name);

    /* PInvoke error tracking. */
    void (CHAOS_RUNTIME_ABI_CALL* set_last_pinvoke_error)(int32_t error);
    int32_t (CHAOS_RUNTIME_ABI_CALL* get_last_os_error)(void);
    void (CHAOS_RUNTIME_ABI_CALL* clear_last_os_error)(void);

    /* GC finalization. */
    void (CHAOS_RUNTIME_ABI_CALL* gc_register_finalizable)(void* obj);

    /* Thread state helpers. */
    void* (CHAOS_RUNTIME_ABI_CALL* get_current_runtime_state)(void);
    void* (CHAOS_RUNTIME_ABI_CALL* get_current_thread_state)(void);

    /* Exception helper. */
    void (CHAOS_RUNTIME_ABI_CALL* raise_exception)(void* exception_obj);

/* ── V1 codegen-facing fields ── */
    uint32_t (CHAOS_RUNTIME_ABI_CALL* register_module)(
        const char* name,
        const void* descriptor);
    bool (CHAOS_RUNTIME_ABI_CALL* hotpatch_is_active)(
        const void* entry);
    bool (CHAOS_RUNTIME_ABI_CALL* hotpatch_should_keep_native)(
        const void* entry);
    void (CHAOS_RUNTIME_ABI_CALL* raise_null_reference_exception)(void);
    uintptr_t (CHAOS_RUNTIME_ABI_CALL* external_runtime_fallback)(
        const char* subject_id);
    void (CHAOS_RUNTIME_ABI_CALL* interpreter_entry_direct)(
        uintptr_t method_key,
        void* args_buf,
        void* ret_buf);
    void (CHAOS_RUNTIME_ABI_CALL* register_gc_layouts)(void);
    void (CHAOS_RUNTIME_ABI_CALL* register_hotpatch_module)(
        const void* module);
    uintptr_t (CHAOS_RUNTIME_ABI_CALL* array_empty)(void);

    } RuntimeAbiV0;

/* RuntimeAbiV1/V2 — V1 adds GC/boxing/vtable/thread-static helpers;
 * V2 adds exception/marshal/PInvoke/GC-finalization/thread-state helpers.
 * The struct is layout-compatible with V0 for the first N fields, so code that
 * only uses V0 slots can continue to consume a V1/V2 table via cast. */
typedef RuntimeAbiV0 RuntimeAbiV1;
typedef RuntimeAbiV0 RuntimeAbiV2;

/* Returns the process-wide v0 table or null when the ABI is unavailable. */
CHAOS_RUNTIME_ABI_EXPORT const RuntimeAbiV0* CHAOS_RUNTIME_ABI_CALL chaos_runtime_get_abi_v0(void);

/* Returns the process-wide v1 table (superset of v0). */
CHAOS_RUNTIME_ABI_EXPORT const RuntimeAbiV1* CHAOS_RUNTIME_ABI_CALL chaos_runtime_get_abi_v1(void);

/* Returns the process-wide v2 table (superset of v0, v1). */
CHAOS_RUNTIME_ABI_EXPORT const RuntimeAbiV2* CHAOS_RUNTIME_ABI_CALL chaos_runtime_get_abi_v2(void);


#ifdef __cplusplus
}
#endif

#endif  // CHAOS_RUNTIME_ABI_H_

