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

#include <cstring>
#include <cstdio>

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

static const TypeInfo* GetTypeInfoFromHandle(CHAOS_IL2CPP_INTPTR handle) noexcept {
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
        std::snprintf(s_buf, sizeof(s_buf), "%s.%s", ns, name);
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_buf);
    }

    // Fallback: decode descriptor
    auto* desc = GetTypeDescriptorFromHandle(type_handle);
    if (desc == nullptr || desc->name_utf8 == nullptr) return 0;
    if (desc->namespace_name_utf8 == nullptr || desc->namespace_name_utf8[0] == '\0') {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<char*>(desc->name_utf8));
    }
    static char s_buf2[1024];
    std::snprintf(s_buf2, sizeof(s_buf2), "%s.%s", desc->namespace_name_utf8, desc->name_utf8);
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
            std::snprintf(s_buf, sizeof(s_buf), "%s.%s, %s", ns, name, assembly);
        } else {
            std::snprintf(s_buf, sizeof(s_buf), "%s, %s", name, assembly);
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
        std::snprintf(s_buf2, sizeof(s_buf2), "%s.%s, %s",
            desc->namespace_name_utf8, desc->name_utf8, image->image_name_utf8);
    } else {
        std::snprintf(s_buf2, sizeof(s_buf2), "%s, %s",
            desc->name_utf8, image->image_name_utf8);
    }
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_buf2);
}

// ── GetAssemblyNameValue ─────────────────────────────────────────
// Reads the runtime_name_value field from a managed AssemblyName object.
// The managed AssemblyName stores the name string pointer at offset 16.
CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssemblyNameValue(CHAOS_IL2CPP_INTPTR assembly_name_handle) noexcept {
    if (assembly_name_handle == 0) return 0;
    auto* name_obj = reinterpret_cast<const chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName*>(assembly_name_handle);
    return name_obj->runtime_name_value;
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
        // Without a declaring-type back-pointer in the method descriptor,
        // we can't resolve this. Future: scan the image for the owning type.
        (void)methodDesc;
        return 0;
    }

    auto* fieldDesc = TryDecodeReflectionQueryHandle<ReflectionQueryFieldDescriptor>(
        static_cast<FieldInfoHandle>(member_handle));
    if (fieldDesc != nullptr) {
        // Same limitation as methods — no declaring-type back-pointer.
        (void)fieldDesc;
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
        if (walk->iface_map != nullptr) {
            for (uint32_t i = 0; i < walk->iface_count; i++) {
                if (walk->iface_map[i].iface_stable_id == target_stable) return 1;
            }
        }
        if (walk->runtime_iface_map != nullptr) {
            for (uint32_t i = 0; i < walk->runtime_iface_count; i++) {
                if (walk->runtime_iface_map[i].iface_stable_id == target_stable) return 1;
            }
        }
        walk = walk->parent;
    }
    return 0;
}

// ── IsInstanceOfType ───────────────────────────────────────────────
// Returns 1 if `obj` is an instance of `type` (or a subclass/implementor).
// Reads TypeInfo* from the object header, compares via parent / iface chain.
CHAOS_IL2CPP_INTPTR ChaosReflectionIsInstanceOfType(
    CHAOS_IL2CPP_INTPTR obj,
    CHAOS_IL2CPP_INTPTR type_handle) noexcept
{
    if (obj == 0 || type_handle == 0) return 0;

    const auto* obj_header = reinterpret_cast<const chaos_object_header*>(
        static_cast<CHAOS_IL2CPP_INTPTR>(obj));
    const auto* obj_type = obj_header->type_info;
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
            if (walk->iface_map != nullptr) {
                for (uint32_t i = 0; i < walk->iface_count; i++) {
                    if (walk->iface_map[i].iface_stable_id == target_stable) return 1;
                }
            }
            if (walk->runtime_iface_map != nullptr) {
                for (uint32_t i = 0; i < walk->runtime_iface_count; i++) {
                    if (walk->runtime_iface_map[i].iface_stable_id == target_stable) return 1;
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

    if (type_info->iface_map != nullptr) {
        for (uint32_t i = 0; i < type_info->iface_count; i++) {
            try_add(type_info->iface_map[i].iface_stable_id);
        }
    }
    if (type_info->runtime_iface_map != nullptr) {
        for (uint32_t i = 0; i < type_info->runtime_iface_count; i++) {
            try_add(type_info->runtime_iface_map[i].iface_stable_id);
        }
    }

    s_buffer[0] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<intptr_t>(count));
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_buffer);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetMembers(CHAOS_IL2CPP_INTPTR type_handle) {
    using namespace chaos::il2cpp::runtime_core;
    (void)type_handle;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetNestedTypes(CHAOS_IL2CPP_INTPTR type_handle) {
    using namespace chaos::il2cpp::runtime_core;
    (void)type_handle;
    return 0;
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
        // Extract parameter count from managed System.Type[] array
        // Layout: object_header (16 bytes) + length (4 bytes)
        const auto* arr_bytes = reinterpret_cast<const uint8_t*>(
            static_cast<CHAOS_IL2CPP_INTPTR>(param_types));
        int32_t param_count = *reinterpret_cast<const CHAOS_IL2CPP_INT32*>(
            arr_bytes + sizeof(chaos_object_header));
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
    (void)type_handle;
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
            abi->method_invoke(runtime, thread, ctor, obj,
                               argv, argc, nullptr, 0, &ex);
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
    if (method_handle == 0) return 0;
    auto* mi = reinterpret_cast<const chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo*>(
        static_cast<CHAOS_IL2CPP_INTPTR>(method_handle));
    MethodInfoHandle method = static_cast<MethodInfoHandle>(mi->runtime_method_handle);
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
        // TODO: propagate exception through managed exception carrier
        return 0;
    }

    // For reference-type returns ret_buf[0] is the managed object pointer.
    // For value-type returns <= 8 bytes, ret_buf[0] holds the raw value
    // (managed wrapper must box if needed).
    return static_cast<CHAOS_IL2CPP_INTPTR>(ret_buf[0]);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionMakeGenericMethod(
    CHAOS_IL2CPP_INTPTR method_handle,
    CHAOS_IL2CPP_INTPTR type_args)
{
    (void)method_handle;
    (void)type_args;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetCustomAttribute(
    CHAOS_IL2CPP_INTPTR member_handle,
    CHAOS_IL2CPP_INTPTR attribute_type_handle)
{
    (void)member_handle;
    (void)attribute_type_handle;
    return 0;
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
CHAOS_IL2CPP_INTPTR ChaosReflectionGetGenericParamPos(CHAOS_IL2CPP_INTPTR type_handle) noexcept {
    TypeRef tr;
    if (ResolveTypeRef(type_handle, tr)) {
        (void)tr;
        // Generic param position would need subject_id parsing of "!N"/"!!N"
    }
    return 0;
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
        (void)throw_on_error;  // TODO: throw TypeLoadException
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

    // V1: args is a managed System.Type[] — TypeInfoHandle extraction deferred
    (void)args;
    return static_cast<CHAOS_IL2CPP_INTPTR>(open_def);
}

// =====================================================================
// String helpers
// =====================================================================

CHAOS_IL2CPP_INTPTR ChaosReflectionConcatStringPairValues(
    CHAOS_IL2CPP_INTPTR left,
    CHAOS_IL2CPP_INTPTR right)
{
    (void)left;
    (void)right;
    return 0;
}

}  // namespace chaos::il2cpp::runtime_core
}  // extern "C"
