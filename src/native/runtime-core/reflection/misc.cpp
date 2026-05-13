// =====================================================================
// Type reflection — Phase 4/6 (generic info, assembly, parameter, etc.)
// =====================================================================

extern "C" {
namespace chaos::il2cpp::runtime_core {

// ── GetContainsGenericParams ───────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionGetContainsGenericParams(CHAOS_IL2CPP_INTPTR type_handle) noexcept {
    TypeRef tr;
    if (ResolveTypeRef(type_handle, tr)) {
        uint32_t flags = tr.module->type_flags[tr.type_index];
        if (flags & kFlagIsConstructedGeneric) return 0;
        if (flags & (kFlagIsGenericType | kFlagIsGenericTypeDef)) return 1;
    }
    return 0;
}

// ── GetGenericParamPos ────────────────────────────────────────────
// Returns the zero-based position of a generic parameter in its owning
// type/method's generic parameter list. Parses "!N"/"!!N" from subject_id.
CHAOS_IL2CPP_INTPTR ChaosReflectionGetGenericParamPos(CHAOS_IL2CPP_INTPTR type_handle) noexcept {
    // Module Registry handles are concrete types, not generic params — return 0.
    TypeRef tr;
    if (ResolveTypeRef(type_handle, tr)) {
        return 0;
    }

    // ReflectionQuery-encoded descriptors have subject_id with "!N" or "!!N" suffix.
    auto* desc = TryDecodeReflectionQueryHandle<ReflectionQueryTypeDescriptor>(
        static_cast<TypeInfoHandle>(type_handle));
    if (desc == nullptr || desc->subject_id_utf8 == nullptr) return 0;

    // subject_id = "Namespace.Type`1/!0" or "Namespace.Method!!0"
    // Find the last '!' which introduces the position.
    const char* bang = std::strrchr(desc->subject_id_utf8, '!');
    if (bang == nullptr) return 0;

    // Skip past '!' (or "!!") to the digits
    const char* digits = bang + 1;
    if (*digits == '!') digits++;  // skip second '!' for "!!N"

    int pos = 0;
    for (; *digits >= '0' && *digits <= '9'; digits++) {
        pos = pos * 10 + (*digits - '0');
    }

    return static_cast<CHAOS_IL2CPP_INTPTR>(pos);
}

// ── GetModuleAssembly ─────────────────────────────────────────────
// Module handle IS the image/assembly handle in this model.
CHAOS_IL2CPP_INTPTR ChaosReflectionGetModuleAssembly(CHAOS_IL2CPP_INTPTR module_handle) noexcept {
    auto* decoded = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(module_handle));
    if (decoded == nullptr) return 0;
    return module_handle;
}

// ── GetModuleName ─────────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionGetModuleName(CHAOS_IL2CPP_INTPTR module_handle) noexcept {
    auto* decoded = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(module_handle));
    if (decoded == nullptr || decoded->image_name_utf8 == nullptr) return 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<char*>(decoded->image_name_utf8));
}

// ── GetModuleNameOnly ─────────────────────────────────────────────
// Returns the last segment after the last '.' or '/'.
CHAOS_IL2CPP_INTPTR ChaosReflectionGetModuleNameOnly(CHAOS_IL2CPP_INTPTR module_handle) noexcept {
    auto* decoded = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(module_handle));
    if (decoded == nullptr || decoded->image_name_utf8 == nullptr) return 0;
    const char* name = decoded->image_name_utf8;
    const char* dot = std::strrchr(name, '.');
    const char* slash = std::strrchr(name, '/');
    const char* sep = (dot > slash) ? dot : slash;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<char*>(sep ? sep + 1 : name));
}

// ── GetAssemblyFullName ───────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssemblyFullName(CHAOS_IL2CPP_INTPTR assembly_handle) noexcept {
    auto* decoded = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(assembly_handle));
    if (decoded == nullptr || decoded->image_name_utf8 == nullptr) return 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<char*>(decoded->image_name_utf8));
}

// ── GetCallingAssembly ─────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionGetCallingAssembly(void) noexcept {
    return static_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryImageHandle(&aot_metadata::kImageCoreLib));
}

// ── GetEntryAssembly ───────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionGetEntryAssembly(void) noexcept {
    uint32_t count = GetModuleCount();
    for (uint32_t i = 1; i < count; i++) {
        const auto* mod = GetModuleByIndex(i);
        if (mod != nullptr && mod->image != nullptr) {
            return static_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryImageHandle(mod->image));
        }
    }
    // Fallback: CoreLib
    return static_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryImageHandle(&aot_metadata::kImageCoreLib));
}

// ── GetExecutingAssembly ───────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionGetExecutingAssembly(void) noexcept {
    return static_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryImageHandle(&aot_metadata::kImageCoreLib));
}

// ── GetImageRuntimeVersion ─────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionGetImageRuntimeVersion(CHAOS_IL2CPP_INTPTR /*assembly*/) noexcept {
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<char*>("v4.0.30319"));
}

// ── GetAssemblyLocation ────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssemblyLocation(CHAOS_IL2CPP_INTPTR /*assembly*/) noexcept {
    return 0;
}

// ── GetCallingConvention ───────────────────────────────────────────
CHAOS_IL2CPP_INT32 ChaosReflectionGetCallingConvention(CHAOS_IL2CPP_INTPTR /*method*/) noexcept {
    return 1;  // Standard
}

// ── GetParamPosition ───────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionGetParamPosition(CHAOS_IL2CPP_INTPTR param_handle) noexcept {
    auto* param = reinterpret_cast<const ReflectionQueryParameterDescriptor*>(param_handle);
    if (param == nullptr) return 0;
    return static_cast<CHAOS_IL2CPP_INTPTR>(param->parameter_index);
}

// ── GetParameterType ───────────────────────────────────────────────
// Returns the parameter type as a type-name string. Managed wrapper
// converts this to System.Type via GetTypeByName.
CHAOS_IL2CPP_INTPTR ChaosReflectionGetParameterType(CHAOS_IL2CPP_INTPTR param_handle) noexcept {
    auto* param = reinterpret_cast<const ReflectionQueryParameterDescriptor*>(param_handle);
    if (param == nullptr || param->member_type_utf8 == nullptr) return 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<char*>(param->member_type_utf8));
}

// ── GetParamAttributes ─────────────────────────────────────────────
CHAOS_IL2CPP_INT32 ChaosReflectionGetParamAttributes(CHAOS_IL2CPP_INTPTR /*param*/) noexcept {
    return 0;
}

// ── GetFieldsBindingflags ───────────────────────────────────────────
// For now, ignore BindingFlags and delegate to GetFields.
CHAOS_IL2CPP_INTPTR ChaosReflectionGetFieldsBindingflags(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INT32 /*flags*/) noexcept {
    return ChaosReflectionGetFields(type);
}

// ── GetMethodsBindingflags ──────────────────────────────────────────
// For now, ignore BindingFlags and delegate to GetMethods.
CHAOS_IL2CPP_INTPTR ChaosReflectionGetMethodsBindingflags(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INT32 /*flags*/) noexcept {
    return ChaosReflectionGetMethods(type);
}

}  // namespace chaos::il2cpp::runtime_core
}  // extern "C"