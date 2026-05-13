// =====================================================================
// Type properties — Module Registry / Image queries
// =====================================================================

extern "C" {
namespace chaos::il2cpp::runtime_core {

CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssembly(CHAOS_IL2CPP_INTPTR type_handle) {
    // Resolve the image from the type handle via Module Registry
    auto* image = GetImageFromTypeHandle(type_handle);
    if (image == nullptr) {
        // Fallback: CoreLib shared metadata
        image = &aot_metadata::kImageCoreLib;
    }

    return static_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryImageHandle(image));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssemblyName(CHAOS_IL2CPP_INTPTR assembly_handle) {
    using namespace chaos::il2cpp::runtime_core;

    auto* decoded = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(assembly_handle));
    if (decoded == nullptr) {
        // Fallback: CoreLib shared metadata
        decoded = &aot_metadata::kImageCoreLib;
    }

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<char*>(decoded->image_name_utf8));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetDeclaringType(CHAOS_IL2CPP_INTPTR type_handle) {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr) return 0;

    return static_cast<CHAOS_IL2CPP_INTPTR>(
        desc->generic_type_definition != nullptr
            ? EncodeReflectionQueryTypeHandle(desc->generic_type_definition)
            : static_cast<TypeInfoHandle>(0));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetMemberName(CHAOS_IL2CPP_INTPTR member_handle) {
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

CHAOS_IL2CPP_INTPTR ChaosReflectionGetParameters(CHAOS_IL2CPP_INTPTR method_handle) {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(static_cast<MethodInfoHandle>(method_handle));
    if (desc == nullptr) return 0;

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<ReflectionQueryParameterDescriptor*>(desc->parameters));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetParameterName(
    CHAOS_IL2CPP_INTPTR parameter_handle)
{
    using namespace chaos::il2cpp::runtime_core;
    auto* paramDesc = reinterpret_cast<const ReflectionQueryParameterDescriptor*>(parameter_handle);
    if (paramDesc == nullptr) return 0;

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<char*>(paramDesc->name_utf8));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetMethodHandle(CHAOS_IL2CPP_INTPTR method_handle) {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(static_cast<MethodInfoHandle>(method_handle));
    if (desc == nullptr) return 0;

    return static_cast<CHAOS_IL2CPP_INTPTR>(desc->metadata_token);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetMetadataToken(CHAOS_IL2CPP_INTPTR member_handle) {
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

CHAOS_IL2CPP_INTPTR ChaosReflectionGetTypeHandle(CHAOS_IL2CPP_INTPTR type_handle) {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr) return 0;

    return static_cast<CHAOS_IL2CPP_INTPTR>(desc->metadata_token);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetConstructorsDefault(CHAOS_IL2CPP_INTPTR type_handle) noexcept {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr || desc->methods == nullptr) return 0;

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<ReflectionQueryMethodDescriptor*>(desc->methods));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetConstructors(CHAOS_IL2CPP_INTPTR type_handle, CHAOS_IL2CPP_INT32 binding_flags) {
    using namespace chaos::il2cpp::runtime_core;
    (void)binding_flags;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr || desc->methods == nullptr) return 0;

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<ReflectionQueryMethodDescriptor*>(desc->methods));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetMethods(CHAOS_IL2CPP_INTPTR type_handle) {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr || desc->methods == nullptr) return 0;

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<ReflectionQueryMethodDescriptor*>(desc->methods));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetFields(CHAOS_IL2CPP_INTPTR type_handle) {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr || desc->fields == nullptr) return 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<ReflectionQueryFieldDescriptor*>(desc->fields));
}

// =====================================================================
// Type property checks — Module Registry Tier 0 (type_flags[])
// =====================================================================

// ── Helper: return 1/0 from a Tier 0 flag check with descriptor fallback ──
#define CHAOS_DEFINE_TYPE_FLAG_STUB(Name, FlagBit)                              \
CHAOS_IL2CPP_INTPTR Name(CHAOS_IL2CPP_INTPTR type_handle) noexcept {            \
    TypeRef tr;                                                                  \
    if (ResolveTypeRef(type_handle, tr))                                         \
        return (tr.module->type_flags[tr.type_index] & (FlagBit)) != 0u ? 1 : 0;\
    return 0;                                                                    \
}

CHAOS_DEFINE_TYPE_FLAG_STUB(ChaosReflectionGetIsInterface,          kFlagIsInterface)
CHAOS_DEFINE_TYPE_FLAG_STUB(ChaosReflectionGetIsArray,              kFlagIsArray)
CHAOS_DEFINE_TYPE_FLAG_STUB(ChaosReflectionGetIsEnum,               kFlagIsEnum)
CHAOS_DEFINE_TYPE_FLAG_STUB(ChaosReflectionGetIsAbstract,           kFlagIsAbstract)
CHAOS_DEFINE_TYPE_FLAG_STUB(ChaosReflectionGetIsSealed,             kFlagIsSealed)
CHAOS_DEFINE_TYPE_FLAG_STUB(ChaosReflectionGetIsValueType,          kFlagIsValueType)
CHAOS_DEFINE_TYPE_FLAG_STUB(ChaosReflectionGetIsGenericType,        kFlagIsGenericType)
CHAOS_DEFINE_TYPE_FLAG_STUB(ChaosReflectionGetIsConstructedGeneric, kFlagIsConstructedGeneric)
CHAOS_DEFINE_TYPE_FLAG_STUB(ChaosReflectionGetIsGenericTypeDef,     kFlagIsGenericTypeDef)
CHAOS_DEFINE_TYPE_FLAG_STUB(ChaosReflectionGetIsPublic,             kFlagIsPublic)

// ── GetBaseType ────────────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionGetBaseType(CHAOS_IL2CPP_INTPTR type_handle) noexcept {
    TypeRef tr;
    if (ResolveTypeRef(type_handle, tr)) {
        uint32_t parent_token = tr.module->type_parent_tokens[tr.type_index];
        if (parent_token == 0) return 0;
        // Same-module parent: encode as (module_id << 32) | parent_token
        return static_cast<CHAOS_IL2CPP_INTPTR>(MakeTypeHandle(tr.module_id, parent_token));
    }
    return 0;
}

// ── GetNamespace ──────────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionGetNamespace(CHAOS_IL2CPP_INTPTR type_handle) noexcept {
    TypeRef tr;
    if (ResolveTypeRef(type_handle, tr)) {
        const char* ns = tr.module->type_namespaces[tr.type_index];
        if (ns == nullptr) return 0;
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<char*>(ns));
    }
    // Fallback: decode descriptor
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr || desc->namespace_name_utf8 == nullptr) return 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<char*>(desc->namespace_name_utf8));
}

// ── GetTypeFullName ──────────────────────────────────────────────
// Returns "Namespace.Name" for types with a namespace, or just "Name" for
// global types. Uses a static buffer for concatenation (caller marshals
// immediately via shape-dispatch wrapper).
CHAOS_IL2CPP_INTPTR ChaosReflectionGetTypeFullName(CHAOS_IL2CPP_INTPTR type_handle) noexcept {
    // Try Tier 0 first
    TypeRef tr;
    if (ResolveTypeRef(type_handle, tr)) {
        const char* ns = tr.module->type_namespaces[tr.type_index];
        const char* name = tr.module->type_names[tr.type_index];
        if (name == nullptr) return 0;
        if (ns == nullptr || ns[0] == '\0') {
            return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<char*>(name));
        }
        static char s_buf[1024];
        fmt::format_to_n(s_buf, sizeof(s_buf) - 1, "{}.{}", ns, name);
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_buf);
    }

    // Fallback: decode descriptor
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr || desc->name_utf8 == nullptr) return 0;
    if (desc->namespace_name_utf8 == nullptr || desc->namespace_name_utf8[0] == '\0') {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<char*>(desc->name_utf8));
    }
    static char s_buf2[1024];
    fmt::format_to_n(s_buf2, sizeof(s_buf2) - 1, "{}.{}", desc->namespace_name_utf8, desc->name_utf8);
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_buf2);
}

// ── GetAssemblyQualifiedName ─────────────────────────────────────
// Returns "Namespace.Name, AssemblyName" using ModuleRegistry Tier 0
// or descriptor fallback. Static buffer for concatenation.
CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssemblyQualifiedName(CHAOS_IL2CPP_INTPTR type_handle) noexcept {
    TypeRef tr;
    if (ResolveTypeRef(type_handle, tr)) {
        const char* ns = tr.module->type_namespaces[tr.type_index];
        const char* name = tr.module->type_names[tr.type_index];
        const char* assembly = tr.module->name_utf8;
        if (name == nullptr || assembly == nullptr) return 0;

        static char s_buf[2048];
        if (ns != nullptr && ns[0] != '\0') {
            fmt::format_to_n(s_buf, sizeof(s_buf) - 1, "{}.{}, {}", ns, name, assembly);
        } else {
            fmt::format_to_n(s_buf, sizeof(s_buf) - 1, "{}, {}", name, assembly);
        }
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_buf);
    }

    // Fallback: decode descriptor + image
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr || desc->name_utf8 == nullptr) return 0;
    auto* image = GetImageFromTypeHandle(type_handle);
    if (image == nullptr || image->image_name_utf8 == nullptr) return 0;

    static char s_buf2[2048];
    if (desc->namespace_name_utf8 != nullptr && desc->namespace_name_utf8[0] != '\0') {
        fmt::format_to_n(s_buf2, sizeof(s_buf2) - 1, "{}.{}, {}",
            desc->namespace_name_utf8, desc->name_utf8, image->image_name_utf8);
    } else {
        fmt::format_to_n(s_buf2, sizeof(s_buf2) - 1, "{}, {}",
            desc->name_utf8, image->image_name_utf8);
    }
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_buf2);
}

// ── GetAssemblyNameValue ─────────────────────────────────────────
// Reads the runtime_name_value field from a managed AssemblyName object.
// The managed AssemblyName stores the name string pointer at offset 16.
CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssemblyNameValue(CHAOS_IL2CPP_INTPTR assembly_name_handle) noexcept {
    if (assembly_name_handle == 0) return 0;
    // The managed AssemblyName stores runtime_name_value at offset 16.
    return *reinterpret_cast<const CHAOS_IL2CPP_INTPTR*>(
        static_cast<const char*>(reinterpret_cast<const void*>(
            static_cast<CHAOS_IL2CPP_INTPTR>(assembly_name_handle))) + 16);
}

// ── GetReflectedType ─────────────────────────────────────────────
// Returns the type that owns the given member. For type handles, this
// returns the declaring type (nested type parent). For method/field
// handles, returns the declaring type via descriptor lookup.
CHAOS_IL2CPP_INTPTR ChaosReflectionGetReflectedType(CHAOS_IL2CPP_INTPTR member_handle) noexcept {
    using namespace chaos::il2cpp::runtime_core;

    if (member_handle == 0) return 0;

    // Try type handle (including Module Registry encoded handles)
    {
        TypeRef tr;
        if (ResolveTypeRef(member_handle, tr)) {
            // For types, we don't track nested-type parent via Tier 0 arrays yet.
            // Fall through to descriptor path.
        }
    }

    // Try decoding as each descriptor type
    auto* typeDesc = TryDecodeReflectionQueryHandle<ReflectionQueryTypeDescriptor>(
        static_cast<TypeInfoHandle>(member_handle));
    if (typeDesc != nullptr) {
        // Return generic type definition if this is a constructed generic
        if (typeDesc->generic_type_definition != nullptr) {
            return static_cast<CHAOS_IL2CPP_INTPTR>(
                EncodeReflectionQueryTypeHandle(typeDesc->generic_type_definition));
        }
        return 0;
    }

    auto* methodDesc = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(
        static_cast<MethodInfoHandle>(member_handle));
    if (methodDesc != nullptr) {
        const uint32_t token = methodDesc->metadata_token;
        if (token != 0u) {
            const uint32_t module_count = GetModuleCount();
            for (uint32_t i = 0u; i < module_count; i++) {
                const auto* mod = GetModuleByIndex(i);
                if (mod == nullptr || mod->image == nullptr) continue;
                for (uint32_t t = 0u; t < mod->image->type_count; t++) {
                    const auto* type = mod->image->types[t];
                    if (type == nullptr || type->methods == nullptr) continue;
                    for (uint32_t m = 0u; m < type->method_count; m++) {
                        if (type->methods[m].metadata_token == token) {
                            return static_cast<CHAOS_IL2CPP_INTPTR>(
                                EncodeReflectionQueryTypeHandle(type));
                        }
                    }
                }
            }
        }
        return 0;
    }

    auto* fieldDesc = TryDecodeReflectionQueryHandle<ReflectionQueryFieldDescriptor>(
        static_cast<FieldInfoHandle>(member_handle));
    if (fieldDesc != nullptr) {
        const uint32_t token = fieldDesc->metadata_token;
        if (token != 0u) {
            const uint32_t module_count = GetModuleCount();
            for (uint32_t i = 0u; i < module_count; i++) {
                const auto* mod = GetModuleByIndex(i);
                if (mod == nullptr || mod->image == nullptr) continue;
                for (uint32_t t = 0u; t < mod->image->type_count; t++) {
                    const auto* type = mod->image->types[t];
                    if (type == nullptr || type->fields == nullptr) continue;
                    for (uint32_t f = 0u; f < type->field_count; f++) {
                        if (type->fields[f].metadata_token == token) {
                            return static_cast<CHAOS_IL2CPP_INTPTR>(
                                EncodeReflectionQueryTypeHandle(type));
                        }
                    }
                }
            }
        }
        return 0;
    }

    return 0;
}

}  // namespace chaos::il2cpp::runtime_core
}  // extern "C"