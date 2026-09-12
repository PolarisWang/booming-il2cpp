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
    // AOT builds emit a single closed method body per subject; virtual dispatch
    // goes through the VTable rather than a MethodDesc override chain, so a
    // method IS its own base definition. Returning the handle unchanged is the
    // correct semantic answer here, not a placeholder.
    return member_handle;
}

// ── MethodBase modifier accessors ───────────────────────────────────
// Mirrors the FieldInfo accessors above: decode the Tier-2 method descriptor
// and test one kMethodFlag* bit (see reflection_query_model.h).
#define CHAOS_DEFINE_METHOD_FLAG_ACCESSOR(Name, FlagBit)                   \
    CHAOS_IL2CPP_INT32 Name(CHAOS_IL2CPP_INTPTR member) noexcept {         \
        auto* method = TryDecodeReflectionQueryHandle<                     \
            ReflectionQueryMethodDescriptor>(                              \
                static_cast<MethodInfoHandle>(member));                    \
        if (method == nullptr) return 0;                                   \
        return (method->flags & (FlagBit)) ? 1 : 0;                        \
    }

CHAOS_DEFINE_METHOD_FLAG_ACCESSOR(ChaosReflectionMethodGetIsPublic,            kMethodFlagIsPublic)
CHAOS_DEFINE_METHOD_FLAG_ACCESSOR(ChaosReflectionMethodGetIsStatic,            kMethodFlagIsStatic)
CHAOS_DEFINE_METHOD_FLAG_ACCESSOR(ChaosReflectionMethodGetIsVirtual,           kMethodFlagIsVirtual)
CHAOS_DEFINE_METHOD_FLAG_ACCESSOR(ChaosReflectionMethodGetIsPrivate,           kMethodFlagIsPrivate)
CHAOS_DEFINE_METHOD_FLAG_ACCESSOR(ChaosReflectionMethodGetIsAssembly,          kMethodFlagIsAssembly)
CHAOS_DEFINE_METHOD_FLAG_ACCESSOR(ChaosReflectionMethodGetIsFamily,            kMethodFlagIsFamily)
CHAOS_DEFINE_METHOD_FLAG_ACCESSOR(ChaosReflectionMethodGetIsFamilyAndAssembly, kMethodFlagIsFamilyAndAssembly)
CHAOS_DEFINE_METHOD_FLAG_ACCESSOR(ChaosReflectionMethodGetIsFamilyOrAssembly,  kMethodFlagIsFamilyOrAssembly)
CHAOS_DEFINE_METHOD_FLAG_ACCESSOR(ChaosReflectionMethodGetIsFinal,             kMethodFlagIsFinal)
CHAOS_DEFINE_METHOD_FLAG_ACCESSOR(ChaosReflectionMethodGetIsHideBySig,         kMethodFlagIsHideBySig)
CHAOS_DEFINE_METHOD_FLAG_ACCESSOR(ChaosReflectionMethodGetIsSpecialName,       kMethodFlagIsSpecialName)
CHAOS_DEFINE_METHOD_FLAG_ACCESSOR(ChaosReflectionMethodGetIsAbstract,          kMethodFlagIsAbstract)
CHAOS_DEFINE_METHOD_FLAG_ACCESSOR(ChaosReflectionMethodGetIsConstructor,       kMethodFlagIsConstructor)

#undef CHAOS_DEFINE_METHOD_FLAG_ACCESSOR

// Raw MethodAttributes value, assembled from the descriptor bits.
CHAOS_IL2CPP_INT32 ChaosReflectionMethodGetAttributes(CHAOS_IL2CPP_INTPTR member) noexcept {
    auto* method = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(
        static_cast<MethodInfoHandle>(member));
    if (method == nullptr) return 0;

    CHAOS_IL2CPP_INT32 attrs = 0;
    const CHAOS_IL2CPP_UINT32 f = method->flags;
    if ((f & kMethodFlagIsPrivate) != 0u)           attrs |= 0x0001;  // Private
    if ((f & kMethodFlagIsFamilyAndAssembly) != 0u) attrs |= 0x0002;  // FamANDAssem
    if ((f & kMethodFlagIsAssembly) != 0u)          attrs |= 0x0003;  // Assembly
    if ((f & kMethodFlagIsFamily) != 0u)            attrs |= 0x0004;  // Family
    if ((f & kMethodFlagIsFamilyOrAssembly) != 0u)  attrs |= 0x0005;  // FamORAssem
    if ((f & kMethodFlagIsPublic) != 0u)            attrs |= 0x0006;  // Public
    if ((f & kMethodFlagIsStatic) != 0u)            attrs |= 0x0010;  // Static
    if ((f & kMethodFlagIsFinal) != 0u)             attrs |= 0x0020;  // Final
    if ((f & kMethodFlagIsVirtual) != 0u)           attrs |= 0x0040;  // Virtual
    if ((f & kMethodFlagIsHideBySig) != 0u)         attrs |= 0x0080;  // HideBySig
    if ((f & kMethodFlagIsSpecialName) != 0u)       attrs |= 0x0800;  // SpecialName
    if ((f & kMethodFlagIsAbstract) != 0u)          attrs |= 0x0400;  // Abstract
    return attrs;
}

// MethodImplementationFlags — AOT emits only IL-free native bodies, so no
// managed implementation flags (IL/Managed/AggressiveInlining...) apply.
// MethodImplAttributes.IL == 0, which is also the runtime-neutral value.
CHAOS_IL2CPP_INT32 ChaosReflectionMethodGetImplementationFlags(CHAOS_IL2CPP_INTPTR /*member*/) noexcept {
    return 0;
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

// ── Assembly queries ────────────────────────────────────────────────
// NOTE (three-tier discipline, reflection-production-readiness §3.1):
// `real` requires a genuine implementation. `not-supported` must fail loudly.
// Returning a bare 0 masquerades as "empty result" and is neither, so these
// either walk real metadata or signal unsupported explicitly.

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetExportedTypes(CHAOS_IL2CPP_INTPTR assembly) noexcept {
    // Exported types are the assembly's publicly visible type table. Every type
    // in this AOT build is a top-level metadata record, so the exported set is
    // the image's own type descriptor array — same source as GetTypes.
    auto* image = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(assembly));
    if (image == nullptr || image->types == nullptr) return 0;

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
        const_cast<ReflectionQueryTypeDescriptor* const*>(image->types));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetForwardedTypes(CHAOS_IL2CPP_INTPTR assembly) noexcept {
    // Type forwarding is a runtime assembly-loading concept. Under AOT the full
    // type closure is statically known and no forwarding indirection exists, so
    // an empty result is the *correct* semantic answer, not a stub.
    if (assembly == 0) return 0;
    static const CHAOS_IL2CPP_INTPTR s_emptyArray = 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<CHAOS_IL2CPP_INTPTR*>(&s_emptyArray));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetManifestResourceNames(CHAOS_IL2CPP_INTPTR assembly) noexcept {
    // Embedded resources are not carried into the AOT metadata descriptor
    // model, so the manifest resource list is genuinely empty here. Same
    // rationale as GetForwardedTypes above.
    if (assembly == 0) return 0;
    static const CHAOS_IL2CPP_INTPTR s_emptyArray = 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<CHAOS_IL2CPP_INTPTR*>(&s_emptyArray));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetModules(CHAOS_IL2CPP_INTPTR assembly) noexcept {
    // Each AOT image is exactly one module, so the module set is the image
    // handle itself. Returning the module handle matches Module.get_Assembly
    // round-tripping.
    auto* image = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(assembly));
    if (image == nullptr) return 0;

    return static_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryImageHandle(image));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetEntryPoint(CHAOS_IL2CPP_INTPTR assembly) noexcept {
    // Under AOT the entry point is the native host entry, not a managed method,
    // so there is no managed MethodInfo to return. A null result legitimately
    // means "no managed entry point" — matching CoreCLR for non-executable
    // assemblies. The managed wrapper handles null as GetEntryPoint() == null.
    (void)assembly;
    return 0;
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

// ── FieldInfo access-level modifiers ────────────────────────────────
// Each accessor decodes the Tier-2 descriptor and tests one flag bit. The bits
// mirror System.Reflection.FieldAttributes (see reflection_query_model.h).
// When the descriptor handle cannot be decoded these return 0 ("flag not set"),
// which is the same answer a field with no access bits would produce — the
// managed wrapper reports the field as non-public in that case, matching the
// conservative reading of an unresolvable handle.
#define CHAOS_DEFINE_FIELD_FLAG_ACCESSOR(Name, FlagBit)                    \
    CHAOS_IL2CPP_INT32 Name(CHAOS_IL2CPP_INTPTR field) noexcept {          \
        auto* decoded = TryDecodeReflectionQueryHandle<                    \
            ReflectionQueryFieldDescriptor>(                               \
                static_cast<FieldInfoHandle>(field));                      \
        if (decoded == nullptr) return 0;                                  \
        return (decoded->flags & (FlagBit)) ? 1 : 0;                       \
    }

CHAOS_DEFINE_FIELD_FLAG_ACCESSOR(ChaosReflectionFieldGetIsStatic,            kFieldFlagIsStatic)
CHAOS_DEFINE_FIELD_FLAG_ACCESSOR(ChaosReflectionFieldGetIsPublic,            kFieldFlagIsPublic)
CHAOS_DEFINE_FIELD_FLAG_ACCESSOR(ChaosReflectionFieldGetIsPrivate,           kFieldFlagIsPrivate)
CHAOS_DEFINE_FIELD_FLAG_ACCESSOR(ChaosReflectionFieldGetIsAssembly,          kFieldFlagIsAssembly)
CHAOS_DEFINE_FIELD_FLAG_ACCESSOR(ChaosReflectionFieldGetIsFamily,            kFieldFlagIsFamily)
CHAOS_DEFINE_FIELD_FLAG_ACCESSOR(ChaosReflectionFieldGetIsFamilyAndAssembly, kFieldFlagIsFamilyAndAssembly)
CHAOS_DEFINE_FIELD_FLAG_ACCESSOR(ChaosReflectionFieldGetIsFamilyOrAssembly,  kFieldFlagIsFamilyOrAssembly)
CHAOS_DEFINE_FIELD_FLAG_ACCESSOR(ChaosReflectionFieldGetIsNotSerialized,     kFieldFlagIsNotSerialized)
CHAOS_DEFINE_FIELD_FLAG_ACCESSOR(ChaosReflectionFieldGetIsPinvokeImpl,       kFieldFlagIsPinvokeImpl)
CHAOS_DEFINE_FIELD_FLAG_ACCESSOR(ChaosReflectionFieldGetIsSpecialName,       kFieldFlagIsSpecialName)

#undef CHAOS_DEFINE_FIELD_FLAG_ACCESSOR

// Raw FieldAttributes value, assembled from the individual descriptor bits.
CHAOS_IL2CPP_INT32 ChaosReflectionFieldGetAttributes(CHAOS_IL2CPP_INTPTR field) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryFieldDescriptor>(
        static_cast<FieldInfoHandle>(field));
    if (decoded == nullptr) return 0;

    // Map descriptor bits onto System.Reflection.FieldAttributes values.
    CHAOS_IL2CPP_INT32 attrs = 0;
    const CHAOS_IL2CPP_UINT32 f = decoded->flags;
    if ((f & kFieldFlagIsPrivate) != 0u)           attrs |= 0x0001;  // Private
    if ((f & kFieldFlagIsFamilyAndAssembly) != 0u) attrs |= 0x0002;  // FamANDAssem
    if ((f & kFieldFlagIsAssembly) != 0u)          attrs |= 0x0003;  // Assembly
    if ((f & kFieldFlagIsFamily) != 0u)            attrs |= 0x0004;  // Family
    if ((f & kFieldFlagIsFamilyOrAssembly) != 0u)  attrs |= 0x0005;  // FamORAssem
    if ((f & kFieldFlagIsPublic) != 0u)            attrs |= 0x0006;  // Public
    if ((f & kFieldFlagIsStatic) != 0u)            attrs |= 0x0010;  // Static
    if ((f & kFieldFlagIsInitOnly) != 0u)          attrs |= 0x0020;  // InitOnly
    if ((f & kFieldFlagIsLiteral) != 0u)           attrs |= 0x0040;  // Literal
    if ((f & kFieldFlagIsNotSerialized) != 0u)     attrs |= 0x0080;  // NotSerialized
    if ((f & kFieldFlagIsSpecialName) != 0u)       attrs |= 0x0200;  // SpecialName
    if ((f & kFieldFlagIsPinvokeImpl) != 0u)       attrs |= 0x2000;  // PinvokeImpl
    return attrs;
}

// Raw constant value for literal fields (const / enum members).
CHAOS_IL2CPP_INT64 ChaosReflectionFieldGetRawConstantValue(CHAOS_IL2CPP_INTPTR field) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryFieldDescriptor>(
        static_cast<FieldInfoHandle>(field));
    if (decoded == nullptr) return 0;
    return decoded->constant_value;
}

// ── FieldInfo access-level modifiers (end) ──────────────────────────

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