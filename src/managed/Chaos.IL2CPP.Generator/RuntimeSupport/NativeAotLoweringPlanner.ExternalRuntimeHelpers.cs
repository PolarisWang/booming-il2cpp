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

internal sealed class _003C_003Ez__ReadOnlyArray<T>(T[] items) : IReadOnlyList<T>
{
	private readonly T[] _items = items;

	public T this[int index] => _items[index];

	public int Count => _items.Length;

	public IEnumerator<T> GetEnumerator()
	{
		return ((IEnumerable<T>)_items).GetEnumerator();
	}

	IEnumerator IEnumerable.GetEnumerator()
	{
		return _items.GetEnumerator();
	}
}

internal sealed class _003C_003Ez__ReadOnlySingleElementList<T>(T item) : IReadOnlyList<T>
{
	private readonly T _item = item;

	public T this[int index] => index == 0 ? _item : throw new ArgumentOutOfRangeException(nameof(index));

	public int Count => 1;

	public IEnumerator<T> GetEnumerator()
	{
		yield return _item;
	}

	IEnumerator IEnumerable.GetEnumerator()
	{
		return GetEnumerator();
	}
}

public sealed partial class NativeAotLoweringPlanner
{
	/// <summary>
	/// Generates AOT Core IR JSON for a crypto method from the managed method model.
	/// The JSON includes full instruction data (op, callee, operand, IlOffset, ResultType,
	/// Reference, ConstrainedTypeSubjectId) for interpreter routing.  Returns null when
	/// the method is not found in _allManagedMethods.
	/// </summary>
	private string? TryBuildCryptoAotIrJson(string callee)
	{
		if (_allManagedMethods == null ||
			callee.IndexOf("System.Security.Cryptography/", StringComparison.Ordinal) < 0)
			return null;

		if (!_allManagedMethods.TryGetValue(callee, out var mm) || mm?.Body?.Blocks == null)
			return null;

		StringBuilder? sb = null;
		bool first = true;
		foreach (var blk in mm.Body.Blocks)
		{
			foreach (var inst in blk.Instructions)
			{
				if (sb == null)
				{
					sb = new StringBuilder();
					sb.Append("{\"subjectId\":\"");
					sb.Append(callee.Replace("\\", "\\\\").Replace("\"", "\\\""));
					sb.Append("\",\"instructions\":[");
				}
				if (!first) sb.Append(",");
				first = false;
				sb.Append("{\"op\":\"");
				sb.Append(inst.Op.Replace("\\", "\\\\").Replace("\"", "\\\""));
				sb.Append("\"");
				if (inst.Callee != null)
				{
					sb.Append(",\"callee\":\"");
					sb.Append(inst.Callee.Replace("\\", "\\\\").Replace("\"", "\\\""));
					sb.Append("\"");
				}
				if (inst.Operand != null)
				{
					sb.Append(",\"operand\":\"");
					var operandStr = inst.Operand.ToString();
					if (operandStr != null)
						sb.Append(operandStr.Replace("\\", "\\\\").Replace("\"", "\\\""));
					sb.Append("\"");
				}
				if (inst.IlOffset.HasValue)
				{
					sb.Append(",\"ilOffset\":");
					sb.Append(inst.IlOffset.Value);
				}
				if (inst.ResultType != null)
				{
					sb.Append(",\"resultType\":\"");
					sb.Append(inst.ResultType.Replace("\\", "\\\\").Replace("\"", "\\\""));
					sb.Append("\"");
				}
				if (inst.Reference != null)
				{
					sb.Append(",\"reference\":{");
					sb.Append("\"assemblyName\":\"");
					sb.Append(inst.Reference.AssemblyName.Replace("\\", "\\\\").Replace("\"", "\\\""));
					sb.Append("\",\"subjectKind\":\"");
					sb.Append(inst.Reference.SubjectKind.Replace("\\", "\\\\").Replace("\"", "\\\""));
					sb.Append("\",\"subjectId\":\"");
					sb.Append(inst.Reference.SubjectId.Replace("\\", "\\\\").Replace("\"", "\\\""));
					sb.Append("\"}");
				}
				if (inst.ConstrainedTypeSubjectId != null)
				{
					sb.Append(",\"constrainedTypeSubjectId\":\"");
					sb.Append(inst.ConstrainedTypeSubjectId.Replace("\\", "\\\\").Replace("\"", "\\\""));
					sb.Append("\"");
				}
				sb.Append("}");
			}
		}

		if (sb == null) return null;
		sb.Append("]}");
		return sb.ToString();
	}

	private bool TryCreateExternalRuntimeHelperDefinition(string callee, out ExternalRuntimeHelperDefinition? helperDefinition)
	{
		// Canonicalize assembly prefix so matching is assembly-agnostic
		callee = ManagedNaming.NormalizeSubjectIdAssembly(callee);

		// === Generic shape dispatch via Registry (check BEFORE _methodsBySubjectId) ===
		// NormalizeSubjectIdAssembly may change System.Numerics.Vectors -> System.Private.CoreLib,
		// making the method appear in _methodsBySubjectId with instructions.  Generic shape
		// entries (RegisterVectorReduction, etc.) must be checked first so they win over the
		// AOT IR path — otherwise Vector<T>.GreaterThanAll etc. never reach TryMatchGenericShape.
		if (callee.Contains("Vector", StringComparison.Ordinal))
			System.Console.Error.WriteLine($"[VECTOR_DEBUG] TryMatchGenericShape called for callee={callee}");
		if (_shapeRegistry.TryMatchGenericShape(callee, out var genericDescriptor, out var typeArgs))
		{
			var resolution = genericDescriptor.Resolver(this, callee, typeArgs);
			if (resolution != null)
			{
				System.Console.Error.WriteLine($"[VECTOR_REDUCTION] MATCHED callee={callee} symbol={resolution.Symbol} directNative={resolution.DirectNativeSymbol}");
				helperDefinition = new ExternalRuntimeHelperDefinition(
					callee,
					resolution.Symbol,
					resolution.CppSource,
					resolution.ParameterAbis,
					resolution.ReturnAbi,
					resolution.RawArgumentIndices,
					resolution.ReferencedStaticFieldSubjectIds,
					DirectNativeSymbol: resolution.DirectNativeSymbol,
					DirectNativeHeader: resolution.DirectNativeHeader);
								_externalRuntimeHelperCache[callee] = helperDefinition;
				return true;
			}
		}

		// If method compiled in AOT IR (has IL body instructions), use its real ParameterAbis.
		// Methods in _methodsBySubjectId with 0 instructions are BCL/import methods that
		// don't have compiled bodies -- they should fall through to external runtime stubs.
		// --- Crypto AOT IR data collection (run BEFORE early return) ---
		// Collect rich AOT IR JSON data for crypto methods that are not AOT-compiled.
		// This data is embedded in kChaosExternalRuntimeIlData[] via BuildExternalRuntimeDispatchTable
		// and used by the interpreter at runtime when dispatching through InterpreterEntryDirect.
		var crCryptoJson = TryBuildCryptoAotIrJson(callee);
		// Add to crypto AOT IR entries immediately (BEFORE early return)
		// so the interpreter has IL data for methods that cannot be AOT-compiled.
		if (crCryptoJson != null)
		{
			_cryptoAotIrEntries.Add((callee, crCryptoJson));
		}

		if (_methodsBySubjectId.TryGetValue(callee, out var existingMethod) &&
			existingMethod is { Instructions.Count: > 0 })
		{
			helperDefinition = null;
			_externalRuntimeHelperCache[callee] = null;
			return false;
		}
		// Check cache first (P0)
		if (_externalRuntimeHelperCache.TryGetValue(callee, out var cached))
		{
			helperDefinition = cached;
			return cached != null;
		}

		// === Simple forward shape dispatch (native function call wrapper) ===
		if (_shapeRegistry.TryMatchShape(callee, out var shapeEntry) &&
			shapeEntry.Kind == RuntimeHelperShapeRegistry.ShapeKind.SimpleForward)
		{
			helperDefinition = CreateDefinitionFromShapeEntry(callee, shapeEntry);
								_externalRuntimeHelperCache[callee] = helperDefinition;
					return true;
		}

		if (TryCreateCustomAttributeRuntimeHelperDefinition(callee, out helperDefinition))
		{
								_externalRuntimeHelperCache[callee] = helperDefinition;
					return true;
		}
		// Catch-all: generate ChaosExternalRuntimeFallback stub for any unmatched callee.
		// Prevents undefined-chaos_external_runtime_* C++ symbol errors and CHAOS_IL2CPP_FAIL.
		// Returns type-appropriate defaults (0/nullptr) via the runtime fallback function.

		// --- Crypto AOT IR data collection (catch-all fallback) ---
		// If the crypto method was not captured by the early-return path above (e.g.,
		// _methodsBySubjectId lookup succeeded but with 0 instructions), try again here.
		if (crCryptoJson == null)
		{
			crCryptoJson = TryBuildCryptoAotIrJson(callee);
			if (crCryptoJson != null)
			{
				_cryptoAotIrEntries.Add((callee, crCryptoJson));
			}
		}
		var failReturnType = InferReturnTypeFromSubjectId(callee);
		var failReturnAbi = !string.IsNullOrEmpty(failReturnType)
			? CreateLegacyAbiSlot(failReturnType)
			: CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ValueType);
		var failSymbol = GetExternalRuntimeHelperSymbol(callee);
		string escapedCallee = callee.Replace("\\", "\\\\").Replace("\"", "\\\"");
		if (failReturnAbi.CarrierKindCode == AotCoreIrAbiCarrierKind.Void)
		{
			var src = RenderSimpleExternalRuntimeHelper("void", failSymbol, "",
				["    ChaosExternalRuntimeFallback(\"" + escapedCallee + "\");"]);
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, failSymbol, src,
				Array.Empty<AotCoreIrAbiSlotArtifact>(), failReturnAbi, EmptyRawArgumentIndices);
		}
		else
		{
			var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", failSymbol, "",
				["    return ChaosExternalRuntimeFallback(\"" + escapedCallee + "\");"]);
			helperDefinition = new ExternalRuntimeHelperDefinition(callee, failSymbol, src,
				Array.Empty<AotCoreIrAbiSlotArtifact>(), failReturnAbi, EmptyRawArgumentIndices);
		}
		_externalRuntimeHelperCache[callee] = helperDefinition;
		return true;

	}

	private ExternalRuntimeHelperDefinition CreateDefinitionFromShapeEntry(
		string callee,
		RuntimeHelperShapeRegistry.ShapeEntry entry)
	{
		var symbol = GetExternalRuntimeHelperSymbol(callee);
		var returnType = MapAbiSlotReturnType(entry.ReturnAbi);
		var parameterSignature = FormatAbiSlotParameterSignature(entry.ParameterAbis);
		var argCount = entry.ParameterAbis.Count;
		var args = argCount == 0 ? string.Empty :
			string.Join(", ", Enumerable.Range(0, argCount).Select(i => $"chaos_fn_arg_{i}"));
		var bodyLines = entry.ReturnAbi.CarrierKindCode == AotCoreIrAbiCarrierKind.Void
			? new[] { $"    {entry.NativeFnSymbol}({args});" }
			: new[] { $"    return {entry.NativeFnSymbol}({args});" };
		return new ExternalRuntimeHelperDefinition(callee, symbol,
			RenderSimpleExternalRuntimeHelper(returnType, symbol, parameterSignature, bodyLines),
			entry.ParameterAbis, entry.ReturnAbi, entry.RawArgumentIndices ?? EmptyRawArgumentIndices,
			entry.ReferencedStaticFieldSubjectIds,
			DirectNativeSymbol: entry.NativeFnSymbol);
	}

}
