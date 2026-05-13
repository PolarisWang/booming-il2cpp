// =====================================================================
// Type resolution
// =====================================================================

extern "C" {
namespace chaos::il2cpp::runtime_core {

CHAOS_IL2CPP_INTPTR ChaosReflectionGetTypeFromHandle(CHAOS_IL2CPP_INTPTR runtime_type_handle) {
    if (runtime_type_handle == 0) return 0;

    uint32_t token = DecodeMetadataToken(runtime_type_handle);
    auto* typeDesc = aot_metadata::FindTypeByMetadataToken(token);
    if (typeDesc == nullptr) return 0;

    return static_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryTypeHandle(typeDesc));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetTypeByName(
    CHAOS_IL2CPP_INTPTR name_string_id,
    CHAOS_IL2CPP_INT32 throw_on_error,
    CHAOS_IL2CPP_INT32 ignore_case)
{
    (void)throw_on_error;
    (void)ignore_case;

    if (name_string_id == 0) return 0;

    const char* full_name = DecodeAndNullTerminateString(name_string_id);
    if (full_name == nullptr) return 0;

    const char* ns = nullptr;
    const char* type_name = nullptr;
    SplitTypeName(full_name, ns, type_name);
    if (ns == nullptr || type_name == nullptr) return 0;

    // Scan all registered modules (skip slot 0 = CoreLib fallback with no image)
    uint32_t count = GetModuleCount();
    for (uint32_t i = 0; i < count; i++) {
        const auto* module = GetModuleByIndex(i);
        if (module == nullptr || module->image == nullptr) continue;

        auto* type = FindReflectionQueryTypeByName(module->image, ns, type_name);
        if (type != nullptr) {
            return static_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryTypeHandle(type));
        }
    }

    return 0;
}

}  // namespace chaos::il2cpp::runtime_core
}  // extern "C"