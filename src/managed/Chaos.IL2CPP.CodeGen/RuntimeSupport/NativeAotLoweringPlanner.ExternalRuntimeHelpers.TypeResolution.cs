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
	private AotCoreIrMethodArtifact ResolveRequiredAsyncRuntimeContinuationMethod(string callee, string stateMachineTypeName)
	{
		if (TryResolveAsyncRuntimeContinuationMethod(callee, out AotCoreIrMethodArtifact continuationMethod))
		{
			return continuationMethod;
		}
		throw new NotSupportedException($"native-aot lowering could not resolve async state-machine continuation for '{stateMachineTypeName}' from '{callee}'.");
	}

	private static bool TryGetAsyncStateMachineTypeName(string callee, out string? stateMachineTypeName)
	{
		stateMachineTypeName = null;
		if (TryParseAsyncTaskBuilderStartStateMachineType(callee, out stateMachineTypeName))
		{
			return true;
		}
		if (TryParseAsyncTaskBuilderAwaitUnsafeOnCompleted(callee, out string _, out stateMachineTypeName))
		{
			return true;
		}
		return false;
	}

	private static bool TryParseAsyncTaskBuilderStartStateMachineType(string callee, out string? stateMachineTypeName)
	{
		stateMachineTypeName = null;
		if (!TryReadGenericArgumentList(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncTaskMethodBuilder<System.Int32>::Start<", out string genericArgumentList))
		{
			return false;
		}
		IReadOnlyList<string> readOnlyList = SplitTopLevelGenericArguments(genericArgumentList);
		if (readOnlyList.Count != 1)
		{
			return false;
		}
		stateMachineTypeName = readOnlyList[0];
		return true;
	}

	private static bool TryParseAsyncTaskBuilderAwaitUnsafeOnCompleted(string callee, out string? awaiterTypeName, out string? stateMachineTypeName)
	{
		awaiterTypeName = null;
		stateMachineTypeName = null;
		if (!TryReadGenericArgumentList(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncTaskMethodBuilder<System.Int32>::AwaitUnsafeOnCompleted<", out string genericArgumentList))
		{
			return false;
		}
		IReadOnlyList<string> readOnlyList = SplitTopLevelGenericArguments(genericArgumentList);
		if (readOnlyList.Count != 2)
		{
			return false;
		}
		awaiterTypeName = readOnlyList[0];
		stateMachineTypeName = readOnlyList[1];
		return true;
	}

	private static bool TryReadGenericArgumentList(string value, string marker, out string genericArgumentList)
	{
		genericArgumentList = string.Empty;
		int num = value.IndexOf(marker, StringComparison.Ordinal);
		if (num < 0)
		{
			return false;
		}
		int num2 = num + marker.Length;
		int num3 = 1;
		for (int i = num2; i < value.Length; i++)
		{
			switch (value[i])
			{
			case '<':
				num3++;
				break;
			case '>':
				num3--;
				if (num3 == 0)
				{
					int num4 = num2;
					genericArgumentList = value.Substring(num4, i - num4);
					return true;
				}
				break;
			}
		}
		return false;
	}

	private static IReadOnlyList<string> SplitTopLevelGenericArguments(string value)
	{
		if (string.IsNullOrWhiteSpace(value))
		{
			return Array.Empty<string>();
		}
		List<string> list = new List<string>();
		int num = 0;
		int num2 = 0;
		int num3;
		for (int i = 0; i < value.Length; i++)
		{
			switch (value[i])
			{
			case '<':
				num++;
				break;
			case '>':
				num--;
				break;
			case ',':
				if (num == 0)
				{
					num3 = num2;
					list.Add(value.Substring(num3, i - num3).Trim());
					num2 = i + 1;
				}
				break;
			}
		}
		num3 = num2;
		list.Add(value.Substring(num3, value.Length - num3).Trim());
		return list;
	}

	private static bool TryReadSingleGenericTypeArgument(string value, string marker, out string typeSubjectId)
	{
		typeSubjectId = string.Empty;
		if (!TryReadGenericArgumentList(value, marker, out string genericArgumentList))
		{
			return false;
		}
		IReadOnlyList<string> readOnlyList = SplitTopLevelGenericArguments(genericArgumentList);
		if (readOnlyList.Count != 1)
		{
			return false;
		}
		typeSubjectId = readOnlyList[0];
		return !string.IsNullOrWhiteSpace(typeSubjectId);
	}

	private static bool TryParseClosedListElementType(string declaringTypeSubjectId, out string elementTypeNameOrSubjectId)
	{
		return TryParseClosedSingleGenericArgument(declaringTypeSubjectId, "System.Collections/System.Collections.Generic.List<", out elementTypeNameOrSubjectId);
	}

	private static bool TryParseClosedReadOnlyCollectionElementType(string declaringTypeSubjectId, out string elementTypeNameOrSubjectId)
	{
		return TryParseClosedSingleGenericArgument(declaringTypeSubjectId, "System.Private.CoreLib/System.Collections.Generic.IReadOnlyCollection<", out elementTypeNameOrSubjectId);
	}

	private static bool TryParseClosedReadOnlyListElementType(string declaringTypeSubjectId, out string elementTypeNameOrSubjectId)
	{
		return TryParseClosedSingleGenericArgument(declaringTypeSubjectId, "System.Private.CoreLib/System.Collections.Generic.IReadOnlyList<", out elementTypeNameOrSubjectId);
	}

	private static bool TryParseClosedSingleGenericArgument(string declaringTypeSubjectId, string typeSubjectIdPrefix, out string elementTypeNameOrSubjectId)
	{
		elementTypeNameOrSubjectId = string.Empty;
		if (!TryReadGenericArgumentList(declaringTypeSubjectId, typeSubjectIdPrefix, out string genericArgumentList))
		{
			return false;
		}
		IReadOnlyList<string> readOnlyList = SplitTopLevelGenericArguments(genericArgumentList);
		if (readOnlyList.Count != 1 || string.IsNullOrWhiteSpace(readOnlyList[0]))
		{
			return false;
		}
		elementTypeNameOrSubjectId = readOnlyList[0];
		return true;
	}

	private static bool TryCreateCollectionArrayElementExpression(AotCoreIrAbiSlotArtifact elementAbi, string slotExpression, out string elementExpression)
	{
		switch (elementAbi.CarrierKindCode)
		{
		case AotCoreIrAbiCarrierKind.NativeInt:
			elementExpression = slotExpression;
			return true;
		case AotCoreIrAbiCarrierKind.Int32:
			elementExpression = "static_cast<std::int32_t>(" + slotExpression + ")";
			return true;
		default:
			elementExpression = string.Empty;
			return false;
		}
	}

	private static bool TryParseClosedDictionaryStringIntType(string declaringTypeSubjectId)
	{
		if (!TryReadGenericArgumentList(declaringTypeSubjectId, "System.Collections/System.Collections.Generic.Dictionary<", out string genericArgumentList))
		{
			return false;
		}
		IReadOnlyList<string> readOnlyList = SplitTopLevelGenericArguments(genericArgumentList);
		if (readOnlyList.Count == 2 && string.Equals(readOnlyList[0], "System.String", StringComparison.Ordinal))
		{
			return string.Equals(readOnlyList[1], "System.Int32", StringComparison.Ordinal);
		}
		return false;
	}

	private bool TryCreateResolvedTypeAbiSlot(string typeNameOrSubjectId, out AotCoreIrAbiSlotArtifact abiSlot)
	{
		if (TryResolveReferenceTypeSubjectId(typeNameOrSubjectId, out string subjectId))
		{
			abiSlot = CreateNativeIntAbiSlot(subjectId, AotCoreIrTypeShapeKind.ReferenceType);
			return true;
		}
		string typeDisplayName = GetTypeDisplayName(typeNameOrSubjectId);
		switch (typeDisplayName)
		{
		case "System.Int64":
		case "System.SByte":
		case "System.Int16":
		case "System.Int32":
		case "System.UInt64":
		case "System.Single":
		case "System.Double":
		case "System.UInt16":
		case "System.UInt32":
		case "System.Boolean":
		case "System.Byte":
			abiSlot = CreateLegacyAbiSlot(typeDisplayName);
			return true;
		default:
		{
			if (TryResolveKnownTypeSubjectId(typeNameOrSubjectId, out string subjectId2))
			{
				abiSlot = new AotCoreIrAbiSlotArtifact
				{
					CarrierKindCode = AotCoreIrAbiCarrierKind.ValueTypeByValue,
					TypeSubjectId = subjectId2,
					TypeShape = AotCoreIrTypeShapeKind.ValueType
				};
				return true;
			}
			abiSlot = null;
			return false;
		}
		}
	}

	private bool TryResolveKnownTypeSubjectId(string typeNameOrSubjectId, out string subjectId)
	{
		if (string.IsNullOrWhiteSpace(typeNameOrSubjectId))
		{
			subjectId = string.Empty;
			return false;
		}
		if (typeNameOrSubjectId.Contains('/', StringComparison.Ordinal))
		{
			subjectId = typeNameOrSubjectId;
			return true;
		}
		string[] array = (from candidate in _methodsBySubjectId.Keys.Select(GetMethodDeclaringTypeSubjectId).Distinct<string>(StringComparer.Ordinal)
			where MatchesTypeName(candidate, typeNameOrSubjectId)
			select candidate).OrderBy<string, string>((string candidate) => candidate, StringComparer.Ordinal).ToArray();
		if (array.Length == 1)
		{
			subjectId = array[0];
			return true;
		}
		subjectId = string.Empty;
		return false;
	}

	private static IReadOnlyList<string> GetMethodParameterTypes(string subjectId)
	{
		string methodSignatureSuffix = GetMethodSignatureSuffix(subjectId);
		int num = methodSignatureSuffix.IndexOf('(');
		if (num >= 0)
		{
			if (methodSignatureSuffix[methodSignatureSuffix.Length - 1] == ')')
			{
				string text = methodSignatureSuffix;
				int num2 = num + 1;
				string value = text.Substring(num2, text.Length - 1 - num2);
				if (string.IsNullOrWhiteSpace(value))
				{
					return Array.Empty<string>();
				}
				return SplitTopLevelGenericArguments(value);
			}
		}
		throw new InvalidOperationException("method subject '" + subjectId + "' is missing parameter signature information");
	}

	private static bool IsDelegateTypeSubjectId(string subjectId, IReadOnlyDictionary<string, string?> referenceTypeBaseSubjectIds)
	{
		string value = subjectId;
		while (!string.IsNullOrWhiteSpace(value))
		{
			if (string.Equals(value, "System.Private.CoreLib/System.Delegate", StringComparison.Ordinal) || string.Equals(value, "System.Private.CoreLib/System.MulticastDelegate", StringComparison.Ordinal))
			{
				return true;
			}
			if (!referenceTypeBaseSubjectIds.TryGetValue(value, out value))
			{
				break;
			}
		}
		if (subjectId.StartsWith("System.Private.CoreLib/System.Action", StringComparison.Ordinal) || subjectId.StartsWith("System.Private.CoreLib/System.Func", StringComparison.Ordinal))
		{
			return true;
		}
		return false;
	}

	private string? ResolveReferenceTypeBaseSubjectId(string subjectId, string? baseTypeSubjectId)
	{
		if (!string.IsNullOrWhiteSpace(baseTypeSubjectId))
		{
			return baseTypeSubjectId;
		}
		if (_referenceTypeBaseSubjectIds.TryGetValue(subjectId, out string value) && !string.IsNullOrWhiteSpace(value))
		{
			return value;
		}
		return GetSyntheticReferenceTypeBaseSubjectId(subjectId);
	}

	private static string? GetSyntheticReferenceTypeBaseSubjectId(string subjectId)
	{
		if (string.Equals(subjectId, "System.Private.CoreLib/System.Delegate", StringComparison.Ordinal))
		{
			return "System.Private.CoreLib/System.Object";
		}
		if (string.Equals(subjectId, "System.Private.CoreLib/System.MulticastDelegate", StringComparison.Ordinal))
		{
			return "System.Private.CoreLib/System.Delegate";
		}
		if (subjectId.StartsWith("System.Private.CoreLib/System.Action", StringComparison.Ordinal) || subjectId.StartsWith("System.Private.CoreLib/System.Func", StringComparison.Ordinal))
		{
			return "System.Private.CoreLib/System.MulticastDelegate";
		}
		return null;
	}

	private bool TryResolveReferenceTypeSubjectId(string typeNameOrSubjectId, out string subjectId)
	{
		if (_referenceTypeBaseSubjectIds.ContainsKey(typeNameOrSubjectId) || !string.IsNullOrWhiteSpace(GetSyntheticReferenceTypeBaseSubjectId(typeNameOrSubjectId)))
		{
			subjectId = typeNameOrSubjectId;
			return true;
		}
		string[] array = _referenceTypeBaseSubjectIds.Keys.Where(delegate(string candidate)
		{
			int num = candidate.IndexOf('/');
			string text;
			if (num < 0 || num + 1 >= candidate.Length)
			{
				text = candidate;
			}
			else
			{
				string text2 = candidate;
				int num2 = num + 1;
				text = text2.Substring(num2, text2.Length - num2);
			}
			string text3 = text;
			if (string.Equals(text3, typeNameOrSubjectId, StringComparison.Ordinal))
			{
				return true;
			}
			int num3 = Math.Max(text3.LastIndexOf('.'), text3.LastIndexOf('+'));
			if (num3 >= 0 && num3 + 1 < text3.Length)
			{
				string text2 = text3;
				int num2 = num3 + 1;
				return string.Equals(text2.Substring(num2, text2.Length - num2), typeNameOrSubjectId, StringComparison.Ordinal);
			}
			return false;
		}).Distinct<string>(StringComparer.Ordinal).ToArray();
		if (array.Length == 1)
		{
			subjectId = array[0];
			return true;
		}
		if (TryResolvePreferredSystemReferenceTypeSubjectId(typeNameOrSubjectId, array, out subjectId))
		{
			return true;
		}
		subjectId = string.Empty;
		return false;
	}

	private static bool TryResolvePreferredSystemReferenceTypeSubjectId(string typeNameOrSubjectId, IReadOnlyList<string> matches, out string subjectId)
	{
		if (!typeNameOrSubjectId.StartsWith("System.", StringComparison.Ordinal))
		{
			subjectId = string.Empty;
			return false;
		}
		string[] array = matches.Where((string candidate) => candidate.StartsWith("System.Private.CoreLib/", StringComparison.Ordinal) && MatchesTypeName(candidate, typeNameOrSubjectId)).Distinct<string>(StringComparer.Ordinal).ToArray();
		if (array.Length == 1)
		{
			subjectId = array[0];
			return true;
		}
		subjectId = string.Empty;
		return false;
	}

}
