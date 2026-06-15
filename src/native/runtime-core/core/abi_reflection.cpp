// Required forward declarations: ChaosRegisterGcLayouts etc.
#include "chaos_runtime_host.h"

namespace chaos::il2cpp::runtime_core {
namespace {

GCHandle CHAOS_RUNTIME_ABI_CALL GcHandleNew(
    RuntimeState* runtime_state,
    void* object_instance,
    bool pinned) {
    if (runtime_state == nullptr || object_instance == nullptr) {
        return CHAOS_GC_HANDLE_INVALID;
    }

    CHAOS_IL2CPP_UINT64 handle;
    if (pinned) {
        handle = GcCreatePinnedHandle(object_instance);
    } else {
        handle = GcCreateStrongHandle(object_instance);
    }

    return static_cast<GCHandle>(handle);
}

void CHAOS_RUNTIME_ABI_CALL GcHandleFree(
    RuntimeState* runtime_state,
    GCHandle gc_handle) {
    if (runtime_state == nullptr || gc_handle == CHAOS_GC_HANDLE_INVALID) return;

    GcFreeHandle(static_cast<CHAOS_IL2CPP_UINT64>(gc_handle));
}

void CHAOS_RUNTIME_ABI_CALL RaiseManagedException(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    ExceptionHandle exception) {
    if (!IsAttached(runtime_state, thread_state)) return;

    chaos_raise_exception(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(exception));
}

RuntimeStatus CHAOS_RUNTIME_ABI_CALL FieldGetValue(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    FieldInfoHandle field,
    void* object_instance,
    void* out_value,
    CHAOS_IL2CPP_SIZE out_value_size) {
    if (!IsAttached(runtime_state, thread_state)
        || object_instance == nullptr
        || out_value == nullptr
        || out_value_size == 0u) {
        return CHAOS_RUNTIME_STATUS_INVALID_ARGUMENT;
    }

    if (field == 0) return CHAOS_RUNTIME_STATUS_NOT_FOUND;

    const auto* ti = *static_cast<const TypeInfoHot* const*>(object_instance);
    const auto header_size = HeaderSizeFromFlags(ti != nullptr ? ti->flags : 0);
    CHAOS_IL2CPP_MEMCPY(out_value,
        static_cast<const unsigned char*>(object_instance) + header_size,
        out_value_size);
    return CHAOS_RUNTIME_STATUS_OK;
}

RuntimeStatus CHAOS_RUNTIME_ABI_CALL FieldSetValue(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    FieldInfoHandle field,
    void* object_instance,
    const void* value,
    CHAOS_IL2CPP_SIZE value_size) {
    if (!IsAttached(runtime_state, thread_state)
        || object_instance == nullptr
        || value == nullptr
        || value_size == 0u) {
        return CHAOS_RUNTIME_STATUS_INVALID_ARGUMENT;
    }

    if (field == 0) return CHAOS_RUNTIME_STATUS_NOT_FOUND;

    const auto* ti = *static_cast<const TypeInfoHot* const*>(object_instance);
    const auto header_size = HeaderSizeFromFlags(ti != nullptr ? ti->flags : 0);
    CHAOS_IL2CPP_MEMCPY(
        static_cast<unsigned char*>(object_instance) + header_size,
        value, value_size);
    return CHAOS_RUNTIME_STATUS_OK;
}

RuntimeStatus CHAOS_RUNTIME_ABI_CALL MethodInvoke(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    MethodInfoHandle method,
    void* object_instance,
    void* const* argv,
    CHAOS_IL2CPP_UINT32 argc,
    void* out_return_value,
    CHAOS_IL2CPP_SIZE out_return_value_size,
    ExceptionHandle* out_exception) {
    using RawMethodInvokerFn = void* (CHAOS_RUNTIME_ABI_CALL*)(
        RuntimeState* runtime,
        ThreadState* thread,
        void* __this,
        void* const* argv,
        CHAOS_IL2CPP_UINT32 argc);

    CHAOS_IL2CPP_PROFILE_SCOPE("MethodInvoke");

    if (!IsAttached(runtime_state, thread_state) || method == 0) {
        if (out_exception != nullptr) *out_exception = nullptr;
        return CHAOS_RUNTIME_STATUS_INVALID_ARGUMENT;
    }

    void* invoker_ptr = nullptr;
    CHAOS_IL2CPP_UINT32 method_token = 0;
    bool is_token_based = false;

    if (IsLikelyMetadataTokenHandle(method)) {
        method_token = static_cast<CHAOS_IL2CPP_UINT32>(static_cast<CHAOS_IL2CPP_UINTPTR>(method));
        is_token_based = true;
    } else if (const auto* desc = TryDecodeReflectionQueryMethodHandle(method)) {
        method_token = desc->metadata_token;
        is_token_based = true;
    } else {
        invoker_ptr = reinterpret_cast<void*>(method);
    }

    if (is_token_based) {
        invoker_ptr = chaos::il2cpp::bootstrap::FindInvokerPointer(method_token);
        if (invoker_ptr == nullptr) {
            if (method_token >= 0x80000000u) {
                const auto* bridge = ChaosRuntimeInstantiationGetBridgeV0();
                if (bridge != nullptr && bridge->interpret_method_call != nullptr) {
                    return bridge->interpret_method_call(
                        runtime_state, thread_state, method,
                        object_instance, argv, argc,
                        out_return_value, out_return_value_size,
                        out_exception);
                }
            }
            return CHAOS_RUNTIME_STATUS_NOT_FOUND;
        }
    }

    try {
        auto* const invoker = reinterpret_cast<RawMethodInvokerFn>(invoker_ptr);
        if (invoker == nullptr) return CHAOS_RUNTIME_STATUS_NOT_FOUND;

        void* return_value = invoker(runtime_state, thread_state, object_instance, argv, argc);
        if (out_return_value != nullptr) {
            if (out_return_value_size == sizeof(void*)) {
                CHAOS_IL2CPP_MEMCPY(out_return_value, &return_value, sizeof(return_value));
            } else {
                auto* indirect_return_value = reinterpret_cast<void* const*>(out_return_value);
                if (indirect_return_value == nullptr || *indirect_return_value == nullptr) {
                    return CHAOS_RUNTIME_STATUS_INVALID_ARGUMENT;
                }
                if (return_value == nullptr) return CHAOS_RUNTIME_STATUS_INTERNAL_ERROR;
                CHAOS_IL2CPP_MEMCPY(*indirect_return_value, return_value, out_return_value_size);
            }
        }

        return CHAOS_RUNTIME_STATUS_OK;
    } catch (const chaos_managed_exception& carrier) {
        if (out_exception != nullptr) {
            *out_exception = reinterpret_cast<ExceptionHandle>(carrier.object_value);
        }
        return CHAOS_RUNTIME_STATUS_MANAGED_EXCEPTION;
    }
}

ImageHandle CHAOS_RUNTIME_ABI_CALL AssemblyGetImage(AssemblyHandle assembly) {
    return reinterpret_cast<ImageHandle>(assembly);
}

TypeInfoHandle CHAOS_RUNTIME_ABI_CALL ImageFindType(
    ImageHandle image,
    const char* namespace_utf8,
    const char* type_name_utf8) {
    if (image == 0 || namespace_utf8 == nullptr || type_name_utf8 == nullptr) return 0;

    if (const auto* reflection_image = TryDecodeReflectionQueryImageHandle(image)) {
        const auto* type = FindReflectionQueryTypeByName(reflection_image, namespace_utf8, type_name_utf8);
        return EncodeReflectionQueryTypeHandle(type);
    }

    return static_cast<TypeInfoHandle>(image);
}

MethodInfoHandle CHAOS_RUNTIME_ABI_CALL TypeFindMethod(
    TypeInfoHandle type,
    const char* method_name_utf8,
    CHAOS_IL2CPP_INT32 parameter_count) {
    if (type == 0 || method_name_utf8 == nullptr || parameter_count < 0) return 0;

    if (const auto* reflection_type = TryDecodeReflectionQueryTypeHandle(type)) {
        const auto* method = FindReflectionQueryMethod(reflection_type, method_name_utf8, parameter_count);
        return EncodeReflectionQueryMethodHandle(method);
    }

    return static_cast<MethodInfoHandle>(type);
}

FieldInfoHandle CHAOS_RUNTIME_ABI_CALL TypeFindField(
    TypeInfoHandle type,
    const char* field_name_utf8) {
    if (type == 0 || field_name_utf8 == nullptr) return 0;

    if (const auto* reflection_type = TryDecodeReflectionQueryTypeHandle(type)) {
        const auto* field = FindReflectionQueryField(reflection_type, field_name_utf8);
        return EncodeReflectionQueryFieldHandle(field);
    }

    return static_cast<FieldInfoHandle>(type);
}

PropertyInfoHandle CHAOS_RUNTIME_ABI_CALL TypeFindProperty(
    TypeInfoHandle type,
    const char* property_name_utf8) {
    if (type == 0 || property_name_utf8 == nullptr) return 0;

    if (const auto* reflection_type = TryDecodeReflectionQueryTypeHandle(type)) {
        const auto* property = FindReflectionQueryProperty(reflection_type, property_name_utf8);
        return EncodeReflectionQueryPropertyHandle(property);
    }

    return 0;
}

EventInfoHandle CHAOS_RUNTIME_ABI_CALL TypeFindEvent(
    TypeInfoHandle type,
    const char* event_name_utf8) {
    (void)type;
    (void)event_name_utf8;
    return 0;
}

TypeInfoHandle CHAOS_RUNTIME_ABI_CALL TypeGetGenericTypeDefinition(TypeInfoHandle type) {
    if (type == 0) return 0;

    if (const auto* reflection_type = TryDecodeReflectionQueryTypeHandle(type)) {
        return EncodeReflectionQueryTypeHandle(reflection_type->generic_type_definition);
    }

    return 0;
}

ParameterInfoHandle CHAOS_RUNTIME_ABI_CALL MethodGetParameter(
    MethodInfoHandle method,
    CHAOS_IL2CPP_UINT32 parameter_index) {
    if (method == 0) return 0;

    if (const auto* reflection_method = TryDecodeReflectionQueryMethodHandle(method)) {
        const auto* parameter = FindReflectionQueryParameter(reflection_method, parameter_index);
        return EncodeReflectionQueryParameterHandle(parameter);
    }

    return 0;
}

GenericContextHandle CHAOS_RUNTIME_ABI_CALL MethodGetGenericContext(MethodInfoHandle method) {
    if (method == 0) return nullptr;
    const CHAOS_IL2CPP_UINT32 method_token = static_cast<CHAOS_IL2CPP_UINT32>(static_cast<CHAOS_IL2CPP_UINTPTR>(method));
    return chaos::il2cpp::generic_context::GetGenericContextForMethod(method_token);
}

CHAOS_IL2CPP_UINT32 CHAOS_RUNTIME_ABI_CALL GenericContextGetClassArgCount(GenericContextHandle generic_context) {
    return chaos::il2cpp::generic_context::GetClassTypeArgCount(generic_context);
}

TypeInfoHandle CHAOS_RUNTIME_ABI_CALL GenericContextGetClassArg(
    GenericContextHandle generic_context,
    CHAOS_IL2CPP_UINT32 index) {
    return chaos::il2cpp::generic_context::GetClassTypeArg(generic_context, index);
}

CHAOS_IL2CPP_UINT32 CHAOS_RUNTIME_ABI_CALL GenericContextGetMethodArgCount(GenericContextHandle generic_context) {
    return chaos::il2cpp::generic_context::GetMethodTypeArgCount(generic_context);
}

TypeInfoHandle CHAOS_RUNTIME_ABI_CALL GenericContextGetMethodArg(
    GenericContextHandle generic_context,
    CHAOS_IL2CPP_UINT32 index) {
    return chaos::il2cpp::generic_context::GetMethodTypeArg(generic_context, index);
}


/* ── V3 GC introspection no-ops ── */
static int64_t CHAOS_RUNTIME_ABI_CALL _gc_total_memory_noop(RuntimeState*) { return 0; }
static void CHAOS_RUNTIME_ABI_CALL _gc_pressure_noop(RuntimeState*, int64_t) {}
/* ── V1 register_gc_layouts ── */
static void CHAOS_RUNTIME_ABI_CALL _register_gc_noop(void) {}
/* ── V2 no-op sentinel ── */
static uintptr_t _noop_sentinel;
static uintptr_t CHAOS_RUNTIME_ABI_CALL _noop0() { return reinterpret_cast<uintptr_t>(&_noop_sentinel); }
static uintptr_t CHAOS_RUNTIME_ABI_CALL _noop1(uintptr_t) { return reinterpret_cast<uintptr_t>(&_noop_sentinel); }
static bool CHAOS_RUNTIME_ABI_CALL _noop_bool1(uintptr_t) { return false; }
static void CHAOS_RUNTIME_ABI_CALL _noop_void1(uintptr_t) {}
static void CHAOS_RUNTIME_ABI_CALL _noop_void3(uintptr_t, uintptr_t, uintptr_t) {}
/* ── V2 GC handle no-ops ── */
static GCHandle CHAOS_RUNTIME_ABI_CALL _gc_handle_noop_new(RuntimeState*, void*, bool, bool) { return 0; }
static void* CHAOS_RUNTIME_ABI_CALL _gc_handle_noop_get(RuntimeState*, GCHandle) { return nullptr; }
static void CHAOS_RUNTIME_ABI_CALL _gc_handle_noop_set(RuntimeState*, GCHandle, void*) {}


{ /* balance */ }  // anonymous namespace

const RuntimeAbiV0* GetRuntimeAbiV0() {
    // Function-local static: initialized on first call to avoid static init
    // order fiasco with generated code dynamic initializers.
    static const RuntimeAbiV0 s_runtime_abi_v0 = 
    CHAOS_RUNTIME_ABI_V0,
    sizeof(RuntimeAbiV0),
    &RuntimeInit,
    &RuntimeShutdown,
    &ThreadAttach,
    &ThreadDetach,
    &ObjectNew,
    &ArrayNew,
    &StringNewUtf8,
    &ClassInit,
    &GcHandleNew,
    &GcHandleFree,
    &RaiseManagedException,
    &FieldGetValue,
    &FieldSetValue,
    &MethodInvoke,
    &AssemblyGetImage,
    &ImageFindType,
    &TypeFindMethod,
    &TypeFindField,
    &TypeFindProperty,
    &TypeFindEvent,
    &TypeGetGenericTypeDefinition,
    &TypeQueryCapabilityImpl,
    &MethodGetParameter,
    &MethodGetGenericContext,
    &GenericContextGetClassArgCount,
    &GenericContextGetClassArg,
    &GenericContextGetMethodArgCount,
    &GenericContextGetMethodArg,
    /* Extended GC handle helpers (V2) — null, filled at runtime by codegen bridge */
    nullptr,  // gc_handle_new_ex
    nullptr,  // gc_handle_get
    nullptr,  // gc_handle_set
    /* GC memory introspection (V3) */
    nullptr,  // gc_get_total_memory
    nullptr,  // gc_add_memory_pressure
    nullptr,  // gc_remove_memory_pressure
    /* Interop kernel32 */
    nullptr,  // interop_kernel32_get_last_error
    nullptr,  // interop_kernel32_get_current_process_id
    nullptr,  // interop_kernel32_get_current_thread_id
    nullptr,  // interop_kernel32_get_current_process
    nullptr,  // interop_kernel32_get_current_thread
    nullptr,  // interop_kernel32_close_handle
    nullptr,  // interop_kernel32_free_library
    /* Marshal alloc/free/realloc */
    nullptr,  // marshal_alloc_h_global
    nullptr,  // marshal_alloc_co_task_mem
    nullptr,  // marshal_realloc_h_global
    nullptr,  // marshal_realloc_co_task_mem
    nullptr,  // marshal_free_h_global
    nullptr,  // marshal_zero_free_co_task_mem_utf8
    nullptr,  // marshal_string_to_co_task_mem_utf8
    /* Task kernel */
    nullptr,  // task_kernel_new_id
    /* V1 additions: dispatch, hotpatch, module, type registration */
    /* Wrappers: match RuntimeAbiV0 function pointer signatures
     * (plain C pointers, not C++ references or namespaced types). */
    [](const char* name, const struct ::ModuleDescriptor* descriptor) -> uint32_t {
        return RegisterModule(name, reinterpret_cast<const chaos::il2cpp::runtime_core::ModuleDescriptor*>(descriptor));
    },
    [](const struct ::HotpatchEntryV0* entry) -> bool {
        return HotpatchIsActive(*entry);
    },
    [](const struct ::HotpatchEntryV0* entry) -> bool {
        return HotpatchShouldKeepNative(*entry);
    },
    &RaiseNullReferenceException,
    &ChaosExternalRuntimeFallback,
    &InterpreterEntryDirect,
    &ChaosRegisterGcLayouts,
    &RegisterHotpatchModule,
    &ChaosArrayEmpty,
};
    return &s_runtime_abi_v0;
}



const MarshalPlatformAbiRootV1* GetMarshalPlatformAbiRootV1() {
    return &kMarshalPlatformAbiRootV1;
}

const TaskRuntimeKernelV1* GetTaskRuntimeKernelV1() {
    return &kTaskRuntimeKernelV1;
}

}  // namespace chaos::il2cpp::runtime_core