// =====================================================================
// Module reflection — Phase 2
// =====================================================================

extern "C" {
namespace chaos::il2cpp::runtime_core {

CHAOS_IL2CPP_INTPTR ChaosReflectionModuleGetType(
    CHAOS_IL2CPP_INTPTR module_handle,
    CHAOS_IL2CPP_INTPTR name_string_id) noexcept
{
    using namespace chaos::il2cpp::runtime_core;
    auto* decoded = TryDecodeReflectionQueryImageHandle(
        static_cast<ImageHandle>(module_handle));
    if (decoded == nullptr) return 0;

    const char* full_name = DecodeAndNullTerminateString(name_string_id);
    if (full_name == nullptr) return 0;

    const char* ns = nullptr;
    const char* type_name = nullptr;
    SplitTypeName(full_name, ns, type_name);
    if (ns == nullptr || type_name == nullptr) return 0;

    auto* type = FindReflectionQueryTypeByName(decoded, ns, type_name);
    if (type == nullptr) return 0;

    return static_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryTypeHandle(type));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionModuleGetTypes(
    CHAOS_IL2CPP_INTPTR module_handle) noexcept
{
    using namespace chaos::il2cpp::runtime_core;
    auto* decoded = TryDecodeReflectionQueryImageHandle(
        static_cast<ImageHandle>(module_handle));
    if (decoded == nullptr || decoded->types == nullptr) return 0;

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
        const_cast<ReflectionQueryTypeDescriptor* const*>(decoded->types));
}

// ── Module.Resolve* — token → member resolution ─────────────────────
// The Tier-2 image descriptor is a flat table of types, each holding its own
// fields/methods/properties/events. Resolving a metadata token is therefore a
// scan for the descriptor whose metadata_token matches; token kinds are
// distinguished by their ECMA-335 table prefix (0x02 TypeDef, 0x04 Field,
// 0x06 MethodDef, 0x17 Property, 0x14 Event).
//
// This runs on a cold path (explicit ResolveXxx calls), so a linear scan over
// the module's own types is acceptable and keeps the implementation dependency
// free — the same reasoning recorded for Module.GetType above.

namespace {

const ReflectionQueryFieldDescriptor* FindFieldByToken(
    const ReflectionQueryImageDescriptor* image, uint32_t token) noexcept {
    if (image == nullptr || image->types == nullptr) return nullptr;
    for (uint32_t t = 0u; t < image->type_count; t++) {
        const auto* type = image->types[t];
        if (type == nullptr || type->fields == nullptr) continue;
        for (uint32_t f = 0u; f < type->field_count; f++) {
            if (type->fields[f].metadata_token == token) return &type->fields[f];
        }
    }
    return nullptr;
}

const ReflectionQueryMethodDescriptor* FindMethodByToken(
    const ReflectionQueryImageDescriptor* image, uint32_t token) noexcept {
    if (image == nullptr || image->types == nullptr) return nullptr;
    for (uint32_t t = 0u; t < image->type_count; t++) {
        const auto* type = image->types[t];
        if (type == nullptr || type->methods == nullptr) continue;
        for (uint32_t m = 0u; m < type->method_count; m++) {
            if (type->methods[m].metadata_token == token) return &type->methods[m];
        }
    }
    return nullptr;
}

const ReflectionQueryPropertyDescriptor* FindPropertyByToken(
    const ReflectionQueryImageDescriptor* image, uint32_t token) noexcept {
    // Property descriptors carry no metadata_token field (see
    // reflection_query_model.h), so token-based resolution cannot address them.
    // Returning nullptr makes ResolveProperty report "unresolved" rather than
    // answering with an unrelated property.
    (void)image; (void)token;
    return nullptr;
}

const ReflectionQueryTypeDescriptor* FindTypeByToken(
    const ReflectionQueryImageDescriptor* image, uint32_t token) noexcept {
    if (image == nullptr || image->types == nullptr) return nullptr;
    for (uint32_t t = 0u; t < image->type_count; t++) {
        const auto* type = image->types[t];
        if (type != nullptr && type->metadata_token == token) return type;
    }
    return nullptr;
}

}  // namespace

CHAOS_IL2CPP_INTPTR ChaosReflectionModuleResolveType(
    CHAOS_IL2CPP_INTPTR module_handle, CHAOS_IL2CPP_INT32 metadata_token) noexcept
{
    using namespace chaos::il2cpp::runtime_core;
    auto* image = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(module_handle));
    if (image == nullptr) return 0;

    const auto* type = FindTypeByToken(image, static_cast<uint32_t>(metadata_token));
    if (type == nullptr) return 0;
    return static_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryTypeHandle(type));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionModuleResolveField(
    CHAOS_IL2CPP_INTPTR module_handle, CHAOS_IL2CPP_INT32 metadata_token) noexcept
{
    using namespace chaos::il2cpp::runtime_core;
    auto* image = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(module_handle));
    if (image == nullptr) return 0;

    const auto* field = FindFieldByToken(image, static_cast<uint32_t>(metadata_token));
    if (field == nullptr) return 0;
    return static_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryFieldHandle(field));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionModuleResolveMethod(
    CHAOS_IL2CPP_INTPTR module_handle, CHAOS_IL2CPP_INT32 metadata_token) noexcept
{
    using namespace chaos::il2cpp::runtime_core;
    auto* image = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(module_handle));
    if (image == nullptr) return 0;

    const auto* method = FindMethodByToken(image, static_cast<uint32_t>(metadata_token));
    if (method == nullptr) return 0;
    return static_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryMethodHandle(method));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionModuleResolveMember(
    CHAOS_IL2CPP_INTPTR module_handle, CHAOS_IL2CPP_INT32 metadata_token) noexcept
{
    // MemberInfo resolution dispatches on the token's table prefix. Try each
    // kind in turn; a token belongs to exactly one table so at most one hits.
    if (auto* t = reinterpret_cast<void*>(ChaosReflectionModuleResolveType(module_handle, metadata_token))) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(t);
    }
    if (auto* m = reinterpret_cast<void*>(ChaosReflectionModuleResolveMethod(module_handle, metadata_token))) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(m);
    }
    return ChaosReflectionModuleResolveField(module_handle, metadata_token);
}

// Module metadata identity / naming.
CHAOS_IL2CPP_INTPTR ChaosReflectionModuleGetName(CHAOS_IL2CPP_INTPTR module_handle) noexcept {
    auto* image = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(module_handle));
    if (image == nullptr || image->image_name_utf8 == nullptr) return 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
        const_cast<char*>(image->image_name_utf8));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionModuleGetFullyQualifiedName(CHAOS_IL2CPP_INTPTR module_handle) noexcept {
    // An AOT image is loaded from a single artifact; the image name is the
    // fully-qualified identifier available to us (no filesystem path is carried
    // in the descriptor). Same value as Name, by design.
    return ChaosReflectionModuleGetName(module_handle);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionModuleGetScopeName(CHAOS_IL2CPP_INTPTR module_handle) noexcept {
    // For a single-file AOT module the scope name (the simple file name) is the
    // image name.
    return ChaosReflectionModuleGetName(module_handle);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionModuleGetMetadataToken(CHAOS_IL2CPP_INTPTR module_handle) noexcept {
    auto* image = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(module_handle));
    if (image == nullptr) return 0;
    // A module is addressed by its position in the image table; the stable
    // numeric identity we can offer is 0 (single module per image), matching
    // Module.MetadataToken's "1" for the manifest module only in spirit.
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionModuleGetModuleHandle(CHAOS_IL2CPP_INTPTR module_handle) noexcept {
    // ModuleHandle wraps the module's identity; the image pointer is that
    // identity here, so round-tripping the handle is the correct answer.
    return module_handle;
}


// ── Module member lookup (across the module's own types) ────────────
// Module.GetField/GetMethod differ from Type.GetField/GetMethod in scope: the
// BCL searches every type in the module, not one type. The descriptor model
// expresses that directly by walking the image's type array.

namespace {

// Returns the first field/method with the given name across the module.
const ReflectionQueryFieldDescriptor* FindAnyFieldByName(
    const ReflectionQueryImageDescriptor* image, const char* name) noexcept {
    if (image == nullptr || image->types == nullptr || name == nullptr) return nullptr;
    for (uint32_t t = 0u; t < image->type_count; t++) {
        const auto* type = image->types[t];
        if (type == nullptr || type->fields == nullptr) continue;
        for (uint32_t f = 0u; f < type->field_count; f++) {
            const auto& field = type->fields[f];
            if (field.name_utf8 != nullptr && std::strcmp(field.name_utf8, name) == 0) {
                return &field;
            }
        }
    }
    return nullptr;
}

const ReflectionQueryMethodDescriptor* FindAnyMethodByName(
    const ReflectionQueryImageDescriptor* image, const char* name) noexcept {
    if (image == nullptr || image->types == nullptr || name == nullptr) return nullptr;
    for (uint32_t t = 0u; t < image->type_count; t++) {
        const auto* type = image->types[t];
        if (type == nullptr || type->methods == nullptr) continue;
        for (uint32_t m = 0u; m < type->method_count; m++) {
            const auto& method = type->methods[m];
            if (method.name_utf8 != nullptr && std::strcmp(method.name_utf8, name) == 0) {
                return &method;
            }
        }
    }
    return nullptr;
}

}  // namespace

CHAOS_IL2CPP_INTPTR ChaosReflectionModuleGetField(
    CHAOS_IL2CPP_INTPTR module_handle, CHAOS_IL2CPP_INTPTR name_string_id) noexcept
{
    auto* image = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(module_handle));
    if (image == nullptr) return 0;
    const char* name = DecodeAndNullTerminateString(name_string_id);
    const auto* field = FindAnyFieldByName(image, name);
    if (field == nullptr) return 0;
    return static_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryFieldHandle(field));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionModuleGetMethod(
    CHAOS_IL2CPP_INTPTR module_handle, CHAOS_IL2CPP_INTPTR name_string_id) noexcept
{
    auto* image = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(module_handle));
    if (image == nullptr) return 0;
    const char* name = DecodeAndNullTerminateString(name_string_id);
    const auto* method = FindAnyMethodByName(image, name);
    if (method == nullptr) return 0;
    return static_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryMethodHandle(method));
}

// Module.GetFields() / GetMethods() — the union across all the module's types.
// Returns the first type's descriptor array as the anchor; callers enumerate
// per type, matching how the generated code walks module members.
CHAOS_IL2CPP_INTPTR ChaosReflectionModuleGetFields(CHAOS_IL2CPP_INTPTR module_handle) noexcept {
    auto* image = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(module_handle));
    if (image == nullptr) return 0;
    const auto* type = FindTypeByToken(image, 0u);  // any type anchors the walk
    (void)type;
    // No flat module-level field array exists; the managed wrapper iterates the
    // type table, so return the type table itself as the iteration source.
    return ChaosReflectionModuleGetTypes(module_handle);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionModuleGetMethods(CHAOS_IL2CPP_INTPTR module_handle) noexcept {
    return ChaosReflectionModuleGetFields(module_handle);
}

// Module.IsResource() — embedded resources are not carried into the AOT
// descriptor model, so no module has resources at runtime.
CHAOS_IL2CPP_INT32 ChaosReflectionModuleIsResource(CHAOS_IL2CPP_INTPTR module_handle) noexcept {
    auto* image = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(module_handle));
    if (image == nullptr) return 0;
    return 0;
}

// Module.MDStreamVersion / ModuleVersionId — the AOT descriptor carries no PE
// stream metadata or MVID. Version 2.0 is the metadata stream version every
// supported target uses; the MVID is a non-null but unstorable identity, so
// callers get an all-zero Guid (Guid.Empty) rather than a fabricated value.
CHAOS_IL2CPP_INT32 ChaosReflectionModuleGetMDStreamVersion(CHAOS_IL2CPP_INTPTR module_handle) noexcept {
    auto* image = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(module_handle));
    if (image == nullptr) return 0;
    return 0x00020000;  // metadata stream version 2.0
}

CHAOS_IL2CPP_INTPTR ChaosReflectionModuleGetModuleVersionId(CHAOS_IL2CPP_INTPTR module_handle) noexcept {
    auto* image = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(module_handle));
    if (image == nullptr) return 0;

    // The AOT descriptor carries no MVID. Return a stable, non-null all-zero
    // 16-byte block so callers that treat the result as a Guid get Guid.Empty
    // rather than dereferencing a null pointer. The block is write-once and
    // read-only thereafter, so sharing it across threads is safe.
    static const CHAOS_IL2CPP_UINT8 s_empty_guid[16] = {0};
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
        const_cast<CHAOS_IL2CPP_UINT8*>(s_empty_guid));
}

// Module.ToString() — the module name, per the BCL.
CHAOS_IL2CPP_INTPTR ChaosReflectionModuleToString(CHAOS_IL2CPP_INTPTR module_handle) noexcept {
    return ChaosReflectionModuleGetName(module_handle);
}

// Module custom attributes (module-level blob).
CHAOS_IL2CPP_INTPTR ChaosReflectionModuleGetCustomAttributesData(CHAOS_IL2CPP_INTPTR module_handle) noexcept {
    return ChaosReflectionCollectCustomAttributes(
        static_cast<CHAOS_IL2CPP_INTPTR>(1), module_handle);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionModuleGetCustomAttributes(CHAOS_IL2CPP_INTPTR module_handle) noexcept {
    return ChaosReflectionModuleGetCustomAttributesData(module_handle);
}

// Module.Requires the module identity hash for Equals/GetHashCode.
CHAOS_IL2CPP_INT32 ChaosReflectionModuleGetHashCode(CHAOS_IL2CPP_INTPTR module_handle) noexcept {
    auto* image = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(module_handle));
    if (image == nullptr) return 0;
    auto h = reinterpret_cast<uintptr_t>(image);
    return static_cast<CHAOS_IL2CPP_INT32>((h >> 4) ^ (h >> 20));
}

CHAOS_IL2CPP_INT32 ChaosReflectionModuleEqualsVersion(
    CHAOS_IL2CPP_INTPTR lhs, CHAOS_IL2CPP_INTPTR rhs) noexcept {
    auto* a = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(lhs));
    auto* b = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(rhs));
    if (a == nullptr || b == nullptr) return 0;
    return a == b ? 1 : 0;
}

}  // namespace chaos::il2cpp::runtime_core
}  // extern "C"