using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
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
        // Depth counter guard: prevents process-terminating StackOverflowException
        // from recursive structured IR tree emission.  This is more reliable than
        // RuntimeHelpers.TryEnsureSufficientExecutionStack() which may not work
        // correctly on threads created with custom maxStackSize.
        _state.Value!.StructuredIrDepth++;
        if (_state.Value!.StructuredIrDepth > MaxStructuredIRDepth)
        {
            _state.Value!.StructuredIrDepth--;
            throw new InvalidOperationException(
                $"Structured IR tree too deep ({_state.Value!.StructuredIrDepth} levels) for method "
                + $"'{SafeShortName(method)}'. Falling back to linear emission.");
        }
        try
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

                case IRPcDispatch pcDispatch:
                    EmitPcDispatch(builder, pcDispatch, method, indentation);
                    break;

                default:
                    throw new NotSupportedException(
                        "StructuredIR: unknown node type '" + node.GetType().Name + "'");
            }
        }
        finally
        {
            _state.Value!.StructuredIrDepth--;
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
        // Emit pending branch skip labels from brtrue/brfalse fallback.
        // Emitted BEFORE the next block so fallthrough path skips target body.
        if (_state.Value!.PendingBranchSkipLabels is { Count: > 0 })
        {
            foreach (var sl in _state.Value!.PendingBranchSkipLabels)
                builder.AppendLine(indentation + $"{sl}:;");
            _state.Value!.PendingBranchSkipLabels.Clear();
        }
        // When an IRBlock is emitted as a child of IRSequence or inside a
        // branch body, it may start with instructions that pop values pushed
        // by predecessor CFG blocks. Ensure the structured slot depth is
        // adequate by simulating the block's stack effect — accounting for
        // call/callvirt/newobj argument pops via TargetParameterCount.
        if (_state.Value!.ActiveStructuredSlotContext is { } ctx)
        {
            int maxDeficit = 0;
            int simDepth = ctx.Depth;
            foreach (var instr in block.BodyInstructions)
            {
                int pops = EstimatePopCountForBlockDepth(instr);
                int pushes = EstimatePushCountForBlockDepth(instr);
                simDepth -= pops;
                if (simDepth < 0 && -simDepth > maxDeficit)
                    maxDeficit = -simDepth;
                simDepth += pushes;
            }
            for (int i = 0; i < maxDeficit; i++)
                ctx.AllocatePushTarget();
        }

        EmitInstructionLookahead(builder, block.BodyInstructions, indentation);

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

            case "brtrue":
            case "brfalse":
                // Fallback: when control flow recovery fails to wrap a conditional
                // branch in IRIfThenElse (e.g., complex CFG with mixed structured +
                // unstructured patterns), emit a goto-based conditional jump instead
                // of crashing.  The label is emitted at the current position so the
                // NEXT IR node (the branch target body) lands between the label and
                // the fallthrough goto.  This pattern:
                //   if (cond) goto chaos_br_N;
                //   goto chaos_skip_N;       // false: skip target block
                //   chaos_br_N:               // true branch target
                //   ... target block code ...
                //   chaos_skip_N:             // resume after target
                // Without the fallthrough goto, both paths converge at chaos_br_N
                // making the branch a no-op — the original bug.
                {
                    int labelIdx = Interlocked.Increment(ref s_structuredBrLabelSeq);
                    string label = $"chaos_br_{labelIdx}";
                    string skipLabel = $"chaos_skip_{labelIdx}";
                    string cond = ConsumeEvalStackValueExpression();
                    bool branchOnTrue = terminator.Op == "brtrue";
                    string condition = branchOnTrue
                        ? $"{cond} != 0"
                        : $"{cond} == 0";
                    builder.AppendLine(indentation + $"if ({condition}) goto {label};");
                    builder.AppendLine(indentation + $"goto {skipLabel};");
                    builder.AppendLine(indentation + $"{label}:;");
                    // The NEXT block emitted by the IR emitter will be the branch
                    // target body.  After it completes, the caller must emit skipLabel
                    // to allow fallthrough to resume correctly.
                    // We store the skip label so the structured IR loop body emitter
                    // can emit it after the target block.
                    _state.Value!.PendingBranchSkipLabels ??= new List<string>();
                    _state.Value!.PendingBranchSkipLabels.Add(skipLabel);
                }
                break;

            case "switch":
                // Fallback: emit a flat switch when IR recovery fails to wrap the
                // switch terminator in IRSwitch (complex irreducible CFG).
                {
                    string switchExpr = ConsumeEvalStackValueExpression();
                    builder.AppendLine(indentation + $"switch ({switchExpr})");
                    builder.AppendLine(indentation + "{");
                    builder.AppendLine(indentation + "    default: break;");
                    builder.AppendLine(indentation + "}");
                }
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
            preConditionDepth = _state.Value!.ActiveStructuredSlotContext?.Depth ?? 0;
        }


        // Compute minimum required depth for condition instructions using
        // the estimated push/pop counts. This compensates for values pushed
        // by predecessor CFG blocks that the condition instructions consume.
        {
            int condPushes = 0, condPops = 0;
            foreach (var instr in ite.ConditionInstructions)
            {
                condPushes += EstimatePushCount(instr.Op);
                condPops += EstimatePopCount(instr.Op);
            }
            int termPops = EstimateTerminatorPopCount(terminator.Op);
            int requiredDepth = Math.Max(0, condPops + termPops - condPushes);
            if (preConditionDepth < requiredDepth)
                preConditionDepth = requiredDepth;
        }

        // Save preConditionDepth so we can restore it for the else/post-merge body.
        // postConditionDepth is captured from ACTUAL depth after condition+terminator
        // emission, as a safety net for any edge-case opcodes not covered by
        // EstimatePushCount/EstimatePopCount (the core comparison/arithmetic opcodes
        // ARE now covered — see EstimatePopCount).
        _state.Value!.ActiveStructuredSlotContext?.RestoreDepth(preConditionDepth);

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
            // Consumed the condition operand; capture actual depth for else/post-merge body.
            int postCondDepth = _state.Value!.ActiveStructuredSlotContext?.Depth ?? 0;
            string _condition = _cType switch
            {
                SlotType.Float32 => branchOnNonZero
                    ? $"ChaosLoadFloat32({_cSlot}) != 0.0f"
                    : $"ChaosLoadFloat32({_cSlot}) == 0.0f",
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
                _state.Value!.ActiveStructuredSlotContext?.RestoreDepth(postCondDepth);
                EmitStructuredIRNode(builder, ite.ElseBody, method, bodyIndent);
                builder.AppendLine(inner + "}");
            }

            if (ite.PostMergeBody != null)
            {
                _state.Value!.ActiveStructuredSlotContext?.RestoreDepth(postCondDepth);
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
                "bgt.un" => ">",
                "ble" => "<=",
                "ble.un" => "<=",
                "blt" => "<",
                "blt.un" => "<",
                _ => throw new NotSupportedException(
                    "StructuredIR: unsupported conditional branch '" + terminator.Op + "'")
            };

            bool isUnsigned = terminator.Op is "bge.un" or "bgt.un" or "ble.un" or "blt.un";
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
            // Captured actual depth after comparison + terminator consumption
            int postCondDepth = _state.Value!.ActiveStructuredSlotContext?.Depth ?? 0;
            string _cmpRight = _cmpRType switch
            {
                SlotType.Float32 => $"ChaosLoadFloat32({_cmpRExpr})",
                SlotType.Float64 => $"ChaosLoadFloat64({_cmpRExpr})",
                _ => isUnsigned
                    ? $"static_cast<CHAOS_IL2CPP_UINT32>(static_cast<CHAOS_IL2CPP_INT32>({_cmpRExpr}))"
                    : $"static_cast<{valueType}>({_cmpRExpr})",
            };
            string _cmpLeft = _cmpLType switch
            {
                SlotType.Float32 => $"ChaosLoadFloat32({_cmpLExpr})",
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
                _state.Value!.ActiveStructuredSlotContext?.RestoreDepth(postCondDepth);
                EmitStructuredIRNode(builder, ite.ElseBody, method, bodyIndent);
                builder.AppendLine(inner + "}");
            }

            if (ite.PostMergeBody != null)
            {
                _state.Value!.ActiveStructuredSlotContext?.RestoreDepth(postCondDepth);
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
            or "stind.i4" or "stind.i1" or "stind.i2" or "stind.i8"
            or "stind.r4" or "stind.r8" or "stind.ref" or "stind.i"
            or "cpblk" or "throw" => 2,
        "stelem" or "stelem.i" or "stelem.ref" or "initblk" => 3,
        // Comparison: pop 2, push 1
        "ceq" or "cgt" or "cgt.un" or "clt" or "clt.un" => 2,
        // Binary arithmetic: pop 2, push 1
        "add" or "sub" or "mul" or "div" or "div.un" or "rem" or "rem.un"
            or "shl" or "shr" or "shr.un"
            or "and" or "or" or "xor"
            or "add.ovf" or "sub.ovf" or "mul.ovf" => 2,
        // Unary: pop 1, push 1
        "neg" or "not" or "dup" => 1,
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



    /// <summary>
    /// Pop-count estimate for IRBlock depth compensation — handles call/callvirt/newobj
    /// argument pops via <see cref="AotCoreIrInstructionArtifact.TargetParameterCount"/>.
    /// </summary>
    private static int EstimatePopCountForBlockDepth(AotCoreIrInstructionArtifact instr)
    {
        return instr.Op switch
        {
            "call" or "callvirt" or "calli" => instr.TargetParameterCount
                ?? EstimateParamCountFromCallee(instr.Callee)
                ?? 1,  // conservative default when Callee is null
            "newobj" => instr.TargetParameterCount ?? 0,
            _ => EstimatePopCount(instr.Op)
        };
    }



    /// <summary>
    /// Parse parameter count from a SubjectId string like
    /// "Namespace.Type::Method:ReturnType(Param1,Param2)"
    /// </summary>
    private static int? EstimateParamCountFromCallee(string? callee)
    {
        if (string.IsNullOrEmpty(callee)) return null;
        int paren = callee.LastIndexOf('(');
        if (paren < 0) return null;
        int close = callee.IndexOf(')', paren);
        if (close < 0 || close == paren + 1) return null;
        string args = callee.Substring(paren + 1, close - paren - 1);
        int count = 1;
        int depth = 0;
        foreach (char c in args)
        {
            if (c == '<' || c == '[') depth++;
            else if (c == '>' || c == ']') depth--;
            else if (c == ',' && depth == 0) count++;
        }
        return count;
    }



    /// <summary>
    /// Push-count estimate for IRBlock depth compensation — for call/callvirt/calli,
    /// returns 0 for void returns and 1 otherwise; newobj always pushes 1.
    /// </summary>
    private static int EstimatePushCountForBlockDepth(AotCoreIrInstructionArtifact instr)
    {
        return instr.Op switch
        {
            "call" or "callvirt" or "calli" =>
                (!string.IsNullOrEmpty(instr.TargetReturnType) && instr.TargetReturnType != "System.Void") ? 1 : 0,
            "newobj" => 1,
            _ => EstimatePushCount(instr.Op)
        };
    }



    private void EmitIRWhileLoop(
        StringBuilder builder,
        IRWhileLoop w,
        AotCoreIrMethodArtifact method,
        string indentation)
    {
        string inner = indentation + "    ";
        string bodyIndent = inner + "    ";

        // Induction variable detection for array bounds check elimination
        if (_state.Value!.LoopArrayAccessSkipOffsets == null)
        {
            var bodyInstrs = new List<AotCoreIrInstructionArtifact>();
            CollectInstructions(w.Body, bodyInstrs);
            int? ivSlot = DetectInductionVariableSlot(bodyInstrs);
            if (ivSlot.HasValue)
            {
                var writtenSlots = new HashSet<int>();
                CollectWrittenSlots(w.Body, writtenSlots);
                writtenSlots.Add(ivSlot.Value);
                _state.Value!.LoopArrayAccessSkipOffsets = PreScanLoopArraySkips(w.Body, ivSlot.Value, writtenSlots);
            }
        }

        // ---- Loop induction variable hoisting ----
        var prevHoistedIVs = _state.Value!.HoistedIVs;
        int? hoistedIVSlot = null;
        if (prevHoistedIVs == null)
        {
            var bodyInstrs = new List<AotCoreIrInstructionArtifact>();
            CollectInstructions(w.Body, bodyInstrs);
            hoistedIVSlot = DetectInductionVariableSlot(bodyInstrs);
        }
        if (hoistedIVSlot.HasValue)
        {
            string ivName = $"_iv_{hoistedIVSlot.Value}";
            _state.Value!.HoistedIVs = new Dictionary<int, string> { { hoistedIVSlot.Value, ivName } };
        }

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
                    ? $"ChaosLoadFloat32({_cSlot}) != 0.0f"
                    : $"ChaosLoadFloat32({_cSlot}) == 0.0f",
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
            if (hoistedIVSlot.HasValue)
                builder.AppendLine(bodyIndent + $"_iv_{hoistedIVSlot.Value} = static_cast<CHAOS_IL2CPP_INT32>(chaos_locals[{hoistedIVSlot.Value}]);");
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
                "bgt.un" => ">",
                "ble" => "<=",
                "ble.un" => "<=",
                "blt" => "<",
                "blt.un" => "<",
                _ => throw new NotSupportedException(
                    "StructuredIR: unsupported while condition '" + terminator.Op + "'")
            };

            bool isUnsigned = terminator.Op is "bge.un" or "bgt.un" or "ble.un" or "blt.un";
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
            // Captured actual depth after comparison + terminator consumption
            int postCondDepth = _state.Value!.ActiveStructuredSlotContext?.Depth ?? 0;
            string _cmpRight = _cmpRType switch
            {
                SlotType.Float32 => $"ChaosLoadFloat32({_cmpRExpr})",
                SlotType.Float64 => $"ChaosLoadFloat64({_cmpRExpr})",
                _ => isUnsigned
                    ? $"static_cast<CHAOS_IL2CPP_UINT32>(static_cast<CHAOS_IL2CPP_INT32>({_cmpRExpr}))"
                    : $"static_cast<{valueType}>({_cmpRExpr})",
            };
            string _cmpLeft = _cmpLType switch
            {
                SlotType.Float32 => $"ChaosLoadFloat32({_cmpLExpr})",
                SlotType.Float64 => $"ChaosLoadFloat64({_cmpLExpr})",
                _ => isUnsigned
                    ? $"static_cast<CHAOS_IL2CPP_UINT32>(static_cast<CHAOS_IL2CPP_INT32>({_cmpLExpr}))"
                    : $"static_cast<{valueType}>({_cmpLExpr})",
            };
            builder.AppendLine(inner + $"const auto chaos_right = {_cmpRight};");
            builder.AppendLine(inner + $"const auto chaos_left = {_cmpLeft};");

            builder.AppendLine(inner + "while (chaos_left " + cmpOp + " chaos_right)");
            builder.AppendLine(inner + "{");
            if (hoistedIVSlot.HasValue)
                builder.AppendLine(bodyIndent + $"_iv_{hoistedIVSlot.Value} = static_cast<CHAOS_IL2CPP_INT32>(chaos_locals[{hoistedIVSlot.Value}]);");
            EmitStructuredIRNode(builder, w.Body, method, bodyIndent);
            builder.AppendLine(inner + "}");
            builder.AppendLine(indentation + "}");
        }
        _state.Value!.HoistedIVs = prevHoistedIVs;
        _state.Value!.LoopArrayAccessSkipOffsets = null;
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

        // Induction variable detection for array bounds check elimination
        if (_state.Value!.LoopArrayAccessSkipOffsets == null)
        {
            var bodyInstrs = new List<AotCoreIrInstructionArtifact>();
            CollectInstructions(dw.Body, bodyInstrs);
            if (dw.LatchInstructions != null)
                bodyInstrs.AddRange(dw.LatchInstructions);
            int? ivSlot = DetectInductionVariableSlot(bodyInstrs);
            if (ivSlot.HasValue)
            {
                var writtenSlots = new HashSet<int>();
                CollectWrittenSlots(dw.Body, writtenSlots);
                writtenSlots.Add(ivSlot.Value);
                _state.Value!.LoopArrayAccessSkipOffsets = PreScanLoopArraySkips(dw.Body, ivSlot.Value, writtenSlots);
            }
        }

        // ---- Loop induction variable hoisting ----
        // Promote detected IV to C++ local, eliminate chaos_locals[] traffic.
        var prevHoistedIVs = _state.Value!.HoistedIVs;
        int? hoistedIVSlot = null;
        if (prevHoistedIVs == null)
        {
            var bodyInstrs2 = new List<AotCoreIrInstructionArtifact>();
            CollectInstructions(dw.Body, bodyInstrs2);
            if (dw.LatchInstructions != null)
                bodyInstrs2.AddRange(dw.LatchInstructions);
            hoistedIVSlot = DetectInductionVariableSlot(bodyInstrs2);
            // Fallback 1: scan latch instructions alone (pattern may be split
            // across body-latch boundary during pipeline rebuild).
            if (!hoistedIVSlot.HasValue && dw.LatchInstructions != null && dw.LatchInstructions.Count >= 4)
                hoistedIVSlot = DetectInductionVariableSlot(dw.LatchInstructions);
            // Fallback 2: scan last 4 body instructions (some pipeline passes
            // move the increment op sequence to the body, leaving only the
            // comparison load in the latch).
            if (!hoistedIVSlot.HasValue)
            {
                // Take last 4, or fewer if body is small (minimum 4 needed for pattern)
                if (bodyInstrs2.Count >= 4)
                {
                    int _startIdx = Math.Max(0, bodyInstrs2.Count - 4);
                    var _tailSlice = bodyInstrs2.Skip(_startIdx).Take(4).ToList();
                    hoistedIVSlot = DetectInductionVariableSlot(_tailSlice);
                }
            }
        }
        if (hoistedIVSlot.HasValue)
        {
            string ivName = $"_iv_{hoistedIVSlot.Value}";
            _state.Value!.HoistedIVs = new Dictionary<int, string> { { hoistedIVSlot.Value, ivName } };
        }

        // ---- Array base pointer hoisting ----
        // Detect loop-invariant array local slots and hoist the base pointer computation
        // (reinterpret_cast + chaos_array_get_elements) outside the loop body.
        _state.Value!.HoistedArrayBaseSlots = null;
        _state.Value!.SlotVarToLocalSlot = null;
        var bodyInstrs3 = new List<AotCoreIrInstructionArtifact>();
        CollectInstructions(dw.Body, bodyInstrs3);
        if (dw.LatchInstructions != null)
            bodyInstrs3.AddRange(dw.LatchInstructions);
        if (bodyInstrs3.Count > 0)
        {
            var writtenSlots = new HashSet<int>();
            CollectWrittenSlots(dw.Body, writtenSlots);
            var invariantArraySlots = DetectInvariantArraySlots(bodyInstrs3, writtenSlots);
            if (invariantArraySlots.Count > 0)
            {
                _state.Value!.HoistedArrayBaseSlots = invariantArraySlots;
                _state.Value!.SlotVarToLocalSlot = new Dictionary<string, int>();
                builder.AppendLine(bodyIndent + "{");
                foreach (var kvp in invariantArraySlots)
                {
                    int slot = kvp.Key;
                    string basePtr = kvp.Value;
                    builder.AppendLine(bodyIndent + $"auto* {basePtr} = chaos_array_get_elements(reinterpret_cast<chaos_managed_array*>(chaos_locals[{slot}]));");
                }
            }
        }

        // ---- Invariant local hoisting (E6) ----
        // Hoist ldloc's of slots that are never stloc'd inside the loop body.
        var prevHoistedInvariantLocals = _state.Value!.HoistedInvariantLocals;
        _state.Value!.HoistedInvariantLocals = null;
        if (bodyInstrs3.Count > 0)
        {
            var readSlots = new HashSet<int>();
            var writtenSlots2 = new HashSet<int>();
            CollectWrittenSlots(dw.Body, writtenSlots2);
            foreach (var instr in bodyInstrs3)
            {
                if (instr.Op == "ldloc" && TryGetIntOperand(instr, out int ldSlot))
                    readSlots.Add(ldSlot);
            }
            readSlots.ExceptWith(writtenSlots2);
            if (hoistedIVSlot.HasValue)
                readSlots.Remove(hoistedIVSlot.Value);
            if (_state.Value!.AccumulatorSlots is not null)
            {
                foreach (var accSlot in _state.Value!.AccumulatorSlots.Keys)
                    readSlots.Remove(accSlot);
            }
            if (readSlots.Count > 0)
            {
                var hoisted = new Dictionary<int, (string VarName, SlotType SlotType)>();
                foreach (int slot in readSlots.OrderBy(s => s))
                {
                    string varName = $"_hld_{slot}";
                    SlotType slotType;
                    string declType;
                    string loadExpr;
                    if (_state.Value!.Int64LocalSlots is not null && _state.Value!.Int64LocalSlots.Contains(slot))
                    {
                        slotType = SlotType.Int64;
                        declType = "CHAOS_IL2CPP_INT64";
                        loadExpr = $"ChaosLoadInt64(chaos_locals[{slot}])";
                    }
                    else if (_state.Value!.FloatLocalSlots is not null && _state.Value!.FloatLocalSlots.TryGetValue(slot, out var fType) && fType != SlotType.NativeInt)
                    {
                        slotType = fType;
                        declType = fType == SlotType.Float64 ? "double" : "float";
                        string wrapper = fType == SlotType.Float64 ? "ChaosLoadFloat64" : "ChaosLoadFloat32";
                        loadExpr = $"{wrapper}(chaos_locals[{slot}])";
                    }
                    else if (_state.Value!.StructLocalSlots is not null && _state.Value!.StructLocalSlots.Contains(slot))
                    {
                        slotType = SlotType.NativeInt;
                        declType = "CHAOS_IL2CPP_INTPTR";
                        loadExpr = $"reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[{slot}])";
                    }
                    else
                    {
                        slotType = SlotType.NativeInt;
                        declType = "CHAOS_IL2CPP_INTPTR";
                        loadExpr = $"chaos_locals[{slot}]";
                    }
                    builder.AppendLine(bodyIndent + $"{declType} {varName} = {loadExpr};");
                    (_state.Value!.EmittedHoistedLocals ??= new System.Collections.Generic.HashSet<int>()).Add(slot);
                    hoisted[slot] = (varName, slotType);
                }
                // Merge with outer-scope hoisted locals: inner scope may need
                // variables that the outer scope didn't hoist (the outer scope's
                // ldloc scan may not have covered all slots used in nested scopes).
                if (prevHoistedInvariantLocals is not null)
                {
                    foreach (var kvp in prevHoistedInvariantLocals)
                        hoisted.TryAdd(kvp.Key, kvp.Value);
                }
                _state.Value!.HoistedInvariantLocals = hoisted;
            }
        }

        // ---- Accumulator promotion (E7) ----
        var prevAccumulatorSlots = _state.Value!.AccumulatorSlots;
        _state.Value!.AccumulatorSlots = null;
        if (prevAccumulatorSlots == null && prevHoistedIVs == null)
        {
            var accBodyInstrs = new List<AotCoreIrInstructionArtifact>();
            CollectInstructions(dw.Body, accBodyInstrs);
            if (dw.LatchInstructions != null)
                accBodyInstrs.AddRange(dw.LatchInstructions);
            var accResult = DetectAccumulatorSlots(accBodyInstrs);
            if (accResult is { Count: > 0 })
            {
                _state.Value!.AccumulatorSlots = accResult;
                foreach (var kvp in accResult)
                {
                    int slot = kvp.Key;
                    string accName = kvp.Value;
                    builder.AppendLine(bodyIndent + $"CHAOS_IL2CPP_INT64 {accName} = ChaosLoadInt64(chaos_locals[{slot}]);");
                }
            }
        }

        builder.AppendLine(indentation + "do");
        builder.AppendLine(indentation + "{");
        if (hoistedIVSlot.HasValue)
            builder.AppendLine(bodyIndent + $"CHAOS_IL2CPP_INT32 _iv_{hoistedIVSlot.Value} = static_cast<CHAOS_IL2CPP_INT32>(chaos_locals[{hoistedIVSlot.Value}]);");
        EmitStructuredIRNode(builder, dw.Body, method, bodyIndent);

        // Emit latch instructions if present
        if (dw.LatchTerminator != null)
        {
            var filteredLatch = FilterRedundantStoreReloadPairs(dw.LatchInstructions!);
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
                        ? $"ChaosLoadFloat32({_cSlot}) != 0.0f"
                        : $"ChaosLoadFloat32({_cSlot}) == 0.0f",
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
                    "bgt.un" => ">",
                    "ble" => "<=",
                    "ble.un" => "<=",
                    "blt" => "<",
                    "blt.un" => "<",
                    _ => throw new NotSupportedException(
                        "StructuredIR: unsupported do-while latch '" + terminator.Op + "'")
                };

                bool isUnsigned = terminator.Op is "bge.un" or "bgt.un" or "ble.un" or "blt.un";
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
                    SlotType.Float32 => $"ChaosLoadFloat32({cmpRExpr})",
                    SlotType.Float64 => $"ChaosLoadFloat64({cmpRExpr})",
                    _ => isUnsigned
                        ? $"static_cast<CHAOS_IL2CPP_UINT32>(static_cast<CHAOS_IL2CPP_INT32>({cmpRExpr}))"
                        : $"static_cast<{valueType}>({cmpRExpr})",
                };
                string cmpLeft = cmpLType switch
                {
                    SlotType.Float32 => $"ChaosLoadFloat32({cmpLExpr})",
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

        if (hoistedIVSlot.HasValue)
            builder.AppendLine(indentation + $"chaos_locals[{hoistedIVSlot.Value}] = _iv_{hoistedIVSlot.Value};");
        // E7: Write back accumulator variables to chaos_locals after the loop
        if (_state.Value!.AccumulatorSlots is { Count: > 0 })
        {
            foreach (var kvp in _state.Value!.AccumulatorSlots)
                builder.AppendLine(indentation + $"chaos_locals[{kvp.Key}] = ChaosStoreInt64({kvp.Value});");
        }
        builder.AppendLine(indentation + "} while (true);");
        if (_state.Value!.HoistedArrayBaseSlots is { Count: > 0 })
            builder.AppendLine(bodyIndent + "}");
        _state.Value!.HoistedIVs = prevHoistedIVs;
        _state.Value!.LoopArrayAccessSkipOffsets = null;
        _state.Value!.HoistedArrayBaseSlots = null;
        _state.Value!.SlotVarToLocalSlot = null;
        _state.Value!.HoistedInvariantLocals = prevHoistedInvariantLocals;
        _state.Value!.AccumulatorSlots = prevAccumulatorSlots;
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
                    int preTryDepth = _state.Value!.ActiveStructuredSlotContext?.Depth ?? 0;
                    builder.AppendLine(indentation + "CHAOS_EH_TRY");
                    EmitStructuredIRNode(builder, er.TryBody, method, bodyIndent);
                    if (er.CatchTypeSubjectId != null)
                        _state.Value!.ActiveStructuredSlotContext?.RestoreDepth(preTryDepth);
                    builder.AppendLine(indentation + "CHAOS_EH_CATCH_BEGIN");
                    if (er.CatchTypeSubjectId != null)
                    {
                        string typeInfoSym = GetNativeTypeInfoSymbol(er.CatchTypeSubjectId);
                        builder.AppendLine(inner +
                            $"if (!chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ, {typeInfoSym}))");
                        builder.AppendLine(inner + "{ CHAOS_EH_RETHROW; }");
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
    /// </summary>
    private int ComputeMaxEvalStackDepth(IReadOnlyList<AotCoreIrInstructionArtifact> instructions,
        AotCoreIrAbiSlotArtifact? returnAbi = null)
    {
        int maxDepth = 0;
        int depth = 0;

        foreach (var instr in instructions)
        {
            string op = instr.Op;
            int pushes = 0, pops = 0;

            switch (op)
            {
                // Pure pushes (+1)
                case "ldc.i4":
                case "ldc.i8":
                case "ldc.r4":
                case "ldc.r8":
                case "ldarg":
                case "ldstr":
                case "ldtoken":
                case "ldarga":
                case "ldnull":
                case "ldloc":
                case "ldloca":
                case "ldsfld":
                case "ldsflda":
                case "ldftn":
                case "newarr":
                case "sizeof":
                    pushes = 1; pops = 0; break;

                // Dup: push a copy of the top value
                case "dup":
                case "arglist":
                    pushes = 1; pops = 0; break;

                // Pure pops (-1)
                case "pop":
                case "stloc":
                case "stloc.s":
                case "starg":
                case "initobj":
                case "throw":
                case "brfalse":
                case "brtrue":
                    pushes = 0; pops = 1; break;

                // Pops 2 (instance stores, indirect stores, conditional branches)
                case "stfld":
                case "stsfld":
                case "stobj":
                case "stind.i4":
                case "stind.i1":
                case "stind.i2":
                case "stind.i8":
                case "stind.r4":
                case "stind.r8":
                case "stind.ref":
                case "stind.i":
                case "beq":
                case "bgt":
                case "blt":
                case "bge":
                case "ble":
                case "bne.un":
                case "bge.un":
                    pushes = 0; pops = 2; break;

                // Pops 3
                case "stelem":
                case "stelem.i":
                case "stelem.ref":
                case "cpblk":
                    pushes = 0; pops = 3; break;

                // Pop 2, push 1 (net -1)
                case "cgt.un":
                case "ceq":
                case "cgt":
                case "clt":
                case "clt.un":
                case "add":
                case "sub":
                case "mul":
                case "div":
                case "div.un":
                case "rem":
                case "rem.un":
                case "shl":
                case "shr":
                case "shr.un":
                case "and":
                case "or":
                case "xor":
                case "add.ovf":
                case "sub.ovf":
                case "mul.ovf":
                case "add.ovf.un":
                case "sub.ovf.un":
                case "mul.ovf.un":
                case "ldelem":
                case "ldelem.i":
                case "ldelem.ref":
                case "ldelema":
                    pushes = 1; pops = 2; break;

                // Pop 1, push 1 (net 0 鈥?in-place transformation)
                case "ldfld":
                case "ldflda":
                case "ldind.i4":
                case "ldind.u1":
                case "ldind.i1":
                case "ldind.u2":
                case "ldind.i2":
                case "ldind.u4":
                case "ldind.i8":
                case "ldind.r4":
                case "ldind.r8":
                case "ldind.ref":
                case "ldind.i":
                case "box":
                case "unbox":
                case "unbox.any":
                case "castclass":
                case "isinst":
                case "ldobj":
                case "ldlen":
                case "localloc":
                case "conv.i4":
                case "conv.i1":
                case "conv.i2":
                case "conv.i8":
                case "conv.u8":
                case "conv.r4":
                case "conv.r8":
                case "conv.u":
                case "conv.u1":
                case "conv.u2":
                case "conv.u4":
                case "conv.r.un":
                case "ckfinite":
                case "conv.ovf.i1":
                case "conv.ovf.u1":
                case "conv.ovf.i2":
                case "conv.ovf.u2":
                case "conv.ovf.i4":
                case "conv.ovf.u4":
                case "conv.ovf.i8":
                case "conv.ovf.u8":
                case "conv.ovf.i":
                case "conv.ovf.u":
                case "conv.ovf.i8.un":
                case "conv.ovf.u8.un":
                case "conv.ovf.i.un":
                case "conv.ovf.u.un":
                case "conv.ovf.i1.un":
                case "conv.ovf.i2.un":
                case "conv.ovf.i4.un":
                case "conv.ovf.u1.un":
                case "conv.ovf.u2.un":
                case "conv.ovf.u4.un":
                case "not":
                case "neg":
                    pushes = 1; pops = 1; break;

                // Pop 1 (typed reference), push 2 (handle + pointer)
                case "mkrefany":
                    pushes = 2; pops = 1; break;
                case "refanyval":
                case "refanytype":
                    pushes = 1; pops = 2; break;  // pop 2 (typeHandle+ptr), push 1 result
                case "ldvirtftn":
                    pushes = 1; pops = 1; break;

                // Call/callvirt/calli: pop N args, push 0/1 result
                // newobj: pop constructor args, push new object/value
                case "call":
                case "callvirt":
                case "calli":
                case "newobj":
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

                // ret: method return (depth resets)
                case "ret":
                    pushes = 0; pops = 0;
                    // Stack-balance guard: if the method is non-void and the simulated
                    // stack is empty at a ret, structured emission would underflow
                    // (EmitStructuredMethodReturn tries to consume a value that was
                    // never pushed).  Return -1 to signal "will underflow", causing
                    // TryBuildStructuredMethodBody to fallback to goto emission.
                    if (depth == 0 &&
                        returnAbi is { CarrierKindCode: not AotCoreIrAbiCarrierKind.Void })
                    {
                        return -1;
                    }
                    depth = 0;
                    break;

                // jmp: forwards chaos_args directly, no stack effect
                case "jmp":
                    pushes = 0; pops = 0; break;

                // switch: pop index value
                case "switch":
                    pushes = 0; pops = 1; break;

                // br/leave: reset depth at unconditional branch target
                case "br":
                case "leave":
                    pushes = 0; pops = 0;
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
        try
        {
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
                if (result)
                {
                    LogStructuredMethod(method, "exception-body", instructions.Count, 0, 0, 0);
                    return true;
                }
                // Fall through to non-exception CFG-based emission below.
                // The generic exception shape fallback should cover all valid EH
                // patterns; if we reach here the method has no structured EH shape
                // and will be emitted as normal CFG-structured code.
            }
            var cfg = BuildControlFlowGraph(instructions, offsets);
            bool reducible = cfg.IsReducible;
            if (!reducible)
            {
                var splitCfg = MakeCfgReducibleViaIntervalAnalysis(cfg);
                if (splitCfg.IsReducible)
                {
                    cfg = splitCfg;
                    LogStructuredMethod(method, "node-split", instructions.Count,
                        cfg.Blocks.Count, cfg.LoopHeaders.Count, method.ExceptionRegionCount);
                }
                else
                {
                    // CFG still irreducible after interval analysis.
                    // Emit a pc-dispatch state machine instead of goto fallback.
                    LogStructuredMethod(method, "pc-dispatch", instructions.Count,
                        cfg.Blocks.Count, cfg.LoopHeaders.Count, method.ExceptionRegionCount);
                    body = BuildPcDispatchBody(cfg);
                    maxDepth = ComputeMaxEvalStackDepth(instructions, method.ReturnAbi);
                    if (maxDepth < 0)
                        maxDepth = ComputeMaxEvalStackDepth(instructions);
                    return true;
                }
            }

            body = RecoverStructure(cfg, 0, cfg.Blocks.Count - 1);
            if (ContainsResidualBranchTerminators(body))
            {
                // Convert remaining branches to structured control flow instead
                // of falling back to IRFlatRegion (goto elimination).
                body = ConvertResidualBranches(body, loopExitOffsets: null, loopHeaderOffset: null);
                if (ContainsResidualBranchTerminators(body))
                {
                    // Some residual branches could not be converted — log but still emit
                    // structured IR rather than goto fallback. Unconverted branches will
                    // emit as C++ goto which is correct albeit suboptimal.
                    LogStructuredMethod(method, "residual-branch-unconverted", instructions.Count,
                        cfg.Blocks.Count, cfg.LoopHeaders.Count, method.ExceptionRegionCount);
                }
                else
                {
                    LogStructuredMethod(method, "residual-branch-converted", instructions.Count,
                        cfg.Blocks.Count, cfg.LoopHeaders.Count, method.ExceptionRegionCount);
                }
            }
            else
            {
                LogStructuredMethod(method, "structured", instructions.Count,
                    cfg.Blocks.Count, cfg.LoopHeaders.Count, method.ExceptionRegionCount);
            }

            maxDepth = ComputeMaxEvalStackDepth(instructions, method.ReturnAbi);
            if (maxDepth < 0)
            {
                // Non-monotonic computation (slot reuse allowed) — may produce slightly
                // larger eval stack than strictly needed for structured emission, but
                // guarantees correctness without goto fallback.
                maxDepth = ComputeMaxEvalStackDepth(instructions);
                LogStructuredMethod(method, "stack-depth-fixup", instructions.Count,
                    cfg.Blocks.Count, cfg.LoopHeaders.Count, method.ExceptionRegionCount);
            }
            // else: non-monotonic (peak concurrent depth) matches
            // StructuredSlotEmissionContext._maxDepth which tracks the maximum
            // concurrently-live slot count via RestoreDepth at merge points.
            return true;
        }
        catch (InvalidOperationException)
        {
            body = null;
            maxDepth = 0;
            return false;
        }
    }



    private StructuredSlotEmissionContext? EmitViaStructuredIR(
        StringBuilder builder,
        AotCoreIrMethodArtifact method,
        IReadOnlyList<AotCoreIrInstructionArtifact> instructions,
        IReadOnlySet<int> offsets,
        StructuredIRNode? body = null)
    {
        if (instructions.Count == 0)
            return null;

        body ??= (TryBuildStructuredMethodBody(method, instructions, offsets, out var b, out _) ? b : null);
        if (body is null)
            return null;

        TotalMethodCount++;

        if (method.ExceptionRegionCount > 0)
            StructuredExceptionBodyCount++;
        else
            StructuredMethodCount++;

        StructuredSlotEmissionContext? previousSlotContext = _state.Value!.ActiveStructuredSlotContext;
        var slotContext = new StructuredSlotEmissionContext();
        _state.Value!.ActiveStructuredSlotContext = slotContext;
        _state.Value!.StructuredSlotTypes.Clear();
        _state.Value!.StructLocalSlots = IdentifyStructLocalSlots(instructions);
        _state.Value!.FloatLocalSlots = IdentifyFloatLocalSlots(instructions);
        _state.Value!.Int64LocalSlots = IdentifyInt64LocalSlots(instructions);
        slotContext.FloatLocalSlots = _state.Value!.FloatLocalSlots;
        try
        {
            EmitStructuredIRNode(builder, body!, method, "    ");
        }
        finally
        {
            _state.Value!.ActiveStructuredSlotContext = previousSlotContext;
            _state.Value!.StructuredSlotTypes.Clear();
            _state.Value!.StructLocalSlots = null;
            _state.Value!.FloatLocalSlots = null;
            _state.Value!.Int64LocalSlots = null;
        }
        return slotContext;
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
        int handlerDepth = 0;

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
            // Catch handler starts with exception value on stack (not in IL)
            handlerDepth = ComputeMaxEvalStackDepth(catchOnly.HandlerInstructions) + 1;
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
            // Filter handler: exception push + filter body re-push = 2 extra
            handlerDepth = ComputeMaxEvalStackDepth(filterOnly.HandlerInstructions) + 2;
        }
        else if (TryCreateFinallyOnlyExceptionMethodShape(method, out var finallyOnly) && finallyOnly is not null)
        {
            body = BuildFinallyOnlyExceptionIRBody(finallyOnly, offsets);
            // finally-only has no caught-exception push
        }
        else if (TryCreateCatchAndFinallyExceptionMethodShape(method, out var catchAndFinally) && catchAndFinally is not null)
        {
            body = BuildCatchAndFinallyExceptionIRBody(catchAndFinally, offsets);
            handlerDepth = ComputeMaxEvalStackDepth(catchAndFinally.HandlerInstructions) + 1;
        }
        else if (TryCreateFilterAndFinallyExceptionMethodShape(method, out var filterAndFinally) && filterAndFinally is not null)
        {
            body = BuildFilterAndFinallyExceptionIRBody(filterAndFinally, offsets);
            handlerDepth = ComputeMaxEvalStackDepth(filterAndFinally.HandlerInstructions) + 2;
        }
        else if (TryCreateMultipleCatchExceptionMethodShape(method, out var multiCatch) && multiCatch is not null)
        {
            body = BuildMultipleCatchExceptionIRBody(multiCatch, offsets);
            int maxHandler = 0;
            for (int i = 0; i < multiCatch.HandlerInstructionsList.Count; i++)
            {
                int hd = ComputeMaxEvalStackDepth(multiCatch.HandlerInstructionsList[i]) + 1;
                if (hd > maxHandler) maxHandler = hd;
            }
            handlerDepth = maxHandler;
        }
        else if (TryCreateGenericExceptionMethodShape(method, out var genericShape) && genericShape is not null)
        {
            body = BuildGenericShapeExceptionIRBody(genericShape, offsets);
            int maxHandler = 0;
            for (int i = 0; i < genericShape.Regions.Count; i++)
            {
                int hd = ComputeMaxEvalStackDepth(genericShape.Regions[i].HandlerInstructions)
                    + genericShape.HandlerPushes;
                if (hd > maxHandler) maxHandler = hd;
            }
            handlerDepth = maxHandler;
        }

        if (body is null)
        {
            return false;
        }

        int peakDepth = ComputeMaxEvalStackDepth(instructions);
        maxDepth = Math.Max(peakDepth, handlerDepth);
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
    /// Build a structured IR tree for a method whose EH shape does not match any
    /// of the 6 standard patterns.  Regions are sorted innermost-first during
    /// shape detection, so each region's TryInstructions already exclude
    /// instructions consumed by inner regions' try+handler ranges.
    /// Nested regions produce nested IRExceptionRegion nodes wrapping from
    /// innermost to outermost; sibling regions (no nesting) produce sequential
    /// IRExceptionRegion nodes sharing a common try body.
    /// </summary>
    private StructuredIRNode BuildGenericShapeExceptionIRBody(
        GenericExceptionMethodShape shape,
        IReadOnlySet<int> offsets)
    {
        // Build from innermost outward: each iteration wraps the previous inner
        // tree with the next region's try body and handler.
        StructuredIRNode inner = new IRSequence(Array.Empty<StructuredIRNode>());

        for (int i = 0; i < shape.Regions.Count; i++)
        {
            var entry = shape.Regions[i];

            // Build try body for this region's remaining (unconsumed) instructions
            var tryTree = entry.TryInstructions.Count > 0
                ? BuildExceptionPartitionTree(entry.TryInstructions, offsets)
                : new IRSequence(Array.Empty<StructuredIRNode>());

            // Combine existing inner tree with this region's try body.
            // When regions are nested (inner first), inner contains the already-wrapped
            // inner region.  When regions are siblings (same try range), inner is empty
            // for the second sibling and tryTree holds the full try body.
            var combinedTry = new IRSequence(new[] { inner, tryTree });

            // Build handler tree
            var handlerTree = entry.HandlerInstructions.Count > 0
                ? BuildExceptionPartitionTree(entry.HandlerInstructions, offsets)
                : new IRSequence(Array.Empty<StructuredIRNode>());

            // Map region kind to IRExceptionKind
            IRExceptionKind kind = entry.Region.HandlingKindCode switch
            {
                AotCoreIrExceptionRegionKind.Catch => IRExceptionKind.TryCatch,
                AotCoreIrExceptionRegionKind.Filter => IRExceptionKind.TryFilter,
                AotCoreIrExceptionRegionKind.Finally => IRExceptionKind.TryFinally,
                _ => IRExceptionKind.TryCatch
            };

            inner = new IRExceptionRegion(
                kind, combinedTry, handlerTree,
                CatchTypeSubjectId: entry.Region.CatchTypeSubjectId,
                FilterInstructions: entry.FilterInstructions);
        }

        // Combine prefix + wrapped tree + tail
        var nodes = new List<StructuredIRNode>();
        if (shape.PrefixInstructions.Count > 0)
            nodes.Add(BuildExceptionPartitionTree(shape.PrefixInstructions, offsets));
        nodes.Add(inner);
        if (shape.TailInstructions.Count > 0)
            nodes.Add(BuildExceptionPartitionTree(shape.TailInstructions, offsets));

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
            // Try interval analysis to make the partition CFG reducible,
            // reducing the need for flat emission fallback.
            var splitCfg = MakeCfgReducibleViaIntervalAnalysis(cfg);
            if (splitCfg.IsReducible)
            {
                return StripExceptionPartitionExitTerminators(
                    RecoverStructure(splitCfg, 0, splitCfg.Blocks.Count - 1));
            }

            // The CFG can become irreducible when a branch target falls
            // between partition boundaries (the target instruction is in
            // a different partition).  Emit the instructions as a flat
            // sequence instead of dropping them silently.
            // TRACE:EMIT disabled — was flooding stderr
            if (instructions.Count >= 3)
                // TRACE:EMIT disabled
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
    private void EmitPcDispatch(
        StringBuilder builder,
        IRPcDispatch pcDispatch,
        AotCoreIrMethodArtifact method,
        string indentation)
    {
        Interlocked.Increment(ref s_pcDispatchCount);

        builder.AppendLine(indentation + "// pc-dispatch state machine for irreducible CFG");
        builder.AppendLine(indentation + "CHAOS_IL2CPP_INT32 chaos_pc = " + pcDispatch.PcVariableInit.ToString() + ";");
        builder.AppendLine(indentation + "while (chaos_pc >= 0)");
        builder.AppendLine(indentation + "{");
        builder.AppendLine(indentation + "    switch (chaos_pc)");
        builder.AppendLine(indentation + "    {");

        foreach (var pcCase in pcDispatch.Cases)
        {
            builder.AppendLine(indentation + "    case " + pcCase.PcValue.ToString() + ":");
            builder.AppendLine(indentation + "    {");

            EmitInstructionLookahead(builder, pcCase.Instructions, indentation + "        ");

            if (pcCase.Terminator != null)
            {
                switch (pcCase.Terminator.Op)
                {
                    case "ret":
                        EmitStructuredMethodReturn(builder, method.ReturnAbi, indentation + "        ");
                        builder.AppendLine(indentation + "        chaos_pc = -1;");
                        break;
                    case "throw":
                    case "rethrow":
                        builder.AppendLine(indentation + "        throw;");
                        builder.AppendLine(indentation + "        chaos_pc = -1;");
                        break;
                    case "br":
                        builder.AppendLine(indentation + "        chaos_pc = " + pcCase.NextPcValue.ToString() + ";");
                        break;
                    default:
                        if (pcCase.FallthroughPcValue >= 0 && IsConditionalBranchOpcode(pcCase.Terminator.Op))
                        {
                            string condition = EmitBranchConditionCpp(pcCase.Terminator);
                            builder.AppendLine(indentation + "        if (" + condition + ") { chaos_pc = " + pcCase.NextPcValue.ToString() + "; } else { chaos_pc = " + pcCase.FallthroughPcValue.ToString() + "; }");
                        }
                        else if (pcCase.NextPcValue >= 0)
                        {
                            builder.AppendLine(indentation + "        chaos_pc = " + pcCase.NextPcValue.ToString() + ";");
                        }
                        else
                        {
                            builder.AppendLine(indentation + "        chaos_pc = -1;");
                        }
                        break;
                }
            }
            else if (pcCase.NextPcValue >= 0)
            {
                builder.AppendLine(indentation + "        chaos_pc = " + pcCase.NextPcValue.ToString() + ";");
            }
            else
            {
                builder.AppendLine(indentation + "        chaos_pc = -1;");
            }

            builder.AppendLine(indentation + "        break;");
            builder.AppendLine(indentation + "    }");
        }

        builder.AppendLine(indentation + "    default:");
        builder.AppendLine(indentation + "        CHAOS_IL2CPP_FAIL(\"invalid pc-dispatch value\");");
        builder.AppendLine(indentation + "        chaos_pc = -1;");
        builder.AppendLine(indentation + "        break;");
        builder.AppendLine(indentation + "    }");
        builder.AppendLine(indentation + "}");
    }



    /// <summary>
    /// Builds a pc-dispatch state machine from an irreducible CFG.
    /// Each basic block is assigned a unique pc value (0..N-1), and the
    /// terminator of each block determines the next pc value.
    /// </summary>
    private static IRPcDispatch BuildPcDispatchBody(ControlFlowGraph cfg)
    {
        var cases = new List<PcDispatchCase>(cfg.Blocks.Count);
        int entryPc = 0;

        var blockIndexToPc = new Dictionary<int, int>(cfg.Blocks.Count);
        for (int i = 0; i < cfg.Blocks.Count; i++)
        {
            blockIndexToPc[i] = i;
        }

        for (int i = 0; i < cfg.Blocks.Count; i++)
        {
            var block = cfg.Blocks[i];
            int pcValue = blockIndexToPc[i];

            if (block.Kind == BasicBlockKind.Entry)
                entryPc = pcValue;

            int nextPcValue = -1;
            int fallthroughPcValue = -1;

            if (block.Terminator != null)
            {
                string op = block.Terminator.Op;
                if (op == "ret" || op == "throw" || op == "rethrow")
                {
                    nextPcValue = -1;
                }
                else if (op == "br" && block.BranchTarget.HasValue)
                {
                    var targetBlockIdx = cfg.OffsetToBlockIndex.GetValueOrDefault(block.BranchTarget.Value, -1);
                    nextPcValue = targetBlockIdx >= 0 ? blockIndexToPc[targetBlockIdx] : -1;
                }
                else if (IsConditionalBranchOpcode(op))
                {
                    if (block.ConditionalTarget.HasValue)
                    {
                        var targetBlockIdx = cfg.OffsetToBlockIndex.GetValueOrDefault(block.ConditionalTarget.Value, -1);
                        nextPcValue = targetBlockIdx >= 0 ? blockIndexToPc[targetBlockIdx] : -1;
                    }
                    else
                    {
                        nextPcValue = -1;
                    }
                    // Fall-through = next sequential block in the CFG
                    if (i + 1 < cfg.Blocks.Count)
                        fallthroughPcValue = blockIndexToPc[i + 1];
                }
                else
                {
                    nextPcValue = -1;
                }
            }
            else
            {
                // No terminator: fall through to next block
                if (i + 1 < cfg.Blocks.Count)
                    nextPcValue = blockIndexToPc[i + 1];
            }

            cases.Add(new PcDispatchCase(
                PcValue: pcValue,
                Instructions: block.BodyInstructions,
                Terminator: block.Terminator,
                NextPcValue: nextPcValue,
                FallthroughPcValue: fallthroughPcValue));
        }

        return new IRPcDispatch(cases, entryPc);
    }



    /// <summary>
    /// Emits the C++ condition expression for a conditional branch terminator
    /// in the pc-dispatch state machine. Operands are peeked from the structured
    /// slot context and popped to keep stack tracking consistent.
    /// </summary>
    private string EmitBranchConditionCpp(AotCoreIrInstructionArtifact terminator)
    {
        string op = terminator.Op;
        var ctx = _state.Value!.ActiveStructuredSlotContext;
        if (ctx == null)
            return "true";

        switch (op)
        {
            case "brtrue":
                {
                    string val = ctx.PeekValue();
                    ctx.PopValue();
                    return val;
                }
            case "brfalse":
                {
                    string val = ctx.PeekValue();
                    ctx.PopValue();
                    return "!" + val;
                }
            case "brnull":
                {
                    string val = ctx.PeekValue();
                    ctx.PopValue();
                    return val + " == 0";
                }
            case "brnonnull":
                {
                    string val = ctx.PeekValue();
                    ctx.PopValue();
                    return val + " != 0";
                }
            case "beq":
                {
                    string right = ctx.PeekValue(); ctx.PopValue();
                    string left = ctx.PeekValue(); ctx.PopValue();
                    return left + " == " + right;
                }
            case "bne.un":
                {
                    string right = ctx.PeekValue(); ctx.PopValue();
                    string left = ctx.PeekValue(); ctx.PopValue();
                    return left + " != " + right;
                }
            case "blt":
                {
                    string right = ctx.PeekValue(); ctx.PopValue();
                    string left = ctx.PeekValue(); ctx.PopValue();
                    return "(CHAOS_IL2CPP_INT32)" + left + " < (CHAOS_IL2CPP_INT32)" + right;
                }
            case "blt.un":
                {
                    string right = ctx.PeekValue(); ctx.PopValue();
                    string left = ctx.PeekValue(); ctx.PopValue();
                    return "(CHAOS_IL2CPP_UINT32)" + left + " < (CHAOS_IL2CPP_UINT32)" + right;
                }
            case "bgt":
                {
                    string right = ctx.PeekValue(); ctx.PopValue();
                    string left = ctx.PeekValue(); ctx.PopValue();
                    return "(CHAOS_IL2CPP_INT32)" + left + " > (CHAOS_IL2CPP_INT32)" + right;
                }
            case "bgt.un":
                {
                    string right = ctx.PeekValue(); ctx.PopValue();
                    string left = ctx.PeekValue(); ctx.PopValue();
                    return "(CHAOS_IL2CPP_UINT32)" + left + " > (CHAOS_IL2CPP_UINT32)" + right;
                }
            case "ble":
                {
                    string right = ctx.PeekValue(); ctx.PopValue();
                    string left = ctx.PeekValue(); ctx.PopValue();
                    return "(CHAOS_IL2CPP_INT32)" + left + " <= (CHAOS_IL2CPP_INT32)" + right;
                }
            case "ble.un":
                {
                    string right = ctx.PeekValue(); ctx.PopValue();
                    string left = ctx.PeekValue(); ctx.PopValue();
                    return "(CHAOS_IL2CPP_UINT32)" + left + " <= (CHAOS_IL2CPP_UINT32)" + right;
                }
            case "bge":
                {
                    string right = ctx.PeekValue(); ctx.PopValue();
                    string left = ctx.PeekValue(); ctx.PopValue();
                    return "(CHAOS_IL2CPP_INT32)" + left + " >= (CHAOS_IL2CPP_INT32)" + right;
                }
            case "bge.un":
                {
                    string right = ctx.PeekValue(); ctx.PopValue();
                    string left = ctx.PeekValue(); ctx.PopValue();
                    return "(CHAOS_IL2CPP_UINT32)" + left + " >= (CHAOS_IL2CPP_UINT32)" + right;
                }
            default:
                // Unknown opcode: fall through to taken target (conservative)
                if (ctx.Depth > 0)
                {
                    string val = ctx.PeekValue();
                    ctx.PopValue();
                    return val;
                }
                return "true";
        }
    }




    private static void LogStructuredMethod(
        AotCoreIrMethodArtifact method, string kind,
        int instrCount, int blocks, int loops, int exceptionRegions)
    {
        Interlocked.Increment(ref s_totalMethodCount);
        if (kind == "exception-body")
            Interlocked.Increment(ref s_exceptionBodyCount);
        else
            Interlocked.Increment(ref s_structuredMethodCount);

        // TRACE:EMIT disabled — was flooding stderr
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

}
