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

	/// <summary>
	/// Generates AOT Core IR JSON for any managed method from _allManagedMethods,
	/// regardless of assembly.  Used in the catch-all fallback path of
	/// TryCreateExternalRuntimeHelperDefinition to provide interpreter-executable
	/// JSON for BCL/referenced-assembly methods that lack AOT lowering plans.
	/// Returns null when the method is not found in _allManagedMethods.
	/// </summary>
	private string? TryBuildExternalRuntimeAotIrJson(string callee)
	{
		if (_allManagedMethods == null)
			return null;

		// Skip methods that already have AOT lowering plans — they compile to
		// dedicated chaos_external_runtime_* functions at the C++ level, so
		// embedding interpreter JSON would create conflicting declarations.
		if (_methodsBySubjectId.ContainsKey(callee))
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
		string originalCallee = callee;

		// Canonicalize assembly prefix so matching is assembly-agnostic
		callee = ManagedNaming.NormalizeSubjectIdAssembly(callee);

		// A2-1 (§15/§17): Vector2/3/4 exact non-generic `_All` reducers MUST match here,
		// BEFORE the generic "System.Numerics.Vector<T>" shape (RegisterVectorReduction).
		// That generic shape's TypeDisplayNamePrefix "System.Numerics.Vector" prefix-matches
		// Vector2/3/4 and routes them to the Vector<T> 256-bit helper (wrong carrier ABI →
		// runtime throw).  Vector2/3/4 need the carrier-specific lane-reducer helper.
		if (TryCreateVectorAllComparerHelper(callee, out helperDefinition))
		{
							_externalRuntimeHelperCache[callee] = helperDefinition;
			return true;
		}

		// === Generic shape dispatch via Registry (check BEFORE _methodsBySubjectId) ===
		// NormalizeSubjectIdAssembly may change System.Numerics.Vectors -> System.Private.CoreLib,
		// making the method appear in _methodsBySubjectId with instructions.  Generic shape
		// entries (RegisterVectorReduction, etc.) must be checked first so they win over the
		// AOT IR path — otherwise Vector<T>.GreaterThanAll etc. never reach TryMatchGenericShape.
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
					resolution.ReferencedStaticFieldSubjectIds,
					DirectNativeSymbol: resolution.DirectNativeSymbol,
					DirectNativeHeader: resolution.DirectNativeHeader);
								_externalRuntimeHelperCache[callee] = helperDefinition;
				return true;
			}
		}

		// If method has an AOT lowering plan, skip catch-all fallback generation.
		// The AOT lowering already generates chaos_external_runtime_* declarations,
		// so the catch-all would create conflicting C++ function declarations.
		// --- Crypto AOT IR data collection (run BEFORE early return) ---
		// Collect rich AOT IR JSON data for crypto methods that are not AOT-compiled.
		// This data is embedded in kChaosExternalRuntimeIlData[] via BuildExternalRuntimeDispatchTable
		// and used by the interpreter at runtime when dispatching through InterpreterEntryDirect.
		var crCryptoJson = TryBuildCryptoAotIrJson(callee);
		// Add to crypto AOT IR entries immediately (BEFORE early return)
		// so the interpreter has IL data for methods that cannot be AOT-compiled.
		if (crCryptoJson != null)
			_cryptoAotIrEntries.Add((callee, crCryptoJson));

		if (_methodsBySubjectId.ContainsKey(callee) && IsSameModuleMethod(callee))
		{
			helperDefinition = null;
			_externalRuntimeHelperCache[callee] = null;
			return false;
		}
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
		// A2-1 (§15): Vector2/3/4 non-generic `_All` reducers route through the external
		// table → fallback → interpreter/throwing.  Emit a native helper that unpacks the
		// two INTPTR-encoded carriers and calls the runtime lane-reducer → returns 1/0.
		if (TryCreateVectorAllComparerHelper(callee, out helperDefinition))
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

		// --- External Runtime AOT IR data collection (catch-all fallback) ---
		// Build AOT Core IR JSON for ANY method that has a managed method model
		// in _allManagedMethods, not just crypto methods.  This JSON data is
		// embedded in kChaosExternalRuntimeIlData[].json_data via
		// BuildExternalRuntimeDispatchTable and used by the interpreter at runtime
		// to dispatch through InterpreterEntryDirect (_TryExecuteViaIlData Phase 1).
		// This enables BCL/referenced-assembly methods to execute correctly even
		// when they lack AOT lowering plans in _methodsBySubjectId.
		var externalIrJson = TryBuildExternalRuntimeAotIrJson(callee);
		if (externalIrJson != null)
		{
			_externalRuntimeIlDataJson[callee] = externalIrJson;
		}
		var failReturnType = InferReturnTypeFromSubjectId(callee);
		var failReturnAbi = !string.IsNullOrEmpty(failReturnType)
			? CreateLegacyAbiSlot(failReturnType)
			: CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ValueType);
		var failSymbol = GetExternalRuntimeHelperSymbol(callee);
		string escapedCallee = callee.Replace("\\", "\\\\").Replace("\"", "\\\"");
		// All catch-all fallback functions use () regardless of actual method params
		// because call sites pass 0 args and the body uses hardcoded subject ID.
		var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", failSymbol, "",
			["    return ChaosExternalRuntimeFallback(\"" + escapedCallee + "\");"]);
		helperDefinition = new ExternalRuntimeHelperDefinition(callee, failSymbol, src,
			Array.Empty<AotCoreIrAbiSlotArtifact>(), failReturnAbi, EmptyRawArgumentIndices);
		_externalRuntimeHelperCache[callee] = helperDefinition;
		return true;

	}

	private bool TryCreateVectorAllComparerHelper(string callee, out ExternalRuntimeHelperDefinition? helperDefinition)
	{
		helperDefinition = null;
		// Match two forms reaching the external table:
		//  (A) named:  System.Numerics.Vectors/System.Numerics.Vector[234]::GreaterThanAll:...
		//  (B) generic: System.Numerics.Vectors/System.Numerics.Vector<System.Int32>::GreaterThanAll:...
		// Detect the carried reduce method and pick the correct native lane-reducer.
		string? carrier = null;      // RuntimeNumericsVector{2,3,4}Carrier  (named form)
		string? nativeFn = null;     // Vector2GreaterThanAll  (named)  or  chaos_vector_greater_than_all_i32  (generic)
		string? methodKey = null;    // GreaterThanAll / GreaterThanOrEqualAll / LessThanAll / LessThanOrEqualAll / EqualsAll / EqualsAny
		string? elemType = null;     // generic element type ("System.Int32" → "i32") when generic form

		var mNamed = System.Text.RegularExpressions.Regex.Match(
			callee, @"System\.Numerics\.Vector([234])::(GreaterThanAll|GreaterThanOrEqualAll|LessThanAll|LessThanOrEqualAll|EqualsAll|EqualsAny):");
		var mGeneric = System.Text.RegularExpressions.Regex.Match(
			callee, @"System\.Numerics\.Vector<([^>]+)>::(GreaterThanAll|GreaterThanOrEqualAll|LessThanAll|LessThanOrEqualAll|EqualsAll|EqualsAny):");

		if (mNamed.Success)
		{
			var dim = mNamed.Groups[1].Value;
			methodKey = mNamed.Groups[2].Value;
			carrier = dim switch { "2" => "RuntimeNumericsVector2Carrier", "3" => "RuntimeNumericsVector3Carrier", _ => "RuntimeNumericsVector4Carrier" };
			nativeFn = $"Vector{dim}{methodKey}";     // Vector2GreaterThanAll — landed in numerics_vectors.cpp
		}
		else if (mGeneric.Success)
		{
			elemType = mGeneric.Groups[1].Value.Trim();
			methodKey = mGeneric.Groups[2].Value;
			// Map element type → native stub suffix (matches vector_stubs.cpp VECTOR_REDUCTION_STUBS).
			var suffix = elemType switch
			{
				var t when t.Contains("System.Int64") => "i64",
				var t when t.Contains("System.UInt64") => "u64",
				var t when t.Contains("System.UInt32") => "u32",
				var t when t.Contains("System.Single") => "f",
				var t when t.Contains("System.Double") => "d",
				var t when t.Contains("System.Int16") => "i16",
				var t when t.Contains("System.UInt16") => "u16",
				var t when t.Contains("System.Byte") => "u8",
				var t when t.Contains("System.SByte") => "i8",
				_ => "i32",   // default incl System.Int32
			};
			// chaos_vector_<methodsnake>_<suffix>(INTPTR,INTPTR) — Vector256 carrier by pointer.
			var snake = methodKey switch
			{
				"GreaterThanAll" => "greater_than_all",
				"GreaterThanOrEqualAll" => "greater_than_or_equal_all",
				"LessThanAll" => "less_than_all",
				"EqualsAll" => "equals_all",
				"EqualsAny" => "equals_any",
				_ => "less_than_or_equal_all",
			};
			nativeFn = $"chaos_vector_{snake}_{suffix}";
			carrier = "RuntimeIntrinsicVector256Carrier";
		}
		if (nativeFn == null || carrier == null)
			return false;

		var symbol = GetExternalRuntimeHelperSymbol(callee);
		// body: two INTPTR args point at carriers.  Unpack → call native lane-reducer → 1/0.
		// Named form (Vector2/3/4): native takes carrier by value → deref the INTPTR.
		// Generic form (Vector<T>): chaos_vector_*_suffix takes INTPTR pointers → pass through.
		var callExpr = (mNamed.Success)
			? $"chaos::il2cpp::runtime_core::{nativeFn}(*chaos_a, *chaos_b)"
			: $"chaos::il2cpp::runtime_core::{nativeFn}(chaos_arg_0, chaos_arg_1)";
		var src = RenderSimpleExternalRuntimeHelper(
			"CHAOS_IL2CPP_INT32", symbol,
			"CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
			new[]
			{
				$"    const auto* chaos_a = reinterpret_cast<const chaos::il2cpp::runtime_core::{carrier}*>(chaos_arg_0);",
				$"    const auto* chaos_b = reinterpret_cast<const chaos::il2cpp::runtime_core::{carrier}*>(chaos_arg_1);",
				$"    return {callExpr};",
			});
		helperDefinition = new ExternalRuntimeHelperDefinition(callee, symbol, src,
			new AotCoreIrAbiSlotArtifact[]
			{
				CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ValueType),
				CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ValueType),
			},
			CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ValueType),
			EmptyRawArgumentIndices);
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
