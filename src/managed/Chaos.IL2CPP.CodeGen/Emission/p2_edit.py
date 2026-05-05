import re

path = r"NativeAotLoweringPlanner.MethodEmission.cs"
with open(path, 'r', encoding='utf-8') as f:
    content = f.read()

# Debug: check exact indentation around the targets
def verify_anchor(name, search_str):
    idx = content.find(search_str)
    if idx >= 0:
        start = content.rfind('\n', 0, idx)
        line = content[start+1:content.find('\n', idx)]
        print(f"Anchor '{name}': {repr(line[:120])}")
    else:
        print(f"Anchor '{name}': NOT FOUND")

verify_anchor("chaos_ts_", "chaos_ts_")
verify_anchor("Native call", "// Native call")
verify_anchor("chaos_marshal_{idx}", "chaos_marshal_{idx}));\")")
verify_anchor("String return", "String return: convert")

# 1. Add struct copy declarations after TLS section
# Based on debug output: builder at 3 tabs, closing } at 2 tabs, next line at 2 tabs
old1 = (
    '\t\t\tbuilder.AppendLine("    auto* chaos_ts_ = ::chaos::il2cpp::runtime_core::GetCurrentThreadState();");\n'
    '\t\t}\n'
    '\n'
    '\t\tbuilder.AppendLine("    if (s_pinvoke_fn_ == nullptr)");'
)

new1 = (
    '\t\t\tbuilder.AppendLine("    auto* chaos_ts_ = ::chaos::il2cpp::runtime_core::GetCurrentThreadState();");\n'
    '\t\t}\n'
    '\n'
    '\t\t// Stack-local copies for non-blittable struct parameters with string fields.\n'
    '\t\tif (hasSimpleNonBlittableStructParams)\n'
    '\t\t{\n'
    '\t\t\tforeach (int idx in method.SimpleNonBlittableStructParameterIndices!)\n'
    '\t\t\t{\n'
    '\t\t\t\tbuilder.AppendLine($"    auto chaos_struct_copy_{idx} = chaos_arg_{idx};");\n'
    '\t\t\t}\n'
    '\t\t}\n'
    '\n'
    '\t\tbuilder.AppendLine("    if (s_pinvoke_fn_ == nullptr)");'
)

if old1 in content:
    content = content.replace(old1, new1, 1)
    print("Edit 1 OK")
else:
    print("Edit 1 FAILED - old string not found")
    # Debug
    # Find the anchor
    idx = content.find("auto* chaos_ts_")
    if idx >= 0:
        context = content[idx-30:idx+300]
        print("Context:", repr(context))

# 2. Add pre-call after string marshal pre-call
old2 = (
    '\t\t\t\t\tbuilder.AppendLine("    }");\n'
    '\t\t\t\t}\n'
    '\t\t\t}\n'
    '\n'
    '\t\t\t// Native call'
)

new2 = (
    '\t\t\t\t\tbuilder.AppendLine("    }");\n'
    '\t\t\t\t}\n'
    '\t\t\t}\n'
    '\n'
    '\t\t\t// Pre-call: marshal string fields in non-blittable struct copies to native UTF-8.\n'
    '\t\t\tif (hasSimpleNonBlittableStructParams)\n'
    '\t\t\t{\n'
    '\t\t\t\tint fieldGroupIdx = 0;\n'
    '\t\t\t\tforeach (int paramIdx in method.SimpleNonBlittableStructParameterIndices!)\n'
    '\t\t\t\t{\n'
    '\t\t\t\t\tvar stringFields = method.SimpleNonBlittableStructStringFieldSubjectIds![fieldGroupIdx];\n'
    '\t\t\t\t\tforeach (string fieldSubjectId in stringFields)\n'
    '\t\t\t\t\t{\n'
    '\t\t\t\t\t\tstring fieldMember = GetNativeFieldMemberName(fieldSubjectId);\n'
    '\t\t\t\t\t\tbuilder.AppendLine($"    if (chaos_struct_copy_{paramIdx}.{fieldMember} != 0)");\n'
    '\t\t\t\t\t\tbuilder.AppendLine("    {");\n'
    '\t\t\t\t\t\tbuilder.AppendLine($"        auto* chaos_marshal_str_ = ::chaos::il2cpp::runtime_core::MarshalStringToCoTaskMemUtf8(");\n'
    '\t\t\t\t\t\tbuilder.AppendLine($"            chaos_rs_, chaos_ts_, reinterpret_cast<void*>(chaos_struct_copy_{paramIdx}.{fieldMember}));");\n'
    '\t\t\t\t\t\tbuilder.AppendLine($"        chaos_struct_copy_{paramIdx}.{fieldMember} = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_marshal_str_);");\n'
    '\t\t\t\t\t\tbuilder.AppendLine("    }");\n'
    '\t\t\t\t\t}\n'
    '\t\t\t\t\tfieldGroupIdx++;\n'
    '\t\t\t\t}\n'
    '\t\t\t}\n'
    '\n'
    '\t\t\t// Native call'
)

if old2 in content:
    content = content.replace(old2, new2, 1)
    print("Edit 2 OK")
else:
    print("Edit 2 FAILED")
    idx = content.find("// Native call")
    if idx >= 0:
        context = content[idx-200:idx+50]
        print("Context for Native call:", repr(context))

# 3. Add post-call cleanup before String return
old3 = (
    '\t\t\t\tbuilder.AppendLine($"            chaos_rs_, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_marshal_{idx}));");\n'
    '\t\t\t\tbuilder.AppendLine("    }");\n'
    '\t\t\t}\n'
    '\t\t}\n'
    '\n'
    '\t\t// String return: convert native char* to managed string.\n'
    '\t\tif (hasStringReturn)\n'
)

new3 = (
    '\t\t\t\tbuilder.AppendLine($"            chaos_rs_, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_marshal_{idx}));");\n'
    '\t\t\t\tbuilder.AppendLine("    }");\n'
    '\t\t\t}\n'
    '\t\t}\n'
    '\n'
    '\t\t\t// Free CoTaskMem buffers in non-blittable struct copies.\n'
    '\t\t\tif (hasSimpleNonBlittableStructParams)\n'
    '\t\t\t{\n'
    '\t\t\t\tint fieldGroupIdx = 0;\n'
    '\t\t\t\tforeach (int paramIdx in method.SimpleNonBlittableStructParameterIndices!)\n'
    '\t\t\t\t{\n'
    '\t\t\t\t\tvar stringFields = method.SimpleNonBlittableStructStringFieldSubjectIds![fieldGroupIdx];\n'
    '\t\t\t\t\tforeach (string fieldSubjectId in stringFields)\n'
    '\t\t\t\t\t{\n'
    '\t\t\t\t\t\tstring fieldMember = GetNativeFieldMemberName(fieldSubjectId);\n'
    '\t\t\t\t\t\tbuilder.AppendLine($"    if (chaos_struct_copy_{paramIdx}.{fieldMember} != 0)");\n'
    '\t\t\t\t\t\tbuilder.AppendLine("    {");\n'
    '\t\t\t\t\t\tbuilder.AppendLine($"        ::chaos::il2cpp::runtime_core::MarshalFreeCoTaskMem(");\n'
    '\t\t\t\t\t\tbuilder.AppendLine($"            chaos_rs_, chaos_struct_copy_{paramIdx}.{fieldMember});");\n'
    '\t\t\t\t\t\tbuilder.AppendLine("    }");\n'
    '\t\t\t\t\t}\n'
    '\t\t\t\t\tfieldGroupIdx++;\n'
    '\t\t\t\t}\n'
    '\t\t\t}\n'
    '\n'
    '\t\t// String return: convert native char* to managed string.\n'
    '\t\tif (hasStringReturn)\n'
)

if old3 in content:
    content = content.replace(old3, new3, 1)
    print("Edit 3 OK")
else:
    print("Edit 3 FAILED")

with open(path, 'w', encoding='utf-8') as f:
    f.write(content)

print("Script done")
