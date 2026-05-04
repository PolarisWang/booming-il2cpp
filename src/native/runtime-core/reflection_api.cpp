// reflection_api.cpp — Native AOT reflection API implementation
//
// Provides extern "C" implementations for chaos_reflection_* functions
// called from generated C++ code. Uses Module Registry + Two-Tier Metadata
// for type/method/field queries:
//
//   - Given a (module_id, token) encoded TypeInfoHandle, the registry
//     locates the module descriptor, then the Tier 2 image descriptor
//     for full type/method/field metadata.
//   - Legacy ReflectionQuery encoded handles (tag bit 63) are decoded
//     directly to constexpr ReflectionQueryTypeDescriptor pointers.
//   - Raw metadata tokens fall back to the aot_metadata shared tables.
//
// These functions are called via SimpleForward dispatch from generated code,
// NOT through the bridge vtable.

#include "runtime_core.h"
#include "module_registry.h"
#include "reflection_query_model.h"
#include "reflection_metadata_impl.h"

#include <cstring>
#include <cstdio>

namespace chaos::il2cpp::runtime_core {

// ── Internal helpers ──

// Extract metadata token from a type handle.
// The handle can be:
//   - A ReflectionQuery encoded handle (tag bit 63) — decode and read token
//   - A Module Registry handle (module_id in upper 32 bits) — low 32 bits = token
//   - A raw metadata token — just return it
//   - Zero — return 0
static inline uint32_t DecodeMetadataToken(CHAOS_IL2CPP_INTPTR handle) {
    if (handle == 0) return 0;

    // Check if it's a ReflectionQuery encoded handle
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryTypeDescriptor>(static_cast<TypeInfoHandle>(handle));
    if (decoded != nullptr) {
        return decoded->metadata_token;
    }

    // For Module Registry handles and raw tokens: low 32 bits = token
    return static_cast<uint32_t>(handle & 0xFFFFFFFFu);
}

// Get a type descriptor from a handle using the full lookup chain:
//   1. ReflectionQuery encoded handle → direct pointer decode
//   2. Module Registry handle → extract module_id → LookupModule → image → FindReflectionQueryTypeByToken
//   3. Raw metadata token → aot_metadata shared tables
static inline const ReflectionQueryTypeDescriptor* GetTypeDescriptorFromHandle(CHAOS_IL2CPP_INTPTR handle) {
    if (handle == 0) return nullptr;

    // Try ReflectionQuery encoded handle first (tag bit 63 set)
    {
        auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryTypeDescriptor>(static_cast<TypeInfoHandle>(handle));
        if (decoded != nullptr) return decoded;
    }

    // Try Module Registry handle (module_id in upper 32 bits)
    {
        TypeInfoHandle type_handle = static_cast<TypeInfoHandle>(handle);
        uint32_t module_id = GetModuleId(type_handle);
        if (module_id != 0) {
            const auto* module = LookupModule(module_id);
            if (module != nullptr && module->image != nullptr) {
                uint32_t token = GetTypeToken(type_handle);
                return FindReflectionQueryTypeByToken(module->image, token);
            }
        }
    }

    // Fall back to raw metadata token lookup
    {
        uint32_t token = static_cast<uint32_t>(handle & 0xFFFFFFFFu);
        if (token != 0) {
            return aot_metadata::FindTypeByMetadataToken(token);
        }
    }

    return nullptr;
}

// Resolve image descriptor from a type handle via Module Registry.
// Returns nullptr if the handle is zero or the module has no Tier 2 image.
static inline const ReflectionQueryImageDescriptor* GetImageFromTypeHandle(CHAOS_IL2CPP_INTPTR handle) {
    if (handle == 0) return nullptr;

    TypeInfoHandle type_handle = static_cast<TypeInfoHandle>(handle);
    uint32_t module_id = GetModuleId(type_handle);
    if (module_id == 0) {
        return nullptr;
    }

    const auto* module = LookupModule(module_id);
    if (module == nullptr) {
        return nullptr;
    }

    return module->image;
}

// Decode a CHAOS_IL2CPP_INTPTR that may be a StringId or a native pointer
static inline const char* DecodeStringValue(CHAOS_IL2CPP_INTPTR value) {
    if (value == 0) return nullptr;
    return reinterpret_cast<const char*>(value);
}

}  // namespace chaos::il2cpp::runtime_core

// =====================================================================
// Type resolution
// =====================================================================

extern "C" CHAOS_IL2CPP_INTPTR ChaosReflectionGetTypeFromHandle(CHAOS_IL2CPP_INTPTR runtime_type_handle) {
    using namespace chaos::il2cpp::runtime_core;

    if (runtime_type_handle == 0) return 0;

    uint32_t token = DecodeMetadataToken(runtime_type_handle);
    auto* typeDesc = aot_metadata::FindTypeByMetadataToken(token);
    if (typeDesc == nullptr) return 0;

    return static_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryTypeHandle(typeDesc));
}

extern "C" CHAOS_IL2CPP_INTPTR ChaosReflectionGetTypeByName(
    CHAOS_IL2CPP_INTPTR name_string_id,
    CHAOS_IL2CPP_INT32 throw_on_error,
    CHAOS_IL2CPP_INT32 ignore_case)
{
    using namespace chaos::il2cpp::runtime_core;
    (void)throw_on_error;
    (void)ignore_case;

    if (name_string_id == 0) return 0;
    (void)name_string_id;
    return 0;
}

// =====================================================================
// Type properties — Module Registry / Image queries
// =====================================================================

extern "C" CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssembly(CHAOS_IL2CPP_INTPTR type_handle) {
    using namespace chaos::il2cpp::runtime_core;

    // Resolve the image from the type handle via Module Registry
    auto* image = GetImageFromTypeHandle(type_handle);
    if (image == nullptr) {
        // Fallback: CoreLib shared metadata
        image = &aot_metadata::kImageCoreLib;
    }

    return static_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryImageHandle(image));
}

extern "C" CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssemblyName(CHAOS_IL2CPP_INTPTR assembly_handle) {
    using namespace chaos::il2cpp::runtime_core;

    auto* decoded = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(assembly_handle));
    if (decoded == nullptr) {
        // Fallback: CoreLib shared metadata
        decoded = &aot_metadata::kImageCoreLib;
    }

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<char*>(decoded->image_name_utf8));
}

// Placeholder for AssemblyName value object
extern "C" CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssemblyNameValue(CHAOS_IL2CPP_INTPTR assembly_name_handle) {
    (void)assembly_name_handle;
    return 0;
}

extern "C" CHAOS_IL2CPP_INTPTR ChaosReflectionGetDeclaringType(CHAOS_IL2CPP_INTPTR type_handle) {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr) return 0;

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
        desc->generic_type_definition != nullptr
            ? EncodeReflectionQueryTypeHandle(desc->generic_type_definition)
            : static_cast<TypeInfoHandle>(0));
}

extern "C" CHAOS_IL2CPP_INTPTR ChaosReflectionGetMemberName(CHAOS_IL2CPP_INTPTR member_handle) {
    using namespace chaos::il2cpp::runtime_core;
    if (member_handle == 0) return 0;

    // Try decoding as each descriptor type
    auto* typeDesc = TryDecodeReflectionQueryHandle<ReflectionQueryTypeDescriptor>(static_cast<TypeInfoHandle>(member_handle));
    if (typeDesc != nullptr) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<char*>(typeDesc->name_utf8));
    }

    auto* methodDesc = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(static_cast<MethodInfoHandle>(member_handle));
    if (methodDesc != nullptr) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<char*>(methodDesc->name_utf8));
    }

    auto* fieldDesc = TryDecodeReflectionQueryHandle<ReflectionQueryFieldDescriptor>(static_cast<FieldInfoHandle>(member_handle));
    if (fieldDesc != nullptr) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<char*>(fieldDesc->name_utf8));
    }

    return 0;
}

extern "C" CHAOS_IL2CPP_INTPTR ChaosReflectionGetParameters(CHAOS_IL2CPP_INTPTR method_handle) {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(static_cast<MethodInfoHandle>(method_handle));
    if (desc == nullptr) return 0;

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<ReflectionQueryParameterDescriptor*>(desc->parameters));
}

extern "C" CHAOS_IL2CPP_INTPTR ChaosReflectionGetParameterName(
    CHAOS_IL2CPP_INTPTR parameter_handle)
{
    using namespace chaos::il2cpp::runtime_core;
    auto* paramDesc = reinterpret_cast<const ReflectionQueryParameterDescriptor*>(parameter_handle);
    if (paramDesc == nullptr) return 0;

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<char*>(paramDesc->name_utf8));
}

extern "C" CHAOS_IL2CPP_INTPTR ChaosReflectionGetMethodHandle(CHAOS_IL2CPP_INTPTR method_handle) {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(static_cast<MethodInfoHandle>(method_handle));
    if (desc == nullptr) return 0;

    return static_cast<CHAOS_IL2CPP_INTPTR>(desc->metadata_token);
}

extern "C" CHAOS_IL2CPP_INTPTR ChaosReflectionGetMetadataToken(CHAOS_IL2CPP_INTPTR member_handle) {
    using namespace chaos::il2cpp::runtime_core;
    if (member_handle == 0) return 0;

    auto* typeDesc = TryDecodeReflectionQueryHandle<ReflectionQueryTypeDescriptor>(static_cast<TypeInfoHandle>(member_handle));
    if (typeDesc != nullptr) return static_cast<CHAOS_IL2CPP_INTPTR>(typeDesc->metadata_token);

    auto* methodDesc = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(static_cast<MethodInfoHandle>(member_handle));
    if (methodDesc != nullptr) return static_cast<CHAOS_IL2CPP_INTPTR>(methodDesc->metadata_token);

    auto* fieldDesc = TryDecodeReflectionQueryHandle<ReflectionQueryFieldDescriptor>(static_cast<FieldInfoHandle>(member_handle));
    if (fieldDesc != nullptr) return static_cast<CHAOS_IL2CPP_INTPTR>(fieldDesc->metadata_token);

    // Raw metadata token passthrough
    return member_handle;
}

extern "C" CHAOS_IL2CPP_INTPTR ChaosReflectionGetTypeHandle(CHAOS_IL2CPP_INTPTR type_handle) {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr) return 0;

    return static_cast<CHAOS_IL2CPP_INTPTR>(desc->metadata_token);
}

extern "C" CHAOS_IL2CPP_INTPTR ChaosReflectionGetConstructorsDefault(CHAOS_IL2CPP_INTPTR type_handle) noexcept {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr || desc->methods == nullptr) return 0;

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<ReflectionQueryMethodDescriptor*>(desc->methods));
}

extern "C" CHAOS_IL2CPP_INTPTR ChaosReflectionGetConstructors(CHAOS_IL2CPP_INTPTR type_handle, CHAOS_IL2CPP_INT32 binding_flags) {
    using namespace chaos::il2cpp::runtime_core;
    (void)binding_flags;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr || desc->methods == nullptr) return 0;

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<ReflectionQueryMethodDescriptor*>(desc->methods));
}

extern "C" CHAOS_IL2CPP_INTPTR ChaosReflectionGetMethods(CHAOS_IL2CPP_INTPTR type_handle) {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr || desc->methods == nullptr) return 0;

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<ReflectionQueryMethodDescriptor*>(desc->methods));
}

extern "C" CHAOS_IL2CPP_INTPTR ChaosReflectionGetFields(CHAOS_IL2CPP_INTPTR type_handle) {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr || desc->fields == nullptr) return 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<ReflectionQueryFieldDescriptor*>(desc->fields));
}

extern "C" CHAOS_IL2CPP_INTPTR ChaosReflectionGetInterfaces(CHAOS_IL2CPP_INTPTR type_handle) {
    using namespace chaos::il2cpp::runtime_core;
    (void)type_handle;
    return 0;
}

extern "C" CHAOS_IL2CPP_INTPTR ChaosReflectionGetMembers(CHAOS_IL2CPP_INTPTR type_handle) {
    using namespace chaos::il2cpp::runtime_core;
    (void)type_handle;
    return 0;
}

extern "C" CHAOS_IL2CPP_INTPTR ChaosReflectionGetNestedTypes(CHAOS_IL2CPP_INTPTR type_handle) {
    using namespace chaos::il2cpp::runtime_core;
    (void)type_handle;
    return 0;
}

extern "C" CHAOS_IL2CPP_INTPTR ChaosReflectionGetField(
    CHAOS_IL2CPP_INTPTR type_handle,
    CHAOS_IL2CPP_INTPTR name_string_id)
{
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr || desc->fields == nullptr) return 0;

    (void)name_string_id;
    return 0;
}

extern "C" CHAOS_IL2CPP_INTPTR ChaosReflectionGetMethod(
    CHAOS_IL2CPP_INTPTR type_handle,
    CHAOS_IL2CPP_INTPTR name_string_id,
    CHAOS_IL2CPP_INTPTR param_types)
{
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr || desc->methods == nullptr) return 0;

    (void)name_string_id;
    (void)param_types;
    return 0;
}

extern "C" CHAOS_IL2CPP_INTPTR ChaosReflectionGetGenericArguments(CHAOS_IL2CPP_INTPTR type_handle) {
    (void)type_handle;
    return 0;
}

extern "C" CHAOS_IL2CPP_INTPTR ChaosReflectionGetGenericTypeDefinition(CHAOS_IL2CPP_INTPTR type_handle) {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr || desc->generic_type_definition == nullptr) return 0;

    return static_cast<CHAOS_IL2CPP_INTPTR>(
        EncodeReflectionQueryTypeHandle(desc->generic_type_definition));
}

extern "C" CHAOS_IL2CPP_INTPTR ChaosReflectionCreateInstance(
    CHAOS_IL2CPP_INTPTR type_handle,
    CHAOS_IL2CPP_INTPTR args)
{
    (void)type_handle;
    (void)args;
    return 0;
}

extern "C" CHAOS_IL2CPP_INTPTR ChaosReflectionInvokeMethod(
    CHAOS_IL2CPP_INTPTR method_handle,
    CHAOS_IL2CPP_INTPTR obj,
    CHAOS_IL2CPP_INTPTR args)
{
    (void)method_handle;
    (void)obj;
    (void)args;
    return 0;
}

extern "C" CHAOS_IL2CPP_INTPTR ChaosReflectionMakeGenericMethod(
    CHAOS_IL2CPP_INTPTR method_handle,
    CHAOS_IL2CPP_INTPTR type_args)
{
    (void)method_handle;
    (void)type_args;
    return 0;
}

extern "C" CHAOS_IL2CPP_INTPTR ChaosReflectionGetCustomAttribute(
    CHAOS_IL2CPP_INTPTR member_handle,
    CHAOS_IL2CPP_INTPTR attribute_type_handle)
{
    (void)member_handle;
    (void)attribute_type_handle;
    return 0;
}

// =====================================================================
// String helpers
// =====================================================================

extern "C" CHAOS_IL2CPP_INTPTR ChaosReflectionConcatStringPairValues(
    CHAOS_IL2CPP_INTPTR left,
    CHAOS_IL2CPP_INTPTR right)
{
    (void)left;
    (void)right;
    return 0;
}
