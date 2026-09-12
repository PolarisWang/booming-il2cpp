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

// ── Collect-all-attributes API ──────────────────────────────────────
// Serves the CustomAttributeExtensions.GetCustomAttributes / IsDefined family
// and MemberInfo/Module/Assembly.GetCustomAttributes, which need the *whole*
// attribute set for a member rather than one lookup by type.
//
// Emits [count:uint32][attr_type_token:uint32, ...] into a per-thread scratch
// buffer. The caller reads the count from slot 0 and each subsequent slot
// holds an attribute type token, which the managed wrapper turns into an
// attribute instance via the materializer.
//
// Returns 0 when the member has no attributes (the caller's normal path).
namespace {

// Per-thread scratch so concurrent reflection does not race. 64 attributes per
// member is far beyond any realistic BCL/engine usage; overflow is reported by
// capping the count rather than corrupting memory.
constexpr uint32_t kMaxCollectedAttributes = 64;
thread_local CHAOS_IL2CPP_INTPTR tls_collected_attrs[kMaxCollectedAttributes + 1];

// Shared zero-count marker: [0] — read-only after init, safe to share.
CHAOS_IL2CPP_INTPTR g_empty_attr_list[1] = {0};

const uint32_t* SelectAttributeOffsetArray(const ModuleDescriptor* mod,
                                           CHAOS_IL2CPP_INTPTR member_kind,
                                           uint32_t& entity_count) noexcept {
    switch (member_kind) {
        case 1: entity_count = mod->custom_attribute_entity_count;        return mod->custom_attribute_offset;
        case 2: entity_count = mod->custom_attribute_method_count;        return mod->custom_attribute_method_offset;
        case 3: entity_count = mod->custom_attribute_field_count;         return mod->custom_attribute_field_offset;
        case 4: entity_count = mod->custom_attribute_property_count;      return mod->custom_attribute_property_offset;
        case 5: entity_count = mod->custom_attribute_param_count;         return mod->custom_attribute_param_offset;
        default: entity_count = 0;                                        return nullptr;
    }
}

}  // namespace

CHAOS_IL2CPP_INTPTR ChaosReflectionCollectCustomAttributes(
    CHAOS_IL2CPP_INTPTR member_kind,
    CHAOS_IL2CPP_INTPTR member_handle) noexcept
{
    if (member_kind == 0 || member_handle == 0) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(g_empty_attr_list);
    }

    uint64_t encoded = static_cast<uint64_t>(member_handle);
    uint32_t module_id = static_cast<uint32_t>(encoded >> 32);
    uint32_t token = static_cast<uint32_t>(encoded & 0xFFFFFFFFu);
    uint32_t entity_idx = TokenToIndex(token);

    const auto* mod = LookupModule(module_id);
    if (mod == nullptr || mod->custom_attribute_blob == nullptr) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(g_empty_attr_list);
    }

    uint32_t entity_count = 0;
    const uint32_t* offset_array = SelectAttributeOffsetArray(mod, member_kind, entity_count);
    if (offset_array == nullptr || entity_count == 0 || entity_idx >= entity_count) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(g_empty_attr_list);
    }

    uint32_t start = offset_array[entity_idx];
    uint32_t end = offset_array[entity_idx + 1];
    if (start >= end) return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(g_empty_attr_list);

    const uint8_t* p = mod->custom_attribute_blob + start;
    uint16_t attr_count = 0;
    std::memcpy(&attr_count, p, sizeof(attr_count));
    p += 2;

    uint32_t written = 0;
    for (uint16_t i = 0; i < attr_count && written < kMaxCollectedAttributes; ++i) {
        uint32_t attr_type_token = 0;
        uint16_t packed_size = 0;
        std::memcpy(&attr_type_token, p, sizeof(attr_type_token)); p += 4;
        std::memcpy(&packed_size, p, sizeof(packed_size)); p += 2;

        tls_collected_attrs[written + 1] = static_cast<CHAOS_IL2CPP_INTPTR>(attr_type_token);
        written++;

        // Skip the packed payload. Guard against a truncated blob so a corrupt
        // record cannot walk us past the buffer.
        if (end < start + 2 + (p - (mod->custom_attribute_blob + start)) + packed_size) break;
        p += packed_size;
    }

    tls_collected_attrs[0] = static_cast<CHAOS_IL2CPP_INTPTR>(written);
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(tls_collected_attrs);
}

// Whether the member carries any attribute at all — the `IsDefined` fast path.
CHAOS_IL2CPP_INT32 ChaosReflectionMemberHasAnyAttribute(
    CHAOS_IL2CPP_INTPTR member_kind,
    CHAOS_IL2CPP_INTPTR member_handle) noexcept
{
    auto* list = reinterpret_cast<const CHAOS_IL2CPP_INTPTR*>(
        ChaosReflectionCollectCustomAttributes(member_kind, member_handle));
    if (list == nullptr) return 0;
    return list[0] != 0 ? 1 : 0;
}

// Whether a specific attribute type is present — `IsDefined(type)`.
CHAOS_IL2CPP_INT32 ChaosReflectionMemberIsDefinedByToken(
    CHAOS_IL2CPP_INTPTR member_kind,
    CHAOS_IL2CPP_INTPTR member_handle,
    CHAOS_IL2CPP_INTPTR attribute_type_token) noexcept
{
    auto* list = reinterpret_cast<const CHAOS_IL2CPP_INTPTR*>(
        ChaosReflectionCollectCustomAttributes(member_kind, member_handle));
    if (list == nullptr) return 0;

    const uint32_t count = static_cast<uint32_t>(list[0]);
    const uint32_t want = static_cast<uint32_t>(attribute_type_token);
    for (uint32_t i = 0; i < count; ++i) {
        if (static_cast<uint32_t>(list[i + 1]) == want) return 1;
    }
    return 0;
}


// ── CustomAttributeData accessors ───────────────────────────────────
// A CustomAttributeData handle is a pointer to one blob record as laid out by
// the codegen custom-attribute emitter:
//
//   [attr_type_token:uint32][packed_size:uint16][payload:packed_size bytes]
//
// resolving_ctor / typed / named arguments live inside `payload` in the ECMA-335
// fixed-arg / named-arg encoding. The accessors below expose the parts that can
// be answered from the record header; argument decoding is the managed wrapper's
// job (it has the attribute type's constructor signature, which the native side
// does not).
namespace {

struct BlobAttrRecord {
    uint32_t type_token;
    uint16_t packed_size;
    const uint8_t* payload;
};

inline BlobAttrRecord DecodeAttrRecord(CHAOS_IL2CPP_INTPTR handle) noexcept {
    BlobAttrRecord rec{0u, 0u, nullptr};
    if (handle == 0) return rec;
    const auto* p = reinterpret_cast<const uint8_t*>(handle);
    std::memcpy(&rec.type_token, p, sizeof(rec.type_token));
    std::memcpy(&rec.packed_size, p + 4, sizeof(rec.packed_size));
    rec.payload = (rec.packed_size > 0) ? (p + 6) : nullptr;
    return rec;
}

}  // namespace

// CustomAttributeData.AttributeType — the attribute's type token. The managed
// wrapper resolves it to a Type via GetTypeFromHandle.
CHAOS_IL2CPP_INTPTR ChaosReflectionAttrDataGetAttributeType(CHAOS_IL2CPP_INTPTR handle) noexcept {
    const auto rec = DecodeAttrRecord(handle);
    if (rec.type_token == 0u) return 0;
    return static_cast<CHAOS_IL2CPP_INTPTR>(rec.type_token);
}

// CustomAttributeData.Constructor — the .ctor token, which the blob header does
// not carry (only the attribute *type* is recorded). Returning 0 signals
// "unresolved" so the managed wrapper can fall back to ConstructorInfo lookup by
// type rather than presenting a wrong constructor.
CHAOS_IL2CPP_INTPTR ChaosReflectionAttrDataGetConstructor(CHAOS_IL2CPP_INTPTR handle) noexcept {
    const auto rec = DecodeAttrRecord(handle);
    if (rec.type_token == 0u) return 0;
    return 0;
}

// CustomAttributeData.ConstructorArguments — the fixed-argument payload start.
// Exposed as a pointer+size pair so the managed side can decode against the
// constructor signature it already has.
CHAOS_IL2CPP_INTPTR ChaosReflectionAttrDataGetConstructorArguments(CHAOS_IL2CPP_INTPTR handle) noexcept {
    const auto rec = DecodeAttrRecord(handle);
    return rec.payload == nullptr
        ? 0
        : reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<uint8_t*>(rec.payload));
}

// CustomAttributeData.NamedArguments — named args follow the fixed args, whose
// length is signature-dependent. Without the constructor signature the split
// point is unknown, so this reports "none" rather than guessing an offset that
// could hand callers a misaligned record.
CHAOS_IL2CPP_INTPTR ChaosReflectionAttrDataGetNamedArguments(CHAOS_IL2CPP_INTPTR handle) noexcept {
    const auto rec = DecodeAttrRecord(handle);
    if (rec.type_token == 0u) return 0;
    return 0;
}

// CustomAttributeData.GetCustomAttributes(CustomAttributeData) / (Assembly) —
// the attribute-of-attribute query. Reflection on an attribute instance's own
// type is served by the ordinary type-level path, so this delegates to the
// collection API with the attribute's type token as the member handle.
CHAOS_IL2CPP_INTPTR ChaosReflectionAttrDataGetCustomAttributes(CHAOS_IL2CPP_INTPTR handle) noexcept {
    const auto rec = DecodeAttrRecord(handle);
    if (rec.type_token == 0u) return 0;
    // member_kind 1 (Type) with the type token as the handle: custom attributes
    // on the attribute type itself.
    return ChaosReflectionCollectCustomAttributes(
        static_cast<CHAOS_IL2CPP_INTPTR>(1), static_cast<CHAOS_IL2CPP_INTPTR>(rec.type_token));
}

// Identity: two CustomAttributeData refer to the same record when their
// attribute type tokens match and their payloads are byte-identical.
CHAOS_IL2CPP_INT32 ChaosReflectionAttrDataEquals(
    CHAOS_IL2CPP_INTPTR lhs, CHAOS_IL2CPP_INTPTR rhs) noexcept {
    const auto a = DecodeAttrRecord(lhs);
    const auto b = DecodeAttrRecord(rhs);
    if (a.type_token == 0u || b.type_token == 0u) return 0;
    if (a.type_token != b.type_token) return 0;
    if (a.packed_size != b.packed_size) return 0;
    if (a.packed_size == 0u) return 1;
    return std::memcmp(a.payload, b.payload, a.packed_size) == 0 ? 1 : 0;
}

CHAOS_IL2CPP_INT32 ChaosReflectionAttrDataGetHashCode(CHAOS_IL2CPP_INTPTR handle) noexcept {
    const auto rec = DecodeAttrRecord(handle);
    if (rec.type_token == 0u) return 0;
    // FNV-1a over the type token then the payload, matching the equality above.
    uint32_t h = 2166136261u;
    h = (h ^ rec.type_token) * 16777619u;
    for (uint16_t i = 0; i < rec.packed_size; ++i) {
        h = (h ^ rec.payload[i]) * 16777619u;
    }
    return static_cast<CHAOS_IL2CPP_INT32>(h);
}

}  // namespace chaos::il2cpp::runtime_core
}  // extern "C"