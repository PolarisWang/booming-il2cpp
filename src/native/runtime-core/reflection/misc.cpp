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
// Returns a string_id (tagged pointer) so the generated code's
// String::get_Length etc. can resolve it.  Raw C string pointers
// would be misread as managed string objects.
CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssemblyFullName(CHAOS_IL2CPP_INTPTR assembly_handle) noexcept {
    auto* decoded = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(assembly_handle));
    if (decoded == nullptr || decoded->image_name_utf8 == nullptr) {
        decoded = &aot_metadata::kImageCoreLib;
    }
    auto id = string_table::Intern(decoded->image_name_utf8,
        static_cast<CHAOS_IL2CPP_UINT32>(std::strlen(decoded->image_name_utf8)));
    return static_cast<CHAOS_IL2CPP_INTPTR>(id | CHAOS_STRING_ID_TAG);
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
// Return an Intern'd path string instead of 0 so the generated code's
// null check doesn't crash.
CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssemblyLocation(CHAOS_IL2CPP_INTPTR /*assembly*/) noexcept {
    auto id = string_table::Intern("unknown", 7);
    return static_cast<CHAOS_IL2CPP_INTPTR>(id | CHAOS_STRING_ID_TAG);
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
// Returns 0 when no special attributes are set — this is valid and expected,
// not a stub. Parameter attributes (In/Out/Optional) are not emitted in AOT
// metadata by default; callers must handle 0 as "no special attributes".
CHAOS_IL2CPP_INT32 ChaosReflectionGetParamAttributes(CHAOS_IL2CPP_INTPTR /*param*/) noexcept {
    return 0;
}

// ── GetFieldType ──────────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionGetFieldType(CHAOS_IL2CPP_INTPTR field_handle) noexcept {
    auto* field = TryDecodeReflectionQueryHandle<ReflectionQueryFieldDescriptor>(
        static_cast<FieldInfoHandle>(field_handle));
    if (field == nullptr || field->member_type_utf8 == nullptr) return 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<char*>(field->member_type_utf8));
}

// ── GetPropertyType ───────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionGetPropertyType(CHAOS_IL2CPP_INTPTR property_handle) noexcept {
    auto* prop = TryDecodeReflectionQueryHandle<ReflectionQueryPropertyDescriptor>(
        static_cast<PropertyInfoHandle>(property_handle));
    if (prop == nullptr || prop->member_type_utf8 == nullptr) return 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<char*>(prop->member_type_utf8));
}

// ── GetFieldsBindingflags ───────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionGetFieldsBindingflags(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INT32 flags) noexcept {
    using namespace chaos::il2cpp::runtime_core;
    flags = NormalizeBindingFlags(flags);
    auto* desc = ResolveTypeFromReflectionOrGcHandle(type);

    constexpr CHAOS_IL2CPP_UINT32 kMaxFields = 256;
    struct FieldsBuf {
        ThinLockableHeader header;
        CHAOS_IL2CPP_UINT8  element_type_shape;
        CHAOS_IL2CPP_INTPTR element_type_info;
        CHAOS_IL2CPP_INTPTR length;
        CHAOS_IL2CPP_INTPTR* elements;
    };
    static FieldsBuf s_buf{};
    static CHAOS_IL2CPP_INTPTR s_elements[kMaxFields]{};

    uint32_t total = 0;
    const ReflectionQueryFieldDescriptor* fields = nullptr;

    if (desc != nullptr && desc->fields != nullptr) {
        total = desc->field_count > kMaxFields ? kMaxFields : desc->field_count;
        fields = desc->fields;
    } else {
        // EEClass fallback for dynamic types (T2-3) — no flags filtering for now
        auto* ee = ResolveEEClassFromHandle(type);
        if (ee != nullptr) {
            EnsureFieldsFilled(ee);
            if (ee->fields.filled && ee->fields.data != nullptr) {
                total = ee->fields.count > kMaxFields ? kMaxFields : ee->fields.count;
                fields = reinterpret_cast<const ReflectionQueryFieldDescriptor*>(ee->fields.data);
            }
        }
    }

    if (fields == nullptr) return 0;

    uint32_t idx = 0;
    for (CHAOS_IL2CPP_UINT32 i = 0; i < total; i++) {
        if (MatchFieldFlags(fields[i].flags, flags)) {
            s_elements[idx++] = static_cast<CHAOS_IL2CPP_INTPTR>(
                EncodeReflectionQueryFieldHandle(&fields[i]));
        }
    }

    s_buf = FieldsBuf{};
    s_buf.element_type_shape = 1;
    s_buf.length = static_cast<CHAOS_IL2CPP_INTPTR>(idx);
    s_buf.elements = s_elements;

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&s_buf);
}

// ── GetPropertiesBindingflags ──────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionGetPropertiesBindingflags(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INT32 flags) noexcept {
    using namespace chaos::il2cpp::runtime_core;
    flags = NormalizeBindingFlags(flags);
    auto* desc = ResolveTypeFromReflectionOrGcHandle(type);

    constexpr CHAOS_IL2CPP_UINT32 kMaxProperties = 256;
    struct PropertiesBuf {
        ThinLockableHeader header;
        CHAOS_IL2CPP_UINT8  element_type_shape;
        CHAOS_IL2CPP_INTPTR element_type_info;
        CHAOS_IL2CPP_INTPTR length;
        CHAOS_IL2CPP_INTPTR* elements;
    };
    static PropertiesBuf s_buf{};
    static CHAOS_IL2CPP_INTPTR s_elements[kMaxProperties]{};

    uint32_t total = 0;
    const ReflectionQueryPropertyDescriptor* properties = nullptr;

    if (desc != nullptr && desc->properties != nullptr) {
        total = desc->property_count > kMaxProperties ? kMaxProperties : desc->property_count;
        properties = desc->properties;
    } else {
        // EEClass fallback for dynamic types (T2-3)
        auto* ee = ResolveEEClassFromHandle(type);
        if (ee != nullptr) {
            EnsurePropertiesFilled(ee);
            if (ee->properties.filled && ee->properties.data != nullptr) {
                total = ee->properties.count > kMaxProperties ? kMaxProperties : ee->properties.count;
                properties = reinterpret_cast<const ReflectionQueryPropertyDescriptor*>(ee->properties.data);
            }
        }
    }

    if (properties == nullptr) return 0;

    uint32_t idx = 0;
    for (CHAOS_IL2CPP_UINT32 i = 0; i < total; i++) {
        if (MatchPropertyFlags(properties[i].flags, flags)) {
            s_elements[idx++] = static_cast<CHAOS_IL2CPP_INTPTR>(
                EncodeReflectionQueryPropertyHandle(&properties[i]));
        }
    }

    s_buf = PropertiesBuf{};
    s_buf.element_type_shape = 1;
    s_buf.length = static_cast<CHAOS_IL2CPP_INTPTR>(idx);
    s_buf.elements = s_elements;

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&s_buf);
}

// ── GetMethodsBindingflags ──────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionGetMethodsBindingflags(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INT32 flags) noexcept {
    using namespace chaos::il2cpp::runtime_core;
    flags = NormalizeBindingFlags(flags);
    auto* desc = ResolveTypeFromReflectionOrGcHandle(type);

    constexpr CHAOS_IL2CPP_UINT32 kMaxMethods = 512;
    struct MethodsBuf {
        ThinLockableHeader header;
        CHAOS_IL2CPP_UINT8  element_type_shape;
        CHAOS_IL2CPP_INTPTR element_type_info;
        CHAOS_IL2CPP_INTPTR length;
        CHAOS_IL2CPP_INTPTR* elements;
    };
    static MethodsBuf s_buf{};
    static CHAOS_IL2CPP_INTPTR s_elements[kMaxMethods]{};

    uint32_t idx = 0;

    // Self methods (descriptor path or EEClass fallback)
    if (desc != nullptr && desc->methods != nullptr) {
        for (CHAOS_IL2CPP_UINT32 i = 0; i < desc->method_count && idx < kMaxMethods; i++) {
            if (MatchMethodFlags(desc->methods[i].flags, flags)) {
                s_elements[idx++] = static_cast<CHAOS_IL2CPP_INTPTR>(
                    EncodeReflectionQueryMethodHandle(&desc->methods[i]));
            }
        }
    } else if (desc == nullptr) {
        // EEClass fallback for dynamic types (T2-3) — no flags filtering for now
        auto* ee = ResolveEEClassFromHandle(type);
        if (ee != nullptr) {
            EnsureMethodsFilled(ee);
            if (ee->methods.filled && ee->methods.data != nullptr) {
                uint32_t count = ee->methods.count > kMaxMethods ? kMaxMethods : ee->methods.count;
                for (uint32_t i = 0; i < count && idx < kMaxMethods; i++) {
                    s_elements[idx++] = static_cast<CHAOS_IL2CPP_INTPTR>(
                        EncodeReflectionQueryMethodHandle(&ee->methods.data[i]));
                }
            }
        } else {
            return 0;
        }
    }

    // Walk the parent TypeInfo chain to include inherited methods
    auto* type_info = GetTypeInfoFromReflectionOrGcHandle(type);
    if (type_info != nullptr) {
        const TypeInfoHot* parent = type_info->parent;
        uint32_t max_depth = 20;
        while (parent != nullptr && max_depth > 0) {
            uint32_t count = GetModuleCount();
            bool found = false;
            for (uint32_t i = 0; i < count && !found; i++) {
                const auto* mod = GetModuleByIndex(i);
                if (mod == nullptr || mod->type_info_ptrs == nullptr || mod->image == nullptr) continue;
                for (uint32_t j = 0; j < mod->type_count && j < mod->image->type_count && !found; j++) {
                    if (mod->type_info_ptrs[j] == parent) {
                        const auto* parent_desc = mod->image->types[j];
                        if (parent_desc != nullptr && parent_desc->methods != nullptr) {
                            for (uint32_t k = 0; k < parent_desc->method_count && idx < kMaxMethods; k++) {
                                if (MatchMethodFlags(parent_desc->methods[k].flags, flags)) {
                                    s_elements[idx++] = static_cast<CHAOS_IL2CPP_INTPTR>(
                                        EncodeReflectionQueryMethodHandle(&parent_desc->methods[k]));
                                }
                            }
                        }
                        found = true;
                    }
                }
            }
            if (!found) break;
            parent = parent->parent;
            max_depth--;
        }
    }

    s_buf = MethodsBuf{};
    s_buf.element_type_shape = 1;
    s_buf.length = static_cast<CHAOS_IL2CPP_INTPTR>(idx);
    s_buf.elements = s_elements;

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&s_buf);
}

}  // namespace chaos::il2cpp::runtime_core
}  // extern "C"