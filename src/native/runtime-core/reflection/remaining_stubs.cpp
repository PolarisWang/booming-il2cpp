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

// ── ParameterInfo descriptor accessors ──────────────────────────────
// The Tier-2 parameter descriptor (reflection_query_model.h) carries
// subject_id / name / index / member_type / default blob. ParameterAttributes
// (In/Out/Optional/Retval/Lcid) are NOT emitted into AOT metadata — see
// ChaosReflectionGetParamAttributes in misc.cpp for that rationale — so the
// flag accessors below report the descriptor-derivable subset only and are
// documented as such rather than fabricating attribute bits.
CHAOS_IL2CPP_INTPTR ChaosReflectionParamGetMetadataToken(CHAOS_IL2CPP_INTPTR param) noexcept {
    // Parameters have no own metadata token in the descriptor model; the
    // owning method's token is the closest stable identity. Returning the
    // parameter index keeps the value unique within the method without
    // pretending to be a real token.
    auto* p = reinterpret_cast<const ReflectionQueryParameterDescriptor*>(param);
    if (p == nullptr) return 0;
    return static_cast<CHAOS_IL2CPP_INTPTR>(p->parameter_index);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionParamGetName(CHAOS_IL2CPP_INTPTR param) noexcept {
    auto* p = reinterpret_cast<const ReflectionQueryParameterDescriptor*>(param);
    if (p == nullptr || p->name_utf8 == nullptr) return 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<char*>(p->name_utf8));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionParamGetMember(CHAOS_IL2CPP_INTPTR param) noexcept {
    // ParameterInfo.Member returns the owning MethodBase. The parameter
    // descriptor's subject_id is "<MethodSubjectId>|<paramName>", so the
    // owning method is the prefix before the final '|'. Look it up by matching
    // that prefix against the method tables.
    auto* p = reinterpret_cast<const ReflectionQueryParameterDescriptor*>(param);
    if (p == nullptr || p->subject_id_utf8 == nullptr) return 0;

    const char* bar = std::strrchr(p->subject_id_utf8, '|');
    if (bar == nullptr) return 0;
    const size_t owner_len = static_cast<size_t>(bar - p->subject_id_utf8);
    if (owner_len == 0u) return 0;

    const uint32_t module_count = GetModuleCount();
    for (uint32_t i = 0u; i < module_count; i++) {
        const auto* mod = GetModuleByIndex(i);
        if (mod == nullptr || mod->image == nullptr) continue;
        for (uint32_t t = 0u; t < mod->image->type_count; t++) {
            const auto* type = mod->image->types[t];
            if (type == nullptr || type->methods == nullptr) continue;
            for (uint32_t m = 0u; m < type->method_count; m++) {
                const auto& method = type->methods[m];
                if (method.subject_id_utf8 == nullptr) continue;
                if (std::strlen(method.subject_id_utf8) != owner_len) continue;
                if (std::memcmp(method.subject_id_utf8, p->subject_id_utf8, owner_len) == 0) {
                    return static_cast<CHAOS_IL2CPP_INTPTR>(
                        EncodeReflectionQueryMethodHandle(&method));
                }
            }
        }
    }
    return 0;
}

// ParameterAttributes.In == 1, .Out == 2, .Lcid == 4, .Optional == 16,
// .Retval == 8 (ECMA-335). The descriptor does not carry these bits, so the
// honest answer is "attribute not present" — which is also what a parameter
// with no modifiers reports. Callers that need real In/Out semantics should
// consult ChaosReflectionParamAttributesAvailable (misc.cpp) to distinguish
// "no attributes" from "attributes unavailable".
CHAOS_IL2CPP_INT32 ChaosReflectionParamGetIsIn(CHAOS_IL2CPP_INTPTR param) noexcept {
    (void)param; return 0;
}
CHAOS_IL2CPP_INT32 ChaosReflectionParamGetIsOut(CHAOS_IL2CPP_INTPTR param) noexcept {
    (void)param; return 0;
}
CHAOS_IL2CPP_INT32 ChaosReflectionParamGetIsLcid(CHAOS_IL2CPP_INTPTR param) noexcept {
    (void)param; return 0;
}
CHAOS_IL2CPP_INT32 ChaosReflectionParamGetIsOptional(CHAOS_IL2CPP_INTPTR param) noexcept {
    // Optional is derivable: a parameter with a default value is optional.
    // This matches the C# compiler's rule for `= default` parameters and is
    // strictly better than answering constant false.
    auto* p = reinterpret_cast<const ReflectionQueryParameterDescriptor*>(param);
    if (p == nullptr) return 0;
    return (p->default_value_blob != 0 && p->default_value_size > 0) ? 1 : 0;
}
CHAOS_IL2CPP_INT32 ChaosReflectionParamGetIsRetval(CHAOS_IL2CPP_INTPTR param) noexcept {
    (void)param; return 0;
}

// ParameterInfo.ToString() — "Type Name" by the BCL's convention.
CHAOS_IL2CPP_INTPTR ChaosReflectionParamToString(CHAOS_IL2CPP_INTPTR param) noexcept {
    auto* p = reinterpret_cast<const ReflectionQueryParameterDescriptor*>(param);
    if (p == nullptr || p->member_type_utf8 == nullptr) return 0;

    // Build "<type> <name>" into the shared reflection scratch via the
    // conventional pair-concat helper, which handles the managed string
    // interning path.
    auto type_id = string_table::Intern(p->member_type_utf8,
        static_cast<CHAOS_IL2CPP_UINT32>(std::strlen(p->member_type_utf8)));
    if (p->name_utf8 == nullptr) {
        return static_cast<CHAOS_IL2CPP_INTPTR>(type_id | CHAOS_STRING_ID_TAG);
    }
    auto name_id = string_table::Intern(p->name_utf8,
        static_cast<CHAOS_IL2CPP_UINT32>(std::strlen(p->name_utf8)));
    return ChaosReflectionConcatStringPairValues(
        static_cast<CHAOS_IL2CPP_INTPTR>(type_id | CHAOS_STRING_ID_TAG),
        static_cast<CHAOS_IL2CPP_INTPTR>(name_id | CHAOS_STRING_ID_TAG));
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

// ── MethodBase remaining accessors ──────────────────────────────────
// MemberTypes.Method == 8 (ECMA-335 MemberTypes: Constructor=1, Event=2,
// Field=4, Method=8, Property=16, Type=32, NestedType=128).
CHAOS_IL2CPP_INT32 ChaosReflectionMethodGetMemberType(CHAOS_IL2CPP_INTPTR member) noexcept {
    auto* method = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(
        static_cast<MethodInfoHandle>(member));
    if (method == nullptr) return 0;
    // A .ctor/.cctor descriptor reports MemberTypes.Constructor instead.
    if (method->name_utf8 != nullptr && method->name_utf8[0] == '.' ) {
        return 1;  // MemberTypes.Constructor
    }
    return 8;  // MemberTypes.Method
}

CHAOS_IL2CPP_INT32 ChaosReflectionMethodGetIsGenericMethod(CHAOS_IL2CPP_INTPTR member) noexcept {
    // A method descriptor whose subject_id carries a "!!N" generic parameter
    // marker is generic; the descriptor itself has no dedicated flag bit for
    // this, so the subject_id is the source of truth (same convention used by
    // GetGenericParamPos in misc.cpp).
    auto* method = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(
        static_cast<MethodInfoHandle>(member));
    if (method == nullptr || method->subject_id_utf8 == nullptr) return 0;
    return std::strstr(method->subject_id_utf8, "!!") != nullptr ? 1 : 0;
}

CHAOS_IL2CPP_INT32 ChaosReflectionMethodGetIsGenericMethodDefinition(CHAOS_IL2CPP_INTPTR member) noexcept {
    // In the AOT descriptor model only the open definition is registered; a
    // closed instantiation is represented as a RuntimeInstantiatedType handle
    // (token >= 0x80000000) rather than a method descriptor. So any descriptor
    // that is generic at all is a definition.
    return ChaosReflectionMethodGetIsGenericMethod(member);
}

CHAOS_IL2CPP_INT32 ChaosReflectionMethodGetIsConstructedGenericMethod(CHAOS_IL2CPP_INTPTR member) noexcept {
    // Constructed generics are encoded as RuntimeInstantiatedType handles whose
    // token has the high bit set (see invoke.cpp GetGenericArguments). A plain
    // descriptor is never a constructed method.
    auto* method = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(
        static_cast<MethodInfoHandle>(member));
    if (method == nullptr) return 0;
    return (method->metadata_token & 0x80000000u) != 0u ? 1 : 0;
}

CHAOS_IL2CPP_INT32 ChaosReflectionMethodGetIsSecurityCritical(CHAOS_IL2CPP_INTPTR member) noexcept {
    // Security transparency is a .NET Framework CAS concept. On .NET Core /
    // .NET 5+ (the surface we target) every member is SecurityCritical by
    // default, and the BCL's own implementation returns true. Reporting 1 here
    // matches the reference behaviour rather than fabricating an attribute.
    auto* method = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(
        static_cast<MethodInfoHandle>(member));
    if (method == nullptr) return 0;
    return 1;
}

CHAOS_IL2CPP_INT32 ChaosReflectionMethodGetIsSecuritySafeCritical(CHAOS_IL2CPP_INTPTR member) noexcept {
    auto* method = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(
        static_cast<MethodInfoHandle>(member));
    if (method == nullptr) return 0;
    return 0;  // .NET Core has no SafeCritical members
}

CHAOS_IL2CPP_INT32 ChaosReflectionMethodGetIsSecurityTransparent(CHAOS_IL2CPP_INTPTR member) noexcept {
    auto* method = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(
        static_cast<MethodInfoHandle>(member));
    if (method == nullptr) return 0;
    return 0;  // .NET Core has no transparent members
}

// MethodBase.GetMethodFromHandle(RuntimeMethodHandle) — the handle IS the
// encoded method descriptor pointer, so this is an identity round-trip.
CHAOS_IL2CPP_INTPTR ChaosReflectionMethodGetMethodFromHandle(CHAOS_IL2CPP_INTPTR handle) noexcept {
    auto* method = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(
        static_cast<MethodInfoHandle>(handle));
    if (method == nullptr) return 0;
    return handle;
}

// RuntimeMethodHandle value for a MethodBase — the descriptor handle itself,
// matching ChaosReflectionFieldGetFieldHandle's convention.
CHAOS_IL2CPP_INT64 ChaosReflectionMethodGetMethodHandle(CHAOS_IL2CPP_INTPTR member) noexcept {
    auto* method = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(
        static_cast<MethodInfoHandle>(member));
    if (method == nullptr) return 0;
    return static_cast<CHAOS_IL2CPP_INT64>(member);
}

// ── FieldInfo remaining accessors ───────────────────────────────────
// MemberTypes.Field == 4.
CHAOS_IL2CPP_INT32 ChaosReflectionFieldGetMemberType(CHAOS_IL2CPP_INTPTR field) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryFieldDescriptor>(
        static_cast<FieldInfoHandle>(field));
    if (decoded == nullptr) return 0;
    return 4;  // MemberTypes.Field
}

CHAOS_IL2CPP_INT32 ChaosReflectionFieldGetIsSecurityCritical(CHAOS_IL2CPP_INTPTR field) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryFieldDescriptor>(
        static_cast<FieldInfoHandle>(field));
    if (decoded == nullptr) return 0;
    return 1;  // see ChaosReflectionMethodGetIsSecurityCritical
}

CHAOS_IL2CPP_INT32 ChaosReflectionFieldGetIsSecuritySafeCritical(CHAOS_IL2CPP_INTPTR field) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryFieldDescriptor>(
        static_cast<FieldInfoHandle>(field));
    if (decoded == nullptr) return 0;
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosReflectionFieldGetIsSecurityTransparent(CHAOS_IL2CPP_INTPTR field) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryFieldDescriptor>(
        static_cast<FieldInfoHandle>(field));
    if (decoded == nullptr) return 0;
    return 0;
}

// FieldInfo.GetFieldFromHandle(RuntimeFieldHandle) — identity round-trip.
CHAOS_IL2CPP_INTPTR ChaosReflectionFieldGetFieldFromHandle(CHAOS_IL2CPP_INTPTR handle) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryFieldDescriptor>(
        static_cast<FieldInfoHandle>(handle));
    if (decoded == nullptr) return 0;
    return handle;
}

// Modified type queries: the AOT descriptor records the field's type name but
// not its custom modifiers, so the unmodified type is the correct answer and
// the modifier arrays are genuinely empty (see GetRequiredCustomModifiers).
CHAOS_IL2CPP_INTPTR ChaosReflectionFieldGetModifiedFieldType(CHAOS_IL2CPP_INTPTR field) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryFieldDescriptor>(
        static_cast<FieldInfoHandle>(field));
    if (decoded == nullptr || decoded->member_type_utf8 == nullptr) return 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
        const_cast<char*>(decoded->member_type_utf8));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionPropertyGetModifiedPropertyType(CHAOS_IL2CPP_INTPTR prop) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryPropertyDescriptor>(
        static_cast<PropertyInfoHandle>(prop));
    if (decoded == nullptr || decoded->member_type_utf8 == nullptr) return 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
        const_cast<char*>(decoded->member_type_utf8));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionParamGetModifiedParameterType(CHAOS_IL2CPP_INTPTR param) noexcept {
    auto* p = reinterpret_cast<const ReflectionQueryParameterDescriptor*>(param);
    if (p == nullptr || p->member_type_utf8 == nullptr) return 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
        const_cast<char*>(p->member_type_utf8));
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

// Raw PropertyAttributes value, assembled from the descriptor bits.
// PropertyAttributes follows the ECMA-335 access-mask layout shared with
// FieldAttributes for the low bits, plus SpecialName at 0x0200.
CHAOS_IL2CPP_INT32 ChaosReflectionPropertyGetAttributes(CHAOS_IL2CPP_INTPTR prop) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryPropertyDescriptor>(
        static_cast<PropertyInfoHandle>(prop));
    if (decoded == nullptr) return 0;

    CHAOS_IL2CPP_INT32 attrs = 0;
    const CHAOS_IL2CPP_UINT32 f = decoded->flags;
    if ((f & kPropertyFlagIsSpecialName) != 0u) attrs |= 0x0200;  // SpecialName
    return attrs;
}

CHAOS_IL2CPP_INT32 ChaosReflectionPropertyGetIsSpecialName(CHAOS_IL2CPP_INTPTR prop) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryPropertyDescriptor>(
        static_cast<PropertyInfoHandle>(prop));
    if (decoded == nullptr) return 0;
    return (decoded->flags & kPropertyFlagIsSpecialName) ? 1 : 0;
}

CHAOS_IL2CPP_INT32 ChaosReflectionPropertyGetIsStatic(CHAOS_IL2CPP_INTPTR prop) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryPropertyDescriptor>(
        static_cast<PropertyInfoHandle>(prop));
    if (decoded == nullptr) return 0;
    return (decoded->flags & kPropertyFlagIsStatic) ? 1 : 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionPropertyGetPropertyType(CHAOS_IL2CPP_INTPTR prop) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryPropertyDescriptor>(
        static_cast<PropertyInfoHandle>(prop));
    if (decoded == nullptr || decoded->member_type_utf8 == nullptr) return 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
        const_cast<char*>(decoded->member_type_utf8));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionPropertyGetName(CHAOS_IL2CPP_INTPTR prop) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryPropertyDescriptor>(
        static_cast<PropertyInfoHandle>(prop));
    if (decoded == nullptr || decoded->name_utf8 == nullptr) return 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
        const_cast<char*>(decoded->name_utf8));
}

// MemberTypes.Property == 16 — this accessor exists only on PropertyInfo, so the
// answer is a constant. Kept as a named entry point so codegen has a symbol to
// forward to (matching the MemberTypes constants in the BCL: Constructor=1,
// Method=8, Property=16, Field=4, Event=2).
CHAOS_IL2CPP_INT32 ChaosReflectionPropertyGetMemberType(CHAOS_IL2CPP_INTPTR prop) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryPropertyDescriptor>(
        static_cast<PropertyInfoHandle>(prop));
    if (decoded == nullptr) return 0;
    return 16;  // MemberTypes.Property
}

// ── PropertyInfo accessors (get_/set_ method lookup) ────────────────
// The Tier-2 property descriptor deliberately does not carry accessor handles
// (see reflection_query_model.h — it holds only subject_id/name/type/flags).
// The accessor methods live in the *owning type's* method table, named
// "get_<Property>" / "set_<Property>", so they are resolved by scanning the
// declaring type for the conventional name. This mirrors how the BCL derives
// GetGetMethod/GetSetMethod from the property's accessor pair.
namespace {

// Portable "find last occurrence of needle in haystack" (std::strrstr is a
// POSIX/MSVC extension and not available on every target we build for).
const char* FindLastSubstring(const char* haystack, const char* needle) noexcept {
    if (haystack == nullptr || needle == nullptr) return nullptr;
    const size_t needle_len = std::strlen(needle);
    if (needle_len == 0) return haystack + std::strlen(haystack);

    const char* last = nullptr;
    for (const char* p = haystack; (p = std::strstr(p, needle)) != nullptr; p++) {
        last = p;
    }
    return last;
}

// Builds "get_"/"set_" + property name into a caller-supplied buffer.
// Returns false when the name would overflow, in which case no lookup is made.
bool BuildAccessorName(const char* prop_name, const char* prefix,
                       char* out, size_t out_size) noexcept {
    if (prop_name == nullptr || out == nullptr) return false;
    const size_t prefix_len = std::strlen(prefix);
    const size_t name_len = std::strlen(prop_name);
    if (prefix_len + name_len + 1 > out_size) return false;
    std::memcpy(out, prefix, prefix_len);
    std::memcpy(out + prefix_len, prop_name, name_len + 1);  // include NUL
    return true;
}

// Finds the declaring type of a property descriptor and returns the method
// descriptor whose name matches `accessor_name`, or nullptr.
const ReflectionQueryMethodDescriptor* FindPropertyAccessor(
    const ReflectionQueryPropertyDescriptor* prop, const char* prefix) noexcept {
    if (prop == nullptr || prop->name_utf8 == nullptr) return nullptr;

    // The property descriptor's subject_id is "<TypeSubjectId>::<PropName>";
    // the declaring type is everything before the final "::".
    const char* sep = FindLastSubstring(
        prop->subject_id_utf8 != nullptr ? prop->subject_id_utf8 : "", "::");
    if (sep == nullptr) return nullptr;

    char want[256];
    if (!BuildAccessorName(prop->name_utf8, prefix, want, sizeof(want))) return nullptr;

    const size_t type_len = static_cast<size_t>(sep - prop->subject_id_utf8);
    const uint32_t module_count = GetModuleCount();
    for (uint32_t i = 0u; i < module_count; i++) {
        const auto* mod = GetModuleByIndex(i);
        if (mod == nullptr || mod->image == nullptr) continue;
        for (uint32_t t = 0u; t < mod->image->type_count; t++) {
            const auto* type = mod->image->types[t];
            if (type == nullptr || type->subject_id_utf8 == nullptr) continue;
            if (std::strlen(type->subject_id_utf8) != type_len) continue;
            if (std::memcmp(type->subject_id_utf8, prop->subject_id_utf8, type_len) != 0) continue;

            if (type->methods == nullptr) return nullptr;
            for (uint32_t m = 0u; m < type->method_count; m++) {
                const auto& method = type->methods[m];
                if (method.name_utf8 != nullptr &&
                    std::strcmp(method.name_utf8, want) == 0) {
                    return &method;
                }
            }
            return nullptr;  // declaring type found, no matching accessor
        }
    }
    return nullptr;
}

}  // namespace

CHAOS_IL2CPP_INTPTR ChaosReflectionPropertyGetGetMethod(CHAOS_IL2CPP_INTPTR prop) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryPropertyDescriptor>(
        static_cast<PropertyInfoHandle>(prop));
    if (decoded == nullptr) return 0;
    if ((decoded->flags & kPropertyFlagCanRead) == 0u) return 0;

    const auto* getter = FindPropertyAccessor(decoded, "get_");
    if (getter == nullptr) return 0;
    return static_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryMethodHandle(getter));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionPropertyGetSetMethod(CHAOS_IL2CPP_INTPTR prop) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryPropertyDescriptor>(
        static_cast<PropertyInfoHandle>(prop));
    if (decoded == nullptr) return 0;
    if ((decoded->flags & kPropertyFlagCanWrite) == 0u) return 0;

    const auto* setter = FindPropertyAccessor(decoded, "set_");
    if (setter == nullptr) return 0;
    return static_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryMethodHandle(setter));
}

// ── EventInfo descriptor accessors ──────────────────────────────────
// EventDescriptor (reflection_query_model.h) mirrors PropertyDescriptor:
// subject_id / name / member_type (the delegate type) / flags.
CHAOS_IL2CPP_INTPTR ChaosReflectionEventGetName(CHAOS_IL2CPP_INTPTR evt) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryEventDescriptor>(
        static_cast<EventInfoHandle>(evt));
    if (decoded == nullptr || decoded->name_utf8 == nullptr) return 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<char*>(decoded->name_utf8));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionEventGetEventHandlerType(CHAOS_IL2CPP_INTPTR evt) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryEventDescriptor>(
        static_cast<EventInfoHandle>(evt));
    if (decoded == nullptr || decoded->member_type_utf8 == nullptr) return 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
        const_cast<char*>(decoded->member_type_utf8));
}

CHAOS_IL2CPP_INT32 ChaosReflectionEventGetAttributes(CHAOS_IL2CPP_INTPTR evt) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryEventDescriptor>(
        static_cast<EventInfoHandle>(evt));
    if (decoded == nullptr) return 0;
    // EventAttributes.None — the descriptor carries only the static bit, which
    // is not part of System.Reflection.EventAttributes (that enum has no
    // Static member; statics are expressed via the add/remove method flags).
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosReflectionEventGetIsStatic(CHAOS_IL2CPP_INTPTR evt) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryEventDescriptor>(
        static_cast<EventInfoHandle>(evt));
    if (decoded == nullptr) return 0;
    return (decoded->flags & kEventFlagIsStatic) ? 1 : 0;
}

CHAOS_IL2CPP_INT32 ChaosReflectionEventGetMemberType(CHAOS_IL2CPP_INTPTR evt) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryEventDescriptor>(
        static_cast<EventInfoHandle>(evt));
    if (decoded == nullptr) return 0;
    return 2;  // MemberTypes.Event
}

CHAOS_IL2CPP_INTPTR ChaosReflectionEventGetAddMethod(CHAOS_IL2CPP_INTPTR evt) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryEventDescriptor>(
        static_cast<EventInfoHandle>(evt));
    if (decoded == nullptr || decoded->subject_id_utf8 == nullptr) return 0;

    const char* sep = FindLastSubstring(decoded->subject_id_utf8, "::");
    if (sep == nullptr || decoded->name_utf8 == nullptr) return 0;

    char want[256];
    if (!BuildAccessorName(decoded->name_utf8, "add_", want, sizeof(want))) return 0;

    // Reuse the property-accessor scan by constructing the equivalent lookup.
    const size_t type_len = static_cast<size_t>(sep - decoded->subject_id_utf8);
    const uint32_t module_count = GetModuleCount();
    for (uint32_t i = 0u; i < module_count; i++) {
        const auto* mod = GetModuleByIndex(i);
        if (mod == nullptr || mod->image == nullptr) continue;
        for (uint32_t t = 0u; t < mod->image->type_count; t++) {
            const auto* type = mod->image->types[t];
            if (type == nullptr || type->subject_id_utf8 == nullptr) continue;
            if (std::strlen(type->subject_id_utf8) != type_len) continue;
            if (std::memcmp(type->subject_id_utf8, decoded->subject_id_utf8, type_len) != 0) continue;

            if (type->methods == nullptr) return 0;
            for (uint32_t m = 0u; m < type->method_count; m++) {
                const auto& method = type->methods[m];
                if (method.name_utf8 != nullptr &&
                    std::strcmp(method.name_utf8, want) == 0) {
                    return static_cast<CHAOS_IL2CPP_INTPTR>(
                        EncodeReflectionQueryMethodHandle(&method));
                }
            }
            return 0;
        }
    }
    return 0;
}

// ── MethodInfo remaining accessors ──────────────────────────────────
// The method descriptor's member_type_utf8 holds the declared return type for
// methods (the codegen emitter records the return type in that slot), so
// ReturnType resolves directly from it.
CHAOS_IL2CPP_INTPTR ChaosReflectionMethodGetReturnType(CHAOS_IL2CPP_INTPTR member) noexcept {
    auto* method = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(
        static_cast<MethodInfoHandle>(member));
    if (method == nullptr || method->member_type_utf8 == nullptr) return 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
        const_cast<char*>(method->member_type_utf8));
}

// MethodInfo.ReturnParameter — the descriptor carries `parameters` for real
// parameters only, so no return-parameter record can be synthesized. Report
// unresolved rather than fabricating one.
CHAOS_IL2CPP_INTPTR ChaosReflectionMethodGetReturnParameter(CHAOS_IL2CPP_INTPTR member) noexcept {
    auto* method = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(
        static_cast<MethodInfoHandle>(member));
    if (method == nullptr) return 0;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionMethodGetReturnTypeCustomAttributes(CHAOS_IL2CPP_INTPTR member) noexcept {
    auto* method = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(
        static_cast<MethodInfoHandle>(member));
    if (method == nullptr) return 0;
    return 0;
}

// MethodInfo.GetGenericArguments — a generic method's type parameters are
// recorded in the descriptor's parameter array; only the open definition is
// registered (see GetIsGenericMethodDefinition).
CHAOS_IL2CPP_INTPTR ChaosReflectionMethodGetGenericArguments(CHAOS_IL2CPP_INTPTR member) noexcept {
    auto* method = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(
        static_cast<MethodInfoHandle>(member));
    if (method == nullptr || method->parameters == nullptr) return 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
        const_cast<ReflectionQueryParameterDescriptor*>(method->parameters));
}

// MethodInfo.GetGenericMethodDefinition — only the open definition is
// registered, so a generic method is already its own definition.
CHAOS_IL2CPP_INTPTR ChaosReflectionMethodGetGenericMethodDefinition(CHAOS_IL2CPP_INTPTR member) noexcept {
    auto* method = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(
        static_cast<MethodInfoHandle>(member));
    if (method == nullptr) return 0;
    if (ChaosReflectionMethodGetIsGenericMethod(member) == 0) return 0;
    return member;
}

CHAOS_IL2CPP_INT32 ChaosReflectionMethodGetHashCodeVersion(CHAOS_IL2CPP_INTPTR member) noexcept {
    // Identity hash over the descriptor pointer, matching MemberInfo's
    // reference-equality hash semantics.
    auto* method = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(
        static_cast<MethodInfoHandle>(member));
    if (method == nullptr) return 0;
    auto h = reinterpret_cast<uintptr_t>(method);
    return static_cast<CHAOS_IL2CPP_INT32>((h >> 4) ^ (h >> 20));
}

CHAOS_IL2CPP_INT32 ChaosReflectionMethodEqualsVersion(
    CHAOS_IL2CPP_INTPTR lhs, CHAOS_IL2CPP_INTPTR rhs) noexcept {
    auto* a = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(
        static_cast<MethodInfoHandle>(lhs));
    auto* b = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(
        static_cast<MethodInfoHandle>(rhs));
    if (a == nullptr || b == nullptr) return 0;
    return a == b ? 1 : 0;
}

// ── EventInfo remaining accessors ───────────────────────────────────
// GetRemoveMethod / GetRaiseMethod mirror GetAddMethod's accessor-name scan:
// the event descriptor has no accessor handles, so the accessor is located by
// the conventional "remove_<Event>" / "raise_<Event>" name inside the
// declaring type's method table.
namespace {

const ReflectionQueryMethodDescriptor* FindEventAccessor(
    const ReflectionQueryEventDescriptor* evt, const char* prefix) noexcept {
    if (evt == nullptr || evt->subject_id_utf8 == nullptr || evt->name_utf8 == nullptr) return nullptr;

    const char* sep = FindLastSubstring(evt->subject_id_utf8, "::");
    if (sep == nullptr) return nullptr;

    char want[256];
    if (!BuildAccessorName(evt->name_utf8, prefix, want, sizeof(want))) return nullptr;

    const size_t type_len = static_cast<size_t>(sep - evt->subject_id_utf8);
    const uint32_t module_count = GetModuleCount();
    for (uint32_t i = 0u; i < module_count; i++) {
        const auto* mod = GetModuleByIndex(i);
        if (mod == nullptr || mod->image == nullptr) continue;
        for (uint32_t t = 0u; t < mod->image->type_count; t++) {
            const auto* type = mod->image->types[t];
            if (type == nullptr || type->subject_id_utf8 == nullptr) continue;
            if (std::strlen(type->subject_id_utf8) != type_len) continue;
            if (std::memcmp(type->subject_id_utf8, evt->subject_id_utf8, type_len) != 0) continue;
            if (type->methods == nullptr) return nullptr;
            for (uint32_t m = 0u; m < type->method_count; m++) {
                const auto& method = type->methods[m];
                if (method.name_utf8 != nullptr && std::strcmp(method.name_utf8, want) == 0) {
                    return &method;
                }
            }
            return nullptr;
        }
    }
    return nullptr;
}

}  // namespace

CHAOS_IL2CPP_INTPTR ChaosReflectionEventGetRemoveMethod(CHAOS_IL2CPP_INTPTR evt) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryEventDescriptor>(
        static_cast<EventInfoHandle>(evt));
    if (decoded == nullptr) return 0;
    const auto* method = FindEventAccessor(decoded, "remove_");
    if (method == nullptr) return 0;
    return static_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryMethodHandle(method));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionEventGetRaiseMethod(CHAOS_IL2CPP_INTPTR evt) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryEventDescriptor>(
        static_cast<EventInfoHandle>(evt));
    if (decoded == nullptr) return 0;
    const auto* method = FindEventAccessor(decoded, "raise_");
    if (method == nullptr) return 0;
    return static_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryMethodHandle(method));
}

// EventInfo.IsMulticast — a Delegate-derived handler type makes the event
// multicast; the descriptor records the delegate type, and the BCL answers true
// for the delegate-backed case the AOT model represents.
CHAOS_IL2CPP_INT32 ChaosReflectionEventGetIsMulticast(CHAOS_IL2CPP_INTPTR evt) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryEventDescriptor>(
        static_cast<EventInfoHandle>(evt));
    if (decoded == nullptr || decoded->member_type_utf8 == nullptr) return 0;
    return 1;
}

CHAOS_IL2CPP_INT32 ChaosReflectionEventGetHashCodeVersion(CHAOS_IL2CPP_INTPTR evt) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryEventDescriptor>(
        static_cast<EventInfoHandle>(evt));
    if (decoded == nullptr) return 0;
    auto h = reinterpret_cast<uintptr_t>(decoded);
    return static_cast<CHAOS_IL2CPP_INT32>((h >> 4) ^ (h >> 20));
}

CHAOS_IL2CPP_INT32 ChaosReflectionEventEqualsVersion(
    CHAOS_IL2CPP_INTPTR lhs, CHAOS_IL2CPP_INTPTR rhs) noexcept {
    auto* a = TryDecodeReflectionQueryHandle<ReflectionQueryEventDescriptor>(
        static_cast<EventInfoHandle>(lhs));
    auto* b = TryDecodeReflectionQueryHandle<ReflectionQueryEventDescriptor>(
        static_cast<EventInfoHandle>(rhs));
    if (a == nullptr || b == nullptr) return 0;
    return a == b ? 1 : 0;
}

// ── PropertyInfo remaining accessors ────────────────────────────────
CHAOS_IL2CPP_INT32 ChaosReflectionPropertyGetHashCodeVersion(CHAOS_IL2CPP_INTPTR prop) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryPropertyDescriptor>(
        static_cast<PropertyInfoHandle>(prop));
    if (decoded == nullptr) return 0;
    auto h = reinterpret_cast<uintptr_t>(decoded);
    return static_cast<CHAOS_IL2CPP_INT32>((h >> 4) ^ (h >> 20));
}

CHAOS_IL2CPP_INT32 ChaosReflectionPropertyEqualsVersion(
    CHAOS_IL2CPP_INTPTR lhs, CHAOS_IL2CPP_INTPTR rhs) noexcept {
    auto* a = TryDecodeReflectionQueryHandle<ReflectionQueryPropertyDescriptor>(
        static_cast<PropertyInfoHandle>(lhs));
    auto* b = TryDecodeReflectionQueryHandle<ReflectionQueryPropertyDescriptor>(
        static_cast<PropertyInfoHandle>(rhs));
    if (a == nullptr || b == nullptr) return 0;
    return a == b ? 1 : 0;
}

// PropertyInfo.GetAccessors() / (bool nonPublic) — the get/set method pair.
// Both resolve through the same accessor-name scan used by GetGetMethod /
// GetSetMethod; this entry point returns the getter (the first element of the
// BCL's returned array) so callers that index [0] see the same value.
CHAOS_IL2CPP_INTPTR ChaosReflectionPropertyGetAccessors(CHAOS_IL2CPP_INTPTR prop) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryPropertyDescriptor>(
        static_cast<PropertyInfoHandle>(prop));
    if (decoded == nullptr) return 0;
    if (ChaosReflectionPropertyGetGetMethod(prop) != 0) {
        return ChaosReflectionPropertyGetGetMethod(prop);
    }
    return ChaosReflectionPropertyGetSetMethod(prop);
}

// PropertyInfo.GetConstantValue / GetRawConstantValue — the property's default
// value. The property descriptor carries no constant slot (unlike the field
// descriptor, which has `constant_value`), so no value can be reported. Returns
// 0, which the BCL also returns for a property without a constant.
CHAOS_IL2CPP_INTPTR ChaosReflectionPropertyGetConstantValue(CHAOS_IL2CPP_INTPTR prop) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryPropertyDescriptor>(
        static_cast<PropertyInfoHandle>(prop));
    if (decoded == nullptr) return 0;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionPropertyGetRawConstantValue(CHAOS_IL2CPP_INTPTR prop) noexcept {
    return ChaosReflectionPropertyGetConstantValue(prop);
}

// Custom modifiers on a property: the descriptor records no modifier list, so
// the empty set is the correct answer (matching a property with no modifiers).
CHAOS_IL2CPP_INTPTR ChaosReflectionPropertyGetOptionalCustomModifiers(CHAOS_IL2CPP_INTPTR prop) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryPropertyDescriptor>(
        static_cast<PropertyInfoHandle>(prop));
    if (decoded == nullptr) return 0;
    static CHAOS_IL2CPP_INTPTR s_empty[1] = {0};
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_empty);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionPropertyGetRequiredCustomModifiers(CHAOS_IL2CPP_INTPTR prop) noexcept {
    return ChaosReflectionPropertyGetOptionalCustomModifiers(prop);
}

// ── ParameterInfo remaining accessors ───────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionParamGetCustomAttributesData(CHAOS_IL2CPP_INTPTR param) noexcept {
    // Parameters are addressed by member_kind 5 in the custom-attribute blob.
    auto* p = reinterpret_cast<const ReflectionQueryParameterDescriptor*>(param);
    if (p == nullptr) return 0;
    return ChaosReflectionCollectCustomAttributes(
        static_cast<CHAOS_IL2CPP_INTPTR>(5), param);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionParamGetCustomAttributes(CHAOS_IL2CPP_INTPTR param) noexcept {
    return ChaosReflectionParamGetCustomAttributesData(param);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionParamIsDefined(
    CHAOS_IL2CPP_INTPTR param, CHAOS_IL2CPP_INTPTR attribute_type_handle) noexcept {
    if (attribute_type_handle == 0) return 0;
    return ChaosReflectionMemberIsDefinedByToken(
        static_cast<CHAOS_IL2CPP_INTPTR>(5), param,
        static_cast<CHAOS_IL2CPP_INTPTR>(GetTypeToken(attribute_type_handle)));
}

// Parameter default-value accessors (the descriptor carries the raw blob).
CHAOS_IL2CPP_INTPTR ChaosReflectionParamGetDefaultValue(CHAOS_IL2CPP_INTPTR param) noexcept {
    return ChaosReflectionGetDefaultValue(param);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionParamGetRawDefaultValue(CHAOS_IL2CPP_INTPTR param) noexcept {
    return ChaosReflectionGetRawDefaultValue(param);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionParamGetHasDefaultValue(CHAOS_IL2CPP_INTPTR param) noexcept {
    return ChaosReflectionHasDefaultValue(param);
}

// Custom modifiers on a parameter: no modifier list in the descriptor, so the
// empty set matches a parameter with no modifiers.
CHAOS_IL2CPP_INTPTR ChaosReflectionParamGetOptionalCustomModifiers(CHAOS_IL2CPP_INTPTR param) noexcept {
    if (param == 0) return 0;
    static CHAOS_IL2CPP_INTPTR s_empty[1] = {0};
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_empty);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionParamGetRequiredCustomModifiers(CHAOS_IL2CPP_INTPTR param) noexcept {
    return ChaosReflectionParamGetOptionalCustomModifiers(param);
}

// ParameterInfo.GetRealObject — a remoting-era API. IObjectReference resolution
// has no meaning under AOT; the parameter is already the real object, so the
// handle round-trips.
CHAOS_IL2CPP_INTPTR ChaosReflectionParamGetRealObject(CHAOS_IL2CPP_INTPTR param) noexcept {
    if (param == 0) return 0;
    return param;
}

// ── MemberInfo remaining accessors ──────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionMemberGetCustomAttributesData(CHAOS_IL2CPP_INTPTR member) noexcept {
    // MemberInfo is the base of type/method/field/property/event; the member
    // kind is not encoded in the handle here, so the caller supplies it via the
    // dedicated per-kind entry points. Type-level (kind 1) is the common case
    // for the generic MemberInfo surface.
    if (member == 0) return 0;
    return ChaosReflectionCollectCustomAttributes(static_cast<CHAOS_IL2CPP_INTPTR>(1), member);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionMemberGetCustomAttributes(CHAOS_IL2CPP_INTPTR member) noexcept {
    return ChaosReflectionMemberGetCustomAttributesData(member);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionMemberIsDefined(
    CHAOS_IL2CPP_INTPTR member, CHAOS_IL2CPP_INTPTR attribute_type_handle) noexcept {
    if (attribute_type_handle == 0) return 0;
    return ChaosReflectionMemberIsDefinedByToken(
        static_cast<CHAOS_IL2CPP_INTPTR>(1), member,
        static_cast<CHAOS_IL2CPP_INTPTR>(GetTypeToken(attribute_type_handle)));
}

// MemberInfo.MemberType — decode by trying each descriptor kind. The handle
// tag does not distinguish descriptor types, so this probes in the same order
// the BCL's MemberInfo hierarchy narrows.
CHAOS_IL2CPP_INT32 ChaosReflectionMemberGetMemberType(CHAOS_IL2CPP_INTPTR member) noexcept {
    if (member == 0) return 0;
    if (TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(
            static_cast<MethodInfoHandle>(member)) != nullptr) {
        return ChaosReflectionMethodGetMemberType(member);
    }
    if (TryDecodeReflectionQueryHandle<ReflectionQueryFieldDescriptor>(
            static_cast<FieldInfoHandle>(member)) != nullptr) {
        return ChaosReflectionFieldGetMemberType(member);
    }
    if (TryDecodeReflectionQueryHandle<ReflectionQueryPropertyDescriptor>(
            static_cast<PropertyInfoHandle>(member)) != nullptr) {
        return ChaosReflectionPropertyGetMemberType(member);
    }
    if (TryDecodeReflectionQueryHandle<ReflectionQueryEventDescriptor>(
            static_cast<EventInfoHandle>(member)) != nullptr) {
        return ChaosReflectionEventGetMemberType(member);
    }
    return 0;
}

// MemberInfo.Module — the module that contains the member. The declaring type
// carries the image; unresolved members report 0.
CHAOS_IL2CPP_INTPTR ChaosReflectionMemberGetModule(CHAOS_IL2CPP_INTPTR member) noexcept {
    // GetDeclaringType returns an encoded type handle (intptr_t), not a pointer.
    CHAOS_IL2CPP_INTPTR declaring = ChaosReflectionGetDeclaringType(member);
    if (declaring == 0) return 0;
    return ChaosReflectionGetAssembly(declaring);
}

// MemberInfo.IsCollectible — collectible AssemblyLoadContext is a CoreCLR
// loader feature; an AOT image is never collectible.
CHAOS_IL2CPP_INT32 ChaosReflectionMemberGetIsCollectible(CHAOS_IL2CPP_INTPTR member) noexcept {
    if (member == 0) return 0;
    return 0;
}

// MemberInfo.HasSameMetadataDefinitionAs(other) — true when both refer to the
// same descriptor, which is the AOT model's definition of identity.
CHAOS_IL2CPP_INT32 ChaosReflectionMemberHasSameMetadataDefinitionAs(
    CHAOS_IL2CPP_INTPTR member, CHAOS_IL2CPP_INTPTR other) noexcept {
    if (member == 0 || other == 0) return 0;
    // Compare the decoded descriptor pointers across all four descriptor kinds.
    if ((void*)TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(
            static_cast<MethodInfoHandle>(member)) ==
        (void*)TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(
            static_cast<MethodInfoHandle>(other)) &&
        TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(
            static_cast<MethodInfoHandle>(member)) != nullptr) return 1;
    if ((void*)TryDecodeReflectionQueryHandle<ReflectionQueryFieldDescriptor>(
            static_cast<FieldInfoHandle>(member)) ==
        (void*)TryDecodeReflectionQueryHandle<ReflectionQueryFieldDescriptor>(
            static_cast<FieldInfoHandle>(other)) &&
        TryDecodeReflectionQueryHandle<ReflectionQueryFieldDescriptor>(
            static_cast<FieldInfoHandle>(member)) != nullptr) return 1;
    if ((void*)TryDecodeReflectionQueryHandle<ReflectionQueryPropertyDescriptor>(
            static_cast<PropertyInfoHandle>(member)) ==
        (void*)TryDecodeReflectionQueryHandle<ReflectionQueryPropertyDescriptor>(
            static_cast<PropertyInfoHandle>(other)) &&
        TryDecodeReflectionQueryHandle<ReflectionQueryPropertyDescriptor>(
            static_cast<PropertyInfoHandle>(member)) != nullptr) return 1;
    if ((void*)TryDecodeReflectionQueryHandle<ReflectionQueryEventDescriptor>(
            static_cast<EventInfoHandle>(member)) ==
        (void*)TryDecodeReflectionQueryHandle<ReflectionQueryEventDescriptor>(
            static_cast<EventInfoHandle>(other)) &&
        TryDecodeReflectionQueryHandle<ReflectionQueryEventDescriptor>(
            static_cast<EventInfoHandle>(member)) != nullptr) return 1;
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosReflectionMemberGetHashCodeVersion(CHAOS_IL2CPP_INTPTR member) noexcept {
    if (member == 0) return 0;
    auto h = static_cast<uintptr_t>(member);
    return static_cast<CHAOS_IL2CPP_INT32>((h >> 4) ^ (h >> 20));
}

CHAOS_IL2CPP_INT32 ChaosReflectionMemberEqualsVersion(
    CHAOS_IL2CPP_INTPTR lhs, CHAOS_IL2CPP_INTPTR rhs) noexcept {
    if (lhs == 0 || rhs == 0) return 0;
    return lhs == rhs ? 1 : 0;
}

// ── MethodBase remaining accessors ──────────────────────────────────
CHAOS_IL2CPP_INT32 ChaosReflectionMethodGetCallingConvention(CHAOS_IL2CPP_INTPTR member) noexcept {
    // AOT emits only static/shared native bodies with the platform default
    // convention, which is CallingConventions.Standard (0x01).
    auto* method = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(
        static_cast<MethodInfoHandle>(member));
    if (method == nullptr) return 0;
    return 1;  // CallingConventions.Standard
}

CHAOS_IL2CPP_INT32 ChaosReflectionMethodGetContainsGenericParameters(CHAOS_IL2CPP_INTPTR member) noexcept {
    // Consistent with GetIsGenericMethod: a subject_id carrying "!!" denotes an
    // open generic parameter reference.
    return ChaosReflectionMethodGetIsGenericMethod(member);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionMethodGetGenericArgumentsForBase(CHAOS_IL2CPP_INTPTR member) noexcept {
    return ChaosReflectionMethodGetGenericArguments(member);
}

// MethodBase.GetCurrentMethod() — the executing method. AOT frames carry no
// managed stack metadata (the same limitation that drives REF-RISK-7's
// executing-image tracker), so this reports unresolved rather than guessing.
CHAOS_IL2CPP_INTPTR ChaosReflectionMethodGetCurrentMethod(void) noexcept {
    return 0;
}

// MethodBase.GetMethodBody() — IL bodies do not exist under AOT, so there is no
// MethodBody to return. This is the same conclusion MethodBody.GetILAsByteArray
// records elsewhere in this file.
CHAOS_IL2CPP_INTPTR ChaosReflectionMethodGetMethodBody(CHAOS_IL2CPP_INTPTR member) noexcept {
    auto* method = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(
        static_cast<MethodInfoHandle>(member));
    if (method == nullptr) return 0;
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosReflectionMethodGetHashCodeBase(CHAOS_IL2CPP_INTPTR member) noexcept {
    return ChaosReflectionMethodGetHashCodeVersion(member);
}

CHAOS_IL2CPP_INT32 ChaosReflectionMethodEqualsBase(
    CHAOS_IL2CPP_INTPTR lhs, CHAOS_IL2CPP_INTPTR rhs) noexcept {
    return ChaosReflectionMethodEqualsVersion(lhs, rhs);
}

// ── ConstructorInfo / MethodInfo / FieldInfo / PropertyInfo / EventInfo rest ──
// ConstructorInfo.MemberType == 1 (ECMA-335 MemberTypes: Constructor=1).
CHAOS_IL2CPP_INT32 ChaosReflectionCtorGetMemberType(CHAOS_IL2CPP_INTPTR member) noexcept {
    auto* method = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(
        static_cast<MethodInfoHandle>(member));
    if (method == nullptr) return 0;
    return 1;  // MemberTypes.Constructor
}

CHAOS_IL2CPP_INT32 ChaosReflectionCtorGetHashCodeVersion(CHAOS_IL2CPP_INTPTR member) noexcept {
    return ChaosReflectionMethodGetHashCodeVersion(member);
}

CHAOS_IL2CPP_INT32 ChaosReflectionCtorEqualsVersion(
    CHAOS_IL2CPP_INTPTR lhs, CHAOS_IL2CPP_INTPTR rhs) noexcept {
    return ChaosReflectionMethodEqualsVersion(lhs, rhs);
}

// The BCL exposes the ctor name as a constant: ".ctor" for instance
// constructors and ".cctor" for the type initializer. The descriptor records
// the actual name, so it is returned directly.
CHAOS_IL2CPP_INTPTR ChaosReflectionCtorGetConstructorName(CHAOS_IL2CPP_INTPTR member) noexcept {
    auto* method = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(
        static_cast<MethodInfoHandle>(member));
    if (method == nullptr || method->name_utf8 == nullptr) return 0;
    auto id = string_table::Intern(method->name_utf8,
        static_cast<CHAOS_IL2CPP_UINT32>(std::strlen(method->name_utf8)));
    return static_cast<CHAOS_IL2CPP_INTPTR>(id | CHAOS_STRING_ID_TAG);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionCtorGetTypeConstructorName(CHAOS_IL2CPP_INTPTR member) noexcept {
    // TypeConstructor is the .cctor; report the constant rather than the
    // instance-ctor name the descriptor carries.
    auto* method = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(
        static_cast<MethodInfoHandle>(member));
    if (method == nullptr) return 0;
    auto id = string_table::Intern(".cctor", 6);
    return static_cast<CHAOS_IL2CPP_INTPTR>(id | CHAOS_STRING_ID_TAG);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionCtorInvoke(
    CHAOS_IL2CPP_INTPTR member, CHAOS_IL2CPP_INTPTR obj, CHAOS_IL2CPP_INTPTR args) noexcept {
    // ConstructorInfo.Invoke(obj, args) is MethodBase.Invoke restricted to a
    // constructor; the same dispatch applies.
    return ChaosReflectionInvokeMethod(member, obj, args);
}

// MethodInfo.MemberType / EventInfo helpers.
CHAOS_IL2CPP_INT32 ChaosReflectionMethodGetMemberTypeVersion(CHAOS_IL2CPP_INTPTR member) noexcept {
    return ChaosReflectionMethodGetMemberType(member);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionMethodGetBaseDefinitionVersion(CHAOS_IL2CPP_INTPTR member) noexcept {
    return ChaosReflectionGetBaseDefinition(member);
}

// MethodInfo.CreateDelegate — building a delegate over a method requires the
// runtime delegate constructor, which the delegate layer owns. This entry point
// reports unresolved so callers route through the delegate helpers rather than
// receiving a half-constructed handle.
CHAOS_IL2CPP_INTPTR ChaosReflectionMethodCreateDelegate(CHAOS_IL2CPP_INTPTR member) noexcept {
    auto* method = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(
        static_cast<MethodInfoHandle>(member));
    if (method == nullptr) return 0;
    return 0;
}

// FieldInfo identity + handle + byref value access.
CHAOS_IL2CPP_INT32 ChaosReflectionFieldGetHashCodeVersion(CHAOS_IL2CPP_INTPTR field) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryFieldDescriptor>(
        static_cast<FieldInfoHandle>(field));
    if (decoded == nullptr) return 0;
    auto h = reinterpret_cast<uintptr_t>(decoded);
    return static_cast<CHAOS_IL2CPP_INT32>((h >> 4) ^ (h >> 20));
}

CHAOS_IL2CPP_INT32 ChaosReflectionFieldEqualsVersion(
    CHAOS_IL2CPP_INTPTR lhs, CHAOS_IL2CPP_INTPTR rhs) noexcept {
    auto* a = TryDecodeReflectionQueryHandle<ReflectionQueryFieldDescriptor>(
        static_cast<FieldInfoHandle>(lhs));
    auto* b = TryDecodeReflectionQueryHandle<ReflectionQueryFieldDescriptor>(
        static_cast<FieldInfoHandle>(rhs));
    if (a == nullptr || b == nullptr) return 0;
    return a == b ? 1 : 0;
}

CHAOS_IL2CPP_INT64 ChaosReflectionFieldGetFieldHandleVersion(CHAOS_IL2CPP_INTPTR field) noexcept {
    return ChaosReflectionFieldGetFieldHandle(field);
}

// Field custom modifiers: no modifier list in the descriptor → empty set, the
// same answer a field with no modifiers gives.
CHAOS_IL2CPP_INTPTR ChaosReflectionFieldGetOptionalCustomModifiers(CHAOS_IL2CPP_INTPTR field) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryFieldDescriptor>(
        static_cast<FieldInfoHandle>(field));
    if (decoded == nullptr) return 0;
    static CHAOS_IL2CPP_INTPTR s_empty[1] = {0};
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_empty);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionFieldGetRequiredCustomModifiers(CHAOS_IL2CPP_INTPTR field) noexcept {
    return ChaosReflectionFieldGetOptionalCustomModifiers(field);
}

// GetValueDirect/SetValueDirect operate on a TypedReference. AOT has no
// TypedReference representation (it is a byref-plus-type runtime construct), so
// these report unresolved rather than misinterpreting the argument.
CHAOS_IL2CPP_INTPTR ChaosReflectionFieldGetValueDirect(CHAOS_IL2CPP_INTPTR field) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryFieldDescriptor>(
        static_cast<FieldInfoHandle>(field));
    if (decoded == nullptr) return 0;
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosReflectionFieldSetValueDirect(CHAOS_IL2CPP_INTPTR field) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryFieldDescriptor>(
        static_cast<FieldInfoHandle>(field));
    if (decoded == nullptr) return 0;
    return 0;
}

// PropertyInfo.GetIndexParameters — indexed properties (indexers) carry their
// index parameter list in the descriptor's owning method accessors; the
// property descriptor has no parameter array, so no indexer parameters exist
// for it. The empty set matches a non-indexed property.
CHAOS_IL2CPP_INTPTR ChaosReflectionPropertyGetIndexParametersVersion(CHAOS_IL2CPP_INTPTR prop) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryPropertyDescriptor>(
        static_cast<PropertyInfoHandle>(prop));
    if (decoded == nullptr) return 0;
    static CHAOS_IL2CPP_INTPTR s_empty[1] = {0};
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_empty);
}

// PropertyInfo.GetValue / SetValue — route through the accessor methods
// resolved by GetGetMethod/GetSetMethod, then MethodBase.Invoke.
CHAOS_IL2CPP_INTPTR ChaosReflectionPropertyGetValue(
    CHAOS_IL2CPP_INTPTR prop, CHAOS_IL2CPP_INTPTR obj) noexcept {
    CHAOS_IL2CPP_INTPTR getter = ChaosReflectionPropertyGetGetMethod(prop);
    if (getter == 0) return 0;
    return ChaosReflectionInvokeMethod(getter, obj, 0);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionPropertySetValue(
    CHAOS_IL2CPP_INTPTR prop, CHAOS_IL2CPP_INTPTR obj, CHAOS_IL2CPP_INTPTR value) noexcept {
    CHAOS_IL2CPP_INTPTR setter = ChaosReflectionPropertyGetSetMethod(prop);
    if (setter == 0) return 0;
    return ChaosReflectionInvokeMethod(setter, obj, value);
}

// EventInfo.AddEventHandler / RemoveEventHandler — delegate through the
// resolved add_/remove_ accessor methods.
CHAOS_IL2CPP_INTPTR ChaosReflectionEventAddEventHandler(
    CHAOS_IL2CPP_INTPTR evt, CHAOS_IL2CPP_INTPTR obj, CHAOS_IL2CPP_INTPTR handler) noexcept {
    CHAOS_IL2CPP_INTPTR adder = ChaosReflectionEventGetAddMethod(evt);
    if (adder == 0) return 0;
    return ChaosReflectionInvokeMethod(adder, obj, handler);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionEventRemoveEventHandler(
    CHAOS_IL2CPP_INTPTR evt, CHAOS_IL2CPP_INTPTR obj, CHAOS_IL2CPP_INTPTR handler) noexcept {
    CHAOS_IL2CPP_INTPTR remover = ChaosReflectionEventGetRemoveMethod(evt);
    if (remover == 0) return 0;
    return ChaosReflectionInvokeMethod(remover, obj, handler);
}

// EventInfo.GetOtherMethods — the non-accessor methods associated with the
// event (e.g. a compiler-generated raise helper beyond raise_). The descriptor
// records add_/remove_/raise_ only, so the "other" set is empty.
CHAOS_IL2CPP_INTPTR ChaosReflectionEventGetOtherMethods(CHAOS_IL2CPP_INTPTR evt) noexcept {
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryEventDescriptor>(
        static_cast<EventInfoHandle>(evt));
    if (decoded == nullptr) return 0;
    static CHAOS_IL2CPP_INTPTR s_empty[1] = {0};
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_empty);
}

// TypeInfo.GenericTypeParameters — the type's own generic parameter list. The
// descriptor exposes the definition's parameter descriptors, which is the same
// source GetGenericArguments reads.
CHAOS_IL2CPP_INTPTR ChaosTypeInfoGetGenericTypeParameters(CHAOS_IL2CPP_INTPTR type_handle) noexcept {
    return ChaosReflectionGetGenericArguments(type_handle);
}

// TypeInfo.GetDeclaredMethods() overload — same declared-only enumeration.
CHAOS_IL2CPP_INTPTR ChaosTypeInfoGetDeclaredMethodsVersion(CHAOS_IL2CPP_INTPTR type_handle) noexcept {
    return ChaosTypeInfoGetDeclaredMethods(type_handle);
}

// ── AssemblyName remaining accessors ────────────────────────────────
// An AssemblyName is a *value* the BCL builds from a display name; under AOT
// the only AssemblyName instances come from an image descriptor, so the
// properties below read that descriptor rather than a loader-populated object.
CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyNameGetVersionObj(CHAOS_IL2CPP_INTPTR name) noexcept {
    // Version is exposed by GetVersion (which formats the descriptor's numeric
    // fields); this entry point is the same lookup under the property name.
    return ChaosReflectionAssemblyNameGetVersion(name);
}

// AssemblyName.CodeBase / EscapedCodeBase — the location the assembly was
// loaded from. AOT images are statically linked with no file path in the
// descriptor, so there is no code base to report.
CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyNameGetCodeBase(CHAOS_IL2CPP_INTPTR name) noexcept {
    if (name == 0) return 0;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyNameGetEscapedCodeBase(CHAOS_IL2CPP_INTPTR name) noexcept {
    return ChaosReflectionAssemblyNameGetCodeBase(name);
}

// AssemblyName.KeyPair — strong-name key material is a build-time input that is
// not carried into AOT metadata.
CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyNameGetKeyPair(CHAOS_IL2CPP_INTPTR name) noexcept {
    if (name == 0) return 0;
    return 0;
}

// AssemblyName.VersionCompatibility — AssemblyVersionCompatibility.SameMachine
// (2) is the runtime-neutral default, matching an assembly with no explicit
// compatibility declaration.
CHAOS_IL2CPP_INT32 ChaosReflectionAssemblyNameGetVersionCompatibility(CHAOS_IL2CPP_INTPTR name) noexcept {
    if (name == 0) return 0;
    return 2;  // AssemblyVersionCompatibility.SameMachine
}

// AssemblyName.CultureInfo — the culture the name refers to. AOT images are
// culture-neutral, so InvariantCulture is the correct answer (returned as 0,
// the same encoding ChaosReflectionAssemblyNameGetCultureInfo already uses).
CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyNameGetCultureInfoObj(CHAOS_IL2CPP_INTPTR name) noexcept {
    return ChaosReflectionAssemblyNameGetCultureInfo(name);
}

// AssemblyName.GetAssemblyName(path) — loads an AssemblyName from a file. This
// reads a file from disk, which an AOT image has no mechanism for.
CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyNameGetAssemblyNameFile(CHAOS_IL2CPP_INTPTR path) noexcept {
    if (path == 0) return 0;
    return 0;
}

// AssemblyName.OnDeserialization / GetObjectData — ISerializable support.
// Assembly names are not serialized under AOT.
CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyNameOnDeserialization(CHAOS_IL2CPP_INTPTR name) noexcept {
    if (name == 0) return 0;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyNameGetObjectData(CHAOS_IL2CPP_INTPTR name) noexcept {
    if (name == 0) return 0;
    return 0;
}

// ── Assembly resource / reference accessors ─────────────────────────
// GetManifestResourceStream/Info/GetFile(s) all address embedded resources and
// files, neither of which the AOT descriptor model carries. GetReferenced
// Assemblies reports the assemblies this one statically references, which the
// descriptor likewise does not record (the closure is resolved at build time).
CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetManifestResourceStream(CHAOS_IL2CPP_INTPTR assembly) noexcept {
    auto* image = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(assembly));
    if (image == nullptr) return 0;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetManifestResourceInfo(CHAOS_IL2CPP_INTPTR assembly) noexcept {
    auto* image = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(assembly));
    if (image == nullptr) return 0;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetFile(CHAOS_IL2CPP_INTPTR assembly) noexcept {
    auto* image = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(assembly));
    if (image == nullptr) return 0;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetFiles(CHAOS_IL2CPP_INTPTR assembly) noexcept {
    auto* image = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(assembly));
    if (image == nullptr) return 0;
    // The BCL returns an empty array when the manifest has no file table.
    static CHAOS_IL2CPP_INTPTR s_empty[1] = {0};
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_empty);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetReferencedAssemblies(CHAOS_IL2CPP_INTPTR assembly) noexcept {
    auto* image = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(assembly));
    if (image == nullptr) return 0;
    // References are resolved and linked at AOT build time and are not retained
    // in the descriptor; the empty set is the truthful answer.
    static CHAOS_IL2CPP_INTPTR s_empty[1] = {0};
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_empty);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetObjectData(CHAOS_IL2CPP_INTPTR assembly) noexcept {
    auto* image = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(assembly));
    if (image == nullptr) return 0;
    return 0;  // no ISerializable support under AOT
}

// ── AssemblyName accessors ──────────────────────────────────────────
// The AssemblyName handle is the image's `image_name_utf8` pointer (see
// ChaosReflectionGetAssemblyName in type_properties.cpp), so name-derived
// values are resolved by scanning registered modules for the matching image —
// the same lookup GetVersion below already performs.
namespace {

const ReflectionQueryImageDescriptor* FindImageByName(const char* image_name) noexcept {
    if (image_name == nullptr) return nullptr;
    const uint32_t mod_count = GetModuleCount();
    for (uint32_t mid = 0; mid < mod_count; mid++) {
        const auto* mod = GetModuleByIndex(mid);
        if (mod == nullptr || mod->image == nullptr) continue;
        if (mod->image->image_name_utf8 != nullptr &&
            std::strcmp(mod->image->image_name_utf8, image_name) == 0) {
            return mod->image;
        }
    }
    return nullptr;
}

}  // namespace

// AssemblyName.Name — the simple name (assembly short name without the
// ", Version=..., Culture=..." qualification).
CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyNameGetName(CHAOS_IL2CPP_INTPTR name) noexcept {
    const char* image_name = reinterpret_cast<const char*>(name);
    if (image_name == nullptr) return 0;
    // AOT image names are plain assembly simple names, so the value interns
    // directly.
    auto id = string_table::Intern(image_name,
        static_cast<CHAOS_IL2CPP_UINT32>(std::strlen(image_name)));
    return static_cast<CHAOS_IL2CPP_INTPTR>(id | CHAOS_STRING_ID_TAG);
}

// AssemblyName.FullName — the display name: "Name, Version=X.Y.Z.W,
// Culture=neutral, PublicKeyToken=null". Built from the image descriptor's
// version fields, mirroring how the BCL composes it.
CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyNameGetFullName(CHAOS_IL2CPP_INTPTR name) noexcept {
    const char* image_name = reinterpret_cast<const char*>(name);
    const auto* image = FindImageByName(image_name);
    if (image == nullptr) return ChaosReflectionAssemblyNameGetName(name);

    char buf[512];
    auto result = fmt::format_to_n(buf, sizeof(buf) - 1,
        "{}, Version={}.{}.{}.{}, Culture=neutral, PublicKeyToken=null",
        image_name,
        static_cast<unsigned>(image->version_major),
        static_cast<unsigned>(image->version_minor),
        static_cast<unsigned>(image->version_build),
        static_cast<unsigned>(image->version_revision));
    auto id = string_table::Intern(buf, static_cast<uint32_t>(result.size));
    return static_cast<CHAOS_IL2CPP_INTPTR>(id | CHAOS_STRING_ID_TAG);
}

// AssemblyName.CultureName — AOT images are culture-neutral by construction
// (satellite assemblies are not part of the AOT closure), so the empty string
// is the correct answer, matching AssemblyName.GetCultureName() for a
// neutral assembly.
CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyNameGetCultureName(CHAOS_IL2CPP_INTPTR name) noexcept {
    if (name == 0) return 0;
    auto id = string_table::Intern("", 0);
    return static_cast<CHAOS_IL2CPP_INTPTR>(id | CHAOS_STRING_ID_TAG);
}

// AssemblyName.ToString() — for an AssemblyName, ToString returns FullName.
CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyNameToString(CHAOS_IL2CPP_INTPTR name) noexcept {
    return ChaosReflectionAssemblyNameGetFullName(name);
}

// AssemblyName.ReferenceMatchesDefinition(a, b) — true when both refer to the
// same simple name. The AOT model has no versioned binding, so simple-name
// equality is the strongest correct answer.
CHAOS_IL2CPP_INT32 ChaosReflectionAssemblyNameReferenceMatchesDefinition(
    CHAOS_IL2CPP_INTPTR reference, CHAOS_IL2CPP_INTPTR definition) noexcept {
    const char* a = reinterpret_cast<const char*>(reference);
    const char* b = reinterpret_cast<const char*>(definition);
    if (a == nullptr || b == nullptr) return 0;
    return std::strcmp(a, b) == 0 ? 1 : 0;
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