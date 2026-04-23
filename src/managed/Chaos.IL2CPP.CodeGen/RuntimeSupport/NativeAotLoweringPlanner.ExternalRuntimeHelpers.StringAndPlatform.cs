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
using Scriban.Runtime;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeAotLoweringPlanner
{
	private bool TryCreateStringRuntimeHelperDefinition(string callee, out ExternalRuntimeHelperDefinition? helperDefinition)
	{
		helperDefinition = null;
		if (MatchesMethodSubject(callee, "System.Private.CoreLib/System.String", "Concat", "System.String", "System.String"))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1",
				[
					"    return chaos_reflection_concat_string_pair_values(chaos_arg_0, chaos_arg_1);",
				]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
			{
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)
			}), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0, 1 });
			return true;
		}
		if (MatchesMethodSubject(callee, "System.Private.CoreLib/System.String", "Concat", "System.String", "System.String", "System.String"))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1, std::intptr_t chaos_arg_2",
				[
					"    const auto chaos_left_pair = chaos_reflection_concat_string_pair_values(chaos_arg_0, chaos_arg_1);",
					"    return chaos_reflection_concat_string_pair_values(chaos_left_pair, chaos_arg_2);",
				]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
			{
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)
			}), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0, 1, 2 });
			return true;
		}
		if (MatchesMethodSubject(callee, "System.Private.CoreLib/System.String", "Concat", "System.String", "System.String", "System.String", "System.String"))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1, std::intptr_t chaos_arg_2, std::intptr_t chaos_arg_3",
				[
					"    const auto chaos_left_pair = chaos_reflection_concat_string_pair_values(chaos_arg_0, chaos_arg_1);",
					"    const auto chaos_right_pair = chaos_reflection_concat_string_pair_values(chaos_arg_2, chaos_arg_3);",
					"    return chaos_reflection_concat_string_pair_values(chaos_left_pair, chaos_right_pair);",
				]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[4]
			{
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)
			}), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0, 1, 2, 3 });
			return true;
		}
		if (MatchesMethodSubject(callee, "System.Private.CoreLib/System.String", "op_Equality", "System.String", "System.String"))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1",
				[
					"    return chaos_object_equals(chaos_arg_0, chaos_arg_1)",
					"        ? static_cast<std::intptr_t>(1)",
					"        : static_cast<std::intptr_t>(0);",
				]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
			{
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)
			}), CreateNativeIntAbiSlot(), new HashSet<int> { 0, 1 });
			return true;
		}
		if (MatchesMethodSubject(callee, "System.Private.CoreLib/System.String", "get_Length"))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::int32_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    if (chaos_arg_0 == static_cast<std::intptr_t>(0))",
					"    {",
					"        std::abort();",
					"    }",
					string.Empty,
					$"    auto* chaos_string = reinterpret_cast<{GetNativeTypeSymbol(StringTypeSubjectId)}*>(chaos_arg_0);",
					"    return static_cast<std::int32_t>(chaos_string->length);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot(StringTypeSubjectId, AotCoreIrTypeShapeKind.ReferenceType)), CreateInt32AbiSlot(), new HashSet<int> { 0 });
			return true;
		}
		if (MatchesMethodSubject(callee, "System.Private.CoreLib/System.String", "StartsWith", "System.String", "System.StringComparison"))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1, std::int32_t chaos_arg_2",
				[
					"    if (chaos_arg_2 != 4)",
					"    {",
					"        std::abort();",
					"    }",
					string.Empty,
					"    if (chaos_arg_1 == static_cast<std::intptr_t>(0))",
					"    {",
					"        std::abort();",
					"    }",
					string.Empty,
					$"    auto* chaos_instance = reinterpret_cast<{GetNativeTypeSymbol(StringTypeSubjectId)}*>(chaos_arg_0);",
					$"    auto* chaos_prefix = reinterpret_cast<{GetNativeTypeSymbol(StringTypeSubjectId)}*>(chaos_arg_1);",
					"    const auto chaos_instance_length = static_cast<std::size_t>(chaos_instance->length);",
					"    const auto chaos_prefix_length = static_cast<std::size_t>(chaos_prefix->length);",
					"    if (chaos_prefix_length > chaos_instance_length)",
					"    {",
					"        return static_cast<std::intptr_t>(0);",
					"    }",
					string.Empty,
					"    if (chaos_prefix_length == 0)",
					"    {",
					"        return static_cast<std::intptr_t>(1);",
					"    }",
					string.Empty,
					"    const char* chaos_instance_utf8 = chaos_reflection_get_string_utf8(chaos_arg_0);",
					"    const char* chaos_prefix_utf8 = chaos_reflection_get_string_utf8(chaos_arg_1);",
					"    if ((chaos_instance_utf8 == nullptr && chaos_instance_length != 0) ||",
					"        (chaos_prefix_utf8 == nullptr && chaos_prefix_length != 0))",
					"    {",
					"        std::abort();",
					"    }",
					string.Empty,
					"    return std::memcmp(chaos_instance_utf8, chaos_prefix_utf8, chaos_prefix_length) == 0",
					"        ? static_cast<std::intptr_t>(1)",
					"        : static_cast<std::intptr_t>(0);",
				]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
			{
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
				CreateInt32AbiSlot()
			}), CreateNativeIntAbiSlot(), new HashSet<int> { 0, 1 });
			return true;
		}
		if (MatchesMethodSubject(callee, "System.Private.CoreLib/System.String", "Contains", "System.String", "System.StringComparison"))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1, std::int32_t chaos_arg_2",
				[
					"    if (chaos_arg_2 != 4)",
					"    {",
					"        std::abort();",
					"    }",
					string.Empty,
					"    if (chaos_arg_1 == static_cast<std::intptr_t>(0))",
					"    {",
					"        std::abort();",
					"    }",
					string.Empty,
					$"    auto* chaos_instance = reinterpret_cast<{GetNativeTypeSymbol(StringTypeSubjectId)}*>(chaos_arg_0);",
					$"    auto* chaos_value = reinterpret_cast<{GetNativeTypeSymbol(StringTypeSubjectId)}*>(chaos_arg_1);",
					"    const auto chaos_instance_length = static_cast<std::size_t>(chaos_instance->length);",
					"    const auto chaos_value_length = static_cast<std::size_t>(chaos_value->length);",
					"    if (chaos_value_length == 0)",
					"    {",
					"        return static_cast<std::intptr_t>(1);",
					"    }",
					string.Empty,
					"    if (chaos_value_length > chaos_instance_length)",
					"    {",
					"        return static_cast<std::intptr_t>(0);",
					"    }",
					string.Empty,
					"    const char* chaos_instance_utf8 = chaos_reflection_get_string_utf8(chaos_arg_0);",
					"    const char* chaos_value_utf8 = chaos_reflection_get_string_utf8(chaos_arg_1);",
					"    if ((chaos_instance_utf8 == nullptr && chaos_instance_length != 0) ||",
					"        (chaos_value_utf8 == nullptr && chaos_value_length != 0))",
					"    {",
					"        std::abort();",
					"    }",
					string.Empty,
					"    const auto chaos_last_start = chaos_instance_length - chaos_value_length;",
					"    for (std::size_t chaos_index = 0; chaos_index <= chaos_last_start; chaos_index++)",
					"    {",
					"        if (std::memcmp(chaos_instance_utf8 + chaos_index, chaos_value_utf8, chaos_value_length) == 0)",
					"        {",
					"            return static_cast<std::intptr_t>(1);",
					"        }",
					"    }",
					string.Empty,
					"    return static_cast<std::intptr_t>(0);",
				]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
			{
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
				CreateInt32AbiSlot()
			}), CreateNativeIntAbiSlot(), new HashSet<int> { 0, 1 });
			return true;
		}
		if (TryGetStringJoinEnumerableElementType(callee, out string elementTypeDisplayName))
		{
			if (string.Equals(elementTypeDisplayName, "System.Int32", StringComparison.Ordinal))
			{
				helperDefinition = CreateStringJoinInt32EnumerableRuntimeHelperDefinition(callee, ResolveEnumerableJoinSupportVariants(elementTypeDisplayName));
				return true;
			}
			if (string.Equals(elementTypeDisplayName, "System.String", StringComparison.Ordinal))
			{
				helperDefinition = CreateStringJoinStringEnumerableRuntimeHelperDefinition(callee, ResolveEnumerableJoinSupportVariants(elementTypeDisplayName));
				return true;
			}
		}
		return false;
	}

	private static string RenderSimpleExternalRuntimeHelper(
		string returnType,
		string helperSymbol,
		string parameterSignature,
		IReadOnlyList<string> bodyLines)
	{
		return ScribanTemplateRenderer.RenderTemplate(
			NativeAotTemplateCatalog.GetSimpleExternalRuntimeHelperTemplate(),
			new ScriptObject
			{
				["return_type"] = returnType,
				["helper_symbol"] = helperSymbol,
				["parameter_signature"] = parameterSignature,
				["body_lines"] = bodyLines.ToArray(),
			}).TrimEnd();
	}

	private ExternalRuntimeHelperDefinition CreateStringJoinInt32EnumerableRuntimeHelperDefinition(string callee, IReadOnlyList<EnumerableJoinSupportVariant> variants)
	{
		var orderedVariants = variants
			.OrderBy(item => item.EnumerableTypeSubjectId, StringComparer.Ordinal)
			.Select(item => new ScriptObject
			{
				["enumerable_type_id_symbol"] = GetNativeTypeIdSymbol(item.EnumerableTypeSubjectId),
				["get_enumerator_symbol"] = item.GetEnumeratorMethod.NativeSymbol,
				["move_next_symbol"] = item.MoveNextMethod.NativeSymbol,
				["get_current_symbol"] = item.GetCurrentMethod.NativeSymbol,
			})
			.ToArray();
		var model = new ScriptObject
		{
			["helper_symbol"] = GetExternalRuntimeHelperSymbol(callee),
			["variant_entries"] = orderedVariants,
		};
		var rendered = ScribanTemplateRenderer.RenderTemplate(
			NativeAotTemplateCatalog.GetStringJoinInt32EnumerableTemplate(),
			model);
		return new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), rendered.TrimEnd(), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
		{
			CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
			CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)
		}), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0, 1 });
	}

	private ExternalRuntimeHelperDefinition CreateStringJoinStringEnumerableRuntimeHelperDefinition(string callee, IReadOnlyList<EnumerableJoinSupportVariant> variants)
	{
		var orderedVariants = variants
			.OrderBy(item => item.EnumerableTypeSubjectId, StringComparer.Ordinal)
			.Select(item => new ScriptObject
			{
				["enumerable_type_id_symbol"] = GetNativeTypeIdSymbol(item.EnumerableTypeSubjectId),
				["get_enumerator_symbol"] = item.GetEnumeratorMethod.NativeSymbol,
				["move_next_symbol"] = item.MoveNextMethod.NativeSymbol,
				["get_current_symbol"] = item.GetCurrentMethod.NativeSymbol,
			})
			.ToArray();
		var model = new ScriptObject
		{
			["helper_symbol"] = GetExternalRuntimeHelperSymbol(callee),
			["reference_type_shape_value"] = GetNativeTypeShapeValue(AotCoreIrTypeShapeKind.ReferenceType),
			["string_type_id_symbol"] = GetNativeTypeIdSymbol("System.Private.CoreLib/System.String"),
			["variant_entries"] = orderedVariants,
		};
		var rendered = ScribanTemplateRenderer.RenderTemplate(
			NativeAotTemplateCatalog.GetStringJoinStringEnumerableTemplate(),
			model);
		return new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), rendered.TrimEnd(), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
		{
			CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
			CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)
		}), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0, 1 });
	}

	private bool TryCreateExceptionRuntimeHelperDefinition(string callee, out ExternalRuntimeHelperDefinition? helperDefinition)
	{
		helperDefinition = null;
		if (MatchesMethodSubject(callee, "System.Private.CoreLib/System.Exception", ".ctor", "System.String") || MatchesMethodSubject(callee, "System.Private.CoreLib/System.InvalidOperationException", ".ctor", "System.String"))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"void",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1",
				[
					"    chaos_reflection_set_exception_metadata(",
					"        chaos_arg_0,",
					"        chaos_arg_1,",
					"        static_cast<std::intptr_t>(0));",
				]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
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
		if (MatchesMethodSubject(callee, "System.Private.CoreLib/System.ArgumentOutOfRangeException", ".ctor", "System.String", "System.String"))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"void",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1, std::intptr_t chaos_arg_2",
				[
					"    auto chaos_message_value = chaos_arg_2;",
					"    if (chaos_arg_1 != static_cast<std::intptr_t>(0))",
					"    {",
					"        chaos_message_value =",
					"            chaos_message_value == static_cast<std::intptr_t>(0)",
					"                ? chaos_arg_1",
					"                : chaos_reflection_concat_string_pair_values(chaos_message_value, chaos_arg_1);",
					"    }",
					string.Empty,
					"    chaos_reflection_set_exception_metadata(",
					"        chaos_arg_0,",
					"        chaos_message_value,",
					"        chaos_arg_1);",
				]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
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
		if (MatchesMethodSubject(callee, "System.Private.CoreLib/System.Exception", "get_Message"))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    return chaos_reflection_get_exception_message(chaos_arg_0);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0 });
			return true;
		}
		if (MatchesMethodSubject(callee, "System.Private.CoreLib/System.ArgumentException", "get_ParamName"))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    return chaos_reflection_get_exception_param_name(chaos_arg_0);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0 });
			return true;
		}
		return false;
	}

	private bool TryCreateNumericFormattingRuntimeHelperDefinition(string callee, out ExternalRuntimeHelperDefinition? helperDefinition)
	{
		helperDefinition = null;
		if (MatchesMethodSubject(callee, "System.Private.CoreLib/System.Int32", "ToString"))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    auto* chaos_value_slot = chaos_resolve_native_int_slot(chaos_arg_0);",
					"    char chaos_buffer[64]{};",
					"    std::snprintf(chaos_buffer, sizeof(chaos_buffer), \"%d\", static_cast<std::int32_t>(*chaos_value_slot));",
					"    return chaos_reflection_create_string_literal(chaos_buffer);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0 });
			return true;
		}
		if (MatchesMethodSubject(callee, "System.Private.CoreLib/System.Single", "ToString", "System.String"))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1",
				[
					"    const char* chaos_format_utf8 = nullptr;",
					"    if (chaos_arg_1 != static_cast<std::intptr_t>(0))",
					"    {",
					$"        auto* chaos_format = reinterpret_cast<{GetNativeTypeSymbol(StringTypeSubjectId)}*>(chaos_arg_1);",
					"        chaos_format_utf8 = chaos_format->utf8_data;",
					"    }",
					string.Empty,
					"    if (chaos_format_utf8 != nullptr && std::strcmp(chaos_format_utf8, \"F1\") != 0)",
					"    {",
					"        std::abort();",
					"    }",
					string.Empty,
					"    char chaos_buffer[64]{};",
					"    std::snprintf(chaos_buffer, sizeof(chaos_buffer), \"%.1f\", static_cast<double>(chaos_load_float32(chaos_arg_0)));",
					"    return chaos_reflection_create_string_literal(chaos_buffer);",
				]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
			{
				CreateNativeIntAbiSlot(),
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)
			}), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0, 1 });
			return true;
		}
		if (MatchesMethodSubject(callee, "System.Private.CoreLib/System.Double", "ToString", "System.String"))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1",
				[
					"    const char* chaos_format_utf8 = nullptr;",
					"    if (chaos_arg_1 != static_cast<std::intptr_t>(0))",
					"    {",
					$"        auto* chaos_format = reinterpret_cast<{GetNativeTypeSymbol(StringTypeSubjectId)}*>(chaos_arg_1);",
					"        chaos_format_utf8 = chaos_format->utf8_data;",
					"    }",
					string.Empty,
					"    if (chaos_format_utf8 != nullptr && std::strcmp(chaos_format_utf8, \"F1\") != 0)",
					"    {",
					"        std::abort();",
					"    }",
					string.Empty,
					"    char chaos_buffer[64]{};",
					"    std::snprintf(chaos_buffer, sizeof(chaos_buffer), \"%.1f\", chaos_load_float64(chaos_arg_0));",
					"    return chaos_reflection_create_string_literal(chaos_buffer);",
				]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
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
		if (MatchesMethodSubject(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.DefaultInterpolatedStringHandler", ".ctor", "System.Int32", "System.Int32"))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"void",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0, std::int32_t chaos_arg_1, std::int32_t chaos_arg_2",
				[
					"    (void)chaos_arg_1;",
					"    (void)chaos_arg_2;",
					"    chaos_default_interpolated_string_handler_reset(chaos_arg_0);",
				]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
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
		if (MatchesMethodSubject(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.DefaultInterpolatedStringHandler", "AppendFormatted", "System.String") || MatchesMethodSubject(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.DefaultInterpolatedStringHandler", "AppendLiteral", "System.String"))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"void",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1",
				[
					"    chaos_default_interpolated_string_handler_append_string(chaos_arg_0, chaos_arg_1);",
				]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
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
		if (IsSupportedDefaultInterpolatedStringHandlerAppendFormattedSubjectId(callee))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"void",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0, std::int32_t chaos_arg_1",
				[
					"    chaos_default_interpolated_string_handler_append_int32(chaos_arg_0, chaos_arg_1);",
				]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
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
		if (MatchesMethodSubject(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.DefaultInterpolatedStringHandler", "ToStringAndClear"))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    return chaos_default_interpolated_string_handler_to_string_and_clear(chaos_arg_0);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0 });
			return true;
		}
		return false;
	}

	private bool TryCreatePlatformRuntimeHelperDefinition(string callee, out ExternalRuntimeHelperDefinition? helperDefinition)
	{
		helperDefinition = null;
		string text = null;
		if (MatchesMethodSubject(callee, "System.Private.CoreLib/System.OperatingSystem", "IsWindows"))
		{
			text = "#if defined(_WIN32)\n    return static_cast<std::intptr_t>(1);\n#else\n    return static_cast<std::intptr_t>(0);\n#endif";
		}
		else if (MatchesMethodSubject(callee, "System.Private.CoreLib/System.OperatingSystem", "IsLinux"))
		{
			text = "#if defined(__linux__)\n    return static_cast<std::intptr_t>(1);\n#else\n    return static_cast<std::intptr_t>(0);\n#endif";
		}
		else if (MatchesMethodSubject(callee, "System.Private.CoreLib/System.OperatingSystem", "IsMacOS"))
		{
			text = "#if defined(__APPLE__)\n    return static_cast<std::intptr_t>(1);\n#else\n    return static_cast<std::intptr_t>(0);\n#endif";
		}
		if (text == null)
		{
			return false;
		}
		helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
			"std::intptr_t",
			GetExternalRuntimeHelperSymbol(callee),
			string.Empty,
			text.Split('\n')), Array.Empty<AotCoreIrAbiSlotArtifact>(), CreateNativeIntAbiSlot(), EmptyRawArgumentIndices);
		return true;
	}

	private bool TryCreateDelegateInteropRuntimeHelperDefinition(string callee, out ExternalRuntimeHelperDefinition? helperDefinition)
	{
		helperDefinition = null;
		if (MatchesMethodSubject(callee, "System.Private.CoreLib/System.Delegate", "Combine", "System.Delegate", "System.Delegate"))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1",
				[
					"    return chaos_delegate_combine(chaos_arg_0, chaos_arg_1);",
				]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
			{
				CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
				CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)
			}), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Delegate", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0, 1 });
			return true;
		}
		if (MatchesMethodSubject(callee, "System.Private.CoreLib/System.Delegate", "Remove", "System.Delegate", "System.Delegate"))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1",
				[
					"    return chaos_delegate_remove(chaos_arg_0, chaos_arg_1);",
				]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
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
		AotCoreIrAbiSlotArtifact voidReturnAbi = new AotCoreIrAbiSlotArtifact
		{
			CarrierKindCode = AotCoreIrAbiCarrierKind.Void,
			TypeShape = (AotCoreIrTypeShapeKind)0
		};
		if (TryReadSingleGenericTypeArgument(callee, MarshalSizeOfMethodPrefix, out string valueTypeNameOrSubjectId) && TryCreateResolvedTypeAbiSlot(valueTypeNameOrSubjectId, out AotCoreIrAbiSlotArtifact valueTypeAbi) && valueTypeAbi.CarrierKindCode == AotCoreIrAbiCarrierKind.ValueTypeByValue)
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::int32_t",
				GetExternalRuntimeHelperSymbol(callee),
				string.Empty,
				[
					$"    return static_cast<std::int32_t>(sizeof({GetRequiredAbiValueTypeSymbol(valueTypeAbi)}));",
				]), Array.Empty<AotCoreIrAbiSlotArtifact>(), CreateInt32AbiSlot(), EmptyRawArgumentIndices);
			return true;
		}
		if (MatchesMethodSubject(callee, "System.Runtime.InteropServices/Marshal", "AllocHGlobal", "System.Int32"))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::int32_t chaos_arg_0",
				[
					"    if (chaos_arg_0 < 0)",
					"    {",
					"        std::abort();",
					"    }",
					string.Empty,
					"    const auto chaos_size = static_cast<std::size_t>(chaos_arg_0 == 0 ? 1 : chaos_arg_0);",
					"    auto* chaos_buffer = std::malloc(chaos_size);",
					"    if (chaos_buffer == nullptr)",
					"    {",
					"        std::abort();",
					"    }",
					string.Empty,
					"    return reinterpret_cast<std::intptr_t>(chaos_buffer);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateInt32AbiSlot()), CreateNativeIntAbiSlot(), EmptyRawArgumentIndices);
			return true;
		}
		if (MatchesMethodSubject(callee, "System.Runtime.InteropServices/Marshal", "FreeHGlobal", "System.IntPtr"))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"void",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    if (chaos_arg_0 != static_cast<std::intptr_t>(0))",
					"    {",
					"        std::free(reinterpret_cast<void*>(chaos_arg_0));",
					"    }",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()), voidReturnAbi, new HashSet<int> { 0 });
			return true;
		}
		if (TryReadSingleGenericTypeArgument(callee, MarshalStructureToPtrMethodPrefix, out valueTypeNameOrSubjectId) && TryCreateResolvedTypeAbiSlot(valueTypeNameOrSubjectId, out valueTypeAbi) && valueTypeAbi.CarrierKindCode == AotCoreIrAbiCarrierKind.ValueTypeByValue)
		{
			string valueTypeSymbol = GetRequiredAbiValueTypeSymbol(valueTypeAbi);
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"void",
				GetExternalRuntimeHelperSymbol(callee),
				$"{valueTypeSymbol} chaos_arg_0, std::intptr_t chaos_arg_1, std::intptr_t chaos_arg_2",
				[
					"    if (chaos_arg_1 == static_cast<std::intptr_t>(0) || chaos_arg_2 != static_cast<std::intptr_t>(0))",
					"    {",
					"        std::abort();",
					"    }",
					string.Empty,
					$"    *reinterpret_cast<{valueTypeSymbol}*>(chaos_arg_1) = chaos_arg_0;",
				]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
			{
				valueTypeAbi,
				CreateNativeIntAbiSlot(),
				CreateNativeIntAbiSlot()
			}), voidReturnAbi, EmptyRawArgumentIndices);
			return true;
		}
		if (TryReadSingleGenericTypeArgument(callee, MarshalPtrToStructureMethodPrefix, out valueTypeNameOrSubjectId) && TryCreateResolvedTypeAbiSlot(valueTypeNameOrSubjectId, out valueTypeAbi) && valueTypeAbi.CarrierKindCode == AotCoreIrAbiCarrierKind.ValueTypeByValue)
		{
			string valueTypeSymbol2 = GetRequiredAbiValueTypeSymbol(valueTypeAbi);
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				valueTypeSymbol2,
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    if (chaos_arg_0 == static_cast<std::intptr_t>(0))",
					"    {",
					"        std::abort();",
					"    }",
					string.Empty,
					$"    return *reinterpret_cast<{valueTypeSymbol2}*>(chaos_arg_0);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()), valueTypeAbi, new HashSet<int> { 0 });
			return true;
		}
		if (MatchesMethodSubject(callee, "System.Runtime.InteropServices/Marshal", "StringToCoTaskMemUTF8", "System.String"))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    if (chaos_arg_0 == static_cast<std::intptr_t>(0))",
					"    {",
					"        return static_cast<std::intptr_t>(0);",
					"    }",
					string.Empty,
					$"    auto* chaos_string = reinterpret_cast<{GetNativeTypeSymbol(StringTypeSubjectId)}*>(chaos_arg_0);",
					"    const auto chaos_length = static_cast<std::size_t>(chaos_string->length);",
					"    const auto* chaos_utf8_data = chaos_string->utf8_data;",
					"    if (chaos_length != 0 && chaos_utf8_data == nullptr)",
					"    {",
					"        std::abort();",
					"    }",
					string.Empty,
					"    auto* chaos_buffer = static_cast<char*>(std::malloc(chaos_length + 1));",
					"    if (chaos_buffer == nullptr)",
					"    {",
					"        std::abort();",
					"    }",
					string.Empty,
					"    if (chaos_length != 0)",
					"    {",
					"        std::memcpy(chaos_buffer, chaos_utf8_data, chaos_length);",
					"    }",
					string.Empty,
					"    chaos_buffer[chaos_length] = '\\0';",
					"    return reinterpret_cast<std::intptr_t>(chaos_buffer);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)), CreateNativeIntAbiSlot(), new HashSet<int> { 0 });
			return true;
		}
		if (MatchesMethodSubject(callee, "System.Runtime.InteropServices/Marshal", "PtrToStringUTF8", "System.IntPtr"))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    return chaos_arg_0 == static_cast<std::intptr_t>(0)",
					"        ? static_cast<std::intptr_t>(0)",
					"        : chaos_reflection_create_string_literal(reinterpret_cast<const char*>(chaos_arg_0));",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0 });
			return true;
		}
		if (MatchesMethodSubject(callee, "System.Runtime.InteropServices/Marshal", "FreeCoTaskMem", "System.IntPtr"))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"void",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    if (chaos_arg_0 != static_cast<std::intptr_t>(0))",
					"    {",
					"        std::free(reinterpret_cast<void*>(chaos_arg_0));",
					"    }",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()), new AotCoreIrAbiSlotArtifact
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
		helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
			"std::intptr_t",
			GetExternalRuntimeHelperSymbol(callee),
			"std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1, std::intptr_t chaos_arg_2",
			[
				"    auto* chaos_slot = chaos_resolve_native_int_slot(chaos_arg_0);",
				"    const auto chaos_original = *chaos_slot;",
				"    if (chaos_original == chaos_arg_2)",
				"    {",
				"        *chaos_slot = chaos_arg_1;",
				"    }",
				string.Empty,
				"    return chaos_original;",
			]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
		{
			CreateNativeIntAbiSlot(),
			CreateNativeIntAbiSlot(typeSubjectId, AotCoreIrTypeShapeKind.ReferenceType),
			CreateNativeIntAbiSlot(typeSubjectId, AotCoreIrTypeShapeKind.ReferenceType)
		}), CreateNativeIntAbiSlot(typeSubjectId, AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0 });
		return true;
	}

}
