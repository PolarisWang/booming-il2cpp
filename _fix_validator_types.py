"""Replace bare cstdint types in C# string literals that become C++ generated code.
Only targets confirmed string-literal occurrences to avoid modifying C# language constructs."""
import sys

def replace_in_file(filepath, patterns):
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()

    original = content
    for old, new in patterns:
        content = content.replace(old, new)

    if content != original:
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(content)
        return True
    return False

# ── GenericRegistration.cs ──
reg_patterns = [
    ('uint32_t kGenericTypeArgTokens', 'CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens'),
    ('uint32_t kGenericMethodArgTokens', 'CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens'),
    ('uint32_t s_method_aot_entry_args', 'CHAOS_IL2CPP_UINT32 s_method_aot_entry_args'),
    ('const uint32_t s_register_method_aot', 'const CHAOS_IL2CPP_UINT32 s_register_method_aot'),
    ('uint32_t* out_type_count', 'CHAOS_IL2CPP_UINT32* out_type_count'),
    ('const uint32_t** out_type_args', 'const CHAOS_IL2CPP_UINT32** out_type_args'),
    ('uint32_t* out_type_arg_count', 'CHAOS_IL2CPP_UINT32* out_type_arg_count'),
    ('uint32_t* out_method_count', 'CHAOS_IL2CPP_UINT32* out_method_count'),
    ('const uint32_t** out_method_args', 'const CHAOS_IL2CPP_UINT32** out_method_args'),
    ('uint32_t* out_method_arg_count', 'CHAOS_IL2CPP_UINT32* out_method_arg_count'),
    ('    uint32_t*, const GenericTypeRegistrationEntryV0**, const uint32_t**, uint32_t*,\n    uint32_t*, const GenericMethodRegistrationEntryV0**, const uint32_t**, uint32_t*);',
     '    CHAOS_IL2CPP_UINT32*, const GenericTypeRegistrationEntryV0**, const CHAOS_IL2CPP_UINT32**, CHAOS_IL2CPP_UINT32*,\n    CHAOS_IL2CPP_UINT32*, const GenericMethodRegistrationEntryV0**, const CHAOS_IL2CPP_UINT32**, CHAOS_IL2CPP_UINT32*);'),
]

# ── ModuleRegistration.cs ──
mod_patterns = [
    ('uint32_t s_type_flags', 'CHAOS_IL2CPP_UINT32 s_type_flags'),
    ('uint32_t s_type_parent_tokens', 'CHAOS_IL2CPP_UINT32 s_type_parent_tokens'),
    ('uint32_t s_nested_type_children', 'CHAOS_IL2CPP_UINT32 s_nested_type_children'),
    ('uint32_t s_nested_type_offset', 'CHAOS_IL2CPP_UINT32 s_nested_type_offset'),
    ('uint32_t s_generic_param_constraint_data', 'CHAOS_IL2CPP_UINT32 s_generic_param_constraint_data'),
    ('uint32_t s_generic_param_constraint_offset', 'CHAOS_IL2CPP_UINT32 s_generic_param_constraint_offset'),
    ('const uint32_t s_native_aot_module_id', 'const CHAOS_IL2CPP_UINT32 s_native_aot_module_id'),
    ('const uint32_t s_name_index_registered', 'const CHAOS_IL2CPP_UINT32 s_name_index_registered'),
    ('const uint32_t s_reverse_pinvoke_registered', 'const CHAOS_IL2CPP_UINT32 s_reverse_pinvoke_registered'),
    ('uint32_t s_custom_attribute_offset', 'CHAOS_IL2CPP_UINT32 s_custom_attribute_offset'),
    ('uint32_t attr_type_token, const uint8_t* field_data',
     'CHAOS_IL2CPP_UINT32 attr_type_token, const CHAOS_IL2CPP_UINT8* field_data'),
    ('int32_t __v; std::memcpy(&__v, p, 4); p += 4; attr->{fieldName} = __v;',
     'CHAOS_IL2CPP_INT32 __v; std::memcpy(&__v, p, 4); p += 4; attr->{fieldName} = __v;'),
    ('int64_t __v; std::memcpy(&__v, p, 8); p += 8; attr->{fieldName} = __v;',
     'CHAOS_IL2CPP_INT64 __v; std::memcpy(&__v, p, 8); p += 8; attr->{fieldName} = __v;'),
    ('uint32_t __v; std::memcpy(&__v, p, 4); p += 4; attr->{fieldName} = __v;',
     'CHAOS_IL2CPP_UINT32 __v; std::memcpy(&__v, p, 4); p += 4; attr->{fieldName} = __v;'),
    ('uint64_t __v; std::memcpy(&__v, p, 8); p += 8; attr->{fieldName} = __v;',
     'CHAOS_IL2CPP_UINT64 __v; std::memcpy(&__v, p, 8); p += 8; attr->{fieldName} = __v;'),
]

# ── NativeAotLoweringPlanner.cs ──
ncs_patterns = [
    ('const uint32_t s_method_table_init', 'const CHAOS_IL2CPP_UINT32 s_method_table_init'),
]

# Run replacements
files = [
    (r'D:\agent\booming-il2cpp\src\managed\Chaos.IL2CPP.CodeGen\Emission\NativeAotLoweringPlanner.GenericRegistration.cs', reg_patterns),
    (r'D:\agent\booming-il2cpp\src\managed\Chaos.IL2CPP.CodeGen\Emission\NativeAotLoweringPlanner.ModuleRegistration.cs', mod_patterns),
    (r'D:\agent\booming-il2cpp\src\managed\Chaos.IL2CPP.CodeGen\NativeAotLoweringPlanner.cs', ncs_patterns),
]

count = 0
for filepath, patterns in files:
    if replace_in_file(filepath, patterns):
        print(f'  MODIFIED: {filepath}')
        count += 1
    else:
        print(f'  unchanged: {filepath}')

print(f'\nTotal files modified: {count}')
