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

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
	private static IReadOnlyList<string> GetReferenceTypeEmissionOrder(IReadOnlySet<string> referenceTypeSubjectIds, IReadOnlyDictionary<string, string?> referenceTypeBaseSubjectIds)
	{
		List<string> ordered = new List<string>();
		HashSet<string> visited = new HashSet<string>(StringComparer.Ordinal);
		HashSet<string> visiting = new HashSet<string>(StringComparer.Ordinal);
		foreach (string item in referenceTypeSubjectIds.OrderBy<string, string>((string value) => value, StringComparer.Ordinal))
		{
			Visit(item);
		}
		return ordered;
		void Visit(string typeSubjectId)
		{
			if (!visited.Contains(typeSubjectId))
			{
				if (!visiting.Add(typeSubjectId))
				{
					throw new InvalidOperationException("reference type inheritance cycle detected for '" + typeSubjectId + "'.");
				}
				if (referenceTypeBaseSubjectIds.TryGetValue(typeSubjectId, out string? value) && !string.IsNullOrEmpty(value) && referenceTypeSubjectIds.Contains(value))
				{
					Visit(value);
				}
				visiting.Remove(typeSubjectId);
				visited.Add(typeSubjectId);
				ordered.Add(typeSubjectId);
			}
		}
	}

	private static IReadOnlySet<string> CollectValueTypeSubjectIds(AotCoreIrArtifact aotCoreIr)
	{
		HashSet<string> hashSet = new HashSet<string>(StringComparer.Ordinal);
		foreach (AotCoreIrMethodArtifact method in aotCoreIr.Methods)
		{
			foreach (AotCoreIrInstructionArtifact instruction in method.Instructions)
			{
				AotCoreIrReferenceArtifact? targetReference = instruction.TargetReference;
				if (targetReference is not null)
				{
					if (targetReference.Kind == AotCoreIrReferenceKind.Type && targetReference.TypeShape == AotCoreIrTypeShapeKind.ValueType)
					{
						hashSet.Add(targetReference.SubjectId);
					}
					AotCoreIrReferenceKind kind = targetReference.Kind;
					bool flag = kind - 2 <= AotCoreIrReferenceKind.Type;
					if (flag && targetReference.DeclaringTypeShape == AotCoreIrTypeShapeKind.ValueType && !string.IsNullOrEmpty(targetReference.DeclaringTypeSubjectId))
					{
						hashSet.Add(targetReference.DeclaringTypeSubjectId);
					}
				}
			}
		}
		return hashSet;
	}

	private static IReadOnlyDictionary<string, string?> CollectReferenceTypeBaseSubjectIds(AotCoreIrArtifact aotCoreIr)
	{
		Dictionary<string, string?> dictionary = new Dictionary<string, string?>(StringComparer.Ordinal);
		foreach (AotCoreIrMethodArtifact method in aotCoreIr.Methods)
		{
			foreach (AotCoreIrInstructionArtifact instruction in method.Instructions)
			{
				AotCoreIrReferenceArtifact? targetReference = instruction.TargetReference;
				if (targetReference is not null)
				{
					if (targetReference.Kind == AotCoreIrReferenceKind.Type && targetReference.TypeShape == AotCoreIrTypeShapeKind.ReferenceType)
					{
						dictionary[targetReference.SubjectId] = targetReference.BaseTypeSubjectId;
					}
					if (!string.IsNullOrEmpty(targetReference.ArrayElementSubjectId) && targetReference.ArrayElementTypeShape == AotCoreIrTypeShapeKind.ReferenceType)
					{
						dictionary[targetReference.ArrayElementSubjectId] = targetReference.ArrayElementBaseTypeSubjectId;
					}
				}
			}
		}

		// Pass 2: Fill in missing base types for delegate types.
		// Delegate constructors always have signature ::.ctor:System.Void(System.Object,System.IntPtr).
		// When the AOT IR's targetReference for a delegate type lacks baseTypeSubjectId
		// (e.g., cross-assembly types like System.Threading.ThreadStart), detect it via the
		// unique callee pattern and set the missing base to MulticastDelegate.
		foreach (AotCoreIrMethodArtifact method in aotCoreIr.Methods)
		{
			foreach (AotCoreIrInstructionArtifact instruction in method.Instructions)
			{
				if (!string.IsNullOrEmpty(instruction.Callee) &&
					instruction.Callee.Contains("::.ctor:", StringComparison.Ordinal) &&
					instruction.Callee.EndsWith("(System.Object,System.IntPtr)", StringComparison.Ordinal))
				{
					string declaringType = GetMethodDeclaringTypeSubjectId(instruction.Callee);
					if (!dictionary.TryGetValue(declaringType, out var existingBase) || string.IsNullOrEmpty(existingBase))
					{
						dictionary[declaringType] = "System.Private.CoreLib/System.MulticastDelegate";
					}
				}
			}
		}

		// Pass 3: Detect delegate types via callvirt Invoke pattern.
		// Delegate types that are only referenced as callee declaring types
		// (e.g., System.Action used as Assert.Throws<Action> parameter) don't
		// have TargetReference entries in Pass 1, and their constructors may
		// not be called directly (Pass 2).  The callvirt Invoke pattern
		// uniquely identifies delegate usage — any method named "Invoke"
		// called via callvirt whose declaring type is not yet in the
		// dictionary is a delegate type.
		foreach (AotCoreIrMethodArtifact method in aotCoreIr.Methods)
		{
			foreach (AotCoreIrInstructionArtifact instruction in method.Instructions)
			{
				if (!string.IsNullOrEmpty(instruction.Callee) &&
					instruction.Op is "callvirt" and not null &&
					string.Equals(GetMethodName(instruction.Callee), "Invoke", StringComparison.Ordinal))
				{
					string declaringType = GetMethodDeclaringTypeSubjectId(instruction.Callee);
					if (!dictionary.TryGetValue(declaringType, out var existingBase) || string.IsNullOrEmpty(existingBase))
					{
						dictionary[declaringType] = "System.Private.CoreLib/System.MulticastDelegate";
					}
				}
			}
		}
		return dictionary;
	}

	private static IReadOnlyDictionary<string, HashSet<string>> CollectReferenceTypeImplementedInterfaceSubjectIds(AotCoreIrArtifact aotCoreIr)
	{
		Dictionary<string, HashSet<string>> dictionary = new Dictionary<string, HashSet<string>>(StringComparer.Ordinal);
		foreach (AotCoreIrMethodArtifact method in aotCoreIr.Methods)
		{
			foreach (AotCoreIrInstructionArtifact instruction in method.Instructions)
			{
				AotCoreIrReferenceArtifact? targetReference = instruction.TargetReference;
				if (targetReference is not null)
				{
					if (targetReference.Kind == AotCoreIrReferenceKind.Type && (targetReference.TypeShape == AotCoreIrTypeShapeKind.ReferenceType || targetReference.TypeShape == AotCoreIrTypeShapeKind.ValueType))
					{
						TrackImplementedInterfaces(dictionary, targetReference.SubjectId, targetReference.ImplementedInterfaceSubjectIds);
					}
					if (!string.IsNullOrEmpty(targetReference.ArrayElementSubjectId) && targetReference.ArrayElementTypeShape == AotCoreIrTypeShapeKind.ReferenceType)
					{
						TrackImplementedInterfaces(dictionary, targetReference.ArrayElementSubjectId, targetReference.ArrayElementImplementedInterfaceSubjectIds);
					}
				}
			}
		}
		return dictionary;
		static void TrackImplementedInterfaces(IDictionary<string, HashSet<string>> implementedInterfaceSubjectIds, string? typeSubjectId, IReadOnlyList<string>? interfaceSubjectIds)
		{
			if (string.IsNullOrEmpty(typeSubjectId) || interfaceSubjectIds == null || interfaceSubjectIds.Count == 0)
			{
				return;
			}
			if (!implementedInterfaceSubjectIds.TryGetValue(typeSubjectId, out HashSet<string>? value))
			{
				value = (implementedInterfaceSubjectIds[typeSubjectId] = new HashSet<string>(StringComparer.Ordinal));
			}
			foreach (string interfaceSubjectId in interfaceSubjectIds)
			{
				if (!string.IsNullOrEmpty(interfaceSubjectId))
				{
					value.Add(interfaceSubjectId);
				}
			}
		}
	}

	private static IReadOnlySet<string> CollectSealedTypeSubjectIds(AotCoreIrArtifact aotCoreIr)
	{
		var sealedTypes = new HashSet<string>(StringComparer.Ordinal);
		foreach (var method in aotCoreIr.Methods)
		{
			foreach (var instruction in method.Instructions)
			{
				AotCoreIrReferenceArtifact? targetReference = instruction.TargetReference;
				if (targetReference is not null)
				{
					if (targetReference.Kind == AotCoreIrReferenceKind.Type && targetReference.IsSealed)
					{
						sealedTypes.Add(targetReference.SubjectId);
					}
				}
			}
		}
		return sealedTypes;
	}

	private static IReadOnlySet<string> CollectInterfaceTypeSubjectIds(AotCoreIrArtifact aotCoreIr)
	{
		HashSet<string> hashSet = new HashSet<string>(StringComparer.Ordinal);
		foreach (AotCoreIrMethodArtifact method in aotCoreIr.Methods)
		{
			foreach (AotCoreIrInstructionArtifact instruction in method.Instructions)
			{
				AotCoreIrReferenceArtifact? targetReference = instruction.TargetReference;
				if (targetReference is not null)
				{
					if (targetReference.Kind == AotCoreIrReferenceKind.Type && targetReference.TypeShape == AotCoreIrTypeShapeKind.InterfaceType)
					{
						hashSet.Add(targetReference.SubjectId);
					}
					AotCoreIrReferenceKind kind = targetReference.Kind;
					bool flag = kind - 2 <= AotCoreIrReferenceKind.Type;
					if (flag && targetReference.DeclaringTypeShape == AotCoreIrTypeShapeKind.InterfaceType && !string.IsNullOrEmpty(targetReference.DeclaringTypeSubjectId))
					{
						hashSet.Add(targetReference.DeclaringTypeSubjectId);
					}
					// Collect interface SubjectIds from ImplementedInterfaceSubjectIds on Type references.
					// This is the primary way interface types appear in the AOT IR -- as interfaces
					// implemented by concrete reference/value types. Without this, the interface
					// devirtualization branch in InvocationPlanning.cs would be dead code.
					if (targetReference.Kind == AotCoreIrReferenceKind.Type && targetReference.ImplementedInterfaceSubjectIds is { Count: > 0 })
					{
						foreach (string implementedInterfaceSubjectId in targetReference.ImplementedInterfaceSubjectIds)
						{
							if (!string.IsNullOrEmpty(implementedInterfaceSubjectId))
							{
								hashSet.Add(implementedInterfaceSubjectId);
							}
						}
					}
					// Also collect from array element ImplementedInterfaceSubjectIds.
					if (targetReference.ArrayElementImplementedInterfaceSubjectIds is { Count: > 0 })
					{
						foreach (string arrayElementImplementedInterfaceSubjectId in targetReference.ArrayElementImplementedInterfaceSubjectIds)
						{
							if (!string.IsNullOrEmpty(arrayElementImplementedInterfaceSubjectId))
							{
								hashSet.Add(arrayElementImplementedInterfaceSubjectId);
							}
						}
					}
				}
			}
		}
		return hashSet;
	}

	private static string GetDeclaringTypeSubjectId(string fieldSubjectId)
	{
		int num = fieldSubjectId.IndexOf("::", StringComparison.Ordinal);
		if (num <= 0)
		{
			throw new InvalidOperationException("field subject '" + fieldSubjectId + "' is missing declaring type information");
		}
		return fieldSubjectId.Substring(0, num);
	}

	private static string GetFieldName(string fieldSubjectId)
	{
		int num = fieldSubjectId.IndexOf("::", StringComparison.Ordinal);
		if (num <= 0 || num + 2 >= fieldSubjectId.Length)
		{
			throw new InvalidOperationException("field subject '" + fieldSubjectId + "' is missing field member information");
		}
		int num2 = num + 2;
		return fieldSubjectId.Substring(num2, fieldSubjectId.Length - num2);
	}

	private static string GetMethodSignatureSuffix(string subjectId)
	{
		int num = subjectId.IndexOf("::", StringComparison.Ordinal);
		if (num <= 0 || num + 2 >= subjectId.Length)
		{
			throw new InvalidOperationException("method subject '" + subjectId + "' is missing method signature information");
		}
		int num2 = num + 2;
		return subjectId.Substring(num2, subjectId.Length - num2);
	}

	private static string GetMethodDeclaringTypeSubjectId(string subjectId)
	{
		int num = subjectId.IndexOf("::", StringComparison.Ordinal);
		if (num <= 0)
		{
			throw new InvalidOperationException("method subject '" + subjectId + "' is missing declaring type information");
		}
		return subjectId.Substring(0, num);
	}

	private IReadOnlyList<AotCoreIrAbiSlotArtifact> ResolveDelegateInvokeParameterAbis(AotCoreIrInstructionArtifact instruction)
	{
		if (!string.IsNullOrEmpty(instruction.Callee) && _methodsBySubjectId.TryGetValue(instruction.Callee, out AotCoreIrMethodArtifact? value))
		{
			return value.ParameterAbis;
		}
		return GetMethodParameterTypes(instruction.Callee!).Select(CreateLegacyAbiSlot).ToArray();
	}

	private AotCoreIrAbiSlotArtifact ResolveDelegateInvokeReturnAbi(AotCoreIrInstructionArtifact instruction)
	{
		if (!string.IsNullOrEmpty(instruction.Callee) && _methodsBySubjectId.TryGetValue(instruction.Callee, out AotCoreIrMethodArtifact? value))
		{
			return value.ReturnAbi;
		}
		return CreateLegacyReturnAbiSlot(instruction.ResultType);
	}

	private static string GetRequiredDeclaringTypeSubjectId(AotCoreIrReferenceArtifact targetReference)
	{
		if (!string.IsNullOrEmpty(targetReference.DeclaringTypeSubjectId))
		{
			return targetReference.DeclaringTypeSubjectId;
		}
		return GetDeclaringTypeSubjectId(targetReference.SubjectId);
	}

	private bool IsDelegateInvokeInstruction(AotCoreIrInstructionArtifact instruction)
	{
		if (!string.Equals(instruction.Op, "callvirt", StringComparison.Ordinal) || string.IsNullOrEmpty(instruction.Callee) || !string.Equals(GetMethodName(instruction.Callee), "Invoke", StringComparison.Ordinal))
		{
			return false;
		}
		return IsDelegateTypeSubjectId(GetMethodDeclaringTypeSubjectId(instruction.Callee), _referenceTypeBaseSubjectIds);
	}

	private static string GetRequiredStringOperand(AotCoreIrInstructionArtifact instruction)
	{
		if (instruction.Operand is string result)
		{
			return result;
		}
		if (instruction.Operand is JsonElement { ValueKind: JsonValueKind.String } jsonElement)
		{
			return jsonElement.GetString() ?? throw new InvalidOperationException("opcode '" + instruction.Op + "' requires a non-null string operand.");
		}
		throw new InvalidOperationException("opcode '" + instruction.Op + "' requires a string operand.");
	}

	private static string GetFieldHandleLiteral(string subjectId)
	{
		return GetPseudoMetadataHandleLiteral(subjectId, 67108864u);
	}

	private static string GetTypeHandleLiteral(string subjectId)
	{
		return GetPseudoMetadataHandleLiteral(subjectId, 33554432u);
	}

	private static string GetMethodHandleLiteral(string subjectId)
	{
		return GetPseudoMetadataHandleLiteral(subjectId, 100663296u);
	}

	private static string GetNativeSymbol(string prefix, string subjectId)
	{
		return prefix + SanitizeSubjectId(subjectId);
	}

	private static string GetNativeTypeSymbol(string subjectId)
	{
		return GetNativeSymbol("chaos_type_", subjectId);
	}

	private static string GetNativeValueTypeSymbol(string subjectId)
	{
		return GetNativeSymbol("chaos_valuetype_", subjectId);
	}

	private static string GetNativeFieldMemberName(string subjectId)
	{
		return GetNativeSymbol("field_", subjectId);
	}

	private static string GetNativeStaticFieldSymbol(string subjectId)
	{
		return GetNativeSymbol("chaos_static_", subjectId);
	}

	private static string GetNativeTypeInitializationFunctionSymbol(string typeSubjectId)
	{
		return GetNativeSymbol("chaos_ensure_type_initialized_", typeSubjectId);
	}

	private static string GetNativeTypeInitializationOnceFlagSymbol(string typeSubjectId)
	{
		return GetNativeSymbol("chaos_type_init_once_", typeSubjectId);
	}

	private static string GetNativeBoxTypeSymbol(string subjectId)
	{
		return GetNativeSymbol("chaos_boxed_type_", subjectId);
	}

	private static string GetNativeTypeIdSymbol(string subjectId)
	{
		return GetNativeSymbol("chaos_type_id_", subjectId);
	}

	private static string GetNativeTypeInfoSymbol(string subjectId)
	{
		    // MethodTable has no .hot member; use AsTypeInfoHot() which does reinterpret_cast.
	    return GetNativeMethodTableSymbol(subjectId) + ".AsTypeInfoHot()";
	}

	private static string GetNativeTypeInfoV0Symbol(string subjectId)
	{
		return GetNativeMethodTableSymbol(subjectId);
	}

	private static string GetNativeMethodTableSymbol(string subjectId)
	{
		return GetNativeSymbol("chaos_mt_", subjectId);
	}

	private static string GetNativeVTableSymbol(string subjectId)
	{
		return GetNativeSymbol("chaos_vtable_", subjectId);
	}

	private static string GetNativeIfaceMapSymbol(string subjectId)
	{
		return GetNativeSymbol("chaos_iface_map_", subjectId);
	}

	private static string GetNativeBoxTypeIdSymbol(string subjectId)
	{
		return GetNativeSymbol("chaos_boxed_type_id_", subjectId);
	}

	private static string GetNativeTypeInfoWarmSymbol(string subjectId)
	{
		// MethodTable has warm fields inline at offset 32; use .warm member access.
		return GetNativeMethodTableSymbol(subjectId) + ".warm";
	}

	private static string GetNativeBoxTypeInfoSymbol(string subjectId)
	{
		return GetNativeTypeInfoSymbol(subjectId);
	}

	private static string GetNativeStructMarshallingDescriptorSymbol(string subjectId)
	{
		return GetNativeSymbol("chaos_marshal_desc_", subjectId);
	}

	private static string GetNativeStructFieldNamesSymbol(string subjectId)
	{
		return "s_marshal_field_names_" + SanitizeSubjectId(subjectId);
	}

	/// <summary>
	/// Compute FNV-1a 64-bit stable type ID from a subject ID.
	/// Must match chaos_compute_type_stable_id() in type_info.h.
	/// </summary>
	private static ulong ComputeStableTypeId(string subjectId)
	{
		const ulong fnvOffsetBasis64 = 14695981039346656037;
		const ulong fnvPrime64 = 1099511628211;
		ulong hash = fnvOffsetBasis64;
		foreach (char c in subjectId)
		{
			hash ^= (byte)c;
			hash *= fnvPrime64;
		}
		return hash;
	}


	private static string GetExternalRuntimeHelperSymbol(string subjectId)
	{
		return "chaos_external_runtime_" + SanitizeSubjectId(subjectId);
	}

	private static bool HasArrayElementReference(AotCoreIrReferenceArtifact targetReference)
	{
		return !string.IsNullOrEmpty(targetReference.ArrayElementSubjectId);
	}

	private static string GetMethodName(string subjectId)
	{
		int num = subjectId.IndexOf("::", StringComparison.Ordinal);
		int num2 = subjectId.IndexOf(':', num + 2);
		int num3 = subjectId.IndexOf('(', num + 2);
		if (num2 < 0 || (num3 >= 0 && num3 < num2))
		{
			num2 = num3;
		}
		if (num <= 0 || num2 <= num + 2)
		{
			throw new InvalidOperationException("failed to extract method name from subject id '" + subjectId + "'");
		}
		int num4 = num + 2;
		return subjectId.Substring(num4, num2 - num4);
	}

	private static string GetTypeDisplayName(string typeSubjectId)
	{
		int num = typeSubjectId.IndexOf('/');
		if (num < 0 || num + 1 >= typeSubjectId.Length)
		{
			return typeSubjectId;
		}
		int num2 = num + 1;
		return typeSubjectId.Substring(num2, typeSubjectId.Length - num2);
	}

	private static string GetTypeNamespace(string typeSubjectId)
	{
		string displayName = GetTypeDisplayName(typeSubjectId);
		int num = displayName.LastIndexOf('.');
		if (num <= 0)
		{
			return string.Empty;
		}
		return displayName.Substring(0, num);
	}

	private static string GetTypeShortName(string typeSubjectId)
	{
		string displayName = GetTypeDisplayName(typeSubjectId);
		int num = displayName.LastIndexOf('.');
		if (num <= 0 || num + 1 >= displayName.Length)
		{
			return displayName;
		}
		return displayName.Substring(num + 1, displayName.Length - num - 1);
	}

	private static string GetAssemblyNameFromSubjectId(string subjectId)
	{
		int num = subjectId.IndexOf('/');
		if (num <= 0)
		{
			throw new InvalidOperationException("subject '" + subjectId + "' is missing assembly information");
		}
		return subjectId.Substring(0, num);
	}

	private static string? TryGetNestedDeclaringTypeSubjectId(string typeSubjectId)
	{
		int num = typeSubjectId.IndexOf('/');
		int num2 = typeSubjectId.LastIndexOf('+');
		if (num2 <= num)
		{
			return null;
		}
		return typeSubjectId.Substring(0, num2);
	}

	private static bool MatchesTypeName(string candidateTypeSubjectId, string typeNameOrSubjectId)
	{
		if (string.Equals(candidateTypeSubjectId, typeNameOrSubjectId, StringComparison.Ordinal))
		{
			return true;
		}
		string typeDisplayName = GetTypeDisplayName(candidateTypeSubjectId);
		if (string.Equals(typeDisplayName, typeNameOrSubjectId, StringComparison.Ordinal))
		{
			return true;
		}
		int num = Math.Max(typeDisplayName.LastIndexOf('.'), typeDisplayName.LastIndexOf('+'));
		if (num >= 0 && num + 1 < typeDisplayName.Length)
		{
			string text = typeDisplayName;
			int num2 = num + 1;
			return string.Equals(text.Substring(num2, text.Length - num2), typeNameOrSubjectId, StringComparison.Ordinal);
		}
		return false;
	}

	private static bool IsSpecialMethodName(string? name)
	{
		if (string.IsNullOrEmpty(name))
		{
			return true;
		}
		if (!name.StartsWith(".", StringComparison.Ordinal) && !name.StartsWith("<", StringComparison.Ordinal) && !name.StartsWith("get_", StringComparison.Ordinal) && !name.StartsWith("set_", StringComparison.Ordinal) && !name.StartsWith("add_", StringComparison.Ordinal))
		{
			return name.StartsWith("remove_", StringComparison.Ordinal);
		}
		return true;
	}

	private static bool IsAsyncRuntimeHelperSubjectId(string subjectId)
	{
		subjectId = ManagedNaming.NormalizeSubjectIdAssembly(subjectId);
		if (subjectId.Contains("YieldAwaitable", StringComparison.Ordinal) || (subjectId.StartsWith("System.Private.CoreLib/System.Threading.Tasks.Task::", StringComparison.Ordinal) && string.Equals(GetMethodName(subjectId), "Yield", StringComparison.Ordinal)))
		{
			return true;
		}
		if (subjectId.StartsWith("System.Private.CoreLib/System.Runtime.CompilerServices.AsyncTaskMethodBuilder<", StringComparison.Ordinal) || subjectId.StartsWith("System.Private.CoreLib/System.Runtime.CompilerServices.AsyncValueTaskMethodBuilder<", StringComparison.Ordinal) || subjectId.StartsWith("System.Private.CoreLib/System.Runtime.CompilerServices.TaskAwaiter<", StringComparison.Ordinal) || subjectId.StartsWith("System.Private.CoreLib/System.Runtime.CompilerServices.ValueTaskAwaiter<", StringComparison.Ordinal))
		{
			return true;
		}
		if (subjectId.StartsWith("System.Private.CoreLib/System.Threading.Tasks.Task<", StringComparison.Ordinal) && string.Equals(GetMethodName(subjectId), "GetAwaiter", StringComparison.Ordinal))
		{
			return true;
		}
		return subjectId.StartsWith("System.Private.CoreLib/System.Threading.Tasks.ValueTask<", StringComparison.Ordinal) && string.Equals(GetMethodName(subjectId), "GetAwaiter", StringComparison.Ordinal);
	}

	private static bool IsCollectionRuntimeHelperSubjectId(string subjectId)
	{
		subjectId = ManagedNaming.NormalizeSubjectIdAssembly(subjectId);
		if (!subjectId.StartsWith("System.Collections/System.Collections.Generic.List<", StringComparison.Ordinal) && !subjectId.StartsWith("System.Collections/System.Collections.Generic.Dictionary<", StringComparison.Ordinal) && !subjectId.StartsWith("System.Private.CoreLib/System.Collections.Generic.IReadOnlyCollection<", StringComparison.Ordinal))
		{
			return subjectId.StartsWith("System.Private.CoreLib/System.Collections.Generic.IReadOnlyList<", StringComparison.Ordinal);
		}
		return true;
	}

	private static bool IsSpanRuntimeHelperSubjectId(string subjectId)
	{
		subjectId = ManagedNaming.NormalizeSubjectIdAssembly(subjectId);
		if (MatchesMethodSubject(subjectId, "System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers", "InitializeArray", "System.Array", "System.RuntimeFieldHandle") || subjectId.StartsWith("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::CreateSpan<", StringComparison.Ordinal) || subjectId.StartsWith("System.Memory/System.MemoryExtensions::AsSpan<", StringComparison.Ordinal) || subjectId.StartsWith("System.Memory/System.MemoryExtensions::AsMemory<", StringComparison.Ordinal))
		{
			return true;
		}
		string text = (subjectId.Contains("::", StringComparison.Ordinal) ? GetMethodDeclaringTypeSubjectId(subjectId) : string.Empty);
		if (!text.StartsWith("System.Private.CoreLib/System.Span<", StringComparison.Ordinal) && !text.StartsWith("System.Private.CoreLib/System.ReadOnlySpan<", StringComparison.Ordinal))
		{
			return text.StartsWith("System.Private.CoreLib/System.Memory<", StringComparison.Ordinal);
		}
		return true;
	}

	private static bool IsMonitorRuntimeHelperSubjectId(string subjectId)
	{
		if (!MatchesMethodSubject(subjectId, "System.Threading/Monitor", "Enter", "System.Object", "System.Boolean&") && !MatchesMethodSubject(subjectId, "System.Threading/Monitor", "Exit", "System.Object"))
		{
			return MatchesMethodSubject(subjectId, "System.Threading/Monitor", "TryEnter", "System.Object", "System.TimeSpan", "System.Boolean&");
		}
		return true;
	}

	private static bool IsThreadRuntimeHelperSubjectId(string subjectId)
	{
		subjectId = ManagedNaming.NormalizeSubjectIdAssembly(subjectId);
		if (!MatchesMethodSubject(subjectId, "System.Private.CoreLib/System.Environment", "get_CurrentManagedThreadId") && !MatchesMethodSubject(subjectId, "System.Threading.Thread/System.Threading.Thread", ".ctor", "System.Threading.ThreadStart") && !MatchesMethodSubject(subjectId, "System.Threading.Thread/System.Threading.Thread", "Start") && !MatchesMethodSubject(subjectId, "System.Threading.Thread/System.Threading.Thread", "Join") && !MatchesMethodSubject(subjectId, "System.Threading.Thread/System.Threading.Thread", "get_CurrentThread") && !MatchesMethodSubject(subjectId, "System.Threading.Thread/System.Threading.Thread", "get_Name"))
		{
			return MatchesMethodSubject(subjectId, "System.Threading.Thread/System.Threading.Thread", "set_Name", "System.String");
		}
		return true;
	}

	private static bool IsDefaultInterpolatedStringHandlerHelperSubjectId(string subjectId)
	{
		subjectId = ManagedNaming.NormalizeSubjectIdAssembly(subjectId);
		if (!MatchesMethodSubject(subjectId, "System.Private.CoreLib/System.Runtime.CompilerServices.DefaultInterpolatedStringHandler", ".ctor", "System.Int32", "System.Int32") && !MatchesMethodSubject(subjectId, "System.Private.CoreLib/System.Runtime.CompilerServices.DefaultInterpolatedStringHandler", "AppendFormatted", "System.String") && !MatchesMethodSubject(subjectId, "System.Private.CoreLib/System.Runtime.CompilerServices.DefaultInterpolatedStringHandler", "AppendLiteral", "System.String") && !IsSupportedDefaultInterpolatedStringHandlerAppendFormattedSubjectId(subjectId))
		{
			return MatchesMethodSubject(subjectId, "System.Private.CoreLib/System.Runtime.CompilerServices.DefaultInterpolatedStringHandler", "ToStringAndClear");
		}
		return true;
	}

	private static bool IsTypeReflectionHelperSubjectId(string subjectId)
	{
		subjectId = ManagedNaming.NormalizeSubjectIdAssembly(subjectId);
		return MatchesMethodSubject(subjectId, "System.Private.CoreLib/System.Type", "GetTypeFromHandle", "System.RuntimeTypeHandle") || string.Equals(subjectId, GetTypeHandleMethodSubjectId, StringComparison.Ordinal) || string.Equals(subjectId, GetFieldMethodSubjectId, StringComparison.Ordinal) || string.Equals(subjectId, GetPropertyMethodSubjectId, StringComparison.Ordinal) || string.Equals(subjectId, GetPropertiesMethodSubjectId, StringComparison.Ordinal) || string.Equals(subjectId, GetMethodByNameMethodSubjectId, StringComparison.Ordinal) || string.Equals(subjectId, GetInterfaceMethodSubjectId, StringComparison.Ordinal) || string.Equals(subjectId, GetInterfaceMapMethodSubjectId, StringComparison.Ordinal) || string.Equals(subjectId, GetGenericArgumentsMethodSubjectId, StringComparison.Ordinal) || string.Equals(subjectId, MemberInfoGetDeclaringTypeMethodSubjectId, StringComparison.Ordinal) || string.Equals(subjectId, GetMethodMethodSubjectId, StringComparison.Ordinal) || string.Equals(subjectId, GetAssemblyMethodSubjectId, StringComparison.Ordinal) || string.Equals(subjectId, GetGenericTypeDefinitionMethodSubjectId, StringComparison.Ordinal) || string.Equals(subjectId, GetConstructorsMethodSubjectId, StringComparison.Ordinal) || string.Equals(subjectId, ActivatorCreateInstanceTypeMethodSubjectId, StringComparison.Ordinal);
	}

	private static bool IsAssemblyReflectionHelperSubjectId(string subjectId)
	{
		subjectId = ManagedNaming.NormalizeSubjectIdAssembly(subjectId);
		return MatchesMethodSubject(subjectId, "System.Private.CoreLib/System.Type", "get_Assembly") || MatchesMethodSubject(subjectId, "System.Private.CoreLib/System.Reflection.Assembly", "GetType", "System.String") || MatchesMethodSubject(subjectId, "System.Private.CoreLib/System.Type", "GetType", "System.String") || MatchesMethodSubject(subjectId, "System.Private.CoreLib/System.Reflection.Assembly", "GetName") || MatchesMethodSubject(subjectId, "System.Private.CoreLib/System.Reflection.AssemblyName", "get_Name");
	}

	private static bool IsReflectionMemberHelperSubjectId(string subjectId)
	{
		subjectId = ManagedNaming.NormalizeSubjectIdAssembly(subjectId);
		return string.Equals(subjectId, GetMethodByNameMethodSubjectId, StringComparison.Ordinal) || string.Equals(subjectId, GetMethodMethodSubjectId, StringComparison.Ordinal) || string.Equals(subjectId, GetPropertyMethodSubjectId, StringComparison.Ordinal) || string.Equals(subjectId, GetPropertiesMethodSubjectId, StringComparison.Ordinal) || string.Equals(subjectId, GetInterfaceMapMethodSubjectId, StringComparison.Ordinal) || string.Equals(subjectId, GetParametersMethodSubjectId, StringComparison.Ordinal) || string.Equals(subjectId, MemberInfoGetNameMethodSubjectId, StringComparison.Ordinal) || string.Equals(subjectId, ParameterInfoGetNameMethodSubjectId, StringComparison.Ordinal) || string.Equals(subjectId, GetMethodHandleMethodSubjectId, StringComparison.Ordinal) || MatchesMethodSubject(subjectId, "System.Private.CoreLib/System.Reflection.MethodBase", "Invoke", "System.Object", "System.Object[]");
	}

	private static bool UsesReachableInstruction(IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods, Func<AotCoreIrInstructionArtifact, bool> predicate)
	{
		return reachableMethods.Any((AotCoreIrMethodArtifact method) => method.Instructions.Any(predicate));
	}

	private bool UsesDefaultInterpolatedStringHandlerHelpers(IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods)
	{
		return UsesReachableInstruction(reachableMethods, (AotCoreIrInstructionArtifact instruction) => IsDefaultInterpolatedStringHandlerHelperSubjectId(instruction.Callee ?? string.Empty));
	}

	private static string GetPseudoMetadataHandleLiteral(string subjectId, uint prefix)
	{
		var key = prefix + subjectId;
		if (!_pseudoMetadataHandleCache.TryGetValue(key, out var cached))
			_pseudoMetadataHandleCache[key] = cached = $"static_cast<CHAOS_IL2CPP_INTPTR>({CreatePseudoMetadataHandle(subjectId, prefix)}u)";
		return cached;
	}

	private static uint CreatePseudoMetadataHandle(string subjectId, uint prefix)
	{
		uint num = 2166136261u;
		foreach (char c in subjectId)
		{
			num ^= c;
			num *= 16777619;
		}
		uint num2 = num & 0xFFFFFF;
		if (num2 == 0)
		{
			num2 = 1u;
		}
		return prefix | num2;
	}

	private static string GetRuntimeTypeInfoExpression(string? subjectId)
	{
		if (string.IsNullOrEmpty(subjectId))
		{
			return "nullptr";
		}
		return GetNativeTypeInfoSymbol(subjectId);
	}

	[Obsolete("Use GetRuntimeTypeInfoExpression instead")]
	private static string GetRuntimeTypeIdExpression(string? subjectId, AotCoreIrTypeShapeKind typeShape)
	{
		return GetRuntimeTypeInfoExpression(subjectId);
	}

	private static byte GetNativeTypeShapeValue(AotCoreIrTypeShapeKind typeShape)
	{
		return typeShape switch
		{
			AotCoreIrTypeShapeKind.ReferenceType => 1,
			AotCoreIrTypeShapeKind.ValueType => 2,
			AotCoreIrTypeShapeKind.InterfaceType => 3,
			_ => 0,
		};
	}

	private static bool RequiresStructuredValueTypePayload(AotCoreIrReferenceArtifact targetReference)
	{
		if (targetReference.TypeShape == AotCoreIrTypeShapeKind.ValueType)
		{
			return IsStructuredValueTypeSubjectId(targetReference.SubjectId);
		}
		return false;
	}

	// Primitive value types that need inline INTPTR storage, not a struct definition.
	private static readonly System.Collections.Generic.HashSet<string> PrimitiveValueTypeSubjectIds = new(System.StringComparer.Ordinal)
	{
		"System.Private.CoreLib/System.Boolean",
		"System.Private.CoreLib/System.Byte",
		"System.Private.CoreLib/System.SByte",
		"System.Private.CoreLib/System.Int16",
		"System.Private.CoreLib/System.UInt16",
		"System.Private.CoreLib/System.Int32",
		"System.Private.CoreLib/System.UInt32",
		"System.Private.CoreLib/System.Int64",
		"System.Private.CoreLib/System.UInt64",
		"System.Private.CoreLib/System.Single",
		"System.Private.CoreLib/System.Double",
		"System.Private.CoreLib/System.Char",
		"System.Private.CoreLib/System.IntPtr",
		"System.Private.CoreLib/System.UIntPtr",
	};

	private static bool IsStructuredValueTypeSubjectId(string subjectId)
	{
		return !PrimitiveValueTypeSubjectIds.Contains(subjectId);
	}

	private static string SanitizeSubjectId(string subjectId)
	{
		if (_sanitizedSubjectIdCache.TryGetValue(subjectId, out var cached))
			return cached;

		StringBuilder stringBuilder = new StringBuilder(subjectId.Length);
		foreach (char c in subjectId)
		{
			stringBuilder.Append(char.IsLetterOrDigit(c) ? c : '_');
		}
		if (stringBuilder.Length == 0)
		{
			stringBuilder.Append("subject");
		}
		if (char.IsDigit(stringBuilder[0]))
		{
			stringBuilder.Insert(0, '_');
		}
		return _sanitizedSubjectIdCache[subjectId] = stringBuilder.ToString();
	}

	private static string ToCppStringLiteral(string value)
	{
		if (_cppStringLiteralCache.TryGetValue(value, out var cached))
			return cached;

		StringBuilder stringBuilder = new StringBuilder(value.Length + 2);
		stringBuilder.Append('"');
		foreach (char c in value)
		{
			switch (c)
			{
			case '\\':
				stringBuilder.Append("\\\\");
				continue;
			case '"':
				stringBuilder.Append("\\\"");
				continue;
			case '\0':
				stringBuilder.Append("\\0");
				continue;
			case '\a':
				stringBuilder.Append("\\a");
				continue;
			case '\b':
				stringBuilder.Append("\\b");
				continue;
			case '\f':
				stringBuilder.Append("\\f");
				continue;
			case '\n':
				stringBuilder.Append("\\n");
				continue;
			case '\r':
				stringBuilder.Append("\\r");
				continue;
			case '\t':
				stringBuilder.Append("\\t");
				continue;
			case '\v':
				stringBuilder.Append("\\v");
				continue;
			}
			if (char.IsControl(c))
			{
				stringBuilder.Append("\\u");
				int num = c;
				stringBuilder.Append(num.ToString("X4", CultureInfo.InvariantCulture));
			}
			else
			{
				stringBuilder.Append(c);
			}
		}
		stringBuilder.Append('"');
		return _cppStringLiteralCache[value] = stringBuilder.ToString();
	}

	private static string FormatCustomAttributeLiteralExpression(CustomAttributeLiteralValue value)
	{
		return value.Kind switch
		{
			CustomAttributeLiteralKind.Null => "0",
			CustomAttributeLiteralKind.Boolean => ((bool)value.Value!) ? "static_cast<CHAOS_IL2CPP_INTPTR>(1)" : "0",
			CustomAttributeLiteralKind.Byte => $"static_cast<CHAOS_IL2CPP_INTPTR>({(byte)value.Value!})",
			CustomAttributeLiteralKind.SByte => $"static_cast<CHAOS_IL2CPP_INTPTR>({(sbyte)value.Value!})",
			CustomAttributeLiteralKind.Int16 => $"static_cast<CHAOS_IL2CPP_INTPTR>({(short)value.Value!})",
			CustomAttributeLiteralKind.Int32 => $"static_cast<CHAOS_IL2CPP_INTPTR>({(int)value.Value!})",
			CustomAttributeLiteralKind.Int64 => $"static_cast<CHAOS_IL2CPP_INTPTR>({(long)value.Value!}ll)",
			CustomAttributeLiteralKind.UInt16 => $"static_cast<CHAOS_IL2CPP_INTPTR>({(ushort)value.Value!}u)",
			CustomAttributeLiteralKind.UInt32 => $"static_cast<CHAOS_IL2CPP_INTPTR>({(uint)value.Value!}u)",
			CustomAttributeLiteralKind.UInt64 => $"static_cast<CHAOS_IL2CPP_INTPTR>({(ulong)value.Value!}ull)",
			CustomAttributeLiteralKind.Char => $"static_cast<CHAOS_IL2CPP_INTPTR>({(char)value.Value!})",
			CustomAttributeLiteralKind.Single => $"ChaosStoreFloat32({(float)value.Value!}f)",
			CustomAttributeLiteralKind.Double => $"ChaosStoreFloat64({(double)value.Value!})",
			CustomAttributeLiteralKind.String => "chaos_reflection_create_string_literal(" + ToCppStringLiteral((string)value.Value!) + ")",
			CustomAttributeLiteralKind.Type or CustomAttributeLiteralKind.Enum => "0",
			_ => throw new NotSupportedException($"unsupported custom attribute literal kind '{value.Kind}'."),
		};
	}

	private static string FormatGenericExecutionAuthorityComment(
		string? openDefinitionSubjectId,
		SharedGenericBodyId? sharedGenericBodyId,
		InstantiationStubId? instantiationStubId,
		RuntimeGenericContextArtifact? runtimeGenericContext)
	{
		if (string.IsNullOrEmpty(openDefinitionSubjectId) &&
			sharedGenericBodyId is null &&
			instantiationStubId is null &&
			runtimeGenericContext is null)
		{
			throw new ArgumentNullException(nameof(runtimeGenericContext));
		}

		string definitionSubjectId = !string.IsNullOrEmpty(openDefinitionSubjectId)
			? openDefinitionSubjectId
			: runtimeGenericContext!.InstantiationKey.DefinitionSubjectId;
		string bodyId = sharedGenericBodyId?.Value
			?? runtimeGenericContext?.SharedGenericBodyId.Value
			?? "<unknown>";
		string stubId = instantiationStubId?.Value
			?? runtimeGenericContext?.InstantiationStubId.Value
			?? "<unknown>";
		string support = runtimeGenericContext?.SupportKindCode.ToString() ?? "<unknown>";
		string specialization = runtimeGenericContext?.SpecializationKindCode.ToString() ?? "<unknown>";
		IReadOnlyList<string>? typeArguments = runtimeGenericContext?.InstantiationKey.TypeArguments;
		IReadOnlyList<string>? methodArguments = runtimeGenericContext?.InstantiationKey.MethodArguments;
		return $"// Generic execution authority: definition={definitionSubjectId}; type={FormatGenericArgumentList(typeArguments)}; method={FormatGenericArgumentList(methodArguments)}; support={support}; specialization={specialization}; body={bodyId}; stub={stubId}";
	}

	private static string FormatGenericArgumentList(IReadOnlyList<string>? arguments)
	{
		if (arguments == null || arguments.Count == 0)
		{
			return "[]";
		}
		return "[" + string.Join(", ", arguments) + "]";
	}

	private static string ExtractComMethodName(string subjectId)
	{
		// Format: "Assembly/TypeName::MethodName:Signature"
		var idx = subjectId.LastIndexOf("::", StringComparison.Ordinal);
		if (idx < 0) return subjectId;
		var after = subjectId.Substring(idx + 2);
		var colonIdx = after.IndexOf(':', StringComparison.Ordinal);
		return colonIdx >= 0 ? after.Substring(0, colonIdx) : after;
	}

	/// <summary>
	/// Collect COM interface vtable data from AotCoreIr references.
	/// Scans all instructions for type references with ComInterfaceGuid set,
	/// then counts the methods declared on each such COM interface type.
	/// </summary>
	internal static Dictionary<string, NativeAotLoweringPlanner.ComInterfaceVtableInfo> CollectComInterfaceVtableData(
			AotCoreIrArtifact aotCoreIr,
			IReadOnlyDictionary<string, AotCoreIrMethodArtifact> methodsBySubjectId,
			MetadataRegistrationArtifact metadataRegistration)
	{
		// Phase 1: Find all COM interface types (type references with ComInterfaceGuid)
		var comInterfaceGuidMap = new Dictionary<string, (string Guid, int TypeKind)>(StringComparer.Ordinal);
		foreach (var method in aotCoreIr.Methods)
		{
			foreach (var instruction in method.Instructions)
			{
				var targetRef = instruction.TargetReference;
				if (targetRef?.Kind == AotCoreIrReferenceKind.Type &&
					targetRef.ComInterfaceGuid is { Length: > 0 } guid)
				{
					comInterfaceGuidMap[targetRef.SubjectId] = (guid, targetRef.ComInterfaceTypeKind);
				}
			}
		}

		if (comInterfaceGuidMap.Count == 0)
			return new Dictionary<string, NativeAotLoweringPlanner.ComInterfaceVtableInfo>(StringComparer.Ordinal);

		// Phase 2: For each COM interface, collect declared methods sorted by metadata token.
		var tokenLookup = new MetadataTokenLookup(metadataRegistration.Registrations);
		var result = new Dictionary<string, NativeAotLoweringPlanner.ComInterfaceVtableInfo>(StringComparer.Ordinal);
		foreach (var kvp in comInterfaceGuidMap)
		{
			var methods = methodsBySubjectId.Values
				.Where(m => string.Equals(m.Identity.DeclaringTypeSubjectId, kvp.Key, StringComparison.Ordinal))
				.ToList();

			// Sort by metadata token for stable vtable slot ordering.
			methods.Sort((a, b) =>
			{
				uint ta = tokenLookup.TryGetMethodToken(a.SubjectId);
				uint tb = tokenLookup.TryGetMethodToken(b.SubjectId);
				return ta.CompareTo(tb);
			});

			var slots = methods.Select(m => {
				var mn = ExtractComMethodName(m.SubjectId);
				return new NativeAotLoweringPlanner.ComInterfaceMethodSlot(
					tokenLookup.TryGetMethodToken(m.SubjectId), m.NativeSymbol, mn, m.ParameterCount);
			}).ToArray();

			var (guid, typeKind) = kvp.Value;
			result[kvp.Key] = new NativeAotLoweringPlanner.ComInterfaceVtableInfo(
				guid, ComputeStableTypeId(kvp.Key), slots,
				IsDispatch: typeKind == 1 || typeKind == 2);
		}

		return result;
	}
}
