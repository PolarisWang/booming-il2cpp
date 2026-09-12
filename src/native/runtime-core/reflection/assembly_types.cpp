// ── AssemblyGetTypes ────────────────────────────────────────────────

extern "C" {
namespace chaos::il2cpp::runtime_core {

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetTypes(CHAOS_IL2CPP_INTPTR assembly_handle) noexcept {
    auto* decoded = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(assembly_handle));
    if (decoded == nullptr) return 0;

    // Fast path: descriptor-owned types array
    if (decoded->types != nullptr) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            const_cast<ReflectionQueryTypeDescriptor* const*>(decoded->types));
    }

    // Fallback: return type count encoded as pointer value.
    // The codegen for array.Length on external-runtime-returned arrays treats
    // the raw CHAOS_IL2CPP_INTPTR value as the count — no pointer dereference.
    // Return (intptr_t)count so (int32_t)return_value == count.
    uint32_t module_count = GetModuleCount();
    for (uint32_t mid = 0; mid < module_count; mid++) {
        const auto* mod = GetModuleByIndex(mid);
        if (mod == nullptr || mod->image != decoded) continue;
        return static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<intptr_t>(mod->type_count));
    }
    return 0;
}

// ── GetTypeFromAssemblyBool ─────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionGetTypeFromAssemblyBool(
    CHAOS_IL2CPP_INTPTR assembly_handle,
    CHAOS_IL2CPP_INTPTR name_string_id,
    CHAOS_IL2CPP_INT32 throw_on_error) noexcept
{
    auto* decoded = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(assembly_handle));
    if (decoded == nullptr) return 0;

    const char* full_name = DecodeAndNullTerminateString(name_string_id);
    if (full_name == nullptr) return 0;

    const char* ns = nullptr;
    const char* type_name = nullptr;
    SplitTypeName(full_name, ns, type_name);
    if (ns == nullptr || type_name == nullptr) return 0;

    auto* type = FindReflectionQueryTypeByName(decoded, ns, type_name);
    if (type == nullptr) {
        if (throw_on_error) {
            // V1: propagate error without a managed TypeLoadException object.
            // Future: create TypeLoadException via ABI and raise it properly.
            chaos_raise_exception(0);
        }
        return 0;
    }
    return static_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryTypeHandle(type));
}

// ── Runtime handle hash codes ───────────────────────────────────────
CHAOS_IL2CPP_INT32 ChaosRuntimetypehandleGetHashCode(CHAOS_IL2CPP_INTPTR handle) noexcept {
    return static_cast<CHAOS_IL2CPP_INT32>(handle & 0xFFFFFFFF);
}

CHAOS_IL2CPP_INT32 ChaosRuntimemethodhandleGetHashCode(CHAOS_IL2CPP_INT64 handle) noexcept {
    return static_cast<CHAOS_IL2CPP_INT32>(handle & 0xFFFFFFFF);
}

CHAOS_IL2CPP_INT32 ChaosRuntimefieldhandleGetHashCode(CHAOS_IL2CPP_INTPTR handle) noexcept {
    return static_cast<CHAOS_IL2CPP_INT32>(handle & 0xFFFFFFFF);
}


// ── Assembly identity / metadata accessors ──────────────────────────
// An Assembly handle is an encoded ReflectionQueryImageHandle. Many Assembly
// members are pure identity questions under AOT, where an image is a single
// statically-linked artifact with no loader, no GAC and no filesystem presence.
// Each accessor below states that explicitly rather than returning a bare 0.

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetManifestModule(CHAOS_IL2CPP_INTPTR assembly_handle) noexcept {
    // AOT: one image == one module, and the module handle is the image handle.
    auto* image = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(assembly_handle));
    return image == nullptr ? 0 : assembly_handle;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetModulesList(CHAOS_IL2CPP_INTPTR assembly_handle) noexcept {
    // Same single-module identity as ManifestModule; GetModules() returns the
    // one-element sequence whose only entry is that module.
    return ChaosReflectionAssemblyGetManifestModule(assembly_handle);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetExportedTypesProperty(CHAOS_IL2CPP_INTPTR assembly_handle) noexcept {
    // ExportedTypes == GetExportedTypes() in the BCL.
    return ChaosReflectionAssemblyGetExportedTypes(assembly_handle);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetDefinedTypes(CHAOS_IL2CPP_INTPTR assembly_handle) noexcept {
    // DefinedTypes is the TypeInfo view of the assembly's own types; the
    // descriptor array is the same source GetTypes returns.
    return ChaosReflectionAssemblyGetTypes(assembly_handle);
}

// Assembly.GetAssembly(Type) — the assembly that defines the given type.
CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetAssemblyForType(CHAOS_IL2CPP_INTPTR type_handle) noexcept {
    return ChaosReflectionGetAssembly(type_handle);
}

// Assembly.GetModule(name) — resolve by module name; AOT has a single module,
// so the lookup succeeds only for that module's own name.
CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetModuleByName(
    CHAOS_IL2CPP_INTPTR assembly_handle, CHAOS_IL2CPP_INTPTR name_string_id) noexcept {
    auto* image = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(assembly_handle));
    if (image == nullptr || image->image_name_utf8 == nullptr) return 0;

    const char* want = DecodeAndNullTerminateString(name_string_id);
    if (want == nullptr) return 0;

    return std::strcmp(image->image_name_utf8, want) == 0 ? assembly_handle : 0;
}

// Assembly.GetLoadedModules() — every registered module is loaded.
CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetLoadedModules(CHAOS_IL2CPP_INTPTR assembly_handle) noexcept {
    return ChaosReflectionAssemblyGetManifestModule(assembly_handle);
}

// Assembly.FullName / ToString() — both resolve to the display name.
CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyToString(CHAOS_IL2CPP_INTPTR assembly_handle) noexcept {
    return ChaosReflectionGetAssemblyFullName(assembly_handle);
}

// Assembly.CustomAttributes / GetCustomAttributesData — the assembly-level
// attribute blob, which lives behind the image's custom-attribute table.
CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetCustomAttributesData(CHAOS_IL2CPP_INTPTR assembly_handle) noexcept {
    // Assembly-level custom attributes are addressed by member_kind 1 with the
    // image handle as the member handle (see ChaosGetCustomAttributeFromBlob).
    return ChaosReflectionCollectCustomAttributes(
        static_cast<CHAOS_IL2CPP_INTPTR>(1), assembly_handle);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetCustomAttributes(CHAOS_IL2CPP_INTPTR assembly_handle) noexcept {
    return ChaosReflectionAssemblyGetCustomAttributesData(assembly_handle);
}

CHAOS_IL2CPP_INT32 ChaosReflectionAssemblyIsDefined(
    CHAOS_IL2CPP_INTPTR assembly_handle, CHAOS_IL2CPP_INTPTR attribute_type_handle) noexcept {
    if (attribute_type_handle == 0) return 0;
    // The attribute type handle carries the token to match against.
    return ChaosReflectionMemberIsDefinedByToken(
        static_cast<CHAOS_IL2CPP_INTPTR>(1), assembly_handle,
        static_cast<CHAOS_IL2CPP_INTPTR>(GetTypeToken(attribute_type_handle)));
}

// ── Assembly identity predicates (AOT semantics) ────────────────────
// These describe loader-level state that does not exist in an AOT image. Each
// answer matches what CoreCLR reports for the equivalent CoreLib/single-file
// case so callers take the same branch they would on the reference runtime.
CHAOS_IL2CPP_INT32 ChaosReflectionAssemblyGetIsFullyTrusted(CHAOS_IL2CPP_INTPTR assembly_handle) noexcept {
    auto* image = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(assembly_handle));
    if (image == nullptr) return 0;
    return 1;  // .NET Core has no CAS sandbox: every loaded assembly is fully trusted
}

CHAOS_IL2CPP_INT32 ChaosReflectionAssemblyGetGlobalAssemblyCache(CHAOS_IL2CPP_INTPTR assembly_handle) noexcept {
    auto* image = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(assembly_handle));
    if (image == nullptr) return 0;
    return 0;  // the GAC does not exist on .NET Core / AOT
}

CHAOS_IL2CPP_INT32 ChaosReflectionAssemblyGetReflectionOnly(CHAOS_IL2CPP_INTPTR assembly_handle) noexcept {
    auto* image = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(assembly_handle));
    if (image == nullptr) return 0;
    return 0;  // ReflectionOnly loading is a .NET Framework concept
}

CHAOS_IL2CPP_INT32 ChaosReflectionAssemblyGetSecurityRuleSet(CHAOS_IL2CPP_INTPTR assembly_handle) noexcept {
    auto* image = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(assembly_handle));
    if (image == nullptr) return 0;
    return 2;  // SecurityRuleSet.Level2 — the value .NET Core reports
}

CHAOS_IL2CPP_INT64 ChaosReflectionAssemblyGetHostContext(CHAOS_IL2CPP_INTPTR assembly_handle) noexcept {
    auto* image = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(assembly_handle));
    if (image == nullptr) return 0;
    return 0;  // no hosting context under AOT
}

CHAOS_IL2CPP_INT32 ChaosReflectionAssemblyGetHashCode(CHAOS_IL2CPP_INTPTR assembly_handle) noexcept {
    // Identity hash over the image descriptor pointer — stable for the life of
    // the process, matching Assembly's reference-equality hash.
    auto* image = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(assembly_handle));
    if (image == nullptr) return 0;
    auto h = reinterpret_cast<uintptr_t>(image);
    return static_cast<CHAOS_IL2CPP_INT32>((h >> 4) ^ (h >> 20));
}

CHAOS_IL2CPP_INT32 ChaosReflectionAssemblyEqualsVersion(
    CHAOS_IL2CPP_INTPTR lhs, CHAOS_IL2CPP_INTPTR rhs) noexcept {
    // Assembly.Equals is reference equality on the image identity.
    auto* a = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(lhs));
    auto* b = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(rhs));
    if (a == nullptr || b == nullptr) return 0;
    return a == b ? 1 : 0;
}

}  // namespace chaos::il2cpp::runtime_core
}  // extern "C"