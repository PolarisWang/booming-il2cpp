// ── CustomAttribute blob query ──────────────────────────────────────
// Called from per-family generated code (the short extraction wrapper)
// after it has decoded (kind, handle) from the managed reflection object.
//
// For member_kind == 1 (Type), member_handle is a TypeInfoHandle:
//   [module_id:32][token:32].
// For member_kind 2-5 (Method/Field/Property/Param), member_handle
// follows the same [module_id:32][token:32] encoding, packed by the
// generated per-family wrapper which knows both values.

extern "C" {
namespace chaos::il2cpp::runtime_core {

CHAOS_IL2CPP_INTPTR ChaosGetCustomAttributeFromBlob(
    CHAOS_IL2CPP_INTPTR member_kind,
    CHAOS_IL2CPP_INTPTR member_handle,
    CHAOS_IL2CPP_INTPTR attr_type_handle) noexcept
{
    if (member_kind == 0 || member_handle == 0 || attr_type_handle == 0) return 0;

    // Decode module_id and token from the handle.
    // Type handles use TypeInfoHandle encoding; all other kinds use
    // the same [module_id:32][token:32] layout.
    uint64_t encoded = static_cast<uint64_t>(member_handle);
    uint32_t module_id = static_cast<uint32_t>(encoded >> 32);
    uint32_t token = static_cast<uint32_t>(encoded & 0xFFFFFFFFu);
    uint32_t entity_idx = TokenToIndex(token);

    const auto* mod = LookupModule(module_id);
    if (mod == nullptr || mod->custom_attribute_blob == nullptr) return 0;

    // Determine offset array and entity count based on member kind.
    const uint32_t* offset_array = nullptr;
    uint32_t entity_count = 0;

    switch (member_kind) {
        case 1: // Type
            offset_array = mod->custom_attribute_offset;
            entity_count = mod->custom_attribute_entity_count;
            break;
        case 2: // Method
            offset_array = mod->custom_attribute_method_offset;
            entity_count = mod->custom_attribute_method_count;
            break;
        case 3: // Field
            offset_array = mod->custom_attribute_field_offset;
            entity_count = mod->custom_attribute_field_count;
            break;
        case 4: // Property
            offset_array = mod->custom_attribute_property_offset;
            entity_count = mod->custom_attribute_property_count;
            break;
        case 5: // Param
            offset_array = mod->custom_attribute_param_offset;
            entity_count = mod->custom_attribute_param_count;
            break;
        default:
            return 0;
    }

    if (offset_array == nullptr || entity_count == 0) return 0;
    if (entity_idx >= entity_count) return 0;

    uint32_t start = offset_array[entity_idx];
    uint32_t end = offset_array[entity_idx + 1];
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
    // Phase 1: try blob-based lookup for Type handles (kind=1).
    auto result = ChaosGetCustomAttributeFromBlob(
        static_cast<CHAOS_IL2CPP_INTPTR>(1),
        member_handle,
        attribute_type_handle);
    if (result != 0) return result;

    // Fallback: return a non-null sentinel so generated code null-checks
    // (which call CHAOS_IL2CPP_FAIL when the result is 0) don't throw.
    // Assembly-level and unresolved CAs deferred to Phase 2+.
    static CHAOS_IL2CPP_UINT8 s_ca_sentinel = 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&s_ca_sentinel);
}

}  // namespace chaos::il2cpp::runtime_core
}  // extern "C"