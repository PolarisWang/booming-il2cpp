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
CHAOS_IL2CPP_INTPTR ChaosReflectionIsDefined(CHAOS_IL2CPP_INTPTR /*assembly*/, CHAOS_IL2CPP_INTPTR /*type*/) noexcept {
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetRequiredCustomModifiers(CHAOS_IL2CPP_INTPTR /*param*/) noexcept {
    return 0;
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
CHAOS_IL2CPP_INTPTR ChaosReflectionGetIsVirtual(CHAOS_IL2CPP_INTPTR /*member*/) noexcept {
    // TODO: Requires flags field in ReflectionQueryMethodDescriptor (codegen change).
    return 0;
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

CHAOS_IL2CPP_INTPTR ChaosTypeGetTypeInfo(CHAOS_IL2CPP_INTPTR type) noexcept {
    // Return the TypeInfoHot* for this type handle, if type_info_ptrs is populated.
    auto* type_info = GetTypeInfoFromHandle(type);
    if (type_info == nullptr) return 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<TypeInfoHot*>(type_info));
}

// ── Assembly stubs ──────────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetExportedTypes(CHAOS_IL2CPP_INTPTR assembly) noexcept {
    // Count types with kFlagIsPublic set = exported types.
    auto* decoded = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(assembly));
    if (decoded == nullptr) return 0;

    uint32_t module_count = GetModuleCount();
    for (uint32_t mid = 0; mid < module_count; mid++) {
        const auto* mod = GetModuleByIndex(mid);
        if (mod == nullptr || mod->image != decoded || mod->type_flags == nullptr) continue;

        uint32_t exported_count = 0;
        for (uint32_t i = 0; i < mod->type_count; i++) {
            if (mod->type_flags[i] & kFlagIsPublic) {
                exported_count++;
            }
        }
        return static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<intptr_t>(exported_count));
    }
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetForwardedTypes(CHAOS_IL2CPP_INTPTR /*assembly*/) noexcept {
    return 0;  // Forwarded type forwarding not tracked in AOT metadata.
}

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetManifestResourceNames(CHAOS_IL2CPP_INTPTR /*assembly*/) noexcept {
    return 0;  // Manifest resources not tracked in AOT.
}

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetModules(CHAOS_IL2CPP_INTPTR assembly) noexcept {
    // AOT assemblies have exactly one module — the assembly itself.
    // Return the image handle as a module handle (same encoding as ImageHandle).
    return assembly;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetEntryPoint(CHAOS_IL2CPP_INTPTR /*assembly*/) noexcept {
    return 0;  // Module not specified / EntryPoint not tracked in AOT.
}

// ── ParameterInfo stubs ─────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionGetOptionalCustomModifiers(CHAOS_IL2CPP_INTPTR /*param*/) noexcept {
    return 0;
}

// ── FieldInfo stubs (Phase 2+: ModuleRegistry Tier 0 flags) ─────────
CHAOS_IL2CPP_INT32 ChaosReflectionFieldGetIsInitOnly(CHAOS_IL2CPP_INTPTR /*field*/) noexcept {
    // TODO: Requires flags field in ReflectionQueryFieldDescriptor (codegen change).
    return 0;
}

CHAOS_IL2CPP_INT64 ChaosReflectionFieldGetFieldHandle(CHAOS_IL2CPP_INTPTR /*field*/) noexcept {
    // TODO: Needs RuntimeFieldHandle construction from metadata token.
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosReflectionFieldGetIsLiteral(CHAOS_IL2CPP_INTPTR /*field*/) noexcept {
    // TODO: Requires flags field in ReflectionQueryFieldDescriptor (codegen change).
    return 0;
}

// ── PropertyInfo stubs (Phase 2+: EEClass property flags) ───────────
CHAOS_IL2CPP_INTPTR ChaosReflectionPropertyGetIndexParameters(CHAOS_IL2CPP_INTPTR /*prop*/) noexcept {
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosReflectionPropertyGetCanRead(CHAOS_IL2CPP_INTPTR /*prop*/) noexcept {
    // TODO: Requires flags field in ReflectionQueryPropertyDescriptor (codegen change).
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosReflectionPropertyGetCanWrite(CHAOS_IL2CPP_INTPTR /*prop*/) noexcept {
    // TODO: Requires flags field in ReflectionQueryPropertyDescriptor (codegen change).
    return 0;
}

// ── AssemblyName stubs ──────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyNameGetCultureInfo(CHAOS_IL2CPP_INTPTR /*name*/) noexcept {
    return 0;  // Invariant culture = nullptr/0.
}

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyNameGetVersion(CHAOS_IL2CPP_INTPTR /*name*/) noexcept {
    return 0;  // Version info not tracked in AOT.
}

}  // namespace chaos::il2cpp::runtime_core
}  // extern "C"