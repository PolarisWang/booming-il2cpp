// reflection_api.cpp — Native AOT reflection API implementation
//
// Provides extern "C" implementations for chaos_reflection_* functions
// called from generated C++ code. Uses the shared AOT constexpr metadata
// tables in reflection_metadata_impl.h for type/method/field queries.
//
// These functions are called via SimpleForward dispatch from generated code,
// NOT through the bridge vtable.

#include "runtime_core.h"
#include "reflection_query_model.h"
#include "reflection_metadata_impl.h"

#include <cstring>
#include <cstdio>

namespace chaos::il2cpp::runtime_core {

// ── Internal helpers ──

// Resolve the current AOT image descriptor.
// In AOT mode, each translation unit sets its image via SetCurrentAotImage().
// The thread_local image pointer allows reflection functions to locate the
// correct constexpr metadata table for the calling family.
static inline const ReflectionQueryImageDescriptor* GetCurrentImage() {
    auto* image = static_cast<const ReflectionQueryImageDescriptor*>(GetCurrentAotImage());
    if (image == nullptr) {
        // Fallback: use CoreLib shared metadata
        return &aot_metadata::kImageCoreLib;
    }
    return image;
}

// Extract metadata token from a type handle.
// The handle can be:
//   - A raw metadata token (ldtoken output) — just use it directly
//   - A ReflectionQuery encoded handle (tag bit 63) — decode and read token
//   - Zero/null — return 0
static inline uint32_t DecodeMetadataToken(CHAOS_IL2CPP_INTPTR handle) {
    if (handle == 0) return 0;

    // Check if it's a ReflectionQuery encoded handle
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryTypeDescriptor>(reinterpret_cast<TypeInfoHandle>(handle));
    if (decoded != nullptr) {
        return decoded->metadata_token;
    }

    // Raw metadata token (fits in 32 bits)
    return static_cast<uint32_t>(handle & 0xFFFFFFFFu);
}

// Get a type descriptor from a handle
static inline const ReflectionQueryTypeDescriptor* GetTypeDescriptorFromHandle(CHAOS_IL2CPP_INTPTR handle) {
    if (handle == 0) return nullptr;

    // Try ReflectionQuery encoded handle first
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryTypeDescriptor>(reinterpret_cast<TypeInfoHandle>(handle));
    if (decoded != nullptr) return decoded;

    // Fall back to metadata token lookup
    uint32_t token = static_cast<uint32_t>(handle & 0xFFFFFFFFu);
    return aot_metadata::FindTypeByMetadataToken(token);
}

// Decode a CHAOS_IL2CPP_INTPTR that may be a StringId or a native pointer
static inline const char* DecodeStringValue(CHAOS_IL2CPP_INTPTR value) {
    if (value == 0) return nullptr;
    // If it's a StringId, we'd need to resolve it via string_table.
    // For now, assume it's a native string pointer (or just return the string_id as-is)
    // Full StringId resolution requires access to the string table.
    return reinterpret_cast<const char*>(value);
}

}  // namespace chaos::il2cpp::runtime_core

// =====================================================================
// Type resolution
// =====================================================================

extern "C" CHAOS_IL2CPP_INTPTR chaos_reflection_get_type_from_handle(CHAOS_IL2CPP_INTPTR runtime_type_handle) {
    using namespace chaos::il2cpp::runtime_core;

    if (runtime_type_handle == 0) return 0;

    uint32_t token = DecodeMetadataToken(runtime_type_handle);
    auto* typeDesc = aot_metadata::FindTypeByMetadataToken(token);
    if (typeDesc == nullptr) return 0;

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryTypeHandle(typeDesc));
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_reflection_get_type_by_name(
    CHAOS_IL2CPP_INTPTR name_string_id,
    CHAOS_IL2CPP_INT32 throw_on_error,
    CHAOS_IL2CPP_INT32 ignore_case)
{
    using namespace chaos::il2cpp::runtime_core;
    (void)throw_on_error;
    (void)ignore_case;

    if (name_string_id == 0) return 0;

    // For AOT mode, we scan the image's type table by display name.
    // The input is a StringId, which we'd need to resolve to a UTF-8 string.
    // For now, return 0 — future implementation with string table resolution.
    (void)name_string_id;
    return 0;
}

// =====================================================================
// Type properties — metadata table queries
// =====================================================================

extern "C" CHAOS_IL2CPP_INTPTR chaos_reflection_get_assembly(CHAOS_IL2CPP_INTPTR type_handle) {
    using namespace chaos::il2cpp::runtime_core;
    auto* image = GetCurrentImage();
    if (image == nullptr) return 0;

    // Return the image handle as a reflection query encoded handle
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryImageHandle(image));
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_reflection_get_assembly_name(CHAOS_IL2CPP_INTPTR assembly_handle) {
    using namespace chaos::il2cpp::runtime_core;
    auto* decoded = TryDecodeReflectionQueryImageHandle(reinterpret_cast<ImageHandle>(assembly_handle));
    if (decoded == nullptr) {
        // Try the current image as fallback
        decoded = GetCurrentImage();
    }
    if (decoded == nullptr) return 0;

    // Return assembly name as a raw pointer (the utf8 string lives in constexpr data)
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(decoded->image_name_utf8);
}

// Placeholder for AssemblyName value object
extern "C" CHAOS_IL2CPP_INTPTR chaos_reflection_get_assembly_name_value(CHAOS_IL2CPP_INTPTR assembly_name_handle) {
    (void)assembly_name_handle;
    return 0;
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_reflection_get_declaring_type(CHAOS_IL2CPP_INTPTR type_handle) {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr) return 0;

    // Declaring type is the generic_type_definition if this is a nested type.
    // For non-nested types, return 0 (declaring type = null).
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
        desc->generic_type_definition != nullptr
            ? EncodeReflectionQueryTypeHandle(desc->generic_type_definition)
            : nullptr);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_reflection_get_member_name(CHAOS_IL2CPP_INTPTR member_handle) {
    using namespace chaos::il2cpp::runtime_core;
    if (member_handle == 0) return 0;

    // Try decoding as each descriptor type
    auto* typeDesc = TryDecodeReflectionQueryHandle<ReflectionQueryTypeDescriptor>(reinterpret_cast<TypeInfoHandle>(member_handle));
    if (typeDesc != nullptr) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(typeDesc->name_utf8);
    }

    auto* methodDesc = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(reinterpret_cast<MethodInfoHandle>(member_handle));
    if (methodDesc != nullptr) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(methodDesc->name_utf8);
    }

    auto* fieldDesc = TryDecodeReflectionQueryHandle<ReflectionQueryFieldDescriptor>(reinterpret_cast<FieldInfoHandle>(member_handle));
    if (fieldDesc != nullptr) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(fieldDesc->name_utf8);
    }

    return 0;
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_reflection_get_parameters(CHAOS_IL2CPP_INTPTR method_handle) {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(reinterpret_cast<MethodInfoHandle>(method_handle));
    if (desc == nullptr) return 0;

    // Return a pointer to the parameter descriptor array
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(desc->parameters);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_reflection_get_parameter_name(
    CHAOS_IL2CPP_INTPTR parameter_handle,
    CHAOS_IL2CPP_INT32 index)
{
    using namespace chaos::il2cpp::runtime_core;
    auto* paramDesc = reinterpret_cast<const ReflectionQueryParameterDescriptor*>(parameter_handle);
    if (paramDesc == nullptr) return 0;

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(paramDesc->name_utf8);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_reflection_get_method_handle(CHAOS_IL2CPP_INTPTR method_handle) {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(reinterpret_cast<MethodInfoHandle>(method_handle));
    if (desc == nullptr) return 0;

    // Return the metadata token as a raw handle
    return static_cast<CHAOS_IL2CPP_INTPTR>(desc->metadata_token);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_reflection_get_metadata_token(CHAOS_IL2CPP_INTPTR member_handle) {
    using namespace chaos::il2cpp::runtime_core;
    if (member_handle == 0) return 0;

    auto* typeDesc = TryDecodeReflectionQueryHandle<ReflectionQueryTypeDescriptor>(reinterpret_cast<TypeInfoHandle>(member_handle));
    if (typeDesc != nullptr) return static_cast<CHAOS_IL2CPP_INTPTR>(typeDesc->metadata_token);

    auto* methodDesc = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(reinterpret_cast<MethodInfoHandle>(member_handle));
    if (methodDesc != nullptr) return static_cast<CHAOS_IL2CPP_INTPTR>(methodDesc->metadata_token);

    auto* fieldDesc = TryDecodeReflectionQueryHandle<ReflectionQueryFieldDescriptor>(reinterpret_cast<FieldInfoHandle>(member_handle));
    if (fieldDesc != nullptr) return static_cast<CHAOS_IL2CPP_INTPTR>(fieldDesc->metadata_token);

    // Raw metadata token passthrough
    return member_handle;
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_reflection_get_type_handle(CHAOS_IL2CPP_INTPTR type_handle) {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr) return 0;

    // Return the metadata token as a handle (matches ldtoken output)
    return static_cast<CHAOS_IL2CPP_INTPTR>(desc->metadata_token);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_reflection_get_constructors(CHAOS_IL2CPP_INTPTR type_handle) {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr || desc->methods == nullptr) return 0;

    // Return pointer to methods array (caller iterates by method_count)
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(desc->methods);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_reflection_get_constructors(CHAOS_IL2CPP_INTPTR type_handle, CHAOS_IL2CPP_INT32 binding_flags) {
    using namespace chaos::il2cpp::runtime_core;
    (void)binding_flags;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr || desc->methods == nullptr) return 0;

    // Return pointer to methods array (caller iterates by method_count)
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(desc->methods);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_reflection_get_methods(CHAOS_IL2CPP_INTPTR type_handle) {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr || desc->methods == nullptr) return 0;

    // Return pointer to methods array (caller iterates by method_count)
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(desc->methods);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_reflection_get_fields(CHAOS_IL2CPP_INTPTR type_handle) {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr || desc->fields == nullptr) return 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(desc->fields);
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_reflection_get_interfaces(CHAOS_IL2CPP_INTPTR type_handle) {
    using namespace chaos::il2cpp::runtime_core;
    (void)type_handle;
    return 0;
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_reflection_get_members(CHAOS_IL2CPP_INTPTR type_handle) {
    using namespace chaos::il2cpp::runtime_core;
    (void)type_handle;
    return 0;
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_reflection_get_nested_types(CHAOS_IL2CPP_INTPTR type_handle) {
    using namespace chaos::il2cpp::runtime_core;
    (void)type_handle;
    return 0;
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_reflection_get_field(
    CHAOS_IL2CPP_INTPTR type_handle,
    CHAOS_IL2CPP_INTPTR name_string_id)
{
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr || desc->fields == nullptr) return 0;

    // Search fields by name
    // For now, return 0 — string comparison needs StringId resolution
    (void)name_string_id;
    return 0;
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_reflection_get_method(
    CHAOS_IL2CPP_INTPTR type_handle,
    CHAOS_IL2CPP_INTPTR name_string_id,
    CHAOS_IL2CPP_INTPTR param_types)
{
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr || desc->methods == nullptr) return 0;

    // Search methods by name
    (void)name_string_id;
    (void)param_types;
    return 0;
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_reflection_get_generic_arguments(CHAOS_IL2CPP_INTPTR type_handle) {
    (void)type_handle;
    return 0;
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_reflection_get_generic_type_definition(CHAOS_IL2CPP_INTPTR type_handle) {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr || desc->generic_type_definition == nullptr) return 0;

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
        EncodeReflectionQueryTypeHandle(desc->generic_type_definition));
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_reflection_create_instance(
    CHAOS_IL2CPP_INTPTR type_handle,
    CHAOS_IL2CPP_INTPTR args)
{
    (void)type_handle;
    (void)args;
    // Requires RuntimeState for GC allocation — placeholder
    return 0;
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_reflection_invoke_method(
    CHAOS_IL2CPP_INTPTR method_handle,
    CHAOS_IL2CPP_INTPTR obj,
    CHAOS_IL2CPP_INTPTR args)
{
    (void)method_handle;
    (void)obj;
    (void)args;
    return 0;
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_reflection_make_generic_method(
    CHAOS_IL2CPP_INTPTR method_handle,
    CHAOS_IL2CPP_INTPTR type_args)
{
    (void)method_handle;
    (void)type_args;
    return 0;
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_reflection_get_custom_attribute(
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

extern "C" CHAOS_IL2CPP_INTPTR chaos_reflection_concat_string_pair_values(
    CHAOS_IL2CPP_INTPTR left,
    CHAOS_IL2CPP_INTPTR right)
{
    (void)left;
    (void)right;
    // String concatenation requires heap allocation — placeholder
    return 0;
}
