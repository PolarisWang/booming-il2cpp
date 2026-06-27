// ABI exports: extern "C" linkage for managed/NativeAOT callability.

// enum_stubs.cpp — Enum helper stub implementations
#include <chaos/native_types.h>
#include <chaos/type_info.h>
#include <chaos/log.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <mutex>

#include "generated_code_compat.h"
#include "string_table.h"
#include "runtime_stubs/stub_common.h"
#include "gc_helpers.h"
#include "gc_layout.h"
#include "gc_events.h"
#include "gc_static_roots.h"
#include "reflection_query_model.h"
#include "reflection_api.h"
#include <chaos/profile.h>
#include "reflection_metadata_impl.h"
#include "exception_helpers.h"

namespace chaos::il2cpp::runtime_core {
extern "C" {

// ── Internal helpers ────────────────────────────────────────────

/// chaos_managed_string header size: ThinLockableHeader(16) + length(4) +
/// padding(4) + utf8_data(8) + string_id(8) = 40 bytes on x64.
static constexpr CHAOS_IL2CPP_SIZE kManagedStringHeader = 40;

// ── Stub-level TypeInfo for GC visibility ──────────────────────────────
// Objects allocated by stubs (strings, boxed values, arrays) must have a
// valid TypeInfoHot* at offset [0] so that the GC's IsValidManagedObject()
// recognizes them during marking.  Without this, the GC treats them as
// non-object memory and may sweep them while the mutator still holds
// references, causing access violations on the next use.
//
// The TypeInfo is registered lazily on first allocation and uses a raw-alloc
// (pointer-free) layout so the GC does not attempt to trace references.
static TypeInfoHot g_stub_string_typeinfo{};
static std::once_flag g_stub_string_typeinfo_flag;
static void init_stub_string_typeinfo() noexcept {
    auto& registry = chaos::il2cpp::runtime_core::GcLayoutRegistry::Instance();
    uint64_t stable_id = registry.RegisterOrGetRawAllocType(
        static_cast<uint32_t>(kManagedStringHeader));
    g_stub_string_typeinfo.parent = nullptr;
    g_stub_string_typeinfo.vtable_array = nullptr;
    g_stub_string_typeinfo.stable_id = stable_id;
    g_stub_string_typeinfo.vtable_length = 0;
    g_stub_string_typeinfo.warm_delta = 0;
    g_stub_string_typeinfo.type_shape = 0;
    g_stub_string_typeinfo.flags = 0;
    registry.RegisterTypeInfoRange(
        reinterpret_cast<uintptr_t>(&g_stub_string_typeinfo),
        reinterpret_cast<uintptr_t>(&g_stub_string_typeinfo) + sizeof(g_stub_string_typeinfo));
}

/// Allocate a managed string with the given byte_count of UTF-8 payload + NUL.
/// Uses chaos_managed_string layout so generated code reads length/utf8_data
/// at correct offsets via the CHAOS_IL2CPP_STRING_TYPE* path.
static CHAOS_IL2CPP_INTPTR enum_alloc_string(CHAOS_IL2CPP_UINTPTR byte_count) noexcept
{
    auto* storage = static_cast<unsigned char*>(
        GcAllocateAtomic(kManagedStringHeader + byte_count + 1));
    if (storage == nullptr) return 0;

    std::memset(storage, 0, kManagedStringHeader);

    // Set TypeInfo at offset 0 for GC visibility.  Without this, the GC
    // cannot recognize the string as a valid managed object during marking,
    // causing it to be swept while the mutator still holds a reference.
    std::call_once(g_stub_string_typeinfo_flag, init_stub_string_typeinfo);
    *reinterpret_cast<TypeInfoHot**>(storage) = &g_stub_string_typeinfo;

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

/// Allocate a managed string in the Pinned Object Heap (POH).
/// POH objects never participate in young GC copying, so thread_local
/// cache pointers to POH strings remain valid across collections.
/// Same chaos_managed_string layout as enum_alloc_string().
static CHAOS_IL2CPP_INTPTR enum_alloc_string_poh(CHAOS_IL2CPP_UINTPTR byte_count) noexcept
{
    auto* storage = static_cast<unsigned char*>(
        GcAllocatePinned(kManagedStringHeader + byte_count + 1));
    if (storage == nullptr) return 0;

    std::memset(storage, 0, kManagedStringHeader);

    std::call_once(g_stub_string_typeinfo_flag, init_stub_string_typeinfo);
    *reinterpret_cast<TypeInfoHot**>(storage) = &g_stub_string_typeinfo;

    auto* len_field = reinterpret_cast<CHAOS_IL2CPP_INT32*>(storage + 16);
    *len_field = static_cast<CHAOS_IL2CPP_INT32>(byte_count);

    char* data_area = reinterpret_cast<char*>(storage + kManagedStringHeader);
    if (byte_count > 0) data_area[0] = '\0';
    auto* utf8_field = reinterpret_cast<const char**>(storage + 24);
    *utf8_field = data_area;

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

// ── Public API for codegen-inline enum ToString ──
// Called from native-aot.generated.cpp inline switch cases to lazily
// allocate enum field name strings on first access.
extern "C" CHAOS_IL2CPP_INTPTR ChaosEnumAllocString(
    const char* data, CHAOS_IL2CPP_INT32 len) noexcept
{
    auto str_h = enum_alloc_string_poh(static_cast<CHAOS_IL2CPP_UINTPTR>(len));
    if (str_h == 0) return 0;
    write_string_data(str_h, data, static_cast<CHAOS_IL2CPP_UINTPTR>(len));
    return str_h;
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

// ── TLS box cache for enum Parse/TryParse ───────────────────────────
// Small TLS hash table that caches boxed enum values keyed by
// (type_handle, value). Repeated calls to Enum.Parse("Monday") return
// the cached box with zero allocation. Cleared on GC young start to
// prevent dangling managed pointers.
struct EnumBoxCacheEntry {
    CHAOS_IL2CPP_INTPTR type_key;  // 0 = empty slot
    CHAOS_IL2CPP_INT64 value;
    CHAOS_IL2CPP_INTPTR boxed_ptr;
};
static constexpr int kEnumBoxCacheSize = 64;
static thread_local EnumBoxCacheEntry tls_enum_box_cache[kEnumBoxCacheSize] = {};
// ── TLS box cache helpers ──────────────────────────────────────────
// Open-addressing hash table with XOR hash of (type_key, value).
// type_key == 0 means empty slot. Max 4 probes for lookup.
static CHAOS_IL2CPP_INTPTR enum_lookup_box_cache(
    CHAOS_IL2CPP_INTPTR type_key, CHAOS_IL2CPP_INT64 value) noexcept
{
    auto h = static_cast<CHAOS_IL2CPP_UINTPTR>(type_key) ^
             static_cast<CHAOS_IL2CPP_UINTPTR>(value);
    h ^= h >> 32;
    h &= (kEnumBoxCacheSize - 1);

    for (int i = 0; i < 4; i++) {
        auto& entry = tls_enum_box_cache[h];
        if (entry.type_key == type_key && entry.value == value)
            return entry.boxed_ptr;
        if (entry.type_key == 0)
            return 0;
        h = (h + 1) & (kEnumBoxCacheSize - 1);
    }
    return 0;
}

static void enum_insert_box_cache(
    CHAOS_IL2CPP_INTPTR type_key, CHAOS_IL2CPP_INT64 value,
    CHAOS_IL2CPP_INTPTR boxed_ptr) noexcept
{
    auto h = static_cast<CHAOS_IL2CPP_UINTPTR>(type_key) ^
             static_cast<CHAOS_IL2CPP_UINTPTR>(value);
    h ^= h >> 32;
    h &= (kEnumBoxCacheSize - 1);

    for (int i = 0; i < kEnumBoxCacheSize; i++) {
        auto& entry = tls_enum_box_cache[h];
        if (entry.type_key == type_key && entry.value == value)
            return;
        if (entry.type_key == 0) {
            entry.type_key = type_key;
            entry.value = value;
            entry.boxed_ptr = boxed_ptr;
            return;
        }
        h = (h + 1) & (kEnumBoxCacheSize - 1);
    }
}

/// Allocate a boxed int64 object, optionally cached by (type_key, value).
/// When type_key != 0 and the (type_key, value) pair is in the TLS box cache,
/// returns the cached pointer with zero allocation.
static CHAOS_IL2CPP_INTPTR enum_alloc_boxed_int64(
    CHAOS_IL2CPP_INT64 value, CHAOS_IL2CPP_INTPTR type_key = 0) noexcept
{
    CHAOS_IL2CPP_PROFILE_SCOPE("enum_alloc_boxed_int64");
    if (type_key != 0) {
        auto cached = enum_lookup_box_cache(type_key, value);
        if (cached != 0) return cached;
    }

    auto* storage = static_cast<unsigned char*>(GcAllocateAtomic(24));
    if (storage == nullptr) return 0;
    std::memset(storage, 0, 16); // header
    std::memcpy(storage + 16, &value, sizeof(value));

    auto result = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(storage);

    if (type_key != 0)
        enum_insert_box_cache(type_key, value, result);

    return result;
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

// ── O(1) hash table for cached enum name lookup ─────────────────
// Open-addressing direct-mapped hash table.  Capacity = 128 (load factor
// ~50% for max 64 entries), power of 2 for cheap modulo via mask.
// Populated in sync with s_enum_str_values/names in ensure_enum_str_cache.
// Cleared on type switch via memset in ensure_enum_str_cache.
static constexpr CHAOS_IL2CPP_UINT32 kEnumNameHashSize = 128;
struct EnumNameHashEntry {
    CHAOS_IL2CPP_INT64 value;   // 0 = empty slot
    CHAOS_IL2CPP_INTPTR name;   // 0 = empty slot
};
static thread_local EnumNameHashEntry s_enum_name_hash_table[kEnumNameHashSize] = {};

/// Insert a (value, name) pair into the O(1) name hash table.
/// Called from ensure_enum_str_cache for each cached field.
static void enum_name_hash_insert(CHAOS_IL2CPP_INT64 value, CHAOS_IL2CPP_INTPTR name) noexcept {
    if (name == 0) return;
    auto h = static_cast<CHAOS_IL2CPP_UINTPTR>(value) ^
             static_cast<CHAOS_IL2CPP_UINTPTR>(value >> 32);
    h &= (kEnumNameHashSize - 1u);
    for (CHAOS_IL2CPP_UINT32 probe = 0; probe < kEnumNameHashSize; probe++) {
        auto& entry = s_enum_name_hash_table[h];
        if (entry.value == value || (entry.value == 0 && entry.name == 0)) {
            entry.value = value;
            entry.name = name;
            return;
        }
        h = (h + 1u) & (kEnumNameHashSize - 1u);
    }
}

// Single-entry direct-mapped value→string cache for ToString/Format hot path.
// Keyed by (s_enum_str_type_key, value) — two integer compares and we return
// the cached pointer without calling ensure_enum_str_cache or doing a linear scan.
// Cleared on type switch (s_enum_str_type_key change) or after GC young collection.
static thread_local CHAOS_IL2CPP_INT64 s_enum_tostring_cache_value = -1;
static thread_local CHAOS_IL2CPP_INTPTR s_enum_tostring_cache_name = 0;

// Single-entry cache for non-enum fallback formatting (is_g/is_d/is_x paths).
// Covers types like byte where no enum fields exist — format is pure decimal/hex.
static thread_local CHAOS_IL2CPP_INT64 s_enum_fallback_cache_val = -1;
static thread_local bool s_enum_fallback_cache_is_x = false;
static thread_local CHAOS_IL2CPP_INTPTR s_enum_fallback_cache_result = 0;

// ── Process-level enum field name string cache ────────────────────
// All threads share this cache so each type's field name strings are
// allocated only once process-wide (in POH, so they never move).
// The cache is registered as an explicit GC root range so the GC does
// not collect the cached POH string objects during mark/sweep.
//
// Direct-mapped: slot = (effective_key >> 3) & (kProcessEnumCacheSize - 1).
// On collision the old entry is evicted — the next access re-allocates.
// 16 entries covers the common concurrent-type count in benchmarks.
static constexpr CHAOS_IL2CPP_UINT32 kProcessEnumCacheSize = 16;
struct ProcessEnumCacheEntry {
    CHAOS_IL2CPP_UINTPTR type_key;   // 0 = empty slot
    CHAOS_IL2CPP_UINT32 count;
    CHAOS_IL2CPP_INT64 values[64];
    CHAOS_IL2CPP_INTPTR names[64];
};
static ProcessEnumCacheEntry g_enum_process_cache[kProcessEnumCacheSize] = {};
static std::once_flag g_enum_process_cache_root_flag;

/// Register g_enum_process_cache as a GC root range so the collector
/// traces the cached POH string pointers during marking.  Without this,
/// the strings could be swept as unreachable even though the cache holds
/// the only live references.
static void register_process_cache_gc_root() noexcept {
    GcRegisterStaticRootRange(
        g_enum_process_cache,
        sizeof(g_enum_process_cache),
        0u);  // domain_id=0: process-lifetime, never unloaded
}

/// Look up the process-level cache for @a effective_key.
/// On hit, populates the thread_local cache (including the O(1) hash table)
/// and returns true.
static bool lookup_process_enum_cache(CHAOS_IL2CPP_UINTPTR effective_key) noexcept {
    if (effective_key == 0) return false;
    auto slot = (effective_key >> 3) & (kProcessEnumCacheSize - 1u);
    auto& entry = g_enum_process_cache[slot];
    if (entry.type_key == effective_key) {
        auto cnt = entry.count;
        s_enum_str_type_key = effective_key;
        s_enum_str_count = cnt;
        std::memset(s_enum_name_hash_table, 0, sizeof(s_enum_name_hash_table));
        for (CHAOS_IL2CPP_UINT32 i = 0; i < cnt; i++) {
            s_enum_str_values[i] = entry.values[i];
            s_enum_str_names[i] = entry.names[i];
            enum_name_hash_insert(entry.values[i], entry.names[i]);
        }
        return true;
    }
    return false;
}

/// Store the current thread_local cache contents into the process-level
/// cache for @a effective_key.  type_key is written last so concurrent
/// readers see a consistent snapshot (x64 store-store ordering).
static void store_process_enum_cache(CHAOS_IL2CPP_UINTPTR effective_key) noexcept {
    if (effective_key == 0) return;
    std::call_once(g_enum_process_cache_root_flag, register_process_cache_gc_root);
    auto slot = (effective_key >> 3) & (kProcessEnumCacheSize - 1u);
    auto& entry = g_enum_process_cache[slot];
    entry.count = s_enum_str_count;
    for (CHAOS_IL2CPP_UINT32 i = 0; i < s_enum_str_count; i++) {
        entry.values[i] = s_enum_str_values[i];
        entry.names[i] = s_enum_str_names[i];
    }
    // type_key published last: readers see consistent data or miss
    entry.type_key = effective_key;
}

// GetNames result array cache: single-entry, keyed by TypeInfoHandle.
// Avoids N string allocations + 1 array allocation per call in hot loops.
//
// thread_local is NOT a GC root, so the cached managed pointers become
// dangling after a young collection promotes nursery objects to old gen.
// To handle this safely, a GC_YOUNG_START callback clears both cache keys
// before each collection, forcing a cache rebuild from metadata on the
// next call.  This ensures the managed array and strings are always
// freshly allocated after any GC event.
static thread_local CHAOS_IL2CPP_UINTPTR s_enum_names_array_key = 0;
static thread_local CHAOS_IL2CPP_INTPTR s_enum_names_array = 0;

// GetValues result array cache: single-entry, keyed by TypeInfoHandle.
static thread_local CHAOS_IL2CPP_UINTPTR s_enum_values_array_key = 0;
static thread_local CHAOS_IL2CPP_INTPTR s_enum_values_array = 0;

// ── GC cache invalidation ──────────────────────────────────────────
// Registered lazily on first call to ChaosEnumGetNames.  Clears
// thread_local cache keys before each young collection so that the
// next call re-allocates managed objects from non-moving metadata.
//
// Note: s_enum_str_type_key is NOT cleared here because the cached
// enum field name strings are allocated in POH (via enum_alloc_string_poh)
// and never move during GC, so thread_local pointers remain valid.
static void enum_stubs_on_gc_event(GcEvent event, void* /*user_data*/) noexcept {
    if (event == GcEvent::GC_YOUNG_START) {
        s_enum_names_array_key = 0;
        s_enum_values_array_key = 0;
        s_enum_tostring_cache_value = -1;
        std::memset(tls_enum_box_cache, 0, sizeof(tls_enum_box_cache));
    }
}

static std::once_flag s_enum_gc_callback_flag;

// Forward declaration: used by enum_resolve_meta and ensure_enum_str_cache.
// Extracts a stable TypeInfoHandle from a managed Type object or tagged handle.
static CHAOS_IL2CPP_UINTPTR enum_extract_type_handle(CHAOS_IL2CPP_INTPTR type_arg) noexcept;

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

// Dispatch table: sorted FNV-24 → metadata array registered by generated code.
// Preferred over g_chaos_resolve_enum_metadata_by_fnv24 because the binary search
// avoids a large switch-case in the generated header. Set by
// ChaosEnumRegisterDispatchTable() called from the generated static initializer.
// Also declared in generated_code_compat.h for visibility from generated code.
extern "C" const EnumMetadataTable* (*g_chaos_enum_dispatch_lookup)(CHAOS_IL2CPP_UINT32 fnv24) noexcept = nullptr;

// Per-enum ToString dispatch lookup function pointer.
// Set by ChaosEnumRegisterToStringDispatchTable() called from the generated
// static initializer.  Default nullptr → stubs use POH cache + linear scan.
// Also declared in generated_code_compat.h for visibility from generated code.
extern "C" CHAOS_IL2CPP_INTPTR (*g_chaos_enum_tostring_dispatch_lookup)(
    CHAOS_IL2CPP_UINT32 fnv24, CHAOS_IL2CPP_INT64 value) noexcept = nullptr;

// External type descriptor lookup by stable_id (defined in type_resolve.cpp).
// Scans the dynamic external type registry for a type whose subject_id
// hashes to the given stable_id.  Used by enum_resolve_meta to resolve
// TypeInfoHot* raw pointers from AOT codegen.
extern "C" const ReflectionQueryTypeDescriptor* ChaosFindExternalTypeDescByStableId(
    CHAOS_IL2CPP_UINT64 stable_id) noexcept;

/// Static dispatch table state for binary-search enum metadata lookup.
/// Set by ChaosEnumRegisterDispatchTable and used by EnumDispatchLookup.
static const EnumDispatchEntry* s_dispatch_entries = nullptr;
static CHAOS_IL2CPP_UINT32 s_dispatch_count = 0;

// Open-addressing hash table for O(1) dispatch lookup.
// Capacity is next power of 2 > 284 * 1.5x load factor.
static constexpr CHAOS_IL2CPP_UINT32 kDispatchHashCapacity = 512;
static EnumDispatchEntry s_dispatch_hash_table[kDispatchHashCapacity] = {};

/// Rebuild the hash table from s_dispatch_entries.
/// Must be called after updating s_dispatch_entries/s_dispatch_count.
static void rebuild_dispatch_hash_table() noexcept {
    std::memset(s_dispatch_hash_table, 0, sizeof(s_dispatch_hash_table));
    for (CHAOS_IL2CPP_UINT32 i = 0; i < s_dispatch_count; i++) {
        auto fnv24 = s_dispatch_entries[i].fnv24;
        auto slot = fnv24 & (kDispatchHashCapacity - 1u);
        while (s_dispatch_hash_table[slot].fnv24 != 0) {
            slot = (slot + 1u) & (kDispatchHashCapacity - 1u);
        }
        s_dispatch_hash_table[slot] = s_dispatch_entries[i];
    }
}

/// Hash-table lookup against the registered dispatch table.
/// Falls back to binary search on hash collision or empty entry.
static const EnumMetadataTable* EnumDispatchLookup(CHAOS_IL2CPP_UINT32 fnv24) noexcept {
    if (s_dispatch_entries == nullptr) return nullptr;

    // Fast path: open-addressing hash table (O(1) average, <2 probes)
    auto slot = fnv24 & (kDispatchHashCapacity - 1u);
    for (CHAOS_IL2CPP_UINT32 probe = 0; probe < kDispatchHashCapacity; probe++) {
        auto hs = s_dispatch_hash_table[slot].fnv24;
        if (hs == fnv24)
            return s_dispatch_hash_table[slot].table;
        if (hs == 0)
            return nullptr;  // empty slot → not in table
        slot = (slot + 1u) & (kDispatchHashCapacity - 1u);
    }

    // Fallback: binary search (should rarely reach here due to load factor)
    CHAOS_IL2CPP_UINT32 lo = 0u, hi = s_dispatch_count;
    while (lo < hi) {
        CHAOS_IL2CPP_UINT32 mid = lo + (hi - lo) / 2u;
        if (s_dispatch_entries[mid].fnv24 < fnv24)
            lo = mid + 1u;
        else if (s_dispatch_entries[mid].fnv24 > fnv24)
            hi = mid;
        else
            return s_dispatch_entries[mid].table;
    }
    return nullptr;
}

/// Register a sorted FNV-24 dispatch table for enum metadata lookup.
/// Called from the generated static initializer in enum_metadata.generated.h.
/// The entries array must be sorted by fnv24 for binary search (used as fallback).
/// Builds an open-addressing hash table for O(1) fast-path lookups.
extern "C" void ChaosEnumRegisterDispatchTable(
    const EnumDispatchEntry* entries, CHAOS_IL2CPP_UINT32 count) noexcept
{
    s_dispatch_entries = entries;
    s_dispatch_count = count;
    rebuild_dispatch_hash_table();
    g_chaos_enum_dispatch_lookup = EnumDispatchLookup;
}

/// Pre-allocate managed strings (in POH) from a compiled-in field entry array.
/// Called once per enum type at static init time from ChaosRegisterEnumGeneratedMetadata.
///
/// After this function returns, out_strings[i] points to a POH-allocated managed
/// string object containing the UTF-8 name of field i.  The strings never move
/// (POH allocation) so thread_local cache pointers remain valid across collections.
///
/// This eliminates the lazy allocation in ensure_enum_str_cache on the first
/// call to Enum.ToString/Format/GetNames for each type.
extern "C" void ChaosEnumPreInitStringCache(
    const EnumFieldEntry* entries, CHAOS_IL2CPP_UINT32 count,
    CHAOS_IL2CPP_INTPTR* out_strings) noexcept
{
    if (entries == nullptr || out_strings == nullptr || count == 0) return;

    CHAOS_IL2CPP_UINT32 n = count > 64 ? 64 : count;
    for (CHAOS_IL2CPP_UINT32 i = 0; i < n; i++)
    {
        size_t nlen = std::strlen(entries[i].name);
        auto str_h = enum_alloc_string_poh(static_cast<CHAOS_IL2CPP_UINTPTR>(nlen));
        if (str_h == 0) continue;
        write_string_data(str_h, entries[i].name, static_cast<CHAOS_IL2CPP_UINTPTR>(nlen));
        out_strings[i] = str_h;
    }
}

/// Hotpatch: replace the dispatch table entries at runtime.
/// New entries are added, old entries remain for existing types.
/// The lambda closure is updated atomically (pointer write on x64/ARM64).
extern "C" void ChaosEnumUpdateDispatchTable(
    const EnumDispatchEntry* entries, CHAOS_IL2CPP_UINT32 count) noexcept
{
    // Rebuild the hash table and atomically replace the lookup closure.
    s_dispatch_entries = entries;
    s_dispatch_count = count;
    rebuild_dispatch_hash_table();
    g_chaos_enum_dispatch_lookup = EnumDispatchLookup;
}

// ── Per-enum ToString dispatch table state ──────────────────────────
// Sorted-by-fnv24 array of (fnv24, to_string_fn) pairs registered by
// the generated code.  Unlike the metadata dispatch table, this one
// uses pure binary search (no hash table) because the ToString dispatch
// is only called sparingly per unique value (the direct-mapped cache
// in Phase 1 absorbs repeated calls to the same value).
static const EnumToStringDispatchEntry* s_tostring_dispatch_entries = nullptr;
static CHAOS_IL2CPP_UINT32 s_tostring_dispatch_count = 0;

/// Binary-search lookup against the registered ToString dispatch table.
/// Returns the result of the per-enum function if found, 0 otherwise.
static CHAOS_IL2CPP_INTPTR EnumToStringDispatchLookup(
    CHAOS_IL2CPP_UINT32 fnv24, CHAOS_IL2CPP_INT64 value) noexcept
{
    if (s_tostring_dispatch_entries == nullptr) return 0;
    CHAOS_IL2CPP_UINT32 lo = 0u, hi = s_tostring_dispatch_count;
    while (lo < hi) {
        CHAOS_IL2CPP_UINT32 mid = lo + (hi - lo) / 2u;
        auto& entry = s_tostring_dispatch_entries[mid];
        if (entry.fnv24 < fnv24) lo = mid + 1u;
        else if (entry.fnv24 > fnv24) hi = mid;
        else return entry.to_string_fn ? entry.to_string_fn(value) : 0;
    }
    return 0;
}

/// Register the per-enum ToString dispatch table from generated code.
/// Called once from the static initializer in the generated translation unit.
extern "C" void ChaosEnumRegisterToStringDispatchTable(
    const EnumToStringDispatchEntry* entries, CHAOS_IL2CPP_UINT32 count) noexcept
{
    s_tostring_dispatch_entries = entries;
    s_tostring_dispatch_count = count;
    g_chaos_enum_tostring_dispatch_lookup = EnumToStringDispatchLookup;
}

// ── Pre-allocated string dispatch table ──────────────────────────
// Map from fnv24 to pre-allocated managed string pointer arrays.
// Registered at static init time by ChaosRegisterEnumGeneratedMetadata.
// When non-null for a given fnv24, ensure_enum_str_cache uses these
// pre-allocated POH strings instead of lazy allocation — zero GC
// allocation on the enum.ToString/Format/GetNames hot path.
// EnumPreInitEntry struct defined in generated_code_compat.h (with guard).

static const EnumPreInitEntry* s_preinit_entries = nullptr;
static CHAOS_IL2CPP_UINT32 s_preinit_count = 0;

/// Binary-search lookup against the registered pre-init table.
/// Returns nullptr if fnv24 not found.
static CHAOS_IL2CPP_INTPTR* EnumPreInitLookup(
    CHAOS_IL2CPP_UINT32 fnv24) noexcept
{
    if (s_preinit_entries == nullptr) return nullptr;
    CHAOS_IL2CPP_UINT32 lo = 0u, hi = s_preinit_count;
    while (lo < hi) {
        CHAOS_IL2CPP_UINT32 mid = lo + (hi - lo) / 2u;
        auto& entry = s_preinit_entries[mid];
        if (entry.fnv24 < fnv24) lo = mid + 1u;
        else if (entry.fnv24 > fnv24) hi = mid;
        else return entry.strings;
    }
    return nullptr;
}

extern "C" CHAOS_IL2CPP_INTPTR* (*g_chaos_enum_preinit_lookup)(
    CHAOS_IL2CPP_UINT32 fnv24) noexcept = nullptr;

/// Register the sorted pre-allocated string dispatch table from generated code.
/// Each entry maps an fnv24 to a pre-allocated array of managed string pointers
/// in POH. Called once per closure at static init time.
extern "C" void ChaosEnumRegisterPreInitTable(
    const EnumPreInitEntry* entries, CHAOS_IL2CPP_UINT32 count) noexcept
{
    s_preinit_entries = entries;
    s_preinit_count = count;
    g_chaos_enum_preinit_lookup = EnumPreInitLookup;
}

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
    // Use stable TypeInfoHandle as cache key to handle GC-moved Type objects
    CHAOS_IL2CPP_UINTPTR handle = enum_extract_type_handle(type_key);
    CHAOS_IL2CPP_UINTPTR effective_key = handle != 0 ? handle : static_cast<CHAOS_IL2CPP_UINTPTR>(type_key);
    if (s_enum_str_type_key == effective_key) return;

    // Check process-level cache before allocating new strings.
    // Other threads may have already cached this type.
    if (lookup_process_enum_cache(effective_key)) return;

    s_enum_str_type_key = effective_key;
    s_enum_str_count = 0;
    s_enum_tostring_cache_value = -1;
    s_enum_tostring_cache_name = 0;
    std::memset(s_enum_name_hash_table, 0, sizeof(s_enum_name_hash_table));

    // Primary: pre-resolved metadata pointer (fastest path)
    if (meta != nullptr && meta->count > 0) {
        CHAOS_IL2CPP_UINT32 cnt = meta->count > 64 ? 64 : meta->count;

        // Check for pre-allocated strings from codegen static init.
        // When the generated code has pre-allocated POH strings for this
        // type (via ChaosEnumPreInitStringCache at static init time), use
        // them directly — zero GC allocation on the hot path.
        // The fnv24 is recovered from the metadata pointer via binary search
        // on the dispatch table (O(log n), called once per type — negligible).
        if (g_chaos_enum_preinit_lookup && s_dispatch_entries != nullptr) {
            CHAOS_IL2CPP_UINT32 fnv24 = 0;
            CHAOS_IL2CPP_UINT32 lo_ = 0u, hi_ = s_dispatch_count;
            while (lo_ < hi_) {
                CHAOS_IL2CPP_UINT32 mid = lo_ + (hi_ - lo_) / 2u;
                auto& entry = s_dispatch_entries[mid];
                if (entry.table < meta) lo_ = mid + 1u;
                else if (entry.table > meta) hi_ = mid;
                else { fnv24 = entry.fnv24; break; }
            }
            if (fnv24 != 0) {
                auto* preinit = g_chaos_enum_preinit_lookup(fnv24);
                if (preinit != nullptr) {
                    // All pre-allocated strings are non-null after static init.
                    for (CHAOS_IL2CPP_UINT32 i = 0; i < cnt; i++) {
                        s_enum_str_values[i] = meta->fields[i].value;
                        s_enum_str_names[i] = preinit[i];
                        enum_name_hash_insert(meta->fields[i].value, preinit[i]);
                    }
                    s_enum_str_count = cnt;
                    store_process_enum_cache(effective_key);
                    return;
                }
            }
        }

        // Fallback: lazy POH string allocation.
        for (CHAOS_IL2CPP_UINT32 i = 0; i < cnt; i++) {
            const EnumFieldEntry& fe = meta->fields[i];
            size_t nlen = std::strlen(fe.name);
            auto str_h = enum_alloc_string_poh(static_cast<CHAOS_IL2CPP_UINTPTR>(nlen));            if (str_h == 0) continue;
            write_string_data(str_h, fe.name, static_cast<CHAOS_IL2CPP_UINTPTR>(nlen));
            s_enum_str_values[i] = fe.value;
            s_enum_str_names[i] = str_h;
            enum_name_hash_insert(fe.value, str_h);
        }
        s_enum_str_count = cnt;
        store_process_enum_cache(effective_key);
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
                auto str_h = enum_alloc_string_poh(static_cast<CHAOS_IL2CPP_UINTPTR>(nlen));                if (str_h == 0) continue;
                write_string_data(str_h, fe.name, static_cast<CHAOS_IL2CPP_UINTPTR>(nlen));
                s_enum_str_values[i] = fe.value;
                s_enum_str_names[i] = str_h;
                enum_name_hash_insert(fe.value, str_h);
            }
            s_enum_str_count = cnt;
            store_process_enum_cache(effective_key);
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
        auto str_handle = enum_alloc_string_poh(name_len);
        if (str_handle == 0) continue;
        write_string_data(str_handle, f.name_utf8, name_len);

        s_enum_str_values[idx] = f.constant_value;
        s_enum_str_names[idx] = str_handle;
        enum_name_hash_insert(f.constant_value, str_handle);
        idx++;
    }
    s_enum_str_count = idx;
    store_process_enum_cache(effective_key);
}

/// Look up a cached enum name string by value using a direct-mapped hash table.
/// O(1) average case (single probe), O(n) worst case on collision.
/// Returns 0 if not cached.
/// Non-static: also used by the inline shape in RuntimeHelperShapeRegistry (generated code).
CHAOS_IL2CPP_INTPTR lookup_cached_enum_name(CHAOS_IL2CPP_INT64 value) noexcept
{
    // Direct-mapped: slot = (value ^ (value >> 32)) & (kEnumNameHashSize - 1)
    auto h = static_cast<CHAOS_IL2CPP_UINTPTR>(value) ^
             static_cast<CHAOS_IL2CPP_UINTPTR>(value >> 32);
    h &= (kEnumNameHashSize - 1u);

    for (CHAOS_IL2CPP_UINT32 probe = 0; probe < kEnumNameHashSize; probe++) {
        auto& entry = s_enum_name_hash_table[h];
        if (entry.value == value && entry.name != 0)
            return entry.name;
        if (entry.value == 0 && entry.name == 0)
            return 0;  // empty slot → not in table
        h = (h + 1u) & (kEnumNameHashSize - 1u);
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
// Single-entry thread_local cache: maps the stable TypeInfoHandle
// to the metadata table. This allows stubs to bypass resolve_type_arg
// entirely when metadata is available via FNV-24 dispatch.
// is available, saving the reflection API round-trip + FNV-1a hash.
//
// The cache is single-entry because the benchmark/common pattern is
// calling the same enum method repeatedly on the same type. When the
// type changes (rare), the cache is updated on miss.
static const ReflectionQueryTypeDescriptor* resolve_type_arg(CHAOS_IL2CPP_INTPTR type_arg) noexcept;
static thread_local CHAOS_IL2CPP_INTPTR s_enum_meta_type_key = 0;
static thread_local const EnumMetadataTable* s_enum_meta_cache = nullptr;

// ── Negative cache: types confirmed NOT to be enum types ─────────────
// When the full resolution chain determines a type is resolvable but
// is NOT an enum (e.g. byte, int), the type handle is stored here so
// subsequent calls to enum_resolve_meta return nullptr immediately
// without re-entering the expensive resolution chain.
// 16 direct-mapped entries, slot = (handle >> 3) & 0xF.
// Thread_local: each thread independently discovers non-enum types.
static constexpr CHAOS_IL2CPP_UINT32 kNonEnumCacheSize = 16;
static thread_local CHAOS_IL2CPP_UINTPTR s_non_enum_cache[kNonEnumCacheSize] = {};

/// Check if type_handle is in the non-enum negative cache.
static bool is_cached_non_enum(CHAOS_IL2CPP_UINTPTR type_handle) noexcept {
    if (type_handle == 0) return false;
    auto slot = (type_handle >> 3) & (kNonEnumCacheSize - 1u);
    return s_non_enum_cache[slot] == type_handle;
}

/// Store type_handle in the non-enum negative cache.
static void cache_non_enum(CHAOS_IL2CPP_UINTPTR type_handle) noexcept {
    if (type_handle == 0) return;
    auto slot = (type_handle >> 3) & (kNonEnumCacheSize - 1u);
    s_non_enum_cache[slot] = type_handle;
}

// ── TypeInfoHot* → EnumMetadataTable* reverse cache ─────────────
// When enum_resolve_meta receives a raw heap pointer (TypeInfoHot* or
// managed Type object), the existing fast paths don't recognize it:
// - 0x02XXXXXX tag check fails (not a codegen pseudo-handle)
// - bit[63]=1 check fails (not a tagged reflection handle)
// - enum_extract_type_handle reads offset+8 as raw_handle (wrong field)
//
// This direct-mapped cache maps the raw pointer directly to metadata,
// avoiding FNV-1a hash computation and resolve_type_arg fallback.
// 32 entries, slot = (ptr >> 4) & 0x1F.
static constexpr CHAOS_IL2CPP_UINT32 kTypeInfoReverseCacheSize = 32;
static struct {
    CHAOS_IL2CPP_INTPTR key;         // raw type_arg pointer, 0 = empty
    const EnumMetadataTable* meta;
} g_type_info_reverse_cache[kTypeInfoReverseCacheSize] = {};

/// Returns true if type_arg is a raw heap pointer (not a tagged handle or
/// codegen pseudo-handle).  Only such pointers are cached in the reverse cache.
static bool is_raw_heap_pointer(CHAOS_IL2CPP_INTPTR ptr) noexcept {
    if (ptr == 0) return false;
    // Codegen pseudo-handle: 0x02XXXXXX in low 32 bits
    uint32_t low32 = static_cast<uint32_t>(static_cast<CHAOS_IL2CPP_UINTPTR>(ptr) & 0xFFFFFFFFu);
    if ((low32 & 0xFF000000u) == 0x02000000u) return false;
    // Tagged reflection handle: bit[63]=1
    if ((static_cast<CHAOS_IL2CPP_UINTPTR>(ptr) >> 63) != 0u) return false;
    return true;
}

static void store_type_info_reverse_cache(CHAOS_IL2CPP_INTPTR type_arg,
                                           const EnumMetadataTable* meta) noexcept {
    if (meta == nullptr || !is_raw_heap_pointer(type_arg)) return;
    auto rev_slot = (static_cast<CHAOS_IL2CPP_UINTPTR>(type_arg) >> 4) & (kTypeInfoReverseCacheSize - 1u);
    g_type_info_reverse_cache[rev_slot].key = type_arg;
    g_type_info_reverse_cache[rev_slot].meta = meta;
}

/// Resolve type_arg to enum metadata table (cached).
/// Returns nullptr if metadata is unavailable for this type.
/// When non-null, the caller can skip resolve_type_arg entirely.
///
/// Fast path: when type_arg has the codegen pseudo-handle format (0x02XXXXXX),
/// extract the FNV-1a 24-bit hash and look up metadata directly via
/// g_chaos_resolve_enum_metadata_by_fnv24 — no resolve_type_arg call needed.
static const EnumMetadataTable* enum_resolve_meta(CHAOS_IL2CPP_INTPTR type_arg) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("enum_resolve_meta");
    // Use stable TypeInfoHandle as cache key to handle GC-moved Type objects
    CHAOS_IL2CPP_UINTPTR handle = enum_extract_type_handle(type_arg);
    if (handle != 0 && handle == s_enum_meta_type_key) { return s_enum_meta_cache; }

    // Negative cache: return nullptr immediately for types already confirmed
    // as non-enum (e.g. byte), skipping the expensive resolution chain entirely.
    if (is_cached_non_enum(handle)) return nullptr;

    // TypeInfoHot* reverse cache: raw heap pointer → metadata.
    // Catches TypeInfoHot* from boxed object headers and managed Type objects
    // that the fast paths above don't recognize.
    if (type_arg != 0) {
        auto rev_slot = (static_cast<CHAOS_IL2CPP_UINTPTR>(type_arg) >> 4) & (kTypeInfoReverseCacheSize - 1u);
        auto& rev = g_type_info_reverse_cache[rev_slot];
        if (rev.key == type_arg && rev.meta != nullptr) {
            s_enum_meta_type_key = handle != 0 ? handle : static_cast<CHAOS_IL2CPP_UINTPTR>(type_arg);
            s_enum_meta_cache = rev.meta;
            return rev.meta;
        }
    }

    // Fast path: direct fnv24 lookup from TypeInfoHandle (no resolve_type_arg)
    uint32_t val = static_cast<uint32_t>(handle & 0xFFFFFFFFu);
    if ((val & 0xFF000000u) == 0x02000000u && (val & 0xFFFFFFu) != 0u) {
        uint32_t fnv24 = val & 0xFFFFFFu;
        // Priority 1: dispatch table (binary search over sorted entries)
        const auto* meta = g_chaos_enum_dispatch_lookup
            ? g_chaos_enum_dispatch_lookup(fnv24)
            : nullptr;
        if (meta == nullptr) {
            // Priority 2: generated switch-case (legacy path)
            meta = g_chaos_resolve_enum_metadata_by_fnv24
                ? g_chaos_resolve_enum_metadata_by_fnv24(fnv24)
                : nullptr;
        }
        if (meta != nullptr) {
            s_enum_meta_type_key = handle;
            s_enum_meta_cache = meta;
            store_type_info_reverse_cache(type_arg, meta);
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
            // Priority 1: dispatch table (FNV-24 binary search)
            const auto* meta = g_chaos_enum_dispatch_lookup
                ? g_chaos_enum_dispatch_lookup(fnv24)
                : nullptr;
            if (meta == nullptr) {
                // Priority 2: generated switch-case (legacy path)
                meta = g_chaos_resolve_enum_metadata_by_fnv24
                    ? g_chaos_resolve_enum_metadata_by_fnv24(fnv24)
                    : nullptr;
            }
            if (meta != nullptr) {
                s_enum_meta_type_key = handle != 0 ? handle : static_cast<CHAOS_IL2CPP_UINTPTR>(type_arg);
                s_enum_meta_cache = meta;
                store_type_info_reverse_cache(type_arg, meta);
                return meta;
            }
        }
    }

    // TypeInfoHot* raw pointer path: read stable_id at offset 16, then scan
    // the external type registry (populated by codegen's `ChaosRegisterEnumGeneratedMetadata`)
    // to find the matching type descriptor and its enum metadata table.
    // This handles the AOT codegen case where Enum.Parse receives a raw
    // TypeInfoHot* pointer (via AsTypeInfoHot<T>()) instead of a codegen
    // pseudo-handle or tagged TypeInfoHandle.
    if (type_arg != 0 && is_raw_heap_pointer(type_arg)) {
        CHAOS_IL2CPP_UINT64 stable_id = 0;
        std::memcpy(&stable_id,
            reinterpret_cast<const void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(type_arg) + 16),
            sizeof(stable_id));
        if (stable_id != 0) {
            const auto* tdesc = ChaosFindExternalTypeDescByStableId(stable_id);
            if (tdesc != nullptr && tdesc->subject_id_utf8 != nullptr) {
                // Look up enum metadata via fnv24 from subject_id
                uint32_t h = 2166136261u;
                for (const char* s = tdesc->subject_id_utf8; *s; ++s) {
                    h ^= static_cast<uint8_t>(*s);
                    h *= 16777619u;
                }
                uint32_t fnv24 = h & 0xFFFFFFu;
                const auto* meta = g_chaos_enum_dispatch_lookup
                    ? g_chaos_enum_dispatch_lookup(fnv24)
                    : nullptr;
                if (meta == nullptr && g_chaos_resolve_enum_metadata_by_fnv24) {
                    meta = g_chaos_resolve_enum_metadata_by_fnv24(fnv24);
                }
                if (meta != nullptr) {
                    s_enum_meta_type_key = static_cast<CHAOS_IL2CPP_UINTPTR>(type_arg);
                    s_enum_meta_cache = meta;
                    store_type_info_reverse_cache(type_arg, meta);
                    return meta;
                }
                // Also try g_chaos_resolve_enum_metadata by subject_id directly
                if (meta == nullptr && g_chaos_resolve_enum_metadata) {
                    meta = g_chaos_resolve_enum_metadata(tdesc->subject_id_utf8);
                    if (meta != nullptr) {
                        s_enum_meta_type_key = static_cast<CHAOS_IL2CPP_UINTPTR>(type_arg);
                        s_enum_meta_cache = meta;
                        store_type_info_reverse_cache(type_arg, meta);
                        return meta;
                    }
                }
            }
        }
    }

    // Fallback: resolve type_arg and look up by subject_id.
    const auto* desc = resolve_type_arg(type_arg);

    // If the type is resolvable but is NOT an enum, cache it in the
    // negative cache so subsequent calls skip the resolution chain.
    if (desc != nullptr && check_enum_type(desc) == nullptr) {
        cache_non_enum(handle);
        s_enum_meta_type_key = handle != 0 ? handle : static_cast<CHAOS_IL2CPP_UINTPTR>(type_arg);
        s_enum_meta_cache = nullptr;
        return nullptr;
    }

    const auto* meta = (desc != nullptr && desc->subject_id_utf8 != nullptr)
        ? (g_chaos_resolve_enum_metadata
            ? g_chaos_resolve_enum_metadata(desc->subject_id_utf8)
            : nullptr)
        : nullptr;

    s_enum_meta_type_key = handle != 0 ? handle : static_cast<CHAOS_IL2CPP_UINTPTR>(type_arg);
    s_enum_meta_cache = meta;
    store_type_info_reverse_cache(type_arg, meta);
    return meta;
}

/// Fast path: find enum field entry by value using pre-computed metadata.
/// Uses the dispatch table (FNV-24 binary search) for O(log n) lookup,
/// falling back to g_chaos_resolve_enum_metadata (FNV-32 switch) if unavailable.
/// Returns the field entry pointer, or nullptr if metadata unavailable / not found.
static const EnumFieldEntry* enum_find_entry_by_value(
    const ReflectionQueryTypeDescriptor* desc, CHAOS_IL2CPP_INT64 value) noexcept
{
    if (desc == nullptr || desc->subject_id_utf8 == nullptr) return nullptr;

    // Priority 1: dispatch table (FNV-24 binary search)
    if (g_chaos_enum_dispatch_lookup) {
        uint32_t h = 2166136261u;
        for (const char* s = desc->subject_id_utf8; *s; ++s) {
            h ^= static_cast<uint8_t>(*s);
            h *= 16777619u;
        }
        const auto* meta = g_chaos_enum_dispatch_lookup(h & 0xFFFFFFu);
        if (meta != nullptr) {
            for (CHAOS_IL2CPP_UINT32 i = 0; i < meta->count; i++) {
                if (meta->fields[i].value == value) return &meta->fields[i];
            }
            return nullptr;
        }
    }

    // Priority 2: FNV-32 switch (legacy path, requires strcmp verification)
    const auto* meta = g_chaos_resolve_enum_metadata
        ? g_chaos_resolve_enum_metadata(desc->subject_id_utf8)
        : nullptr;
    if (meta == nullptr) return nullptr;
    for (CHAOS_IL2CPP_UINT32 i = 0; i < meta->count; i++) {
        if (meta->fields[i].value == value) return &meta->fields[i];
    }
    return nullptr;
}

/// Pre-compute field name lengths from an EnumMetadataTable into a stack array.
/// Returns the number of lengths computed (min of count or 64).
/// Avoids per-iteration strlen() in hot field-scanning loops.
static CHAOS_IL2CPP_UINT32 precompute_name_lengths(
    const EnumMetadataTable* meta,
    CHAOS_IL2CPP_UINTPTR* out_lengths) noexcept
{
    CHAOS_IL2CPP_UINT32 cnt = meta->count > 64 ? 64 : meta->count;
    for (CHAOS_IL2CPP_UINT32 i = 0; i < cnt; i++) {
        out_lengths[i] = std::strlen(meta->fields[i].name);
    }
    return cnt;
}

// ── Numeric/comma-separated parsing helpers (generality) ──────────
// .NET's Enum.Parse supports:
//   - "42"       → numeric (decimal)
//   - "0xFF"     → hex (auto-detected by strtoll base=0)
//   - "A, B"     → comma-separated names/values OR'd together
//   - "1, A"     → mixed numeric + name segments
// These helpers add the missing generality on top of name-only scanning.

/// Try to parse a string segment as a numeric value (decimal or 0x/0X hex).
/// Returns true and sets out_value on success.
static bool enum_try_parse_numeric_segment(
    const char* data, CHAOS_IL2CPP_UINTPTR len,
    CHAOS_IL2CPP_INT64& out_value) noexcept
{
    if (data == nullptr || len == 0) return false;
    char* end = nullptr;
    long long val = std::strtoll(data, &end, 0);
    if (end != data + static_cast<ptrdiff_t>(len)) return false;
    if (val == 0 && end == data) return false;
    out_value = static_cast<CHAOS_IL2CPP_INT64>(val);
    return true;
}

/// Find a field value by name in a metadata table (case-sensitive, then insensitive).
/// Returns true and sets out_value on match.
static bool enum_metadata_lookup_name(
    const EnumMetadataTable* meta, const char* name,
    CHAOS_IL2CPP_UINTPTR name_len, CHAOS_IL2CPP_INT64& out_value) noexcept
{
    CHAOS_IL2CPP_UINTPTR fname_len[64];
    CHAOS_IL2CPP_UINT32 cnt = precompute_name_lengths(meta, fname_len);
    for (CHAOS_IL2CPP_UINT32 i = 0; i < cnt; i++) {
        if (fname_len[i] == name_len && std::memcmp(meta->fields[i].name, name, name_len) == 0) {
            out_value = meta->fields[i].value;
            return true;
        }
    }
    for (CHAOS_IL2CPP_UINT32 i = 0; i < cnt; i++) {
        if (fname_len[i] != name_len) continue;
        bool match = true;
        for (CHAOS_IL2CPP_UINTPTR j = 0; j < name_len; j++) {
            char a = meta->fields[i].name[j];
            char b = name[j];
            if (a >= 'A' && a <= 'Z') a = static_cast<char>(a + 32);
            if (b >= 'A' && b <= 'Z') b = static_cast<char>(b + 32);
            if (a != b) { match = false; break; }
        }
        if (match) { out_value = meta->fields[i].value; return true; }
    }
    return false;
}

/// Split input on ',' trim whitespace, parse each segment as name or
/// numeric, OR values together. Returns true if at least one segment parsed.
static bool enum_try_parse_comma_separated_meta(
    const EnumMetadataTable* meta, const char* data,
    CHAOS_IL2CPP_UINTPTR len, CHAOS_IL2CPP_INT64& out_value) noexcept
{
    CHAOS_IL2CPP_INT64 result = 0;
    bool any = false;
    CHAOS_IL2CPP_UINTPTR start = 0;
    for (CHAOS_IL2CPP_UINTPTR i = 0; i <= len; i++) {
        if (i == len || data[i] == ',') {
            CHAOS_IL2CPP_UINTPTR end = i;
            while (end > start && (data[end - 1] == ' ' || data[end - 1] == '\t')) end--;
            CHAOS_IL2CPP_UINTPTR seg_start = start;
            while (seg_start < end && (data[seg_start] == ' ' || data[seg_start] == '\t')) seg_start++;
            CHAOS_IL2CPP_UINTPTR seg_len = (seg_start < end) ? (end - seg_start) : 0;

            if (seg_len > 0) {
                CHAOS_IL2CPP_INT64 seg_val = 0;
                if (!enum_metadata_lookup_name(meta, data + seg_start, seg_len, seg_val)) {
                    if (!enum_try_parse_numeric_segment(data + seg_start, seg_len, seg_val)) {
                        return false;
                    }
                }
                result |= seg_val;
                any = true;
            }
            start = i + 1;
        }
    }
    if (!any) return false;
    out_value = result;
    return true;
}

/// Parse comma-separated using reflection descriptor (fallback path).
static bool enum_try_parse_comma_separated_reflection(
    const ReflectionQueryTypeDescriptor* desc, const char* data,
    CHAOS_IL2CPP_UINTPTR len, CHAOS_IL2CPP_INT64& out_value) noexcept
{
    CHAOS_IL2CPP_INT64 result = 0;
    bool any = false;
    CHAOS_IL2CPP_UINTPTR start = 0;
    for (CHAOS_IL2CPP_UINTPTR i = 0; i <= len; i++) {
        if (i == len || data[i] == ',') {
            CHAOS_IL2CPP_UINTPTR end = i;
            while (end > start && (data[end - 1] == ' ' || data[end - 1] == '\t')) end--;
            CHAOS_IL2CPP_UINTPTR seg_start = start;
            while (seg_start < end && (data[seg_start] == ' ' || data[seg_start] == '\t')) seg_start++;
            CHAOS_IL2CPP_UINTPTR seg_len = (seg_start < end) ? (end - seg_start) : 0;

            if (seg_len > 0) {
                CHAOS_IL2CPP_INT64 seg_val = 0;
                auto* field = find_field_by_name(desc, data + seg_start, seg_len);
                if (field == nullptr)
                    field = find_field_by_name_icase(desc, data + seg_start, seg_len);
                if (field != nullptr) {
                    seg_val = field->constant_value;
                } else if (!enum_try_parse_numeric_segment(data + seg_start, seg_len, seg_val)) {
                    return false;
                }
                result |= seg_val;
                any = true;
            }
            start = i + 1;
        }
    }
    if (!any) return false;
    out_value = result;
    return true;
}

/// Fast path: find enum field entry by name using pre-computed metadata.
static const EnumFieldEntry* enum_find_entry_by_name(
    const ReflectionQueryTypeDescriptor* desc,
    const char* name, CHAOS_IL2CPP_UINTPTR name_len) noexcept
{
    if (desc == nullptr || desc->subject_id_utf8 == nullptr) return nullptr;

    // Priority 1: dispatch table (FNV-24 hash → O(1) hash table)
    if (g_chaos_enum_dispatch_lookup) {
        uint32_t h = 2166136261u;
        for (const char* s = desc->subject_id_utf8; *s; ++s) {
            h ^= static_cast<uint8_t>(*s);
            h *= 16777619u;
        }
        const auto* meta = g_chaos_enum_dispatch_lookup(h & 0xFFFFFFu);
        if (meta != nullptr) {
            CHAOS_IL2CPP_UINTPTR fname_len[64];
            CHAOS_IL2CPP_UINT32 cnt = precompute_name_lengths(meta, fname_len);
            for (CHAOS_IL2CPP_UINT32 i = 0; i < cnt; i++) {
                if (fname_len[i] == name_len && std::memcmp(meta->fields[i].name, name, name_len) == 0) {
                    return &meta->fields[i];
                }
            }
            return nullptr;
        }
    }

    // Priority 2: FNV-32 switch (legacy path)
    const auto* meta = g_chaos_resolve_enum_metadata
        ? g_chaos_resolve_enum_metadata(desc->subject_id_utf8)
        : nullptr;
    if (meta == nullptr) return nullptr;
    CHAOS_IL2CPP_UINTPTR fname_len[64];
    CHAOS_IL2CPP_UINT32 cnt = precompute_name_lengths(meta, fname_len);
    for (CHAOS_IL2CPP_UINT32 i = 0; i < cnt; i++) {
        if (fname_len[i] == name_len && std::memcmp(meta->fields[i].name, name, name_len) == 0) {
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

    // Priority 1: dispatch table (FNV-24 hash → O(1) hash table)
    if (g_chaos_enum_dispatch_lookup) {
        uint32_t h = 2166136261u;
        for (const char* s = desc->subject_id_utf8; *s; ++s) {
            h ^= static_cast<uint8_t>(*s);
            h *= 16777619u;
        }
        const auto* meta = g_chaos_enum_dispatch_lookup(h & 0xFFFFFFu);
        if (meta != nullptr) {
            CHAOS_IL2CPP_UINTPTR fname_len[64];
            CHAOS_IL2CPP_UINT32 cnt = precompute_name_lengths(meta, fname_len);
            for (CHAOS_IL2CPP_UINT32 i = 0; i < cnt; i++) {
                if (fname_len[i] != name_len) continue;
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
    }

    // Priority 2: FNV-32 switch (legacy path)
    const auto* meta = g_chaos_resolve_enum_metadata
        ? g_chaos_resolve_enum_metadata(desc->subject_id_utf8)
        : nullptr;
    if (meta == nullptr) return nullptr;
    CHAOS_IL2CPP_UINTPTR fname_len[64];
    CHAOS_IL2CPP_UINT32 cnt = precompute_name_lengths(meta, fname_len);
    for (CHAOS_IL2CPP_UINT32 i = 0; i < cnt; i++) {
        if (fname_len[i] != name_len) continue;
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

    // Priority 1: dispatch table (FNV-24 binary search)
    if (g_chaos_enum_dispatch_lookup) {
        uint32_t h = 2166136261u;
        for (const char* s = desc->subject_id_utf8; *s; ++s) {
            h ^= static_cast<uint8_t>(*s);
            h *= 16777619u;
        }
        const auto* meta = g_chaos_enum_dispatch_lookup(h & 0xFFFFFFu);
        if (meta != nullptr) return meta->count;
    }

    // Priority 2: FNV-32 switch (legacy path)
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
///      runtime_type_handle field at offset 8 (after ThinLockableHeader)
///      and converts it to a TypeInfoHandle via ChaosReflectionGetTypeFromHandle.
///
/// Uses a small thread-local direct-mapped cache to skip redundant decoding
/// when the same type argument is used repeatedly (common in benchmark loops).
///
/// Returns nullptr if the type cannot be resolved.
static const ReflectionQueryTypeDescriptor* resolve_type_arg(CHAOS_IL2CPP_INTPTR type_arg) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("resolve_type_arg");
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

    // Case 2: managed Type object - read runtime_type_handle at offset 8 (after ThinLockableHeader)
    CHAOS_IL2CPP_INTPTR raw_handle = 0;
    std::memcpy(&raw_handle, reinterpret_cast<const void*>(type_arg + 8), sizeof(raw_handle));
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
        // Fallback: allocate from metadata, insert into hash table for re-use
        for (CHAOS_IL2CPP_UINT32 i = 0; i < meta->count; i++) {
            if (meta->fields[i].value == val) {
                const auto name_len = std::strlen(meta->fields[i].name);
                auto result = enum_alloc_string(name_len);
                write_string_data(result, meta->fields[i].name, name_len);
                enum_name_hash_insert(val, result);
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
    enum_name_hash_insert(val, result);
    return result;
}

/// Extract stable TypeInfoHandle from a type argument.
/// Returns the raw runtime_type_handle from the managed Type object
/// at offset 8 (after ThinLockableHeader), or the argument itself
/// if it's already a tagged TypeInfoHandle (0x02XXXXXX).
/// Returns 0 if neither.
static CHAOS_IL2CPP_UINTPTR enum_extract_type_handle(CHAOS_IL2CPP_INTPTR type_arg) noexcept {
    if (type_arg == 0) return 0;

    // Codegen pseudo-handle: 0x02XXXXXX (FNV-24 with prefix tag)
    uint32_t val = static_cast<uint32_t>(type_arg & 0xFFFFFFFFu);
    if ((val & 0xFF000000u) == 0x02000000u && (val & 0xFFFFFFu) != 0u)
        return static_cast<CHAOS_IL2CPP_UINTPTR>(val);

    // TypeInfoHandle with bit[63]=1 (reflection query tagged pointer).
    // Return the handle itself as a stable cache key. Do NOT try to read
    // +8 as a pointer — the handle is not a memory address and doing so
    // may crash (Subject_2 exit_code=0xFFFFFFFF).
    if (TryDecodeReflectionQueryTypeHandle(static_cast<TypeInfoHandle>(type_arg)) != nullptr)
        return static_cast<CHAOS_IL2CPP_UINTPTR>(type_arg);

    // Managed Type object: read runtime_type_handle at offset 8 (after ThinLockableHeader)
    CHAOS_IL2CPP_UINTPTR raw_handle = 0;
    std::memcpy(&raw_handle, reinterpret_cast<const void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(type_arg) + 8), sizeof(raw_handle));
    return raw_handle;
}

/// Enum.GetNames(Type) — returns a string[] of all enum literal names.
/// Results are cached per TypeInfoHandle (single-entry) to avoid repeated
/// GC allocation of strings and array in hot loops.
CHAOS_IL2CPP_INTPTR ChaosEnumGetNames(CHAOS_IL2CPP_INTPTR type) noexcept
{
    if (type == 0) return 0;

    // Register GC callback once per process to invalidate thread_local
    // caches before young collections (prevents dangling managed pointers).
    std::call_once(s_enum_gc_callback_flag, [] {
        GcRegisterEventCallback(enum_stubs_on_gc_event, nullptr);
    });

    // Result array cache: single-entry, keyed by TypeInfoHandle.
    // Extracting the handle early avoids the resolve_type_arg round-trip.
    CHAOS_IL2CPP_UINTPTR type_handle = enum_extract_type_handle(type);
    if (type_handle != 0 && type_handle == s_enum_names_array_key)
        return s_enum_names_array;

    // Fast path: direct metadata (no resolve_type_arg)
    const auto* meta = enum_resolve_meta(type);
    if (meta != nullptr && meta->count > 0)
    {
        // Pre-populate string cache so strings are allocated only once.
        ensure_enum_str_cache(type, meta);

        auto arr = enum_alloc_ptr_array(meta->count);
        if (arr == 0) return 0;
        auto* accessor = reinterpret_cast<ManagedArrayAccessor*>(arr);
        if (s_enum_str_count == meta->count) {
            // Reuse cached strings (zero-alloc on subsequent calls).
            for (CHAOS_IL2CPP_UINT32 i = 0; i < meta->count; i++)
                accessor_get_elements(accessor)[i] = s_enum_str_names[i];
        } else {
            // Fall back to direct allocation from meta (should be rare).
            for (CHAOS_IL2CPP_UINT32 i = 0; i < meta->count; i++) {
                const auto* entry = &meta->fields[i];
                // Try the hash table first (O(1) lookup)
                auto cached = lookup_cached_enum_name(entry->value);
                if (cached != 0) {
                    accessor_get_elements(accessor)[i] = cached;
                } else {
                    const auto name_len = std::strlen(entry->name);
                    auto str_handle = enum_alloc_string(name_len);
                    write_string_data(str_handle, entry->name, name_len);
                    accessor_get_elements(accessor)[i] = str_handle;
                    // Insert into hash table for subsequent O(1) lookup
                    enum_name_hash_insert(entry->value, str_handle);
                }
            }
        }

        // Cache the result array.
        s_enum_names_array_key = type_handle;
        s_enum_names_array = arr;
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
        accessor_get_elements(accessor)[idx++] = str_handle;
        // Insert each allocated string into the O(1) hash table
        enum_name_hash_insert(f.constant_value, str_handle);
    }

    s_enum_names_array_key = type_handle;
    s_enum_names_array = arr;
    return arr;
}

/// Enum.GetValues(Type) — returns an object[] of all enum literal values (boxed).
/// Results are cached per TypeInfoHandle (single-entry) to avoid repeated
/// GC allocation of boxed values in hot loops.
CHAOS_IL2CPP_INTPTR ChaosEnumGetValues(CHAOS_IL2CPP_INTPTR type) noexcept
{
    if (type == 0) return 0;

    // Result array cache: single-entry, keyed by TypeInfoHandle.
    CHAOS_IL2CPP_UINTPTR type_handle = enum_extract_type_handle(type);
    if (type_handle != 0 && type_handle == s_enum_values_array_key)
        return s_enum_values_array;

    // Fast path: direct metadata (no resolve_type_arg)
    const auto* meta = enum_resolve_meta(type);
    if (meta != nullptr && meta->count > 0)
    {
        auto arr = enum_alloc_ptr_array(meta->count);
        if (arr == 0) return 0;
        auto* accessor = reinterpret_cast<ManagedArrayAccessor*>(arr);
        for (CHAOS_IL2CPP_UINT32 i = 0; i < meta->count; i++) {
            accessor_get_elements(accessor)[i] = enum_alloc_boxed_int64(meta->fields[i].value);
        }
        s_enum_values_array_key = type_handle;
        s_enum_values_array = arr;
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
        accessor_get_elements(accessor)[idx++] = enum_alloc_boxed_int64(f.constant_value);
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
            CHAOS_IL2CPP_UINTPTR fname_len[64];
            CHAOS_IL2CPP_UINT32 cnt = precompute_name_lengths(meta, fname_len);
            for (CHAOS_IL2CPP_UINT32 i = 0; i < cnt; i++) {
                if (fname_len[i] == name_len && std::memcmp(meta->fields[i].name, name_data, name_len) == 0) {
                    return enum_alloc_boxed_int64(meta->fields[i].value, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(meta));
                }
            }
            // Case-insensitive fallback
            for (CHAOS_IL2CPP_UINT32 i = 0; i < cnt; i++) {
                if (fname_len[i] != name_len) continue;
                bool match = true;
                for (CHAOS_IL2CPP_UINTPTR j = 0; j < name_len; j++) {
                    char a = meta->fields[i].name[j];
                    char b = name_data[j];
                    if (a >= 'A' && a <= 'Z') a = static_cast<char>(a + 32);
                    if (b >= 'A' && b <= 'Z') b = static_cast<char>(b + 32);
                    if (a != b) { match = false; break; }
                }
                if (match) return enum_alloc_boxed_int64(meta->fields[i].value, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(meta));
            }
            // Numeric fallback (e.g., "42", "0xFF") — strtoll with base=0
            CHAOS_IL2CPP_INT64 numeric_val = 0;
            if (enum_try_parse_numeric_segment(name_data, name_len, numeric_val)) {
                return enum_alloc_boxed_int64(numeric_val, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(meta));
            }
            // Comma-separated fallback (e.g., "Monday, Tuesday", "1, Tuesday")
            CHAOS_IL2CPP_INT64 combined_val = 0;
            if (enum_try_parse_comma_separated_meta(meta, name_data, name_len, combined_val)) {
                return enum_alloc_boxed_int64(combined_val, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(meta));
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
        // Try numeric fallback (e.g., "42", "0xFF")
        CHAOS_IL2CPP_INT64 numeric_val = 0;
        if (enum_try_parse_numeric_segment(name_data, name_len, numeric_val)) {
            return enum_alloc_boxed_int64(numeric_val, type);
        }
        // Try comma-separated fallback (e.g., "Monday, Tuesday")
        CHAOS_IL2CPP_INT64 combined_val = 0;
        if (enum_try_parse_comma_separated_reflection(desc, name_data, name_len, combined_val)) {
            return enum_alloc_boxed_int64(combined_val, type);
        }
        RaiseArgumentException((std::string("Requested value '") + std::string(name_data, name_len) + "' was not found.").c_str());
    }
    return enum_alloc_boxed_int64(field->constant_value, type);
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
            CHAOS_IL2CPP_UINTPTR fname_len[64];
            CHAOS_IL2CPP_UINT32 cnt = precompute_name_lengths(meta, fname_len);
            for (CHAOS_IL2CPP_UINT32 i = 0; i < cnt; i++) {
                if (fname_len[i] == name_len && std::memcmp(meta->fields[i].name, name_data, name_len) == 0) {
                    return enum_alloc_boxed_int64(meta->fields[i].value, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(meta));
                }
            }
            if (ignoreCase) {
                for (CHAOS_IL2CPP_UINT32 i = 0; i < cnt; i++) {
                    if (fname_len[i] != name_len) continue;
                    bool match = true;
                    for (CHAOS_IL2CPP_UINTPTR j = 0; j < name_len; j++) {
                        char a = meta->fields[i].name[j];
                        char b = name_data[j];
                        if (a >= 'A' && a <= 'Z') a = static_cast<char>(a + 32);
                        if (b >= 'A' && b <= 'Z') b = static_cast<char>(b + 32);
                        if (a != b) { match = false; break; }
                    }
                    if (match) return enum_alloc_boxed_int64(meta->fields[i].value, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(meta));
                }
            }
            // Numeric fallback (e.g., "42", "0xFF")
            CHAOS_IL2CPP_INT64 numeric_val = 0;
            if (enum_try_parse_numeric_segment(name_data, name_len, numeric_val)) {
                return enum_alloc_boxed_int64(numeric_val, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(meta));
            }
            // Comma-separated fallback (e.g., "Monday, Tuesday")
            CHAOS_IL2CPP_INT64 combined_val = 0;
            if (enum_try_parse_comma_separated_meta(meta, name_data, name_len, combined_val)) {
                return enum_alloc_boxed_int64(combined_val, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(meta));
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
        // Try numeric fallback (e.g., "42", "0xFF")
        CHAOS_IL2CPP_INT64 numeric_val = 0;
        if (enum_try_parse_numeric_segment(name_data, name_len, numeric_val)) {
            return enum_alloc_boxed_int64(numeric_val, type);
        }
        // Try comma-separated fallback (e.g., "Monday, Tuesday")
        CHAOS_IL2CPP_INT64 combined_val = 0;
        if (enum_try_parse_comma_separated_reflection(desc, name_data, name_len, combined_val)) {
            return enum_alloc_boxed_int64(combined_val, type);
        }
        RaiseArgumentException((std::string("Requested value '") + std::string(name_data, name_len) + "' was not found.").c_str());
    }
    return enum_alloc_boxed_int64(field->constant_value, type);
}

/// ── Internal format implementation (shared by Format and FormatRaw) ─────
/// Formats an enum value (int64) as a string according to the format specifier.
/// type: managed Type handle for the enum type
/// val: raw int64 value to format
/// format_str: managed string for the format specifier
/// meta: pre-resolved EnumMetadataTable (may be null)
static CHAOS_IL2CPP_INTPTR enum_format_impl(
    CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INT64 val, CHAOS_IL2CPP_INTPTR format_str,
    const EnumMetadataTable* cached_meta) noexcept
{
    // Read format specifier
    CHAOS_IL2CPP_UINTPTR fmt_len = 0;
    const char* fmt_data = get_string_data(format_str, fmt_len);
    const bool is_g = (fmt_len >= 1 && (fmt_data[0] == 'G' || fmt_data[0] == 'g'));
    const bool is_d = (fmt_len >= 1 && (fmt_data[0] == 'D' || fmt_data[0] == 'd'));
    const bool is_x = (fmt_len >= 1 && (fmt_data[0] == 'X' || fmt_data[0] == 'x'));

    if (is_g || fmt_len == 0) {
        const bool meta_has_fields = (cached_meta != nullptr && cached_meta->count > 0);
        if (meta_has_fields) {
        if (g_chaos_enum_tostring_dispatch_lookup) {
            CHAOS_IL2CPP_UINTPTR handle = enum_extract_type_handle(type);
            uint32_t val_low32 = static_cast<uint32_t>(handle & 0xFFFFFFFFu);
            if ((val_low32 & 0xFF000000u) == 0x02000000u && (val_low32 & 0xFFFFFFu) != 0u) {
                auto result = g_chaos_enum_tostring_dispatch_lookup(val_low32 & 0xFFFFFFu, val);
                if (result != 0) { s_enum_tostring_cache_value = val; s_enum_tostring_cache_name = result; return result; }
            }
        }

        CHAOS_IL2CPP_UINTPTR effective_key = enum_extract_type_handle(type);
        if (effective_key != 0 && s_enum_str_type_key == effective_key
            && s_enum_tostring_cache_value == val && s_enum_tostring_cache_name != 0) {
            return s_enum_tostring_cache_name;
        }

        ensure_enum_str_cache(type, cached_meta);
        auto cached = lookup_cached_enum_name(val);
        if (cached != 0) { s_enum_tostring_cache_value = val; s_enum_tostring_cache_name = cached; return cached; }
        for (CHAOS_IL2CPP_UINT32 i = 0; i < cached_meta->count; i++) {
            if (cached_meta->fields[i].value == val) {
                const auto name_len = std::strlen(cached_meta->fields[i].name);
                auto result = enum_alloc_string(name_len);
                write_string_data(result, cached_meta->fields[i].name, name_len);
                s_enum_tostring_cache_value = val;
                s_enum_tostring_cache_name = result;
                enum_name_hash_insert(val, result);
                return result;
            }
        }
        }
    }

    if (is_g || is_d) {
        if (s_enum_fallback_cache_val == val && !s_enum_fallback_cache_is_x
            && s_enum_fallback_cache_result != 0) {
            return s_enum_fallback_cache_result;
        }
        char buf[32];
        char* const buf_end = buf + sizeof(buf);
        char* start = format_i64_dec(buf_end, val);
        const auto len = static_cast<CHAOS_IL2CPP_UINTPTR>(buf_end - start);
        auto result = enum_alloc_string(len);
        write_string_data(result, start, len);
        s_enum_fallback_cache_val = val;
        s_enum_fallback_cache_is_x = false;
        s_enum_fallback_cache_result = result;
        return result;
    }

    if (is_x) {
        unsigned int width = 0;
        if (fmt_len > 1) {
            width = 0;
            for (CHAOS_IL2CPP_UINTPTR i = 1; i < fmt_len; i++) {
                if (fmt_data[i] >= '0' && fmt_data[i] <= '9') {
                    width = width * 10 + static_cast<unsigned int>(fmt_data[i] - '0');
                } else break;
            }
        }
        if (width == 0 && s_enum_fallback_cache_val == val && s_enum_fallback_cache_is_x
            && s_enum_fallback_cache_result != 0) {
            return s_enum_fallback_cache_result;
        }
        char buf[32];
        char* const buf_end = buf + sizeof(buf);
        char* start = format_u64_hex(buf_end, static_cast<uint64_t>(val), width);
        const auto len = static_cast<CHAOS_IL2CPP_UINTPTR>(buf_end - start);
        auto result = enum_alloc_string(len);
        write_string_data(result, start, len);
        if (width == 0) {
            s_enum_fallback_cache_val = val;
            s_enum_fallback_cache_is_x = true;
            s_enum_fallback_cache_result = result;
        }
        return result;
    }

    return 0;
}

/// Enum.Format(Type, Object, String) — formats an enum value as a string.
/// Supports "G" (name), "D" (decimal), "X" (hex, lowercase).
CHAOS_IL2CPP_INTPTR ChaosEnumFormat(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR value, CHAOS_IL2CPP_INTPTR format_str) noexcept
{
    if (type == 0 || value == 0) return 0;
    const CHAOS_IL2CPP_INT64 val = read_boxed_value(value);
    const EnumMetadataTable* cached_meta = enum_resolve_meta(type);
    if (cached_meta == nullptr) {
        const auto* desc = resolve_type_arg(type);
        check_enum_type(desc); // validate (discard result — meta will be null for non-enum)
    }
    return enum_format_impl(type, val, format_str, cached_meta);
}

/// Enum.Format(Type, Int64, String) — box-free variant for box-eliminated codegen.
/// Same as ChaosEnumFormat but takes a raw int64 value instead of a boxed object.
CHAOS_IL2CPP_INTPTR ChaosEnumFormatRaw(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INT64 raw_value, CHAOS_IL2CPP_INTPTR format_str) noexcept
{
    if (type == 0 || format_str == 0) return 0;
    const EnumMetadataTable* meta = enum_resolve_meta(type);
    if (meta == nullptr) {
        const auto* desc = resolve_type_arg(type);
        check_enum_type(desc);
    }
    return enum_format_impl(type, raw_value, format_str, meta);
}

                const auto* field = find_field_by_value(desc, val);
                if (field != nullptr && field->name_utf8 != nullptr) {
                    const auto name_len = std::strlen(field->name_utf8);
                    auto result = enum_alloc_string(name_len);
                    write_string_data(result, field->name_utf8, name_len);
                    s_enum_tostring_cache_value = val;
                    s_enum_tostring_cache_name = result;
                    enum_name_hash_insert(val, result);
                    return result;
                }
            }
        }
    }

    if (is_g || is_d) {
        // Single-entry cache for repeated calls with the same value
        if (s_enum_fallback_cache_val == val && !s_enum_fallback_cache_is_x
            && s_enum_fallback_cache_result != 0) {
            return s_enum_fallback_cache_result;
        }
        char buf[32];
        char* const buf_end = buf + sizeof(buf);
        char* start = format_i64_dec(buf_end, val);
        const auto len = static_cast<CHAOS_IL2CPP_UINTPTR>(buf_end - start);
        auto result = enum_alloc_string(len);
        write_string_data(result, start, len);
        s_enum_fallback_cache_val = val;
        s_enum_fallback_cache_is_x = false;
        s_enum_fallback_cache_result = result;
        return result;
    }

    if (is_x) {
        unsigned int width = 0;
        if (fmt_len > 1) {
            width = 0;
            for (CHAOS_IL2CPP_UINTPTR i = 1; i < fmt_len; i++) {
                if (fmt_data[i] >= '0' && fmt_data[i] <= '9') {
                    width = width * 10 + static_cast<unsigned int>(fmt_data[i] - '0');
                } else break;
            }
        }
        // Single-entry cache: only when width == 0 (no precision qualifier)
        if (width == 0 && s_enum_fallback_cache_val == val && s_enum_fallback_cache_is_x
            && s_enum_fallback_cache_result != 0) {
            return s_enum_fallback_cache_result;
        }
        char buf[32];
        char* const buf_end = buf + sizeof(buf);
        char* start = format_u64_hex(buf_end, static_cast<uint64_t>(val), width);
        const auto len = static_cast<CHAOS_IL2CPP_UINTPTR>(buf_end - start);
        auto result = enum_alloc_string(len);
        write_string_data(result, start, len);
        if (width == 0) {
            s_enum_fallback_cache_val = val;
            s_enum_fallback_cache_is_x = true;
            s_enum_fallback_cache_result = result;
        }
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

    // Per-enum codegen switch dispatch (~5ns, zero allocation).
    // Only works with codegen pseudo-handles (0x02XXXXXX).
    // For other handle types (reflection handles, raw pointers), falls
    // through to the generic cache path — ensuring compatibility.
    if (g_chaos_enum_tostring_dispatch_lookup) {
        CHAOS_IL2CPP_UINTPTR handle = enum_extract_type_handle(type_handle);
        uint32_t val_low32 = static_cast<uint32_t>(handle & 0xFFFFFFFFu);
        if ((val_low32 & 0xFF000000u) == 0x02000000u && (val_low32 & 0xFFFFFFu) != 0u) {
            auto result = g_chaos_enum_tostring_dispatch_lookup(val_low32 & 0xFFFFFFu, val);
            if (result != 0) { s_enum_tostring_cache_value = val; s_enum_tostring_cache_name = result; return result; }
            // 0 = not a named field → fall through to decimal
        }
    }

    // Fastest path: single-entry direct-mapped cache (2 integer compares)
    CHAOS_IL2CPP_UINTPTR effective_key = enum_extract_type_handle(type_handle);
    if (effective_key != 0 && s_enum_str_type_key == effective_key
        && s_enum_tostring_cache_value == val && s_enum_tostring_cache_name != 0) {
        return s_enum_tostring_cache_name;
    }

    // Fast path: direct metadata (no resolve_type_arg)
    const auto* meta = enum_resolve_meta(type_handle);
    if (meta != nullptr) {
        // POH cache: zero-alloc on repeated calls for the same type
        ensure_enum_str_cache(type_handle, meta);
        auto cached = lookup_cached_enum_name(val);
        if (cached != 0) { s_enum_tostring_cache_value = val; s_enum_tostring_cache_name = cached; return cached; }
        // Fallback: allocate from metadata and insert into hash table for re-use
        for (CHAOS_IL2CPP_UINT32 i = 0; i < meta->count; i++) {
            if (meta->fields[i].value == val) {
                const auto name_len = std::strlen(meta->fields[i].name);
                auto result = enum_alloc_string(name_len);
                write_string_data(result, meta->fields[i].name, name_len);
                s_enum_tostring_cache_value = val;
                s_enum_tostring_cache_name = result;
                enum_name_hash_insert(val, result);
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
        s_enum_tostring_cache_value = val;
        s_enum_tostring_cache_name = result;
        return result;
    }

    // Fallback: reflection API
    const auto* desc = resolve_type_arg(type_handle);
    if (desc == nullptr) return 0;
    if (check_enum_type(desc) == nullptr) return 0;

    ensure_enum_str_cache(type_handle, nullptr, desc);
    auto cached = lookup_cached_enum_name(val);
    if (cached != 0) { s_enum_tostring_cache_value = val; s_enum_tostring_cache_name = cached; return cached; }

    const auto* field = find_field_by_value(desc, val);
    if (field == nullptr || field->name_utf8 == nullptr) {
        // Fallback: return decimal (manual itoa)
        char buf[32];
        char* const buf_end = buf + sizeof(buf);
        char* start = format_i64_dec(buf_end, val);
        const auto len = static_cast<CHAOS_IL2CPP_UINTPTR>(buf_end - start);
        auto result = enum_alloc_string(len);
        write_string_data(result, start, len);
        s_enum_tostring_cache_value = val;
        s_enum_tostring_cache_name = result;
        return result;
    }

    const auto name_len = std::strlen(field->name_utf8);
    auto result = enum_alloc_string(name_len);
    write_string_data(result, field->name_utf8, name_len);
    s_enum_tostring_cache_value = val;
    s_enum_tostring_cache_name = result;
    enum_name_hash_insert(val, result);
    return result;
}

/// Enum.ToString() raw path — skips box reading for the box-elimination peephole.
/// Takes type_handle and raw_value directly instead of reading them from a boxed object.
/// Used by codegen when it detects box + call Enum::ToString and fuses them.
CHAOS_IL2CPP_INTPTR ChaosEnumToStringRaw(CHAOS_IL2CPP_INTPTR type_handle, CHAOS_IL2CPP_INT64 raw_value) noexcept
{
    if (type_handle == 0) return 0;

    const CHAOS_IL2CPP_INT64 val = raw_value;

    // Per-enum codegen switch dispatch (~5ns, zero allocation)
    if (g_chaos_enum_tostring_dispatch_lookup) {
        CHAOS_IL2CPP_UINTPTR handle = enum_extract_type_handle(type_handle);
        uint32_t val_low32 = static_cast<uint32_t>(handle & 0xFFFFFFFFu);
        if ((val_low32 & 0xFF000000u) == 0x02000000u && (val_low32 & 0xFFFFFFu) != 0u) {
            auto result = g_chaos_enum_tostring_dispatch_lookup(val_low32 & 0xFFFFFFu, val);
            if (result != 0) { s_enum_tostring_cache_value = val; s_enum_tostring_cache_name = result; return result; }
        }
    }

    // Fastest path: single-entry direct-mapped cache (2 integer compares)
    CHAOS_IL2CPP_UINTPTR effective_key = enum_extract_type_handle(type_handle);
    if (effective_key != 0 && s_enum_str_type_key == effective_key
        && s_enum_tostring_cache_value == val && s_enum_tostring_cache_name != 0) {
        return s_enum_tostring_cache_name;
    }

    // Fast path: direct metadata (no resolve_type_arg)
    const auto* meta = enum_resolve_meta(type_handle);
    if (meta != nullptr) {
        // POH cache: zero-alloc on repeated calls for the same type
        ensure_enum_str_cache(type_handle, meta);
        auto cached = lookup_cached_enum_name(val);
        if (cached != 0) { s_enum_tostring_cache_value = val; s_enum_tostring_cache_name = cached; return cached; }
        // Fallback: allocate from metadata and insert into hash table for re-use
        for (CHAOS_IL2CPP_UINT32 i = 0; i < meta->count; i++) {
            if (meta->fields[i].value == val) {
                const auto name_len = std::strlen(meta->fields[i].name);
                auto result = enum_alloc_string(name_len);
                write_string_data(result, meta->fields[i].name, name_len);
                s_enum_tostring_cache_value = val;
                s_enum_tostring_cache_name = result;
                enum_name_hash_insert(val, result);
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
        s_enum_tostring_cache_value = val;
        s_enum_tostring_cache_name = result;
        return result;
    }

    // Fallback: reflection API
    const auto* desc = resolve_type_arg(type_handle);
    if (desc == nullptr) return 0;
    if (check_enum_type(desc) == nullptr) return 0;

    ensure_enum_str_cache(type_handle, nullptr, desc);
    auto cached = lookup_cached_enum_name(val);
    if (cached != 0) { s_enum_tostring_cache_value = val; s_enum_tostring_cache_name = cached; return cached; }

    const auto* field = find_field_by_value(desc, val);
    if (field == nullptr || field->name_utf8 == nullptr) {
        // Fallback: return decimal (manual itoa)
        char buf[32];
        char* const buf_end = buf + sizeof(buf);
        char* start = format_i64_dec(buf_end, val);
        const auto len = static_cast<CHAOS_IL2CPP_UINTPTR>(buf_end - start);
        auto result = enum_alloc_string(len);
        write_string_data(result, start, len);
        s_enum_tostring_cache_value = val;
        s_enum_tostring_cache_name = result;
        return result;
    }

    const auto name_len = std::strlen(field->name_utf8);
    auto result = enum_alloc_string(name_len);
    write_string_data(result, field->name_utf8, name_len);
    s_enum_tostring_cache_value = val;
    s_enum_tostring_cache_name = result;
    enum_name_hash_insert(val, result);
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
    CHAOS_IL2CPP_PROFILE_SCOPE("ChaosEnumTryParse");
    if (type == 0 || name == 0 || result_out == 0) return 0;

    CHAOS_IL2CPP_UINTPTR name_len = 0;
    const char* name_data = get_string_data(name, name_len);

    // Fast path: direct metadata (no resolve_type_arg)
    {
        const auto* meta = enum_resolve_meta(type);
        if (meta != nullptr) {
            CHAOS_IL2CPP_UINTPTR fname_len[64];
            CHAOS_IL2CPP_UINT32 cnt = precompute_name_lengths(meta, fname_len);
            for (CHAOS_IL2CPP_UINT32 i = 0; i < cnt; i++) {
                if (fname_len[i] == name_len && std::memcmp(meta->fields[i].name, name_data, name_len) == 0) {
                    auto boxed = enum_alloc_boxed_int64(meta->fields[i].value, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(meta));
                    std::memcpy(reinterpret_cast<void*>(result_out), &boxed, sizeof(boxed));
                    return 1;
                }
            }
            // Case-insensitive fallback
            for (CHAOS_IL2CPP_UINT32 i = 0; i < cnt; i++) {
                if (fname_len[i] != name_len) continue;
                bool match = true;
                for (CHAOS_IL2CPP_UINTPTR j = 0; j < name_len; j++) {
                    char a = meta->fields[i].name[j];
                    char b = name_data[j];
                    if (a >= 'A' && a <= 'Z') a = static_cast<char>(a + 32);
                    if (b >= 'A' && b <= 'Z') b = static_cast<char>(b + 32);
                    if (a != b) { match = false; break; }
                }
                if (match) {
                    auto boxed = enum_alloc_boxed_int64(meta->fields[i].value, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(meta));
                    std::memcpy(reinterpret_cast<void*>(result_out), &boxed, sizeof(boxed));
                    return 1;
                }
            }
            // Numeric fallback (e.g., "42", "0xFF")
            CHAOS_IL2CPP_INT64 numeric_val = 0;
            if (enum_try_parse_numeric_segment(name_data, name_len, numeric_val)) {
                auto boxed = enum_alloc_boxed_int64(numeric_val, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(meta));
                std::memcpy(reinterpret_cast<void*>(result_out), &boxed, sizeof(boxed));
                return 1;
            }
            // Comma-separated fallback (e.g., "Monday, Tuesday")
            CHAOS_IL2CPP_INT64 combined_val = 0;
            if (enum_try_parse_comma_separated_meta(meta, name_data, name_len, combined_val)) {
                auto boxed = enum_alloc_boxed_int64(combined_val, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(meta));
                std::memcpy(reinterpret_cast<void*>(result_out), &boxed, sizeof(boxed));
                return 1;
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
    if (field == nullptr) {
        // Try numeric fallback (e.g., "42", "0xFF")
        CHAOS_IL2CPP_INT64 numeric_val = 0;
        if (enum_try_parse_numeric_segment(name_data, name_len, numeric_val)) {
            auto boxed = enum_alloc_boxed_int64(numeric_val, type);
            std::memcpy(reinterpret_cast<void*>(result_out), &boxed, sizeof(boxed));
            return 1;
        }
        // Try comma-separated fallback (e.g., "Monday, Tuesday")
        CHAOS_IL2CPP_INT64 combined_val = 0;
        if (enum_try_parse_comma_separated_reflection(desc, name_data, name_len, combined_val)) {
            auto boxed = enum_alloc_boxed_int64(combined_val, type);
            std::memcpy(reinterpret_cast<void*>(result_out), &boxed, sizeof(boxed));
            return 1;
        }
        return 0;
    }

    auto boxed = enum_alloc_boxed_int64(field->constant_value, type);
    std::memcpy(reinterpret_cast<void*>(result_out), &boxed, sizeof(boxed));
    return 1;
}

/// Enum.TryParse(Type, String, Boolean, out Object) — tries to parse with ignoreCase.
CHAOS_IL2CPP_INT32 ChaosEnumTryParseWithIgnoreCase(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR name, CHAOS_IL2CPP_INT32 ignoreCase, CHAOS_IL2CPP_INTPTR result_out) noexcept
{
    CHAOS_IL2CPP_PROFILE_SCOPE("ChaosEnumTryParseWithIgnoreCase");
    if (type == 0 || name == 0 || result_out == 0) return 0;

    CHAOS_IL2CPP_UINTPTR name_len = 0;
    const char* name_data = get_string_data(name, name_len);

    // Fast path: direct metadata (no resolve_type_arg)
    {
        const auto* meta = enum_resolve_meta(type);
        if (meta != nullptr) {
            CHAOS_IL2CPP_UINTPTR fname_len[64];
            CHAOS_IL2CPP_UINT32 cnt = precompute_name_lengths(meta, fname_len);
            for (CHAOS_IL2CPP_UINT32 i = 0; i < cnt; i++) {
                if (fname_len[i] == name_len && std::memcmp(meta->fields[i].name, name_data, name_len) == 0) {
                    auto boxed = enum_alloc_boxed_int64(meta->fields[i].value, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(meta));
                    std::memcpy(reinterpret_cast<void*>(result_out), &boxed, sizeof(boxed));
                    return 1;
                }
            }
            if (ignoreCase) {
                for (CHAOS_IL2CPP_UINT32 i = 0; i < cnt; i++) {
                    if (fname_len[i] != name_len) continue;
                    bool match = true;
                    for (CHAOS_IL2CPP_UINTPTR j = 0; j < name_len; j++) {
                        char a = meta->fields[i].name[j];
                        char b = name_data[j];
                        if (a >= 'A' && a <= 'Z') a = static_cast<char>(a + 32);
                        if (b >= 'A' && b <= 'Z') b = static_cast<char>(b + 32);
                        if (a != b) { match = false; break; }
                    }
                    if (match) {
                        auto boxed = enum_alloc_boxed_int64(meta->fields[i].value, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(meta));
                        std::memcpy(reinterpret_cast<void*>(result_out), &boxed, sizeof(boxed));
                        return 1;
                    }
                }
            }
            // Numeric fallback (e.g., "42", "0xFF")
            CHAOS_IL2CPP_INT64 numeric_val = 0;
            if (enum_try_parse_numeric_segment(name_data, name_len, numeric_val)) {
                auto boxed = enum_alloc_boxed_int64(numeric_val, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(meta));
                std::memcpy(reinterpret_cast<void*>(result_out), &boxed, sizeof(boxed));
                return 1;
            }
            // Comma-separated fallback (e.g., "Monday, Tuesday")
            CHAOS_IL2CPP_INT64 combined_val = 0;
            if (enum_try_parse_comma_separated_meta(meta, name_data, name_len, combined_val)) {
                auto boxed = enum_alloc_boxed_int64(combined_val, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(meta));
                std::memcpy(reinterpret_cast<void*>(result_out), &boxed, sizeof(boxed));
                return 1;
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
    if (field == nullptr) {
        // Try numeric fallback (e.g., "42", "0xFF")
        CHAOS_IL2CPP_INT64 numeric_val = 0;
        if (enum_try_parse_numeric_segment(name_data, name_len, numeric_val)) {
            auto boxed = enum_alloc_boxed_int64(numeric_val, type);
            std::memcpy(reinterpret_cast<void*>(result_out), &boxed, sizeof(boxed));
            return 1;
        }
        // Try comma-separated fallback (e.g., "Monday, Tuesday")
        CHAOS_IL2CPP_INT64 combined_val = 0;
        if (enum_try_parse_comma_separated_reflection(desc, name_data, name_len, combined_val)) {
            auto boxed = enum_alloc_boxed_int64(combined_val, type);
            std::memcpy(reinterpret_cast<void*>(result_out), &boxed, sizeof(boxed));
            return 1;
        }
        return 0;
    }

    auto boxed = enum_alloc_boxed_int64(field->constant_value, type);
    std::memcpy(reinterpret_cast<void*>(result_out), &boxed, sizeof(boxed));
    return 1;
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core
