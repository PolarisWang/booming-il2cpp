// =====================================================================
// Remaining reflection stubs — deferred to Phase 2+/3+
// =====================================================================
// These are either not yet wired to data sources (assembly-level queries,
// custom attributes) or waiting on EEClass expansion.
// Keeping them in one place makes it easy to track Phase 1+ progress.

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
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetBaseDefinition(CHAOS_IL2CPP_INTPTR member_handle) noexcept {
    return member_handle;  // self is the base definition (no overrides in AOT)
}

// ── Runtime type handle stubs ───────────────────────────────────────
// Phase 2+: implement managed System.Type identity resolution.
CHAOS_IL2CPP_INTPTR ChaosRuntimeTypeFromHandle(CHAOS_IL2CPP_INTPTR handle) noexcept {
    (void)handle;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosTypeGetTypeFromHandle(CHAOS_IL2CPP_INTPTR handle) noexcept {
    (void)handle;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosTypeEquals(CHAOS_IL2CPP_INTPTR type_a, CHAOS_IL2CPP_INTPTR type_b) noexcept {
    (void)type_a; (void)type_b;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosTypeGetTypeInfo(CHAOS_IL2CPP_INTPTR type) noexcept {
    (void)type;
    return 0;
}

// ── Assembly stubs (Phase 2: ALC iteration) ─────────────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetExportedTypes(CHAOS_IL2CPP_INTPTR /*assembly*/) noexcept {
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetForwardedTypes(CHAOS_IL2CPP_INTPTR /*assembly*/) noexcept {
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetManifestResourceNames(CHAOS_IL2CPP_INTPTR /*assembly*/) noexcept {
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetModules(CHAOS_IL2CPP_INTPTR /*assembly*/) noexcept {
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetEntryPoint(CHAOS_IL2CPP_INTPTR /*assembly*/) noexcept {
    return 0;
}

// ── ParameterInfo stubs ─────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionGetOptionalCustomModifiers(CHAOS_IL2CPP_INTPTR /*param*/) noexcept {
    return 0;
}

// ── FieldInfo stubs (Phase 2+: ModuleRegistry Tier 0 flags) ─────────
CHAOS_IL2CPP_INT32 ChaosReflectionFieldGetIsInitOnly(CHAOS_IL2CPP_INTPTR /*field*/) noexcept {
    return 0;
}

CHAOS_IL2CPP_INT64 ChaosReflectionFieldGetFieldHandle(CHAOS_IL2CPP_INTPTR /*field*/) noexcept {
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosReflectionFieldGetIsLiteral(CHAOS_IL2CPP_INTPTR /*field*/) noexcept {
    return 0;
}

// ── PropertyInfo stubs (Phase 2+: EEClass property flags) ───────────
CHAOS_IL2CPP_INTPTR ChaosReflectionPropertyGetIndexParameters(CHAOS_IL2CPP_INTPTR /*prop*/) noexcept {
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosReflectionPropertyGetCanRead(CHAOS_IL2CPP_INTPTR /*prop*/) noexcept {
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosReflectionPropertyGetCanWrite(CHAOS_IL2CPP_INTPTR /*prop*/) noexcept {
    return 0;
}

// ── AssemblyName stubs (Phase 2+: metadata tables) ──────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyNameGetCultureInfo(CHAOS_IL2CPP_INTPTR /*name*/) noexcept {
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyNameGetVersion(CHAOS_IL2CPP_INTPTR /*name*/) noexcept {
    return 0;
}

}  // namespace chaos::il2cpp::runtime_core
}  // extern "C"