#!/usr/bin/env python3
"""Replace all ensure_enum_type calls with check_enum_type + null-return."""

with open('D:/agent/booming-il2cpp/src/native/runtime-core/runtime_stubs/enum_stubs.cpp', 'r', encoding='utf-8') as f:
    content = f.read()

# ChaosEnumParse fallback
old = (
    "    // Fallback: reflection API\n"
    "    const auto* desc = resolve_type_arg(type);\n"
    "    if (desc == nullptr) return 0;\n"
    "    ensure_enum_type(desc);\n"
    "\n"
    "    const auto* field = find_field_by_name(desc, name_data, name_len);"
)
new = (
    "    // Fallback: reflection API\n"
    "    const auto* desc = resolve_type_arg(type);\n"
    "    if (desc == nullptr) return 0;\n"
    "    if (check_enum_type(desc) == nullptr) return 0;\n"
    "\n"
    "    const auto* field = find_field_by_name(desc, name_data, name_len);"
)
assert old in content, "ChaosEnumParse pattern not found!"
content = content.replace(old, new, 1)
print("Parse fixed")

# ChaosEnumFormat early check
old_fmt = (
    "        if (meta == nullptr) {\n"
    "            const auto* desc_eh = resolve_type_arg(type);\n"
    "            ensure_enum_type(desc_eh);\n"
    "        }"
)
new_fmt = (
    "        if (meta == nullptr) {\n"
    "            const auto* desc_eh = resolve_type_arg(type);\n"
    "            if (check_enum_type(desc_eh) == nullptr) return 0;\n"
    "        }"
)
if old_fmt in content:
    content = content.replace(old_fmt, new_fmt, 1)
    print("Format early check fixed")
else:
    old_fmt2 = (
        "        if (meta == nullptr) {\n"
        "            const auto* desc = resolve_type_arg(type);\n"
        "            ensure_enum_type(desc);\n"
        "        }"
    )
    if old_fmt2 in content:
        content = content.replace(old_fmt2, new_fmt.replace('desc_eh', 'desc'), 1)
        print("Format early check fixed (desc)")
    else:
        print("Format early check NOT FOUND")

# ChaosEnumParseWithIgnoreCase fallback
old_pic = (
    "    ensure_enum_type(desc);\n"
    "\n"
    "    const auto* field = find_field_by_name(desc, name_data, name_len);\n"
    "    if (field == nullptr && ignoreCase) {"
)
new_pic = (
    "    if (check_enum_type(desc) == nullptr) return 0;\n"
    "\n"
    "    const auto* field = find_field_by_name(desc, name_data, name_len);\n"
    "    if (field == nullptr && ignoreCase) {"
)
assert old_pic in content, "ParseWithIgnoreCase pattern not found!"
content = content.replace(old_pic, new_pic, 1)
print("ParseWithIgnoreCase fixed")

# ChaosEnumToString fallback
old_ts = (
    "    ensure_enum_type(desc);\n"
    "\n"
    "    ensure_enum_str_cache(type_handle, nullptr, desc);"
)
new_ts = (
    "    if (check_enum_type(desc) == nullptr) return 0;\n"
    "\n"
    "    ensure_enum_str_cache(type_handle, nullptr, desc);"
)
assert old_ts in content, "ToString pattern not found!"
content = content.replace(old_ts, new_ts, 1)
print("ToString fixed")

# ChaosEnumToStringRaw should have the same pattern but different variable name
old_tsr = (
    "    ensure_enum_type(desc);\n"
    "\n"
    "    ensure_enum_str_cache(type_handle, nullptr, desc);\n"
    "    auto cached = lookup_cached_enum_name(val);"
)
new_tsr = (
    "    if (check_enum_type(desc) == nullptr) return 0;\n"
    "\n"
    "    ensure_enum_str_cache(type_handle, nullptr, desc);\n"
    "    auto cached = lookup_cached_enum_name(val);"
)
assert old_tsr in content, "ToStringRaw pattern not found!"
content = content.replace(old_tsr, new_tsr, 1)
print("ToStringRaw fixed")

# ChaosEnumTryParse fallback
old_tp = (
    "    const auto* desc = resolve_type_arg(type);\n"
    "    if (desc == nullptr) return 0;\n"
    "    ensure_enum_type(desc);\n"
    "\n"
    "    const auto* field = find_field_by_name(desc, name_data, name_len);\n"
    "    if (field == nullptr) {\n"
    "        field = find_field_by_name_icase(desc, name_data, name_len);\n"
    "    }\n"
    "    if (field == nullptr) return 0;\n"
    "\n"
    "    auto boxed = enum_alloc_boxed_int64(field->constant_value);\n"
    "    std::memcpy(reinterpret_cast<void*>(result_out), &boxed, sizeof(boxed));\n"
    "    return 1;\n"
    "}\n"
    "\n"
    "/// Enum.TryParse(Type, String, Boolean, out Object)"
)
new_tp = (
    "    const auto* desc = resolve_type_arg(type);\n"
    "    if (desc == nullptr) return 0;\n"
    "    if (check_enum_type(desc) == nullptr) return 0;\n"
    "\n"
    "    const auto* field = find_field_by_name(desc, name_data, name_len);\n"
    "    if (field == nullptr) {\n"
    "        field = find_field_by_name_icase(desc, name_data, name_len);\n"
    "    }\n"
    "    if (field == nullptr) return 0;\n"
    "\n"
    "    auto boxed = enum_alloc_boxed_int64(field->constant_value);\n"
    "    std::memcpy(reinterpret_cast<void*>(result_out), &boxed, sizeof(boxed));\n"
    "    return 1;\n"
    "}\n"
    "\n"
    "/// Enum.TryParse(Type, String, Boolean, out Object)"
)
assert old_tp in content, "TryParse pattern not found!"
content = content.replace(old_tp, new_tp, 1)
print("TryParse fixed")

# ChaosEnumTryParseWithIgnoreCase fallback
old_tpic = (
    "    const auto* desc = resolve_type_arg(type);\n"
    "    if (desc == nullptr) return 0;\n"
    "    ensure_enum_type(desc);\n"
    "\n"
    "    const auto* field = find_field_by_name(desc, name_data, name_len);\n"
    "    if (field == nullptr && ignoreCase) {"
)
new_tpic = (
    "    const auto* desc = resolve_type_arg(type);\n"
    "    if (desc == nullptr) return 0;\n"
    "    if (check_enum_type(desc) == nullptr) return 0;\n"
    "\n"
    "    const auto* field = find_field_by_name(desc, name_data, name_len);\n"
    "    if (field == nullptr && ignoreCase) {"
)
assert old_tpic in content, "TryParseWithIgnoreCase pattern not found!"
content = content.replace(old_tpic, new_tpic, 1)
print("TryParseWithIgnoreCase fixed")

with open('D:/agent/booming-il2cpp/src/native/runtime-core/runtime_stubs/enum_stubs.cpp', 'w', encoding='utf-8') as f:
    f.write(content)

remaining = content.count('ensure_enum_type')
print(f"Remaining: {remaining}")
print("Done!")
