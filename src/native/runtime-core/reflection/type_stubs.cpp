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
    auto* desc = GetTypeDescriptorFromHandle(type);
    if (desc == nullptr || desc->events == nullptr) return 0;

    static CHAOS_IL2CPP_INTPTR s_buffer[65];
    uint32_t count = desc->event_count > 32 ? 32 : desc->event_count;
    s_buffer[0] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<intptr_t>(count));
    for (uint32_t i = 0; i < count; i++) {
        s_buffer[1 + i] = static_cast<CHAOS_IL2CPP_INTPTR>(
            EncodeReflectionQueryEventHandle(&desc->events[i]));
    }
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_buffer);
}

CHAOS_IL2CPP_INTPTR ChaosTypeGetEvent(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR name) noexcept {
    auto* desc = GetTypeDescriptorFromHandle(type);
    if (desc == nullptr || desc->events == nullptr) return 0;

    const char* evt_name = DecodeAndNullTerminateString(name);
    if (evt_name == nullptr) return 0;

    auto* evt = FindReflectionQueryEvent(desc, evt_name);
    if (evt == nullptr) return 0;

    return static_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryEventHandle(evt));
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
    CHAOS_IL2CPP_INTPTR bindingFlags) noexcept
{
    // Resolve type descriptor and find field by name.
    auto* desc = GetTypeDescriptorFromHandle(type);
    if (desc == nullptr || desc->fields == nullptr) return 0;

    const char* field_name = DecodeAndNullTerminateString(name);
    if (field_name == nullptr) return 0;

    // Find the field by name, then check BindingFlags
    for (CHAOS_IL2CPP_UINT32 i = 0; i < desc->field_count; i++) {
        if (NamesMatch(desc->fields[i].name_utf8, field_name)) {
            const int normalized_flags = NormalizeBindingFlags(static_cast<int>(bindingFlags));
            if (MatchFieldFlags(desc->fields[i].flags, normalized_flags)) {
                return static_cast<CHAOS_IL2CPP_INTPTR>(
                    EncodeReflectionQueryFieldHandle(&desc->fields[i]));
            }
            // Field exists but doesn't match flags — return 0
            return 0;
        }
    }

    return 0;
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

// ── Internal helpers: cross-module type search ─────────────────────────

// Search for a type descriptor by subject_id across all module images and
// static AOT metadata (aot_metadata::kAllTypes).
static const ReflectionQueryTypeDescriptor* FindDescBySubjectId(const char* subject_id) noexcept {
    if (subject_id == nullptr) return nullptr;
    uint32_t mod_count = GetModuleCount();
    for (uint32_t mid = 0; mid < mod_count; mid++) {
        const auto* mod = GetModuleByIndex(mid);
        if (mod == nullptr || mod->image == nullptr) continue;
        for (uint32_t ti = 0; ti < mod->image->type_count; ti++) {
            auto* t = mod->image->types[ti];
            if (t != nullptr && t->subject_id_utf8 != nullptr &&
                std::strcmp(t->subject_id_utf8, subject_id) == 0) {
                return t;
            }
        }
    }
    // Fallback: scan static AOT metadata
    for (uint32_t i = 0; i < aot_metadata::kAllTypeCount; i++) {
        auto* t = aot_metadata::kAllTypes[i];
        if (t != nullptr && t->subject_id_utf8 != nullptr &&
            std::strcmp(t->subject_id_utf8, subject_id) == 0) {
            return t;
        }
    }
    return nullptr;
}

// Search for a TypeRef (module_id + token) by type name across all modules.
// Used to resolve "TypeName[]", "TypeName&", "TypeName*" composite type names.
// Returns true and fills out_mod_id / out_token on match.
static bool FindTypeRefByName(const char* target_name,
                              uint32_t& out_mod_id,
                              uint32_t& out_token) noexcept {
    if (target_name == nullptr) return false;
    uint32_t mod_count = GetModuleCount();
    for (uint32_t mid = 0; mid < mod_count; mid++) {
        const auto* mod = GetModuleByIndex(mid);
        if (mod == nullptr || mod->tombstone || mod->type_names == nullptr) continue;
        for (uint32_t ti = 0; ti < mod->type_count; ti++) {
            if (mod->type_names[ti] != nullptr &&
                std::strcmp(mod->type_names[ti], target_name) == 0) {
                out_mod_id = mid;
                out_token = 0x02000000u | (ti + 1);
                return true;
            }
        }
    }
    return false;
}

CHAOS_IL2CPP_INTPTR ChaosTypeGetElementType(CHAOS_IL2CPP_INTPTR type) noexcept {
    // Shared static buffer for constructing element type name/subject_id
    static char s_elem_buf[1024];

    // ── Path 1: ModuleRegistry handle (TypeRef) ──
    TypeRef tr;
    if (ResolveTypeRef(type, tr)) {
        const char* name = tr.module->type_names[tr.type_index];
        if (name != nullptr) {
            size_t len = std::strlen(name);
            uint32_t flags = tr.module->type_flags[tr.type_index];

            // Determine which suffix to strip: ByRef ("&") > Pointer ("*") > Array ("[]")
            size_t suffix_len = 0;
            if ((flags & kFlagIsByRef) != 0u && len >= 1 && name[len - 1] == '&') {
                suffix_len = 1;
            } else if ((flags & kFlagIsPointer) != 0u && len >= 1 && name[len - 1] == '*') {
                suffix_len = 1;
            } else if (len >= 2 && name[len - 1] == ']' && name[len - 2] == '[') {
                suffix_len = 2;
            }

            if (suffix_len > 0) {
                size_t elem_len = len - suffix_len;
                if (elem_len >= sizeof(s_elem_buf)) elem_len = sizeof(s_elem_buf) - 1;
                std::memcpy(s_elem_buf, name, elem_len);
                s_elem_buf[elem_len] = '\0';

                // Search for element type by name across all modules
                uint32_t elem_mod_id = 0;
                uint32_t elem_token = 0;
                if (FindTypeRefByName(s_elem_buf, elem_mod_id, elem_token)) {
                    return static_cast<CHAOS_IL2CPP_INTPTR>(
                        MakeTypeHandle(elem_mod_id, elem_token));
                }
            }
        }
    }

    // ── Path 2: Descriptor-based handle (ReflectionQuery or fallback) ──
    auto* desc = GetTypeDescriptorFromHandle(type);
    if (desc != nullptr && desc->subject_id_utf8 != nullptr) {
        auto sid_len = std::strlen(desc->subject_id_utf8);

        // Determine which suffix to strip: ByRef ("&") > Pointer ("*") > Array ("[]")
        size_t suffix_len = 0;
        if (sid_len >= 1 && desc->subject_id_utf8[sid_len - 1] == '&') {
            suffix_len = 1;
        } else if (sid_len >= 1 && desc->subject_id_utf8[sid_len - 1] == '*') {
            suffix_len = 1;
        } else if (sid_len >= 2 && desc->subject_id_utf8[sid_len - 1] == ']' &&
                   desc->subject_id_utf8[sid_len - 2] == '[') {
            suffix_len = 2;
        }

        if (suffix_len > 0) {
            size_t elem_len = sid_len - suffix_len;
            if (elem_len >= sizeof(s_elem_buf)) elem_len = sizeof(s_elem_buf) - 1;
            std::memcpy(s_elem_buf, desc->subject_id_utf8, elem_len);
            s_elem_buf[elem_len] = '\0';

            // Search for element type descriptor by subject_id
            auto* elem_desc = FindDescBySubjectId(s_elem_buf);
            if (elem_desc != nullptr) {
                return static_cast<CHAOS_IL2CPP_INTPTR>(
                    EncodeReflectionQueryTypeHandle(elem_desc));
            }
        }
    }

    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosTypeMakeArrayType(CHAOS_IL2CPP_INTPTR type) noexcept {
    static char s_buf[1024];

    // Path 1: ModuleRegistry TypeRef handle — append "[]" and search type_names
    TypeRef tr;
    if (ResolveTypeRef(type, tr)) {
        const char* name = tr.module->type_names[tr.type_index];
        if (name != nullptr) {
            size_t len = std::strlen(name);
            if (len + 3 > sizeof(s_buf)) return 0;
            std::memcpy(s_buf, name, len);
            s_buf[len] = '['; s_buf[len + 1] = ']'; s_buf[len + 2] = '\0';

            uint32_t found_mod_id = 0;
            uint32_t found_token = 0;
            if (FindTypeRefByName(s_buf, found_mod_id, found_token)) {
                return static_cast<CHAOS_IL2CPP_INTPTR>(
                    MakeTypeHandle(found_mod_id, found_token));
            }
        }
    }

    // Path 2: Descriptor-based handle (ReflectionQuery or fallback) — append "[]" and search subject_id
    auto* desc = GetTypeDescriptorFromHandle(type);
    if (desc != nullptr && desc->subject_id_utf8 != nullptr) {
        size_t len = std::strlen(desc->subject_id_utf8);
        if (len + 3 > sizeof(s_buf)) return 0;
        std::memcpy(s_buf, desc->subject_id_utf8, len);
        s_buf[len] = '['; s_buf[len + 1] = ']'; s_buf[len + 2] = '\0';

        auto* found_desc = FindDescBySubjectId(s_buf);
        if (found_desc != nullptr) {
            return static_cast<CHAOS_IL2CPP_INTPTR>(
                EncodeReflectionQueryTypeHandle(found_desc));
        }
    }

    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosTypeMakeByRefType(CHAOS_IL2CPP_INTPTR type) noexcept {
    static char s_buf[1024];

    // Path 1: ModuleRegistry TypeRef handle — append "&" and search type_names
    TypeRef tr;
    if (ResolveTypeRef(type, tr)) {
        const char* name = tr.module->type_names[tr.type_index];
        if (name != nullptr) {
            size_t len = std::strlen(name);
            if (len + 2 > sizeof(s_buf)) return 0;
            std::memcpy(s_buf, name, len);
            s_buf[len] = '&'; s_buf[len + 1] = '\0';

            uint32_t found_mod_id = 0;
            uint32_t found_token = 0;
            if (FindTypeRefByName(s_buf, found_mod_id, found_token)) {
                return static_cast<CHAOS_IL2CPP_INTPTR>(
                    MakeTypeHandle(found_mod_id, found_token));
            }
        }
    }

    // Path 2: Descriptor-based handle — append "&" and search subject_id
    auto* desc = GetTypeDescriptorFromHandle(type);
    if (desc != nullptr && desc->subject_id_utf8 != nullptr) {
        size_t len = std::strlen(desc->subject_id_utf8);
        if (len + 2 > sizeof(s_buf)) return 0;
        std::memcpy(s_buf, desc->subject_id_utf8, len);
        s_buf[len] = '&'; s_buf[len + 1] = '\0';

        auto* found_desc = FindDescBySubjectId(s_buf);
        if (found_desc != nullptr) {
            return static_cast<CHAOS_IL2CPP_INTPTR>(
                EncodeReflectionQueryTypeHandle(found_desc));
        }
    }

    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosTypeMakePointerType(CHAOS_IL2CPP_INTPTR type) noexcept {
    static char s_buf[1024];

    // Path 1: ModuleRegistry TypeRef handle — append "*" and search type_names
    TypeRef tr;
    if (ResolveTypeRef(type, tr)) {
        const char* name = tr.module->type_names[tr.type_index];
        if (name != nullptr) {
            size_t len = std::strlen(name);
            if (len + 2 > sizeof(s_buf)) return 0;
            std::memcpy(s_buf, name, len);
            s_buf[len] = '*'; s_buf[len + 1] = '\0';

            uint32_t found_mod_id = 0;
            uint32_t found_token = 0;
            if (FindTypeRefByName(s_buf, found_mod_id, found_token)) {
                return static_cast<CHAOS_IL2CPP_INTPTR>(
                    MakeTypeHandle(found_mod_id, found_token));
            }
        }
    }

    // Path 2: Descriptor-based handle — append "*" and search subject_id
    auto* desc = GetTypeDescriptorFromHandle(type);
    if (desc != nullptr && desc->subject_id_utf8 != nullptr) {
        size_t len = std::strlen(desc->subject_id_utf8);
        if (len + 2 > sizeof(s_buf)) return 0;
        std::memcpy(s_buf, desc->subject_id_utf8, len);
        s_buf[len] = '*'; s_buf[len + 1] = '\0';

        auto* found_desc = FindDescBySubjectId(s_buf);
        if (found_desc != nullptr) {
            return static_cast<CHAOS_IL2CPP_INTPTR>(
                EncodeReflectionQueryTypeHandle(found_desc));
        }
    }

    return 0;
}

}  // namespace chaos::il2cpp::runtime_core
}  // extern "C"