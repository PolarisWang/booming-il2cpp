// =====================================================================
// Remaining reflection stubs — deferred to Phase 2+/3+
// =====================================================================
// These are either not yet wired to data sources (assembly-level queries,
// custom attributes) or waiting on EEClass expansion.
// Keeping them in one place makes it easy to track Phase 1+ progress.
//
// Note: This file is #included from reflection_api.cpp, so it shares
// the same translation unit — internal helpers from internal_helpers.cpp
// (GetTypeDescriptorFromHandle, GetTypeInfoFromHandle, etc.) are available.

extern "C" {
namespace chaos::il2cpp::runtime_core {

// ── Custom attribute stubs ─────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionIsDefined(CHAOS_IL2CPP_INTPTR assembly, CHAOS_IL2CPP_INTPTR type) noexcept {
    // Phase 2: try blob-based lookup for type-level custom attributes.
    // assembly: encoded assembly/module handle.
    // type:     TypeInfoHandle of the attribute type to check for.
    // Returns non-null if the type has a matching custom attribute in its
    // module's custom_attribute_blob.
    //
    // Note: For assembly-level IsDefined, this may need to scan all types
    // in the assembly — deferred to Phase 3.
    //
    // For the simple type-level check, call through to ChaosGetCustomAttributeFromBlob
    // with member_kind=1 (Type) and pass type as the attr_type_handle.
    // If the assembly parameter can be treated as a member_handle (TypeInfoHandle),
    // call the blob lookup directly.
    return ChaosGetCustomAttributeFromBlob(
        static_cast<CHAOS_IL2CPP_INTPTR>(1),     // member_kind = Type
        assembly,                                  // member_handle = TypeInfoHandle from assembly param
        type);                                     // attr_type_handle = attribute type to check for
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetRequiredCustomModifiers(CHAOS_IL2CPP_INTPTR /*param*/) noexcept {
    static CHAOS_IL2CPP_INTPTR s_empty[1] = {0};
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_empty);
}

// ── Parameter default values (real implementations) ─────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionHasDefaultValue(CHAOS_IL2CPP_INTPTR param) noexcept {
    if (param == 0) return 0;
    auto* p = reinterpret_cast<const ReflectionQueryParameterDescriptor*>(
        static_cast<CHAOS_IL2CPP_INTPTR>(param));
    return (p->default_value_blob != 0 && p->default_value_size > 0) ? 1 : 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetDefaultValue(CHAOS_IL2CPP_INTPTR param) noexcept {
    if (param == 0) return 0;
    auto* p = reinterpret_cast<const ReflectionQueryParameterDescriptor*>(
        static_cast<CHAOS_IL2CPP_INTPTR>(param));
    return p->default_value_blob;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetRawDefaultValue(CHAOS_IL2CPP_INTPTR param) noexcept {
    return ChaosReflectionGetDefaultValue(param);
}

// ── MethodInfo stubs ────────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionGetIsVirtual(CHAOS_IL2CPP_INTPTR member) noexcept {
    // Decode handle to ReflectionQueryMethodDescriptor and return IsVirtual flag.
    auto* method = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(static_cast<MethodInfoHandle>(member));
    if (method == nullptr) return 0;
    return (method->flags & kMethodFlagIsVirtual) ? 1 : 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetBaseDefinition(CHAOS_IL2CPP_INTPTR member_handle) noexcept {
    return member_handle;  // self is the base definition (no overrides in AOT)
}

// ── Runtime type handle stubs ───────────────────────────────────────

CHAOS_IL2CPP_INTPTR ChaosTypeGetTypeFromHandle(CHAOS_IL2CPP_INTPTR handle) noexcept {
    // Resolve the TypeInfoHandle → ReflectionQueryTypeDescriptor → encoded Type handle.
    auto* desc = GetTypeDescriptorFromHandle(handle);
    if (desc == nullptr) return 0;
    return static_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryTypeHandle(desc));
}

CHAOS_IL2CPP_INTPTR ChaosRuntimeTypeFromHandle(CHAOS_IL2CPP_INTPTR handle) noexcept {
    // A RuntimeTypeHandle is a TypeInfoHandle. Convert to Type handle.
    return ChaosTypeGetTypeFromHandle(handle);
}

CHAOS_IL2CPP_INTPTR ChaosTypeEquals(CHAOS_IL2CPP_INTPTR type_a, CHAOS_IL2CPP_INTPTR type_b) noexcept {
    if (type_a == 0 || type_b == 0) return 0;
    // Fast path: identical handles are the same type.
    if (type_a == type_b) return 1;
    // Full path: decode both to descriptors and compare pointers.
    auto* desc_a = GetTypeDescriptorFromHandle(type_a);
    auto* desc_b = GetTypeDescriptorFromHandle(type_b);
    return (desc_a != nullptr && desc_a == desc_b) ? 1 : 0;
}

CHAOS_IL2CPP_INTPTR ChaosTypeInequality(CHAOS_IL2CPP_INTPTR type_a, CHAOS_IL2CPP_INTPTR type_b) noexcept {
    // Negation of ChaosTypeEquals
    return ChaosTypeEquals(type_a, type_b) ? 0 : 1;
}

CHAOS_IL2CPP_INTPTR ChaosTypeGetTypeInfo(CHAOS_IL2CPP_INTPTR type) noexcept {
    // Return the TypeInfoHot* for this type handle, if type_info_ptrs is populated.
    auto* type_info = GetTypeInfoFromAnyHandle(type);
    if (type_info == nullptr) return 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<TypeInfoHot*>(type_info));
}

// ── Assembly stubs ──────────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetExportedTypes(CHAOS_IL2CPP_INTPTR /*assembly*/) noexcept {
    return 0;  // Exported type array not yet wired; returns empty.
}

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetForwardedTypes(CHAOS_IL2CPP_INTPTR assembly) noexcept {
    // Return a non-zero pointer to an empty-array marker so the managed
    // wrapper treats this as "empty list" rather than "not supported".
    // Format: [count=0] so any first-field read yields zero.
    if (assembly == 0) return 0;
    static const CHAOS_IL2CPP_INTPTR s_emptyArray = 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<CHAOS_IL2CPP_INTPTR*>(&s_emptyArray));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetManifestResourceNames(CHAOS_IL2CPP_INTPTR assembly) noexcept {
    // Return a non-zero pointer to an empty-array marker (same as GetForwardedTypes).
    if (assembly == 0) return 0;
    static const CHAOS_IL2CPP_INTPTR s_emptyArray = 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<CHAOS_IL2CPP_INTPTR*>(&s_emptyArray));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetModules(CHAOS_IL2CPP_INTPTR /*assembly*/) noexcept {
    return 0;  // Module array not yet wired; returns empty.
}

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetEntryPoint(CHAOS_IL2CPP_INTPTR /*assembly*/) noexcept {
    static CHAOS_IL2CPP_INTPTR s_empty[1] = {0};  // AOT has no entry point — return empty marker.
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_empty);
}

// ── ParameterInfo stubs ─────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionGetOptionalCustomModifiers(CHAOS_IL2CPP_INTPTR /*param*/) noexcept {
    static CHAOS_IL2CPP_INTPTR s_empty[1] = {0};
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_empty);
}

// ── FieldInfo stubs (Phase 1.2+: ModuleRegistry Tier 0 flags) ─────────
CHAOS_IL2CPP_INT32 ChaosReflectionFieldGetIsInitOnly(CHAOS_IL2CPP_INTPTR field) noexcept {
    // Decode handle to ReflectionQueryFieldDescriptor and return IsInitOnly flag.
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryFieldDescriptor>(static_cast<FieldInfoHandle>(field));
    if (decoded == nullptr) return 0;
    return (decoded->flags & kFieldFlagIsInitOnly) ? 1 : 0;
}

CHAOS_IL2CPP_INT64 ChaosReflectionFieldGetFieldHandle(CHAOS_IL2CPP_INTPTR field) noexcept {
    // The field parameter is already a FieldInfoHandle (tagged pointer to
    // ReflectionQueryFieldDescriptor). RuntimeFieldHandle expects this handle value.
    return static_cast<CHAOS_IL2CPP_INT64>(field);
}

CHAOS_IL2CPP_INT32 ChaosReflectionFieldGetIsLiteral(CHAOS_IL2CPP_INTPTR field) noexcept {
    // Decode handle to ReflectionQueryFieldDescriptor and return IsLiteral flag.
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryFieldDescriptor>(static_cast<FieldInfoHandle>(field));
    if (decoded == nullptr) return 0;
    return (decoded->flags & kFieldFlagIsLiteral) ? 1 : 0;
}

// ── PropertyInfo stubs (Phase 1.2+: EEClass property flags) ───────────
CHAOS_IL2CPP_INTPTR ChaosReflectionPropertyGetIndexParameters(CHAOS_IL2CPP_INTPTR /*prop*/) noexcept {
    static CHAOS_IL2CPP_INTPTR s_empty[1] = {0};
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_empty);
}

CHAOS_IL2CPP_INT32 ChaosReflectionPropertyGetCanRead(CHAOS_IL2CPP_INTPTR prop) noexcept {
    // Decode handle to ReflectionQueryPropertyDescriptor and return CanRead flag.
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryPropertyDescriptor>(static_cast<PropertyInfoHandle>(prop));
    if (decoded == nullptr) return 0;
    return (decoded->flags & kPropertyFlagCanRead) ? 1 : 0;
}

CHAOS_IL2CPP_INT32 ChaosReflectionPropertyGetCanWrite(CHAOS_IL2CPP_INTPTR prop) noexcept {
    // Decode handle to ReflectionQueryPropertyDescriptor and return CanWrite flag.
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryPropertyDescriptor>(static_cast<PropertyInfoHandle>(prop));
    if (decoded == nullptr) return 0;
    return (decoded->flags & kPropertyFlagCanWrite) ? 1 : 0;
}

// ── AssemblyName stubs ──────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyNameGetCultureInfo(CHAOS_IL2CPP_INTPTR /*name*/) noexcept {
    return 0;  // Invariant culture = nullptr/0. Non-invariant culture deferred to Phase 3+.
}

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyNameGetVersion(CHAOS_IL2CPP_INTPTR name) noexcept {
    // name is the image_name_utf8 pointer returned by ChaosReflectionGetAssemblyName.
    // Scan all registered modules for a matching image descriptor, read the version
    // fields from the descriptor, format as "X.Y.Z.W", and return an intern'd string id.
    if (name == 0) return 0;

    const char* image_name = reinterpret_cast<const char*>(name);
    const uint32_t mod_count = GetModuleCount();
    for (uint32_t mid = 0; mid < mod_count; mid++) {
        const auto* mod = GetModuleByIndex(mid);
        if (mod == nullptr || mod->image == nullptr) continue;
        if (std::strcmp(mod->image->image_name_utf8, image_name) != 0) continue;

        // Found matching image — build version string from descriptor fields
        char ver_buf[64];
        auto result = fmt::format_to_n(ver_buf, sizeof(ver_buf) - 1, "{}.{}.{}.{}",
            static_cast<unsigned>(mod->image->version_major),
            static_cast<unsigned>(mod->image->version_minor),
            static_cast<unsigned>(mod->image->version_build),
            static_cast<unsigned>(mod->image->version_revision));
        auto id = string_table::Intern(ver_buf, static_cast<uint32_t>(result.size));
        return static_cast<CHAOS_IL2CPP_INTPTR>(id | CHAOS_STRING_ID_TAG);
    }

    // Fallback: CoreLib
    {
        const auto* corelib = &aot_metadata::kImageCoreLib;
        if (std::strcmp(corelib->image_name_utf8, image_name) == 0) {
            char ver_buf[64];
            auto result = fmt::format_to_n(ver_buf, sizeof(ver_buf) - 1, "{}.{}.{}.{}",
                static_cast<unsigned>(corelib->version_major),
                static_cast<unsigned>(corelib->version_minor),
                static_cast<unsigned>(corelib->version_build),
                static_cast<unsigned>(corelib->version_revision));
            auto id = string_table::Intern(ver_buf, static_cast<uint32_t>(result.size));
            return static_cast<CHAOS_IL2CPP_INTPTR>(id | CHAOS_STRING_ID_TAG);
        }
    }

    return 0;  // Unknown assembly name.
}

}  // namespace chaos::il2cpp::runtime_core
}  // extern "C"