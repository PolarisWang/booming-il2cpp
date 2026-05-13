namespace chaos::il2cpp::runtime_core {
namespace {

// ── Internal helpers ──

// Extract metadata token from a type handle.
// The handle can be:
//   - A ReflectionQuery encoded handle (tag bit 63) — decode and read token
//   - A Module Registry handle (module_id in upper 32 bits) — low 32 bits = token
//   - A raw metadata token — just return it
//   - Zero — return 0
static inline uint32_t DecodeMetadataToken(CHAOS_IL2CPP_INTPTR handle) {
    if (handle == 0) return 0;

    // Check if it's a ReflectionQuery encoded handle
    auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryTypeDescriptor>(static_cast<TypeInfoHandle>(handle));
    if (decoded != nullptr) {
        return decoded->metadata_token;
    }

    // For Module Registry handles and raw tokens: low 32 bits = token
    return static_cast<uint32_t>(handle & 0xFFFFFFFFu);
}

// Get a type descriptor from a handle using the full lookup chain:
//   1. ReflectionQuery encoded handle → direct pointer decode
//   2. Module Registry handle → extract module_id → LookupModule → image → FindReflectionQueryTypeByToken
//   3. Raw metadata token → aot_metadata shared tables
static inline const ReflectionQueryTypeDescriptor* GetTypeDescriptorFromHandle(CHAOS_IL2CPP_INTPTR handle) {
    if (handle == 0) return nullptr;

    // Try ReflectionQuery encoded handle first (tag bit 63 set)
    {
        auto* decoded = TryDecodeReflectionQueryHandle<ReflectionQueryTypeDescriptor>(static_cast<TypeInfoHandle>(handle));
        if (decoded != nullptr) return decoded;
    }

    // Try Module Registry handle (module_id in upper 32 bits)
    {
        TypeInfoHandle type_handle = static_cast<TypeInfoHandle>(handle);
        uint32_t module_id = GetModuleId(type_handle);
        if (module_id != 0) {
            const auto* module = LookupModule(module_id);
            if (module != nullptr && module->image != nullptr) {
                uint32_t token = GetTypeToken(type_handle);
                return FindReflectionQueryTypeByToken(module->image, token);
            }
        }
    }

    // Fall back to raw metadata token lookup
    {
        uint32_t token = static_cast<uint32_t>(handle & 0xFFFFFFFFu);
        if (token != 0) {
            return aot_metadata::FindTypeByMetadataToken(token);
        }
    }

    return nullptr;
}

// Resolve image descriptor from a type handle via Module Registry.
// Returns nullptr if the handle is zero or the module has no Tier 2 image.
static inline const ReflectionQueryImageDescriptor* GetImageFromTypeHandle(CHAOS_IL2CPP_INTPTR handle) {
    if (handle == 0) return nullptr;

    TypeInfoHandle type_handle = static_cast<TypeInfoHandle>(handle);
    uint32_t module_id = GetModuleId(type_handle);
    if (module_id == 0) {
        return nullptr;
    }

    const auto* module = LookupModule(module_id);
    if (module == nullptr) {
        return nullptr;
    }

    return module->image;
}

// Decode a CHAOS_IL2CPP_INTPTR that may be a StringId or a native pointer
static inline const char* DecodeStringValue(CHAOS_IL2CPP_INTPTR value) {
    if (value == 0) return nullptr;
    return reinterpret_cast<const char*>(value);
}

// ── String input decoding ──────────────────────────────────────────
// The name_string_id parameter in GetTypeByName/GetField/GetMethod may be:
//   1. A StringId (tagged pointer, bit 63 set) — resolve via string_table
//   2. A managed string pointer (chaos_managed_string) — extract data
// Returns a null-terminated copy in a static buffer (single-threaded safe;
// caller must consume before next call into this translation unit).

static const char* DecodeAndNullTerminateString(CHAOS_IL2CPP_INTPTR value) {
    static char s_buf[4096];
    if (value == 0) return nullptr;

    const char* source = nullptr;
    int32_t source_len = 0;

    if (chaos_is_string_id(static_cast<intptr_t>(value))) {
        auto id = static_cast<string_table::StringId>(
            chaos_extract_string_id(static_cast<intptr_t>(value)));
        auto view = string_table::Resolve(id);
        source = view.utf8_data;
        source_len = static_cast<int32_t>(view.byte_count);
    } else {
        auto* ms = reinterpret_cast<const chaos_managed_string*>(
            static_cast<CHAOS_IL2CPP_INTPTR>(value));
        source_len = ms->length;
        if (source_len > 0) {
            source = reinterpret_cast<const char*>(ms + 1);
        }
    }

    if (source == nullptr || source_len <= 0) return nullptr;

    int32_t copy_len = source_len;
    if (copy_len > static_cast<int32_t>(sizeof(s_buf) - 1))
        copy_len = static_cast<int32_t>(sizeof(s_buf) - 1);

    std::memcpy(s_buf, source, copy_len);
    s_buf[copy_len] = '\0';
    return s_buf;
}

// Split a fully qualified type name ("Namespace.TypeName") into ns and type_name.
// "System.Collections.Generic.List`1" → ns="System.Collections.Generic", type_name="List`1"
// "Int32"                              → ns="", type_name="Int32"
// Uses an internal static buffer for the namespace copy.

static void SplitTypeName(const char* full_name, const char*& ns, const char*& type_name) {
    if (full_name == nullptr || full_name[0] == '\0') {
        ns = nullptr;
        type_name = nullptr;
        return;
    }

    const char* dot = std::strrchr(full_name, '.');
    if (dot == nullptr) {
        ns = "";
        type_name = full_name;
        return;
    }

    // Namespace portion needs a null-terminated copy
    static char s_ns_buf[4096];
    ptrdiff_t ns_len = dot - full_name;
    if (ns_len >= static_cast<ptrdiff_t>(sizeof(s_ns_buf)))
        ns_len = sizeof(s_ns_buf) - 1;
    if (ns_len > 0) {
        std::memcpy(s_ns_buf, full_name, ns_len);
        s_ns_buf[ns_len] = '\0';
        ns = s_ns_buf;
    } else {
        ns = "";
    }
    type_name = dot + 1;
}

// ── TypeRef resolution ──────────────────────────────────────────────
// Resolves a handle to Module Registry Tier 0 entries.
// Returns true for Module Registry handles and raw tokens.
// Returns false for tag-bit-63 (ReflectionQuery encoded) handles —
// callers should fall back to GetTypeDescriptorFromHandle.

struct TypeRef {
    uint32_t module_id = 0;
    uint32_t token = 0;
    uint32_t type_index = 0;
    const ModuleDescriptor* module = nullptr;
};

static bool ResolveTypeRef(CHAOS_IL2CPP_INTPTR handle, TypeRef& out) noexcept {
    if (handle == 0) return false;

    auto type_handle = static_cast<TypeInfoHandle>(handle);

    // Reject tag-bit-63 handles (ReflectionQuery encoded — not resolvable to module registry)
    if ((static_cast<CHAOS_IL2CPP_UINTPTR>(type_handle) & kReflectionQueryHandleTag) != 0) {
        return false;
    }

    uint32_t mid = GetModuleId(type_handle);
    uint32_t tok = GetTypeToken(type_handle);
    if (tok == 0) return false;

    const ModuleDescriptor* mod = LookupModule(mid != 0 ? mid : 0);
    if (mod == nullptr || mod->tombstone) return false;
    if (mod->type_flags == nullptr) return false;  // Tier 0 not populated

    uint32_t idx = TokenToIndex(tok);
    if (idx >= mod->type_count) return false;

    out.module_id = mid;
    out.token = tok;
    out.type_index = idx;
    out.module = mod;
    return true;
}

// ── TypeInfo* resolution (Phase 3+) ─────────────────────────────────
// Returns the TypeInfo* for a handle that resolves via ModuleRegistry.
// Returns nullptr for ReflectionQuery encoded handles or when Tier 0
// type_info_ptrs is not populated.

static const TypeInfoHot* GetTypeInfoFromHandle(CHAOS_IL2CPP_INTPTR handle) noexcept {
    TypeRef tr;
    if (!ResolveTypeRef(handle, tr)) return nullptr;
    if (tr.module->type_info_ptrs == nullptr) return nullptr;
    return tr.module->type_info_ptrs[tr.type_index];
}

// ── Reverse lookup: stable_id -> TypeInfoHandle ────────────────────
static TypeInfoHandle FindTypeHandleByStableId(uint64_t stable_id) noexcept {
    uint32_t count = GetModuleCount();
    for (uint32_t i = 0; i < count; i++) {
        const auto* mod = GetModuleByIndex(i);
        if (mod == nullptr || mod->type_info_ptrs == nullptr || mod->image == nullptr) continue;
        for (uint32_t j = 0; j < mod->type_count; j++) {
            if (mod->type_info_ptrs[j] != nullptr &&
                mod->type_info_ptrs[j]->stable_id == stable_id) {
                return EncodeReflectionQueryTypeHandle(mod->image->types[j]);
            }
        }
    }
    return 0;
}

// ── Stable-id resolution fallback ───────────────────────────────────
// Computes the TypeInfo stable_id from a descriptor's subject_id.
// Used when type_info_ptrs is not available for a handle.

static uint64_t ComputeStableIdFromHandle(CHAOS_IL2CPP_INTPTR handle) noexcept {
    auto* desc = GetTypeDescriptorFromHandle(handle);
    if (desc != nullptr && desc->subject_id_utf8 != nullptr) {
        return chaos_compute_type_stable_id(desc->subject_id_utf8);
    }
    return 0;
}

}  // anonymous namespace
}  // namespace chaos::il2cpp::runtime_core