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

}  // namespace chaos::il2cpp::runtime_core
}  // extern "C"