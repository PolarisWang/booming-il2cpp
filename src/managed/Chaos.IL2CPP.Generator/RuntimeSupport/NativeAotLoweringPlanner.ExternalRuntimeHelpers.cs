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

		// Primary path: method body IR from the managed world model (reachable closure).
		if (_allManagedMethods.TryGetValue(callee, out var mm) && mm?.Body?.Blocks != null)
		{
			return TryBuildExternalRuntimeAotIrJsonFromBlocks(callee, mm);
		}

		// Fallback path: BCL / referenced-assembly method bodies are NOT in the
		// reachable closure (they are cross-assembly callees, not reachable methods).
		// Read the method body directly from the on-disk assembly (same source the
		// kChaosExternalRuntimeIlData s_il_* bytes come from) and lower the raw IL
		// to a conservative AOT-IR instruction list the interpreter can execute.
		return TryBuildExternalRuntimeAotIrJsonFromDisk(callee);
	}

	private string? TryBuildExternalRuntimeAotIrJsonFromBlocks(string callee, Contracts.ManagedMethodModel mm)
	{
		StringBuilder? sb = null;
		bool first = true;
		foreach (var blk in mm.Body!.Blocks)
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

	/// <summary>Read the IL body of a BCL method from its on-disk assembly and
	/// lower it to a conservative AOT-IR JSON instruction list the interpreter
	/// can execute. Returns null when the IL is too complex to lower safely
	/// (the caller then falls back to return-0, never worse than today).</summary>
	private string? TryBuildExternalRuntimeAotIrJsonFromDisk(string callee)
	{
		try
		{
			int slashIdx = callee.IndexOf('/');
			if (slashIdx <= 0) return null;
			string assemblyName = callee.Substring(0, slashIdx);
			int methodSep = callee.IndexOf("::", StringComparison.Ordinal);
			if (methodSep <= 0) return null;
			string typePart = callee.Substring(slashIdx + 1, methodSep - slashIdx - 1);

			// Method name + return-type-free signature for token matching.
			string sigPart = callee.Substring(methodSep + 2);
			int colonIdx = sigPart.LastIndexOf(':');
			string methodOnly = colonIdx > 0 ? sigPart.Substring(0, colonIdx) : sigPart;
			int parenIdx = methodOnly.IndexOf('(');
			string methodName = parenIdx > 0 ? methodOnly.Substring(0, parenIdx) : methodOnly;

			string? rtDir = System.IO.Path.GetDirectoryName(typeof(object).Assembly.Location);
			if (rtDir == null) return null;
			string dllPath = System.IO.Path.Combine(rtDir, assemblyName + ".dll");
			if (!System.IO.File.Exists(dllPath)) return null;

			using var peReader = new System.Reflection.PortableExecutable.PEReader(
				System.IO.File.OpenRead(dllPath));
			if (!peReader.HasMetadata) return null;
			var md = peReader.GetMetadataReader();

			// Find type (handle both simple & fully-qualified type names).
			int? methodDefHandle = null;
			foreach (var td in md.TypeDefinitions)
			{
				var tdef = md.GetTypeDefinition(td);
				if (md.GetString(tdef.Name) == "<Module>") continue;
				string ns = tdef.Namespace.IsNil ? "" : md.GetString(tdef.Namespace);
				string tn = md.GetString(tdef.Name);
				string fullName = string.IsNullOrEmpty(ns) ? tn : ns + "." + tn;
				if (fullName != typePart && tn != typePart) continue;
				foreach (var mh in tdef.GetMethods())
				{
					var mdef = md.GetMethodDefinition(mh);
					if (md.GetString(mdef.Name) == methodName && mdef.RelativeVirtualAddress != 0)
					{
						methodDefHandle = MetadataTokens.GetToken(mh);
						break;
					}
				}
				break;
			}
			if (methodDefHandle == null) return null;

			// Locate the MethodDefinition by token to read its body RVA.
			var methodDef = md.GetMethodDefinition(MetadataTokens.MethodDefinitionHandle(methodDefHandle.Value));
			if (methodDef.RelativeVirtualAddress == 0) return null;
			var body = peReader.GetMethodBody(methodDef.RelativeVirtualAddress);
			var ilReader = body.GetILReader();
			byte[] il = ilReader.ReadBytes(ilReader.RemainingBytes);
			if (il.Length == 0) return null;

			// Conservative IL decoder — build IR JSON instructions for the op set
			// the interpreter supports. Unknown/complex IL → null (return-0).
			return LowerRawIlToAotIrJson(callee, il, md);
		}
		catch
		{
			return null;
		}
	}

	/// <summary>Decode raw ECMA-335 IL bytes into the interpreter's AOT-IR JSON
	/// instruction format. Conservative: only supports the flat, single-block
	/// op set that the interpreter's IROpCode handles; anything requiring basic
	/// blocks (branches), exception handling, or unsupported opcodes returns null.</summary>
	private string? LowerRawIlToAotIrJson(string callee, byte[] il, MetadataReader md)
	{
		var sb = new StringBuilder();
		sb.Append("{\"subjectId\":\"");
		sb.Append(callee.Replace("\\", "\\\\").Replace("\"", "\\\""));
		sb.Append("\",\"instructions\":[");
		int pos = 0;
		bool first = true;
		bool unsupported = false;

		while (pos < il.Length && !unsupported)
		{
			byte op1 = il[pos++];
			bool longForm = op1 == 0xFE;
			ushort opCode = longForm ? (ushort)(il[pos++] + 0xFE00) : op1;

			if (first) first = false; else sb.Append(",");
			string mappedOp = MappingOpToJson(opCode);
			// SAFETY: any opcode we cannot map to an interpreter-supported IROpCode
			// must cause the whole method to degrade to return-0 (null).  Emitting a
			// JSON with an "unknown" op could make the interpreter produce a wrong
			// (silently incorrect) result or crash — strictly worse than return-0.
			if (mappedOp == "unknown") { unsupported = true; break; }
			sb.Append("{\"op\":\"").Append(mappedOp).Append('"');

			switch (opCode)
			{
				case 0x00: /* nop */ break;
				case 0x2A: /* ret */ break;
				case 0x14: /* ldnull */ break;
				case 0x02: case 0x03: case 0x04: case 0x05: /* ldc.i4.0..3 */
					sb.Append(",\"operand\":\"").Append(opCode - 0x02).Append('"');
					break;
				case 0x16: case 0x17: case 0x18: case 0x19: case 0x1A: case 0x1B: case 0x1C: case 0x1D: /* ldc.i4.m1..8 */
					sb.Append(",\"operand\":\"").Append(opCode - 0x16).Append('"');
					break;
				case 0x20: /* ldc.i4 */
					if (pos + 4 > il.Length) { unsupported = true; break; }
					sb.Append(",\"operand\":\"").Append(BitConverter.ToInt32(il, pos)).Append('"');
					pos += 4;
					break;
				case 0x21: /* ldc.i8 */
					if (pos + 8 > il.Length) { unsupported = true; break; }
					sb.Append(",\"operand\":\"").Append(BitConverter.ToInt64(il, pos)).Append('"');
					pos += 8;
					break;
				case 0x22: /* ldc.r4 */
					if (pos + 4 > il.Length) { unsupported = true; break; }
					sb.Append(",\"operand\":\"").Append(BitConverter.ToSingle(il, pos)).Append('"');
					pos += 4;
					break;
				case 0x23: /* ldc.r8 */
					if (pos + 8 > il.Length) { unsupported = true; break; }
					sb.Append(",\"operand\":\"").Append(BitConverter.ToDouble(il, pos)).Append('"');
					pos += 8;
					break;
				case 0x0A: case 0x0B: case 0x0C: case 0x0D: /* ldarg.0..3 */
					sb.Append(",\"operand\":\"").Append(opCode - 0x0A).Append('"');
					break;
				case 0x0E: case 0x0F: case 0x10: case 0x11: /* ldloc.0..3 */
					sb.Append(",\"operand\":\"").Append(opCode - 0x0E).Append('"');
					break;
				case 0x06: case 0x07: case 0x08: case 0x09: /* stloc.0..3 */
					sb.Append(",\"operand\":\"").Append(opCode - 0x06).Append('"');
					break;
				case 0xFE0C: /* ldarg.s */ case 0xFE0D: /* ldarga.s */
				case 0xFE0E: /* ldloc.s */ case 0xFE0F: /* stloc.s */
				case 0xFE10: /* ldloca.s */ case 0xFE11: /* starg.s */
					if (pos + 1 > il.Length) { unsupported = true; break; }
					sb.Append(",\"operand\":\"").Append(il[pos++]).Append('"');
					break;
				case 0x72: /* ldstr */
					if (pos + 4 > il.Length) { unsupported = true; break; }
					int userStrTok = BitConverter.ToInt32(il, pos);
					pos += 4;
					try
					{
						var ush = MetadataTokens.UserStringHandle(userStrTok);
						string s = md.GetUserString(ush);
						sb.Append(",\"operand\":\"").Append(s.Replace("\\", "\\\\").Replace("\"", "\\\"")).Append('"');
					}
					catch { sb.Append(",\"operand\":\"\""); }
					break;
				case 0x28: /* call */ case 0x6F: /* callvirt */ case 0x73: /* newobj */
					if (pos + 4 > il.Length) { unsupported = true; break; }
					int methTok = BitConverter.ToInt32(il, pos);
					pos += 4;
					string? callee2 = ResolveMethodTokenSubjectId(md, methTok);
					if (callee2 != null) sb.Append(",\"callee\":\"").Append(callee2.Replace("\\", "\\\\").Replace("\"", "\\\"")).Append('"');
					break;
				case 0x7B: /* ldfld */ case 0x7D: /* stfld */ case 0x7E: /* ldsfld */
				case 0x80: /* stsfld */ case 0x7C: /* ldflda */ case 0x7F: /* ldsflda */
					if (pos + 4 > il.Length) { unsupported = true; break; }
					pos += 4; /* field token */
					break;
				case 0x8C: /* box */ case 0x8D: /* castclass */ case 0x8E: /* isinst */
				case 0x8F: /* unbox */ case 0x74: /* unbox.any */
					if (pos + 4 > il.Length) { unsupported = true; break; }
					pos += 4; /* type token */
					break;
				case 0x25: /* dup */ break;
				case 0x26: /* pop */ break;
				// ── Arithmetic / comparison (single-byte) — op name from MappingOpToJson ──
				case 0x58: case 0x59: case 0x5A: case 0x5B: case 0x5C: case 0x5D: case 0x5E:
				case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x66:
					break;
				// ── 0xFE (long-form) — op name from MappingOpToJson ──
				case 0xFE01: case 0xFE02: case 0xFE03: case 0xFE04: case 0xFE05: case 0xFE06:
				case 0xFE09: case 0xFE12: case 0xFE13: case 0xFE14: case 0xFE15: case 0xFE16:
				case 0xFE1D: case 0xFE1E:
					break;
				// ── Array element access ──
				case 0x9E: break; /* throw */
				case 0x9A: break; /* stelem (type) */
				case 0x8B: case 0x94: case 0xA0: break; /* ldelema/stelem */
				case 0x9B: case 0x9C: case 0x9D: break; /* stelem.i/i1/u1 */

				default:
					// Unsupported opcode — degrade to return-0 (never worse than today).
					unsupported = true;
					break;
			}
			sb.Append("}");
		}

		if (unsupported) return null;
		sb.Append("]}");
		return sb.ToString();
	}

	private string? ResolveMethodTokenSubjectId(MetadataReader md, int token)
	{
		try
		{
			// Method tokens have tag 0x06 in the high byte.
			if ((token & 0xFF000000) != 0x06000000)
				return null;
			var mdef = md.GetMethodDefinition(MetadataTokens.MethodDefinitionHandle(token));
			string name = md.GetString(mdef.Name);
			var declType = mdef.GetDeclaringType();
			var tdef = md.GetTypeDefinition(declType);
			string ns = tdef.Namespace.IsNil ? "" : md.GetString(tdef.Namespace);
			string tn = md.GetString(tdef.Name);
			string typeFull = string.IsNullOrEmpty(ns) ? tn : ns + "." + tn;
			string asm = AssemblyNameFromMetadata(md, declType);
			return $"{asm}/{typeFull}::{name}";
		}
		catch { return null; }
	}

	private static string AssemblyNameFromMetadata(MetadataReader md, TypeDefinitionHandle tdefHandle)
	{
		// The assembly containing this type definition is either the module's
		// defining assembly or the first assembly reference (for non-Primary
		// Module assemblies). Prefer the AssemblyDefinition of the module.
		try
		{
			var adef = md.GetAssemblyDefinition();
			return md.GetString(adef.Name);
		}
		catch { }
		// Fallback: first AssemblyReference.
		foreach (var ad in md.AssemblyReferences)
		{
			var aref = md.GetAssemblyReference(ad);
			return md.GetString(aref.Name);
		}
		return "";
	}

	private string MappingOpToJson(ushort opCode)
	{
		return opCode switch
		{
			0x00 => "nop",
			0x2A => "ret",
			0x14 => "ldnull",
			0x02 or 0x03 or 0x04 or 0x05 or 0x16 or 0x17 or 0x18 or 0x19 or 0x1A or 0x1B or 0x1C or 0x1D or 0x20 => "ldc.i4",
			0x21 => "ldc.i8",
			0x22 => "ldc.r4",
			0x23 => "ldc.r8",
			0x0A or 0x0B or 0x0C or 0x0D => "ldarg",
			0xFE0C => "ldarg",
			0xFE0E => "ldloc",
			0xFE0F => "stloc",
			0xFE0D => "ldarga",
			0xFE10 => "ldloca",
			0xFE11 => "starg",
			0x0E or 0x0F or 0x10 or 0x11 => "ldloc",
			0x06 or 0x07 or 0x08 or 0x09 => "stloc",
			0x72 => "ldstr",
			0x28 => "call",
			0x6F => "callvirt",
			0x73 => "newobj",
			0x7B => "ldfld",
			0x7D => "stfld",
			0x7E => "ldsfld",
			0x80 => "stsfld",
			0x7C => "ldflda",
			0x7F => "ldsflda",
			0x8C => "box",
			0x8D => "castclass",
			0x8E => "isinst",
			0x8F => "unbox",
			0x74 => "unbox.any",
			0x25 => "dup",
			0x26 => "pop",
			0x58 => "add",
			0x59 => "sub",
			0x5A => "mul",
			0x5B => "div",
			0x5C => "div",
			0x5D => "rem",
			0x5E => "rem",
			0x60 => "and",
			0x61 => "or",
			0x62 => "xor",
			0x63 => "shl",
			0x64 => "shr",
			0x65 => "shr",
			0x66 => "neg",
			>= 0xFE01 and <= 0xFE1E => opCode switch
			{
				0xFE01 => "ceq",
				0xFE02 => "cgt",
				0xFE03 => "cgt",
				0xFE04 => "clt",
				0xFE05 => "clt",
				0xFE06 => "ldlen",
				0xFE09 => "ldelema",
				0xFE12 => "conv.i4",
				0xFE13 => "conv.i8",
				0xFE14 => "conv.r4",
				0xFE15 => "conv.r8",
				0xFE16 => "conv.u",
				0xFE1D => "conv.i4",
				0xFE1E => "conv.u",
				_ => "unknown",
			},
			_ => "unknown",
		};
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
