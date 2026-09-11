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

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
	private AotCoreIrMethodArtifact ResolveRequiredAsyncRuntimeContinuationMethod(string callee, string stateMachineTypeName)
	{
		if (TryResolveAsyncRuntimeContinuationMethod(callee, out AotCoreIrMethodArtifact? continuationMethod))
		{
			return continuationMethod!;
		}
		throw new NotSupportedException($"native-aot lowering could not resolve async state-machine continuation for '{stateMachineTypeName}' from '{callee}'.");
	}

	private static bool TryGetAsyncStateMachineTypeName(string callee, out string? stateMachineTypeName)
	{
		stateMachineTypeName = null;
		// ASYNC-P2-8 A1/A3.  Iterators take their own branch here.
		//
		// A1 returned false unconditionally for every AsyncIteratorMethodBuilder callee,
		// which was right when there was no runtime to lower them onto — but it also made
		// the iterator's OWN state machine name unresolvable, and A3 needs that name to
		// find the iterator's MoveNext symbol (the same lookup the async Task path uses).
		// A3 therefore resolves the name for the ops that embed an <SM> argument, and
		// keeps returning false for the rest.
		//
		// What A1 was actually protecting is unchanged and still enforced: the
		// "we do not support this shape" signal is raised in ClassifyAsyncMethod /
		// EmitManagedMethod, which record the subject id so the generator surfaces
		// kUnsupportedAsyncIterator*.  Resolving a NAME here has never been the signal.
		//
		// This must not throw: callers run inside BuildMethodSourceSafe, which catches
		// every exception and substitutes a silent stub — a throw would be invisible.
		if (IsAsyncIteratorBuilderCallee(callee))
		{
			if (TryParseAsyncIteratorBuilderMoveNextStateMachineType(callee, out stateMachineTypeName))
			{
				return true;
			}
			if (TryParseAsyncIteratorBuilderAwaitOnCompleted(callee, out _, out stateMachineTypeName))
			{
				return true;
			}
			return false;
		}
		if (TryParseAsyncTaskBuilderStartStateMachineType(callee, out stateMachineTypeName))
		{
			return true;
		}
		if (TryParseAsyncValueTaskBuilderStartStateMachineType(callee, out stateMachineTypeName))
		{
			return true;
		}
		if (TryParseAsyncTaskBuilderAwaitUnsafeOnCompleted(callee, out _, out stateMachineTypeName))
		{
			return true;
		}
		if (TryParseAsyncValueTaskBuilderAwaitUnsafeOnCompleted(callee, out _, out stateMachineTypeName))
		{
			return true;
		}
		return false;
	}

	private static bool TryParseAsyncTaskBuilderStartStateMachineType(string callee, out string? stateMachineTypeName)
	{
		return TryParseAsyncTaskBuilderStartStateMachineType(callee, out _, out stateMachineTypeName);
	}

	/// <summary>
	/// ASYNC-P2-8 A1. True if <paramref name="callee"/> is an
	/// <c>AsyncIteratorMethodBuilder</c> call — i.e. the enclosing method is an
	/// <c>async IAsyncEnumerable&lt;T&gt;</c> / <c>async IEnumerator&lt;T&gt;</c> iterator
	/// state machine. Such calls reach the lowering planner for the iterator's own
	/// <c>MoveNext</c> (the builder is driven from inside the state machine, not from a
	/// separate entry method as <c>AsyncTaskMethodBuilder::Start</c> is).
	/// <para>
	/// Detecting on the method subject id would be more precise, but this helper is
	/// static and only has the callee spelling; every <c>AsyncIteratorMethodBuilder</c>
	/// member is declared on that one type, so a substring test cannot false-positive on
	/// a different builder.  It CAN false-positive on a user type with coincidentally
	/// matching text — accepted, because the alternative is a silent wrong answer.
	/// </para>
	/// </summary>
	private static bool IsAsyncIteratorBuilderCallee(string callee)
	{
		return callee.Contains("AsyncIteratorMethodBuilder", StringComparison.Ordinal);
	}

	private static bool TryParseAsyncTaskBuilderAwaitUnsafeOnCompleted(string callee, out string? awaiterTypeName, out string? stateMachineTypeName)
	{
		return TryParseAsyncTaskBuilderAwaitUnsafeOnCompleted(callee, out _, out awaiterTypeName, out stateMachineTypeName);
	}

	private static bool TryParseAsyncValueTaskBuilderStartStateMachineType(string callee, out string? stateMachineTypeName)
	{
		return TryParseAsyncValueTaskBuilderStartStateMachineType(callee, out _, out stateMachineTypeName);
	}

	private static bool TryParseAsyncValueTaskBuilderAwaitUnsafeOnCompleted(string callee, out string? awaiterTypeName, out string? stateMachineTypeName)
	{
		return TryParseAsyncValueTaskBuilderAwaitUnsafeOnCompleted(callee, out _, out awaiterTypeName, out stateMachineTypeName);
	}

	private static bool TryParseAsyncTaskBuilderStartStateMachineType(string callee, out string? builderResultTypeName, out string? stateMachineTypeName)
	{
		return TryParseAsyncBuilderStartStateMachineType(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncTaskMethodBuilder", out builderResultTypeName, out stateMachineTypeName);
	}

	private static bool TryParseAsyncTaskBuilderAwaitUnsafeOnCompleted(string callee, out string? builderResultTypeName, out string? awaiterTypeName, out string? stateMachineTypeName)
	{
		return TryParseAsyncBuilderAwaitUnsafeOnCompleted(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncTaskMethodBuilder", out builderResultTypeName, out awaiterTypeName, out stateMachineTypeName);
	}

	private static bool TryParseAsyncValueTaskBuilderStartStateMachineType(string callee, out string? builderResultTypeName, out string? stateMachineTypeName)
	{
		return TryParseAsyncBuilderStartStateMachineType(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncValueTaskMethodBuilder", out builderResultTypeName, out stateMachineTypeName);
	}

	private static bool TryParseAsyncValueTaskBuilderAwaitUnsafeOnCompleted(string callee, out string? builderResultTypeName, out string? awaiterTypeName, out string? stateMachineTypeName)
	{
		return TryParseAsyncBuilderAwaitUnsafeOnCompleted(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncValueTaskMethodBuilder", out builderResultTypeName, out awaiterTypeName, out stateMachineTypeName);
	}

	/// <summary>
	/// ASYNC-P2-8 A3. The builder type prefix for
	/// <c>System.Runtime.CompilerServices.AsyncIteratorMethodBuilder</c>.
	/// <para>
	/// Spellings verified against the committed contract artifact
	/// <c>runtime-helper-contracts-v1-01.json</c> (not written by hand):
	/// <c>AsyncIteratorMethodBuilder::Create/0</c>, <c>::MoveNext`1/1</c>,
	/// <c>::AwaitOnCompleted`2/2</c>, <c>::AwaitUnsafeOnCompleted`2/2</c>, <c>::Complete/0</c>.
	/// </para>
	/// </summary>
	private const string AsyncIteratorBuilderTypePrefix =
		"System.Private.CoreLib/System.Runtime.CompilerServices.AsyncIteratorMethodBuilder";

	/// <summary>
	/// ASYNC-P2-8 A3. Parse <c>AsyncIteratorMethodBuilder::MoveNext&lt;TStateMachine&gt;</c>.
	/// <para>
	/// Shape differs from <c>AsyncTaskMethodBuilder::Start&lt;SM&gt;</c> only in the method
	/// name — both take exactly one state-machine type argument and no builder result type
	/// (the iterator builder is non-generic; its element type lives on the state machine).
	/// So the existing <c>Start</c> parser is reused with the method name swapped, rather
	/// than duplicating the generic-argument scanning.
	/// </para>
	/// </summary>
	private static bool TryParseAsyncIteratorBuilderMoveNextStateMachineType(string callee, out string? stateMachineTypeName)
	{
		return TryParseAsyncBuilderStateMachineTypeArg(
			callee, AsyncIteratorBuilderTypePrefix, "MoveNext", out stateMachineTypeName);
	}

	/// <summary>
	/// ASYNC-P2-8 A3. Parse
	/// <c>AsyncIteratorMethodBuilder::AwaitOnCompleted&lt;TAwaiter,TStateMachine&gt;</c> /
	/// <c>::AwaitUnsafeOnCompleted&lt;TAwaiter,TStateMachine&gt;</c>.
	/// <para>
	/// Deliberately does NOT delegate to <see cref="TryParseAsyncBuilderAwaitUnsafeOnCompleted"/>:
	/// that helper hardcodes the method name <c>AwaitUnsafeOnCompleted</c> and would therefore
	/// silently fail on <c>AwaitOnCompleted</c>. Both must parse — the iterator state machine
	/// emits whichever the awaiter permits, and missing one leaves that await unresolvable.
	/// </para>
	/// </summary>
	private static bool TryParseAsyncIteratorBuilderAwaitOnCompleted(
		string callee, out string? awaiterTypeName, out string? stateMachineTypeName)
	{
		awaiterTypeName = null;
		stateMachineTypeName = null;
		foreach (string methodName in new[] { "AwaitOnCompleted", "AwaitUnsafeOnCompleted" })
		{
			string marker = $"{AsyncIteratorBuilderTypePrefix}::{methodName}<";
			if (TryReadGenericArgumentList(callee, marker, out string genericArgumentList))
			{
				IReadOnlyList<string> args = SplitTopLevelGenericArguments(genericArgumentList);
				if (args.Count == 2)
				{
					awaiterTypeName = args[0];
					stateMachineTypeName = args[1];
					return true;
				}
			}
		}
		return false;
	}

	/// <summary>
	/// Parse a <c>Builder::MethodName&lt;TStateMachine&gt;</c> callee down to its single
	/// state-machine type argument. Shared by the iterator <c>MoveNext</c> parser; kept
	/// separate from the literal-reading <c>Start</c> parser so the two can diverge if the
	/// managed shape ever changes.
	/// </summary>
	private static bool TryParseAsyncBuilderStateMachineTypeArg(
		string callee, string builderTypePrefix, string methodName, out string? stateMachineTypeName)
	{
		stateMachineTypeName = null;
		string marker = $"{builderTypePrefix}::{methodName}<";
		if (!TryReadGenericArgumentList(callee, marker, out string genericArgumentList))
		{
			return false;
		}
		IReadOnlyList<string> args = SplitTopLevelGenericArguments(genericArgumentList);
		if (args.Count != 1)
		{
			return false;
		}
		stateMachineTypeName = args[0];
		return true;
	}

	private static bool TryParseAsyncBuilderStartStateMachineType(string callee, string openGenericBuilderTypePrefix, out string? builderResultTypeName, out string? stateMachineTypeName)
	{
		builderResultTypeName = null;
		stateMachineTypeName = null;
		// Try generic form: AsyncTaskMethodBuilder<T>::Start<SM>(...) with <T> generic type arg
		if (TryReadSingleGenericTypeArgument(callee, openGenericBuilderTypePrefix + "<", out string builderResultTypeNameValue))
		{
			string marker = $"{openGenericBuilderTypePrefix}<{builderResultTypeNameValue}>::Start<";
			if (TryReadGenericArgumentList(callee, marker, out string genericArgumentList))
			{
				IReadOnlyList<string> readOnlyList = SplitTopLevelGenericArguments(genericArgumentList);
				if (readOnlyList.Count == 1)
				{
					builderResultTypeName = builderResultTypeNameValue;
					stateMachineTypeName = readOnlyList[0];
					return true;
				}
			}
		}
		// Fallback: non-generic form AsyncTaskMethodBuilder::Start<SM>(...) (no <T>)
		string marker2 = $"{openGenericBuilderTypePrefix}::Start<";
		if (TryReadGenericArgumentList(callee, marker2, out string genericArgumentList2))
		{
			IReadOnlyList<string> readOnlyList2 = SplitTopLevelGenericArguments(genericArgumentList2);
			if (readOnlyList2.Count == 1)
			{
				builderResultTypeName = string.Empty; // non-generic builder
				stateMachineTypeName = readOnlyList2[0];
				return true;
			}
		}
		return false;
	}

	private static bool TryParseAsyncBuilderAwaitUnsafeOnCompleted(string callee, string openGenericBuilderTypePrefix, out string? builderResultTypeName, out string? awaiterTypeName, out string? stateMachineTypeName)
	{
		builderResultTypeName = null;
		awaiterTypeName = null;
		stateMachineTypeName = null;
		// Try generic form: AsyncTaskMethodBuilder<T>::AwaitUnsafeOnCompleted<TAwaiter,SM>(...) with <T>
		if (TryReadSingleGenericTypeArgument(callee, openGenericBuilderTypePrefix + "<", out string builderResultTypeNameValue))
		{
			string marker = $"{openGenericBuilderTypePrefix}<{builderResultTypeNameValue}>::AwaitUnsafeOnCompleted<";
			if (TryReadGenericArgumentList(callee, marker, out string genericArgumentList))
			{
				IReadOnlyList<string> readOnlyList = SplitTopLevelGenericArguments(genericArgumentList);
				if (readOnlyList.Count == 2)
				{
					builderResultTypeName = builderResultTypeNameValue;
					awaiterTypeName = readOnlyList[0];
					stateMachineTypeName = readOnlyList[1];
					return true;
				}
			}
		}
		// Fallback: non-generic form AsyncTaskMethodBuilder::AwaitUnsafeOnCompleted<TAwaiter,SM>(...) (no <T>)
		string marker2 = $"{openGenericBuilderTypePrefix}::AwaitUnsafeOnCompleted<";
		if (TryReadGenericArgumentList(callee, marker2, out string genericArgumentList2))
		{
			IReadOnlyList<string> readOnlyList2 = SplitTopLevelGenericArguments(genericArgumentList2);
			if (readOnlyList2.Count == 2)
			{
				builderResultTypeName = string.Empty;
				awaiterTypeName = readOnlyList2[0];
				stateMachineTypeName = readOnlyList2[1];
				return true;
			}
		}
		return false;
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
		if (string.IsNullOrEmpty(value))
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
		return !string.IsNullOrEmpty(typeSubjectId);
	}

	private static bool TryParseDefaultInterpolatedStringHandlerAppendFormattedType(string subjectId, out string? formattedTypeDisplayName)
	{
		formattedTypeDisplayName = null;
		if (!TryReadSingleGenericTypeArgument(subjectId, DefaultInterpolatedStringHandlerAppendFormattedMethodSubjectPrefix, out string formattedTypeName) || !string.Equals(GetMethodDeclaringTypeSubjectId(subjectId), "System.Private.CoreLib/System.Runtime.CompilerServices.DefaultInterpolatedStringHandler", StringComparison.Ordinal) || !GetMethodName(subjectId).StartsWith("AppendFormatted<", StringComparison.Ordinal) || !GetMethodParameterTypes(subjectId).SequenceEqual(new string[1] { formattedTypeName }))
		{
			return false;
		}
		formattedTypeDisplayName = GetTypeDisplayName(formattedTypeName);
		return !string.IsNullOrEmpty(formattedTypeDisplayName);
	}

	private static bool IsSupportedDefaultInterpolatedStringHandlerAppendFormattedSubjectId(string subjectId)
	{
		return TryParseDefaultInterpolatedStringHandlerAppendFormattedType(subjectId, out string? formattedTypeDisplayName) && string.Equals(formattedTypeDisplayName, "System.Int32", StringComparison.Ordinal);
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
			elementExpression = "static_cast<CHAOS_IL2CPP_INT32>(" + slotExpression + ")";
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
		if (TryCreateNativeIntBackedValueTypeAbiSlot(typeDisplayName, typeNameOrSubjectId, out abiSlot))
		{
			return true;
		}
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
			abiSlot = default!;
			return false;
		}
		}
	}

	private bool TryCreateNativeIntBackedValueTypeAbiSlot(string typeDisplayName, string typeNameOrSubjectId, out AotCoreIrAbiSlotArtifact abiSlot)
	{
		switch (typeDisplayName)
		{
		case "System.IntPtr":
		case "System.UIntPtr":
		case "System.RuntimeFieldHandle":
		case "System.RuntimeMethodHandle":
		case "System.RuntimeTypeHandle":
			if (TryResolveKnownTypeSubjectId(typeNameOrSubjectId, out string subjectId))
			{
				abiSlot = CreateNativeIntAbiSlot(subjectId, AotCoreIrTypeShapeKind.ValueType);
				return true;
			}
			abiSlot = CreateNativeIntAbiSlot();
			return true;
		default:
			abiSlot = default!;
			return false;
		}
	}

	private bool TryResolveKnownTypeSubjectId(string typeNameOrSubjectId, out string subjectId)
	{
		if (string.IsNullOrEmpty(typeNameOrSubjectId))
		{
			subjectId = string.Empty;
			return false;
		}
		if (typeNameOrSubjectId.Contains('/', StringComparison.Ordinal))
		{
			subjectId = typeNameOrSubjectId;
			return true;
		}
		string[] array = (from candidate in _methodsBySubjectId.Keys.Select(GetMethodDeclaringTypeSubjectId).Concat(_valueTypeSubjectIds).Distinct<string>(StringComparer.Ordinal)
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
				if (string.IsNullOrEmpty(value))
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
		string? value = subjectId;
		while (!string.IsNullOrEmpty(value))
		{
			string valueDisplayName = ManagedNaming.GetTypeDisplayNameFromSubjectId(value);
			if (string.Equals(valueDisplayName, "System.Delegate", StringComparison.Ordinal) || string.Equals(valueDisplayName, "System.MulticastDelegate", StringComparison.Ordinal))
			{
				return true;
			}
			if (!referenceTypeBaseSubjectIds.TryGetValue(value, out string? nextValue))
			{
				break;
			}
			value = nextValue;
		}
		string subjectIdDisplayName = ManagedNaming.GetTypeDisplayNameFromSubjectId(subjectId);
		if (subjectIdDisplayName.StartsWith("System.Action", StringComparison.Ordinal) || subjectIdDisplayName.StartsWith("System.Func", StringComparison.Ordinal))
		{
			return true;
		}
		return false;
	}

	private string? ResolveReferenceTypeBaseSubjectId(string subjectId, string? baseTypeSubjectId)
	{
		if (!string.IsNullOrEmpty(baseTypeSubjectId))
		{
			return baseTypeSubjectId;
		}
		if (_referenceTypeBaseSubjectIds.TryGetValue(subjectId, out string? value) && !string.IsNullOrEmpty(value))
		{
			return value;
		}
		return GetSyntheticReferenceTypeBaseSubjectId(subjectId);
	}

	private static string? GetSyntheticReferenceTypeBaseSubjectId(string subjectId)
	{
		string subjectIdDisplayName = ManagedNaming.GetTypeDisplayNameFromSubjectId(subjectId);
		if (string.Equals(subjectIdDisplayName, "System.Delegate", StringComparison.Ordinal))
		{
			return "System.Private.CoreLib/System.Object";
		}
		if (string.Equals(subjectIdDisplayName, "System.MulticastDelegate", StringComparison.Ordinal))
		{
			return "System.Private.CoreLib/System.Delegate";
		}
		if (subjectIdDisplayName.StartsWith("System.Action", StringComparison.Ordinal) || subjectIdDisplayName.StartsWith("System.Func", StringComparison.Ordinal))
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
		switch (typeNameOrSubjectId)
		{
		case "System.String":
			subjectId = "System.Private.CoreLib/System.String";
			return true;
		case "System.Object":
			subjectId = "System.Private.CoreLib/System.Object";
			return true;
		case "System.Exception":
			subjectId = "System.Private.CoreLib/System.Exception";
			return true;
		case "System.Type":
			subjectId = "System.Private.CoreLib/System.Type";
			return true;
		}
		subjectId = string.Empty;
		return false;
	}

}
