// reflection_api.cpp — Native AOT reflection API implementation
//
// Provides extern "C" implementations for chaos_reflection_* functions
// called from generated C++ code. Uses Module Registry + Two-Tier Metadata
#include <chaos/trace.h>
//
// for type/method/field queries:
//
//   - Given a (module_id, token) encoded TypeInfoHandle, the registry
//     locates the module descriptor, then the Tier 2 image descriptor
//     for full type/method/field metadata.
//   - Legacy ReflectionQuery encoded handles (tag bit 63) are decoded
//     directly to constexpr ReflectionQueryTypeDescriptor pointers.
//   - Raw metadata tokens fall back to the aot_metadata shared tables.
//
// These functions are called via SimpleForward dispatch from generated code,
// NOT through the bridge vtable.

#include "runtime_core.h"
#include "module_registry.h"
#include "reflection_query_model.h"
#include "reflection_metadata_impl.h"
#include "string_table.h"
#include "runtime_instantiation.h"
#include "generic_context.h"

#include <cstring>
#include <fmt/format.h>

namespace chaos::il2cpp::runtime_core {

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

}  // namespace chaos::il2cpp::runtime_core

// =====================================================================
// Type resolution
// =====================================================================
// Functions are declared in reflection_api.h as
//   extern "C" { namespace chaos::il2cpp::runtime_core { ... } }
// with using-declarations at global scope for generated code.
// Define them in the same linkage/namespace scope to avoid C2883.

extern "C" {
namespace chaos::il2cpp::runtime_core {

CHAOS_IL2CPP_INTPTR ChaosReflectionGetTypeFromHandle(CHAOS_IL2CPP_INTPTR runtime_type_handle) {
    if (runtime_type_handle == 0) return 0;

    uint32_t token = DecodeMetadataToken(runtime_type_handle);
    auto* typeDesc = aot_metadata::FindTypeByMetadataToken(token);
    if (typeDesc == nullptr) return 0;

    return static_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryTypeHandle(typeDesc));
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

    return 0;
}

// =====================================================================
// Type properties — Module Registry / Image queries
// =====================================================================

CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssembly(CHAOS_IL2CPP_INTPTR type_handle) {
    // Resolve the image from the type handle via Module Registry
    auto* image = GetImageFromTypeHandle(type_handle);
    if (image == nullptr) {
        // Fallback: CoreLib shared metadata
        image = &aot_metadata::kImageCoreLib;
    }

    return static_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryImageHandle(image));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssemblyName(CHAOS_IL2CPP_INTPTR assembly_handle) {
    using namespace chaos::il2cpp::runtime_core;

    auto* decoded = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(assembly_handle));
    if (decoded == nullptr) {
        // Fallback: CoreLib shared metadata
        decoded = &aot_metadata::kImageCoreLib;
    }

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<char*>(decoded->image_name_utf8));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetDeclaringType(CHAOS_IL2CPP_INTPTR type_handle) {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr) return 0;

    return static_cast<CHAOS_IL2CPP_INTPTR>(
        desc->generic_type_definition != nullptr
            ? EncodeReflectionQueryTypeHandle(desc->generic_type_definition)
            : static_cast<TypeInfoHandle>(0));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetMemberName(CHAOS_IL2CPP_INTPTR member_handle) {
    using namespace chaos::il2cpp::runtime_core;
    if (member_handle == 0) return 0;

    // Try decoding as each descriptor type
    auto* typeDesc = TryDecodeReflectionQueryHandle<ReflectionQueryTypeDescriptor>(static_cast<TypeInfoHandle>(member_handle));
    if (typeDesc != nullptr) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<char*>(typeDesc->name_utf8));
    }

    auto* methodDesc = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(static_cast<MethodInfoHandle>(member_handle));
    if (methodDesc != nullptr) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<char*>(methodDesc->name_utf8));
    }

    auto* fieldDesc = TryDecodeReflectionQueryHandle<ReflectionQueryFieldDescriptor>(static_cast<FieldInfoHandle>(member_handle));
    if (fieldDesc != nullptr) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<char*>(fieldDesc->name_utf8));
    }

    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetParameters(CHAOS_IL2CPP_INTPTR method_handle) {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(static_cast<MethodInfoHandle>(method_handle));
    if (desc == nullptr) return 0;

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<ReflectionQueryParameterDescriptor*>(desc->parameters));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetParameterName(
    CHAOS_IL2CPP_INTPTR parameter_handle)
{
    using namespace chaos::il2cpp::runtime_core;
    auto* paramDesc = reinterpret_cast<const ReflectionQueryParameterDescriptor*>(parameter_handle);
    if (paramDesc == nullptr) return 0;

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<char*>(paramDesc->name_utf8));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetMethodHandle(CHAOS_IL2CPP_INTPTR method_handle) {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(static_cast<MethodInfoHandle>(method_handle));
    if (desc == nullptr) return 0;

    return static_cast<CHAOS_IL2CPP_INTPTR>(desc->metadata_token);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetMetadataToken(CHAOS_IL2CPP_INTPTR member_handle) {
    using namespace chaos::il2cpp::runtime_core;
    if (member_handle == 0) return 0;

    auto* typeDesc = TryDecodeReflectionQueryHandle<ReflectionQueryTypeDescriptor>(static_cast<TypeInfoHandle>(member_handle));
    if (typeDesc != nullptr) return static_cast<CHAOS_IL2CPP_INTPTR>(typeDesc->metadata_token);

    auto* methodDesc = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(static_cast<MethodInfoHandle>(member_handle));
    if (methodDesc != nullptr) return static_cast<CHAOS_IL2CPP_INTPTR>(methodDesc->metadata_token);

    auto* fieldDesc = TryDecodeReflectionQueryHandle<ReflectionQueryFieldDescriptor>(static_cast<FieldInfoHandle>(member_handle));
    if (fieldDesc != nullptr) return static_cast<CHAOS_IL2CPP_INTPTR>(fieldDesc->metadata_token);

    // Raw metadata token passthrough
    return member_handle;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetTypeHandle(CHAOS_IL2CPP_INTPTR type_handle) {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr) return 0;

    return static_cast<CHAOS_IL2CPP_INTPTR>(desc->metadata_token);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetConstructorsDefault(CHAOS_IL2CPP_INTPTR type_handle) noexcept {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr || desc->methods == nullptr) return 0;

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<ReflectionQueryMethodDescriptor*>(desc->methods));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetConstructors(CHAOS_IL2CPP_INTPTR type_handle, CHAOS_IL2CPP_INT32 binding_flags) {
    using namespace chaos::il2cpp::runtime_core;
    (void)binding_flags;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr || desc->methods == nullptr) return 0;

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<ReflectionQueryMethodDescriptor*>(desc->methods));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetMethods(CHAOS_IL2CPP_INTPTR type_handle) {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr || desc->methods == nullptr) return 0;

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<ReflectionQueryMethodDescriptor*>(desc->methods));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetFields(CHAOS_IL2CPP_INTPTR type_handle) {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr || desc->fields == nullptr) return 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<ReflectionQueryFieldDescriptor*>(desc->fields));
}

// =====================================================================
// Type property checks — Module Registry Tier 0 (type_flags[])
// =====================================================================

// ── Helper: return 1/0 from a Tier 0 flag check with descriptor fallback ──
#define CHAOS_DEFINE_TYPE_FLAG_STUB(Name, FlagBit)                              \
CHAOS_IL2CPP_INTPTR Name(CHAOS_IL2CPP_INTPTR type_handle) noexcept {            \
    TypeRef tr;                                                                  \
    if (ResolveTypeRef(type_handle, tr))                                         \
        return (tr.module->type_flags[tr.type_index] & (FlagBit)) != 0u ? 1 : 0;\
    return 0;                                                                    \
}

CHAOS_DEFINE_TYPE_FLAG_STUB(ChaosReflectionGetIsInterface,          kFlagIsInterface)
CHAOS_DEFINE_TYPE_FLAG_STUB(ChaosReflectionGetIsArray,              kFlagIsArray)
CHAOS_DEFINE_TYPE_FLAG_STUB(ChaosReflectionGetIsEnum,               kFlagIsEnum)
CHAOS_DEFINE_TYPE_FLAG_STUB(ChaosReflectionGetIsAbstract,           kFlagIsAbstract)
CHAOS_DEFINE_TYPE_FLAG_STUB(ChaosReflectionGetIsSealed,             kFlagIsSealed)
CHAOS_DEFINE_TYPE_FLAG_STUB(ChaosReflectionGetIsValueType,          kFlagIsValueType)
CHAOS_DEFINE_TYPE_FLAG_STUB(ChaosReflectionGetIsGenericType,        kFlagIsGenericType)
CHAOS_DEFINE_TYPE_FLAG_STUB(ChaosReflectionGetIsConstructedGeneric, kFlagIsConstructedGeneric)
CHAOS_DEFINE_TYPE_FLAG_STUB(ChaosReflectionGetIsGenericTypeDef,     kFlagIsGenericTypeDef)
CHAOS_DEFINE_TYPE_FLAG_STUB(ChaosReflectionGetIsPublic,             kFlagIsPublic)

// ── GetBaseType ────────────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionGetBaseType(CHAOS_IL2CPP_INTPTR type_handle) noexcept {
    TypeRef tr;
    if (ResolveTypeRef(type_handle, tr)) {
        uint32_t parent_token = tr.module->type_parent_tokens[tr.type_index];
        if (parent_token == 0) return 0;
        // Same-module parent: encode as (module_id << 32) | parent_token
        return static_cast<CHAOS_IL2CPP_INTPTR>(MakeTypeHandle(tr.module_id, parent_token));
    }
    return 0;
}

// ── GetNamespace ──────────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionGetNamespace(CHAOS_IL2CPP_INTPTR type_handle) noexcept {
    TypeRef tr;
    if (ResolveTypeRef(type_handle, tr)) {
        const char* ns = tr.module->type_namespaces[tr.type_index];
        if (ns == nullptr) return 0;
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<char*>(ns));
    }
    // Fallback: decode descriptor
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr || desc->namespace_name_utf8 == nullptr) return 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<char*>(desc->namespace_name_utf8));
}

// ── GetTypeFullName ──────────────────────────────────────────────
// Returns "Namespace.Name" for types with a namespace, or just "Name" for
// global types. Uses a static buffer for concatenation (caller marshals
// immediately via shape-dispatch wrapper).
CHAOS_IL2CPP_INTPTR ChaosReflectionGetTypeFullName(CHAOS_IL2CPP_INTPTR type_handle) noexcept {
    // Try Tier 0 first
    TypeRef tr;
    if (ResolveTypeRef(type_handle, tr)) {
        const char* ns = tr.module->type_namespaces[tr.type_index];
        const char* name = tr.module->type_names[tr.type_index];
        if (name == nullptr) return 0;
        if (ns == nullptr || ns[0] == '\0') {
            return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<char*>(name));
        }
        static char s_buf[1024];
        fmt::format_to_n(s_buf, sizeof(s_buf) - 1, "{}.{}", ns, name);
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_buf);
    }

    // Fallback: decode descriptor
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr || desc->name_utf8 == nullptr) return 0;
    if (desc->namespace_name_utf8 == nullptr || desc->namespace_name_utf8[0] == '\0') {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<char*>(desc->name_utf8));
    }
    static char s_buf2[1024];
    fmt::format_to_n(s_buf2, sizeof(s_buf2) - 1, "{}.{}", desc->namespace_name_utf8, desc->name_utf8);
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_buf2);
}

// ── GetAssemblyQualifiedName ─────────────────────────────────────
// Returns "Namespace.Name, AssemblyName" using ModuleRegistry Tier 0
// or descriptor fallback. Static buffer for concatenation.
CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssemblyQualifiedName(CHAOS_IL2CPP_INTPTR type_handle) noexcept {
    TypeRef tr;
    if (ResolveTypeRef(type_handle, tr)) {
        const char* ns = tr.module->type_namespaces[tr.type_index];
        const char* name = tr.module->type_names[tr.type_index];
        const char* assembly = tr.module->name_utf8;
        if (name == nullptr || assembly == nullptr) return 0;

        static char s_buf[2048];
        if (ns != nullptr && ns[0] != '\0') {
            fmt::format_to_n(s_buf, sizeof(s_buf) - 1, "{}.{}, {}", ns, name, assembly);
        } else {
            fmt::format_to_n(s_buf, sizeof(s_buf) - 1, "{}, {}", name, assembly);
        }
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_buf);
    }

    // Fallback: decode descriptor + image
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr || desc->name_utf8 == nullptr) return 0;
    auto* image = GetImageFromTypeHandle(type_handle);
    if (image == nullptr || image->image_name_utf8 == nullptr) return 0;

    static char s_buf2[2048];
    if (desc->namespace_name_utf8 != nullptr && desc->namespace_name_utf8[0] != '\0') {
        fmt::format_to_n(s_buf2, sizeof(s_buf2) - 1, "{}.{}, {}",
            desc->namespace_name_utf8, desc->name_utf8, image->image_name_utf8);
    } else {
        fmt::format_to_n(s_buf2, sizeof(s_buf2) - 1, "{}, {}",
            desc->name_utf8, image->image_name_utf8);
    }
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_buf2);
}

// ── GetAssemblyNameValue ─────────────────────────────────────────
// Reads the runtime_name_value field from a managed AssemblyName object.
// The managed AssemblyName stores the name string pointer at offset 16.
CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssemblyNameValue(CHAOS_IL2CPP_INTPTR assembly_name_handle) noexcept {
    if (assembly_name_handle == 0) return 0;
    // The managed AssemblyName stores runtime_name_value at offset 16.
    return *reinterpret_cast<const CHAOS_IL2CPP_INTPTR*>(
        static_cast<const char*>(reinterpret_cast<const void*>(
            static_cast<CHAOS_IL2CPP_INTPTR>(assembly_name_handle))) + 16);
}

// ── GetReflectedType ─────────────────────────────────────────────
// Returns the type that owns the given member. For type handles, this
// returns the declaring type (nested type parent). For method/field
// handles, returns the declaring type via descriptor lookup.
CHAOS_IL2CPP_INTPTR ChaosReflectionGetReflectedType(CHAOS_IL2CPP_INTPTR member_handle) noexcept {
    using namespace chaos::il2cpp::runtime_core;

    if (member_handle == 0) return 0;

    // Try type handle (including Module Registry encoded handles)
    {
        TypeRef tr;
        if (ResolveTypeRef(member_handle, tr)) {
            // For types, we don't track nested-type parent via Tier 0 arrays yet.
            // Fall through to descriptor path.
        }
    }

    // Try decoding as each descriptor type
    auto* typeDesc = TryDecodeReflectionQueryHandle<ReflectionQueryTypeDescriptor>(
        static_cast<TypeInfoHandle>(member_handle));
    if (typeDesc != nullptr) {
        // Return generic type definition if this is a constructed generic
        if (typeDesc->generic_type_definition != nullptr) {
            return static_cast<CHAOS_IL2CPP_INTPTR>(
                EncodeReflectionQueryTypeHandle(typeDesc->generic_type_definition));
        }
        return 0;
    }

    auto* methodDesc = TryDecodeReflectionQueryHandle<ReflectionQueryMethodDescriptor>(
        static_cast<MethodInfoHandle>(member_handle));
    if (methodDesc != nullptr) {
        const uint32_t token = methodDesc->metadata_token;
        if (token != 0u) {
            const uint32_t module_count = GetModuleCount();
            for (uint32_t i = 0u; i < module_count; i++) {
                const auto* mod = GetModuleByIndex(i);
                if (mod == nullptr || mod->image == nullptr) continue;
                for (uint32_t t = 0u; t < mod->image->type_count; t++) {
                    const auto* type = mod->image->types[t];
                    if (type == nullptr || type->methods == nullptr) continue;
                    for (uint32_t m = 0u; m < type->method_count; m++) {
                        if (type->methods[m].metadata_token == token) {
                            return static_cast<CHAOS_IL2CPP_INTPTR>(
                                EncodeReflectionQueryTypeHandle(type));
                        }
                    }
                }
            }
        }
        return 0;
    }

    auto* fieldDesc = TryDecodeReflectionQueryHandle<ReflectionQueryFieldDescriptor>(
        static_cast<FieldInfoHandle>(member_handle));
    if (fieldDesc != nullptr) {
        const uint32_t token = fieldDesc->metadata_token;
        if (token != 0u) {
            const uint32_t module_count = GetModuleCount();
            for (uint32_t i = 0u; i < module_count; i++) {
                const auto* mod = GetModuleByIndex(i);
                if (mod == nullptr || mod->image == nullptr) continue;
                for (uint32_t t = 0u; t < mod->image->type_count; t++) {
                    const auto* type = mod->image->types[t];
                    if (type == nullptr || type->fields == nullptr) continue;
                    for (uint32_t f = 0u; f < type->field_count; f++) {
                        if (type->fields[f].metadata_token == token) {
                            return static_cast<CHAOS_IL2CPP_INTPTR>(
                                EncodeReflectionQueryTypeHandle(type));
                        }
                    }
                }
            }
        }
        return 0;
    }

    return 0;
}

// =====================================================================
// Type hierarchy — Phase 3
// =====================================================================

// ── IsSubclassOf ───────────────────────────────────────────────────
// Returns 1 if `type` is a subclass of `base`, 0 otherwise.
// Uses TypeInfo* parent-chain walk (pointer equality on inline constexpr
// TypeInfo instances — safe across modules for static AOT types).
CHAOS_IL2CPP_INTPTR ChaosReflectionIsSubclassOf(
    CHAOS_IL2CPP_INTPTR type_handle,
    CHAOS_IL2CPP_INTPTR base_handle) noexcept
{
    if (type_handle == 0 || base_handle == 0) return 0;
    if (type_handle == base_handle) return 0;  // same type is not subclass

    const auto* type_info = GetTypeInfoFromHandle(type_handle);
    const auto* base_info = GetTypeInfoFromHandle(base_handle);
    if (type_info == nullptr || base_info == nullptr) return 0;

    // Walk parent chain (TypeInfo::parent forms a cross-module linked list)
    while (type_info->parent != nullptr) {
        if (type_info->parent == base_info) return 1;
        type_info = type_info->parent;
    }
    return 0;
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
        if (warm->iface_map != nullptr) {
            for (uint32_t i = 0; i < warm->iface_count; i++) {
                if (warm->iface_map[i].iface_stable_id == target_stable) return 1;
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
            if (warm->iface_map != nullptr) {
                for (uint32_t i = 0; i < warm->iface_count; i++) {
                    if (warm->iface_map[i].iface_stable_id == target_stable) return 1;
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

// ── GetMembers ────────────────────────────────────────────────────
// Merges fields, methods, properties, and nested types into a single
// flat buffer: [total_count, (kind_0, handle_0), (kind_1, handle_1), ...]
// (kind = 0 field, 1 method, 2 property, 3 nested type)
// The managed wrapper iterates this buffer to construct MemberInfo[].
CHAOS_IL2CPP_INTPTR ChaosReflectionGetMembers(CHAOS_IL2CPP_INTPTR type_handle) {
    using namespace chaos::il2cpp::runtime_core;

    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr) return 0;

    uint32_t field_count    = (desc->fields    != nullptr) ? desc->field_count    : 0;
    uint32_t method_count   = (desc->methods   != nullptr) ? desc->method_count   : 0;
    uint32_t property_count = (desc->properties != nullptr) ? desc->property_count : 0;

    // Nested types via Module Registry Tier 0 (tag-bit-63 handles skip this)
    TypeRef tr;
    uint32_t nested_count = 0;
    uint32_t nested_module_id = 0;
    uint32_t nested_start = 0;
    if (ResolveTypeRef(type_handle, tr) && tr.module->nested_type_offset != nullptr) {
        nested_start      = tr.module->nested_type_offset[tr.type_index];
        uint32_t end      = tr.module->nested_type_offset[tr.type_index + 1];
        nested_count      = end - nested_start;
        nested_module_id  = tr.module_id;
    }

    uint32_t total = field_count + method_count + property_count + nested_count;
    if (total == 0 || total > 128) return 0;

    // Buffer entries: [count, (kind, handle) × total] = 1 + 2*total
    static CHAOS_IL2CPP_INTPTR s_buffer[257];
    s_buffer[0] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<intptr_t>(total));

    constexpr CHAOS_IL2CPP_INTPTR kField      = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    constexpr CHAOS_IL2CPP_INTPTR kMethod     = static_cast<CHAOS_IL2CPP_INTPTR>(1);
    constexpr CHAOS_IL2CPP_INTPTR kProperty   = static_cast<CHAOS_IL2CPP_INTPTR>(2);
    constexpr CHAOS_IL2CPP_INTPTR kNestedType = static_cast<CHAOS_IL2CPP_INTPTR>(3);

    uint32_t idx = 1;

    for (uint32_t i = 0; i < field_count; i++) {
        s_buffer[idx++] = kField;
        s_buffer[idx++] = static_cast<CHAOS_IL2CPP_INTPTR>(
            EncodeReflectionQueryFieldHandle(&desc->fields[i]));
    }
    for (uint32_t i = 0; i < method_count; i++) {
        s_buffer[idx++] = kMethod;
        s_buffer[idx++] = static_cast<CHAOS_IL2CPP_INTPTR>(
            EncodeReflectionQueryMethodHandle(&desc->methods[i]));
    }
    for (uint32_t i = 0; i < property_count; i++) {
        s_buffer[idx++] = kProperty;
        s_buffer[idx++] = static_cast<CHAOS_IL2CPP_INTPTR>(
            EncodeReflectionQueryPropertyHandle(&desc->properties[i]));
    }
    for (uint32_t i = 0; i < nested_count; i++) {
        s_buffer[idx++] = kNestedType;
        uint32_t child_token = tr.module->nested_type_children[nested_start + i];
        s_buffer[idx++] = static_cast<CHAOS_IL2CPP_INTPTR>(
            MakeTypeHandle(nested_module_id, child_token));
    }

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_buffer);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetNestedTypes(CHAOS_IL2CPP_INTPTR type_handle) {
    TypeRef tr;
    if (!ResolveTypeRef(type_handle, tr)) return 0;
    if (tr.module->nested_type_offset == nullptr) return 0;

    uint32_t start = tr.module->nested_type_offset[tr.type_index];
    uint32_t end = tr.module->nested_type_offset[tr.type_index + 1];
    uint32_t count = end - start;
    if (count == 0 || count > 32) return 0;

    static CHAOS_IL2CPP_INTPTR buffer[33];
    buffer[0] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<intptr_t>(count));
    for (uint32_t i = 0; i < count; i++) {
        uint32_t child_token = tr.module->nested_type_children[start + i];
        buffer[1 + i] = static_cast<CHAOS_IL2CPP_INTPTR>(
            MakeTypeHandle(tr.module_id, child_token));
    }
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(buffer);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetField(
    CHAOS_IL2CPP_INTPTR type_handle,
    CHAOS_IL2CPP_INTPTR name_string_id)
{
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr || desc->fields == nullptr) return 0;

    const char* field_name = DecodeAndNullTerminateString(name_string_id);
    if (field_name == nullptr) return 0;

    auto* field = FindReflectionQueryField(desc, field_name);
    if (field == nullptr) return 0;

    return static_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryFieldHandle(field));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetMethod(
    CHAOS_IL2CPP_INTPTR type_handle,
    CHAOS_IL2CPP_INTPTR name_string_id,
    CHAOS_IL2CPP_INTPTR param_types)
{
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr || desc->methods == nullptr) return 0;

    const char* method_name = DecodeAndNullTerminateString(name_string_id);
    if (method_name == nullptr) return 0;

    if (param_types != 0) {
        // Layout: object_header (24 bytes FatHeader) + length (4 bytes)
        const auto* arr_bytes = reinterpret_cast<const uint8_t*>(
            static_cast<CHAOS_IL2CPP_INTPTR>(param_types));
        int32_t param_count = *reinterpret_cast<const CHAOS_IL2CPP_INT32*>(
            arr_bytes + sizeof(FatHeader));
        if (param_count < 0) param_count = 0;

        auto* method = FindReflectionQueryMethod(desc, method_name, param_count);
        if (method != nullptr) {
            return static_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryMethodHandle(method));
        }
    } else {
        // No param_types — return first method with matching name
        for (CHAOS_IL2CPP_UINT32 i = 0; i < desc->method_count; i++) {
            if (NamesMatch(desc->methods[i].name_utf8, method_name)) {
                return static_cast<CHAOS_IL2CPP_INTPTR>(
                    EncodeReflectionQueryMethodHandle(&desc->methods[i]));
            }
        }
    }

    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetGenericArguments(CHAOS_IL2CPP_INTPTR type_handle) {
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr) return 0;

    // Runtime-instantiated type: token >= 0x80000000, descriptor embedded in
    // RuntimeInstantiatedType — recover type_args from container struct.
    if (desc->metadata_token >= 0x80000000u) {
        using rt_type = chaos::il2cpp::runtime_instantiation::RuntimeInstantiatedType;
        auto* rti = reinterpret_cast<const rt_type*>(desc);
        if (rti == nullptr || rti->type_args == nullptr || rti->arg_count == 0) return 0;

        static CHAOS_IL2CPP_INTPTR s_buffer[33];
        uint32_t count = rti->arg_count > 32 ? 32 : rti->arg_count;
        for (uint32_t i = 0; i < count; i++) {
            s_buffer[1 + i] = static_cast<CHAOS_IL2CPP_INTPTR>(rti->type_args[i]);
        }
        s_buffer[0] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<intptr_t>(count));
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_buffer);
    }

    // AOT concrete generic type: look up in generic context registry.
    // The type_handle (a TypeInfoHandle) was stored at registration time and
    // is directly usable as a lookup key into the by_open_type closed_types.
    {
        TypeInfoHandle closed_handle = static_cast<TypeInfoHandle>(static_cast<uint64_t>(type_handle));
        if (closed_handle != 0) {
            static CHAOS_IL2CPP_INTPTR s_args_buffer[33];
            uint32_t count = generic_context::GetClosedTypeGenericArgs(
                closed_handle,
                reinterpret_cast<TypeInfoHandle*>(s_args_buffer + 1),
                32);
            if (count > 0) {
                s_args_buffer[0] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<intptr_t>(count));
                return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_args_buffer);
            }
        }
    }

    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetGenericTypeDefinition(CHAOS_IL2CPP_INTPTR type_handle) {
    using namespace chaos::il2cpp::runtime_core;
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr || desc->generic_type_definition == nullptr) return 0;

    return static_cast<CHAOS_IL2CPP_INTPTR>(
        EncodeReflectionQueryTypeHandle(desc->generic_type_definition));
}

CHAOS_IL2CPP_INTPTR ChaosReflectionCreateInstance(
    CHAOS_IL2CPP_INTPTR type_handle,
    CHAOS_IL2CPP_INTPTR args)
{
    auto* runtime = GetCurrentRuntimeState();
    auto* thread  = GetCurrentThreadState();
    const auto* abi = GetRuntimeAbiV0();
    if (runtime == nullptr || thread == nullptr || abi == nullptr) return 0;
    if (abi->object_new == nullptr || abi->class_init == nullptr) return 0;

    TypeInfoHandle type = static_cast<TypeInfoHandle>(static_cast<uint64_t>(type_handle));
    if (type == 0) return 0;

    // Ensure static constructor has run.
    abi->class_init(runtime, type);

    // Allocate the object.
    void* obj = abi->object_new(runtime, thread, type);
    if (obj == nullptr) return 0;

    // If constructor arguments are provided, find and invoke the matching .ctor.
    if (args != 0) {
        // managed System.Object[]: [vtable(8)][type_info(8)][length(4)][pad(4)][elements...]
        const auto argc = static_cast<uint32_t>(
            *reinterpret_cast<const CHAOS_IL2CPP_INT32*>(
                static_cast<CHAOS_IL2CPP_INTPTR>(args) + 16));
        void* const* argv = reinterpret_cast<void* const*>(
            static_cast<CHAOS_IL2CPP_INTPTR>(args) + 24);

        MethodInfoHandle ctor = 0;
        if (abi->type_find_method != nullptr) {
            ctor = abi->type_find_method(type, ".ctor", static_cast<int32_t>(argc));
        }
        if (ctor != 0 && abi->method_invoke != nullptr) {
            ExceptionHandle ex = nullptr;
            RuntimeStatus ctor_status = abi->method_invoke(runtime, thread, ctor, obj,
                               argv, argc, nullptr, 0, &ex);
            if (ctor_status == CHAOS_RUNTIME_STATUS_MANAGED_EXCEPTION) {
                throw chaos_managed_exception{reinterpret_cast<CHAOS_IL2CPP_INTPTR>(ex)};
            }
        }
    }

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(obj);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionInvokeMethod(
    CHAOS_IL2CPP_INTPTR method_handle,
    CHAOS_IL2CPP_INTPTR obj,
    CHAOS_IL2CPP_INTPTR args)
{
    auto* runtime = GetCurrentRuntimeState();
    auto* thread  = GetCurrentThreadState();
    const auto* abi = GetRuntimeAbiV0();
    if (runtime == nullptr || thread == nullptr || abi == nullptr) return 0;
    if (abi->method_invoke == nullptr) return 0;

    // method_handle is a pointer to managed MethodInfo object.
    // Extract runtime_method_handle at offset 24 (after 16-byte object header).
    // Use raw pointer arithmetic to avoid MSVC auto+const+complex-type issues.
    if (method_handle == 0) return 0;
    const void* methodInfoObj = reinterpret_cast<const void*>(
        static_cast<CHAOS_IL2CPP_INTPTR>(method_handle));
    MethodInfoHandle method = static_cast<MethodInfoHandle>(
        *reinterpret_cast<const CHAOS_IL2CPP_INTPTR*>(
            static_cast<const char*>(methodInfoObj) + 24));
    if (method == 0) return 0;

    // Decode managed System.Object[] args array.
    uint32_t argc = 0;
    void* const* argv = nullptr;
    if (args != 0) {
        argc = static_cast<uint32_t>(
            *reinterpret_cast<const CHAOS_IL2CPP_INT32*>(
                static_cast<CHAOS_IL2CPP_INTPTR>(args) + 16));
        argv = reinterpret_cast<void* const*>(
            static_cast<CHAOS_IL2CPP_INTPTR>(args) + 24);
    }

    // Return value buffer (64 bytes covers SIMD types up to Vector512).
    uint64_t ret_buf[8] = {};
    ExceptionHandle ex = nullptr;

    RuntimeStatus status = abi->method_invoke(
        runtime, thread, method,
        reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_INTPTR>(obj)),
        argv, argc, ret_buf, sizeof(ret_buf), &ex);

    if (status == CHAOS_RUNTIME_STATUS_MANAGED_EXCEPTION) {
        throw chaos_managed_exception{reinterpret_cast<CHAOS_IL2CPP_INTPTR>(ex)};
    }

    // For reference-type returns ret_buf[0] is the managed object pointer.
    // For value-type returns <= 8 bytes, ret_buf[0] holds the raw value
    // (managed wrapper must box if needed).
    return static_cast<CHAOS_IL2CPP_INTPTR>(ret_buf[0]);
}

// Extract TypeInfoHandle[] from a managed System.Type[] pointer.
// Returns the number of handles extracted (clamped to max_count).
// Each managed Type object has runtime_type_handle at offset 16 (after object header).
static uint32_t ExtractTypeArgsFromManagedArray(
    CHAOS_IL2CPP_INTPTR type_args,
    TypeInfoHandle* out_handles,
    uint32_t max_count) noexcept
{
    if (type_args == 0) return 0u;
    const auto* raw = reinterpret_cast<const uint8_t*>(
        static_cast<CHAOS_IL2CPP_INTPTR>(type_args));
    // Managed array layout: [header(16)][length(4)][pad(4)][elements...]
    const uint32_t count = static_cast<uint32_t>(
        *reinterpret_cast<const CHAOS_IL2CPP_INT32*>(raw + 16u));
    const uint32_t actual = (count < max_count) ? count : max_count;
    for (uint32_t i = 0u; i < actual; i++) {
        const void* type_obj = *reinterpret_cast<void* const*>(
            raw + 24u + i * sizeof(void*));
        if (type_obj == nullptr) {
            out_handles[i] = 0u;
            continue;
        }
        // Managed Type layout: [header(16)][runtime_type_handle(8)]...
        out_handles[i] = static_cast<TypeInfoHandle>(
            *reinterpret_cast<const CHAOS_IL2CPP_INTPTR*>(
                static_cast<const uint8_t*>(type_obj) + 16u));
    }
    return actual;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionMakeGenericMethod(
    CHAOS_IL2CPP_INTPTR method_handle,
    CHAOS_IL2CPP_INTPTR type_args)
{
    if (method_handle == 0) return 0;

    auto* bridge = chaos::il2cpp::runtime_instantiation::GetBridgeV0();
    if (bridge == nullptr || bridge->resolve_or_instantiate_method == nullptr)
        return 0;

    MethodInfoHandle open_method = static_cast<MethodInfoHandle>(static_cast<uint64_t>(method_handle));

    if (type_args == 0) {
        return static_cast<CHAOS_IL2CPP_INTPTR>(
            bridge->resolve_or_instantiate_method(open_method, nullptr, 0u));
    }

    TypeInfoHandle arg_handles[32];
    const uint32_t arg_count = ExtractTypeArgsFromManagedArray(type_args, arg_handles, 32u);
    if (arg_count == 0u) {
        return static_cast<CHAOS_IL2CPP_INTPTR>(open_method);
    }

    MethodInfoHandle result = bridge->resolve_or_instantiate_method(
        open_method, arg_handles, arg_count);
    return static_cast<CHAOS_IL2CPP_INTPTR>(result);
}

// ── CustomAttribute blob query ──────────────────────────────────────
// Called from per-family generated code (the short extraction wrapper)
// after it has decoded (kind, handle) from the managed reflection object.

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

// =====================================================================
// Module reflection — Phase 2
// =====================================================================

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

// =====================================================================
// Type reflection — Phase 4/6 (generic info, assembly, parameter, etc.)
// =====================================================================

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
CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssemblyFullName(CHAOS_IL2CPP_INTPTR assembly_handle) noexcept {
    auto* decoded = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(assembly_handle));
    if (decoded == nullptr || decoded->image_name_utf8 == nullptr) return 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<char*>(decoded->image_name_utf8));
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
CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssemblyLocation(CHAOS_IL2CPP_INTPTR /*assembly*/) noexcept {
    return 0;
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
CHAOS_IL2CPP_INT32 ChaosReflectionGetParamAttributes(CHAOS_IL2CPP_INTPTR /*param*/) noexcept {
    return 0;
}

// ── GetFieldsBindingflags ───────────────────────────────────────────
// For now, ignore BindingFlags and delegate to GetFields.
CHAOS_IL2CPP_INTPTR ChaosReflectionGetFieldsBindingflags(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INT32 /*flags*/) noexcept {
    return ChaosReflectionGetFields(type);
}

// ── GetMethodsBindingflags ──────────────────────────────────────────
// For now, ignore BindingFlags and delegate to GetMethods.
CHAOS_IL2CPP_INTPTR ChaosReflectionGetMethodsBindingflags(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INT32 /*flags*/) noexcept {
    return ChaosReflectionGetMethods(type);
}

// ── AssemblyGetTypes ────────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetTypes(CHAOS_IL2CPP_INTPTR assembly_handle) noexcept {
    auto* decoded = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(assembly_handle));
    if (decoded == nullptr || decoded->types == nullptr) return 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
        const_cast<ReflectionQueryTypeDescriptor* const*>(decoded->types));
}

// ── GetTypeFromAssemblyBool ─────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionGetTypeFromAssemblyBool(
    CHAOS_IL2CPP_INTPTR assembly_handle,
    CHAOS_IL2CPP_INTPTR name_string_id,
    CHAOS_IL2CPP_INT32 throw_on_error) noexcept
{
    auto* decoded = TryDecodeReflectionQueryImageHandle(static_cast<ImageHandle>(assembly_handle));
    if (decoded == nullptr) return 0;

    const char* full_name = DecodeAndNullTerminateString(name_string_id);
    if (full_name == nullptr) return 0;

    const char* ns = nullptr;
    const char* type_name = nullptr;
    SplitTypeName(full_name, ns, type_name);
    if (ns == nullptr || type_name == nullptr) return 0;

    auto* type = FindReflectionQueryTypeByName(decoded, ns, type_name);
    if (type == nullptr) {
        if (throw_on_error) {
            // V1: propagate error without a managed TypeLoadException object.
            // Future: create TypeLoadException via ABI and raise it properly.
            throw chaos_managed_exception{0};
        }
        return 0;
    }
    return static_cast<CHAOS_IL2CPP_INTPTR>(EncodeReflectionQueryTypeHandle(type));
}

// ── Runtime handle hash codes ───────────────────────────────────────
CHAOS_IL2CPP_INT32 ChaosRuntimetypehandleGetHashCode(CHAOS_IL2CPP_INTPTR handle) noexcept {
    return static_cast<CHAOS_IL2CPP_INT32>(handle & 0xFFFFFFFF);
}

CHAOS_IL2CPP_INT32 ChaosRuntimemethodhandleGetHashCode(CHAOS_IL2CPP_INT64 handle) noexcept {
    return static_cast<CHAOS_IL2CPP_INT32>(handle & 0xFFFFFFFF);
}

CHAOS_IL2CPP_INT32 ChaosRuntimefieldhandleGetHashCode(CHAOS_IL2CPP_INTPTR handle) noexcept {
    return static_cast<CHAOS_IL2CPP_INT32>(handle & 0xFFFFFFFF);
}

// ── MakeGenericType ─────────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosReflectionMakeGenericType(CHAOS_IL2CPP_INTPTR def, CHAOS_IL2CPP_INTPTR args) noexcept
{
    if (def == 0) return 0;

    auto* bridge = chaos::il2cpp::runtime_instantiation::GetBridgeV0();
    if (bridge == nullptr || bridge->resolve_or_instantiate_type == nullptr)
        return 0;

    TypeInfoHandle open_def = static_cast<TypeInfoHandle>(static_cast<uint64_t>(def));

    if (args == 0) {
        return static_cast<CHAOS_IL2CPP_INTPTR>(
            bridge->resolve_or_instantiate_type(open_def, nullptr, 0u));
    }

    TypeInfoHandle arg_handles[32];
    const uint32_t arg_count = ExtractTypeArgsFromManagedArray(args, arg_handles, 32u);
    if (arg_count == 0u) {
        return static_cast<CHAOS_IL2CPP_INTPTR>(open_def);
    }

    TypeInfoHandle result = bridge->resolve_or_instantiate_type(
        open_def, arg_handles, arg_count);
    return static_cast<CHAOS_IL2CPP_INTPTR>(result);
}

// =====================================================================
// String helpers
// =====================================================================

CHAOS_IL2CPP_INTPTR ChaosReflectionConcatStringPairValues(
    CHAOS_IL2CPP_INTPTR left,
    CHAOS_IL2CPP_INTPTR right)
{
    if (left == 0 && right == 0) return 0;

    auto* runtime = GetCurrentRuntimeState();
    auto* thread  = GetCurrentThreadState();
    const auto* abi = GetRuntimeAbiV0();
    if (runtime == nullptr || thread == nullptr || abi == nullptr || abi->string_new_utf8 == nullptr)
        return 0;

    const char* left_str = DecodeStringValue(left);
    const char* right_str = DecodeStringValue(right);
    if (left_str == nullptr) left_str = "";
    if (right_str == nullptr) right_str = "";

    static char s_buf[4096];
    auto result = fmt::format_to_n(s_buf, sizeof(s_buf) - 1, "{}{}", left_str, right_str);
    size_t len = result.size;
    if (len >= sizeof(s_buf)) len = sizeof(s_buf) - 1;

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
        abi->string_new_utf8(runtime, thread, s_buf, len));
}

}  // namespace chaos::il2cpp::runtime_core
}  // extern "C"
