// =====================================================================
// Type resolution
// =====================================================================

extern "C" {
namespace chaos::il2cpp::runtime_core {

// ── Dynamic type registration (for codegen-generated enum types) ──────────
static constexpr CHAOS_IL2CPP_UINT32 kMaxDynamicTypes = 256u;
struct DynamicTypeEntry {
    CHAOS_IL2CPP_UINT32 fnv24_hash;
    const ReflectionQueryTypeDescriptor* type_desc;
};
static DynamicTypeEntry s_dynamicTypes[kMaxDynamicTypes];
static CHAOS_IL2CPP_UINT32 s_dynamicTypeCount = 0u;

extern "C" void ChaosRegisterExternalType(
    CHAOS_IL2CPP_UINT32 fnv24_hash,
    const ReflectionQueryTypeDescriptor* type_desc) noexcept
{
    if (s_dynamicTypeCount < kMaxDynamicTypes && type_desc != nullptr) {
        s_dynamicTypes[s_dynamicTypeCount].fnv24_hash = fnv24_hash;
        s_dynamicTypes[s_dynamicTypeCount].type_desc = type_desc;
        s_dynamicTypeCount++;
    }
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetTypeFromHandle(CHAOS_IL2CPP_INTPTR runtime_type_handle) {
    if (runtime_type_handle == 0) return 0;

    // Try raw metadata token lookup first.
    uint32_t token = DecodeMetadataToken(runtime_type_handle);
    auto* typeDesc = aot_metadata::FindTypeByMetadataToken(token);
    if (typeDesc != nullptr) {
        return static_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryTypeHandle(typeDesc));
    }

    // Try pseudo-metadata handle (codegen FNV-1a convention: 0x02XXXXXX).
    uint32_t val = static_cast<uint32_t>(runtime_type_handle & 0xFFFFFFFFu);
    if ((val & 0xFF000000u) == 0x02000000u && (val & 0xFFFFFFu) != 0u) {
        uint32_t target_hash = val & 0xFFFFFFu;
        // Scan aot_metadata::kAllTypes (static descriptors).
        for (uint32_t i = 0u; i < aot_metadata::kAllTypeCount; i++) {
            auto* type = aot_metadata::kAllTypes[i];
            if (type == nullptr || type->subject_id_utf8 == nullptr) continue;
            uint32_t h = 2166136261u;
            for (const char* s = type->subject_id_utf8; *s; s++) {
                h ^= static_cast<uint8_t>(*s);
                h *= 16777619u;
            }
            if ((h & 0xFFFFFFu) == target_hash) {
                return static_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryTypeHandle(type));
            }
        }

        // Scan dynamically registered types (codegen enum types, etc.).
        for (CHAOS_IL2CPP_UINT32 i = 0u; i < s_dynamicTypeCount; i++) {
            if (s_dynamicTypes[i].fnv24_hash == target_hash) {
                return static_cast<CHAOS_IL2CPP_INTPTR>(
                    EncodeReflectionQueryTypeHandle(s_dynamicTypes[i].type_desc));
            }
        }
    }

    // Try ModuleRegistry handle: [module_id:32][token:32] encoding.
    // Hot-update modules registered via RegisterModule() can be resolved
    // through their module_id and TypeDef token.
    {
        TypeInfoHandle type_handle = static_cast<TypeInfoHandle>(runtime_type_handle);
        uint32_t mid = GetModuleId(type_handle);
        uint32_t tok = GetTypeToken(type_handle);
        if (mid != 0 && tok != 0) {
            const auto* mod = LookupModule(mid);
            if (mod != nullptr && !mod->tombstone) {
                // Path A: resolve through type_info_ptrs[] (Tier 1).
                uint32_t idx = TokenToIndex(tok);
                if (idx < mod->type_count && mod->type_info_ptrs != nullptr) {
                    const auto* ti = mod->type_info_ptrs[idx];
                    if (ti != nullptr) {
                        // Encode as ReflectionQuery handle if image is available.
                        if (mod->image != nullptr && idx < mod->image->type_count) {
                            return static_cast<CHAOS_IL2CPP_INTPTR>(
                                EncodeReflectionQueryTypeHandle(mod->image->types[idx]));
                        }
                        // Otherwise return the raw module_id+token as the handle.
                        return runtime_type_handle;
                    }
                }
                // Path B: resolve through image descriptor (Tier 2).
                if (mod->image != nullptr) {
                    auto* typeDesc = FindReflectionQueryTypeByToken(mod->image, tok);
                    if (typeDesc != nullptr) {
                        return static_cast<CHAOS_IL2CPP_INTPTR>(
                            EncodeReflectionQueryTypeHandle(typeDesc));
                    }
                }
            }
        }
    }

    return 0;
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

    // Fallback: scan ModuleRegistry type_names[] for modules without an image.
    // Hot-update modules registered via RegisterModule() with a minimal
    // ModuleDescriptor (image == nullptr) are not covered by the loop above.
    // This path scans type_names[] / type_namespaces[] directly and returns
    // a ModuleRegistry-style handle [module_id:32][synthetic_token:32].
    for (uint32_t i = 0; i < count; i++) {
        const auto* module = GetModuleByIndex(i);
        if (module == nullptr || module->type_names == nullptr) continue;
        if (module->image != nullptr) continue;  // already scanned in the loop above
        for (uint32_t j = 0; j < module->type_count; j++) {
            const char* mod_ns = module->type_namespaces != nullptr
                ? module->type_namespaces[j] : "";
            const char* mod_name = module->type_names[j];
            if (mod_name != nullptr &&
                NamesMatch(mod_ns, ns) &&
                NamesMatch(mod_name, type_name)) {
                // Return ModuleRegistry handle: [module_id:32][synthetic_typedef_token:32]
                uint32_t synthetic_token = 0x02000000u | (j + 1);
                return static_cast<CHAOS_IL2CPP_INTPTR>(MakeTypeHandle(i, synthetic_token));
            }
        }
    }

    return 0;
}

}  // namespace chaos::il2cpp::runtime_core
}  // extern "C"