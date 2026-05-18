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
using Scriban.Runtime;

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
		IReadOnlyList<ExternalRuntimeHelperDefinition> externalRuntimeHelpers,
		MetadataRegistrationArtifact metadataRegistration)
	{
		HashSet<string> referenceTypeSubjectIds = new HashSet<string>(StringComparer.Ordinal);
		Dictionary<string, string?> referenceTypeBaseSubjectIds = new Dictionary<string, string?>(StringComparer.Ordinal);
		HashSet<string> interfaceTypeSubjectIds = new HashSet<string>(StringComparer.Ordinal);
		Dictionary<string, HashSet<string>> referenceTypeImplementedInterfaceSubjectIds = new Dictionary<string, HashSet<string>>(StringComparer.Ordinal);
		HashSet<string> valueTypeSubjectIds = new HashSet<string>(StringComparer.Ordinal);
		Dictionary<string, string?> hashSet = new Dictionary<string, string?>(StringComparer.Ordinal);
		Dictionary<string, string?> hashSet2 = new Dictionary<string, string?>(StringComparer.Ordinal);
		HashSet<string> hashSet3 = new HashSet<string>(StringComparer.Ordinal);
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
					hashSet2[referencedStaticFieldSubjectId] = null;
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
				case "add.ovf.un":
				case "sub.ovf.un":
				case "mul.ovf.un":
				case "conv.ovf.i1":
				case "conv.ovf.u1":
				case "conv.ovf.i2":
				case "conv.ovf.u2":
				case "conv.ovf.i4":
				case "conv.ovf.u4":
				case "conv.ovf.i8":
				case "conv.ovf.u8":
				case "conv.ovf.i":
				case "conv.ovf.u":
				case "conv.ovf.i8.un":
				case "conv.ovf.u8.un":
				case "conv.ovf.i.un":
				case "conv.ovf.u.un":
				case "conv.ovf.i1.un":
				case "conv.ovf.i2.un":
				case "conv.ovf.i4.un":
				case "conv.ovf.u1.un":
				case "conv.ovf.u2.un":
				case "conv.ovf.u4.un":
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
					hashSet2[targetReference.SubjectId] = targetReference.FieldTypeSubjectId;
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
			hashSet[additionalInstanceFieldSubjectId] = null;
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
			bool hasConstructor = false;
			bool hasMethod = false;
			foreach (var entry in _reflectionMemberSupport.MethodEntries)
			{
				if (entry.IsConstructor) hasConstructor = true;
				else hasMethod = true;
			}
			if (hasConstructor)
			{
				TrackReferenceType("System.Private.CoreLib/System.Reflection.ConstructorInfo", "System.Private.CoreLib/System.Object");
			}
			if (hasMethod)
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
		// Ensure closed generic reference types with virtual methods are tracked
		// for vtable emission. These types may not appear in instruction scanning
		// but still need VTableSlot[] + VTableDescriptorV0 emission.
		foreach (var method in _methodsBySubjectId.Values)
		{
			if (method.IsStatic) continue;
			var dt = method.Identity.DeclaringTypeSubjectId;
			if (string.IsNullOrEmpty(dt)) continue;
			if (referenceTypeSubjectIds.Contains(dt)) continue;
			if (valueTypeSubjectIds.Contains(dt)) continue;
			if (interfaceTypeSubjectIds.Contains(dt)) continue;
			if (!dt.Contains("[[")) continue;
			TrackReferenceType(dt, null);
		}
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
			var sortedReferenceTypes = TopologicalSortReferenceTypes(referenceTypeSubjectIds, referenceTypeBaseSubjectIds).ToArray();
			foreach (string typeId in sortedReferenceTypes)
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
			foreach (string typeId in sortedReferenceTypes)
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
		// ── Forward-declare vtable arrays so TypeInfoHot.vtable_array can reference them ──
			if (referenceTypeSubjectIds.Count > 0)
			{
				foreach (string typeId in sortedReferenceTypes)
					{
						if (!_vtableLengths.TryGetValue(typeId, out int vtLen) || vtLen == 0) continue;
					builder.Append("const void* ");
					builder.Append(GetNativeVTableSymbol(typeId));
					builder.AppendLine("[];");
				}
			}
			// ── TypeInfo instances (replace integer type_id system) ──
		foreach (string item in sortedReferenceTypes)
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
				// Determine header flags from decision engine
				HeaderKind hdrKind = GetHeaderKind(item);
				byte flags = (byte)hdrKind; // PureType=0, ThinLockable=1, Fat=2
					if (TypeHasFinalizer(item))
					    flags |= 0x04; // kTypeInfoHasFinalizer
				_vtableLengths.TryGetValue(item, out int vtLen);
				// TypeInfoV0 = hot(32B) + warm(32B) nested initializer
				handler.AppendLiteral("inline TypeInfoV0 ");
				handler.AppendFormatted(GetNativeMethodTableSymbol(item));
				handler.AppendLiteral(" = {{");
				handler.AppendFormatted(parentExpr);
				handler.AppendLiteral(", ");
				string vtableArrayExpr = (vtLen > 0) ? GetNativeVTableSymbol(item) : "nullptr";
				handler.AppendFormatted(vtableArrayExpr);
				handler.AppendLiteral(", ");
				handler.AppendFormatted(stableId.ToString() + "ULL");
				handler.AppendLiteral(", ");
				handler.AppendFormatted(vtLen.ToString());
				handler.AppendLiteral("u, 32, 1, ");
				handler.AppendFormatted(flags.ToString());
				handler.AppendLiteral("}, {");
				handler.AppendFormatted(ifaceMapExpr);
				handler.AppendLiteral(", nullptr, ");
				handler.AppendFormatted(ifaceCountExpr);
				handler.AppendLiteral(", 0, 0, 0}};");
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
			}
			num++;
		}
		foreach (string item2 in interfaceTypeSubjectIds.OrderBy<string, string>((string result) => result, StringComparer.Ordinal))
		{
			ulong stableId = ComputeStableTypeId(item2);
			{
				StringBuilder stringBuilder = builder;
				StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(28, 2, stringBuilder);
				handler.AppendLiteral("inline TypeInfoV0 ");
				handler.AppendFormatted(GetNativeMethodTableSymbol(item2));
				handler.AppendLiteral(" = {{nullptr, nullptr, ");
				handler.AppendFormatted(stableId.ToString() + "ULL");
				handler.AppendLiteral(", 0u, 32, 3, 0}, {nullptr, nullptr, 0, 0, 0, 0}};");
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
				handler.AppendLiteral("inline TypeInfoV0 ");
				handler.AppendFormatted(GetNativeMethodTableSymbol(item3));
				handler.AppendLiteral(" = {{nullptr, nullptr, ");
				handler.AppendFormatted(stableId.ToString() + "ULL");
				handler.AppendLiteral(", 0u, 32, 2, 0}, {nullptr, nullptr, 0, 0, 0, 0}};");
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
				handler.AppendLiteral("inline TypeInfoV0 ");
				handler.AppendFormatted(GetNativeMethodTableSymbol(item3));
				handler.AppendLiteral(" = {{nullptr, nullptr, ");
				handler.AppendFormatted(stableId.ToString() + "ULL");
				handler.AppendLiteral(", 0u, 32, 2, 0}, {");
				handler.AppendFormatted(ifaceMapExpr);
				handler.AppendLiteral(", nullptr, ");
				handler.AppendFormatted(ifaceCountExpr);
				handler.AppendLiteral(", 0, 0, 0}};");
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
			foreach (string typeId in sortedReferenceTypes)
			{
				if (!_vtableLengths.TryGetValue(typeId, out int vtLen) || vtLen == 0) continue;
				var entries = new AotCoreIrMethodArtifact?[vtLen];
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
								entries[slot] = method;
							}
						}
					}
					referenceTypeBaseSubjectIds.TryGetValue(current, out string? nextCurrent);
					current = nextCurrent;
				}
				// Emit extern "C" declarations for methods referenced in vtable array
				var externDeclared = new HashSet<string>(StringComparer.Ordinal);
				foreach (var entry in entries)
				{
					if (entry is null || !externDeclared.Add(entry.NativeSymbol)) continue;
					builder.AppendLine(FormatMethodDeclaration(entry));
					var stub = TryGetInstantiationStubSymbol(entry);
					if (stub != null && externDeclared.Add(stub))
					{
						builder.AppendLine(FormatMethodDeclaration(stub, entry.ReturnAbi, GetMethodAbiParameterSlots(entry)));
					}
				}
				// Emit vtable array
				StringBuilder stringBuilder = builder;
				StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(24, 1, stringBuilder);
				handler.AppendLiteral("const void* ");
				handler.AppendFormatted(GetNativeVTableSymbol(typeId));
				handler.AppendLiteral("[] =");
				stringBuilder.AppendLine(ref handler);
				builder.AppendLine("{");
				foreach (var entry in entries)
				{
					if (entry != null)
					{
						builder.Append("    reinterpret_cast<void*>(");
						builder.Append(TryGetInstantiationStubSymbol(entry) ?? entry.NativeSymbol);
						builder.AppendLine("),");
					}
					else
					{
						builder.AppendLine("    nullptr,");
					}
				}
				builder.AppendLine("};");

			}
		}
					// ---- VTableSlot arrays (for BootstrapRuntime TypeVTable registration) ----
			var tokenLookup = new MetadataTokenLookup(metadataRegistration.Registrations);
			var vtableDescriptors = new List<VTableDescriptorData>();
			foreach (string typeId in sortedReferenceTypes)
			{
				if (!_vtableLengths.TryGetValue(typeId, out int vtLen) || vtLen == 0) continue;
				var entries = new AotCoreIrMethodArtifact?[vtLen];
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
								entries[slot] = method;
							}
						}
					}
					referenceTypeBaseSubjectIds.TryGetValue(current, out string? nextCurrent);
					current = nextCurrent;
				}
				// Build VTableSlot entries
				var slotEntries = new List<VTableSlotEntry>();
				for (int i = 0; i < vtLen; i++)
				{
					var entry = entries[i];
					if (entry != null)
					{
						uint methodToken = tokenLookup.TryGetMethodToken(entry.SubjectId);
						if (methodToken == 0 && entry.Identity != null)
						{
							var fakeToken = (uint)(0x06000000u | (uint)(entry.SubjectId.GetHashCode() & 0x00FFFFFF));
							methodToken = fakeToken;
						}
						string nativeSym = TryGetInstantiationStubSymbol(entry) ?? entry.NativeSymbol;
						slotEntries.Add(new VTableSlotEntry(methodToken, nativeSym));
					}
					else
					{
						slotEntries.Add(new VTableSlotEntry(0, string.Empty));
					}
				}
				// Emit const VTableSlot[] array
				string slotsSym = GetNativeSymbol("kSlots_", typeId);
				builder.Append("static const ::chaos::il2cpp::vtable_registry::VTableSlot ");
				builder.Append(slotsSym);
				builder.AppendLine("[] =");
				builder.AppendLine("{");
				foreach (var se in slotEntries)
				{
					if (se.MethodToken != 0)
					{
						builder.Append("    { 0x");
						builder.Append(se.MethodToken.ToString("X8"));
						builder.Append("u, reinterpret_cast<void*>(&");
						builder.Append(se.NativeSymbol);
						builder.AppendLine(") },");
					}
					else
					{
						builder.AppendLine("    { 0u, nullptr },");
					}
				}
				builder.AppendLine("};");
				builder.AppendLine();
				// Build VTableDescriptorData for later emission
				ulong stableId = ComputeStableTypeId(typeId);
				uint typeToken = tokenLookup.TryGetTypeToken(typeId);
				string typeTokenLit = typeToken != 0
					? string.Format("0x{0:X8}u", typeToken)
					: string.Format("0x{0:X8}u", 0x02000000u | ((uint)typeId.GetHashCode() & 0x00FFFFFFu));
				string baseTokenLit = "0u";
				if (referenceTypeBaseSubjectIds.TryGetValue(typeId, out string? baseId) && baseId != null)
				{
					uint baseToken = tokenLookup.TryGetTypeToken(baseId);
					baseTokenLit = baseToken != 0
						? string.Format("0x{0:X8}u", baseToken)
						: "0u";
				}
				string ifaceMapSym = null;
				int ifaceCount = 0;
				if (referenceTypeImplementedInterfaceSubjectIds.TryGetValue(typeId, out var ifaces) && ifaces.Count > 0)
				{
					ifaceMapSym = GetNativeIfaceMapSymbol(typeId);
					ifaceCount = ifaces.Count;
				}
				byte typeShape = (byte)(interfaceTypeSubjectIds.Contains(typeId) ? 3 : 1);
				vtableDescriptors.Add(new VTableDescriptorData(
					stableId, typeTokenLit, baseTokenLit,
					slotEntries.ToArray(),
					GetNativeVTableSymbol(typeId),
					vtLen, typeShape,
					ifaceMapSym, ifaceCount,
					SanitizeSubjectId(typeId)));
			}
			_vtableDescriptors = vtableDescriptors;
			
builder.AppendLine("bool chaos_is_array_store_compatible(const chaos_managed_array* chaos_array, CHAOS_IL2CPP_INTPTR chaos_value) noexcept");
		builder.AppendLine("{");
		builder.AppendLine("    if (chaos_array == nullptr)");
		builder.AppendLine("    {");
		builder.AppendLine("        return false;");
		builder.AppendLine("    }");
		builder.AppendLine();
		builder.AppendLine("    if (chaos_value == 0)");
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
		builder.AppendLine("    auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_value);");
		builder.AppendLine("    if (chaos_array->element_type_shape == chaos_type_shape_interface)");
		builder.AppendLine("    {");
		builder.AppendLine("        return chaos_does_type_implement_interface(chaos_object_get_type_info(chaos_header), chaos_array->element_type_info);");
		builder.AppendLine("    }");
		builder.AppendLine();
		builder.AppendLine("    if (chaos_array->element_type_shape == chaos_type_shape_reference)");
		builder.AppendLine("    {");
		builder.AppendLine("        return chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), chaos_array->element_type_info);");
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
				builder.AppendLine("    PureTypeHeader header{};");
				builder.AppendLine("};");
				builder.AppendLine();
				continue;
			}
			// Always use C++ inheritance. The unified ThinLockableHeader (16B)
			// has no vtable field, so no header shadowing can occur.
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
			// Emit header always for types without base. Types with base inherit
			// it through C++ inheritance.
			bool _hasBase = referenceTypeBaseSubjectIds.TryGetValue(typeSubjectId, out string? value7) && !string.IsNullOrWhiteSpace(value7) && referenceTypeSubjectIds.Contains(value7);
			if (!_hasBase)
			{
				HeaderKind hdrKind = GetHeaderKind(typeSubjectId);
				string headerType = hdrKind switch
				{
				    HeaderKind.PureType => "PureTypeHeader",
				    HeaderKind.ThinLockable => "ThinLockableHeader",
				    _ => "ThinLockableHeader"
				};
				builder.AppendLine($"    {headerType} header{{}};");

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
			// System.Exception instance fields — the runtime accesses these by hardcoded
			// offset (see exception_api.cpp kException*Offset), so the C++ struct must
			// have exactly the layout the runtime expects: header(16) + _message(8) +
			// _innerException(8) + _stackTrace(8) + _HResult(4) = 44 bytes (padded to 48).
			// All Exception-derived types need these fields since runtime helpers operate
			// on the base Exception layout at fixed offsets from the header.
			bool isExceptionOrDerived =
				ns.StartsWith("System.Private.CoreLib/", StringComparison.Ordinal) &&
				ns.EndsWith("Exception", StringComparison.Ordinal);
			if (isExceptionOrDerived){
				builder.AppendLine("    CHAOS_IL2CPP_INTPTR _message = 0;");
				builder.AppendLine("    CHAOS_IL2CPP_INTPTR _innerException = 0;");
				builder.AppendLine("    CHAOS_IL2CPP_INTPTR _stackTrace = 0;");
				builder.AppendLine("    CHAOS_IL2CPP_INT32 _HResult = 0;");
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
			builder.AppendLine("    PureTypeHeader header{};");
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

			// Build a lookup for nested type references
			var descriptorByTypeId = descriptors.ToDictionary(d => d.TypeSubjectId, StringComparer.Ordinal);

			// Build descriptor models
			var descriptorModels = descriptors.Select(desc =>
			{
				string safeName = SanitizeSubjectId(desc.TypeSubjectId);
				string descSymbol = GetNativeStructMarshallingDescriptorSymbol(desc.TypeSubjectId);
				string fieldArraySymbol = "s_marshal_fields_" + safeName;
				ulong stableId = ComputeStableTypeId(desc.TypeSubjectId);

				var fieldModels = desc.Fields.Select(f =>
				{
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

					return new ScriptObject
					{
						["kind_value"] = GetStructFieldKindValue(f.Kind),
						["offset"] = f.Offset,
						["size"] = f.Size,
						["array_count"] = f.ArrayCount,
						["element_type_value"] = GetNativeElementTypeValue(f.ElementType),
						["nested_ptr_expr"] = nestedPtrExpr,
						["field_name"] = f.Name ?? "",
					};
				}).ToArray();

				return new ScriptObject
				{
					["type_subject_id"] = desc.TypeSubjectId,
					["safe_name"] = safeName,
					["symbol"] = descSymbol,
					["field_array_symbol"] = fieldArraySymbol,
					["field_names_symbol"] = GetNativeStructFieldNamesSymbol(desc.TypeSubjectId),
					["total_size"] = desc.TotalSize,
					["field_count"] = desc.Fields.Count,
					["stable_id"] = stableId.ToString(),
					["fields"] = fieldModels,
				};
			}).ToArray();

			var model = new ScriptObject
			{
				["descriptors"] = descriptorModels,
			};

			var result = ScribanTemplateRenderer.RenderTemplate(
				NativeAotTemplateCatalog.GetStructMarshallingDescriptorsTemplate(), model);
			builder.AppendLine(result);
		}



	private bool TypeHasFinalizer(string typeSubjectId)
	{
		// System.Object has a default (empty) Finalize — not a real finalizer.
		if (typeSubjectId.Contains("/System.Object"))
			return false;
		return _methodsBySubjectId.Values.Any(m =>
			!m.IsStatic &&
			string.Equals(GetMethodName(m.SubjectId), "Finalize", StringComparison.Ordinal) &&
			string.Equals(m.Identity.DeclaringTypeSubjectId, typeSubjectId, StringComparison.Ordinal));
	}
}
