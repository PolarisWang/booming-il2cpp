using System;
using System.Collections;
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
	private bool TryCreateStringRuntimeHelperDefinition(string callee, out ExternalRuntimeHelperDefinition? helperDefinition)
	{
		helperDefinition = null;
		if (string.Equals(callee, "System.Private.CoreLib/System.String::Concat(System.String,System.String)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), "extern \"C\" std::intptr_t " + GetExternalRuntimeHelperSymbol(callee) + "(std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1)\n{\n    return chaos_reflection_concat_string_pair_values(chaos_arg_0, chaos_arg_1);\n}", new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
			{
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)
			}), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0, 1 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.String::Concat(System.String,System.String,System.String,System.String)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), "extern \"C\" std::intptr_t " + GetExternalRuntimeHelperSymbol(callee) + "(std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1, std::intptr_t chaos_arg_2, std::intptr_t chaos_arg_3)\n{\n    const auto chaos_left_pair = chaos_reflection_concat_string_pair_values(chaos_arg_0, chaos_arg_1);\n    const auto chaos_right_pair = chaos_reflection_concat_string_pair_values(chaos_arg_2, chaos_arg_3);\n    return chaos_reflection_concat_string_pair_values(chaos_left_pair, chaos_right_pair);\n}", new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[4]
			{
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)
			}), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0, 1, 2, 3 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.String::op_Equality(System.String,System.String)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), "extern \"C\" std::intptr_t " + GetExternalRuntimeHelperSymbol(callee) + "(std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1)\n{\n    return chaos_object_equals(chaos_arg_0, chaos_arg_1)\n        ? static_cast<std::intptr_t>(1)\n        : static_cast<std::intptr_t>(0);\n}", new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
			{
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)
			}), CreateNativeIntAbiSlot(), new HashSet<int> { 0, 1 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.String::StartsWith(System.String,System.StringComparison)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), $"extern \"C\" std::intptr_t {GetExternalRuntimeHelperSymbol(callee)}(std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1, std::int32_t chaos_arg_2)\n{{\n    if (chaos_arg_2 != {4})\n    {{\n        std::abort();\n    }}\n\n    if (chaos_arg_1 == static_cast<std::intptr_t>(0))\n    {{\n        std::abort();\n    }}\n\n    auto* chaos_instance = reinterpret_cast<{GetNativeTypeSymbol("System.Private.CoreLib/System.String")}*>(chaos_arg_0);\n    auto* chaos_prefix = reinterpret_cast<{GetNativeTypeSymbol("System.Private.CoreLib/System.String")}*>(chaos_arg_1);\n    const auto chaos_instance_length = static_cast<std::size_t>(chaos_instance->length);\n    const auto chaos_prefix_length = static_cast<std::size_t>(chaos_prefix->length);\n    if (chaos_prefix_length > chaos_instance_length)\n    {{\n        return static_cast<std::intptr_t>(0);\n    }}\n\n    if (chaos_prefix_length == 0)\n    {{\n        return static_cast<std::intptr_t>(1);\n    }}\n\n    const char* chaos_instance_utf8 = chaos_reflection_get_string_utf8(chaos_arg_0);\n    const char* chaos_prefix_utf8 = chaos_reflection_get_string_utf8(chaos_arg_1);\n    if ((chaos_instance_utf8 == nullptr && chaos_instance_length != 0) ||\n        (chaos_prefix_utf8 == nullptr && chaos_prefix_length != 0))\n    {{\n        std::abort();\n    }}\n\n    return std::memcmp(chaos_instance_utf8, chaos_prefix_utf8, chaos_prefix_length) == 0\n        ? static_cast<std::intptr_t>(1)\n        : static_cast<std::intptr_t>(0);\n}}", new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
			{
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
				CreateInt32AbiSlot()
			}), CreateNativeIntAbiSlot(), new HashSet<int> { 0, 1 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.String::Contains(System.String,System.StringComparison)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), $"extern \"C\" std::intptr_t {GetExternalRuntimeHelperSymbol(callee)}(std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1, std::int32_t chaos_arg_2)\n{{\n    if (chaos_arg_2 != {4})\n    {{\n        std::abort();\n    }}\n\n    if (chaos_arg_1 == static_cast<std::intptr_t>(0))\n    {{\n        std::abort();\n    }}\n\n    auto* chaos_instance = reinterpret_cast<{GetNativeTypeSymbol("System.Private.CoreLib/System.String")}*>(chaos_arg_0);\n    auto* chaos_value = reinterpret_cast<{GetNativeTypeSymbol("System.Private.CoreLib/System.String")}*>(chaos_arg_1);\n    const auto chaos_instance_length = static_cast<std::size_t>(chaos_instance->length);\n    const auto chaos_value_length = static_cast<std::size_t>(chaos_value->length);\n    if (chaos_value_length == 0)\n    {{\n        return static_cast<std::intptr_t>(1);\n    }}\n\n    if (chaos_value_length > chaos_instance_length)\n    {{\n        return static_cast<std::intptr_t>(0);\n    }}\n\n    const char* chaos_instance_utf8 = chaos_reflection_get_string_utf8(chaos_arg_0);\n    const char* chaos_value_utf8 = chaos_reflection_get_string_utf8(chaos_arg_1);\n    if ((chaos_instance_utf8 == nullptr && chaos_instance_length != 0) ||\n        (chaos_value_utf8 == nullptr && chaos_value_length != 0))\n    {{\n        std::abort();\n    }}\n\n    const auto chaos_last_start = chaos_instance_length - chaos_value_length;\n    for (std::size_t chaos_index = 0; chaos_index <= chaos_last_start; chaos_index++)\n    {{\n        if (std::memcmp(chaos_instance_utf8 + chaos_index, chaos_value_utf8, chaos_value_length) == 0)\n        {{\n            return static_cast<std::intptr_t>(1);\n        }}\n    }}\n\n    return static_cast<std::intptr_t>(0);\n}}", new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
			{
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
				CreateInt32AbiSlot()
			}), CreateNativeIntAbiSlot(), new HashSet<int> { 0, 1 });
			return true;
		}
		if (TryParseStringJoinGenericEnumerableElementType(callee, out string elementTypeDisplayName) && string.Equals(elementTypeDisplayName, "System.Int32", StringComparison.Ordinal))
		{
			helperDefinition = CreateStringJoinInt32EnumerableRuntimeHelperDefinition(callee, ResolveEnumerableJoinSupportVariants(elementTypeDisplayName));
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.String::Join(System.String,System.Collections.Generic.IEnumerable<System.String>)", StringComparison.Ordinal))
		{
			helperDefinition = CreateStringJoinStringEnumerableRuntimeHelperDefinition(callee, ResolveEnumerableJoinSupportVariants("System.String"));
			return true;
		}
		return false;
	}

	private ExternalRuntimeHelperDefinition CreateStringJoinInt32EnumerableRuntimeHelperDefinition(string callee, IReadOnlyList<EnumerableJoinSupportVariant> variants)
	{
		StringBuilder stringBuilder = new StringBuilder();
		StringBuilder stringBuilder2 = stringBuilder;
		StringBuilder stringBuilder3 = stringBuilder2;
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(79, 1, stringBuilder2);
		handler.AppendLiteral("extern \"C\" std::intptr_t ");
		handler.AppendFormatted(GetExternalRuntimeHelperSymbol(callee));
		handler.AppendLiteral("(std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1)");
		stringBuilder3.AppendLine(ref handler);
		stringBuilder.AppendLine("{");
		stringBuilder.AppendLine("    if (chaos_arg_1 == static_cast<std::intptr_t>(0))");
		stringBuilder.AppendLine("    {");
		stringBuilder.AppendLine("        std::abort();");
		stringBuilder.AppendLine("    }");
		stringBuilder.AppendLine();
		stringBuilder.AppendLine("    const char* chaos_separator_utf8 = chaos_arg_0 == static_cast<std::intptr_t>(0)");
		stringBuilder.AppendLine("        ? \"\"");
		stringBuilder.AppendLine("        : chaos_reflection_get_string_utf8(chaos_arg_0);");
		stringBuilder.AppendLine("    std::string chaos_joined;");
		stringBuilder.AppendLine("    bool chaos_first = true;");
		stringBuilder.AppendLine("    auto chaos_append_separator = [&]()");
		stringBuilder.AppendLine("    {");
		stringBuilder.AppendLine("        if (chaos_first)");
		stringBuilder.AppendLine("        {");
		stringBuilder.AppendLine("            chaos_first = false;");
		stringBuilder.AppendLine("            return;");
		stringBuilder.AppendLine("        }");
		stringBuilder.AppendLine();
		stringBuilder.AppendLine("        if (chaos_separator_utf8 != nullptr)");
		stringBuilder.AppendLine("        {");
		stringBuilder.AppendLine("            chaos_joined += chaos_separator_utf8;");
		stringBuilder.AppendLine("        }");
		stringBuilder.AppendLine("    };");
		stringBuilder.AppendLine();
		stringBuilder.AppendLine("    auto* chaos_enumerable_header = reinterpret_cast<chaos_object_header*>(chaos_arg_1);");
		foreach (EnumerableJoinSupportVariant item in variants.OrderBy<EnumerableJoinSupportVariant, string>((EnumerableJoinSupportVariant item) => item.EnumerableTypeSubjectId, StringComparer.Ordinal))
		{
			stringBuilder2 = stringBuilder;
			StringBuilder stringBuilder4 = stringBuilder2;
			handler = new StringBuilder.AppendInterpolatedStringHandler(45, 1, stringBuilder2);
			handler.AppendLiteral("    if (chaos_enumerable_header->type_id == ");
			handler.AppendFormatted(GetNativeTypeIdSymbol(item.EnumerableTypeSubjectId));
			handler.AppendLiteral(")");
			stringBuilder4.AppendLine(ref handler);
			stringBuilder.AppendLine("    {");
			stringBuilder2 = stringBuilder;
			StringBuilder stringBuilder5 = stringBuilder2;
			handler = new StringBuilder.AppendInterpolatedStringHandler(52, 1, stringBuilder2);
			handler.AppendLiteral("        const auto chaos_enumerator = ");
			handler.AppendFormatted(item.GetEnumeratorMethod.NativeSymbol);
			handler.AppendLiteral("(chaos_arg_1);");
			stringBuilder5.AppendLine(ref handler);
			stringBuilder.AppendLine("        while (true)");
			stringBuilder.AppendLine("        {");
			stringBuilder2 = stringBuilder;
			StringBuilder stringBuilder6 = stringBuilder2;
			handler = new StringBuilder.AppendInterpolatedStringHandler(59, 1, stringBuilder2);
			handler.AppendLiteral("            const auto chaos_has_next = ");
			handler.AppendFormatted(item.MoveNextMethod.NativeSymbol);
			handler.AppendLiteral("(chaos_enumerator);");
			stringBuilder6.AppendLine(ref handler);
			stringBuilder.AppendLine("            if (chaos_has_next == static_cast<std::intptr_t>(0))");
			stringBuilder.AppendLine("            {");
			stringBuilder.AppendLine("                break;");
			stringBuilder.AppendLine("            }");
			stringBuilder.AppendLine();
			stringBuilder.AppendLine("            chaos_append_separator();");
			stringBuilder2 = stringBuilder;
			StringBuilder stringBuilder7 = stringBuilder2;
			handler = new StringBuilder.AppendInterpolatedStringHandler(58, 1, stringBuilder2);
			handler.AppendLiteral("            const auto chaos_current = ");
			handler.AppendFormatted(item.GetCurrentMethod.NativeSymbol);
			handler.AppendLiteral("(chaos_enumerator);");
			stringBuilder7.AppendLine(ref handler);
			stringBuilder.AppendLine("            char chaos_buffer[64]{};");
			stringBuilder.AppendLine("            std::snprintf(chaos_buffer, sizeof(chaos_buffer), \"%d\", static_cast<std::int32_t>(chaos_current));");
			stringBuilder.AppendLine("            chaos_joined += chaos_buffer;");
			stringBuilder.AppendLine("        }");
			stringBuilder.AppendLine();
			stringBuilder.AppendLine("        return chaos_reflection_create_string_utf8_copy(chaos_joined.c_str(), chaos_joined.size());");
			stringBuilder.AppendLine("    }");
			stringBuilder.AppendLine();
		}
		stringBuilder.AppendLine("    std::abort();");
		stringBuilder.AppendLine("}");
		return new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), stringBuilder.ToString().TrimEnd(), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
		{
			CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
			CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)
		}), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0, 1 });
	}

	private ExternalRuntimeHelperDefinition CreateStringJoinStringEnumerableRuntimeHelperDefinition(string callee, IReadOnlyList<EnumerableJoinSupportVariant> variants)
	{
		StringBuilder stringBuilder = new StringBuilder();
		StringBuilder stringBuilder2 = stringBuilder;
		StringBuilder stringBuilder3 = stringBuilder2;
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(79, 1, stringBuilder2);
		handler.AppendLiteral("extern \"C\" std::intptr_t ");
		handler.AppendFormatted(GetExternalRuntimeHelperSymbol(callee));
		handler.AppendLiteral("(std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1)");
		stringBuilder3.AppendLine(ref handler);
		stringBuilder.AppendLine("{");
		stringBuilder.AppendLine("    if (chaos_arg_1 == static_cast<std::intptr_t>(0))");
		stringBuilder.AppendLine("    {");
		stringBuilder.AppendLine("        std::abort();");
		stringBuilder.AppendLine("    }");
		stringBuilder.AppendLine();
		stringBuilder.AppendLine("    const char* chaos_separator_utf8 = chaos_arg_0 == static_cast<std::intptr_t>(0)");
		stringBuilder.AppendLine("        ? \"\"");
		stringBuilder.AppendLine("        : chaos_reflection_get_string_utf8(chaos_arg_0);");
		stringBuilder.AppendLine("    std::string chaos_joined;");
		stringBuilder.AppendLine("    bool chaos_first = true;");
		stringBuilder.AppendLine("    auto chaos_append_separator = [&]()");
		stringBuilder.AppendLine("    {");
		stringBuilder.AppendLine("        if (chaos_first)");
		stringBuilder.AppendLine("        {");
		stringBuilder.AppendLine("            chaos_first = false;");
		stringBuilder.AppendLine("            return;");
		stringBuilder.AppendLine("        }");
		stringBuilder.AppendLine();
		stringBuilder.AppendLine("        if (chaos_separator_utf8 != nullptr)");
		stringBuilder.AppendLine("        {");
		stringBuilder.AppendLine("            chaos_joined += chaos_separator_utf8;");
		stringBuilder.AppendLine("        }");
		stringBuilder.AppendLine("    };");
		stringBuilder.AppendLine();
		stringBuilder.AppendLine("    auto* chaos_enumerable_header = reinterpret_cast<chaos_object_header*>(chaos_arg_1);");
		stringBuilder.AppendLine("    if (chaos_enumerable_header->type_id == chaos_type_id_managed_array)");
		stringBuilder.AppendLine("    {");
		stringBuilder.AppendLine("        auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_arg_1);");
		stringBuilder2 = stringBuilder;
		StringBuilder stringBuilder4 = stringBuilder2;
		handler = new StringBuilder.AppendInterpolatedStringHandler(50, 1, stringBuilder2);
		handler.AppendLiteral("        if (chaos_array->element_type_shape != ");
		handler.AppendFormatted(GetNativeTypeShapeValue(AotCoreIrTypeShapeKind.ReferenceType));
		handler.AppendLiteral(" ||");
		stringBuilder4.AppendLine(ref handler);
		stringBuilder2 = stringBuilder;
		StringBuilder stringBuilder5 = stringBuilder2;
		handler = new StringBuilder.AppendInterpolatedStringHandler(45, 1, stringBuilder2);
		handler.AppendLiteral("            chaos_array->element_type_id != ");
		handler.AppendFormatted(GetNativeTypeIdSymbol("System.Private.CoreLib/System.String"));
		handler.AppendLiteral(")");
		stringBuilder5.AppendLine(ref handler);
		stringBuilder.AppendLine("        {");
		stringBuilder.AppendLine("            std::abort();");
		stringBuilder.AppendLine("        }");
		stringBuilder.AppendLine();
		stringBuilder.AppendLine("        for (std::intptr_t chaos_index = 0; chaos_index < chaos_array->length; chaos_index++)");
		stringBuilder.AppendLine("        {");
		stringBuilder.AppendLine("            chaos_append_separator();");
		stringBuilder.AppendLine("            const auto chaos_current = chaos_array->elements[static_cast<std::size_t>(chaos_index)];");
		stringBuilder.AppendLine("            if (chaos_current == static_cast<std::intptr_t>(0))");
		stringBuilder.AppendLine("            {");
		stringBuilder.AppendLine("                continue;");
		stringBuilder.AppendLine("            }");
		stringBuilder.AppendLine();
		stringBuilder.AppendLine("            const char* chaos_current_utf8 = chaos_reflection_get_string_utf8(chaos_current);");
		stringBuilder.AppendLine("            if (chaos_current_utf8 != nullptr)");
		stringBuilder.AppendLine("            {");
		stringBuilder.AppendLine("                chaos_joined += chaos_current_utf8;");
		stringBuilder.AppendLine("            }");
		stringBuilder.AppendLine("        }");
		stringBuilder.AppendLine();
		stringBuilder.AppendLine("        return chaos_reflection_create_string_utf8_copy(chaos_joined.c_str(), chaos_joined.size());");
		stringBuilder.AppendLine("    }");
		stringBuilder.AppendLine();
		foreach (EnumerableJoinSupportVariant item in variants.OrderBy<EnumerableJoinSupportVariant, string>((EnumerableJoinSupportVariant item) => item.EnumerableTypeSubjectId, StringComparer.Ordinal))
		{
			stringBuilder2 = stringBuilder;
			StringBuilder stringBuilder6 = stringBuilder2;
			handler = new StringBuilder.AppendInterpolatedStringHandler(45, 1, stringBuilder2);
			handler.AppendLiteral("    if (chaos_enumerable_header->type_id == ");
			handler.AppendFormatted(GetNativeTypeIdSymbol(item.EnumerableTypeSubjectId));
			handler.AppendLiteral(")");
			stringBuilder6.AppendLine(ref handler);
			stringBuilder.AppendLine("    {");
			stringBuilder2 = stringBuilder;
			StringBuilder stringBuilder7 = stringBuilder2;
			handler = new StringBuilder.AppendInterpolatedStringHandler(52, 1, stringBuilder2);
			handler.AppendLiteral("        const auto chaos_enumerator = ");
			handler.AppendFormatted(item.GetEnumeratorMethod.NativeSymbol);
			handler.AppendLiteral("(chaos_arg_1);");
			stringBuilder7.AppendLine(ref handler);
			stringBuilder.AppendLine("        while (true)");
			stringBuilder.AppendLine("        {");
			stringBuilder2 = stringBuilder;
			StringBuilder stringBuilder8 = stringBuilder2;
			handler = new StringBuilder.AppendInterpolatedStringHandler(59, 1, stringBuilder2);
			handler.AppendLiteral("            const auto chaos_has_next = ");
			handler.AppendFormatted(item.MoveNextMethod.NativeSymbol);
			handler.AppendLiteral("(chaos_enumerator);");
			stringBuilder8.AppendLine(ref handler);
			stringBuilder.AppendLine("            if (chaos_has_next == static_cast<std::intptr_t>(0))");
			stringBuilder.AppendLine("            {");
			stringBuilder.AppendLine("                break;");
			stringBuilder.AppendLine("            }");
			stringBuilder.AppendLine();
			stringBuilder.AppendLine("            chaos_append_separator();");
			stringBuilder2 = stringBuilder;
			StringBuilder stringBuilder9 = stringBuilder2;
			handler = new StringBuilder.AppendInterpolatedStringHandler(58, 1, stringBuilder2);
			handler.AppendLiteral("            const auto chaos_current = ");
			handler.AppendFormatted(item.GetCurrentMethod.NativeSymbol);
			handler.AppendLiteral("(chaos_enumerator);");
			stringBuilder9.AppendLine(ref handler);
			stringBuilder.AppendLine("            if (chaos_current == static_cast<std::intptr_t>(0))");
			stringBuilder.AppendLine("            {");
			stringBuilder.AppendLine("                continue;");
			stringBuilder.AppendLine("            }");
			stringBuilder.AppendLine();
			stringBuilder.AppendLine("            const char* chaos_current_utf8 = chaos_reflection_get_string_utf8(chaos_current);");
			stringBuilder.AppendLine("            if (chaos_current_utf8 != nullptr)");
			stringBuilder.AppendLine("            {");
			stringBuilder.AppendLine("                chaos_joined += chaos_current_utf8;");
			stringBuilder.AppendLine("            }");
			stringBuilder.AppendLine("        }");
			stringBuilder.AppendLine();
			stringBuilder.AppendLine("        return chaos_reflection_create_string_utf8_copy(chaos_joined.c_str(), chaos_joined.size());");
			stringBuilder.AppendLine("    }");
			stringBuilder.AppendLine();
		}
		stringBuilder.AppendLine("    std::abort();");
		stringBuilder.AppendLine("}");
		return new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), stringBuilder.ToString().TrimEnd(), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
		{
			CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
			CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)
		}), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0, 1 });
	}

	private bool TryCreateExceptionRuntimeHelperDefinition(string callee, out ExternalRuntimeHelperDefinition? helperDefinition)
	{
		helperDefinition = null;
		if (string.Equals(callee, "System.Private.CoreLib/System.Exception::.ctor(System.String)", StringComparison.Ordinal) || string.Equals(callee, "System.Private.CoreLib/System.InvalidOperationException::.ctor(System.String)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), "extern \"C\" void " + GetExternalRuntimeHelperSymbol(callee) + "(std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1)\n{\n    chaos_reflection_set_exception_metadata(\n        chaos_arg_0,\n        chaos_arg_1,\n        static_cast<std::intptr_t>(0));\n}", new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
			{
				CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)
			}), new AotCoreIrAbiSlotArtifact
			{
				CarrierKindCode = AotCoreIrAbiCarrierKind.Void,
				TypeShape = (AotCoreIrTypeShapeKind)0
			}, new HashSet<int> { 0, 1 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.ArgumentOutOfRangeException::.ctor(System.String,System.String)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), "extern \"C\" void " + GetExternalRuntimeHelperSymbol(callee) + "(std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1, std::intptr_t chaos_arg_2)\n{\n    auto chaos_message_value = chaos_arg_2;\n    if (chaos_arg_1 != static_cast<std::intptr_t>(0))\n    {\n        chaos_message_value =\n            chaos_message_value == static_cast<std::intptr_t>(0)\n                ? chaos_arg_1\n                : chaos_reflection_concat_string_pair_values(chaos_message_value, chaos_arg_1);\n    }\n\n    chaos_reflection_set_exception_metadata(\n        chaos_arg_0,\n        chaos_message_value,\n        chaos_arg_1);\n}", new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
			{
				CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)
			}), new AotCoreIrAbiSlotArtifact
			{
				CarrierKindCode = AotCoreIrAbiCarrierKind.Void,
				TypeShape = (AotCoreIrTypeShapeKind)0
			}, new HashSet<int> { 0, 1, 2 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Exception::get_Message()", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), "extern \"C\" std::intptr_t " + GetExternalRuntimeHelperSymbol(callee) + "(std::intptr_t chaos_arg_0)\n{\n    return chaos_reflection_get_exception_message(chaos_arg_0);\n}", new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.ArgumentException::get_ParamName()", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), "extern \"C\" std::intptr_t " + GetExternalRuntimeHelperSymbol(callee) + "(std::intptr_t chaos_arg_0)\n{\n    return chaos_reflection_get_exception_param_name(chaos_arg_0);\n}", new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0 });
			return true;
		}
		return false;
	}

	private bool TryCreateNumericFormattingRuntimeHelperDefinition(string callee, out ExternalRuntimeHelperDefinition? helperDefinition)
	{
		helperDefinition = null;
		if (string.Equals(callee, "System.Private.CoreLib/System.Int32::ToString()", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), "extern \"C\" std::intptr_t " + GetExternalRuntimeHelperSymbol(callee) + "(std::intptr_t chaos_arg_0)\n{\n    auto* chaos_value_slot = chaos_resolve_native_int_slot(chaos_arg_0);\n    char chaos_buffer[64]{};\n    std::snprintf(chaos_buffer, sizeof(chaos_buffer), \"%d\", static_cast<std::int32_t>(*chaos_value_slot));\n    return chaos_reflection_create_string_literal(chaos_buffer);\n}", new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Single::ToString(System.String)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), $"extern \"C\" std::intptr_t {GetExternalRuntimeHelperSymbol(callee)}(std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1)\n{{\n    const char* chaos_format_utf8 = nullptr;\n    if (chaos_arg_1 != static_cast<std::intptr_t>(0))\n    {{\n        auto* chaos_format = reinterpret_cast<{GetNativeTypeSymbol("System.Private.CoreLib/System.String")}*>(chaos_arg_1);\n        chaos_format_utf8 = chaos_format->utf8_data;\n    }}\n\n    if (chaos_format_utf8 != nullptr && std::strcmp(chaos_format_utf8, \"F1\") != 0)\n    {{\n        std::abort();\n    }}\n\n    char chaos_buffer[64]{{}};\n    std::snprintf(chaos_buffer, sizeof(chaos_buffer), \"%.1f\", static_cast<double>(chaos_load_float32(chaos_arg_0)));\n    return chaos_reflection_create_string_literal(chaos_buffer);\n}}", new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
			{
				CreateNativeIntAbiSlot(),
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)
			}), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0, 1 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Double::ToString(System.String)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), $"extern \"C\" std::intptr_t {GetExternalRuntimeHelperSymbol(callee)}(std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1)\n{{\n    const char* chaos_format_utf8 = nullptr;\n    if (chaos_arg_1 != static_cast<std::intptr_t>(0))\n    {{\n        auto* chaos_format = reinterpret_cast<{GetNativeTypeSymbol("System.Private.CoreLib/System.String")}*>(chaos_arg_1);\n        chaos_format_utf8 = chaos_format->utf8_data;\n    }}\n\n    if (chaos_format_utf8 != nullptr && std::strcmp(chaos_format_utf8, \"F1\") != 0)\n    {{\n        std::abort();\n    }}\n\n    char chaos_buffer[64]{{}};\n    std::snprintf(chaos_buffer, sizeof(chaos_buffer), \"%.1f\", chaos_load_float64(chaos_arg_0));\n    return chaos_reflection_create_string_literal(chaos_buffer);\n}}", new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
			{
				CreateNativeIntAbiSlot(),
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)
			}), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0, 1 });
			return true;
		}
		return false;
	}

	private bool TryCreateInterpolatedStringRuntimeHelperDefinition(string callee, out ExternalRuntimeHelperDefinition? helperDefinition)
	{
		helperDefinition = null;
		if (string.Equals(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::.ctor(System.Int32,System.Int32)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), "extern \"C\" void " + GetExternalRuntimeHelperSymbol(callee) + "(std::intptr_t chaos_arg_0, std::int32_t chaos_arg_1, std::int32_t chaos_arg_2)\n{\n    (void)chaos_arg_1;\n    (void)chaos_arg_2;\n    chaos_default_interpolated_string_handler_reset(chaos_arg_0);\n}", new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
			{
				CreateNativeIntAbiSlot(),
				CreateInt32AbiSlot(),
				CreateInt32AbiSlot()
			}), new AotCoreIrAbiSlotArtifact
			{
				CarrierKindCode = AotCoreIrAbiCarrierKind.Void,
				TypeShape = (AotCoreIrTypeShapeKind)0
			}, new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted(System.String)", StringComparison.Ordinal) || string.Equals(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendLiteral(System.String)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), "extern \"C\" void " + GetExternalRuntimeHelperSymbol(callee) + "(std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1)\n{\n    chaos_default_interpolated_string_handler_append_string(chaos_arg_0, chaos_arg_1);\n}", new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
			{
				CreateNativeIntAbiSlot(),
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)
			}), new AotCoreIrAbiSlotArtifact
			{
				CarrierKindCode = AotCoreIrAbiCarrierKind.Void,
				TypeShape = (AotCoreIrTypeShapeKind)0
			}, new HashSet<int> { 0, 1 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::AppendFormatted<System.Int32>(System.Int32)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), "extern \"C\" void " + GetExternalRuntimeHelperSymbol(callee) + "(std::intptr_t chaos_arg_0, std::int32_t chaos_arg_1)\n{\n    chaos_default_interpolated_string_handler_append_int32(chaos_arg_0, chaos_arg_1);\n}", new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
			{
				CreateNativeIntAbiSlot(),
				CreateInt32AbiSlot()
			}), new AotCoreIrAbiSlotArtifact
			{
				CarrierKindCode = AotCoreIrAbiCarrierKind.Void,
				TypeShape = (AotCoreIrTypeShapeKind)0
			}, new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::ToStringAndClear()", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), "extern \"C\" std::intptr_t " + GetExternalRuntimeHelperSymbol(callee) + "(std::intptr_t chaos_arg_0)\n{\n    return chaos_default_interpolated_string_handler_to_string_and_clear(chaos_arg_0);\n}", new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0 });
			return true;
		}
		return false;
	}

	private bool TryCreatePlatformRuntimeHelperDefinition(string callee, out ExternalRuntimeHelperDefinition? helperDefinition)
	{
		helperDefinition = null;
		string text = null;
		if (string.Equals(callee, "System.Private.CoreLib/System.OperatingSystem::IsWindows()", StringComparison.Ordinal))
		{
			text = "#if defined(_WIN32)\n    return static_cast<std::intptr_t>(1);\n#else\n    return static_cast<std::intptr_t>(0);\n#endif";
		}
		else if (string.Equals(callee, "System.Private.CoreLib/System.OperatingSystem::IsLinux()", StringComparison.Ordinal))
		{
			text = "#if defined(__linux__)\n    return static_cast<std::intptr_t>(1);\n#else\n    return static_cast<std::intptr_t>(0);\n#endif";
		}
		else if (string.Equals(callee, "System.Private.CoreLib/System.OperatingSystem::IsMacOS()", StringComparison.Ordinal))
		{
			text = "#if defined(__APPLE__)\n    return static_cast<std::intptr_t>(1);\n#else\n    return static_cast<std::intptr_t>(0);\n#endif";
		}
		if (text == null)
		{
			return false;
		}
		helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), $"extern \"C\" std::intptr_t {GetExternalRuntimeHelperSymbol(callee)}()\n{{\n{text}\n}}", Array.Empty<AotCoreIrAbiSlotArtifact>(), CreateNativeIntAbiSlot(), EmptyRawArgumentIndices);
		return true;
	}

	private bool TryCreateDelegateInteropRuntimeHelperDefinition(string callee, out ExternalRuntimeHelperDefinition? helperDefinition)
	{
		helperDefinition = null;
		if (string.Equals(callee, "System.Private.CoreLib/System.Delegate::Combine(System.Delegate,System.Delegate)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), "extern \"C\" std::intptr_t " + GetExternalRuntimeHelperSymbol(callee) + "(std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1)\n{\n    return chaos_delegate_combine(chaos_arg_0, chaos_arg_1);\n}", new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
			{
				CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
				CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)
			}), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Delegate", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0, 1 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Delegate::Remove(System.Delegate,System.Delegate)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), "extern \"C\" std::intptr_t " + GetExternalRuntimeHelperSymbol(callee) + "(std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1)\n{\n    return chaos_delegate_remove(chaos_arg_0, chaos_arg_1);\n}", new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
			{
				CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
				CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)
			}), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Delegate", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0, 1 });
			return true;
		}
		if (TryReadSingleGenericTypeArgument(callee, "System.Runtime.InteropServices/Marshal::GetFunctionPointerForDelegate<", out string typeSubjectId))
		{
			if (!TryResolveReferenceTypeSubjectId(typeSubjectId, out typeSubjectId) || !IsDelegateTypeSubjectId(typeSubjectId, _referenceTypeBaseSubjectIds))
			{
				return false;
			}
			var helperSymbol = GetExternalRuntimeHelperSymbol(callee);
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, helperSymbol, $$"""
extern "C" std::intptr_t {{helperSymbol}}(std::intptr_t chaos_arg_0)
{
    struct chaos_delegate_function_pointer_bridge
    {
        std::uint64_t magic = 0;
        std::intptr_t delegate_target = 0;
        std::intptr_t delegate_method_ptr = 0;
        std::intptr_t delegate_invocation_list = 0;
        std::intptr_t delegate_invocation_count = 0;
    };

    constexpr std::uint64_t chaos_delegate_function_pointer_bridge_magic = 0x4348414F535F4446ULL;

    if (chaos_arg_0 == static_cast<std::intptr_t>(0))
    {
        return static_cast<std::intptr_t>(0);
    }

    auto* chaos_delegate = reinterpret_cast<{{GetNativeTypeSymbol(typeSubjectId)}}*>(chaos_arg_0);
    if (chaos_delegate->chaos_delegate_target == static_cast<std::intptr_t>(0) &&
        chaos_delegate->chaos_delegate_invocation_count == static_cast<std::intptr_t>(0))
    {
        return chaos_delegate->chaos_delegate_method_ptr;
    }

    auto* chaos_bridge = new chaos_delegate_function_pointer_bridge{};
    chaos_bridge->magic = chaos_delegate_function_pointer_bridge_magic;
    chaos_bridge->delegate_target = chaos_delegate->chaos_delegate_target;
    chaos_bridge->delegate_method_ptr = chaos_delegate->chaos_delegate_method_ptr;
    chaos_bridge->delegate_invocation_list = chaos_delegate->chaos_delegate_invocation_list;
    chaos_bridge->delegate_invocation_count = chaos_delegate->chaos_delegate_invocation_count;
    return reinterpret_cast<std::intptr_t>(chaos_bridge);
}
""", new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot(typeSubjectId, AotCoreIrTypeShapeKind.ReferenceType)), CreateNativeIntAbiSlot(), new HashSet<int> { 0 });
			return true;
		}
		if (TryReadSingleGenericTypeArgument(callee, "System.Runtime.InteropServices/Marshal::GetDelegateForFunctionPointer<", out typeSubjectId))
		{
			if (!TryResolveReferenceTypeSubjectId(typeSubjectId, out typeSubjectId) || !IsDelegateTypeSubjectId(typeSubjectId, _referenceTypeBaseSubjectIds))
			{
				return false;
			}
			var helperSymbol = GetExternalRuntimeHelperSymbol(callee);
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, helperSymbol, $$"""
extern "C" std::intptr_t {{helperSymbol}}(std::intptr_t chaos_arg_0)
{
    struct chaos_delegate_function_pointer_bridge
    {
        std::uint64_t magic = 0;
        std::intptr_t delegate_target = 0;
        std::intptr_t delegate_method_ptr = 0;
        std::intptr_t delegate_invocation_list = 0;
        std::intptr_t delegate_invocation_count = 0;
    };

    constexpr std::uint64_t chaos_delegate_function_pointer_bridge_magic = 0x4348414F535F4446ULL;

    auto* chaos_delegate = new {{GetNativeTypeSymbol(typeSubjectId)}}{};
    chaos_delegate->header.type_id = {{GetNativeTypeIdSymbol(typeSubjectId)}};
    if (chaos_arg_0 != static_cast<std::intptr_t>(0))
    {
        const auto* chaos_bridge = reinterpret_cast<const chaos_delegate_function_pointer_bridge*>(chaos_arg_0);
        if (chaos_bridge->magic == chaos_delegate_function_pointer_bridge_magic)
        {
            chaos_delegate->chaos_delegate_target = chaos_bridge->delegate_target;
            chaos_delegate->chaos_delegate_method_ptr = chaos_bridge->delegate_method_ptr;
            chaos_delegate->chaos_delegate_invocation_list = chaos_bridge->delegate_invocation_list;
            chaos_delegate->chaos_delegate_invocation_count = chaos_bridge->delegate_invocation_count;
            return reinterpret_cast<std::intptr_t>(chaos_delegate);
        }
    }

    chaos_delegate->chaos_delegate_target = static_cast<std::intptr_t>(0);
    chaos_delegate->chaos_delegate_method_ptr = chaos_arg_0;
    return reinterpret_cast<std::intptr_t>(chaos_delegate);
}
""", new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()), CreateNativeIntAbiSlot(typeSubjectId, AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0 });
			return true;
		}
		return false;
	}

	private bool TryCreateMarshalingRuntimeHelperDefinition(string callee, out ExternalRuntimeHelperDefinition? helperDefinition)
	{
		helperDefinition = null;
		if (string.Equals(callee, "System.Runtime.InteropServices/Marshal::StringToCoTaskMemUTF8(System.String)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), $"extern \"C\" std::intptr_t {GetExternalRuntimeHelperSymbol(callee)}(std::intptr_t chaos_arg_0)\n{{\n    if (chaos_arg_0 == static_cast<std::intptr_t>(0))\n    {{\n        return static_cast<std::intptr_t>(0);\n    }}\n\n    auto* chaos_string = reinterpret_cast<{GetNativeTypeSymbol("System.Private.CoreLib/System.String")}*>(chaos_arg_0);\n    const auto chaos_length = static_cast<std::size_t>(chaos_string->length);\n    const auto* chaos_utf8_data = chaos_string->utf8_data;\n    if (chaos_length != 0 && chaos_utf8_data == nullptr)\n    {{\n        std::abort();\n    }}\n\n    auto* chaos_buffer = static_cast<char*>(std::malloc(chaos_length + 1));\n    if (chaos_buffer == nullptr)\n    {{\n        std::abort();\n    }}\n\n    if (chaos_length != 0)\n    {{\n        std::memcpy(chaos_buffer, chaos_utf8_data, chaos_length);\n    }}\n\n    chaos_buffer[chaos_length] = '\\0';\n    return reinterpret_cast<std::intptr_t>(chaos_buffer);\n}}", new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)), CreateNativeIntAbiSlot(), new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Runtime.InteropServices/Marshal::PtrToStringUTF8(System.IntPtr)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), "extern \"C\" std::intptr_t " + GetExternalRuntimeHelperSymbol(callee) + "(std::intptr_t chaos_arg_0)\n{\n    return chaos_arg_0 == static_cast<std::intptr_t>(0)\n        ? static_cast<std::intptr_t>(0)\n        : chaos_reflection_create_string_literal(reinterpret_cast<const char*>(chaos_arg_0));\n}", new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Runtime.InteropServices/Marshal::FreeCoTaskMem(System.IntPtr)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), "extern \"C\" void " + GetExternalRuntimeHelperSymbol(callee) + "(std::intptr_t chaos_arg_0)\n{\n    if (chaos_arg_0 != static_cast<std::intptr_t>(0))\n    {\n        std::free(reinterpret_cast<void*>(chaos_arg_0));\n    }\n}", new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()), new AotCoreIrAbiSlotArtifact
			{
				CarrierKindCode = AotCoreIrAbiCarrierKind.Void,
				TypeShape = (AotCoreIrTypeShapeKind)0
			}, new HashSet<int> { 0 });
			return true;
		}
		return false;
	}

	private bool TryCreateInterlockedRuntimeHelperDefinition(string callee, out ExternalRuntimeHelperDefinition? helperDefinition)
	{
		helperDefinition = null;
		if (!TryReadSingleGenericTypeArgument(callee, "System.Threading/Interlocked::CompareExchange<", out string typeSubjectId))
		{
			return false;
		}
		if (!TryResolveReferenceTypeSubjectId(typeSubjectId, out typeSubjectId))
		{
			return false;
		}
		helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), "extern \"C\" std::intptr_t " + GetExternalRuntimeHelperSymbol(callee) + "(std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1, std::intptr_t chaos_arg_2)\n{\n    auto* chaos_slot = chaos_resolve_native_int_slot(chaos_arg_0);\n    const auto chaos_original = *chaos_slot;\n    if (chaos_original == chaos_arg_2)\n    {\n        *chaos_slot = chaos_arg_1;\n    }\n\n    return chaos_original;\n}", new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
		{
			CreateNativeIntAbiSlot(),
			CreateNativeIntAbiSlot(typeSubjectId, AotCoreIrTypeShapeKind.ReferenceType),
			CreateNativeIntAbiSlot(typeSubjectId, AotCoreIrTypeShapeKind.ReferenceType)
		}), CreateNativeIntAbiSlot(typeSubjectId, AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0 });
		return true;
	}

}
