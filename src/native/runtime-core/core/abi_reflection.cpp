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

/* ── V2 extended GC handle helpers ── */

GCHandle CHAOS_RUNTIME_ABI_CALL GcHandleNewEx(
    RuntimeState* runtime_state,
    void* object_instance,
    bool pinned,
    bool weak) {
    if (runtime_state == nullptr || object_instance == nullptr) {
        return CHAOS_GC_HANDLE_INVALID;
    }
    CHAOS_IL2CPP_UINT64 handle;
    if (weak) {
        handle = GcCreateWeakHandle(object_instance);
    } else if (pinned) {
        handle = GcCreatePinnedHandle(object_instance);
    } else {
        handle = GcCreateStrongHandle(object_instance);
    }
    return static_cast<GCHandle>(handle);
}

void* CHAOS_RUNTIME_ABI_CALL GcHandleGet(
    RuntimeState* runtime_state,
    GCHandle gc_handle) {
    if (runtime_state == nullptr || gc_handle == CHAOS_GC_HANDLE_INVALID) return nullptr;
    return GcGetHandleTarget(static_cast<CHAOS_IL2CPP_UINT64>(gc_handle));
}

void CHAOS_RUNTIME_ABI_CALL GcHandleSet(
    RuntimeState* runtime_state,
    GCHandle gc_handle,
    void* object_instance) {
    if (runtime_state == nullptr || gc_handle == CHAOS_GC_HANDLE_INVALID) return;
    GcSetHandleTarget(static_cast<CHAOS_IL2CPP_UINT64>(gc_handle), object_instance);
}

/* ── V3 kernel helper wrappers (delegate to MarshalPlatformAbiRootV1 / TaskRuntimeKernelV1) ── */

CHAOS_IL2CPP_INTPTR CHAOS_RUNTIME_ABI_CALL ABI_InteropKernel32GetLastError(void) {
    const auto* marshal = GetMarshalPlatformAbiRootV1();
    return marshal != nullptr && marshal->interop_kernel32_get_last_error != nullptr
        ? marshal->interop_kernel32_get_last_error() : 0;
}

CHAOS_IL2CPP_UINT32 CHAOS_RUNTIME_ABI_CALL ABI_InteropKernel32GetCurrentProcessId(void) {
    const auto* marshal = GetMarshalPlatformAbiRootV1();
    return marshal != nullptr && marshal->interop_kernel32_get_current_process_id != nullptr
        ? marshal->interop_kernel32_get_current_process_id() : 0;
}

CHAOS_IL2CPP_INT32 CHAOS_RUNTIME_ABI_CALL ABI_InteropKernel32GetCurrentThreadId(void) {
    const auto* marshal = GetMarshalPlatformAbiRootV1();
    return marshal != nullptr && marshal->interop_kernel32_get_current_thread_id != nullptr
        ? marshal->interop_kernel32_get_current_thread_id() : 0;
}

CHAOS_IL2CPP_INTPTR CHAOS_RUNTIME_ABI_CALL ABI_InteropKernel32GetCurrentProcess(void) {
    const auto* marshal = GetMarshalPlatformAbiRootV1();
    return marshal != nullptr && marshal->interop_kernel32_get_current_process != nullptr
        ? marshal->interop_kernel32_get_current_process() : 0;
}

CHAOS_IL2CPP_INTPTR CHAOS_RUNTIME_ABI_CALL ABI_InteropKernel32GetCurrentThread(void) {
    const auto* marshal = GetMarshalPlatformAbiRootV1();
    return marshal != nullptr && marshal->interop_kernel32_get_current_thread != nullptr
        ? marshal->interop_kernel32_get_current_thread() : 0;
}

bool CHAOS_RUNTIME_ABI_CALL ABI_InteropKernel32CloseHandle(CHAOS_IL2CPP_INTPTR handle) {
    const auto* marshal = GetMarshalPlatformAbiRootV1();
    return marshal != nullptr && marshal->interop_kernel32_close_handle != nullptr
        ? marshal->interop_kernel32_close_handle(handle) : false;
}

bool CHAOS_RUNTIME_ABI_CALL ABI_InteropKernel32FreeLibrary(CHAOS_IL2CPP_INTPTR handle) {
    const auto* marshal = GetMarshalPlatformAbiRootV1();
    return marshal != nullptr && marshal->interop_kernel32_free_library != nullptr
        ? marshal->interop_kernel32_free_library(handle) : false;
}

void* CHAOS_RUNTIME_ABI_CALL ABI_MarshalAllocHGlobal(size_t size) {
    const auto* marshal = GetMarshalPlatformAbiRootV1();
    return marshal != nullptr && marshal->marshal_alloc_h_global != nullptr
        ? marshal->marshal_alloc_h_global(size) : nullptr;
}

void* CHAOS_RUNTIME_ABI_CALL ABI_MarshalAllocCoTaskMem(size_t size) {
    const auto* marshal = GetMarshalPlatformAbiRootV1();
    return marshal != nullptr && marshal->marshal_alloc_co_task_mem != nullptr
        ? marshal->marshal_alloc_co_task_mem(size) : nullptr;
}

void* CHAOS_RUNTIME_ABI_CALL ABI_MarshalReallocHGlobal(void* ptr, size_t size) {
    const auto* marshal = GetMarshalPlatformAbiRootV1();
    return marshal != nullptr && marshal->marshal_realloc_h_global != nullptr
        ? marshal->marshal_realloc_h_global(ptr, size) : nullptr;
}

void* CHAOS_RUNTIME_ABI_CALL ABI_MarshalReallocCoTaskMem(void* ptr, size_t size) {
    const auto* marshal = GetMarshalPlatformAbiRootV1();
    return marshal != nullptr && marshal->marshal_realloc_co_task_mem != nullptr
        ? marshal->marshal_realloc_co_task_mem(ptr, size) : nullptr;
}

void CHAOS_RUNTIME_ABI_CALL ABI_MarshalFreeHGlobal(void* ptr) {
    const auto* marshal = GetMarshalPlatformAbiRootV1();
    if (marshal != nullptr && marshal->marshal_free_h_global != nullptr) {
        marshal->marshal_free_h_global(ptr);
    }
}

void CHAOS_RUNTIME_ABI_CALL ABI_MarshalZeroFreeCoTaskMemUtf8(void* ptr) {
    const auto* marshal = GetMarshalPlatformAbiRootV1();
    if (marshal != nullptr && marshal->marshal_zero_free_co_task_mem_utf8 != nullptr) {
        marshal->marshal_zero_free_co_task_mem_utf8(ptr);
    }
}

void* CHAOS_RUNTIME_ABI_CALL ABI_MarshalStringToCoTaskMemUtf8(const char* str) {
    const auto* marshal = GetMarshalPlatformAbiRootV1();
    return marshal != nullptr && marshal->marshal_string_to_co_task_mem_utf8 != nullptr
        ? marshal->marshal_string_to_co_task_mem_utf8(str) : nullptr;
}

int32_t CHAOS_RUNTIME_ABI_CALL ABI_TaskKernelNewId(void) {
    const auto* task = GetTaskRuntimeKernelV1();
    return task != nullptr && task->task_kernel_new_id != nullptr
        ? task->task_kernel_new_id() : 0;
}

/* ── V1 GC/boxing/vtable (wrappers around existing runtime API) ── */

void* CHAOS_RUNTIME_ABI_CALL ABI_GcAlloc(size_t size, int kind) {
    (void)kind;
    return GcAllocate(static_cast<CHAOS_IL2CPP_SIZE>(size));
}

void* CHAOS_RUNTIME_ABI_CALL ABI_GcAllocAtomic(size_t size) {
    return GcAllocateAtomic(static_cast<CHAOS_IL2CPP_SIZE>(size));
}

/* ── V2 thread state helpers ── */

void* CHAOS_RUNTIME_ABI_CALL ABI_GetCurrentRuntimeState(void) {
    return GetCurrentRuntimeState();
}

void* CHAOS_RUNTIME_ABI_CALL ABI_GetCurrentThreadState(void) {
    return GetCurrentThreadState();
}

/* ── V2 exception helpers ── */

void CHAOS_RUNTIME_ABI_CALL ABI_RaiseException(void* exception_obj) {
    chaos_raise_exception(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(exception_obj));
}

/* ── V2 PInvoke error helpers (delegate to marshal ABI) ── */

void CHAOS_RUNTIME_ABI_CALL ABI_SetLastPinvokeError(int32_t error) {
    auto* ts = GetCurrentThreadState();
    if (ts != nullptr) SetLastPInvokeError(ts, error);
}

int32_t CHAOS_RUNTIME_ABI_CALL ABI_GetLastOsError(void) {
    auto* ts = GetCurrentThreadState();
    if (ts == nullptr) return 0;
    return GetLastPInvokeError(ts);
}

void CHAOS_RUNTIME_ABI_CALL ABI_ClearLastOsError(void) {
    auto* ts = GetCurrentThreadState();
    if (ts != nullptr) SetLastPInvokeError(ts, 0);
}

/* ── V2 native library helpers (delegate to marshal ABI) ── */

void* CHAOS_RUNTIME_ABI_CALL ABI_NativeLibraryLoad(const char* path) {
    const auto* marshal = GetMarshalPlatformAbiRootV1();
    return marshal != nullptr && marshal->native_library_load != nullptr
        ? marshal->native_library_load(path) : nullptr;
}

void* CHAOS_RUNTIME_ABI_CALL ABI_NativeLibraryGetProcAddress(void* handle, const char* name) {
    const auto* marshal = GetMarshalPlatformAbiRootV1();
    return marshal != nullptr && marshal->native_library_get_proc_address != nullptr
        ? marshal->native_library_get_proc_address(handle, name) : nullptr;
}

void* CHAOS_RUNTIME_ABI_CALL ABI_TryResolveDllImport(const char* dll_name, const char* entry_point) {
    const auto* marshal = GetMarshalPlatformAbiRootV1();
    return marshal != nullptr && marshal->try_resolve_dll_import != nullptr
        ? marshal->try_resolve_dll_import(dll_name, entry_point) : nullptr;
}

/* ── ABI v0 function table, fully positionally aligned with runtime_abi.h ── */
const RuntimeAbiV0 kRuntimeAbiV0 = {
    CHAOS_RUNTIME_ABI_V0,
    sizeof(RuntimeAbiV0),

    /* ═══ V0 base (28 function pointer fields) ═══ */
    &RuntimeInit,                                    // field  2: runtime_init
    &RuntimeShutdown,                                // field  3: runtime_shutdown
    &ThreadAttach,                                   // field  4: thread_attach
    &ThreadDetach,                                   // field  5: thread_detach
    &ObjectNew,                                      // field  6: object_new
    &ArrayNew,                                       // field  7: array_new
    &StringNewUtf8,                                  // field  8: string_new_utf8
    &ClassInit,                                      // field  9: class_init
    &GcHandleNew,                                    // field 10: gc_handle_new
    &GcHandleFree,                                   // field 11: gc_handle_free
    &RaiseManagedException,                          // field 12: raise_managed_exception
    &FieldGetValue,                                  // field 13: field_get_value
    &FieldSetValue,                                  // field 14: field_set_value
    &MethodInvoke,                                   // field 15: method_invoke
    &AssemblyGetImage,                               // field 16: assembly_get_image
    &ImageFindType,                                  // field 17: image_find_type
    &TypeFindMethod,                                 // field 18: type_find_method
    &TypeFindField,                                  // field 19: type_find_field
    &TypeFindProperty,                               // field 20: type_find_property
    &TypeFindEvent,                                  // field 21: type_find_event
    &TypeGetGenericTypeDefinition,                   // field 22: type_get_generic_type_definition
    &TypeQueryCapabilityImpl,                        // field 23: type_query_capability
    &MethodGetParameter,                             // field 24: method_get_parameter
    &MethodGetGenericContext,                        // field 25: method_get_generic_context
    &GenericContextGetClassArgCount,                 // field 26: generic_context_get_class_arg_count
    &GenericContextGetClassArg,                      // field 27: generic_context_get_class_arg
    &GenericContextGetMethodArgCount,                // field 28: generic_context_get_method_arg_count
    &GenericContextGetMethodArg,                     // field 29: generic_context_get_method_arg

    /* ═══ V2 extended GC handles (3 fields, between V0 and V3 in struct) ═══ */
    &GcHandleNewEx,                                  // field 30: gc_handle_new_ex
    &GcHandleGet,                                    // field 31: gc_handle_get
    &GcHandleSet,                                    // field 32: gc_handle_set

    /* ═══ V3 GC introspection (3 fields) — NOT YET IMPLEMENTED ═══ */
    nullptr,  /* field 33: gc_get_total_memory */
    nullptr,  /* field 34: gc_add_memory_pressure */
    nullptr,  /* field 35: gc_remove_memory_pressure */

    /* ═══ V3 kernel helpers (15 fields) ═══ */
    &ABI_InteropKernel32GetLastError,                // field 36: interop_kernel32_get_last_error
    &ABI_InteropKernel32GetCurrentProcessId,         // field 37: interop_kernel32_get_current_process_id
    &ABI_InteropKernel32GetCurrentThreadId,          // field 38: interop_kernel32_get_current_thread_id
    &ABI_InteropKernel32GetCurrentProcess,           // field 39: interop_kernel32_get_current_process
    &ABI_InteropKernel32GetCurrentThread,            // field 40: interop_kernel32_get_current_thread
    &ABI_InteropKernel32CloseHandle,                 // field 41: interop_kernel32_close_handle
    &ABI_InteropKernel32FreeLibrary,                 // field 42: interop_kernel32_free_library
    &ABI_MarshalAllocHGlobal,                        // field 43: marshal_alloc_h_global
    &ABI_MarshalAllocCoTaskMem,                      // field 44: marshal_alloc_co_task_mem
    &ABI_MarshalReallocHGlobal,                      // field 45: marshal_realloc_h_global
    &ABI_MarshalReallocCoTaskMem,                    // field 46: marshal_realloc_co_task_mem
    &ABI_MarshalFreeHGlobal,                         // field 47: marshal_free_h_global
    &ABI_MarshalZeroFreeCoTaskMemUtf8,               // field 48: marshal_zero_free_co_task_mem_utf8
    &ABI_MarshalStringToCoTaskMemUtf8,               // field 49: marshal_string_to_co_task_mem_utf8
    &ABI_TaskKernelNewId,                            // field 50: task_kernel_new_id

    /* ═══ V1 module/hotpatch/interpreter (9 fields) ═══ */
    &RegisterModule,                                 // field 51: register_module
    &HotpatchIsActive,                               // field 52: hotpatch_is_active
    &HotpatchShouldKeepNative,                       // field 53: hotpatch_should_keep_native
    &RaiseNullReferenceException,                    // field 54: raise_null_reference_exception
    &ChaosExternalRuntimeFallback,                   // field 55: external_runtime_fallback
    &InterpreterEntryDirect,                         // field 56: interpreter_entry_direct
    &ChaosRegisterGcLayouts,                         // field 57: register_gc_layouts
    &RegisterHotpatchModule,                         // field 58: register_hotpatch_module
    &ChaosArrayEmpty,                                // field 59: array_empty

    /* ═══ V1 GC/boxing/vtable/thread-static (10 fields) ═══ */
    &ABI_GcAlloc,                /* field 60: gc_alloc */
    &ABI_GcAllocAtomic,          /* field 61: gc_alloc_atomic */
    nullptr,  /* field 62: box_value_object */
    nullptr,  /* field 63: resolve_virtual_method */
    nullptr,  /* field 64: resolve_method_table */
    nullptr,  /* field 65: get_type_info_handle */
    nullptr,  /* field 66: resolve_string_id */
    nullptr,  /* field 67: get_thread_static */
    nullptr,  /* field 68: set_thread_static */
    nullptr,  /* field 69: allocate_thread_static */

    /* ═══ V2 exception/marshal/PInvoke/thread-state (20 fields) ═══ */
    &ABI_RaiseException,                         /* field 70: raise_exception */
    nullptr,  /* field 71: marshal_is_rcw_handle */
    nullptr,  /* field 72: marshal_get_rcw_unknown */
    nullptr,  /* field 73: throw_com_exception_for_hr */
    nullptr,  /* field 74: delegate_hotpatch_checkpoint */
    nullptr,  /* field 75: marshal_free_co_task_mem */
    nullptr,  /* field 76: marshal_ptr_to_string_utf8 */
    nullptr,  /* field 77: marshal_ptr_to_string_wide */
    nullptr,  /* field 78: marshal_safe_handle_get_handle */
    nullptr,  /* field 79: marshal_struct_managed_to_native */
    nullptr,  /* field 80: marshal_struct_native_to_managed */
    &ABI_NativeLibraryLoad,                      /* field 81: native_library_load */
    &ABI_NativeLibraryGetProcAddress,            /* field 82: native_library_get_proc_address */
    &ABI_SetLastPinvokeError,                    /* field 83: set_last_pinvoke_error */
    &ABI_GetLastOsError,                         /* field 84: get_last_os_error */
    &ABI_ClearLastOsError,                       /* field 85: clear_last_os_error */
    &ABI_TryResolveDllImport,                    /* field 86: try_resolve_dll_import */
    nullptr,  /* field 87: gc_register_finalizable */
    &ABI_GetCurrentRuntimeState,                  /* field 88: get_current_runtime_state */
    &ABI_GetCurrentThreadState,                   /* field 89: get_current_thread_state */
};

}  // anonymous namespace

const RuntimeAbiV0* GetRuntimeAbiV0() {
    return &kRuntimeAbiV0;
}

const MarshalPlatformAbiRootV1* GetMarshalPlatformAbiRootV1() {
    return &kMarshalPlatformAbiRootV1;
}

const TaskRuntimeKernelV1* GetTaskRuntimeKernelV1() {
    return &kTaskRuntimeKernelV1;
}

}  // namespace chaos::il2cpp::runtime_core