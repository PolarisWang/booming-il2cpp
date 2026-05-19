// =====================================================================
// ChaosType* reflection stubs — managed-side type query functions
// =====================================================================
// These are called directly from managed/NativeAOT code as SimpleForward
// replacements for GenericShapeDescriptors.  Most delegate to existing
// ChaosReflection* implementations or ModuleRegistry Tier 0 flags.

extern "C" {
namespace chaos::il2cpp::runtime_core {

// ── Type flag queries (delegate to ModuleRegistry Tier 0) ──────────────

CHAOS_IL2CPP_INT32 ChaosTypeGetIsByRef(CHAOS_IL2CPP_INTPTR type) noexcept {
    TypeRef tr;
    if (ResolveTypeRef(type, tr))
        return (tr.module->type_flags[tr.type_index] & kFlagIsByRef) != 0u ? 1 : 0;
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosTypeGetIsPointer(CHAOS_IL2CPP_INTPTR type) noexcept {
    TypeRef tr;
    if (ResolveTypeRef(type, tr))
        return (tr.module->type_flags[tr.type_index] & kFlagIsPointer) != 0u ? 1 : 0;
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosTypeGetIsNested(CHAOS_IL2CPP_INTPTR type) noexcept {
    TypeRef tr;
    if (ResolveTypeRef(type, tr))
        return (tr.module->type_flags[tr.type_index] & kFlagIsNested) != 0u ? 1 : 0;
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosTypeGetIsNestedPrivate(CHAOS_IL2CPP_INTPTR type) noexcept {
    // Nested + not public = private/protected/internal. For now:
    // return 1 if nested but not public.
    TypeRef tr;
    if (ResolveTypeRef(type, tr)) {
        uint32_t flags = tr.module->type_flags[tr.type_index];
        return ((flags & kFlagIsNested) && !(flags & kFlagIsPublic)) ? 1 : 0;
    }
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosTypeGetIsNestedPublic(CHAOS_IL2CPP_INTPTR type) noexcept {
    TypeRef tr;
    if (ResolveTypeRef(type, tr)) {
        uint32_t flags = tr.module->type_flags[tr.type_index];
        return ((flags & kFlagIsNested) && (flags & kFlagIsPublic)) ? 1 : 0;
    }
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosTypeGetIsNotPublic(CHAOS_IL2CPP_INTPTR type) noexcept {
    TypeRef tr;
    if (ResolveTypeRef(type, tr))
        return (tr.module->type_flags[tr.type_index] & kFlagIsPublic) == 0u ? 1 : 0;
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosTypeGetIsPublic(CHAOS_IL2CPP_INTPTR type) noexcept {
    TypeRef tr;
    if (ResolveTypeRef(type, tr))
        return (tr.module->type_flags[tr.type_index] & kFlagIsPublic) != 0u ? 1 : 0;
    return 0;
}

// ── Type member queries (delegate to EEClass or descriptor) ────────────

CHAOS_IL2CPP_INTPTR ChaosTypeGetProperties(CHAOS_IL2CPP_INTPTR type) noexcept {
    // Delegate to GetMembers and filter (Phase 1+: direct EEClass path)
    auto* desc = GetTypeDescriptorFromHandle(type);
    if (desc == nullptr || desc->properties == nullptr) return 0;

    static CHAOS_IL2CPP_INTPTR s_buffer[65];
    uint32_t count = desc->property_count > 32 ? 32 : desc->property_count;
    s_buffer[0] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<intptr_t>(count));
    for (uint32_t i = 0; i < count; i++) {
        s_buffer[1 + i] = static_cast<CHAOS_IL2CPP_INTPTR>(
            EncodeReflectionQueryPropertyHandle(&desc->properties[i]));
    }
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_buffer);
}

CHAOS_IL2CPP_INTPTR ChaosTypeGetEvents(CHAOS_IL2CPP_INTPTR type) noexcept {
    // Events not yet in Tier 2 descriptors. Return empty array.
    static CHAOS_IL2CPP_INTPTR s_buffer[1] = {0};
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_buffer);
}

CHAOS_IL2CPP_INTPTR ChaosTypeGetEvent(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR name) noexcept {
    (void)type; (void)name;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosTypeGetProperty(
    CHAOS_IL2CPP_INTPTR type,
    CHAOS_IL2CPP_INTPTR name) noexcept
{
    auto* desc = GetTypeDescriptorFromHandle(type);
    if (desc == nullptr || desc->properties == nullptr) return 0;

    const char* prop_name = DecodeAndNullTerminateString(name);
    if (prop_name == nullptr) return 0;

    auto* prop = FindReflectionQueryProperty(desc, prop_name);
    if (prop == nullptr) return 0;

    return static_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryPropertyHandle(prop));
}

CHAOS_IL2CPP_INTPTR ChaosTypeGetFieldBindingFlags(
    CHAOS_IL2CPP_INTPTR type,
    CHAOS_IL2CPP_INTPTR name,
    CHAOS_IL2CPP_INTPTR /*bindingFlags*/) noexcept
{
    // Resolve type descriptor and find field by name.
    auto* desc = GetTypeDescriptorFromHandle(type);
    if (desc == nullptr || desc->fields == nullptr) return 0;

    const char* field_name = DecodeAndNullTerminateString(name);
    if (field_name == nullptr) return 0;

    const auto* field = FindReflectionQueryField(desc, field_name);
    if (field == nullptr) return 0;

    return static_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryFieldHandle(field));
}

CHAOS_IL2CPP_INTPTR ChaosTypeGetMembers(CHAOS_IL2CPP_INTPTR type) noexcept {
    return ChaosReflectionGetMembers(type);
}

CHAOS_IL2CPP_INTPTR ChaosTypeGetNestedTypes(CHAOS_IL2CPP_INTPTR type) noexcept {
    return ChaosReflectionGetNestedTypes(type);
}

CHAOS_IL2CPP_INTPTR ChaosTypeGetNestedType(
    CHAOS_IL2CPP_INTPTR type,
    CHAOS_IL2CPP_INTPTR name) noexcept
{
    TypeRef tr;
    if (!ResolveTypeRef(type, tr)) return 0;
    if (tr.module->nested_type_offset == nullptr) return 0;

    const char* type_name = DecodeAndNullTerminateString(name);
    if (type_name == nullptr) return 0;

    uint32_t start = tr.module->nested_type_offset[tr.type_index];
    uint32_t end = tr.module->nested_type_offset[tr.type_index + 1];

    for (uint32_t i = start; i < end; i++) {
        uint32_t child_token = tr.module->nested_type_children[i];
        uint32_t child_idx = TokenToIndex(child_token);
        if (child_idx < tr.module->type_count) {
            const char* child_name = tr.module->type_names[child_idx];
            if (child_name != nullptr && NamesMatch(child_name, type_name)) {
                return static_cast<CHAOS_IL2CPP_INTPTR>(
                    MakeTypeHandle(tr.module_id, child_token));
            }
        }
    }
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosTypeGetInterfaces(CHAOS_IL2CPP_INTPTR type) noexcept {
    return ChaosReflectionGetInterfaces(type);
}

CHAOS_IL2CPP_INTPTR ChaosTypeGetUnderlyingSystemType(CHAOS_IL2CPP_INTPTR type) noexcept {
    // For enum types: resolve underlying type from Module Registry parent token.
    TypeRef tr;
    if (ResolveTypeRef(type, tr)) {
        uint32_t flags = tr.module->type_flags[tr.type_index];
        if (flags & kFlagIsEnum) {
            // Enum's parent chain: Enum → ValueType → Object.
            // For simplicity, return System.Int32 as default underlying type.
            // Phase 2+: read [EnumUnderlyingTypeAttribute] or parent token chain.
            return static_cast<CHAOS_IL2CPP_INTPTR>(
                MakeTypeHandle(0, 0x02000004));  // System.Int32 token
        }
    }
    return type;  // Non-enum types return themselves
}

CHAOS_IL2CPP_INTPTR ChaosTypeGetElementType(CHAOS_IL2CPP_INTPTR type) noexcept {
    // For array types: resolve element type from the type name.
    // Array names end with "[]" — strip them and look up the element type.
    TypeRef tr;
    if (ResolveTypeRef(type, tr)) {
        const char* name = tr.module->type_names[tr.type_index];
        if (name != nullptr) {
            size_t len = std::strlen(name);
            if (len >= 2 && name[len - 1] == ']' && name[len - 2] == '[') {
                // Element type name = name without trailing "[]"
                // This requires module search — Phase 2+ TBD.
            }
        }
    }

    // Fallback: try descriptor-based element type resolution
    auto* desc = GetTypeDescriptorFromHandle(type);
    if (desc != nullptr && desc->subject_id_utf8 != nullptr) {
        // subject_id format: "ElementType[]" — strip trailing "[]"
        auto sid_len = std::strlen(desc->subject_id_utf8);
        if (sid_len >= 3 && desc->subject_id_utf8[sid_len - 1] == ']') {
            // Build element type subject_id
            static char s_elem_buf[1024];
            size_t elem_len = sid_len - 2;
            if (elem_len >= sizeof(s_elem_buf)) elem_len = sizeof(s_elem_buf) - 1;
            std::memcpy(s_elem_buf, desc->subject_id_utf8, elem_len);
            s_elem_buf[elem_len] = '\0';

            // Scan modules for a type with matching subject_id
            uint32_t mod_count = GetModuleCount();
            for (uint32_t mid = 0; mid < mod_count; mid++) {
                const auto* mod = GetModuleByIndex(mid);
                if (mod == nullptr || mod->image == nullptr) continue;
                for (uint32_t ti = 0; ti < mod->image->type_count; ti++) {
                    auto* t = mod->image->types[ti];
                    if (t != nullptr && t->subject_id_utf8 != nullptr &&
                        std::strcmp(t->subject_id_utf8, s_elem_buf) == 0) {
                        return static_cast<CHAOS_IL2CPP_INTPTR>(
                            EncodeReflectionQueryTypeHandle(t));
                    }
                }
            }
        }
    }
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosTypeMakeArrayType(CHAOS_IL2CPP_INTPTR type) noexcept {
    (void)type;
    return 0;  // Phase 2: instantiate SzArray type wrapper
}

CHAOS_IL2CPP_INTPTR ChaosTypeMakeByRefType(CHAOS_IL2CPP_INTPTR type) noexcept {
    (void)type;
    return 0;  // Phase 2: instantiate ByRef-like wrapper
}

CHAOS_IL2CPP_INTPTR ChaosTypeMakePointerType(CHAOS_IL2CPP_INTPTR type) noexcept {
    (void)type;
    return 0;  // Phase 2: instantiate pointer-like wrapper
}

}  // namespace chaos::il2cpp::runtime_core
}  // extern "C"