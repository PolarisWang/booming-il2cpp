// =====================================================================
// Type hierarchy — Phase 3
// =====================================================================

extern "C" {
namespace chaos::il2cpp::runtime_core {

// ── IsSubclassOf (pointer-based internal helper) ───────────────────
// Returns 1 if `type` is a subclass of `base`, 0 otherwise.
// Both pointers must be non-null and already resolved from handles.
// Used directly by IsAssignableFrom to avoid redundant handle resolution.
static CHAOS_IL2CPP_INTPTR IsSubclassOfPtr(
    const TypeInfoHot* type_info,
    const TypeInfoHot* base_info) noexcept
{
    if (type_info == base_info) return 0;  // same type is not subclass
    while (type_info->parent != nullptr) {
        if (type_info->parent == base_info) return 1;
        type_info = type_info->parent;
    }
    return 0;
}

// ── IsSubclassOf (handle-based public API) ─────────────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionIsSubclassOf(
    CHAOS_IL2CPP_INTPTR type_handle,
    CHAOS_IL2CPP_INTPTR base_handle) noexcept
{
    if (type_handle == 0 || base_handle == 0) return 0;
    if (type_handle == base_handle) return 0;

    const auto* type_info = GetTypeInfoFromHandle(type_handle);
    const auto* base_info = GetTypeInfoFromHandle(base_handle);
    if (type_info == nullptr || base_info == nullptr) return 0;

    return IsSubclassOfPtr(type_info, base_info);
}

// ── IsAssignableFrom ───────────────────────────────────────────────
// Returns 1 if `target.IsAssignableFrom(source)`, i.e. source can be
// assigned to a variable of target type.  Checks:
//   1. Subclass relationship (source is subclass of target)
//   2. Interface implementation (target is interface, source implements it)
CHAOS_IL2CPP_INTPTR ChaosReflectionIsAssignableFrom(
    CHAOS_IL2CPP_INTPTR target_handle,
    CHAOS_IL2CPP_INTPTR source_handle) noexcept
{
    if (target_handle == 0 || source_handle == 0) return 0;
    if (target_handle == source_handle) return 1;

    // Check subclass relationship
    if (ChaosReflectionIsSubclassOf(source_handle, target_handle)) return 1;

    // Interface check: target must be an interface type
    const auto* target_info = GetTypeInfoFromHandle(target_handle);
    const auto* source_info = GetTypeInfoFromHandle(source_handle);
    if (target_info == nullptr || source_info == nullptr) return 0;
    if (target_info->type_shape != chaos_type_shape_interface) return 0;

    uint64_t target_stable = target_info->stable_id;

    // Walk source's parent chain checking each type's iface_map
    const auto* walk = source_info;
    while (walk != nullptr) {
        const auto* warm = GetWarmPtr(walk);
        // Bloom filter: skip static iface_map scan if bitmap says miss
        if (IfaceBitmapMaybeContains(warm, target_stable)) {
            if (warm->iface_map != nullptr) {
                for (uint32_t i = 0; i < warm->iface_count; i++) {
                    if (warm->iface_map[i].iface_stable_id == target_stable) return 1;
                }
            }
        }
        if (warm->runtime_iface_map != nullptr) {
            for (uint32_t i = 0; i < warm->runtime_iface_count; i++) {
                if (warm->runtime_iface_map[i].iface_stable_id == target_stable) return 1;
            }
        }
        walk = walk->parent;
    }
    return 0;
}

// ── IsInstanceOfType ───────────────────────────────────────────────
// Returns 1 if `obj` is an instance of `type` (or a subclass/implementor).
// Reads TypeInfoHot* from the object header, compares via parent / iface chain.
CHAOS_IL2CPP_INTPTR ChaosReflectionIsInstanceOfType(
    CHAOS_IL2CPP_INTPTR obj,
    CHAOS_IL2CPP_INTPTR type_handle) noexcept
{
    if (obj == 0 || type_handle == 0) return 0;

    const auto* obj_type = chaos_object_get_type_info(
        reinterpret_cast<const void*>(static_cast<CHAOS_IL2CPP_INTPTR>(obj)));
    if (obj_type == nullptr) return 0;

    const auto* target_info = GetTypeInfoFromHandle(type_handle);
    if (target_info == nullptr) return 0;

    // Same type pointer → instance
    if (obj_type == target_info) return 1;

    // Walk parent chain
    const auto* walk = obj_type;
    while (walk->parent != nullptr) {
        if (walk->parent == target_info) return 1;
        walk = walk->parent;
    }

    // Interface check
    if (target_info->type_shape == chaos_type_shape_interface) {
        uint64_t target_stable = target_info->stable_id;
        walk = obj_type;
        while (walk != nullptr) {
            const auto* warm = GetWarmPtr(walk);
            // Bloom filter: skip static iface_map scan if bitmap says miss
            if (IfaceBitmapMaybeContains(warm, target_stable)) {
                if (warm->iface_map != nullptr) {
                    for (uint32_t i = 0; i < warm->iface_count; i++) {
                        if (warm->iface_map[i].iface_stable_id == target_stable) return 1;
                    }
                }
            }
            if (warm->runtime_iface_map != nullptr) {
                for (uint32_t i = 0; i < warm->runtime_iface_count; i++) {
                    if (warm->runtime_iface_map[i].iface_stable_id == target_stable) return 1;
                }
            }
            walk = walk->parent;
        }
    }

    return 0;
}

// ── IsAssignableTo ─────────────────────────────────────────────────
// Delegates to IsAssignableFrom with swapped args:
//   source.IsAssignableTo(target) === target.IsAssignableFrom(source)
CHAOS_IL2CPP_INTPTR ChaosReflectionIsAssignableTo(
    CHAOS_IL2CPP_INTPTR target_handle,
    CHAOS_IL2CPP_INTPTR source_handle) noexcept
{
    return ChaosReflectionIsAssignableFrom(source_handle, target_handle);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetInterfaces(CHAOS_IL2CPP_INTPTR type_handle) {
    using namespace chaos::il2cpp::runtime_core;
    const auto* type_info = GetTypeInfoFromHandle(type_handle);
    if (type_info == nullptr) return 0;

    // ReturnValueBuffer: [0]=count, [1..N]=handles (up to 32 interfaces)
    static CHAOS_IL2CPP_INTPTR s_buffer[33];
    uint32_t count = 0;

    auto try_add = [&](uint64_t stable_id) {
        if (count >= 32) return;
        TypeInfoHandle h = FindTypeHandleByStableId(stable_id);
        if (h != 0) { s_buffer[1 + count++] = static_cast<CHAOS_IL2CPP_INTPTR>(h); }
    };

    const auto* type_info_warm = GetWarmPtr(type_info);
    if (type_info_warm->iface_map != nullptr) {
        for (uint32_t i = 0; i < type_info_warm->iface_count; i++) {
            try_add(type_info_warm->iface_map[i].iface_stable_id);
        }
    }
    if (type_info_warm->runtime_iface_map != nullptr) {
        for (uint32_t i = 0; i < type_info_warm->runtime_iface_count; i++) {
            try_add(type_info_warm->runtime_iface_map[i].iface_stable_id);
        }
    }

    s_buffer[0] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<intptr_t>(count));
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_buffer);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetGenericParamConstraints(CHAOS_IL2CPP_INTPTR type_handle) noexcept {
    if (type_handle == 0) return 0;

    // Generic params are ReflectionQuery-encoded descriptors with subject_id like
    // "Namespace.Type`N/!M". Need owning type + param index for constraint lookup.
    auto* desc = TryDecodeReflectionQueryHandle<ReflectionQueryTypeDescriptor>(
        static_cast<TypeInfoHandle>(type_handle));
    if (desc == nullptr || desc->subject_id_utf8 == nullptr) return 0;

    // Parse "!N" suffix to extract param_index
    const char* bang = std::strrchr(desc->subject_id_utf8, '!');
    if (bang == nullptr) return 0;
    int param_index = 0;
    for (const char* p = bang + 1; *p >= '0' && *p <= '9'; p++) {
        param_index = param_index * 10 + (*p - '0');
    }
    if (param_index < 0 || param_index > 7) return 0;

    // subject_id = "Namespace.Type`1/!0"  → owning_type_subject = "Namespace.Type`1"
    ptrdiff_t owning_len = bang - desc->subject_id_utf8 - 1; // skip the '/'
    if (owning_len <= 0) return 0;

    // Scan all modules looking for the owning type
    uint32_t module_count = GetModuleCount();
    for (uint32_t mid = 0; mid < module_count; mid++) {
        const auto* mod = GetModuleByIndex(mid);
        if (mod == nullptr || mod->image == nullptr || mod->image->types == nullptr) continue;
        if (mod->generic_param_constraint_data == nullptr) continue;

        for (uint32_t ti = 0; ti < mod->image->type_count; ti++) {
            auto* t = mod->image->types[ti];
            if (t == nullptr || t->subject_id_utf8 == nullptr) continue;
            if (static_cast<ptrdiff_t>(std::strlen(t->subject_id_utf8)) != owning_len) continue;
            if (std::strncmp(t->subject_id_utf8, desc->subject_id_utf8, static_cast<size_t>(owning_len)) == 0) {
                // Found owning type at index ti in module mid. Look up constraints.
                uint32_t start = mod->generic_param_constraint_offset[ti];
                uint32_t end = mod->generic_param_constraint_offset[ti + 1];
                if (start == end) return 0;

                uint32_t matches[32];
                uint32_t match_count = 0;
                for (uint32_t ci = start; ci < end && match_count < 32; ci++) {
                    uint32_t entry = mod->generic_param_constraint_data[ci];
                    if (static_cast<int>((entry >> 29) & 0x7u) == param_index) {
                        matches[match_count++] = entry & 0x1FFFFFFFu;
                    }
                }
                if (match_count == 0) return 0;

                static CHAOS_IL2CPP_INTPTR buffer[33];
                buffer[0] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<intptr_t>(match_count));
                for (uint32_t i = 0; i < match_count; i++) {
                    buffer[1 + i] = static_cast<CHAOS_IL2CPP_INTPTR>(
                        MakeTypeHandle(mid, matches[i]));
                }
                return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(buffer);
            }
        }
    }
    return 0;
}

}  // namespace chaos::il2cpp::runtime_core
}  // extern "C"