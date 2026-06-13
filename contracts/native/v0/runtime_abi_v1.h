// ── ABI V1: Extended runtime function table (Phase 2) ──
// Extends RuntimeAbiV0 with additional function categories needed
// by generated code.  The goal is to replace direct includes of
// internal runtime headers (runtime_core.h, module_registry.h, etc.)
// with indirect calls through this ABI table.
//
// NOTE: Generated code caches the table pointer at startup and
// calls through g_chaos_runtime_abi->function_name() instead of
// directly calling runtime_core::function_name().

#ifndef CHAOS_RUNTIME_ABI_V1_H_
#define CHAOS_RUNTIME_ABI_V1_H_

#include "runtime_abi.h"  // RuntimeAbiV0 base

#ifdef __cplusplus
extern "C" {
#endif

// ── Module registration (3 functions) ──
// Replaces: module_registry.h, codegen_bridge.h
typedef struct ModuleDescriptor ModuleDescriptor;
typedef uint32_t (CHAOS_RUNTIME_ABI_CALL* FnRegisterModule)(
    const char* module_name,
    const ModuleDescriptor* desc);
typedef bool (CHAOS_RUNTIME_ABI_CALL* FnActivateGeneratedModule)(void* host);
typedef const void* (CHAOS_RUNTIME_ABI_CALL* FnGetDispatchEntry)(uint32_t slot);

// ── GC allocation fast paths (4 functions) ──
// Replaces: gc/gc_bgc_inline.h, gc/gc_card_table.h, gc/gc_root_change.h
typedef void* (CHAOS_RUNTIME_ABI_CALL* FnGcAlloc)(size_t size);
typedef void* (CHAOS_RUNTIME_ABI_CALL* FnGcAllocAtomic)(size_t size);
typedef void* (CHAOS_RUNTIME_ABI_CALL* FnGcAllocArray)(size_t size);
typedef void (CHAOS_RUNTIME_ABI_CALL* FnGcWriteBarrier)(void* field, void* value);

// ── Interpreter dispatch (1 function) ──
// Replaces: interpreter_entry.h
typedef CHAOS_IL2CPP_INTPTR (CHAOS_RUNTIME_ABI_CALL* FnInterpreterDispatch)(
    CHAOS_IL2CPP_UINTPTR method_key,
    void* args_buf, void* ret_buf);

// ── Array operations (3 functions) ──
// Replaces: inline array helpers in generated code
typedef void* (CHAOS_RUNTIME_ABI_CALL* FnArrayCreate)(void* type, int32_t length);
typedef int32_t (CHAOS_RUNTIME_ABI_CALL* FnArrayGetLength)(void* array);
typedef void* (CHAOS_RUNTIME_ABI_CALL* FnArrayGetElement)(void* array, int32_t index);

// ── Runtime vtable / instantiation (2 functions) ──
// Replaces: runtime_vtable.h, runtime_instantiation.h
typedef void* (CHAOS_RUNTIME_ABI_CALL* FnResolveVTableSlot)(void* type, uint32_t slot);
typedef void* (CHAOS_RUNTIME_ABI_CALL* FnResolveInstantiation)(const char* subject_id);

// ── Extended RuntimeAbiV1 ──
// Layout: RuntimeAbiV0 fields (struct_size, abi_version + 40 functions)
//         + RuntimeAbiV1 extension fields below
typedef struct RuntimeAbiV1 {
    // V0 base (embedded, same layout)
    uint32_t abi_version;
    uint32_t struct_size;

    // [V0 fields — same order as RuntimeAbiV0]
    RuntimeStatus (CHAOS_RUNTIME_ABI_CALL* runtime_init)(const RuntimeInitParams*, const RuntimeConfig*, RuntimeState**);
    void (CHAOS_RUNTIME_ABI_CALL* runtime_shutdown)(RuntimeState*);
    RuntimeStatus (CHAOS_RUNTIME_ABI_CALL* thread_attach)(RuntimeState*, ThreadState**);
    void (CHAOS_RUNTIME_ABI_CALL* thread_detach)(RuntimeState*, ThreadState*);
    void* (CHAOS_RUNTIME_ABI_CALL* object_new)(RuntimeState*, ThreadState*, TypeInfoHandle);
    void* (CHAOS_RUNTIME_ABI_CALL* array_new)(RuntimeState*, ThreadState*, TypeInfoHandle, uintptr_t);
    void* (CHAOS_RUNTIME_ABI_CALL* string_new_utf8)(RuntimeState*, ThreadState*, const char*, uintptr_t);
    RuntimeStatus (CHAOS_RUNTIME_ABI_CALL* class_init)(RuntimeState*, TypeInfoHandle);
    GCHandle (CHAOS_RUNTIME_ABI_CALL* gc_handle_new)(RuntimeState*, void*, bool);
    void (CHAOS_RUNTIME_ABI_CALL* gc_handle_free)(RuntimeState*, GCHandle);
    void (CHAOS_RUNTIME_ABI_CALL* raise_managed_exception)(RuntimeState*, ThreadState*, ExceptionHandle);
    RuntimeStatus (CHAOS_RUNTIME_ABI_CALL* field_get_value)(RuntimeState*, ThreadState*, FieldInfoHandle, void*, void*, size_t);
    RuntimeStatus (CHAOS_RUNTIME_ABI_CALL* field_set_value)(RuntimeState*, ThreadState*, FieldInfoHandle, void*, const void*, size_t);
    RuntimeStatus (CHAOS_RUNTIME_ABI_CALL* method_invoke)(RuntimeState*, ThreadState*, MethodInfoHandle, void*, void* const*, uint32_t, void*, size_t, ExceptionHandle*);
    ImageHandle (CHAOS_RUNTIME_ABI_CALL* assembly_get_image)(AssemblyHandle);
    TypeInfoHandle (CHAOS_RUNTIME_ABI_CALL* image_find_type)(ImageHandle, const char*, const char*);
    MethodInfoHandle (CHAOS_RUNTIME_ABI_CALL* type_find_method)(TypeInfoHandle, const char*, int32_t);
    FieldInfoHandle (CHAOS_RUNTIME_ABI_CALL* type_find_field)(TypeInfoHandle, const char*);
    PropertyInfoHandle (CHAOS_RUNTIME_ABI_CALL* type_find_property)(TypeInfoHandle, const char*);
    EventInfoHandle (CHAOS_RUNTIME_ABI_CALL* type_find_event)(TypeInfoHandle, const char*);
    TypeInfoHandle (CHAOS_RUNTIME_ABI_CALL* type_get_generic_type_definition)(TypeInfoHandle);
    RuntimeStatus (CHAOS_RUNTIME_ABI_CALL* type_query_capability)(TypeInfoHandle, RuntimeTypeCapabilityInfoV0*);
    ParameterInfoHandle (CHAOS_RUNTIME_ABI_CALL* method_get_parameter)(MethodInfoHandle, uint32_t);
    GenericContextHandle (CHAOS_RUNTIME_ABI_CALL* method_get_generic_context)(MethodInfoHandle);
    uint32_t (CHAOS_RUNTIME_ABI_CALL* generic_context_get_class_arg_count)(GenericContextHandle);
    TypeInfoHandle (CHAOS_RUNTIME_ABI_CALL* generic_context_get_class_arg)(GenericContextHandle, uint32_t);
    uint32_t (CHAOS_RUNTIME_ABI_CALL* generic_context_get_method_arg_count)(GenericContextHandle);
    TypeInfoHandle (CHAOS_RUNTIME_ABI_CALL* generic_context_get_method_arg)(GenericContextHandle, uint32_t);
    GCHandle (CHAOS_RUNTIME_ABI_CALL* gc_handle_new_ex)(RuntimeState*, void*, bool, bool);
    void* (CHAOS_RUNTIME_ABI_CALL* gc_handle_get)(RuntimeState*, GCHandle);
    void (CHAOS_RUNTIME_ABI_CALL* gc_handle_set)(RuntimeState*, GCHandle, void*);
    int64_t (CHAOS_RUNTIME_ABI_CALL* gc_get_total_memory)(RuntimeState*);
    void (CHAOS_RUNTIME_ABI_CALL* gc_add_memory_pressure)(RuntimeState*, int64_t);
    void (CHAOS_RUNTIME_ABI_CALL* gc_remove_memory_pressure)(RuntimeState*, int64_t);

    // ── V1 extension fields ──
    FnRegisterModule register_module;
    FnActivateGeneratedModule activate_generated_module;
    FnGetDispatchEntry get_dispatch_entry;
    FnGcAlloc gc_alloc;
    FnGcAllocAtomic gc_alloc_atomic;
    FnGcAllocArray gc_alloc_array;
    FnGcWriteBarrier gc_write_barrier;
    FnInterpreterDispatch interpreter_dispatch;
    FnArrayCreate array_create;
    FnArrayGetLength array_get_length;
    FnArrayGetElement array_get_element;
    FnResolveVTableSlot resolve_vtable_slot;
    FnResolveInstantiation resolve_instantiation;
} RuntimeAbiV1;

#define CHAOS_RUNTIME_ABI_V1_VERSION 1u

// ── Global ABI pointer for generated code ──
// Set once at startup by ChaosRuntimeHost::Initialize() or equivalent.
// Generated code uses g_chaos_runtime_abi->function_name() instead of
// directly including and calling runtime internal headers.
// Declared as extern so the single definition lives in abi_export.cpp.
extern __declspec(selectany) const RuntimeAbiV1* g_chaos_runtime_abi = nullptr;

#ifdef __cplusplus
}
#endif

#endif  // CHAOS_RUNTIME_ABI_V1_H_
