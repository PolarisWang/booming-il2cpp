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
	private void EmitObjectModelDeclarations(
		StringBuilder builder,
		IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods,
		IReadOnlyList<ExternalRuntimeHelperDefinition> externalRuntimeHelpers)
	{
		HashSet<string> referenceTypeSubjectIds = new HashSet<string>(StringComparer.Ordinal);
		Dictionary<string, string?> referenceTypeBaseSubjectIds = new Dictionary<string, string>(StringComparer.Ordinal);
		HashSet<string> interfaceTypeSubjectIds = new HashSet<string>(StringComparer.Ordinal);
		Dictionary<string, HashSet<string>> referenceTypeImplementedInterfaceSubjectIds = new Dictionary<string, HashSet<string>>(StringComparer.Ordinal);
		HashSet<string> valueTypeSubjectIds = new HashSet<string>(StringComparer.Ordinal);
		HashSet<string> hashSet = new HashSet<string>(StringComparer.Ordinal);
		HashSet<string> hashSet2 = new HashSet<string>(StringComparer.Ordinal);
		HashSet<string> hashSet3 = new HashSet<string>(StringComparer.Ordinal);
		builder.AppendLine("#include <chaos/type_info.h>");
		builder.AppendLine();
		builder.AppendLine("struct chaos_object_header");
		builder.AppendLine("{");
		builder.AppendLine("    const void** vtable = nullptr;");
		builder.AppendLine("    const TypeInfo* type_info = nullptr;");
		builder.AppendLine("};");
		builder.AppendLine();
		builder.AppendLine("constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_managed_array = 1;");
		builder.AppendLine("inline constexpr TypeInfo chaos_type_info_managed_array = { nullptr, 1ULL, 2 };");
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
					hashSet2.Add(referencedStaticFieldSubjectId);
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
				AotCoreIrReferenceArtifact targetReference = instruction.TargetReference;
				if ((object)targetReference == null)
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
						TrackCarrierType(targetReference.ArrayElementSubjectId, targetReference.ArrayElementTypeShape, targetReference.ArrayElementBaseTypeSubjectId, targetReference.ArrayElementImplementedInterfaceSubjectIds);
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
					hashSet2.Add(targetReference.SubjectId);
				}
				else
				{
					hashSet.Add(targetReference.SubjectId);
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
			hashSet.Add(additionalInstanceFieldSubjectId);
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
		foreach (var field in hashSet)
		{
			var declaringType = GetDeclaringTypeSubjectId(field);
			if (!fieldsByDeclaringType.TryGetValue(declaringType, out var list))
				fieldsByDeclaringType[declaringType] = list = new List<string>();
			list.Add(field);
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
		// ── TypeInfo instances (replace integer type_id system) ──
		foreach (string item in TopologicalSortReferenceTypes(referenceTypeSubjectIds, referenceTypeBaseSubjectIds))
		{
			ulong stableId = ComputeStableTypeId(item);
			string parentExpr = "nullptr";
			if (referenceTypeBaseSubjectIds.TryGetValue(item, out string? baseTypeId) && !string.IsNullOrEmpty(baseTypeId))
			{
				parentExpr = "&" + GetNativeTypeInfoSymbol(baseTypeId);
			}
			{
				StringBuilder stringBuilder = builder;
				StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(28, 2, stringBuilder);
				handler.AppendLiteral("inline constexpr TypeInfo ");
				handler.AppendFormatted(GetNativeTypeInfoSymbol(item));
				handler.AppendLiteral(" = { ");
				handler.AppendFormatted(parentExpr);
				handler.AppendLiteral(", ");
				handler.AppendFormatted(stableId.ToString() + "ULL");
				handler.AppendLiteral(", 1 /* reference */ };");
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
				handler.AppendLiteral("inline constexpr TypeInfo ");
				handler.AppendFormatted(GetNativeTypeInfoSymbol(item2));
				handler.AppendLiteral(" = { nullptr, ");
				handler.AppendFormatted(stableId.ToString() + "ULL");
				handler.AppendLiteral(", 3 /* interface */ };");
				stringBuilder.AppendLine(ref handler);
			}
			{
				StringBuilder stringBuilder = builder;
				StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(28, 2, stringBuilder);
				handler.AppendLiteral("inline constexpr CHAOS_IL2CPP_INTPTR ");
				handler.AppendFormatted(GetNativeTypeIdSymbol(item2));
				handler.AppendLiteral(" = static_cast<CHAOS_IL2CPP_INTPTR>(");
				handler.AppendFormatted(stableId.ToString() + "ULL");
				handler.AppendLiteral(");");
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
				handler.AppendLiteral("inline constexpr TypeInfo ");
				handler.AppendFormatted(GetNativeTypeInfoSymbol(item3));
				handler.AppendLiteral(" = { nullptr, ");
				handler.AppendFormatted(stableId.ToString() + "ULL");
				handler.AppendLiteral(", 2 /* value */ };");
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
			{
				StringBuilder stringBuilder = builder;
				StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(28, 2, stringBuilder);
				handler.AppendLiteral("inline constexpr TypeInfo ");
				handler.AppendFormatted(GetNativeTypeInfoSymbol(item3));
				handler.AppendLiteral(" = { nullptr, ");
				handler.AppendFormatted(stableId.ToString() + "ULL");
				handler.AppendLiteral(", 2 /* value (boxed) */ };");
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
		// ── VTable slot allocation ──
		var slotMap = new Dictionary<string, int>(StringComparer.Ordinal);
		var vtableLengths = new Dictionary<string, int>(StringComparer.Ordinal);
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
					if (method.IsStatic || !CanEmitMethodBody(method)) continue;
					var sig = GetMethodSignatureSuffix(method.SubjectId);
					if (!slotMap.ContainsKey(sig))
					{
						slotMap[sig] = nextSlot++;
					}
				}
			}
			vtableLengths[typeId] = nextSlot;
		}
		// ── VTable arrays ──
		if (referenceTypeSubjectIds.Count > 0)
		{
			builder.AppendLine("// ── Virtual method table arrays ──");
			foreach (string typeId in TopologicalSortReferenceTypes(referenceTypeSubjectIds, referenceTypeBaseSubjectIds))
			{
				if (!vtableLengths.TryGetValue(typeId, out int vtLen) || vtLen == 0) continue;
				var entries = new string[vtLen];
				// Walk hierarchy to fill entries (most derived first)
				string current = typeId;
				while (current != null && referenceTypeSubjectIds.Contains(current))
				{
					if (methodsByDeclaringTypeVT.TryGetValue(current, out var typeMethods))
					{
						foreach (var method in typeMethods)
						{
							if (method.IsStatic || !CanEmitMethodBody(method)) continue;
							var sig = GetMethodSignatureSuffix(method.SubjectId);
							if (slotMap.TryGetValue(sig, out int slot) && slot < vtLen && entries[slot] == null)
							{
								entries[slot] = TryGetInstantiationStubSymbol(method) ?? method.NativeSymbol;
							}
						}
					}
					referenceTypeBaseSubjectIds.TryGetValue(current, out current);
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
				builder.AppendLine();
			}
		}
		var slotMap = new Dictionary<string, int>(StringComparer.Ordinal);
		var vtableLengths = new Dictionary<string, int>(StringComparer.Ordinal);
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
					if (method.IsStatic || !CanEmitMethodBody(method)) continue;
					var sig = GetMethodSignatureSuffix(method.SubjectId);
					if (!slotMap.ContainsKey(sig))
					{
						slotMap[sig] = nextSlot++;
					}
				}
			}
			vtableLengths[typeId] = nextSlot;
		}
		// ── VTable arrays ──
		if (referenceTypeSubjectIds.Count > 0)
		{
			builder.AppendLine("// ── Virtual method table arrays ──");
			foreach (string typeId in TopologicalSortReferenceTypes(referenceTypeSubjectIds, referenceTypeBaseSubjectIds))
			{
				if (!vtableLengths.TryGetValue(typeId, out int vtLen) || vtLen == 0) continue;
				var entries = new string?[vtLen];
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
							if (slotMap.TryGetValue(sig, out int slot) && slot < vtLen && entries[slot] == null)
							{
								entries[slot] = TryGetInstantiationStubSymbol(method) ?? method.NativeSymbol;
							}
						}
					}
					referenceTypeBaseSubjectIds.TryGetValue(current, out current);
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
		builder.AppendLine("        if (chaos_current == chaos_target_type_info)");
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
		// ── Interface check (switch on stable_id) ──
		builder.AppendLine("bool chaos_type_implements_interface(const TypeInfo* chaos_actual_type_info, const TypeInfo* chaos_target_interface_type_info) noexcept");
		builder.AppendLine("{");
		builder.AppendLine("    switch (chaos_actual_type_info->stable_id)");
		builder.AppendLine("    {");
		foreach (string item6 in TopologicalSortReferenceTypes(referenceTypeSubjectIds, referenceTypeBaseSubjectIds))
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder9 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(14, 1, stringBuilder);
			handler.AppendLiteral("        case ");
			handler.AppendFormatted(GetNativeTypeIdSymbol(item6));
			handler.AppendLiteral(":");
			stringBuilder9.AppendLine(ref handler);
			if (referenceTypeImplementedInterfaceSubjectIds.TryGetValue(item6, out HashSet<string> value2) && value2.Count > 0)
			{
				string value3 = string.Join(" || ", from interfaceSubjectId in value2.OrderBy<string, string>((string result) => result, StringComparer.Ordinal)
					select "chaos_target_interface_type_info->stable_id == " + GetNativeTypeIdSymbol(interfaceSubjectId));
				stringBuilder = builder;
				StringBuilder stringBuilder10 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(20, 1, stringBuilder);
				handler.AppendLiteral("            return ");
				handler.AppendFormatted(value3);
				handler.AppendLiteral(";");
				stringBuilder10.AppendLine(ref handler);
			}
			else
			{
				builder.AppendLine("            return false;");
			}
		}
		foreach (string item7 in sortedHashSet3)
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder11 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(14, 1, stringBuilder);
			handler.AppendLiteral("        case ");
			handler.AppendFormatted(GetNativeBoxTypeIdSymbol(item7));
			handler.AppendLiteral(":");
			stringBuilder11.AppendLine(ref handler);
			if (referenceTypeImplementedInterfaceSubjectIds.TryGetValue(item7, out HashSet<string> value4) && value4.Count > 0)
			{
				string value5 = string.Join(" || ", from interfaceSubjectId in value4.OrderBy<string, string>((string result) => result, StringComparer.Ordinal)
					select "chaos_target_interface_type_info->stable_id == " + GetNativeTypeIdSymbol(interfaceSubjectId));
				stringBuilder = builder;
				StringBuilder stringBuilder12 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(20, 1, stringBuilder);
				handler.AppendLiteral("            return ");
				handler.AppendFormatted(value5);
				handler.AppendLiteral(";");
				stringBuilder12.AppendLine(ref handler);
			}
			else
			{
				builder.AppendLine("            return false;");
			}
		}
		builder.AppendLine("        default:");
		builder.AppendLine("            return false;");
		builder.AppendLine("    }");
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
			bool num2 = string.Equals(ns, "System.Private.CoreLib/System.String", StringComparison.Ordinal);
			bool flag3 = string.Equals(ns, "System.Private.CoreLib/System.Delegate", StringComparison.Ordinal);
			bool flag4 = string.Equals(ns, "System.Private.CoreLib/System.Type", StringComparison.Ordinal);
			bool flag5 = string.Equals(ns, "System.Private.CoreLib/System.Reflection.MethodInfo", StringComparison.Ordinal);
			bool flag6 = string.Equals(ns, "System.Private.CoreLib/System.Reflection.ConstructorInfo", StringComparison.Ordinal);
			bool flag7 = string.Equals(ns, "System.Private.CoreLib/System.Reflection.FieldInfo", StringComparison.Ordinal);
			bool flag8 = string.Equals(ns, "System.Private.CoreLib/System.Reflection.ParameterInfo", StringComparison.Ordinal);
			bool flag9 = string.Equals(ns, "System.Private.CoreLib/System.Reflection.Assembly", StringComparison.Ordinal);
			bool flag10 = string.Equals(ns, "System.Private.CoreLib/System.Reflection.AssemblyName", StringComparison.Ordinal);
			if (referenceTypeBaseSubjectIds.TryGetValue(typeSubjectId, out string value6) && !string.IsNullOrWhiteSpace(value6) && referenceTypeSubjectIds.Contains(value6))
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
			if (!referenceTypeBaseSubjectIds.TryGetValue(typeSubjectId, out string value7) || string.IsNullOrWhiteSpace(value7) || !referenceTypeSubjectIds.Contains(value7))
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
				StringBuilder stringBuilder = builder;
				StringBuilder stringBuilder15 = stringBuilder;
				StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(23, 1, stringBuilder);
				handler.AppendLiteral("    CHAOS_IL2CPP_INTPTR ");
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
				stringBuilder = builder;
				StringBuilder stringBuilder17 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(23, 1, stringBuilder);
				handler.AppendLiteral("    CHAOS_IL2CPP_INTPTR ");
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
		foreach (string item11 in hashSet2.OrderBy<string, string>((string result) => result, StringComparer.Ordinal))
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder20 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(19, 1, stringBuilder);
			handler.AppendLiteral("CHAOS_IL2CPP_INTPTR ");
			handler.AppendFormatted(GetNativeStaticFieldSymbol(item11));
			handler.AppendLiteral(" = 0;");
			stringBuilder20.AppendLine(ref handler);
		}
		if (hashSet2.Count > 0)
		{
			builder.AppendLine();
		}
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
			if (implementedInterfaceSubjectIds == null && _referenceTypeImplementedInterfaceSubjectIds.TryGetValue(subjectId, out HashSet<string> value8) && value8.Count > 0)
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

}
