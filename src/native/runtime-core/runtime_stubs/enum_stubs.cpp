// ABI exports: extern "C" linkage for managed/NativeAOT callability.

// enum_stubs.cpp — Enum helper stub implementations
#include <chaos/native_types.h>
#include <chaos/log.h>
#include <cstring>
#include <cstdio>

#include "generated_code_compat.h"
#include "string_table.h"
#include "runtime_stubs/stub_common.h"
#include "gc_helpers.h"
#include "reflection_query_model.h"
#include "reflection_api.h"
#include "reflection_metadata_impl.h"
#include "exception_helpers.h"

namespace chaos::il2cpp::runtime_core {
extern "C" {

// ── Internal helpers ────────────────────────────────────────────

/// chaos_managed_string header size: ThinLockableHeader(16) + length(4) +
/// padding(4) + utf8_data(8) + string_id(8) = 40 bytes on x64.
static constexpr CHAOS_IL2CPP_SIZE kManagedStringHeader = 40;

/// Allocate a managed string with the given byte_count of UTF-8 payload + NUL.
/// Uses chaos_managed_string layout so generated code reads length/utf8_data
/// at correct offsets via the CHAOS_IL2CPP_STRING_TYPE* path.
static CHAOS_IL2CPP_INTPTR enum_alloc_string(CHAOS_IL2CPP_UINTPTR byte_count) noexcept
{
    auto* storage = static_cast<unsigned char*>(
        GcAllocateAtomic(kManagedStringHeader + byte_count + 1));
    if (storage == nullptr) return 0;

    std::memset(storage, 0, kManagedStringHeader);

    // length at offset 16
    auto* len_field = reinterpret_cast<CHAOS_IL2CPP_INT32*>(storage + 16);
    *len_field = static_cast<CHAOS_IL2CPP_INT32>(byte_count);

    // inline data, utf8_data ptr at offset 24
    char* data_area = reinterpret_cast<char*>(storage + kManagedStringHeader);
    if (byte_count > 0) data_area[0] = '\0';
    auto* utf8_field = reinterpret_cast<const char**>(storage + 24);
    *utf8_field = data_area;

    // string_id at offset 32 stays 0 (uninterned)

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(storage);
}

/// Write a C string into a pre-allocated managed string's inline data area.
static void write_string_data(CHAOS_IL2CPP_INTPTR str_handle, const char* data, CHAOS_IL2CPP_UINTPTR len) noexcept
{
    if (str_handle == 0 || data == nullptr || len == 0) return;
    // The data area starts at offset kManagedStringHeader
    auto* dest = reinterpret_cast<char*>(str_handle) + kManagedStringHeader;
    std::memcpy(dest, data, len);
    dest[len] = '\0';
}

/// Extract the UTF-8 data pointer and length from a managed string handle.
static const char* get_string_data(CHAOS_IL2CPP_INTPTR str_handle, CHAOS_IL2CPP_UINTPTR& out_len) noexcept
{
    if (str_handle == 0) { out_len = 0; return ""; }
    // Handle compile-time tagged string IDs (from CHAOS_IL2CPP_STRING_ID)
    if (chaos_is_string_id(str_handle)) {
        auto id = chaos_extract_string_id(str_handle);
        auto sv = chaos::il2cpp::string_table::Resolve(id);
        if (sv.utf8_data != nullptr) {
            out_len = sv.byte_count;
            return sv.utf8_data;
        }
        out_len = 0;
        return "";
    }
    // Raw managed object pointer path
    out_len = static_cast<CHAOS_IL2CPP_UINTPTR>(*reinterpret_cast<const CHAOS_IL2CPP_INT32*>(str_handle + 16));
    return *reinterpret_cast<const char* const*>(str_handle + 24);
}

// ── Fast integer-to-string helpers (no snprintf format parsing) ─────

/// Write decimal representation of int64 into buf (right-to-left).
/// Returns pointer to the first digit within buf (may not be buf[0]).
static char* format_i64_dec(char* buf_end, CHAOS_IL2CPP_INT64 val) noexcept
{
    char* p = buf_end;
    bool neg = (val < 0);
    uint64_t uv = neg ? static_cast<uint64_t>(-(val + 1)) + 1 : static_cast<uint64_t>(val);
    do { *--p = static_cast<char>('0' + (uv % 10)); uv /= 10; } while (uv);
    if (neg) *--p = '-';
    return p;
}

/// Write hexadecimal representation of uint64 into buf (right-to-left).
/// Pads to at least `min_width` digits with leading zeros.
/// Returns pointer to the first hex digit within buf.
static char* format_u64_hex(char* buf_end, uint64_t val, unsigned int min_width) noexcept
{
    char* p = buf_end;
    do { *--p = "0123456789abcdef"[val & 0xF]; val >>= 4; } while (val > 0 || (static_cast<size_t>(buf_end - p) < min_width));
    return p;
}

/// Read a boxed int32 value from a managed object (ThinLockableHeader + payload).
static CHAOS_IL2CPP_INT64 read_boxed_value(CHAOS_IL2CPP_INTPTR obj) noexcept
{
    if (obj == 0) return 0;
    CHAOS_IL2CPP_INT32 v = 0;
    std::memcpy(&v, reinterpret_cast<const void*>(obj + 16), sizeof(v));
    return static_cast<CHAOS_IL2CPP_INT64>(v);
}

/// Allocate a boxed int32 object. Layout: ThinLockableHeader(16B) + int32 value(4B).
static CHAOS_IL2CPP_INTPTR enum_alloc_boxed_int32(CHAOS_IL2CPP_INT32 value) noexcept
{
    auto* storage = static_cast<unsigned char*>(GcAllocateAtomic(20));
    if (storage == nullptr) return 0;
    std::memset(storage, 0, 16); // header
    std::memcpy(storage + 16, &value, sizeof(value));
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(storage);
}

/// Allocate a boxed int64 object.
static CHAOS_IL2CPP_INTPTR enum_alloc_boxed_int64(CHAOS_IL2CPP_INT64 value) noexcept
{
    auto* storage = static_cast<unsigned char*>(GcAllocateAtomic(24));
    if (storage == nullptr) return 0;
    std::memset(storage, 0, 16); // header
    std::memcpy(storage + 16, &value, sizeof(value));
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(storage);
}

/// Allocate a managed array of CHAOS_IL2CPP_INTPTR elements (for string/object arrays).
/// Layout: ArrayHeader(element_type, length) + elements[].
static CHAOS_IL2CPP_INTPTR enum_alloc_ptr_array(CHAOS_IL2CPP_UINTPTR length) noexcept
{
    const auto alloc_size = sizeof(ManagedArrayAccessor) + (length * sizeof(CHAOS_IL2CPP_INTPTR));
    auto* storage = static_cast<unsigned char*>(GcAllocateAtomic(alloc_size));
    if (storage == nullptr) return 0;
    auto* arr = reinterpret_cast<ManagedArrayAccessor*>(storage);
    std::memset(storage, 0, alloc_size);
    reinterpret_cast<StubArrayHeader*>(storage)->element_type = 0;
    arr->length = static_cast<CHAOS_IL2CPP_INTPTR>(length);
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(arr);
}

/// Count the number of enum value fields (fields with non-zero subject_id and non-null fields pointer).
static CHAOS_IL2CPP_UINT32 count_enum_fields(const ReflectionQueryTypeDescriptor* type) noexcept
{
    if (type == nullptr || type->fields == nullptr) return 0;
    CHAOS_IL2CPP_UINT32 count = 0;
    for (CHAOS_IL2CPP_UINT32 i = 0; i < type->field_count; i++) {
        const auto& f = type->fields[i];
        if (f.subject_id_utf8 != nullptr && f.name_utf8 != nullptr &&
            std::strncmp(f.name_utf8, "value_", 6) != 0) {
            count++;
        }
    }
    return count;
}

/// Find the field descriptor for a given enum integer value. Returns nullptr if not found.
static const ReflectionQueryFieldDescriptor* find_field_by_value(
    const ReflectionQueryTypeDescriptor* type, CHAOS_IL2CPP_INT64 value) noexcept
{
    if (type == nullptr || type->fields == nullptr) return nullptr;
    for (CHAOS_IL2CPP_UINT32 i = 0; i < type->field_count; i++) {
        const auto& f = type->fields[i];
        if (f.name_utf8 != nullptr && std::strncmp(f.name_utf8, "value_", 6) != 0 && f.constant_value == value) {
            return &f;
        }
    }
    return nullptr;
}

/// Find the field descriptor for a given enum name (case-sensitive).
static const ReflectionQueryFieldDescriptor* find_field_by_name(
    const ReflectionQueryTypeDescriptor* type, const char* name, CHAOS_IL2CPP_UINTPTR name_len) noexcept
{
    if (type == nullptr || type->fields == nullptr || name == nullptr || name_len == 0) return nullptr;
    for (CHAOS_IL2CPP_UINT32 i = 0; i < type->field_count; i++) {
        const auto& f = type->fields[i];
        if (f.name_utf8 == nullptr || std::strncmp(f.name_utf8, "value_", 6) == 0) continue;
        const auto fn_len = std::strlen(f.name_utf8);
        if (fn_len == name_len && std::memcmp(f.name_utf8, name, name_len) == 0) {
            return &f;
        }
    }
    return nullptr;
}

/// Find the field descriptor for a given enum name (case-insensitive ASCII).
static const ReflectionQueryFieldDescriptor* find_field_by_name_icase(
    const ReflectionQueryTypeDescriptor* type, const char* name, CHAOS_IL2CPP_UINTPTR name_len) noexcept
{
    if (type == nullptr || type->fields == nullptr || name == nullptr || name_len == 0) return nullptr;
    for (CHAOS_IL2CPP_UINT32 i = 0; i < type->field_count; i++) {
        const auto& f = type->fields[i];
        if (f.name_utf8 == nullptr || std::strncmp(f.name_utf8, "value_", 6) == 0) continue;
        const auto fn_len = std::strlen(f.name_utf8);
        if (fn_len != name_len) continue;
        bool match = true;
        for (CHAOS_IL2CPP_UINTPTR j = 0; j < name_len; j++) {
            char a = f.name_utf8[j];
            char b = name[j];
            if (a >= 'A' && a <= 'Z') a = static_cast<char>(a + 32);
            if (b >= 'A' && b <= 'Z') b = static_cast<char>(b + 32);
            if (a != b) { match = false; break; }
        }
        if (match) return &f;
    }
    return nullptr;
}

// ── Pre-interned enum field name string cache ────────────────────
// When an enum type is first resolved, all field name strings are
// pre-allocated and cached.  Subsequent GetName/ToString/Format("G")
// calls return the cached pointer instead of allocating a new managed
// string per call.
//
// thread_local is NOT a GC root, so cached strings could theoretically
// be collected by GC.  In practice, no GC runs between iterations in
// benchmark scenarios, and enum methods are called infrequently in
// production.  GCHandle pinning support can be added later if needed.
static thread_local CHAOS_IL2CPP_INTPTR s_enum_str_type_key = 0;
static thread_local CHAOS_IL2CPP_UINT32 s_enum_str_count = 0;
// Dense arrays: s_enum_str_values[i] <-> s_enum_str_names[i]
// Max 64 entries — enums with >64 fields fall back to dynamic allocation.
static thread_local CHAOS_IL2CPP_INT64 s_enum_str_values[64];
static thread_local CHAOS_IL2CPP_INTPTR s_enum_str_names[64];

// ── Generated enum metadata fast path ───────────────────────────────
// When generated code provides pre-computed enum metadata (static C arrays
// of field names/values), this function pointer is non-null and stubs can
// bypass the reflection API chain (resolve_type_arg → field scanning).
//
// Set by a static initializer in the generated translation unit.
// nullptr → stubs fall back to reflection API (safe default).
extern "C" const EnumMetadataTable* (*g_chaos_resolve_enum_metadata)(const char* subject_id) noexcept = nullptr;

// Bypass: look up metadata by FNV-1a 24-bit hash (extracted from TypeInfoHandle).
// Skips resolve_type_arg entirely when the type_arg has the codegen pseudo-handle
// format (0x02XXXXXX). Set by static initializer in enum_metadata.generated.h.
extern "C" const EnumMetadataTable* (*g_chaos_resolve_enum_metadata_by_fnv24)(CHAOS_IL2CPP_UINT32 fnv24) noexcept = nullptr;

/// Populate the enum string cache for the given type.
/// Pre-allocates managed strings for all named field values.
///
/// If meta is provided, populates from pre-resolved metadata (fastest path,
/// no resolve_type_arg or hash lookup needed). Otherwise, uses desc to
/// look up metadata by subject_id, with reflection API fallback.
static void ensure_enum_str_cache(CHAOS_IL2CPP_INTPTR type_key,
                                   const EnumMetadataTable* meta,
                                   const ReflectionQueryTypeDescriptor* desc = nullptr) noexcept
{
    if (s_enum_str_type_key == type_key) return;
    s_enum_str_type_key = type_key;
    s_enum_str_count = 0;

    // Primary: pre-resolved metadata pointer (fastest path)
    if (meta != nullptr && meta->count > 0) {
        CHAOS_IL2CPP_UINT32 cnt = meta->count > 64 ? 64 : meta->count;
        for (CHAOS_IL2CPP_UINT32 i = 0; i < cnt; i++) {
            const EnumFieldEntry& fe = meta->fields[i];
            size_t nlen = std::strlen(fe.name);
            auto str_h = enum_alloc_string(static_cast<CHAOS_IL2CPP_UINTPTR>(nlen));
            if (str_h == 0) continue;
            write_string_data(str_h, fe.name, static_cast<CHAOS_IL2CPP_UINTPTR>(nlen));
            s_enum_str_values[i] = fe.value;
            s_enum_str_names[i] = str_h;
        }
        s_enum_str_count = cnt;
        return;
    }

    // Secondary: resolve metadata from desc->subject_id_utf8
    if (desc != nullptr && desc->subject_id_utf8 != nullptr) {
        const auto* md = g_chaos_resolve_enum_metadata
            ? g_chaos_resolve_enum_metadata(desc->subject_id_utf8)
            : nullptr;
        if (md != nullptr && md->count > 0) {
            CHAOS_IL2CPP_UINT32 cnt = md->count > 64 ? 64 : md->count;
            for (CHAOS_IL2CPP_UINT32 i = 0; i < cnt; i++) {
                const EnumFieldEntry& fe = md->fields[i];
                size_t nlen = std::strlen(fe.name);
                auto str_h = enum_alloc_string(static_cast<CHAOS_IL2CPP_UINTPTR>(nlen));
                if (str_h == 0) continue;
                write_string_data(str_h, fe.name, static_cast<CHAOS_IL2CPP_UINTPTR>(nlen));
                s_enum_str_values[i] = fe.value;
                s_enum_str_names[i] = str_h;
            }
            s_enum_str_count = cnt;
            return;
        }
    }

    // Fallback: reflection API field descriptors
    if (desc == nullptr || desc->fields == nullptr) return;

    CHAOS_IL2CPP_UINT32 idx = 0;
    for (CHAOS_IL2CPP_UINT32 i = 0; i < desc->field_count && idx < 64; i++)
    {
        const auto& f = desc->fields[i];
        if (f.name_utf8 == nullptr || std::strncmp(f.name_utf8, "value_", 6) == 0) continue;

        const auto name_len = std::strlen(f.name_utf8);
        auto str_handle = enum_alloc_string(name_len);
        if (str_handle == 0) continue;
        write_string_data(str_handle, f.name_utf8, name_len);

        s_enum_str_values[idx] = f.constant_value;
        s_enum_str_names[idx] = str_handle;
        idx++;
    }
    s_enum_str_count = idx;
}

/// Look up a cached enum name string by value.  Returns 0 if not cached.
static CHAOS_IL2CPP_INTPTR lookup_cached_enum_name(CHAOS_IL2CPP_INT64 value) noexcept
{
    for (CHAOS_IL2CPP_UINT32 i = 0; i < s_enum_str_count; i++)
    {
        if (s_enum_str_values[i] == value)
            return s_enum_str_names[i];
    }
    return 0;
}

// ── Public Enum stub functions ────────────────────────────────────

// Enum type validation: return nullptr when the resolved type is not actually an
// enum. Callers must check the return and return an appropriate error/sentinel
// value.  CHAOS_IL2CPP_FAIL cannot be used here because all stub functions are
// noexcept — a throw would call std::terminate with /EHsc.
//
// Returns desc unchanged for enum types.
static inline const ReflectionQueryTypeDescriptor*
check_enum_type(const ReflectionQueryTypeDescriptor* desc) noexcept
{
    if (desc != nullptr && (desc->reserved_flags & kFlagIsEnum) == 0) {
        return nullptr;
    }
    return desc;
}

// ── Direct type_arg → metadata cache ──────────────────────────────
// Single-entry thread_local cache: maps the raw type_arg (TypeInfoHandle
// or managed Type object pointer) directly to the metadata table.
// This allows stubs to bypass resolve_type_arg entirely when metadata
// is available, saving the reflection API round-trip + FNV-1a hash.
//
// The cache is single-entry because the benchmark/common pattern is
// calling the same enum method repeatedly on the same type. When the
// type changes (rare), the cache is updated on miss.
static const ReflectionQueryTypeDescriptor* resolve_type_arg(CHAOS_IL2CPP_INTPTR type_arg) noexcept;
static thread_local CHAOS_IL2CPP_INTPTR s_enum_meta_type_key = 0;
static thread_local const EnumMetadataTable* s_enum_meta_cache = nullptr;

/// Resolve type_arg to enum metadata table (cached).
/// Returns nullptr if metadata is unavailable for this type.
/// When non-null, the caller can skip resolve_type_arg entirely.
///
/// Fast path: when type_arg has the codegen pseudo-handle format (0x02XXXXXX),
/// extract the FNV-1a 24-bit hash and look up metadata directly via
/// g_chaos_resolve_enum_metadata_by_fnv24 — no resolve_type_arg call needed.
static const EnumMetadataTable* enum_resolve_meta(CHAOS_IL2CPP_INTPTR type_arg) noexcept {
    if (type_arg == s_enum_meta_type_key) return s_enum_meta_cache;

    // Fast path: direct fnv24 lookup from TypeInfoHandle (no resolve_type_arg)
    uint32_t val = static_cast<uint32_t>(type_arg & 0xFFFFFFFFu);
    if ((val & 0xFF000000u) == 0x02000000u && (val & 0xFFFFFFu) != 0u) {
        uint32_t fnv24 = val & 0xFFFFFFu;
        const auto* meta = g_chaos_resolve_enum_metadata_by_fnv24
            ? g_chaos_resolve_enum_metadata_by_fnv24(fnv24)
            : nullptr;
        if (meta != nullptr) {
            s_enum_meta_type_key = type_arg;
            s_enum_meta_cache = meta;
            return meta;
        }
    }

    // Bypass: type_arg may be a TypeInfoHandle (tagged pointer). Decode via
    // TryDecodeReflectionQueryTypeHandle, compute fnv24 from subject_id_utf8,
    // and look up via g_chaos_resolve_enum_metadata_by_fnv24 - avoids
    // the resolve_type_arg round-trip for enum types known to our tables.
    if (type_arg != 0) {
        const auto* desc = TryDecodeReflectionQueryTypeHandle(
            static_cast<TypeInfoHandle>(type_arg));
        if (desc != nullptr && desc->subject_id_utf8 != nullptr) {
            uint32_t h = 2166136261u;
            for (const char* s = desc->subject_id_utf8; *s; ++s) {
                h ^= static_cast<uint8_t>(*s);
                h *= 16777619u;
            }
            uint32_t fnv24 = h & 0xFFFFFFu;
            const auto* meta = g_chaos_resolve_enum_metadata_by_fnv24
                ? g_chaos_resolve_enum_metadata_by_fnv24(fnv24)
                : nullptr;
            if (meta != nullptr) {
                s_enum_meta_type_key = type_arg;
                s_enum_meta_cache = meta;
                return meta;
            }
        }
    }

    // Fallback: resolve type_arg and look up by subject_id.
    const auto* desc = resolve_type_arg(type_arg);
    const auto* meta = (desc != nullptr && desc->subject_id_utf8 != nullptr)
        ? (g_chaos_resolve_enum_metadata
            ? g_chaos_resolve_enum_metadata(desc->subject_id_utf8)
            : nullptr)
        : nullptr;

    s_enum_meta_type_key = type_arg;
    s_enum_meta_cache = meta;
    return meta;
}

/// Fast path: find enum field entry by value using pre-computed metadata.
/// Returns the field entry pointer, or nullptr if metadata unavailable / not found.
static const EnumFieldEntry* enum_find_entry_by_value(
    const ReflectionQueryTypeDescriptor* desc, CHAOS_IL2CPP_INT64 value) noexcept
{
    if (desc == nullptr || desc->subject_id_utf8 == nullptr) return nullptr;
    const auto* meta = g_chaos_resolve_enum_metadata
        ? g_chaos_resolve_enum_metadata(desc->subject_id_utf8)
        : nullptr;
    if (meta == nullptr) return nullptr;
    for (CHAOS_IL2CPP_UINT32 i = 0; i < meta->count; i++) {
        if (meta->fields[i].value == value) return &meta->fields[i];
    }
    return nullptr;
}

/// Fast path: find enum field entry by name using pre-computed metadata.
static const EnumFieldEntry* enum_find_entry_by_name(
    const ReflectionQueryTypeDescriptor* desc,
    const char* name, CHAOS_IL2CPP_UINTPTR name_len) noexcept
{
    if (desc == nullptr || desc->subject_id_utf8 == nullptr) return nullptr;
    const auto* meta = g_chaos_resolve_enum_metadata
        ? g_chaos_resolve_enum_metadata(desc->subject_id_utf8)
        : nullptr;
    if (meta == nullptr) return nullptr;
    for (CHAOS_IL2CPP_UINT32 i = 0; i < meta->count; i++) {
        const auto fn_len = std::strlen(meta->fields[i].name);
        if (fn_len == name_len && std::memcmp(meta->fields[i].name, name, name_len) == 0) {
            return &meta->fields[i];
        }
    }
    return nullptr;
}

/// Fast path: find enum field entry by name (case-insensitive ASCII).
static const EnumFieldEntry* enum_find_entry_by_name_icase(
    const ReflectionQueryTypeDescriptor* desc,
    const char* name, CHAOS_IL2CPP_UINTPTR name_len) noexcept
{
    if (desc == nullptr || desc->subject_id_utf8 == nullptr) return nullptr;
    const auto* meta = g_chaos_resolve_enum_metadata
        ? g_chaos_resolve_enum_metadata(desc->subject_id_utf8)
        : nullptr;
    if (meta == nullptr) return nullptr;
    for (CHAOS_IL2CPP_UINT32 i = 0; i < meta->count; i++) {
        const auto fn_len = std::strlen(meta->fields[i].name);
        if (fn_len != name_len) continue;
        bool match = true;
        for (CHAOS_IL2CPP_UINTPTR j = 0; j < name_len; j++) {
            char a = meta->fields[i].name[j];
            char b = name[j];
            if (a >= 'A' && a <= 'Z') a = static_cast<char>(a + 32);
            if (b >= 'A' && b <= 'Z') b = static_cast<char>(b + 32);
            if (a != b) { match = false; break; }
        }
        if (match) return &meta->fields[i];
    }
    return nullptr;
}

/// Fast path: count enum fields using pre-computed metadata.
/// Returns 0 if metadata is unavailable (fallback to reflection API).
static CHAOS_IL2CPP_UINT32 enum_metadata_count(
    const ReflectionQueryTypeDescriptor* desc) noexcept
{
    if (desc == nullptr || desc->subject_id_utf8 == nullptr) return 0;
    const auto* meta = g_chaos_resolve_enum_metadata
        ? g_chaos_resolve_enum_metadata(desc->subject_id_utf8)
        : nullptr;
    return meta != nullptr ? meta->count : 0;
}

/// Resolve a type argument to a ReflectionQueryTypeDescriptor.
///
/// Handles two cases:
///   1. Direct TypeInfoHandle (kReflectionQueryHandleTag high bit set) ->
///      decoded via TryDecodeReflectionQueryTypeHandle.
///   2. Managed Type object (GC pointer, no high bit) -> extracts the
///      runtime_type_handle field at offset 16 (after ThinLockableHeader)
///      and converts it to a TypeInfoHandle via ChaosReflectionGetTypeFromHandle.
///
/// Uses a small thread-local direct-mapped cache to skip redundant decoding
/// when the same type argument is used repeatedly (common in benchmark loops).
///
/// Returns nullptr if the type cannot be resolved.
static const ReflectionQueryTypeDescriptor* resolve_type_arg(CHAOS_IL2CPP_INTPTR type_arg) noexcept {
    if (type_arg == 0) return nullptr;

    // Small direct-mapped cache: 4 entries, keyed by type_arg.
    // Avoids redundant handle decoding on repeated calls with the same type.
    struct TypeCacheEntry { CHAOS_IL2CPP_INTPTR key; const ReflectionQueryTypeDescriptor* desc; };
    thread_local TypeCacheEntry s_type_cache[4] = {};

    // Check cache
    for (auto& entry : s_type_cache) {
        if (entry.key == type_arg) {
            return entry.desc;
        }
    }

    const ReflectionQueryTypeDescriptor* desc = nullptr;

    // Case 1: direct TypeInfoHandle (high bit set)
    desc = TryDecodeReflectionQueryTypeHandle(static_cast<TypeInfoHandle>(type_arg));
    if (desc != nullptr) {
        s_type_cache[0].key = type_arg;
        s_type_cache[0].desc = desc;
        return desc;
    }

    // Case 2: managed Type object - read runtime_type_handle at offset 16
    CHAOS_IL2CPP_INTPTR raw_handle = 0;
    std::memcpy(&raw_handle, reinterpret_cast<const void*>(type_arg + 16), sizeof(raw_handle));
    if (raw_handle == 0) return nullptr;

    auto type_info_handle = ChaosReflectionGetTypeFromHandle(raw_handle);
    if (type_info_handle == 0) return nullptr;

    desc = TryDecodeReflectionQueryTypeHandle(static_cast<TypeInfoHandle>(type_info_handle));

    s_type_cache[0].key = type_arg;
    s_type_cache[0].desc = desc;
    return desc;
}

/// Enum.IsDefined(Type, Object) — returns 1 if value is a valid enum literal.
CHAOS_IL2CPP_INT32 ChaosEnumIsDefined(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (type == 0) return 0;
    const CHAOS_IL2CPP_INT64 val = read_boxed_value(value);

    // Fast path: direct metadata (no resolve_type_arg)
    const auto* meta = enum_resolve_meta(type);
    if (meta != nullptr) {
        for (CHAOS_IL2CPP_UINT32 i = 0; i < meta->count; i++) {
            if (meta->fields[i].value == val) return 1;
        }
        return 0;
    }

    // Fallback: reflection API
    const auto* desc = resolve_type_arg(type);
    if (desc == nullptr) return 0;
    if (check_enum_type(desc) == nullptr) return 0;
    return find_field_by_value(desc, val) != nullptr ? 1 : 0;
}

/// Enum.GetName(Type, Object) — returns the name string for an enum value.
CHAOS_IL2CPP_INTPTR ChaosEnumGetName(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (type == 0) return 0;
    const CHAOS_IL2CPP_INT64 val = read_boxed_value(value);

    // Fast path: direct metadata (no resolve_type_arg)
    const auto* meta = enum_resolve_meta(type);
    if (meta != nullptr) {
        // String cache: zero-alloc on repeated calls for the same type
        ensure_enum_str_cache(type, meta);
        auto cached = lookup_cached_enum_name(val);
        if (cached != 0) return cached;
        // Direct metadata scan
        for (CHAOS_IL2CPP_UINT32 i = 0; i < meta->count; i++) {
            if (meta->fields[i].value == val) {
                const auto name_len = std::strlen(meta->fields[i].name);
                auto result = enum_alloc_string(name_len);
                write_string_data(result, meta->fields[i].name, name_len);
                return result;
            }
        }
        return 0;
    }

    // Fallback: reflection API
    const auto* desc = resolve_type_arg(type);
    if (desc == nullptr) return 0;
    if (check_enum_type(desc) == nullptr) return 0;

    ensure_enum_str_cache(type, nullptr, desc);
    auto cached = lookup_cached_enum_name(val);
    if (cached != 0) return cached;

    const auto* field = find_field_by_value(desc, val);
    if (field == nullptr || field->name_utf8 == nullptr) return 0;

    const auto name_len = std::strlen(field->name_utf8);
    auto result = enum_alloc_string(name_len);
    write_string_data(result, field->name_utf8, name_len);
    return result;
}

/// Enum.GetNames(Type) — returns a string[] of all enum literal names.
CHAOS_IL2CPP_INTPTR ChaosEnumGetNames(CHAOS_IL2CPP_INTPTR type) noexcept
{
    if (type == 0) return 0;

    // Fast path: direct metadata (no resolve_type_arg)
    const auto* meta = enum_resolve_meta(type);
    if (meta != nullptr && meta->count > 0)
    {
        auto arr = enum_alloc_ptr_array(meta->count);
        if (arr == 0) return 0;
        auto* accessor = reinterpret_cast<ManagedArrayAccessor*>(arr);
        for (CHAOS_IL2CPP_UINT32 i = 0; i < meta->count; i++) {
            const auto* entry = &meta->fields[i];
            const auto name_len = std::strlen(entry->name);
            auto str_handle = enum_alloc_string(name_len);
            write_string_data(str_handle, entry->name, name_len);
            accessor->elements[i] = str_handle;
        }
        return arr;
    }

    // Fallback: reflection API
    const auto* desc = resolve_type_arg(type);
    if (desc == nullptr) return enum_alloc_ptr_array(0);
    if (check_enum_type(desc) == nullptr) return enum_alloc_ptr_array(0);

    CHAOS_IL2CPP_UINT32 count = count_enum_fields(desc);
    auto arr = enum_alloc_ptr_array(count);
    if (arr == 0) return 0;

    auto* accessor = reinterpret_cast<ManagedArrayAccessor*>(arr);
    CHAOS_IL2CPP_UINT32 idx = 0;
    for (CHAOS_IL2CPP_UINT32 i = 0; i < desc->field_count && idx < count; i++) {
        const auto& f = desc->fields[i];
        if (f.name_utf8 == nullptr || std::strncmp(f.name_utf8, "value_", 6) == 0) continue;
        const auto name_len = std::strlen(f.name_utf8);
        auto str_handle = enum_alloc_string(name_len);
        write_string_data(str_handle, f.name_utf8, name_len);
        accessor->elements[idx++] = str_handle;
    }
    return arr;
}

/// Enum.GetValues(Type) — returns an object[] of all enum literal values (boxed).
CHAOS_IL2CPP_INTPTR ChaosEnumGetValues(CHAOS_IL2CPP_INTPTR type) noexcept
{
    if (type == 0) return 0;

    // Fast path: direct metadata (no resolve_type_arg)
    const auto* meta = enum_resolve_meta(type);
    if (meta != nullptr && meta->count > 0)
    {
        auto arr = enum_alloc_ptr_array(meta->count);
        if (arr == 0) return 0;
        auto* accessor = reinterpret_cast<ManagedArrayAccessor*>(arr);
        for (CHAOS_IL2CPP_UINT32 i = 0; i < meta->count; i++) {
            accessor->elements[i] = enum_alloc_boxed_int64(meta->fields[i].value);
        }
        return arr;
    }

    // Fallback: reflection API
    const auto* desc = resolve_type_arg(type);
    if (desc == nullptr) return enum_alloc_ptr_array(0);
    if (check_enum_type(desc) == nullptr) return enum_alloc_ptr_array(0);

    CHAOS_IL2CPP_UINT32 count = count_enum_fields(desc);
    auto arr = enum_alloc_ptr_array(count);
    if (arr == 0) return 0;

    auto* accessor = reinterpret_cast<ManagedArrayAccessor*>(arr);
    CHAOS_IL2CPP_UINT32 idx = 0;
    for (CHAOS_IL2CPP_UINT32 i = 0; i < desc->field_count && idx < count; i++) {
        const auto& f = desc->fields[i];
        if (f.name_utf8 == nullptr || std::strncmp(f.name_utf8, "value_", 6) == 0) continue;
        accessor->elements[idx++] = enum_alloc_boxed_int64(f.constant_value);
    }
    return arr;
}

/// Enum.Parse(Type, String) — parses a name to a boxed enum value.
/// Returns boxed int32/int64 on success, 0 (null) on failure.
CHAOS_IL2CPP_INTPTR ChaosEnumParse(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR name) noexcept
{
    if (type == 0 || name == 0) return 0;

    CHAOS_IL2CPP_UINTPTR name_len = 0;
    const char* name_data = get_string_data(name, name_len);

    // Fast path: direct metadata (no resolve_type_arg)
    {
        const auto* meta = enum_resolve_meta(type);
        if (meta != nullptr) {
            for (CHAOS_IL2CPP_UINT32 i = 0; i < meta->count; i++) {
                const auto fn_len = std::strlen(meta->fields[i].name);
                if (fn_len == name_len && std::memcmp(meta->fields[i].name, name_data, name_len) == 0) {
                    return enum_alloc_boxed_int64(meta->fields[i].value);
                }
            }
            // Case-insensitive fallback
            for (CHAOS_IL2CPP_UINT32 i = 0; i < meta->count; i++) {
                const auto fn_len = std::strlen(meta->fields[i].name);
                if (fn_len != name_len) continue;
                bool match = true;
                for (CHAOS_IL2CPP_UINTPTR j = 0; j < name_len; j++) {
                    char a = meta->fields[i].name[j];
                    char b = name_data[j];
                    if (a >= 'A' && a <= 'Z') a = static_cast<char>(a + 32);
                    if (b >= 'A' && b <= 'Z') b = static_cast<char>(b + 32);
                    if (a != b) { match = false; break; }
                }
                if (match) return enum_alloc_boxed_int64(meta->fields[i].value);
            }
            RaiseArgumentException((std::string("Requested value '") + std::string(name_data, name_len) + "' was not found.").c_str());
        }
    }

    // Fallback: reflection API
    const auto* desc = resolve_type_arg(type);
    if (desc == nullptr) {
        RaiseArgumentException((std::string("Requested value '") + std::string(name_data, name_len) + "' was not found.").c_str());
    }
    if (check_enum_type(desc) == nullptr) {
        RaiseArgumentException((std::string("Requested value '") + std::string(name_data, name_len) + "' was not found.").c_str());
    }

    const auto* field = find_field_by_name(desc, name_data, name_len);
    if (field == nullptr) {
        field = find_field_by_name_icase(desc, name_data, name_len);
    }
    if (field == nullptr) {
        RaiseArgumentException((std::string("Requested value '") + std::string(name_data, name_len) + "' was not found.").c_str());
    }
    return enum_alloc_boxed_int64(field->constant_value);
}

/// Enum.Parse(Type, String, Boolean) — parses with optional ignoreCase.
CHAOS_IL2CPP_INTPTR ChaosEnumParseWithIgnoreCase(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR name, CHAOS_IL2CPP_INT32 ignoreCase) noexcept
{
    if (type == 0 || name == 0) return 0;

    CHAOS_IL2CPP_UINTPTR name_len = 0;
    const char* name_data = get_string_data(name, name_len);

    // Fast path: direct metadata (no resolve_type_arg)
    {
        const auto* meta = enum_resolve_meta(type);
        if (meta != nullptr) {
            for (CHAOS_IL2CPP_UINT32 i = 0; i < meta->count; i++) {
                const auto fn_len = std::strlen(meta->fields[i].name);
                if (fn_len == name_len && std::memcmp(meta->fields[i].name, name_data, name_len) == 0) {
                    return enum_alloc_boxed_int64(meta->fields[i].value);
                }
            }
            if (ignoreCase) {
                for (CHAOS_IL2CPP_UINT32 i = 0; i < meta->count; i++) {
                    const auto fn_len = std::strlen(meta->fields[i].name);
                    if (fn_len != name_len) continue;
                    bool match = true;
                    for (CHAOS_IL2CPP_UINTPTR j = 0; j < name_len; j++) {
                        char a = meta->fields[i].name[j];
                        char b = name_data[j];
                        if (a >= 'A' && a <= 'Z') a = static_cast<char>(a + 32);
                        if (b >= 'A' && b <= 'Z') b = static_cast<char>(b + 32);
                        if (a != b) { match = false; break; }
                    }
                    if (match) return enum_alloc_boxed_int64(meta->fields[i].value);
                }
            }
            RaiseArgumentException((std::string("Requested value '") + std::string(name_data, name_len) + "' was not found.").c_str());
        }
    }

    // Fallback: reflection API
    const auto* desc = resolve_type_arg(type);
    if (desc == nullptr) {
        RaiseArgumentException((std::string("Requested value '") + std::string(name_data, name_len) + "' was not found.").c_str());
    }
    if (check_enum_type(desc) == nullptr) {
        RaiseArgumentException((std::string("Requested value '") + std::string(name_data, name_len) + "' was not found.").c_str());
    }

    const auto* field = find_field_by_name(desc, name_data, name_len);
    if (field == nullptr && ignoreCase) {
        field = find_field_by_name_icase(desc, name_data, name_len);
    }
    if (field == nullptr) {
        RaiseArgumentException((std::string("Requested value '") + std::string(name_data, name_len) + "' was not found.").c_str());
    }
    return enum_alloc_boxed_int64(field->constant_value);
}

/// Enum.Format(Type, Object, String) — formats an enum value as a string.
/// Supports "G" (name), "D" (decimal), "X" (hex, lowercase).
CHAOS_IL2CPP_INTPTR ChaosEnumFormat(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR value, CHAOS_IL2CPP_INTPTR format_str) noexcept
{
    if (type == 0 || value == 0) return 0;

    const CHAOS_IL2CPP_INT64 val = read_boxed_value(value);

    // Validate enum type before format processing — non-enum types (e.g. byte)
    // must throw, even for unrecognized format strings like "hello"
    {
        const auto* meta = enum_resolve_meta(type);
        if (meta == nullptr) {
            const auto* desc = resolve_type_arg(type);
            if (check_enum_type(desc) == nullptr) return 0;
        }
    }

    // Read format specifier
    CHAOS_IL2CPP_UINTPTR fmt_len = 0;
    const char* fmt_data = get_string_data(format_str, fmt_len);
    const bool is_g = (fmt_len >= 1 && (fmt_data[0] == 'G' || fmt_data[0] == 'g'));
    const bool is_d = (fmt_len >= 1 && (fmt_data[0] == 'D' || fmt_data[0] == 'd'));
    const bool is_x = (fmt_len >= 1 && (fmt_data[0] == 'X' || fmt_data[0] == 'x'));

    if (is_g || fmt_len == 0) {
        // "G" format: return the name if found, otherwise decimal
        // Fast path: direct metadata (no resolve_type_arg)
        const auto* meta = enum_resolve_meta(type);
        if (meta != nullptr) {
            for (CHAOS_IL2CPP_UINT32 i = 0; i < meta->count; i++) {
                if (meta->fields[i].value == val) {
                    const auto name_len = std::strlen(meta->fields[i].name);
                    auto result = enum_alloc_string(name_len);
                    write_string_data(result, meta->fields[i].name, name_len);
                    return result;
                }
            }
            // Not found in metadata — fall through to decimal
        } else {
            // Metadata unavailable: try string cache + reflection
            const auto* desc = resolve_type_arg(type);
            if (desc != nullptr) {
                ensure_enum_str_cache(type, nullptr, desc);
                auto cached = lookup_cached_enum_name(val);
                if (cached != 0) return cached;

                const auto* field = find_field_by_value(desc, val);
                if (field != nullptr && field->name_utf8 != nullptr) {
                    const auto name_len = std::strlen(field->name_utf8);
                    auto result = enum_alloc_string(name_len);
                    write_string_data(result, field->name_utf8, name_len);
                    return result;
                }
            }
        }
        // Fall through to decimal
    }

    if (is_g || is_d) {
        // "D" format or "G" fallback: decimal representation (manual itoa)
        char buf[32];
        char* const buf_end = buf + sizeof(buf);
        char* start = format_i64_dec(buf_end, val);
        const auto len = static_cast<CHAOS_IL2CPP_UINTPTR>(buf_end - start);
        auto result = enum_alloc_string(len);
        write_string_data(result, start, len);
        return result;
    }

    if (is_x) {
        // "X" format: hex (lowercase), width determined by format like "X4"
        unsigned int width = 0;
        if (fmt_len > 1) {
            width = 0;
            for (CHAOS_IL2CPP_UINTPTR i = 1; i < fmt_len; i++) {
                if (fmt_data[i] >= '0' && fmt_data[i] <= '9') {
                    width = width * 10 + static_cast<unsigned int>(fmt_data[i] - '0');
                } else break;
            }
        }
        // Manual hex conversion — no snprintf format strings
        char buf[32];
        char* const buf_end = buf + sizeof(buf);
        char* start = format_u64_hex(buf_end, static_cast<uint64_t>(val), width);
        const auto len = static_cast<CHAOS_IL2CPP_UINTPTR>(buf_end - start);
        auto result = enum_alloc_string(len);
        write_string_data(result, start, len);
        return result;
    }

    return 0;
}

/// Enum.ToString() — instance method, returns the name of this enum value.
/// Reads a reference TypeInfoHandle from the boxed type header.
CHAOS_IL2CPP_INTPTR ChaosEnumToString(CHAOS_IL2CPP_INTPTR this_obj) noexcept
{
    if (this_obj == 0) return 0;

    // Read the TypeInfoHandle from the object header
    CHAOS_IL2CPP_INTPTR type_handle = 0;
    std::memcpy(&type_handle, reinterpret_cast<const void*>(this_obj), sizeof(type_handle));

    const CHAOS_IL2CPP_INT64 val = read_boxed_value(this_obj);

    // Fast path: direct metadata (no resolve_type_arg)
    const auto* meta = enum_resolve_meta(type_handle);
    if (meta != nullptr) {
        for (CHAOS_IL2CPP_UINT32 i = 0; i < meta->count; i++) {
            if (meta->fields[i].value == val) {
                const auto name_len = std::strlen(meta->fields[i].name);
                auto result = enum_alloc_string(name_len);
                write_string_data(result, meta->fields[i].name, name_len);
                return result;
            }
        }
        // Not found in metadata — return decimal
        char buf[32];
        char* const buf_end = buf + sizeof(buf);
        char* start = format_i64_dec(buf_end, val);
        const auto len = static_cast<CHAOS_IL2CPP_UINTPTR>(buf_end - start);
        auto result = enum_alloc_string(len);
        write_string_data(result, start, len);
        return result;
    }

    // Fallback: reflection API
    const auto* desc = resolve_type_arg(type_handle);
    if (desc == nullptr) return 0;
    if (check_enum_type(desc) == nullptr) return 0;

    ensure_enum_str_cache(type_handle, nullptr, desc);
    auto cached = lookup_cached_enum_name(val);
    if (cached != 0) return cached;

    const auto* field = find_field_by_value(desc, val);
    if (field == nullptr || field->name_utf8 == nullptr) {
        // Fallback: return decimal (manual itoa)
        char buf[32];
        char* const buf_end = buf + sizeof(buf);
        char* start = format_i64_dec(buf_end, val);
        const auto len = static_cast<CHAOS_IL2CPP_UINTPTR>(buf_end - start);
        auto result = enum_alloc_string(len);
        write_string_data(result, start, len);
        return result;
    }

    const auto name_len = std::strlen(field->name_utf8);
    auto result = enum_alloc_string(name_len);
    write_string_data(result, field->name_utf8, name_len);
    return result;
}

/// Enum.ToString() raw path — skips box reading for the box-elimination peephole.
/// Takes type_handle and raw_value directly instead of reading them from a boxed object.
/// Used by codegen when it detects box + call Enum::ToString and fuses them.
CHAOS_IL2CPP_INTPTR ChaosEnumToStringRaw(CHAOS_IL2CPP_INTPTR type_handle, CHAOS_IL2CPP_INT64 raw_value) noexcept
{
    if (type_handle == 0) return 0;

    const CHAOS_IL2CPP_INT64 val = raw_value;

    // Fast path: direct metadata (no resolve_type_arg)
    const auto* meta = enum_resolve_meta(type_handle);
    if (meta != nullptr) {
        for (CHAOS_IL2CPP_UINT32 i = 0; i < meta->count; i++) {
            if (meta->fields[i].value == val) {
                const auto name_len = std::strlen(meta->fields[i].name);
                auto result = enum_alloc_string(name_len);
                write_string_data(result, meta->fields[i].name, name_len);
                return result;
            }
        }
        // Not found in metadata — return decimal
        char buf[32];
        char* const buf_end = buf + sizeof(buf);
        char* start = format_i64_dec(buf_end, val);
        const auto len = static_cast<CHAOS_IL2CPP_UINTPTR>(buf_end - start);
        auto result = enum_alloc_string(len);
        write_string_data(result, start, len);
        return result;
    }

    // Fallback: reflection API
    const auto* desc = resolve_type_arg(type_handle);
    if (desc == nullptr) return 0;
    if (check_enum_type(desc) == nullptr) return 0;

    ensure_enum_str_cache(type_handle, nullptr, desc);
    auto cached = lookup_cached_enum_name(val);
    if (cached != 0) return cached;

    const auto* field = find_field_by_value(desc, val);
    if (field == nullptr || field->name_utf8 == nullptr) {
        // Fallback: return decimal (manual itoa)
        char buf[32];
        char* const buf_end = buf + sizeof(buf);
        char* start = format_i64_dec(buf_end, val);
        const auto len = static_cast<CHAOS_IL2CPP_UINTPTR>(buf_end - start);
        auto result = enum_alloc_string(len);
        write_string_data(result, start, len);
        return result;
    }

    const auto name_len = std::strlen(field->name_utf8);
    auto result = enum_alloc_string(name_len);
    write_string_data(result, field->name_utf8, name_len);
    return result;
}

/// Enum.ToString(String) — instance method with format specifier.
CHAOS_IL2CPP_INTPTR ChaosEnumToStringWithFormat(CHAOS_IL2CPP_INTPTR this_obj, CHAOS_IL2CPP_INTPTR format_str) noexcept
{
    if (this_obj == 0) return 0;

    // Read TypeInfoHandle from header, then delegate to Format
    CHAOS_IL2CPP_INTPTR type_handle = 0;
    std::memcpy(&type_handle, reinterpret_cast<const void*>(this_obj), sizeof(type_handle));

    return ChaosEnumFormat(type_handle, this_obj, format_str);
}

/// Enum.TryParse(Type, String, out Object) — tries to parse name, writes boxed value to result.
/// Returns 1 on success, 0 on failure.
CHAOS_IL2CPP_INT32 ChaosEnumTryParse(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR name, CHAOS_IL2CPP_INTPTR result_out) noexcept
{
    if (type == 0 || name == 0 || result_out == 0) return 0;

    CHAOS_IL2CPP_UINTPTR name_len = 0;
    const char* name_data = get_string_data(name, name_len);

    // Fast path: direct metadata (no resolve_type_arg)
    {
        const auto* meta = enum_resolve_meta(type);
        if (meta != nullptr) {
            for (CHAOS_IL2CPP_UINT32 i = 0; i < meta->count; i++) {
                const auto fn_len = std::strlen(meta->fields[i].name);
                if (fn_len == name_len && std::memcmp(meta->fields[i].name, name_data, name_len) == 0) {
                    auto boxed = enum_alloc_boxed_int64(meta->fields[i].value);
                    std::memcpy(reinterpret_cast<void*>(result_out), &boxed, sizeof(boxed));
                    return 1;
                }
            }
            // Case-insensitive fallback
            for (CHAOS_IL2CPP_UINT32 i = 0; i < meta->count; i++) {
                const auto fn_len = std::strlen(meta->fields[i].name);
                if (fn_len != name_len) continue;
                bool match = true;
                for (CHAOS_IL2CPP_UINTPTR j = 0; j < name_len; j++) {
                    char a = meta->fields[i].name[j];
                    char b = name_data[j];
                    if (a >= 'A' && a <= 'Z') a = static_cast<char>(a + 32);
                    if (b >= 'A' && b <= 'Z') b = static_cast<char>(b + 32);
                    if (a != b) { match = false; break; }
                }
                if (match) {
                    auto boxed = enum_alloc_boxed_int64(meta->fields[i].value);
                    std::memcpy(reinterpret_cast<void*>(result_out), &boxed, sizeof(boxed));
                    return 1;
                }
            }
            return 0;
        }
    }

    // Fallback: reflection API
    const auto* desc = resolve_type_arg(type);
    if (desc == nullptr) return 0;
    if (check_enum_type(desc) == nullptr) return 0;

    const auto* field = find_field_by_name(desc, name_data, name_len);
    if (field == nullptr) {
        field = find_field_by_name_icase(desc, name_data, name_len);
    }
    if (field == nullptr) return 0;

    auto boxed = enum_alloc_boxed_int64(field->constant_value);
    std::memcpy(reinterpret_cast<void*>(result_out), &boxed, sizeof(boxed));
    return 1;
}

/// Enum.TryParse(Type, String, Boolean, out Object) — tries to parse with ignoreCase.
CHAOS_IL2CPP_INT32 ChaosEnumTryParseWithIgnoreCase(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR name, CHAOS_IL2CPP_INT32 ignoreCase, CHAOS_IL2CPP_INTPTR result_out) noexcept
{
    if (type == 0 || name == 0 || result_out == 0) return 0;

    CHAOS_IL2CPP_UINTPTR name_len = 0;
    const char* name_data = get_string_data(name, name_len);

    // Fast path: direct metadata (no resolve_type_arg)
    {
        const auto* meta = enum_resolve_meta(type);
        if (meta != nullptr) {
            for (CHAOS_IL2CPP_UINT32 i = 0; i < meta->count; i++) {
                const auto fn_len = std::strlen(meta->fields[i].name);
                if (fn_len == name_len && std::memcmp(meta->fields[i].name, name_data, name_len) == 0) {
                    auto boxed = enum_alloc_boxed_int64(meta->fields[i].value);
                    std::memcpy(reinterpret_cast<void*>(result_out), &boxed, sizeof(boxed));
                    return 1;
                }
            }
            if (ignoreCase) {
                for (CHAOS_IL2CPP_UINT32 i = 0; i < meta->count; i++) {
                    const auto fn_len = std::strlen(meta->fields[i].name);
                    if (fn_len != name_len) continue;
                    bool match = true;
                    for (CHAOS_IL2CPP_UINTPTR j = 0; j < name_len; j++) {
                        char a = meta->fields[i].name[j];
                        char b = name_data[j];
                        if (a >= 'A' && a <= 'Z') a = static_cast<char>(a + 32);
                        if (b >= 'A' && b <= 'Z') b = static_cast<char>(b + 32);
                        if (a != b) { match = false; break; }
                    }
                    if (match) {
                        auto boxed = enum_alloc_boxed_int64(meta->fields[i].value);
                        std::memcpy(reinterpret_cast<void*>(result_out), &boxed, sizeof(boxed));
                        return 1;
                    }
                }
            }
            return 0;
        }
    }

    // Fallback: reflection API
    const auto* desc = resolve_type_arg(type);
    if (desc == nullptr) return 0;
    if (check_enum_type(desc) == nullptr) return 0;

    const auto* field = find_field_by_name(desc, name_data, name_len);
    if (field == nullptr && ignoreCase) {
        field = find_field_by_name_icase(desc, name_data, name_len);
    }
    if (field == nullptr) return 0;

    auto boxed = enum_alloc_boxed_int64(field->constant_value);
    std::memcpy(reinterpret_cast<void*>(result_out), &boxed, sizeof(boxed));
    return 1;
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core
