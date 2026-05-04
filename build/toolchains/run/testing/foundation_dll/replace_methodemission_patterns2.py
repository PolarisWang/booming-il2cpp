"""Fix remaining multi-line patterns in MethodEmission.cs."""
import re

filepath = 'D:/agent/booming-il2cpp/src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.MethodEmission.cs'

with open(filepath, 'r', encoding='utf-8') as f:
    content = f.read()

original = content

# ─── Pattern 1: header.type_id = GetNativeTypeIdSymbol split across 3 lines ───
# Old:
#   handler.AppendLiteral("...header.type_id = ");
#   handler.AppendFormatted(GetNativeTypeIdSymbol(X));
#   handler.AppendLiteral(";");
# New:
#   handler.AppendLiteral("...header.type_info = &");
#   handler.AppendFormatted(GetNativeTypeInfoSymbol(X));
#   handler.AppendLiteral(";");

# Match: literal ending with .type_id =
content = re.sub(
    r'(handler\.AppendLiteral\("[^"]*)header\.type_id = ("\);)\n(\t+\w+\.AppendFormatted\()GetNativeTypeIdSymbol\(',
    r'\1header.type_info = &\2\n\3GetNativeTypeInfoSymbol(',
    content
)

# ─── Pattern 2: chaos_boxed->header.type_id = GetNativeBoxTypeIdSymbol ───
content = re.sub(
    r'(handler\.AppendLiteral\("[^"]*)chaos_boxed->header\.type_id = ("\);)\n(\t+\w+\.AppendFormatted\()GetNativeBoxTypeIdSymbol\(',
    r'\1chaos_boxed->header.type_info = &\2\n\3GetNativeBoxTypeInfoSymbol(',
    content
)

# ─── Pattern 3: chaos_array->element_type_id = (AppendLiteral) ───
content = re.sub(
    r'(handler\.AppendLiteral\("[^"]*)element_type_id ("\);)\n(\t+\w+\.AppendFormatted\()GetRuntimeTypeIdExpression\(',
    r'\1element_type_info \2\n\3GetRuntimeTypeInfoExpression(',
    content
)

# ─── Pattern 4: chaos_does_type_implement_interface(chaos_header->type_id, GetNativeTypeIdSymbol( ───
content = re.sub(
    r'(handler\.AppendLiteral\("[^"]*)chaos_does_type_implement_interface\(chaos_header->type_id, ("\);)\n(\t+\w+\.AppendFormatted\()GetNativeTypeIdSymbol\(',
    r'\1chaos_does_type_implement_interface(chaos_header->type_info, &\2\n\3GetNativeTypeInfoSymbol(',
    content
)

# ─── Pattern 5: chaos_is_type_compatible(chaos_header->type_id, GetNativeTypeIdSymbol( ───
content = re.sub(
    r'(handler\.AppendLiteral\("[^"]*)chaos_is_type_compatible\(chaos_header->type_id, ("\);)\n(\t+\w+\.AppendFormatted\()GetNativeTypeIdSymbol\(',
    r'\1chaos_is_type_compatible(chaos_header->type_info, &\2\n\3GetNativeTypeInfoSymbol(',
    content
)

# ─── Pattern 6: chaos_header->type_id !=/== GetNativeTypeIdSymbol( ───
content = re.sub(
    r'(handler\.AppendLiteral\("[^"]*)chaos_header->type_id (==|!= )("\);)\n(\t+\w+\.AppendFormatted\()GetNativeTypeIdSymbol\(',
    r'\1chaos_header->type_info \2&\3\n\4GetNativeTypeInfoSymbol(',
    content
)

# ─── Pattern 7: chaos_matches = chaos_header->type_id == GetNativeTypeIdSymbol( ───
content = re.sub(
    r'(handler\.AppendLiteral\("[^"]*chaos_matches = )chaos_header->type_id == ("\);)\n(\t+\w+\.AppendFormatted\()GetNativeTypeIdSymbol\(',
    r'\1chaos_header->type_info == &\2\n\3GetNativeTypeInfoSymbol(',
    content
)

if content == original:
    print("ERROR: No changes made (remaining patterns not found)")
    # Debug: show what's around line 1390
    lines = original.split('\n')
    for i in range(1388, 1396):
        if i < len(lines):
            print(f"  {i+1}: {repr(lines[i])}")
else:
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(content)
    print("SUCCESS: Remaining multi-line patterns fixed")
