#include "runtime_core.h"

#include "reflection_query_model.h"
#include "generic_context.h"
#include "vtable_registry.h"

#include <gc.h>

#include <cstdio>
#include <cmath>
#include <atomic>
#include <limits>
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
constexpr std::uint64_t kDateTimeTicksMask = 0x3FFFFFFFFFFFFFFFull;
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

struct UInt128Layout {
    std::uint64_t lower;
    std::uint64_t upper;
};

struct Int128Layout {
    std::uint64_t lower;
    std::int64_t upper;
};

constexpr ValueTypeKernelBackendKind DetectCharClassificationKernelBackend() {
    return ValueTypeKernelBackendKind::Intrinsic;
}

constexpr ValueTypeKernelBackendKind DetectFloatingClassificationKernelBackend() {
    return ValueTypeKernelBackendKind::Intrinsic;
}

constexpr ValueTypeKernelBackendKind DetectBitArithmeticKernelBackend() {
    return ValueTypeKernelBackendKind::Intrinsic;
}

constexpr ValueTypeKernelBackendKind DetectTickArithmeticKernelBackend() {
    return ValueTypeKernelBackendKind::Generic;
}

constexpr ValueTypeKernelBackendKind kCharClassificationKernelBackend = DetectCharClassificationKernelBackend();
constexpr ValueTypeKernelBackendKind kFloatingClassificationKernelBackend = DetectFloatingClassificationKernelBackend();
constexpr ValueTypeKernelBackendKind kBitArithmeticKernelBackend = DetectBitArithmeticKernelBackend();
constexpr ValueTypeKernelBackendKind kTickArithmeticKernelBackend = DetectTickArithmeticKernelBackend();

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
            if (out_return_value_size == sizeof(void*)) {
                std::memcpy(out_return_value, &return_value, sizeof(return_value));
            } else {
                auto* indirect_return_value = reinterpret_cast<void* const*>(out_return_value);
                if (indirect_return_value == nullptr || *indirect_return_value == nullptr) {
                    return CHAOS_RUNTIME_STATUS_INVALID_ARGUMENT;
                }

                if (return_value == nullptr) {
                    return CHAOS_RUNTIME_STATUS_INTERNAL_ERROR;
                }

                std::memcpy(*indirect_return_value, return_value, out_return_value_size);
            }
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

ValueTypeKernelBackendKind GetCharClassificationKernelBackend() {
    return kCharClassificationKernelBackend;
}

ValueTypeKernelBackendKind GetFloatingClassificationKernelBackend() {
    return kFloatingClassificationKernelBackend;
}

ValueTypeKernelBackendKind GetBitArithmeticKernelBackend() {
    return kBitArithmeticKernelBackend;
}

ValueTypeKernelBackendKind GetTickArithmeticKernelBackend() {
    return kTickArithmeticKernelBackend;
}

static bool CharIsAsciiGeneric(std::uint16_t value) {
    return value <= 0x7Fu;
}

static bool CharIsAsciiIntrinsicImpl(std::uint16_t value) {
    return (value & 0xFF80u) == 0u;
}

bool CharIsAscii(std::uint16_t value) {
    return kCharClassificationKernelBackend == ValueTypeKernelBackendKind::Intrinsic
        ? CharIsAsciiIntrinsicImpl(value)
        : CharIsAsciiGeneric(value);
}

bool CharIsAsciiDigit(std::uint16_t value) {
    return static_cast<std::uint16_t>(value - static_cast<std::uint16_t>('0')) <= 9u;
}

bool CharIsAsciiHexDigitLower(std::uint16_t value) {
    return static_cast<std::uint16_t>(value - static_cast<std::uint16_t>('a')) <= 5u;
}

bool CharIsAsciiHexDigitUpper(std::uint16_t value) {
    return static_cast<std::uint16_t>(value - static_cast<std::uint16_t>('A')) <= 5u;
}

bool CharIsAsciiHexDigit(std::uint16_t value) {
    return CharIsAsciiDigit(value) || CharIsAsciiHexDigitLower(value) || CharIsAsciiHexDigitUpper(value);
}

bool CharIsAsciiLetterLower(std::uint16_t value) {
    return static_cast<std::uint16_t>(value - static_cast<std::uint16_t>('a')) <= 25u;
}

bool CharIsAsciiLetterUpper(std::uint16_t value) {
    return static_cast<std::uint16_t>(value - static_cast<std::uint16_t>('A')) <= 25u;
}

bool CharIsAsciiLetter(std::uint16_t value) {
    return CharIsAsciiLetterLower(value) || CharIsAsciiLetterUpper(value);
}

bool CharIsAsciiLetterOrDigit(std::uint16_t value) {
    return CharIsAsciiLetter(value) || CharIsAsciiDigit(value);
}

bool CharIsBetween(std::uint16_t value, std::uint16_t lower_bound, std::uint16_t upper_bound) {
    return value >= lower_bound && value <= upper_bound;
}

static bool CharIsLatin1Generic(std::uint16_t value) {
    return value <= 0xFFu;
}

static bool CharIsLatin1IntrinsicImpl(std::uint16_t value) {
    return (value & 0xFF00u) == 0u;
}

bool CharIsLatin1(std::uint16_t value) {
    return kCharClassificationKernelBackend == ValueTypeKernelBackendKind::Intrinsic
        ? CharIsLatin1IntrinsicImpl(value)
        : CharIsLatin1Generic(value);
}

bool CharIsHighSurrogate(std::uint16_t value) {
    return CharIsBetween(value, 0xD800u, 0xDBFFu);
}

bool CharIsLowSurrogate(std::uint16_t value) {
    return CharIsBetween(value, 0xDC00u, 0xDFFFu);
}

bool CharIsSeparatorLatin1(std::uint16_t value) {
    return value == 0x20u || value == 0xA0u;
}

bool CharIsSurrogate(std::uint16_t value) {
    return CharIsBetween(value, 0xD800u, 0xDFFFu);
}

bool CharIsSurrogatePair(std::uint16_t high_surrogate, std::uint16_t low_surrogate) {
    return CharIsHighSurrogate(high_surrogate) && CharIsLowSurrogate(low_surrogate);
}

bool CharIsWhiteSpaceLatin1(std::uint16_t value) {
    return value == 0x20u ||
           static_cast<std::uint16_t>(value - 0x09u) <= static_cast<std::uint16_t>(0x0Du - 0x09u) ||
           value == 0x85u ||
           value == 0xA0u;
}

int32_t CharCompare(std::uint16_t left_value, std::uint16_t right_value) {
    if (left_value < right_value) {
        return -1;
    }

    if (left_value > right_value) {
        return 1;
    }

    return 0;
}

bool CharEquals(std::uint16_t left_value, std::uint16_t right_value) {
    return left_value == right_value;
}

static std::uint32_t BitCastSingleToUInt32(float value) {
    std::uint32_t bits = 0u;
    std::memcpy(&bits, &value, sizeof(bits));
    return bits;
}

static std::uint64_t BitCastDoubleToUInt64(double value) {
    std::uint64_t bits = 0u;
    std::memcpy(&bits, &value, sizeof(bits));
    return bits;
}

static bool HalfIsFiniteGeneric(std::uint16_t value) {
    return (value & 0x7C00u) != 0x7C00u;
}

static bool HalfIsNaNGeneric(std::uint16_t value) {
    return (value & 0x7C00u) == 0x7C00u && (value & 0x03FFu) != 0u;
}

static bool HalfIsInfinityGeneric(std::uint16_t value) {
    return (value & 0x7FFFu) == 0x7C00u;
}

static float HalfToFloatValue(std::uint16_t value);
bool HalfIsInteger(std::uint16_t value);

bool HalfIsFinite(std::uint16_t value) {
    return HalfIsFiniteGeneric(value);
}

bool HalfIsEvenInteger(std::uint16_t value) {
    return HalfIsInteger(value) &&
           std::fabs(std::fmod(HalfToFloatValue(value), 2.0f)) == 0.0f;
}

bool HalfIsNaN(std::uint16_t value) {
    return HalfIsNaNGeneric(value);
}

bool HalfIsNaNOrZero(std::uint16_t value) {
    return (value & 0x7FFFu) == 0u || HalfIsNaN(value);
}

bool HalfIsNegative(std::uint16_t value) {
    return (value & 0x8000u) == 0x8000u;
}

bool HalfIsNegativeInfinity(std::uint16_t value) {
    return value == 0xFC00u;
}

bool HalfIsNormal(std::uint16_t value) {
    const std::uint16_t abs_value = static_cast<std::uint16_t>(value & 0x7FFFu);
    return abs_value < 0x7C00u && abs_value != 0u && (abs_value & 0x7C00u) != 0u;
}

bool HalfIsOddInteger(std::uint16_t value) {
    return HalfIsInteger(value) &&
           std::fabs(std::fmod(HalfToFloatValue(value), 2.0f)) == 1.0f;
}

bool HalfIsPositive(std::uint16_t value) {
    return !HalfIsNegative(value);
}

bool HalfIsPositiveInfinity(std::uint16_t value) {
    return value == 0x7C00u;
}

bool HalfIsPow2(std::uint16_t value) {
    if (!HalfIsFinite(value)) {
        return false;
    }

    const float float_value = HalfToFloatValue(value);
    if (!(float_value > 0.0f)) {
        return false;
    }

    int exponent = 0;
    return std::frexp(float_value, &exponent) == 0.5f;
}

bool HalfIsRealNumber(std::uint16_t value) {
    return !HalfIsNaN(value);
}

bool HalfIsSubnormal(std::uint16_t value) {
    const std::uint16_t abs_value = static_cast<std::uint16_t>(value & 0x7FFFu);
    return abs_value != 0u && (abs_value & 0x7C00u) == 0u;
}

bool HalfIsZero(std::uint16_t value) {
    return (value & 0x7FFFu) == 0u;
}

bool HalfIsInteger(std::uint16_t value) {
    if (!HalfIsFinite(value)) {
        return false;
    }

    const float float_value = HalfToFloatValue(value);
    return std::trunc(float_value) == float_value;
}

bool HalfIsInfinity(std::uint16_t value) {
    return HalfIsInfinityGeneric(value);
}

static float HalfToFloatValue(std::uint16_t value) {
    const std::uint16_t exponent = static_cast<std::uint16_t>((value >> 10) & 0x1Fu);
    const std::uint16_t mantissa = static_cast<std::uint16_t>(value & 0x03FFu);
    const bool negative = (value & 0x8000u) != 0u;

    float result = 0.0f;
    if (exponent == 0u) {
        if (mantissa == 0u) {
            result = 0.0f;
        } else {
            result = std::ldexp(static_cast<float>(mantissa), -24);
        }
    } else if (exponent == 0x1Fu) {
        result = mantissa == 0u
            ? std::numeric_limits<float>::infinity()
            : std::numeric_limits<float>::quiet_NaN();
    } else {
        result = std::ldexp(1.0f + (static_cast<float>(mantissa) / 1024.0f), static_cast<int>(exponent) - 15);
    }

    return negative ? -result : result;
}

int32_t HalfCompare(std::uint16_t left_value, std::uint16_t right_value) {
    const bool left_is_nan = HalfIsNaN(left_value);
    const bool right_is_nan = HalfIsNaN(right_value);
    if (left_is_nan && right_is_nan) {
        return 0;
    }

    if (left_is_nan) {
        return -1;
    }

    if (right_is_nan) {
        return 1;
    }

    const float left = HalfToFloatValue(left_value);
    const float right = HalfToFloatValue(right_value);
    if (left < right) {
        return -1;
    }

    if (left > right) {
        return 1;
    }

    return 0;
}

bool HalfEquals(std::uint16_t left_value, std::uint16_t right_value) {
    return left_value == right_value ||
           (HalfIsZero(left_value) && HalfIsZero(right_value)) ||
           (HalfIsNaN(left_value) && HalfIsNaN(right_value));
}

static bool HalfAreZero(std::uint16_t left_value, std::uint16_t right_value) {
    return ((left_value | right_value) & 0x7FFFu) == 0u;
}

bool HalfOperatorEquals(std::uint16_t left_value, std::uint16_t right_value) {
    return !HalfIsNaN(left_value) &&
           !HalfIsNaN(right_value) &&
           (left_value == right_value || HalfAreZero(left_value, right_value));
}

bool HalfOperatorLessThan(std::uint16_t left_value, std::uint16_t right_value) {
    if (HalfIsNaN(left_value) || HalfIsNaN(right_value)) {
        return false;
    }

    const bool left_is_negative = HalfIsNegative(left_value);
    if (left_is_negative != HalfIsNegative(right_value)) {
        return left_is_negative && !HalfAreZero(left_value, right_value);
    }

    return left_value != right_value && ((left_value < right_value) ^ left_is_negative);
}

bool HalfOperatorLessThanOrEqual(std::uint16_t left_value, std::uint16_t right_value) {
    if (HalfIsNaN(left_value) || HalfIsNaN(right_value)) {
        return false;
    }

    const bool left_is_negative = HalfIsNegative(left_value);
    if (left_is_negative != HalfIsNegative(right_value)) {
        return left_is_negative || HalfAreZero(left_value, right_value);
    }

    return left_value == right_value || ((left_value < right_value) ^ left_is_negative);
}

bool HalfOperatorGreaterThan(std::uint16_t left_value, std::uint16_t right_value) {
    return HalfOperatorLessThan(right_value, left_value);
}

bool HalfOperatorGreaterThanOrEqual(std::uint16_t left_value, std::uint16_t right_value) {
    return HalfOperatorLessThanOrEqual(right_value, left_value);
}

template <typename T>
static int32_t FloatingCompareGeneric(T left_value, T right_value) {
    const bool left_is_nan = std::isnan(left_value);
    const bool right_is_nan = std::isnan(right_value);
    if (left_is_nan && right_is_nan) {
        return 0;
    }

    if (left_is_nan) {
        return -1;
    }

    if (right_is_nan) {
        return 1;
    }

    if (left_value < right_value) {
        return -1;
    }

    if (left_value > right_value) {
        return 1;
    }

    return 0;
}

template <typename T>
static bool FloatingEqualsGeneric(T left_value, T right_value) {
    return left_value == right_value || (std::isnan(left_value) && std::isnan(right_value));
}

template <typename T>
static bool FloatingIsIntegerGeneric(T value) {
    return std::isfinite(value) && std::trunc(value) == value;
}

template <typename T>
static bool FloatingIsEvenIntegerGeneric(T value) {
    return FloatingIsIntegerGeneric(value) &&
           std::fabs(std::fmod(value, static_cast<T>(2))) == static_cast<T>(0);
}

template <typename T>
static bool FloatingIsOddIntegerGeneric(T value) {
    return FloatingIsIntegerGeneric(value) &&
           std::fabs(std::fmod(value, static_cast<T>(2))) == static_cast<T>(1);
}

template <typename T>
static bool FloatingIsPow2Generic(T value) {
    if (!std::isfinite(value) || !(value > static_cast<T>(0))) {
        return false;
    }

    int exponent = 0;
    return std::frexp(value, &exponent) == static_cast<T>(0.5);
}

static bool SingleIsFiniteGeneric(float value) {
    return std::isfinite(value);
}

static bool SingleIsFiniteIntrinsicImpl(float value) {
    return (BitCastSingleToUInt32(value) & 0x7F800000u) != 0x7F800000u;
}

bool SingleIsFinite(float value) {
    return kFloatingClassificationKernelBackend == ValueTypeKernelBackendKind::Intrinsic
        ? SingleIsFiniteIntrinsicImpl(value)
        : SingleIsFiniteGeneric(value);
}

static bool SingleIsNaNGeneric(float value) {
    return std::isnan(value);
}

static bool SingleIsNaNIntrinsicImpl(float value) {
    const std::uint32_t bits = BitCastSingleToUInt32(value);
    return (bits & 0x7F800000u) == 0x7F800000u && (bits & 0x007FFFFFu) != 0u;
}

bool SingleIsNaN(float value) {
    return kFloatingClassificationKernelBackend == ValueTypeKernelBackendKind::Intrinsic
        ? SingleIsNaNIntrinsicImpl(value)
        : SingleIsNaNGeneric(value);
}

static bool SingleIsInfinityGeneric(float value) {
    return std::isinf(value);
}

static bool SingleIsInfinityIntrinsicImpl(float value) {
    return (BitCastSingleToUInt32(value) & 0x7FFFFFFFu) == 0x7F800000u;
}

bool SingleIsInfinity(float value) {
    return kFloatingClassificationKernelBackend == ValueTypeKernelBackendKind::Intrinsic
        ? SingleIsInfinityIntrinsicImpl(value)
        : SingleIsInfinityGeneric(value);
}

int32_t SingleCompare(float left_value, float right_value) {
    return FloatingCompareGeneric(left_value, right_value);
}

bool SingleEquals(float left_value, float right_value) {
    return FloatingEqualsGeneric(left_value, right_value);
}

bool SingleIsEvenInteger(float value) {
    return FloatingIsEvenIntegerGeneric(value);
}

bool SingleIsNaNOrZero(float value) {
    return (BitCastSingleToUInt32(value) & 0x7FFFFFFFu) == 0u || SingleIsNaN(value);
}

bool SingleIsNegative(float value) {
    return (BitCastSingleToUInt32(value) & 0x80000000u) != 0u;
}

bool SingleIsNegativeInfinity(float value) {
    return BitCastSingleToUInt32(value) == 0xFF800000u;
}

bool SingleIsNormal(float value) {
    const std::uint32_t bits = BitCastSingleToUInt32(value);
    return (bits & 0x7FFFFFFFu) < 0x7F800000u &&
           (bits & 0x7FFFFFFFu) != 0u &&
           (bits & 0x7F800000u) != 0u;
}

bool SingleIsOddInteger(float value) {
    return FloatingIsOddIntegerGeneric(value);
}

bool SingleIsPositive(float value) {
    return (BitCastSingleToUInt32(value) & 0x80000000u) == 0u;
}

bool SingleIsPositiveInfinity(float value) {
    return BitCastSingleToUInt32(value) == 0x7F800000u;
}

bool SingleIsPow2(float value) {
    return FloatingIsPow2Generic(value);
}

bool SingleIsRealNumber(float value) {
    return !SingleIsNaN(value);
}

bool SingleIsSubnormal(float value) {
    const std::uint32_t bits = BitCastSingleToUInt32(value);
    return (bits & 0x7FFFFFFFu) != 0u && (bits & 0x7F800000u) == 0u;
}

bool SingleIsZero(float value) {
    return value == 0.0f;
}

bool SingleIsInteger(float value) {
    return FloatingIsIntegerGeneric(value);
}

static bool DoubleIsFiniteGeneric(double value) {
    return std::isfinite(value);
}

static bool DoubleIsFiniteIntrinsicImpl(double value) {
    return (BitCastDoubleToUInt64(value) & 0x7FF0000000000000ull) != 0x7FF0000000000000ull;
}

bool DoubleIsFinite(double value) {
    return kFloatingClassificationKernelBackend == ValueTypeKernelBackendKind::Intrinsic
        ? DoubleIsFiniteIntrinsicImpl(value)
        : DoubleIsFiniteGeneric(value);
}

static bool DoubleIsNaNGeneric(double value) {
    return std::isnan(value);
}

static bool DoubleIsNaNIntrinsicImpl(double value) {
    const std::uint64_t bits = BitCastDoubleToUInt64(value);
    return (bits & 0x7FF0000000000000ull) == 0x7FF0000000000000ull && (bits & 0x000FFFFFFFFFFFFFull) != 0u;
}

static bool DoubleIsInfinityGeneric(double value) {
    return std::isinf(value);
}

static bool DoubleIsInfinityIntrinsicImpl(double value) {
    return (BitCastDoubleToUInt64(value) & 0x7FFFFFFFFFFFFFFFull) == 0x7FF0000000000000ull;
}

bool DoubleIsNaN(double value) {
    return kFloatingClassificationKernelBackend == ValueTypeKernelBackendKind::Intrinsic
        ? DoubleIsNaNIntrinsicImpl(value)
        : DoubleIsNaNGeneric(value);
}

bool DoubleIsInfinity(double value) {
    return kFloatingClassificationKernelBackend == ValueTypeKernelBackendKind::Intrinsic
        ? DoubleIsInfinityIntrinsicImpl(value)
        : DoubleIsInfinityGeneric(value);
}

int32_t DoubleCompare(double left_value, double right_value) {
    return FloatingCompareGeneric(left_value, right_value);
}

bool DoubleEquals(double left_value, double right_value) {
    return FloatingEqualsGeneric(left_value, right_value);
}

bool DoubleIsEvenInteger(double value) {
    return FloatingIsEvenIntegerGeneric(value);
}

bool DoubleIsNaNOrZero(double value) {
    return (BitCastDoubleToUInt64(value) & 0x7FFFFFFFFFFFFFFFull) == 0u || DoubleIsNaN(value);
}

bool DoubleIsNegative(double value) {
    return (BitCastDoubleToUInt64(value) & 0x8000000000000000ull) != 0u;
}

bool DoubleIsNegativeInfinity(double value) {
    return BitCastDoubleToUInt64(value) == 0xFFF0000000000000ull;
}

bool DoubleIsNormal(double value) {
    const std::uint64_t bits = BitCastDoubleToUInt64(value);
    return (bits & 0x7FFFFFFFFFFFFFFFull) < 0x7FF0000000000000ull &&
           (bits & 0x7FFFFFFFFFFFFFFFull) != 0u &&
           (bits & 0x7FF0000000000000ull) != 0u;
}

bool DoubleIsOddInteger(double value) {
    return FloatingIsOddIntegerGeneric(value);
}

bool DoubleIsPositive(double value) {
    return (BitCastDoubleToUInt64(value) & 0x8000000000000000ull) == 0u;
}

bool DoubleIsPositiveInfinity(double value) {
    return BitCastDoubleToUInt64(value) == 0x7FF0000000000000ull;
}

bool DoubleIsPow2(double value) {
    return FloatingIsPow2Generic(value);
}

bool DoubleIsRealNumber(double value) {
    return !DoubleIsNaN(value);
}

bool DoubleIsSubnormal(double value) {
    const std::uint64_t bits = BitCastDoubleToUInt64(value);
    return (bits & 0x7FFFFFFFFFFFFFFFull) != 0u && (bits & 0x7FF0000000000000ull) == 0u;
}

bool DoubleIsZero(double value) {
    return value == 0.0;
}

bool DoubleIsInteger(double value) {
    return FloatingIsIntegerGeneric(value);
}

bool NFloatIsFinite(double value) {
    return DoubleIsFinite(value);
}

bool NFloatIsNaN(double value) {
    return DoubleIsNaN(value);
}

bool NFloatIsInfinity(double value) {
    return DoubleIsInfinity(value);
}

int32_t NFloatCompare(double left_value, double right_value) {
    return DoubleCompare(left_value, right_value);
}

bool NFloatEquals(double left_value, double right_value) {
    return DoubleEquals(left_value, right_value);
}

bool NFloatIsEvenInteger(double value) {
    return DoubleIsEvenInteger(value);
}

bool NFloatIsNegative(double value) {
    return DoubleIsNegative(value);
}

bool NFloatIsNegativeInfinity(double value) {
    return DoubleIsNegativeInfinity(value);
}

bool NFloatIsNormal(double value) {
    return DoubleIsNormal(value);
}

bool NFloatIsOddInteger(double value) {
    return DoubleIsOddInteger(value);
}

bool NFloatIsPositive(double value) {
    return DoubleIsPositive(value);
}

bool NFloatIsPositiveInfinity(double value) {
    return DoubleIsPositiveInfinity(value);
}

bool NFloatIsPow2(double value) {
    return DoubleIsPow2(value);
}

bool NFloatIsRealNumber(double value) {
    return DoubleIsRealNumber(value);
}

bool NFloatIsSubnormal(double value) {
    return DoubleIsSubnormal(value);
}

bool NFloatIsInteger(double value) {
    return DoubleIsInteger(value);
}

static int32_t Int128CompareGeneric(const void* left_value, const void* right_value) {
    if (left_value == nullptr || right_value == nullptr) {
        return 0;
    }

    Int128Layout left = {};
    Int128Layout right = {};
    std::memcpy(&left, left_value, sizeof(left));
    std::memcpy(&right, right_value, sizeof(right));
    if (left.upper < right.upper) {
        return -1;
    }

    if (left.upper > right.upper) {
        return 1;
    }

    if (left.lower < right.lower) {
        return -1;
    }

    if (left.lower > right.lower) {
        return 1;
    }

    return 0;
}

static int32_t Int128CompareIntrinsicImpl(const void* left_value, const void* right_value) {
    return Int128CompareGeneric(left_value, right_value);
}

int32_t Int128Compare(const void* left_value, const void* right_value) {
    return kBitArithmeticKernelBackend == ValueTypeKernelBackendKind::Intrinsic
        ? Int128CompareIntrinsicImpl(left_value, right_value)
        : Int128CompareGeneric(left_value, right_value);
}

bool Int128Equals(const void* left_value, const void* right_value) {
    return Int128Compare(left_value, right_value) == 0;
}

static int32_t UInt128CompareGeneric(const void* left_value, const void* right_value) {
    if (left_value == nullptr || right_value == nullptr) {
        return 0;
    }

    UInt128Layout left = {};
    UInt128Layout right = {};
    std::memcpy(&left, left_value, sizeof(left));
    std::memcpy(&right, right_value, sizeof(right));
    if (left.upper < right.upper) {
        return -1;
    }

    if (left.upper > right.upper) {
        return 1;
    }

    if (left.lower < right.lower) {
        return -1;
    }

    if (left.lower > right.lower) {
        return 1;
    }

    return 0;
}

static int32_t UInt128CompareIntrinsicImpl(const void* left_value, const void* right_value) {
    return UInt128CompareGeneric(left_value, right_value);
}

int32_t UInt128Compare(const void* left_value, const void* right_value) {
    return kBitArithmeticKernelBackend == ValueTypeKernelBackendKind::Intrinsic
        ? UInt128CompareIntrinsicImpl(left_value, right_value)
        : UInt128CompareGeneric(left_value, right_value);
}

bool UInt128Equals(const void* left_value, const void* right_value) {
    return UInt128Compare(left_value, right_value) == 0;
}

int32_t IntPtrCompare(std::intptr_t left_value, std::intptr_t right_value) {
    if (left_value < right_value) {
        return -1;
    }

    if (left_value > right_value) {
        return 1;
    }

    return 0;
}

bool IntPtrEquals(std::intptr_t left_value, std::intptr_t right_value) {
    return left_value == right_value;
}

int32_t UIntPtrCompare(std::uintptr_t left_value, std::uintptr_t right_value) {
    if (left_value < right_value) {
        return -1;
    }

    if (left_value > right_value) {
        return 1;
    }

    return 0;
}

bool UIntPtrEquals(std::uintptr_t left_value, std::uintptr_t right_value) {
    return left_value == right_value;
}

int32_t DateTimeCompareTicks(const void* left_value, const void* right_value) {
    if (left_value == nullptr || right_value == nullptr) {
        return 0;
    }

    std::uint64_t left = 0u;
    std::uint64_t right = 0u;
    std::memcpy(&left, left_value, sizeof(left));
    std::memcpy(&right, right_value, sizeof(right));
    left &= kDateTimeTicksMask;
    right &= kDateTimeTicksMask;
    if (left < right) {
        return -1;
    }

    if (left > right) {
        return 1;
    }

    return 0;
}

bool DateTimeEqualsTicks(const void* left_value, const void* right_value) {
    return DateTimeCompareTicks(left_value, right_value) == 0;
}

int32_t TimeSpanCompareTicks(const void* left_value, const void* right_value) {
    if (left_value == nullptr || right_value == nullptr) {
        return 0;
    }

    std::int64_t left = 0;
    std::int64_t right = 0;
    std::memcpy(&left, left_value, sizeof(left));
    std::memcpy(&right, right_value, sizeof(right));
    if (left < right) {
        return -1;
    }

    if (left > right) {
        return 1;
    }

    return 0;
}

bool TimeSpanEqualsTicks(const void* left_value, const void* right_value) {
    return TimeSpanCompareTicks(left_value, right_value) == 0;
}

int32_t DateOnlyCompareDayNumber(std::int32_t left_value, std::int32_t right_value) {
    if (left_value < right_value) {
        return -1;
    }

    if (left_value > right_value) {
        return 1;
    }

    return 0;
}

bool DateOnlyEqualsDayNumber(std::int32_t left_value, std::int32_t right_value) {
    return left_value == right_value;
}

int32_t TimeOnlyCompareTicksValue(std::int64_t left_value, std::int64_t right_value) {
    if (left_value < right_value) {
        return -1;
    }

    if (left_value > right_value) {
        return 1;
    }

    return 0;
}

bool TimeOnlyEqualsTicksValue(std::int64_t left_value, std::int64_t right_value) {
    return left_value == right_value;
}

}  // namespace chaos::il2cpp::runtime_core

extern "C" CHAOS_RUNTIME_ABI_EXPORT const RuntimeAbiV0* CHAOS_RUNTIME_ABI_CALL chaos_runtime_get_abi_v0(void) {
    return chaos::il2cpp::runtime_core::GetRuntimeAbiV0();
}

