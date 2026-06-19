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
	private int _linearScratchCounter;
	private int _nextInlineId;
	private int _dispatchLabelSeq;
	private string? _pendingEnumBoxSubjectId;
	private string? _pendingBoxSubjectId;
	private bool _pendingBoxHasProvider;
	private IReadOnlyList<AotCoreIrInstructionArtifact>? _lookaheadInstructionList;
	private int _lookaheadInstructionIndex;
	private IReadOnlyList<AotCoreIrInstructionArtifact>? _linearInstructionList;
	private int _linearInstructionIndex;

	private static readonly HashSet<string> ToCharEligiblePrimitives = new()
	{
		"System.Byte", "System.SByte", "System.Int16", "System.Int32", "System.Int64",
		"System.UInt16", "System.UInt32", "System.UInt64",
		"System.Single", "System.Double",
		"System.Boolean", "System.Char"
	};

	// Array bounds check cache for structured slot mode.
	// When consecutive array operations access the same array and index
	// expressions, the second access can skip redundant null/bounds checks.
	// Only active in structured mode (_activeStructuredSlotContext != null)
	// since flat linear mode uses opaque eval-stack indices that are always
	// the same expression string regardless of actual values.
	private string? _lastCheckedArrayExpr;
	private string? _lastCheckedIndexExpr;

	// Array access instruction IL offsets to skip bounds checks for,
	// populated by PreScanLoopArraySkips when a loop induction variable
	// pattern is detected (e.g., for (int i = 0; i < arr.Length; i++) { arr[i]; }).
	private HashSet<int>? _loopArrayAccessSkipOffsets;

	// Hoisted loop induction variables: maps chaos_locals slot → C++ local variable name.
	// When set, ldloc/stloc for these slots emit direct C++ local access instead of
	// chaos_locals[] traffic, keeping the IV in a register across loop iterations.
	private Dictionary<int, string>? _hoistedIVs;

	// Hoisted invariant locals for do-while loops: maps chaos_locals slot → C++ local variable name.
	// These locals are ldloc'd but never stloc'd inside the loop, so the load is hoisted
	// before the loop, eliminating per-iteration chaos_locals[] read traffic.
	private Dictionary<int, (string VarName, SlotType SlotType)>? _hoistedInvariantLocals;

	// Hoisted array base pointers for do-while loops: maps chaos_locals slot → C++ base pointer variable name.
	// When set, array load/store for these slots use direct base pointer access instead of
	// calling chaos_array_get_elements(reinterpret_cast<chaos_managed_array*>(...)) every iteration.
	private Dictionary<int, string>? _hoistedArrayBaseSlots;
	// Tracks slot variable names (_sN) to their chaos_locals source slot index.
	// Used by EmitLinearArrayLoad/Store to resolve hoisted array base pointers.
	private Dictionary<string, int>? _slotVarToLocalSlot;

	// Accumulator variables for do-while loops: maps chaos_locals slot → C++ local variable name.
	// These are loop-carried dependency chains (ldloc->add/sub->stloc) promoted to C++ locals.
	private Dictionary<int, string>? _accumulatorSlots;

	// Parallel type tracking for structured IR slot emission.
	// Mirrors the eval stack: each entry tracks whether the slot
	// contains Float32/Float64 (stored via ChaosStoreFloat32/64) or
	// NativeInt (plain integer-as-pointer).  Consumers (conv.i4,
	// ceq, etc.) use this to emit ChaosLoadFloat32/ChaosLoadFloat64
	// before operating on the value.
	private enum SlotType : byte { NativeInt, Int64, Float32, Float64, WideValue }

	/// <summary>Number of CHAOS_IL2CPP_INTPTR slots consumed by one WideValue.</summary>
	private const int WideValueSlotCount = 2;

	private readonly Stack<SlotType> _structuredSlotTypes = new();

	/// <summary>
	/// Local slots that are struct value types. Populated per-method in
	/// <c>EmitViaStructuredIR</c>. When set, ldloc for any slot in this
	/// set emits &amp;chaos_locals[N] (address) instead of chaos_locals[N] (value),
	/// because struct data is inline in CHAOS_IL2CPP_INTPTR slots and downstream
	/// consumers (ldfld) expect a pointer via chaos_resolve_managed_value_pointer.
	/// </summary>
	private HashSet<int>? _structLocalSlots;

	/// <summary>
	/// Local slots that hold Float32/Float64 values. Populated per-method
	/// in <c>EmitViaStructuredIR</c> by <c>IdentifyFloatLocalSlots</c>.
	/// When set, ldloc for any slot in this map emits
	/// ChaosLoadFloat32/ChaosLoadFloat64(chaos_locals[N]) and pushes the
	/// corresponding SlotType, enabling direct _dN/_fN slot allocation.
	/// </summary>
	private Dictionary<int, SlotType>? _floatLocalSlots;

	/// <summary>
	/// Local slots that hold Int64 values (packed via ChaosStoreInt64 into
	/// CHAOS_IL2CPP_INTPTR slots). Populated per-method in
	/// <c>EmitViaStructuredIR</c> by <c>IdentifyInt64LocalSlots</c>.
	/// When set, ldloc for any slot in this set pushes SlotType.Int64,
	/// enabling EmitLinearBinaryArithmetic to emit 64-bit arithmetic instead
	/// of int32 truncation.
	/// </summary>
	private HashSet<int>? _int64LocalSlots;

	/// <summary>
	/// Pre-scan the instruction list to identify which local slots hold
	/// Float32 or Float64 values. Simulates the eval stack type stack
	/// linearly across all instructions, recording the SlotType stored
	/// into each local by stloc. Falls back to NativeInt for ambiguous
	/// cases (locals stored with multiple types, or unresolvable types).
	/// </summary>

	/// <summary>
	/// Pre-scan the instruction list to identify which local slots hold
	/// Int64 values. Simulates the eval stack type stack linearly, recording
	/// the slot for each stloc of an Int64-typed value. This enables ldloc
	/// to push SlotType.Int64, so that EmitLinearBinaryArithmetic emits
	/// 64-bit arithmetic instead of int32 truncation.
	/// </summary>

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
