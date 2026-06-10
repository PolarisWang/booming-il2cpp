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

		// --- Crypto AOT IR data collection ---
		ManagedMethodModel? _crMm = null;
		if (callee.IndexOf("System.Security.Cryptography/", System.StringComparison.Ordinal) >= 0 && _allManagedMethods != null &&
		    _allManagedMethods.TryGetValue(callee, out _crMm))
		{
			var _sb = new System.Text.StringBuilder();
			_sb.Append("[");
			bool _first = true;
			foreach (var _blk in _crMm.Body.Blocks)
				foreach (var _inst in _blk.Instructions)
				{
					if (!_first) _sb.Append(",");
					_first = false;
					_sb.Append("{\"op\":\"" + _inst.Op + "\"");
					if (_inst.Callee != null)
						_sb.Append(",\"callee\":\"" + _inst.Callee + "\"");
					if (_inst.Operand != null)
						_sb.Append(",\"operand\":\"" + _inst.Operand + "\"");
					_sb.Append("}");
				}
			_sb.Append("]");
			var _json = "{\"subjectId\":\"" + callee + "\",\"instructions\":" + _sb + "}";
			_cryptoAotIrEntries.Add((callee, _json));
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
