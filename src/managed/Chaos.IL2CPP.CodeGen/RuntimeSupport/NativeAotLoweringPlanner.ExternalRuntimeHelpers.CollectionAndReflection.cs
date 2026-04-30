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
		if (TryReadSingleGenericTypeArgument(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::CreateSpan<", out string typeSubjectId) && IsInt32ElementType(typeSubjectId))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"CHAOS_IL2CPP_INTPTR",
				GetExternalRuntimeHelperSymbol(callee),
				"CHAOS_IL2CPP_INTPTR chaos_arg_0",
				[
					"    return chaos_create_field_data_span_int32(chaos_arg_0);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()), CreateNativeIntAbiSlot(), new HashSet<int> { 0 });
			return true;
		}
		if (TryReadSingleGenericTypeArgument(callee, "System.Memory/System.MemoryExtensions::AsSpan<", out string typeSubjectId2) && IsInt32ElementType(typeSubjectId2) && callee.Contains("(System.Int32[],System.Int32,System.Int32)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"CHAOS_IL2CPP_INTPTR",
				GetExternalRuntimeHelperSymbol(callee),
				"CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1, CHAOS_IL2CPP_INT32 chaos_arg_2",
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
				"CHAOS_IL2CPP_INTPTR",
				GetExternalRuntimeHelperSymbol(callee),
				"CHAOS_IL2CPP_INTPTR chaos_arg_0",
				[
					"    return chaos_create_memory_int32(chaos_arg_0);",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)), CreateNativeIntAbiSlot(), new HashSet<int> { 0 });
			return true;
		}
		if (TryReadSingleGenericTypeArgument(callee, "System.Memory/System.MemoryExtensions::AsMemory<", out string typeSubjectId4) && IsInt32ElementType(typeSubjectId4) && callee.Contains("(System.Int32[],System.Int32,System.Int32)", StringComparison.Ordinal))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"CHAOS_IL2CPP_INTPTR",
				GetExternalRuntimeHelperSymbol(callee),
				"CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1, CHAOS_IL2CPP_INT32 chaos_arg_2",
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
						"CHAOS_IL2CPP_INTPTR",
						GetExternalRuntimeHelperSymbol(callee),
						"CHAOS_IL2CPP_INTPTR chaos_arg_0",
						[
							"    if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))",
							"    {",
							"        return chaos_create_raw_span_int32(nullptr, 0, false);",
							"    }",
							string.Empty,
							"    auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_arg_0);",
							"    return chaos_create_array_span_int32(chaos_arg_0, 0, static_cast<CHAOS_IL2CPP_INT32>(chaos_array->length));",
						]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)), CreateNativeIntAbiSlot(), new HashSet<int> { 0 });
					return true;
				}
				if (string.Equals(methodName, "get_Item", StringComparison.Ordinal))
				{
					helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
						"CHAOS_IL2CPP_INTPTR",
						GetExternalRuntimeHelperSymbol(callee),
						"CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1",
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
						"CHAOS_IL2CPP_INT32",
						GetExternalRuntimeHelperSymbol(callee),
						"CHAOS_IL2CPP_INTPTR chaos_arg_0",
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
						"CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INT32 chaos_arg_2",
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
						"CHAOS_IL2CPP_INTPTR",
						GetExternalRuntimeHelperSymbol(callee),
						"CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1",
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
						"CHAOS_IL2CPP_INT32",
						GetExternalRuntimeHelperSymbol(callee),
						"CHAOS_IL2CPP_INTPTR chaos_arg_0",
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
					"CHAOS_IL2CPP_INTPTR",
					GetExternalRuntimeHelperSymbol(callee),
					"CHAOS_IL2CPP_INTPTR chaos_arg_0",
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
			string value3 = $"    if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))\n    {{\n        CHAOS_IL2CPP_ABORT();\n    }}\n\n    auto* chaos_list = reinterpret_cast<{GetNativeTypeSymbol(methodDeclaringTypeSubjectId)}*>(chaos_arg_0);\n    if (chaos_list->header.type_id != {GetNativeTypeIdSymbol(methodDeclaringTypeSubjectId)})\n    {{\n        CHAOS_IL2CPP_ABORT();\n    }}";
			if (string.Equals(methodName, "Add", StringComparison.Ordinal))
			{
				helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), $"extern \"C\" void {GetExternalRuntimeHelperSymbol(callee)}(CHAOS_IL2CPP_INTPTR chaos_arg_0, {value} chaos_arg_1)\n{{\n{value3}\n    auto* chaos_storage = chaos_require_list_runtime_storage<{value}>(chaos_arg_0);\n    chaos_storage->items.push_back(chaos_arg_1);\n}}", new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
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
				helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), $"extern \"C\" CHAOS_IL2CPP_INT32 {GetExternalRuntimeHelperSymbol(callee)}(CHAOS_IL2CPP_INTPTR chaos_arg_0)\n{{\n{value3}\n    auto* chaos_storage = chaos_require_list_runtime_storage<{value}>(chaos_arg_0);\n    return static_cast<CHAOS_IL2CPP_INT32>(chaos_storage->items.size());\n}}", new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot(methodDeclaringTypeSubjectId, AotCoreIrTypeShapeKind.ReferenceType)), CreateInt32AbiSlot(), new HashSet<int> { 0 });
				return true;
			}
			if (string.Equals(methodName, "get_Item", StringComparison.Ordinal))
			{
				helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), $"extern \"C\" {value2} {GetExternalRuntimeHelperSymbol(callee)}(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1)\n{{\n{value3}\n    auto* chaos_storage = chaos_require_list_runtime_storage<{value}>(chaos_arg_0);\n    if (chaos_arg_1 < 0 ||\n        static_cast<CHAOS_IL2CPP_SIZE>(chaos_arg_1) >= chaos_storage->items.size())\n    {{\n        CHAOS_IL2CPP_ABORT();\n    }}\n\n    return chaos_storage->items[static_cast<CHAOS_IL2CPP_SIZE>(chaos_arg_1)];\n}}", new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
				{
					CreateNativeIntAbiSlot(methodDeclaringTypeSubjectId, AotCoreIrTypeShapeKind.ReferenceType),
					CreateInt32AbiSlot()
				}), abiSlot, new HashSet<int> { 0 });
				return true;
			}
		}
		if (TryParseClosedDictionaryStringIntType(methodDeclaringTypeSubjectId))
		{
			string value4 = $"    if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))\n    {{\n        CHAOS_IL2CPP_ABORT();\n    }}\n\n    auto* chaos_dictionary = reinterpret_cast<{GetNativeTypeSymbol(methodDeclaringTypeSubjectId)}*>(chaos_arg_0);\n    if (chaos_dictionary->header.type_id != {GetNativeTypeIdSymbol(methodDeclaringTypeSubjectId)})\n    {{\n        CHAOS_IL2CPP_ABORT();\n    }}";
			if (string.Equals(methodName, "set_Item", StringComparison.Ordinal))
			{
				helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), $"extern \"C\" void {GetExternalRuntimeHelperSymbol(callee)}(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INT32 chaos_arg_2)\n{{\n{value4}\n    auto* chaos_storage = chaos_require_dictionary_runtime_storage<CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32>(chaos_arg_0);\n    for (auto& chaos_entry : chaos_storage->entries)\n    {{\n        if (chaos_object_equals(chaos_entry.first, chaos_arg_1))\n        {{\n            chaos_entry.second = chaos_arg_2;\n            return;\n        }}\n    }}\n\n    chaos_storage->entries.emplace_back(chaos_arg_1, chaos_arg_2);\n}}", new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
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
				helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), $"extern \"C\" CHAOS_IL2CPP_INT32 {GetExternalRuntimeHelperSymbol(callee)}(CHAOS_IL2CPP_INTPTR chaos_arg_0)\n{{\n{value4}\n    auto* chaos_storage = chaos_require_dictionary_runtime_storage<CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32>(chaos_arg_0);\n    return static_cast<CHAOS_IL2CPP_INT32>(chaos_storage->entries.size());\n}}", new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot(methodDeclaringTypeSubjectId, AotCoreIrTypeShapeKind.ReferenceType)), CreateInt32AbiSlot(), new HashSet<int> { 0 });
				return true;
			}
			if (string.Equals(methodName, "get_Item", StringComparison.Ordinal))
			{
				helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), $"extern \"C\" CHAOS_IL2CPP_INT32 {GetExternalRuntimeHelperSymbol(callee)}(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)\n{{\n{value4}\n    auto* chaos_storage = chaos_require_dictionary_runtime_storage<CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32>(chaos_arg_0);\n    for (const auto& chaos_entry : chaos_storage->entries)\n    {{\n        if (chaos_object_equals(chaos_entry.first, chaos_arg_1))\n        {{\n            return chaos_entry.second;\n        }}\n    }}\n\n    CHAOS_IL2CPP_ABORT();\n}}", new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
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
			StringBuilder stringBuilder = new StringBuilder(2048);
			stringBuilder2 = stringBuilder;
			StringBuilder stringBuilder3 = stringBuilder2;
			handler = new StringBuilder.AppendInterpolatedStringHandler(51, 1, stringBuilder2);
			handler.AppendLiteral("extern \"C\" CHAOS_IL2CPP_INT32 ");
			handler.AppendFormatted(GetExternalRuntimeHelperSymbol(callee));
			handler.AppendLiteral("(CHAOS_IL2CPP_INTPTR chaos_arg_0)");
			stringBuilder3.AppendLine(ref handler);
			stringBuilder.AppendLine("{");
			stringBuilder.AppendLine("    if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
			stringBuilder.AppendLine("    {");
			stringBuilder.AppendLine("        CHAOS_IL2CPP_ABORT();");
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
				stringBuilder.AppendLine("        return static_cast<CHAOS_IL2CPP_INT32>(chaos_array->length);");
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
				stringBuilder.AppendLine("        return static_cast<CHAOS_IL2CPP_INT32>(chaos_storage->items.size());");
				stringBuilder.AppendLine("    }");
				stringBuilder.AppendLine();
			}
			stringBuilder.AppendLine("    CHAOS_IL2CPP_ABORT();");
			stringBuilder.AppendLine("}");
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), stringBuilder.ToString().TrimEnd(), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot(declaringTypeSubjectId, AotCoreIrTypeShapeKind.ReferenceType)), CreateInt32AbiSlot(), new HashSet<int> { 0 });
			return true;
		}
		if (!string.Equals(methodName, "get_Item", StringComparison.Ordinal) || !TryCreateCollectionArrayElementExpression(abiSlot, "chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_arg_1)]", out string elementExpression))
		{
			return false;
		}
		StringBuilder stringBuilder8 = new StringBuilder(1024);
		stringBuilder2 = stringBuilder8;
		StringBuilder stringBuilder9 = stringBuilder2;
		handler = new StringBuilder.AppendInterpolatedStringHandler(65, 2, stringBuilder2);
		handler.AppendLiteral("extern \"C\" ");
		handler.AppendFormatted(value2);
		handler.AppendLiteral(" ");
		handler.AppendFormatted(GetExternalRuntimeHelperSymbol(callee));
		handler.AppendLiteral("(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1)");
		stringBuilder9.AppendLine(ref handler);
		stringBuilder8.AppendLine("{");
		stringBuilder8.AppendLine("    if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
		stringBuilder8.AppendLine("    {");
		stringBuilder8.AppendLine("        CHAOS_IL2CPP_ABORT();");
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
			stringBuilder8.AppendLine("        if (chaos_arg_1 < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_arg_1) >= chaos_array->length)");
			stringBuilder8.AppendLine("        {");
			stringBuilder8.AppendLine("            CHAOS_IL2CPP_ABORT();");
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
			stringBuilder8.AppendLine("        if (chaos_arg_1 < 0 || static_cast<CHAOS_IL2CPP_SIZE>(chaos_arg_1) >= chaos_storage->items.size())");
			stringBuilder8.AppendLine("        {");
			stringBuilder8.AppendLine("            CHAOS_IL2CPP_ABORT();");
			stringBuilder8.AppendLine("        }");
			stringBuilder8.AppendLine();
			stringBuilder8.AppendLine("        return chaos_storage->items[static_cast<CHAOS_IL2CPP_SIZE>(chaos_arg_1)];");
			stringBuilder8.AppendLine("    }");
			stringBuilder8.AppendLine();
		}
		stringBuilder8.AppendLine("    CHAOS_IL2CPP_ABORT();");
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
		if (_customAttributeSupport.UsesMemberInfoIsDefined && MatchesMethodSubject(callee, "System.Private.CoreLib/System.Reflection.MemberInfo", "IsDefined", "System.Type", "System.Boolean"))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"CHAOS_IL2CPP_INT32",
				GetExternalRuntimeHelperSymbol(callee),
				"CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INT32 chaos_arg_2",
				[
					"    (void)chaos_arg_2;",
					"    if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0) || chaos_arg_1 == static_cast<CHAOS_IL2CPP_INTPTR>(0))",
					"    {",
					"        return 0;",
					"    }",
					string.Empty,
					$"    auto* chaos_type = reinterpret_cast<{GetNativeTypeSymbol("System.Private.CoreLib/System.Type")}*>(chaos_arg_1);",
					"    if (chaos_type->runtime_type_handle == static_cast<CHAOS_IL2CPP_INTPTR>(0))",
					"    {",
					"        return 0;",
					"    }",
					string.Empty,
					"    return chaos_reflection_get_custom_attribute(chaos_arg_0, chaos_type->runtime_type_handle) == static_cast<CHAOS_IL2CPP_INTPTR>(0)",
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
				"CHAOS_IL2CPP_INTPTR",
				GetExternalRuntimeHelperSymbol(callee),
				"CHAOS_IL2CPP_INTPTR chaos_arg_0",
				[
					$"    return chaos_reflection_get_custom_attribute(chaos_arg_0, {GetTypeHandleLiteral(value)});",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)), CreateNativeIntAbiSlot(value, AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0 });
			return true;
		}
		if (_customAttributeSupport.SyntheticGetterFieldByMethodSubjectId.TryGetValue(callee, out string value2))
		{
			string declaringTypeSubjectId = GetDeclaringTypeSubjectId(value2);
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
		if (MatchesMethodSubject(callee, "System.Threading.Thread/System.Threading.Thread", "get_Name"))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"CHAOS_IL2CPP_INTPTR",
				GetExternalRuntimeHelperSymbol(callee),
				"CHAOS_IL2CPP_INTPTR chaos_arg_0",
				[
					"    auto& chaos_thread_entry = chaos_require_thread_runtime_entry(chaos_arg_0);",
					"    CHAOS_IL2CPP_LOCK_GUARD<CHAOS_IL2CPP_MUTEX> chaos_guard(chaos_thread_entry.mutex);",
					"    return chaos_thread_entry.name;",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot("System.Threading.Thread/System.Threading.Thread", AotCoreIrTypeShapeKind.ReferenceType)), CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType), new HashSet<int> { 0 });
			return true;
		}
		if (MatchesMethodSubject(callee, "System.Threading.Thread/System.Threading.Thread", "set_Name", "System.String"))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"void",
				GetExternalRuntimeHelperSymbol(callee),
				"CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
				[
					"    auto& chaos_thread_entry = chaos_require_thread_runtime_entry(chaos_arg_0);",
					"    CHAOS_IL2CPP_LOCK_GUARD<CHAOS_IL2CPP_MUTEX> chaos_guard(chaos_thread_entry.mutex);",
					"    chaos_thread_entry.name = chaos_arg_1;",
				]), new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
			{
				CreateNativeIntAbiSlot("System.Threading.Thread/System.Threading.Thread", AotCoreIrTypeShapeKind.ReferenceType),
				CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)
			}), returnVoidAbi, new HashSet<int> { 0, 1 });
			return true;
		}
		if (MatchesMethodSubject(callee, "System.GC", "Collect") || MatchesMethodSubject(callee, "System.GC", "WaitForPendingFinalizers"))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"void",
				GetExternalRuntimeHelperSymbol(callee),
				string.Empty,
				[
					"    CHAOS_IL2CPP_ABORT();",
				]), Array.Empty<AotCoreIrAbiSlotArtifact>(), returnVoidAbi, EmptyRawArgumentIndices);
			return true;
		}
		if (MatchesMethodSubject(callee, "System.GC", "KeepAlive", "System.Object"))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(
				"void",
				GetExternalRuntimeHelperSymbol(callee),
				"CHAOS_IL2CPP_INTPTR chaos_arg_0",
				[
					"    (void)chaos_arg_0;",
				]), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)), returnVoidAbi, new HashSet<int> { 0 });
			return true;
		}
		if (MatchesMethodSubject(callee, "System.Threading/Monitor", "TryEnter", "System.Object", "System.TimeSpan", "System.Boolean&"))
		{
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), $"extern \"C\" void {GetExternalRuntimeHelperSymbol(callee)}(CHAOS_IL2CPP_INTPTR chaos_arg_0, {GetNativeValueTypeSymbol("System.Private.CoreLib/System.TimeSpan")} chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2)\n{{\n    auto& chaos_monitor_entry = chaos_require_monitor_runtime_entry(chaos_arg_0);\n    auto* chaos_lock_taken_slot = chaos_resolve_native_int_slot(chaos_arg_2);\n    *chaos_lock_taken_slot = static_cast<CHAOS_IL2CPP_INTPTR>(0);\n\n    CHAOS_IL2CPP_INT64 chaos_ticks = 0;\n    CHAOS_IL2CPP_MEMCPY(&chaos_ticks, &chaos_arg_1, sizeof(chaos_ticks));\n    if (chaos_ticks < 0)\n    {{\n        chaos_ticks = 0;\n    }}\n\n    const auto chaos_timeout = CHAOS_IL2CPP_CHRONO_DURATION_CAST(CHAOS_IL2CPP_CHRONO_STEADY_CLOCK::duration>(\n        CHAOS_IL2CPP_CHRONO_NANOSECONDS(chaos_ticks * 100));\n    if (chaos_monitor_entry.mutex.try_lock_for(chaos_timeout))\n    {{\n        *chaos_lock_taken_slot = static_cast<CHAOS_IL2CPP_INTPTR>(1);\n    }}\n}}", new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
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
		if (MatchesMethodSubject(callee, "System.IDisposable", "Dispose"))
		{
			IReadOnlyList<AotCoreIrMethodArtifact> readOnlyList = ResolveInterfaceDispatchTargets(callee);
			if (readOnlyList.Count == 0)
			{
				return false;
			}
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), BuildVoidInterfaceDispatchHelperSource(GetExternalRuntimeHelperSymbol(callee), readOnlyList), new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot("System.Private.CoreLib/System.IDisposable", AotCoreIrTypeShapeKind.ReferenceType)), returnVoidAbi, new HashSet<int> { 0 });
			return true;
		}
		return false;
	}

	private IReadOnlyList<AotCoreIrMethodArtifact> ResolveInterfaceDispatchTargets(string interfaceMethodSubjectId)
	{
		string interfaceTypeSubjectId = GetMethodDeclaringTypeSubjectId(interfaceMethodSubjectId);
		string slotSignatureSuffix = GetMethodSignatureSuffix(interfaceMethodSubjectId);
		return _methodsBySubjectId.Values.Where(delegate(AotCoreIrMethodArtifact method)
		{
			if (method.IsStatic || !CanEmitMethodBody(method) || !string.Equals(GetMethodSignatureSuffix(method.SubjectId), slotSignatureSuffix, StringComparison.Ordinal))
			{
				return false;
			}
			string declaringTypeSubjectId = method.Identity.DeclaringTypeSubjectId;
			return !string.IsNullOrEmpty(declaringTypeSubjectId) && ImplementsInterface(declaringTypeSubjectId, interfaceTypeSubjectId, interfaceTypeSubjectId);
		}).OrderBy((AotCoreIrMethodArtifact method) => method.SubjectId, StringComparer.Ordinal).ToArray();
	}

	private string BuildVoidInterfaceDispatchHelperSource(string helperSymbol, IReadOnlyList<AotCoreIrMethodArtifact> dispatchTargets)
	{
		StringBuilder stringBuilder = new StringBuilder(2048);
		stringBuilder.Append("extern \"C\" void ");
		stringBuilder.Append(helperSymbol);
		stringBuilder.AppendLine("(CHAOS_IL2CPP_INTPTR chaos_arg_0)");
		stringBuilder.AppendLine("{");
		stringBuilder.AppendLine("    if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
		stringBuilder.AppendLine("    {");
		stringBuilder.AppendLine("        CHAOS_IL2CPP_ABORT();");
		stringBuilder.AppendLine("    }");
		stringBuilder.AppendLine("    auto* chaos_header = reinterpret_cast<chaos_object_header*>(chaos_arg_0);");
		stringBuilder.AppendLine("    switch (chaos_header->type_id)");
		stringBuilder.AppendLine("    {");
		foreach (AotCoreIrMethodArtifact dispatchTarget in dispatchTargets)
		{
			stringBuilder.Append("        case ");
			stringBuilder.Append(GetVirtualDispatchTargetTypeIdSymbol(dispatchTarget.Identity.DeclaringTypeSubjectId));
			stringBuilder.AppendLine(":");
			stringBuilder.Append("            ");
			stringBuilder.Append(dispatchTarget.NativeSymbol);
			stringBuilder.Append("(");
			stringBuilder.Append(FormatAbiInvocationArgumentList(GetMethodAbiParameterSlots(dispatchTarget), GetVirtualDispatchInstanceExpression(dispatchTarget.Identity.DeclaringTypeSubjectId, "chaos_arg_0")));
			stringBuilder.AppendLine(");");
			stringBuilder.AppendLine("            return;");
		}
		stringBuilder.AppendLine("        default:");
		stringBuilder.AppendLine("            break;");
		stringBuilder.AppendLine("    }");
		stringBuilder.AppendLine();
		stringBuilder.AppendLine("    auto chaos_current_type_id = chaos_get_base_type_id(chaos_header->type_id);");
		stringBuilder.AppendLine("    while (chaos_current_type_id != static_cast<CHAOS_IL2CPP_INTPTR>(0))");
		stringBuilder.AppendLine("    {");
		stringBuilder.AppendLine("        switch (chaos_current_type_id)");
		stringBuilder.AppendLine("        {");
		foreach (AotCoreIrMethodArtifact dispatchTarget2 in dispatchTargets)
		{
			stringBuilder.Append("            case ");
			stringBuilder.Append(GetVirtualDispatchTargetTypeIdSymbol(dispatchTarget2.Identity.DeclaringTypeSubjectId));
			stringBuilder.AppendLine(":");
			stringBuilder.Append("                ");
			stringBuilder.Append(dispatchTarget2.NativeSymbol);
			stringBuilder.Append("(");
			stringBuilder.Append(FormatAbiInvocationArgumentList(GetMethodAbiParameterSlots(dispatchTarget2), GetVirtualDispatchInstanceExpression(dispatchTarget2.Identity.DeclaringTypeSubjectId, "chaos_arg_0")));
			stringBuilder.AppendLine(");");
			stringBuilder.AppendLine("                return;");
		}
		stringBuilder.AppendLine("            default:");
		stringBuilder.AppendLine("                break;");
		stringBuilder.AppendLine("        }");
		stringBuilder.AppendLine();
		stringBuilder.AppendLine("        chaos_current_type_id = chaos_get_base_type_id(chaos_current_type_id);");
		stringBuilder.AppendLine("    }");
		stringBuilder.AppendLine();
		stringBuilder.AppendLine("    CHAOS_IL2CPP_ABORT();");
		stringBuilder.AppendLine("}");
		return stringBuilder.ToString().TrimEnd();
	}

	private bool TryCreateAsyncRuntimeHelperDefinition(string callee, out ExternalRuntimeHelperDefinition? helperDefinition)
	{
		if (TryCreateAsyncBuilderCreateHelperDefinition(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncTaskMethodBuilder", out helperDefinition) || TryCreateAsyncBuilderCreateHelperDefinition(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncValueTaskMethodBuilder", out helperDefinition) || TryCreateAsyncBuilderGetTaskHelperDefinition(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncTaskMethodBuilder", out helperDefinition) || TryCreateAsyncBuilderGetTaskHelperDefinition(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncValueTaskMethodBuilder", out helperDefinition) || TryCreateAsyncBuilderSetResultHelperDefinition(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncTaskMethodBuilder", out helperDefinition) || TryCreateAsyncBuilderSetResultHelperDefinition(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncValueTaskMethodBuilder", out helperDefinition) || TryCreateAsyncBuilderSetExceptionHelperDefinition(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncTaskMethodBuilder", out helperDefinition) || TryCreateAsyncBuilderSetExceptionHelperDefinition(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncValueTaskMethodBuilder", out helperDefinition))
		{
			return true;
		}
		if (TryCreateAsyncTaskGetAwaiterHelperDefinition(callee, "System.Private.CoreLib/System.Threading.Tasks.Task", isValueTask: false, out helperDefinition) || TryCreateAsyncTaskGetAwaiterHelperDefinition(callee, "System.Private.CoreLib/System.Threading.Tasks.ValueTask", isValueTask: true, out helperDefinition) || TryCreateAsyncAwaiterIsCompletedHelperDefinition(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.TaskAwaiter", out helperDefinition) || TryCreateAsyncAwaiterIsCompletedHelperDefinition(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.ValueTaskAwaiter", out helperDefinition) || TryCreateAsyncAwaiterGetResultHelperDefinition(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.TaskAwaiter", out helperDefinition) || TryCreateAsyncAwaiterGetResultHelperDefinition(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.ValueTaskAwaiter", out helperDefinition) || TryCreateAsyncBuilderStartHelperDefinition(callee, isValueTaskBuilder: false, out helperDefinition) || TryCreateAsyncBuilderStartHelperDefinition(callee, isValueTaskBuilder: true, out helperDefinition) || TryCreateAsyncBuilderAwaitUnsafeOnCompletedHelperDefinition(callee, isValueTaskBuilder: false, out helperDefinition) || TryCreateAsyncBuilderAwaitUnsafeOnCompletedHelperDefinition(callee, isValueTaskBuilder: true, out helperDefinition))
		{
			return true;
		}
		helperDefinition = null;
		return false;
	}

	private sealed record AsyncRuntimeResultShape(string ManagedResultTypeName, AotCoreIrAbiSlotArtifact ResultAbi)
	{
		public string ResultCppType => MapAbiSlotReturnType(ResultAbi);

		public string ConvertResultToRawExpression(string sourceName)
		{
			return ResultAbi.CarrierKindCode switch
			{
				AotCoreIrAbiCarrierKind.Int32 => $"static_cast<CHAOS_IL2CPP_INTPTR>({sourceName})",
				AotCoreIrAbiCarrierKind.Int8 => $"static_cast<CHAOS_IL2CPP_INTPTR>({sourceName})",
				AotCoreIrAbiCarrierKind.UInt8 => $"static_cast<CHAOS_IL2CPP_INTPTR>({sourceName})",
				AotCoreIrAbiCarrierKind.Int16 => $"static_cast<CHAOS_IL2CPP_INTPTR>({sourceName})",
				AotCoreIrAbiCarrierKind.UInt16 => $"static_cast<CHAOS_IL2CPP_INTPTR>({sourceName})",
				AotCoreIrAbiCarrierKind.Float32 => $"chaos_store_float32({sourceName})",
				AotCoreIrAbiCarrierKind.Float64 => $"chaos_store_float64({sourceName})",
				AotCoreIrAbiCarrierKind.Int64 => $"chaos_store_int64({sourceName})",
				AotCoreIrAbiCarrierKind.UInt64 => $"chaos_store_uint64({sourceName})",
				AotCoreIrAbiCarrierKind.NativeInt => sourceName,
				_ => throw new NotSupportedException($"native-aot async runtime helper does not support result carrier '{ResultAbi.CarrierKindCode}'.")
			};
		}

		public string ConvertRawToResultExpression(string sourceName)
		{
			return ResultAbi.CarrierKindCode switch
			{
				AotCoreIrAbiCarrierKind.Int32 => $"static_cast<CHAOS_IL2CPP_INT32>({sourceName})",
				AotCoreIrAbiCarrierKind.Int8 => $"static_cast<CHAOS_IL2CPP_INT8>({sourceName})",
				AotCoreIrAbiCarrierKind.UInt8 => $"static_cast<CHAOS_IL2CPP_UINT8>({sourceName})",
				AotCoreIrAbiCarrierKind.Int16 => $"static_cast<CHAOS_IL2CPP_INT16>({sourceName})",
				AotCoreIrAbiCarrierKind.UInt16 => $"static_cast<CHAOS_IL2CPP_UINT16>({sourceName})",
				AotCoreIrAbiCarrierKind.Float32 => $"chaos_load_float32({sourceName})",
				AotCoreIrAbiCarrierKind.Float64 => $"chaos_load_float64({sourceName})",
				AotCoreIrAbiCarrierKind.Int64 => $"chaos_load_int64({sourceName})",
				AotCoreIrAbiCarrierKind.UInt64 => $"chaos_load_uint64({sourceName})",
				AotCoreIrAbiCarrierKind.NativeInt => sourceName,
				_ => throw new NotSupportedException($"native-aot async runtime helper does not support result carrier '{ResultAbi.CarrierKindCode}'.")
			};
		}
	}

	private static AotCoreIrAbiSlotArtifact CreateVoidAbiSlot()
	{
		return new AotCoreIrAbiSlotArtifact
		{
			CarrierKindCode = AotCoreIrAbiCarrierKind.Void,
			TypeShape = (AotCoreIrTypeShapeKind)0
		};
	}

	private static bool MatchesMethodSubject(string subjectId, string declaringTypeSubjectId, string methodName, params string[] parameterTypes)
	{
		// Strip assembly prefix (e.g. "System.Private.CoreLib/System.String" -> "System.String")
		// to make matching assembly-agnostic. The typeDisplayName is everything after the first '/'.
		var slashIndex = declaringTypeSubjectId.IndexOf('/');
		var typeDisplayName = slashIndex >= 0
			? declaringTypeSubjectId[(slashIndex + 1)..]
			: declaringTypeSubjectId;
		return ManagedNaming.MatchesMethod(subjectId, typeDisplayName, methodName, parameterTypes);
	}

	private static bool MatchesMethodSignature(string subjectId, string methodName, params string[] parameterTypes)
	{
		return !string.IsNullOrEmpty(subjectId) && string.Equals(GetMethodName(subjectId), methodName, StringComparison.Ordinal) && GetMethodParameterTypes(subjectId).SequenceEqual(parameterTypes);
	}

	private bool TryCreateSupportedAsyncRuntimeResultShape(string managedResultTypeName, out AsyncRuntimeResultShape? resultShape)
	{
		resultShape = null;
		if (!TryCreateResolvedTypeAbiSlot(managedResultTypeName, out AotCoreIrAbiSlotArtifact resultAbi))
		{
			return false;
		}
		if (!IsSupportedAsyncRuntimeResultCarrier(resultAbi.CarrierKindCode))
		{
			return false;
		}
		resultShape = new AsyncRuntimeResultShape(managedResultTypeName, resultAbi);
		return true;
	}

	private static bool IsSupportedAsyncRuntimeResultCarrier(AotCoreIrAbiCarrierKind carrierKind)
	{
		return carrierKind switch
		{
			AotCoreIrAbiCarrierKind.Int8 => true,
			AotCoreIrAbiCarrierKind.UInt8 => true,
			AotCoreIrAbiCarrierKind.Int16 => true,
			AotCoreIrAbiCarrierKind.UInt16 => true,
			AotCoreIrAbiCarrierKind.Int32 => true,
			AotCoreIrAbiCarrierKind.Float32 => true,
			AotCoreIrAbiCarrierKind.Float64 => true,
			AotCoreIrAbiCarrierKind.Int64 => true,
			AotCoreIrAbiCarrierKind.UInt64 => true,
			AotCoreIrAbiCarrierKind.NativeInt => true,
			_ => false
		};
	}

	private bool TryCreateAsyncBuilderCreateHelperDefinition(string callee, string openGenericBuilderTypePrefix, out ExternalRuntimeHelperDefinition? helperDefinition)
	{
		helperDefinition = null;
		if (!TryReadSingleGenericTypeArgument(callee, openGenericBuilderTypePrefix + "<", out string resultTypeName) || !MatchesMethodSignature(callee, "Create") || !TryCreateSupportedAsyncRuntimeResultShape(resultTypeName, out _))
		{
			return false;
		}
		helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", GetExternalRuntimeHelperSymbol(callee), string.Empty, [
			"    return chaos_async_task_create();"
		]), Array.Empty<AotCoreIrAbiSlotArtifact>(), CreateNativeIntAbiSlot(), EmptyRawArgumentIndices);
		return true;
	}

	private bool TryCreateAsyncBuilderGetTaskHelperDefinition(string callee, string openGenericBuilderTypePrefix, out ExternalRuntimeHelperDefinition? helperDefinition)
	{
		helperDefinition = null;
		if (!TryReadSingleGenericTypeArgument(callee, openGenericBuilderTypePrefix + "<", out string resultTypeName) || !MatchesMethodSignature(callee, "get_Task") || !TryCreateSupportedAsyncRuntimeResultShape(resultTypeName, out _))
		{
			return false;
		}
		helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", GetExternalRuntimeHelperSymbol(callee), "CHAOS_IL2CPP_INTPTR chaos_arg_0", [
			"    return chaos_async_task_builder_get_task(chaos_arg_0);"
		]), new AotCoreIrAbiSlotArtifact[]
		{
			CreateNativeIntAbiSlot()
		}, CreateNativeIntAbiSlot(), new HashSet<int> { 0 });
		return true;
	}

	private bool TryCreateAsyncBuilderSetResultHelperDefinition(string callee, string openGenericBuilderTypePrefix, out ExternalRuntimeHelperDefinition? helperDefinition)
	{
		helperDefinition = null;
		if (!TryReadSingleGenericTypeArgument(callee, openGenericBuilderTypePrefix + "<", out string resultTypeName) || !MatchesMethodSignature(callee, "SetResult", resultTypeName) || !TryCreateSupportedAsyncRuntimeResultShape(resultTypeName, out AsyncRuntimeResultShape? resultShape))
		{
			return false;
		}
		var parameterAbis = new AotCoreIrAbiSlotArtifact[]
		{
			CreateNativeIntAbiSlot(),
			resultShape.ResultAbi
		};
		helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper("void", GetExternalRuntimeHelperSymbol(callee), FormatAbiSlotParameterSignature(parameterAbis), [
			$"    chaos_async_task_builder_set_result_raw(chaos_arg_0, {resultShape.ConvertResultToRawExpression("chaos_arg_1")});"
		]), parameterAbis, CreateVoidAbiSlot(), new HashSet<int> { 0 });
		return true;
	}

	private bool TryCreateAsyncBuilderSetExceptionHelperDefinition(string callee, string openGenericBuilderTypePrefix, out ExternalRuntimeHelperDefinition? helperDefinition)
	{
		helperDefinition = null;
		if (!TryReadSingleGenericTypeArgument(callee, openGenericBuilderTypePrefix + "<", out string resultTypeName) || !MatchesMethodSignature(callee, "SetException", "System.Exception") || !TryCreateSupportedAsyncRuntimeResultShape(resultTypeName, out _))
		{
			return false;
		}
		helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper("void", GetExternalRuntimeHelperSymbol(callee), "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1", [
			"    chaos_async_task_builder_set_exception(chaos_arg_0, chaos_arg_1);"
		]), new AotCoreIrAbiSlotArtifact[]
		{
			CreateNativeIntAbiSlot(),
			CreateNativeIntAbiSlot()
		}, CreateVoidAbiSlot(), new HashSet<int> { 0 });
		return true;
	}

	private bool TryCreateAsyncTaskGetAwaiterHelperDefinition(string callee, string openGenericTaskTypePrefix, bool isValueTask, out ExternalRuntimeHelperDefinition? helperDefinition)
	{
		helperDefinition = null;
		if (!TryReadSingleGenericTypeArgument(callee, openGenericTaskTypePrefix + "<", out string resultTypeName) || !MatchesMethodSignature(callee, "GetAwaiter") || !TryCreateSupportedAsyncRuntimeResultShape(resultTypeName, out _))
		{
			return false;
		}
		string[] bodyLines = isValueTask ? [
			"    const auto chaos_task_handle = *chaos_resolve_native_int_slot(chaos_arg_0);",
			"    (void)chaos_require_async_task(chaos_task_handle);",
			"    return chaos_task_handle;"
		] : [
			"    return chaos_async_task_get_awaiter(chaos_arg_0);"
		];
		helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", GetExternalRuntimeHelperSymbol(callee), "CHAOS_IL2CPP_INTPTR chaos_arg_0", bodyLines), new AotCoreIrAbiSlotArtifact[]
		{
			CreateNativeIntAbiSlot()
		}, CreateNativeIntAbiSlot(), new HashSet<int> { 0 });
		return true;
	}

	private bool TryCreateAsyncAwaiterIsCompletedHelperDefinition(string callee, string openGenericAwaiterTypePrefix, out ExternalRuntimeHelperDefinition? helperDefinition)
	{
		helperDefinition = null;
		if (!TryReadSingleGenericTypeArgument(callee, openGenericAwaiterTypePrefix + "<", out string resultTypeName) || !MatchesMethodSignature(callee, "get_IsCompleted") || !TryCreateSupportedAsyncRuntimeResultShape(resultTypeName, out _))
		{
			return false;
		}
		helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", GetExternalRuntimeHelperSymbol(callee), "CHAOS_IL2CPP_INTPTR chaos_arg_0", [
			"    return chaos_async_task_awaiter_get_is_completed(chaos_arg_0);"
		]), new AotCoreIrAbiSlotArtifact[]
		{
			CreateNativeIntAbiSlot()
		}, CreateNativeIntAbiSlot(), new HashSet<int> { 0 });
		return true;
	}

	private bool TryCreateAsyncAwaiterGetResultHelperDefinition(string callee, string openGenericAwaiterTypePrefix, out ExternalRuntimeHelperDefinition? helperDefinition)
	{
		helperDefinition = null;
		if (!TryReadSingleGenericTypeArgument(callee, openGenericAwaiterTypePrefix + "<", out string resultTypeName) || !MatchesMethodSignature(callee, "GetResult") || !TryCreateSupportedAsyncRuntimeResultShape(resultTypeName, out AsyncRuntimeResultShape? resultShape))
		{
			return false;
		}
		helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), RenderSimpleExternalRuntimeHelper(resultShape.ResultCppType, GetExternalRuntimeHelperSymbol(callee), "CHAOS_IL2CPP_INTPTR chaos_arg_0", [
			$"    return {resultShape.ConvertRawToResultExpression("chaos_async_task_awaiter_get_result_raw(chaos_arg_0)")};"
		]), new AotCoreIrAbiSlotArtifact[]
		{
			CreateNativeIntAbiSlot()
		}, resultShape.ResultAbi, new HashSet<int> { 0 });
		return true;
	}

	private bool TryCreateAsyncBuilderStartHelperDefinition(string callee, bool isValueTaskBuilder, out ExternalRuntimeHelperDefinition? helperDefinition)
	{
		helperDefinition = null;
		bool parsed = isValueTaskBuilder ? TryParseAsyncValueTaskBuilderStartStateMachineType(callee, out string? builderResultTypeName, out string? stateMachineTypeName) : TryParseAsyncTaskBuilderStartStateMachineType(callee, out builderResultTypeName, out stateMachineTypeName);
		if (!parsed || string.IsNullOrEmpty(builderResultTypeName) || string.IsNullOrEmpty(stateMachineTypeName) || !TryCreateSupportedAsyncRuntimeResultShape(builderResultTypeName, out _))
		{
			return false;
		}
		AotCoreIrMethodArtifact continuationMethod = ResolveRequiredAsyncRuntimeContinuationMethod(callee, stateMachineTypeName);
		helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), $"extern \"C\" void {GetExternalRuntimeHelperSymbol(callee)}(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1)\n{{\n    (void)chaos_async_task_builder_get_task(chaos_arg_0);\n    {continuationMethod.NativeSymbol}(chaos_arg_1);\n}}", new AotCoreIrAbiSlotArtifact[]
		{
			CreateNativeIntAbiSlot(),
			CreateNativeIntAbiSlot()
		}, CreateVoidAbiSlot(), new HashSet<int> { 0, 1 });
		return true;
	}

	private bool TryCreateAsyncBuilderAwaitUnsafeOnCompletedHelperDefinition(string callee, bool isValueTaskBuilder, out ExternalRuntimeHelperDefinition? helperDefinition)
	{
		helperDefinition = null;
		bool parsed = isValueTaskBuilder ? TryParseAsyncValueTaskBuilderAwaitUnsafeOnCompleted(callee, out string? builderResultTypeName, out string? awaiterTypeName, out string? stateMachineTypeName) : TryParseAsyncTaskBuilderAwaitUnsafeOnCompleted(callee, out builderResultTypeName, out awaiterTypeName, out stateMachineTypeName);
		if (!parsed || string.IsNullOrEmpty(builderResultTypeName) || string.IsNullOrEmpty(awaiterTypeName) || string.IsNullOrEmpty(stateMachineTypeName) || !TryCreateSupportedAsyncRuntimeResultShape(builderResultTypeName, out _))
		{
			return false;
		}
		AotCoreIrMethodArtifact continuationMethod = ResolveRequiredAsyncRuntimeContinuationMethod(callee, stateMachineTypeName);
		string source;
		if (string.Equals(awaiterTypeName, "System.Runtime.CompilerServices.YieldAwaitable+YieldAwaiter", StringComparison.Ordinal))
		{
			source = $"extern \"C\" void {GetExternalRuntimeHelperSymbol(callee)}(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2)\n{{\n    (void)chaos_arg_0;\n    auto* chaos_awaiter_slot = chaos_resolve_native_int_slot(chaos_arg_1);\n    *chaos_awaiter_slot = static_cast<CHAOS_IL2CPP_INTPTR>(2);\n    {continuationMethod.NativeSymbol}(chaos_arg_2);\n}}";
		}
		else
		{
			bool supportedTaskAwaiter = (TryReadSingleGenericTypeArgument(awaiterTypeName, "System.Runtime.CompilerServices.TaskAwaiter<", out string awaiterResultTypeName) || TryReadSingleGenericTypeArgument(awaiterTypeName, "System.Runtime.CompilerServices.ValueTaskAwaiter<", out awaiterResultTypeName)) && TryCreateSupportedAsyncRuntimeResultShape(awaiterResultTypeName, out _);
			if (!supportedTaskAwaiter)
			{
				return false;
			}
			source = $"extern \"C\" void {GetExternalRuntimeHelperSymbol(callee)}(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2)\n{{\n    (void)chaos_arg_0;\n    auto* chaos_task = chaos_require_async_task(*chaos_resolve_native_int_slot(chaos_arg_1));\n    if (!chaos_task->completed)\n    {{\n        CHAOS_IL2CPP_ABORT();\n    }}\n\n    {continuationMethod.NativeSymbol}(chaos_arg_2);\n}}";
		}
		helperDefinition = new ExternalRuntimeHelperDefinition(callee, GetExternalRuntimeHelperSymbol(callee), source, new AotCoreIrAbiSlotArtifact[]
		{
			CreateNativeIntAbiSlot(),
			CreateNativeIntAbiSlot(),
			CreateNativeIntAbiSlot()
		}, CreateVoidAbiSlot(), new HashSet<int> { 0, 1, 2 });
		return true;
	}


}

