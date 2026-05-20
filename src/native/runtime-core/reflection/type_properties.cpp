// =====================================================================
// Type properties — Module Registry / Image queries
// =====================================================================

extern "C" {
namespace chaos::il2cpp::runtime_core {

CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssembly(CHAOS_IL2CPP_INTPTR type_handle) {
    // Resolve the image from the type handle via Module Registry
    auto* image = GetImageFromReflectionOrGcHandle(type_handle);
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
    auto* desc = ResolveTypeFromReflectionOrGcHandle(type_handle);
    if (desc == nullptr) return 0;

    return static_cast<CHAOS_IL2CPP_INTPTR>(
        desc->generic_type_definition != nullptr
            ? EncodeReflectionQueryTypeHandle(desc->generic_type_definition)
            : static_cast<TypeInfoHandle>(0));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetMemberName(CHAOS_IL2CPP_INTPTR member_handle) {
    using namespace chaos::il2cpp::runtime_core;
    if (member_handle == 0) return 0;

    const char* name = nullptr;

    // Try decoding as each descriptor type
    auto* typeDesc = TryDecodeReflectionQueryHandle<ReflectionQueryTypeDescriptor>(static_cast<TypeInfoHandle>(member_handle));
    if (typeDesc != nullptr) {
        name = typeDesc->name_utf8;
    }

    auto* methodDesc = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(static_cast<MethodInfoHandle>(member_handle));
    if (methodDesc != nullptr) {
        name = methodDesc->name_utf8;
    }

    auto* fieldDesc = TryDecodeReflectionQueryHandle<ReflectionQueryFieldDescriptor>(static_cast<FieldInfoHandle>(member_handle));
    if (fieldDesc != nullptr) {
        name = fieldDesc->name_utf8;
    }

    // Fallback: could be a GC Type object (no tag bit). Read runtime_type_handle
    // at offset 16 and resolve the type descriptor.
    if (name == nullptr) {
        auto* desc = ResolveTypeFromReflectionOrGcHandle(member_handle);
        if (desc != nullptr) {
            name = desc->name_utf8;
        }
    }

    if (name == nullptr) return 0;

    auto str_id = static_cast<intptr_t>(
        string_table::Intern(name, static_cast<CHAOS_IL2CPP_UINT32>(std::strlen(name))));
    return static_cast<CHAOS_IL2CPP_INTPTR>(str_id | CHAOS_STRING_ID_TAG);
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
    auto* desc = ResolveTypeFromReflectionOrGcHandle(type_handle);
    if (desc == nullptr) return 0;

    return static_cast<CHAOS_IL2CPP_INTPTR>(desc->metadata_token);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetConstructorsDefault(CHAOS_IL2CPP_INTPTR type_handle) noexcept {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = ResolveTypeFromReflectionOrGcHandle(type_handle);
    if (desc == nullptr || desc->methods == nullptr) return 0;

    // Count only .ctor methods (method_count includes all methods)
    uint32_t ctor_count = 0;
    for (uint32_t i = 0; i < desc->method_count; i++) {
        if (desc->methods[i].name_utf8 != nullptr &&
            std::strcmp(desc->methods[i].name_utf8, ".ctor") == 0) {
            ctor_count++;
        }
    }
    return static_cast<CHAOS_IL2CPP_INTPTR>(ctor_count);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetConstructors(CHAOS_IL2CPP_INTPTR type_handle, CHAOS_IL2CPP_INT32 binding_flags) {
    using namespace chaos::il2cpp::runtime_core;
    (void)binding_flags;
    auto* desc = ResolveTypeFromReflectionOrGcHandle(type_handle);
    if (desc == nullptr || desc->methods == nullptr) return 0;

    // Count only .ctor methods
    uint32_t ctor_count = 0;
    for (uint32_t i = 0; i < desc->method_count; i++) {
        if (desc->methods[i].name_utf8 != nullptr &&
            std::strcmp(desc->methods[i].name_utf8, ".ctor") == 0) {
            ctor_count++;
        }
    }
    return static_cast<CHAOS_IL2CPP_INTPTR>(ctor_count);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetMethods(CHAOS_IL2CPP_INTPTR type_handle) {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = ResolveTypeFromReflectionOrGcHandle(type_handle);
    if (desc == nullptr) return 0;

    // codegen reads .Length via (int32)result — return total count directly
    uint32_t total = desc->method_count;

    // Walk the parent TypeInfo chain to include inherited methods
    auto* type_info = GetTypeInfoFromReflectionOrGcHandle(type_handle);
    if (type_info != nullptr) {
        const TypeInfoHot* parent = type_info->parent;
        uint32_t max_depth = 20;
        while (parent != nullptr && max_depth > 0) {
            uint32_t count = GetModuleCount();
            bool found = false;
            for (uint32_t i = 0; i < count && !found; i++) {
                const auto* mod = GetModuleByIndex(i);
                if (mod == nullptr || mod->type_info_ptrs == nullptr || mod->image == nullptr) continue;
                for (uint32_t j = 0; j < mod->type_count && j < mod->image->type_count && !found; j++) {
                    if (mod->type_info_ptrs[j] == parent) {
                        total += mod->image->types[j]->method_count;
                        found = true;
                    }
                }
            }
            if (!found) break;
            parent = parent->parent;
            max_depth--;
        }
    }

    return static_cast<CHAOS_IL2CPP_INTPTR>(total);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetFields(CHAOS_IL2CPP_INTPTR type_handle) {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = ResolveTypeFromReflectionOrGcHandle(type_handle);
    if (desc == nullptr || desc->fields == nullptr) return 0;

    constexpr CHAOS_IL2CPP_UINT32 kMaxFields = 256;
    struct GetFieldsBuf {
        ThinLockableHeader header;
        CHAOS_IL2CPP_UINT8  element_type_shape;
        CHAOS_IL2CPP_INTPTR element_type_info;
        CHAOS_IL2CPP_INTPTR length;
        CHAOS_IL2CPP_INTPTR* elements;
    };
    static GetFieldsBuf s_buf{};
    static CHAOS_IL2CPP_INTPTR s_elements[kMaxFields]{};

    const CHAOS_IL2CPP_UINT32 count = desc->field_count > kMaxFields ? kMaxFields : desc->field_count;
    s_buf = GetFieldsBuf{};
    s_buf.element_type_shape = 1;
    s_buf.length = static_cast<CHAOS_IL2CPP_INTPTR>(count);
    s_buf.elements = s_elements;

    for (CHAOS_IL2CPP_UINT32 i = 0; i < count; i++)
        s_elements[i] = static_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryFieldHandle(&desc->fields[i]));

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&s_buf);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetProperties(CHAOS_IL2CPP_INTPTR type_handle) {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = ResolveTypeFromReflectionOrGcHandle(type_handle);
    if (desc == nullptr || desc->properties == nullptr) return 0;

    constexpr CHAOS_IL2CPP_UINT32 kMaxProperties = 256;
    struct GetPropertiesBuf {
        ThinLockableHeader header;
        CHAOS_IL2CPP_UINT8  element_type_shape;
        CHAOS_IL2CPP_INTPTR element_type_info;
        CHAOS_IL2CPP_INTPTR length;
        CHAOS_IL2CPP_INTPTR* elements;
    };
    static GetPropertiesBuf s_buf{};
    static CHAOS_IL2CPP_INTPTR s_elements[kMaxProperties]{};

    const CHAOS_IL2CPP_UINT32 count = desc->property_count > kMaxProperties ? kMaxProperties : desc->property_count;
    s_buf = GetPropertiesBuf{};
    s_buf.element_type_shape = 1;
    s_buf.length = static_cast<CHAOS_IL2CPP_INTPTR>(count);
    s_buf.elements = s_elements;

    for (CHAOS_IL2CPP_UINT32 i = 0; i < count; i++)
        s_elements[i] = static_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryPropertyHandle(&desc->properties[i]));

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&s_buf);
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
        auto result = static_cast<CHAOS_IL2CPP_INTPTR>(MakeTypeHandle(tr.module_id, parent_token));
        return result;
    }

    // Fallback for ReflectionQuery encoded handles (bit 63) or GC Type objects
    auto* desc = ResolveTypeFromReflectionOrGcHandle(type_handle);
    if (desc == nullptr || desc->metadata_token == 0) {
        return 0;
    }

    // For aot_metadata::kAllTypes descriptors (hand-written), the
    // generic_type_definition field doubles as the parent type pointer.
    // Scriban-generated descriptors always set it to nullptr and rely on
    // module-registry type_parent_tokens instead.
    if (desc->generic_type_definition != nullptr) {
        return static_cast<CHAOS_IL2CPP_INTPTR>(
            EncodeReflectionQueryTypeHandle(desc->generic_type_definition));
    }

    return 0;
}

// ── GetNamespace ──────────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionGetNamespace(CHAOS_IL2CPP_INTPTR type_handle) noexcept {
    const char* ns = nullptr;

    TypeRef tr;
    if (ResolveTypeRef(type_handle, tr)) {
        ns = tr.module->type_namespaces[tr.type_index];
    } else {
        // Fallback: decode descriptor
        auto* desc = ResolveTypeFromReflectionOrGcHandle(type_handle);
        if (desc != nullptr) {
            ns = desc->namespace_name_utf8;
        }
    }

    if (ns == nullptr || ns[0] == '\0') return 0;

    auto str_id = static_cast<intptr_t>(
        string_table::Intern(ns, static_cast<CHAOS_IL2CPP_UINT32>(std::strlen(ns))));
    return static_cast<CHAOS_IL2CPP_INTPTR>(str_id | CHAOS_STRING_ID_TAG);
}

// ── GetTypeFullName ──────────────────────────────────────────────
// Returns "Namespace.Name" for types with a namespace, or just "Name" for
// global types. Uses string_table::Intern to return a StringId that the
// codegen's String.get_Length can resolve via string_table::Resolve.
CHAOS_IL2CPP_INTPTR ChaosReflectionGetTypeFullName(CHAOS_IL2CPP_INTPTR type_handle) noexcept {
    const char* ns = nullptr;
    const char* name = nullptr;

    // Try Tier 0 first
    TypeRef tr;
    if (ResolveTypeRef(type_handle, tr)) {
        ns = tr.module->type_namespaces[tr.type_index];
        name = tr.module->type_names[tr.type_index];
    } else {
        // Fallback: decode descriptor
        auto* desc = ResolveTypeFromReflectionOrGcHandle(type_handle);
        if (desc != nullptr && desc->name_utf8 != nullptr) {
            ns = desc->namespace_name_utf8;
            name = desc->name_utf8;
        }
    }

    if (name == nullptr) {
        return 0;
    }

    // Build full name and intern as StringId
    static char s_buf[1024];
    if (ns != nullptr && ns[0] != '\0') {
        auto result = fmt::format_to_n(s_buf, sizeof(s_buf) - 1, "{}.{}", ns, name);
        auto str_id = static_cast<intptr_t>(
            string_table::Intern(s_buf, static_cast<CHAOS_IL2CPP_UINT32>(result.size)));
        return static_cast<CHAOS_IL2CPP_INTPTR>(str_id | CHAOS_STRING_ID_TAG);
    }

    {
        auto str_id = static_cast<intptr_t>(
            string_table::Intern(name, static_cast<CHAOS_IL2CPP_UINT32>(std::strlen(name))));
        return static_cast<CHAOS_IL2CPP_INTPTR>(str_id | CHAOS_STRING_ID_TAG);
    }
}

// ── GetAssemblyQualifiedName ─────────────────────────────────────
// Returns "Namespace.Name, AssemblyName, Version=..., Culture=..., PublicKeyToken=..."
// using ModuleRegistry Tier 0 or descriptor fallback.
// Uses string_table::Intern for StringId return.
CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssemblyQualifiedName(CHAOS_IL2CPP_INTPTR type_handle) noexcept {
    const char* ns = nullptr;
    const char* name = nullptr;
    const char* assembly = nullptr;

    TypeRef tr;
    if (ResolveTypeRef(type_handle, tr)) {
        ns = tr.module->type_namespaces[tr.type_index];
        name = tr.module->type_names[tr.type_index];
        assembly = tr.module->name_utf8;
    } else {
        // Fallback: decode descriptor + image
        auto* desc = ResolveTypeFromReflectionOrGcHandle(type_handle);
        if (desc == nullptr || desc->name_utf8 == nullptr) return 0;
        auto* image = GetImageFromReflectionOrGcHandle(type_handle);
        if (image == nullptr || image->image_name_utf8 == nullptr) return 0;
        ns = desc->namespace_name_utf8;
        name = desc->name_utf8;
        assembly = image->image_name_utf8;
    }

    if (name == nullptr || assembly == nullptr) return 0;

    // Build the full assembly qualified name including version/culture/token.
    // .NET format: "Namespace.Type, Assembly, Version=X.Y.Z.W, Culture=neutral, PublicKeyToken=..."
    static char s_buf[2048];
    size_t len = 0;
    if (ns != nullptr && ns[0] != '\0') {
        auto result = fmt::format_to_n(s_buf, sizeof(s_buf) - 1,
            "{}.{}, {}, Version=10.0.0.0, Culture=neutral, PublicKeyToken=7cec85d7bea7798e",
            ns, name, assembly);
        len = result.size;
    } else {
        auto result = fmt::format_to_n(s_buf, sizeof(s_buf) - 1,
            "{}, {}, Version=10.0.0.0, Culture=neutral, PublicKeyToken=7cec85d7bea7798e",
            name, assembly);
        len = result.size;
    }

    auto str_id = static_cast<intptr_t>(
        string_table::Intern(s_buf, static_cast<CHAOS_IL2CPP_UINT32>(len)));
    return static_cast<CHAOS_IL2CPP_INTPTR>(str_id | CHAOS_STRING_ID_TAG);
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

    // Fallback for GC Type objects: resolve descriptor and return it
    {
        auto* desc = ResolveTypeFromReflectionOrGcHandle(member_handle);
        if (desc != nullptr) {
            if (desc->generic_type_definition != nullptr) {
                return static_cast<CHAOS_IL2CPP_INTPTR>(
                    EncodeReflectionQueryTypeHandle(desc->generic_type_definition));
            }
            return static_cast<CHAOS_IL2CPP_INTPTR>(
                EncodeReflectionQueryTypeHandle(desc));
        }
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