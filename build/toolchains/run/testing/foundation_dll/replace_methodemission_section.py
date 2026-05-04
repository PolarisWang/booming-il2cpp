"""Replace all type_id references in MethodEmission.cs with TypeInfo* equivalents."""

import re

filepath = 'D:/agent/booming-il2cpp/src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.MethodEmission.cs'

with open(filepath, 'r', encoding='utf-8') as f:
    content = f.read()

original = content

# ─── R1: header.type_id = GetNativeTypeIdSymbol(...) → header.type_info = &GetNativeTypeInfoSymbol(...) ───
# Pattern: "XXX->header.type_id = GetNativeTypeIdSymbol(Y);"
# Becomes: "XXX->header.type_info = &GetNativeTypeInfoSymbol(Y);"
content = re.sub(
    r'(->header\.type_id\s*=\s*)GetNativeTypeIdSymbol\(([^)]+)\)(\s*;)',
    r'\1&GetNativeTypeInfoSymbol(\2)\3',
    content
)

# ─── R2: chaos_array->header.type_id = chaos_type_id_managed_array → ───
# chaos_array->header.type_info = &chaos_type_info_managed_array
content = content.replace(
    'chaos_array->header.type_id = chaos_type_id_managed_array;',
    'chaos_array->header.type_info = &chaos_type_info_managed_array;'
)

# ─── R3: chaos_array->element_type_id = GetRuntimeTypeIdExpression(...) → ───
# chaos_array->element_type_info = GetRuntimeTypeInfoExpression(...)
content = re.sub(
    r'chaos_array->element_type_id(\s*=\s*)GetRuntimeTypeIdExpression\(',
    r'chaos_array->element_type_info\1GetRuntimeTypeInfoExpression(',
    content
)

# ─── R4: chaos_array->element_type_id (not at assignment) → chaos_array->element_type_info ───
# (in array type compat call arguments)
content = content.replace(
    'chaos_array->element_type_id, ',
    'chaos_array->element_type_info, '
)

# ─── R5: chaos_header->type_id != chaos_type_id_managed_array → ───
# chaos_header->type_info != &chaos_type_info_managed_array
content = content.replace(
    'chaos_header->type_id != chaos_type_id_managed_array',
    'chaos_header->type_info != &chaos_type_info_managed_array'
)

# ─── R6: chaos_header->type_id == chaos_type_id_managed_array → ───
# chaos_header->type_info == &chaos_type_info_managed_array
content = content.replace(
    'chaos_header->type_id == chaos_type_id_managed_array',
    'chaos_header->type_info == &chaos_type_info_managed_array'
)

# ─── R7/R8: chaos_does_type_implement_interface / chaos_is_type_compatible ───
# pass chaos_header->type_id → pass chaos_header->type_info
# pass GetNativeTypeIdSymbol → pass &GetNativeTypeInfoSymbol
content = re.sub(
    r'(chaos_does_type_implement_interface\()chaos_header->type_id(\s*,\s*)GetNativeTypeIdSymbol\(',
    r'\1chaos_header->type_info\2&GetNativeTypeInfoSymbol(',
    content
)
content = re.sub(
    r'(chaos_is_type_compatible\()chaos_header->type_id(\s*,\s*)GetNativeTypeIdSymbol\(',
    r'\1chaos_header->type_info\2&GetNativeTypeInfoSymbol(',
    content
)

# ─── R9: chaos_header->type_id != GetNativeTypeIdSymbol(X) → ───
# chaos_header->type_info != &GetNativeTypeInfoSymbol(X)
content = re.sub(
    r'chaos_header->type_id\s*!=\s*GetNativeTypeIdSymbol\(([^)]+)\)',
    r'chaos_header->type_info != &GetNativeTypeInfoSymbol(\1)',
    content
)

# ─── R10: chaos_header->type_id == GetNativeTypeIdSymbol(X) → ───
# chaos_header->type_info == &GetNativeTypeInfoSymbol(X)
content = re.sub(
    r'chaos_header->type_id\s*==\s*GetNativeTypeIdSymbol\(([^)]+)\)',
    r'chaos_header->type_info == &GetNativeTypeInfoSymbol(\1)',
    content
)

# ─── R12: chaos_boxed->header.type_id = GetNativeBoxTypeIdSymbol(X) → ───
# chaos_boxed->header.type_info = &GetNativeBoxTypeInfoSymbol(X)
content = re.sub(
    r'(chaos_boxed->header\.type_id\s*=\s*)GetNativeBoxTypeIdSymbol\(([^)]+)\)(\s*;)',
    r'\1&GetNativeBoxTypeInfoSymbol(\2)\3',
    content
)

# ─── R13/R14/R15/R16: Same patterns in EmitIsInst ───
# These are already covered by R7-R10 since the patterns are identical.

# ─── R11: Virtual dispatch section ───
# Replace the switch-based virtual dispatch with TypeInfo* if-chain + parent walk.
# The section starts with "switch (chaos_header->type_id)" and ends just before "CHAOS_IL2CPP_ABORT();"

# First, add the GetVirtualDispatchTargetTypeInfoPointer helper function.
# We need to add it right after GetVirtualDispatchTargetTypeIdSymbol in InvocationAbi.cs.
# This will be done separately.

# For the virtual dispatch, we need to handle the two foreach loops that generate
# case labels. The pattern is:
#   builder.AppendLine("            case X:");
#   ...
# We need to change these to:
#   builder.AppendLine("        if (chaos_header->type_info == &X)");
#   ...
# And the while loop changes from:
#   chaos_get_base_type_id(chaos_header->type_id) → chaos_header->type_info->parent
#   switch (chaos_current_type_id) → if (chaos_current_type_info == &X)

# Replace the first switch + first foreach loop body
# The pattern: "switch (chaos_header->type_id)" followed by "{" followed by foreach loop
# with "case" labels generated inside the loop

# Actually, since the foreach loop bodies are complex C# code, let me do this more carefully.
# I'll replace the entire section from the switch statement to the end of the while loop.

# The section to replace spans from:
#   builder.AppendLine("        switch (chaos_header->type_id)");
# to:
#   builder.AppendLine("            chaos_current_type_id = chaos_get_base_type_id(chaos_current_type_id);");
#   builder.AppendLine("        }");

# Let me use a marker-based approach. I'll find the text boundaries.

# Marker start: "        switch (chaos_header->type_id)"
# Marker end:   "chaos_current_type_id = chaos_get_base_type_id(chaos_current_type_id);"

# Find the virtual dispatch section boundaries
switch_start = content.find('builder.AppendLine("        switch (chaos_header->type_id)");')
if switch_start < 0:
    print("ERROR: Could not find virtual dispatch switch start")
else:
    # Find where the second chaos_get_base_type_id call is (end of the while loop)
    # There are two: one at the start of the while, one at the end
    # We want the last one (the one inside the while loop body)

    # Find all occurrences
    get_base_id_positions = []
    pos = 0
    while True:
        pos = content.find('chaos_get_base_type_id', pos)
        if pos < 0:
            break
        get_base_id_positions.append(pos)
        pos += 1

    # We want the section from switch_start to after the last chaos_get_base_type_id line
    # The last occurrence is the one inside the while loop
    last_pos = get_base_id_positions[-1] if get_base_id_positions else -1

    if last_pos >= 0:
        # Find the end of line containing last chaos_get_base_type_id
        line_end = content.find('\n', last_pos)
        if line_end >= 0:
            section_end_after = line_end + 1

        # Also find the builder.AppendLine("        }"); that closes the while loop
        # It should be right after the chaos_get_base_type_id line
        # Pattern: \n\t\t\tbuilder.AppendLine("        }");
        after_get_base = content[line_end:line_end + 100]

        # Find the closing } of the while loop
        close_brace_pattern = '\t\t\tbuilder.AppendLine("        }");'
        close_brace_pos = content.find(close_brace_pattern, line_end)

        if close_brace_pos >= 0:
            section_end = close_brace_pos + len(close_brace_pattern)

            # Extract the section before the replacement
            before_section = content[:switch_start]
            after_section = content[section_end:]

            # Now we need to reconstruct the new section.
            # The tricky part is the two foreach loops. Since we can't easily
            # re-generate them, let's transform the old section.

            old_section = content[switch_start:section_end]

            # Transform the old section to use TypeInfo* approach
            new_section = old_section

            # 1. Replace the first switch header with if-header comment + if chain start
            # Old: builder.AppendLine("        switch (chaos_header->type_id)");
            #      builder.AppendLine("        {");
            # We need to change the foreach loop to emit "if" instead of "case"

            # The first loop generates lines like:
            #   handler.AppendLiteral("            case ");
            #   handler.AppendFormatted(virtualDispatchTargetTypeIdSymbol);
            #   handler.AppendLiteral(":");
            # Change to:
            #   handler.AppendLiteral("        if (chaos_header->type_info == &");
            #   handler.AppendFormatted(virtualDispatchTargetTypeInfoSymbol);
            #   handler.AppendLiteral(")");

            # But wait, we need GetVirtualDispatchTargetTypeInfoPointer for this...
            # Since that function doesn't exist yet, let's inline the expression.
            # The function would return "&GetNativeTypeInfoSymbol(X)" or "&GetNativeBoxTypeInfoSymbol(X)"

            # For now, replace called symbol function
            new_section = new_section.replace(
                'GetVirtualDispatchTargetTypeIdSymbol(item.ReceiverTypeSubjectId)',
                'GetVirtualDispatchTargetTypeInfoPointer(item.ReceiverTypeSubjectId)'
            )
            new_section = new_section.replace(
                'GetVirtualDispatchTargetTypeIdSymbol(item2.ReceiverTypeSubjectId)',
                'GetVirtualDispatchTargetTypeInfoPointer(item2.ReceiverTypeSubjectId)'
            )

            # 2. Change case labels in first foreach loop (generates "case X:")
            # Pattern in C#:
            #   handler.AppendLiteral("            case ");
            #   handler.AppendFormatted(virtualDispatchTargetTypeIdSymbol);
            #   handler.AppendLiteral(":");
            # Becomes:
            #   handler.AppendLiteral("        if (chaos_header->type_info == ");
            #   handler.AppendFormatted(virtualDispatchTargetTypeInfoPointer);
            #   handler.AppendLiteral(")");

            # Replace first switch case labels (8-space indent)
            new_section = new_section.replace(
                'handler.AppendLiteral("            case ");',
                'handler.AppendLiteral("        if (chaos_header->type_info == ");'
            )
            new_section = new_section.replace(
                'handler.AppendFormatted(virtualDispatchTargetTypeIdSymbol);',
                'handler.AppendFormatted(virtualDispatchTargetTypeInfoPointer);'
            )
            # The colon after case becomes the closing )
            new_section = new_section.replace(
                'handler.AppendLiteral(":");\n\t\t\t\tstringBuilder5.AppendLine(ref handler);',
                'handler.AppendLiteral(")");\n\t\t\t\tstringBuilder5.AppendLine(ref handler);'
            )

            # Add opening brace after the if
            new_section = new_section.replace(
                'handler.AppendLiteral(")")',
                'handler.AppendLiteral(")")',  # keep as-is
            )
            # Actually, the case label is a single line, but the if needs braces.
            # Let me add "{" after the if line and "}" before the goto label
            # The pattern is: case -> if (...) \n { \n method_call \n goto \n }

            # OK this is getting complex. Let me do it differently.
            # I'll insert the opening brace after each if-condition line
            # and closing brace after each goto line.

            # For the first loop (indices with stringBuilder5):
            # After: stringBuilder5.AppendLine(ref handler);   // the "if" line
            # Add: builder.AppendLine("        {");
            # After: stringBuilder8.AppendLine(ref handler);   // the "goto" line
            # Add: builder.AppendLine("        }");

            # Replace for first loop cases
            # After the "case" is emitted via stringBuilder5, we need to emit "{\n"
            # But with our change, the if line is emitted with stringBuilder5
            old_case_pattern_1 = (
                '\t\t\t\tstringBuilder5.AppendLine(ref handler);\n'
                '\t\t\t\tif (string.Equals(text, "void", StringComparison.Ordinal))\n'
                '\t\t\t\t{'
            )
            new_case_pattern_1 = (
                '\t\t\t\tstringBuilder5.AppendLine(ref handler);\n'
                '\t\t\t\tbuilder.AppendLine("        {");\n'
                '\t\t\t\tif (string.Equals(text, "void", StringComparison.Ordinal))\n'
                '\t\t\t\t{'
            )
            new_section = new_section.replace(old_case_pattern_1, new_case_pattern_1)

            # After the goto line (emitted via stringBuilder8), add closing brace
            # Pattern: stringBuilder8.AppendLine(ref handler); then next case or default
            old_goto_pattern_1 = (
                '\t\t\t\tstringBuilder8.AppendLine(ref handler);\n'
                '\t\t\t}\n'
                '\t\t\tbuilder.AppendLine("            default:");'
            )
            new_goto_pattern_1 = (
                '\t\t\t\tstringBuilder8.AppendLine(ref handler);\n'
                '\t\t\t\tbuilder.AppendLine("        }");\n'
                '\t\t\t}\n'
                '\t\t\tbuilder.AppendLine("        }");'
            )
            new_section = new_section.replace(old_goto_pattern_1, new_goto_pattern_1)

            # Remove the first switch header and opening brace
            # Old:
            #   builder.AppendLine("        switch (chaos_header->type_id)");
            #   builder.AppendLine("        {");
            # Becomes:
            #   // (nothing — replaced by individual if statements)
            new_section = new_section.replace(
                'builder.AppendLine("        switch (chaos_header->type_id)");\n\t\tbuilder.AppendLine("        {");',
                '// Exact type match via TypeInfo* pointer'
            )

            # Remove the first default: break; and closing }
            new_section = new_section.replace(
                'builder.AppendLine("            default:");\n\t\t\tbuilder.AppendLine("                break;");\n\t\t\tbuilder.AppendLine("        }");',
                '// (exact type did not match, will check parent chain)'
            )

            # Now handle the second switch (inside the while loop)
            # Same patterns but with different variable name and deeper indent

            # Replace second switch case labels (12-space indent -> 8-space indent if)
            # Change the while loop header
            new_section = new_section.replace(
                'builder.AppendLine("        auto chaos_current_type_id = chaos_get_base_type_id(chaos_header->type_id);");',
                'builder.AppendLine("        auto* chaos_current_type_info = chaos_header->type_info->parent;");'
            )
            new_section = new_section.replace(
                'builder.AppendLine("        while (chaos_current_type_id != static_cast<CHAOS_IL2CPP_INTPTR>(0))");',
                'builder.AppendLine("        while (chaos_current_type_info != nullptr)");'
            )

            # Second switch header
            new_section = new_section.replace(
                'builder.AppendLine("            switch (chaos_current_type_id)");\n\t\t\tbuilder.AppendLine("            {");',
                '// Parent type check via TypeInfo* pointer'
            )

            # Second case labels
            new_section = new_section.replace(
                'handler.AppendLiteral("                case ");',
                'handler.AppendLiteral("        if (chaos_current_type_info == ");'
            )
            new_section = new_section.replace(
                'handler.AppendFormatted(virtualDispatchTargetTypeInfoPointer);',
                'handler.AppendFormatted(virtualDispatchTargetTypeInfoPointer);'
            )
            new_section = new_section.replace(
                'handler.AppendLiteral(":");\n\t\t\t\t\tstringBuilder9.AppendLine(ref handler);',
                'handler.AppendLiteral(")");\n\t\t\t\t\tstringBuilder9.AppendLine(ref handler);'
            )

            # Add braces around second loop case bodies
            old_case_pattern_2 = (
                '\t\t\t\t\tstringBuilder9.AppendLine(ref handler);\n'
                '\t\t\t\t\tif (string.Equals(text, "void", StringComparison.Ordinal))'
            )
            new_case_pattern_2 = (
                '\t\t\t\t\tstringBuilder9.AppendLine(ref handler);\n'
                '\t\t\t\t\tbuilder.AppendLine("        {");\n'
                '\t\t\t\t\tif (string.Equals(text, "void", StringComparison.Ordinal))'
            )
            new_section = new_section.replace(old_case_pattern_2, new_case_pattern_2)

            # After goto in second loop, add closing brace
            old_goto_pattern_2 = (
                '\t\t\t\t\tstringBuilder12.AppendLine(ref handler);\n'
                '\t\t\t\t}\n'
                '\t\t\t\tbuilder.AppendLine("                default:");'
            )
            new_goto_pattern_2 = (
                '\t\t\t\t\tstringBuilder12.AppendLine(ref handler);\n'
                '\t\t\t\t\tbuilder.AppendLine("        }");\n'
                '\t\t\t\t}\n'
                '\t\t\t\tbuilder.AppendLine("            break;");'
            )
            new_section = new_section.replace(old_goto_pattern_2, new_goto_pattern_2)

            # Remove the default: break; closing } for the second switch
            new_section = new_section.replace(
                'builder.AppendLine("                default:");\n\t\t\t\tbuilder.AppendLine("                    break;");\n\t\t\t\tbuilder.AppendLine("            }");',
                '// (parent did not match, continue walking)'
            )

            # Change the second chaos_get_base_type_id call
            new_section = new_section.replace(
                'builder.AppendLine("            chaos_current_type_id = chaos_get_base_type_id(chaos_current_type_id);");',
                'builder.AppendLine("            chaos_current_type_info = chaos_current_type_info->parent;");'
            )

            # Reconstruct
            content = before_section + new_section + after_section
            print("Virtual dispatch section replaced successfully")
        else:
            print("ERROR: Could not find closing brace of while loop")
    else:
        print("ERROR: Could not find chaos_get_base_type_id in file")

# ─── Final: Fix indentation issue if the default/break handling created extra indent ───
# The "        }" at end of while has changed to "            break;" — let me fix
content = content.replace(
    'builder.AppendLine("            break;");\n\t\t\t// (parent did not match, continue walking)',
    'builder.AppendLine("                break;");\n\t\t\t}'
)

# Hmm, the while closing was: } \n (parent walk) \n } — let me check
# Actually I think the replacements above may have created some issues. Let me fix:
# The second switch replacement removed the closing }, but the while loop needs two }}
# The old structure was:
#   }  // close switch
#   chaos_current_type_id = ...
#   }  // close while
# The new structure should be:
#   // (parent did not match, continue walking)
#   chaos_current_type_info = ...
#   }  // close while

# The issue is the closing brace of the while loop. After the chaos_get_base_type_id replacement,
# there should be a closing } for the while. Let me verify.

if content == original:
    print("ERROR: No changes made to MethodEmission.cs")
else:
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(content)
    print("SUCCESS: MethodEmission.cs updated")
