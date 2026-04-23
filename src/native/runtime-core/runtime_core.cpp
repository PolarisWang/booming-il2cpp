#include "runtime_core.h"

#include "reflection_query_model.h"
#include "generic_context.h"
#include "vtable_registry.h"

#include <gc.h>

#include <cstdio>
#include <atomic>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <mutex>
#include <new>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

struct ThreadRootRecord {
    const void* address;
    size_t size;
};

struct FinalizerWorkItem {
    void* object_instance;
    chaos::il2cpp::runtime_core::FinalizerCallback finalizer;
};

struct RuntimeInternalState {
    std::mutex finalizer_queue_mutex;
    std::vector<FinalizerWorkItem> finalizer_queue = {};
};

struct ThreadInternalState {
    std::unordered_map<std::string, int32_t> thread_static_int32_slots = {};
    std::vector<ThreadRootRecord> reported_roots = {};
    bool at_gc_safepoint = false;
};

struct RuntimeState {
    RuntimeConfig config;
    RuntimeInternalState* internal_state;
};

struct ThreadState {
    RuntimeState* runtime_state;
    ThreadInternalState* internal_state;
};

namespace chaos::il2cpp::runtime_core {

namespace {

constexpr size_t kInlineFieldStorageSize = sizeof(void*) * 4u;
// struct ManagedExceptionCarrier is declared in runtime_core.h and used as the cold EH payload.

struct ObjectHeader {
    TypeInfoHandle type;
    unsigned char field_storage[kInlineFieldStorageSize];
};

struct StringObjectHeader {
    TypeInfoHandle type;
    uintptr_t byte_count;
};

struct ArrayHeader {
    TypeInfoHandle element_type;
    uintptr_t length;
};

struct BoxedValueHeader {
    TypeInfoHandle type;
    uintptr_t byte_count;
};

struct EngineLifecycleRegistration {
    std::string phase;
    EngineLifecycleCallback callback;
    void* user_data;
};

constexpr const char* kEngineObservePrefix = "CHAOS_ENGINE_OBSERVE ";

std::mutex g_engine_binding_mutex;
uintptr_t g_next_engine_handle = 1u;
std::unordered_map<uintptr_t, void*> g_engine_handles = {};
std::vector<EngineLifecycleRegistration> g_engine_lifecycle_registrations = {};
std::mutex g_monitor_registry_mutex;
std::unordered_map<void*, std::shared_ptr<std::recursive_mutex>> g_monitor_registry = {};
const std::thread::id g_main_thread_id = std::this_thread::get_id();
std::atomic<RuntimeMode> g_runtime_mode = RuntimeMode::Aot;

// GC handle table: maps handle IDs to object instances.
// Pinned handles are registered as explicit GC roots so the collector never moves/collects them.
struct GcHandleEntry {
    void* object_instance;
    bool pinned;
};
static std::mutex s_gc_handle_mutex;
static std::atomic<uint64_t> s_next_gc_handle{1};
static std::unordered_map<uint64_t, GcHandleEntry> s_gc_handle_table;

void* CHAOS_RUNTIME_ABI_CALL DefaultAllocate(size_t size, void* user_data) {
    (void)user_data;
    return GC_MALLOC(size);
}

void CHAOS_RUNTIME_ABI_CALL DefaultDeallocate(void* ptr, void* user_data) {
    (void)user_data;
    (void)ptr;
    // GC manages deallocation automatically — no explicit free needed
}

// Allocate memory that contains no pointers (e.g., string bytes, boxed value data).
// GC_MALLOC_ATOMIC allows the GC to skip scanning this region for pointers,
// improving collection performance.
static void* AllocateBytesAtomic(size_t size) {
    return GC_MALLOC_ATOMIC(size);
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

RuntimeInternalState* GetRuntimeInternalState(RuntimeState* runtime_state) {
    return runtime_state != nullptr ? runtime_state->internal_state : nullptr;
}

ThreadInternalState* GetThreadInternalState(ThreadState* thread_state) {
    return thread_state != nullptr ? thread_state->internal_state : nullptr;
}

std::shared_ptr<std::recursive_mutex> GetOrCreateMonitor(void* monitor_target) {
    if (monitor_target == nullptr) {
        return nullptr;
    }

    std::lock_guard<std::mutex> lock(g_monitor_registry_mutex);
    auto& monitor = g_monitor_registry[monitor_target];
    if (!monitor) {
        monitor = std::make_shared<std::recursive_mutex>();
    }

    return monitor;
}

bool IsLikelyMetadataTokenHandle(MethodInfoHandle method) {
    const uintptr_t raw_method = reinterpret_cast<uintptr_t>(method);
    return raw_method != 0u && raw_method <= static_cast<uintptr_t>(0x0FFFFFFFu);
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

    // Initialize BDWGC exactly once per process.
    static std::once_flag s_gc_init_flag;
    std::call_once(s_gc_init_flag, []() { GC_INIT(); });

    RuntimeState* runtime_state = static_cast<RuntimeState*>(AllocateBytes(normalized_config, sizeof(RuntimeState)));
    if (runtime_state == nullptr) {
        return CHAOS_RUNTIME_STATUS_INTERNAL_ERROR;
    }

    runtime_state->config = normalized_config;
    runtime_state->internal_state = new (std::nothrow) RuntimeInternalState();
    if (runtime_state->internal_state == nullptr) {
        FreeBytes(normalized_config, runtime_state);
        return CHAOS_RUNTIME_STATUS_INTERNAL_ERROR;
    }

    SetRuntimeMode(RuntimeMode::Aot);
    *out_runtime_state = runtime_state;
    return CHAOS_RUNTIME_STATUS_OK;
}

void CHAOS_RUNTIME_ABI_CALL RuntimeShutdown(RuntimeState* runtime_state) {
    if (runtime_state == nullptr) {
        return;
    }

    SetRuntimeMode(RuntimeMode::Aot);
    delete runtime_state->internal_state;
    runtime_state->internal_state = nullptr;
    // RuntimeState itself is GC-managed; no explicit free needed.
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
    thread_state->internal_state = new (std::nothrow) ThreadInternalState();
    if (thread_state->internal_state == nullptr) {
        FreeBytes(runtime_state->config, thread_state);
        return CHAOS_RUNTIME_STATUS_INTERNAL_ERROR;
    }

    // Register this thread with BDWGC so it can scan the thread's stack for roots.
    // On Windows (GC_WIN32_THREADS), BDWGC auto-registers threads via DllMain / Win32 hooks;
    // explicit registration is neither needed nor allowed.
    // On pthreads platforms, we must register manually.
#if !defined(_WIN32) && !defined(_WIN64)
    struct GC_stack_base sb;
    if (GC_get_stack_base(&sb) == GC_SUCCESS) {
        const int gc_reg_result = GC_register_my_thread(&sb);
        (void)gc_reg_result;  // GC_DUPLICATE is fine for the main thread
    }
#endif

    *out_thread_state = thread_state;
    return CHAOS_RUNTIME_STATUS_OK;
}

void CHAOS_RUNTIME_ABI_CALL ThreadDetach(
    RuntimeState* runtime_state,
    ThreadState* thread_state) {
    if (runtime_state == nullptr || thread_state == nullptr) {
        return;
    }

    // Unregister this thread from BDWGC on pthreads platforms only.
    // On Windows, BDWGC auto-manages thread lifecycle via Win32 hooks.
#if !defined(_WIN32) && !defined(_WIN64)
    GC_unregister_my_thread();
#endif

    delete thread_state->internal_state;
    thread_state->internal_state = nullptr;
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
    if (!IsAttached(runtime_state, thread_state) || element_type == nullptr) {
        return nullptr;
    }

    const size_t allocation_size =
        sizeof(ArrayHeader) + (static_cast<size_t>(length) * sizeof(void*));
    unsigned char* storage = static_cast<unsigned char*>(AllocateBytes(runtime_state->config, allocation_size));
    if (storage == nullptr) {
        return nullptr;
    }

    auto* header = reinterpret_cast<ArrayHeader*>(storage);
    header->element_type = element_type;
    header->length = length;

    void** elements = reinterpret_cast<void**>(storage + sizeof(ArrayHeader));
    if (length != 0u) {
        std::memset(elements, 0, static_cast<size_t>(length) * sizeof(void*));
    }

    return header;
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
    // Use atomic allocation: string bytes contain no pointers, so GC need not scan them.
    unsigned char* storage = static_cast<unsigned char*>(AllocateBytesAtomic(allocation_size));
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
    if (runtime_state == nullptr || object_instance == nullptr) {
        return CHAOS_GC_HANDLE_INVALID;
    }

    std::lock_guard<std::mutex> lock(s_gc_handle_mutex);
    uint64_t handle = s_next_gc_handle++;
    s_gc_handle_table[handle] = GcHandleEntry{ object_instance, pinned };

    if (pinned) {
        // Tell GC this address is an explicit root so the object is never collected.
        GC_add_roots(object_instance,
            static_cast<char*>(object_instance) + sizeof(void*));
    }

    return static_cast<GCHandle>(handle);
}

void CHAOS_RUNTIME_ABI_CALL GcHandleFree(
    RuntimeState* runtime_state,
    GCHandle gc_handle) {
    if (runtime_state == nullptr || gc_handle == CHAOS_GC_HANDLE_INVALID) {
        return;
    }

    std::lock_guard<std::mutex> lock(s_gc_handle_mutex);
    auto it = s_gc_handle_table.find(static_cast<uint64_t>(gc_handle));
    if (it != s_gc_handle_table.end()) {
        // Note: for pinned handles we intentionally do not call GC_remove_roots here.
        // GC_add_roots registers a memory range (not the object itself) as a scan root.
        // Once the handle is removed from the table, no live code will use this object
        // through the handle, so the range is a harmless false positive until the next GC.
        // GC_remove_roots requires DYNAMIC_LOADING and is not available on all platforms.
        s_gc_handle_table.erase(it);
    }
}

void CHAOS_RUNTIME_ABI_CALL RaiseManagedException(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    ExceptionHandle exception) {
    if (!IsAttached(runtime_state, thread_state)) {
        return;
    }

    throw ManagedExceptionCarrier
    {
        exception,
    };
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
    using RawMethodInvokerFn = void* (CHAOS_RUNTIME_ABI_CALL*)(
        RuntimeState* runtime,
        ThreadState* thread,
        void* __this,
        void* const* argv,
        uint32_t argc);

    if (!IsAttached(runtime_state, thread_state) || method == nullptr) {
        return CHAOS_RUNTIME_STATUS_INVALID_ARGUMENT;
    }

    if (out_exception != nullptr) {
        *out_exception = nullptr;
    }

    if (IsLikelyMetadataTokenHandle(method)) {
        return CHAOS_RUNTIME_STATUS_NOT_SUPPORTED;
    }

    try {
        auto* const invoker = reinterpret_cast<RawMethodInvokerFn>(method);
        if (invoker == nullptr) {
            return CHAOS_RUNTIME_STATUS_NOT_FOUND;
        }

        void* return_value = invoker(runtime_state, thread_state, object_instance, argv, argc);
        if (out_return_value != nullptr) {
            if (out_return_value_size != sizeof(void*)) {
                return CHAOS_RUNTIME_STATUS_INVALID_ARGUMENT;
            }

            std::memcpy(out_return_value, &return_value, sizeof(return_value));
        }

        return CHAOS_RUNTIME_STATUS_OK;
    } catch (const ManagedExceptionCarrier& carrier) {
        if (out_exception != nullptr) {
            *out_exception = carrier.exception;
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
    if (method == nullptr) {
        return nullptr;
    }
    const uint32_t method_token = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(method));
    return chaos::il2cpp::generic_context::GetGenericContextForMethod(method_token);
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

RuntimeMode GetRuntimeMode() {
    return g_runtime_mode.load(std::memory_order_acquire);
}

void SetRuntimeMode(RuntimeMode mode) {
    g_runtime_mode.store(mode, std::memory_order_release);
}

bool IsMixedMode() {
    return GetRuntimeMode() == RuntimeMode::Mixed;
}

void* BoxValueObject(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    TypeInfoHandle value_type,
    const void* value,
    size_t value_size) {
    if (!IsAttached(runtime_state, thread_state)
        || value_type == nullptr
        || value == nullptr
        || value_size == 0u) {
        return nullptr;
    }

    const size_t allocation_size = sizeof(BoxedValueHeader) + value_size;
    // Value data contains no pointers; use atomic allocation so GC skips scanning it.
    unsigned char* storage = static_cast<unsigned char*>(AllocateBytesAtomic(allocation_size));
    if (storage == nullptr) {
        return nullptr;
    }

    auto* header = reinterpret_cast<BoxedValueHeader*>(storage);
    header->type = value_type;
    header->byte_count = value_size;
    std::memcpy(storage + sizeof(BoxedValueHeader), value, value_size);
    return header;
}

RuntimeStatus UnboxValueObject(
    RuntimeState* runtime_state,
    void* boxed_object,
    void* out_value,
    size_t out_value_size) {
    if (runtime_state == nullptr || boxed_object == nullptr || out_value == nullptr || out_value_size == 0u) {
        return CHAOS_RUNTIME_STATUS_INVALID_ARGUMENT;
    }

    const auto* header = reinterpret_cast<const BoxedValueHeader*>(boxed_object);
    if (header->byte_count < out_value_size) {
        return CHAOS_RUNTIME_STATUS_INVALID_ARGUMENT;
    }

    std::memcpy(out_value, reinterpret_cast<const unsigned char*>(boxed_object) + sizeof(BoxedValueHeader), out_value_size);
    return CHAOS_RUNTIME_STATUS_OK;
}

bool ArrayStoreReference(
    void* array_instance,
    uintptr_t index,
    void* value) {
    if (array_instance == nullptr) {
        return false;
    }

    auto* header = reinterpret_cast<ArrayHeader*>(array_instance);
    if (index >= header->length) {
        return false;
    }

    auto* elements = reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(array_instance) + sizeof(ArrayHeader));
    elements[index] = value;
    return true;
}

void* ArrayLoadReference(
    void* array_instance,
    uintptr_t index) {
    if (array_instance == nullptr) {
        return nullptr;
    }

    auto* header = reinterpret_cast<ArrayHeader*>(array_instance);
    if (index >= header->length) {
        return nullptr;
    }

    auto* elements = reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(array_instance) + sizeof(ArrayHeader));
    return elements[index];
}

bool ArrayCopyReferenceRange(
    void* source_array_instance,
    uintptr_t source_index,
    void* target_array_instance,
    uintptr_t target_index,
    uintptr_t length) {
    if (source_array_instance == nullptr || target_array_instance == nullptr) {
        return false;
    }

    auto* source_header = reinterpret_cast<ArrayHeader*>(source_array_instance);
    auto* target_header = reinterpret_cast<ArrayHeader*>(target_array_instance);
    if (source_header->element_type == nullptr ||
        target_header->element_type == nullptr ||
        source_header->element_type != target_header->element_type) {
        return false;
    }

    if (source_index > source_header->length ||
        target_index > target_header->length ||
        length > (source_header->length - source_index) ||
        length > (target_header->length - target_index)) {
        return false;
    }

    if (length == 0u) {
        return true;
    }

    auto* source_elements = reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(source_array_instance) + sizeof(ArrayHeader));
    auto* target_elements = reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(target_array_instance) + sizeof(ArrayHeader));
    std::memmove(
        target_elements + target_index,
        source_elements + source_index,
        static_cast<size_t>(length) * sizeof(void*));
    return true;
}

bool ArrayClearReferenceRange(
    void* array_instance,
    uintptr_t start_index,
    uintptr_t length) {
    if (array_instance == nullptr) {
        return false;
    }

    auto* header = reinterpret_cast<ArrayHeader*>(array_instance);
    if (header->element_type == nullptr) {
        return false;
    }

    if (start_index > header->length || length > (header->length - start_index)) {
        return false;
    }

    auto* elements = reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(array_instance) + sizeof(ArrayHeader));
    for (uintptr_t index = 0; index < length; ++index) {
        elements[start_index + index] = nullptr;
    }

    return true;
}

bool ArrayReverseReferenceRange(
    void* array_instance,
    uintptr_t start_index,
    uintptr_t length) {
    if (array_instance == nullptr) {
        return false;
    }

    auto* header = reinterpret_cast<ArrayHeader*>(array_instance);
    if (header->element_type == nullptr) {
        return false;
    }

    if (start_index > header->length || length > (header->length - start_index)) {
        return false;
    }

    if (length <= 1u) {
        return true;
    }

    auto* elements = reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(array_instance) + sizeof(ArrayHeader));
    uintptr_t left = start_index;
    uintptr_t right = start_index + length - 1u;
    while (left < right) {
        void* temporary = elements[left];
        elements[left] = elements[right];
        elements[right] = temporary;
        ++left;
        --right;
    }

    return true;
}

int32_t EngineLogWrite(
    const char* category_utf8,
    const char* message_utf8) {
    (void)category_utf8;

    if (message_utf8 == nullptr) {
        return 1;
    }

    const size_t prefix_length = std::strlen(kEngineObservePrefix);
    if (std::fwrite(kEngineObservePrefix, 1u, prefix_length, stdout) != prefix_length) {
        return 1;
    }

    const size_t message_length = std::strlen(message_utf8);
    if (std::fwrite(message_utf8, 1u, message_length, stdout) != message_length) {
        return 1;
    }

    if (std::fputc('\n', stdout) == EOF) {
        return 1;
    }

    return std::fflush(stdout) == 0 ? 0 : 1;
}

uintptr_t CreateEngineObjectHandle(void* object_instance) {
    if (object_instance == nullptr) {
        return 0u;
    }

    std::lock_guard<std::mutex> lock(g_engine_binding_mutex);
    const uintptr_t handle = g_next_engine_handle++;
    g_engine_handles[handle] = object_instance;
    return handle;
}

void* ResolveEngineObjectHandle(uintptr_t handle) {
    if (handle == 0u) {
        return nullptr;
    }

    std::lock_guard<std::mutex> lock(g_engine_binding_mutex);
    const auto iterator = g_engine_handles.find(handle);
    return iterator != g_engine_handles.end() ? iterator->second : nullptr;
}

bool RegisterEngineLifecycleCallback(
    const char* phase_utf8,
    EngineLifecycleCallback callback,
    void* user_data) {
    if (phase_utf8 == nullptr || callback == nullptr) {
        return false;
    }

    std::lock_guard<std::mutex> lock(g_engine_binding_mutex);
    g_engine_lifecycle_registrations.push_back(
        EngineLifecycleRegistration
        {
            phase_utf8,
            callback,
            user_data,
        });
    return true;
}

bool DispatchEngineLifecycleCallbacks(const char* phase_utf8) {
    if (phase_utf8 == nullptr) {
        return false;
    }

    std::vector<EngineLifecycleRegistration> callbacks = {};
    {
        std::lock_guard<std::mutex> lock(g_engine_binding_mutex);
        for (const auto& registration : g_engine_lifecycle_registrations) {
            if (registration.phase == phase_utf8) {
                callbacks.push_back(registration);
            }
        }
    }

    for (const auto& registration : callbacks) {
        registration.callback(phase_utf8, registration.user_data);
    }

    return !callbacks.empty();
}

bool IsMainThreadLane() {
    return std::this_thread::get_id() == g_main_thread_id;
}

bool ThreadStaticInt32Add(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    const char* slot_key_utf8,
    int32_t delta,
    int32_t* out_value) {
    if (!IsAttached(runtime_state, thread_state) || slot_key_utf8 == nullptr || out_value == nullptr) {
        return false;
    }

    auto* thread_internal_state = GetThreadInternalState(thread_state);
    if (thread_internal_state == nullptr) {
        return false;
    }

    int32_t& value = thread_internal_state->thread_static_int32_slots[slot_key_utf8];
    value += delta;
    *out_value = value;
    return true;
}

bool MonitorEnter(void* monitor_target) {
    auto monitor = GetOrCreateMonitor(monitor_target);
    if (!monitor) {
        return false;
    }

    monitor->lock();
    return true;
}

bool MonitorExit(void* monitor_target) {
    if (monitor_target == nullptr) {
        return false;
    }

    std::shared_ptr<std::recursive_mutex> monitor = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_monitor_registry_mutex);
        const auto iterator = g_monitor_registry.find(monitor_target);
        if (iterator == g_monitor_registry.end()) {
            return false;
        }

        monitor = iterator->second;
    }

    if (!monitor) {
        return false;
    }

    monitor->unlock();
    return true;
}

bool GcSafepoint(
    RuntimeState* runtime_state,
    ThreadState* thread_state) {
    if (!IsAttached(runtime_state, thread_state)) {
        return false;
    }

    auto* thread_internal_state = GetThreadInternalState(thread_state);
    if (thread_internal_state == nullptr) {
        return false;
    }

    thread_internal_state->at_gc_safepoint = true;
    // Perform a small slice of incremental GC work.
    // This distributes GC pauses across frames rather than causing a single large pause.
    GC_collect_a_little();
    thread_internal_state->at_gc_safepoint = false;
    return true;
}

size_t ReportThreadRoot(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    const void* root_address,
    size_t root_size) {
    if (!IsAttached(runtime_state, thread_state) || root_address == nullptr || root_size == 0u) {
        return 0u;
    }

    auto* thread_internal_state = GetThreadInternalState(thread_state);
    if (thread_internal_state == nullptr) {
        return 0u;
    }

    thread_internal_state->reported_roots.push_back(
        ThreadRootRecord
        {
            root_address,
            root_size,
        });
    return thread_internal_state->reported_roots.size();
}

bool EnqueueFinalizer(
    RuntimeState* runtime_state,
    void* object_instance,
    FinalizerCallback finalizer) {
    auto* runtime_internal_state = GetRuntimeInternalState(runtime_state);
    if (runtime_internal_state == nullptr || object_instance == nullptr || finalizer == nullptr) {
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(runtime_internal_state->finalizer_queue_mutex);
        runtime_internal_state->finalizer_queue.push_back(
            FinalizerWorkItem
            {
                object_instance,
                finalizer,
            });
    }

    // Also register with BDWGC so it can invoke the finalizer automatically
    // when the object becomes unreachable during a GC cycle.
    GC_register_finalizer_no_order(
        object_instance,
        [](void* obj, void* client_data) {
            auto* cb = reinterpret_cast<FinalizerCallback>(client_data);
            if (cb) { cb(obj); }
        },
        reinterpret_cast<void*>(finalizer),
        nullptr, nullptr);

    return true;
}

size_t DrainFinalizerQueue(RuntimeState* runtime_state) {
    auto* runtime_internal_state = GetRuntimeInternalState(runtime_state);
    if (runtime_internal_state == nullptr) {
        return 0u;
    }

    // Flush any GC-triggered finalizers first.
    GC_invoke_finalizers();

    std::vector<FinalizerWorkItem> pending_finalizers = {};
    {
        std::lock_guard<std::mutex> lock(runtime_internal_state->finalizer_queue_mutex);
        pending_finalizers.swap(runtime_internal_state->finalizer_queue);
    }

    for (const auto& work_item : pending_finalizers) {
        work_item.finalizer(work_item.object_instance);
    }

    return pending_finalizers.size();
}

}  // namespace chaos::il2cpp::runtime_core

extern "C" CHAOS_RUNTIME_ABI_EXPORT const RuntimeAbiV0* CHAOS_RUNTIME_ABI_CALL chaos_runtime_get_abi_v0(void) {
    return chaos::il2cpp::runtime_core::GetRuntimeAbiV0();
}

