// Generic arguments, CreateInstance, InvokeMethod, MakeGeneric*, MakeGenericType

extern "C" {
namespace chaos::il2cpp::runtime_core {

namespace {

// Extract TypeInfoHandle[] from a managed System.Type[] pointer.
// Returns the number of handles extracted (clamped to max_count).
// Each managed Type object has runtime_type_handle at offset 16 (after object header).
static uint32_t ExtractTypeArgsFromManagedArray(
    CHAOS_IL2CPP_INTPTR type_args,
    TypeInfoHandle* out_handles,
    uint32_t max_count) noexcept
{
    if (type_args == 0) return 0u;
    const auto* raw = reinterpret_cast<const uint8_t*>(
        static_cast<CHAOS_IL2CPP_INTPTR>(type_args));
    // Managed array layout: [header(16)][length(4)][pad(4)][elements...]
    const uint32_t count = static_cast<uint32_t>(
        *reinterpret_cast<const CHAOS_IL2CPP_INT32*>(raw + 16u));
    const uint32_t actual = (count < max_count) ? count : max_count;
    for (uint32_t i = 0u; i < actual; i++) {
        const void* type_obj = *reinterpret_cast<void* const*>(
            raw + 24u + i * sizeof(void*));
        if (type_obj == nullptr) {
            out_handles[i] = 0u;
            continue;
        }
        // Managed Type layout: [header(16)][runtime_type_handle(8)]...
        out_handles[i] = static_cast<TypeInfoHandle>(
            *reinterpret_cast<const CHAOS_IL2CPP_INTPTR*>(
                static_cast<const uint8_t*>(type_obj) + 16u));
    }
    return actual;
}

}  // anonymous namespace

CHAOS_IL2CPP_INTPTR ChaosReflectionGetGenericArguments(CHAOS_IL2CPP_INTPTR type_handle) {
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr) return 0;

    // Runtime-instantiated type: token >= 0x80000000, descriptor embedded in
    // RuntimeInstantiatedType — recover type_args from container struct.
    if (desc->metadata_token >= 0x80000000u) {
        using rt_type = chaos::il2cpp::runtime_instantiation::RuntimeInstantiatedType;
        auto* rti = reinterpret_cast<const rt_type*>(desc);
        if (rti == nullptr || rti->type_args == nullptr || rti->arg_count == 0) return 0;

        static CHAOS_IL2CPP_INTPTR s_buffer[33];
        uint32_t count = rti->arg_count > 32 ? 32 : rti->arg_count;
        for (uint32_t i = 0; i < count; i++) {
            s_buffer[1 + i] = static_cast<CHAOS_IL2CPP_INTPTR>(rti->type_args[i]);
        }
        s_buffer[0] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<intptr_t>(count));
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_buffer);
    }

    // AOT concrete generic type: look up in generic context registry.
    // The type_handle (a TypeInfoHandle) was stored at registration time and
    // is directly usable as a lookup key into the by_open_type closed_types.
    {
        TypeInfoHandle closed_handle = static_cast<TypeInfoHandle>(static_cast<uint64_t>(type_handle));
        if (closed_handle != 0) {
            static CHAOS_IL2CPP_INTPTR s_args_buffer[33];
            uint32_t count = generic_context::GetClosedTypeGenericArgs(
                closed_handle,
                reinterpret_cast<TypeInfoHandle*>(s_args_buffer + 1),
                32);
            if (count > 0) {
                s_args_buffer[0] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<intptr_t>(count));
                return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_args_buffer);
            }
        }
    }

    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetGenericTypeDefinition(CHAOS_IL2CPP_INTPTR type_handle) {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr || desc->generic_type_definition == nullptr) return 0;

    return static_cast<CHAOS_IL2CPP_INTPTR>(
        EncodeReflectionQueryTypeHandle(desc->generic_type_definition));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionCreateInstance(
    CHAOS_IL2CPP_INTPTR type_handle,
    CHAOS_IL2CPP_INTPTR args)
{
    auto* runtime = GetCurrentRuntimeState();
    auto* thread  = GetCurrentThreadState();
    const auto* abi = GetRuntimeAbiV0();
    if (runtime == nullptr || thread == nullptr || abi == nullptr) return 0;
    if (abi->object_new == nullptr || abi->class_init == nullptr) return 0;

    TypeInfoHandle type = static_cast<TypeInfoHandle>(static_cast<uint64_t>(type_handle));
    if (type == 0) return 0;

    // Ensure static constructor has run.
    abi->class_init(runtime, type);

    // Allocate the object.
    void* obj = abi->object_new(runtime, thread, type);
    if (obj == nullptr) return 0;

    // If constructor arguments are provided, find and invoke the matching .ctor.
    if (args != 0) {
        // managed System.Object[]: [vtable(8)][type_info(8)][length(4)][pad(4)][elements...]
        const auto argc = static_cast<uint32_t>(
            *reinterpret_cast<const CHAOS_IL2CPP_INT32*>(
                static_cast<CHAOS_IL2CPP_INTPTR>(args) + 16));
        void* const* argv = reinterpret_cast<void* const*>(
            static_cast<CHAOS_IL2CPP_INTPTR>(args) + 24);

        MethodInfoHandle ctor = 0;
        if (abi->type_find_method != nullptr) {
            ctor = abi->type_find_method(type, ".ctor", static_cast<int32_t>(argc));
        }
        if (ctor != 0 && abi->method_invoke != nullptr) {
            ExceptionHandle ex = nullptr;
            RuntimeStatus ctor_status = abi->method_invoke(runtime, thread, ctor, obj,
                               argv, argc, nullptr, 0, &ex);
            if (ctor_status == CHAOS_RUNTIME_STATUS_MANAGED_EXCEPTION) {
                throw chaos_managed_exception{reinterpret_cast<CHAOS_IL2CPP_INTPTR>(ex)};
            }
        }
    }

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(obj);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionInvokeMethod(
    CHAOS_IL2CPP_INTPTR method_handle,
    CHAOS_IL2CPP_INTPTR obj,
    CHAOS_IL2CPP_INTPTR args)
{
    auto* runtime = GetCurrentRuntimeState();
    auto* thread  = GetCurrentThreadState();
    const auto* abi = GetRuntimeAbiV0();
    if (runtime == nullptr || thread == nullptr || abi == nullptr) return 0;
    if (abi->method_invoke == nullptr) return 0;

    // method_handle is a pointer to managed MethodInfo object.
    // Extract runtime_method_handle at offset 24 (after 16-byte object header).
    // Use raw pointer arithmetic to avoid MSVC auto+const+complex-type issues.
    if (method_handle == 0) return 0;
    const void* methodInfoObj = reinterpret_cast<const void*>(
        static_cast<CHAOS_IL2CPP_INTPTR>(method_handle));
    MethodInfoHandle method = static_cast<MethodInfoHandle>(
        *reinterpret_cast<const CHAOS_IL2CPP_INTPTR*>(
            static_cast<const char*>(methodInfoObj) + 24));
    if (method == 0) return 0;

    // Decode managed System.Object[] args array.
    uint32_t argc = 0;
    void* const* argv = nullptr;
    if (args != 0) {
        argc = static_cast<uint32_t>(
            *reinterpret_cast<const CHAOS_IL2CPP_INT32*>(
                static_cast<CHAOS_IL2CPP_INTPTR>(args) + 16));
        argv = reinterpret_cast<void* const*>(
            static_cast<CHAOS_IL2CPP_INTPTR>(args) + 24);
    }

    // Return value buffer (64 bytes covers SIMD types up to Vector512).
    uint64_t ret_buf[8] = {};
    ExceptionHandle ex = nullptr;

    RuntimeStatus status = abi->method_invoke(
        runtime, thread, method,
        reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_INTPTR>(obj)),
        argv, argc, ret_buf, sizeof(ret_buf), &ex);

    if (status == CHAOS_RUNTIME_STATUS_MANAGED_EXCEPTION) {
        throw chaos_managed_exception{reinterpret_cast<CHAOS_IL2CPP_INTPTR>(ex)};
    }

    // For reference-type returns ret_buf[0] is the managed object pointer.
    // For value-type returns <= 8 bytes, ret_buf[0] holds the raw value
    // (managed wrapper must box if needed).
    return static_cast<CHAOS_IL2CPP_INTPTR>(ret_buf[0]);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionMakeGenericMethod(
    CHAOS_IL2CPP_INTPTR method_handle,
    CHAOS_IL2CPP_INTPTR type_args)
{
    if (method_handle == 0) return 0;

    auto* bridge = chaos::il2cpp::runtime_instantiation::GetBridgeV0();
    if (bridge == nullptr || bridge->resolve_or_instantiate_method == nullptr)
        return 0;

    MethodInfoHandle open_method = static_cast<MethodInfoHandle>(static_cast<uint64_t>(method_handle));

    if (type_args == 0) {
        return static_cast<CHAOS_IL2CPP_INTPTR>(
            bridge->resolve_or_instantiate_method(open_method, nullptr, 0u));
    }

    TypeInfoHandle arg_handles[32];
    const uint32_t arg_count = ExtractTypeArgsFromManagedArray(type_args, arg_handles, 32u);
    if (arg_count == 0u) {
        return static_cast<CHAOS_IL2CPP_INTPTR>(open_method);
    }

    MethodInfoHandle result = bridge->resolve_or_instantiate_method(
        open_method, arg_handles, arg_count);
    return static_cast<CHAOS_IL2CPP_INTPTR>(result);
}

// ── MakeGenericType ─────────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionMakeGenericType(CHAOS_IL2CPP_INTPTR def, CHAOS_IL2CPP_INTPTR args) noexcept
{
    if (def == 0) return 0;

    auto* bridge = chaos::il2cpp::runtime_instantiation::GetBridgeV0();
    if (bridge == nullptr || bridge->resolve_or_instantiate_type == nullptr)
        return 0;

    TypeInfoHandle open_def = static_cast<TypeInfoHandle>(static_cast<uint64_t>(def));

    if (args == 0) {
        return static_cast<CHAOS_IL2CPP_INTPTR>(
            bridge->resolve_or_instantiate_type(open_def, nullptr, 0u));
    }

    TypeInfoHandle arg_handles[32];
    const uint32_t arg_count = ExtractTypeArgsFromManagedArray(args, arg_handles, 32u);
    if (arg_count == 0u) {
        return static_cast<CHAOS_IL2CPP_INTPTR>(open_def);
    }

    TypeInfoHandle result = bridge->resolve_or_instantiate_type(
        open_def, arg_handles, arg_count);
    return static_cast<CHAOS_IL2CPP_INTPTR>(result);
}

}  // namespace chaos::il2cpp::runtime_core
}  // extern "C"