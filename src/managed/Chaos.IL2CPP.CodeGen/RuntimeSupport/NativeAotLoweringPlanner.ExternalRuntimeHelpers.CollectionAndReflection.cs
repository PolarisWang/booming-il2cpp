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
	private bool TryCreateSpanRuntimeHelperDefinition(string callee, out ExternalRuntimeHelperDefinition? helperDefinition)
	{
		helperDefinition = null;
		AotCoreIrAbiSlotArtifact returnAbi = new AotCoreIrAbiSlotArtifact
		{
			CarrierKindCode = AotCoreIrAbiCarrierKind.Void,
			TypeShape = (AotCoreIrTypeShapeKind)0
		};
		if (string.Equals(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray(System.Array,System.RuntimeFieldHandle)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"void",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1",
				[
					"    chaos_initialize_array_from_field_data_int32(chaos_arg_0, chaos_arg_1);",
				]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
			{
				CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
				CreateNativeIntAbiSlot()
			}), returnAbi, new HashSet<int> { 1 });
			return true;
		}
		if (TryReadSingleGenericTypeArgument(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::CreateSpan<", out string typeSubjectId) && IsInt32ElementType(typeSubjectId))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    return chaos_create_field_data_span_int32(chaos_arg_0);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()), CreateNativeIntAbiSlot(), new HashSet<int> { 0 });
			return true;
		}
		if (TryReadSingleGenericTypeArgument(callee, "System.Memory/System.MemoryExtensions::AsSpan<", out string typeSubjectId2) && IsInt32ElementType(typeSubjectId2) && callee.Contains("(System.Int32[],System.Int32,System.Int32)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0, std::int32_t chaos_arg_1, std::int32_t chaos_arg_2",
				[
					"    return chaos_create_array_span_int32(chaos_arg_0, chaos_arg_1, chaos_arg_2);",
				]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
			{
				CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
				CreateInt32AbiSlot(),
				CreateInt32AbiSlot()
			}), CreateNativeIntAbiSlot(), new HashSet<int> { 0 });
			return true;
		}
		if (TryReadSingleGenericTypeArgument(callee, "System.Memory/System.MemoryExtensions::AsMemory<", out string typeSubjectId3) && IsInt32ElementType(typeSubjectId3) && callee.Contains("(System.Int32[])", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    return chaos_create_memory_int32(chaos_arg_0);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)), CreateNativeIntAbiSlot(), new HashSet<int> { 0 });
			return true;
		}
		if (TryReadSingleGenericTypeArgument(callee, "System.Memory/System.MemoryExtensions::AsMemory<", out string typeSubjectId4) && IsInt32ElementType(typeSubjectId4) && callee.Contains("(System.Int32[],System.Int32,System.Int32)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0, std::int32_t chaos_arg_1, std::int32_t chaos_arg_2",
				[
					"    return chaos_create_array_memory_int32(chaos_arg_0, chaos_arg_1, chaos_arg_2);",
				]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
			{
				CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
				CreateInt32AbiSlot(),
				CreateInt32AbiSlot()
			}), CreateNativeIntAbiSlot(), new HashSet<int> { 0 });
			return true;
		}
		string methodDeclaringTypeSubjectId = GetMethodDeclaringTypeSubjectId(callee);
		string methodName = GetMethodName(callee);
		if (TryReadGenericArgumentList(methodDeclaringTypeSubjectId, "System.Private.CoreLib/System.Span<", out string genericArgumentList))
		{
			IReadOnlyList<string> readOnlyList = SplitTopLevelGenericArguments(genericArgumentList);
			if (readOnlyList.Count == 1 && IsInt32ElementType(readOnlyList[0]))
			{
				if (string.Equals(methodName, "op_Implicit", StringComparison.Ordinal) && callee.Contains("(System.Int32[])", StringComparison.Ordinal))
				{
					helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
						"std::intptr_t",
						GetExternalRuntimeHelperSymbol(callee),
						"std::intptr_t chaos_arg_0",
						[
							"    if (chaos_arg_0 == static_cast<std::intptr_t>(0))",
							"    {",
							"        return chaos_create_raw_span_int32(nullptr, 0, false);",
							"    }",
							string.Empty,
							"    auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_arg_0);",
							"    return chaos_create_array_span_int32(chaos_arg_0, 0, static_cast<std::int32_t>(chaos_array->length));",
						]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)), CreateNativeIntAbiSlot(), new HashSet<int> { 0 });
					return true;
				}
				if (string.Equals(methodName, "get_Item", StringComparison.Ordinal))
				{
					helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
						"std::intptr_t",
						GetExternalRuntimeHelperSymbol(callee),
						"std::intptr_t chaos_arg_0, std::int32_t chaos_arg_1",
						[
							"    const auto chaos_span_handle = *chaos_resolve_native_int_slot(chaos_arg_0);",
							"    return chaos_span_int32_get_item_address(chaos_span_handle, chaos_arg_1);",
						]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
					{
						CreateNativeIntAbiSlot(),
						CreateInt32AbiSlot()
					}), CreateNativeIntAbiSlot(), new HashSet<int> { 0 });
					return true;
				}
				if (string.Equals(methodName, "get_Length", StringComparison.Ordinal))
				{
					helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
						"std::int32_t",
						GetExternalRuntimeHelperSymbol(callee),
						"std::intptr_t chaos_arg_0",
						[
							"    const auto chaos_span_handle = *chaos_resolve_native_int_slot(chaos_arg_0);",
							"    return chaos_span_int32_get_length(chaos_span_handle);",
						]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()), CreateInt32AbiSlot(), new HashSet<int> { 0 });
					return true;
				}
				if (string.Equals(methodName, ".ctor", StringComparison.Ordinal) && callee.Contains("(System.Void*,System.Int32)", StringComparison.Ordinal))
				{
					helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
						"void",
						GetExternalRuntimeHelperSymbol(callee),
						"std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1, std::int32_t chaos_arg_2",
						[
							"    auto* chaos_span_slot = chaos_resolve_native_int_slot(chaos_arg_0);",
							"    *chaos_span_slot = chaos_create_raw_span_int32(reinterpret_cast<void*>(chaos_arg_1), chaos_arg_2, false);",
						]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
					{
						CreateNativeIntAbiSlot(),
						CreateNativeIntAbiSlot(),
						CreateInt32AbiSlot()
					}), returnAbi, new HashSet<int> { 0, 1 });
					return true;
				}
			}
		}
		if (TryReadGenericArgumentList(methodDeclaringTypeSubjectId, "System.Private.CoreLib/System.ReadOnlySpan<", out string genericArgumentList2))
		{
			IReadOnlyList<string> readOnlyList2 = SplitTopLevelGenericArguments(genericArgumentList2);
			if (readOnlyList2.Count == 1 && IsInt32ElementType(readOnlyList2[0]))
			{
				if (string.Equals(methodName, "get_Item", StringComparison.Ordinal))
				{
					helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
						"std::intptr_t",
						GetExternalRuntimeHelperSymbol(callee),
						"std::intptr_t chaos_arg_0, std::int32_t chaos_arg_1",
						[
							"    const auto chaos_span_handle = *chaos_resolve_native_int_slot(chaos_arg_0);",
							"    return chaos_span_int32_get_item_address(chaos_span_handle, chaos_arg_1);",
						]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
					{
						CreateNativeIntAbiSlot(),
						CreateInt32AbiSlot()
					}), CreateNativeIntAbiSlot(), new HashSet<int> { 0 });
					return true;
				}
				if (string.Equals(methodName, "get_Length", StringComparison.Ordinal))
				{
					helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
						"std::int32_t",
						GetExternalRuntimeHelperSymbol(callee),
						"std::intptr_t chaos_arg_0",
						[
							"    const auto chaos_span_handle = *chaos_resolve_native_int_slot(chaos_arg_0);",
							"    return chaos_span_int32_get_length(chaos_span_handle);",
						]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()), CreateInt32AbiSlot(), new HashSet<int> { 0 });
					return true;
				}
			}
		}
		if (TryReadGenericArgumentList(methodDeclaringTypeSubjectId, "System.Private.CoreLib/System.Memory<", out string genericArgumentList3))
		{
			IReadOnlyList<string> readOnlyList3 = SplitTopLevelGenericArguments(genericArgumentList3);
			if (readOnlyList3.Count == 1 && IsInt32ElementType(readOnlyList3[0]) && string.Equals(methodName, "get_Span", StringComparison.Ordinal))
			{
				helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
					"std::intptr_t",
					GetExternalRuntimeHelperSymbol(callee),
					"std::intptr_t chaos_arg_0",
					[
						"    const auto chaos_memory_handle = *chaos_resolve_native_int_slot(chaos_arg_0);",
						"    return chaos_memory_int32_get_span(chaos_memory_handle);",
					]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()), CreateNativeIntAbiSlot(), new HashSet<int> { 0 });
				return true;
			}
		}
		return false;
		static bool IsInt32ElementType(string typeNameOrSubjectId)
		{
			return string.Equals(GetTypeDisplayName(typeNameOrSubjectId), "System.Int32", StringComparison.Ordinal);
		}
	}

	private bool TryCreateCollectionRuntimeHelperDefinition(string callee, out ExternalRuntimeHelperDefinition? helperDefinition)
	{
		helperDefinition = null;
		string methodDeclaringTypeSubjectId = GetMethodDeclaringTypeSubjectId(callee);
		string methodName = GetMethodName(callee);
		if ((TryParseClosedReadOnlyCollectionElementType(methodDeclaringTypeSubjectId, out string elementTypeNameOrSubjectId) || TryParseClosedReadOnlyListElementType(methodDeclaringTypeSubjectId, out elementTypeNameOrSubjectId)) && TryCreateReadOnlyCollectionInterfaceRuntimeHelperDefinition(callee, methodDeclaringTypeSubjectId, methodName, elementTypeNameOrSubjectId, out helperDefinition))
		{
			return true;
		}
		if (TryParseClosedListElementType(methodDeclaringTypeSubjectId, out string elementTypeNameOrSubjectId2))
		{
			if (!TryCreateResolvedTypeAbiSlot(elementTypeNameOrSubjectId2, out AotCoreIrAbiSlotArtifact abiSlot))
			{
				return false;
			}
			string value = MapAbiSlotParameterType(abiSlot);
			string value2 = MapAbiSlotReturnType(abiSlot);
			string value3 = $"    if (chaos_arg_0 == static_cast<std::intptr_t>(0))\n    {{\n        std::abort();\n    }}\n\n    auto* chaos_list = reinterpret_cast<{GetNativeTypeSymbol(methodDeclaringTypeSubjectId)}*>(chaos_arg_0);\n    if (chaos_list->header.type_id != {GetNativeTypeIdSymbol(methodDeclaringTypeSubjectId)})\n    {{\n        std::abort();\n    }}";
			if (string.Equals(methodName, "Add", StringComparison.Ordinal))
			{
				helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), $"extern \"C\" void {GetExternalRuntimeHelperSymbol(callee)}(std::intptr_t chaos_arg_0, {value} chaos_arg_1)\n{{\n{value3}\n    auto* chaos_storage = chaos_require_list_runtime_storage<{value}>(chaos_arg_0);\n    chaos_storage->items.push_back(chaos_arg_1);\n}}", new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
				{
					CreateNativeIntAbiSlot(methodDeclaringTypeSubjectId, AotCoreIrTypeShapeKind.ReferenceType),
					abiSlot
				}), new AotCoreIrAbiSlotArtifact
				{
					CarrierKindCode = AotCoreIrAbiCarrierKind.Void,
					TypeShape = (AotCoreIrTypeShapeKind)0
				}, new HashSet<int> { 0 });
				return true;
			}
			if (string.Equals(methodName, "get_Count", StringComparison.Ordinal))
			{
				helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), $"extern \"C\" std::int32_t {GetExternalRuntimeHelperSymbol(callee)}(std::intptr_t chaos_arg_0)\n{{\n{value3}\n    auto* chaos_storage = chaos_require_list_runtime_storage<{value}>(chaos_arg_0);\n    return static_cast<std::int32_t>(chaos_storage->items.size());\n}}", new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot(methodDeclaringTypeSubjectId, AotCoreIrTypeShapeKind.ReferenceType)), CreateInt32AbiSlot(), new HashSet<int> { 0 });
				return true;
			}
			if (string.Equals(methodName, "get_Item", StringComparison.Ordinal))
			{
				helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), $"extern \"C\" {value2} {GetExternalRuntimeHelperSymbol(callee)}(std::intptr_t chaos_arg_0, std::int32_t chaos_arg_1)\n{{\n{value3}\n    auto* chaos_storage = chaos_require_list_runtime_storage<{value}>(chaos_arg_0);\n    if (chaos_arg_1 < 0 ||\n        static_cast<std::size_t>(chaos_arg_1) >= chaos_storage->items.size())\n    {{\n        std::abort();\n    }}\n\n    return chaos_storage->items[static_cast<std::size_t>(chaos_arg_1)];\n}}", new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
				{
					CreateNativeIntAbiSlot(methodDeclaringTypeSubjectId, AotCoreIrTypeShapeKind.ReferenceType),
					CreateInt32AbiSlot()
				}), abiSlot, new HashSet<int> { 0 });
				return true;
			}
		}
		if (TryParseClosedDictionaryStringIntType(methodDeclaringTypeSubjectId))
		{
			string value4 = $"    if (chaos_arg_0 == static_cast<std::intptr_t>(0))\n    {{\n        std::abort();\n    }}\n\n    auto* chaos_dictionary = reinterpret_cast<{GetNativeTypeSymbol(methodDeclaringTypeSubjectId)}*>(chaos_arg_0);\n    if (chaos_dictionary->header.type_id != {GetNativeTypeIdSymbol(methodDeclaringTypeSubjectId)})\n    {{\n        std::abort();\n    }}";
			if (string.Equals(methodName, "set_Item", StringComparison.Ordinal))
			{
				helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), $"extern \"C\" void {GetExternalRuntimeHelperSymbol(callee)}(std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1, std::int32_t chaos_arg_2)\n{{\n{value4}\n    auto* chaos_storage = chaos_require_dictionary_runtime_storage<std::intptr_t, std::int32_t>(chaos_arg_0);\n    for (auto& chaos_entry : chaos_storage->entries)\n    {{\n        if (chaos_object_equals(chaos_entry.first, chaos_arg_1))\n        {{\n            chaos_entry.second = chaos_arg_2;\n            return;\n        }}\n    }}\n\n    chaos_storage->entries.emplace_back(chaos_arg_1, chaos_arg_2);\n}}", new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
				{
					CreateNativeIntAbiSlot(methodDeclaringTypeSubjectId, AotCoreIrTypeShapeKind.ReferenceType),
					CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
					CreateInt32AbiSlot()
				}), new AotCoreIrAbiSlotArtifact
				{
					CarrierKindCode = AotCoreIrAbiCarrierKind.Void,
					TypeShape = (AotCoreIrTypeShapeKind)0
				}, new HashSet<int> { 0, 1 });
				return true;
			}
			if (string.Equals(methodName, "get_Count", StringComparison.Ordinal))
			{
				helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), $"extern \"C\" std::int32_t {GetExternalRuntimeHelperSymbol(callee)}(std::intptr_t chaos_arg_0)\n{{\n{value4}\n    auto* chaos_storage = chaos_require_dictionary_runtime_storage<std::intptr_t, std::int32_t>(chaos_arg_0);\n    return static_cast<std::int32_t>(chaos_storage->entries.size());\n}}", new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot(methodDeclaringTypeSubjectId, AotCoreIrTypeShapeKind.ReferenceType)), CreateInt32AbiSlot(), new HashSet<int> { 0 });
				return true;
			}
			if (string.Equals(methodName, "get_Item", StringComparison.Ordinal))
			{
				helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), $"extern \"C\" std::int32_t {GetExternalRuntimeHelperSymbol(callee)}(std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1)\n{{\n{value4}\n    auto* chaos_storage = chaos_require_dictionary_runtime_storage<std::intptr_t, std::int32_t>(chaos_arg_0);\n    for (const auto& chaos_entry : chaos_storage->entries)\n    {{\n        if (chaos_object_equals(chaos_entry.first, chaos_arg_1))\n        {{\n            return chaos_entry.second;\n        }}\n    }}\n\n    std::abort();\n}}", new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
				{
					CreateNativeIntAbiSlot(methodDeclaringTypeSubjectId, AotCoreIrTypeShapeKind.ReferenceType),
					CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)
				}), CreateInt32AbiSlot(), new HashSet<int> { 0, 1 });
				return true;
			}
		}
		return false;
	}

	private bool TryCreateReadOnlyCollectionInterfaceRuntimeHelperDefinition(string callee, string declaringTypeSubjectId, string methodName, string elementTypeNameOrSubjectId, out ExternalRuntimeHelperDefinition? helperDefinition)
	{
		helperDefinition = null;
		if (!TryCreateResolvedTypeAbiSlot(elementTypeNameOrSubjectId, out AotCoreIrAbiSlotArtifact abiSlot))
		{
			return false;
		}
		string typeDisplayName = GetTypeDisplayName(elementTypeNameOrSubjectId);
		string value = MapAbiSlotParameterType(abiSlot);
		string value2 = MapAbiSlotReturnType(abiSlot);
		string subjectId;
		bool flag = TryResolveCollectionArrayElementTypeSubjectId(elementTypeNameOrSubjectId, out subjectId);
		string typeNameOrSubjectId = "System.Collections/System.Collections.Generic.List<" + typeDisplayName + ">";
		string subjectId2;
		bool flag2 = TryResolveReferenceTypeSubjectId(typeNameOrSubjectId, out subjectId2);
		if (!flag && !flag2)
		{
			return false;
		}
		StringBuilder stringBuilder2;
		StringBuilder.AppendInterpolatedStringHandler handler;
		if (string.Equals(methodName, "get_Count", StringComparison.Ordinal))
		{
			StringBuilder stringBuilder = new StringBuilder();
			stringBuilder2 = stringBuilder;
			StringBuilder stringBuilder3 = stringBuilder2;
			handler = new StringBuilder.AppendInterpolatedStringHandler(51, 1, stringBuilder2);
			handler.AppendLiteral("extern \"C\" std::int32_t ");
			handler.AppendFormatted(GetExternalRuntimeHelperSymbol(callee));
			handler.AppendLiteral("(std::intptr_t chaos_arg_0)");
			stringBuilder3.AppendLine(ref handler);
			stringBuilder.AppendLine("{");
			stringBuilder.AppendLine("    if (chaos_arg_0 == static_cast<std::intptr_t>(0))");
			stringBuilder.AppendLine("    {");
			stringBuilder.AppendLine("        std::abort();");
			stringBuilder.AppendLine("    }");
			stringBuilder.AppendLine();
			if (flag)
			{
				stringBuilder.AppendLine("    auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_arg_0);");
				stringBuilder.AppendLine("    if (chaos_array->header.type_id == chaos_type_id_managed_array &&");
				stringBuilder2 = stringBuilder;
				StringBuilder stringBuilder4 = stringBuilder2;
				handler = new StringBuilder.AppendInterpolatedStringHandler(41, 1, stringBuilder2);
				handler.AppendLiteral("        chaos_array->element_type_id == ");
				handler.AppendFormatted(GetNativeTypeIdSymbol(subjectId));
				handler.AppendLiteral(")");
				stringBuilder4.AppendLine(ref handler);
				stringBuilder.AppendLine("    {");
				stringBuilder.AppendLine("        return static_cast<std::int32_t>(chaos_array->length);");
				stringBuilder.AppendLine("    }");
				stringBuilder.AppendLine();
			}
			if (flag2)
			{
				stringBuilder2 = stringBuilder;
				StringBuilder stringBuilder5 = stringBuilder2;
				handler = new StringBuilder.AppendInterpolatedStringHandler(56, 1, stringBuilder2);
				handler.AppendLiteral("    auto* chaos_list = reinterpret_cast<");
				handler.AppendFormatted(GetNativeTypeSymbol(subjectId2));
				handler.AppendLiteral("*>(chaos_arg_0);");
				stringBuilder5.AppendLine(ref handler);
				stringBuilder2 = stringBuilder;
				StringBuilder stringBuilder6 = stringBuilder2;
				handler = new StringBuilder.AppendInterpolatedStringHandler(39, 1, stringBuilder2);
				handler.AppendLiteral("    if (chaos_list->header.type_id == ");
				handler.AppendFormatted(GetNativeTypeIdSymbol(subjectId2));
				handler.AppendLiteral(")");
				stringBuilder6.AppendLine(ref handler);
				stringBuilder.AppendLine("    {");
				stringBuilder2 = stringBuilder;
				StringBuilder stringBuilder7 = stringBuilder2;
				handler = new StringBuilder.AppendInterpolatedStringHandler(80, 1, stringBuilder2);
				handler.AppendLiteral("        auto* chaos_storage = chaos_require_list_runtime_storage<");
				handler.AppendFormatted(value);
				handler.AppendLiteral(">(chaos_arg_0);");
				stringBuilder7.AppendLine(ref handler);
				stringBuilder.AppendLine("        return static_cast<std::int32_t>(chaos_storage->items.size());");
				stringBuilder.AppendLine("    }");
				stringBuilder.AppendLine();
			}
			stringBuilder.AppendLine("    std::abort();");
			stringBuilder.AppendLine("}");
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), stringBuilder.ToString().TrimEnd(), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot(declaringTypeSubjectId, AotCoreIrTypeShapeKind.ReferenceType)), CreateInt32AbiSlot(), new HashSet<int> { 0 });
			return true;
		}
		if (!string.Equals(methodName, "get_Item", StringComparison.Ordinal) || !TryCreateCollectionArrayElementExpression(abiSlot, "chaos_array->elements[static_cast<std::size_t>(chaos_arg_1)]", out string elementExpression))
		{
			return false;
		}
		StringBuilder stringBuilder8 = new StringBuilder();
		stringBuilder2 = stringBuilder8;
		StringBuilder stringBuilder9 = stringBuilder2;
		handler = new StringBuilder.AppendInterpolatedStringHandler(65, 2, stringBuilder2);
		handler.AppendLiteral("extern \"C\" ");
		handler.AppendFormatted(value2);
		handler.AppendLiteral(" ");
		handler.AppendFormatted(GetExternalRuntimeHelperSymbol(callee));
		handler.AppendLiteral("(std::intptr_t chaos_arg_0, std::int32_t chaos_arg_1)");
		stringBuilder9.AppendLine(ref handler);
		stringBuilder8.AppendLine("{");
		stringBuilder8.AppendLine("    if (chaos_arg_0 == static_cast<std::intptr_t>(0))");
		stringBuilder8.AppendLine("    {");
		stringBuilder8.AppendLine("        std::abort();");
		stringBuilder8.AppendLine("    }");
		stringBuilder8.AppendLine();
		if (flag)
		{
			stringBuilder8.AppendLine("    auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_arg_0);");
			stringBuilder8.AppendLine("    if (chaos_array->header.type_id == chaos_type_id_managed_array &&");
			stringBuilder2 = stringBuilder8;
			StringBuilder stringBuilder10 = stringBuilder2;
			handler = new StringBuilder.AppendInterpolatedStringHandler(41, 1, stringBuilder2);
			handler.AppendLiteral("        chaos_array->element_type_id == ");
			handler.AppendFormatted(GetNativeTypeIdSymbol(subjectId));
			handler.AppendLiteral(")");
			stringBuilder10.AppendLine(ref handler);
			stringBuilder8.AppendLine("    {");
			stringBuilder8.AppendLine("        if (chaos_arg_1 < 0 || static_cast<std::intptr_t>(chaos_arg_1) >= chaos_array->length)");
			stringBuilder8.AppendLine("        {");
			stringBuilder8.AppendLine("            std::abort();");
			stringBuilder8.AppendLine("        }");
			stringBuilder8.AppendLine();
			stringBuilder2 = stringBuilder8;
			StringBuilder stringBuilder11 = stringBuilder2;
			handler = new StringBuilder.AppendInterpolatedStringHandler(16, 1, stringBuilder2);
			handler.AppendLiteral("        return ");
			handler.AppendFormatted(elementExpression);
			handler.AppendLiteral(";");
			stringBuilder11.AppendLine(ref handler);
			stringBuilder8.AppendLine("    }");
			stringBuilder8.AppendLine();
		}
		if (flag2)
		{
			stringBuilder2 = stringBuilder8;
			StringBuilder stringBuilder12 = stringBuilder2;
			handler = new StringBuilder.AppendInterpolatedStringHandler(56, 1, stringBuilder2);
			handler.AppendLiteral("    auto* chaos_list = reinterpret_cast<");
			handler.AppendFormatted(GetNativeTypeSymbol(subjectId2));
			handler.AppendLiteral("*>(chaos_arg_0);");
			stringBuilder12.AppendLine(ref handler);
			stringBuilder2 = stringBuilder8;
			StringBuilder stringBuilder13 = stringBuilder2;
			handler = new StringBuilder.AppendInterpolatedStringHandler(39, 1, stringBuilder2);
			handler.AppendLiteral("    if (chaos_list->header.type_id == ");
			handler.AppendFormatted(GetNativeTypeIdSymbol(subjectId2));
			handler.AppendLiteral(")");
			stringBuilder13.AppendLine(ref handler);
			stringBuilder8.AppendLine("    {");
			stringBuilder2 = stringBuilder8;
			StringBuilder stringBuilder14 = stringBuilder2;
			handler = new StringBuilder.AppendInterpolatedStringHandler(80, 1, stringBuilder2);
			handler.AppendLiteral("        auto* chaos_storage = chaos_require_list_runtime_storage<");
			handler.AppendFormatted(value);
			handler.AppendLiteral(">(chaos_arg_0);");
			stringBuilder14.AppendLine(ref handler);
			stringBuilder8.AppendLine("        if (chaos_arg_1 < 0 || static_cast<std::size_t>(chaos_arg_1) >= chaos_storage->items.size())");
			stringBuilder8.AppendLine("        {");
			stringBuilder8.AppendLine("            std::abort();");
			stringBuilder8.AppendLine("        }");
			stringBuilder8.AppendLine();
			stringBuilder8.AppendLine("        return chaos_storage->items[static_cast<std::size_t>(chaos_arg_1)];");
			stringBuilder8.AppendLine("    }");
			stringBuilder8.AppendLine();
		}
		stringBuilder8.AppendLine("    std::abort();");
		stringBuilder8.AppendLine("}");
		helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), stringBuilder8.ToString().TrimEnd(), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
		{
			CreateNativeIntAbiSlot(declaringTypeSubjectId, AotCoreIrTypeShapeKind.ReferenceType),
			CreateInt32AbiSlot()
		}), abiSlot, new HashSet<int> { 0 });
		return true;
	}

	private bool TryResolveCollectionArrayElementTypeSubjectId(string elementTypeNameOrSubjectId, out string subjectId)
	{
		if (TryResolveReferenceTypeSubjectId(elementTypeNameOrSubjectId, out subjectId))
		{
			return true;
		}
		return TryResolveKnownTypeSubjectId(elementTypeNameOrSubjectId, out subjectId);
	}

	private bool TryCreateCustomAttributeRuntimeHelperDefinition(string callee, out ExternalRuntimeHelperDefinition? helperDefinition)
	{
		helperDefinition = null;
		if (_customAttributeSupport.UsesMemberInfoIsDefined && string.Equals(callee, "System.Private.CoreLib/System.Reflection.MemberInfo::IsDefined(System.Type,System.Boolean)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::int32_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1, std::int32_t chaos_arg_2",
				[
					"    (void)chaos_arg_2;",
					"    if (chaos_arg_0 == static_cast<std::intptr_t>(0) || chaos_arg_1 == static_cast<std::intptr_t>(0))",
					"    {",
					"        return 0;",
					"    }",
					string.Empty,
					$"    auto* chaos_type = reinterpret_cast<{GetNativeTypeSymbol("System.Private.CoreLib/System.Type")}*>(chaos_arg_1);",
					"    if (chaos_type->runtime_type_handle == static_cast<std::intptr_t>(0))",
					"    {",
					"        return 0;",
					"    }",
					string.Empty,
					"    return chaos_reflection_get_custom_attribute(chaos_arg_0, chaos_type->runtime_type_handle) == static_cast<std::intptr_t>(0)",
					"        ? 0",
					"        : 1;",
				]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
			{
				CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
				CreateInt32AbiSlot()
			}), CreateInt32AbiSlot(), new HashSet<int> { 0, 1 });
			return true;
		}
		if (_customAttributeSupport.QueryAttributeTypeByCallee.TryGetValue(callee, out string value))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					$"    return chaos_reflection_get_custom_attribute(chaos_arg_0, {GetTypeHandleLiteral(value)});",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)), CreateNativeIntAbiSlot(value, AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0 });
			return true;
		}
		if (_customAttributeSupport.SyntheticGetterFieldByMethodSubjectId.TryGetValue(callee, out string value2))
		{
			string declaringTypeSubjectId = GetDeclaringTypeSubjectId(value2);
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
					$"    auto* chaos_attribute = reinterpret_cast<{GetNativeTypeSymbol(declaringTypeSubjectId)}*>(chaos_arg_0);",
					$"    return chaos_attribute->{GetNativeFieldMemberName(value2)};",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot(declaringTypeSubjectId, AotCoreIrTypeShapeKind.ReferenceType)), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0 });
			return true;
		}
		return false;
	}

	private bool TryCreateObjectRuntimeHelperDefinition(string callee, out ExternalRuntimeHelperDefinition? helperDefinition)
	{
		helperDefinition = null;
		AotCoreIrAbiSlotArtifact returnVoidAbi = new AotCoreIrAbiSlotArtifact
		{
			CarrierKindCode = AotCoreIrAbiCarrierKind.Void,
			TypeShape = (AotCoreIrTypeShapeKind)0
		};
		if (string.Equals(callee, "System.Private.CoreLib/System.Environment::get_CurrentManagedThreadId()", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::int32_t",
				GetExternalRuntimeHelperSymbol(callee),
				string.Empty,
				[
					"    return chaos_current_managed_thread_id;",
				]), Array.Empty<AotCoreIrAbiSlotArtifact>(), CreateInt32AbiSlot(), EmptyRawArgumentIndices);
			return true;
		}
		if (string.Equals(callee, "System.Threading.Thread/System.Threading.Thread::.ctor(System.Threading.ThreadStart)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"void",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1",
				[
					"    auto& chaos_thread_entry = chaos_require_thread_runtime_entry(chaos_arg_0);",
					"    std::lock_guard<std::mutex> chaos_guard(chaos_thread_entry.mutex);",
					"    chaos_thread_entry.thread_start_delegate = chaos_arg_1;",
					"    if (chaos_thread_entry.managed_thread_id == 0)",
					"    {",
					"        chaos_thread_entry.managed_thread_id = chaos_allocate_managed_thread_id();",
					"    }",
				]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
			{
				CreateNativeIntAbiSlot("System.Threading.Thread/System.Threading.Thread", AotCoreIrTypeShapeKind.ReferenceType),
				CreateNativeIntAbiSlot("System.Threading.Thread/System.Threading.ThreadStart", AotCoreIrTypeShapeKind.ReferenceType)
			}), returnVoidAbi, new HashSet<int> { 0, 1 });
			return true;
		}
		if (string.Equals(callee, "System.Threading.Thread/System.Threading.Thread::Start()", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), $"extern \"C\" void {GetExternalRuntimeHelperSymbol(callee)}(std::intptr_t chaos_arg_0)\n{{\n    auto& chaos_thread_entry = chaos_require_thread_runtime_entry(chaos_arg_0);\n    std::lock_guard<std::mutex> chaos_guard(chaos_thread_entry.mutex);\n    if (chaos_thread_entry.worker != nullptr ||\n        chaos_thread_entry.thread_start_delegate == static_cast<std::intptr_t>(0))\n    {{\n        std::abort();\n    }}\n\n    if (chaos_thread_entry.managed_thread_id == 0)\n    {{\n        chaos_thread_entry.managed_thread_id = chaos_allocate_managed_thread_id();\n    }}\n\n    const auto chaos_delegate_value = chaos_thread_entry.thread_start_delegate;\n    const auto chaos_managed_thread_id = chaos_thread_entry.managed_thread_id;\n    chaos_thread_entry.worker = std::make_unique<std::thread>(\n        [chaos_delegate_value, chaos_managed_thread_id, chaos_arg_0]()\n        {{\n            chaos_current_thread_object = chaos_arg_0;\n            chaos_current_managed_thread_id = chaos_managed_thread_id;\n\n            const auto chaos_invoke_single_delegate = [](const {GetNativeTypeSymbol("System.Private.CoreLib/System.Delegate")}* chaos_invocation_delegate)\n            {{\n                if (chaos_invocation_delegate->chaos_delegate_method_ptr == static_cast<std::intptr_t>(0))\n                {{\n                    std::abort();\n                }}\n\n                if (chaos_invocation_delegate->chaos_delegate_target == static_cast<std::intptr_t>(0))\n                {{\n                    const auto chaos_open_function =\n                        reinterpret_cast<void(*)()>(chaos_invocation_delegate->chaos_delegate_method_ptr);\n                    chaos_open_function();\n                    return;\n                }}\n\n                const auto chaos_closed_function =\n                    reinterpret_cast<void(*)(std::intptr_t)>(chaos_invocation_delegate->chaos_delegate_method_ptr);\n                chaos_closed_function(chaos_invocation_delegate->chaos_delegate_target);\n            }};\n\n            const auto* chaos_delegate = chaos_require_delegate(chaos_delegate_value);\n            if (const auto* chaos_invocation_list = chaos_try_get_delegate_invocation_list(chaos_delegate);\n                chaos_invocation_list != nullptr)\n            {{\n                for (const auto chaos_entry_value : *chaos_invocation_list)\n                {{\n                    chaos_invoke_single_delegate(chaos_require_delegate(chaos_entry_value));\n                }}\n\n                return;\n            }}\n\n            chaos_invoke_single_delegate(chaos_delegate);\n        }});\n}}", new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot("System.Threading.Thread/System.Threading.Thread", AotCoreIrTypeShapeKind.ReferenceType)), new AotCoreIrAbiSlotArtifact
			{
				CarrierKindCode = AotCoreIrAbiCarrierKind.Void,
				TypeShape = (AotCoreIrTypeShapeKind)0
			}, new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Threading.Thread/System.Threading.Thread::Join()", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"void",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    auto& chaos_thread_entry = chaos_require_thread_runtime_entry(chaos_arg_0);",
					"    std::thread* chaos_worker = nullptr;",
					"    {",
					"        std::lock_guard<std::mutex> chaos_guard(chaos_thread_entry.mutex);",
					"        chaos_worker = chaos_thread_entry.worker.get();",
					"    }",
					string.Empty,
					"    if (chaos_worker != nullptr && chaos_worker->joinable())",
					"    {",
					"        chaos_worker->join();",
					"    }",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot("System.Threading.Thread/System.Threading.Thread", AotCoreIrTypeShapeKind.ReferenceType)), returnVoidAbi, new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Threading.Thread/System.Threading.Thread::get_CurrentThread()", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), $"extern \"C\" std::intptr_t {GetExternalRuntimeHelperSymbol(callee)}()\n{{\n    if (chaos_current_thread_object == static_cast<std::intptr_t>(0))\n    {{\n        auto* chaos_thread_object = new {GetNativeTypeSymbol("System.Threading.Thread/System.Threading.Thread")}{{}};\n        chaos_thread_object->header.type_id = {GetNativeTypeIdSymbol("System.Threading.Thread/System.Threading.Thread")};\n        chaos_current_thread_object = reinterpret_cast<std::intptr_t>(chaos_thread_object);\n        auto& chaos_thread_entry = chaos_require_thread_runtime_entry(chaos_current_thread_object);\n        if (chaos_thread_entry.managed_thread_id == 0)\n        {{\n            chaos_thread_entry.managed_thread_id = chaos_current_managed_thread_id;\n        }}\n    }}\n\n    return chaos_current_thread_object;\n}}", Array.Empty<AotCoreIrAbiSlotArtifact>(), CreateNativeIntAbiSlot("System.Threading.Thread/System.Threading.Thread", AotCoreIrTypeShapeKind.ReferenceType), EmptyRawArgumentIndices);
			return true;
		}
		if (string.Equals(callee, "System.Threading.Thread/System.Threading.Thread::get_Name()", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    auto& chaos_thread_entry = chaos_require_thread_runtime_entry(chaos_arg_0);",
					"    std::lock_guard<std::mutex> chaos_guard(chaos_thread_entry.mutex);",
					"    return chaos_thread_entry.name;",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot("System.Threading.Thread/System.Threading.Thread", AotCoreIrTypeShapeKind.ReferenceType)), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Threading.Thread/System.Threading.Thread::set_Name(System.String)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"void",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1",
				[
					"    auto& chaos_thread_entry = chaos_require_thread_runtime_entry(chaos_arg_0);",
					"    std::lock_guard<std::mutex> chaos_guard(chaos_thread_entry.mutex);",
					"    chaos_thread_entry.name = chaos_arg_1;",
				]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
			{
				CreateNativeIntAbiSlot("System.Threading.Thread/System.Threading.Thread", AotCoreIrTypeShapeKind.ReferenceType),
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)
			}), returnVoidAbi, new HashSet<int> { 0, 1 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.GC::Collect()", StringComparison.Ordinal) || string.Equals(callee, "System.Private.CoreLib/System.GC::WaitForPendingFinalizers()", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"void",
				GetExternalRuntimeHelperSymbol(callee),
				string.Empty,
				[
					"    std::abort();",
				]), Array.Empty<AotCoreIrAbiSlotArtifact>(), returnVoidAbi, EmptyRawArgumentIndices);
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.GC::KeepAlive(System.Object)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"void",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    (void)chaos_arg_0;",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)), returnVoidAbi, new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.TimeSpan::FromMilliseconds(System.Double)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), $"extern \"C\" {GetNativeValueTypeSymbol("System.Private.CoreLib/System.TimeSpan")} {GetExternalRuntimeHelperSymbol(callee)}(double chaos_arg_0)\n{{\n    const auto chaos_ticks_value = chaos_arg_0 * 10000.0;\n    if (chaos_ticks_value < static_cast<double>(std::numeric_limits<std::int64_t>::min()) ||\n        chaos_ticks_value > static_cast<double>(std::numeric_limits<std::int64_t>::max()))\n    {{\n        std::abort();\n    }}\n\n    const auto chaos_ticks = static_cast<std::int64_t>(\n        chaos_ticks_value + (chaos_ticks_value >= 0.0 ? 0.5 : -0.5));\n    {GetNativeValueTypeSymbol("System.Private.CoreLib/System.TimeSpan")} chaos_result{{}};\n    std::memcpy(&chaos_result, &chaos_ticks, sizeof(chaos_ticks));\n    return chaos_result;\n}}", new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact
			{
				CarrierKindCode = AotCoreIrAbiCarrierKind.Float64,
				TypeShape = AotCoreIrTypeShapeKind.ValueType
			}), new AotCoreIrAbiSlotArtifact
			{
				CarrierKindCode = AotCoreIrAbiCarrierKind.ValueTypeByValue,
				TypeSubjectId = "System.Private.CoreLib/System.TimeSpan",
				TypeShape = AotCoreIrTypeShapeKind.ValueType
			}, new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Threading/Monitor::Enter(System.Object,System.Boolean&)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), "extern \"C\" void " + GetExternalRuntimeHelperSymbol(callee) + "(std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1)\n{\n    auto& chaos_monitor_entry = chaos_require_monitor_runtime_entry(chaos_arg_0);\n    auto* chaos_lock_taken_slot = chaos_resolve_native_int_slot(chaos_arg_1);\n    *chaos_lock_taken_slot = static_cast<std::intptr_t>(0);\n    chaos_monitor_entry.mutex.lock();\n    *chaos_lock_taken_slot = static_cast<std::intptr_t>(1);\n}", new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
			{
				CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
				CreateNativeIntAbiSlot()
			}), new AotCoreIrAbiSlotArtifact
			{
				CarrierKindCode = AotCoreIrAbiCarrierKind.Void,
				TypeShape = (AotCoreIrTypeShapeKind)0
			}, new HashSet<int> { 0, 1 });
			return true;
		}
		if (string.Equals(callee, "System.Threading/Monitor::Exit(System.Object)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), "extern \"C\" void " + GetExternalRuntimeHelperSymbol(callee) + "(std::intptr_t chaos_arg_0)\n{\n    auto& chaos_monitor_entry = chaos_require_monitor_runtime_entry(chaos_arg_0);\n    chaos_monitor_entry.mutex.unlock();\n}", new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)), new AotCoreIrAbiSlotArtifact
			{
				CarrierKindCode = AotCoreIrAbiCarrierKind.Void,
				TypeShape = (AotCoreIrTypeShapeKind)0
			}, new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Threading/Monitor::TryEnter(System.Object,System.TimeSpan,System.Boolean&)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), $"extern \"C\" void {GetExternalRuntimeHelperSymbol(callee)}(std::intptr_t chaos_arg_0, {GetNativeValueTypeSymbol("System.Private.CoreLib/System.TimeSpan")} chaos_arg_1, std::intptr_t chaos_arg_2)\n{{\n    auto& chaos_monitor_entry = chaos_require_monitor_runtime_entry(chaos_arg_0);\n    auto* chaos_lock_taken_slot = chaos_resolve_native_int_slot(chaos_arg_2);\n    *chaos_lock_taken_slot = static_cast<std::intptr_t>(0);\n\n    std::int64_t chaos_ticks = 0;\n    std::memcpy(&chaos_ticks, &chaos_arg_1, sizeof(chaos_ticks));\n    if (chaos_ticks < 0)\n    {{\n        chaos_ticks = 0;\n    }}\n\n    const auto chaos_timeout = std::chrono::duration_cast<std::chrono::steady_clock::duration>(\n        std::chrono::nanoseconds(chaos_ticks * 100));\n    if (chaos_monitor_entry.mutex.try_lock_for(chaos_timeout))\n    {{\n        *chaos_lock_taken_slot = static_cast<std::intptr_t>(1);\n    }}\n}}", new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
			{
				CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
				new AotCoreIrAbiSlotArtifact
				{
					CarrierKindCode = AotCoreIrAbiCarrierKind.ValueTypeByValue,
					TypeSubjectId = "System.Private.CoreLib/System.TimeSpan",
					TypeShape = AotCoreIrTypeShapeKind.ValueType
				},
				CreateNativeIntAbiSlot()
			}), new AotCoreIrAbiSlotArtifact
			{
				CarrierKindCode = AotCoreIrAbiCarrierKind.Void,
				TypeShape = (AotCoreIrTypeShapeKind)0
			}, new HashSet<int> { 0, 2 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Object::.ctor()", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), "extern \"C\" void " + GetExternalRuntimeHelperSymbol(callee) + "(std::intptr_t chaos_arg_0)\n{\n    (void)chaos_arg_0;\n}", new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot("System.Private.CoreLib/System.Object", AotCoreIrTypeShapeKind.ReferenceType)), new AotCoreIrAbiSlotArtifact
			{
				CarrierKindCode = AotCoreIrAbiCarrierKind.Void,
				TypeShape = (AotCoreIrTypeShapeKind)0
			}, new HashSet<int> { 0 });
			return true;
		}
		if (!string.Equals(callee, "System.Private.CoreLib/System.Object::Equals(System.Object)", StringComparison.Ordinal))
		{
			return false;
		}
		helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), "extern \"C\" std::intptr_t " + GetExternalRuntimeHelperSymbol(callee) + "(std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1)\n{\n    return chaos_object_equals(chaos_arg_0, chaos_arg_1)\n        ? static_cast<std::intptr_t>(1)\n        : static_cast<std::intptr_t>(0);\n}", new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
		{
			CreateNativeIntAbiSlot(),
			CreateNativeIntAbiSlot()
		}), CreateNativeIntAbiSlot(), new HashSet<int> { 0, 1 });
		return true;
	}

	private bool TryCreateAsyncRuntimeHelperDefinition(string callee, out ExternalRuntimeHelperDefinition? helperDefinition)
	{
		helperDefinition = null;
		AotCoreIrAbiSlotArtifact returnVoidAbi = new AotCoreIrAbiSlotArtifact
		{
			CarrierKindCode = AotCoreIrAbiCarrierKind.Void,
			TypeShape = (AotCoreIrTypeShapeKind)0
		};
		if (string.Equals(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncTaskMethodBuilder<System.Int32>::Create()", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				string.Empty,
				[
					"    return chaos_async_task_int32_create();",
				]), Array.Empty<AotCoreIrAbiSlotArtifact>(), CreateNativeIntAbiSlot(), EmptyRawArgumentIndices);
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncValueTaskMethodBuilder<System.Int32>::Create()", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				string.Empty,
				[
					"    return chaos_async_task_int32_create();",
				]), Array.Empty<AotCoreIrAbiSlotArtifact>(), CreateNativeIntAbiSlot(), EmptyRawArgumentIndices);
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncTaskMethodBuilder<System.Int32>::get_Task()", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    return chaos_async_task_int32_builder_get_task(chaos_arg_0);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()), CreateNativeIntAbiSlot(), new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncValueTaskMethodBuilder<System.Int32>::get_Task()", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    return chaos_async_task_int32_builder_get_task(chaos_arg_0);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()), CreateNativeIntAbiSlot(), new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncTaskMethodBuilder<System.Int32>::SetResult(System.Int32)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"void",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0, std::int32_t chaos_arg_1",
				[
					"    chaos_async_task_int32_builder_set_result(chaos_arg_0, chaos_arg_1);",
				]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
			{
				CreateNativeIntAbiSlot(),
				CreateInt32AbiSlot()
			}), returnVoidAbi, new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncValueTaskMethodBuilder<System.Int32>::SetResult(System.Int32)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"void",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0, std::int32_t chaos_arg_1",
				[
					"    chaos_async_task_int32_builder_set_result(chaos_arg_0, chaos_arg_1);",
				]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
			{
				CreateNativeIntAbiSlot(),
				CreateInt32AbiSlot()
			}), returnVoidAbi, new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncTaskMethodBuilder<System.Int32>::SetException(System.Exception)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"void",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1",
				[
					"    chaos_async_task_int32_builder_set_exception(chaos_arg_0, chaos_arg_1);",
				]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
			{
				CreateNativeIntAbiSlot(),
				CreateNativeIntAbiSlot()
			}), returnVoidAbi, new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncValueTaskMethodBuilder<System.Int32>::SetException(System.Exception)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"void",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1",
				[
					"    chaos_async_task_int32_builder_set_exception(chaos_arg_0, chaos_arg_1);",
				]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
			{
				CreateNativeIntAbiSlot(),
				CreateNativeIntAbiSlot()
			}), returnVoidAbi, new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Threading.Tasks.Task::Yield()", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				string.Empty,
				[
					"    return chaos_async_yield_create();",
				]), Array.Empty<AotCoreIrAbiSlotArtifact>(), CreateNativeIntAbiSlot(), EmptyRawArgumentIndices);
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.YieldAwaitable::GetAwaiter()", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    return chaos_async_yield_get_awaiter(chaos_arg_0);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()), CreateNativeIntAbiSlot(), new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.YieldAwaitable+YieldAwaiter::get_IsCompleted()", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    return chaos_async_yield_get_is_completed(chaos_arg_0);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()), CreateNativeIntAbiSlot(), new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.YieldAwaitable+YieldAwaiter::GetResult()", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"void",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    chaos_async_yield_get_result(chaos_arg_0);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()), returnVoidAbi, new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Threading.Tasks.Task<System.Int32>::GetAwaiter()", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    return chaos_async_task_int32_get_awaiter(chaos_arg_0);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()), CreateNativeIntAbiSlot(), new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Threading.Tasks.ValueTask<System.Int32>::GetAwaiter()", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    const auto chaos_task_handle = *chaos_resolve_native_int_slot(chaos_arg_0);",
					"    (void)chaos_require_async_task_int32(chaos_task_handle);",
					"    return chaos_task_handle;",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()), CreateNativeIntAbiSlot(), new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.TaskAwaiter<System.Int32>::get_IsCompleted()", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    return chaos_async_task_int32_awaiter_get_is_completed(chaos_arg_0);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()), CreateNativeIntAbiSlot(), new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.TaskAwaiter<System.Int32>::GetResult()", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::int32_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    return chaos_async_task_int32_awaiter_get_result(chaos_arg_0);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()), CreateInt32AbiSlot(), new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.ValueTaskAwaiter<System.Int32>::GetResult()", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::int32_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    return chaos_async_task_int32_awaiter_get_result(chaos_arg_0);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()), CreateInt32AbiSlot(), new HashSet<int> { 0 });
			return true;
		}
		if (TryParseAsyncTaskBuilderStartStateMachineType(callee, out string stateMachineTypeName))
		{
			AotCoreIrMethodArtifact aotCoreIrMethodArtifact = ResolveRequiredAsyncRuntimeContinuationMethod(callee, stateMachineTypeName);
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), $"extern \"C\" void {GetExternalRuntimeHelperSymbol(callee)}(std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1)\n{{\n    (void)chaos_async_task_int32_builder_get_task(chaos_arg_0);\n    {aotCoreIrMethodArtifact.NativeSymbol}(chaos_arg_1);\n}}", new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
			{
				CreateNativeIntAbiSlot(),
				CreateNativeIntAbiSlot()
			}), new AotCoreIrAbiSlotArtifact
			{
				CarrierKindCode = AotCoreIrAbiCarrierKind.Void,
				TypeShape = (AotCoreIrTypeShapeKind)0
			}, new HashSet<int> { 0, 1 });
			return true;
		}
		if (TryParseAsyncValueTaskBuilderStartStateMachineType(callee, out string stateMachineTypeName4))
		{
			AotCoreIrMethodArtifact aotCoreIrMethodArtifact4 = ResolveRequiredAsyncRuntimeContinuationMethod(callee, stateMachineTypeName4);
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), $"extern \"C\" void {GetExternalRuntimeHelperSymbol(callee)}(std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1)\n{{\n    (void)chaos_async_task_int32_builder_get_task(chaos_arg_0);\n    {aotCoreIrMethodArtifact4.NativeSymbol}(chaos_arg_1);\n}}", new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
			{
				CreateNativeIntAbiSlot(),
				CreateNativeIntAbiSlot()
			}), new AotCoreIrAbiSlotArtifact
			{
				CarrierKindCode = AotCoreIrAbiCarrierKind.Void,
				TypeShape = (AotCoreIrTypeShapeKind)0
			}, new HashSet<int> { 0, 1 });
			return true;
		}
		if (TryParseAsyncTaskBuilderAwaitUnsafeOnCompleted(callee, out string awaiterTypeName, out string stateMachineTypeName2))
		{
			AotCoreIrMethodArtifact aotCoreIrMethodArtifact2 = ResolveRequiredAsyncRuntimeContinuationMethod(callee, stateMachineTypeName2);
			string source;
			if (string.Equals(awaiterTypeName, "System.Runtime.CompilerServices.YieldAwaitable+YieldAwaiter", StringComparison.Ordinal))
			{
				source = $"extern \"C\" void {GetExternalRuntimeHelperSymbol(callee)}(std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1, std::intptr_t chaos_arg_2)\n{{\n    (void)chaos_arg_0;\n    auto* chaos_awaiter_slot = chaos_resolve_native_int_slot(chaos_arg_1);\n    *chaos_awaiter_slot = static_cast<std::intptr_t>(2);\n    {aotCoreIrMethodArtifact2.NativeSymbol}(chaos_arg_2);\n}}";
			}
			else
			{
				if (!string.Equals(awaiterTypeName, "System.Runtime.CompilerServices.TaskAwaiter<System.Int32>", StringComparison.Ordinal))
				{
					return false;
				}
				source = $"extern \"C\" void {GetExternalRuntimeHelperSymbol(callee)}(std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1, std::intptr_t chaos_arg_2)\n{{\n    (void)chaos_arg_0;\n    auto* chaos_task = chaos_require_async_task_int32(*chaos_resolve_native_int_slot(chaos_arg_1));\n    if (!chaos_task->completed)\n    {{\n        std::abort();\n    }}\n\n    {aotCoreIrMethodArtifact2.NativeSymbol}(chaos_arg_2);\n}}";
			}
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), source, new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
			{
				CreateNativeIntAbiSlot(),
				CreateNativeIntAbiSlot(),
				CreateNativeIntAbiSlot()
			}), new AotCoreIrAbiSlotArtifact
			{
				CarrierKindCode = AotCoreIrAbiCarrierKind.Void,
				TypeShape = (AotCoreIrTypeShapeKind)0
			}, new HashSet<int> { 0, 1, 2 });
			return true;
		}
		if (TryParseAsyncValueTaskBuilderAwaitUnsafeOnCompleted(callee, out string awaiterTypeName2, out string stateMachineTypeName3))
		{
			AotCoreIrMethodArtifact aotCoreIrMethodArtifact3 = ResolveRequiredAsyncRuntimeContinuationMethod(callee, stateMachineTypeName3);
			string source2;
			if (string.Equals(awaiterTypeName2, "System.Runtime.CompilerServices.YieldAwaitable+YieldAwaiter", StringComparison.Ordinal))
			{
				source2 = $"extern \"C\" void {GetExternalRuntimeHelperSymbol(callee)}(std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1, std::intptr_t chaos_arg_2)\n{{\n    (void)chaos_arg_0;\n    auto* chaos_awaiter_slot = chaos_resolve_native_int_slot(chaos_arg_1);\n    *chaos_awaiter_slot = static_cast<std::intptr_t>(2);\n    {aotCoreIrMethodArtifact3.NativeSymbol}(chaos_arg_2);\n}}";
			}
			else
			{
				if (!string.Equals(awaiterTypeName2, "System.Runtime.CompilerServices.TaskAwaiter<System.Int32>", StringComparison.Ordinal))
				{
					return false;
				}
				source2 = $"extern \"C\" void {GetExternalRuntimeHelperSymbol(callee)}(std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1, std::intptr_t chaos_arg_2)\n{{\n    (void)chaos_arg_0;\n    auto* chaos_task = chaos_require_async_task_int32(*chaos_resolve_native_int_slot(chaos_arg_1));\n    if (!chaos_task->completed)\n    {{\n        std::abort();\n    }}\n\n    {aotCoreIrMethodArtifact3.NativeSymbol}(chaos_arg_2);\n}}";
			}
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), source2, new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
			{
				CreateNativeIntAbiSlot(),
				CreateNativeIntAbiSlot(),
				CreateNativeIntAbiSlot()
			}), new AotCoreIrAbiSlotArtifact
			{
				CarrierKindCode = AotCoreIrAbiCarrierKind.Void,
				TypeShape = (AotCoreIrTypeShapeKind)0
			}, new HashSet<int> { 0, 1, 2 });
			return true;
		}
		return false;
	}

	private bool TryCreateReflectionRuntimeHelperDefinition(string callee, out ExternalRuntimeHelperDefinition? helperDefinition)
	{
		helperDefinition = null;
		if (string.Equals(callee, "System.Private.CoreLib/System.Type::GetTypeFromHandle(System.RuntimeTypeHandle)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    return chaos_reflection_get_type_from_handle(chaos_arg_0);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Type::get_TypeHandle()", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    return chaos_reflection_get_type_handle(chaos_arg_0);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType)), CreateNativeIntAbiSlot(), new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Type::GetField(System.String)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1",
				[
					"    return chaos_reflection_get_field(chaos_arg_0, chaos_arg_1);",
				]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
			{
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)
			}), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.FieldInfo", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0, 1 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Type::GetMethod(System.String)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1",
				[
					"    return chaos_reflection_get_method(chaos_arg_0, chaos_arg_1, 0);",
				]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
			{
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)
			}), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.MethodInfo", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0, 1 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Type::get_Assembly()", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    return chaos_reflection_get_assembly(chaos_arg_0);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType)), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Type::GetGenericTypeDefinition()", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    return chaos_reflection_get_generic_type_definition(chaos_arg_0);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType)), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Type::GetGenericArguments()", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    return chaos_reflection_get_generic_arguments(chaos_arg_0);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType)), CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Reflection.Assembly::GetType(System.String)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1",
				[
					"    return chaos_reflection_get_type_from_assembly(chaos_arg_0, chaos_arg_1);",
				]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
			{
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType),
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)
			}), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0, 1 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Type::GetType(System.String)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    return chaos_reflection_get_type_by_name(chaos_arg_0);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Reflection.Assembly::GetName()", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    return chaos_reflection_get_assembly_name(chaos_arg_0);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType)), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.AssemblyName", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Reflection.AssemblyName::get_Name()", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    return chaos_reflection_get_assembly_name_value(chaos_arg_0);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.AssemblyName", AotCoreIrTypeShapeKind.ReferenceType)), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Type::GetConstructors(System.Reflection.BindingFlags)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0, std::int32_t chaos_arg_1",
				[
					"    return chaos_reflection_get_constructors(chaos_arg_0, chaos_arg_1);",
				]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
			{
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
				CreateInt32AbiSlot()
			}), CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Type::GetMethod(System.String,System.Reflection.BindingFlags)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1, std::int32_t chaos_arg_2",
				[
					"    return chaos_reflection_get_method(chaos_arg_0, chaos_arg_1, chaos_arg_2);",
				]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
			{
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
				new AotCoreIrAbiSlotArtifact
				{
					CarrierKindCode = AotCoreIrAbiCarrierKind.Int32,
					TypeShape = AotCoreIrTypeShapeKind.ValueType
				}
			}), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.MethodInfo", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0, 1 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Reflection.MethodBase::get_MethodHandle()", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    return chaos_reflection_get_method_handle(chaos_arg_0);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.MethodInfo", AotCoreIrTypeShapeKind.ReferenceType)), CreateNativeIntAbiSlot(), new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Reflection.MethodBase::GetParameters()", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    return chaos_reflection_get_parameters(chaos_arg_0);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)), CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Reflection.MethodBase::Invoke(System.Object,System.Object[])", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1, std::intptr_t chaos_arg_2",
				[
					"    return chaos_reflection_invoke_method(chaos_arg_0, chaos_arg_1, chaos_arg_2);",
				]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
			{
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.MethodInfo", AotCoreIrTypeShapeKind.ReferenceType),
				CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
				CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)
			}), CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0, 1, 2 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Reflection.MethodInfo::MakeGenericMethod(System.Type[])", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0, std::intptr_t chaos_arg_1",
				[
					"    return chaos_reflection_make_generic_method(chaos_arg_0, chaos_arg_1);",
				]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
			{
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.MethodInfo", AotCoreIrTypeShapeKind.ReferenceType),
				CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)
			}), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.MethodInfo", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0, 1 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Reflection.MemberInfo::get_Name()", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    return chaos_reflection_get_member_name(chaos_arg_0);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Reflection.MemberInfo::get_DeclaringType()", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    return chaos_reflection_get_declaring_type(chaos_arg_0);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Reflection.MemberInfo::get_MetadataToken()", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::int32_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    return chaos_reflection_get_metadata_token(chaos_arg_0);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)), CreateInt32AbiSlot(), new HashSet<int> { 0 });
			return true;
		}
		if (string.Equals(callee, "System.Private.CoreLib/System.Reflection.ParameterInfo::get_Name()", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"std::intptr_t",
				GetExternalRuntimeHelperSymbol(callee),
				"std::intptr_t chaos_arg_0",
				[
					"    return chaos_reflection_get_parameter_name(chaos_arg_0);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.ParameterInfo", AotCoreIrTypeShapeKind.ReferenceType)), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0 });
			return true;
		}
		return false;
	}

}
