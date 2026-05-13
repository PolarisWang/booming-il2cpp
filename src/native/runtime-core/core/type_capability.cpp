namespace chaos::il2cpp::runtime_core {
namespace {

static const ReflectionQueryTypeDescriptor* TryResolveRuntimeCoreTypeDescriptor(TypeInfoHandle type) {
    if (type == 0) return nullptr;
    return TryDecodeReflectionQueryTypeHandle(type);
}

static bool TryPopulateVectorCapabilityFromDisplayName(
    const CHAOS_IL2CPP_STRING& display_name,
    RuntimeTypeCapabilityInfoV0* out_capability_info) {
    constexpr CHAOS_IL2CPP_STRING_VIEW kNumericsVectorPrefix = "System.Numerics.Vector<";
    if (display_name.size() >= kNumericsVectorPrefix.size() &&
        memcmp(display_name.data(), kNumericsVectorPrefix.data(), kNumericsVectorPrefix.size()) == 0 &&
        display_name.size() > kNumericsVectorPrefix.size() &&
        display_name.back() == '>') {
        const auto scalar_display_name = display_name.substr(
            kNumericsVectorPrefix.size(),
            display_name.size() - kNumericsVectorPrefix.size() - 1u);
        return TryPopulateVectorCapabilityFromDisplayName(scalar_display_name, out_capability_info);
    }

    if (display_name == "System.Byte" || display_name == "System.SByte") {
        out_capability_info->value_size_bytes = 1u;
        out_capability_info->vector_lane_kind = TypeCapabilityVectorLaneKindInteger;
        out_capability_info->scalar_kind = display_name == "System.SByte"
            ? TypeCapabilityScalarKindSignedInteger
            : TypeCapabilityScalarKindUnsignedInteger;
        return true;
    }

    if (display_name == "System.Int16" || display_name == "System.UInt16") {
        out_capability_info->value_size_bytes = 2u;
        out_capability_info->vector_lane_kind = TypeCapabilityVectorLaneKindInteger;
        out_capability_info->scalar_kind = display_name == "System.Int16"
            ? TypeCapabilityScalarKindSignedInteger
            : TypeCapabilityScalarKindUnsignedInteger;
        return true;
    }

    if (display_name == "System.Int32" || display_name == "System.UInt32" || display_name == "System.Single") {
        out_capability_info->value_size_bytes = 4u;
        out_capability_info->vector_lane_kind = display_name == "System.Single"
            ? TypeCapabilityVectorLaneKindFloating
            : TypeCapabilityVectorLaneKindInteger;
        out_capability_info->scalar_kind = display_name == "System.Single"
            ? TypeCapabilityScalarKindFloating
            : (display_name == "System.Int32"
                ? TypeCapabilityScalarKindSignedInteger
                : TypeCapabilityScalarKindUnsignedInteger);
        return true;
    }

    if (display_name == "System.Int64" || display_name == "System.UInt64" || display_name == "System.Double") {
        out_capability_info->value_size_bytes = 8u;
        out_capability_info->vector_lane_kind = display_name == "System.Double"
            ? TypeCapabilityVectorLaneKindFloating
            : TypeCapabilityVectorLaneKindInteger;
        out_capability_info->scalar_kind = display_name == "System.Double"
            ? TypeCapabilityScalarKindFloating
            : (display_name == "System.Int64"
                ? TypeCapabilityScalarKindSignedInteger
                : TypeCapabilityScalarKindUnsignedInteger);
        return true;
    }

    if (display_name == "System.IntPtr" || display_name == "System.UIntPtr") {
#if INTPTR_MAX == INT64_MAX
        out_capability_info->value_size_bytes = 8u;
#else
        out_capability_info->value_size_bytes = 4u;
#endif
        out_capability_info->vector_lane_kind = TypeCapabilityVectorLaneKindInteger;
        out_capability_info->scalar_kind = display_name == "System.IntPtr"
            ? TypeCapabilityScalarKindNativeSignedInteger
            : TypeCapabilityScalarKindNativeUnsignedInteger;
        return true;
    }

    return false;
}

static bool TryPopulateRegisteredTypeCapability(
    TypeInfoHandle type,
    RuntimeTypeCapabilityInfoV0* out_capability_info) {
    if (type == 0) return false;
    const CHAOS_IL2CPP_UINTPTR raw_handle = static_cast<CHAOS_IL2CPP_UINTPTR>(type);
    if ((raw_handle & kReflectionQueryHandleTag) != 0u) return false;

    const CHAOS_IL2CPP_UINT32 type_token = static_cast<CHAOS_IL2CPP_UINT32>(raw_handle);
    if (const auto* entry = chaos::il2cpp::bootstrap::FindRegisteredTypeCapabilityEntry(type_token)) {
        *out_capability_info = entry->capability_info;
        return true;
    }

    return false;
}

RuntimeStatus TypeQueryCapabilityImpl(
    TypeInfoHandle type,
    RuntimeTypeCapabilityInfoV0* out_capability_info) {
    if (out_capability_info == nullptr || out_capability_info->struct_size < sizeof(RuntimeTypeCapabilityInfoV0)) {
        return CHAOS_RUNTIME_STATUS_INVALID_ARGUMENT;
    }

    out_capability_info->capability_bits = 0u;
    out_capability_info->value_size_bytes = 0u;
    out_capability_info->vector_width_bytes = 0u;
    out_capability_info->vector_lane_count = 0u;
    out_capability_info->vector_lane_kind = TypeCapabilityVectorLaneKindNone;
    out_capability_info->scalar_kind = TypeCapabilityScalarKindNone;

    if (TryPopulateRegisteredTypeCapability(type, out_capability_info)) {
        return CHAOS_RUNTIME_STATUS_OK;
    }

    if (const auto* descriptor = TryResolveRuntimeCoreTypeDescriptor(type)) {
        CHAOS_IL2CPP_STRING display_name = descriptor->display_name_utf8 != nullptr
            ? descriptor->display_name_utf8
            : "";
        if (TryPopulateVectorCapabilityFromDisplayName(display_name, out_capability_info)) {
            out_capability_info->vector_width_bytes = [&display_name]() -> CHAOS_IL2CPP_UINT32 {
                auto starts_with = [&](const char* prefix) -> bool {
                    const auto len = strlen(prefix);
                    return display_name.size() >= len && memcmp(display_name.data(), prefix, len) == 0;
                };
                if (starts_with("System.Numerics.Vector512<")) return 64u;
                if (starts_with("System.Numerics.Vector256<")) return 32u;
                if (starts_with("System.Numerics.Vector128<")) return 16u;
                if (starts_with("System.Numerics.Vector64<"))  return 8u;
                if (starts_with("System.Numerics.Vector<"))
                    return static_cast<CHAOS_IL2CPP_UINT32>(VectorPlatformByteWidth());
                return 16u;
            }();
            out_capability_info->vector_lane_count = out_capability_info->value_size_bytes == 0u
                ? 0u
                : (out_capability_info->vector_width_bytes / out_capability_info->value_size_bytes);
            out_capability_info->capability_bits = 1u;
            return CHAOS_RUNTIME_STATUS_OK;
        }

        return CHAOS_RUNTIME_STATUS_NOT_SUPPORTED;
    }

    return CHAOS_RUNTIME_STATUS_NOT_SUPPORTED;
}

}  // anonymous namespace

RuntimeStatus TypeQueryCapability(
    TypeInfoHandle type,
    RuntimeTypeCapabilityInfoV0* out_capability_info) {
    return TypeQueryCapabilityImpl(type, out_capability_info);
}

}  // namespace chaos::il2cpp::runtime_core