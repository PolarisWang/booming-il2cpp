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
	private static readonly List<string> s_emptyFieldList = new List<string>(0);

	private (int vtableOffset, int methodCount) ComputeInterfaceVtableInfo(string ifaceSubjectId)
	{
		if (_vtableSlotMap == null)
			return (0, 0);

		var slots = new List<int>();
		foreach (var method in _methodsBySubjectId.Values)
		{
			if (method.IsStatic) continue;
			if (!string.Equals(method.Identity.DeclaringTypeSubjectId, ifaceSubjectId, StringComparison.Ordinal))
				continue;

			var sig = GetMethodSignatureSuffix(method.SubjectId);
			if (_vtableSlotMap.TryGetValue(sig, out int slot))
			{
				slots.Add(slot);
			}
		}

		if (slots.Count == 0)
			return (0, 0);

		return (slots.Min(), slots.Count);
	}

	private void EmitObjectModelDeclarations(
		StringBuilder builder,
		IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods,
		IReadOnlyList<ExternalRuntimeHelperDefinition> externalRuntimeHelpers)
	{
		HashSet<string> referenceTypeSubjectIds = new HashSet<string>(StringComparer.Ordinal);
		Dictionary<string, string?> referenceTypeBaseSubjectIds = new Dictionary<string, string?>(StringComparer.Ordinal);
		HashSet<string> interfaceTypeSubjectIds = new HashSet<string>(StringComparer.Ordinal);
		Dictionary<string, HashSet<string>> referenceTypeImplementedInterfaceSubjectIds = new Dictionary<string, HashSet<string>>(StringComparer.Ordinal);
		HashSet<string> valueTypeSubjectIds = new HashSet<string>(StringComparer.Ordinal);
		Dictionary<string, string?> hashSet = new Dictionary<string, string?>(StringComparer.Ordinal);
		Dictionary<string, string?> hashSet2 = new Dictionary<string, string?>(StringComparer.Ordinal);
		HashSet<string> hashSet3 = new HashSet<string>(StringComparer.Ordinal);
		builder.AppendLine("#include <chaos/type_info.h>");
		builder.AppendLine();
		builder.AppendLine("constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_managed_array = 1;");
		builder.AppendLine("inline TypeInfo chaos_type_info_managed_array = { nullptr, 1ULL, nullptr, nullptr, 0, 0, 2 };");
		builder.AppendLine();
		builder.AppendLine("struct chaos_managed_array");
		builder.AppendLine("{");
		builder.AppendLine("    chaos_object_header header{};");
		builder.AppendLine("    CHAOS_IL2CPP_UINT8 element_type_shape = 0;");
		builder.AppendLine("    const TypeInfo* element_type_info = nullptr;");
		builder.AppendLine("    CHAOS_IL2CPP_INTPTR length = 0;");
		builder.AppendLine("    CHAOS_IL2CPP_INTPTR* elements = nullptr;");
		builder.AppendLine("};");
		builder.AppendLine();
		builder.AppendLine("constexpr CHAOS_IL2CPP_UINT8 chaos_type_shape_reference = 1;");
		builder.AppendLine("constexpr CHAOS_IL2CPP_UINT8 chaos_type_shape_value = 2;");
		builder.AppendLine("constexpr CHAOS_IL2CPP_UINT8 chaos_type_shape_interface = 3;");
		builder.AppendLine();
		builder.AppendLine("constexpr CHAOS_IL2CPP_INTPTR chaos_managed_pointer_local_slot_tag = 1;");
		builder.AppendLine();
		builder.AppendLine("CHAOS_IL2CPP_INTPTR chaos_normalize_native_int_argument(CHAOS_IL2CPP_INTPTR chaos_value) noexcept");
		builder.AppendLine("{");
		builder.AppendLine("    if ((chaos_value & chaos_managed_pointer_local_slot_tag) == 0)");
		builder.AppendLine("    {");
		builder.AppendLine("        return chaos_value;");
		builder.AppendLine("    }");
		builder.AppendLine();
		builder.AppendLine("    auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_value & ~chaos_managed_pointer_local_slot_tag));");
		builder.AppendLine("    return *chaos_slot;");
		builder.AppendLine("}");
		builder.AppendLine();
		builder.AppendLine("template <typename TValue>");
		builder.AppendLine("TValue* chaos_resolve_managed_value_pointer(CHAOS_IL2CPP_INTPTR chaos_managed_pointer)");
		builder.AppendLine("{");
		builder.AppendLine("    if ((chaos_managed_pointer & chaos_managed_pointer_local_slot_tag) != 0)");
		builder.AppendLine("    {");
		builder.AppendLine("        auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_managed_pointer & ~chaos_managed_pointer_local_slot_tag));");
		builder.AppendLine("        if (*chaos_slot == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
		builder.AppendLine("        {");
		builder.AppendLine("            *chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(new TValue{});");
		builder.AppendLine("        }");
		builder.AppendLine("        return reinterpret_cast<TValue*>(*chaos_slot);");
		builder.AppendLine("    }");
		builder.AppendLine();
		builder.AppendLine("    return reinterpret_cast<TValue*>(chaos_managed_pointer);");
		builder.AppendLine("}");
		builder.AppendLine();
		foreach (ExternalRuntimeHelperDefinition externalRuntimeHelper in externalRuntimeHelpers)
		{
			IEnumerable<string> referencedStaticFieldSubjectIds = externalRuntimeHelper.ReferencedStaticFieldSubjectIds is null
				? Array.Empty<string>()
				: externalRuntimeHelper.ReferencedStaticFieldSubjectIds;
			foreach (string referencedStaticFieldSubjectId in referencedStaticFieldSubjectIds)
			{
				if (!string.IsNullOrEmpty(referencedStaticFieldSubjectId))
				{
					hashSet2.Add(referencedStaticFieldSubjectId, null);
				}
			}
		}
		foreach (AotCoreIrMethodArtifact reachableMethod in reachableMethods)
		{
			TrackAbiSlotCarrier(reachableMethod.ReturnAbi);
			foreach (AotCoreIrAbiSlotArtifact parameterAbi in reachableMethod.ParameterAbis)
			{
				TrackAbiSlotCarrier(parameterAbi);
			}
			foreach (AotCoreIrExceptionRegionArtifact exceptionRegion in reachableMethod.ExceptionRegions)
			{
				if (exceptionRegion.HandlingKindCode == AotCoreIrExceptionRegionKind.Catch && !string.IsNullOrEmpty(exceptionRegion.CatchTypeSubjectId))
				{
					TrackReferenceType(exceptionRegion.CatchTypeSubjectId, null);
				}
			}
			foreach (AotCoreIrInstructionArtifact instruction in reachableMethod.Instructions)
			{
				bool flag;
				switch (instruction.Op)
				{
				case "add.ovf":
				case "sub.ovf":
				case "mul.ovf":
				case "conv.ovf.i1":
				case "conv.ovf.u1":
					flag = true;
					break;
				default:
					flag = false;
					break;
				}
				if (flag)
				{
					TrackReferenceType("System.Private.CoreLib/System.OverflowException", null);
				}
				if (string.Equals(instruction.Op, "ldstr", StringComparison.Ordinal))
				{
					TrackReferenceType("System.Private.CoreLib/System.String", null);
				}
				if (IsTypeReflectionHelperSubjectId(instruction.Callee ?? string.Empty))
				{
					TrackReferenceType("System.Private.CoreLib/System.Type", "System.Private.CoreLib/System.Object");
				}
				if (IsReflectionMemberHelperSubjectId(instruction.Callee ?? string.Empty) || MatchesMethodSubject(instruction.Callee ?? string.Empty, "System.Reflection.MethodInfo", "MakeGenericMethod", "System.Type[]"))
				{
					TrackReferenceType("System.Private.CoreLib/System.Reflection.MethodInfo", "System.Private.CoreLib/System.Object");
				}
				if (string.Equals(ManagedNaming.NormalizeSubjectIdAssembly(instruction.Callee ?? string.Empty), "System.Private.CoreLib/System.Type::GetConstructors(System.Reflection.BindingFlags)", StringComparison.Ordinal))
				{
					TrackReferenceType("System.Private.CoreLib/System.Reflection.ConstructorInfo", "System.Private.CoreLib/System.Object");
				}
				if (string.Equals(ManagedNaming.NormalizeSubjectIdAssembly(instruction.Callee ?? string.Empty), "System.Private.CoreLib/System.Type::GetField(System.String)", StringComparison.Ordinal))
				{
					TrackReferenceType("System.Private.CoreLib/System.Reflection.FieldInfo", "System.Private.CoreLib/System.Object");
				}
				if (string.Equals(ManagedNaming.NormalizeSubjectIdAssembly(instruction.Callee ?? string.Empty), "System.Private.CoreLib/System.Reflection.MethodBase::GetParameters()", StringComparison.Ordinal) || string.Equals(ManagedNaming.NormalizeSubjectIdAssembly(instruction.Callee ?? string.Empty), "System.Private.CoreLib/System.Reflection.ParameterInfo::get_Name()", StringComparison.Ordinal))
				{
					TrackReferenceType("System.Private.CoreLib/System.Reflection.ParameterInfo", "System.Private.CoreLib/System.Object");
				}
				AotCoreIrReferenceArtifact? targetReference = instruction.TargetReference;
				if (targetReference is null)
				{
					continue;
				}
				flag = targetReference.Kind == AotCoreIrReferenceKind.Type;
				bool flag2;
				if (flag)
				{
					switch (instruction.RuntimeServiceKind)
					{
					case AotCoreIrRuntimeServiceKind.NewObject:
					case AotCoreIrRuntimeServiceKind.NewArray:
					case AotCoreIrRuntimeServiceKind.CastClass:
					case AotCoreIrRuntimeServiceKind.IsInst:
						flag2 = true;
						break;
					default:
						flag2 = false;
						break;
					}
					flag = flag2;
				}
				if (flag)
				{
					if (HasArrayElementReference(targetReference))
					{
						TrackCarrierType(targetReference.ArrayElementSubjectId!, targetReference.ArrayElementTypeShape, targetReference.ArrayElementBaseTypeSubjectId, targetReference.ArrayElementImplementedInterfaceSubjectIds);
					}
					else
					{
						TrackCarrierType(targetReference.SubjectId, targetReference.TypeShape, targetReference.BaseTypeSubjectId, targetReference.ImplementedInterfaceSubjectIds);
					}
					continue;
				}
				flag = targetReference.Kind == AotCoreIrReferenceKind.Type;
				AotCoreIrRuntimeServiceKind? runtimeServiceKind;
				if (flag)
				{
					runtimeServiceKind = instruction.RuntimeServiceKind;
					if (runtimeServiceKind.HasValue)
					{
						AotCoreIrRuntimeServiceKind valueOrDefault = runtimeServiceKind.GetValueOrDefault();
						if (valueOrDefault - 9 <= AotCoreIrRuntimeServiceKind.LoadInstanceField)
						{
							flag2 = true;
							goto IL_06a8;
						}
					}
					flag2 = false;
					goto IL_06a8;
				}
				goto IL_06ac;
				IL_07fe:
				if (flag)
				{
					hashSet2.Add(targetReference.SubjectId, targetReference.FieldTypeSubjectId);
				}
				continue;
				IL_06ac:
				if (flag)
				{
					if (targetReference.TypeShape == AotCoreIrTypeShapeKind.ValueType)
					{
						valueTypeSubjectIds.Add(targetReference.SubjectId);
					}
					hashSet3.Add(targetReference.SubjectId);
					continue;
				}
				if (targetReference.Kind == AotCoreIrReferenceKind.Type && instruction.RuntimeServiceKind == AotCoreIrRuntimeServiceKind.InitObject)
				{
					if (targetReference.TypeShape == AotCoreIrTypeShapeKind.ValueType)
					{
						valueTypeSubjectIds.Add(targetReference.SubjectId);
					}
					continue;
				}
				flag = targetReference.Kind == AotCoreIrReferenceKind.Type;
				if (flag)
				{
					string op = instruction.Op;
					flag2 = ((op == "ldobj" || op == "stobj") ? true : false);
					flag = flag2;
				}
				if (flag)
				{
					if (targetReference.TypeShape == AotCoreIrTypeShapeKind.ValueType)
					{
						valueTypeSubjectIds.Add(targetReference.SubjectId);
					}
					continue;
				}
				if (targetReference.Kind != AotCoreIrReferenceKind.Field)
				{
					continue;
				}
				string requiredDeclaringTypeSubjectId = GetRequiredDeclaringTypeSubjectId(targetReference);
				if (targetReference.DeclaringTypeShape == AotCoreIrTypeShapeKind.ValueType)
				{
					valueTypeSubjectIds.Add(requiredDeclaringTypeSubjectId);
				}
				else
				{
					TrackReferenceType(requiredDeclaringTypeSubjectId, null);
				}
				runtimeServiceKind = instruction.RuntimeServiceKind;
				if (runtimeServiceKind.HasValue)
				{
					AotCoreIrRuntimeServiceKind valueOrDefault = runtimeServiceKind.GetValueOrDefault();
					if (valueOrDefault - 4 <= AotCoreIrRuntimeServiceKind.NewObject)
					{
						flag = true;
						goto IL_07fe;
					}
				}
				flag = false;
				goto IL_07fe;
				IL_06a8:
				flag = flag2;
				goto IL_06ac;
			}
		}
		foreach (string additionalReferenceTypeSubjectId in _customAttributeSupport.AdditionalReferenceTypeSubjectIds)
		{
			TrackReferenceType(additionalReferenceTypeSubjectId, "System.Private.CoreLib/System.Object");
		}
		foreach (string additionalInstanceFieldSubjectId in _customAttributeSupport.AdditionalInstanceFieldSubjectIds)
		{
			string declaringTypeSubjectId = GetDeclaringTypeSubjectId(additionalInstanceFieldSubjectId);
			TrackReferenceType(declaringTypeSubjectId, "System.Private.CoreLib/System.Object");
			hashSet.Add(additionalInstanceFieldSubjectId, null);
		}
		foreach (StaticInitializationPlan value9 in _staticInitializationSupport.PlansByTypeSubjectId.Values)
		{
			foreach (StaticInitializationAction action in value9.Actions)
			{
				TrackReferenceType(action.ConstructedTypeSubjectId, null);
			}
		}
		if (_reflectionMemberSupport.TypeEntries.Count > 0 || _reflectionMemberSupport.FieldEntries.Count > 0 || _reflectionMemberSupport.MethodEntries.Count > 0)
		{
			TrackReferenceType("System.Private.CoreLib/System.Type", "System.Private.CoreLib/System.Object");
		}
		if (_reflectionMemberSupport.FieldEntries.Count > 0)
		{
			TrackReferenceType("System.Private.CoreLib/System.Reflection.FieldInfo", "System.Private.CoreLib/System.Object");
		}
		if (_reflectionMemberSupport.MethodEntries.Count > 0)
		{
			TrackReferenceType("System.Private.CoreLib/System.Reflection.ParameterInfo", "System.Private.CoreLib/System.Object");
			if (_reflectionMemberSupport.MethodEntries.Any((ReflectionMemberMethodEntry entry) => entry.IsConstructor))
			{
				TrackReferenceType("System.Private.CoreLib/System.Reflection.ConstructorInfo", "System.Private.CoreLib/System.Object");
			}
			if (_reflectionMemberSupport.MethodEntries.Any((ReflectionMemberMethodEntry entry) => !entry.IsConstructor))
			{
				TrackReferenceType("System.Private.CoreLib/System.Reflection.MethodInfo", "System.Private.CoreLib/System.Object");
			}
		}
		if (_assemblyReflectionSupport.TypeEntries.Count > 0)
		{
			TrackReferenceType("System.Private.CoreLib/System.Type", "System.Private.CoreLib/System.Object");
			TrackReferenceType("System.Private.CoreLib/System.Reflection.Assembly", "System.Private.CoreLib/System.Object");
			TrackReferenceType("System.Private.CoreLib/System.Reflection.AssemblyName", "System.Private.CoreLib/System.Object");
		}
		// Track all types referenced in reflection type entries so their TypeInfo/type_id
		// declarations are emitted (needed by scriban templates like ReflectionGetObjectType).
		foreach (var entry in _reflectionMemberSupport.TypeEntries)
		{
			TrackReferenceType(entry.TypeSubjectId, "System.Private.CoreLib/System.Object");
		}
		// Pre-index fields by declaring type to avoid O(n*m) Where+OrderBy per type.
		var fieldsByDeclaringType = new Dictionary<string, List<string>>(StringComparer.Ordinal);
		var fieldTypeMap = new Dictionary<string, string?>(StringComparer.Ordinal);
		foreach (var field in hashSet)
		{
			var declaringType = GetDeclaringTypeSubjectId(field.Key);
			if (!fieldsByDeclaringType.TryGetValue(declaringType, out var list))
				fieldsByDeclaringType[declaringType] = list = new List<string>();
			list.Add(field.Key);
			fieldTypeMap[field.Key] = field.Value;
		}
		// Sort each type's field list once.
		foreach (var list in fieldsByDeclaringType.Values)
			list.Sort(StringComparer.Ordinal);
		int num = 2;
		// Ensure System.String is always tracked (used in IsArrayStoreCompatible fast path)
		TrackReferenceType("System.Private.CoreLib/System.String", null);
		// Ensure reflection types used by ReflectionObjectEmission are tracked
		TrackReferenceType("System.Private.CoreLib/System.Reflection.MethodInfo", null);
		TrackReferenceType("System.Private.CoreLib/System.Reflection.ConstructorInfo", null);
		TrackReferenceType("System.Private.CoreLib/System.Reflection.FieldInfo", null);
		TrackReferenceType("System.Private.CoreLib/System.Reflection.Assembly", null);
		TrackReferenceType("System.Private.CoreLib/System.Reflection.AssemblyName", null);
		// ── VTable slot allocation (must precede iface_map emission)
			var vtableLengths = new Dictionary<string, int>(StringComparer.Ordinal);
			var slotMap = new Dictionary<string, int>(StringComparer.Ordinal);
			var methodsByDeclaringTypeVT = new Dictionary<string, List<AotCoreIrMethodArtifact>>(StringComparer.Ordinal);
			foreach (var method in _methodsBySubjectId.Values)
			{
				var dt = method.Identity.DeclaringTypeSubjectId;
				if (string.IsNullOrEmpty(dt)) continue;
				if (!methodsByDeclaringTypeVT.TryGetValue(dt, out var list))
					methodsByDeclaringTypeVT[dt] = list = new List<AotCoreIrMethodArtifact>();
				list.Add(method);
			}
			int nextSlot = 0;
			foreach (string typeId in TopologicalSortReferenceTypes(referenceTypeSubjectIds, referenceTypeBaseSubjectIds))
			{
				if (methodsByDeclaringTypeVT.TryGetValue(typeId, out var typeMethods))
				{
					typeMethods.Sort((a, b) => string.Compare(a.SubjectId, b.SubjectId, StringComparison.Ordinal));
					foreach (var method in typeMethods)
					{
						if (method.IsStatic) continue;
						var sig = GetMethodSignatureSuffix(method.SubjectId);
						if (!slotMap.ContainsKey(sig))
						{
							slotMap[sig] = nextSlot++;
						}
					}
				}
			}
			// ── Compute per-type vtable length: max slot in hierarchy + 1 ──
			// Global nextSlot ensures slot uniqueness (interface dispatch correctness),
			// but vtableLengths must only cover the type's own hierarchy to avoid
			// unnecessary trailing nullptr entries from unrelated hierarchies.
			foreach (string typeId in TopologicalSortReferenceTypes(referenceTypeSubjectIds, referenceTypeBaseSubjectIds))
			{
				int maxSlot = -1;
				string? current = typeId;
				while (current != null && referenceTypeSubjectIds.Contains(current))
				{
					if (methodsByDeclaringTypeVT.TryGetValue(current, out var typeMethods))
					{
						foreach (var method in typeMethods)
						{
							if (method.IsStatic) continue;
							var sig = GetMethodSignatureSuffix(method.SubjectId);
							if (slotMap.TryGetValue(sig, out int slot) && slot > maxSlot)
								maxSlot = slot;
						}
					}
					referenceTypeBaseSubjectIds.TryGetValue(current, out current);
				}
				vtableLengths[typeId] = maxSlot + 1;
			}
			_vtableSlotMap = slotMap;
			_vtableLengths = vtableLengths;
			_vtableTypes = new HashSet<string>(vtableLengths.Where(x => x.Value > 0).Select(x => x.Key), StringComparer.Ordinal);
			// ── Pre-declare interface type_ids so iface_map arrays can reference them ──
		foreach (string ifaceId in interfaceTypeSubjectIds.OrderBy(id => id, StringComparer.Ordinal))
		{
			ulong ifaceStableId = ComputeStableTypeId(ifaceId);
			builder.Append("inline constexpr CHAOS_IL2CPP_INTPTR ");
			builder.Append(GetNativeTypeIdSymbol(ifaceId));
			builder.Append(" = static_cast<CHAOS_IL2CPP_INTPTR>(");
			builder.Append(ifaceStableId.ToString());
			builder.AppendLine("ULL);");
		}
		// ── TypeInfo instances (replace integer type_id system) ──
		foreach (string item in TopologicalSortReferenceTypes(referenceTypeSubjectIds, referenceTypeBaseSubjectIds))
		{
			ulong stableId = ComputeStableTypeId(item);
			string parentExpr = "nullptr";
			if (referenceTypeBaseSubjectIds.TryGetValue(item, out string? baseTypeId) && !string.IsNullOrEmpty(baseTypeId))
			{
				parentExpr = "&" + GetNativeTypeInfoSymbol(baseTypeId);
			}
			// ── iface_map (InterfaceMapEntry array with vtable_offset) ──
			bool hasIfaceMap = _referenceTypeImplementedInterfaceSubjectIds.TryGetValue(item, out var ifaceSubjectIds) && ifaceSubjectIds.Count > 0;
			string ifaceMapExpr;
			string ifaceCountExpr;
			if (hasIfaceMap)
			{
				var sortedIfaceIds = ifaceSubjectIds!.OrderBy(id => ComputeStableTypeId(id)).ToArray();
				ifaceMapExpr = GetNativeIfaceMapSymbol(item);
				ifaceCountExpr = sortedIfaceIds.Length.ToString();
				{
					StringBuilder sb = builder;
					sb.Append("static constexpr InterfaceMapEntry ");
					sb.Append(ifaceMapExpr);
					sb.AppendLine("[] = {");
					for (int i = 0; i < sortedIfaceIds.Length; i++)
					{
						var (vtableOffset, methodCount) = ComputeInterfaceVtableInfo(sortedIfaceIds[i]);
						sb.Append("    { ");
						sb.Append(GetNativeTypeIdSymbol(sortedIfaceIds[i]));
						sb.Append(", ");
						sb.Append(vtableOffset.ToString());
						sb.Append(", ");
						sb.Append(methodCount.ToString());
						sb.Append(" }");
						if (i < sortedIfaceIds.Length - 1) sb.AppendLine(",");
						else sb.AppendLine();
					}
					sb.AppendLine("};");
				}
			}
			else
			{
				ifaceMapExpr = "nullptr";
				ifaceCountExpr = "0";
			}
			{
				StringBuilder stringBuilder = builder;
				StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(28, 2, stringBuilder);
				handler.AppendLiteral("inline TypeInfo ");
				handler.AppendFormatted(GetNativeTypeInfoSymbol(item));
				handler.AppendLiteral(" = { ");
				handler.AppendFormatted(parentExpr);
				handler.AppendLiteral(", ");
				handler.AppendFormatted(stableId.ToString() + "ULL");
				handler.AppendLiteral(", ");
				handler.AppendFormatted(ifaceMapExpr);
				handler.AppendLiteral(", nullptr, ");
				handler.AppendFormatted(ifaceCountExpr);
				handler.AppendLiteral(", 0, 1 /* reference */ };");
				stringBuilder.AppendLine(ref handler);
			}
			{
				StringBuilder stringBuilder = builder;
				StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(28, 2, stringBuilder);
				handler.AppendLiteral("inline constexpr CHAOS_IL2CPP_INTPTR ");
				handler.AppendFormatted(GetNativeTypeIdSymbol(item));
				handler.AppendLiteral(" = static_cast<CHAOS_IL2CPP_INTPTR>(");
				handler.AppendFormatted(stableId.ToString() + "ULL");
				handler.AppendLiteral(");");
				stringBuilder.AppendLine(ref handler);
			}
			num++;
		}
		foreach (string item2 in interfaceTypeSubjectIds.OrderBy<string, string>((string result) => result, StringComparer.Ordinal))
		{
			ulong stableId = ComputeStableTypeId(item2);
			{
				StringBuilder stringBuilder = builder;
				StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(28, 2, stringBuilder);
				handler.AppendLiteral("inline TypeInfo ");
				handler.AppendFormatted(GetNativeTypeInfoSymbol(item2));
				handler.AppendLiteral(" = { nullptr, ");
				handler.AppendFormatted(stableId.ToString() + "ULL");
				handler.AppendLiteral(", nullptr, nullptr, 0, 0, 3 /* interface */ };");
				stringBuilder.AppendLine(ref handler);
			}

			num++;
		}
		foreach (string item3 in valueTypeSubjectIds.OrderBy<string, string>((string result) => result, StringComparer.Ordinal))
		{
			ulong stableId = ComputeStableTypeId(item3);
			{
				StringBuilder stringBuilder = builder;
				StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(28, 2, stringBuilder);
				handler.AppendLiteral("inline TypeInfo ");
				handler.AppendFormatted(GetNativeTypeInfoSymbol(item3));
				handler.AppendLiteral(" = { nullptr, ");
				handler.AppendFormatted(stableId.ToString() + "ULL");
				handler.AppendLiteral(", nullptr, nullptr, 0, 0, 2 /* value */ };");
				stringBuilder.AppendLine(ref handler);
			}
			{
				StringBuilder stringBuilder = builder;
				StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(28, 2, stringBuilder);
				handler.AppendLiteral("inline constexpr CHAOS_IL2CPP_INTPTR ");
				handler.AppendFormatted(GetNativeTypeIdSymbol(item3));
				handler.AppendLiteral(" = static_cast<CHAOS_IL2CPP_INTPTR>(");
				handler.AppendFormatted(stableId.ToString() + "ULL");
				handler.AppendLiteral(");");
				stringBuilder.AppendLine(ref handler);
			}
			num++;
		}
		var sortedHashSet3 = hashSet3.OrderBy<string, string>((string result) => result, StringComparer.Ordinal).ToArray();
		foreach (string item3 in sortedHashSet3)
		{
			ulong stableId = ComputeStableTypeId(item3);
			// ── iface_map (InterfaceMapEntry array with vtable_offset) ──
			bool hasIfaceMap = _referenceTypeImplementedInterfaceSubjectIds.TryGetValue(item3, out var ifaceSubjectIds) && ifaceSubjectIds.Count > 0;
			string ifaceMapExpr;
			string ifaceCountExpr;
			if (hasIfaceMap)
			{
				var sortedIfaceIds = ifaceSubjectIds!.OrderBy(id => ComputeStableTypeId(id)).ToArray();
				ifaceMapExpr = GetNativeIfaceMapSymbol(item3);
				ifaceCountExpr = sortedIfaceIds.Length.ToString();
				{
					StringBuilder sb = builder;
					sb.Append("static constexpr InterfaceMapEntry ");
					sb.Append(ifaceMapExpr);
					sb.AppendLine("[] = {");
					for (int i = 0; i < sortedIfaceIds.Length; i++)
					{
						var (vtableOffset, methodCount) = ComputeInterfaceVtableInfo(sortedIfaceIds[i]);
						sb.Append("    { ");
						sb.Append(GetNativeTypeIdSymbol(sortedIfaceIds[i]));
						sb.Append(", ");
						sb.Append(vtableOffset.ToString());
						sb.Append(", ");
						sb.Append(methodCount.ToString());
						sb.Append(" }");
						if (i < sortedIfaceIds.Length - 1) sb.AppendLine(",");
						else sb.AppendLine();
					}
					sb.AppendLine("};");
				}
			}
			else
			{
				ifaceMapExpr = "nullptr";
				ifaceCountExpr = "0";
			}
			{
				StringBuilder stringBuilder = builder;
				StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(28, 2, stringBuilder);
				handler.AppendLiteral("inline TypeInfo ");
				handler.AppendFormatted(GetNativeTypeInfoSymbol(item3));
				handler.AppendLiteral(" = { nullptr, ");
				handler.AppendFormatted(stableId.ToString() + "ULL");
				handler.AppendLiteral(", ");
				handler.AppendFormatted(ifaceMapExpr);
				handler.AppendLiteral(", nullptr, ");
				handler.AppendFormatted(ifaceCountExpr);
				handler.AppendLiteral(", 0, 2 /* value (boxed) */ };");
				stringBuilder.AppendLine(ref handler);
			}
			{
				StringBuilder stringBuilder = builder;
				StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(28, 2, stringBuilder);
				handler.AppendLiteral("inline constexpr CHAOS_IL2CPP_INTPTR ");
				handler.AppendFormatted(GetNativeBoxTypeIdSymbol(item3));
				handler.AppendLiteral(" = static_cast<CHAOS_IL2CPP_INTPTR>(");
				handler.AppendFormatted(stableId.ToString() + "ULL");
				handler.AppendLiteral(");");
				stringBuilder.AppendLine(ref handler);
			}
			num++;
		}
		if (referenceTypeSubjectIds.Count > 0 || interfaceTypeSubjectIds.Count > 0 || valueTypeSubjectIds.Count > 0 || hashSet3.Count > 0)
		{
			builder.AppendLine();
		}
		// ── VTable arrays (uses pre-computed _vtableSlotMap, _vtableLengths) ──
		if (referenceTypeSubjectIds.Count > 0)
		{
				builder.AppendLine("// ── Virtual method table arrays ──");
				// RegisterVTable is declared via #include "runtime_vtable.h"
			foreach (string typeId in TopologicalSortReferenceTypes(referenceTypeSubjectIds, referenceTypeBaseSubjectIds))
			{
				if (!_vtableLengths.TryGetValue(typeId, out int vtLen) || vtLen == 0) continue;
				var entries = new string[vtLen];
				// Walk hierarchy to fill entries (most derived first)
				string? current = typeId;
				while (current != null && referenceTypeSubjectIds.Contains(current))
				{
					if (methodsByDeclaringTypeVT.TryGetValue(current, out var typeMethods))
					{
						foreach (var method in typeMethods)
						{
							if (method.IsStatic || !CanEmitMethodBody(method)) continue;
							var sig = GetMethodSignatureSuffix(method.SubjectId);
							if (_vtableSlotMap.TryGetValue(sig, out int slot) && slot < vtLen && entries[slot] == null)
							{
								entries[slot] = TryGetInstantiationStubSymbol(method) ?? method.NativeSymbol;
							}
						}
					}
					referenceTypeBaseSubjectIds.TryGetValue(current, out string? nextCurrent);
					current = nextCurrent;
				}
				// Emit extern declarations for methods referenced in vtable array
				var externDeclared = new HashSet<string>(StringComparer.Ordinal);
				foreach (var entry in entries)
				{
					if (entry is not null && externDeclared.Add(entry))
					{
						builder.Append("extern void ");
						builder.Append(entry);
						builder.AppendLine("();");
					}
				}
				// Emit vtable array
				StringBuilder stringBuilder = builder;
				StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(24, 1, stringBuilder);
				handler.AppendLiteral("static const void* ");
				handler.AppendFormatted(GetNativeVTableSymbol(typeId));
				handler.AppendLiteral("[] =");
				stringBuilder.AppendLine(ref handler);
				builder.AppendLine("{");
				foreach (var entry in entries)
				{
					if (entry != null)
					{
						builder.Append("    reinterpret_cast<void*>(");
						builder.Append(entry);
						builder.AppendLine("),");
					}
					else
					{
						builder.AppendLine("    nullptr,");
					}
				}
				builder.AppendLine("};");
				ulong stableId = ComputeStableTypeId(typeId);
				builder.Append("static const int s_vtreg_");
				builder.Append(GetNativeSymbol("", typeId));
				builder.Append(" = (::chaos::il2cpp::runtime_vtable::RegisterVTable(CHAOS_IL2CPP_UINT64_C(");
				builder.Append(stableId.ToString());
				builder.Append("), ");
				builder.Append(GetNativeVTableSymbol(typeId));
				builder.Append(", ");
				builder.Append(vtLen.ToString());
				builder.AppendLine("u), 0);");
				builder.AppendLine();
			}
		}
		// ── Virtual dispatch helper (replaces switch-based dispatch) ──
		builder.AppendLine("inline void* chaos_vtable_resolve(const void** vtable, CHAOS_IL2CPP_UINT32 slot) noexcept");
		builder.AppendLine("{");
		builder.AppendLine("    if (vtable == nullptr) CHAOS_IL2CPP_ABORT();");
		builder.AppendLine("    if (vtable[slot] == nullptr) CHAOS_IL2CPP_ABORT();");
		builder.AppendLine("    return const_cast<void*>(vtable[slot]);");
		builder.AppendLine("}");
		builder.AppendLine();
		// ── Parent type info resolver (replaces chaos_get_base_type_id switch) ──
		builder.AppendLine("inline const TypeInfo* chaos_get_parent_type_info(const TypeInfo* chaos_ti) noexcept");
		builder.AppendLine("{");
		builder.AppendLine("    if (chaos_ti == nullptr) return nullptr;");
		builder.AppendLine("    return chaos_ti->parent;");
		builder.AppendLine("}");
		builder.AppendLine();
		// ── Type compatibility (TypeInfo* pointer based, replaces integer walk) ──
		builder.AppendLine("bool chaos_is_type_compatible(const TypeInfo* chaos_actual_type_info, const TypeInfo* chaos_target_type_info) noexcept");
		builder.AppendLine("{");
		builder.AppendLine("    auto* chaos_current = chaos_actual_type_info;");
		builder.AppendLine("    while (chaos_current != nullptr)");
		builder.AppendLine("    {");
		builder.AppendLine("        if (chaos_current == chaos_target_type_info || chaos_current->stable_id == chaos_target_type_info->stable_id)");
		builder.AppendLine("        {");
		builder.AppendLine("            return true;");
		builder.AppendLine("        }");
		builder.AppendLine();
		builder.AppendLine("        chaos_current = chaos_current->parent;");
		builder.AppendLine("    }");
		builder.AppendLine();
		builder.AppendLine("    return false;");
		builder.AppendLine("}");
		builder.AppendLine();
		// ── Interface check (iface_map + runtime_iface_map linear scan) ──
		builder.AppendLine("bool chaos_type_implements_interface(const TypeInfo* chaos_actual_type_info, const TypeInfo* chaos_target_interface_type_info) noexcept");
		builder.AppendLine("{");
		builder.AppendLine("    if (chaos_actual_type_info->iface_count == 0 &&");
		builder.AppendLine("        chaos_actual_type_info->runtime_iface_count == 0)");
		builder.AppendLine("    {");
		builder.AppendLine("        return false;");
		builder.AppendLine("    }");
		builder.AppendLine();
		builder.AppendLine("    for (CHAOS_IL2CPP_UINT32 chaos_i = 0; chaos_i < chaos_actual_type_info->iface_count; chaos_i++)");
		builder.AppendLine("    {");
		builder.AppendLine("        if (chaos_actual_type_info->iface_map[chaos_i].iface_stable_id == chaos_target_interface_type_info->stable_id)");
		builder.AppendLine("        {");
		builder.AppendLine("            return true;");
		builder.AppendLine("        }");
		builder.AppendLine("    }");
		builder.AppendLine();
		builder.AppendLine("    for (CHAOS_IL2CPP_UINT32 chaos_i = 0; chaos_i < chaos_actual_type_info->runtime_iface_count; chaos_i++)");
		builder.AppendLine("    {");
		builder.AppendLine("        if (chaos_actual_type_info->runtime_iface_map[chaos_i].iface_stable_id == chaos_target_interface_type_info->stable_id)");
		builder.AppendLine("        {");
		builder.AppendLine("            return true;");
		builder.AppendLine("        }");
		builder.AppendLine("    }");
		builder.AppendLine();
		builder.AppendLine("    return false;");
		builder.AppendLine("}");
		builder.AppendLine();
		builder.Append("inline CHAOS_IL2CPP_UINT32 chaos_find_interface_offset(\n");
		builder.Append("    const TypeInfo* chaos_actual_type_info,\n");
		builder.Append("    const TypeInfo* chaos_target_interface_type_info) noexcept\n");
		builder.AppendLine("{");
		builder.AppendLine("    for (CHAOS_IL2CPP_UINT32 chaos_i = 0; chaos_i < chaos_actual_type_info->iface_count; chaos_i++)");
		builder.AppendLine("    {");
		builder.AppendLine("        if (chaos_actual_type_info->iface_map[chaos_i].iface_stable_id == chaos_target_interface_type_info->stable_id)");
		builder.AppendLine("        {");
		builder.AppendLine("            return chaos_actual_type_info->iface_map[chaos_i].vtable_offset;");
		builder.AppendLine("        }");
		builder.AppendLine("    }");
		builder.AppendLine("    for (CHAOS_IL2CPP_UINT32 chaos_i = 0; chaos_i < chaos_actual_type_info->runtime_iface_count; chaos_i++)");
		builder.AppendLine("    {");
		builder.AppendLine("        if (chaos_actual_type_info->runtime_iface_map[chaos_i].iface_stable_id == chaos_target_interface_type_info->stable_id)");
		builder.AppendLine("        {");
		builder.AppendLine("            return chaos_actual_type_info->runtime_iface_map[chaos_i].vtable_offset;");
		builder.AppendLine("        }");
		builder.AppendLine("    }");
		builder.AppendLine("    CHAOS_IL2CPP_ABORT();");
		builder.AppendLine("}");
		builder.AppendLine();
		builder.AppendLine("bool chaos_does_type_implement_interface(const TypeInfo* chaos_actual_type_info, const TypeInfo* chaos_target_interface_type_info) noexcept");
		builder.AppendLine("{");
		builder.AppendLine("    auto* chaos_current = chaos_actual_type_info;");
		builder.AppendLine("    while (chaos_current != nullptr)");
		builder.AppendLine("    {");
		builder.AppendLine("        if (chaos_type_implements_interface(chaos_current, chaos_target_interface_type_info))");
		builder.AppendLine("        {");
		builder.AppendLine("            return true;");
		builder.AppendLine("        }");
		builder.AppendLine();
		builder.AppendLine("        chaos_current = chaos_current->parent;");
		builder.AppendLine("    }");
		builder.AppendLine();
		builder.AppendLine("    return false;");
		builder.AppendLine("}");
		builder.AppendLine();
		builder.AppendLine("bool chaos_is_array_type_compatible(");
		builder.AppendLine("    CHAOS_IL2CPP_UINT8 chaos_actual_element_shape,");
		builder.AppendLine("    const TypeInfo* chaos_actual_element_type_info,");
		builder.AppendLine("    CHAOS_IL2CPP_UINT8 chaos_target_element_shape,");
		builder.AppendLine("    const TypeInfo* chaos_target_element_type_info) noexcept");
		builder.AppendLine("{");
		builder.AppendLine("    if (chaos_actual_element_shape == chaos_type_shape_reference)");
		builder.AppendLine("    {");
		builder.AppendLine("        if (chaos_target_element_shape == chaos_type_shape_reference)");
		builder.AppendLine("        {");
		builder.AppendLine("            return chaos_is_type_compatible(chaos_actual_element_type_info, chaos_target_element_type_info);");
		builder.AppendLine("        }");
		builder.AppendLine();
		builder.AppendLine("        if (chaos_target_element_shape == chaos_type_shape_interface)");
		builder.AppendLine("        {");
		builder.AppendLine("            return chaos_does_type_implement_interface(chaos_actual_element_type_info, chaos_target_element_type_info);");
		builder.AppendLine("        }");
		builder.AppendLine();
		builder.AppendLine("        return false;");
		builder.AppendLine("    }");
		builder.AppendLine();
		builder.AppendLine("    return chaos_actual_element_shape == chaos_target_element_shape");
		builder.AppendLine("        && chaos_actual_element_type_info == chaos_target_element_type_info;");
		builder.AppendLine("}");
		builder.AppendLine();
		builder.AppendLine("bool chaos_is_array_store_compatible(const chaos_managed_array* chaos_array, CHAOS_IL2CPP_INTPTR chaos_value) noexcept");
		builder.AppendLine("{");
		builder.AppendLine("    if (chaos_array == nullptr)");
		builder.AppendLine("    {");
		builder.AppendLine("        return false;");
		builder.AppendLine("    }");
		builder.AppendLine();
		builder.AppendLine("    if (chaos_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
		builder.AppendLine("    {");
		builder.AppendLine("        return true;");
		builder.AppendLine("    }");
		builder.AppendLine();
		builder.AppendLine("    // StringId fast path: materialized value carries a heap pointer;");
		builder.AppendLine("    // this line is reached only if caller skipped materialization.");
		builder.AppendLine("    if (chaos_is_string_id(chaos_value))");
		builder.AppendLine("    {");
		builder.Append("        return chaos_array->element_type_shape == chaos_type_shape_reference");
		builder.Append("            && chaos_is_type_compatible(");
		string stringTypeInfoSymbol = GetNativeTypeInfoSymbol("System.Private.CoreLib/System.String");
		builder.Append("&" + stringTypeInfoSymbol);
		builder.AppendLine(", chaos_array->element_type_info);");
		builder.AppendLine("    }");
		builder.AppendLine();
		builder.AppendLine("    auto* chaos_header = reinterpret_cast<chaos_object_header*>(chaos_value);");
		builder.AppendLine("    if (chaos_array->element_type_shape == chaos_type_shape_interface)");
		builder.AppendLine("    {");
		builder.AppendLine("        return chaos_does_type_implement_interface(chaos_header->type_info, chaos_array->element_type_info);");
		builder.AppendLine("    }");
		builder.AppendLine();
		builder.AppendLine("    if (chaos_array->element_type_shape == chaos_type_shape_reference)");
		builder.AppendLine("    {");
		builder.AppendLine("        return chaos_is_type_compatible(chaos_header->type_info, chaos_array->element_type_info);");
		builder.AppendLine("    }");
		builder.AppendLine();
		builder.AppendLine("    return false;");
		builder.AppendLine("}");
		builder.AppendLine();
		foreach (string typeSubjectId in GetReferenceTypeEmissionOrder(referenceTypeSubjectIds, referenceTypeBaseSubjectIds))
		{
			var ns = ManagedNaming.NormalizeSubjectIdAssembly(typeSubjectId);
			// Decimal is a value type that may appear in referenceTypeSubjectIds when
			// used via newobj (boxed heap allocation). Emit a minimal struct with header only.
			bool num2 = string.Equals(ns, "System.Private.CoreLib/System.String", StringComparison.Ordinal);
			bool flag3 = string.Equals(ns, "System.Private.CoreLib/System.Delegate", StringComparison.Ordinal);
			bool flag4 = string.Equals(ns, "System.Private.CoreLib/System.Type", StringComparison.Ordinal);
			bool flag5 = string.Equals(ns, "System.Private.CoreLib/System.Reflection.MethodInfo", StringComparison.Ordinal);
			bool flag6 = string.Equals(ns, "System.Private.CoreLib/System.Reflection.ConstructorInfo", StringComparison.Ordinal);
			bool flag7 = string.Equals(ns, "System.Private.CoreLib/System.Reflection.FieldInfo", StringComparison.Ordinal);
			bool flag8 = string.Equals(ns, "System.Private.CoreLib/System.Reflection.ParameterInfo", StringComparison.Ordinal);
			bool flag9 = string.Equals(ns, "System.Private.CoreLib/System.Reflection.Assembly", StringComparison.Ordinal);
			bool flag10 = string.Equals(ns, "System.Private.CoreLib/System.Reflection.AssemblyName", StringComparison.Ordinal);
			if (string.Equals(ns, "System.Private.CoreLib/System.Decimal", StringComparison.Ordinal))
			{
				builder.AppendLine("struct " + GetNativeTypeSymbol(typeSubjectId));
				builder.AppendLine("{");
				builder.AppendLine("    chaos_object_header header{};");
				builder.AppendLine("};");
				builder.AppendLine();
				continue;
			}
			if (referenceTypeBaseSubjectIds.TryGetValue(typeSubjectId, out string? value6) && !string.IsNullOrWhiteSpace(value6) && referenceTypeSubjectIds.Contains(value6))
			{
				StringBuilder stringBuilder = builder;
				StringBuilder stringBuilder13 = stringBuilder;
				StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(17, 2, stringBuilder);
				handler.AppendLiteral("struct ");
				handler.AppendFormatted(GetNativeTypeSymbol(typeSubjectId));
				handler.AppendLiteral(" : public ");
				handler.AppendFormatted(GetNativeTypeSymbol(value6));
				stringBuilder13.AppendLine(ref handler);
			}
			else
			{
				StringBuilder stringBuilder = builder;
				StringBuilder stringBuilder14 = stringBuilder;
				StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(7, 1, stringBuilder);
				handler.AppendLiteral("struct ");
				handler.AppendFormatted(GetNativeTypeSymbol(typeSubjectId));
				stringBuilder14.AppendLine(ref handler);
			}
			builder.AppendLine("{");
			if (!referenceTypeBaseSubjectIds.TryGetValue(typeSubjectId, out string? value7) || string.IsNullOrWhiteSpace(value7) || !referenceTypeSubjectIds.Contains(value7))
			{
				builder.AppendLine("    chaos_object_header header{};");
			}
			List<string> list = fieldsByDeclaringType.TryGetValue(typeSubjectId, out var fields) ? fields : s_emptyFieldList;
			if (num2)
			{
				builder.AppendLine("    CHAOS_IL2CPP_INTPTR length = 0;");
				builder.AppendLine("    const char* utf8_data = nullptr;");
				builder.AppendLine("    CHAOS_IL2CPP_UINT64 string_id = 0u;  // stable StringId, 0 = not yet materialized");
			}
			if (flag3)
			{
				builder.AppendLine("    CHAOS_IL2CPP_INTPTR chaos_delegate_target = 0;");
				builder.AppendLine("    CHAOS_IL2CPP_INTPTR chaos_delegate_method_ptr = 0;");
				builder.AppendLine("    CHAOS_IL2CPP_INTPTR chaos_delegate_invocation_list = 0;");
				builder.AppendLine("    CHAOS_IL2CPP_INTPTR chaos_delegate_invocation_count = 0;");
			}
			if (flag4)
			{
				builder.AppendLine("    CHAOS_IL2CPP_INTPTR runtime_type_handle = 0;");
				builder.AppendLine("    CHAOS_IL2CPP_INTPTR runtime_name_value = 0;");
				builder.AppendLine("    CHAOS_IL2CPP_INT32 runtime_metadata_token_value = 0;");
			}
			if (flag5 || flag6)
			{
				builder.AppendLine("    CHAOS_IL2CPP_INTPTR declaring_type_handle = 0;");
				builder.AppendLine("    CHAOS_IL2CPP_INTPTR runtime_method_handle = 0;");
				builder.AppendLine("    CHAOS_IL2CPP_INTPTR generic_definition_method_handle = 0;");
				builder.AppendLine("    CHAOS_IL2CPP_INTPTR generic_argument_type_handle = 0;");
				builder.AppendLine("    CHAOS_IL2CPP_INTPTR runtime_name_value = 0;");
				builder.AppendLine("    CHAOS_IL2CPP_INT32 runtime_metadata_token_value = 0;");
			}
			if (flag7)
			{
				builder.AppendLine("    CHAOS_IL2CPP_INTPTR declaring_type_handle = 0;");
				builder.AppendLine("    CHAOS_IL2CPP_INTPTR runtime_name_value = 0;");
				builder.AppendLine("    CHAOS_IL2CPP_INT32 runtime_metadata_token_value = 0;");
			}
			if (flag8)
			{
				builder.AppendLine("    CHAOS_IL2CPP_INTPTR runtime_name_value = 0;");
			}
			if (flag9)
			{
				builder.AppendLine("    CHAOS_IL2CPP_INTPTR runtime_assembly_name_value = 0;");
			}
			if (flag10)
			{
				builder.AppendLine("    CHAOS_IL2CPP_INTPTR runtime_name_value = 0;");
			}
			if (list.Count == 0)
			{
				builder.AppendLine("};");
				builder.AppendLine();
				continue;
			}
			foreach (string item8 in list)
			{
				var cppType = MapFieldTypeToCppType(fieldTypeMap.GetValueOrDefault(item8));
				StringBuilder stringBuilder = builder;
				StringBuilder stringBuilder15 = stringBuilder;
				StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(23, 1, stringBuilder);
				handler.AppendLiteral("    ");
				handler.AppendFormatted(cppType);
				handler.AppendLiteral(" ");
				handler.AppendFormatted(GetNativeFieldMemberName(item8));
				handler.AppendLiteral(" = 0;");
				stringBuilder15.AppendLine(ref handler);
			}
			builder.AppendLine("};");
			builder.AppendLine();
		}
		foreach (string typeSubjectId2 in valueTypeSubjectIds.OrderBy<string, string>((string result) => result, StringComparer.Ordinal))
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder16 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(7, 1, stringBuilder);
			handler.AppendLiteral("struct ");
			handler.AppendFormatted(GetNativeValueTypeSymbol(typeSubjectId2));
			stringBuilder16.AppendLine(ref handler);
			builder.AppendLine("{");
			List<string> list2 = fieldsByDeclaringType.TryGetValue(typeSubjectId2, out var fields2) ? fields2 : s_emptyFieldList;
			if (list2.Count == 0)
			{
				builder.AppendLine("};");
				builder.AppendLine();
				continue;
			}
			foreach (string item9 in list2)
			{
				var cppType = MapFieldTypeToCppType(fieldTypeMap.GetValueOrDefault(item9));
				stringBuilder = builder;
				StringBuilder stringBuilder17 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(23, 1, stringBuilder);
				handler.AppendLiteral("    ");
				handler.AppendFormatted(cppType);
				handler.AppendLiteral(" ");
				handler.AppendFormatted(GetNativeFieldMemberName(item9));
				handler.AppendLiteral(" = 0;");
				stringBuilder17.AppendLine(ref handler);
			}
			builder.AppendLine("};");
			builder.AppendLine();
		}
		foreach (string item10 in sortedHashSet3)
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder18 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(7, 1, stringBuilder);
			handler.AppendLiteral("struct ");
			handler.AppendFormatted(GetNativeBoxTypeSymbol(item10));
			stringBuilder18.AppendLine(ref handler);
			builder.AppendLine("{");
			builder.AppendLine("    chaos_object_header header{};");
			if (IsStructuredValueTypeSubjectId(item10))
			{
				stringBuilder = builder;
				StringBuilder stringBuilder19 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(13, 1, stringBuilder);
				handler.AppendLiteral("    ");
				handler.AppendFormatted(GetNativeValueTypeSymbol(item10));
				handler.AppendLiteral(" value{};");
				stringBuilder19.AppendLine(ref handler);
			}
			else
			{
				builder.AppendLine("    CHAOS_IL2CPP_INTPTR value = 0;");
			}
			builder.AppendLine("};");
			builder.AppendLine();
		}
		EmitObjectEqualityHelpers(builder, reachableMethods, referenceTypeSubjectIds, hashSet3);
		EmitReflectionObjectHelpers(builder, reachableMethods, referenceTypeSubjectIds, hashSet3);
		EmitExceptionMetadataHelpers(builder, reachableMethods);
		foreach (KeyValuePair<string, string?> item11 in hashSet2.OrderBy<KeyValuePair<string, string?>, string>((KeyValuePair<string, string?> result) => result.Key, StringComparer.Ordinal))
		{
			var cppType = MapFieldTypeToCppType(item11.Value);
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder20 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(23, 1, stringBuilder);
			handler.AppendLiteral(cppType);
			handler.AppendLiteral(" ");
			handler.AppendFormatted(GetNativeStaticFieldSymbol(item11.Key));
			handler.AppendLiteral(" = 0;");
			stringBuilder20.AppendLine(ref handler);
		}
		if (hashSet2.Count > 0)
		{
			builder.AppendLine();
		}
		// Capture emitted type subject IDs for Phase 0 ModuleRegistry Tier 0 arrays
		_allEmittedTypeSubjectIds = new HashSet<string>(referenceTypeSubjectIds, StringComparer.Ordinal);
		_allEmittedTypeSubjectIds.UnionWith(interfaceTypeSubjectIds);
		_allEmittedTypeSubjectIds.UnionWith(valueTypeSubjectIds);
		_allEmittedTypeSubjectIds.UnionWith(hashSet3);
		void TrackAbiSlotCarrier(AotCoreIrAbiSlotArtifact abiSlot)
		{
			if (abiSlot.CarrierKindCode == AotCoreIrAbiCarrierKind.ValueTypeByValue && !string.IsNullOrEmpty(abiSlot.TypeSubjectId))
			{
				valueTypeSubjectIds.Add(abiSlot.TypeSubjectId);
			}
		}
		void TrackCarrierType(string subjectId, AotCoreIrTypeShapeKind typeShape, string? baseTypeSubjectId = null, IReadOnlyList<string>? implementedInterfaceSubjectIds = null)
		{
			if (!string.IsNullOrEmpty(subjectId))
			{
				switch (typeShape)
				{
				case AotCoreIrTypeShapeKind.ValueType:
					valueTypeSubjectIds.Add(subjectId);
					break;
				case AotCoreIrTypeShapeKind.InterfaceType:
					TrackInterfaceType(subjectId);
					break;
				case AotCoreIrTypeShapeKind.ReferenceType:
					TrackReferenceType(subjectId, baseTypeSubjectId, implementedInterfaceSubjectIds);
					break;
				}
			}
		}
		void TrackInterfaceType(string subjectId)
		{
			interfaceTypeSubjectIds.Add(subjectId);
		}
		void TrackReferenceType(string subjectId, string? baseTypeSubjectId, IReadOnlyList<string>? implementedInterfaceSubjectIds = null)
		{
			baseTypeSubjectId = ResolveReferenceTypeBaseSubjectId(subjectId, baseTypeSubjectId);
			if (implementedInterfaceSubjectIds == null && _referenceTypeImplementedInterfaceSubjectIds.TryGetValue(subjectId, out HashSet<string>? value8) && value8.Count > 0)
			{
				implementedInterfaceSubjectIds = value8.OrderBy<string, string>((string result) => result, StringComparer.Ordinal).ToArray();
			}
			referenceTypeSubjectIds.Add(subjectId);
			if (!referenceTypeBaseSubjectIds.ContainsKey(subjectId))
			{
				referenceTypeBaseSubjectIds[subjectId] = null;
			}
			if (!referenceTypeImplementedInterfaceSubjectIds.ContainsKey(subjectId))
			{
				referenceTypeImplementedInterfaceSubjectIds[subjectId] = new HashSet<string>(StringComparer.Ordinal);
			}
			if (!string.IsNullOrEmpty(baseTypeSubjectId))
			{
				referenceTypeBaseSubjectIds[subjectId] = baseTypeSubjectId;
				referenceTypeSubjectIds.Add(baseTypeSubjectId);
				if (!referenceTypeBaseSubjectIds.ContainsKey(baseTypeSubjectId))
				{
					referenceTypeBaseSubjectIds[baseTypeSubjectId] = null;
				}
				if (!referenceTypeImplementedInterfaceSubjectIds.ContainsKey(baseTypeSubjectId))
				{
					referenceTypeImplementedInterfaceSubjectIds[baseTypeSubjectId] = new HashSet<string>(StringComparer.Ordinal);
				}
				TrackReferenceType(baseTypeSubjectId, null);
			}
			if (implementedInterfaceSubjectIds == null)
			{
				return;
			}
			foreach (string implementedInterfaceSubjectId in implementedInterfaceSubjectIds)
			{
				referenceTypeImplementedInterfaceSubjectIds[subjectId].Add(implementedInterfaceSubjectId);
				TrackInterfaceType(implementedInterfaceSubjectId);
			}
			_interfaceTypeSubjectIds = interfaceTypeSubjectIds;
		}
	}

	private static IEnumerable<string> TopologicalSortReferenceTypes(
		HashSet<string> types,
		Dictionary<string, string?> baseTypes)
	{
		var visited = new HashSet<string>();
		var result = new List<string>();

		void Dfs(string type)
		{
			if (visited.Contains(type)) return;
			visited.Add(type);

			// Visit parent first (ensures parents emitted before children)
			if (baseTypes.TryGetValue(type, out var parent) &&
				!string.IsNullOrEmpty(parent) &&
				types.Contains(parent))
			{
				Dfs(parent);
			}

			result.Add(type);
		}

		// Iterate in deterministic order via sorted initial sequence
		foreach (var type in types.OrderBy(x => x, StringComparer.Ordinal))
		{
			Dfs(type);
		}

		return result;
	}

		// ── Struct marshalling descriptor emission ──────────────────────────────

		private static int GetStructFieldKindValue(string kind)
		{
			return kind switch
			{
				"Blittable" => 0,
				"BoolField" => 1,
				"StringField" => 2,
				"NestedStruct" => 3,
				"ByValArray" => 4,
				"LPArray" => 5,
				"DecimalField" => 6,
				"DateTimeField" => 7,
				"ObjectField" => 8,
				"GuidField" => 9,
				_ => 0,
			};
		}

		private static int GetNativeElementTypeValue(string? elementType)
		{
			return elementType switch
			{
				"None" or null => 0,
				"U1" => 1,
				"I1" => 2,
				"U2" => 3,
				"I2" => 4,
				"U4" => 5,
				"I4" => 6,
				"U8" => 7,
				"I8" => 8,
				"R4" => 9,
				"R8" => 10,
				"Struct" => 11,
				_ => 0,
			};
		}

		private static string MapFieldTypeToCppType(string? fieldTypeSubjectId)
		{
			return fieldTypeSubjectId switch
			{
				"System.Boolean" => "CHAOS_IL2CPP_UINT8",
				"System.Byte" => "CHAOS_IL2CPP_UINT8",
				"System.SByte" => "CHAOS_IL2CPP_INT8",
				"System.Int16" => "CHAOS_IL2CPP_INT16",
				"System.UInt16" => "CHAOS_IL2CPP_UINT16",
				"System.Char" => "CHAOS_IL2CPP_UINT16",
				"System.Int32" => "CHAOS_IL2CPP_INT32",
				"System.UInt32" => "CHAOS_IL2CPP_UINT32",
				"System.Int64" => "CHAOS_IL2CPP_INT64",
				"System.UInt64" => "CHAOS_IL2CPP_UINT64",
				"System.Single" => "CHAOS_IL2CPP_FLOAT32",
				"System.Double" => "CHAOS_IL2CPP_FLOAT64",
				"System.IntPtr" => "CHAOS_IL2CPP_INTPTR",
				"System.UIntPtr" => "CHAOS_IL2CPP_UINTPTR",
				null => "CHAOS_IL2CPP_INTPTR",
				_ => "CHAOS_IL2CPP_INTPTR",
			};
		}

		private void EmitStructMarshallingDescriptors(
			StringBuilder builder,
			IReadOnlyList<StructMarshallingDescriptorArtifact>? descriptors)
		{
			if (descriptors == null || descriptors.Count == 0)
				return;

			builder.AppendLine();
			builder.AppendLine("// ── Struct marshalling descriptors (codegen static) ──");
			builder.AppendLine();

			// Build a lookup for nested type references
			var descriptorByTypeId = descriptors.ToDictionary(d => d.TypeSubjectId, StringComparer.Ordinal);

			// Emit each descriptor
			foreach (var desc in descriptors)
			{
				string safeName = SanitizeSubjectId(desc.TypeSubjectId);
				string descSymbol = GetNativeStructMarshallingDescriptorSymbol(desc.TypeSubjectId);
				string fieldArraySymbol = "s_marshal_fields_" + safeName;

				// Emit the field descriptor array as a static constexpr array
				builder.Append("static constexpr StructFieldDescriptorV1 ");
				builder.Append(fieldArraySymbol);
				builder.AppendLine("[] =");
				builder.AppendLine("{");

				for (int i = 0; i < desc.Fields.Count; i++)
				{
					var f = desc.Fields[i];

					// Resolve nested descriptor pointer expression
					string nestedPtrExpr;
					if (f.Kind == "NestedStruct" && f.NestedTypeSubjectId != null
						&& descriptorByTypeId.ContainsKey(f.NestedTypeSubjectId))
					{
						nestedPtrExpr = "&" + GetNativeStructMarshallingDescriptorSymbol(f.NestedTypeSubjectId);
					}
					else
					{
						nestedPtrExpr = "nullptr";
					}

					int kindVal = GetStructFieldKindValue(f.Kind);
					int elemTypeVal = GetNativeElementTypeValue(f.ElementType);

					builder.Append("    { static_cast<StructFieldKind>(");
					builder.Append(kindVal.ToString());
					builder.Append("), ");
					builder.Append(f.Offset.ToString());
					builder.Append(", ");
					builder.Append(f.Size.ToString());
					builder.Append(", ");
					builder.Append(f.ArrayCount.ToString());
					builder.Append(", static_cast<NativeElementType>(");
					builder.Append(elemTypeVal.ToString());
					builder.Append("), 0, ");
					builder.Append(nestedPtrExpr);
					builder.Append(" }");

					if (i < desc.Fields.Count - 1)
						builder.AppendLine(",");
					else
						builder.AppendLine();
				}

				builder.AppendLine("};");

				// Emit the StructMarshallingDescriptorV1 instance
				builder.Append("constinit StructMarshallingDescriptorV1 ");
				builder.Append(descSymbol);
				builder.AppendLine(" =");
				builder.AppendLine("{");
				builder.Append("    ");
				builder.Append(desc.TotalSize.ToString());
				builder.AppendLine(",");
				builder.Append("    ");
				builder.Append(desc.Fields.Count.ToString());
				builder.AppendLine(",");
				builder.Append("    "); // fields[] flexible array initialization
				builder.Append(fieldArraySymbol);
				builder.AppendLine("[0]");
				builder.AppendLine("};");

				// Emit static registration (same pattern as VTable registration)
				ulong stableId = ComputeStableTypeId(desc.TypeSubjectId);
				builder.Append("static const int s_ms_reg_");
				builder.Append(safeName);
				builder.Append(" = (::chaos::il2cpp::runtime_core::RegisterStaticMarshallingDescriptor(");
				builder.Append("CHAOS_IL2CPP_UINT64_C(");
				builder.Append(stableId.ToString());
				builder.Append("), &");
				builder.Append(descSymbol);
				builder.AppendLine("), 0);");

				builder.AppendLine();
			}
		}

}
