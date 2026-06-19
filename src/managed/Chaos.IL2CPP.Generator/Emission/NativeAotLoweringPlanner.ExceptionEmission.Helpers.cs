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

	private void ResetArrayCheckCache()
	{
		_lastCheckedArrayExpr = null;
		_lastCheckedIndexExpr = null;
	}



	private bool TrySkipArrayChecks(string arrayExpr, string indexExpr)
	{
		if (_activeStructuredSlotContext is null)
			return false;
		if (arrayExpr == _lastCheckedArrayExpr && indexExpr == _lastCheckedIndexExpr)
			return true;
		_lastCheckedArrayExpr = arrayExpr;
		_lastCheckedIndexExpr = indexExpr;
		return false;
	}



	private static HashSet<int> IdentifyStructLocalSlots(IReadOnlyList<AotCoreIrInstructionArtifact> instructions)
	{
		var structLocals = new HashSet<int>();
		for (int i = 1; i < instructions.Count; i++)
		{
			if (!string.Equals(instructions[i].Op, "initobj", StringComparison.Ordinal))
				continue;
			// initobj only operates on value types per IL spec, so the
			// targetRef typeShape check is redundant. Explicitly omitted
			// because generic value types (e.g. Vector128<T>) may carry
			// an incorrect typeShape (ReferenceType=1) in the AOT core IR.
			if (!string.Equals(instructions[i - 1].Op, "ldloca", StringComparison.Ordinal))
				continue;
			structLocals.Add(GetRequiredIntOperand(instructions[i - 1]));
		}
		return structLocals;
	}



	/// <summary>
	/// Pre-scan the instruction list to identify which local slots hold
	/// Float32 or Float64 values. Simulates the eval stack type stack
	/// linearly across all instructions, recording the SlotType stored
	/// into each local by stloc. Falls back to NativeInt for ambiguous
	/// cases (locals stored with multiple types, or unresolvable types).
	/// </summary>
	private static Dictionary<int, SlotType> IdentifyFloatLocalSlots(IReadOnlyList<AotCoreIrInstructionArtifact> instructions)
	{
		var result = new Dictionary<int, SlotType>();
		var typeStack = new Stack<SlotType>();

		// Iterate all instructions linearly, simulating the eval stack type flow.
		// This captures the type at each stloc, which tells us what type
		// ldloc should restore when loading the same local.
		for (int i = 0; i < instructions.Count; i++)
		{
			var op = instructions[i].Op;
			switch (op)
			{
				// Push float types
				case "ldc.r8":
					typeStack.Push(SlotType.Float64);
					break;
				case "ldc.r4":
					typeStack.Push(SlotType.Float32);
					break;

				// stloc: pop type and record for this local slot
				case "stloc":
					if (typeStack.Count > 0)
					{
						int slot = GetRequiredIntOperand(instructions[i]);
						SlotType storedType = typeStack.Pop();
						// Only record if the type is float (NativeInt is the default, no need to track)
						if (storedType is SlotType.Float32 or SlotType.Float64)
							result[slot] = storedType;
					}
					break;

				// ldloc: push conservative type (we don't know yet; stloc above will capture)
		case "ldloc":
					typeStack.Push(SlotType.NativeInt);
					break;

				// Binary arithmetic: pop 2, push float if either operand is float
				case "add": case "sub": case "mul": case "div": case "rem":
				case "add.ovf": case "add.ovf.un":
				case "sub.ovf": case "sub.ovf.un":
				case "mul.ovf": case "mul.ovf.un":
				case "div.un": case "rem.un":
					if (typeStack.Count >= 2)
					{
						SlotType rightType = typeStack.Pop();
						SlotType leftType = typeStack.Pop();
						typeStack.Push(
							rightType is SlotType.Float32 or SlotType.Float64 ||
							leftType is SlotType.Float32 or SlotType.Float64
								? SlotType.Float64
								: SlotType.NativeInt);
					}
					break;

				// Conv: in-place type change
				case "conv.r8":
					if (typeStack.Count > 0)
					{
						typeStack.Pop();
						typeStack.Push(SlotType.Float64);
					}
					break;
				case "conv.r4":
				case "conv.r.un":
					if (typeStack.Count > 0)
					{
						typeStack.Pop();
						typeStack.Push(SlotType.Float32);
					}
					break;
				case "conv.i4": case "conv.i8": case "conv.u4": case "conv.u8":
				case "conv.i": case "conv.u":
					if (typeStack.Count > 0)
					{
						typeStack.Pop();
						typeStack.Push(SlotType.NativeInt);
					}
					break;

				// ldelem: element type determined by opcode
				case "ldelem.r8":
					typeStack.Push(SlotType.Float64);
					break;
				case "ldelem.r4":
					typeStack.Push(SlotType.Float32);
					break;

				// ceq/cgt/clt: always produce integer (NativeInt)
				case "ceq": case "cgt": case "clt":
				case "cgt.un": case "clt.un":
					if (typeStack.Count >= 2)
					{
						typeStack.Pop();
						typeStack.Pop();
					}
					typeStack.Push(SlotType.NativeInt);
					break;

				// Instructions that push one value (ldc.i4, ldnull, ldarg, etc.)
				case "ldc.i4": case "ldc.i8": case "ldnull":
				case "ldarg": case "ldarga": case "ldloca":
				case "ldsfld": case "ldfld": case "ldlen":
				case "ldind.i4": case "ldind.i8": case "ldind.i":
				case "ldind.u4": case "ldind.u8":
				case "ldind.ref": case "ldobj":
					typeStack.Push(SlotType.NativeInt);
					break;

				// Instructions with no stack effect, or unknown: keep stack as-is
				case "nop": case "br": case "leave": case "endfinally":
				case "brtrue.s": case "brfalse.s": case "brtrue": case "brfalse":
				case "beq.s": case "bge.s": case "bgt.s": case "ble.s": case "blt.s": case "beq": case "bge": case "bgt": case "ble": case "blt":
				case "bne.un": case "bge.un": case "bgt.un": case "ble.un": case "blt.un":
				case "switch":
					break;

				// pop: remove one value
				case "pop":
					if (typeStack.Count > 0)
						typeStack.Pop();
					break;

				// dup: duplicate top
				case "dup":
					if (typeStack.Count > 0)
						typeStack.Push(typeStack.Peek());
					break;

				// Default: assume unknown ops push one value (conservative)
				default:
					typeStack.Push(SlotType.NativeInt);
					break;
			}
		}

		return result;
	}



	/// <summary>
	/// Pre-scan the instruction list to identify which local slots hold
	/// Int64 values. Simulates the eval stack type stack linearly, recording
	/// the slot for each stloc of an Int64-typed value. This enables ldloc
	/// to push SlotType.Int64, so that EmitLinearBinaryArithmetic emits
	/// 64-bit arithmetic instead of int32 truncation.
	/// </summary>
	private static HashSet<int> IdentifyInt64LocalSlots(IReadOnlyList<AotCoreIrInstructionArtifact> instructions)
	{
		var int64Locals = new HashSet<int>();
		var typeStack = new Stack<SlotType>();

		for (int i = 0; i < instructions.Count; i++)
		{
			var op = instructions[i].Op;
			switch (op)
			{
				// Int64 producers
				case "conv.i8":
				case "conv.u8":
				case "conv.ovf.i8":
				case "conv.ovf.u8":
				case "conv.ovf.i8.un":
				case "conv.ovf.u8.un":
				case "ldc.i8":
				case "ldind.i8":
				case "ldelem.i8":
					typeStack.Push(SlotType.Int64);
					break;
				case "ldelem.i":
					typeStack.Push(SlotType.NativeInt);
					break;

				// stloc: pop type and record for this local slot
				case "stloc":
					if (typeStack.Count > 0)
					{
						int slot = GetRequiredIntOperand(instructions[i]);
						SlotType storedType = typeStack.Pop();
						if (storedType == SlotType.Int64)
							int64Locals.Add(slot);
					}
					break;

				// ldloc: conservative — push NativeInt (overridden by int64Locals in emission)
				case "ldloc":
					typeStack.Push(SlotType.NativeInt);
					break;

				// Binary arithmetic: Int64 + any → Int64 (promotion)
				case "add": case "sub": case "mul": case "div": case "rem":
				case "add.ovf": case "add.ovf.un":
				case "sub.ovf": case "sub.ovf.un":
				case "mul.ovf": case "mul.ovf.un":
				case "div.un": case "rem.un":
					if (typeStack.Count >= 2)
					{
						SlotType rightType = typeStack.Pop();
						SlotType leftType = typeStack.Pop();
						bool isInt64Result = (rightType == SlotType.Int64 || leftType == SlotType.Int64);
						bool isFloatResult = rightType is SlotType.Float32 or SlotType.Float64 ||
						                     leftType is SlotType.Float32 or SlotType.Float64;
						if (isFloatResult)
							typeStack.Push(SlotType.Float64);
						else if (isInt64Result)
							typeStack.Push(SlotType.Int64);
						else
							typeStack.Push(SlotType.NativeInt);
					}
					break;

				// Conv narrowing: Int64 → NativeInt
				case "conv.i4": case "conv.u4":
				case "conv.i2": case "conv.u2":
				case "conv.i1": case "conv.u1":
				case "conv.i": case "conv.u":
					if (typeStack.Count > 0)
					{
						typeStack.Pop();
						typeStack.Push(SlotType.NativeInt);
					}
					break;

				// Conv float: Int64 → Float64
				case "conv.r4":
				case "conv.r.un":
					if (typeStack.Count > 0)
					{
						typeStack.Pop();
						typeStack.Push(SlotType.Float32);
					}
					break;
				case "conv.r8":
					if (typeStack.Count > 0)
					{
						typeStack.Pop();
						typeStack.Push(SlotType.Float64);
					}
					break;

				// ldelem: element type determined by opcode
				case "ldelem.r8":
					typeStack.Push(SlotType.Float64);
					break;
				case "ldelem.r4":
					typeStack.Push(SlotType.Float32);
					break;

				// ceq/cgt/clt: always produce integer (NativeInt)
				case "ceq": case "cgt": case "clt":
				case "cgt.un": case "clt.un":
					if (typeStack.Count >= 2)
					{
						typeStack.Pop();
						typeStack.Pop();
					}
					typeStack.Push(SlotType.NativeInt);
					break;

				// Instructions that push one value (default to NativeInt)
				case "ldc.i4": case "ldnull":
				case "ldarg": case "ldarga": case "ldloca":
				case "ldsfld": case "ldfld": case "ldlen":
				case "ldind.i4": case "ldind.i":
				case "ldind.u4":
				case "ldind.ref": case "ldobj":
				case "ldelem.i4": case "ldelem.u4":
				case "ldelem.i2": case "ldelem.u2":
				case "ldelem.i1": case "ldelem.u1":
				case "ldelem.ref":
				case "newarr": case "box": case "isinst": case "castclass":
					typeStack.Push(SlotType.NativeInt);
					break;

				// Instructions with no stack effect
				case "nop": case "br": case "leave": case "endfinally":
				case "brtrue.s": case "brfalse.s": case "brtrue": case "brfalse":
				case "beq.s": case "bge.s": case "bgt.s": case "ble.s": case "blt.s": case "beq": case "bge": case "bgt": case "ble": case "blt":
				case "bne.un": case "bge.un": case "bgt.un": case "ble.un": case "blt.un":
				case "switch":
					break;

				// pop: remove one value
				case "pop":
					if (typeStack.Count > 0)
						typeStack.Pop();
					break;

				// dup: duplicate top
				case "dup":
					if (typeStack.Count > 0)
						typeStack.Push(typeStack.Peek());
					break;

				// Default: assume unknown ops produce NativeInt
				default:
					typeStack.Push(SlotType.NativeInt);
					break;
			}
		}

		return int64Locals;
	}



	private SlotType PeekSlotType()
		=> _activeStructuredSlotContext is not null && _structuredSlotTypes.Count > 0
			? _structuredSlotTypes.Peek()
			: SlotType.NativeInt;



	private void PushSlotType(SlotType type)
	{
		if (_activeStructuredSlotContext is not null)
			_structuredSlotTypes.Push(type);
	}



	private SlotType ConsumeSlotType()
	{
		if (_activeStructuredSlotContext is not null && _structuredSlotTypes.Count > 0)
			return _structuredSlotTypes.Pop();
		return SlotType.NativeInt;
	}



	private void UpdateSlotType(SlotType type)
	{
		if (_activeStructuredSlotContext is not null && _structuredSlotTypes.Count > 0)
		{
			_structuredSlotTypes.Pop();
			_structuredSlotTypes.Push(type);
		}
	}



	private string AllocateLinearScratchName(string prefix)
		=> "chaos_" + prefix + "_" + (_linearScratchCounter++).ToString(CultureInfo.InvariantCulture);




	private static IReadOnlyList<AotCoreIrInstructionArtifact> FilterRedundantStoreReloadPairs(
		IReadOnlyList<AotCoreIrInstructionArtifact> instructions,
		IReadOnlySet<int>? branchTargetOffsets = null,
		IReadOnlySet<int>? externallyReferencedLocals = null)
	{
		var result = new List<AotCoreIrInstructionArtifact>(instructions.Count);

		// Pre-scan: count ldloc occurrences per local slot.
		// If a local is read by ldloc instructions outside a stloc+ldloc pair,
		// we must preserve the stloc so chaos_locals[N] is populated for those reads.
		var ldlocCountBySlot = new Dictionary<int, int>();
		for (int i = 0; i < instructions.Count; i++)
		{
			if (instructions[i].Op is "ldloc")
			{
				int slot = GetRequiredIntOperand(instructions[i]);
				ldlocCountBySlot.TryGetValue(slot, out int count);
				ldlocCountBySlot[slot] = count + 1;
			}
		}

		int idx = 0;
		while (idx < instructions.Count)
		{
			if (idx + 1 < instructions.Count &&
				instructions[idx].Op is "stloc" &&
				instructions[idx + 1].Op is "ldloc" &&
				GetRequiredIntOperand(instructions[idx]) == GetRequiredIntOperand(instructions[idx + 1]))
			{
				// Don't skip the ldloc if it's a branch target ¡ª the label must be preserved.
				if (branchTargetOffsets is not null &&
					branchTargetOffsets.Contains(GetRequiredIlOffset(instructions[idx + 1])))
				{
					result.Add(instructions[idx]);
					idx++;
					continue;
				}

				int slot = GetRequiredIntOperand(instructions[idx]);

				// If the local slot is referenced by other instruction lists (e.g., then-body or
				// else-body of an if-then-else), we must preserve the stloc so chaos_locals[N] is
				// populated for those external references.
				if (externallyReferencedLocals is not null && externallyReferencedLocals.Contains(slot))
				{
					// Keep stloc (other instruction lists read chaos_locals[slot]).
					// Skip ldloc since the value remains on the eval stack.
					result.Add(instructions[idx]);
					idx++;
					continue;
				}

				// If any OTHER instruction reads from this local slot in the current list,
				// we must keep the stloc so chaos_locals[N] is populated.
				if (ldlocCountBySlot.TryGetValue(slot, out int totalLdloc) && totalLdloc > 1)
				{
					// Keep stloc (other instructions read chaos_locals[slot]).
					// Skip ldloc since the value remains on the eval stack.
					result.Add(instructions[idx]);
					idx++;
					continue;
				}

				idx += 2;  // Skip BOTH stloc and ldloc
				continue;
			}
			result.Add(instructions[idx]);
			idx++;
		}
		return result;
	}



	/// <summary>
	/// Emit a pre-filtered instruction list with next-instruction lookahead.
	/// Unlike EmitInstructionSequence, this does NOT call
	/// FilterRedundantStoreReloadPairs — the caller is responsible for
	/// any filtering.  Used by the structured IR path which already
	/// filters before each loop.
	/// </summary>



	/// <summary>
	/// Resolve the effective next instruction for box elimination lookahead.
	/// When <paramref name="nextInstruction"/> is provided (linear path), use it directly.
	/// When null (structured IR path), fall back to the lookahead context fields.
	/// </summary>



		/// <summary>
		/// Two-step lookahead for patterns spanning 3 instructions (box + ldnull + call).
		/// Only used in structured IR path; linear path uses nextInstruction directly.
		/// </summary>



	// ©¤©¤ Shared throw/rethrow emission helpers ©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤
	// Consolidated from three duplicate implementations:
	//   EmitInstruction (structured EH linear)
	//   EmitFlatGotoBody (flat goto fallback)
	//   EmitIRBlockTerminator (structured IR)



		
		/// <summary>
		/// Scan backward from current instruction to find the target method for Assert.Throws.
		/// Looks for the pattern: ldnull + ldftn <method> + newobj Action::.ctor.
		/// Returns (symbol, isStatic) — isStatic indicates whether the target method is
		/// static (no implicit 'this' argument) vs. an instance method on a DisplayClass.
		/// </summary>



	/// <summary>
	/// Try to inline a resolved callee method at the call site.
	/// Used by both direct calls and devirtualized callvirt.
	/// Supports simple multi-BB callees (up to 3 basic blocks) with branch
	/// target remapping and unified exit via goto chaos_inline_end{N}.
	/// </summary>



	/// <summary>Try to inline the callee method identified by the invocation target.</summary>

}
