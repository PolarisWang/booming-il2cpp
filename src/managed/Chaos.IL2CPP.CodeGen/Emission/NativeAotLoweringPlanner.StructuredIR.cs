using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeAotLoweringPlanner
{
    // ──────────────────────────────────────────────────────────────
    // Pure Structured IR — type definitions
    //
    // These types represent the output of control flow recovery,
    // completely decoupled from CFG analysis (BasicBlock,
    // ControlFlowGraph, etc.).  The IR tree is a pure control-flow
    // representation that can be directly lowered to C++ without
    // any `goto chaos_ip_*` merge labels or `_suppressGotoNext`.
    // ──────────────────────────────────────────────────────────────

    internal abstract record StructuredIRNode;

    /// <summary>Linear sequence of instructions with optional terminator.</summary>
    internal sealed record IRBlock(
        IReadOnlyList<AotCoreIrInstructionArtifact> BodyInstructions,
        AotCoreIrInstructionArtifact? Terminator
    ) : StructuredIRNode;

    /// <summary>Sequential composition of IR nodes.</summary>
    internal sealed record IRSequence(IReadOnlyList<StructuredIRNode> Nodes) : StructuredIRNode;

    /// <summary>If-then-else — condition expressed via eval-stack + branch opcode.</summary>
    internal sealed record IRIfThenElse(
        IReadOnlyList<AotCoreIrInstructionArtifact> ConditionInstructions,
        AotCoreIrInstructionArtifact BranchTerminator,
        StructuredIRNode ThenBody,
        StructuredIRNode? ElseBody
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

    /// <summary>Switch dispatch.</summary>
    internal sealed record IRSwitch(
        IReadOnlyList<AotCoreIrInstructionArtifact> SwitchInstructions,
        IReadOnlyDictionary<int, StructuredIRNode> CaseBodies,
        StructuredIRNode? DefaultBody,
        int ExitOffset
    ) : StructuredIRNode;

    // ── Leaf control-flow nodes ──

    internal sealed record IRBreak : StructuredIRNode;
    internal sealed record IRContinue : StructuredIRNode;
    internal sealed record IRReturn : StructuredIRNode;
    internal sealed record IRThrow : StructuredIRNode;
    internal sealed record IRGoto(int TargetOffset) : StructuredIRNode;

    // ── Exception regions (first-class IR nodes) ──

    internal enum IRExceptionKind { TryCatch, TryFinally, TryFilter }

    internal sealed record IRExceptionRegion(
        IRExceptionKind Kind,
        StructuredIRNode TryBody,
        StructuredIRNode HandlerBody,
        string? CatchTypeSubjectId = null,
        IReadOnlyList<AotCoreIrInstructionArtifact>? FilterInstructions = null
    ) : StructuredIRNode;

    // ──────────────────────────────────────────────────────────────
    // Stack Slot Context — maps IL eval stack positions to C++ local
    // variable names (_s0, _s1, …) so that push/pop operations become
    // local-variable assignments instead of array operations.
    //
    // Each push to a given stack depth gets a unique slot id, so
    // the mapping is: depth → slotId → "_s{slotId}".  Pop returns
    // the name at the current top and discards it.
    // ──────────────────────────────────────────────────────────────

    private sealed class SlotContext
    {
        private int _counter;
        private readonly List<int> _slots = new(); // depth→slotId mapping

        public int Depth => _slots.Count;

        /// <summary>Declare a new slot at the current top.</summary>
        public string Push()
        {
            int id = _counter++;
            _slots.Add(id);
            return $"_s{id}";
        }

        /// <summary>Pop and return the top slot name.</summary>
        public string Pop()
        {
            int id = _slots[^1];
            _slots.RemoveAt(_slots.Count - 1);
            return $"_s{id}";
        }

        /// <summary>Peek at the top slot name without popping.</summary>
        public string Peek() => $"_s{_slots[^1]}";
    }

    // ──────────────────────────────────────────────────────────────
    // Stack operation pattern post-processor
    //
    // Takes the output of EmitLinearInstruction / helpers (which
    // contains chaos_eval_stack[chaos_stack_top++], etc.) and replaces
    // the array operations with _sN slot names, tracking depth through
    // a SlotContext.
    //
    // This avoids rewriting every opcode handler — the patterns are
    // predictable and the substitution is purely mechanical.
    // ──────────────────────────────────────────────────────────────

    private static string ReplaceStackOpsWithSlots(string code, SlotContext slots)
    {
        var lines = code.Split('\n');
        for (int i = 0; i < lines.Length; i++)
        {
            string line = lines[i];

            // ── Push: chaos_eval_stack[chaos_stack_top++] = VALUE;
            int pushIdx = line.IndexOf("chaos_eval_stack[chaos_stack_top++]", StringComparison.Ordinal);
            if (pushIdx >= 0 && line.Contains('='))
            {
                int eqIdx = line.IndexOf('=', pushIdx);
                string prefix = line[..pushIdx].TrimEnd();
                string valueExpr = line[(eqIdx + 1)..].TrimEnd().TrimEnd(';');
                string slotName = slots.Push();
                lines[i] = $"{prefix}auto {slotName} = {valueExpr};";
                continue;
            }

            // ── Pop:  ... = chaos_eval_stack[--chaos_stack_top];
            int popIdx = line.IndexOf("chaos_eval_stack[--chaos_stack_top]", StringComparison.Ordinal);
            if (popIdx >= 0 && line.Contains('='))
            {
                int eqIdx = line.IndexOf('=', popIdx);
                string target = line[..eqIdx].TrimEnd();
                // The pop expression might be wrapped: static_cast<T>(...[...]) etc.
                // We need to extract just the [...[...]] part and replace it with _sN
                string beforePop = line[..popIdx].TrimEnd();
                string afterPop = line[(popIdx + "chaos_eval_stack[--chaos_stack_top]".Length)..];
                string slotName = slots.Pop();
                // Reconstruct: keep the prefix operations intact
                // e.g.: "const auto x = static_cast<T>(chaos_eval_stack[--chaos_stack_top]);"
                // becomes: "const auto x = static_cast<T>(_sN);"
                lines[i] = $"{beforePop}{slotName}{afterPop}";
                continue;
            }

            // ── Top modify: chaos_eval_stack[chaos_stack_top - 1] = VALUE;
            int topModIdx = line.IndexOf("chaos_eval_stack[chaos_stack_top - 1]", StringComparison.Ordinal);
            if (topModIdx >= 0)
            {
                int eqIdx = line.IndexOf('=', topModIdx);
                string prefix = line[..topModIdx].TrimEnd();
                string valueExpr = line[(eqIdx + 1)..].TrimEnd().TrimEnd(';');
                string slotName = slots.Peek();
                lines[i] = $"{prefix}{slotName} = {valueExpr};";
                continue;
            }

            // ── Top decrement: chaos_stack_top--;
            if (line.Contains("chaos_stack_top--;"))
            {
                string prefix = line[..line.IndexOf("chaos_stack_top--;")].TrimEnd();
                slots.Pop();
                lines[i] = string.IsNullOrEmpty(prefix) ? $"// (pop)" : $"{prefix}// (pop)";
                continue;
            }

            // ── Top decrement by N: chaos_stack_top -= N;
            int minusEqIdx = line.IndexOf("chaos_stack_top -=", StringComparison.Ordinal);
            if (minusEqIdx >= 0)
            {
                int semiIdx = line.IndexOf(';', minusEqIdx);
                string nStr = line[(minusEqIdx + "chaos_stack_top -=".Length)..semiIdx].Trim();
                if (int.TryParse(nStr, out int n))
                {
                    string prefix = line[..minusEqIdx].TrimEnd();
                    for (int j = 0; j < n; j++) slots.Pop();
                    lines[i] = string.IsNullOrEmpty(prefix) ? $"// (pop {n})" : $"{prefix}// (pop {n})";
                    continue;
                }
            }

            // ── Dup: chaos_eval_stack[chaos_stack_top] = chaos_eval_stack[chaos_stack_top - 1]; + stack_top++;
            if (line.Contains("chaos_eval_stack[chaos_stack_top] = chaos_eval_stack[chaos_stack_top - 1]"))
            {
                string prefix = line[..line.IndexOf("chaos_eval_stack[chaos_stack_top]", StringComparison.Ordinal)].TrimEnd();
                string slotName = slots.Push();
                string sourceSlot = slots.Peek(); // the push incremented depth; Peek is the copy
                // Actually for dup: the top slot before dup is at depth-1, after dup depth has +1
                // But the Push() already incremented depth. We need the PREVIOUS top.
                // Fix: Pop the slot we just pushed, read the (now restored) previous top
                string newSlot = slots.Pop(); // undo the Push
                string prevTop = slots.Peek(); // this is the original top
                slots.Push(); // redo the Push — the new slot name is the same
                lines[i] = $"{prefix}auto {newSlot} = {prevTop};";
                continue;
            }
        }

        return string.Join("\n", lines);
    }

    // ──────────────────────────────────────────────────────────────
    // Pure Structured IR → C++ Recursive Emitter
    //
    // The emitter is a recursive tree walk that produces C++ control
    // flow directly — no `goto chaos_ip_*`, no `_suppressGotoNext`,
    // no merge labels.  Leaf instructions use EmitLinearInstruction
    // (no goto-next appended).
    // ──────────────────────────────────────────────────────────────

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
                EmitMethodReturn(builder, method.ReturnAbi);
                break;

            case IRThrow:
                builder.AppendLine(indentation + "throw;");
                break;

            case IRGoto g:
                builder.AppendLine(indentation + "goto chaos_ip_" + g.TargetOffset + ";");
                break;

            case IRExceptionRegion er:
                EmitIRExceptionRegion(builder, er, method, indentation);
                break;

            default:
                throw new NotSupportedException(
                    "StructuredIR: unknown node type '" + node.GetType().Name + "'");
        }
    }

    // ── Block ─────────────────────────────────────────────────────

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
        var slots = new SlotContext();

        foreach (var instr in block.BodyInstructions)
        {
            var temp = new StringBuilder();
            EmitLinearInstruction(temp, instr, indentation);
            string processed = ReplaceStackOpsWithSlots(temp.ToString(), slots);
            builder.Append(processed);
            if (!processed.EndsWith("\n", StringComparison.Ordinal))
                builder.AppendLine();
        }

        if (block.Terminator != null)
        {
            // Terminators that pop from the stack can also use slot mapping
            var temp = new StringBuilder();
            EmitIRBlockTerminator(temp, block.Terminator, method, indentation);
            string processed = ReplaceStackOpsWithSlots(temp.ToString(), slots);
            builder.Append(processed);
            if (!processed.EndsWith("\n", StringComparison.Ordinal))
                builder.AppendLine();
        }

        // If stack is not empty after the block (shouldn't happen in structured code),
        // emit a comment showing residual slots (debug aid).
        if (slots.Depth > 0)
        {
            builder.AppendLine($"{indentation}// (stack depth {slots.Depth} after block)");
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
                EmitMethodReturn(builder, method.ReturnAbi);
                break;

            case "throw":
            case "rethrow":
                builder.AppendLine(indentation + "throw;");
                break;

            case "endfinally":
                // RAII scope guard handles the finally body — no explicit code needed.
                break;

            case "endfilter":
                // endfilter: pop eval stack, re-throw if 0, else pass through to handler
                builder.AppendLine(indentation +
                    "if (chaos_eval_stack[--chaos_stack_top] == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
                builder.AppendLine(indentation + "{");
                builder.AppendLine(indentation + "    throw;");
                builder.AppendLine(indentation + "}");
                break;

            case "br":
            case "leave":
                // In a pure StructuredIR tree, unconditional branches should have been
                // converted to IRBreak, IRContinue, or eliminated.  A residual IRBlock
                // with br/leave means the IR builder could not structure this branch.
                // Emit a goto as a correctness fallback.
                EmitIRGoto(builder, terminator, indentation);
                break;

            default:
                throw new NotSupportedException(
                    "StructuredIR: unsupported block terminator '" + terminator.Op + "'");
        }
    }

    /// <summary>
    /// Emit an unconditional branch as a goto (correctness fallback
    /// for branches that could not be structured).
    /// </summary>
    private static void EmitIRGoto(
        StringBuilder builder,
        AotCoreIrInstructionArtifact terminator,
        string indentation)
    {
        int target = GetRequiredIntOperand(terminator);
        builder.Append(indentation);
        builder.Append("goto chaos_ip_");
        builder.Append(target);
        builder.AppendLine(";");
    }

    // ── If-then-else ──────────────────────────────────────────────

    private void EmitIRIfThenElse(
        StringBuilder builder,
        IRIfThenElse ite,
        AotCoreIrMethodArtifact method,
        string indentation)
    {
        var terminator = ite.BranchTerminator;

        // Emit condition instructions (push operands onto eval stack)
        foreach (var instr in ite.ConditionInstructions)
            EmitLinearInstruction(builder, instr, indentation);

        string inner = indentation + "    ";
        string bodyIndent = inner + "    ";

        if (terminator.Op == "brtrue" || terminator.Op == "brfalse")
        {
            bool branchOnNonZero = terminator.Op == "brtrue";
            string condition = branchOnNonZero
                ? "chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0)"
                : "chaos_condition == static_cast<CHAOS_IL2CPP_INTPTR>(0)";

            builder.AppendLine(indentation + "{");
            builder.AppendLine(inner + "const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];");
            builder.AppendLine(inner + "if (" + condition + ")");
            builder.AppendLine(inner + "{");
            EmitStructuredIRNode(builder, ite.ThenBody, method, bodyIndent);
            builder.AppendLine(inner + "}");

            if (ite.ElseBody != null)
            {
                builder.AppendLine(inner + "else");
                builder.AppendLine(inner + "{");
                EmitStructuredIRNode(builder, ite.ElseBody, method, bodyIndent);
                builder.AppendLine(inner + "}");
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

            if (isUnsigned)
            {
                builder.AppendLine(inner +
                    "const auto chaos_right = static_cast<CHAOS_IL2CPP_UINT32>(static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]));");
                builder.AppendLine(inner +
                    "const auto chaos_left = static_cast<CHAOS_IL2CPP_UINT32>(static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]));");
            }
            else
            {
                builder.AppendLine(inner +
                    "const auto chaos_right = static_cast<" + valueType + ">(chaos_eval_stack[--chaos_stack_top]);");
                builder.AppendLine(inner +
                    "const auto chaos_left = static_cast<" + valueType + ">(chaos_eval_stack[--chaos_stack_top]);");
            }

            builder.AppendLine(inner + "if (chaos_left " + cmpOp + " chaos_right)");
            builder.AppendLine(inner + "{");
            EmitStructuredIRNode(builder, ite.ThenBody, method, bodyIndent);
            builder.AppendLine(inner + "}");

            if (ite.ElseBody != null)
            {
                builder.AppendLine(inner + "else");
                builder.AppendLine(inner + "{");
                EmitStructuredIRNode(builder, ite.ElseBody, method, bodyIndent);
                builder.AppendLine(inner + "}");
            }

            builder.AppendLine(indentation + "}");
        }
    }

    // ── While loop ───────────────────────────────────────────────

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
            // No condition — infinite loop (while (true) { ... })
            // Condition instructions might still contain setup code.
            foreach (var instr in w.ConditionInstructions)
                EmitLinearInstruction(builder, instr, indentation);

            builder.AppendLine(indentation + "while (true)");
            builder.AppendLine(indentation + "{");
            EmitStructuredIRNode(builder, w.Body, method, bodyIndent);
            builder.AppendLine(indentation + "}");
            return;
        }

        var terminator = w.ConditionTerminator;

        foreach (var instr in w.ConditionInstructions)
            EmitLinearInstruction(builder, instr, indentation);

        if (terminator.Op == "brtrue" || terminator.Op == "brfalse")
        {
            bool branchOnNonZero = terminator.Op == "brtrue";
            string condition = branchOnNonZero
                ? "chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0)"
                : "chaos_condition == static_cast<CHAOS_IL2CPP_INTPTR>(0)";

            builder.AppendLine(indentation + "{");
            builder.AppendLine(inner + "const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];");
            builder.AppendLine(inner + "while (" + condition + ")");
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

            if (isUnsigned)
            {
                builder.AppendLine(inner +
                    "const auto chaos_right = static_cast<CHAOS_IL2CPP_UINT32>(static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]));");
                builder.AppendLine(inner +
                    "const auto chaos_left = static_cast<CHAOS_IL2CPP_UINT32>(static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]));");
            }
            else
            {
                builder.AppendLine(inner +
                    "const auto chaos_right = static_cast<" + valueType + ">(chaos_eval_stack[--chaos_stack_top]);");
                builder.AppendLine(inner +
                    "const auto chaos_left = static_cast<" + valueType + ">(chaos_eval_stack[--chaos_stack_top]);");
            }

            builder.AppendLine(inner + "while (chaos_left " + cmpOp + " chaos_right)");
            builder.AppendLine(inner + "{");
            EmitStructuredIRNode(builder, w.Body, method, bodyIndent);
            builder.AppendLine(inner + "}");
            builder.AppendLine(indentation + "}");
        }
    }

    // ── Do-while loop ─────────────────────────────────────────────

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
            foreach (var instr in dw.LatchInstructions)
                EmitLinearInstruction(builder, instr, bodyIndent);

            var terminator = dw.LatchTerminator;
            if (terminator.Op == "brtrue" || terminator.Op == "brfalse")
            {
                bool branchOnNonZero = terminator.Op == "brtrue";
                // For do-while, the latch branch is the loop-back branch.
                // When it's false, we break out (invert the condition).
                string condition = branchOnNonZero
                    ? "chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0)"
                    : "chaos_condition == static_cast<CHAOS_IL2CPP_INTPTR>(0)";

                builder.AppendLine(bodyIndent + "const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];");
                builder.AppendLine(bodyIndent + "if (!(" + condition + ")) break;");
            }
            else if (terminator.Op == "br")
            {
                // Infinite loop latch — no condition to check
            }
            else
            {
                throw new NotSupportedException(
                    "StructuredIR: unsupported do-while latch '" + terminator.Op + "'");
            }
        }

        builder.AppendLine(indentation + "} while (true);");
    }

    // ── Switch ────────────────────────────────────────────────────

    private void EmitIRSwitch(
        StringBuilder builder,
        IRSwitch sw,
        AotCoreIrMethodArtifact method,
        string indentation)
    {
        string inner = indentation + "    ";
        string caseIndent = inner + "    ";
        string bodyIndent = caseIndent + "    ";

        foreach (var instr in sw.SwitchInstructions)
            EmitLinearInstruction(builder, instr, indentation);

        builder.AppendLine(indentation + "{");
        builder.AppendLine(inner + "const auto chaos_switch_value = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);");
        builder.AppendLine(inner + "switch (chaos_switch_value)");
        builder.AppendLine(inner + "{");

        var sortedCaseValues = sw.CaseBodies.Keys.OrderBy(k => k).ToList();
        foreach (var caseValue in sortedCaseValues)
        {
            var body = sw.CaseBodies[caseValue];
            builder.AppendLine(caseIndent + "case " + caseValue + ":");
            builder.AppendLine(caseIndent + "{");
            EmitStructuredIRNode(builder, body, method, bodyIndent);
            // Fall through implicitly, or break at end of case body
            builder.AppendLine(caseIndent + "}");
        }

        if (sw.DefaultBody != null)
        {
            builder.AppendLine(caseIndent + "default:");
            builder.AppendLine(caseIndent + "{");
            EmitStructuredIRNode(builder, sw.DefaultBody, method, bodyIndent);
            builder.AppendLine(caseIndent + "}");
        }

        builder.AppendLine(inner + "}");
        builder.AppendLine(indentation + "}");
    }

    // ── Exception regions ─────────────────────────────────────────

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
                builder.AppendLine(indentation + "try");
                builder.AppendLine(indentation + "{");
                EmitStructuredIRNode(builder, er.TryBody, method, bodyIndent);
                builder.AppendLine(indentation + "}");
                builder.AppendLine(indentation + "catch (const chaos_managed_exception& chaos_exception)");
                builder.AppendLine(indentation + "{");
                if (er.CatchTypeSubjectId != null)
                {
                    builder.AppendLine(inner +
                        "auto* chaos_header = reinterpret_cast<chaos_object_header*>(chaos_exception.object_value);");
                    builder.AppendLine(inner + "if (chaos_header == nullptr)");
                    builder.AppendLine(inner + "{");
                    builder.AppendLine(inner + "    throw;");
                    builder.AppendLine(inner + "}");
                    builder.AppendLine(inner +
                        "if (!chaos_is_type_compatible(chaos_header->type_info, &" +
                        GetNativeTypeInfoSymbol(er.CatchTypeSubjectId) + "))");
                    builder.AppendLine(inner + "{");
                    builder.AppendLine(inner + "    throw;");
                    builder.AppendLine(inner + "}");
                    builder.AppendLine(inner + "chaos_eval_stack[chaos_stack_top++] = chaos_exception.object_value;");
                }
                EmitStructuredIRNode(builder, er.HandlerBody, method, bodyIndent);
                builder.AppendLine(indentation + "}");
                break;
            }

            case IRExceptionKind.TryFinally:
            {
                // RAII scope guard approach: wrap try body in a block with a finally guard
                builder.AppendLine(indentation + "{");
                builder.AppendLine(inner + "auto chaos_finally_guard = chaos_make_finally_scope_guard([&]()");
                builder.AppendLine(inner + "{");
                EmitStructuredIRNode(builder, er.HandlerBody, method, inner + "    ");
                builder.AppendLine(inner + "});");
                EmitStructuredIRNode(builder, er.TryBody, method, inner);
                builder.AppendLine(indentation + "}");
                break;
            }

            case IRExceptionKind.TryFilter:
            {
                builder.AppendLine(indentation + "try");
                builder.AppendLine(indentation + "{");
                EmitStructuredIRNode(builder, er.TryBody, method, bodyIndent);
                builder.AppendLine(indentation + "}");
                builder.AppendLine(indentation + "catch (const chaos_managed_exception& chaos_exception)");
                builder.AppendLine(indentation + "{");
                builder.AppendLine(inner + "chaos_eval_stack[chaos_stack_top++] = chaos_exception.object_value;");

                // Emit filter instructions (endfilter decides rethrow vs accept)
                if (er.FilterInstructions != null)
                {
                    foreach (var instr in er.FilterInstructions)
                    {
                        if (instr.Op == "endfilter")
                        {
                            builder.AppendLine(inner +
                                "if (chaos_eval_stack[--chaos_stack_top] == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
                            builder.AppendLine(inner + "{");
                            builder.AppendLine(inner + "    throw;");
                            builder.AppendLine(inner + "}");
                        }
                        else
                        {
                            EmitLinearInstruction(builder, instr, inner);
                        }
                    }
                }

                builder.AppendLine(inner + "chaos_eval_stack[chaos_stack_top++] = chaos_exception.object_value;");
                EmitStructuredIRNode(builder, er.HandlerBody, method, bodyIndent);
                builder.AppendLine(indentation + "}");
                break;
            }

            default:
                throw new NotSupportedException(
                    "StructuredIR: unknown exception kind '" + er.Kind + "'");
        }
    }

    // ── Convenience: try to use this emitter for a set of instructions ──

    /// <summary>
    /// Emit a list of instructions using the new StructuredIR emitter
    /// (no goto fallback).  Builds a CFG, runs structured recovery,
    /// and emits the IR tree.  For irreducible CFGs, falls back to
    /// flat goto emission via EmitInstructionRange.
    /// </summary>
    private void EmitViaStructuredIR(
        StringBuilder builder,
        AotCoreIrMethodArtifact method,
        IReadOnlyList<AotCoreIrInstructionArtifact> instructions,
        IReadOnlyDictionary<int, int?> nextOffsetsByIlOffset,
        IReadOnlySet<int> offsets)
    {
        if (instructions.Count == 0)
            return;

        var cfg = BuildControlFlowGraph(instructions, offsets);
        if (!cfg.IsReducible)
        {
            // Fallback: flat goto mode (emits chaos_ip_OFFSET: labels + goto-next)
            // TODO: replace with node-splitting in Phase 2 when IR builder is rewritten
            EmitInstructionRange(builder, method, instructions, nextOffsetsByIlOffset, offsets);
            return;
        }

        var tree = RecoverStructure(cfg, 0, cfg.Blocks.Count - 1);
        StructuredIRNode ir = tree;
        EmitStructuredIRNode(builder, ir, method, "    ");
    }

}
