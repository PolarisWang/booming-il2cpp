// =====================================================================
// Module reflection — Phase 2
// =====================================================================

extern "C" {
namespace chaos::il2cpp::runtime_core {

CHAOS_IL2CPP_INTPTR ChaosReflectionModuleGetType(
    CHAOS_IL2CPP_INTPTR module_handle,
    CHAOS_IL2CPP_INTPTR name_string_id) noexcept
{
    using namespace chaos::il2cpp::runtime_core;
    auto* decoded = TryDecodeReflectionQueryImageHandle(
        static_cast<ImageHandle>(module_handle));
    if (decoded == nullptr) return 0;

    const char* full_name = DecodeAndNullTerminateString(name_string_id);
    if (full_name == nullptr) return 0;

    const char* ns = nullptr;
    const char* type_name = nullptr;
    SplitTypeName(full_name, ns, type_name);
    if (ns == nullptr || type_name == nullptr) return 0;

    auto* type = FindReflectionQueryTypeByName(decoded, ns, type_name);
    if (type == nullptr) return 0;

    return static_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryTypeHandle(type));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionModuleGetTypes(
    CHAOS_IL2CPP_INTPTR module_handle) noexcept
{
    using namespace chaos::il2cpp::runtime_core;
    auto* decoded = TryDecodeReflectionQueryImageHandle(
        static_cast<ImageHandle>(module_handle));
    if (decoded == nullptr || decoded->types == nullptr) return 0;

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
        const_cast<ReflectionQueryTypeDescriptor* const*>(decoded->types));
}

}  // namespace chaos::il2cpp::runtime_core
}  // extern "C"