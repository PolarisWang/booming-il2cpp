// ── CustomAttribute blob query ──────────────────────────────────────
// Called from per-family generated code (the short extraction wrapper)
// after it has decoded (kind, handle) from the managed reflection object.

extern "C" {
namespace chaos::il2cpp::runtime_core {

CHAOS_IL2CPP_INTPTR ChaosGetCustomAttributeFromBlob(
    CHAOS_IL2CPP_INTPTR member_kind,
    CHAOS_IL2CPP_INTPTR member_handle,
    CHAOS_IL2CPP_INTPTR attr_type_handle) noexcept
{
    if (member_kind == 0 || member_handle == 0 || attr_type_handle == 0) return 0;

    // Phase 1: only Type kind (kind == 1). Method kind deferred.
    if (member_kind != 1) return 0;

    TypeInfoHandle handle = static_cast<TypeInfoHandle>(member_handle);
    uint32_t module_id = GetModuleId(handle);
    uint32_t token = GetTypeToken(handle);
    uint32_t entity_idx = TokenToIndex(token);

    const auto* mod = LookupModule(module_id);
    if (mod == nullptr || mod->custom_attribute_blob == nullptr) return 0;
    if (entity_idx >= mod->custom_attribute_entity_count) return 0;

    uint32_t start = mod->custom_attribute_offset[entity_idx];
    uint32_t end = mod->custom_attribute_offset[entity_idx + 1];
    if (start >= end) return 0;  // no attributes for this entity

    uint32_t target_attr_token = GetTypeToken(attr_type_handle);

    // Parse blob: [attr_count:uint16] then per-attribute records
    const uint8_t* p = mod->custom_attribute_blob + start;
    uint16_t attr_count;
    std::memcpy(&attr_count, p, sizeof(attr_count)); p += 2;

    for (uint16_t i = 0; i < attr_count; ++i) {
        uint32_t attr_type_token;
        uint16_t packed_size;
        std::memcpy(&attr_type_token, p, sizeof(attr_type_token)); p += 4;
        std::memcpy(&packed_size, p, sizeof(packed_size)); p += 2;

        if (attr_type_token == target_attr_token) {
            // Found matching attribute — call materializer
            if (mod->custom_attribute_materializer != nullptr) {
                return mod->custom_attribute_materializer(
                    attr_type_token, packed_size > 0 ? p : nullptr);
            }
            // No materializer registered — return sentinel non-null to
            // signal "attribute exists" (no-field attributes).
            return static_cast<CHAOS_IL2CPP_INTPTR>(1);
        }
        p += packed_size;
    }

    return 0;
}

// Legacy entry point — kept as fallback for families that don't generate
// the per-family extraction wrapper. The per-family generated code
// overrides this in practice.
CHAOS_IL2CPP_INTPTR ChaosReflectionGetCustomAttribute(
    CHAOS_IL2CPP_INTPTR member_handle,
    CHAOS_IL2CPP_INTPTR attribute_type_handle)
{
    return ChaosGetCustomAttributeFromBlob(
        static_cast<CHAOS_IL2CPP_INTPTR>(1),
        member_handle,
        attribute_type_handle);
}

}  // namespace chaos::il2cpp::runtime_core
}  // extern "C"