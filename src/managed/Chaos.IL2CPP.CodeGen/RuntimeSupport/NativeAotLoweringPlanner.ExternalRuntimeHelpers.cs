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
		if (TryCreateCustomAttributeRuntimeHelperDefinition(callee, out helperDefinition))
		{
			return true;
		}
		if (TryCreateAsyncRuntimeHelperDefinition(callee, out helperDefinition))
		{
			return true;
		}
		if (TryCreateStringRuntimeHelperDefinition(callee, out helperDefinition))
		{
			return true;
		}
		if (TryCreateExceptionRuntimeHelperDefinition(callee, out helperDefinition))
		{
			return true;
		}
		if (TryCreateNumericFormattingRuntimeHelperDefinition(callee, out helperDefinition))
		{
			return true;
		}
		if (TryCreateInterpolatedStringRuntimeHelperDefinition(callee, out helperDefinition))
		{
			return true;
		}
		if (TryCreatePlatformRuntimeHelperDefinition(callee, out helperDefinition))
		{
			return true;
		}
		if (TryCreateDelegateInteropRuntimeHelperDefinition(callee, out helperDefinition))
		{
			return true;
		}
		if (TryCreateMarshalingRuntimeHelperDefinition(callee, out helperDefinition))
		{
			return true;
		}
		if (TryCreateInterlockedRuntimeHelperDefinition(callee, out helperDefinition))
		{
			return true;
		}
		if (TryCreateSpanRuntimeHelperDefinition(callee, out helperDefinition))
		{
			return true;
		}
		if (TryCreateCollectionRuntimeHelperDefinition(callee, out helperDefinition))
		{
			return true;
		}
		if (TryCreateObjectRuntimeHelperDefinition(callee, out helperDefinition))
		{
			return true;
		}
		return TryCreateReflectionRuntimeHelperDefinition(callee, out helperDefinition);
	}

}
