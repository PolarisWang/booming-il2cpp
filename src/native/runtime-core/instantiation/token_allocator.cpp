namespace chaos::il2cpp::runtime_instantiation {
namespace {

/* ── Runtime token allocator ────────────────────────────────────────────── */
/* Start at 0x80000000 to leave the AOT token space (0x00000000–0x7FFFFFFF)
 * uncontested.  Each allocation bumps the counter atomically. */

std::atomic<CHAOS_IL2CPP_UINT32> s_next_runtime_token{0x80000000u};

/* ── Track all heap-allocated RuntimeInstantiatedType instances so we can
 *    unregister + free them during hot-update unload.                     ── */
struct RuntimeInstantiatedTypeRecord {
    RuntimeInstantiatedType* type;
    CHAOS_IL2CPP_UINT32 module_id;
};

CHAOS_IL2CPP_MUTEX s_runtime_types_mutex;
CHAOS_IL2CPP_VECTOR(RuntimeInstantiatedTypeRecord) s_runtime_types;

/* ── Track all heap-allocated RuntimeInstantiatedMethod instances so we can
 *    mark them as unloaded during hot-update unload.                     ── */
struct RuntimeInstantiatedMethodRecord {
    RuntimeInstantiatedMethod* method;
    CHAOS_IL2CPP_UINT32 module_id;
};

CHAOS_IL2CPP_MUTEX s_runtime_methods_mutex;
CHAOS_IL2CPP_VECTOR(RuntimeInstantiatedMethodRecord) s_runtime_methods;

/* ── Helper: compute a short display name for a type argument handle.    ── */

static const char* GetTypeDisplayName(TypeInfoHandle handle) {
    if (handle == 0) {
        return "?";
    }
    const auto* desc = chaos::il2cpp::runtime_core::TryDecodeReflectionQueryTypeHandle(handle);
    if (desc != nullptr && desc->display_name_utf8 != nullptr) {
        return desc->display_name_utf8;
    }
    if (desc != nullptr && desc->name_utf8 != nullptr) {
        return desc->name_utf8;
    }
    return "?";
}

/* ── Helper: copy a C string into a heap-allocated buffer.              ── */

static char* StrDup(const char* src) {
    if (src == nullptr) return nullptr;
    CHAOS_IL2CPP_SIZE len = std::strlen(src);
    auto* buf = static_cast<char*>(CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE(len + 1));
    if (buf == nullptr) return nullptr;
    CHAOS_IL2CPP_MEMCPY(buf, src, len + 1);
    return buf;
}

/* ── V1: Signature-aware type mapping for argument marshalling ──────── */

/// Map a .NET fully-qualified type name ("System.Int32", etc.) to the
/// corresponding InterpreterValue tag for argument marshalling.
/// Falls back to Int32 (V0 compat) for null/generic-parameter references.
static interpreter::ValueTag MapMemberTypeToValueTag(const char* member_type) {
    if (member_type == nullptr)
        return interpreter::ValueTag::Int32;

    // Generic parameter references ("!N", "!!N") — cannot determine the
    // concrete tag statically; V0 int32 fallback.
    if (member_type[0] == '!') {
        return interpreter::ValueTag::Int32;
    }

    // 4-byte or promoted-to-4-byte primitives
    if (std::strcmp(member_type, "System.Int32") == 0 ||
        std::strcmp(member_type, "System.UInt32") == 0 ||
        std::strcmp(member_type, "System.Boolean") == 0 ||
        std::strcmp(member_type, "System.Byte") == 0 ||
        std::strcmp(member_type, "System.SByte") == 0 ||
        std::strcmp(member_type, "System.Int16") == 0 ||
        std::strcmp(member_type, "System.UInt16") == 0 ||
        std::strcmp(member_type, "System.Char") == 0) {
        return interpreter::ValueTag::Int32;
    }

    // 8-byte primitives
    if (std::strcmp(member_type, "System.Int64") == 0 ||
        std::strcmp(member_type, "System.UInt64") == 0) {
        return interpreter::ValueTag::Int64;
    }

    // 4-byte floating point
    if (std::strcmp(member_type, "System.Single") == 0) {
        return interpreter::ValueTag::Float32;
    }

    // 8-byte floating point
    if (std::strcmp(member_type, "System.Double") == 0) {
        return interpreter::ValueTag::Float64;
    }

    // Native-sized integers
    if (std::strcmp(member_type, "System.IntPtr") == 0 ||
        std::strcmp(member_type, "System.UIntPtr") == 0) {
        return (sizeof(void*) == 8u)
            ? interpreter::ValueTag::Int64
            : interpreter::ValueTag::Int32;
    }

    // Everything else (String, Object, arrays, value types) — ObjectRef
    return interpreter::ValueTag::ObjectRef;
}

/// Scan all registered modules for a type identified by the given metadata
/// token.  Returns the TypeInfoHandle (reflection-query encoding) or 0.
static TypeInfoHandle FindTypeByModuleToken(CHAOS_IL2CPP_UINT32 type_token) {
    for (CHAOS_IL2CPP_UINT32 mid = 0u; mid < runtime_core::kMaxModules; ++mid) {
        const auto* module = runtime_core::LookupModule(mid);
        if (module == nullptr || module->image == nullptr || module->tombstone) {
            continue;
        }
        const auto* type_desc = runtime_core::FindReflectionQueryTypeByToken(
            module->image, type_token);
        if (type_desc != nullptr) {
            return runtime_core::EncodeReflectionQueryTypeHandle(type_desc);
        }
    }
    return 0u;
}

/* ── V1: Type resolution helpers for struct marshalling ───────────────── */

/// Scan all registered modules to find a type by its fully-qualified
/// subject_id (e.g. "System.Guid", "System.Numerics.Vector2").
static TypeInfoHandle FindTypeByName(const char* fully_qualified_name) {
    if (fully_qualified_name == nullptr) return 0u;

    for (CHAOS_IL2CPP_UINT32 mid = 0u; mid < runtime_core::kMaxModules; ++mid) {
        const auto* module = runtime_core::LookupModule(mid);
        if (module == nullptr || module->image == nullptr || module->tombstone) {
            continue;
        }
        const auto* image = module->image;
        if (image == nullptr) continue;

        for (CHAOS_IL2CPP_UINT32 ti = 0u; ti < image->type_count; ++ti) {
            const auto* type_desc = image->types[ti];
            if (type_desc == nullptr) continue;
            if (type_desc->subject_id_utf8 != nullptr &&
                std::strcmp(type_desc->subject_id_utf8,
                    fully_qualified_name) == 0) {
                return runtime_core::MakeTypeHandle(
                    mid, type_desc->metadata_token);
            }
        }
    }
    return 0u;
}

/// Check whether the given TypeInfoHandle refers to a value type (struct).
/// Uses the module registry's type_flags bitfield.
static bool IsValueTypeByHandle(TypeInfoHandle handle) {
    if (handle == 0u) return false;
    const CHAOS_IL2CPP_UINT32 mid = runtime_core::GetModuleId(handle);
    const auto* module = runtime_core::LookupModule(mid);
    if (module == nullptr || module->type_flags == nullptr) return false;
    const CHAOS_IL2CPP_UINT32 type_index = runtime_core::TokenToIndex(
        runtime_core::GetTypeToken(handle));
    if (type_index >= module->type_count) return false;
    return (module->type_flags[type_index] &
            runtime_core::kFlagIsValueType) != 0u;
}

/// Resolve a parameter's member_type_utf8 to a TypeInfoHandle.
/// Handles generic parameter references ("!N", "!!N") and named types.
static TypeInfoHandle ResolveParameterType(
    const char* member_type_utf8,
    const TypeInfoHandle* type_args,
    CHAOS_IL2CPP_UINT32 arg_count)
{
    if (member_type_utf8 == nullptr) return 0u;

    // Generic parameter reference: "!N" (type generic) or "!!N" (method generic)
    if (member_type_utf8[0] == '!') {
        if (type_args == nullptr || arg_count == 0u) return 0u;
        const char* num_str = member_type_utf8 + 1;
        if (member_type_utf8[1] == '!') num_str++;  // "!!N"
        char* end = nullptr;
        long idx = std::strtol(num_str, &end, 10);
        if (end == num_str || idx < 0 ||
            static_cast<CHAOS_IL2CPP_UINT32>(idx) >= arg_count) {
            return 0u;
        }
        return type_args[idx];
    }

    // Named type: scan all registered modules.
    return FindTypeByName(member_type_utf8);
}

}  // anonymous namespace
}  // namespace chaos::il2cpp::runtime_instantiation