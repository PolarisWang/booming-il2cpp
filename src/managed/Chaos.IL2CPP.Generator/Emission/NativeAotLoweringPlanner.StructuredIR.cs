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

    /// <summary>Linear sequence of instructions with optional terminator.</summary>
    internal sealed record IRBlock(
        IReadOnlyList<AotCoreIrInstructionArtifact> BodyInstructions,
        AotCoreIrInstructionArtifact? Terminator
    ) : StructuredIRNode;

    /// <summary>Sequential composition of IR nodes.</summary>
    internal sealed record IRSequence(IReadOnlyList<StructuredIRNode> Nodes) : StructuredIRNode;

    /// <summary>If-then-else 鈥?condition expressed via eval-stack + branch opcode.</summary>
    internal sealed record IRIfThenElse(
        IReadOnlyList<AotCoreIrInstructionArtifact> ConditionInstructions,
        AotCoreIrInstructionArtifact BranchTerminator,
        StructuredIRNode ThenBody,
        StructuredIRNode? ElseBody,
        StructuredIRNode? PostMergeBody = null,
        int PreConditionDepth = 0
    ) : StructuredIRNode;

    /// <summary>Header-controlled while loop.</summary>
    internal sealed record IRWhileLoop(
        IReadOnlyList<AotCoreIrInstructionArtifact> ConditionInstructions,
        AotCoreIrInstructionArtifact? ConditionTerminator,
        StructuredIRNode Body,
        int ExitOffset
    ) : StructuredIRNode;

    /// <summary>Latch-controlled do-while loop.</summary>
    internal sealed record IRDoWhileLoop(
        StructuredIRNode Body,
        IReadOnlyList<AotCoreIrInstructionArtifact> LatchInstructions,
        AotCoreIrInstructionArtifact? LatchTerminator,
        int HeaderOffset,
        int ExitOffset
    ) : StructuredIRNode;

    /// <summary>Flat region (kept for safety — should be unreachable at runtime).</summary>
    internal sealed record IRFlatRegion(
        IReadOnlyList<AotCoreIrInstructionArtifact> Instructions,
        IReadOnlySet<int> Offsets
    ) : StructuredIRNode;

    /// <summary>Switch dispatch.</summary>
    internal sealed record IRSwitch(
        IReadOnlyList<AotCoreIrInstructionArtifact> SwitchInstructions,
        IReadOnlyDictionary<int, StructuredIRNode> CaseBodies,
        StructuredIRNode? DefaultBody,
        int ExitOffset,
        IReadOnlySet<int> FallthroughCaseValues
    ) : StructuredIRNode;

    // 鈹€鈹€ Leaf control-flow nodes 鈹€鈹€

    internal sealed record IRBreak : StructuredIRNode;
    internal sealed record IRContinue : StructuredIRNode;
    internal sealed record IRReturn : StructuredIRNode;
    internal sealed record IRThrow : StructuredIRNode;

    // 鈹€鈹€ Exception regions (first-class IR nodes) 鈹€鈹€

    internal enum IRExceptionKind { TryCatch, TryFinally, TryFilter }

    internal sealed record IRExceptionRegion(
        IRExceptionKind Kind,
        StructuredIRNode TryBody,
        StructuredIRNode HandlerBody,
        string? CatchTypeSubjectId = null,
        IReadOnlyList<AotCoreIrInstructionArtifact>? FilterInstructions = null
    ) : StructuredIRNode;

    // 鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€
    // Stack Slot Context 鈥?maps IL eval stack positions to C++ local
    // variable names (_s0, _s1, 鈥? so that push/pop operations become
    // local-variable assignments instead of array operations.
    //
    // Each push to a given stack depth gets a unique slot id, so
    // the mapping is: depth 鈫?slotId 鈫?"_s{slotId}".  Pop returns
    // the name at the current top and discards it.
    // 鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€

    private StructuredSlotEmissionContext? _activeStructuredSlotContext;

    private sealed class StructuredSlotEmissionContext
    {
        private int _depth;
        private int _maxDepth;

        public int Depth => _depth;
        public int MaxDepth => _maxDepth;

        public string AllocatePushTarget()
        {
            string slotName = FormatStructuredSlotName(_depth);
            _depth++;
            if (_depth > _maxDepth)
                _maxDepth = _depth;
            return slotName;
        }

        public string PopValue()
        {
            if (_depth <= 0)
            {
                throw new InvalidOperationException("structured slot stack underflow.");
            }

            _depth--;
            return FormatStructuredSlotName(_depth);
        }

        public string PeekValue()
        {
            // When the slot stack is empty, return a freshly-allocated slot name
            // instead of throwing. This handles the case where the structured IR
            // reconstruction places a `dup` instruction at the start of a nested
            // if-then-else's condition instructions -- the value was left on the
            // eval stack by the outer branch, and `dup` needs to reference it.
            // The actual duplication (destination = source) will be emitted by
            // the caller; the slot allocation here is purely a naming concern.
            if (_depth <= 0)
            {
                return FormatStructuredSlotName(_depth);
            }

            return FormatStructuredSlotName(_depth - 1);
        }

        public void Discard(int count = 1)
        {
            if (count < 0 || count > _depth)
            {
                throw new InvalidOperationException("structured slot stack underflow.");
            }

            _depth -= count;
        }

        public void RestoreDepth(int savedDepth)
        {
            _depth = savedDepth;
        }
    }

    private static string FormatStructuredSlotName(int slotIndex) => $"_s{slotIndex}";

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
                builder.AppendLine(indentation + $"return chaos_load_float32({valueExpression});");
                return;
            case AotCoreIrAbiCarrierKind.Float64:
                builder.AppendLine(indentation + $"return ChaosLoadFloat64({valueExpression});");
                return;
            case AotCoreIrAbiCarrierKind.Int64:
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

    private string AllocateEvalStackTargetExpression()
        => _activeStructuredSlotContext is null
            ? "chaos_eval_stack[chaos_stack_top++]"
            : _activeStructuredSlotContext.AllocatePushTarget();

    private string ConsumeEvalStackValueExpression()
        => _activeStructuredSlotContext is null
            ? "chaos_eval_stack[--chaos_stack_top]"
            : _activeStructuredSlotContext.PopValue();

    private string AccessEvalStackTopExpression()
        => _activeStructuredSlotContext is null
            ? "chaos_eval_stack[chaos_stack_top - 1]"
            : _activeStructuredSlotContext.PeekValue();

    private void EmitEvalStackPush(StringBuilder builder, string indentation, string valueExpression)
        => builder.AppendLine($"{indentation}{AllocateEvalStackTargetExpression()} = {NormalizeStoredStackValueExpression(valueExpression)};");

    private void EmitEvalStackDiscard(StringBuilder builder, string indentation, int count = 1)
    {
        if (_activeStructuredSlotContext is null)
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

        _activeStructuredSlotContext.Discard(count);
    }

    private static void EmitStructuredSlotDeclarations(StringBuilder builder, int maxDepth, string indentation)
    {
        for (int i = 0; i < maxDepth; i++)
        {
            builder.AppendLine($"{indentation}CHAOS_IL2CPP_INTPTR {FormatStructuredSlotName(i)}{{}};");
        }
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
            IRFlatRegion fr => fr,
            _ => node,
        };
    }
    /// <summary>
    /// Top-level entry point: emit a StructuredIR tree as C++ code.
    /// Called from EmitManagedMethod (and future exception region
    /// emitters) instead of EmitStructuredInstructionRange.
    /// </summary>
    private void EmitStructuredIRNode(
        StringBuilder builder,
        StructuredIRNode node,
        AotCoreIrMethodArtifact method,
        string indentation)
    {
        switch (node)
        {
            case IRBlock block:
                EmitIRBlock(builder, block, method, indentation);
                break;

            case IRSequence seq:
                foreach (var child in seq.Nodes)
                    EmitStructuredIRNode(builder, child, method, indentation);
                break;

            case IRIfThenElse ite:
                EmitIRIfThenElse(builder, ite, method, indentation);
                break;

            case IRWhileLoop w:
                EmitIRWhileLoop(builder, w, method, indentation);
                break;

            case IRDoWhileLoop dw:
                EmitIRDoWhileLoop(builder, dw, method, indentation);
                break;

            case IRSwitch sw:
                EmitIRSwitch(builder, sw, method, indentation);
                break;

            case IRBreak:
                builder.AppendLine(indentation + "break;");
                break;

            case IRContinue:
                builder.AppendLine(indentation + "continue;");
                break;

            case IRReturn:
                EmitStructuredMethodReturn(builder, method.ReturnAbi, indentation);
                break;

            case IRThrow:
                builder.AppendLine(indentation + "throw;");
                break;

            case IRExceptionRegion er:
                EmitIRExceptionRegion(builder, er, method, indentation);
                break;

            default:
                throw new NotSupportedException(
                    "StructuredIR: unknown node type '" + node.GetType().Name + "'");
        }
    }

    // 鈹€鈹€ Block 鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€

    /// <summary>
    /// Emit an IRBlock using slot-mapped local variables for the eval stack.
    /// Each instruction is emitted into a temporary StringBuilder, then
    /// post-processed to replace array operations with _sN slot names.
    /// </summary>
    private void EmitIRBlock(
        StringBuilder builder,
        IRBlock block,
        AotCoreIrMethodArtifact method,
        string indentation)
    {
        // When an IRBlock is emitted as a child of IRSequence or inside a
        // branch body, it may start with instructions that pop values pushed
        // by predecessor CFG blocks. Ensure the structured slot depth is
        // adequate by simulating the initial pop-only prefix.
        if (_activeStructuredSlotContext is { Depth: 0 } ctx)
        {
            int requiredDepth = 0;
            foreach (var instr in block.BodyInstructions)
            {
                int pops = EstimatePopCount(instr.Op);
                int pushes = EstimatePushCount(instr.Op);
                if (pushes > 0 && pushes >= pops)
                    break; // Self-sustaining from here
                if (pops > pushes)
                    requiredDepth += pops - pushes;
                else
                    break;
            }
            if (requiredDepth > 0)
                ctx.RestoreDepth(requiredDepth);
        }

        foreach (var instr in block.BodyInstructions)
        {
            EmitInstruction(builder, instr, indentation);
        }

        if (block.Terminator != null)
        {
            EmitIRBlockTerminator(builder, block.Terminator, method, indentation);
        }
    }

    /// <summary>
    /// Emit a block terminator (ret, throw, rethrow, endfinally,
    /// endfilter, br, leave) as the appropriate C++ statement.
    /// </summary>
    private void EmitIRBlockTerminator(
        StringBuilder builder,
        AotCoreIrInstructionArtifact terminator,
        AotCoreIrMethodArtifact method,
        string indentation)
    {
        switch (terminator.Op)
        {
            case "ret":
                EmitStructuredMethodReturn(builder, method.ReturnAbi, indentation);
                break;

            case "throw":
            {
                string throwVal = ConsumeEvalStackValueExpression();
                EmitThrowCpp(builder, throwVal, indentation);
                break;
            }

            case "rethrow":
                EmitRethrowCpp(builder, indentation);
                break;

            case "endfinally":
                // RAII scope guard handles the finally body - no explicit code needed.
                break;

            case "endfilter":
                builder.AppendLine(indentation +
                    $"if ({ConsumeEvalStackValueExpression()} == 0)");
                builder.AppendLine(indentation + "{");
                EmitRethrowCpp(builder, indentation + "    ");
                builder.AppendLine(indentation + "}");
                break;

            case "br":
            case "leave":
                // Safety net: residual br/leave are stripped earlier by
                // RemoveTrailingBranch for known structured patterns. Any
                // remaining br/leave is a non-structured pattern that
                // should not reach emission in practice.
                break;

            default:
                throw new NotSupportedException(
                    "StructuredIR: unsupported block terminator '" + terminator.Op + "'");
        }
    }

    // 鈹€鈹€ If-then-else 鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€


    /// <summary>
    /// Recursively collects all local slots referenced by ldloc instructions
    /// in a structured IR node tree. Used by EmitIRIfThenElse to prevent
    /// FilterRedundantStoreReloadPairs from eliminating stloc pairs whose
    /// values are needed by then/else bodies.
    /// </summary>
    private static HashSet<int> CollectLdlocSlots(StructuredIRNode node)
    {
        var slots = new HashSet<int>();
        CollectLdlocSlotsRecursive(node, slots);
        return slots;
    }

    private static void CollectLdlocSlotsRecursive(StructuredIRNode node, HashSet<int> slots)
    {
        switch (node)
        {
            case IRBlock block:
                foreach (var instr in block.BodyInstructions)
                {
                    if (instr.Op is "ldloc")
                        slots.Add(GetRequiredIntOperand(instr));
                }
                break;
            case IRIfThenElse ite:
                CollectLdlocSlotsRecursive(ite.ThenBody, slots);
                if (ite.ElseBody != null)
                    CollectLdlocSlotsRecursive(ite.ElseBody, slots);
                break;
            case IRWhileLoop w:
                CollectLdlocSlotsRecursive(w.Body, slots);
                break;
            case IRDoWhileLoop dw:
                CollectLdlocSlotsRecursive(dw.Body, slots);
                break;
            case IRSwitch sw:
                foreach (var caseBody in sw.CaseBodies.Values)
                    CollectLdlocSlotsRecursive(caseBody, slots);
                if (sw.DefaultBody != null)
                    CollectLdlocSlotsRecursive(sw.DefaultBody, slots);
                break;
            case IRSequence seq:
                foreach (var sub in seq.Nodes)
                    CollectLdlocSlotsRecursive(sub, slots);
                break;
        }
    }

    private void EmitIRIfThenElse(
        StringBuilder builder,
        IRIfThenElse ite,
        AotCoreIrMethodArtifact method,
        string indentation)
    {
        var terminator = ite.BranchTerminator;

        // Normalize: if then-body is empty and else-body is not, invert condition and swap.
        // This eliminates the C++ pattern "if (x) { } else { ... }".
        if (IsEmptyBody(ite.ThenBody) && ite.ElseBody != null && !IsEmptyBody(ite.ElseBody))
        {
            string invertedOp = terminator.Op switch
            {
                "brtrue" => "brfalse",
                "brfalse" => "brtrue",
                "beq" => "bne.un",
                "bne.un" => "beq",
                "bge" => "blt",
                "bgt" => "ble",
                "ble" => "bgt",
                "blt" => "bge",
                "bge.un" => "blt.un",
                _ => terminator.Op,
            };
            var invTerm = new AotCoreIrInstructionArtifact
            {
                Op = invertedOp,
                Operand = terminator.Operand,
                IlOffset = terminator.IlOffset,
            };
            ite = ite with { BranchTerminator = invTerm, ThenBody = ite.ElseBody, ElseBody = null };
            terminator = invTerm;
        }

        // Capture depth before condition instructions — both branches must start
        // from this depth so they converge on the same slot names.
        int preConditionDepth = ite.PreConditionDepth;
        if (preConditionDepth == 0)
        {
            preConditionDepth = _activeStructuredSlotContext?.Depth ?? 0;
        }

        // postConditionDepth will be set after the condition-effect computation below.
        int postConditionDepth = preConditionDepth;

        // Compute net eval-stack effect of condition instructions + terminator.
        // When the structured IR reconstruction places values pushed by a
        // predecessor CFG block outside the condition instruction list, the
        // depth may be undercounted. We compensate by computing the required
        // depth from the instructions themselves.
        {
            int condPushes = 0, condPops = 0;
            foreach (var instr in ite.ConditionInstructions)
            {
                condPushes += EstimatePushCount(instr.Op);
                condPops += EstimatePopCount(instr.Op);
            }
            int termPops = EstimateTerminatorPopCount(terminator.Op);
            // net = (preDepth + condPushes - condPops - termPops) must be >= 0
            // ⇒ minimum preDepth = condPops + termPops - condPushes
            int requiredDepth = Math.Max(0, condPops + termPops - condPushes);
            if (preConditionDepth < requiredDepth)
                preConditionDepth = requiredDepth;

            // Depth AFTER condition instructions + terminator have run.
            // Both then-body and else-body execute at this depth, since the
            // terminator leaves residual values on the eval stack that the
            // branch bodies must see (e.g., ldsfld; dup; brtrue → brtrue pops
            // only the dup copy, leaving the original ldsfld value for the
            // else body's pop).
            postConditionDepth = preConditionDepth + condPushes - condPops - termPops;
            if (postConditionDepth < 0) postConditionDepth = 0;
        }
        _activeStructuredSlotContext?.RestoreDepth(preConditionDepth);

        // Scan then/else bodies for ldloc slots referenced externally. When a stloc+ldloc
        // pair in the condition writes to a slot later read by the body, the filter must
        // preserve the stloc so chaos_locals[N] is populated for the body's ldloc.
        var externallyReferencedLocals = CollectLdlocSlots(ite.ThenBody);
        if (ite.ElseBody != null)
        {
            foreach (int s in CollectLdlocSlots(ite.ElseBody))
                externallyReferencedLocals.Add(s);
        }

        // Emit condition instructions (push operands onto eval stack)
        var filteredConditions = FilterRedundantStoreReloadPairs(
            ite.ConditionInstructions,
            externallyReferencedLocals: externallyReferencedLocals);
        foreach (var instr in filteredConditions)
            EmitInstruction(builder, instr, indentation);

        string inner = indentation + "    ";
        string bodyIndent = inner + "    ";

        if (terminator.Op == "brtrue" || terminator.Op == "brfalse")
        {
            bool branchOnNonZero = terminator.Op == "brtrue";
            SlotType _cType = PeekSlotType();
            string _cSlot = ConsumeEvalStackValueExpression();
            ConsumeSlotType();
            // Condition operand consumed — depth is back to preConditionDepth
            string _condition = _cType switch
            {
                SlotType.Float32 => branchOnNonZero
                    ? $"chaos_load_float32({_cSlot}) != 0.0f"
                    : $"chaos_load_float32({_cSlot}) == 0.0f",
                SlotType.Float64 => branchOnNonZero
                    ? $"ChaosLoadFloat64({_cSlot}) != 0.0"
                    : $"ChaosLoadFloat64({_cSlot}) == 0.0",
                _ => branchOnNonZero
                    ? $"{_cSlot} != 0"
                    : $"{_cSlot} == 0",
            };

            builder.AppendLine(indentation + "{");
            builder.AppendLine(inner + "if (" + _condition + ")");
            builder.AppendLine(inner + "{");
            EmitStructuredIRNode(builder, ite.ThenBody, method, bodyIndent);
            builder.AppendLine(inner + "}");

            if (ite.ElseBody != null)
            {
                builder.AppendLine(inner + "else");
                builder.AppendLine(inner + "{");
                _activeStructuredSlotContext?.RestoreDepth(postConditionDepth);
                EmitStructuredIRNode(builder, ite.ElseBody, method, bodyIndent);
                builder.AppendLine(inner + "}");
            }

            if (ite.PostMergeBody != null)
            {
                _activeStructuredSlotContext?.RestoreDepth(postConditionDepth);
                EmitStructuredIRNode(builder, ite.PostMergeBody, method, inner);
            }

            builder.AppendLine(indentation + "}");
        }
        else
        {
            // Comparison opcodes: beq, bne.un, bge, bge.un, bgt, ble, blt
            string cmpOp = terminator.Op switch
            {
                "beq" => "==",
                "bne.un" => "!=",
                "bge" => ">=",
                "bge.un" => ">=",
                "bgt" => ">",
                "ble" => "<=",
                "blt" => "<",
                _ => throw new NotSupportedException(
                    "StructuredIR: unsupported conditional branch '" + terminator.Op + "'")
            };

            bool isUnsigned = terminator.Op == "bge.un";
            string valueType = isUnsigned
                ? "CHAOS_IL2CPP_UINT32"
                : (cmpOp == "==" || cmpOp == "!=" ? "CHAOS_IL2CPP_INTPTR" : "CHAOS_IL2CPP_INT32");

            builder.AppendLine(indentation + "{");

            SlotType _cmpRType = PeekSlotType();
            string _cmpRExpr = ConsumeEvalStackValueExpression();
            ConsumeSlotType();
            SlotType _cmpLType = PeekSlotType();
            string _cmpLExpr = ConsumeEvalStackValueExpression();
            ConsumeSlotType();
            string _cmpRight = _cmpRType switch
            {
                SlotType.Float32 => $"chaos_load_float32({_cmpRExpr})",
                SlotType.Float64 => $"ChaosLoadFloat64({_cmpRExpr})",
                _ => isUnsigned
                    ? $"static_cast<CHAOS_IL2CPP_UINT32>(static_cast<CHAOS_IL2CPP_INT32>({_cmpRExpr}))"
                    : $"static_cast<{valueType}>({_cmpRExpr})",
            };
            string _cmpLeft = _cmpLType switch
            {
                SlotType.Float32 => $"chaos_load_float32({_cmpLExpr})",
                SlotType.Float64 => $"ChaosLoadFloat64({_cmpLExpr})",
                _ => isUnsigned
                    ? $"static_cast<CHAOS_IL2CPP_UINT32>(static_cast<CHAOS_IL2CPP_INT32>({_cmpLExpr}))"
                    : $"static_cast<{valueType}>({_cmpLExpr})",
            };
            builder.AppendLine(inner + $"const auto chaos_right = {_cmpRight};");
            builder.AppendLine(inner + $"const auto chaos_left = {_cmpLeft};");

            builder.AppendLine(inner + "if (chaos_left " + cmpOp + " chaos_right)");
            builder.AppendLine(inner + "{");
            EmitStructuredIRNode(builder, ite.ThenBody, method, bodyIndent);
            builder.AppendLine(inner + "}");

            if (ite.ElseBody != null)
            {
                builder.AppendLine(inner + "else");
                builder.AppendLine(inner + "{");
                _activeStructuredSlotContext?.RestoreDepth(postConditionDepth);
                EmitStructuredIRNode(builder, ite.ElseBody, method, bodyIndent);
                builder.AppendLine(inner + "}");
            }

            if (ite.PostMergeBody != null)
            {
                _activeStructuredSlotContext?.RestoreDepth(postConditionDepth);
                EmitStructuredIRNode(builder, ite.PostMergeBody, method, inner);
            }

            builder.AppendLine(indentation + "}");
        }
    }

    // ─── Eval-stack helpers for depth compensation ───────────────────────

    /// <summary>
    /// Estimate how many values a compare/branch terminator pops from the eval stack.
    /// </summary>
    private static int EstimateTerminatorPopCount(string op) => op switch
    {
        "brtrue" or "brfalse" => 1,
        "beq" or "bne.un" or "bge" or "bge.un" or "bgt" or "ble" or "blt" => 2,
        _ => 0,
    };

    private static int EstimatePopCount(string op) => op switch
    {
        "stloc" or "stloc.s" or "starg" or "pop" or "initobj" => 1,
        "stfld" or "stobj" or "stsfld"
            or "stelem" or "stelem.i" or "stelem.ref"
            or "stind.i4" or "stind.i1" or "stind.i2" or "stind.i8"
            or "stind.r4" or "stind.r8" or "stind.ref" or "stind.i"
            or "cpblk" or "initblk" or "throw" => 2,
        _ => 0,
    };

    private static int EstimatePushCount(string op) => op switch
    {
        "ldc.i4" or "ldc.i8" or "ldc.r4" or "ldc.r8"
            or "ldarg" or "ldstr" or "ldtoken" or "ldarga"
            or "ldnull" or "ldloc" or "ldloc.s" or "ldloca"
            or "ldsfld" or "ldsflda" or "ldftn" or "ldvirtftn"
            or "newarr" or "sizeof" => 1,
        "box" or "unbox" or "unbox.any"
            or "castclass" or "isinst"
            or "ldlen" or "localloc"
            or "ldfld" or "ldflda" or "ldobj" or "ldelema"
            or "ldelem" or "ldelem.i" or "ldelem.ref"
            or "ldind.i4" or "ldind.u1" or "ldind.i1"
            or "ldind.u2" or "ldind.i2" or "ldind.u4"
            or "ldind.i8" or "ldind.r4" or "ldind.r8" or "ldind.ref" or "ldind.i"
            or "conv.i4" or "conv.i1" or "conv.i2" or "conv.i8"
            or "conv.u8" or "conv.r4" or "conv.r8" or "conv.u"
            or "conv.u1" or "conv.u2" or "conv.u4"
            or "conv.r.un" or "ckfinite"
            or "not" or "neg" or "dup"
            or "cgt.un" or "ceq" or "cgt" or "clt"
            or "add" or "sub" or "mul" or "div" or "div.un" or "rem" or "rem.un"
            or "shl" or "shr" or "shr.un"
            or "and" or "or" or "xor"
            or "add.ovf" or "sub.ovf" or "mul.ovf"
            or "ldlen" or "localloc" => 1,
        "mkrefany" => 2,
        "call" or "callvirt" or "calli" or "newobj" => 1,
        _ => 0,
    };

    private void EmitIRWhileLoop(
        StringBuilder builder,
        IRWhileLoop w,
        AotCoreIrMethodArtifact method,
        string indentation)
    {
        string inner = indentation + "    ";
        string bodyIndent = inner + "    ";

        if (w.ConditionTerminator == null)
        {
            // No condition 鈥?infinite loop (while (true) { ... })
            // Condition instructions might still contain setup code.
            var filteredSetup = FilterRedundantStoreReloadPairs(w.ConditionInstructions);
            foreach (var instr in filteredSetup)
                EmitInstruction(builder, instr, indentation);

            builder.AppendLine(indentation + "while (true)");
            builder.AppendLine(indentation + "{");
            builder.AppendLine(indentation + "    chaos_safepoint_poll();");
            EmitStructuredIRNode(builder, w.Body, method, bodyIndent);
            builder.AppendLine(indentation + "}");
            return;
        }

        var terminator = w.ConditionTerminator;

        var filteredConditions = FilterRedundantStoreReloadPairs(w.ConditionInstructions);
        foreach (var instr in filteredConditions)
            EmitInstruction(builder, instr, indentation);

        if (terminator.Op == "brtrue" || terminator.Op == "brfalse")
        {
            bool branchOnNonZero = terminator.Op == "brtrue";
            SlotType _cType = PeekSlotType();
            string _cSlot = ConsumeEvalStackValueExpression();
            ConsumeSlotType();
            string _condition = _cType switch
            {
                SlotType.Float32 => branchOnNonZero
                    ? $"chaos_load_float32({_cSlot}) != 0.0f"
                    : $"chaos_load_float32({_cSlot}) == 0.0f",
                SlotType.Float64 => branchOnNonZero
                    ? $"ChaosLoadFloat64({_cSlot}) != 0.0"
                    : $"ChaosLoadFloat64({_cSlot}) == 0.0",
                _ => branchOnNonZero
                    ? $"{_cSlot} != 0"
                    : $"{_cSlot} == 0",
            };

            builder.AppendLine(indentation + "{");
            builder.AppendLine(inner + "while (" + _condition + ")");
            builder.AppendLine(inner + "{");
            EmitStructuredIRNode(builder, w.Body, method, bodyIndent);
            builder.AppendLine(inner + "}");
            builder.AppendLine(indentation + "}");
        }
        else
        {
            // Comparison opcodes
            string cmpOp = terminator.Op switch
            {
                "beq" => "==",
                "bne.un" => "!=",
                "bge" => ">=",
                "bge.un" => ">=",
                "bgt" => ">",
                "ble" => "<=",
                "blt" => "<",
                _ => throw new NotSupportedException(
                    "StructuredIR: unsupported while condition '" + terminator.Op + "'")
            };

            bool isUnsigned = terminator.Op == "bge.un";
            string valueType = isUnsigned
                ? "CHAOS_IL2CPP_UINT32"
                : (cmpOp == "==" || cmpOp == "!=" ? "CHAOS_IL2CPP_INTPTR" : "CHAOS_IL2CPP_INT32");

            builder.AppendLine(indentation + "{");

            SlotType _cmpRType = PeekSlotType();
            string _cmpRExpr = ConsumeEvalStackValueExpression();
            ConsumeSlotType();
            SlotType _cmpLType = PeekSlotType();
            string _cmpLExpr = ConsumeEvalStackValueExpression();
            ConsumeSlotType();
            string _cmpRight = _cmpRType switch
            {
                SlotType.Float32 => $"chaos_load_float32({_cmpRExpr})",
                SlotType.Float64 => $"ChaosLoadFloat64({_cmpRExpr})",
                _ => isUnsigned
                    ? $"static_cast<CHAOS_IL2CPP_UINT32>(static_cast<CHAOS_IL2CPP_INT32>({_cmpRExpr}))"
                    : $"static_cast<{valueType}>({_cmpRExpr})",
            };
            string _cmpLeft = _cmpLType switch
            {
                SlotType.Float32 => $"chaos_load_float32({_cmpLExpr})",
                SlotType.Float64 => $"ChaosLoadFloat64({_cmpLExpr})",
                _ => isUnsigned
                    ? $"static_cast<CHAOS_IL2CPP_UINT32>(static_cast<CHAOS_IL2CPP_INT32>({_cmpLExpr}))"
                    : $"static_cast<{valueType}>({_cmpLExpr})",
            };
            builder.AppendLine(inner + $"const auto chaos_right = {_cmpRight};");
            builder.AppendLine(inner + $"const auto chaos_left = {_cmpLeft};");

            builder.AppendLine(inner + "while (chaos_left " + cmpOp + " chaos_right)");
            builder.AppendLine(inner + "{");
            EmitStructuredIRNode(builder, w.Body, method, bodyIndent);
            builder.AppendLine(inner + "}");
            builder.AppendLine(indentation + "}");
        }
    }

    // 鈹€鈹€ Do-while loop 鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€

    private void EmitIRDoWhileLoop(
        StringBuilder builder,
        IRDoWhileLoop dw,
        AotCoreIrMethodArtifact method,
        string indentation)
    {
        string inner = indentation + "    ";
        string bodyIndent = inner + "    ";

        builder.AppendLine(indentation + "do");
        builder.AppendLine(indentation + "{");
        EmitStructuredIRNode(builder, dw.Body, method, bodyIndent);

        // Emit latch instructions if present
        if (dw.LatchTerminator != null)
        {
            var filteredLatch = FilterRedundantStoreReloadPairs(dw.LatchInstructions);
            foreach (var instr in filteredLatch)
                EmitInstruction(builder, instr, bodyIndent);

            var terminator = dw.LatchTerminator;
            if (terminator.Op == "brtrue" || terminator.Op == "brfalse")
            {
                bool branchOnNonZero = terminator.Op == "brtrue";
                // For do-while, the latch branch is the loop-back branch.
                // When it's false, we break out (invert the condition).
                SlotType _cType = PeekSlotType();
                string _cSlot = ConsumeEvalStackValueExpression();
                ConsumeSlotType();
                string _condition = _cType switch
                {
                    SlotType.Float32 => branchOnNonZero
                        ? $"chaos_load_float32({_cSlot}) != 0.0f"
                        : $"chaos_load_float32({_cSlot}) == 0.0f",
                    SlotType.Float64 => branchOnNonZero
                        ? $"ChaosLoadFloat64({_cSlot}) != 0.0"
                        : $"ChaosLoadFloat64({_cSlot}) == 0.0",
                    _ => branchOnNonZero
                        ? $"{_cSlot} != 0"
                        : $"{_cSlot} == 0",
                };

                builder.AppendLine(bodyIndent + "if (!(" + _condition + ")) break;");
            }
            else if (terminator.Op is "beq" or "bne.un" or "bge" or "bge.un" or "bgt" or "ble" or "blt")
            {
                string cmpOp = terminator.Op switch
                {
                    "beq" => "==",
                    "bne.un" => "!=",
                    "bge" => ">=",
                    "bge.un" => ">=",
                    "bgt" => ">",
                    "ble" => "<=",
                    "blt" => "<",
                    _ => throw new NotSupportedException(
                        "StructuredIR: unsupported do-while latch '" + terminator.Op + "'")
                };

                bool isUnsigned = terminator.Op == "bge.un";
                string valueType = isUnsigned
                    ? "CHAOS_IL2CPP_UINT32"
                    : (cmpOp == "==" || cmpOp == "!=" ? "CHAOS_IL2CPP_INTPTR" : "CHAOS_IL2CPP_INT32");

                SlotType cmpRType = PeekSlotType();
                string cmpRExpr = ConsumeEvalStackValueExpression();
                ConsumeSlotType();
                SlotType cmpLType = PeekSlotType();
                string cmpLExpr = ConsumeEvalStackValueExpression();
                ConsumeSlotType();
                string cmpRight = cmpRType switch
                {
                    SlotType.Float32 => $"chaos_load_float32({cmpRExpr})",
                    SlotType.Float64 => $"ChaosLoadFloat64({cmpRExpr})",
                    _ => isUnsigned
                        ? $"static_cast<CHAOS_IL2CPP_UINT32>(static_cast<CHAOS_IL2CPP_INT32>({cmpRExpr}))"
                        : $"static_cast<{valueType}>({cmpRExpr})",
                };
                string cmpLeft = cmpLType switch
                {
                    SlotType.Float32 => $"chaos_load_float32({cmpLExpr})",
                    SlotType.Float64 => $"ChaosLoadFloat64({cmpLExpr})",
                    _ => isUnsigned
                        ? $"static_cast<CHAOS_IL2CPP_UINT32>(static_cast<CHAOS_IL2CPP_INT32>({cmpLExpr}))"
                        : $"static_cast<{valueType}>({cmpLExpr})",
                };

                builder.AppendLine($"{bodyIndent}if (!({cmpLeft} {cmpOp} {cmpRight})) break;");
            }
            else if (terminator.Op == "br")
            {
                // Infinite loop latch 鈥?no condition to check
            }
            else
            {
                throw new NotSupportedException(
                    "StructuredIR: unsupported do-while latch '" + terminator.Op + "'");
            }
        }

        builder.AppendLine(indentation + "} while (true);");
    }

    // 鈹€鈹€ Switch 鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€

    private void EmitIRSwitch(
        StringBuilder builder,
        IRSwitch sw,
        AotCoreIrMethodArtifact method,
        string indentation)
    {
        string inner = indentation + "    ";
        string caseIndent = inner + "    ";
        string bodyIndent = caseIndent + "    ";

        // Collect ldloc slots from all case bodies + default body so the
        // DCE filter preserves stloc instructions whose values are consumed
        // by downstream case bodies through chaos_locals[N].
        var switchBodyLocals = new HashSet<int>();
        foreach (var caseBody in sw.CaseBodies.Values)
            switchBodyLocals.UnionWith(CollectLdlocSlots(caseBody));
        if (sw.DefaultBody != null)
            switchBodyLocals.UnionWith(CollectLdlocSlots(sw.DefaultBody));

        var filteredSwitch = FilterRedundantStoreReloadPairs(
            sw.SwitchInstructions, externallyReferencedLocals: switchBodyLocals.Count > 0 ? switchBodyLocals : null);
        foreach (var instr in filteredSwitch)
            EmitInstruction(builder, instr, indentation);

        builder.AppendLine(indentation + "{");
        builder.AppendLine(inner + $"const auto chaos_switch_value = static_cast<CHAOS_IL2CPP_INT32>({ConsumeEvalStackValueExpression()});");
        builder.AppendLine(inner + "switch (chaos_switch_value)");
        builder.AppendLine(inner + "{");

        var sortedCaseValues = sw.CaseBodies.Keys.OrderBy(k => k).ToList();
        foreach (var caseValue in sortedCaseValues)
        {
            var body = sw.CaseBodies[caseValue];
            builder.AppendLine(caseIndent + "case " + caseValue + ":");
            bool isEmptyFallthrough = sw.FallthroughCaseValues.Contains(caseValue);
            if (!isEmptyFallthrough)
                builder.AppendLine(caseIndent + "{");
            EmitStructuredIRNode(builder, body, method, bodyIndent);
            if (!IsControlFlowTerminator(body) && !isEmptyFallthrough)
                builder.AppendLine(caseIndent + "    break;");
            if (!isEmptyFallthrough)
                builder.AppendLine(caseIndent + "}");
        }

        if (sw.DefaultBody != null)
        {
            builder.AppendLine(caseIndent + "default:");
            bool isSimpleTerminator = IsControlFlowTerminator(sw.DefaultBody);
            if (!isSimpleTerminator)
                builder.AppendLine(caseIndent + "{");
            EmitStructuredIRNode(builder, sw.DefaultBody, method, bodyIndent);
            if (!isSimpleTerminator)
            {
                if (!IsControlFlowTerminator(sw.DefaultBody))
                    builder.AppendLine(caseIndent + "    break;");
                builder.AppendLine(caseIndent + "}");
            }
        }

        builder.AppendLine(inner + "}");
        builder.AppendLine(indentation + "}");
    }

    // 鈹€鈹€ Exception regions 鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€

    private void EmitIRExceptionRegion(
        StringBuilder builder,
        IRExceptionRegion er,
        AotCoreIrMethodArtifact method,
        string indentation)
    {
        string inner = indentation + "    ";
        string bodyIndent = inner + "    ";

        switch (er.Kind)
        {
            case IRExceptionKind.TryCatch:
            {
                int preTryDepth = _activeStructuredSlotContext?.Depth ?? 0;
                builder.AppendLine(indentation + "CHAOS_EH_TRY");
                EmitStructuredIRNode(builder, er.TryBody, method, bodyIndent);
                if (er.CatchTypeSubjectId != null)
                    _activeStructuredSlotContext?.RestoreDepth(preTryDepth);
                builder.AppendLine(indentation + "CHAOS_EH_CATCH_BEGIN");
                // Phase 5 sentinel guard: propagate sentinel values (< 0) through rethrow
                builder.AppendLine(inner +
                    "if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }");
                if (er.CatchTypeSubjectId != null)
                {
                    string typeInfoSym = GetNativeTypeInfoSymbol(er.CatchTypeSubjectId);
                    builder.AppendLine(inner + "auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);");
                    builder.AppendLine(inner + "if (chaos_header != nullptr)");
                    builder.AppendLine(inner + "{");
                    builder.AppendLine(inner +
                        "    if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &" +
                        typeInfoSym + "))");
                    builder.AppendLine(inner + "    { CHAOS_EH_RETHROW; }");
                    builder.AppendLine(inner + "}");
                    EmitEvalStackPush(builder, inner, "CHAOS_EH_EXCEPTION_OBJ");
                }
                else
                {
                    EmitEvalStackPush(builder, inner, "CHAOS_EH_EXCEPTION_OBJ");
                }
                EmitStructuredIRNode(builder, er.HandlerBody, method, bodyIndent);
                builder.AppendLine(indentation + "CHAOS_EH_END");
                break;
            }

            case IRExceptionKind.TryFinally:
            {
                builder.AppendLine(inner + "auto _chaos_finally = [&]()");
                builder.AppendLine(inner + "{");
                EmitStructuredIRNode(builder, er.HandlerBody, method, inner + "    ");
                builder.AppendLine(inner + "};");
                builder.AppendLine(indentation + "CHAOS_EH_TRY_FINALLY");
                EmitStructuredIRNode(builder, er.TryBody, method, bodyIndent);
                builder.AppendLine(indentation + "CHAOS_EH_FINALLY_END");
                break;
            }

            case IRExceptionKind.TryFilter:
            {
                builder.AppendLine(indentation + "CHAOS_EH_TRY");
                EmitStructuredIRNode(builder, er.TryBody, method, bodyIndent);
                builder.AppendLine(indentation + "CHAOS_EH_CATCH_BEGIN");
                EmitEvalStackPush(builder, inner, "CHAOS_EH_EXCEPTION_OBJ");

                // Emit structured filter body first, then let endfilter decide rethrow vs accept.
                if (er.FilterInstructions != null && er.FilterInstructions.Count > 0)
                {
                    var filterInstructions = er.FilterInstructions;
                    int terminalIndex = filterInstructions.Count - 1;
                    bool hasTerminalEndFilter = string.Equals(filterInstructions[terminalIndex].Op, "endfilter", StringComparison.Ordinal);
                    if (terminalIndex > 0)
                    {
                        StructuredIRNode filterBody = BuildExceptionPartitionTree(filterInstructions.Take(terminalIndex).ToArray(), offsets: new HashSet<int>(filterInstructions.Take(terminalIndex).Select(GetRequiredIlOffset)));
                        EmitStructuredIRNode(builder, filterBody, method, inner);
                    }

                    if (hasTerminalEndFilter)
                    {
                        builder.AppendLine(inner +
                            $"if ({ConsumeEvalStackValueExpression()} == 0)");
                        builder.AppendLine(inner + "{");
                        builder.AppendLine(inner + "    CHAOS_EH_RETHROW;");
                        builder.AppendLine(inner + "}");
                    }
                }

                EmitEvalStackPush(builder, inner, "CHAOS_EH_EXCEPTION_OBJ");
                EmitStructuredIRNode(builder, er.HandlerBody, method, bodyIndent);
                builder.AppendLine(indentation + "CHAOS_EH_END");
                break;
            }

            default:
                throw new NotSupportedException(
                    "StructuredIR: unknown exception kind '" + er.Kind + "'");
        }
    }


    /// <summary>
    /// Compute the maximum eval-stack depth across all instructions in the method.
    /// This determines the number of structured slot locals (_s0, _s1, ...).
    /// Simulates the net
    /// push/pop effect of each IL opcode to find the peak concurrent depth.
    /// When returnAbi is provided, returns -1 if a non-void method has a `ret`
    /// with nothing on the stack (would underflow during structured emission).
    ///
    /// When monotonic=true, depth is never reset (used for structured slot
    /// emission where slot IDs are monotonic rather than reusing stack slots).
    /// </summary>
    private int ComputeMaxEvalStackDepth(IReadOnlyList<AotCoreIrInstructionArtifact> instructions,
        AotCoreIrAbiSlotArtifact? returnAbi = null,
        bool monotonic = false)
    {
        if (monotonic)
        {
            // StructuredSlotEmissionContext.AllocatePushTarget() assigns a new
            // monotonically-increasing slot ID on every push and never reuses IDs.
            // So we count TOTAL CUMULATIVE PUSHES across the entire instruction
            // stream — NOT peak concurrent depth.  Each push consumes exactly one
            // slot declaration regardless of how many pops happen between them.
            int totalPushes = 0;
            foreach (var instr in instructions)
            {
                string op = instr.Op;
                switch (op)
                {
                    case "ldc.i4": case "ldc.i8": case "ldc.r4": case "ldc.r8":
                    case "ldarg": case "ldstr": case "ldtoken": case "ldarga":
                    case "ldnull": case "ldloc": case "ldloca":
                    case "ldsfld": case "ldsflda":
                    case "ldftn": case "newarr": case "sizeof":
                    case "dup":
                    case "arglist":
                        totalPushes++; break;

                    case "pop": case "stloc": case "starg": case "initobj":
                    case "stsfld":
                    case "throw":
                    case "brfalse": case "brtrue":
                        break;

                    case "initblk": case "stfld": case "stobj":
                    case "stind.i4": case "stind.i1": case "stind.i2":
                    case "stind.i8": case "stind.r4": case "stind.r8": case "stind.ref":
                    case "stind.i":
                    case "beq": case "bgt": case "blt": case "bge": case "ble":
                    case "bne.un": case "bge.un":
                        break;

                    case "stelem": case "stelem.i": case "stelem.ref":
                    case "cpblk":
                        break;

                    case "cgt.un": case "ceq": case "cgt": case "clt":
                    case "add": case "sub": case "mul": case "div": case "div.un": case "rem": case "rem.un":
                    case "shl": case "shr": case "shr.un":
                    case "and": case "or": case "xor":
                    case "add.ovf": case "sub.ovf": case "mul.ovf":
                    case "ldelem": case "ldelem.i": case "ldelem.ref": case "ldelema":
                        totalPushes++; break;  // net: pop 2 push 1 = one new slot

                    case "ldfld": case "ldflda":
                    case "ldind.i4": case "ldind.u1": case "ldind.i1":
                    case "ldind.u2": case "ldind.i2": case "ldind.u4":
                    case "ldind.i8": case "ldind.r4": case "ldind.r8": case "ldind.ref":
                    case "ldind.i":
                    case "box": case "unbox": case "unbox.any":
                    case "castclass": case "isinst":
                    case "ldobj": case "ldlen": case "localloc":
                    case "conv.i4": case "conv.i1": case "conv.i2": case "conv.i8":
                    case "conv.u8": case "conv.r4": case "conv.r8": case "conv.u":
                    case "conv.u1": case "conv.u2": case "conv.u4":
                    case "conv.r.un": case "ckfinite":
                    case "conv.ovf.i1": case "conv.ovf.u1": case "conv.ovf.i2": case "conv.ovf.u2":
                    case "conv.ovf.i4": case "conv.ovf.u4": case "conv.ovf.i8": case "conv.ovf.u8":
                    case "conv.ovf.i": case "conv.ovf.u": case "conv.ovf.i8.un": case "conv.ovf.u8.un": case "conv.ovf.i.un": case "conv.ovf.u.un":
                    case "conv.ovf.i1.un": case "conv.ovf.i2.un": case "conv.ovf.i4.un":
                    case "conv.ovf.u1.un": case "conv.ovf.u2.un": case "conv.ovf.u4.un":
                    case "not":
                    case "mkrefany":
                        totalPushes += 2; break;  // pop 1 ptr, push 2 (typeHandle + ptr)
                    case "refanyval": case "refanytype":
                    case "ldvirtftn":
                        totalPushes++; break;  // pop 2 push 1 (refanyval/refanytype) or pop 1 push 1 (ldvirtftn)

                    case "jmp":
                        break;  // forwards chaos_args directly, no stack effect

                    case "call": case "callvirt": case "calli": case "newobj":
                        string? callee = instr.Callee ?? instr.TargetReference?.SubjectId;
                        if (!string.IsNullOrEmpty(callee) && TryGetLowerableMethod(callee) is { } lowerableMethod)
                        {
                            if (op == "newobj") totalPushes++;  // push new object
                            else if (lowerableMethod.ReturnAbi.CarrierKindCode != AotCoreIrAbiCarrierKind.Void) totalPushes++;
                        }
                        else if (!string.IsNullOrEmpty(callee) &&
                                 TryResolveDirectInvocationTarget(callee) is { } resolvedTarget)
                        {
                            if (op == "newobj") totalPushes++;
                            else if (resolvedTarget.ReturnAbi.CarrierKindCode != AotCoreIrAbiCarrierKind.Void) totalPushes++;
                        }
                        else
                        {
                            if (op == "newobj") totalPushes++;
                            else
                            {
                                string? retType = instr.TargetReturnType;
                                if (string.IsNullOrEmpty(retType) && !string.IsNullOrEmpty(callee))
                                    retType = InferReturnTypeFromSubjectId(callee);
                                if (!string.IsNullOrEmpty(retType) && retType != "System.Void") totalPushes++;
                            }
                        }
                        break;

                    case "ret": case "br": case "leave": case "switch":
                        break;

                    default:
                        totalPushes++; break;
                }
            }
            return totalPushes;
        }

        int maxDepth = 0;
        int depth = 0;

        foreach (var instr in instructions)
        {
            string op = instr.Op;
            int pushes = 0, pops = 0;

            switch (op)
            {
                // Pure pushes (+1)
                case "ldc.i4": case "ldc.i8": case "ldc.r4": case "ldc.r8":
                case "ldarg": case "ldstr": case "ldtoken": case "ldarga":
                case "ldnull": case "ldloc": case "ldloca":
                case "ldsfld": case "ldsflda":
                case "ldftn": case "newarr": case "sizeof":
                    pushes = 1; pops = 0; break;

                // Dup: push a copy of the top value
                case "dup":
                case "arglist":
                    pushes = 1; pops = 0; break;

                // Pure pops (-1)
                case "pop": case "stloc": case "starg": case "initobj":
                case "stsfld":
                case "throw":
                case "brfalse": case "brtrue":
                    pushes = 0; pops = 1; break;

                // Pops 2 (instance stores, indirect stores, conditional branches)
                case "stfld":
                case "stobj":
                case "stind.i4": case "stind.i1": case "stind.i2":
                case "stind.i8": case "stind.r4": case "stind.r8": case "stind.ref":
                case "stind.i":
                case "beq": case "bgt": case "blt": case "bge": case "ble":
                case "bne.un": case "bge.un":
                    pushes = 0; pops = 2; break;

                // Pops 3
                case "stelem": case "stelem.i": case "stelem.ref":
                case "cpblk":
                    pushes = 0; pops = 3; break;

                // Pop 2, push 1 (net -1)
                case "cgt.un": case "ceq": case "cgt": case "clt":
                case "add": case "sub": case "mul": case "div": case "div.un": case "rem": case "rem.un":
                case "shl": case "shr": case "shr.un":
                case "and": case "or": case "xor":
                case "add.ovf": case "sub.ovf": case "mul.ovf": case "add.ovf.un": case "sub.ovf.un": case "mul.ovf.un":
                case "ldelem": case "ldelem.i": case "ldelem.ref": case "ldelema":
                    pushes = 1; pops = 2; break;

                // Pop 1, push 1 (net 0 鈥?in-place transformation)
                case "ldfld": case "ldflda":
                case "ldind.i4": case "ldind.u1": case "ldind.i1":
                case "ldind.u2": case "ldind.i2": case "ldind.u4":
                case "ldind.i8": case "ldind.r4": case "ldind.r8": case "ldind.ref":
                case "ldind.i":
                case "box": case "unbox": case "unbox.any":
                case "castclass": case "isinst":
                case "ldobj": case "ldlen": case "localloc":
                case "conv.i4": case "conv.i1": case "conv.i2": case "conv.i8":
                case "conv.u8": case "conv.r4": case "conv.r8": case "conv.u":
                case "conv.u1": case "conv.u2": case "conv.u4":
                case "conv.r.un": case "ckfinite":
                case "conv.ovf.i1": case "conv.ovf.u1": case "conv.ovf.i2": case "conv.ovf.u2":
                case "conv.ovf.i4": case "conv.ovf.u4": case "conv.ovf.i8": case "conv.ovf.u8":
                case "conv.ovf.i": case "conv.ovf.u": case "conv.ovf.i8.un": case "conv.ovf.u8.un": case "conv.ovf.i.un": case "conv.ovf.u.un":
                    case "conv.ovf.i1.un": case "conv.ovf.i2.un": case "conv.ovf.i4.un":
                    case "conv.ovf.u1.un": case "conv.ovf.u2.un": case "conv.ovf.u4.un":
                case "not":
                case "mkrefany":
                    pushes = 2; pops = 1; break;  // pop 1 ptr, push 2 (typeHandle + ptr)
                case "refanyval": case "refanytype":
                    pushes = 1; pops = 2; break;  // pop 2 (typeHandle+ptr), push 1 result
                case "ldvirtftn":
                    pushes = 1; pops = 1; break;

                // Call/callvirt/calli: pop N args, push 0/1 result
                // newobj: pop constructor args, push new object/value
                case "call": case "callvirt": case "calli": case "newobj":
                    string? callee = instr.Callee ?? instr.TargetReference?.SubjectId;
                    if (!string.IsNullOrEmpty(callee) && TryGetLowerableMethod(callee) is { } lowerableMethod)
                    {
                        // Method is in _methodsBySubjectId — use ABI-level parameter count.
                        // For non-static methods, GetMethodAbiParameterSlots prepends a `this`
                        // pointer that is NOT on the IL eval stack for newobj (it's the new object).
                        pops = GetMethodAbiParameterSlots(lowerableMethod).Count;
                        if (op == "newobj") pops--;
                        pushes = (op == "newobj")
                            ? 1
                            : (lowerableMethod.ReturnAbi.CarrierKindCode != AotCoreIrAbiCarrierKind.Void) ? 1 : 0;
                    }
                    else if (!string.IsNullOrEmpty(callee) &&
                             TryResolveDirectInvocationTarget(callee) is { } resolvedTarget)
                    {
                        // Use the same resolution as EmitLinearCall: external runtime helper
                        // or legacy ABI.  This matches the exact parameter count the emission
                        // path will pop from the structured slot stack.
                        // Same newobj caveat: `this` is in the ABI but not on the IL stack.
                        pops = resolvedTarget.ParameterAbis.Count;
                        if (op == "newobj") pops--;
                        pushes = (op == "newobj")
                            ? 1
                            : (resolvedTarget.ReturnAbi.CarrierKindCode != AotCoreIrAbiCarrierKind.Void) ? 1 : 0;
                    }
                    else
                    {
                        // Fallback: infer from IL-level metadata when no resolution available.
                        // InferParameterCountFromSubjectId already returns IL-level count
                        // (no `this` pointer), so no newobj adjustment needed.
                        int? paramCount = instr.TargetParameterCount;
                        if (!paramCount.HasValue && !string.IsNullOrEmpty(callee))
                            paramCount = InferParameterCountFromSubjectId(callee);
                        pops = paramCount ?? 0;
                        string? retType = instr.TargetReturnType;
                        if (string.IsNullOrEmpty(retType) && !string.IsNullOrEmpty(callee))
                            retType = InferReturnTypeFromSubjectId(callee);
                        pushes = (op == "newobj")
                            ? 1
                            : (!string.IsNullOrEmpty(retType) && retType != "System.Void") ? 1 : 0;
                    }
                    break;

                // ret: method return (depth resets in non-monotonic mode)
                case "ret":
                    pushes = 0; pops = 0;
                    // Stack-balance guard: if the method is non-void and the simulated
                    // stack is empty at a ret, structured emission would underflow
                    // (EmitStructuredMethodReturn tries to consume a value that was
                    // never pushed).  Return -1 to signal "will underflow", causing
                    // TryBuildStructuredMethodBody to fallback to goto emission.
                    if (!monotonic && depth == 0 &&
                        returnAbi is { CarrierKindCode: not AotCoreIrAbiCarrierKind.Void })
                    {
                        return -1;
                    }
                    if (!monotonic)
                        depth = 0;
                    break;

                // jmp: forwards chaos_args directly, no stack effect
                case "jmp":
                    pushes = 0; pops = 0; break;

                // switch: pop index value
                case "switch":
                    pushes = 0; pops = 1; break;

                // br/leave: reset depth at unconditional branch target (non-monotonic only)
                case "br": case "leave":
                    pushes = 0; pops = 0;
                    if (!monotonic)
                        depth = 0;
                    break;

                default:
                    // Unknown opcode: be conservative, assume pushes 1
                    pushes = 1; pops = 0; break;
            }

            depth -= pops;
            if (depth < 0) depth = 0;
            depth += pushes;
            if (depth > maxDepth) maxDepth = depth;
        }

        return maxDepth;
    }

    // ════════════════════════════════════════════════════════════════════════════
    // Convenience: try to use this emitter for a set of instructions
    // ════════════════════════════════════════════════════════════════════════════

    /// <summary>
    /// Emit a list of instructions using the new StructuredIR emitter
    /// (no goto fallback).  Builds a CFG, runs structured recovery,
    /// and emits the IR tree.  For irreducible CFGs, falls back to
    /// flat goto emission via EmitInstructionRange.
    /// </summary>
    private bool TryBuildStructuredMethodBody(
        AotCoreIrMethodArtifact method,
        IReadOnlyList<AotCoreIrInstructionArtifact> instructions,
        IReadOnlySet<int> offsets,
        out StructuredIRNode? body,
        out int maxDepth)
    {
        body = null;
        maxDepth = 0;
        if (instructions.Count == 0)
        {
            body = new IRSequence(Array.Empty<StructuredIRNode>());
            return true;
        }

        if (ReferenceEquals(instructions, method.Instructions) &&
            method.ExceptionRegionCount > 0 &&
            method.ExceptionRegions is { Count: > 0 })
        {
            bool result = TryBuildStructuredExceptionMethodBody(method, instructions, offsets, out body, out maxDepth);
            if (!result || body is IRFlatRegion)
            {
                Interlocked.Increment(ref s_flatRegionCount);
                LogIrreducibleMethod(method, isException: true,
                    reason: body is IRFlatRegion ? "exception-shape-unhandled" : "exception-shape-failed",
                    instructions.Count, 0, 0, 0);
                // Fall back to flat-goto for the entire method body instead of
                // returning false (which produces an empty function body).
                // The IRFlatRegion path in EmitViaStructuredIR calls EmitFlatGotoBody.
                body = new IRFlatRegion(instructions, offsets);
                maxDepth = ComputeMaxEvalStackDepth(instructions);
                return true;
            }
            else
            {
                LogStructuredMethod(method, "exception-body", instructions.Count, 0, 0, 0);
            }
            return result;
        }

        var cfg = BuildControlFlowGraph(instructions, offsets);
        Console.WriteLine($"TRYBUILD: {method.SubjectId}, blocks={cfg.Blocks.Count}, loops={cfg.LoopHeaders.Count}, reducible={cfg.IsReducible}");
        if (!cfg.IsReducible)
        {
            var splitCfg = ApplyNodeSplitting(cfg);
            if (splitCfg.IsReducible)
            {
                cfg = splitCfg;
                LogStructuredMethod(method, "node-split", instructions.Count,
                    cfg.Blocks.Count, cfg.LoopHeaders.Count, method.ExceptionRegionCount);
            }
            else
            {
                Interlocked.Increment(ref s_flatRegionCount);
                body = new IRFlatRegion(instructions, offsets);
                maxDepth = ComputeMaxEvalStackDepth(instructions);
                LogIrreducibleMethod(method, isException: false,
                    reason: ClassifyIrreducibleReason(cfg) + "+split-failed",
                    instructions.Count, cfg.Blocks.Count, cfg.LoopHeaders.Count,
                    method.ExceptionRegionCount);
                return true;
            }
        }

        body = RecoverStructure(cfg, 0, cfg.Blocks.Count - 1);
        if (ContainsResidualBranchTerminators(body))
        {
            LogIrreducibleMethod(method, isException: false,
                reason: "residual-br-leave",
                instructions.Count, cfg.Blocks.Count, cfg.LoopHeaders.Count,
                method.ExceptionRegionCount);
            Interlocked.Increment(ref s_flatRegionCount);
            body = new IRFlatRegion(instructions, offsets);
        }
        else
        {
            LogStructuredMethod(method, "structured", instructions.Count,
                cfg.Blocks.Count, cfg.LoopHeaders.Count, method.ExceptionRegionCount);
        }

        maxDepth = ComputeMaxEvalStackDepth(instructions, method.ReturnAbi);
        if (maxDepth < 0)
        {
            Interlocked.Increment(ref s_flatRegionCount);
            body = new IRFlatRegion(instructions, offsets);
            maxDepth = ComputeMaxEvalStackDepth(instructions);
            LogIrreducibleMethod(method, isException: false,
                reason: "stack-underflow-on-ret",
                instructions.Count, cfg.Blocks.Count, cfg.LoopHeaders.Count,
                method.ExceptionRegionCount);
        }
        else
        {
            // Use monotonic computation to match StructuredSlotEmissionContext
            // which never reuses slot IDs across branch boundaries.
            maxDepth = ComputeMaxEvalStackDepth(instructions, monotonic: true);
        }
        return true;
    }

    private void EmitViaStructuredIR(
        StringBuilder builder,
        AotCoreIrMethodArtifact method,
        IReadOnlyList<AotCoreIrInstructionArtifact> instructions,
        IReadOnlySet<int> offsets,
        StructuredIRNode? body = null)
    {
        if (instructions.Count == 0)
            return;

        body ??= (TryBuildStructuredMethodBody(method, instructions, offsets, out var b, out _) ? b : null);
        if (body is null)
            return;

        TotalMethodCount++;

        if (body is IRFlatRegion flatRegion)
        {
            FlatFallbackCount++;
            _structLocalSlots = IdentifyStructLocalSlots(instructions);
            EmitFlatGotoBody(builder, method, flatRegion.Instructions, flatRegion.Offsets);
            _structLocalSlots = null;
            return;
        }

        if (method.ExceptionRegionCount > 0)
            StructuredExceptionBodyCount++;
        else
            StructuredMethodCount++;

        StructuredSlotEmissionContext? previousSlotContext = _activeStructuredSlotContext;
        _activeStructuredSlotContext = new StructuredSlotEmissionContext();
        _structuredSlotTypes.Clear();
        _structLocalSlots = IdentifyStructLocalSlots(instructions);
        try
        {
            EmitStructuredIRNode(builder, body!, method, "    ");
        }
        finally
        {
            _activeStructuredSlotContext = previousSlotContext;
            _structuredSlotTypes.Clear();
            _structLocalSlots = null;
        }
    }

    private bool TryBuildStructuredExceptionMethodBody(
        AotCoreIrMethodArtifact method,
        IReadOnlyList<AotCoreIrInstructionArtifact> instructions,
        IReadOnlySet<int> offsets,
        out StructuredIRNode? body,
        out int maxDepth)
    {
        body = null;
        maxDepth = 0;
        int extraHandlerPushes = 0;

        if (TryCreateCatchOnlyExceptionMethodShape(method, out var catchOnly) && catchOnly is not null)
        {
            body = BuildExceptionIRBody(
                catchOnly.PrefixInstructions,
                catchOnly.TryInstructions,
                catchOnly.HandlerInstructions,
                catchOnly.TailInstructions,
                IRExceptionKind.TryCatch,
                offsets,
                catchTypeSubjectId: catchOnly.ExceptionRegion.CatchTypeSubjectId);
            extraHandlerPushes = 1;
        }
        else if (TryCreateFilterOnlyExceptionMethodShape(method, out var filterOnly) && filterOnly is not null)
        {
            body = BuildExceptionIRBody(
                filterOnly.PrefixInstructions,
                filterOnly.TryInstructions,
                filterOnly.HandlerInstructions,
                filterOnly.TailInstructions,
                IRExceptionKind.TryFilter,
                offsets,
                filterInstructions: filterOnly.FilterInstructions,
                catchTypeSubjectId: filterOnly.FilterRegion.CatchTypeSubjectId);
            extraHandlerPushes = 2;
        }
        else if (TryCreateFinallyOnlyExceptionMethodShape(method, out var finallyOnly) && finallyOnly is not null)
        {
            body = BuildFinallyOnlyExceptionIRBody(finallyOnly, offsets);
            // finally-only has no caught-exception push
        }
        else if (TryCreateCatchAndFinallyExceptionMethodShape(method, out var catchAndFinally) && catchAndFinally is not null)
        {
            body = BuildCatchAndFinallyExceptionIRBody(catchAndFinally, offsets);
            extraHandlerPushes = 1;
        }
        else if (TryCreateFilterAndFinallyExceptionMethodShape(method, out var filterAndFinally) && filterAndFinally is not null)
        {
            body = BuildFilterAndFinallyExceptionIRBody(filterAndFinally, offsets);
            extraHandlerPushes = 2;
        }
        else if (TryCreateMultipleCatchExceptionMethodShape(method, out var multiCatch) && multiCatch is not null)
        {
            body = BuildMultipleCatchExceptionIRBody(multiCatch, offsets);
            extraHandlerPushes = 1;
        }

        if (body is null)
        {
            return false;
        }

        // Exception handlers synthesize pushes of the caught exception value onto
        // the eval stack (see EmitExceptionRegion at line ~1134 for TryCatch, and
        // lines ~1346/1382/1412/1428/1456 for TryFilter).  These are NOT present
        // in the IL instruction stream, so ComputeMaxEvalStackDepth cannot count
        // them.  Add the extra slot declarations here to match.
        //   - TryCatch: 1 push (caught exception object_value)
        //   - TryFilter: 2 pushes (caught exception + filter body re-push)
        //   - finally-only: 0 pushes
        maxDepth = ComputeMaxEvalStackDepth(instructions, monotonic: true) + extraHandlerPushes;
        return true;
    }

    /// <summary>
    /// Build a StructuredIR tree for an exception region method body.
    /// Partitions the instructions into prefix/try/handler/tail and wraps
    /// the try+handler in an IRExceptionRegion node.
    /// </summary>
    private StructuredIRNode BuildExceptionIRBody(
        IReadOnlyList<AotCoreIrInstructionArtifact> prefix,
        IReadOnlyList<AotCoreIrInstructionArtifact> tryBody,
        IReadOnlyList<AotCoreIrInstructionArtifact> handler,
        IReadOnlyList<AotCoreIrInstructionArtifact> tail,
        IRExceptionKind kind,
        IReadOnlySet<int> offsets,
        string? catchTypeSubjectId = null,
        IReadOnlyList<AotCoreIrInstructionArtifact>? filterInstructions = null)
    {
        var nodes = new List<StructuredIRNode>();

        // Build IR tree for prefix (instructions before the try block)
        if (prefix.Count > 0)
        {
            var prefixOffsets = new HashSet<int>(prefix.Select(GetRequiredIlOffset));
            var prefixCfg = BuildControlFlowGraph(prefix, prefixOffsets);
            if (prefixCfg.IsReducible)
                nodes.Add(RecoverStructure(prefixCfg, 0, prefixCfg.Blocks.Count - 1));
            else
                nodes.Add(new IRSequence(Array.Empty<StructuredIRNode>()));
        }

        // Build IR trees for try and handler bodies
        var tryTree = BuildExceptionPartitionTree(tryBody, offsets);
        var handlerTree = BuildExceptionPartitionTree(handler, offsets);

        nodes.Add(new IRExceptionRegion(
            kind, tryTree, handlerTree,
            CatchTypeSubjectId: catchTypeSubjectId,
            FilterInstructions: filterInstructions));

        // Build IR tree for tail (instructions after the handler)
        if (tail.Count > 0)
        {
            var tailTree = BuildExceptionPartitionTree(tail, offsets);
            nodes.Add(tailTree);
        }

        return nodes.Count == 1 ? nodes[0] : new IRSequence(nodes);
    }

    private StructuredIRNode BuildFinallyOnlyExceptionIRBody(
        FinallyOnlyExceptionMethodShape finallyOnly,
        IReadOnlySet<int> offsets)
    {
        StructuredIRNode inner = BuildExceptionPartitionTree(finallyOnly.TryInstructions, offsets);
        for (int i = finallyOnly.FinallyHandlers.Count - 1; i >= 0; i--)
        {
            inner = new IRExceptionRegion(
                IRExceptionKind.TryFinally,
                inner,
                BuildExceptionPartitionTree(finallyOnly.FinallyHandlers[i].Instructions, offsets));
        }

        var nodes = new List<StructuredIRNode>();
        if (finallyOnly.PrefixInstructions.Count > 0)
            nodes.Add(BuildExceptionPartitionTree(finallyOnly.PrefixInstructions, offsets));
        nodes.Add(inner);
        if (finallyOnly.TailInstructions.Count > 0)
            nodes.Add(BuildExceptionPartitionTree(finallyOnly.TailInstructions, offsets));
        return nodes.Count == 1 ? nodes[0] : new IRSequence(nodes);
    }

    private StructuredIRNode BuildCatchAndFinallyExceptionIRBody(
        CatchAndFinallyExceptionMethodShape catchAndFinally,
        IReadOnlySet<int> offsets)
    {
        var tryNodes = new List<StructuredIRNode>();
        if (catchAndFinally.PreInnerFinallyInstructions.Count > 0)
            tryNodes.Add(BuildExceptionPartitionTree(catchAndFinally.PreInnerFinallyInstructions, offsets));

        StructuredIRNode innerTry = BuildExceptionPartitionTree(catchAndFinally.InnerTryInstructions, offsets);
        if (catchAndFinally.InnerFinallyHandler is not null)
        {
            innerTry = new IRExceptionRegion(
                IRExceptionKind.TryFinally,
                innerTry,
                BuildExceptionPartitionTree(catchAndFinally.InnerFinallyHandler.Instructions, offsets));
        }
        tryNodes.Add(innerTry);

        if (catchAndFinally.PostInnerTryInstructions.Count > 0)
            tryNodes.Add(BuildExceptionPartitionTree(catchAndFinally.PostInnerTryInstructions, offsets));

        StructuredIRNode catchRegion = new IRExceptionRegion(
            IRExceptionKind.TryCatch,
            tryNodes.Count == 1 ? tryNodes[0] : new IRSequence(tryNodes),
            BuildExceptionPartitionTree(catchAndFinally.HandlerInstructions, offsets),
            CatchTypeSubjectId: catchAndFinally.CatchRegion.CatchTypeSubjectId);

        StructuredIRNode wrapped = catchRegion;
        for (int i = catchAndFinally.OuterFinallyHandlers.Count - 1; i >= 0; i--)
        {
            wrapped = new IRExceptionRegion(
                IRExceptionKind.TryFinally,
                wrapped,
                BuildExceptionPartitionTree(catchAndFinally.OuterFinallyHandlers[i].Instructions, offsets));
        }

        var nodes = new List<StructuredIRNode>();
        if (catchAndFinally.PrefixInstructions.Count > 0)
            nodes.Add(BuildExceptionPartitionTree(catchAndFinally.PrefixInstructions, offsets));
        nodes.Add(wrapped);
        if (catchAndFinally.TailInstructions.Count > 0)
            nodes.Add(BuildExceptionPartitionTree(catchAndFinally.TailInstructions, offsets));
        return nodes.Count == 1 ? nodes[0] : new IRSequence(nodes);
    }

    private StructuredIRNode BuildFilterAndFinallyExceptionIRBody(
        FilterAndFinallyExceptionMethodShape filterAndFinally,
        IReadOnlySet<int> offsets)
    {
        StructuredIRNode filterRegion = new IRExceptionRegion(
            IRExceptionKind.TryFilter,
            BuildExceptionPartitionTree(filterAndFinally.TryInstructions, offsets),
            BuildExceptionPartitionTree(filterAndFinally.HandlerInstructions, offsets),
            CatchTypeSubjectId: filterAndFinally.FilterRegion.CatchTypeSubjectId,
            FilterInstructions: filterAndFinally.FilterInstructions);

        StructuredIRNode wrapped = filterRegion;
        for (int i = filterAndFinally.FinallyHandlers.Count - 1; i >= 0; i--)
        {
            wrapped = new IRExceptionRegion(
                IRExceptionKind.TryFinally,
                wrapped,
                BuildExceptionPartitionTree(filterAndFinally.FinallyHandlers[i].Instructions, offsets));
        }

        var nodes = new List<StructuredIRNode>();
        if (filterAndFinally.PrefixInstructions.Count > 0)
            nodes.Add(BuildExceptionPartitionTree(filterAndFinally.PrefixInstructions, offsets));
        nodes.Add(wrapped);
        if (filterAndFinally.TailInstructions.Count > 0)
            nodes.Add(BuildExceptionPartitionTree(filterAndFinally.TailInstructions, offsets));
        return nodes.Count == 1 ? nodes[0] : new IRSequence(nodes);
    }

    private StructuredIRNode BuildMultipleCatchExceptionIRBody(
        MultipleCatchExceptionMethodShape multiCatch,
        IReadOnlySet<int> offsets)
    {
        LogMultipleCatchShape();

        var nodes = new List<StructuredIRNode>();

        // Build IR tree for prefix (instructions before the try block)
        if (multiCatch.PrefixInstructions.Count > 0)
        {
            var prefixOffsets = new HashSet<int>(multiCatch.PrefixInstructions.Select(GetRequiredIlOffset));
            var prefixCfg = BuildControlFlowGraph(multiCatch.PrefixInstructions, prefixOffsets);
            if (prefixCfg.IsReducible)
                nodes.Add(RecoverStructure(prefixCfg, 0, prefixCfg.Blocks.Count - 1));
            else
                nodes.Add(new IRSequence(Array.Empty<StructuredIRNode>()));
        }

        // Build the shared try body tree
        var tryTree = BuildExceptionPartitionTree(multiCatch.TryInstructions, offsets);

        // For each catch region, create a sequential IRExceptionRegion with the shared try body.
        // The catch regions are siblings (sequential), not nested.
        for (int i = 0; i < multiCatch.CatchRegions.Count; i++)
        {
            var handlerTree = BuildExceptionPartitionTree(multiCatch.HandlerInstructionsList[i], offsets);
            nodes.Add(new IRExceptionRegion(
                IRExceptionKind.TryCatch,
                tryTree,
                handlerTree,
                CatchTypeSubjectId: multiCatch.CatchRegions[i].CatchTypeSubjectId));
        }

        // Build IR tree for tail (instructions after all handlers)
        if (multiCatch.TailInstructions.Count > 0)
        {
            var tailTree = BuildExceptionPartitionTree(multiCatch.TailInstructions, offsets);
            nodes.Add(tailTree);
        }

        return nodes.Count == 1 ? nodes[0] : new IRSequence(nodes);
    }

    /// <summary>
    /// Build a structured IR tree from a sub-list of instructions (for exception
    /// region partitions).  Returns an empty IRSequence if the partition has no
    /// instructions or the CFG is irreducible.
    /// </summary>
    private static StructuredIRNode BuildExceptionPartitionTree(
        IReadOnlyList<AotCoreIrInstructionArtifact> instructions,
        IReadOnlySet<int> offsets)
    {
        if (instructions.Count == 0)
            return new IRSequence(Array.Empty<StructuredIRNode>());

        // Include the full offset set so branch targets that point to
        // locations outside the current partition (e.g., after trailing
        // leave stripping removed the block-exit terminator) are recognized
        // as valid block boundaries by BuildControlFlowGraph.
        var subOffsets = new HashSet<int>(instructions.Select(GetRequiredIlOffset));
        subOffsets.UnionWith(offsets);

        var cfg = BuildControlFlowGraph(instructions, subOffsets);
        if (!cfg.IsReducible)
        {
            // The CFG can become irreducible when a branch target falls
            // between partition boundaries (the target instruction is in
            // a different partition).  Emit the instructions as a flat
            // sequence instead of dropping them silently.
            Console.Error.WriteLine(
                $"TRACE:EMIT exception-partition-fallback " +
                $"instructions={instructions.Count} blocks={cfg.Blocks.Count} " +
                $"firstOp={instructions[0].Op} lastOp={instructions[^1].Op}");
            if (instructions.Count >= 3)
                Console.Error.WriteLine(
                    $"TRACE:EMIT   ops={instructions[0].Op},{instructions[1].Op},{instructions[2].Op},...");
            return EmitExceptionPartitionFallback(instructions);
        }

        return StripExceptionPartitionExitTerminators(RecoverStructure(cfg, 0, cfg.Blocks.Count - 1));
    }

    /// <summary>
    /// Fallback emission for an exception partition whose CFG is irreducible
    /// (typically because branch targets cross partition boundaries).  Produces
    /// a single IRBlock containing all instructions without structured control
    /// flow, preserving correctness at the cost of flat linear emission.
    /// </summary>
    private static StructuredIRNode EmitExceptionPartitionFallback(
        IReadOnlyList<AotCoreIrInstructionArtifact> instructions)
    {
        // Separate trailing terminator from body instructions.
        // Only opcodes handled by EmitIRBlockTerminator need separation.
        int last = instructions.Count - 1;
        if (last >= 0)
        {
            var op = instructions[last].Op;
            if (op is "ret" or "throw" or "rethrow" or "leave" or "br" or "endfinally" or "endfilter")
                return new IRBlock(instructions.Take(last).ToList(), instructions[last]);
        }
        return new IRBlock(instructions, null);
    }

    // ════════════════════════════════════════════════════════════════════════════
    // Phase 1 diagnostics: irreducible CFG classification
    // ════════════════════════════════════════════════════════════════════════════

    private static long s_structuredMethodCount;
    private static long s_exceptionBodyCount;
    private static long s_irreducibleCount;
    private static long s_totalMethodCount;
    private static readonly System.Collections.Concurrent.ConcurrentDictionary<string, long> s_irreducibleReasons
        = new(System.StringComparer.Ordinal);
    private static long s_flatRegionCount;

    private static void LogStructuredMethod(
        AotCoreIrMethodArtifact method, string kind,
        int instrCount, int blocks, int loops, int exceptionRegions)
    {
        Interlocked.Increment(ref s_totalMethodCount);
        if (kind == "exception-body")
            Interlocked.Increment(ref s_exceptionBodyCount);
        else
            Interlocked.Increment(ref s_structuredMethodCount);

        System.Console.Error.WriteLine(
            $"TRACE:EMIT method={SafeShortName(method)} " +
            $"kind={kind} " +
            $"instr={instrCount} " +
            $"blocks={blocks} " +
            $"loops={loops} " +
            $"exceptions={exceptionRegions}");
    }

    private static void LogIrreducibleMethod(
        AotCoreIrMethodArtifact method, bool isException,
        string reason, int instrCount, int blocks, int loops, int exceptionRegions)
    {
        Interlocked.Increment(ref s_totalMethodCount);
        Interlocked.Increment(ref s_irreducibleCount);
        s_irreducibleReasons.AddOrUpdate(reason, 1, (_, existing) => existing + 1);

        System.Console.Error.WriteLine(
            $"TRACE:FLAT method={SafeShortName(method)} " +
            $"reason={reason} " +
            $"instr={instrCount} " +
            $"blocks={blocks} " +
            $"loops={loops} " +
            $"exceptions={exceptionRegions}");
    }

    private static string SafeShortName(AotCoreIrMethodArtifact method)
    {
        var id = method.SubjectId;
        if (string.IsNullOrEmpty(id)) return "<null>";
        return id.Length > 80 ? id.Substring(0, 80) : id;
    }

    private static string ClassifyIrreducibleReason(ControlFlowGraph cfg)
    {
        int unnaturalBackedges = 0;
        int crossNestedLoopPairs = 0;
        int switchWithLoopBack = 0;

        var blocks = cfg.Blocks;
        var headers = cfg.LoopHeaders;

        // Count unnatural backedges
        for (int i = 0; i < blocks.Count; i++)
        {
            var block = blocks[i];
            if (block.ConditionalTarget.HasValue)
            {
                if (cfg.OffsetToBlockIndex.TryGetValue(block.ConditionalTarget.Value, out var tgt) && tgt < i)
                {
                    if (!headers.ContainsKey(tgt) || !headers[tgt].LatchIndices.Contains(i))
                        unnaturalBackedges++;
                }
            }
            if (block.BranchTarget.HasValue)
            {
                if (cfg.OffsetToBlockIndex.TryGetValue(block.BranchTarget.Value, out var tgt) && tgt < i)
                {
                    if (!headers.ContainsKey(tgt) || !headers[tgt].LatchIndices.Contains(i))
                        unnaturalBackedges++;
                }
            }
            if (block.SwitchTargets.Count > 0)
            {
                foreach (var t in block.SwitchTargets)
                {
                    if (cfg.OffsetToBlockIndex.TryGetValue(t, out var tgt) && tgt < i)
                    {
                        if (!headers.ContainsKey(tgt))
                            switchWithLoopBack++;
                    }
                }
            }
        }

        // Count cross-nested loop pairs
        foreach (var kvp1 in headers)
        {
            foreach (var kvp2 in headers)
            {
                if (kvp1.Key >= kvp2.Key) continue;
                if (kvp1.Value.BodyIndices.Contains(kvp2.Key) &&
                    kvp2.Value.BodyIndices.Contains(kvp1.Key))
                    crossNestedLoopPairs++;
            }
        }

        // Determine primary reason
        if (crossNestedLoopPairs > 0 && unnaturalBackedges > 0)
            return $"multi-loop-cross-nested+{unnaturalBackedges}unat+{crossNestedLoopPairs}xnested";
        if (unnaturalBackedges > 1)
            return $"multi-entry-loop+{unnaturalBackedges}unat";
        if (switchWithLoopBack > 0)
            return $"switch-loop-back+{switchWithLoopBack}sw";
        if (unnaturalBackedges == 1)
            return $"single-unnatural-backedge";
        return $"unknown-irreducible-b{blocks}l{headers.Count}";
    }

    // ════════════════════════════════════════════════════════════════════════════
    // Report summary (call at end of codegen session)
    // ════════════════════════════════════════════════════════════════════════════

    internal static void LogPhase1Summary()
    {
        long total = s_totalMethodCount;
        long structured = s_structuredMethodCount;
        long exceptionBody = s_exceptionBodyCount;
        long flat = s_irreducibleCount;

        System.Console.Error.WriteLine("");
        System.Console.Error.WriteLine("╔══════════════════════════════════════════════════╗");
        System.Console.Error.WriteLine("║  Phase 1 — StructuredIR Coverage Summary        ║");
        System.Console.Error.WriteLine("╠══════════════════════════════════════════════════╣");
        System.Console.Error.WriteLine($"║  Total methods:          {total,8}                ║");
        System.Console.Error.WriteLine($"║  StructuredIR:           {structured,8} ({Pct(structured, total),6})       ║");
        System.Console.Error.WriteLine($"║  Exception-body:         {exceptionBody,8} ({Pct(exceptionBody, total),6})       ║");
        System.Console.Error.WriteLine($"║  Flat goto (total):      {flat,8} ({Pct(flat, total),6})       ║");
        System.Console.Error.WriteLine("╠══════════════════════════════════════════════════╣");
        System.Console.Error.WriteLine("║  Irreducible reasons:                            ║");

        foreach (var kvp in s_irreducibleReasons.OrderByDescending(k => k.Value))
        {
            System.Console.Error.WriteLine($"║    {kvp.Key,-36} {kvp.Value,4} ({Pct(kvp.Value, flat),6}) ║");
        }

        System.Console.Error.WriteLine("╚══════════════════════════════════════════════════╝");
        System.Console.Error.WriteLine("");
    }

    private static string Pct(long part, long total)
    {
        if (total == 0) return " 0.0%";
        return (part * 100.0 / total).ToString("F1") + "%";
    }

    internal static void LogMultipleCatchShape() =>
        System.Console.Error.WriteLine("[MultipleCatch] Detected multi-catch EH shape");
}



