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
				"CHAOS_IL2CPP_INTPTR",
				GetExternalRuntimeHelperSymbol(callee),
				"CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
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
				"CHAOS_IL2CPP_INTPTR",
				GetExternalRuntimeHelperSymbol(callee),
				"CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2",
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
				"CHAOS_IL2CPP_INTPTR",
				GetExternalRuntimeHelperSymbol(callee),
				"CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2, CHAOS_IL2CPP_INTPTR chaos_arg_3",
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
				"CHAOS_IL2CPP_INTPTR",
				GetExternalRuntimeHelperSymbol(callee),
				"CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
				[
					"    return chaos_object_equals(chaos_arg_0, chaos_arg_1)",
					"        ? static_cast<CHAOS_IL2CPP_INTPTR>(1)",
					"        : static_cast<CHAOS_IL2CPP_INTPTR>(0);",
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
				"CHAOS_IL2CPP_INT32",
				GetExternalRuntimeHelperSymbol(callee),
				"CHAOS_IL2CPP_INTPTR chaos_arg_0",
				[
					"    if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))",
					"    {",
					"        CHAOS_IL2CPP_ABORT();",
					"    }",
					string.Empty,
					"    if (chaos_is_string_id(chaos_arg_0))",
					"    {",
					"        return static_cast<CHAOS_IL2CPP_INT32>(",
					"            chaos::il2cpp::string_table::Resolve(",
					"                chaos_extract_string_id(chaos_arg_0)).byte_count);",
					"    }",
					string.Empty,
					$"    auto* chaos_string = reinterpret_cast<{GetNativeTypeSymbol(StringTypeSubjectId)}*>(chaos_arg_0);",
					"    return static_cast<CHAOS_IL2CPP_INT32>(chaos_string->length);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot(StringTypeSubjectId, AotCoreIrTypeShapeKind.ReferenceType)), CreateInt32AbiSlot(), new HashSet<int> { 0 });
			return true;
		}
		if (MatchesMethodSubject(callee, "System.Private.CoreLib/System.String", "StartsWith", "System.String", "System.StringComparison"))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"CHAOS_IL2CPP_INTPTR",
				GetExternalRuntimeHelperSymbol(callee),
				"CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INT32 chaos_arg_2",
				[
					"    if (chaos_arg_2 != 4)",
					"    {",
					"        CHAOS_IL2CPP_ABORT();",
					"    }",
					string.Empty,
					"    if (chaos_arg_1 == static_cast<CHAOS_IL2CPP_INTPTR>(0))",
					"    {",
					"        CHAOS_IL2CPP_ABORT();",
					"    }",
					string.Empty,
					"    const char* chaos_instance_utf8 = chaos_reflection_get_string_utf8(chaos_arg_0);",
					"    const char* chaos_prefix_utf8 = chaos_reflection_get_string_utf8(chaos_arg_1);",
					"    const auto chaos_instance_length = static_cast<CHAOS_IL2CPP_SIZE>(",
					"        chaos_is_string_id(chaos_arg_0)",
					"            ? static_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::string_table::Resolve(",
					"                chaos_extract_string_id(chaos_arg_0)).byte_count)",
					$"            : static_cast<CHAOS_IL2CPP_INTPTR>(reinterpret_cast<{GetNativeTypeSymbol(StringTypeSubjectId)}*>(chaos_arg_0)->length));",
					"    const auto chaos_prefix_length = static_cast<CHAOS_IL2CPP_SIZE>(",
					"        chaos_is_string_id(chaos_arg_1)",
					"            ? static_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::string_table::Resolve(",
					"                chaos_extract_string_id(chaos_arg_1)).byte_count)",
					$"            : static_cast<CHAOS_IL2CPP_INTPTR>(reinterpret_cast<{GetNativeTypeSymbol(StringTypeSubjectId)}*>(chaos_arg_1)->length));",
					"    if (chaos_prefix_length > chaos_instance_length)",
					"    {",
					"        return static_cast<CHAOS_IL2CPP_INTPTR>(0);",
					"    }",
					string.Empty,
					"    if (chaos_prefix_length == 0)",
					"    {",
					"        return static_cast<CHAOS_IL2CPP_INTPTR>(1);",
					"    }",
					string.Empty,
					"    if ((chaos_instance_utf8 == nullptr && chaos_instance_length != 0) ||",
					"        (chaos_prefix_utf8 == nullptr && chaos_prefix_length != 0))",
					"    {",
					"        CHAOS_IL2CPP_ABORT();",
					"    }",
					string.Empty,
					"    return CHAOS_IL2CPP_MEMCMP(chaos_instance_utf8, chaos_prefix_utf8, chaos_prefix_length) == 0",
					"        ? static_cast<CHAOS_IL2CPP_INTPTR>(1)",
					"        : static_cast<CHAOS_IL2CPP_INTPTR>(0);",
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
				"CHAOS_IL2CPP_INTPTR",
				GetExternalRuntimeHelperSymbol(callee),
				"CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INT32 chaos_arg_2",
				[
					"    if (chaos_arg_2 != 4)",
					"    {",
					"        CHAOS_IL2CPP_ABORT();",
					"    }",
					string.Empty,
					"    if (chaos_arg_1 == static_cast<CHAOS_IL2CPP_INTPTR>(0))",
					"    {",
					"        CHAOS_IL2CPP_ABORT();",
					"    }",
					string.Empty,
					"    const char* chaos_instance_utf8 = chaos_reflection_get_string_utf8(chaos_arg_0);",
					"    const char* chaos_value_utf8 = chaos_reflection_get_string_utf8(chaos_arg_1);",
					"    const auto chaos_instance_length = static_cast<CHAOS_IL2CPP_SIZE>(",
					"        chaos_is_string_id(chaos_arg_0)",
					"            ? static_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::string_table::Resolve(",
					"                chaos_extract_string_id(chaos_arg_0)).byte_count)",
					$"            : static_cast<CHAOS_IL2CPP_INTPTR>(reinterpret_cast<{GetNativeTypeSymbol(StringTypeSubjectId)}*>(chaos_arg_0)->length));",
					"    const auto chaos_value_length = static_cast<CHAOS_IL2CPP_SIZE>(",
					"        chaos_is_string_id(chaos_arg_1)",
					"            ? static_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::string_table::Resolve(",
					"                chaos_extract_string_id(chaos_arg_1)).byte_count)",
					$"            : static_cast<CHAOS_IL2CPP_INTPTR>(reinterpret_cast<{GetNativeTypeSymbol(StringTypeSubjectId)}*>(chaos_arg_1)->length));",
					"    if (chaos_value_length == 0)",
					"    {",
					"        return static_cast<CHAOS_IL2CPP_INTPTR>(1);",
					"    }",
					string.Empty,
					"    if (chaos_value_length > chaos_instance_length)",
					"    {",
					"        return static_cast<CHAOS_IL2CPP_INTPTR>(0);",
					"    }",
					string.Empty,
					"    if ((chaos_instance_utf8 == nullptr && chaos_instance_length != 0) ||",
					"        (chaos_value_utf8 == nullptr && chaos_value_length != 0))",
					"    {",
					"        CHAOS_IL2CPP_ABORT();",
					"    }",
					string.Empty,
					"    const auto chaos_last_start = chaos_instance_length - chaos_value_length;",
					"    for (CHAOS_IL2CPP_SIZE chaos_index = 0; chaos_index <= chaos_last_start; chaos_index++)",
					"    {",
					"        if (CHAOS_IL2CPP_MEMCMP(chaos_instance_utf8 + chaos_index, chaos_value_utf8, chaos_value_length) == 0)",
					"        {",
					"            return static_cast<CHAOS_IL2CPP_INTPTR>(1);",
					"        }",
					"    }",
					string.Empty,
					"    return static_cast<CHAOS_IL2CPP_INTPTR>(0);",
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
				"CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
				[
					"    chaos_reflection_set_exception_metadata(",
					"        chaos_arg_0,",
					"        chaos_arg_1,",
					"        static_cast<CHAOS_IL2CPP_INTPTR>(0));",
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
				"CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2",
				[
					"    auto chaos_message_value = chaos_arg_2;",
					"    if (chaos_arg_1 != static_cast<CHAOS_IL2CPP_INTPTR>(0))",
					"    {",
					"        chaos_message_value =",
					"            chaos_message_value == static_cast<CHAOS_IL2CPP_INTPTR>(0)",
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
				"CHAOS_IL2CPP_INTPTR",
				GetExternalRuntimeHelperSymbol(callee),
				"CHAOS_IL2CPP_INTPTR chaos_arg_0",
				[
					"    return chaos_reflection_get_exception_message(chaos_arg_0);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0 });
			return true;
		}
		if (MatchesMethodSubject(callee, "System.Private.CoreLib/System.ArgumentException", "get_ParamName"))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"CHAOS_IL2CPP_INTPTR",
				GetExternalRuntimeHelperSymbol(callee),
				"CHAOS_IL2CPP_INTPTR chaos_arg_0",
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
				"CHAOS_IL2CPP_INTPTR",
				GetExternalRuntimeHelperSymbol(callee),
				"CHAOS_IL2CPP_INTPTR chaos_arg_0",
				[
					"    auto* chaos_value_slot = chaos_resolve_native_int_slot(chaos_arg_0);",
					"    const CHAOS_IL2CPP_STRING chaos_formatted = ChaosIl2cpp::Common::format_int32(static_cast<CHAOS_IL2CPP_INT32>(*chaos_value_slot));",
					"    const auto chaos_id = chaos::il2cpp::string_table::Intern(",
					"        chaos_formatted.c_str(), static_cast<CHAOS_IL2CPP_UINT32>(chaos_formatted.size()));",
					"    return chaos_make_string_id_value(chaos_id);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0 });
			return true;
		}
		if (MatchesMethodSubject(callee, "System.Private.CoreLib/System.Single", "ToString", "System.String"))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"CHAOS_IL2CPP_INTPTR",
				GetExternalRuntimeHelperSymbol(callee),
				"CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
				[
					"    const char* chaos_format_utf8 = nullptr;",
					"    if (chaos_arg_1 != static_cast<CHAOS_IL2CPP_INTPTR>(0))",
					"    {",
					"        chaos_format_utf8 = chaos_reflection_get_string_utf8(chaos_arg_1);",
					"    }",
					string.Empty,
					"    if (chaos_format_utf8 != nullptr && CHAOS_IL2CPP_STRCMP(chaos_format_utf8, \"F1\") != 0)",
					"    {",
					"        CHAOS_IL2CPP_ABORT();",
					"    }",
					string.Empty,
					"    const CHAOS_IL2CPP_STRING chaos_formatted = ChaosIl2cpp::Common::format_float(chaos_load_float32(chaos_arg_0));",
					"    const auto chaos_id = chaos::il2cpp::string_table::Intern(",
					"        chaos_formatted.c_str(), static_cast<CHAOS_IL2CPP_UINT32>(chaos_formatted.size()));",
					"    return chaos_make_string_id_value(chaos_id);",
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
				"CHAOS_IL2CPP_INTPTR",
				GetExternalRuntimeHelperSymbol(callee),
				"CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
				[
					"    const char* chaos_format_utf8 = nullptr;",
					"    if (chaos_arg_1 != static_cast<CHAOS_IL2CPP_INTPTR>(0))",
					"    {",
					"        chaos_format_utf8 = chaos_reflection_get_string_utf8(chaos_arg_1);",
					"    }",
					string.Empty,
					"    if (chaos_format_utf8 != nullptr && CHAOS_IL2CPP_STRCMP(chaos_format_utf8, \"F1\") != 0)",
					"    {",
					"        CHAOS_IL2CPP_ABORT();",
					"    }",
					string.Empty,
					"    const CHAOS_IL2CPP_STRING chaos_formatted = ChaosIl2cpp::Common::format_double(chaos_load_float64(chaos_arg_0));",
					"    const auto chaos_id = chaos::il2cpp::string_table::Intern(",
					"        chaos_formatted.c_str(), static_cast<CHAOS_IL2CPP_UINT32>(chaos_formatted.size()));",
					"    return chaos_make_string_id_value(chaos_id);",
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
				"CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1, CHAOS_IL2CPP_INT32 chaos_arg_2",
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
				"CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
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
				"CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1",
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
				"CHAOS_IL2CPP_INTPTR",
				GetExternalRuntimeHelperSymbol(callee),
				"CHAOS_IL2CPP_INTPTR chaos_arg_0",
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
			text = "#if defined(_WIN32)\n    return static_cast<CHAOS_IL2CPP_INTPTR>(1);\n#else\n    return static_cast<CHAOS_IL2CPP_INTPTR>(0);\n#endif";
		}
		else if (MatchesMethodSubject(callee, "System.Private.CoreLib/System.OperatingSystem", "IsLinux"))
		{
			text = "#if defined(__linux__)\n    return static_cast<CHAOS_IL2CPP_INTPTR>(1);\n#else\n    return static_cast<CHAOS_IL2CPP_INTPTR>(0);\n#endif";
		}
		else if (MatchesMethodSubject(callee, "System.Private.CoreLib/System.OperatingSystem", "IsMacOS"))
		{
			text = "#if defined(__APPLE__)\n    return static_cast<CHAOS_IL2CPP_INTPTR>(1);\n#else\n    return static_cast<CHAOS_IL2CPP_INTPTR>(0);\n#endif";
		}
		if (text == null)
		{
			return false;
		}
		helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
			"CHAOS_IL2CPP_INTPTR",
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
				"CHAOS_IL2CPP_INTPTR",
				GetExternalRuntimeHelperSymbol(callee),
				"CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
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
				"CHAOS_IL2CPP_INTPTR",
				GetExternalRuntimeHelperSymbol(callee),
				"CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
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
extern "C" CHAOS_IL2CPP_INTPTR {{helperSymbol}}(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    struct chaos_delegate_function_pointer_bridge
    {
        CHAOS_IL2CPP_UINT64 magic = 0;
        CHAOS_IL2CPP_INTPTR delegate_target = 0;
        CHAOS_IL2CPP_INTPTR delegate_method_ptr = 0;
        CHAOS_IL2CPP_INTPTR delegate_invocation_list = 0;
        CHAOS_IL2CPP_INTPTR delegate_invocation_count = 0;
    };

    constexpr CHAOS_IL2CPP_UINT64 chaos_delegate_function_pointer_bridge_magic = 0x4348414F535F4446ULL;

    if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
    {
        return static_cast<CHAOS_IL2CPP_INTPTR>(0);
    }

    auto* chaos_delegate = reinterpret_cast<{{GetNativeTypeSymbol(typeSubjectId)}}*>(chaos_arg_0);
    if (chaos_delegate->chaos_delegate_target == static_cast<CHAOS_IL2CPP_INTPTR>(0) &&
        chaos_delegate->chaos_delegate_invocation_count == static_cast<CHAOS_IL2CPP_INTPTR>(0))
    {
        return chaos_delegate->chaos_delegate_method_ptr;
    }

    auto* chaos_bridge = new chaos_delegate_function_pointer_bridge{};
    chaos_bridge->magic = chaos_delegate_function_pointer_bridge_magic;
    chaos_bridge->delegate_target = chaos_delegate->chaos_delegate_target;
    chaos_bridge->delegate_method_ptr = chaos_delegate->chaos_delegate_method_ptr;
    chaos_bridge->delegate_invocation_list = chaos_delegate->chaos_delegate_invocation_list;
    chaos_bridge->delegate_invocation_count = chaos_delegate->chaos_delegate_invocation_count;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_bridge);
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
extern "C" CHAOS_IL2CPP_INTPTR {{helperSymbol}}(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
    struct chaos_delegate_function_pointer_bridge
    {
        CHAOS_IL2CPP_UINT64 magic = 0;
        CHAOS_IL2CPP_INTPTR delegate_target = 0;
        CHAOS_IL2CPP_INTPTR delegate_method_ptr = 0;
        CHAOS_IL2CPP_INTPTR delegate_invocation_list = 0;
        CHAOS_IL2CPP_INTPTR delegate_invocation_count = 0;
    };

    constexpr CHAOS_IL2CPP_UINT64 chaos_delegate_function_pointer_bridge_magic = 0x4348414F535F4446ULL;

    auto* chaos_delegate = new {{GetNativeTypeSymbol(typeSubjectId)}}{};
    chaos_delegate->header.type_id = {{GetNativeTypeIdSymbol(typeSubjectId)}};
    if (chaos_arg_0 != static_cast<CHAOS_IL2CPP_INTPTR>(0))
    {
        const auto* chaos_bridge = reinterpret_cast<const chaos_delegate_function_pointer_bridge*>(chaos_arg_0);
        if (chaos_bridge->magic == chaos_delegate_function_pointer_bridge_magic)
        {
            chaos_delegate->chaos_delegate_target = chaos_bridge->delegate_target;
            chaos_delegate->chaos_delegate_method_ptr = chaos_bridge->delegate_method_ptr;
            chaos_delegate->chaos_delegate_invocation_list = chaos_bridge->delegate_invocation_list;
            chaos_delegate->chaos_delegate_invocation_count = chaos_bridge->delegate_invocation_count;
            return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_delegate);
        }
    }

    chaos_delegate->chaos_delegate_target = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    chaos_delegate->chaos_delegate_method_ptr = chaos_arg_0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_delegate);
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
				"CHAOS_IL2CPP_INT32",
				GetExternalRuntimeHelperSymbol(callee),
				string.Empty,
				[
					$"    return static_cast<CHAOS_IL2CPP_INT32>(sizeof({GetRequiredAbiValueTypeSymbol(valueTypeAbi)}));",
				]), Array.Empty<AotCoreIrAbiSlotArtifact>(), CreateInt32AbiSlot(), EmptyRawArgumentIndices);
			return true;
		}
		if (MatchesMethodSubject(callee, "System.Runtime.InteropServices/Marshal", "AllocHGlobal", "System.Int32"))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"CHAOS_IL2CPP_INTPTR",
				GetExternalRuntimeHelperSymbol(callee),
				"CHAOS_IL2CPP_INT32 chaos_arg_0",
				[
					"    if (chaos_arg_0 < 0)",
					"    {",
					"        CHAOS_IL2CPP_ABORT();",
					"    }",
					string.Empty,
					"    auto* chaos_runtime = chaos::il2cpp::runtime_core::GetCurrentRuntimeState();",
					"    if (chaos_runtime == nullptr) { CHAOS_IL2CPP_ABORT(); }",
					"    return chaos::il2cpp::runtime_core::MarshalAllocHGlobal(", 
					"        chaos_runtime, static_cast<CHAOS_IL2CPP_INTPTR>(chaos_size));",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateInt32AbiSlot()), CreateNativeIntAbiSlot(), EmptyRawArgumentIndices);
			return true;
		}
		if (MatchesMethodSubject(callee, "System.Runtime.InteropServices/Marshal", "FreeHGlobal", "System.IntPtr"))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"void",
				GetExternalRuntimeHelperSymbol(callee),
				"CHAOS_IL2CPP_INTPTR chaos_arg_0",
				[
					"    auto* chaos_runtime = chaos::il2cpp::runtime_core::GetCurrentRuntimeState();",
					"    if (chaos_runtime == nullptr) { CHAOS_IL2CPP_ABORT(); }",
					"    (void)chaos::il2cpp::runtime_core::MarshalFreeHGlobal(chaos_runtime, chaos_arg_0);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()), voidReturnAbi, new HashSet<int> { 0 });
			return true;
		}
		if (TryReadSingleGenericTypeArgument(callee, MarshalStructureToPtrMethodPrefix, out valueTypeNameOrSubjectId) && TryCreateResolvedTypeAbiSlot(valueTypeNameOrSubjectId, out valueTypeAbi) && valueTypeAbi.CarrierKindCode == AotCoreIrAbiCarrierKind.ValueTypeByValue)
		{
			string valueTypeSymbol = GetRequiredAbiValueTypeSymbol(valueTypeAbi);
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"void",
				GetExternalRuntimeHelperSymbol(callee),
				$"{valueTypeSymbol} chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2",
				[
					"    if (chaos_arg_1 == static_cast<CHAOS_IL2CPP_INTPTR>(0) || chaos_arg_2 != static_cast<CHAOS_IL2CPP_INTPTR>(0))",
					"    {",
					"        CHAOS_IL2CPP_ABORT();",
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
				"CHAOS_IL2CPP_INTPTR chaos_arg_0",
				[
					"    if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))",
					"    {",
					"        CHAOS_IL2CPP_ABORT();",
					"    }",
					string.Empty,
					$"    return *reinterpret_cast<{valueTypeSymbol2}*>(chaos_arg_0);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()), valueTypeAbi, new HashSet<int> { 0 });
			return true;
		}
		if (MatchesMethodSubject(callee, "System.Runtime.InteropServices/Marshal", "StringToCoTaskMemUTF8", "System.String"))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"CHAOS_IL2CPP_INTPTR",
				GetExternalRuntimeHelperSymbol(callee),
				"CHAOS_IL2CPP_INTPTR chaos_arg_0",
				[
					"    if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))",
					"    {",
					"        return static_cast<CHAOS_IL2CPP_INTPTR>(0);",
					"    }",
					string.Empty,
					$"    auto* chaos_string = reinterpret_cast<{GetNativeTypeSymbol(StringTypeSubjectId)}*>(chaos_arg_0);",
					"    (void)chaos_length; (void)chaos_utf8_data;",
					"    auto* chaos_runtime = chaos::il2cpp::runtime_core::GetCurrentRuntimeState();",
					"    if (chaos_runtime == nullptr) { CHAOS_IL2CPP_ABORT(); }",
					"    return chaos::il2cpp::runtime_core::MarshalStringToCoTaskMemUtf8(", 
					"        chaos_runtime, nullptr, reinterpret_cast<void*>(chaos_arg_0));",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)), CreateNativeIntAbiSlot(), new HashSet<int> { 0 });
			return true;
		}
		if (MatchesMethodSubject(callee, "System.Runtime.InteropServices/Marshal", "PtrToStringUTF8", "System.IntPtr"))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"CHAOS_IL2CPP_INTPTR",
				GetExternalRuntimeHelperSymbol(callee),
				"CHAOS_IL2CPP_INTPTR chaos_arg_0",
				[
					"    return chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0)",
					"        ? static_cast<CHAOS_IL2CPP_INTPTR>(0)",
					"        : chaos_reflection_create_string_literal(reinterpret_cast<const char*>(chaos_arg_0));",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0 });
			return true;
		}
		if (MatchesMethodSubject(callee, "System.Runtime.InteropServices/Marshal", "FreeCoTaskMem", "System.IntPtr"))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"void",
				GetExternalRuntimeHelperSymbol(callee),
				"CHAOS_IL2CPP_INTPTR chaos_arg_0",
				[
					"    auto* chaos_runtime = chaos::il2cpp::runtime_core::GetCurrentRuntimeState();",
					"    if (chaos_runtime == nullptr) { CHAOS_IL2CPP_ABORT(); }",
					"    (void)chaos::il2cpp::runtime_core::MarshalFreeCoTaskMem(chaos_runtime, chaos_arg_0);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()), new AotCoreIrAbiSlotArtifact
			{
				CarrierKindCode = AotCoreIrAbiCarrierKind.Void,
				TypeShape = (AotCoreIrTypeShapeKind)0
			}, new HashSet<int> { 0 });
			return true;
		}
		if (TryCreateMarshalCopyRuntimeHelperDefinition(callee, out helperDefinition))
		{
			return true;
		}
		return false;
	}

	private static readonly IReadOnlyDictionary<string, string> MarshalCopyElementTypeMap = new Dictionary<string, string>
	{
		["System.Byte"] = "CHAOS_IL2CPP_UINT8",
		["System.Int16"] = "CHAOS_IL2CPP_INT16",
		["System.Int32"] = "CHAOS_IL2CPP_INT32",
		["System.Int64"] = "CHAOS_IL2CPP_INT64",
		["System.IntPtr"] = "CHAOS_IL2CPP_INTPTR",
	};

	private bool TryCreateMarshalCopyRuntimeHelperDefinition(string callee, out ExternalRuntimeHelperDefinition? helperDefinition)
	{
		helperDefinition = null;
		if (!string.Equals(GetMethodDeclaringTypeSubjectId(callee), "System.Runtime.InteropServices/Marshal", StringComparison.Ordinal) ||
		    !string.Equals(GetMethodName(callee), "Copy", StringComparison.Ordinal))
		{
			return false;
		}
		var parameterTypes = GetMethodParameterTypes(callee);
		if (parameterTypes.Count != 4)
		{
			return false;
		}

		bool isArrayToPtr;
		string elementTypeName;
		if (parameterTypes[0].EndsWith("[]", StringComparison.Ordinal))
		{
			isArrayToPtr = true;
			elementTypeName = parameterTypes[0].Substring(0, parameterTypes[0].Length - 2);
		}
		else if (string.Equals(parameterTypes[0], "System.IntPtr", StringComparison.Ordinal) &&
		         parameterTypes[1].EndsWith("[]", StringComparison.Ordinal))
		{
			isArrayToPtr = false;
			elementTypeName = parameterTypes[1].Substring(0, parameterTypes[1].Length - 2);
		}
		else
		{
			return false;
		}

		if (!MarshalCopyElementTypeMap.TryGetValue(elementTypeName, out var cppElementType))
		{
			return false;
		}

		AotCoreIrAbiSlotArtifact voidReturnAbi = new()
		{
			CarrierKindCode = AotCoreIrAbiCarrierKind.Void,
			TypeShape = (AotCoreIrTypeShapeKind)0,
		};

		AotCoreIrAbiSlotArtifact[] paramAbis;
		string paramSignature;
		string[] bodyLines;

		if (isArrayToPtr)
		{
			paramSignature = "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2, CHAOS_IL2CPP_INT32 chaos_arg_3";
			bodyLines =
			[
				"    auto* chaos_array = reinterpret_cast<void*>(chaos_arg_0);",
				"    auto chaos_start_index = chaos_arg_1;",
				"    auto chaos_dest = chaos_arg_2;",
				"    auto chaos_length = chaos_arg_3;",
				$"    chaos::il2cpp::runtime_core::MarshalCopyArrayToPtr<{cppElementType}>(chaos_array, chaos_start_index, chaos_dest, chaos_length);",
			];
			paramAbis =
			[
				CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
				CreateInt32AbiSlot(),
				CreateNativeIntAbiSlot(),
				CreateInt32AbiSlot(),
			];
		}
		else
		{
			paramSignature = "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INT32 chaos_arg_2, CHAOS_IL2CPP_INT32 chaos_arg_3";
			bodyLines =
			[
				"    auto chaos_source = chaos_arg_0;",
				"    auto* chaos_array = reinterpret_cast<void*>(chaos_arg_1);",
				"    auto chaos_start_index = chaos_arg_2;",
				"    auto chaos_length = chaos_arg_3;",
				$"    chaos::il2cpp::runtime_core::MarshalCopyPtrToArray<{cppElementType}>(chaos_source, chaos_array, chaos_start_index, chaos_length);",
			];
			paramAbis =
			[
				CreateNativeIntAbiSlot(),
				CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
				CreateInt32AbiSlot(),
				CreateInt32AbiSlot(),
			];
		}

		helperDefinition = new ExternalRuntimeHelperDefinition(
			callee,
			GetExternalRuntimeHelperSymbol(callee),
			RenderSimpleExternalRuntimeHelper("void", GetExternalRuntimeHelperSymbol(callee), paramSignature, bodyLines),
			new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(paramAbis),
			voidReturnAbi,
			new HashSet<int> { 0, 1, 2, 3 });
		return true;
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
			"CHAOS_IL2CPP_INTPTR",
			GetExternalRuntimeHelperSymbol(callee),
			"CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2",
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
