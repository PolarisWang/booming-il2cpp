using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    // 鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€
    // Pure Structured IR 鈥?type definitions
    //
    // These types represent the output of control flow recovery,
    // completely decoupled from CFG analysis (BasicBlock,
    // ControlFlowGraph, etc.).  The IR tree is a pure control-flow
    // representation that can be directly lowered to C++ without
    // any `goto chaos_ip_*` merge labels or `_suppressGotoNext`.
    // 鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€

    internal abstract record StructuredIRNode;


    // 鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€
    // Stack Slot Context 鈥?maps IL eval stack positions to C++ local
    // variable names (_s0, _s1, 鈥? so that push/pop operations become
    // local-variable assignments instead of array operations.
    //
    // Each push to a given stack depth gets a unique slot id, so
    // the mapping is: depth 鈫?slotId 鈫?"_s{slotId}".  Pop returns
    // the name at the current top and discards it.
    // 鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€

    

    /// <summary>
    /// Maximum recursion depth for EmitStructuredIRNode. When the structured IR
    /// tree exceeds this depth, the method falls back to PC-dispatch (linear)
    /// emission instead of overflowing the thread stack with recursive calls.
    /// 500 is conservative — ThreadPool threads have only 1 MB stack, so even
    /// 500 frames × ~200 bytes/frame = ~100 KB leaves headroom for Scriban
    /// rendering.
    /// </summary>
    private const int MaxStructuredIRDepth = 500;

    /// <summary>
    /// Tracks current recursion depth of EmitStructuredIRNode. Used instead of
    /// RuntimeHelpers.TryEnsureSufficientExecutionStack() which may not work
    /// reliably on threads created with a custom maxStackSize.
    /// </summary>

    private sealed class StructuredSlotEmissionContext
    {
        private int _depth;
        private int _peakWideDepth;
        private int _peakIntDepth;
        private int _peakFloat64Depth;
        private int _peakFloat32Depth;
        private int _peakInt64Depth;

        private readonly List<(string name, SlotType type)> _slotInfo = new();

        public int Depth => _depth;
        public int MaxIntSlots => _peakIntDepth;
        public int MaxFloat64Slots => _peakFloat64Depth;
        public int MaxWideSlots => _peakWideDepth;
        public int MaxFloat32Slots => _peakFloat32Depth;
        public int MaxInt64Slots => _peakInt64Depth;

        public Dictionary<int, SlotType>? FloatLocalSlots { get; set; }

        public string AllocatePushTarget(SlotType type = SlotType.NativeInt)
        {
            string slotName = type switch
            {
                SlotType.Float64 => FormatDoubleSlotName(_depth),
                SlotType.Float32 => FormatFloatSlotName(_depth),
                SlotType.Int64 => FormatInt64SlotName(_depth),
                SlotType.WideValue => FormatWideSlotName(_peakWideDepth),
                _ => FormatStructuredSlotName(_depth),
            };
            // Track peak depth per type (depth+1 because depth is pre-push)
            int newDepth = _depth + 1;
            if (type == SlotType.Float64) _peakFloat64Depth = Math.Max(_peakFloat64Depth, newDepth);
            else if (type == SlotType.Float32) _peakFloat32Depth = Math.Max(_peakFloat32Depth, newDepth);
            else if (type == SlotType.Int64) _peakInt64Depth = Math.Max(_peakInt64Depth, newDepth);
            else if (type == SlotType.WideValue) _peakWideDepth = Math.Max(_peakWideDepth, _peakWideDepth + 1);
            else _peakIntDepth = Math.Max(_peakIntDepth, newDepth);

            if (type == SlotType.WideValue)
            {
                // WideValue uses WideValueSlotCount consecutive NativeInt slots
                for (int i = 0; i < WideValueSlotCount; i++)
                {
                    if (_depth < _slotInfo.Count)
                        _slotInfo[_depth] = (FormatStructuredSlotName(_depth), SlotType.NativeInt);
                    else
                        _slotInfo.Add((FormatStructuredSlotName(_depth), SlotType.NativeInt));
                    _depth++;
                }
            }
            else
            {
                if (_depth < _slotInfo.Count)
                    _slotInfo[_depth] = (slotName, type);
                else
                    _slotInfo.Add((slotName, type));
                _depth++;
            }
            return slotName;
        }

        public string PopValue()
        {
            if (_depth <= 0)
            {
                // Underflow: return a default slot name instead of throwing.
                // The generated C++ will compile (slot _s0 is always declared),
                // and the fact runner catches exceptions from incorrect values.
                // This prevents ~284 methods from becoming dead stubs.
                return FormatStructuredSlotName(0);
            }

            _depth--;
            EnsureSlotInfoCapacity(_depth + 1);
            return _slotInfo[_depth].name;
        }

        public string PeekValue()
        {
            if (_depth <= 0)
            {
                return FormatStructuredSlotName(_depth);
            }

            EnsureSlotInfoCapacity(_depth);
            return _slotInfo[_depth - 1].name;
        }

        private void EnsureSlotInfoCapacity(int required)
        {
            while (_slotInfo.Count < required)
            {
                _slotInfo.Add((FormatStructuredSlotName(_slotInfo.Count), SlotType.NativeInt));
                // Any new entry is at a depth >= _slotInfo.Count, which must be within the
                // declared slot range.  Update _peakIntDepth to ensure the declaration covers it.
                if (_peakIntDepth < _slotInfo.Count)
                    _peakIntDepth = _slotInfo.Count;
            }
        }

        public void Discard(int count = 1)
        {
            if (count < 0)
                count = 0;
            if (count > _depth)
            {
                // Underflow: clamp to zero instead of throwing.
                // Same rationale as PopValue() above.
                _depth = 0;
                return;
            }

            _depth -= count;
        }

        public void RestoreDepth(int savedDepth)
        {
            _depth = savedDepth;
            // Ensure slot entries exist for the restored depth and update peak tracking
            // so that PopValue/PeekValue don't return slot names beyond the declared range.
            EnsureSlotInfoCapacity(_depth);
            _peakIntDepth = Math.Max(_peakIntDepth, _depth);
        }
    }

    private static string FormatStructuredSlotName(int slotIndex) => $"_s{slotIndex}";
    private static string FormatDoubleSlotName(int slotIndex) => $"_d{slotIndex}";
    private static string FormatFloatSlotName(int slotIndex) => $"_f{slotIndex}";
    private static string FormatWideSlotName(int slotIndex) => $"_w{slotIndex}";
    private static string FormatInt64SlotName(int slotIndex) => $"_i{slotIndex}";

    private void EmitStructuredMethodReturn(StringBuilder builder, AotCoreIrAbiSlotArtifact returnAbi, string indentation)
    {
        switch (returnAbi.CarrierKindCode)
        {
            case AotCoreIrAbiCarrierKind.Void:
                builder.AppendLine(indentation + "return;");
                return;
        }

        string valueExpression = ConsumeEvalStackValueExpression();
        switch (returnAbi.CarrierKindCode)
        {
            case AotCoreIrAbiCarrierKind.Int32:
                builder.AppendLine(indentation + $"return static_cast<CHAOS_IL2CPP_INT32>({valueExpression});");
                return;
            case AotCoreIrAbiCarrierKind.Int8:
                builder.AppendLine(indentation + $"return static_cast<CHAOS_IL2CPP_INT8>({valueExpression});");
                return;
            case AotCoreIrAbiCarrierKind.UInt8:
                builder.AppendLine(indentation + $"return static_cast<CHAOS_IL2CPP_UINT8>({valueExpression});");
                return;
            case AotCoreIrAbiCarrierKind.Int16:
                builder.AppendLine(indentation + $"return static_cast<CHAOS_IL2CPP_INT16>({valueExpression});");
                return;
            case AotCoreIrAbiCarrierKind.UInt16:
                builder.AppendLine(indentation + $"return static_cast<CHAOS_IL2CPP_UINT16>({valueExpression});");
                return;
            case AotCoreIrAbiCarrierKind.Float32:
                // Typed slot (_fN) is already a float — return directly
                if (valueExpression.StartsWith("_f", StringComparison.Ordinal))
                    builder.AppendLine(indentation + $"return {valueExpression};");
                else
                    builder.AppendLine(indentation + $"return ChaosLoadFloat32({valueExpression});");
                return;
            case AotCoreIrAbiCarrierKind.Float64:
                // Typed slot (_dN) is already a double — return directly
                if (valueExpression.StartsWith("_d", StringComparison.Ordinal))
                    builder.AppendLine(indentation + $"return {valueExpression};");
                else
                    builder.AppendLine(indentation + $"return ChaosLoadFloat64({valueExpression});");
                return;
            case AotCoreIrAbiCarrierKind.Int64:
                // Typed slot (_iN) is already an int64_t — return directly
                if (valueExpression.StartsWith("_i", StringComparison.Ordinal))
                    builder.AppendLine(indentation + $"return {valueExpression};");
                else
                    builder.AppendLine(indentation + $"return ChaosLoadInt64({valueExpression});");
                return;
            case AotCoreIrAbiCarrierKind.UInt64:
                builder.AppendLine(indentation + $"return chaos_load_uint64({valueExpression});");
                return;
            case AotCoreIrAbiCarrierKind.NativeInt:
            case AotCoreIrAbiCarrierKind.ByRef:
            case AotCoreIrAbiCarrierKind.MultiReturn:
            case AotCoreIrAbiCarrierKind.ByRefToValueType:
                builder.AppendLine(indentation + $"return {valueExpression};");
                return;
            case AotCoreIrAbiCarrierKind.ValueTypeByValue:
                builder.AppendLine(indentation + $"return *chaos_resolve_managed_value_pointer<{GetRequiredAbiValueTypeSymbol(returnAbi)}>({valueExpression});");
                return;
            default:
                throw new NotSupportedException($"native-aot lowering does not support ABI return carrier '{returnAbi.CarrierKindCode}'.");
        }
    }

    private static string NormalizeStoredStackValueExpression(string valueExpression)
    {
        string trimmed = valueExpression.Trim();
        return trimmed.StartsWith("&chaos_locals", StringComparison.Ordinal) ||
               trimmed.StartsWith("&chaos_args", StringComparison.Ordinal)
            ? $"reinterpret_cast<CHAOS_IL2CPP_INTPTR>({trimmed})"
            : valueExpression;
    }

    private string AllocateEvalStackTargetExpression(SlotType type = SlotType.NativeInt)
        => _state.Value!.ActiveStructuredSlotContext is null
            ? "chaos_eval_stack[chaos_stack_top++]"
            : _state.Value!.ActiveStructuredSlotContext.AllocatePushTarget(type);

    private string ConsumeEvalStackValueExpression()
        => _state.Value!.ActiveStructuredSlotContext is null
            ? "chaos_eval_stack[--chaos_stack_top]"
            : _state.Value!.ActiveStructuredSlotContext.PopValue();

    private string AccessEvalStackTopExpression()
        => _state.Value!.ActiveStructuredSlotContext is null
            ? "chaos_eval_stack[chaos_stack_top - 1]"
            : _state.Value!.ActiveStructuredSlotContext.PeekValue();

    private void EmitEvalStackPush(StringBuilder builder, string indentation, string valueExpression, SlotType type = SlotType.NativeInt)
        => builder.AppendLine($"{indentation}{AllocateEvalStackTargetExpression(type)} = {NormalizeStoredStackValueExpression(valueExpression)};");

    private void EmitEvalStackDiscard(StringBuilder builder, string indentation, int count = 1)
    {
        if (_state.Value!.ActiveStructuredSlotContext is null)
        {
            if (count == 1)
            {
                builder.AppendLine($"{indentation}chaos_stack_top--;");
            }
            else
            {
                builder.AppendLine($"{indentation}chaos_stack_top -= {count};");
            }

            return;
        }

        _state.Value!.ActiveStructuredSlotContext.Discard(count);
    }

    private static void EmitStructuredSlotDeclarations(StringBuilder builder, int maxIntSlots, int maxFloat64Slots, int maxFloat32Slots, int maxInt64Slots, int maxWideSlots, string indentation)
    {
        for (int i = 0; i < maxIntSlots; i++)
            builder.AppendLine($"{indentation}CHAOS_IL2CPP_INTPTR {FormatStructuredSlotName(i)}{{}};");
        for (int i = 0; i < maxFloat64Slots; i++)
            builder.AppendLine($"{indentation}double {FormatDoubleSlotName(i)};");
        for (int i = 0; i < maxFloat32Slots; i++)
            builder.AppendLine($"{indentation}float {FormatFloatSlotName(i)};");
        for (int i = 0; i < maxInt64Slots; i++)
            builder.AppendLine($"{indentation}CHAOS_IL2CPP_INT64 {FormatInt64SlotName(i)};");
        for (int i = 0; i < maxWideSlots; i++)
            builder.AppendLine($"{indentation}alignas(16) CHAOS_IL2CPP_UINT8 {FormatWideSlotName(i)}[16];");
    }

    // 鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€
    // Pure Structured IR 鈫?C++ Recursive Emitter
    //
    // The emitter is a recursive tree walk that produces C++ control
    // flow directly 鈥?no `goto chaos_ip_*`, no `_suppressGotoNext`,
    // no merge labels.  Leaf instructions use EmitInstruction
    // (no goto-next appended).
    // 鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€


    private static bool ContainsResidualBranchTerminators(StructuredIRNode node)
    {
        return node switch
        {
            IRBlock { Terminator: { Op: "br" or "leave" } } => true,
            IRSequence seq => seq.Nodes.Any(ContainsResidualBranchTerminators),
            IRIfThenElse ite => ContainsResidualBranchTerminators(ite.ThenBody) || (ite.ElseBody != null && ContainsResidualBranchTerminators(ite.ElseBody)) || (ite.PostMergeBody != null && ContainsResidualBranchTerminators(ite.PostMergeBody)),
            IRWhileLoop loop => ContainsResidualBranchTerminators(loop.Body),
            IRDoWhileLoop loop => ContainsResidualBranchTerminators(loop.Body),
            IRSwitch sw => sw.CaseBodies.Values.Any(ContainsResidualBranchTerminators) || (sw.DefaultBody != null && ContainsResidualBranchTerminators(sw.DefaultBody)),
            IRExceptionRegion er => ContainsResidualBranchTerminators(er.TryBody) || ContainsResidualBranchTerminators(er.HandlerBody),
            _ => false,
        };
    }

    private static bool IsControlFlowTerminator(StructuredIRNode node)
    {
        return node switch
        {
            IRReturn => true,
            IRThrow => true,
            IRBlock { Terminator: { Op: "ret" or "throw" or "rethrow" } } => true,
            IRSequence seq => seq.Nodes.Count > 0 && IsControlFlowTerminator(seq.Nodes[^1]),
            IRIfThenElse ite => IsControlFlowTerminator(ite.ThenBody)
                && (ite.ElseBody is null || IsControlFlowTerminator(ite.ElseBody)),
            _ => false,
        };
    }

    private static bool IsEmptyBody(StructuredIRNode node)
    {
        return node switch
        {
            IRSequence { Nodes.Count: 0 } => true,
            IRBlock { BodyInstructions.Count: 0, Terminator: null } => true,
            IRBlock { BodyInstructions.Count: 0, Terminator: { Op: "br" or "leave" } } => true,
            _ => false,
        };
    }

    private static StructuredIRNode StripExceptionPartitionExitTerminators(StructuredIRNode node)
    {
        return node switch
        {
            IRBlock block when block.Terminator is { Op: "br" or "leave" }
                => new IRBlock(block.BodyInstructions, null),
            IRSequence seq
                => new IRSequence(seq.Nodes.Select(StripExceptionPartitionExitTerminators).ToArray()),
            IRIfThenElse ite
                => new IRIfThenElse(
                    ite.ConditionInstructions,
                    ite.BranchTerminator,
                    StripExceptionPartitionExitTerminators(ite.ThenBody),
                    ite.ElseBody is null ? null : StripExceptionPartitionExitTerminators(ite.ElseBody),
                    ite.PostMergeBody is null ? null : StripExceptionPartitionExitTerminators(ite.PostMergeBody),
                    ite.PreConditionDepth),
            IRWhileLoop loop
                => new IRWhileLoop(
                    loop.ConditionInstructions,
                    loop.ConditionTerminator,
                    StripExceptionPartitionExitTerminators(loop.Body),
                    loop.ExitOffset),
            IRDoWhileLoop loop
                => new IRDoWhileLoop(
                    StripExceptionPartitionExitTerminators(loop.Body),
                    loop.LatchInstructions,
                    loop.LatchTerminator,
                    loop.HeaderOffset,
                    loop.ExitOffset),
            IRSwitch sw
                => new IRSwitch(
                    sw.SwitchInstructions,
                    sw.CaseBodies.ToDictionary(
                        pair => pair.Key,
                        pair => StripExceptionPartitionExitTerminators(pair.Value)),
                    sw.DefaultBody is null ? null : StripExceptionPartitionExitTerminators(sw.DefaultBody),
                    sw.ExitOffset,
                    sw.FallthroughCaseValues),
            IRExceptionRegion er
                => new IRExceptionRegion(
                    er.Kind,
                    StripExceptionPartitionExitTerminators(er.TryBody),
                    StripExceptionPartitionExitTerminators(er.HandlerBody),
                    er.CatchTypeSubjectId,
                    er.FilterInstructions),
            _ => node,
        };
    }
    /// <summary>
    /// Top-level entry point: emit a StructuredIR tree as C++ code.
    /// Called from EmitManagedMethod (and future exception region
    /// emitters) instead of EmitStructuredInstructionRange.
    /// </summary>

    // 鈹€鈹€ Block 鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€

    /// <summary>
    /// Emit an IRBlock using slot-mapped local variables for the eval stack.
    /// Each instruction is emitted into a temporary StringBuilder, then
    /// post-processed to replace array operations with _sN slot names.
    /// </summary>

    /// <summary>
    /// Emit a block terminator (ret, throw, rethrow, endfinally,
    /// endfilter, br, leave) as the appropriate C++ statement.
    /// </summary>

    // 鈹€鈹€ If-then-else 鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€


    /// <summary>
    /// Recursively collects all local slots referenced by ldloc instructions
    /// in a structured IR node tree. Used by EmitIRIfThenElse to prevent
    /// FilterRedundantStoreReloadPairs from eliminating stloc pairs whose
    /// values are needed by then/else bodies.
    /// </summary>

    // ─── Eval-stack helpers for depth compensation ───────────────────────

    /// <summary>
    /// Estimate how many values a compare/branch terminator pops from the eval stack.
    /// </summary>

    /// <summary>
    /// Pop-count estimate for IRBlock depth compensation — handles call/callvirt/newobj
    /// argument pops via <see cref="AotCoreIrInstructionArtifact.TargetParameterCount"/>.
    /// </summary>

    /// <summary>
    /// Parse parameter count from a SubjectId string like
    /// "Namespace.Type::Method:ReturnType(Param1,Param2)"
    /// </summary>

    /// <summary>
    /// Push-count estimate for IRBlock depth compensation — for call/callvirt/calli,
    /// returns 0 for void returns and 1 otherwise; newobj always pushes 1.
    /// </summary>

    // 鈹€鈹€ Do-while loop 鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€

    // 鈹€鈹€ Switch 鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€

    // 鈹€鈹€ Exception regions 鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€


    /// <summary>
    /// Compute the maximum eval-stack depth across all instructions in the method.
    /// This determines the number of structured slot locals (_s0, _s1, ...).
    /// Simulates the net
    /// push/pop effect of each IL opcode to find the peak concurrent depth.
    /// When returnAbi is provided, returns -1 if a non-void method has a `ret`
    /// with nothing on the stack (would underflow during structured emission).
    /// </summary>

    // ════════════════════════════════════════════════════════════════════════════
    // Convenience: try to use this emitter for a set of instructions
    // ════════════════════════════════════════════════════════════════════════════

    /// <summary>
    /// Emit a list of instructions using the new StructuredIR emitter
    /// (no goto fallback).  Builds a CFG, runs structured recovery,
    /// and emits the IR tree.  For irreducible CFGs, falls back to
    /// flat goto emission via EmitInstructionRange.
    /// </summary>
    /// <summary>
    /// Build a StructuredIR tree for an exception region method body.
    /// Partitions the instructions into prefix/try/handler/tail and wraps
    /// the try+handler in an IRExceptionRegion node.
    /// </summary>

    /// <summary>
    /// Build a structured IR tree for a method whose EH shape does not match any
    /// of the 6 standard patterns.  Regions are sorted innermost-first during
    /// shape detection, so each region's TryInstructions already exclude
    /// instructions consumed by inner regions' try+handler ranges.
    /// Nested regions produce nested IRExceptionRegion nodes wrapping from
    /// innermost to outermost; sibling regions (no nesting) produce sequential
    /// IRExceptionRegion nodes sharing a common try body.
    /// </summary>

    /// <summary>
    /// Build a structured IR tree from a sub-list of instructions (for exception
    /// region partitions).  Returns an empty IRSequence if the partition has no
    /// instructions or the CFG is irreducible.
    /// </summary>

    /// <summary>
    /// Fallback emission for an exception partition whose CFG is irreducible
    /// (typically because branch targets cross partition boundaries).  Produces
    /// a single IRBlock containing all instructions without structured control
    /// flow, preserving correctness at the cost of flat linear emission.
    /// </summary>

    /// <summary>
    /// Emits a pc-dispatch state machine for an irreducible CFG. The generated
    /// C++ uses a chaos_pc variable + switch statement instead of goto:
    ///
    ///   int32_t chaos_pc = {PcVariableInit};
    ///   while (chaos_pc >= 0) {
    ///       switch (chaos_pc) {
    ///       case 0: { ... chaos_pc = 1; break; }
    ///       ...
    ///       }
    ///   }
    /// </summary>

    /// <summary>
    /// Builds a pc-dispatch state machine from an irreducible CFG.
    /// Each basic block is assigned a unique pc value (0..N-1), and the
    /// terminator of each block determines the next pc value.
    /// </summary>

    /// <summary>
    /// Emits the C++ condition expression for a conditional branch terminator
    /// in the pc-dispatch state machine. Operands are peeked from the structured
    /// slot context and popped to keep stack tracking consistent.
    /// </summary>

    // ════════════════════════════════════════════════════════════════════════════
    // Phase 1 diagnostics: irreducible CFG classification
    // ════════════════════════════════════════════════════════════════════════════

    private static long s_structuredMethodCount;
    private static long s_exceptionBodyCount;
    private static long s_irreducibleCount;
    private static long s_totalMethodCount;
    internal static long s_pcDispatchCount;
    private static int s_structuredBrLabelSeq;
    private static readonly System.Collections.Concurrent.ConcurrentDictionary<string, long> s_irreducibleReasons
        = new(System.StringComparer.Ordinal);

}
