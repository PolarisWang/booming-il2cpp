// ABI exports: extern "C" linkage for managed/NativeAOT callability.

// enum_stubs.cpp — Enum helper stub implementations
#include <chaos/native_types.h>
#include <chaos/log.h>
#include <cstring>
#include <cstdio>

#include "generated_code_compat.h"
#include "runtime_stubs/stub_common.h"
#include "gc_helpers.h"
#include "reflection_query_model.h"
#include "reflection_api.h"

namespace chaos::il2cpp::runtime_core {
extern "C" {

// ── Internal helpers ────────────────────────────────────────────

/// Allocate a managed string with the given byte_count of UTF-8 payload + NUL.
static CHAOS_IL2CPP_INTPTR enum_alloc_string(CHAOS_IL2CPP_UINTPTR byte_count) noexcept
{
    auto* result = static_cast<StubStringHeader*>(
        GcAllocateAtomic(sizeof(StubStringHeader) + byte_count + 1));
    if (result == nullptr) return 0;
    result->type = 0;
    result->byte_count = byte_count;
    reinterpret_cast<char*>(result + 1)[byte_count] = '\0';
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(result);
}

/// Write a C string into a pre-allocated stub string's data area.
static void write_string_data(CHAOS_IL2CPP_INTPTR str_handle, const char* data, CHAOS_IL2CPP_UINTPTR len) noexcept
{
    if (str_handle == 0 || data == nullptr || len == 0) return;
    auto* sh = reinterpret_cast<StubStringHeader*>(str_handle);
    if (len > sh->byte_count) len = sh->byte_count;
    char* dest = reinterpret_cast<char*>(sh + 1);
    std::memcpy(dest, data, len);
    dest[len] = '\0';
}

/// Extract the UTF-8 data pointer and length from a managed string handle.
static const char* get_string_data(CHAOS_IL2CPP_INTPTR str_handle, CHAOS_IL2CPP_UINTPTR& out_len) noexcept
{
    if (str_handle == 0) { out_len = 0; return ""; }
    auto* sh = reinterpret_cast<const StubStringHeader*>(str_handle);
    out_len = sh->byte_count;
    return stub_string_data(sh);
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
        // Enum value fields have a constant_value (literal fields in the IL metadata).
        // Fields with constant_value == 0 AND name matching something like "value__" are the
        // underlying value field, not an enum literal. We include all fields with a name
        // that does NOT start with "value_" (the backing field names are typically "value__").
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

// ── Public Enum stub functions ────────────────────────────────────

/// Resolve a type argument to a ReflectionQueryTypeDescriptor.
///
/// Handles two cases:
///   1. Direct TypeInfoHandle (kReflectionQueryHandleTag high bit set) ->
///      decoded via TryDecodeReflectionQueryTypeHandle.
///   2. Managed Type object (GC pointer, no high bit) -> extracts the
///      runtime_type_handle field at offset 16 (after ThinLockableHeader)
///      and converts it to a TypeInfoHandle via ChaosReflectionGetTypeFromHandle.
///
/// Returns nullptr if the type cannot be resolved.
static const ReflectionQueryTypeDescriptor* resolve_type_arg(CHAOS_IL2CPP_INTPTR type_arg) noexcept {
    if (type_arg == 0) return nullptr;

    // Case 1: direct TypeInfoHandle (high bit set)
    auto* desc = TryDecodeReflectionQueryTypeHandle(static_cast<TypeInfoHandle>(type_arg));
    if (desc != nullptr) {
        CHAOS_IL2CPP_LOG_DEBUG_M("enum_stubs", "resolve_type_arg: direct handle, desc={0} fields={1}", (void*)desc, desc->field_count);
        return desc;
    }

    // Case 2: managed Type object - read runtime_type_handle at offset 16
    CHAOS_IL2CPP_INTPTR raw_handle = 0;
    std::memcpy(&raw_handle, reinterpret_cast<const void*>(type_arg + 16), sizeof(raw_handle));
    CHAOS_IL2CPP_LOG_DEBUG_M("enum_stubs", "resolve_type_arg: managed obj, type_arg={0} raw_handle={1}", (void*)type_arg, (unsigned long long)raw_handle);
    if (raw_handle == 0) return nullptr;

    auto type_info_handle = ChaosReflectionGetTypeFromHandle(raw_handle);
    CHAOS_IL2CPP_LOG_DEBUG_M("enum_stubs", "resolve_type_arg: type_info_handle={0}", (void*)type_info_handle);
    if (type_info_handle == 0) return nullptr;

    desc = TryDecodeReflectionQueryTypeHandle(static_cast<TypeInfoHandle>(type_info_handle));
    CHAOS_IL2CPP_LOG_DEBUG_M("enum_stubs", "resolve_type_arg: decoded desc={0} fields={1}", (void*)desc, desc ? desc->field_count : 0);
    return desc;
}

/// Enum.IsDefined(Type, Object) — returns 1 if value is a valid enum literal.
CHAOS_IL2CPP_INT32 ChaosEnumIsDefined(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (type == 0) return 0;
    const auto* desc = resolve_type_arg(type);
    if (desc == nullptr) return 0;

    const CHAOS_IL2CPP_INT64 val = read_boxed_value(value);
    return find_field_by_value(desc, val) != nullptr ? 1 : 0;
}

/// Enum.GetName(Type, Object) — returns the name string for an enum value.
CHAOS_IL2CPP_INTPTR ChaosEnumGetName(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (type == 0) return 0;
    const auto* desc = resolve_type_arg(type);
    if (desc == nullptr) return 0;

    const CHAOS_IL2CPP_INT64 val = read_boxed_value(value);
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
    const auto* desc = resolve_type_arg(type);
    if (desc == nullptr) return 0;

    const auto count = count_enum_fields(desc);
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
    const auto* desc = resolve_type_arg(type);
    if (desc == nullptr) return 0;

    const auto count = count_enum_fields(desc);
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
    const auto* desc = resolve_type_arg(type);
    if (desc == nullptr) return 0;

    CHAOS_IL2CPP_UINTPTR name_len = 0;
    const char* name_data = get_string_data(name, name_len);

    const auto* field = find_field_by_name(desc, name_data, name_len);
    if (field == nullptr) {
        // Try case-insensitive
        field = find_field_by_name_icase(desc, name_data, name_len);
    }
    if (field == nullptr) return 0;

    return enum_alloc_boxed_int64(field->constant_value);
}

/// Enum.Parse(Type, String, Boolean) — parses with optional ignoreCase.
CHAOS_IL2CPP_INTPTR ChaosEnumParseWithIgnoreCase(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR name, CHAOS_IL2CPP_INT32 ignoreCase) noexcept
{
    if (type == 0 || name == 0) return 0;
    const auto* desc = resolve_type_arg(type);
    if (desc == nullptr) return 0;

    CHAOS_IL2CPP_UINTPTR name_len = 0;
    const char* name_data = get_string_data(name, name_len);

    const auto* field = find_field_by_name(desc, name_data, name_len);
    if (field == nullptr && ignoreCase) {
        field = find_field_by_name_icase(desc, name_data, name_len);
    }
    if (field == nullptr) return 0;

    return enum_alloc_boxed_int64(field->constant_value);
}

/// Enum.Format(Type, Object, String) — formats an enum value as a string.
/// Supports "G" (name), "D" (decimal), "X" (hex, lowercase).
CHAOS_IL2CPP_INTPTR ChaosEnumFormat(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR value, CHAOS_IL2CPP_INTPTR format_str) noexcept
{
    if (type == 0 || value == 0) return 0;
    const auto* desc = resolve_type_arg(type);
    if (desc == nullptr) return 0;

    const CHAOS_IL2CPP_INT64 val = read_boxed_value(value);

    // Read format specifier
    CHAOS_IL2CPP_UINTPTR fmt_len = 0;
    const char* fmt_data = get_string_data(format_str, fmt_len);
    const bool is_g = (fmt_len >= 1 && (fmt_data[0] == 'G' || fmt_data[0] == 'g'));
    const bool is_d = (fmt_len >= 1 && (fmt_data[0] == 'D' || fmt_data[0] == 'd'));
    const bool is_x = (fmt_len >= 1 && (fmt_data[0] == 'X' || fmt_data[0] == 'x'));

    if (is_g || fmt_len == 0) {
        // "G" format: return the name if found, otherwise decimal
        const auto* field = find_field_by_value(desc, val);
        if (field != nullptr && field->name_utf8 != nullptr) {
            const auto name_len = std::strlen(field->name_utf8);
            auto result = enum_alloc_string(name_len);
            write_string_data(result, field->name_utf8, name_len);
            return result;
        }
        // Fall through to decimal
    }

    if (is_g || is_d) {
        // "D" format or "G" fallback: decimal representation
        char buf[32];
        const auto len = static_cast<CHAOS_IL2CPP_UINTPTR>(
            std::snprintf(buf, sizeof(buf), "%lld", static_cast<long long>(val)));
        auto result = enum_alloc_string(len);
        write_string_data(result, buf, len);
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
        char fmt_buf[8];
        char out_buf[32];
        if (width > 0 && width <= 16) {
            std::snprintf(fmt_buf, sizeof(fmt_buf), "%%0%ullx", static_cast<unsigned long>(width));
        } else {
            std::snprintf(fmt_buf, sizeof(fmt_buf), "%%llx");
        }
        const auto len = static_cast<CHAOS_IL2CPP_UINTPTR>(
            std::snprintf(out_buf, sizeof(out_buf), fmt_buf, static_cast<unsigned long long>(val)));
        auto result = enum_alloc_string(len);
        write_string_data(result, out_buf, len);
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

    const auto* desc = resolve_type_arg(type_handle);
    if (desc == nullptr) return 0;

    const CHAOS_IL2CPP_INT64 val = read_boxed_value(this_obj);
    const auto* field = find_field_by_value(desc, val);
    if (field == nullptr || field->name_utf8 == nullptr) {
        // Fallback: return decimal
        char buf[32];
        const auto len = static_cast<CHAOS_IL2CPP_UINTPTR>(
            std::snprintf(buf, sizeof(buf), "%lld", static_cast<long long>(val)));
        auto result = enum_alloc_string(len);
        write_string_data(result, buf, len);
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
CHAOS_IL2CPP_INT32 ChaosEnumTryParse(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR name, CHAOS_IL2CPP_INTPTR result) noexcept
{
    if (type == 0 || name == 0 || result == 0) return 0;
    const auto* desc = resolve_type_arg(type);
    if (desc == nullptr) return 0;

    CHAOS_IL2CPP_UINTPTR name_len = 0;
    const char* name_data = get_string_data(name, name_len);

    const auto* field = find_field_by_name(desc, name_data, name_len);
    if (field == nullptr) {
        field = find_field_by_name_icase(desc, name_data, name_len);
    }
    if (field == nullptr) return 0;

    // Write boxed value through the out pointer
    auto boxed = enum_alloc_boxed_int64(field->constant_value);
    std::memcpy(reinterpret_cast<void*>(result), &boxed, sizeof(boxed));
    return 1;
}

/// Enum.TryParse(Type, String, Boolean, out Object) — tries to parse with ignoreCase.
CHAOS_IL2CPP_INT32 ChaosEnumTryParseWithIgnoreCase(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR name, CHAOS_IL2CPP_INT32 ignoreCase, CHAOS_IL2CPP_INTPTR result) noexcept
{
    if (type == 0 || name == 0 || result == 0) return 0;
    const auto* desc = resolve_type_arg(type);
    if (desc == nullptr) return 0;

    CHAOS_IL2CPP_UINTPTR name_len = 0;
    const char* name_data = get_string_data(name, name_len);

    const auto* field = find_field_by_name(desc, name_data, name_len);
    if (field == nullptr && ignoreCase) {
        field = find_field_by_name_icase(desc, name_data, name_len);
    }
    if (field == nullptr) return 0;

    auto boxed = enum_alloc_boxed_int64(field->constant_value);
    std::memcpy(reinterpret_cast<void*>(result), &boxed, sizeof(boxed));
    return 1;
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core
