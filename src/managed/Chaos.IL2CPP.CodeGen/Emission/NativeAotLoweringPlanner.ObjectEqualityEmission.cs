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
	private void EmitObjectEqualityHelpers(StringBuilder builder, IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods, IReadOnlySet<string> referenceTypeSubjectIds, IReadOnlySet<string> boxedTypeSubjectIds)
	{
		if (!UsesReachableInstruction(reachableMethods, (AotCoreIrInstructionArtifact instruction) => MatchesMethodSubject(instruction.Callee ?? string.Empty, "System.Private.CoreLib/System.Object", "Equals", "System.Object") || MatchesMethodSubject(instruction.Callee ?? string.Empty, "System.Private.CoreLib/System.String", "op_Equality", "System.String", "System.String") || IsCollectionRuntimeHelperSubjectId(instruction.Callee ?? string.Empty)))
		{
			return;
		}
		builder.AppendLine("bool chaos_object_equals(CHAOS_IL2CPP_INTPTR chaos_left_value, CHAOS_IL2CPP_INTPTR chaos_right_value) noexcept");
		builder.AppendLine("{");
		builder.AppendLine("    // StringId fast path: tagged integers compare directly (O(1)).");
		builder.AppendLine("    if (chaos_is_string_id(chaos_left_value) && chaos_is_string_id(chaos_right_value))");
		builder.AppendLine("    {");
		builder.AppendLine("        return chaos_left_value == chaos_right_value;");
		builder.AppendLine("    }");
		builder.AppendLine();
		builder.AppendLine("    // Mixed StringId vs pointer: never equal (different representations).");
		builder.AppendLine("    if (chaos_is_string_id(chaos_left_value) != chaos_is_string_id(chaos_right_value))");
		builder.AppendLine("    {");
		builder.AppendLine("        return false;");
		builder.AppendLine("    }");
		builder.AppendLine();
		builder.AppendLine("    if (chaos_left_value == chaos_right_value)");
		builder.AppendLine("    {");
		builder.AppendLine("        return true;");
		builder.AppendLine("    }");
		builder.AppendLine();
		builder.AppendLine("    if (chaos_left_value == static_cast<CHAOS_IL2CPP_INTPTR>(0) || chaos_right_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
		builder.AppendLine("    {");
		builder.AppendLine("        return false;");
		builder.AppendLine("    }");
		builder.AppendLine();
		builder.AppendLine("    auto* chaos_left_header = reinterpret_cast<chaos_object_header*>(chaos_left_value);");
		builder.AppendLine("    auto* chaos_right_header = reinterpret_cast<chaos_object_header*>(chaos_right_value);");
		builder.AppendLine("    if (chaos_left_header->type_id != chaos_right_header->type_id)");
		builder.AppendLine("    {");
		builder.AppendLine("        return false;");
		builder.AppendLine("    }");
		builder.AppendLine();
		if (referenceTypeSubjectIds.Contains("System.Private.CoreLib/System.String"))
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder2 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(39, 1, stringBuilder);
			handler.AppendLiteral("    if (chaos_left_header->type_id == ");
			handler.AppendFormatted(GetNativeTypeIdSymbol("System.Private.CoreLib/System.String"));
			handler.AppendLiteral(")");
			stringBuilder2.AppendLine(ref handler);
			builder.AppendLine("    {");
			stringBuilder = builder;
			StringBuilder stringBuilder3 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(72, 1, stringBuilder);
			handler.AppendLiteral("        auto* chaos_left_string = reinterpret_cast<");
			handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.String"));
			handler.AppendLiteral("*>(chaos_left_value);");
			stringBuilder3.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder4 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(74, 1, stringBuilder);
			handler.AppendLiteral("        auto* chaos_right_string = reinterpret_cast<");
			handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.String"));
			handler.AppendLiteral("*>(chaos_right_value);");
			stringBuilder4.AppendLine(ref handler);
			builder.AppendLine("        if (chaos_left_string->length != chaos_right_string->length)");
			builder.AppendLine("        {");
			builder.AppendLine("            return false;");
			builder.AppendLine("        }");
			builder.AppendLine();
			builder.AppendLine("        if (chaos_left_string->utf8_data == nullptr || chaos_right_string->utf8_data == nullptr)");
			builder.AppendLine("        {");
			builder.AppendLine("            return chaos_left_string->utf8_data == chaos_right_string->utf8_data;");
			builder.AppendLine("        }");
			builder.AppendLine();
			builder.AppendLine("        return CHAOS_IL2CPP_STRCMP(chaos_left_string->utf8_data, chaos_right_string->utf8_data) == 0;");
			builder.AppendLine("    }");
			builder.AppendLine();
		}
		builder.AppendLine("    switch (chaos_left_header->type_id)");
		builder.AppendLine("    {");
		foreach (string item in boxedTypeSubjectIds.OrderBy<string, string>((string value) => value, StringComparer.Ordinal))
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder5 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(14, 1, stringBuilder);
			handler.AppendLiteral("        case ");
			handler.AppendFormatted(GetNativeBoxTypeIdSymbol(item));
			handler.AppendLiteral(":");
			stringBuilder5.AppendLine(ref handler);
			if (IsStructuredValueTypeSubjectId(item))
			{
				stringBuilder = builder;
				StringBuilder stringBuilder6 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(141, 3, stringBuilder);
				handler.AppendLiteral("            return CHAOS_IL2CPP_MEMCMP(&reinterpret_cast<");
				handler.AppendFormatted(GetNativeBoxTypeSymbol(item));
				handler.AppendLiteral("*>(chaos_left_value)->value, &reinterpret_cast<");
				handler.AppendFormatted(GetNativeBoxTypeSymbol(item));
				handler.AppendLiteral("*>(chaos_right_value)->value, sizeof(");
				handler.AppendFormatted(GetNativeValueTypeSymbol(item));
				handler.AppendLiteral(")) == 0;");
				stringBuilder6.AppendLine(ref handler);
			}
			else
			{
				stringBuilder = builder;
				StringBuilder stringBuilder7 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(113, 2, stringBuilder);
				handler.AppendLiteral("            return reinterpret_cast<");
				handler.AppendFormatted(GetNativeBoxTypeSymbol(item));
				handler.AppendLiteral("*>(chaos_left_value)->value == reinterpret_cast<");
				handler.AppendFormatted(GetNativeBoxTypeSymbol(item));
				handler.AppendLiteral("*>(chaos_right_value)->value;");
				stringBuilder7.AppendLine(ref handler);
			}
		}
		builder.AppendLine("        default:");
		builder.AppendLine("            return false;");
		builder.AppendLine("    }");
		builder.AppendLine("}");
		builder.AppendLine();
	}

}
