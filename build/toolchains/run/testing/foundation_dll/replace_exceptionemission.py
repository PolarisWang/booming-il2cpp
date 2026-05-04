"""Replace type_id references in ExceptionEmission.cs with TypeInfo* equivalents."""

filepath = 'D:/agent/booming-il2cpp/src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.ExceptionEmission.cs'

with open(filepath, 'r', encoding='utf-8') as f:
    content = f.read()

original = content

# Pattern 1a: catch block type compatibility check (line ~49)
content = content.replace(
    'handler.AppendLiteral("        if (!chaos_is_type_compatible(chaos_header->type_id, ");\n'
    '\t\thandler.AppendFormatted(GetNativeTypeIdSymbol(catchOnlyShape.ExceptionRegion.CatchTypeSubjectId));\n'
    '\t\thandler.AppendLiteral("))");',
    'handler.AppendLiteral("        if (!chaos_is_type_compatible(chaos_header->type_info, &");\n'
    '\t\thandler.AppendFormatted(GetNativeTypeInfoSymbol(catchOnlyShape.ExceptionRegion.CatchTypeSubjectId));\n'
    '\t\thandler.AppendLiteral("))");'
)

# Pattern 1b: catch-and-finally type compatibility check (line ~218)
content = content.replace(
    'handler.AppendLiteral("            if (!chaos_is_type_compatible(chaos_header->type_id, ");\n'
    '\t\thandler.AppendFormatted(GetNativeTypeIdSymbol(catchAndFinallyShape.CatchRegion.CatchTypeSubjectId));\n'
    '\t\thandler.AppendLiteral("))");',
    'handler.AppendLiteral("            if (!chaos_is_type_compatible(chaos_header->type_info, &");\n'
    '\t\thandler.AppendFormatted(GetNativeTypeInfoSymbol(catchAndFinallyShape.CatchRegion.CatchTypeSubjectId));\n'
    '\t\thandler.AppendLiteral("))");'
)

# Pattern 2: string interpolation with header.type_id (3 occurrences)
content = content.replace(
    'builder.AppendLine($"{indentation}    chaos_object->header.type_id = {GetNativeTypeIdSymbol(requiredTargetReference.SubjectId)};");',
    'builder.AppendLine($"{indentation}    chaos_object->header.type_info = &{GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId)};");'
)

# Pattern 3: chaos_string->header.type_id (line ~954)
content = content.replace(
    'handler.AppendLiteral("    chaos_string->header.type_id = ");',
    'handler.AppendLiteral("    chaos_string->header.type_info = &");'
)

if content == original:
    print("ERROR: No changes made to ExceptionEmission.cs")
    # Debug: show what's around line 49
    lines = original.split('\n')
    for i in range(48, 53):
        if i < len(lines):
            print(f"  {i+1}: {repr(lines[i])}")
    for i in range(217, 222):
        if i < len(lines):
            print(f"  {i+1}: {repr(lines[i])}")
    for i in range(658, 662):
        if i < len(lines):
            print(f"  {i+1}: {repr(lines[i])}")
    for i in range(710, 722):
        if i < len(lines):
            print(f"  {i+1}: {repr(lines[i])}")
    for i in range(953, 956):
        if i < len(lines):
            print(f"  {i+1}: {repr(lines[i])}")
else:
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(content)
    print("SUCCESS: ExceptionEmission.cs updated")
