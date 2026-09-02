// enum_stubs_parse.cpp — Enum Parse/TryParse stub implementations
//
// Text-included from enum_stubs.cpp (same TU, no CMakeLists.txt change).
// This file provides the Parse/TryParse family of functions:
//   ChaosEnumParse, ChaosEnumParseWithIgnoreCase,
//   ChaosEnumTryParse, ChaosEnumTryParseWithIgnoreCase
//
// All static helper functions referenced here are defined in enum_stubs.cpp
// and are visible because text-inclusion keeps everything in the same TU.

namespace chaos::il2cpp::runtime_core {
extern "C" {

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
        // Try comma-separated fallback (e.g., "Monday", "Tuesday")
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