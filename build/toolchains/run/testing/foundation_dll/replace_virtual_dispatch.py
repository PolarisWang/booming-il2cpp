"""Replace the virtual dispatch section in MethodEmission.cs with clean version."""
import re

filepath = 'D:/agent/booming-il2cpp/src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.MethodEmission.cs'

with open(filepath, 'r', encoding='utf-8') as f:
    content = f.read()

# Find the virtual dispatch section boundaries using unique markers
# Start marker: the comment we inserted + "foreach (VirtualDispatchRoute item in readOnlyList)"
start_marker = '// Exact type match via TypeInfo* pointer\n\t\tforeach (VirtualDispatchRoute item in readOnlyList)'

# End marker: just before "CHAOS_IL2CPP_ABORT();"
# We need to include "chaos_current_type_info->parent" and the closing "}" of the while
end_marker = 'builder.AppendLine("        CHAOS_IL2CPP_ABORT();");'

start_pos = content.find(start_marker)
end_pos = content.find(end_marker)

if start_pos < 0 or end_pos < 0:
    print(f"ERROR: markers not found. start={start_pos}, end={end_pos}")
else:
    # Extract the section before and after
    before = content[:start_pos]
    after = content[end_pos:]

    # Build new section
    new_section = '''// ── Exact type match via TypeInfo* pointer ──
\t\tforeach (VirtualDispatchRoute item in readOnlyList)
\t\t{
\t\t\tstring virtualDispatchTargetTypeInfoPointer = GetVirtualDispatchTargetTypeInfoPointer(item.ReceiverTypeSubjectId);
\t\t\tstring virtualDispatchInstanceExpression = GetVirtualDispatchInstanceExpression(item.ReceiverTypeSubjectId, "chaos_instance");
\t\t\tAotCoreIrMethodArtifact implementationMethod = item.ImplementationMethod;
\t\t\tstringBuilder = builder;
\t\t\tStringBuilder stringBuilder5 = stringBuilder;
\t\t\tStringBuilder.AppendInterpolatedStringHandler handler2 = new StringBuilder.AppendInterpolatedStringHandler(46, 1, stringBuilder);
\t\t\thandler2.AppendLiteral("        if (chaos_header->type_info == ");
\t\t\thandler2.AppendFormatted(virtualDispatchTargetTypeInfoPointer);
\t\t\thandler2.AppendLiteral(")");
\t\t\tstringBuilder5.AppendLine(ref handler2);
\t\t\tbuilder.AppendLine("        {");
\t\t\tif (string.Equals(text, "void", StringComparison.Ordinal))
\t\t\t{
\t\t\t\tstringBuilder = builder;
\t\t\t\tStringBuilder stringBuilder6 = stringBuilder;
\t\t\t\tStringBuilder.AppendInterpolatedStringHandler handler3 = new StringBuilder.AppendInterpolatedStringHandler(19, 2, stringBuilder);
\t\t\t\thandler3.AppendLiteral("            ");
\t\t\t\thandler3.AppendFormatted(implementationMethod.NativeSymbol);
\t\t\t\thandler3.AppendLiteral("(");
\t\t\t\thandler3.AppendFormatted(FormatAbiInvocationArgumentList(methodAbiParameterSlots, virtualDispatchInstanceExpression));
\t\t\t\thandler3.AppendLiteral(");");
\t\t\t\tstringBuilder6.AppendLine(ref handler3);
\t\t\t}
\t\t\telse
\t\t\t{
\t\t\t\tstringBuilder = builder;
\t\t\t\tStringBuilder stringBuilder7 = stringBuilder;
\t\t\t\tStringBuilder.AppendInterpolatedStringHandler handler4 = new StringBuilder.AppendInterpolatedStringHandler(43, 2, stringBuilder);
\t\t\t\thandler4.AppendLiteral("            chaos_callvirt_result = ");
\t\t\t\thandler4.AppendFormatted(implementationMethod.NativeSymbol);
\t\t\t\thandler4.AppendLiteral("(");
\t\t\t\thandler4.AppendFormatted(FormatAbiInvocationArgumentList(methodAbiParameterSlots, virtualDispatchInstanceExpression));
\t\t\t\thandler4.AppendLiteral(");");
\t\t\t\tstringBuilder7.AppendLine(ref handler4);
\t\t\t}
\t\t\tstringBuilder = builder;
\t\t\tStringBuilder stringBuilder8 = stringBuilder;
\t\t\tStringBuilder.AppendInterpolatedStringHandler handler5 = new StringBuilder.AppendInterpolatedStringHandler(22, 1, stringBuilder);
\t\t\thandler5.AppendLiteral("            goto ");
\t\t\thandler5.AppendFormatted(value);
\t\t\thandler5.AppendLiteral(";");
\t\t\tstringBuilder8.AppendLine(ref handler5);
\t\t\tbuilder.AppendLine("        }");
\t\t}
\t\t// ── Parent chain walk via TypeInfo* pointer ──
\t\tbuilder.AppendLine("        auto* chaos_current_type_info = chaos_header->type_info->parent;");
\t\tbuilder.AppendLine("        while (chaos_current_type_info != nullptr)");
\t\tbuilder.AppendLine("        {");
\t\tforeach (VirtualDispatchRoute item2 in readOnlyList)
\t\t{
\t\t\tstring virtualDispatchTargetTypeInfoPointer2 = GetVirtualDispatchTargetTypeInfoPointer(item2.ReceiverTypeSubjectId);
\t\t\tstring virtualDispatchInstanceExpression2 = GetVirtualDispatchInstanceExpression(item2.ReceiverTypeSubjectId, "chaos_instance");
\t\t\tAotCoreIrMethodArtifact implementationMethod2 = item2.ImplementationMethod;
\t\t\tstringBuilder = builder;
\t\t\tStringBuilder stringBuilder9 = stringBuilder;
\t\t\tStringBuilder.AppendInterpolatedStringHandler handler6 = new StringBuilder.AppendInterpolatedStringHandler(46, 1, stringBuilder);
\t\t\thandler6.AppendLiteral("            if (chaos_current_type_info == ");
\t\t\thandler6.AppendFormatted(virtualDispatchTargetTypeInfoPointer2);
\t\t\thandler6.AppendLiteral(")");
\t\t\tstringBuilder9.AppendLine(ref handler6);
\t\t\tbuilder.AppendLine("            {");
\t\t\tif (string.Equals(text, "void", StringComparison.Ordinal))
\t\t\t{
\t\t\t\tstringBuilder = builder;
\t\t\t\tStringBuilder stringBuilder10 = stringBuilder;
\t\t\t\tStringBuilder.AppendInterpolatedStringHandler handler7 = new StringBuilder.AppendInterpolatedStringHandler(23, 2, stringBuilder);
\t\t\t\thandler7.AppendLiteral("                ");
\t\t\t\thandler7.AppendFormatted(implementationMethod2.NativeSymbol);
\t\t\t\thandler7.AppendLiteral("(");
\t\t\t\thandler7.AppendFormatted(FormatAbiInvocationArgumentList(methodAbiParameterSlots, virtualDispatchInstanceExpression2));
\t\t\t\thandler7.AppendLiteral(");");
\t\t\t\tstringBuilder10.AppendLine(ref handler7);
\t\t\t}
\t\t\telse
\t\t\t{
\t\t\t\tstringBuilder = builder;
\t\t\t\tStringBuilder stringBuilder11 = stringBuilder;
\t\t\t\tStringBuilder.AppendInterpolatedStringHandler handler8 = new StringBuilder.AppendInterpolatedStringHandler(47, 2, stringBuilder);
\t\t\t\thandler8.AppendLiteral("                chaos_callvirt_result = ");
\t\t\t\thandler8.AppendFormatted(implementationMethod2.NativeSymbol);
\t\t\t\thandler8.AppendLiteral("(");
\t\t\t\thandler8.AppendFormatted(FormatAbiInvocationArgumentList(methodAbiParameterSlots, virtualDispatchInstanceExpression2));
\t\t\t\thandler8.AppendLiteral(");");
\t\t\t\tstringBuilder11.AppendLine(ref handler8);
\t\t\t}
\t\t\tstringBuilder = builder;
\t\t\tStringBuilder stringBuilder12 = stringBuilder;
\t\t\tStringBuilder.AppendInterpolatedStringHandler handler9 = new StringBuilder.AppendInterpolatedStringHandler(26, 1, stringBuilder);
\t\t\thandler9.AppendLiteral("                goto ");
\t\t\thandler9.AppendFormatted(value);
\t\t\thandler9.AppendLiteral(";");
\t\t\tstringBuilder12.AppendLine(ref handler9);
\t\t\tbuilder.AppendLine("            }");
\t\t}
\t\tbuilder.AppendLine("            chaos_current_type_info = chaos_current_type_info->parent;");
\t\tbuilder.AppendLine("        }");
\t\tbuilder.AppendLine();
'''

    content = before + new_section + after

    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(content)
    print("SUCCESS: Virtual dispatch section rewritten")
