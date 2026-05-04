"""Replace type_id references in ObjectEqualityEmission.cs with TypeInfo* equivalents."""

filepath = 'D:/agent/booming-il2cpp/src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.ObjectEqualityEmission.cs'

with open(filepath, 'r', encoding='utf-8') as f:
    content = f.read()

original = content

# Pattern 1: type_id != type_id pointer comparison (single line)
content = content.replace(
    'builder.AppendLine("    if (chaos_left_header->type_id != chaos_right_header->type_id)");',
    'builder.AppendLine("    if (chaos_left_header->type_info != chaos_right_header->type_info)");'
)

# Pattern 2: multi-line type_id == with GetNativeTypeIdSymbol
old_block = (
    '\t\t\thandler.AppendLiteral("    if (chaos_left_header->type_id == ");\n'
    '\t\t\thandler.AppendFormatted(GetNativeTypeIdSymbol("System.Private.CoreLib/System.String"));\n'
    '\t\t\thandler.AppendLiteral(")");'
)
new_block = (
    '\t\t\thandler.AppendLiteral("    if (chaos_left_header->type_info == &");\n'
    '\t\t\thandler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.String"));\n'
    '\t\t\thandler.AppendLiteral(")");'
)
content = content.replace(old_block, new_block)

# Pattern 3: switch on type_id
content = content.replace(
    'builder.AppendLine("    switch (chaos_left_header->type_id)");',
    'builder.AppendLine("    switch (chaos_left_header->type_info->stable_id)");'
)

if content == original:
    print("ERROR: No changes made to ObjectEqualityEmission.cs")
    lines = original.split('\n')
    for i in [51, 52, 61, 62, 63, 94, 95]:
        if i < len(lines):
            print(f"  {i+1}: {repr(lines[i])}")
else:
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(content)
    print("SUCCESS: ObjectEqualityEmission.cs updated")
