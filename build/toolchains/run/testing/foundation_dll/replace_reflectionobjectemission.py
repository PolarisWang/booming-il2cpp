"""Replace all type_id references in ReflectionObjectEmission.cs with TypeInfo* equivalents."""

filepath = 'D:/agent/booming-il2cpp/src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.ReflectionObjectEmission.cs'

with open(filepath, 'r', encoding='utf-8') as f:
    content = f.read()

original = content

# ─── Pattern A: header.type_id = assignments ───

# A1: builder.Append pair (line ~157-159) — 3-tab indent
content = content.replace(
    '\t\t\tbuilder.Append("    chaos_string->header.type_id = ");\n'
    '\t\t\tbuilder.Append(GetNativeTypeIdSymbol("System.Private.CoreLib/System.String"));',
    '\t\t\tbuilder.Append("    chaos_string->header.type_info = &");\n'
    '\t\t\tbuilder.Append(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.String"));'
)

# A2: handler.AppendLiteral patterns — single-line replacements
replacements_literal = [
    ('handler.AppendLiteral("    chaos_type->header.type_id = ");',
     'handler.AppendLiteral("    chaos_type->header.type_info = &");'),
    ('handler.AppendLiteral("    chaos_method->header.type_id = ");',
     'handler.AppendLiteral("    chaos_method->header.type_info = &");'),
    ('handler.AppendLiteral("    chaos_closed_method->header.type_id = ");',
     'handler.AppendLiteral("    chaos_closed_method->header.type_info = &");'),
    ('handler.AppendLiteral("    chaos_assembly_name->header.type_id = ");',
     'handler.AppendLiteral("    chaos_assembly_name->header.type_info = &");'),
    ('handler.AppendLiteral("            chaos_assembly->header.type_id = ");',
     'handler.AppendLiteral("            chaos_assembly->header.type_info = &");'),
    ('handler.AppendLiteral("                chaos_field->header.type_id = ");',
     'handler.AppendLiteral("                chaos_field->header.type_info = &");'),
    ('handler.AppendLiteral("        chaos_attribute->header.type_id = ");',
     'handler.AppendLiteral("        chaos_attribute->header.type_info = &");'),
    ('handler.AppendLiteral("->header.type_id = ");',
     'handler.AppendLiteral("->header.type_info = &");'),
    ('handler.AppendLiteral("            chaos_boxed_result->header.type_id = ");',
     'handler.AppendLiteral("            chaos_boxed_result->header.type_info = &");'),
    ('handler.AppendLiteral("    chaos_array->element_type_id = ");',
     'handler.AppendLiteral("    chaos_array->element_type_info = ");'),
]
for old, new in replacements_literal:
    content = content.replace(old, new)

# ─── Pattern A AppendFormatted: GetNativeTypeIdSymbol → GetNativeTypeInfoSymbol ───
# These follow the AppendLiteral patterns above
formatted_replacements = [
    ('handler.AppendFormatted(GetNativeTypeIdSymbol("System.Private.CoreLib/System.String"));',
     'handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.String"));'),
    ('handler.AppendFormatted(GetNativeTypeIdSymbol("System.Private.CoreLib/System.Type"));',
     'handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Type"));'),
    ('handler.AppendFormatted(GetNativeTypeIdSymbol("System.Private.CoreLib/System.Reflection.MethodInfo"));',
     'handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Reflection.MethodInfo"));'),
    ('handler.AppendFormatted(GetNativeTypeIdSymbol("System.Private.CoreLib/System.Reflection.ConstructorInfo"));',
     'handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Reflection.ConstructorInfo"));'),
    ('handler.AppendFormatted(GetNativeTypeIdSymbol("System.Private.CoreLib/System.Reflection.FieldInfo"));',
     'handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Reflection.FieldInfo"));'),
    ('handler.AppendFormatted(GetNativeTypeIdSymbol("System.Private.CoreLib/System.Reflection.ParameterInfo"));',
     'handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Reflection.ParameterInfo"));'),
    ('handler.AppendFormatted(GetNativeTypeIdSymbol("System.Private.CoreLib/System.Reflection.Assembly"));',
     'handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Reflection.Assembly"));'),
    ('handler.AppendFormatted(GetNativeTypeIdSymbol("System.Private.CoreLib/System.Reflection.AssemblyName"));',
     'handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Reflection.AssemblyName"));'),
    ('handler.AppendFormatted(GetNativeTypeIdSymbol(item15.AttributeTypeSubjectId));',
     'handler.AppendFormatted(GetNativeTypeInfoSymbol(item15.AttributeTypeSubjectId));'),
    ('handler.AppendFormatted(GetNativeBoxTypeIdSymbol(item14));',
     'handler.AppendFormatted(GetNativeBoxTypeInfoSymbol(item14));'),
]
for old, new in formatted_replacements:
    content = content.replace(old, new)

# ─── Pattern B: chaos_array->header.type_id = chaos_type_id_managed_array ───
content = content.replace(
    'builder.AppendLine("    chaos_array->header.type_id = chaos_type_id_managed_array;");',
    'builder.AppendLine("    chaos_array->header.type_info = &chaos_type_info_managed_array;");'
)

# ─── Pattern C: chaos_header->type_id == GetNativeTypeIdSymbol(X) comparisons ───
# Replace the literal (the AppendFormatted is handled by the generic type_id→type_info replacements above)
content = content.replace(
    'handler.AppendLiteral("    if (chaos_header->type_id == ");',
    'handler.AppendLiteral("    if (chaos_header->type_info == &");'
)
content = content.replace(
    'handler.AppendLiteral("    else if (chaos_header->type_id == ");',
    'handler.AppendLiteral("    else if (chaos_header->type_info == &");'
)

# ─── Pattern E: element_type_id in struct init ───
content = content.replace(
    'chaos_array->element_type_id = chaos_element_type_id;',
    'chaos_array->element_type_info = chaos_element_type_info;'
)

# ─── Catch any remaining GetNativeTypeIdSymbol calls that should be GetNativeTypeInfoSymbol ───
# (but NOT the [Obsolete] GetRuntimeTypeIdExpression which delegates correctly)
content = content.replace(
    'builder.Append(GetNativeTypeIdSymbol("System.Private.CoreLib/System.String"));',
    'builder.Append(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.String"));'
)

if content == original:
    print("ERROR: No changes made to ReflectionObjectEmission.cs")
    # Show first few remaining type_id patterns
    import re
    for m in re.finditer(r'\.type_id[^_\w]|->type_id[^_\w]|header\.type_id', original):
        line_start = original.rfind('\n', 0, m.start()) + 1
        line_end = original.find('\n', m.start())
        print(f"  Remaining: {repr(original[line_start:line_end].rstrip())}")
else:
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(content)
    print("SUCCESS: ReflectionObjectEmission.cs updated")
