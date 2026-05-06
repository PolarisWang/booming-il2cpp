using System;
using System.Collections.Generic;
using System.Collections.Immutable;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using System.Runtime.CompilerServices;
using System.Text;
using System.Text.Json;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeAotLoweringPlanner
{
	private void EmitReflectionObjectHelpers(StringBuilder builder, IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods, IReadOnlySet<string> referenceTypeSubjectIds, IReadOnlySet<string> boxedTypeSubjectIds)
	{
		bool flag = _customAttributeSupport.QueryAttributeTypeByCallee.Count > 0 || _customAttributeSupport.SyntheticGetterFieldByMethodSubjectId.Count > 0 || _customAttributeSupport.UsesMemberInfoIsDefined;
		bool flag2 = UsesReachableInstruction(reachableMethods, (AotCoreIrInstructionArtifact instruction) => IsTypeReflectionHelperSubjectId(instruction.Callee ?? string.Empty));
		bool flag3 = UsesReachableInstruction(reachableMethods, (AotCoreIrInstructionArtifact instruction) => IsAssemblyReflectionHelperSubjectId(instruction.Callee ?? string.Empty));
		bool flag4 = UsesReachableInstruction(reachableMethods, (AotCoreIrInstructionArtifact instruction) => IsReflectionMemberHelperSubjectId(instruction.Callee ?? string.Empty));
		bool flag5 = _reflectionMemberSupport.TypeEntries.Count > 0 || _reflectionMemberSupport.FieldEntries.Count > 0 || _reflectionMemberSupport.MethodEntries.Count > 0;
		bool flag6 = UsesDefaultInterpolatedStringHandlerHelpers(reachableMethods);
		bool flag7 = RequiresManagedStringRuntime(reachableMethods);
		if (!flag2 && !flag3 && !flag4 && !flag5 && !flag && !flag7 && _stringIdMapping is not { Count: > 0 })
		{
			return;
		}
		builder.AppendLine("const char* chaos_reflection_get_string_utf8(CHAOS_IL2CPP_INTPTR chaos_string_value) noexcept");
		builder.AppendLine("{");
		builder.AppendLine("    if (chaos_string_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
		builder.AppendLine("    {");
		builder.AppendLine("        return nullptr;");
		builder.AppendLine("    }");
		builder.AppendLine();
		builder.AppendLine("    if (chaos_is_string_id(chaos_string_value))");
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_view = chaos::il2cpp::string_table::Resolve(");
		builder.AppendLine("            chaos_extract_string_id(chaos_string_value));");
		builder.AppendLine("        return chaos_view.utf8_data;");
		builder.AppendLine("    }");
		builder.AppendLine();
		StringBuilder stringBuilder = builder;
		StringBuilder stringBuilder2 = stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(65, 1, stringBuilder);
		handler.AppendLiteral("    auto* chaos_string = reinterpret_cast<");
		handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.String"));
		handler.AppendLiteral("*>(chaos_string_value);");
		stringBuilder2.AppendLine(ref handler);
		builder.AppendLine("    return chaos_string->utf8_data;");
		builder.AppendLine("}");
		builder.AppendLine();
		if (RequiresManagedStringFactory(reachableMethods))
		{
			builder.AppendLine("CHAOS_IL2CPP_INTPTR chaos_reflection_create_string_utf8_copy(const char* chaos_utf8_data, CHAOS_IL2CPP_SIZE chaos_length)");
			builder.AppendLine("{");
			builder.AppendLine("    if (chaos_utf8_data == nullptr)");
			builder.AppendLine("    {");
			builder.AppendLine("        return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
			builder.AppendLine("    }");
			builder.AppendLine();
			builder.AppendLine("    const auto chaos_id = chaos::il2cpp::string_table::Intern(");
			builder.AppendLine("        chaos_utf8_data, static_cast<CHAOS_IL2CPP_UINT32>(chaos_length));");
			builder.AppendLine("    return chaos_make_string_id_value(chaos_id);");
			builder.AppendLine("}");
			builder.AppendLine();
			builder.AppendLine("CHAOS_IL2CPP_INTPTR ChaosReflectionConcatStringPairValues(CHAOS_IL2CPP_INTPTR chaos_left_string_value, CHAOS_IL2CPP_INTPTR chaos_right_string_value)");
			builder.AppendLine("{");
			builder.AppendLine("    const auto chaos_left_length =");
			builder.AppendLine("        chaos_left_string_value == static_cast<CHAOS_IL2CPP_INTPTR>(0)");
			builder.AppendLine("            ? static_cast<CHAOS_IL2CPP_SIZE>(0)");
			builder.AppendLine("            : chaos_is_string_id(chaos_left_string_value)");
			builder.AppendLine("                ? static_cast<CHAOS_IL2CPP_SIZE>(chaos::il2cpp::string_table::Resolve(");
			builder.AppendLine("                    chaos_extract_string_id(chaos_left_string_value)).byte_count)");
			stringBuilder = builder;
			StringBuilder stringBuilder5 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(93, 1, stringBuilder);
			handler.AppendLiteral("                : static_cast<CHAOS_IL2CPP_SIZE>(reinterpret_cast<");
			handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.String"));
			handler.AppendLiteral("*>(chaos_left_string_value)->length);");
			stringBuilder5.AppendLine(ref handler);
			builder.AppendLine("    const auto chaos_right_length =");
			builder.AppendLine("        chaos_right_string_value == static_cast<CHAOS_IL2CPP_INTPTR>(0)");
			builder.AppendLine("            ? static_cast<CHAOS_IL2CPP_SIZE>(0)");
			builder.AppendLine("            : chaos_is_string_id(chaos_right_string_value)");
			builder.AppendLine("                ? static_cast<CHAOS_IL2CPP_SIZE>(chaos::il2cpp::string_table::Resolve(");
			builder.AppendLine("                    chaos_extract_string_id(chaos_right_string_value)).byte_count)");
			stringBuilder = builder;
			StringBuilder stringBuilder6 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(94, 1, stringBuilder);
			handler.AppendLiteral("                : static_cast<CHAOS_IL2CPP_SIZE>(reinterpret_cast<");
			handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.String"));
			handler.AppendLiteral("*>(chaos_right_string_value)->length);");
			stringBuilder6.AppendLine(ref handler);
			builder.AppendLine("    const char* chaos_left_utf8 = chaos_reflection_get_string_utf8(chaos_left_string_value);");
			builder.AppendLine("    const char* chaos_right_utf8 = chaos_reflection_get_string_utf8(chaos_right_string_value);");
			builder.AppendLine("    if ((chaos_left_string_value != static_cast<CHAOS_IL2CPP_INTPTR>(0) && chaos_left_utf8 == nullptr && chaos_left_length != 0) ||");
			builder.AppendLine("        (chaos_right_string_value != static_cast<CHAOS_IL2CPP_INTPTR>(0) && chaos_right_utf8 == nullptr && chaos_right_length != 0))");
			builder.AppendLine("    {");
			builder.AppendLine("        CHAOS_IL2CPP_ABORT();");
			builder.AppendLine("    }");
			builder.AppendLine();
			builder.AppendLine("    const auto chaos_combined_length = chaos_left_length + chaos_right_length;");
			builder.AppendLine("    auto* chaos_combined_utf8 = new char[chaos_combined_length + 1];");
			builder.AppendLine("    if (chaos_left_length != 0)");
			builder.AppendLine("    {");
			builder.AppendLine("        CHAOS_IL2CPP_MEMCPY(chaos_combined_utf8, chaos_left_utf8, chaos_left_length);");
			builder.AppendLine("    }");
			builder.AppendLine("    if (chaos_right_length != 0)");
			builder.AppendLine("    {");
			builder.AppendLine("        CHAOS_IL2CPP_MEMCPY(chaos_combined_utf8 + chaos_left_length, chaos_right_utf8, chaos_right_length);");
			builder.AppendLine("    }");
			builder.AppendLine("    chaos_combined_utf8[chaos_combined_length] = '\\0';");
			builder.AppendLine("    const auto chaos_result = chaos_reflection_create_string_utf8_copy(chaos_combined_utf8, chaos_combined_length);");
			builder.AppendLine("    delete[] chaos_combined_utf8;");
			builder.AppendLine("    return chaos_result;");
			builder.AppendLine("}");
			builder.AppendLine();
			builder.AppendLine("CHAOS_IL2CPP_INTPTR chaos_reflection_create_string_literal(const char* chaos_utf8_data)");
			builder.AppendLine("{");
			builder.AppendLine("    if (chaos_utf8_data == nullptr)");
			builder.AppendLine("    {");
			builder.AppendLine("        return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
			builder.AppendLine("    }");
			builder.AppendLine();
			builder.AppendLine("    return chaos_reflection_create_string_utf8_copy(chaos_utf8_data, CHAOS_IL2CPP_STRLEN(chaos_utf8_data));");
			builder.AppendLine("}");
			builder.AppendLine();
		}
		if (_stringIdMapping is { Count: > 0 })
		{
			builder.AppendLine();
			builder.AppendLine("CHAOS_IL2CPP_INTPTR chaos_string_materialize(CHAOS_IL2CPP_INTPTR chaos_value) noexcept");
			builder.AppendLine("{");
			builder.AppendLine("    if (chaos_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
			builder.AppendLine("    {");
			builder.AppendLine("        return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
			builder.AppendLine("    }");
			builder.AppendLine();
			builder.AppendLine("    if (!chaos_is_string_id(chaos_value))");
			builder.AppendLine("    {");
			builder.AppendLine("        return chaos_value;");
			builder.AppendLine("    }");
			builder.AppendLine();
			builder.AppendLine("    const auto chaos_id = chaos_extract_string_id(chaos_value);");
			builder.AppendLine("    const auto chaos_view = chaos::il2cpp::string_table::Resolve(chaos_id);");
			builder.AppendLine();
			builder.Append("    auto* chaos_string = static_cast<");
			builder.Append(GetNativeTypeSymbol("System.Private.CoreLib/System.String"));
			builder.AppendLine("*>(chaos::il2cpp::runtime_core::GcAllocate(sizeof(");
			builder.Append(GetNativeTypeSymbol("System.Private.CoreLib/System.String"));
			builder.AppendLine(")));");
			builder.Append("    chaos_string->header.type_info = &");
			builder.Append(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.String"));
			builder.AppendLine(";");
			if (_vtableTypes?.Contains("System.Private.CoreLib/System.String") == true)
			{
				builder.Append("    chaos_string->header.vtable = ");
				builder.Append(GetNativeVTableSymbol("System.Private.CoreLib/System.String"));
				builder.AppendLine(";");
			}
			builder.AppendLine("    chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_view.byte_count);");
			builder.AppendLine("    auto* owned_utf8 = static_cast<char*>(chaos::il2cpp::runtime_core::GcAllocateAtomic(chaos_view.byte_count + 1));");
			builder.AppendLine("    CHAOS_IL2CPP_MEMCPY(owned_utf8, chaos_view.utf8_data, chaos_view.byte_count);");
			builder.AppendLine("    owned_utf8[chaos_view.byte_count] = '\\0';");
			builder.AppendLine("    chaos_string->utf8_data = owned_utf8;");
			builder.AppendLine("    chaos_string->string_id = chaos_id;");
			builder.AppendLine("    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);");
			builder.AppendLine("}");
			builder.AppendLine();
		}
		if (flag6)
		{
			builder.AppendLine("struct chaos_default_interpolated_string_handler_state");
			builder.AppendLine("{");
			builder.AppendLine("    CHAOS_IL2CPP_VECTOR(CHAOS_IL2CPP_INTPTR> fragments;");
			builder.AppendLine("};");
			builder.AppendLine();
			builder.AppendLine("CHAOS_IL2CPP_INTPTR chaos_default_interpolated_string_handler_key(CHAOS_IL2CPP_INTPTR chaos_handler_ref)");
			builder.AppendLine("{");
			builder.AppendLine("    if (chaos_handler_ref == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
			builder.AppendLine("    {");
			builder.AppendLine("        CHAOS_IL2CPP_ABORT();");
			builder.AppendLine("    }");
			builder.AppendLine();
			builder.AppendLine("    if ((chaos_handler_ref & chaos_managed_pointer_local_slot_tag) != 0)");
			builder.AppendLine("    {");
			builder.AppendLine("        return static_cast<CHAOS_IL2CPP_INTPTR>(chaos_handler_ref & ~chaos_managed_pointer_local_slot_tag);");
			builder.AppendLine("    }");
			builder.AppendLine();
			builder.AppendLine("    return chaos_handler_ref;");
			builder.AppendLine("}");
			builder.AppendLine();
			builder.AppendLine("chaos_default_interpolated_string_handler_state* chaos_require_default_interpolated_string_handler_state(CHAOS_IL2CPP_INTPTR chaos_handler_ref)");
			builder.AppendLine("{");
			builder.AppendLine("    static CHAOS_IL2CPP_UNORDERED_MAP(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_UNIQUE_PTR(chaos_default_interpolated_string_handler_state>> chaos_state_by_key;");
			builder.AppendLine("    const auto chaos_key = chaos_default_interpolated_string_handler_key(chaos_handler_ref);");
			builder.AppendLine("    auto& chaos_state = chaos_state_by_key[chaos_key];");
			builder.AppendLine("    if (chaos_state == nullptr)");
			builder.AppendLine("    {");
			builder.AppendLine("        chaos_state = CHAOS_IL2CPP_MAKE_UNIQUE(chaos_default_interpolated_string_handler_state>();");
			builder.AppendLine("    }");
			builder.AppendLine();
			builder.AppendLine("    return chaos_state.get();");
			builder.AppendLine("}");
			builder.AppendLine();
			builder.AppendLine("void chaos_default_interpolated_string_handler_reset(CHAOS_IL2CPP_INTPTR chaos_handler_ref)");
			builder.AppendLine("{");
			builder.AppendLine("    auto* chaos_state = chaos_require_default_interpolated_string_handler_state(chaos_handler_ref);");
			builder.AppendLine("    chaos_state->fragments.clear();");
			builder.AppendLine("}");
			builder.AppendLine();
			builder.AppendLine("void chaos_default_interpolated_string_handler_append_string(CHAOS_IL2CPP_INTPTR chaos_handler_ref, CHAOS_IL2CPP_INTPTR chaos_string_value)");
			builder.AppendLine("{");
			builder.AppendLine("    auto* chaos_state = chaos_require_default_interpolated_string_handler_state(chaos_handler_ref);");
			builder.AppendLine("    if (chaos_string_value != static_cast<CHAOS_IL2CPP_INTPTR>(0))");
			builder.AppendLine("    {");
			builder.AppendLine("        chaos_state->fragments.push_back(chaos_string_value);");
			builder.AppendLine("    }");
			builder.AppendLine("}");
			builder.AppendLine();
			builder.AppendLine("void chaos_default_interpolated_string_handler_append_int32(CHAOS_IL2CPP_INTPTR chaos_handler_ref, CHAOS_IL2CPP_INT32 chaos_value)");
			builder.AppendLine("{");
			builder.AppendLine("    const CHAOS_IL2CPP_STRING chaos_formatted = ChaosIl2cpp::Common::format_int32(chaos_value);");
			builder.AppendLine("    const auto chaos_id = chaos::il2cpp::string_table::Intern(");
			builder.AppendLine("        chaos_formatted.c_str(), static_cast<CHAOS_IL2CPP_UINT32>(chaos_formatted.size()));");
			builder.AppendLine("    chaos_default_interpolated_string_handler_append_string(");
			builder.AppendLine("        chaos_handler_ref, chaos_make_string_id_value(chaos_id));");
			builder.AppendLine("}");
			builder.AppendLine();
			builder.AppendLine("CHAOS_IL2CPP_INTPTR chaos_default_interpolated_string_handler_to_string_and_clear(CHAOS_IL2CPP_INTPTR chaos_handler_ref)");
			builder.AppendLine("{");
			builder.AppendLine("    auto* chaos_state = chaos_require_default_interpolated_string_handler_state(chaos_handler_ref);");
			builder.AppendLine("    if (chaos_state->fragments.empty())");
			builder.AppendLine("    {");
			builder.AppendLine("        const auto chaos_id = chaos::il2cpp::string_table::Intern(\"\", 0u);");
			builder.AppendLine("        return chaos_make_string_id_value(chaos_id);");
			builder.AppendLine("    }");
			builder.AppendLine();
			builder.AppendLine("    if (chaos_state->fragments.size() == 1)");
			builder.AppendLine("    {");
			builder.AppendLine("        const auto chaos_result = chaos_state->fragments[0];");
			builder.AppendLine("        chaos_state->fragments.clear();");
			builder.AppendLine("        return chaos_result;");
			builder.AppendLine("    }");
			builder.AppendLine();
			builder.AppendLine("    // Accumulate total byte count.");
			builder.AppendLine("    CHAOS_IL2CPP_SIZE chaos_total_bytes = 0u;");
			builder.AppendLine("    for (const auto chaos_fragment : chaos_state->fragments)");
			builder.AppendLine("    {");
			builder.AppendLine("        if (chaos_is_string_id(chaos_fragment))");
			builder.AppendLine("        {");
			builder.AppendLine("            const auto chaos_view = chaos::il2cpp::string_table::Resolve(");
			builder.AppendLine("                chaos_extract_string_id(chaos_fragment));");
			builder.AppendLine("            chaos_total_bytes += chaos_view.byte_count;");
			builder.AppendLine("        }");
			builder.AppendLine("        else");
			builder.AppendLine("        {");
			builder.AppendLine("            auto* chaos_s = reinterpret_cast<chaos_SR_String*>(chaos_fragment);");
			builder.AppendLine("            chaos_total_bytes += static_cast<CHAOS_IL2CPP_SIZE>(chaos_s->length);");
			builder.AppendLine("        }");
			builder.AppendLine("    }");
			builder.AppendLine();
			builder.AppendLine("    // Concatenate all fragments into a single buffer.");
			builder.AppendLine("    CHAOS_IL2CPP_STRING chaos_concat;");
			builder.AppendLine("    chaos_concat.reserve(chaos_total_bytes);");
			builder.AppendLine("    for (const auto chaos_fragment : chaos_state->fragments)");
			builder.AppendLine("    {");
			builder.AppendLine("        if (chaos_is_string_id(chaos_fragment))");
			builder.AppendLine("        {");
			builder.AppendLine("            const auto chaos_view = chaos::il2cpp::string_table::Resolve(");
			builder.AppendLine("                chaos_extract_string_id(chaos_fragment));");
			builder.AppendLine("            chaos_concat.append(chaos_view.utf8_data, chaos_view.byte_count);");
			builder.AppendLine("        }");
			builder.AppendLine("        else");
			builder.AppendLine("        {");
			builder.AppendLine("            auto* chaos_s = reinterpret_cast<chaos_SR_String*>(chaos_fragment);");
			builder.AppendLine("            chaos_concat.append(chaos_s->utf8_data, static_cast<CHAOS_IL2CPP_SIZE>(chaos_s->length));");
			builder.AppendLine("        }");
			builder.AppendLine("    }");
			builder.AppendLine();
			builder.AppendLine("    const auto chaos_id = chaos::il2cpp::string_table::Intern(");
			builder.AppendLine("        chaos_concat.data(), static_cast<CHAOS_IL2CPP_UINT32>(chaos_concat.size()));");
			builder.AppendLine("    chaos_state->fragments.clear();");
			builder.AppendLine("    return chaos_make_string_id_value(chaos_id);");
			builder.AppendLine("}");
			builder.AppendLine();
		}
		if (flag2 || flag4 || flag5)
		{
			builder.AppendLine("CHAOS_IL2CPP_INTPTR chaos_reflection_create_reference_array(const TypeInfo* chaos_element_type_info, CHAOS_IL2CPP_SIZE chaos_length)");
			builder.AppendLine("{");
			builder.AppendLine("    auto* chaos_array = new chaos_managed_array{};");
			builder.AppendLine("    chaos_array->header.type_info = &chaos_type_info_managed_array;");
			builder.AppendLine("    chaos_array->element_type_shape = chaos_type_shape_reference;");
			builder.AppendLine("    chaos_array->element_type_info = chaos_element_type_info;");
			builder.AppendLine("    chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);");
			builder.AppendLine("    chaos_array->elements = chaos_length == 0 ? nullptr : new CHAOS_IL2CPP_INTPTR[chaos_length]{};");
			builder.AppendLine("    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);");
			builder.AppendLine("}");
			builder.AppendLine();
			builder.AppendLine("CHAOS_IL2CPP_INTPTR chaos_reflection_get_type_name_value_from_handle(CHAOS_IL2CPP_INTPTR chaos_type_handle) noexcept");
			builder.AppendLine("{");
			builder.AppendLine("    switch (chaos_type_handle)");
			builder.AppendLine("    {");
			foreach (ReflectionMemberTypeEntry typeEntry in _reflectionMemberSupport.TypeEntries)
			{
				stringBuilder = builder;
				StringBuilder stringBuilder7 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(14, 1, stringBuilder);
				handler.AppendLiteral("        case ");
				handler.AppendFormatted(GetTypeHandleLiteral(typeEntry.TypeSubjectId));
				handler.AppendLiteral(":");
				stringBuilder7.AppendLine(ref handler);
				stringBuilder = builder;
				StringBuilder stringBuilder8 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(60, 1, stringBuilder);
				handler.AppendLiteral("            return chaos_reflection_create_string_literal(");
				handler.AppendFormatted(ToCppStringLiteral(typeEntry.TypeName));
				handler.AppendLiteral(");");
				stringBuilder8.AppendLine(ref handler);
			}
			builder.AppendLine("        default:");
			builder.AppendLine("            return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
			builder.AppendLine("    }");
			builder.AppendLine("}");
			builder.AppendLine();
			builder.AppendLine("CHAOS_IL2CPP_INT32 chaos_reflection_get_type_metadata_token_from_handle(CHAOS_IL2CPP_INTPTR chaos_type_handle) noexcept");
			builder.AppendLine("{");
			builder.AppendLine("    switch (chaos_type_handle)");
			builder.AppendLine("    {");
			foreach (ReflectionMemberTypeEntry typeEntry2 in _reflectionMemberSupport.TypeEntries)
			{
				stringBuilder = builder;
				StringBuilder stringBuilder9 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(14, 1, stringBuilder);
				handler.AppendLiteral("        case ");
				handler.AppendFormatted(GetTypeHandleLiteral(typeEntry2.TypeSubjectId));
				handler.AppendLiteral(":");
				stringBuilder9.AppendLine(ref handler);
				stringBuilder = builder;
				StringBuilder stringBuilder10 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(20, 1, stringBuilder);
				handler.AppendLiteral("            return ");
				handler.AppendFormatted(typeEntry2.MetadataToken);
				handler.AppendLiteral(";");
				stringBuilder10.AppendLine(ref handler);
			}
			builder.AppendLine("        default:");
			builder.AppendLine("            return 0;");
			builder.AppendLine("    }");
			builder.AppendLine("}");
			builder.AppendLine();
			builder.AppendLine("CHAOS_IL2CPP_INTPTR chaos_reflection_create_type_value(CHAOS_IL2CPP_INTPTR chaos_type_handle)");
			builder.AppendLine("{");
			stringBuilder = builder;
			StringBuilder stringBuilder11 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(30, 1, stringBuilder);
			handler.AppendLiteral("    auto* chaos_type = new ");
			handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Type"));
			handler.AppendLiteral("{};");
			stringBuilder11.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder12 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(34, 1, stringBuilder);
			handler.AppendLiteral("    chaos_type->header.type_info = &");
			handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Type"));
			handler.AppendLiteral(";");
			stringBuilder12.AppendLine(ref handler);
			if (_vtableTypes?.Contains("System.Private.CoreLib/System.Type") == true)
			{
				stringBuilder = builder;
				StringBuilder stringBuilder_vt9 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(32, 1, stringBuilder);
				handler.AppendLiteral("    chaos_type->header.vtable = ");
				handler.AppendFormatted(GetNativeVTableSymbol("System.Private.CoreLib/System.Type"));
				handler.AppendLiteral(";");
				stringBuilder_vt9.AppendLine(ref handler);
			}
			builder.AppendLine("    chaos_type->runtime_type_handle = chaos_type_handle;");
			builder.AppendLine("    chaos_type->runtime_name_value = chaos_reflection_get_type_name_value_from_handle(chaos_type_handle);");
			builder.AppendLine("    chaos_type->runtime_metadata_token_value = chaos_reflection_get_type_metadata_token_from_handle(chaos_type_handle);");
			builder.AppendLine("    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_type);");
			builder.AppendLine("}");
			builder.AppendLine();
			builder.AppendLine("CHAOS_IL2CPP_INTPTR ChaosReflectionGetMethod_name_value_from_handle(CHAOS_IL2CPP_INTPTR chaos_method_handle) noexcept");
			builder.AppendLine("{");
			builder.AppendLine("    switch (chaos_method_handle)");
			builder.AppendLine("    {");
			foreach (ReflectionMemberMethodEntry methodEntry in _reflectionMemberSupport.MethodEntries)
			{
				stringBuilder = builder;
				StringBuilder stringBuilder13 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(14, 1, stringBuilder);
				handler.AppendLiteral("        case ");
				handler.AppendFormatted(GetMethodHandleLiteral(methodEntry.MethodSubjectId));
				handler.AppendLiteral(":");
				stringBuilder13.AppendLine(ref handler);
				stringBuilder = builder;
				StringBuilder stringBuilder14 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(60, 1, stringBuilder);
				handler.AppendLiteral("            return chaos_reflection_create_string_literal(");
				handler.AppendFormatted(ToCppStringLiteral(methodEntry.MethodName));
				handler.AppendLiteral(");");
				stringBuilder14.AppendLine(ref handler);
			}
			builder.AppendLine("        default:");
			builder.AppendLine("            return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
			builder.AppendLine("    }");
			builder.AppendLine("}");
			builder.AppendLine();
			builder.AppendLine("CHAOS_IL2CPP_INT32 ChaosReflectionGetMethod_metadata_token_from_handle(CHAOS_IL2CPP_INTPTR chaos_method_handle) noexcept");
			builder.AppendLine("{");
			builder.AppendLine("    switch (chaos_method_handle)");
			builder.AppendLine("    {");
			foreach (ReflectionMemberMethodEntry methodEntry2 in _reflectionMemberSupport.MethodEntries)
			{
				stringBuilder = builder;
				StringBuilder stringBuilder15 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(14, 1, stringBuilder);
				handler.AppendLiteral("        case ");
				handler.AppendFormatted(GetMethodHandleLiteral(methodEntry2.MethodSubjectId));
				handler.AppendLiteral(":");
				stringBuilder15.AppendLine(ref handler);
				stringBuilder = builder;
				StringBuilder stringBuilder16 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(20, 1, stringBuilder);
				handler.AppendLiteral("            return ");
				handler.AppendFormatted(methodEntry2.MetadataToken);
				handler.AppendLiteral(";");
				stringBuilder16.AppendLine(ref handler);
			}
			builder.AppendLine("        default:");
			builder.AppendLine("            return 0;");
			builder.AppendLine("    }");
			builder.AppendLine("}");
			builder.AppendLine();
			builder.AppendLine("CHAOS_IL2CPP_INTPTR ChaosReflectionGetDeclaringType_handle_from_type_handle(CHAOS_IL2CPP_INTPTR chaos_type_handle) noexcept");
			builder.AppendLine("{");
			builder.AppendLine("    switch (chaos_type_handle)");
			builder.AppendLine("    {");
			foreach (ReflectionMemberTypeEntry item in _reflectionMemberSupport.TypeEntries.Where((ReflectionMemberTypeEntry entry) => !string.IsNullOrWhiteSpace(TryGetNestedDeclaringTypeSubjectId(entry.TypeSubjectId))).OrderBy<ReflectionMemberTypeEntry, string>((ReflectionMemberTypeEntry entry) => entry.TypeSubjectId, StringComparer.Ordinal))
			{
				string? subjectId = TryGetNestedDeclaringTypeSubjectId(item.TypeSubjectId);
				stringBuilder = builder;
				StringBuilder stringBuilder17 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(14, 1, stringBuilder);
				handler.AppendLiteral("        case ");
				handler.AppendFormatted(GetTypeHandleLiteral(item.TypeSubjectId));
				handler.AppendLiteral(":");
				stringBuilder17.AppendLine(ref handler);
				stringBuilder = builder;
				StringBuilder stringBuilder18 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(20, 1, stringBuilder);
				handler.AppendLiteral("            return ");
				handler.AppendFormatted(GetTypeHandleLiteral(subjectId!));
				handler.AppendLiteral(";");
				stringBuilder18.AppendLine(ref handler);
			}
			builder.AppendLine("        default:");
			builder.AppendLine("            return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
			builder.AppendLine("    }");
			builder.AppendLine("}");
			builder.AppendLine();
			if (flag5)
			{
				builder.AppendLine("CHAOS_IL2CPP_INTPTR ChaosReflectionGetMemberName(CHAOS_IL2CPP_INTPTR chaos_member_value) noexcept");
				builder.AppendLine("{");
				builder.AppendLine("    if (chaos_member_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
				builder.AppendLine("    {");
				builder.AppendLine("        return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
				builder.AppendLine("    }");
				builder.AppendLine();
				builder.AppendLine("    auto* chaos_header = reinterpret_cast<chaos_object_header*>(chaos_member_value);");
				stringBuilder = builder;
				StringBuilder stringBuilder19 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(34, 1, stringBuilder);
				handler.AppendLiteral("    if (chaos_header->type_info == &");
				handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Type"));
				handler.AppendLiteral("\n        || chaos_header->type_info->stable_id == (&");
				handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Type"));
				handler.AppendLiteral(")->stable_id)");
				stringBuilder19.AppendLine(ref handler);
				builder.AppendLine("    {");
				stringBuilder = builder;
				StringBuilder stringBuilder20 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(67, 1, stringBuilder);
				handler.AppendLiteral("        auto* chaos_type = reinterpret_cast<");
				handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Type"));
				handler.AppendLiteral("*>(chaos_member_value);");
				stringBuilder20.AppendLine(ref handler);
				builder.AppendLine("        return chaos_type->runtime_name_value == static_cast<CHAOS_IL2CPP_INTPTR>(0)");
				builder.AppendLine("            ? chaos_reflection_get_type_name_value_from_handle(chaos_type->runtime_type_handle)");
				builder.AppendLine("            : chaos_type->runtime_name_value;");
				builder.AppendLine("    }");
				builder.AppendLine();
				stringBuilder = builder;
				StringBuilder stringBuilder21 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(34, 1, stringBuilder);
				handler.AppendLiteral("    if (chaos_header->type_info == &");
				handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Reflection.MethodInfo"));
				handler.AppendLiteral("\n        || chaos_header->type_info->stable_id == (&");
				handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Reflection.MethodInfo"));
				handler.AppendLiteral(")->stable_id)");
				stringBuilder21.AppendLine(ref handler);
				builder.AppendLine("    {");
				stringBuilder = builder;
				StringBuilder stringBuilder22 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(69, 1, stringBuilder);
				handler.AppendLiteral("        auto* chaos_method = reinterpret_cast<");
				handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Reflection.MethodInfo"));
				handler.AppendLiteral("*>(chaos_member_value);");
				stringBuilder22.AppendLine(ref handler);
				builder.AppendLine("        return chaos_method->runtime_name_value == static_cast<CHAOS_IL2CPP_INTPTR>(0)");
				builder.AppendLine("            ? ChaosReflectionGetMethod_name_value_from_handle(chaos_method->runtime_method_handle)");
				builder.AppendLine("            : chaos_method->runtime_name_value;");
				builder.AppendLine("    }");
				builder.AppendLine();
				stringBuilder = builder;
				StringBuilder stringBuilder23 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(34, 1, stringBuilder);
				handler.AppendLiteral("    if (chaos_header->type_info == &");
				handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Reflection.ConstructorInfo"));
				handler.AppendLiteral("\n        || chaos_header->type_info->stable_id == (&");
				handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Reflection.ConstructorInfo"));
				handler.AppendLiteral(")->stable_id)");
				stringBuilder23.AppendLine(ref handler);
				builder.AppendLine("    {");
				stringBuilder = builder;
				StringBuilder stringBuilder24 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(69, 1, stringBuilder);
				handler.AppendLiteral("        auto* chaos_method = reinterpret_cast<");
				handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Reflection.ConstructorInfo"));
				handler.AppendLiteral("*>(chaos_member_value);");
				stringBuilder24.AppendLine(ref handler);
				builder.AppendLine("        return chaos_method->runtime_name_value == static_cast<CHAOS_IL2CPP_INTPTR>(0)");
				builder.AppendLine("            ? ChaosReflectionGetMethod_name_value_from_handle(chaos_method->runtime_method_handle)");
				builder.AppendLine("            : chaos_method->runtime_name_value;");
				builder.AppendLine("    }");
				builder.AppendLine();
				stringBuilder = builder;
				StringBuilder stringBuilder25 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(34, 1, stringBuilder);
				handler.AppendLiteral("    if (chaos_header->type_info == &");
				handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Reflection.FieldInfo"));
				handler.AppendLiteral("\n        || chaos_header->type_info->stable_id == (&");
				handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Reflection.FieldInfo"));
				handler.AppendLiteral(")->stable_id)");
				stringBuilder25.AppendLine(ref handler);
				builder.AppendLine("    {");
				stringBuilder = builder;
				StringBuilder stringBuilder26 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(68, 1, stringBuilder);
				handler.AppendLiteral("        auto* chaos_field = reinterpret_cast<");
				handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Reflection.FieldInfo"));
				handler.AppendLiteral("*>(chaos_member_value);");
				stringBuilder26.AppendLine(ref handler);
				builder.AppendLine("        return chaos_field->runtime_name_value;");
				builder.AppendLine("    }");
				builder.AppendLine();
				builder.AppendLine("    return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
				builder.AppendLine("}");
				builder.AppendLine();
				builder.AppendLine("CHAOS_IL2CPP_INTPTR ChaosReflectionGetDeclaringType(CHAOS_IL2CPP_INTPTR chaos_member_value)");
				builder.AppendLine("{");
				builder.AppendLine("    if (chaos_member_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
				builder.AppendLine("    {");
				builder.AppendLine("        return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
				builder.AppendLine("    }");
				builder.AppendLine();
				builder.AppendLine("    auto* chaos_header = reinterpret_cast<chaos_object_header*>(chaos_member_value);");
				builder.AppendLine("    CHAOS_IL2CPP_INTPTR chaos_declaring_type_handle = static_cast<CHAOS_IL2CPP_INTPTR>(0);");
				stringBuilder = builder;
				StringBuilder stringBuilder27 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(34, 1, stringBuilder);
				handler.AppendLiteral("    if (chaos_header->type_info == &");
				handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Type"));
				handler.AppendLiteral("\n        || chaos_header->type_info->stable_id == (&");
				handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Type"));
				handler.AppendLiteral(")->stable_id)");
				stringBuilder27.AppendLine(ref handler);
				builder.AppendLine("    {");
				stringBuilder = builder;
				StringBuilder stringBuilder28 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(67, 1, stringBuilder);
				handler.AppendLiteral("        auto* chaos_type = reinterpret_cast<");
				handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Type"));
				handler.AppendLiteral("*>(chaos_member_value);");
				stringBuilder28.AppendLine(ref handler);
				builder.AppendLine("        chaos_declaring_type_handle = ChaosReflectionGetDeclaringType_handle_from_type_handle(chaos_type->runtime_type_handle);");
				builder.AppendLine("    }");
				stringBuilder = builder;
				StringBuilder stringBuilder29 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(39, 1, stringBuilder);
				handler.AppendLiteral("    else if (chaos_header->type_info == &");
				handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Reflection.MethodInfo"));
				handler.AppendLiteral("\n        || chaos_header->type_info->stable_id == (&");
				handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Reflection.MethodInfo"));
				handler.AppendLiteral(")->stable_id)");
				stringBuilder29.AppendLine(ref handler);
				builder.AppendLine("    {");
				stringBuilder = builder;
				StringBuilder stringBuilder30 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(69, 1, stringBuilder);
				handler.AppendLiteral("        auto* chaos_method = reinterpret_cast<");
				handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Reflection.MethodInfo"));
				handler.AppendLiteral("*>(chaos_member_value);");
				stringBuilder30.AppendLine(ref handler);
				builder.AppendLine("        chaos_declaring_type_handle = chaos_method->declaring_type_handle;");
				builder.AppendLine("    }");
				stringBuilder = builder;
				StringBuilder stringBuilder31 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(39, 1, stringBuilder);
				handler.AppendLiteral("    else if (chaos_header->type_info == &");
				handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Reflection.ConstructorInfo"));
				handler.AppendLiteral("\n        || chaos_header->type_info->stable_id == (&");
				handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Reflection.ConstructorInfo"));
				handler.AppendLiteral(")->stable_id)");
				stringBuilder31.AppendLine(ref handler);
				builder.AppendLine("    {");
				stringBuilder = builder;
				StringBuilder stringBuilder32 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(69, 1, stringBuilder);
				handler.AppendLiteral("        auto* chaos_method = reinterpret_cast<");
				handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Reflection.ConstructorInfo"));
				handler.AppendLiteral("*>(chaos_member_value);");
				stringBuilder32.AppendLine(ref handler);
				builder.AppendLine("        chaos_declaring_type_handle = chaos_method->declaring_type_handle;");
				builder.AppendLine("    }");
				stringBuilder = builder;
				StringBuilder stringBuilder33 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(39, 1, stringBuilder);
				handler.AppendLiteral("    else if (chaos_header->type_info == &");
				handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Reflection.FieldInfo"));
				handler.AppendLiteral("\n        || chaos_header->type_info->stable_id == (&");
				handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Reflection.FieldInfo"));
				handler.AppendLiteral(")->stable_id)");
				stringBuilder33.AppendLine(ref handler);
				builder.AppendLine("    {");
				stringBuilder = builder;
				StringBuilder stringBuilder34 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(68, 1, stringBuilder);
				handler.AppendLiteral("        auto* chaos_field = reinterpret_cast<");
				handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Reflection.FieldInfo"));
				handler.AppendLiteral("*>(chaos_member_value);");
				stringBuilder34.AppendLine(ref handler);
				builder.AppendLine("        chaos_declaring_type_handle = chaos_field->declaring_type_handle;");
				builder.AppendLine("    }");
				builder.AppendLine();
				builder.AppendLine("    return chaos_declaring_type_handle == static_cast<CHAOS_IL2CPP_INTPTR>(0)");
				builder.AppendLine("        ? static_cast<CHAOS_IL2CPP_INTPTR>(0)");
				builder.AppendLine("        : chaos_reflection_create_type_value(chaos_declaring_type_handle);");
				builder.AppendLine("}");
				builder.AppendLine();
				builder.AppendLine("CHAOS_IL2CPP_INT32 ChaosReflectionGetMetadataToken(CHAOS_IL2CPP_INTPTR chaos_member_value) noexcept");
				builder.AppendLine("{");
				builder.AppendLine("    if (chaos_member_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
				builder.AppendLine("    {");
				builder.AppendLine("        return 0;");
				builder.AppendLine("    }");
				builder.AppendLine();
				builder.AppendLine("    auto* chaos_header = reinterpret_cast<chaos_object_header*>(chaos_member_value);");
				stringBuilder = builder;
				StringBuilder stringBuilder35 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(34, 1, stringBuilder);
				handler.AppendLiteral("    if (chaos_header->type_info == &");
				handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Type"));
				handler.AppendLiteral("\n        || chaos_header->type_info->stable_id == (&");
				handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Type"));
				handler.AppendLiteral(")->stable_id)");
				stringBuilder35.AppendLine(ref handler);
				builder.AppendLine("    {");
				stringBuilder = builder;
				StringBuilder stringBuilder36 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(67, 1, stringBuilder);
				handler.AppendLiteral("        auto* chaos_type = reinterpret_cast<");
				handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Type"));
				handler.AppendLiteral("*>(chaos_member_value);");
				stringBuilder36.AppendLine(ref handler);
				builder.AppendLine("        return chaos_type->runtime_metadata_token_value != 0");
				builder.AppendLine("            ? chaos_type->runtime_metadata_token_value");
				builder.AppendLine("            : chaos_reflection_get_type_metadata_token_from_handle(chaos_type->runtime_type_handle);");
				builder.AppendLine("    }");
				builder.AppendLine();
				stringBuilder = builder;
				StringBuilder stringBuilder37 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(34, 1, stringBuilder);
				handler.AppendLiteral("    if (chaos_header->type_info == &");
				handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Reflection.MethodInfo"));
				handler.AppendLiteral("\n        || chaos_header->type_info->stable_id == (&");
				handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Reflection.MethodInfo"));
				handler.AppendLiteral(")->stable_id)");
				stringBuilder37.AppendLine(ref handler);
				builder.AppendLine("    {");
				stringBuilder = builder;
				StringBuilder stringBuilder38 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(69, 1, stringBuilder);
				handler.AppendLiteral("        auto* chaos_method = reinterpret_cast<");
				handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Reflection.MethodInfo"));
				handler.AppendLiteral("*>(chaos_member_value);");
				stringBuilder38.AppendLine(ref handler);
				builder.AppendLine("        const auto chaos_lookup_handle = chaos_method->generic_definition_method_handle != static_cast<CHAOS_IL2CPP_INTPTR>(0)");
				builder.AppendLine("            ? chaos_method->generic_definition_method_handle");
				builder.AppendLine("            : chaos_method->runtime_method_handle;");
				builder.AppendLine("        return chaos_method->runtime_metadata_token_value != 0");
				builder.AppendLine("            ? chaos_method->runtime_metadata_token_value");
				builder.AppendLine("            : ChaosReflectionGetMethod_metadata_token_from_handle(chaos_lookup_handle);");
				builder.AppendLine("    }");
				builder.AppendLine();
				stringBuilder = builder;
				StringBuilder stringBuilder39 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(34, 1, stringBuilder);
				handler.AppendLiteral("    if (chaos_header->type_info == &");
				handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Reflection.ConstructorInfo"));
				handler.AppendLiteral("\n        || chaos_header->type_info->stable_id == (&");
				handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Reflection.ConstructorInfo"));
				handler.AppendLiteral(")->stable_id)");
				stringBuilder39.AppendLine(ref handler);
				builder.AppendLine("    {");
				stringBuilder = builder;
				StringBuilder stringBuilder40 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(69, 1, stringBuilder);
				handler.AppendLiteral("        auto* chaos_method = reinterpret_cast<");
				handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Reflection.ConstructorInfo"));
				handler.AppendLiteral("*>(chaos_member_value);");
				stringBuilder40.AppendLine(ref handler);
				builder.AppendLine("        const auto chaos_lookup_handle = chaos_method->generic_definition_method_handle != static_cast<CHAOS_IL2CPP_INTPTR>(0)");
				builder.AppendLine("            ? chaos_method->generic_definition_method_handle");
				builder.AppendLine("            : chaos_method->runtime_method_handle;");
				builder.AppendLine("        return chaos_method->runtime_metadata_token_value != 0");
				builder.AppendLine("            ? chaos_method->runtime_metadata_token_value");
				builder.AppendLine("            : ChaosReflectionGetMethod_metadata_token_from_handle(chaos_lookup_handle);");
				builder.AppendLine("    }");
				builder.AppendLine();
				stringBuilder = builder;
				StringBuilder stringBuilder41 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(34, 1, stringBuilder);
				handler.AppendLiteral("    if (chaos_header->type_info == &");
				handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Reflection.FieldInfo"));
				handler.AppendLiteral("\n        || chaos_header->type_info->stable_id == (&");
				handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Reflection.FieldInfo"));
				handler.AppendLiteral(")->stable_id)");
				stringBuilder41.AppendLine(ref handler);
				builder.AppendLine("    {");
				stringBuilder = builder;
				StringBuilder stringBuilder42 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(68, 1, stringBuilder);
				handler.AppendLiteral("        auto* chaos_field = reinterpret_cast<");
				handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Reflection.FieldInfo"));
				handler.AppendLiteral("*>(chaos_member_value);");
				stringBuilder42.AppendLine(ref handler);
				builder.AppendLine("        return chaos_field->runtime_metadata_token_value;");
				builder.AppendLine("    }");
				builder.AppendLine();
				builder.AppendLine("    return 0;");
				builder.AppendLine("}");
				builder.AppendLine();
				builder.AppendLine("CHAOS_IL2CPP_INTPTR ChaosReflectionGetParameterName(CHAOS_IL2CPP_INTPTR chaos_parameter_value) noexcept");
				builder.AppendLine("{");
				builder.AppendLine("    if (chaos_parameter_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
				builder.AppendLine("    {");
				builder.AppendLine("        return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
				builder.AppendLine("    }");
				builder.AppendLine();
				stringBuilder = builder;
				StringBuilder stringBuilder43 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(71, 1, stringBuilder);
				handler.AppendLiteral("    auto* chaos_parameter = reinterpret_cast<");
				handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Reflection.ParameterInfo"));
				handler.AppendLiteral("*>(chaos_parameter_value);");
				stringBuilder43.AppendLine(ref handler);
				builder.AppendLine("    return chaos_parameter->runtime_name_value;");
				builder.AppendLine("}");
				builder.AppendLine();
				builder.AppendLine("CHAOS_IL2CPP_INTPTR ChaosReflectionGetField(CHAOS_IL2CPP_INTPTR chaos_type_value, CHAOS_IL2CPP_INTPTR chaos_name_value)");
				builder.AppendLine("{");
				stringBuilder = builder;
				StringBuilder stringBuilder44 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(61, 1, stringBuilder);
				handler.AppendLiteral("    auto* chaos_type = reinterpret_cast<");
				handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Type"));
				handler.AppendLiteral("*>(chaos_type_value);");
				stringBuilder44.AppendLine(ref handler);
				builder.AppendLine("    const auto* chaos_field_name = chaos_reflection_get_string_utf8(chaos_name_value);");
				builder.AppendLine("    if (chaos_type == nullptr || chaos_field_name == nullptr)");
				builder.AppendLine("    {");
				builder.AppendLine("        return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
				builder.AppendLine("    }");
				builder.AppendLine();
				builder.AppendLine("    switch (chaos_type->runtime_type_handle)");
				builder.AppendLine("    {");
				foreach (IGrouping<string, ReflectionMemberFieldEntry> item2 in _reflectionMemberSupport.FieldEntries.GroupBy<ReflectionMemberFieldEntry, string>((ReflectionMemberFieldEntry entry) => entry.DeclaringTypeSubjectId, StringComparer.Ordinal).OrderBy<IGrouping<string, ReflectionMemberFieldEntry>, string>((IGrouping<string, ReflectionMemberFieldEntry> group) => group.Key, StringComparer.Ordinal))
				{
					stringBuilder = builder;
					StringBuilder stringBuilder45 = stringBuilder;
					handler = new StringBuilder.AppendInterpolatedStringHandler(14, 1, stringBuilder);
					handler.AppendLiteral("        case ");
					handler.AppendFormatted(GetTypeHandleLiteral(item2.Key));
					handler.AppendLiteral(":");
					stringBuilder45.AppendLine(ref handler);
					foreach (ReflectionMemberFieldEntry item3 in item2.OrderBy<ReflectionMemberFieldEntry, string>((ReflectionMemberFieldEntry entry) => entry.FieldName, StringComparer.Ordinal))
					{
						stringBuilder = builder;
						StringBuilder stringBuilder46 = stringBuilder;
						handler = new StringBuilder.AppendInterpolatedStringHandler(53, 1, stringBuilder);
						handler.AppendLiteral("            if (CHAOS_IL2CPP_STRCMP(chaos_field_name, ");
						handler.AppendFormatted(ToCppStringLiteral(item3.FieldName));
						handler.AppendLiteral(") == 0)");
						stringBuilder46.AppendLine(ref handler);
						builder.AppendLine("            {");
						stringBuilder = builder;
						StringBuilder stringBuilder47 = stringBuilder;
						handler = new StringBuilder.AppendInterpolatedStringHandler(43, 1, stringBuilder);
						handler.AppendLiteral("                auto* chaos_field = new ");
						handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Reflection.FieldInfo"));
						handler.AppendLiteral("{};");
						stringBuilder47.AppendLine(ref handler);
						stringBuilder = builder;
						StringBuilder stringBuilder48 = stringBuilder;
						handler = new StringBuilder.AppendInterpolatedStringHandler(47, 1, stringBuilder);
						handler.AppendLiteral("                chaos_field->header.type_info = &");
						handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Reflection.FieldInfo"));
						handler.AppendLiteral(";");
						stringBuilder48.AppendLine(ref handler);
						if (_vtableTypes?.Contains("System.Private.CoreLib/System.Reflection.FieldInfo") == true)
						{
							stringBuilder = builder;
							StringBuilder stringBuilder_vt8 = stringBuilder;
							handler = new StringBuilder.AppendInterpolatedStringHandler(45, 1, stringBuilder);
							handler.AppendLiteral("                chaos_field->header.vtable = ");
							handler.AppendFormatted(GetNativeVTableSymbol("System.Private.CoreLib/System.Reflection.FieldInfo"));
							handler.AppendLiteral(";");
							stringBuilder_vt8.AppendLine(ref handler);
						}
						builder.AppendLine("                chaos_field->declaring_type_handle = chaos_type->runtime_type_handle;");
						stringBuilder = builder;
						StringBuilder stringBuilder49 = stringBuilder;
						handler = new StringBuilder.AppendInterpolatedStringHandler(91, 1, stringBuilder);
						handler.AppendLiteral("                chaos_field->runtime_name_value = chaos_reflection_create_string_literal(");
						handler.AppendFormatted(ToCppStringLiteral(item3.FieldName));
						handler.AppendLiteral(");");
						stringBuilder49.AppendLine(ref handler);
						stringBuilder = builder;
						StringBuilder stringBuilder50 = stringBuilder;
						handler = new StringBuilder.AppendInterpolatedStringHandler(61, 1, stringBuilder);
						handler.AppendLiteral("                chaos_field->runtime_metadata_token_value = ");
						handler.AppendFormatted(item3.MetadataToken);
						handler.AppendLiteral(";");
						stringBuilder50.AppendLine(ref handler);
						builder.AppendLine("                return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_field);");
						builder.AppendLine("            }");
						builder.AppendLine();
					}
					builder.AppendLine("            break;");
				}
				builder.AppendLine("        default:");
				builder.AppendLine("            break;");
				builder.AppendLine("    }");
				builder.AppendLine();
				builder.AppendLine("    return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
				builder.AppendLine("}");
				builder.AppendLine();
				builder.AppendLine("CHAOS_IL2CPP_INTPTR ChaosReflectionGetGenericTypeDefinition(CHAOS_IL2CPP_INTPTR chaos_type_value)");
				builder.AppendLine("{");
				stringBuilder = builder;
				StringBuilder stringBuilder51 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(61, 1, stringBuilder);
				handler.AppendLiteral("    auto* chaos_type = reinterpret_cast<");
				handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Type"));
				handler.AppendLiteral("*>(chaos_type_value);");
				stringBuilder51.AppendLine(ref handler);
				builder.AppendLine("    if (chaos_type == nullptr)");
				builder.AppendLine("    {");
				builder.AppendLine("        return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
				builder.AppendLine("    }");
				builder.AppendLine();
				builder.AppendLine("    switch (chaos_type->runtime_type_handle)");
				builder.AppendLine("    {");
				foreach (ReflectionMemberTypeEntry item4 in _reflectionMemberSupport.TypeEntries.Where((ReflectionMemberTypeEntry entry) => !string.IsNullOrWhiteSpace(entry.GenericDefinitionTypeSubjectId)).OrderBy<ReflectionMemberTypeEntry, string>((ReflectionMemberTypeEntry entry) => entry.TypeSubjectId, StringComparer.Ordinal))
				{
					stringBuilder = builder;
					StringBuilder stringBuilder52 = stringBuilder;
					handler = new StringBuilder.AppendInterpolatedStringHandler(14, 1, stringBuilder);
					handler.AppendLiteral("        case ");
					handler.AppendFormatted(GetTypeHandleLiteral(item4.TypeSubjectId));
					handler.AppendLiteral(":");
					stringBuilder52.AppendLine(ref handler);
					stringBuilder = builder;
					StringBuilder stringBuilder53 = stringBuilder;
					handler = new StringBuilder.AppendInterpolatedStringHandler(56, 1, stringBuilder);
					handler.AppendLiteral("            return chaos_reflection_create_type_value(");
					handler.AppendFormatted(GetTypeHandleLiteral(item4.GenericDefinitionTypeSubjectId!));
					handler.AppendLiteral(");");
					stringBuilder53.AppendLine(ref handler);
				}
				builder.AppendLine("        default:");
				builder.AppendLine("            return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
				builder.AppendLine("    }");
				builder.AppendLine("}");
				builder.AppendLine();
				builder.AppendLine("CHAOS_IL2CPP_INTPTR ChaosReflectionGetGenericArguments(CHAOS_IL2CPP_INTPTR chaos_type_value)");
				builder.AppendLine("{");
				stringBuilder = builder;
				StringBuilder stringBuilder54 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(61, 1, stringBuilder);
				handler.AppendLiteral("    auto* chaos_type = reinterpret_cast<");
				handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Type"));
				handler.AppendLiteral("*>(chaos_type_value);");
				stringBuilder54.AppendLine(ref handler);
				builder.AppendLine("    if (chaos_type == nullptr)");
				builder.AppendLine("    {");
				builder.AppendLine("        return chaos_reflection_create_reference_array(");
				stringBuilder = builder;
				StringBuilder stringBuilder55 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(13, 1, stringBuilder);
				handler.AppendLiteral("            ");
				handler.AppendFormatted(GetRuntimeTypeInfoExpression("System.Private.CoreLib/System.Type"));
				handler.AppendLiteral(",");
				stringBuilder55.AppendLine(ref handler);
				builder.AppendLine("            static_cast<CHAOS_IL2CPP_SIZE>(0));");
				builder.AppendLine("    }");
				builder.AppendLine();
				builder.AppendLine("    switch (chaos_type->runtime_type_handle)");
				builder.AppendLine("    {");
				foreach (ReflectionMemberTypeEntry item5 in _reflectionMemberSupport.TypeEntries.Where((ReflectionMemberTypeEntry entry) => entry.GenericParameterCount > 0).OrderBy<ReflectionMemberTypeEntry, string>((ReflectionMemberTypeEntry entry) => entry.TypeSubjectId, StringComparer.Ordinal))
				{
					int value = ((item5.GenericArgumentTypeSubjectIds.Count > 0) ? item5.GenericArgumentTypeSubjectIds.Count : item5.GenericParameterCount);
					stringBuilder = builder;
					StringBuilder stringBuilder56 = stringBuilder;
					handler = new StringBuilder.AppendInterpolatedStringHandler(14, 1, stringBuilder);
					handler.AppendLiteral("        case ");
					handler.AppendFormatted(GetTypeHandleLiteral(item5.TypeSubjectId));
					handler.AppendLiteral(":");
					stringBuilder56.AppendLine(ref handler);
					builder.AppendLine("        {");
					builder.AppendLine("            const auto chaos_array_value = chaos_reflection_create_reference_array(");
					stringBuilder = builder;
					StringBuilder stringBuilder57 = stringBuilder;
					handler = new StringBuilder.AppendInterpolatedStringHandler(17, 1, stringBuilder);
					handler.AppendLiteral("                ");
					handler.AppendFormatted(GetRuntimeTypeInfoExpression("System.Private.CoreLib/System.Type"));
					handler.AppendLiteral(",");
					stringBuilder57.AppendLine(ref handler);
					stringBuilder = builder;
					StringBuilder stringBuilder58 = stringBuilder;
					handler = new StringBuilder.AppendInterpolatedStringHandler(44, 1, stringBuilder);
					handler.AppendLiteral("                static_cast<CHAOS_IL2CPP_SIZE>(");
					handler.AppendFormatted(value);
					handler.AppendLiteral("));");
					stringBuilder58.AppendLine(ref handler);
					builder.AppendLine("            auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_array_value);");
					for (int num = 0; num < item5.GenericArgumentTypeSubjectIds.Count; num++)
					{
						stringBuilder = builder;
						StringBuilder stringBuilder59 = stringBuilder;
						handler = new StringBuilder.AppendInterpolatedStringHandler(75, 2, stringBuilder);
						handler.AppendLiteral("            chaos_array->elements[");
						handler.AppendFormatted(num);
						handler.AppendLiteral("] = chaos_reflection_create_type_value(");
						handler.AppendFormatted(GetTypeHandleLiteral(item5.GenericArgumentTypeSubjectIds[num]));
						handler.AppendLiteral(");");
						stringBuilder59.AppendLine(ref handler);
					}
					builder.AppendLine("            return chaos_array_value;");
					builder.AppendLine("        }");
				}
				builder.AppendLine("        default:");
				builder.AppendLine("            return chaos_reflection_create_reference_array(");
				stringBuilder = builder;
				StringBuilder stringBuilder60 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(17, 1, stringBuilder);
				handler.AppendLiteral("                ");
				handler.AppendFormatted(GetRuntimeTypeInfoExpression("System.Private.CoreLib/System.Type"));
				handler.AppendLiteral(",");
				stringBuilder60.AppendLine(ref handler);
				builder.AppendLine("                static_cast<CHAOS_IL2CPP_SIZE>(0));");
				builder.AppendLine("    }");
				builder.AppendLine("}");
				builder.AppendLine();
				builder.AppendLine("CHAOS_IL2CPP_INTPTR ChaosReflectionGetConstructors(CHAOS_IL2CPP_INTPTR chaos_type_value, CHAOS_IL2CPP_INT32 chaos_binding_flags)");
				builder.AppendLine("{");
				builder.AppendLine("    (void)chaos_binding_flags;");
				stringBuilder = builder;
				StringBuilder stringBuilder61 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(61, 1, stringBuilder);
				handler.AppendLiteral("    auto* chaos_type = reinterpret_cast<");
				handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Type"));
				handler.AppendLiteral("*>(chaos_type_value);");
				stringBuilder61.AppendLine(ref handler);
				builder.AppendLine("    if (chaos_type == nullptr)");
				builder.AppendLine("    {");
				builder.AppendLine("        return chaos_reflection_create_reference_array(");
				stringBuilder = builder;
				StringBuilder stringBuilder62 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(13, 1, stringBuilder);
				handler.AppendLiteral("            ");
				handler.AppendFormatted(GetRuntimeTypeInfoExpression("System.Private.CoreLib/System.Reflection.ConstructorInfo"));
				handler.AppendLiteral(",");
				stringBuilder62.AppendLine(ref handler);
				builder.AppendLine("            static_cast<CHAOS_IL2CPP_SIZE>(0));");
				builder.AppendLine("    }");
				builder.AppendLine();
				builder.AppendLine("    switch (chaos_type->runtime_type_handle)");
				builder.AppendLine("    {");
				foreach (IGrouping<string, ReflectionMemberMethodEntry> item6 in _reflectionMemberSupport.MethodEntries.Where((ReflectionMemberMethodEntry entry) => entry.IsConstructor).GroupBy<ReflectionMemberMethodEntry, string>((ReflectionMemberMethodEntry entry) => entry.DeclaringTypeSubjectId, StringComparer.Ordinal).OrderBy<IGrouping<string, ReflectionMemberMethodEntry>, string>((IGrouping<string, ReflectionMemberMethodEntry> group) => group.Key, StringComparer.Ordinal))
				{
					stringBuilder = builder;
					StringBuilder stringBuilder63 = stringBuilder;
					handler = new StringBuilder.AppendInterpolatedStringHandler(14, 1, stringBuilder);
					handler.AppendLiteral("        case ");
					handler.AppendFormatted(GetTypeHandleLiteral(item6.Key));
					handler.AppendLiteral(":");
					stringBuilder63.AppendLine(ref handler);
					builder.AppendLine("        {");
					stringBuilder = builder;
					StringBuilder stringBuilder64 = stringBuilder;
					handler = new StringBuilder.AppendInterpolatedStringHandler(113, 2, stringBuilder);
					handler.AppendLiteral("            const auto chaos_array_value = chaos_reflection_create_reference_array(");
					handler.AppendFormatted(GetRuntimeTypeInfoExpression("System.Private.CoreLib/System.Reflection.ConstructorInfo"));
					handler.AppendLiteral(", static_cast<CHAOS_IL2CPP_SIZE>(");
					handler.AppendFormatted(item6.Count());
					handler.AppendLiteral("));");
					stringBuilder64.AppendLine(ref handler);
					builder.AppendLine("            auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_array_value);");
					int num2 = 0;
					foreach (ReflectionMemberMethodEntry item7 in item6.OrderBy<ReflectionMemberMethodEntry, string>((ReflectionMemberMethodEntry entry) => entry.MethodSubjectId, StringComparer.Ordinal))
					{
						stringBuilder = builder;
						StringBuilder stringBuilder65 = stringBuilder;
						handler = new StringBuilder.AppendInterpolatedStringHandler(46, 2, stringBuilder);
						handler.AppendLiteral("            auto* chaos_constructor_");
						handler.AppendFormatted(num2);
						handler.AppendLiteral(" = new ");
						handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Reflection.ConstructorInfo"));
						handler.AppendLiteral("{};");
						stringBuilder65.AppendLine(ref handler);
						stringBuilder = builder;
						StringBuilder stringBuilder66 = stringBuilder;
						handler = new StringBuilder.AppendInterpolatedStringHandler(50, 2, stringBuilder);
						handler.AppendLiteral("            chaos_constructor_");
						handler.AppendFormatted(num2);
						handler.AppendLiteral("->header.type_info = &");
						handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Reflection.ConstructorInfo"));
						handler.AppendLiteral(";");
						stringBuilder66.AppendLine(ref handler);
						if (_vtableTypes?.Contains("System.Private.CoreLib/System.Reflection.ConstructorInfo") == true)
						{
							stringBuilder = builder;
							StringBuilder stringBuilder_vt7 = stringBuilder;
							handler = new StringBuilder.AppendInterpolatedStringHandler(34, 1, stringBuilder);
							handler.AppendLiteral("        __VAR_PLACEHOLDER__->header.vtable = ");
							handler.AppendFormatted(GetNativeVTableSymbol("System.Private.CoreLib/System.Reflection.ConstructorInfo"));
							handler.AppendLiteral(";");
							stringBuilder_vt7.AppendLine(ref handler);
						}
						stringBuilder = builder;
						StringBuilder stringBuilder67 = stringBuilder;
						handler = new StringBuilder.AppendInterpolatedStringHandler(57, 2, stringBuilder);
						handler.AppendLiteral("            chaos_constructor_");
						handler.AppendFormatted(num2);
						handler.AppendLiteral("->declaring_type_handle = ");
						handler.AppendFormatted(GetTypeHandleLiteral(item6.Key));
						handler.AppendLiteral(";");
						stringBuilder67.AppendLine(ref handler);
						stringBuilder = builder;
						StringBuilder stringBuilder68 = stringBuilder;
						handler = new StringBuilder.AppendInterpolatedStringHandler(57, 2, stringBuilder);
						handler.AppendLiteral("            chaos_constructor_");
						handler.AppendFormatted(num2);
						handler.AppendLiteral("->runtime_method_handle = ");
						handler.AppendFormatted(GetMethodHandleLiteral(item7.MethodSubjectId));
						handler.AppendLiteral(";");
						stringBuilder68.AppendLine(ref handler);
						stringBuilder = builder;
						StringBuilder stringBuilder69 = stringBuilder;
						handler = new StringBuilder.AppendInterpolatedStringHandler(68, 2, stringBuilder);
						handler.AppendLiteral("            chaos_constructor_");
						handler.AppendFormatted(num2);
						handler.AppendLiteral("->generic_definition_method_handle = ");
						handler.AppendFormatted(GetMethodHandleLiteral(item7.MethodSubjectId));
						handler.AppendLiteral(";");
						stringBuilder69.AppendLine(ref handler);
						stringBuilder = builder;
						StringBuilder stringBuilder70 = stringBuilder;
						handler = new StringBuilder.AppendInterpolatedStringHandler(106, 2, stringBuilder);
						handler.AppendLiteral("            chaos_constructor_");
						handler.AppendFormatted(num2);
						handler.AppendLiteral("->runtime_name_value = ChaosReflectionGetMethod_name_value_from_handle(");
						handler.AppendFormatted(GetMethodHandleLiteral(item7.MethodSubjectId));
						handler.AppendLiteral(");");
						stringBuilder70.AppendLine(ref handler);
						stringBuilder = builder;
						StringBuilder stringBuilder71 = stringBuilder;
						handler = new StringBuilder.AppendInterpolatedStringHandler(64, 2, stringBuilder);
						handler.AppendLiteral("            chaos_constructor_");
						handler.AppendFormatted(num2);
						handler.AppendLiteral("->runtime_metadata_token_value = ");
						handler.AppendFormatted(item7.MetadataToken);
						handler.AppendLiteral(";");
						stringBuilder71.AppendLine(ref handler);
						stringBuilder = builder;
						StringBuilder stringBuilder72 = stringBuilder;
						handler = new StringBuilder.AppendInterpolatedStringHandler(90, 2, stringBuilder);
						handler.AppendLiteral("            chaos_array->elements[");
						handler.AppendFormatted(num2);
						handler.AppendLiteral("] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_constructor_");
						handler.AppendFormatted(num2);
						handler.AppendLiteral(");");
						stringBuilder72.AppendLine(ref handler);
						num2++;
					}
					builder.AppendLine("            return chaos_array_value;");
					builder.AppendLine("        }");
				}
				builder.AppendLine("        default:");
				builder.AppendLine("            return chaos_reflection_create_reference_array(");
				stringBuilder = builder;
				StringBuilder stringBuilder73 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(17, 1, stringBuilder);
				handler.AppendLiteral("                ");
				handler.AppendFormatted(GetRuntimeTypeInfoExpression("System.Private.CoreLib/System.Reflection.ConstructorInfo"));
				handler.AppendLiteral(",");
				stringBuilder73.AppendLine(ref handler);
				builder.AppendLine("                static_cast<CHAOS_IL2CPP_SIZE>(0));");
				builder.AppendLine("    }");
				builder.AppendLine("}");
				builder.AppendLine();
				builder.AppendLine("CHAOS_IL2CPP_INTPTR ChaosReflectionGetParameters(CHAOS_IL2CPP_INTPTR chaos_method_value)");
				builder.AppendLine("{");
				builder.AppendLine("    if (chaos_method_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
				builder.AppendLine("    {");
				builder.AppendLine("        return chaos_reflection_create_reference_array(");
				stringBuilder = builder;
				StringBuilder stringBuilder74 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(13, 1, stringBuilder);
				handler.AppendLiteral("            ");
				handler.AppendFormatted(GetRuntimeTypeInfoExpression("System.Private.CoreLib/System.Reflection.ParameterInfo"));
				handler.AppendLiteral(",");
				stringBuilder74.AppendLine(ref handler);
				builder.AppendLine("            static_cast<CHAOS_IL2CPP_SIZE>(0));");
				builder.AppendLine("    }");
				builder.AppendLine();
				builder.AppendLine("    auto* chaos_header = reinterpret_cast<chaos_object_header*>(chaos_method_value);");
				builder.AppendLine("    CHAOS_IL2CPP_INTPTR chaos_method_handle = static_cast<CHAOS_IL2CPP_INTPTR>(0);");
				stringBuilder = builder;
				StringBuilder stringBuilder75 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(34, 1, stringBuilder);
				handler.AppendLiteral("    if (chaos_header->type_info == &");
				handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Reflection.MethodInfo"));
				handler.AppendLiteral("\n        || chaos_header->type_info->stable_id == (&");
				handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Reflection.MethodInfo"));
				handler.AppendLiteral(")->stable_id)");
				stringBuilder75.AppendLine(ref handler);
				builder.AppendLine("    {");
				stringBuilder = builder;
				StringBuilder stringBuilder76 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(69, 1, stringBuilder);
				handler.AppendLiteral("        auto* chaos_method = reinterpret_cast<");
				handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Reflection.MethodInfo"));
				handler.AppendLiteral("*>(chaos_method_value);");
				stringBuilder76.AppendLine(ref handler);
				builder.AppendLine("        chaos_method_handle = chaos_method->generic_definition_method_handle != static_cast<CHAOS_IL2CPP_INTPTR>(0)");
				builder.AppendLine("            ? chaos_method->generic_definition_method_handle");
				builder.AppendLine("            : chaos_method->runtime_method_handle;");
				builder.AppendLine("    }");
				stringBuilder = builder;
				StringBuilder stringBuilder77 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(39, 1, stringBuilder);
				handler.AppendLiteral("    else if (chaos_header->type_info == &");
				handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Reflection.ConstructorInfo"));
				handler.AppendLiteral("\n        || chaos_header->type_info->stable_id == (&");
				handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Reflection.ConstructorInfo"));
				handler.AppendLiteral(")->stable_id)");
				stringBuilder77.AppendLine(ref handler);
				builder.AppendLine("    {");
				stringBuilder = builder;
				StringBuilder stringBuilder78 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(69, 1, stringBuilder);
				handler.AppendLiteral("        auto* chaos_method = reinterpret_cast<");
				handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Reflection.ConstructorInfo"));
				handler.AppendLiteral("*>(chaos_method_value);");
				stringBuilder78.AppendLine(ref handler);
				builder.AppendLine("        chaos_method_handle = chaos_method->runtime_method_handle;");
				builder.AppendLine("    }");
				builder.AppendLine();
				builder.AppendLine("    switch (chaos_method_handle)");
				builder.AppendLine("    {");
				foreach (ReflectionMemberMethodEntry item8 in _reflectionMemberSupport.MethodEntries.OrderBy<ReflectionMemberMethodEntry, string>((ReflectionMemberMethodEntry entry) => entry.MethodSubjectId, StringComparer.Ordinal))
				{
					stringBuilder = builder;
					StringBuilder stringBuilder79 = stringBuilder;
					handler = new StringBuilder.AppendInterpolatedStringHandler(14, 1, stringBuilder);
					handler.AppendLiteral("        case ");
					handler.AppendFormatted(GetMethodHandleLiteral(item8.MethodSubjectId));
					handler.AppendLiteral(":");
					stringBuilder79.AppendLine(ref handler);
					builder.AppendLine("        {");
					stringBuilder = builder;
					StringBuilder stringBuilder80 = stringBuilder;
					handler = new StringBuilder.AppendInterpolatedStringHandler(113, 2, stringBuilder);
					handler.AppendLiteral("            const auto chaos_array_value = chaos_reflection_create_reference_array(");
					handler.AppendFormatted(GetRuntimeTypeInfoExpression("System.Private.CoreLib/System.Reflection.ParameterInfo"));
					handler.AppendLiteral(", static_cast<CHAOS_IL2CPP_SIZE>(");
					handler.AppendFormatted(item8.ParameterNames.Count);
					handler.AppendLiteral("));");
					stringBuilder80.AppendLine(ref handler);
					builder.AppendLine("            auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_array_value);");
					for (int num3 = 0; num3 < item8.ParameterNames.Count; num3++)
					{
						string value2 = item8.ParameterNames[num3];
						stringBuilder = builder;
						StringBuilder stringBuilder81 = stringBuilder;
						handler = new StringBuilder.AppendInterpolatedStringHandler(44, 2, stringBuilder);
						handler.AppendLiteral("            auto* chaos_parameter_");
						handler.AppendFormatted(num3);
						handler.AppendLiteral(" = new ");
						handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Reflection.ParameterInfo"));
						handler.AppendLiteral("{};");
						stringBuilder81.AppendLine(ref handler);
						stringBuilder = builder;
						StringBuilder stringBuilder82 = stringBuilder;
						handler = new StringBuilder.AppendInterpolatedStringHandler(48, 2, stringBuilder);
						handler.AppendLiteral("            chaos_parameter_");
						handler.AppendFormatted(num3);
						handler.AppendLiteral("->header.type_info = &");
						handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Reflection.ParameterInfo"));
						handler.AppendLiteral(";");
						stringBuilder82.AppendLine(ref handler);
						if (_vtableTypes?.Contains("System.Private.CoreLib/System.Reflection.ParameterInfo") == true)
						{
							stringBuilder = builder;
							StringBuilder stringBuilder_vt6 = stringBuilder;
							handler = new StringBuilder.AppendInterpolatedStringHandler(34, 1, stringBuilder);
							handler.AppendLiteral("        __VAR_PLACEHOLDER__->header.vtable = ");
							handler.AppendFormatted(GetNativeVTableSymbol("System.Private.CoreLib/System.Reflection.ParameterInfo"));
							handler.AppendLiteral(";");
							stringBuilder_vt6.AppendLine(ref handler);
						}
						stringBuilder = builder;
						StringBuilder stringBuilder83 = stringBuilder;
						handler = new StringBuilder.AppendInterpolatedStringHandler(92, 2, stringBuilder);
						handler.AppendLiteral("            chaos_parameter_");
						handler.AppendFormatted(num3);
						handler.AppendLiteral("->runtime_name_value = chaos_reflection_create_string_literal(");
						handler.AppendFormatted(ToCppStringLiteral(value2));
						handler.AppendLiteral(");");
						stringBuilder83.AppendLine(ref handler);
						stringBuilder = builder;
						StringBuilder stringBuilder84 = stringBuilder;
						handler = new StringBuilder.AppendInterpolatedStringHandler(88, 2, stringBuilder);
						handler.AppendLiteral("            chaos_array->elements[");
						handler.AppendFormatted(num3);
						handler.AppendLiteral("] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_parameter_");
						handler.AppendFormatted(num3);
						handler.AppendLiteral(");");
						stringBuilder84.AppendLine(ref handler);
					}
					builder.AppendLine("            return chaos_array_value;");
					builder.AppendLine("        }");
				}
				builder.AppendLine("        default:");
				builder.AppendLine("            return chaos_reflection_create_reference_array(");
				stringBuilder = builder;
				StringBuilder stringBuilder85 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(17, 1, stringBuilder);
				handler.AppendLiteral("                ");
				handler.AppendFormatted(GetRuntimeTypeInfoExpression("System.Private.CoreLib/System.Reflection.ParameterInfo"));
				handler.AppendLiteral(",");
				stringBuilder85.AppendLine(ref handler);
				builder.AppendLine("                static_cast<CHAOS_IL2CPP_SIZE>(0));");
				builder.AppendLine("    }");
				builder.AppendLine("}");
				builder.AppendLine();
			}
		}
		if (flag3)
		{
			builder.AppendLine("CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssembly(CHAOS_IL2CPP_INTPTR chaos_type_value)");
			builder.AppendLine("{");
			stringBuilder = builder;
			StringBuilder stringBuilder86 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(61, 1, stringBuilder);
			handler.AppendLiteral("    auto* chaos_type = reinterpret_cast<");
			handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Type"));
			handler.AppendLiteral("*>(chaos_type_value);");
			stringBuilder86.AppendLine(ref handler);
			builder.AppendLine("    if (chaos_type == nullptr)");
			builder.AppendLine("    {");
			builder.AppendLine("        return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
			builder.AppendLine("    }");
			builder.AppendLine();
			builder.AppendLine("    switch (chaos_type->runtime_type_handle)");
			builder.AppendLine("    {");
			foreach (IGrouping<string, AssemblyReflectionTypeEntry> item9 in _assemblyReflectionSupport.TypeEntries.GroupBy<AssemblyReflectionTypeEntry, string>((AssemblyReflectionTypeEntry entry) => entry.AssemblyName, StringComparer.Ordinal).OrderBy<IGrouping<string, AssemblyReflectionTypeEntry>, string>((IGrouping<string, AssemblyReflectionTypeEntry> group) => group.Key, StringComparer.Ordinal))
			{
				foreach (AssemblyReflectionTypeEntry item10 in item9.OrderBy<AssemblyReflectionTypeEntry, string>((AssemblyReflectionTypeEntry entry) => entry.TypeSubjectId, StringComparer.Ordinal))
				{
					stringBuilder = builder;
					StringBuilder stringBuilder87 = stringBuilder;
					handler = new StringBuilder.AppendInterpolatedStringHandler(14, 1, stringBuilder);
					handler.AppendLiteral("        case ");
					handler.AppendFormatted(GetTypeHandleLiteral(item10.TypeSubjectId));
					handler.AppendLiteral(":");
					stringBuilder87.AppendLine(ref handler);
				}
				builder.AppendLine("        {");
				stringBuilder = builder;
				StringBuilder stringBuilder88 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(42, 1, stringBuilder);
				handler.AppendLiteral("            auto* chaos_assembly = new ");
				handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Reflection.Assembly"));
				handler.AppendLiteral("{};");
				stringBuilder88.AppendLine(ref handler);
				stringBuilder = builder;
				StringBuilder stringBuilder89 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(46, 1, stringBuilder);
				handler.AppendLiteral("            chaos_assembly->header.type_info = &");
				handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Reflection.Assembly"));
				handler.AppendLiteral(";");
				stringBuilder89.AppendLine(ref handler);
				if (_vtableTypes?.Contains("System.Private.CoreLib/System.Reflection.Assembly") == true)
				{
					stringBuilder = builder;
					StringBuilder stringBuilder_vt5 = stringBuilder;
					handler = new StringBuilder.AppendInterpolatedStringHandler(44, 1, stringBuilder);
					handler.AppendLiteral("            chaos_assembly->header.vtable = ");
					handler.AppendFormatted(GetNativeVTableSymbol("System.Private.CoreLib/System.Reflection.Assembly"));
					handler.AppendLiteral(";");
					stringBuilder_vt5.AppendLine(ref handler);
				}
				stringBuilder = builder;
				StringBuilder stringBuilder90 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(99, 1, stringBuilder);
				handler.AppendLiteral("            chaos_assembly->runtime_assembly_name_value = chaos_reflection_create_string_literal(");
				handler.AppendFormatted(ToCppStringLiteral(item9.Key));
				handler.AppendLiteral(");");
				stringBuilder90.AppendLine(ref handler);
				builder.AppendLine("            return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_assembly);");
				builder.AppendLine("        }");
			}
			builder.AppendLine("        default:");
			builder.AppendLine("            return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
			builder.AppendLine("    }");
			builder.AppendLine("}");
			builder.AppendLine();
			builder.AppendLine("CHAOS_IL2CPP_INTPTR chaos_reflection_resolve_type_handle(CHAOS_IL2CPP_INTPTR chaos_assembly_name_value, const char* chaos_type_name) noexcept");
			builder.AppendLine("{");
			builder.AppendLine("    const char* chaos_assembly_name = chaos_reflection_get_string_utf8(chaos_assembly_name_value);");
			builder.AppendLine("    if (chaos_assembly_name == nullptr || chaos_type_name == nullptr)");
			builder.AppendLine("    {");
			builder.AppendLine("        return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
			builder.AppendLine("    }");
			builder.AppendLine();
			foreach (AssemblyReflectionTypeEntry typeEntry3 in _assemblyReflectionSupport.TypeEntries)
			{
				stringBuilder = builder;
				StringBuilder stringBuilder91 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(87, 2, stringBuilder);
				handler.AppendLiteral("    if (CHAOS_IL2CPP_STRCMP(chaos_assembly_name, ");
				handler.AppendFormatted(ToCppStringLiteral(typeEntry3.AssemblyName));
				handler.AppendLiteral(") == 0 && CHAOS_IL2CPP_STRCMP(chaos_type_name, ");
				handler.AppendFormatted(ToCppStringLiteral(typeEntry3.TypeDisplayName));
				handler.AppendLiteral(") == 0)");
				stringBuilder91.AppendLine(ref handler);
				builder.AppendLine("    {");
				stringBuilder = builder;
				StringBuilder stringBuilder92 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(16, 1, stringBuilder);
				handler.AppendLiteral("        return ");
				handler.AppendFormatted(GetTypeHandleLiteral(typeEntry3.TypeSubjectId));
				handler.AppendLiteral(";");
				stringBuilder92.AppendLine(ref handler);
				builder.AppendLine("    }");
				builder.AppendLine();
			}
			builder.AppendLine("    return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
			builder.AppendLine("}");
			builder.AppendLine();
			builder.AppendLine("CHAOS_IL2CPP_INTPTR chaos_reflection_get_type_from_assembly(CHAOS_IL2CPP_INTPTR chaos_assembly_value, CHAOS_IL2CPP_INTPTR chaos_name_value)");
			builder.AppendLine("{");
			stringBuilder = builder;
			StringBuilder stringBuilder93 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(69, 1, stringBuilder);
			handler.AppendLiteral("    auto* chaos_assembly = reinterpret_cast<");
			handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Reflection.Assembly"));
			handler.AppendLiteral("*>(chaos_assembly_value);");
			stringBuilder93.AppendLine(ref handler);
			builder.AppendLine("    if (chaos_assembly == nullptr)");
			builder.AppendLine("    {");
			builder.AppendLine("        return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
			builder.AppendLine("    }");
			builder.AppendLine();
			builder.AppendLine("    const auto chaos_type_handle = chaos_reflection_resolve_type_handle(");
			builder.AppendLine("        chaos_assembly->runtime_assembly_name_value,");
			builder.AppendLine("        chaos_reflection_get_string_utf8(chaos_name_value));");
			builder.AppendLine("    if (chaos_type_handle == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
			builder.AppendLine("    {");
			builder.AppendLine("        return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
			builder.AppendLine("    }");
			builder.AppendLine();
			builder.AppendLine("    return chaos_reflection_create_type_value(chaos_type_handle);");
			builder.AppendLine("}");
			builder.AppendLine();
			builder.AppendLine("CHAOS_IL2CPP_INTPTR ChaosReflectionGetTypeByName(CHAOS_IL2CPP_INTPTR chaos_name_value)");
			builder.AppendLine("{");
			builder.AppendLine("    const char* chaos_name = chaos_reflection_get_string_utf8(chaos_name_value);");
			builder.AppendLine("    if (chaos_name == nullptr)");
			builder.AppendLine("    {");
			builder.AppendLine("        return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
			builder.AppendLine("    }");
			builder.AppendLine();
			foreach (AssemblyReflectionTypeEntry typeEntry4 in _assemblyReflectionSupport.TypeEntries)
			{
				string value3 = typeEntry4.TypeDisplayName + ", " + typeEntry4.AssemblyName;
				stringBuilder = builder;
				StringBuilder stringBuilder94 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(39, 1, stringBuilder);
				handler.AppendLiteral("    if (CHAOS_IL2CPP_STRCMP(chaos_name, ");
				handler.AppendFormatted(ToCppStringLiteral(value3));
				handler.AppendLiteral(") == 0)");
				stringBuilder94.AppendLine(ref handler);
				builder.AppendLine("    {");
				stringBuilder = builder;
				StringBuilder stringBuilder95 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(52, 1, stringBuilder);
				handler.AppendLiteral("        return chaos_reflection_create_type_value(");
				handler.AppendFormatted(GetTypeHandleLiteral(typeEntry4.TypeSubjectId));
				handler.AppendLiteral(");");
				stringBuilder95.AppendLine(ref handler);
				builder.AppendLine("    }");
				builder.AppendLine();
			}
			builder.AppendLine("    return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
			builder.AppendLine("}");
			builder.AppendLine();
			builder.AppendLine("CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssembly_name(CHAOS_IL2CPP_INTPTR chaos_assembly_value)");
			builder.AppendLine("{");
			stringBuilder = builder;
			StringBuilder stringBuilder96 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(69, 1, stringBuilder);
			handler.AppendLiteral("    auto* chaos_assembly = reinterpret_cast<");
			handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Reflection.Assembly"));
			handler.AppendLiteral("*>(chaos_assembly_value);");
			stringBuilder96.AppendLine(ref handler);
			builder.AppendLine("    if (chaos_assembly == nullptr)");
			builder.AppendLine("    {");
			builder.AppendLine("        return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
			builder.AppendLine("    }");
			builder.AppendLine();
			stringBuilder = builder;
			StringBuilder stringBuilder97 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(39, 1, stringBuilder);
			handler.AppendLiteral("    auto* chaos_assembly_name = new ");
			handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Reflection.AssemblyName"));
			handler.AppendLiteral("{};");
			stringBuilder97.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder98 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(43, 1, stringBuilder);
			handler.AppendLiteral("    chaos_assembly_name->header.type_info = &");
			handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Reflection.AssemblyName"));
			handler.AppendLiteral(";");
			stringBuilder98.AppendLine(ref handler);
			if (_vtableTypes?.Contains("System.Private.CoreLib/System.Reflection.AssemblyName") == true)
			{
				stringBuilder = builder;
				StringBuilder stringBuilder_vt4 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(41, 1, stringBuilder);
				handler.AppendLiteral("    chaos_assembly_name->header.vtable = ");
				handler.AppendFormatted(GetNativeVTableSymbol("System.Private.CoreLib/System.Reflection.AssemblyName"));
				handler.AppendLiteral(";");
				stringBuilder_vt4.AppendLine(ref handler);
			}
			builder.AppendLine("    chaos_assembly_name->runtime_name_value = chaos_assembly->runtime_assembly_name_value;");
			builder.AppendLine("    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_assembly_name);");
			builder.AppendLine("}");
			builder.AppendLine();
			builder.AppendLine("CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssembly_name_value(CHAOS_IL2CPP_INTPTR chaos_assembly_name_value) noexcept");
			builder.AppendLine("{");
			stringBuilder = builder;
			StringBuilder stringBuilder99 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(79, 1, stringBuilder);
			handler.AppendLiteral("    auto* chaos_assembly_name = reinterpret_cast<");
			handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Reflection.AssemblyName"));
			handler.AppendLiteral("*>(chaos_assembly_name_value);");
			stringBuilder99.AppendLine(ref handler);
			builder.AppendLine("    return chaos_assembly_name == nullptr");
			builder.AppendLine("        ? static_cast<CHAOS_IL2CPP_INTPTR>(0)");
			builder.AppendLine("        : chaos_assembly_name->runtime_name_value;");
			builder.AppendLine("}");
			builder.AppendLine();
		}
		if (flag2 || flag4 || flag5)
		{
			builder.AppendLine("CHAOS_IL2CPP_INTPTR ChaosReflectionGetTypeFromHandle(CHAOS_IL2CPP_INTPTR chaos_type_handle)");
			builder.AppendLine("{");
			builder.AppendLine("    return chaos_reflection_create_type_value(chaos_type_handle);");
			builder.AppendLine("}");
			builder.AppendLine();
			builder.AppendLine("CHAOS_IL2CPP_INTPTR ChaosReflectionGetTypeHandle(CHAOS_IL2CPP_INTPTR chaos_type_value) noexcept");
			builder.AppendLine("{");
			stringBuilder = builder;
			StringBuilder stringBuilder100 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(61, 1, stringBuilder);
			handler.AppendLiteral("    auto* chaos_type = reinterpret_cast<");
			handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Type"));
			handler.AppendLiteral("*>(chaos_type_value);");
			stringBuilder100.AppendLine(ref handler);
			builder.AppendLine("    return chaos_type->runtime_type_handle;");
			builder.AppendLine("}");
			builder.AppendLine();
			var getObjectTypeModel = new Scriban.Runtime.ScriptObject
			{
				["type_entries"] = _reflectionMemberSupport.TypeEntries
					.OrderBy((ReflectionMemberTypeEntry entry) => entry.TypeSubjectId, StringComparer.Ordinal)
					.Select(entry => new Scriban.Runtime.ScriptObject
					{
						["type_id_symbol"] = GetNativeTypeIdSymbol(entry.TypeSubjectId),
						["type_info_symbol"] = GetNativeTypeInfoSymbol(entry.TypeSubjectId),
						["type_handle_literal"] = GetTypeHandleLiteral(entry.TypeSubjectId),
					})
					.ToArray(),
			};
			builder.AppendLine(
				ScribanTemplateRenderer.RenderTemplate(
					NativeAotTemplateCatalog.GetReflectionGetObjectTypeTemplate(),
					getObjectTypeModel).TrimEnd());
			builder.AppendLine();
			var createInstanceModel = new Scriban.Runtime.ScriptObject
			{
				["type_native_symbol"] = GetNativeTypeSymbol("System.Private.CoreLib/System.Type"),
				["ctor_entries"] = _reflectionMemberSupport.MethodEntries
					.Where((ReflectionMemberMethodEntry entry) => entry.IsConstructor && entry.ParameterNames.Count == 0)
					.OrderBy((ReflectionMemberMethodEntry entry) => entry.DeclaringTypeSubjectId, StringComparer.Ordinal)
					.ThenBy((ReflectionMemberMethodEntry entry) => entry.MethodSubjectId, StringComparer.Ordinal)
					.Select(entry =>
					{
						InvocationTarget? ctorTarget = TryResolveDirectInvocationTarget(entry.MethodSubjectId);
						if (!ctorTarget.HasValue)
						{
							return null;
						}

						InvocationTarget resolvedCtorTarget = ctorTarget.Value;
						if (resolvedCtorTarget.ParameterAbis.Count != 1 ||
							resolvedCtorTarget.ReturnAbi.CarrierKindCode != AotCoreIrAbiCarrierKind.Void)
						{
							return null;
						}

						return new Scriban.Runtime.ScriptObject
						{
							["type_handle_literal"] = GetTypeHandleLiteral(entry.DeclaringTypeSubjectId),
							["native_type_symbol"] = GetNativeTypeSymbol(entry.DeclaringTypeSubjectId),
							["type_id_symbol"] = GetNativeTypeIdSymbol(entry.DeclaringTypeSubjectId),
							["type_info_symbol"] = GetNativeTypeInfoSymbol(entry.DeclaringTypeSubjectId),
							["ctor_symbol"] = resolvedCtorTarget.TargetSymbol,
						};
					})
					.Where(model => model is not null)
					.ToArray()!,
			};
			builder.AppendLine(
				ScribanTemplateRenderer.RenderTemplate(
					NativeAotTemplateCatalog.GetReflectionCreateInstanceTemplate(),
					createInstanceModel).TrimEnd());
			builder.AppendLine();
			builder.AppendLine("CHAOS_IL2CPP_INTPTR chaos_reflection_resolve_method_handle(CHAOS_IL2CPP_INTPTR chaos_type_handle, const char* chaos_method_name) noexcept");
			builder.AppendLine("{");
			builder.AppendLine("    if (chaos_method_name == nullptr)");
			builder.AppendLine("    {");
			builder.AppendLine("        return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
			builder.AppendLine("    }");
			builder.AppendLine();
			builder.AppendLine("    switch (chaos_type_handle)");
			builder.AppendLine("    {");
			foreach (var item11 in _methodsGroupedByDeclaringType)
			{
				if (string.Equals(item11.Key, "System.Private.CoreLib/System.Type", StringComparison.Ordinal) ||
					string.Equals(item11.Key, "System.Private.CoreLib/System.Reflection.MethodInfo", StringComparison.Ordinal))
				{
					continue;
				}
				if (item11.All(m => IsSpecialMethodName(GetMethodName(m.SubjectId))))
				{
					continue;
				}
				stringBuilder = builder;
				StringBuilder stringBuilder101 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(14, 1, stringBuilder);
				handler.AppendLiteral("        case ");
				handler.AppendFormatted(GetTypeHandleLiteral(item11.Key));
				handler.AppendLiteral(":");
				stringBuilder101.AppendLine(ref handler);
				foreach (AotCoreIrMethodArtifact item12 in item11)
				{
					stringBuilder = builder;
					StringBuilder stringBuilder102 = stringBuilder;
					handler = new StringBuilder.AppendInterpolatedStringHandler(54, 1, stringBuilder);
					handler.AppendLiteral("            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, ");
					handler.AppendFormatted(ToCppStringLiteral(GetMethodName(item12.SubjectId)));
					handler.AppendLiteral(") == 0)");
					stringBuilder102.AppendLine(ref handler);
					builder.AppendLine("            {");
					stringBuilder = builder;
					StringBuilder stringBuilder103 = stringBuilder;
					handler = new StringBuilder.AppendInterpolatedStringHandler(24, 1, stringBuilder);
					handler.AppendLiteral("                return ");
					handler.AppendFormatted(GetMethodHandleLiteral(item12.SubjectId));
					handler.AppendLiteral(";");
					stringBuilder103.AppendLine(ref handler);
					builder.AppendLine("            }");
					builder.AppendLine();
				}
				builder.AppendLine("            break;");
			}
			builder.AppendLine("        default:");
			builder.AppendLine("            break;");
			builder.AppendLine("    }");
			builder.AppendLine();
			builder.AppendLine("    return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
			builder.AppendLine("}");
			builder.AppendLine();
			builder.AppendLine("CHAOS_IL2CPP_INTPTR ChaosReflectionGetMethod(CHAOS_IL2CPP_INTPTR chaos_type_value, CHAOS_IL2CPP_INTPTR chaos_name_value, CHAOS_IL2CPP_INT32 chaos_binding_flags = CHAOS_IL2CPP_INT32(0))");
			builder.AppendLine("{");
			builder.AppendLine("    (void)chaos_binding_flags;");
			stringBuilder = builder;
			StringBuilder stringBuilder104 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(61, 1, stringBuilder);
			handler.AppendLiteral("    auto* chaos_type = reinterpret_cast<");
			handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Type"));
			handler.AppendLiteral("*>(chaos_type_value);");
			stringBuilder104.AppendLine(ref handler);
			builder.AppendLine("    const auto* chaos_method_name = chaos_reflection_get_string_utf8(chaos_name_value);");
			builder.AppendLine("    const auto chaos_method_handle = chaos_reflection_resolve_method_handle(chaos_type->runtime_type_handle, chaos_method_name);");
			builder.AppendLine("    if (chaos_method_handle == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
			builder.AppendLine("    {");
			builder.AppendLine("        return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
			builder.AppendLine("    }");
			builder.AppendLine();
			stringBuilder = builder;
			StringBuilder stringBuilder105 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(32, 1, stringBuilder);
			handler.AppendLiteral("    auto* chaos_method = new ");
			handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Reflection.MethodInfo"));
			handler.AppendLiteral("{};");
			stringBuilder105.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder106 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(36, 1, stringBuilder);
			handler.AppendLiteral("    chaos_method->header.type_info = &");
			handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Reflection.MethodInfo"));
			handler.AppendLiteral(";");
			stringBuilder106.AppendLine(ref handler);
			if (_vtableTypes?.Contains("System.Private.CoreLib/System.Reflection.MethodInfo") == true)
			{
				stringBuilder = builder;
				StringBuilder stringBuilder_vt3 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(34, 1, stringBuilder);
				handler.AppendLiteral("    chaos_method->header.vtable = ");
				handler.AppendFormatted(GetNativeVTableSymbol("System.Private.CoreLib/System.Reflection.MethodInfo"));
				handler.AppendLiteral(";");
				stringBuilder_vt3.AppendLine(ref handler);
			}
			builder.AppendLine("    chaos_method->declaring_type_handle = chaos_type->runtime_type_handle;");
			builder.AppendLine("    chaos_method->runtime_method_handle = chaos_method_handle;");
			builder.AppendLine("    chaos_method->generic_definition_method_handle = chaos_method_handle;");
			builder.AppendLine("    chaos_method->runtime_name_value = ChaosReflectionGetMethod_name_value_from_handle(chaos_method_handle);");
			builder.AppendLine("    chaos_method->runtime_metadata_token_value = ChaosReflectionGetMethod_metadata_token_from_handle(chaos_method_handle);");
			builder.AppendLine("    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_method);");
			builder.AppendLine("}");
			builder.AppendLine();
			builder.AppendLine("CHAOS_IL2CPP_INTPTR chaos_reflection_create_closed_generic_method_handle(CHAOS_IL2CPP_INTPTR chaos_definition_handle, CHAOS_IL2CPP_INTPTR chaos_type_handle) noexcept");
			builder.AppendLine("{");
			builder.AppendLine("    CHAOS_IL2CPP_UINT32 hash = 2166136261u;");
			builder.AppendLine("    const auto chaos_definition_bits = static_cast<CHAOS_IL2CPP_UINT64>(chaos_definition_handle);");
			builder.AppendLine("    const auto chaos_type_bits = static_cast<CHAOS_IL2CPP_UINT64>(chaos_type_handle);");
			builder.AppendLine("    hash ^= static_cast<CHAOS_IL2CPP_UINT32>(chaos_definition_bits & 0xFFFFFFFFu);");
			builder.AppendLine("    hash *= 16777619u;");
			builder.AppendLine("    hash ^= static_cast<CHAOS_IL2CPP_UINT32>(chaos_definition_bits >> 32);");
			builder.AppendLine("    hash *= 16777619u;");
			builder.AppendLine("    hash ^= static_cast<CHAOS_IL2CPP_UINT32>(chaos_type_bits & 0xFFFFFFFFu);");
			builder.AppendLine("    hash *= 16777619u;");
			builder.AppendLine("    hash ^= static_cast<CHAOS_IL2CPP_UINT32>(chaos_type_bits >> 32);");
			builder.AppendLine("    hash *= 16777619u;");
			builder.AppendLine("    auto row_index = hash & 0x00FFFFFFu;");
			builder.AppendLine("    if (row_index == 0u)");
			builder.AppendLine("    {");
			builder.AppendLine("        row_index = 1u;");
			builder.AppendLine("    }");
			builder.AppendLine();
			builder.AppendLine("    return static_cast<CHAOS_IL2CPP_INTPTR>(0x06000000u | row_index);");
			builder.AppendLine("}");
			builder.AppendLine();
			builder.AppendLine("CHAOS_IL2CPP_INTPTR ChaosReflectionMakeGenericMethod(CHAOS_IL2CPP_INTPTR chaos_method_value, CHAOS_IL2CPP_INTPTR chaos_type_array_value)");
			builder.AppendLine("{");
			builder.AppendLine("    if (chaos_method_value == static_cast<CHAOS_IL2CPP_INTPTR>(0) || chaos_type_array_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
			builder.AppendLine("    {");
			builder.AppendLine("        return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
			builder.AppendLine("    }");
			builder.AppendLine();
			stringBuilder = builder;
			StringBuilder stringBuilder107 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(65, 1, stringBuilder);
			handler.AppendLiteral("    auto* chaos_method = reinterpret_cast<");
			handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Reflection.MethodInfo"));
			handler.AppendLiteral("*>(chaos_method_value);");
			stringBuilder107.AppendLine(ref handler);
			builder.AppendLine("    auto* chaos_type_array = reinterpret_cast<chaos_managed_array*>(chaos_type_array_value);");
			builder.AppendLine("    if (chaos_type_array->length <= static_cast<CHAOS_IL2CPP_INTPTR>(0))");
			builder.AppendLine("    {");
			builder.AppendLine("        return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
			builder.AppendLine("    }");
			builder.AppendLine();
			builder.AppendLine("    const auto chaos_type_argument_value = chaos_type_array->elements[0];");
			builder.AppendLine("    if (chaos_type_argument_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
			builder.AppendLine("    {");
			builder.AppendLine("        return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
			builder.AppendLine("    }");
			builder.AppendLine();
			stringBuilder = builder;
			StringBuilder stringBuilder108 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(79, 1, stringBuilder);
			handler.AppendLiteral("    auto* chaos_type_argument = reinterpret_cast<");
			handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Type"));
			handler.AppendLiteral("*>(chaos_type_argument_value);");
			stringBuilder108.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder109 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(39, 1, stringBuilder);
			handler.AppendLiteral("    auto* chaos_closed_method = new ");
			handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Reflection.MethodInfo"));
			handler.AppendLiteral("{};");
			stringBuilder109.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder110 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(43, 1, stringBuilder);
			handler.AppendLiteral("    chaos_closed_method->header.type_info = &");
			handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Reflection.MethodInfo"));
			handler.AppendLiteral(";");
			stringBuilder110.AppendLine(ref handler);
			if (_vtableTypes?.Contains("System.Private.CoreLib/System.Reflection.MethodInfo") == true)
			{
				stringBuilder = builder;
				StringBuilder stringBuilder_vt2 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(41, 1, stringBuilder);
				handler.AppendLiteral("    chaos_closed_method->header.vtable = ");
				handler.AppendFormatted(GetNativeVTableSymbol("System.Private.CoreLib/System.Reflection.MethodInfo"));
				handler.AppendLiteral(";");
				stringBuilder_vt2.AppendLine(ref handler);
			}
			builder.AppendLine("    chaos_closed_method->declaring_type_handle = chaos_method->declaring_type_handle;");
			builder.AppendLine("    chaos_closed_method->generic_definition_method_handle = chaos_method->generic_definition_method_handle != static_cast<CHAOS_IL2CPP_INTPTR>(0)");
			builder.AppendLine("        ? chaos_method->generic_definition_method_handle");
			builder.AppendLine("        : chaos_method->runtime_method_handle;");
			builder.AppendLine("    chaos_closed_method->generic_argument_type_handle = chaos_type_argument->runtime_type_handle;");
			builder.AppendLine("    chaos_closed_method->runtime_method_handle = chaos_reflection_create_closed_generic_method_handle(");
			builder.AppendLine("        chaos_closed_method->generic_definition_method_handle,");
			builder.AppendLine("        chaos_closed_method->generic_argument_type_handle);");
			builder.AppendLine("    chaos_closed_method->runtime_name_value = chaos_method->runtime_name_value == static_cast<CHAOS_IL2CPP_INTPTR>(0)");
			builder.AppendLine("        ? ChaosReflectionGetMethod_name_value_from_handle(chaos_closed_method->generic_definition_method_handle)");
			builder.AppendLine("        : chaos_method->runtime_name_value;");
			builder.AppendLine("    chaos_closed_method->runtime_metadata_token_value = chaos_method->runtime_metadata_token_value != 0");
			builder.AppendLine("        ? chaos_method->runtime_metadata_token_value");
			builder.AppendLine("        : ChaosReflectionGetMethod_metadata_token_from_handle(chaos_closed_method->generic_definition_method_handle);");
			builder.AppendLine("    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_closed_method);");
			builder.AppendLine("}");
			builder.AppendLine();
			builder.AppendLine("CHAOS_IL2CPP_INTPTR ChaosReflectionGetMethod_handle(CHAOS_IL2CPP_INTPTR chaos_method_value) noexcept");
			builder.AppendLine("{");
			stringBuilder = builder;
			StringBuilder stringBuilder111 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(65, 1, stringBuilder);
			handler.AppendLiteral("    auto* chaos_method = reinterpret_cast<");
			handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Reflection.MethodInfo"));
			handler.AppendLiteral("*>(chaos_method_value);");
			stringBuilder111.AppendLine(ref handler);
			builder.AppendLine("    return chaos_method->runtime_method_handle;");
			builder.AppendLine("}");
			builder.AppendLine();
			builder.AppendLine("CHAOS_IL2CPP_INTPTR ChaosReflectionInvokeMethod(CHAOS_IL2CPP_INTPTR chaos_method_value, CHAOS_IL2CPP_INTPTR chaos_target_value, CHAOS_IL2CPP_INTPTR chaos_args_array_value)");
			builder.AppendLine("{");
			builder.AppendLine("    if (chaos_method_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
			builder.AppendLine("    {");
			builder.AppendLine("        return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
			builder.AppendLine("    }");
			builder.AppendLine();
			stringBuilder = builder;
			StringBuilder stringBuilder112 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(65, 1, stringBuilder);
			handler.AppendLiteral("    auto* chaos_method = reinterpret_cast<");
			handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Reflection.MethodInfo"));
			handler.AppendLiteral("*>(chaos_method_value);");
			stringBuilder112.AppendLine(ref handler);
			builder.AppendLine("    auto* chaos_args = reinterpret_cast<chaos_managed_array*>(chaos_args_array_value);");
			builder.AppendLine();
			foreach (AotCoreIrMethodArtifact item13 in _genericStaticMethodCandidates)
			{
				stringBuilder = builder;
				StringBuilder stringBuilder113 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(59, 1, stringBuilder);
				handler.AppendLiteral("    if (chaos_method->generic_definition_method_handle == ");
				handler.AppendFormatted(GetMethodHandleLiteral(item13.SubjectId));
				handler.AppendLiteral(")");
				stringBuilder113.AppendLine(ref handler);
				builder.AppendLine("    {");
				builder.AppendLine("        if (chaos_target_value != static_cast<CHAOS_IL2CPP_INTPTR>(0) || chaos_args == nullptr || chaos_args->length < static_cast<CHAOS_IL2CPP_INTPTR>(1))");
				builder.AppendLine("        {");
				builder.AppendLine("            return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
				builder.AppendLine("        }");
				builder.AppendLine();
				if (referenceTypeSubjectIds.Contains("System.Private.CoreLib/System.String"))
				{
					stringBuilder = builder;
					StringBuilder stringBuilder114 = stringBuilder;
					handler = new StringBuilder.AppendInterpolatedStringHandler(59, 1, stringBuilder);
					handler.AppendLiteral("        if (chaos_method->generic_argument_type_handle == ");
					handler.AppendFormatted(GetTypeHandleLiteral("System.Private.CoreLib/System.String"));
					handler.AppendLiteral(")");
					stringBuilder114.AppendLine(ref handler);
					builder.AppendLine("        {");
					stringBuilder = builder;
					StringBuilder stringBuilder115 = stringBuilder;
					handler = new StringBuilder.AppendInterpolatedStringHandler(45, 1, stringBuilder);
					handler.AppendLiteral("            return ");
					handler.AppendFormatted(item13.NativeSymbol);
					handler.AppendLiteral("(chaos_args->elements[0]);");
					stringBuilder115.AppendLine(ref handler);
					builder.AppendLine("        }");
					builder.AppendLine();
				}
				foreach (string item14 in boxedTypeSubjectIds.Where((string candidate) => !IsStructuredValueTypeSubjectId(candidate)).OrderBy<string, string>((string candidate) => candidate, StringComparer.Ordinal))
				{
					stringBuilder = builder;
					StringBuilder stringBuilder116 = stringBuilder;
					handler = new StringBuilder.AppendInterpolatedStringHandler(59, 1, stringBuilder);
					handler.AppendLiteral("        if (chaos_method->generic_argument_type_handle == ");
					handler.AppendFormatted(GetTypeHandleLiteral(item14));
					handler.AppendLiteral(")");
					stringBuilder116.AppendLine(ref handler);
					builder.AppendLine("        {");
					builder.AppendLine("            const auto chaos_arg_value = chaos_args->elements[0];");
					builder.AppendLine("            if (chaos_arg_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
					builder.AppendLine("            {");
					builder.AppendLine("                return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
					builder.AppendLine("            }");
					builder.AppendLine();
					stringBuilder = builder;
					StringBuilder stringBuilder117 = stringBuilder;
					handler = new StringBuilder.AppendInterpolatedStringHandler(73, 1, stringBuilder);
					handler.AppendLiteral("            auto* chaos_boxed_arg = reinterpret_cast<");
					handler.AppendFormatted(GetNativeBoxTypeSymbol(item14));
					handler.AppendLiteral("*>(chaos_arg_value);");
					stringBuilder117.AppendLine(ref handler);
					stringBuilder = builder;
					StringBuilder stringBuilder118 = stringBuilder;
					handler = new StringBuilder.AppendInterpolatedStringHandler(63, 1, stringBuilder);
					handler.AppendLiteral("            const auto chaos_result = ");
					handler.AppendFormatted(item13.NativeSymbol);
					handler.AppendLiteral("(chaos_boxed_arg->value);");
					stringBuilder118.AppendLine(ref handler);
					stringBuilder = builder;
					StringBuilder stringBuilder119 = stringBuilder;
					handler = new StringBuilder.AppendInterpolatedStringHandler(46, 1, stringBuilder);
					handler.AppendLiteral("            auto* chaos_boxed_result = new ");
					handler.AppendFormatted(GetNativeBoxTypeSymbol(item14));
					handler.AppendLiteral("{};");
					stringBuilder119.AppendLine(ref handler);
					stringBuilder = builder;
					StringBuilder stringBuilder120 = stringBuilder;
					handler = new StringBuilder.AppendInterpolatedStringHandler(50, 1, stringBuilder);
					handler.AppendLiteral("            chaos_boxed_result->header.type_info = &");
					handler.AppendFormatted(GetNativeBoxTypeInfoSymbol(item14));
					handler.AppendLiteral(";");
					stringBuilder120.AppendLine(ref handler);
					builder.AppendLine("            chaos_boxed_result->value = chaos_result;");
					builder.AppendLine("            return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed_result);");
					builder.AppendLine("        }");
					builder.AppendLine();
				}
				builder.AppendLine("        return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
				builder.AppendLine("    }");
				builder.AppendLine();
			}
			builder.AppendLine("    return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
			builder.AppendLine("}");
			builder.AppendLine();
		}
		if (_customAttributeSupport.QueryAttributeTypeByCallee.Count <= 0 && !_customAttributeSupport.UsesMemberInfoIsDefined)
		{
			return;
		}
		builder.AppendLine("CHAOS_IL2CPP_INTPTR ChaosReflectionGetCustomAttribute(CHAOS_IL2CPP_INTPTR chaos_member_value, CHAOS_IL2CPP_INTPTR chaos_attribute_type_handle)");
		builder.AppendLine("{");
		builder.AppendLine("    if (chaos_member_value == static_cast<CHAOS_IL2CPP_INTPTR>(0) || chaos_attribute_type_handle == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
		builder.AppendLine("    {");
		builder.AppendLine("        return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
		builder.AppendLine("    }");
		builder.AppendLine("");
		builder.AppendLine("    auto* chaos_header = reinterpret_cast<chaos_object_header*>(chaos_member_value);");
		builder.AppendLine("    CHAOS_IL2CPP_INTPTR chaos_member_handle = 0;");
		builder.AppendLine("    CHAOS_IL2CPP_UINT8 chaos_member_kind = 0;");
		stringBuilder = builder;
		StringBuilder stringBuilder_attr1 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(34, 1, stringBuilder);
		handler.AppendLiteral("    if (chaos_header->type_info == &");
		handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Type"));
		handler.AppendLiteral("\n        || chaos_header->type_info->stable_id == (&");
		handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Type"));
		handler.AppendLiteral(")->stable_id)");
		stringBuilder_attr1.AppendLine(ref handler);
		builder.AppendLine("    {");
		stringBuilder = builder;
		StringBuilder stringBuilder_attr2 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(67, 1, stringBuilder);
		handler.AppendLiteral("        auto* chaos_type = reinterpret_cast<");
		handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Type"));
		handler.AppendLiteral("*>(chaos_member_value);");
		stringBuilder_attr2.AppendLine(ref handler);
		builder.AppendLine("        chaos_member_kind = 1;");
		builder.AppendLine("        chaos_member_handle = chaos_type->runtime_type_handle;");
		builder.AppendLine("    }");
		builder.AppendLine("");
		stringBuilder = builder;
		StringBuilder stringBuilder_attr3 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(39, 1, stringBuilder);
		handler.AppendLiteral("    else if (chaos_header->type_info == &");
		handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Reflection.MethodInfo"));
		handler.AppendLiteral("\n        || chaos_header->type_info->stable_id == (&");
		handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.Reflection.MethodInfo"));
		handler.AppendLiteral(")->stable_id)");
		stringBuilder_attr3.AppendLine(ref handler);
		builder.AppendLine("    {");
		stringBuilder = builder;
		StringBuilder stringBuilder_attr4 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(69, 1, stringBuilder);
		handler.AppendLiteral("        auto* chaos_method = reinterpret_cast<");
		handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.Reflection.MethodInfo"));
		handler.AppendLiteral("*>(chaos_member_value);");
		stringBuilder_attr4.AppendLine(ref handler);
		builder.AppendLine("        chaos_member_kind = 2;");
		builder.AppendLine("        chaos_member_handle = chaos_method->runtime_method_handle;");
		builder.AppendLine("    }");
		builder.AppendLine("    else");
		builder.AppendLine("    {");
		builder.AppendLine("        return static_cast<CHAOS_IL2CPP_INTPTR>(0);");
		builder.AppendLine("    }");
		builder.AppendLine("");
		builder.AppendLine("    return ChaosGetCustomAttributeFromBlob(chaos_member_kind, chaos_member_handle, chaos_attribute_type_handle);");
		builder.AppendLine("}");
		builder.AppendLine("");
	}

	private void EmitExceptionMetadataHelpers(StringBuilder builder, IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods)
	{
		if (UsesReachableInstruction(reachableMethods, (AotCoreIrInstructionArtifact instruction) => string.Equals(instruction.Callee, "System.Private.CoreLib/System.Exception::.ctor(System.String)", StringComparison.Ordinal) || string.Equals(instruction.Callee, "System.Private.CoreLib/System.InvalidOperationException::.ctor(System.String)", StringComparison.Ordinal) || string.Equals(instruction.Callee, "System.Private.CoreLib/System.ArgumentOutOfRangeException::.ctor(System.String,System.String)", StringComparison.Ordinal) || string.Equals(instruction.Callee, "System.Private.CoreLib/System.Exception::get_Message()", StringComparison.Ordinal) || string.Equals(instruction.Callee, "System.Private.CoreLib/System.ArgumentException::get_ParamName()", StringComparison.Ordinal)))
		{
			builder.AppendLine("struct chaos_exception_metadata_entry");
			builder.AppendLine("{");
			builder.AppendLine("    CHAOS_IL2CPP_INTPTR object_value = 0;");
			builder.AppendLine("    CHAOS_IL2CPP_INTPTR message_value = 0;");
			builder.AppendLine("    CHAOS_IL2CPP_INTPTR param_name_value = 0;");
			builder.AppendLine("};");
			builder.AppendLine();
			builder.AppendLine("CHAOS_IL2CPP_VECTOR(chaos_exception_metadata_entry> chaos_exception_metadata_entries{};");
			builder.AppendLine();
			builder.AppendLine("chaos_exception_metadata_entry* chaos_reflection_get_or_add_exception_metadata(CHAOS_IL2CPP_INTPTR chaos_exception_value)");
			builder.AppendLine("{");
			builder.AppendLine("    if (chaos_exception_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
			builder.AppendLine("    {");
			builder.AppendLine("        CHAOS_IL2CPP_ABORT();");
			builder.AppendLine("    }");
			builder.AppendLine();
			builder.AppendLine("    for (auto& chaos_entry : chaos_exception_metadata_entries)");
			builder.AppendLine("    {");
			builder.AppendLine("        if (chaos_entry.object_value == chaos_exception_value)");
			builder.AppendLine("        {");
			builder.AppendLine("            return &chaos_entry;");
			builder.AppendLine("        }");
			builder.AppendLine("    }");
			builder.AppendLine();
			builder.AppendLine("    chaos_exception_metadata_entries.push_back(chaos_exception_metadata_entry{");
			builder.AppendLine("        chaos_exception_value,");
			builder.AppendLine("        static_cast<CHAOS_IL2CPP_INTPTR>(0),");
			builder.AppendLine("        static_cast<CHAOS_IL2CPP_INTPTR>(0),");
			builder.AppendLine("    });");
			builder.AppendLine("    return &chaos_exception_metadata_entries.back();");
			builder.AppendLine("}");
			builder.AppendLine();
			builder.AppendLine("const chaos_exception_metadata_entry* chaos_reflection_find_exception_metadata(CHAOS_IL2CPP_INTPTR chaos_exception_value) noexcept");
			builder.AppendLine("{");
			builder.AppendLine("    if (chaos_exception_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
			builder.AppendLine("    {");
			builder.AppendLine("        return nullptr;");
			builder.AppendLine("    }");
			builder.AppendLine();
			builder.AppendLine("    for (const auto& chaos_entry : chaos_exception_metadata_entries)");
			builder.AppendLine("    {");
			builder.AppendLine("        if (chaos_entry.object_value == chaos_exception_value)");
			builder.AppendLine("        {");
			builder.AppendLine("            return &chaos_entry;");
			builder.AppendLine("        }");
			builder.AppendLine("    }");
			builder.AppendLine();
			builder.AppendLine("    return nullptr;");
			builder.AppendLine("}");
			builder.AppendLine();
			builder.AppendLine("void ChaosReflectionSetExceptionMetadata(");
			builder.AppendLine("    CHAOS_IL2CPP_INTPTR chaos_exception_value,");
			builder.AppendLine("    CHAOS_IL2CPP_INTPTR chaos_message_value,");
			builder.AppendLine("    CHAOS_IL2CPP_INTPTR chaos_param_name_value)");
			builder.AppendLine("{");
			builder.AppendLine("    auto* chaos_entry = chaos_reflection_get_or_add_exception_metadata(chaos_exception_value);");
			builder.AppendLine("    chaos_entry->message_value = chaos_message_value;");
			builder.AppendLine("    chaos_entry->param_name_value = chaos_param_name_value;");
			builder.AppendLine("}");
			builder.AppendLine();
			builder.AppendLine("CHAOS_IL2CPP_INTPTR ChaosReflectionGetExceptionMessage(CHAOS_IL2CPP_INTPTR chaos_exception_value) noexcept");
			builder.AppendLine("{");
			builder.AppendLine("    const auto* chaos_entry = chaos_reflection_find_exception_metadata(chaos_exception_value);");
			builder.AppendLine("    return chaos_entry == nullptr ? static_cast<CHAOS_IL2CPP_INTPTR>(0) : chaos_entry->message_value;");
			builder.AppendLine("}");
			builder.AppendLine();
			builder.AppendLine("CHAOS_IL2CPP_INTPTR chaos_reflection_get_exception_param_name(CHAOS_IL2CPP_INTPTR chaos_exception_value) noexcept");
			builder.AppendLine("{");
			builder.AppendLine("    const auto* chaos_entry = chaos_reflection_find_exception_metadata(chaos_exception_value);");
			builder.AppendLine("    return chaos_entry == nullptr ? static_cast<CHAOS_IL2CPP_INTPTR>(0) : chaos_entry->param_name_value;");
			builder.AppendLine("}");
			builder.AppendLine();
		}
	}

	private bool RequiresManagedStringFactory(IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods)
	{
		if (_customAttributeSupport.RequiresStringSupport)
		{
			return true;
		}
		string? elementTypeDisplayName;
		return UsesReachableInstruction(reachableMethods, (AotCoreIrInstructionArtifact instruction) => MatchesMethodSubject(instruction.Callee ?? string.Empty, "System.Private.CoreLib/System.String", "Concat", "System.String", "System.String") || MatchesMethodSubject(instruction.Callee ?? string.Empty, "System.Private.CoreLib/System.String", "Concat", "System.String", "System.String", "System.String", "System.String") || TryGetStringJoinEnumerableElementType(instruction.Callee ?? string.Empty, out elementTypeDisplayName) || MatchesMethodSubject(instruction.Callee ?? string.Empty, "System.Private.CoreLib/System.ArgumentOutOfRangeException", ".ctor", "System.String", "System.String") || MatchesMethodSubject(instruction.Callee ?? string.Empty, "System.Private.CoreLib/System.Int32", "ToString") || MatchesMethodSubject(instruction.Callee ?? string.Empty, "System.Runtime.InteropServices/Marshal", "PtrToStringUTF8", "System.IntPtr") || IsAssemblyReflectionHelperSubjectId(instruction.Callee ?? string.Empty) || IsTypeReflectionHelperSubjectId(instruction.Callee ?? string.Empty) || IsReflectionMemberHelperSubjectId(instruction.Callee ?? string.Empty) || MatchesMethodSubject(instruction.Callee ?? string.Empty, "System.Private.CoreLib/System.Single", "ToString", "System.String") || MatchesMethodSubject(instruction.Callee ?? string.Empty, "System.Private.CoreLib/System.Double", "ToString", "System.String") || IsDefaultInterpolatedStringHandlerHelperSubjectId(instruction.Callee ?? string.Empty));
	}

	private bool RequiresManagedStringRuntime(IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods)
	{
		if (RequiresManagedStringFactory(reachableMethods))
		{
			return true;
		}
		string? elementTypeDisplayName;
		return UsesReachableInstruction(reachableMethods, (AotCoreIrInstructionArtifact instruction) => TryGetStringJoinEnumerableElementType(instruction.Callee ?? string.Empty, out elementTypeDisplayName) || MatchesMethodSubject(instruction.Callee ?? string.Empty, "System.Private.CoreLib/System.String", "StartsWith", "System.String", "System.StringComparison") || MatchesMethodSubject(instruction.Callee ?? string.Empty, "System.Private.CoreLib/System.String", "Contains", "System.String", "System.StringComparison"));
	}

}
