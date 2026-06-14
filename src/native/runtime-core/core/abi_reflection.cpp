// Required forward declarations: ChaosRegisterGcLayouts etc.
#include "chaos_runtime_host.h"
#include <generic_context.h>
#include <bootstrap.h>
#include <instantiation_engine.h>

// ── Forward declarations for functions defined in other .cpp files ──
// These are used to populate the RuntimeAbiV0 dispatch table below.
// Declarations were previously in the generated runtime_abi.h but that
// file is now part of the per-chunk SDK rather than the runtime library.

RuntimeStatus CHAOS_RUNTIME_ABI_CALL RuntimeInit(
    const RuntimeInitParams* init_params,
    const RuntimeConfig* config,
    RuntimeState** out_runtime_state);
void CHAOS_RUNTIME_ABI_CALL RuntimeShutdown(RuntimeState* runtime_state);
RuntimeStatus CHAOS_RUNTIME_ABI_CALL ThreadAttach(
    RuntimeState* runtime_state, ThreadState** out_thread_state);
void CHAOS_RUNTIME_ABI_CALL ThreadDetach(
    RuntimeState* runtime_state, ThreadState* thread_state);
void* CHAOS_RUNTIME_ABI_CALL ObjectNew(
    RuntimeState* runtime_state, ThreadState* thread_state, TypeInfoHandle type);
void* CHAOS_RUNTIME_ABI_CALL ArrayNew(
    RuntimeState* runtime_state, ThreadState* thread_state,
    TypeInfoHandle element_type, uintptr_t length);
void* CHAOS_RUNTIME_ABI_CALL StringNewUtf8(
    RuntimeState* runtime_state, ThreadState* thread_state,
    const char* utf8_bytes, CHAOS_IL2CPP_UINTPTR byte_count);
RuntimeStatus CHAOS_RUNTIME_ABI_CALL ClassInit(
    RuntimeState* runtime_state, TypeInfoHandle type);
RuntimeStatus CHAOS_RUNTIME_ABI_CALL TypeQueryCapabilityImpl(
    TypeInfoHandle type, RuntimeTypeCapabilityInfoV0* out_capability_info);
CHAOS_RUNTIME_ABI_CALL ChaosRuntimeInstantiationGetBridgeV0(void);

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

/* ── V1 ABI wrapper functions (type-safe bridges from void* to runtime types) ── */

// register_module: void* descriptor → ModuleDescriptor*
uint32_t CHAOS_RUNTIME_ABI_CALL RegisterModuleWrapper(
    const char* name, const void* descriptor)
{
    return RegisterModule(name, static_cast<const ModuleDescriptor*>(descriptor));
}

// hotpatch_is_active: void* entry → HotpatchEntryV0&
bool CHAOS_RUNTIME_ABI_CALL HotpatchIsActiveWrapper(const void* entry)
{
    return HotpatchIsActive(*static_cast<const HotpatchEntryV0*>(entry));
}

// hotpatch_should_keep_native: void* entry → HotpatchEntryV0&
bool CHAOS_RUNTIME_ABI_CALL HotpatchShouldKeepNativeWrapper(const void* entry)
{
    return HotpatchShouldKeepNative(*static_cast<const HotpatchEntryV0*>(entry));
}

// raise_null_reference_exception: void wrapper
void CHAOS_RUNTIME_ABI_CALL RaiseNullReferenceExceptionWrapper(void)
{
    RaiseNullReferenceException();
}

// external_runtime_fallback: CHAOS_IL2CPP_INTPTR → uintptr_t
uintptr_t CHAOS_RUNTIME_ABI_CALL ChaosExternalRuntimeFallbackWrapper(
    const char* subject_id)
{
    return static_cast<uintptr_t>(ChaosExternalRuntimeFallback(subject_id));
}

// interpreter_entry_direct: uintptr_t key → CHAOS_IL2CPP_UINTPTR
void CHAOS_RUNTIME_ABI_CALL InterpreterEntryDirectWrapper(
    uintptr_t method_key, void* args_buf, void* ret_buf)
{
    InterpreterEntryDirect(method_key, args_buf, ret_buf);
}

// register_hotpatch_module: void* module → HotpatchModuleV0*
void CHAOS_RUNTIME_ABI_CALL RegisterHotpatchModuleWrapper(const void* module)
{
    RegisterHotpatchModule(static_cast<const HotpatchModuleV0*>(module));
}

// array_empty: CHAOS_IL2CPP_INTPTR → uintptr_t
uintptr_t CHAOS_RUNTIME_ABI_CALL ChaosArrayEmptyWrapper(void)
{
    return static_cast<uintptr_t>(ChaosArrayEmpty());
}


/* ── V3 GC introspection no-ops ── */
static int64_t CHAOS_RUNTIME_ABI_CALL _gc_total_memory_noop(RuntimeState*) { return 0; }
static void CHAOS_RUNTIME_ABI_CALL _gc_pressure_noop(RuntimeState*, int64_t) {}
/* ── V1 register_gc_layouts ── */
static void CHAOS_RUNTIME_ABI_CALL _register_gc_noop(void) {}
/* ── CHAOS_IL2CPP_INTPTR sentinel for V2 no-ops ── */
static uintptr_t _noop_sentinel;
static uintptr_t CHAOS_RUNTIME_ABI_CALL _noop0() { return reinterpret_cast<uintptr_t>(&_noop_sentinel); }
static uintptr_t CHAOS_RUNTIME_ABI_CALL _noop1(uintptr_t) { return reinterpret_cast<uintptr_t>(&_noop_sentinel); }
static bool CHAOS_RUNTIME_ABI_CALL _noop_bool1(CHAOS_IL2CPP_INTPTR) { return false; }
static void CHAOS_RUNTIME_ABI_CALL _noop_void1(CHAOS_IL2CPP_INTPTR) {}
static void CHAOS_RUNTIME_ABI_CALL _noop_void3(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR) {}

/* ── V2 GC handle no-ops ── */
static GCHandle CHAOS_RUNTIME_ABI_CALL _gc_handle_noop_new(RuntimeState*, void*, bool, bool) { return 0; }
static void* CHAOS_RUNTIME_ABI_CALL _gc_handle_noop_get(RuntimeState*, GCHandle) { return nullptr; }
static void CHAOS_RUNTIME_ABI_CALL _gc_handle_noop_set(RuntimeState*, GCHandle, void*) {}
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
    &_gc_handle_noop_new,
    &_gc_handle_noop_get,
    &_gc_handle_noop_set,
    &_gc_total_memory_noop,  // gc_get_total_memory
    &_gc_pressure_noop,  // gc_add_memory_pressure
    &_gc_pressure_noop,  // gc_remove_memory_pressure
    &RegisterModuleWrapper,
    &HotpatchIsActiveWrapper,
    &HotpatchShouldKeepNativeWrapper,
    &RaiseNullReferenceExceptionWrapper,
    &ChaosExternalRuntimeFallbackWrapper,
    &InterpreterEntryDirectWrapper,
    &_register_gc_noop,  // register_gc_layouts
    &RegisterHotpatchModuleWrapper,
    &ChaosArrayEmptyWrapper,
    &_noop0,  // V2 extended
    &_noop0,  // V2 extended
    &_noop0,  // V2 extended
    &_noop0,  // V2 extended
    &_noop0,  // V2 extended
    &_noop0,  // V2 extended
    &_noop0,  // V2 extended
    &_noop0,  // V2 extended
    &_noop0,  // V2 extended
    &_noop0,  // V2 extended
    &_noop0,  // V2 extended
    &_noop0,  // V2 extended
    &_noop0,  // V2 extended
    &_noop0,  // V2 extended
    &_noop0,  // V2 extended
    &_noop0,  // V2 extended
    &_noop0,  // V2 extended
    &_noop0,  // V2 extended
    &_noop0,  // V2 extended
    &_noop0,  // V2 extended
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