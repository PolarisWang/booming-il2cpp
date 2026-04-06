#include "runtime_core.h"

#include "reflection_query_model.h"

#include <cstdlib>
#include <cstring>

struct RuntimeState {
    RuntimeConfig config;
};

struct ThreadState {
    RuntimeState* runtime_state;
};

namespace chaos::il2cpp::runtime_core {

namespace {

constexpr size_t kInlineFieldStorageSize = sizeof(void*) * 4u;

struct ObjectHeader {
    TypeInfoHandle type;
    unsigned char field_storage[kInlineFieldStorageSize];
};

struct StringObjectHeader {
    TypeInfoHandle type;
    uintptr_t byte_count;
};

void* CHAOS_RUNTIME_ABI_CALL DefaultAllocate(size_t size, void* user_data) {
    (void)user_data;
    return std::malloc(size);
}

void CHAOS_RUNTIME_ABI_CALL DefaultDeallocate(void* ptr, void* user_data) {
    (void)user_data;
    std::free(ptr);
}

bool TryNormalizeConfig(const RuntimeConfig* config, RuntimeConfig* out_config) {
    if (out_config == nullptr) {
        return false;
    }

    RuntimeConfig normalized = {};
    if (config != nullptr) {
        if (config->struct_size < sizeof(RuntimeConfig)) {
            return false;
        }

        normalized = *config;
    }

    if ((normalized.allocator == nullptr) != (normalized.deallocator == nullptr)) {
        return false;
    }

    if (normalized.allocator == nullptr) {
        normalized.allocator = &DefaultAllocate;
        normalized.deallocator = &DefaultDeallocate;
        normalized.allocator_user_data = nullptr;
    }

    normalized.struct_size = sizeof(RuntimeConfig);
    *out_config = normalized;
    return true;
}

void* AllocateBytes(const RuntimeConfig& config, size_t size) {
    if (config.allocator == nullptr) {
        return nullptr;
    }

    return config.allocator(size, config.allocator_user_data);
}

void FreeBytes(const RuntimeConfig& config, void* ptr) {
    if (ptr == nullptr || config.deallocator == nullptr) {
        return;
    }

    config.deallocator(ptr, config.allocator_user_data);
}

bool IsAttached(RuntimeState* runtime_state, ThreadState* thread_state) {
    return runtime_state != nullptr && thread_state != nullptr && thread_state->runtime_state == runtime_state;
}

RuntimeStatus CHAOS_RUNTIME_ABI_CALL RuntimeInit(
    const RuntimeInitParams* init_params,
    const RuntimeConfig* config,
    RuntimeState** out_runtime_state) {
    if (init_params == nullptr || out_runtime_state == nullptr) {
        return CHAOS_RUNTIME_STATUS_INVALID_ARGUMENT;
    }

    *out_runtime_state = nullptr;
    if (init_params->struct_size < sizeof(RuntimeInitParams)) {
        return CHAOS_RUNTIME_STATUS_INVALID_ARGUMENT;
    }

    RuntimeConfig normalized_config = {};
    if (!TryNormalizeConfig(config, &normalized_config)) {
        return CHAOS_RUNTIME_STATUS_INVALID_ARGUMENT;
    }

    RuntimeState* runtime_state = static_cast<RuntimeState*>(AllocateBytes(normalized_config, sizeof(RuntimeState)));
    if (runtime_state == nullptr) {
        return CHAOS_RUNTIME_STATUS_INTERNAL_ERROR;
    }

    runtime_state->config = normalized_config;
    *out_runtime_state = runtime_state;
    return CHAOS_RUNTIME_STATUS_OK;
}

void CHAOS_RUNTIME_ABI_CALL RuntimeShutdown(RuntimeState* runtime_state) {
    if (runtime_state == nullptr) {
        return;
    }

    FreeBytes(runtime_state->config, runtime_state);
}

RuntimeStatus CHAOS_RUNTIME_ABI_CALL ThreadAttach(
    RuntimeState* runtime_state,
    ThreadState** out_thread_state) {
    if (runtime_state == nullptr || out_thread_state == nullptr) {
        return CHAOS_RUNTIME_STATUS_INVALID_ARGUMENT;
    }

    *out_thread_state = nullptr;
    ThreadState* thread_state = static_cast<ThreadState*>(AllocateBytes(runtime_state->config, sizeof(ThreadState)));
    if (thread_state == nullptr) {
        return CHAOS_RUNTIME_STATUS_INTERNAL_ERROR;
    }

    thread_state->runtime_state = runtime_state;
    *out_thread_state = thread_state;
    return CHAOS_RUNTIME_STATUS_OK;
}

void CHAOS_RUNTIME_ABI_CALL ThreadDetach(
    RuntimeState* runtime_state,
    ThreadState* thread_state) {
    if (runtime_state == nullptr || thread_state == nullptr) {
        return;
    }

    FreeBytes(runtime_state->config, thread_state);
}

void* CHAOS_RUNTIME_ABI_CALL ObjectNew(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    TypeInfoHandle type) {
    if (!IsAttached(runtime_state, thread_state)) {
        return nullptr;
    }

    ObjectHeader* object = static_cast<ObjectHeader*>(AllocateBytes(runtime_state->config, sizeof(ObjectHeader)));
    if (object == nullptr) {
        return nullptr;
    }

    object->type = type;
    std::memset(object->field_storage, 0, sizeof(object->field_storage));
    return object;
}

void* CHAOS_RUNTIME_ABI_CALL ArrayNew(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    TypeInfoHandle element_type,
    uintptr_t length) {
    (void)runtime_state;
    (void)thread_state;
    (void)element_type;
    (void)length;
    return nullptr;
}

void* CHAOS_RUNTIME_ABI_CALL StringNewUtf8(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    const char* utf8_bytes,
    uintptr_t byte_count) {
    if (!IsAttached(runtime_state, thread_state)) {
        return nullptr;
    }

    if (utf8_bytes == nullptr && byte_count != 0u) {
        return nullptr;
    }

    const size_t allocation_size = sizeof(StringObjectHeader) + static_cast<size_t>(byte_count) + 1u;
    unsigned char* storage = static_cast<unsigned char*>(AllocateBytes(runtime_state->config, allocation_size));
    if (storage == nullptr) {
        return nullptr;
    }

    auto* header = reinterpret_cast<StringObjectHeader*>(storage);
    header->type = nullptr;
    header->byte_count = byte_count;

    char* text = reinterpret_cast<char*>(storage + sizeof(StringObjectHeader));
    if (byte_count != 0u) {
        std::memcpy(text, utf8_bytes, static_cast<size_t>(byte_count));
    }

    text[byte_count] = '\0';
    return header;
}

RuntimeStatus CHAOS_RUNTIME_ABI_CALL ClassInit(
    RuntimeState* runtime_state,
    TypeInfoHandle type) {
    if (runtime_state == nullptr) {
        return CHAOS_RUNTIME_STATUS_INVALID_ARGUMENT;
    }

    return type != nullptr ? CHAOS_RUNTIME_STATUS_OK : CHAOS_RUNTIME_STATUS_NOT_FOUND;
}

GCHandle CHAOS_RUNTIME_ABI_CALL GcHandleNew(
    RuntimeState* runtime_state,
    void* object_instance,
    bool pinned) {
    (void)runtime_state;
    (void)object_instance;
    (void)pinned;
    return CHAOS_GC_HANDLE_INVALID;
}

void CHAOS_RUNTIME_ABI_CALL GcHandleFree(
    RuntimeState* runtime_state,
    GCHandle gc_handle) {
    (void)runtime_state;
    (void)gc_handle;
}

void CHAOS_RUNTIME_ABI_CALL RaiseManagedException(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    ExceptionHandle exception) {
    (void)runtime_state;
    (void)thread_state;
    (void)exception;
}

RuntimeStatus CHAOS_RUNTIME_ABI_CALL FieldGetValue(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    FieldInfoHandle field,
    void* object_instance,
    void* out_value,
    size_t out_value_size) {
    if (!IsAttached(runtime_state, thread_state)
        || object_instance == nullptr
        || out_value == nullptr
        || out_value_size == 0u
        || out_value_size > kInlineFieldStorageSize) {
        return CHAOS_RUNTIME_STATUS_INVALID_ARGUMENT;
    }

    if (field == nullptr) {
        return CHAOS_RUNTIME_STATUS_NOT_FOUND;
    }

    auto* object = static_cast<ObjectHeader*>(object_instance);
    std::memcpy(out_value, object->field_storage, out_value_size);
    return CHAOS_RUNTIME_STATUS_OK;
}

RuntimeStatus CHAOS_RUNTIME_ABI_CALL FieldSetValue(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    FieldInfoHandle field,
    void* object_instance,
    const void* value,
    size_t value_size) {
    if (!IsAttached(runtime_state, thread_state)
        || object_instance == nullptr
        || value == nullptr
        || value_size == 0u
        || value_size > kInlineFieldStorageSize) {
        return CHAOS_RUNTIME_STATUS_INVALID_ARGUMENT;
    }

    if (field == nullptr) {
        return CHAOS_RUNTIME_STATUS_NOT_FOUND;
    }

    auto* object = static_cast<ObjectHeader*>(object_instance);
    std::memcpy(object->field_storage, value, value_size);
    return CHAOS_RUNTIME_STATUS_OK;
}

RuntimeStatus CHAOS_RUNTIME_ABI_CALL MethodInvoke(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    MethodInfoHandle method,
    void* object_instance,
    void* const* argv,
    uint32_t argc,
    void* out_return_value,
    size_t out_return_value_size,
    ExceptionHandle* out_exception) {
    (void)runtime_state;
    (void)thread_state;
    (void)method;
    (void)object_instance;
    (void)argv;
    (void)argc;
    (void)out_return_value;
    (void)out_return_value_size;
    (void)out_exception;
    return CHAOS_RUNTIME_STATUS_NOT_SUPPORTED;
}

ImageHandle CHAOS_RUNTIME_ABI_CALL AssemblyGetImage(AssemblyHandle assembly) {
    return reinterpret_cast<ImageHandle>(assembly);
}

TypeInfoHandle CHAOS_RUNTIME_ABI_CALL ImageFindType(
    ImageHandle image,
    const char* namespace_utf8,
    const char* type_name_utf8) {
    if (image == nullptr || namespace_utf8 == nullptr || type_name_utf8 == nullptr) {
        return nullptr;
    }

    if (const auto* reflection_image = TryDecodeReflectionQueryImageHandle(image)) {
        const auto* type = FindReflectionQueryTypeByName(reflection_image, namespace_utf8, type_name_utf8);
        return EncodeReflectionQueryTypeHandle(type);
    }

    return reinterpret_cast<TypeInfoHandle>(image);
}

MethodInfoHandle CHAOS_RUNTIME_ABI_CALL TypeFindMethod(
    TypeInfoHandle type,
    const char* method_name_utf8,
    int32_t parameter_count) {
    if (type == nullptr || method_name_utf8 == nullptr || parameter_count < 0) {
        return nullptr;
    }

    if (const auto* reflection_type = TryDecodeReflectionQueryTypeHandle(type)) {
        const auto* method = FindReflectionQueryMethod(reflection_type, method_name_utf8, parameter_count);
        return EncodeReflectionQueryMethodHandle(method);
    }

    return reinterpret_cast<MethodInfoHandle>(type);
}

FieldInfoHandle CHAOS_RUNTIME_ABI_CALL TypeFindField(
    TypeInfoHandle type,
    const char* field_name_utf8) {
    if (type == nullptr || field_name_utf8 == nullptr) {
        return nullptr;
    }

    if (const auto* reflection_type = TryDecodeReflectionQueryTypeHandle(type)) {
        const auto* field = FindReflectionQueryField(reflection_type, field_name_utf8);
        return EncodeReflectionQueryFieldHandle(field);
    }

    return reinterpret_cast<FieldInfoHandle>(type);
}

PropertyInfoHandle CHAOS_RUNTIME_ABI_CALL TypeFindProperty(
    TypeInfoHandle type,
    const char* property_name_utf8) {
    if (type == nullptr || property_name_utf8 == nullptr) {
        return nullptr;
    }

    if (const auto* reflection_type = TryDecodeReflectionQueryTypeHandle(type)) {
        const auto* property = FindReflectionQueryProperty(reflection_type, property_name_utf8);
        return EncodeReflectionQueryPropertyHandle(property);
    }

    return nullptr;
}

EventInfoHandle CHAOS_RUNTIME_ABI_CALL TypeFindEvent(
    TypeInfoHandle type,
    const char* event_name_utf8) {
    (void)type;
    (void)event_name_utf8;
    return nullptr;
}

TypeInfoHandle CHAOS_RUNTIME_ABI_CALL TypeGetGenericTypeDefinition(TypeInfoHandle type) {
    if (type == nullptr) {
        return nullptr;
    }

    if (const auto* reflection_type = TryDecodeReflectionQueryTypeHandle(type)) {
        return EncodeReflectionQueryTypeHandle(reflection_type->generic_type_definition);
    }

    return nullptr;
}

ParameterInfoHandle CHAOS_RUNTIME_ABI_CALL MethodGetParameter(
    MethodInfoHandle method,
    uint32_t parameter_index) {
    if (method == nullptr) {
        return nullptr;
    }

    if (const auto* reflection_method = TryDecodeReflectionQueryMethodHandle(method)) {
        const auto* parameter = FindReflectionQueryParameter(reflection_method, parameter_index);
        return EncodeReflectionQueryParameterHandle(parameter);
    }

    return nullptr;
}

GenericContextHandle CHAOS_RUNTIME_ABI_CALL MethodGetGenericContext(MethodInfoHandle method) {
    (void)method;
    return nullptr;
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
    &MethodGetParameter,
    &MethodGetGenericContext,
};

}  // namespace

const RuntimeAbiV0* GetRuntimeAbiV0() {
    return &kRuntimeAbiV0;
}

}  // namespace chaos::il2cpp::runtime_core

extern "C" CHAOS_RUNTIME_ABI_EXPORT const RuntimeAbiV0* CHAOS_RUNTIME_ABI_CALL chaos_runtime_get_abi_v0(void) {
    return chaos::il2cpp::runtime_core::GetRuntimeAbiV0();
}

