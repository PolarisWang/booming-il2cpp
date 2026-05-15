namespace chaos::il2cpp::runtime_core {
namespace {

GCHandle CHAOS_RUNTIME_ABI_CALL GcHandleNew(
    RuntimeState* runtime_state,
    void* object_instance,
    bool pinned) {
    if (runtime_state == nullptr || object_instance == nullptr) {
        return CHAOS_GC_HANDLE_INVALID;
    }

    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(s_gc_handle_mutex);
    CHAOS_IL2CPP_UINT64 handle = s_next_gc_handle++;
    s_gc_handle_table[handle] = GcHandleEntry{ object_instance, pinned, false /*weak*/ };

    if (pinned) {
        GC_add_roots(object_instance,
            static_cast<char*>(object_instance) + sizeof(void*));
    }

    return static_cast<GCHandle>(handle);
}

void CHAOS_RUNTIME_ABI_CALL GcHandleFree(
    RuntimeState* runtime_state,
    GCHandle gc_handle) {
    if (runtime_state == nullptr || gc_handle == CHAOS_GC_HANDLE_INVALID) return;

    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(s_gc_handle_mutex);
    auto it = s_gc_handle_table.find(static_cast<CHAOS_IL2CPP_UINT64>(gc_handle));
    if (it != s_gc_handle_table.end()) {
        s_gc_handle_table.erase(it);
    }
}

void CHAOS_RUNTIME_ABI_CALL RaiseManagedException(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    ExceptionHandle exception) {
    if (!IsAttached(runtime_state, thread_state)) return;

    throw chaos_managed_exception
    {
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(exception),
    };
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

const RuntimeAbiV0 kRuntimeAbiV0 = {
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