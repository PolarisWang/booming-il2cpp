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


		// === Generic shape dispatch via Registry (runs first -- GenericShapeDescriptor resolves
		//     complex shapes with custom body logic that cannot be expressed as a simple forward) ===
		if (_shapeRegistry.TryMatchGenericShape(callee, out var genericDescriptor, out var typeArgs))
		{
			var resolution = genericDescriptor.Resolver(this, callee, typeArgs);
			if (resolution != null)
			{
				helperDefinition = new ExternalRuntimeHelperDefinition(
					callee,
					resolution.Symbol,
					resolution.CppSource,
					resolution.ParameterAbis,
					resolution.ReturnAbi,
					resolution.RawArgumentIndices,
					resolution.ReferencedStaticFieldSubjectIds);
				return true;
			}
		}

		// === Simple forward shape dispatch (native function call wrapper) ===
		if (_shapeRegistry.TryMatchShape(callee, out var shapeEntry) &&
			shapeEntry.Kind == RuntimeHelperShapeRegistry.ShapeKind.SimpleForward)
		{
			helperDefinition = CreateDefinitionFromShapeEntry(callee, shapeEntry);
			return true;
		}

		if (TryCreateCustomAttributeRuntimeHelperDefinition(callee, out helperDefinition))
		{
			return true;
		}
		return false;
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
			entry.ReferencedStaticFieldSubjectIds);
	}

}
