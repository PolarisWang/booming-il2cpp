"""Replace ObjectModelEmission.cs core section with TypeInfo-based code."""

import re

filepath = 'D:/agent/booming-il2cpp/src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.ObjectModelEmission.cs'

with open(filepath, 'r', encoding='utf-8') as f:
    content = f.read()

# Build replacement section — base indentation is 2 tabs (matching the file)
new_code = '''\t\tint num = 2;
\t\t// ── TypeInfo instances (replace integer type_id system) ──
\t\tforeach (string item in referenceTypeSubjectIds.OrderBy<string, string>((string result) => result, StringComparer.Ordinal))
\t\t{
\t\t\tulong stableId = ComputeStableTypeId(item);
\t\t\tstring parentExpr = "nullptr";
\t\t\tif (referenceTypeBaseSubjectIds.TryGetValue(item, out string? baseTypeId) && !string.IsNullOrEmpty(baseTypeId))
\t\t\t{
\t\t\t\tparentExpr = "&" + GetNativeTypeInfoSymbol(baseTypeId);
\t\t\t}
\t\t\t{
\t\t\t\tStringBuilder stringBuilder = builder;
\t\t\t\tStringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(28, 2, stringBuilder);
\t\t\t\thandler.AppendLiteral("inline constexpr TypeInfo ");
\t\t\t\thandler.AppendFormatted(GetNativeTypeInfoSymbol(item));
\t\t\t\thandler.AppendLiteral(" = { ");
\t\t\t\thandler.AppendFormatted(parentExpr);
\t\t\t\thandler.AppendLiteral(", ");
\t\t\t\thandler.AppendFormatted(stableId.ToString() + "ULL");
\t\t\t\thandler.AppendLiteral(", 1 /* reference */ };");
\t\t\t\tstringBuilder.AppendLine(ref handler);
\t\t\t}
\t\t\t{
\t\t\t\tStringBuilder stringBuilder = builder;
\t\t\t\tStringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(28, 2, stringBuilder);
\t\t\t\thandler.AppendLiteral("inline constexpr CHAOS_IL2CPP_INTPTR ");
\t\t\t\thandler.AppendFormatted(GetNativeTypeIdSymbol(item));
\t\t\t\thandler.AppendLiteral(" = static_cast<CHAOS_IL2CPP_INTPTR>(");
\t\t\t\thandler.AppendFormatted(stableId.ToString() + "ULL");
\t\t\t\thandler.AppendLiteral(");");
\t\t\t\tstringBuilder.AppendLine(ref handler);
\t\t\t}
\t\t\tnum++;
\t\t}
\t\tforeach (string item2 in interfaceTypeSubjectIds.OrderBy<string, string>((string result) => result, StringComparer.Ordinal))
\t\t{
\t\t\tulong stableId = ComputeStableTypeId(item2);
\t\t\t{
\t\t\t\tStringBuilder stringBuilder = builder;
\t\t\t\tStringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(28, 2, stringBuilder);
\t\t\t\thandler.AppendLiteral("inline constexpr TypeInfo ");
\t\t\t\thandler.AppendFormatted(GetNativeTypeInfoSymbol(item2));
\t\t\t\thandler.AppendLiteral(" = { nullptr, ");
\t\t\t\thandler.AppendFormatted(stableId.ToString() + "ULL");
\t\t\t\thandler.AppendLiteral(", 3 /* interface */ };");
\t\t\t\tstringBuilder.AppendLine(ref handler);
\t\t\t}
\t\t\t{
\t\t\t\tStringBuilder stringBuilder = builder;
\t\t\t\tStringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(28, 2, stringBuilder);
\t\t\t\thandler.AppendLiteral("inline constexpr CHAOS_IL2CPP_INTPTR ");
\t\t\t\thandler.AppendFormatted(GetNativeTypeIdSymbol(item2));
\t\t\t\thandler.AppendLiteral(" = static_cast<CHAOS_IL2CPP_INTPTR>(");
\t\t\t\thandler.AppendFormatted(stableId.ToString() + "ULL");
\t\t\t\thandler.AppendLiteral(");");
\t\t\t\tstringBuilder.AppendLine(ref handler);
\t\t\t}
\t\t\tnum++;
\t\t}
\t\tforeach (string item3 in valueTypeSubjectIds.OrderBy<string, string>((string result) => result, StringComparer.Ordinal))
\t\t{
\t\t\tulong stableId = ComputeStableTypeId(item3);
\t\t\t{
\t\t\t\tStringBuilder stringBuilder = builder;
\t\t\t\tStringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(28, 2, stringBuilder);
\t\t\t\thandler.AppendLiteral("inline constexpr TypeInfo ");
\t\t\t\thandler.AppendFormatted(GetNativeTypeInfoSymbol(item3));
\t\t\t\thandler.AppendLiteral(" = { nullptr, ");
\t\t\t\thandler.AppendFormatted(stableId.ToString() + "ULL");
\t\t\t\thandler.AppendLiteral(", 2 /* value */ };");
\t\t\t\tstringBuilder.AppendLine(ref handler);
\t\t\t}
\t\t\t{
\t\t\t\tStringBuilder stringBuilder = builder;
\t\t\t\tStringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(28, 2, stringBuilder);
\t\t\t\thandler.AppendLiteral("inline constexpr CHAOS_IL2CPP_INTPTR ");
\t\t\t\thandler.AppendFormatted(GetNativeTypeIdSymbol(item3));
\t\t\t\thandler.AppendLiteral(" = static_cast<CHAOS_IL2CPP_INTPTR>(");
\t\t\t\thandler.AppendFormatted(stableId.ToString() + "ULL");
\t\t\t\thandler.AppendLiteral(");");
\t\t\t\tstringBuilder.AppendLine(ref handler);
\t\t\t}
\t\t\tnum++;
\t\t}
\t\tvar sortedHashSet3 = hashSet3.OrderBy<string, string>((string result) => result, StringComparer.Ordinal).ToArray();
\t\tforeach (string item3 in sortedHashSet3)
\t\t{
\t\t\tulong stableId = ComputeStableTypeId(item3);
\t\t\t{
\t\t\t\tStringBuilder stringBuilder = builder;
\t\t\t\tStringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(28, 2, stringBuilder);
\t\t\t\thandler.AppendLiteral("inline constexpr TypeInfo ");
\t\t\t\thandler.AppendFormatted(GetNativeTypeInfoSymbol(item3));
\t\t\t\thandler.AppendLiteral(" = { nullptr, ");
\t\t\t\thandler.AppendFormatted(stableId.ToString() + "ULL");
\t\t\t\thandler.AppendLiteral(", 2 /* value (boxed) */ };");
\t\t\t\tstringBuilder.AppendLine(ref handler);
\t\t\t}
\t\t\t{
\t\t\t\tStringBuilder stringBuilder = builder;
\t\t\t\tStringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(28, 2, stringBuilder);
\t\t\t\thandler.AppendLiteral("inline constexpr CHAOS_IL2CPP_INTPTR ");
\t\t\t\thandler.AppendFormatted(GetNativeBoxTypeIdSymbol(item3));
\t\t\t\thandler.AppendLiteral(" = static_cast<CHAOS_IL2CPP_INTPTR>(");
\t\t\t\thandler.AppendFormatted(stableId.ToString() + "ULL");
\t\t\t\thandler.AppendLiteral(");");
\t\t\t\tstringBuilder.AppendLine(ref handler);
\t\t\t}
\t\t\tnum++;
\t\t}
\t\tif (referenceTypeSubjectIds.Count > 0 || interfaceTypeSubjectIds.Count > 0 || valueTypeSubjectIds.Count > 0 || hashSet3.Count > 0)
\t\t{
\t\t\tbuilder.AppendLine();
\t\t}
\t\t// ── Parent type info resolver (replaces chaos_get_base_type_id switch) ──
\t\tbuilder.AppendLine("inline const TypeInfo* chaos_get_parent_type_info(const TypeInfo* chaos_ti) noexcept");
\t\tbuilder.AppendLine("{");
\t\tbuilder.AppendLine("    if (chaos_ti == nullptr) return nullptr;");
\t\tbuilder.AppendLine("    return chaos_ti->parent;");
\t\tbuilder.AppendLine("}");
\t\tbuilder.AppendLine();
\t\t// ── Type compatibility (TypeInfo* pointer based, replaces integer walk) ──
\t\tbuilder.AppendLine("bool chaos_is_type_compatible(const TypeInfo* chaos_actual_type_info, const TypeInfo* chaos_target_type_info) noexcept");
\t\tbuilder.AppendLine("{");
\t\tbuilder.AppendLine("    auto* chaos_current = chaos_actual_type_info;");
\t\tbuilder.AppendLine("    while (chaos_current != nullptr)");
\t\tbuilder.AppendLine("    {");
\t\tbuilder.AppendLine("        if (chaos_current == chaos_target_type_info)");
\t\tbuilder.AppendLine("        {");
\t\tbuilder.AppendLine("            return true;");
\t\tbuilder.AppendLine("        }");
\t\tbuilder.AppendLine();
\t\tbuilder.AppendLine("        chaos_current = chaos_current->parent;");
\t\tbuilder.AppendLine("    }");
\t\tbuilder.AppendLine();
\t\tbuilder.AppendLine("    return false;");
\t\tbuilder.AppendLine("}");
\t\tbuilder.AppendLine();
\t\t// ── Interface check (switch on stable_id) ──
\t\tbuilder.AppendLine("bool chaos_type_implements_interface(const TypeInfo* chaos_actual_type_info, const TypeInfo* chaos_target_interface_type_info) noexcept");
\t\tbuilder.AppendLine("{");
\t\tbuilder.AppendLine("    switch (chaos_actual_type_info->stable_id)");
\t\tbuilder.AppendLine("    {");
\t\tforeach (string item6 in referenceTypeSubjectIds.OrderBy<string, string>((string result) => result, StringComparer.Ordinal))
\t\t{
\t\t\tStringBuilder stringBuilder = builder;
\t\t\tStringBuilder stringBuilder9 = stringBuilder;
\t\t\tStringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(14, 1, stringBuilder);
\t\t\thandler.AppendLiteral("        case ");
\t\t\thandler.AppendFormatted(GetNativeTypeIdSymbol(item6));
\t\t\thandler.AppendLiteral(":");
\t\t\tstringBuilder9.AppendLine(ref handler);
\t\t\tif (referenceTypeImplementedInterfaceSubjectIds.TryGetValue(item6, out HashSet<string> value2) && value2.Count > 0)
\t\t\t{
\t\t\t\tstring value3 = string.Join(" || ", from interfaceSubjectId in value2.OrderBy<string, string>((string result) => result, StringComparer.Ordinal)
\t\t\t\t\tselect "chaos_target_interface_type_info->stable_id == " + GetNativeTypeIdSymbol(interfaceSubjectId));
\t\t\t\tstringBuilder = builder;
\t\t\t\tStringBuilder stringBuilder10 = stringBuilder;
\t\t\t\thandler = new StringBuilder.AppendInterpolatedStringHandler(20, 1, stringBuilder);
\t\t\t\thandler.AppendLiteral("            return ");
\t\t\t\thandler.AppendFormatted(value3);
\t\t\t\thandler.AppendLiteral(";");
\t\t\t\tstringBuilder10.AppendLine(ref handler);
\t\t\t}
\t\t\telse
\t\t\t{
\t\t\t\tbuilder.AppendLine("            return false;");
\t\t\t}
\t\t}
\t\tforeach (string item7 in sortedHashSet3)
\t\t{
\t\t\tStringBuilder stringBuilder = builder;
\t\t\tStringBuilder stringBuilder11 = stringBuilder;
\t\t\tStringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(14, 1, stringBuilder);
\t\t\thandler.AppendLiteral("        case ");
\t\t\thandler.AppendFormatted(GetNativeBoxTypeIdSymbol(item7));
\t\t\thandler.AppendLiteral(":");
\t\t\tstringBuilder11.AppendLine(ref handler);
\t\t\tif (referenceTypeImplementedInterfaceSubjectIds.TryGetValue(item7, out HashSet<string> value4) && value4.Count > 0)
\t\t\t{
\t\t\t\tstring value5 = string.Join(" || ", from interfaceSubjectId in value4.OrderBy<string, string>((string result) => result, StringComparer.Ordinal)
\t\t\t\t\tselect "chaos_target_interface_type_info->stable_id == " + GetNativeTypeIdSymbol(interfaceSubjectId));
\t\t\t\tstringBuilder = builder;
\t\t\t\tStringBuilder stringBuilder12 = stringBuilder;
\t\t\t\thandler = new StringBuilder.AppendInterpolatedStringHandler(20, 1, stringBuilder);
\t\t\t\thandler.AppendLiteral("            return ");
\t\t\t\thandler.AppendFormatted(value5);
\t\t\t\thandler.AppendLiteral(";");
\t\t\t\tstringBuilder12.AppendLine(ref handler);
\t\t\t}
\t\t\telse
\t\t\t{
\t\t\t\tbuilder.AppendLine("            return false;");
\t\t\t}
\t\t}
\t\tbuilder.AppendLine("        default:");
\t\tbuilder.AppendLine("            return false;");
\t\tbuilder.AppendLine("    }");
\t\tbuilder.AppendLine("}");
\t\tbuilder.AppendLine();
\t\tbuilder.AppendLine("bool chaos_does_type_implement_interface(const TypeInfo* chaos_actual_type_info, const TypeInfo* chaos_target_interface_type_info) noexcept");
\t\tbuilder.AppendLine("{");
\t\tbuilder.AppendLine("    auto* chaos_current = chaos_actual_type_info;");
\t\tbuilder.AppendLine("    while (chaos_current != nullptr)");
\t\tbuilder.AppendLine("    {");
\t\tbuilder.AppendLine("        if (chaos_type_implements_interface(chaos_current, chaos_target_interface_type_info))");
\t\tbuilder.AppendLine("        {");
\t\tbuilder.AppendLine("            return true;");
\t\tbuilder.AppendLine("        }");
\t\tbuilder.AppendLine();
\t\tbuilder.AppendLine("        chaos_current = chaos_current->parent;");
\t\tbuilder.AppendLine("    }");
\t\tbuilder.AppendLine();
\t\tbuilder.AppendLine("    return false;");
\t\tbuilder.AppendLine("}");
\t\tbuilder.AppendLine();
\t\tbuilder.AppendLine("bool chaos_is_array_type_compatible(");
\t\tbuilder.AppendLine("    CHAOS_IL2CPP_UINT8 chaos_actual_element_shape,");
\t\tbuilder.AppendLine("    const TypeInfo* chaos_actual_element_type_info,");
\t\tbuilder.AppendLine("    CHAOS_IL2CPP_UINT8 chaos_target_element_shape,");
\t\tbuilder.AppendLine("    const TypeInfo* chaos_target_element_type_info) noexcept");
\t\tbuilder.AppendLine("{");
\t\tbuilder.AppendLine("    if (chaos_actual_element_shape == chaos_type_shape_reference)");
\t\tbuilder.AppendLine("    {");
\t\tbuilder.AppendLine("        if (chaos_target_element_shape == chaos_type_shape_reference)");
\t\tbuilder.AppendLine("        {");
\t\tbuilder.AppendLine("            return chaos_is_type_compatible(chaos_actual_element_type_info, chaos_target_element_type_info);");
\t\tbuilder.AppendLine("        }");
\t\tbuilder.AppendLine();
\t\tbuilder.AppendLine("        if (chaos_target_element_shape == chaos_type_shape_interface)");
\t\tbuilder.AppendLine("        {");
\t\tbuilder.AppendLine("            return chaos_does_type_implement_interface(chaos_actual_element_type_info, chaos_target_element_type_info);");
\t\tbuilder.AppendLine("        }");
\t\tbuilder.AppendLine();
\t\tbuilder.AppendLine("        return false;");
\t\tbuilder.AppendLine("    }");
\t\tbuilder.AppendLine();
\t\tbuilder.AppendLine("    return chaos_actual_element_shape == chaos_target_element_shape");
\t\tbuilder.AppendLine("        && chaos_actual_element_type_info == chaos_target_element_type_info;");
\t\tbuilder.AppendLine("}");
\t\tbuilder.AppendLine();
\t\tbuilder.AppendLine("bool chaos_is_array_store_compatible(const chaos_managed_array* chaos_array, CHAOS_IL2CPP_INTPTR chaos_value) noexcept");
\t\tbuilder.AppendLine("{");
\t\tbuilder.AppendLine("    if (chaos_array == nullptr)");
\t\tbuilder.AppendLine("    {");
\t\tbuilder.AppendLine("        return false;");
\t\tbuilder.AppendLine("    }");
\t\tbuilder.AppendLine();
\t\tbuilder.AppendLine("    if (chaos_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
\t\tbuilder.AppendLine("    {");
\t\tbuilder.AppendLine("        return true;");
\t\tbuilder.AppendLine("    }");
\t\tbuilder.AppendLine();
\t\tbuilder.AppendLine("    // StringId fast path: materialized value carries a heap pointer;");
\t\tbuilder.AppendLine("    // this line is reached only if caller skipped materialization.");
\t\tbuilder.AppendLine("    if (chaos_is_string_id(chaos_value))");
\t\tbuilder.AppendLine("    {");
\t\tbuilder.Append("        return chaos_array->element_type_shape == chaos_type_shape_reference");
\t\tbuilder.Append("            && chaos_is_type_compatible(");
\t\tstring stringTypeInfoSymbol = GetNativeTypeInfoSymbol("System.Private.CoreLib/System.String");
\t\tbuilder.Append("&" + stringTypeInfoSymbol);
\t\tbuilder.AppendLine(", chaos_array->element_type_info);");
\t\tbuilder.AppendLine("    }");
\t\tbuilder.AppendLine();
\t\tbuilder.AppendLine("    auto* chaos_header = reinterpret_cast<chaos_object_header*>(chaos_value);");
\t\tbuilder.AppendLine("    if (chaos_array->element_type_shape == chaos_type_shape_interface)");
\t\tbuilder.AppendLine("    {");
\t\tbuilder.AppendLine("        return chaos_does_type_implement_interface(chaos_header->type_info, chaos_array->element_type_info);");
\t\tbuilder.AppendLine("    }");
\t\tbuilder.AppendLine();
\t\tbuilder.AppendLine("    if (chaos_array->element_type_shape == chaos_type_shape_reference)");
\t\tbuilder.AppendLine("    {");
\t\tbuilder.AppendLine("        return chaos_is_type_compatible(chaos_header->type_info, chaos_array->element_type_info);");
\t\tbuilder.AppendLine("    }");
\t\tbuilder.AppendLine();
\t\tbuilder.AppendLine("    return false;");
\t\tbuilder.AppendLine("}");
\t\tbuilder.AppendLine();'''

# Replace from 'int num = 2;' up to (but not including) 'foreach (string typeSubjectId in GetReferenceTypeEmissionOrder'
pattern = r'\t\tint num = 2;.*?(?=\t\tforeach \(string typeSubjectId in GetReferenceTypeEmissionOrder)'

result = re.sub(pattern, new_code, content, count=1, flags=re.DOTALL)

if result == content:
    print('ERROR: Pattern not found!')
    # Debug: show context
    idx = content.find('int num = 2;')
    if idx >= 0:
        print(f'Found int num = 2; at position {idx}')
        print(repr(content[idx:idx+200]))
    idx2 = content.find('GetReferenceTypeEmissionOrder')
    if idx2 >= 0:
        print(f'Found GetReferenceTypeEmissionOrder at position {idx2}')
        print(repr(content[idx2-50:idx2+50]))
else:
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(result)
    print('SUCCESS: Section replaced')
